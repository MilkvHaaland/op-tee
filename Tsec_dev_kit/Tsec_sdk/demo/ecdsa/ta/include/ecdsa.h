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

#ifndef __MEM_TA_H__
#define __MEM_TA_H__

#include <stdint.h>

/*
 * This UUID is generated with uuidgen
 * the ITU-T UUID generator at http://www.itu.int/ITU-T/asn1/uuid.html
 */
#define TA_ECDSA_UUID_1 {0x1265d0f1, 0xa186, 0x4101, { 0xa1, 0x18, 0xba, 0x33, 0x21, 0xe6, 0x3c, 0x11}}

/* The function ID implemented in this TA */
#define TA_MEM_TEST     0
#define TA_SHARE_MEM_TEST   1
#define TA_CRYPT_DERIVE_KEY_TEST    2
#define TA_CRYPT_CMD_ALLOCATE_TRANSIENT_OBJECT    3
#define TA_CRYPT_CMD_POPULATE_TRANSIENT_OBJECT    4
#define TA_CRYPT_CMD_FREE_TRANSIENT_OBJECT    5
#define TA_CRYPT_CMD_ALLOCATE_OPERATION    6
#define TA_CRYPT_CMD_SET_OPERATION_KEY    7
#define TA_CRYPT_CMD_GET_OBJECT_BUFFER_ATTRIBUTE    8
#define TA_CRYPT_CMD_FREE_OPERATION    9
#define TA_25519_CMD_GET_KEY		10
#define TA_DSA_CMD_GEN_KEY		    11
#define TA_CRYPT_CMD_GENERATE_KEY   12
#define TA_CRYPT_CMD_GET_OBJECT_VALUE_ATTRIBUTE 13
#define TA_CRYPT_CMD_ASYMMETRIC_VERIFY_DIGEST 15

#define TEE_ALG_ECDH_P256                       0x80003042
#define TEE_TYPE_ECDH_KEYPAIR               0xA1000042
#define TEE_ATTR_ECC_PRIVATE_VALUE          0xC0000341
#define TEE_ATTR_ECC_PUBLIC_VALUE_X         0xD0000141
#define TEE_ATTR_ECC_PUBLIC_VALUE_Y         0xD0000241
#define TEE_ECC_CURVE_NIST_P256             0x00000003
#define TEST_BUFF_SIZE 1024
#define TEE_ATTR_ECC_CURVE                  0xF0000441
#define TEE_ATTR_FLAG_VALUE		(1 << 29)
#define TEE_TYPE_DSA_KEYPAIR                0xA1000031
#define TEE_ALG_SHA1                            0x50000002
#define TEE_ATTR_SECRET_VALUE               0xC0000000
#define TEE_TYPE_GENERIC_SECRET             0xA0000000
#define TEE_HANDLE_NULL                   0
#define TEE_MAX_HASH_SIZE 64
#define TEE_TYPE_DSA_PUBLIC_KEY             0xA0000031
#define TEE_TYPE_ECDSA_PUBLIC_KEY           0xA0000041
#define TEE_CRYPTO_ELEMENT_NONE             0x00000000
#define TEE_ECC_CURVE_NIST_P192             0x00000001
#define TEE_ECC_CURVE_NIST_P224             0x00000002
#define TEE_ECC_CURVE_NIST_P256             0x00000003
#define TEE_ECC_CURVE_NIST_P384             0x00000004
#define TEE_ECC_CURVE_NIST_P521             0x00000005
#define TEE_ECC_CURVE_SM2                   0x00000300
#define TEE_TYPE_ECDSA_KEYPAIR              0xA1000041
#define TEE_ALG_ECDSA_P256                      0x70003041
#define TA_CRYPT_CMD_ASYMMETRIC_SIGN_DIGEST     29
#define TEE_ALG_ECDSA_P256                      0x70003041

#define TEEC_OPERATION_INITIALIZER	{ }
typedef uint32_t TEE_ObjectType;
typedef uint32_t TEE_Result;

#define TEE_ATTR_BIT_VALUE		TEE_ATTR_FLAG_VALUE
typedef struct __TEE_OperationHandle *TEE_OperationHandle;
typedef struct __TEE_ObjectHandle *TEE_ObjectHandle;
#define ECDSA_ROUNDUP(v, size) (((v) + (size - 1)) & ~(size - 1))

#define KEY_ATTR(x, y) { #x, (x), y }

struct key_attrs {
	const char *name;
	uint32_t attr;
	/*
	 * When keysize_check != 0: size of attribute is checked
	 * Expected value is key_size bits except for DH in which case it is
	 * the value of keysize_check.
	 */
	uint32_t keysize_check;
};


#endif /* __MEM_TA_H__ */
