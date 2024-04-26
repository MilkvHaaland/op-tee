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
#include <tee_client_api.h>
#include <se_ta.h>
#include <spi_cfg.h>
#include <time.h>
#include <scp_cfg.h>

#define SE_APDU_OUTDATA_MAX_SIZE 258
void display_hex(char *desc, uint8_t *data, size_t len) {
    int i = 0;
    printf("\n%s: ", desc);
    for (i = 0; i < len; i++) {
        if (data[i] <= 0x0f) {
            printf("0%x ", data[i]);
        } else {
            printf("%x", data[i]);
        }
    }
    printf("\n");
}

static int se_spi_config_get(struct csi_spi_config *cfg)
{
    if (!cfg) {
        return -1;
    }
    cfg->speed = 3000000;
    cfg->format = 0;
    cfg->mode = 0;
    cfg->timeout = 1000;
    return 0;
}

static int se_scp_config_get(struct csi_scp_config *cfg)
{
    if (!cfg) {
        return -1;
    }
    cfg->protocol = CSI_GP211_SCP03;
    cfg->param_i = CSI_GP211_SCP03_IMPL_i00;
    cfg->security_level = CSI_GP211_SCP01_SECURITY_LEVEL_C_DEC_C_MAC;
    return 0;
}

static int se_scp_status_config_get(struct csi_status_config *cfg)
{
    if (!cfg) {
        return -1;
    }
    cfg->card_element = CSI_GP211_STATUS_APPLICATIONS;
    cfg->format = CSI_GP211_STATUS_FORMAT_NEW;
    return 0;
}

int ca_se_init(TEEC_Session *sess, struct csi_spi_config *cfg, uint32_t idx)
{
    int res;
    TEEC_Operation op;
    uint32_t err_origin;

    /* Clear the TEEC_Operation struct */
    memset(&op, 0, sizeof(op));

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = cfg;
    op.params[0].tmpref.size = sizeof(*cfg);
    op.params[1].value.a = idx;

    /*
     * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
     * called.
     */
    res = TEEC_InvokeCommand(sess, TA_SE_INIT, &op, &err_origin);
    if (res != 0) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x cmdID:%d\n", res, err_origin, TA_SE_INIT);
    }

    return 0;
}

int ca_se_uninit(TEEC_Session *sess)
{
    TEEC_Result res;
    uint32_t err_origin;

    /*
     * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
     * called.
     */
    res = TEEC_InvokeCommand(sess, TA_SE_UNINIT, NULL,  &err_origin);
    if (res != 0) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x cmdID:%d\n", res, err_origin, TA_SE_UNINIT);
    }

    return 0;
}

int ca_se_atr(TEEC_Session *sess)
{
    TEEC_Result res;
    uint32_t err_origin;

    /*
     * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
     * called.
     */
    res = TEEC_InvokeCommand(sess, TA_SE_ATR, NULL,  &err_origin);
    if (res != 0) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x cmdID:%d\n", res, err_origin, TA_SE_ATR);
    }

    return 0;
}

int ca_se_channel_open(TEEC_Session *sess)
{
    TEEC_Result res;
    uint32_t err_origin;

    /*
     * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
     * called.
     */
    res = TEEC_InvokeCommand(sess, TA_SE_CHANNEL_OPEN, NULL,  &err_origin);
    if (res != 0) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x cmdID:%d\n", res, err_origin, TA_SE_CHANNEL_OPEN);
    }

    return 0;
}

int ca_se_channel_close(TEEC_Session *sess)
{
    TEEC_Result res;
    uint32_t err_origin;

    /*
     * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
     * called.
     */
    res = TEEC_InvokeCommand(sess, TA_SE_CHANNEL_CLOSE, NULL,  &err_origin);
    if (res != 0) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x cmdID:%d\n", res, err_origin, TA_SE_CHANNEL_CLOSE);
    }

    return 0;
}

int ca_se_channel_reset(TEEC_Session *sess)
{
    TEEC_Result res;
    uint32_t err_origin;

    /*
     * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
     * called.
     */
    res = TEEC_InvokeCommand(sess, TA_SE_RESET, NULL,  &err_origin);
    if (res != 0) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x cmdID:%d\n", res, err_origin, TA_SE_RESET);
    }

    return 0;
}

int ca_se_channel_tranmit(TEEC_Session *sess, void *sdata, size_t slen, void *rdata, size_t *rlen)
{
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t err_origin;

    /* Clear the TEEC_Operation struct */
    memset(&op, 0, sizeof(op));

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE);
    op.params[0].tmpref.buffer = sdata;
    op.params[0].tmpref.size = slen;

    op.params[1].tmpref.buffer = rdata;
    op.params[1].tmpref.size = SE_APDU_OUTDATA_MAX_SIZE;

    op.params[2].tmpref.buffer = rlen;

    /*
     * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
     * called.
     */
    res = TEEC_InvokeCommand(sess, TA_SE_CHANNEL_TRANSMIT, &op, &err_origin);
    if (res != 0) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x cmdID:%d\n", res, err_origin, TA_SE_CHANNEL_TRANSMIT);
    }

    return 0;
}

int ca_se_scp_mutual_auth(TEEC_Session *sess, struct csi_scp_config *cfg)
{
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t err_origin;

    /* Clear the TEEC_Operation struct */
    memset(&op, 0, sizeof(op));

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = cfg;

    /*
     * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
     * called.
     */
    res = TEEC_InvokeCommand(sess, TA_SE_SCP_MUTUAL_AUTH, &op, &err_origin);
    if (res != 0) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x cmdID:%d\n", res, err_origin, TA_SE_CHANNEL_TRANSMIT);
    }

    return 0;
}

int ca_se_scp_get_status(TEEC_Session *sess, struct csi_status_config *cfg)
{
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t err_origin;

    /* Clear the TEEC_Operation struct */
    memset(&op, 0, sizeof(op));

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = cfg;

    /*
     * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
     * called.
     */
    res = TEEC_InvokeCommand(sess, TA_SE_SCP_GET_STATUS, &op, &err_origin);
    if (res != 0) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x cmdID:%d\n", res, err_origin, TA_SE_CHANNEL_TRANSMIT);
    }

    return 0;
}

uint16_t se_test_count = 1;
int se_scp_test(TEEC_Session *sess)
{
    int res;
    struct csi_scp_config scp_cfg;
    struct csi_status_config get_status_cfg;

    res = se_scp_config_get(&scp_cfg);
    if (res != TEEC_SUCCESS)
        errx(1, "se_scp_config_get failed with code 0x%x", res);

    res = se_scp_status_config_get(&get_status_cfg);
    if (res != TEEC_SUCCESS)
        errx(1, "se_scp_status_config_get failed with code 0x%x", res);

    res = ca_se_scp_mutual_auth(sess, &scp_cfg);
    if (res != TEEC_SUCCESS)
        errx(1, "ca_se_scp_mutual_auth failed with code 0x%x", res);
    printf("SCP mutual authentication success!\n");

    res = ca_se_scp_get_status(sess, &get_status_cfg);
    if (res != TEEC_SUCCESS)
        errx(1, "ca_se_scp_get_status failed with code 0x%x", res);
    printf("SCP get status success!\n");

    return 0;
}

int se_chip_version_get_test(TEEC_Session *sess)
{
    int res, i;

    uint8_t  ui8Capdu_isd[] = {0x00,0xa4,0x04,0x00,0x00};
    uint8_t  ui8Capdu_cdm[] = {0x00,0xA4,0x04,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    uint8_t  ui8Capdu_ver[] = {0x80,0x02,0x01,0x00,0x0C};
    uint8_t  out_data[SE_APDU_OUTDATA_MAX_SIZE] = {0};
    size_t out_len = 0;

    for (i = 0; i < se_test_count; i++) {
        res = ca_se_channel_tranmit(sess, ui8Capdu_isd, sizeof(ui8Capdu_isd), out_data, &out_len);
        if (res != TEEC_SUCCESS)
            errx(1, "ca_se_channel_tranmit failed with code 0x%x", res);
        printf("select se isd success!\n");

        res = ca_se_channel_tranmit(sess, ui8Capdu_cdm, sizeof(ui8Capdu_cdm), out_data, &out_len);
        if (res != TEEC_SUCCESS)
            errx(1, "ca_se_channel_tranmit failed with code 0x%x", res);
        printf("select se cardManger success!\n");

        res = ca_se_channel_tranmit(sess, ui8Capdu_ver, sizeof(ui8Capdu_ver), out_data, &out_len);
        if (res != TEEC_SUCCESS)
            errx(1, "ca_se_channel_tranmit failed with code 0x%x", res);
        printf("get se version success!\n");
        display_hex("version", out_data, out_len);

        res = ca_se_channel_reset(sess);
        if (res != TEEC_SUCCESS)
            errx(1, "ca_se_channel_reset failed with code 0x%x", res);
        printf("reset se success!\n");
    }

    return 0;
}

int main(void)
{
    int res;
    TEEC_Context ctx;
    TEEC_Session sess;

    TEEC_UUID uuid = TA_SE_UUID;
    uint32_t err_origin;
    uint32_t spi_idx = 0;
    struct csi_spi_config cfg;

    /* Initialize a context connecting us to the TEE */
    res = TEEC_InitializeContext(NULL, &ctx);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

    /*
     * Open a session to the "hello world" TA, the TA will print "hello
     * world!" in the log when the session is created.
     */
    res = TEEC_OpenSession(&ctx, &sess, &uuid,
                   TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
    if (res != TEEC_SUCCESS)
        errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
            res, err_origin);

    res = se_spi_config_get(&cfg);
    if (res != TEEC_SUCCESS)
        errx(1, "se_spi_config_get failed with code 0x%x", res);

    res = ca_se_init(&sess, &cfg, spi_idx);
    if (res != TEEC_SUCCESS)
        errx(1, "ca_se_init failed with code 0x%x", res);
    printf("init se spi success!\n");
    
    res = ca_se_atr(&sess);
    if (res != TEEC_SUCCESS)
        errx(1, "ca_se_atr failed with code 0x%x", res);
    
    res = ca_se_channel_open(&sess);
    if (res != TEEC_SUCCESS)
        errx(1, "ca_se_channel_open failed with code 0x%x", res);

    printf("se_chip_version_get_test start!\n");
    res = se_chip_version_get_test(&sess);
    if (res != TEEC_SUCCESS)
        errx(1, "se_chip_version_get_test failed with code 0x%x", res);
    printf("se_chip_version_get_test end!\n");

    printf("se_scp_test start!\n");
    res = se_scp_test(&sess);
    if (res != TEEC_SUCCESS)
        errx(1, "se_scp_test failed with code 0x%x", res);
    printf("se_scp_test end!\n");

    res = ca_se_channel_close(&sess);
    if (res != TEEC_SUCCESS)
        errx(1, "ca_se_channel_close failed with code 0x%x", res);
    printf("close se channel success!\n");

    res = ca_se_uninit(&sess);
    if (res != TEEC_SUCCESS)
        errx(1, "ta_spi_uninit failed with code 0x%x", res);
    printf("uninit se success!\n");
    
    TEEC_CloseSession(&sess);

    TEEC_FinalizeContext(&ctx);

    return 0;
}
