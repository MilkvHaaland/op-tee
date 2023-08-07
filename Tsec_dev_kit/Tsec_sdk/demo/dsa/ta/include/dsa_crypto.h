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

#ifndef __DSA_TA_H__
#define __DSA_TA_H__

/* UUID of the RSA example trusted application */
#define TA_DSA_UUID \
	 { 0xde013140, 0xd35d, 0x4cbb, \
                { 0xb9, 0xa8, 0x95, 0xe5, 0xa2, 0xba, 0x5f, 0x46} }


#define HASH_MD5                        0x50000001
#define HASH_SHA1                       0x50000002
#define HASH_SHA224                     0x50000003
#define HASH_SHA256                     0x50000004
#define HASH_SHA384                     0x50000005
#define HASH_SHA512                     0x50000006
#define HASH_MD5SHA1                    0x5000000F

#define HASH_MD5_HASH_DATA_SIZE       (128 / 8)
#define HASH_SHA1_HASH_DATA_SIZE      (160 / 8)
#define HASH_SHA224_HASH_DATA_SIZE    (224 / 8)
#define HASH_SHA256_HASH_DATA_SIZE    (256 / 8)
#define HASH_SHA384_HASH_DATA_SIZE    (384 / 8)
#define HASH_SHA512_HASH_DATA_SIZE    (512 / 8)

#define TA_RSA_KEY_SIZE_256	    	(256 / 8)
#define TA_RSA_KEY_SIZE_384	    	(384 / 8)
#define TA_RSA_KEY_SIZE_512	    	(512 / 8)
#define TA_RSA_KEY_SIZE_640		    (640 / 8)
#define TA_RSA_KEY_SIZE_768		    (768 / 8)
#define TA_RSA_KEY_SIZE_896		    (896 / 8)
#define TA_RSA_KEY_SIZE_1024		(1024 / 8)
#define TA_RSA_KEY_SIZE_2048		(2048 / 8)
#define TA_RSA_KEY_SIZE_3072		(3072 / 8)
#define TA_DSA_KEY_SIZE_4096		(4096 / 8)

#define TEE_MODE_ENCRYPT		0
#define TEE_MODE_DECRYPT		1
#define TEE_MODE_SIGN           2
#define TEE_MODE_VERIFY         3
#define TEE_MODE_MAC            4
#define TEE_MODE_DIGEST         5
#define TEE_MODE_DERIVE         6

#define DSA_PUBLIC_KEY          0
#define DSA_KEYPAIR             1

#define TEE_ALG_DSA_SHA1                        0x70002131
#define TEE_ALG_DSA_SHA224                      0x70003131
#define TEE_ALG_DSA_SHA256                      0x70004131

/*
 * TA_RSA_CMD_PREPARE - Allocate resources for the RSA ciphering
 * param[0] (value) a: TA_RSA_ALGO_xxx, b: unused
 * param[1] (value) a: key size in bytes, b: key type
 * param[2] (value) a: TEE_MODE_ENCRYPT/_DECODE, b: unused
 * param[3] unused
 */
#define TA_DSA_CMD_PREPARE		0

/*
 * TA_RSA_CMD_SET_KEY - Allocate resources for the RSA ciphering
 * param[0] (memref) modulus data, size shall equal modulus length
 * param[1] (memref) pub_exp data, size shall equal pub_exp length
 * param[2] (memref) priv_exp data, size shall equal priv_exp length
 * param[3] unused
 */
#define TA_DSA_CMD_SET_KEY		1

/*
 * TA_RSA_CMD_ASYMMETRIC_SIGN_DIGEST - Sign input buffer into output buffer
 * param[0] (value) a: TEE_ATTR_RSA_PSS_SALT_LENGTH, b: unused
 * param[1] (memref) input buffer
 * param[2] (memref) output buffer
 * param[3] unused
 */
#define TA_DSA_CMD_ASYMMETRIC_SIGN_DIGEST		2

/*
 * TA_RSA_CMD_ASYMMETRIC_VERIFY_DIGEST - Verify input buffer into output buffer
 * param[0] (value) a: TEE_ATTR_RSA_PSS_SALT_LENGTH, b: unused
 * param[1] (memref) input buffer
 * param[2] (memref) input buffer
 * param[3] unused
 */
#define TA_DSA_CMD_ASYMMETRIC_VERIFY_DIGEST		3

#endif /* ___RSA_TA_H */

