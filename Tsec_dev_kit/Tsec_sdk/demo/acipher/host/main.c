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
#include <rsa_crypto.h>
#include <rsa_data.h>

#define RSA_TEST_KEY_SIZE 2048

/* TEE resources */
struct test_ctx {
	TEEC_Context ctx;
	TEEC_Session sess;
};


void prepare_tee_session(struct test_ctx *ctx)
{
	TEEC_UUID uuid = TA_RSA_UUID;
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

void prepare_rsa(struct test_ctx *ctx, uint32_t algo, uint32_t mode, uint32_t key_size)
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

	if (mode == TEE_MODE_ENCRYPT) {
		key_type = RSA_PUBLIC_KEY;
		op.params[1].value.b = key_type;
	} else if (mode == TEE_MODE_DECRYPT) {
		key_type = RSA_KEYPAIR;
		op.params[1].value.b = key_type;
	} else {
		errx(1, "Invalid mode: %d\n", mode);
	}

	res = TEEC_InvokeCommand(&ctx->sess, TA_RSA_CMD_PREPARE,
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
	op.params[0].tmpref.buffer = (void *)rsa_data_modulus_256;
	op.params[0].tmpref.size = 256;
	op.params[1].tmpref.buffer = (void *)rsa_data_pub_exp_256;
	op.params[1].tmpref.size = 3;

	if (mode == TEE_MODE_ENCRYPT) {
		op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
						TEEC_NONE, TEEC_NONE);
	} else if (mode == TEE_MODE_DECRYPT) {
		op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT,
						TEEC_MEMREF_TEMP_INPUT, TEEC_NONE);
		op.params[2].tmpref.buffer = (void *)rsa_data_priv_exp_256;
		op.params[2].tmpref.size = 256;
		
	} else {
		printf("Invalid mode: %d\n", mode);
		return;
	}

	res = TEEC_InvokeCommand(&ctx->sess, TA_RSA_CMD_SET_KEY,
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

	if (mode == TEE_MODE_ENCRYPT) {
		cmd = TA_RSA_CMD_ASYMMETRIC_ENCRYPT;
	} else if (mode == TEE_MODE_DECRYPT) {
		cmd = TA_RSA_CMD_ASYMMETRIC_DECRYPT;
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

int main()
{
	struct test_ctx ctx;
	uint8_t out[TA_RSA_KEY_SIZE_4096] = { };
	size_t out_size = 0;
	uint8_t out_enc[TA_RSA_KEY_SIZE_4096] = { };
	size_t out_enc_size = 0;

	prepare_tee_session(&ctx);

	prepare_rsa(&ctx, RSA_NOPAD,  TEE_MODE_ENCRYPT, RSA_TEST_KEY_SIZE);  /*set encrypt mode nopadding*/

	set_key(&ctx, TEE_MODE_ENCRYPT); /*set module and pub_exp of RSA*/

	out_size = RSA_TEST_KEY_SIZE/8;
	out_enc_size = RSA_TEST_KEY_SIZE/8;
	memset(out, 0, sizeof(out));
	memset(out_enc, 0, sizeof(out_enc));

	asymmetric_operate(&ctx, TEE_MODE_ENCRYPT, 0, rsa_data_ptx_data, RSA_TEST_KEY_SIZE/8-1, out_enc, &out_size); /*encrypt*/
	
	prepare_rsa(&ctx, RSA_NOPAD, TEE_MODE_DECRYPT, RSA_TEST_KEY_SIZE); /*set decypt mode nopadding*/
	
	set_key(&ctx,TEE_MODE_DECRYPT); /*set module and pub_exp and prv_exp of RSA*/
	
	asymmetric_operate(&ctx, TEE_MODE_DECRYPT, 0, out_enc, out_enc_size, out, &out_size); /*decypt*/

	if (memcmp(rsa_data_ptx_data, out, RSA_TEST_KEY_SIZE/8-1)) /*Determine whether the decrypted data matches the plaintext*/
		printf("\ndecoded text differ => ERROR\n");
	else
		printf("\ndecoded text match\n");

	terminate_tee_session(&ctx);

	return 0;
}
