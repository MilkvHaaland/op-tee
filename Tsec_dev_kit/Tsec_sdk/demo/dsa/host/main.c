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

#include <err.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* For the UUID (found in the TA's h-file(s)) */
#include <dsa_crypto.h>
#include <dsa_data.h>



#define STR(str) #str

/* TEE resources */
struct test_ctx {
	TEEC_Context ctx;
	TEEC_Session sess;
};


void prepare_tee_session(struct test_ctx *ctx)
{
	TEEC_UUID uuid = TA_DSA_UUID;
	uint32_t origin;
	TEEC_Result res;

	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx->ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	/* Open a session with the TA */
	res = TEEC_OpenSession(&ctx->ctx, &ctx->sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, origin);
}

void terminate_tee_session(struct test_ctx *ctx)
{
	TEEC_CloseSession(&ctx->sess);
	TEEC_FinalizeContext(&ctx->ctx);
}

static uint16_t ta2tee_get_hash_size(uint32_t algo)
{
	switch (algo) {
	case HASH_MD5:
		return HASH_MD5_HASH_DATA_SIZE;
	case HASH_SHA1:
		return HASH_SHA1_HASH_DATA_SIZE;
	case HASH_SHA224:
		return HASH_SHA224_HASH_DATA_SIZE;
	case HASH_SHA256:
		return HASH_SHA256_HASH_DATA_SIZE;
	case HASH_SHA384:
		return HASH_SHA384_HASH_DATA_SIZE;
	case HASH_SHA512:
		return HASH_SHA512_HASH_DATA_SIZE;
	case HASH_MD5SHA1:
		return HASH_MD5_HASH_DATA_SIZE;
	default:
		printf("Invalid algo 0x%x", algo);
		return -1;
	}
}

void prepare_dsa(struct test_ctx *ctx, uint32_t algo, uint32_t mode, uint32_t key_size)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;
	uint32_t key_type;

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
					 TEEC_VALUE_INPUT,
					 TEEC_VALUE_INPUT,
					 TEEC_NONE);

	op.params[0].value.a = algo;
	op.params[1].value.a = key_size;
	op.params[2].value.a = mode;

	if (mode == TEE_MODE_VERIFY) {
		key_type = DSA_KEYPAIR;
		op.params[1].value.b = key_type;
	} else if (mode == TEE_MODE_SIGN) {
		key_type = DSA_KEYPAIR;
		op.params[1].value.b = key_type;
	} else if (mode == TEE_MODE_DIGEST) {

	} else {
		errx(1, "Invalid mode: %d\n", mode);
	}

	res = TEEC_InvokeCommand(&ctx->sess, TA_DSA_CMD_PREPARE,
				 &op, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(PREPARE) failed 0x%x origin 0x%x",
			res, origin);
}

void set_key(struct test_ctx *ctx, uint32_t mode)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;

	memset(&op, 0, sizeof(op));
	

	if (mode == TEE_MODE_VERIFY || mode == TEE_MODE_SIGN) {
		op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
					  TEEC_MEMREF_TEMP_INPUT, TEEC_NONE);
		op.params[0].tmpref.buffer = (void *)keygen_dsa_2048_p;
		op.params[0].tmpref.size = 256;
		op.params[1].tmpref.buffer = (void *)keygen_dsa_2048_q;
		op.params[1].tmpref.size = 32;
		op.params[2].tmpref.buffer = (void *)keygen_dsa_2048_g;
		op.params[2].tmpref.size = 256;
	} else {
		printf("Invalid mode: %d\n", mode);
		return;
	}

	res = TEEC_InvokeCommand(&ctx->sess, TA_DSA_CMD_SET_KEY,
				 &op, &origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(SET_KEY) failed 0x%x origin 0x%x",
			res, origin);
}


void asymmetric_operate(struct test_ctx *ctx, uint32_t mode, int salt_len, const void *in, size_t in_size, void *out, size_t *out_size)
{
	TEEC_Operation op;
	uint32_t origin;
	TEEC_Result res;
	uint32_t cmd;

	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT,
					 TEEC_MEMREF_TEMP_INPUT,
					 TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE);
	op.params[0].value.a = salt_len;
	op.params[1].tmpref.buffer = (void *)in;
	op.params[1].tmpref.size = in_size;
	op.params[2].tmpref.buffer = out;
	op.params[2].tmpref.size = *out_size;

	if (mode == TEE_MODE_SIGN) {
		cmd = TA_DSA_CMD_ASYMMETRIC_SIGN_DIGEST;
	} else {
		printf("Invalid mode: %d\n", mode);
		return;
	}

	res = TEEC_InvokeCommand(&ctx->sess, cmd, &op, &origin);
	*out_size = op.params[2].tmpref.size;
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand(ASYMMETRIC_OPERATE) failed 0x%x origin 0x%x",
			res, origin);
}


int main(int argc, char *const argv[])
{
	struct test_ctx ctx;
	uint8_t out[TA_DSA_KEY_SIZE_4096] = { };
	size_t out_size = 0;
	size_t ptx_hash_size = 0;

	printf("Prepare session with the TA\n");
	prepare_tee_session(&ctx);

	ptx_hash_size = ta2tee_get_hash_size(HASH_SHA256);

	prepare_dsa(&ctx, TEE_ALG_DSA_SHA256, TEE_MODE_SIGN, 2048/8);
	set_key(&ctx, TEE_MODE_SIGN);
	out_size = 256;
	memset(out, 0, sizeof(out));
	asymmetric_operate(&ctx, TEE_MODE_SIGN, 0, dsa_data_ptx_data, ptx_hash_size, out, &out_size);
	printf("DSA signature completed\n");

	terminate_tee_session(&ctx);

	return 0;
}
