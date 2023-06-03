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
#define TA_ECDH_UUID_1 {0x8165d0ff, 0x2186, 0x4105, { 0xa1, 0x18, 0xba, 0x53, 0x28, 0xe6, 0x3c, 0x12}}

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
#define TEE_ECC_CURVE_25519                 0x00000301
#define TEE_ATTR_X25519_PUBLIC_VALUE         0xD0000944
#define TEE_ATTR_X25519_PRIVATE_VALUE        0xC0000A44
#define TEE_ALG_ECDH_P256                       0x80003042
#define TEE_TYPE_ECDH_KEYPAIR               0xA1000042
#define TEE_ATTR_ECC_PRIVATE_VALUE          0xC0000341
#define TEE_ATTR_ECC_PUBLIC_VALUE_X         0xD0000141
#define TEE_ATTR_ECC_PUBLIC_VALUE_Y         0xD0000241
#define TEE_ECC_CURVE_NIST_P256             0x00000003
#define TEE_ALG_X25519                      0x80000044
#define TEE_ATTR_X25519_PUBLIC_VALUE         0xD0000944
#define TEE_ATTR_X25519_PRIVATE_VALUE        0xC0000A44
#define TEST_BUFF_SIZE 1024
#define TEE_ATTR_ECC_CURVE                  0xF0000441
#define TEE_ATTR_FLAG_VALUE		(1 << 29)

#define TEE_ALG_X25519                  0x80000044
#define TEE_ATTR_SECRET_VALUE               0xC0000000
#define TEE_TYPE_GENERIC_SECRET             0xA0000000
#define TEE_HANDLE_NULL                   0
#define TEEC_OPERATION_INITIALIZER	{ }
typedef uint32_t TEE_ObjectType;
typedef uint32_t TEE_Result;

#define TEE_ATTR_BIT_VALUE		TEE_ATTR_FLAG_VALUE
typedef struct __TEE_OperationHandle *TEE_OperationHandle;
typedef struct __TEE_ObjectHandle *TEE_ObjectHandle;
#define ROUNDUP(v, size) (((v) + (size - 1)) & ~(size - 1))

#endif /* __MEM_TA_H__ */
