// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (C) 2021 Alibaba Group Holding Limited.
 *
 */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <tee_client_api.h>
#include <pta_misc.h>

static TEEC_Result get_lifecycle(char *life_cycle_str)
{
    TEEC_Result res = TEEC_SUCCESS;
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = TA_MISC_UUID;
    uint32_t err_origin;

    if (life_cycle_str == NULL) {
        return TEEC_ERROR_BAD_PARAMETERS;
    }

    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        return res;
    }

	res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS) {
	    TEEC_FinalizeContext(&ctx);
        return res;
    }

	memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_OUTPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    res = TEEC_InvokeCommand(&sess, PTA_MISC_GET_LC, &op, &err_origin);
    if (res != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x\r\n", res, err_origin);
    } else {
        switch (op.params[0].value.a) {
            case TEE_PLAT_LC_INIT: {
                strcpy(life_cycle_str, "INIT");
                break;
            }
            case TEE_PLAT_LC_DEV: {
                strcpy(life_cycle_str, "DEV");
                break;
            }
            case TEE_PLAT_LC_OEM: {
                strcpy(life_cycle_str, "OEM");
                break;
            }
            case TEE_PLAT_LC_PRO: {
                strcpy(life_cycle_str, "PRO");
                break;
            }
            case TEE_PLAT_LC_RMA: {
                strcpy(life_cycle_str, "RMA");
                break;
            }
            default: {
                strcpy(life_cycle_str, "UNKNOWN");
                break;
            }
        }
    }

	TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);

	return res;
}

static TEEC_Result csi_efuse_update_lc_pro(void)
{
    TEEC_Result res = TEEC_SUCCESS;
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = TA_MISC_UUID;
    uint32_t err_origin;

    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        return res;
    }

	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS) {
	    TEEC_FinalizeContext(&ctx);
        return res;
    }

	memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].value.a = TEE_PLAT_LC_PRO;

    res = TEEC_InvokeCommand(&sess, PTA_MISC_UPDATE_LC, &op, &err_origin);
    if (res != TEEC_SUCCESS) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x\r\n", res, err_origin);
        goto exit;
    }

exit:
	TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);

	return res;
}

static int csi_efuse_read(uint32_t offset, void *data, uint32_t cnt)
{
    TEEC_Result res;
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = TA_MISC_UUID;
    uint32_t err_origin;
    int ret = 0;

    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        return 0;
    }

	res = TEEC_OpenSession(&ctx, &sess, &uuid,
			       TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS) {
	    TEEC_FinalizeContext(&ctx);
        return 0;
    }

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_OUTPUT,TEEC_NONE, TEEC_NONE);
    op.params[0].value.a = offset;
    op.params[0].value.b = 0;
    op.params[1].tmpref.buffer = data;
    op.params[1].tmpref.size = cnt;

    res = TEEC_InvokeCommand(&sess, PTA_MISC_EFUSE_READ, &op, &err_origin);
    if (res != TEEC_SUCCESS) {
        ret = 0;
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x\r\n", res, err_origin);
        goto exit;
    }

    ret = cnt;
    
exit:
	TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);

	return ret;
}

static int csi_efuse_write(uint32_t offset, void *data, uint32_t cnt)
{
    TEEC_Result res;
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_Operation op;
    TEEC_UUID uuid = TA_MISC_UUID;
    uint32_t err_origin;
    int ret = 0;

    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS) {
        return -2;
    }

	res = TEEC_OpenSession(&ctx, &sess, &uuid, TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS) {
	    TEEC_FinalizeContext(&ctx);
        return -3;
    }

    memset(&op, 0, sizeof(op));
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE);
    op.params[0].value.a = offset;
    op.params[0].value.b = 0;
    op.params[1].tmpref.buffer = data;
    op.params[1].tmpref.size = cnt;

    res = TEEC_InvokeCommand(&sess, PTA_MISC_EFUSE_WRITE, &op, &err_origin);
    if (res != TEEC_SUCCESS){
        ret = -4;
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x\r\n", res, err_origin);
        goto exit;
    }
    
    ret = cnt;
exit:
	TEEC_CloseSession(&sess);
	TEEC_FinalizeContext(&ctx);

	return ret;
}

static TEEC_Result csi_efuse_offset_write_demo(void)
{
    int ret = 0;
    int i = 0;
    unsigned char data[16] = {0xff, 0xef, 0xdf, 0xcf, 0xbf, 0xaf, 0x9f, 0x8f,0x7f, 0x6f, 0x5f, 0x4f, 0x3f, 0x2f, 0x1f, 0x0f};
    unsigned char data2[16] = {0};
    uint32_t addr = 480;

    /* Write data */
    ret = csi_efuse_write(addr, data, sizeof(data));
    if (ret != sizeof(data)) {
        printf("csi_efuse_write failed");
		return TEEC_ERROR_GENERIC;
    }

    /* read back */
    ret = csi_efuse_read(addr, data2, sizeof(data2));
    if (ret != sizeof(data)) {
        printf("csi_efuse_read readback failed");
        return TEEC_ERROR_GENERIC;
    }

    /* compare data readback */
    if (memcmp(data, data2, sizeof(data)) != 0) {
        printf("efuse demo readback compare failed");
        return TEEC_ERROR_GENERIC;
    }

    printf("efuse write & read demo exec success\r\n");

    return TEEC_SUCCESS;
}

int main()
{
    int ret;
    char life[32] = {0};

    /*efuse read write test*/
    ret = csi_efuse_offset_write_demo();
    if (ret != TEEC_SUCCESS) {
        printf("csi_efuse_offset_write_demo failed, ret:%d\r\n", ret);
        return -1;
    } else {
        printf("csi_efuse_offset_write_demo failed:%d\r\n", ret);
    }

    /*get life cycle*/
    ret = get_lifecycle(life);
    if (ret != TEEC_SUCCESS) {
        printf("get_lifecycle failed, ret:%d\r\n", ret);
        return -2;
    } else {
        printf("get life cycle:%s\r\n", life);
    }

    /*update life cycle to product*/
    ret = csi_efuse_update_lc_pro();
    if (ret != TEEC_SUCCESS) {
        printf("update life cycle to product failed, ret:%d\r\n",ret);
        return -3;
    } else {
        printf("update life cycle to product success\r\n");
    }

    return 0;
}
