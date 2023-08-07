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

#ifndef __RSA_TA_H__
#define __RSA_TA_H__

/* UUID of the RSA example trusted application */
#define TA_RSA_UUID \
	{ 0x69d97060, 0x9d3c, 0x4499, \
                { 0xb7, 0x67, 0x01, 0xdb, 0x71, 0xb7, 0xba, 0x03} }


#define RSA_NOPAD                       0x60000030


#define TA_RSA_KEY_SIZE_4096		(4096 / 8)

#define TEE_MODE_ENCRYPT		0
#define TEE_MODE_DECRYPT		1


#define RSA_PUBLIC_KEY          0
#define RSA_KEYPAIR             1

#define RSA_MODULUS                0xD0000130
#define RSA_PUBLIC_EXPONENT        0xD0000230
#define RSA_PRIVATE_EXPONENT       0xC0000330
#define RSA_PRIME1                 0xC0000430
#define RSA_PRIME2                 0xC0000530
#define RSA_EXPONENT1              0xC0000630
#define RSA_EXPONENT2              0xC0000730
#define RSA_COEFFICIENT            0xC0000830

/*
 * TA_RSA_CMD_PREPARE - Allocate resources for the RSA ciphering
 * param[0] (value) a: TA_RSA_ALGO_xxx, b: unused
 * param[1] (value) a: key size in bytes, b: key type
 * param[2] (value) a: TEE_MODE_ENCRYPT/_DECODE, b: unused
 * param[3] unused
 */
#define TA_RSA_CMD_PREPARE		0

/*
 * TA_RSA_CMD_SET_KEY - Allocate resources for the RSA ciphering
 * param[0] (memref) modulus data, size shall equal modulus length
 * param[1] (memref) pub_exp data, size shall equal pub_exp length
 * param[2] (memref) priv_exp data, size shall equal priv_exp length
 * param[3] unused
 */
#define TA_RSA_CMD_SET_KEY		1

/*
 * TA_RSA_CMD_ASYMMETRIC_ENCRYPT - Encrypt input buffer into output buffer
 * param[0] (value) a: TEE_ATTR_RSA_PSS_SALT_LENGTH, b: unused
 * param[1] (memref) input buffer
 * param[2] (memref) output buffer
 * param[3] unused
 */
#define TA_RSA_CMD_ASYMMETRIC_ENCRYPT		2

/*
 * TA_RSA_CMD_ASYMMETRIC_DECRYPT - Decrypt input buffer into output buffer
 * param[0] (value) a: TEE_ATTR_RSA_PSS_SALT_LENGTH, b: unused
 * param[1] (memref) input buffer
 * param[2] (memref) output buffer
 * param[3] unused
 */
#define TA_RSA_CMD_ASYMMETRIC_DECRYPT		3

/*
 * TA_RSA_CMD_ASYMMETRIC_SIGN_DIGEST - Sign input buffer into output buffer
 * param[0] (value) a: TEE_ATTR_RSA_PSS_SALT_LENGTH, b: unused
 * param[1] (memref) input buffer
 * param[2] (memref) output buffer
 * param[3] unused
 */


#endif /* __RSA_TA_H */
