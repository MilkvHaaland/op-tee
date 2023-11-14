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
#include <string.h>
#include <shared_memory.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

#define SHARED_MEMORY_TEST_TRANSFER_SIZE 4096

int main(void)
{
	int i = 0;
	TEEC_Result res;
	TEEC_Context ctx;
	TEEC_Session sess;
	TEEC_Operation op;
	TEEC_UUID uuid = TA_SHARED_MEMORY_UUID;
	uint32_t err_origin;
    
    TEEC_SharedMemory shm = { };

	shm.size = SHARED_MEMORY_TEST_TRANSFER_SIZE;
	shm.flags = TEEC_MEM_INPUT;

	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

    res = TEEC_AllocateSharedMemory(&ctx, &shm);
    if (res != TEEC_SUCCESS)
		errx(1, "TEEC_AllocateSharedMemory failed with code 0x%x", res);
	
	/*init data*/
	for(i = 0;i < SHARED_MEMORY_TEST_TRANSFER_SIZE;++i) {
        ((uint8_t*)(shm.buffer))[i] = i % 127;
	}


	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			res, err_origin);

	/*
	 * Execute a function in the TA by invoking it, in this case
	 * we're incrementing a number.
	 *
	 * The value of command ID part and how the parameters are
	 * interpreted is part of the interface provided by the TA.
	 */
	memset(&op, 0, sizeof(op));
	op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT,
					 TEEC_NONE,
					 TEEC_NONE, TEEC_NONE);
	op.params[0].tmpref.buffer = (void *)shm.buffer;
	op.params[0].tmpref.size = SHARED_MEMORY_TEST_TRANSFER_SIZE;

	/*
	 * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
	 * called.
	 */
	res = TEEC_InvokeCommand(&sess, TA_SHARED_MEMORY_CMD_TRANSFER_TEST, &op,
				 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
			res, err_origin);
    else
	    printf("shared memory transfer test success\n");

	/*
	 * We're done with the TA, close the session and
	 * destroy the context.
	 *
	 * The TA will print "Goodbye!" in the log when the
	 * session is closed.
	 */
	TEEC_ReleaseSharedMemory(&shm);
	TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);

	return 0;
}
