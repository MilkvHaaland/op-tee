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
#include <curve25519_derive_key.h>
#include <stdlib.h>
#include <tee_client_api.h>
#define CURVE_25519_KEY_BITS_SIZE 256
#define ROUNDUP(v, size) (((v) + (size - 1)) & ~(size - 1))
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
		bl += ROUNDUP(attrs[n].content.ref.length, 4);
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
		b += ROUNDUP(attrs[n].content.ref.length, 4);
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
		errx(1, "TEEC_InvokeCommand(SET_KEY2) failed 0x%x origin 0x%x",
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

TEEC_Result ta_crypt_cmd_derive_key_25519(TEEC_UUID* args,void *pubkey,void *privkey,void *derived_key)
{
	TEEC_Result res = TEEC_ERROR_GENERIC;
	TEEC_Operation op = TEEC_OPERATION_INITIALIZER;
	uint32_t ret_orig = 0;
	uint32_t err_origin;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_UUID uuid = (*args);
    TEE_Attribute params[4] = { };
    size_t param_count = 0;
    size_t out_size = CURVE_25519_KEY_BITS_SIZE;
    size_t max_size = 256;
    TEE_OperationHandle oph = TEE_HANDLE_NULL;
    TEE_ObjectHandle key_handle = TEE_HANDLE_NULL;
    TEE_ObjectHandle sv_handle = 0;
    uint8_t *buf = NULL;
    size_t blen = 0;
    int	size_bytes = (max_size + 7) / 8;
     
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);
    

	res = ta_crypt_cmd_allocate_operation(&sess, &oph, TEE_ALG_X25519, TEE_MODE_DERIVE, max_size);
	if (res != TEEC_SUCCESS)
		errx(1, "ta_crypt_cmd_allocate_transient_object failed with code 0x%x origin 0x%x",
			res, err_origin);

    res = ta_crypt_cmd_allocate_transient_object(&sess,
        TEE_TYPE_X25519_KEYPAIR, size_bytes *
        8, &key_handle);
    if (res != TEEC_SUCCESS)
		errx(1, "ta_crypt_cmd_allocate_transient_object failed with code 0x%x origin 0x%x",
			res, err_origin);

    param_count = 0;
    xtest_add_attr(&param_count, params, TEE_ATTR_X25519_PUBLIC_VALUE,
		       pubkey,32);
    xtest_add_attr(&param_count, params, TEE_ATTR_X25519_PRIVATE_VALUE,
		       privkey,32);
	
	res = ta_crypt_cmd_populate_transient_object(&sess,
				key_handle, params, param_count);
    if (res != TEEC_SUCCESS)
		errx(1, "ta_crypt_cmd_populate_transient_object failed with code 0x%x origin 0x%x",
			res, err_origin);
        
	res = ta_crypt_cmd_set_operation_key(&sess, oph,
						       key_handle);
    if (res != TEEC_SUCCESS)
		errx(1, "ta_crypt_cmd_set_operation_key failed with code 0x%x origin 0x%x",
			res, err_origin);

	res = ta_crypt_cmd_free_transient_object(&sess,
							   key_handle);
    if (res != TEEC_SUCCESS)
		errx(1, "ta_crypt_cmd_free_transient_object failed with code 0x%x origin 0x%x",
			res, err_origin);

	res = ta_crypt_cmd_allocate_transient_object(&sess,
				TEE_TYPE_GENERIC_SECRET, size_bytes * 8, &sv_handle);
    if (res != TEEC_SUCCESS)
		errx(1, "ta_crypt_cmd_allocate_transient_object failed with code 0x%x origin 0x%x",
			res, err_origin);
   
	memset(&op, 0, sizeof(op));
	param_count = 0;
    xtest_add_attr(&param_count, params, TEE_ATTR_X25519_PUBLIC_VALUE,
		       pubkey,32);
    xtest_add_attr(&param_count, params, TEE_ATTR_X25519_PRIVATE_VALUE,
		       privkey,32);

	res = pack_attrs(params, param_count, &buf, &blen);

	assert((uintptr_t)oph <= UINT32_MAX);
	op.params[0].value.a = (uint32_t)(uintptr_t)oph;

	assert((uintptr_t)sv_handle <= UINT32_MAX);
	op.params[0].value.b = (uint32_t)(uintptr_t)sv_handle;

	op.params[1].tmpref.buffer = buf;
	op.params[1].tmpref.size = blen;

	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
					 TEEC_MEMREF_TEMP_INPUT, TEEC_NONE,
					 TEEC_NONE);
    
	res = TEEC_InvokeCommand(&sess, TA_CRYPT_DERIVE_KEY_TEST, &op, &ret_orig);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(SET_KEY8) failed 0x%x origin 0x%x",
			res, ret_orig);

	res = ta_crypt_cmd_get_object_buffer_attribute(&sess,
				sv_handle, TEE_ATTR_SECRET_VALUE, derived_key,
				&out_size);
	ta_crypt_cmd_free_operation(&sess, oph);
	ta_crypt_cmd_free_transient_object(&sess,
							   sv_handle);
	free(buf);

	return res;
}

int gen_key(void *pub_exp, void *priv_exp)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;
	TEEC_UUID uuid = TA_25519_UUID_1;
    TEEC_Context ctx;
	TEEC_Session sess;
	uint8_t out[32] = {0};
	size_t out_size = 0;
	size_t m = 0;
	uint32_t attr_id[] = {TEE_ATTR_X25519_PUBLIC_VALUE, TEE_ATTR_X25519_PRIVATE_VALUE};

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

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
	op.params[0].value.a = CURVE_25519_KEY_BITS_SIZE / 8;
	op.params[0].value.b = TEE_TYPE_X25519_KEYPAIR;

	res = TEEC_InvokeCommand(&sess, TA_25519_CMD_GEN_KEY,
				 &op, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(GEN_KEY) failed 0x%x origin 0x%x",
			res, origin);
	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
					 TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);

	for (m = 0; m < sizeof(attr_id) / sizeof(attr_id[0]); m++) {
		out_size = sizeof(out);
		memset(out, 0, sizeof(out));
		op.params[0].value.a = attr_id[m];
		op.params[1].tmpref.buffer = out;
		op.params[1].tmpref.size = out_size;
		res = TEEC_InvokeCommand(&sess, TA_25519_CMD_GET_KEY,&op, &origin);
		if (res != TEEC_SUCCESS)
			errx(1, "TEEC_InvokeCommand(GET_KEY) failed 0x%x origin 0x%x",
				res, origin);
		out_size = op.params[1].tmpref.size;
		memcpy(out, op.params[1].tmpref.buffer, out_size);

		if (out_size >= 0) {
			if (m == 0) {
				memcpy(pub_exp, out, out_size);
			} else if (m == 1) {
				memcpy(priv_exp, out, out_size);
			} 
		} else {
			return -1;
		}
	}

	return 0;
}

int main(int argc, char** argv)
{
    TEEC_UUID uuid_1 = TA_25519_UUID_1;
    TEE_Result ret = TEEC_SUCCESS;
    uint8_t pubkey1[32] = {0};
    uint8_t privkey1[32] = {0};
    uint8_t pubkey2[32] = {0};
    uint8_t privkey2[32] = {0};
    uint8_t derive_key1[32] = {0};
    uint8_t derive_key2[32] = {0};

    /*gen 25519 key1*/
    ret = gen_key(pubkey1, privkey1);
    if (ret != TEEC_SUCCESS) {
        printf("test_25519_keypair_gen error\r\n");
        return -1;
    } else {
        printf("test_25519_keypair_gen success\r\n");
    }

    /*gen 25519 key2*/
    ret = gen_key(pubkey2, privkey2);
    if (ret != TEEC_SUCCESS) {
        printf("test_25519_keypair_gen error\r\n");
        return -2;
    } else {
        printf("test_25519_keypair_gen success\r\n");
    }

    /*derive 25519 key1.pub with key2.priv*/
    ret = ta_crypt_cmd_derive_key_25519(&uuid_1, pubkey1, privkey2, derive_key1);
    if (ret != 0) {
        printf("25519 derive key error\r\n");
        return -3;
    } else {
        printf("25519 derive key success\r\n");
    }
    
    /*derive 25519 key2.pub with key1.priv*/
    ret = ta_crypt_cmd_derive_key_25519(&uuid_1, pubkey2,privkey1, derive_key2);
    if (ret != 0) {
        printf("25519 derive key error\r\n");
        return -4;
    } else {
        printf("25519 derive key success\r\n");
    }

    /*check if sk is the same*/
    if (memcmp(derive_key1, derive_key2, 32) == 0) {
        printf("25519 derive key check success\r\n");
        return -5;
    } else {
        printf("25519 derive key check error\r\n");
    }

	return 0;
}

