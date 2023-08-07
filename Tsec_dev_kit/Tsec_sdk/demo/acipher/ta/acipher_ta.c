/*
 * Copyright (c) 2017, Linaro Limited
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <rsa_crypto.h>

/*
 * Ciphering context: each opened session relates to a cipehring operation.
 * - configure the RSA flavour from a command.
 * - load key from a command (here the key is provided by the REE)
 * - reset init vector (here IV is provided by the REE)
 * - cipher a buffer frame (here input and output buffers are non-secure)
 */
struct rsa_cipher {
	uint32_t algo;			/* RSA flavour */
	uint32_t mode;			/* Encode or decode */
	uint32_t key_size;		/* RSA key size in byte */
	uint32_t key_type;		/* RSA key size in byte */
	TEE_OperationHandle op_handle;	/* RSA ciphering operation */
	TEE_ObjectHandle key_handle;	/* transient object to load the key */
};


static TEE_Result ta2tee_key_type(uint32_t param, uint32_t *key_type)
{
	switch (param) {
	case RSA_PUBLIC_KEY:
		*key_type = TEE_TYPE_RSA_PUBLIC_KEY;
		return TEE_SUCCESS;
	case RSA_KEYPAIR:
		*key_type = TEE_TYPE_RSA_KEYPAIR;
		return TEE_SUCCESS;
	default:
		EMSG("Invalid key type %u", param);
		return TEE_ERROR_BAD_PARAMETERS;
	}
}


/*
 * Process command TA_RSA_CMD_PREPARE. API in rsa_ta.h
 *
 * Allocate resources required for the ciphering operation.
 * During ciphering operation, when expect client can:
 * - update the key materials (provided by client)
 * - reset the initial vector (provided by client)
 * - cipher an input buffer into an output buffer (provided by client)
 */
static TEE_Result alloc_transient_object(void *session, uint32_t param_types,
				  TEE_Param params[4])
{
	const uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
	struct rsa_cipher *sess;
	TEE_Result res;

	/* Get ciphering context from session ID */
	DMSG("Session %p: allocate transient object", session);
	sess = (struct rsa_cipher *)session;

	/* Safely get the invocation parameters */
	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;


	sess->key_size = params[0].value.a;

	res = ta2tee_key_type(params[0].value.b, &sess->key_type);
	if (res != TEE_SUCCESS)
		return res;
	

	/* Free potential previous transient object */
	if (sess->key_handle != TEE_HANDLE_NULL)
		TEE_FreeTransientObject(sess->key_handle);

	/* Allocate transient object according to target key size */
	res = TEE_AllocateTransientObject(sess->key_type,
					sess->key_size,
					&sess->key_handle);
	if (res != TEE_SUCCESS) {
		EMSG("Failed to allocate transient object");
		sess->key_handle = TEE_HANDLE_NULL;
		goto err;
	}

	return res;

err:
	if (sess->key_handle != TEE_HANDLE_NULL)
		TEE_FreeTransientObject(sess->key_handle);
	sess->key_handle = TEE_HANDLE_NULL;

	return res;
}

static TEE_Result alloc_resources(void *session, uint32_t param_types,
				  TEE_Param params[4])
{
	const uint32_t exp_param_types =
		TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
				TEE_PARAM_TYPE_VALUE_INPUT,
				TEE_PARAM_TYPE_VALUE_INPUT,
				TEE_PARAM_TYPE_NONE);
	struct rsa_cipher *sess;
	TEE_Result res;

	/* Get ciphering context from session ID */
	DMSG("Session %p: get ciphering resources", session);
	sess = (struct rsa_cipher *)session;

	/* Safely get the invocation parameters */
	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;


	sess->algo = params[0].value.a;

	sess->mode = params[2].value.a;
	
	sess->key_size = params[1].value.a;


	/*
	 * Ready to allocate the resources which are:
	 * - an operation handle, for an RSA ciphering of given configuration
	 * - a transient object that will be use to load the key materials
	 *   into the RSA ciphering operation.
	 */

	/* Free potential previous operation */
	if (sess->op_handle != TEE_HANDLE_NULL)
		TEE_FreeOperation(sess->op_handle);

	/* Allocate operation: RSA/CTR, mode and size from params */
	res = TEE_AllocateOperation(&sess->op_handle,
				    sess->algo,
				    sess->mode,
				    sess->key_size);
	if (res != TEE_SUCCESS) {
		EMSG("Failed to allocate operation, %x", res);
		sess->op_handle = TEE_HANDLE_NULL;
		goto err;
	}
	DMSG("TEE_AllocateOperation(TEE_MODE_ENCRYPT, %#" PRIx32 ", %" PRId32 "): %#" PRIx32, sess->algo, sess->key_size, res);

	param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);
	params[0].value.a = params[1].value.a;
	params[0].value.b = params[1].value.b;
	res = alloc_transient_object(session, param_types, params);
	if (res != TEE_SUCCESS)
		goto err;

	return res;

err:
	if (sess->op_handle != TEE_HANDLE_NULL)
		TEE_FreeOperation(sess->op_handle);
	sess->op_handle = TEE_HANDLE_NULL;

	return res;
}

/*
 * Process command TA_RSA_CMD_SET_KEY. API in rsa_ta.h
 */
static TEE_Result set_key(void *session, uint32_t param_types,
				TEE_Param params[4])
{
	struct rsa_cipher *sess;
	char *modulus;
	uint32_t modulus_len;
	char *pub_exp;
	uint32_t pub_exp_len;
	char *priv_exp;
	uint32_t priv_exp_len;
	TEE_Result res;

	/* Get ciphering context from session ID */
	DMSG("Session %p: load key material", session);
	sess = (struct rsa_cipher *)session;

	/* Safely get the invocation parameters */
	if (param_types == TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
					  TEE_PARAM_TYPE_MEMREF_INPUT,
					  TEE_PARAM_TYPE_NONE,
					  TEE_PARAM_TYPE_NONE)) {
		TEE_Attribute attrs[2];
		modulus = params[0].memref.buffer;
		modulus_len = params[0].memref.size;
		pub_exp = params[1].memref.buffer;
		pub_exp_len = params[1].memref.size;
		TEE_InitRefAttribute(&attrs[0], TEE_ATTR_RSA_MODULUS, modulus, modulus_len);
		TEE_InitRefAttribute(&attrs[1], TEE_ATTR_RSA_PUBLIC_EXPONENT, pub_exp, pub_exp_len);

		TEE_ResetTransientObject(sess->key_handle);
		res = TEE_PopulateTransientObject(sess->key_handle, attrs, sizeof(attrs)/sizeof(TEE_Attribute));
	} else if (param_types == TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
						 TEE_PARAM_TYPE_MEMREF_INPUT,
						 TEE_PARAM_TYPE_MEMREF_INPUT,
						 TEE_PARAM_TYPE_NONE)) {
		TEE_Attribute attrs[3];
		modulus = params[0].memref.buffer;
		modulus_len = params[0].memref.size;
		pub_exp = params[1].memref.buffer;
		pub_exp_len = params[1].memref.size;
		priv_exp = params[2].memref.buffer;
		priv_exp_len = params[2].memref.size;
		TEE_InitRefAttribute(&attrs[0], TEE_ATTR_RSA_MODULUS, modulus, modulus_len);
		TEE_InitRefAttribute(&attrs[1], TEE_ATTR_RSA_PUBLIC_EXPONENT, pub_exp, pub_exp_len);
		TEE_InitRefAttribute(&attrs[2], TEE_ATTR_RSA_PRIVATE_EXPONENT, priv_exp, priv_exp_len);

		TEE_ResetTransientObject(sess->key_handle);
		res = TEE_PopulateTransientObject(sess->key_handle, attrs, sizeof(attrs)/sizeof(TEE_Attribute));
	} else
		return TEE_ERROR_BAD_PARAMETERS;

	if (res != TEE_SUCCESS) {
		EMSG("TEE_PopulateTransientObject failed, %x", res);
		return res;
	}

	// TEE_ResetOperation(sess->op_handle);
	res = TEE_SetOperationKey(sess->op_handle, sess->key_handle);
	if (res != TEE_SUCCESS) {
		EMSG("TEE_SetOperationKey failed %x", res);
		return res;
	}

	return res;
}

/*
 * Process command TA_RSA_CMD_ASYMMETRIC_ENCRYPT. API in rsa_crypto.h
 */
static TEE_Result rsa_asymmetric_encrypt(void *session, uint32_t param_types,
				TEE_Param params[4])
{
	const uint32_t exp_param_types =
		TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
				TEE_PARAM_TYPE_MEMREF_INPUT,
				TEE_PARAM_TYPE_MEMREF_OUTPUT,
				TEE_PARAM_TYPE_NONE);
	struct rsa_cipher *sess;
	TEE_Attribute *attrs = NULL;
	uint32_t attr_count = 0;

	/* Get ciphering context from session ID */
	DMSG("Session %p: cipher buffer", session);
	sess = (struct rsa_cipher *)session;

	/* Safely get the invocation parameters */
	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	if (sess->op_handle == TEE_HANDLE_NULL)
		return TEE_ERROR_BAD_STATE;
	
	/*
	 * Process Encrypting operation on provided buffers
	 */
	TEE_Result res = TEE_ERROR_GENERIC;

	res = TEE_AsymmetricEncrypt(sess->op_handle, attrs, attr_count,
			params[1].memref.buffer, params[1].memref.size,
			params[2].memref.buffer, &params[2].memref.size);

	return res;
}

/*
 * Process command TA_RSA_CMD_ASYMMETRIC_DECRYPT. API in rsa_crypto.h
 */
static TEE_Result rsa_asymmetric_decrypt(void *session, uint32_t param_types,
				TEE_Param params[4])
{
	const uint32_t exp_param_types =
		TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
				TEE_PARAM_TYPE_MEMREF_INPUT,
				TEE_PARAM_TYPE_MEMREF_OUTPUT,
				TEE_PARAM_TYPE_NONE);
	struct rsa_cipher *sess;
	TEE_Attribute *attrs = NULL;
	uint32_t attr_count = 0;

	/* Get ciphering context from session ID */
	DMSG("Session %p: cipher buffer", session);
	sess = (struct rsa_cipher *)session;

	/* Safely get the invocation parameters */
	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	if (sess->op_handle == TEE_HANDLE_NULL)
		return TEE_ERROR_BAD_STATE;
	
	/*
	 * Process Encrypting operation on provided buffers
	 */
	TEE_Result res = TEE_ERROR_GENERIC;

	res = TEE_AsymmetricDecrypt(sess->op_handle, attrs, attr_count,
			params[1].memref.buffer, params[1].memref.size,
			params[2].memref.buffer, &params[2].memref.size);

	return res;
}


TEE_Result TA_CreateEntryPoint(void)
{
	/* Nothing to do */
	return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void)
{
	/* Nothing to do */
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t __unused param_types,
					TEE_Param __unused params[4],
					void __unused **session)
{
	struct rsa_cipher *sess;

	/*
	 * Allocate and init ciphering materials for the session.
	 * The address of the structure is used as session ID for
	 * the client.
	 */
	sess = TEE_Malloc(sizeof(*sess), 0);
	if (!sess)
		return TEE_ERROR_OUT_OF_MEMORY;

	sess->key_handle = TEE_HANDLE_NULL;
	sess->op_handle = TEE_HANDLE_NULL;

	*session = (void *)sess;
	DMSG("Session %p: newly allocated", *session);

	return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void *session)
{
	struct rsa_cipher *sess;

	/* Get ciphering context from session ID */
	DMSG("Session %p: release session", session);
	sess = (struct rsa_cipher *)session;

	/* Release the session resources */
	if (sess->key_handle != TEE_HANDLE_NULL)
		TEE_FreeTransientObject(sess->key_handle);
	if (sess->op_handle != TEE_HANDLE_NULL)
		TEE_FreeOperation(sess->op_handle);
	TEE_Free(sess);
}

TEE_Result TA_InvokeCommandEntryPoint(void *session,
					uint32_t cmd,
					uint32_t param_types,
					TEE_Param params[4])
{
	switch (cmd) {
	case TA_RSA_CMD_PREPARE:
		return alloc_resources(session, param_types, params);
	case TA_RSA_CMD_SET_KEY:
		return set_key(session, param_types, params);
	case TA_RSA_CMD_ASYMMETRIC_ENCRYPT:
		return rsa_asymmetric_encrypt(session, param_types, params);
	case TA_RSA_CMD_ASYMMETRIC_DECRYPT:
		return rsa_asymmetric_decrypt(session, param_types, params);
	default:
		EMSG("Command ID 0x%x is not supported", cmd);
		return TEE_ERROR_NOT_SUPPORTED;
	}
}
