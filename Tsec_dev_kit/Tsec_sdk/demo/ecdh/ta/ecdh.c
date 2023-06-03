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

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>

#include <ecdh_derive_key.h>

#define VAL2HANDLE(v) (void *)(uintptr_t)(v)
#define HANDLE2VAL(h) (uint32_t)((uintptr_t)h)

struct attr_packed {
	uint32_t id;
	uint32_t a;
	uint32_t b;
};

TEE_Result TA_CreateEntryPoint(void)
{
	return TEE_SUCCESS;
}

void TA_DestroyEntryPoint(void)
{
}

TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
		TEE_Param __maybe_unused params[4],
		void __maybe_unused **sess_ctx)
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE,
						   TEE_PARAM_TYPE_NONE);
	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	(void)&params;
	(void)&sess_ctx;
	DMSG("has been called1");

	return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	DMSG("has been called2");
	(void)&sess_ctx;
}

static TEE_Result test_tee_malloc(uint32_t param_types,
	TEE_Param params[4])
{
	DMSG("has been called3");
	uint32_t exp_param_types =
				TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_OUTPUT,
						TEE_PARAM_TYPE_NONE,
						TEE_PARAM_TYPE_NONE,
						TEE_PARAM_TYPE_NONE);
	void *buf = NULL;

	DMSG("has been called4");
	if (param_types != exp_param_types) {
		return TEE_ERROR_BAD_PARAMETERS;
	}

	buf = TEE_Malloc(params[0].memref.size, 0);
	// buf = TEE_Malloc(16, 0);
	if (!buf)
		return TEE_ERROR_OUT_OF_MEMORY;
	//IMSG("Generating random data over %u bytes.", params[0].memref.size);
	IMSG("Generating random data over %u bytes.", params[0].memref.size);
	/*
	 * The TEE_GenerateRandom function is a part of TEE Internal Core API,
	 * which generates random data
	 *
	 * Parameters:
	 * @ randomBuffer : Reference to generated random data
	 * @ randomBufferLen : Byte length of requested random data
	 */
	// TEE_GenerateRandom(buf, params[0].memref.size);
	TEE_MemMove(params[0].memref.buffer, buf, params[0].memref.size);
	TEE_Free(buf);

	return TEE_SUCCESS;
}

static TEE_Result unpack_attrs(const uint8_t *buf, size_t blen,
			       TEE_Attribute **attrs, uint32_t *attr_count)
{
	TEE_Result res = TEE_SUCCESS;
	TEE_Attribute *a = NULL;
	const struct attr_packed *ap = NULL;
	size_t num_attrs = 0;
	const size_t num_attrs_size = sizeof(uint32_t);

	if (blen == 0)
		goto out;

	if (((uintptr_t)buf & 0x3) != 0 || blen < num_attrs_size)
		return TEE_ERROR_BAD_PARAMETERS;
	num_attrs = *(uint32_t *) (void *)buf;
	if ((blen - num_attrs_size) < (num_attrs * sizeof(*ap)))
		return TEE_ERROR_BAD_PARAMETERS;
	ap = (const struct attr_packed *)(const void *)(buf + num_attrs_size);

	if (num_attrs > 0) {
		size_t n;

		a = TEE_Malloc(num_attrs * sizeof(TEE_Attribute), 0);
		if (!a)
			return TEE_ERROR_OUT_OF_MEMORY;
		for (n = 0; n < num_attrs; n++) {
			uintptr_t p;

			a[n].attributeID = ap[n].id;
			if (ap[n].id & TEE_ATTR_FLAG_VALUE) {
				a[n].content.value.a = ap[n].a;
				a[n].content.value.b = ap[n].b;
				continue;
			}

			a[n].content.ref.length = ap[n].b;
			p = (uintptr_t)ap[n].a;
			if (p) {
				if ((p + a[n].content.ref.length) > blen) {
					res = TEE_ERROR_BAD_PARAMETERS;
					goto out;
				}
				p += (uintptr_t)buf;
			}
			a[n].content.ref.buffer = (void *)p;
		}
	}

	res = TEE_SUCCESS;
out:
	if (res == TEE_SUCCESS) {
		*attrs = a;
		*attr_count = num_attrs;
	} else {
		TEE_Free(a);
	}
	return res;
}

TEE_Result ta_entry_free_operation(uint32_t param_type, TEE_Param params[4])
{
	TEE_OperationHandle op = VAL2HANDLE(params[0].value.a);

	TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_NONE,
			   TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);

	TEE_FreeOperation(op);
	return TEE_SUCCESS;
}

TEE_Result ta_entry_populate_transient_object(uint32_t param_type,
					      TEE_Param params[4])
{
	TEE_Result res = TEE_ERROR_GENERIC;
	TEE_Attribute *attrs = NULL;
	uint32_t attr_count = 0;
	TEE_ObjectHandle o = VAL2HANDLE(params[0].value.a);

	TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
			   TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_NONE,
			   TEE_PARAM_TYPE_NONE);

	res = unpack_attrs(params[1].memref.buffer, params[1].memref.size,
			   &attrs, &attr_count);
	if (res != TEE_SUCCESS)
		return res;

	res = TEE_PopulateTransientObject(o, attrs, attr_count);
	TEE_Free(attrs);
	return res;
}

TEE_Result ta_entry_allocate_transient_object(uint32_t param_type,
					      TEE_Param params[4])
{
	TEE_Result res = TEE_ERROR_GENERIC;
	TEE_ObjectHandle o = TEE_HANDLE_NULL;

	TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
			   TEE_PARAM_TYPE_VALUE_OUTPUT, TEE_PARAM_TYPE_NONE,
			   TEE_PARAM_TYPE_NONE);

	res = TEE_AllocateTransientObject(params[0].value.a, params[0].value.b,
					  &o);
	if (res == TEE_SUCCESS)
		params[1].value.a = HANDLE2VAL(o);
	return res;
}

TEE_Result ta_entry_free_transient_object(uint32_t param_type,
					  TEE_Param params[4])
{
	TEE_ObjectHandle o = VAL2HANDLE(params[0].value.a);

	TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_NONE,
			   TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);

	TEE_FreeTransientObject(o);
	return TEE_SUCCESS;
}

TEE_Result ta_entry_get_object_buffer_attribute(uint32_t param_type,
						TEE_Param params[4])
{
	TEE_ObjectHandle o = VAL2HANDLE(params[0].value.a);

	TEE_PARAM_TYPES
			  (TEE_PARAM_TYPE_VALUE_INPUT,
			   TEE_PARAM_TYPE_MEMREF_OUTPUT, TEE_PARAM_TYPE_NONE,
			   TEE_PARAM_TYPE_NONE);

	return TEE_GetObjectBufferAttribute(o, params[0].value.b,
			params[1].memref.buffer, &params[1].memref.size);
}

TEE_Result ta_entry_set_operation_key(uint32_t param_type, TEE_Param params[4])
{
	TEE_OperationHandle op = VAL2HANDLE(params[0].value.a);
	TEE_ObjectHandle key = VAL2HANDLE(params[0].value.b);

	TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_NONE,
			   TEE_PARAM_TYPE_NONE, TEE_PARAM_TYPE_NONE);

	return TEE_SetOperationKey(op, key);
}

TEE_Result TEE_AllocateOperation1(TEE_OperationHandle *operation,
				 uint32_t algorithm, uint32_t mode,
				 uint32_t maxKeySize);
TEE_Result ta_entry_allocate_operation(uint32_t param_type, TEE_Param params[4])
{
	TEE_Result res = TEE_ERROR_GENERIC;
	TEE_OperationHandle op = TEE_HANDLE_NULL;

	TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INOUT,
			   TEE_PARAM_TYPE_VALUE_INPUT, TEE_PARAM_TYPE_NONE,
			   TEE_PARAM_TYPE_NONE);
	res = TEE_AllocateOperation(&op,
				    params[0].value.b, params[1].value.a,
				    params[1].value.b);
	params[0].value.a = HANDLE2VAL(op);
	return res;
}

#define TEE_ALG_GET_CLASS(algo) __tee_alg_get_class(algo)

enum p_type {
	P_TYPE_BOOL,
	P_TYPE_INT,
	P_TYPE_UUID,
	P_TYPE_IDENTITY,
	P_TYPE_STRING,
	P_TYPE_BINARY_BLOCK,
};

static TEE_Result test_derive_key(uint32_t param_types,
	TEE_Param params[4])
{
	TEE_OperationHandle op = VAL2HANDLE(params[0].value.a);
	TEE_ObjectHandle key = VAL2HANDLE(params[0].value.b);
	TEE_Result res = TEE_ERROR_GENERIC;
	TEE_Attribute *attrs = NULL;
	uint32_t attr_count = 0;
	DMSG("has been called51");
	
	TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
			   TEE_PARAM_TYPE_MEMREF_INPUT, TEE_PARAM_TYPE_NONE,
			   TEE_PARAM_TYPE_NONE);
	

	res = unpack_attrs(params[1].memref.buffer, params[1].memref.size,
			   &attrs, &attr_count);
	if (res != TEE_SUCCESS) {
		return res;
	}

	TEE_DeriveKey(op, attrs, attr_count, key);

	TEE_Free(attrs);
	return TEE_SUCCESS;
}

TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
			uint32_t cmd_id,
			uint32_t param_types, TEE_Param params[4])
{
	(void)&sess_ctx;

	switch (cmd_id) {
		
	case TA_MEM_TEST:
		return test_tee_malloc(param_types, params);
	case TA_CRYPT_CMD_ALLOCATE_OPERATION:
		return ta_entry_allocate_operation(param_types, params);
	case TA_CRYPT_CMD_POPULATE_TRANSIENT_OBJECT:
		return ta_entry_populate_transient_object(param_types, params);
	case TA_CRYPT_CMD_FREE_TRANSIENT_OBJECT:
		return ta_entry_free_transient_object(param_types, params);
	case TA_CRYPT_CMD_ALLOCATE_TRANSIENT_OBJECT:
		return ta_entry_allocate_transient_object(param_types, params);
	case TA_CRYPT_CMD_SET_OPERATION_KEY:
		return ta_entry_set_operation_key(param_types, params);
	case TA_CRYPT_CMD_GET_OBJECT_BUFFER_ATTRIBUTE:
		return ta_entry_get_object_buffer_attribute(param_types, params);
	case TA_CRYPT_CMD_FREE_OPERATION:
		return ta_entry_free_operation(param_types, params);
	case TA_CRYPT_DERIVE_KEY_TEST:
		return test_derive_key(param_types, params);
	default:
		IMSG("Generating random data over");
		return TEE_ERROR_BAD_PARAMETERS;
	}
}
