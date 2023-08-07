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
#include <ecdh_derive_key.h>
#include <stdlib.h>
#include <tee_client_api.h>

#define ECC_KEY_SIZE 256

static const uint8_t nist_kas_ecc_cdh_testvector_69_private[] = {
/* dIUT */
	0xdd, 0x5e, 0x9f, 0x70, 0xae, 0x74, 0x00, 0x73, 0xca, 0x02, 0x04, 0xdf,
	0x60, 0x76, 0x3f, 0xb6, 0x03, 0x6c, 0x45, 0x70, 0x9b, 0xf4, 0xa7, 0xbb,
	0x4e, 0x67, 0x14, 0x12, 0xfa, 0xd6, 0x5d, 0xa3
};

static const uint8_t nist_kas_ecc_cdh_testvector_69_public_x[] = {
/* QCAVSx */
	0xa2, 0xef, 0x85, 0x7a, 0x08, 0x1f, 0x9d, 0x6e, 0xb2, 0x06, 0xa8, 0x1c,
	0x4c, 0xf7, 0x8a, 0x80, 0x2b, 0xdf, 0x59, 0x8a, 0xe3, 0x80, 0xc8, 0x88,
	0x6e, 0xcd, 0x85, 0xfd, 0xc1, 0xed, 0x76, 0x44
};

static const uint8_t nist_kas_ecc_cdh_testvector_69_public_y[] = {
/* QCAVSy */
	0x56, 0x3c, 0x4c, 0x20, 0x41, 0x9f, 0x07, 0xbc, 0x17, 0xd0, 0x53, 0x9f,
	0xad, 0xe1, 0x85, 0x5e, 0x34, 0x83, 0x95, 0x15, 0xb8, 0x92, 0xc0, 0xf5,
	0xd2, 0x65, 0x61, 0xf9, 0x7f, 0xa0, 0x4d, 0x1a
};

static const uint8_t nist_kas_ecc_cdh_testvector_69_out[] = {
/* ZIUT */
	0x43, 0x0e, 0x6a, 0x4f, 0xba, 0x44, 0x49, 0xd7, 0x00, 0xd2, 0x73, 0x3e,
	0x55, 0x7f, 0x66, 0xa3, 0xbf, 0x3d, 0x50, 0x51, 0x7c, 0x12, 0x71, 0xb1,
	0xdd, 0xae, 0x11, 0x61, 0xb7, 0xac, 0x79, 0x8c
};

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
		bl += ECDH_ROUNDUP(attrs[n].content.ref.length, 4);
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
		b += ECDH_ROUNDUP(attrs[n].content.ref.length, 4);
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

TEEC_Result ta_crypt_cmd_derive_key_ecdh(TEEC_UUID* args)
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
    uint8_t out[ECC_KEY_SIZE] = {0};
    size_t out_size = 0;
    size_t max_size = ECC_KEY_SIZE;
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
    

	res = ta_crypt_cmd_allocate_operation(&sess, &oph, TEE_ALG_ECDH_P256, TEE_MODE_DERIVE, max_size);
	if (res != TEEC_SUCCESS)
		errx(1, "ta_crypt_cmd_allocate_transient_object failed with code 0x%x origin 0x%x",
			res, err_origin);

	res = ta_crypt_cmd_allocate_transient_object(&sess,
				TEE_TYPE_ECDH_KEYPAIR, max_size, &key_handle);
    if (res != TEEC_SUCCESS)
		errx(1, "ta_crypt_cmd_allocate_transient_object failed with code 0x%x origin 0x%x",
			res, err_origin);

    param_count = 0;
    xtest_add_attr_value(&param_count, params,
                    TEE_ATTR_ECC_CURVE, TEE_ECC_CURVE_NIST_P256, 0);
    xtest_add_attr(&param_count, params,
                TEE_ATTR_ECC_PRIVATE_VALUE,
                nist_kas_ecc_cdh_testvector_69_private, size_bytes);
    xtest_add_attr(&param_count, params,
                TEE_ATTR_ECC_PUBLIC_VALUE_X,
                nist_kas_ecc_cdh_testvector_69_public_x, size_bytes);
    xtest_add_attr(&param_count, params,
                TEE_ATTR_ECC_PUBLIC_VALUE_Y,
                nist_kas_ecc_cdh_testvector_69_public_y, size_bytes);

	res = ta_crypt_cmd_populate_transient_object(&sess,
				key_handle, params, param_count);

	res = ta_crypt_cmd_set_operation_key(&sess, oph,
						       key_handle);
	
	res = ta_crypt_cmd_free_transient_object(&sess,
							   key_handle);

	res = ta_crypt_cmd_allocate_transient_object(&sess,
				TEE_TYPE_GENERIC_SECRET, size_bytes * 8, &sv_handle);

	memset(&op, 0, sizeof(op));
	param_count = 0;
    xtest_add_attr(&param_count, params,
                TEE_ATTR_ECC_PUBLIC_VALUE_X,
                nist_kas_ecc_cdh_testvector_69_public_x, size_bytes);
    xtest_add_attr(&param_count, params,
                TEE_ATTR_ECC_PUBLIC_VALUE_Y,
                nist_kas_ecc_cdh_testvector_69_public_y, size_bytes);


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

	out_size = sizeof(out);
	memset(out, 0, sizeof(out));
	res = ta_crypt_cmd_get_object_buffer_attribute(&sess,
				sv_handle, TEE_ATTR_SECRET_VALUE, out,
				&out_size);
    
    
	ta_crypt_cmd_free_operation(&sess, oph);
	ta_crypt_cmd_free_transient_object(&sess,
							   sv_handle);
	free(buf);

    if (memcmp(nist_kas_ecc_cdh_testvector_69_out, out, sizeof(nist_kas_ecc_cdh_testvector_69_out)) == 0) {
        return TEEC_SUCCESS;
    }

	return TEEC_ERROR_GENERIC;
}

int main(int argc, char** argv)
{
    TEEC_UUID uuid_1 = TA_ECDH_UUID_1;
    TEE_Result ret = TEEC_SUCCESS;

    ret = ta_crypt_cmd_derive_key_ecdh(&uuid_1);
    if (ret != 0) {
        printf("ecdh derive key error\r\n");
        return -1;
    } else {
        printf("ecdh derive key success\r\n");
    }

	return 0;
}

