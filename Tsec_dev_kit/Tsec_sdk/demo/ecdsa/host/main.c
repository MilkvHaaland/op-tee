/*
 * Copyright (c) 2016, Linaro Limited
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

#include <err.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <ecdsa.h>
#include <stdlib.h>
#include <tee_client_api.h>

#define KEY_SIZE 256

struct tee_attr_packed {
	uint32_t attr_id;
	uint32_t a;
	uint32_t b;
};

typedef struct {
	uint32_t attributeID;
	union {
		struct {
			void *buffer;
			uint32_t length;
		} ref;
		struct {
			uint32_t a, b;
		} value;
	} content;
} TEE_Attribute;

typedef enum {
	TEE_MODE_ENCRYPT = 0,
	TEE_MODE_DECRYPT = 1,
	TEE_MODE_SIGN = 2,
	TEE_MODE_VERIFY = 3,
	TEE_MODE_MAC = 4,
	TEE_MODE_DIGEST = 5,
	TEE_MODE_DERIVE = 6
} TEE_OperationMode;

void xtest_add_attr(size_t *attr_count, TEE_Attribute *attrs, uint32_t attr_id,
		    const void *buf, size_t len)
{
	attrs[*attr_count].attributeID = attr_id;
	attrs[*attr_count].content.ref.buffer = (void *)buf;
	attrs[*attr_count].content.ref.length = len;
	(*attr_count)++;
}

TEEC_Result pack_attrs(const TEE_Attribute *attrs, uint32_t attr_count,
		      uint8_t **buf, size_t *blen)
{
	struct tee_attr_packed *a = NULL;
	uint8_t *b = NULL;
	size_t bl = 0;
	size_t n = 0;

	*buf = NULL;
	*blen = 0;
	if (attr_count == 0)
		return TEEC_SUCCESS;

	bl = sizeof(uint32_t) + sizeof(struct tee_attr_packed) * attr_count;
	for (n = 0; n < attr_count; n++) {
		if ((attrs[n].attributeID & TEE_ATTR_BIT_VALUE) != 0) {
			continue; /* Only memrefs need to be updated */
		}

		if (!attrs[n].content.ref.buffer) {
			continue;
		}

		/* Make room for padding */
		bl += ECDSA_ROUNDUP(attrs[n].content.ref.length, 4);
	}

	b = calloc(1, bl);
	if (!b)
		return TEEC_ERROR_OUT_OF_MEMORY;

	*buf = b;
	*blen = bl;

	*(uint32_t *)(void *)b = attr_count;
	b += sizeof(uint32_t);
	a = (struct tee_attr_packed *)(void *)b;
	b += sizeof(struct tee_attr_packed) * attr_count;

	for (n = 0; n < attr_count; n++) {
		a[n].attr_id = attrs[n].attributeID;
		if (attrs[n].attributeID & TEE_ATTR_BIT_VALUE) {
			a[n].a = attrs[n].content.value.a;
			a[n].b = attrs[n].content.value.b;
			continue;
		}

		a[n].b = attrs[n].content.ref.length;

		if (!attrs[n].content.ref.buffer) {
			a[n].a = 0;
			continue;
		}

		memcpy(b, attrs[n].content.ref.buffer,
		       attrs[n].content.ref.length);

		/* Make buffer pointer relative to *buf */
		a[n].a = (uint32_t)(uintptr_t)(b - *buf);

		/* Round up to good alignment */
		b += ECDSA_ROUNDUP(attrs[n].content.ref.length, 4);
	}

	return TEEC_SUCCESS;
}

TEEC_Result ta_crypt_cmd_allocate_transient_object(
						   TEEC_Session *s,
						   TEE_ObjectType obj_type,
						   uint32_t max_obj_size,
						   TEE_ObjectHandle *o)
{
	TEEC_Result res = TEEC_ERROR_GENERIC;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t ret_orig = 0;

	op.params[0].value.a = obj_type;
	op.params[0].value.b = max_obj_size;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_VALUE_OUTPUT,
					 TEEC_NONE, TEEC_NONE);

	res = TEEC_InvokeCommand(s, TA_CRYPT_CMD_ALLOCATE_TRANSIENT_OBJECT, &op,
				 &ret_orig);

	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(SET_KEY1) failed 0x%x origin 0x%x",
			res, ret_orig);

	if (res == TEEC_SUCCESS)
		*o = (TEE_ObjectHandle)(uintptr_t)op.params[1].value.a;

	return res;
}

TEEC_Result ta_crypt_cmd_populate_transient_object(
						   TEEC_Session *s,
						   TEE_ObjectHandle o,
						   const TEE_Attribute *attrs,
						   uint32_t attr_count)
{
	TEEC_Result res = TEEC_ERROR_GENERIC;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t ret_orig = 0;
	uint8_t *buf = NULL;
	size_t blen = 0;

	res = pack_attrs(attrs, attr_count, &buf, &blen);

	assert((uintptr_t)o <= UINT32_MAX);
	op.params[0].value.a = (uint32_t)(uintptr_t)o;

	op.params[1].tmpref.buffer = buf;
	op.params[1].tmpref.size = blen;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
					 TEEC_MEMREF_TEMP_INPUT, TEEC_NONE,
					 TEEC_NONE);

	res = TEEC_InvokeCommand(s, TA_CRYPT_CMD_POPULATE_TRANSIENT_OBJECT, &op,
				 &ret_orig);

	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(SET_KEY21) failed 0x%x origin 0x%x",
			res, ret_orig);

	free(buf);
	return res;
}

TEE_Result ta_crypt_cmd_set_operation_key(TEEC_Session *s,
					  TEE_OperationHandle oph,
					  TEE_ObjectHandle key)
{
	TEEC_Result res = TEEC_ERROR_GENERIC;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t ret_orig = 0;

	assert((uintptr_t)oph <= UINT32_MAX);
	op.params[0].value.a = (uint32_t)(uintptr_t)oph;

	assert((uintptr_t)key <= UINT32_MAX);
	op.params[0].value.b = (uint32_t)(uintptr_t)key;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE,
					 TEEC_NONE);

	res = TEEC_InvokeCommand(s, TA_CRYPT_CMD_SET_OPERATION_KEY, &op,
				 &ret_orig);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(SET_KEY3) failed 0x%x origin 0x%x",
			res, ret_orig);

	return res;
}

TEEC_Result ta_crypt_cmd_get_object_buffer_attribute(
						     TEEC_Session *s,
						     TEE_ObjectHandle o,
						     uint32_t attr_id,
						     void *buf, size_t *blen)
{
	TEEC_Result res = TEEC_ERROR_GENERIC;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t ret_orig = 0;

	assert((uintptr_t)o <= UINT32_MAX);
	op.params[0].value.a = (uint32_t)(uintptr_t)o;
	op.params[0].value.b = attr_id;

	op.params[1].tmpref.buffer = buf;
	op.params[1].tmpref.size = *blen;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
					 TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE,
					 TEEC_NONE);

	res = TEEC_InvokeCommand(s, TA_CRYPT_CMD_GET_OBJECT_BUFFER_ATTRIBUTE,
				 &op, &ret_orig);
	
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(SET_KEY4) failed 0x%x origin 0x%x",
			res, ret_orig);

	if (res == TEEC_SUCCESS)
		*blen = op.params[1].tmpref.size;

	return res;
}


TEEC_Result ta_crypt_cmd_free_operation(TEEC_Session *s,
					TEE_OperationHandle oph)
{
	TEEC_Result res = TEEC_ERROR_GENERIC;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t ret_orig = 0;

	op.params[0].value.a = (uint32_t)(uintptr_t)oph;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE,
					 TEEC_NONE);

	res = TEEC_InvokeCommand(s, TA_CRYPT_CMD_FREE_OPERATION, &op,
				 &ret_orig);

	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(SET_KEY5) failed 0x%x origin 0x%x",
			res, ret_orig);

	return res;
}

TEEC_Result ta_crypt_cmd_free_transient_object(TEEC_Session *s,
					       TEE_ObjectHandle o)
{
	TEEC_Result res = TEEC_ERROR_GENERIC;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t ret_orig = 0;

	assert((uintptr_t)o <= UINT32_MAX);
	op.params[0].value.a = (uint32_t)(uintptr_t)o;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE,
					 TEEC_NONE);

	res = TEEC_InvokeCommand(s, TA_CRYPT_CMD_FREE_TRANSIENT_OBJECT, &op,
				 &ret_orig);
	
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(SET_KEY6) failed 0x%x origin 0x%x",
			res, ret_orig);

	return res;
}

TEEC_Result ta_crypt_cmd_allocate_operation( TEEC_Session *s,
					    TEE_OperationHandle *oph,
					    uint32_t algo, uint32_t mode,
					    uint32_t max_key_size)
{
	TEEC_Result res = TEEC_ERROR_GENERIC;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t ret_orig = 0;

	op.params[0].value.a = 0;
	op.params[0].value.b = algo;
	op.params[1].value.a = mode;
	op.params[1].value.b = max_key_size;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INOUT, TEEC_VALUE_INPUT,
					 TEEC_NONE, TEEC_NONE);

	res = TEEC_InvokeCommand(s, TA_CRYPT_CMD_ALLOCATE_OPERATION, &op,
				 &ret_orig);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(SET_KEY7) failed 0x%x origin 0x%x",
			res, ret_orig);

	if (res == TEEC_SUCCESS)
		*oph = (TEE_OperationHandle)(uintptr_t)op.params[0].value.a;

	return res;
}

void xtest_add_attr_value(size_t *attr_count, TEE_Attribute *attrs,
			  uint32_t attr_id, uint32_t value_a, uint32_t value_b)
{
	attrs[*attr_count].attributeID = attr_id;
	attrs[*attr_count].content.value.a = value_a;
	attrs[*attr_count].content.value.b = value_b;
	(*attr_count)++;
}

static TEEC_Result ta_crypt_cmd_generate_key(TEEC_Session *s,
					     TEE_ObjectHandle o,
					     uint32_t key_size,
					     const TEE_Attribute *params,
					     uint32_t paramCount)
{
	TEEC_Result res = TEEC_ERROR_GENERIC;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t ret_orig = 0;
	uint8_t *buf = NULL;
	size_t blen = 0;

	res = pack_attrs(params, paramCount, &buf, &blen);
	if (res !=TEEC_SUCCESS)
		return res;

	assert((uintptr_t)o <= UINT32_MAX);
	op.params[0].value.a = (uint32_t)(uintptr_t)o;
	op.params[0].value.b = key_size;

	op.params[1].tmpref.buffer = buf;
	op.params[1].tmpref.size = blen;
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
					 TEEC_MEMREF_TEMP_INPUT, TEEC_NONE,
					 TEEC_NONE);

	res = TEEC_InvokeCommand(s, TA_CRYPT_CMD_GENERATE_KEY, &op, &ret_orig);
    
	free(buf);
	return res;
}

static bool keygen_attributes_get(TEEC_Session *s,
				   TEE_ObjectHandle key, uint32_t key_size,
				   struct key_attrs *attrs, size_t num_attrs,uint8_t *pubkey_x,uint8_t *pubkey_y,uint8_t *privkey)
{
	uint8_t out[2048] = {0};
	size_t out_size = 0;
	size_t m = 0;

	for (m = 0; m < num_attrs; m++) {
		if ((attrs[m].attr & TEE_ATTR_BIT_VALUE) == 0) {
			out_size = sizeof(out);
			memset(out, 0, sizeof(out));
			if (ta_crypt_cmd_get_object_buffer_attribute(s,
					key, attrs[m].attr, out, &out_size) != TEEC_SUCCESS)
				return -1;

            if(TEE_ATTR_ECC_PRIVATE_VALUE == attrs[m].attr) {
                memcpy(privkey,out,out_size);
            } else if(TEE_ATTR_ECC_PUBLIC_VALUE_X == attrs[m].attr) {
                memcpy(pubkey_x,out,out_size);
            } else if(TEE_ATTR_ECC_PUBLIC_VALUE_Y == attrs[m].attr) {
                memcpy(pubkey_y,out,out_size);
            }
		}
	}
	return 0;
}

int gen_key(void *pubkey_x,void *pubkey_y,void *privkey)
{
	uint32_t origin;
	TEEC_Result res;
	TEEC_UUID uuid = TA_ECDSA_UUID_1;
    TEEC_Context ctx;
	TEEC_Session sess;
    size_t param_count = 0;
    int ret = 0;
	TEE_Attribute params[4] = { };
	TEE_ObjectHandle key = TEE_HANDLE_NULL;
	int key_size = KEY_SIZE;
	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
	/* Open a session with the TA */
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, origin);
    
    param_count = 0;

    xtest_add_attr_value(&param_count, params, TEE_ATTR_ECC_CURVE,
                    TEE_ECC_CURVE_NIST_P256, 0);

	res = ta_crypt_cmd_allocate_transient_object(&sess, TEE_TYPE_ECDSA_KEYPAIR, key_size,&key);
    if (res != TEEC_SUCCESS)
		errx(1, "ta_crypt_cmd_allocate_transient_object failed with code 0x%x origin 0x%x",
			res, origin);

    res = ta_crypt_cmd_generate_key(&sess, key, key_size, params,
                    param_count);
    if (res != TEEC_SUCCESS)
		errx(1, "ta_crypt_cmd_generate_key failed with code 0x%x origin 0x%x",
			res, origin);
    
    const struct key_attrs attrs[] = {
		KEY_ATTR(TEE_ATTR_ECC_PRIVATE_VALUE, false),
		KEY_ATTR(TEE_ATTR_ECC_PUBLIC_VALUE_X , false),
		KEY_ATTR(TEE_ATTR_ECC_PUBLIC_VALUE_Y , false),
	};

	ret = keygen_attributes_get(&sess, key, key_size,
				      (struct key_attrs *)&attrs,
				      sizeof(attrs) / sizeof(attrs[0]),pubkey_x,pubkey_y,privkey);
    
    if(ret != 0) {
        return -1;
    }

	return 0;
}

static bool create_key(TEEC_Session *s,
		       uint32_t max_key_size, uint32_t key_type,
		       TEE_Attribute *attrs, size_t num_attrs,
		       TEE_ObjectHandle *handle)
{
	size_t n = 0;
	if (ta_crypt_cmd_allocate_transient_object(s, key_type,
			max_key_size, handle) != TEEC_SUCCESS) {
        printf("ta_crypt_cmd_allocate_transient_object error\r\n");
		return -1;
    }
 
	if (ta_crypt_cmd_populate_transient_object(s, *handle, attrs,
			num_attrs) != TEEC_SUCCESS) {
        printf("ta_crypt_cmd_populate_transient_object error\r\n");
		return -2;
    }

	for (n = 0; n < num_attrs; n++) {
		uint8_t out[512] = {0};
		size_t out_size = sizeof(out);

		if (attrs[n].attributeID == TEE_ATTR_ECC_CURVE)
			continue;

		if (ta_crypt_cmd_get_object_buffer_attribute(s, *handle,
				attrs[n].attributeID, out, &out_size) != TEEC_SUCCESS)
			return -3;

		if (out_size < attrs[n].content.ref.length) {
			memmove(out + (attrs[n].content.ref.length - out_size),
				out,
				attrs[n].content.ref.length);
			memset(out, 0, attrs[n].content.ref.length - out_size);
			out_size = attrs[n].content.ref.length;
		}
	}

	return 0;
}

static TEEC_Result ta_crypt_cmd_asymmetric_operate(TEEC_Session *s,
						   TEE_OperationHandle oph,
						   uint32_t cmd,
						   const TEE_Attribute *params,
						   uint32_t paramCount,
						   const void *src,
						   size_t src_len,
						   void *dst,
						   size_t *dst_len)
{
	TEEC_Result res = TEEC_ERROR_GENERIC;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t ret_orig = 0;
	uint8_t *buf = NULL;
	size_t blen = 0;

	res = pack_attrs(params, paramCount, &buf, &blen);
	if (res != TEEC_SUCCESS)
		return res;

	assert((uintptr_t)oph <= UINT32_MAX);
	op.params[0].value.a = (uint32_t)(uintptr_t)oph;

	op.params[1].tmpref.buffer = buf;
	op.params[1].tmpref.size = blen;

	op.params[2].tmpref.buffer = (void *)src;
	op.params[2].tmpref.size = src_len;

	op.params[3].tmpref.buffer = dst;
	op.params[3].tmpref.size = *dst_len;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
					 TEEC_MEMREF_TEMP_INPUT,
					 TEEC_MEMREF_TEMP_INPUT,
					 TEEC_MEMREF_TEMP_OUTPUT);

	res = TEEC_InvokeCommand(s, cmd, &op, &ret_orig);

	if (res != TEEC_SUCCESS) {
        errx(1, "ta_crypt_cmd_asymmetric_operate failed with code 0x%x origin 0x%x",
			res, ret_orig);
	}

	if (res == TEEC_SUCCESS)
		*dst_len = op.params[3].tmpref.size;

	free(buf);
	return res;
}

static TEEC_Result ta_crypt_cmd_asymmetric_sign(TEEC_Session *s,
						TEE_OperationHandle oph,
						const TEE_Attribute *params,
						uint32_t paramCount,
						const void *digest,
						size_t digest_len,
						void *signature,
						size_t *signature_len)
{
	return ta_crypt_cmd_asymmetric_operate(s, oph,
			TA_CRYPT_CMD_ASYMMETRIC_SIGN_DIGEST, params, paramCount,
			digest, digest_len, signature, signature_len);
}

static TEEC_Result ta_crypt_cmd_asymmetric_verify(TEEC_Session *s,
						  TEE_OperationHandle oph,
						  const TEE_Attribute *params,
						  uint32_t paramCount,
						  const void *digest,
						  size_t digest_len,
						  const void *signature,
						  size_t signature_len)
{
	TEEC_Result res = TEEC_ERROR_GENERIC;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t ret_orig = 0;
	uint8_t *buf = NULL;
	size_t blen = 0;

	res = pack_attrs(params, paramCount, &buf, &blen);
	if (res != TEEC_SUCCESS)
		return res;

	assert((uintptr_t)oph <= UINT32_MAX);
	op.params[0].value.a = (uint32_t)(uintptr_t)oph;

	op.params[1].tmpref.buffer = buf;
	op.params[1].tmpref.size = blen;

	op.params[2].tmpref.buffer = (void *)digest;
	op.params[2].tmpref.size = digest_len;

	op.params[3].tmpref.buffer = (void *)signature;
	op.params[3].tmpref.size = signature_len;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
					 TEEC_MEMREF_TEMP_INPUT,
					 TEEC_MEMREF_TEMP_INPUT,
					 TEEC_MEMREF_TEMP_INPUT);

	res = TEEC_InvokeCommand(s, TA_CRYPT_CMD_ASYMMETRIC_VERIFY_DIGEST,
				 &op, &ret_orig);

	free(buf);
	return res;
}


static int sign_verify(uint8_t *pubkey_x,uint8_t *pubkey_y,uint8_t *privkey)
{
    TEE_OperationHandle op = TEE_HANDLE_NULL;
	uint32_t origin;
	TEEC_Result res;
	TEEC_UUID uuid = TA_ECDSA_UUID_1;
    TEEC_Context ctx;
	TEEC_Session sess;
	uint8_t out[64] = {0};
	size_t out_size = 64;
    int max_key_size = 0;
    size_t num_key_attrs = 0;
	TEE_Attribute key_attrs[8] = { };
	TEE_ObjectHandle priv_key_handle = TEE_HANDLE_NULL;
	TEE_ObjectHandle pub_key_handle = TEE_HANDLE_NULL;
	TEE_Attribute algo_params[1] = { };
	size_t num_algo_params = 0;
    uint8_t ptx_hash[20] = {0,1};
    int ptx_hash_size = sizeof(ptx_hash);

	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	/* Open a session with the TA */
	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, origin);
    
    max_key_size = KEY_SIZE;
    xtest_add_attr_value(&num_key_attrs, key_attrs,TEE_ATTR_ECC_CURVE, TEE_ECC_CURVE_NIST_P256, 0);
    xtest_add_attr(&num_key_attrs, key_attrs,TEE_ATTR_ECC_PUBLIC_VALUE_X,pubkey_x,KEY_SIZE / 8);
    xtest_add_attr(&num_key_attrs, key_attrs,
                TEE_ATTR_ECC_PUBLIC_VALUE_Y,
                pubkey_y,
                KEY_SIZE / 8);

    if ( create_key(&sess, max_key_size,
                TEE_TYPE_ECDSA_PUBLIC_KEY, key_attrs,
                num_key_attrs, &pub_key_handle) != 0)
        return -1;

    xtest_add_attr(&num_key_attrs, key_attrs,
                TEE_ATTR_ECC_PRIVATE_VALUE,
                privkey,
                KEY_SIZE / 8);

    if (create_key(&sess, max_key_size,
                TEE_TYPE_ECDSA_KEYPAIR, key_attrs,
                num_key_attrs, &priv_key_handle) != 0)
        return -2;

    
    if (ta_crypt_cmd_allocate_operation(&sess,
        &op, TEE_ALG_ECDSA_P256, TEE_MODE_SIGN,
        max_key_size) != TEEC_SUCCESS)
        return -3;

    if (ta_crypt_cmd_set_operation_key(&sess, op,
            priv_key_handle) != TEEC_SUCCESS)
        return -5;

    if (ta_crypt_cmd_free_transient_object(&sess,
            priv_key_handle) != TEEC_SUCCESS)
        return -6;

    priv_key_handle = TEE_HANDLE_NULL;

    if (ta_crypt_cmd_asymmetric_sign(&sess, op,
            algo_params, num_algo_params, ptx_hash,
            ptx_hash_size, out, &out_size) != TEEC_SUCCESS)
        return -7;

    if (ta_crypt_cmd_free_operation(&sess,
                        op) != TEEC_SUCCESS)
        return -8;

    if (ta_crypt_cmd_allocate_operation(&sess, &op, TEE_ALG_ECDSA_P256,
            TEE_MODE_VERIFY, max_key_size) != TEEC_SUCCESS)
        return -9;

    if (ta_crypt_cmd_set_operation_key(&sess, op, pub_key_handle) != TEEC_SUCCESS)
        return -10;


    if (ta_crypt_cmd_free_transient_object(&sess, pub_key_handle) != TEEC_SUCCESS)
        return -11;

    if (ta_crypt_cmd_asymmetric_verify(&sess, op, algo_params,
						num_algo_params, ptx_hash,
						ptx_hash_size, out, out_size)!= TEEC_SUCCESS)
        return -12;

    return 0;
}

int main(int argc, char** argv)
{
    TEE_Result ret = TEEC_SUCCESS;
    uint8_t pubkey_x[32] = {0};
    uint8_t pubkey_y[32] = {0};
    uint8_t privkey[32] = {0};

    /*gen ecdsa key*/
    ret = gen_key(pubkey_x, pubkey_y, privkey);
    if (ret != TEEC_SUCCESS) {
        printf("gen_key error\r\n");
        return -1;
    } else {
        printf("gen_key success\r\n");
    }

    /*sign verify test*/
    ret = sign_verify(pubkey_x, pubkey_y, privkey);
    if (ret != TEEC_SUCCESS) {
        printf("sign_verify error:%d\r\n",ret);
        return -1;
    } else {
        printf("sign_verify success\r\n");
    }

	return 0;
}

