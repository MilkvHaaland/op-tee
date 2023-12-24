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
#include <spi_ta.h>
#include <spi_cfg.h>
#include <se_crc.h>

#define SE_SPI_FORMAT  1
#define SE_SPI_SPEED   1000000

#define NOR_SPI_FORMAT 0
#define NOR_SPI_SPEED  1000000
#define NAND_QSPI_SPEED 100000
unsigned char spi_io_buf[1024] = {0};

/* 内部使用 */
#define CRC_ST_CUSTOM            0x10
#define CRC_ST_INIT              0x11
#define CRC_ST_UPDATE            0x13

struct _crc {
    uint16_t init;   /* basic parameters */
    uint16_t xor;
    uint8_t  reverse_in;
    uint8_t  reverse_out;
    uint8_t  mode;
    uint16_t value;  /* result */
    uint8_t  st;     /* status */
    uint8_t  endian; /* endian */
}crc;

static void crc_ccit_custom(uint16_t init, uint16_t xor, uint8_t reverse_in, uint8_t reverse_out);
static uint8_t crc_ccit_init(uint8_t mode, uint8_t endian);
static uint8_t crc_ccit_update(uint8_t * data, uint32_t len);
static uint16_t crc_ccit_get_result(void);
crc_t g_crc = {
    crc_ccit_custom,
    crc_ccit_init,
    crc_ccit_update,
    crc_ccit_get_result,
};

const static uint16_t crc_ccit_table[256] = {
    0x0000U, 0x1021U, 0x2042U, 0x3063U, 0x4084U, 0x50A5U, 0x60C6U, 0x70E7U,
    0x8108U, 0x9129U, 0xA14AU, 0xB16BU, 0xC18CU, 0xD1ADU, 0xE1CEU, 0xF1EFU,
    0x1231U, 0x0210U, 0x3273U, 0x2252U, 0x52B5U, 0x4294U, 0x72F7U, 0x62D6U,
    0x9339U, 0x8318U, 0xB37BU, 0xA35AU, 0xD3BDU, 0xC39CU, 0xF3FFU, 0xE3DEU,
    0x2462U, 0x3443U, 0x0420U, 0x1401U, 0x64E6U, 0x74C7U, 0x44A4U, 0x5485U,
    0xA56AU, 0xB54BU, 0x8528U, 0x9509U, 0xE5EEU, 0xF5CFU, 0xC5ACU, 0xD58DU,
    0x3653U, 0x2672U, 0x1611U, 0x0630U, 0x76D7U, 0x66F6U, 0x5695U, 0x46B4U,
    0xB75BU, 0xA77AU, 0x9719U, 0x8738U, 0xF7DFU, 0xE7FEU, 0xD79DU, 0xC7BCU,
    0x48C4U, 0x58E5U, 0x6886U, 0x78A7U, 0x0840U, 0x1861U, 0x2802U, 0x3823U,
    0xC9CCU, 0xD9EDU, 0xE98EU, 0xF9AFU, 0x8948U, 0x9969U, 0xA90AU, 0xB92BU,
    0x5AF5U, 0x4AD4U, 0x7AB7U, 0x6A96U, 0x1A71U, 0x0A50U, 0x3A33U, 0x2A12U,
    0xDBFDU, 0xCBDCU, 0xFBBFU, 0xEB9EU, 0x9B79U, 0x8B58U, 0xBB3BU, 0xAB1AU,
    0x6CA6U, 0x7C87U, 0x4CE4U, 0x5CC5U, 0x2C22U, 0x3C03U, 0x0C60U, 0x1C41U,
    0xEDAEU, 0xFD8FU, 0xCDECU, 0xDDCDU, 0xAD2AU, 0xBD0BU, 0x8D68U, 0x9D49U,
    0x7E97U, 0x6EB6U, 0x5ED5U, 0x4EF4U, 0x3E13U, 0x2E32U, 0x1E51U, 0x0E70U,
    0xFF9FU, 0xEFBEU, 0xDFDDU, 0xCFFCU, 0xBF1BU, 0xAF3AU, 0x9F59U, 0x8F78U,
    0x9188U, 0x81A9U, 0xB1CAU, 0xA1EBU, 0xD10CU, 0xC12DU, 0xF14EU, 0xE16FU,
    0x1080U, 0x00A1U, 0x30C2U, 0x20E3U, 0x5004U, 0x4025U, 0x7046U, 0x6067U,
    0x83B9U, 0x9398U, 0xA3FBU, 0xB3DAU, 0xC33DU, 0xD31CU, 0xE37FU, 0xF35EU,
    0x02B1U, 0x1290U, 0x22F3U, 0x32D2U, 0x4235U, 0x5214U, 0x6277U, 0x7256U,
    0xB5EAU, 0xA5CBU, 0x95A8U, 0x8589U, 0xF56EU, 0xE54FU, 0xD52CU, 0xC50DU,
    0x34E2U, 0x24C3U, 0x14A0U, 0x0481U, 0x7466U, 0x6447U, 0x5424U, 0x4405U,
    0xA7DBU, 0xB7FAU, 0x8799U, 0x97B8U, 0xE75FU, 0xF77EU, 0xC71DU, 0xD73CU,
    0x26D3U, 0x36F2U, 0x0691U, 0x16B0U, 0x6657U, 0x7676U, 0x4615U, 0x5634U,
    0xD94CU, 0xC96DU, 0xF90EU, 0xE92FU, 0x99C8U, 0x89E9U, 0xB98AU, 0xA9ABU,
    0x5844U, 0x4865U, 0x7806U, 0x6827U, 0x18C0U, 0x08E1U, 0x3882U, 0x28A3U,
    0xCB7DU, 0xDB5CU, 0xEB3FU, 0xFB1EU, 0x8BF9U, 0x9BD8U, 0xABBBU, 0xBB9AU,
    0x4A75U, 0x5A54U, 0x6A37U, 0x7A16U, 0x0AF1U, 0x1AD0U, 0x2AB3U, 0x3A92U,
    0xFD2EU, 0xED0FU, 0xDD6CU, 0xCD4DU, 0xBDAAU, 0xAD8BU, 0x9DE8U, 0x8DC9U,
    0x7C26U, 0x6C07U, 0x5C64U, 0x4C45U, 0x3CA2U, 0x2C83U, 0x1CE0U, 0x0CC1U,
    0xEF1FU, 0xFF3EU, 0xCF5DU, 0xDF7CU, 0xAF9BU, 0xBFBAU, 0x8FD9U, 0x9FF8U,
    0x6E17U, 0x7E36U, 0x4E55U, 0x5E74U, 0x2E93U, 0x3EB2U, 0x0ED1U, 0x1EF0U
};

static uint16_t u16_bit_reverse(uint16_t data)
{
    data = (data & 0xFF00) >> 8 | (data & 0x00FF) << 8;
    data = (data & 0xF0F0) >> 4 | (data & 0x0F0F) << 4;
    data = (data & 0xCCCC) >> 2 | (data & 0x3333) << 2;
    data = (data & 0xAAAA) >> 1 | (data & 0x5555) << 1;
    return data;
}

static uint8_t u8_bit_reverse(uint8_t data)
{
    data = (data & 0xF0) >> 4 | (data & 0x0F) << 4;
    data = (data & 0xCC) >> 2 | (data & 0x33) << 2;
    data = (data & 0xAA) >> 1 | (data & 0x55) << 1;
    return data;
}

static void crc_ccit_custom(uint16_t init, uint16_t xor, uint8_t reverse_in, uint8_t reverse_out)
{
    crc.init = init;
    crc.xor = xor;
    crc.reverse_in = reverse_in;
    crc.reverse_out = reverse_out;
    crc.mode = CRC_CUSTOM;
    crc.st = CRC_ST_CUSTOM;
}

static uint8_t crc_ccit_init(uint8_t mode, uint8_t endian)
{
    if (crc.st != CRC_ST_CUSTOM) {
        crc.init = 0x0000;
        crc.xor = 0x0000;
        crc.reverse_in = 0;
        crc.reverse_out = 0;
        crc.mode = mode;
        crc.st = CRC_ST_INIT;
    }

    if (endian != 0 && endian != 1) {
        crc.st = CRC_ST_ERR;
        return CRC_ST_ERR;
    }

    crc.value = 0;
    crc.endian = endian;
    switch (mode) {
        case CRC_CCIT_KERMIT: {
            crc.reverse_in = 1;
            crc.reverse_out = 1;
        }break;
        case CRC_CCIT_XMODEM: {
            /* crc_calc default mode */
        }break;
        case CRC_CCIT_FALSE: {
            crc.init = 0xFFFF;
        }break;
        case CRC_CCIT_GENIBUS:
        {
            crc.init = 0xFFFF;
            crc.xor = 0xFFFF;
        }break;
        case CRC_CCIT_X_25: {
            crc.init = 0xFFFF;
            crc.xor = 0xFFFF;
            crc.reverse_in = 1;
            crc.reverse_out = 1;
        }break;
        case CRC_CCIT_MCRF4XX: {
            crc.init = 0xFFFF;
        }break;
        case CRC_CCIT_AUG_CCIT: {
            crc.init = 0x1D0F;
        }break;
        case CRC_CCIT_TMS37157: {
            crc.init = 0x89EC;
            crc.reverse_in = 1;
            crc.reverse_out = 1;
        }break;
        case CRC_CCIT_RIELLO: {
            crc.init = 0xB2AA;
            crc.reverse_in = 1;
            crc.reverse_out = 1;
        }break;
        case CRC_CCIT_CRC_A: {
            crc.init = 0xC6C6;
            crc.reverse_in = 1;
            crc.reverse_out = 1;
        }break;
        case CRC_CUSTOM: {
            if (crc.st != CRC_ST_CUSTOM) {
                crc.st = CRC_ST_ERR;
                return CRC_ST_ERR;
            }
            crc.st = CRC_ST_INIT;
        }break;
        default: {
            crc.st = CRC_ST_ERR;
            return CRC_ST_ERR;
        }/*break;*/
    }

    crc.value = crc.init;

    return CRC_ST_SUCCESS;
}

static uint8_t crc_ccit_update(uint8_t *data, uint32_t len)
{
    uint16_t crc_value;
    uint16_t temp;

    if (crc.st != CRC_ST_INIT && crc.st != CRC_ST_UPDATE) {
        crc.st = CRC_ST_ERR;
        return CRC_ST_ERR;
    }

    crc_value = crc.value;
    while (len --) {
        temp = (uint16_t)(crc_value >> 8);
        if (crc.reverse_in == 1) {
            temp ^= u8_bit_reverse(*data);
        } else {
            temp ^= *(data);
        }

        crc_value = ((crc_value << 8) ^ crc_ccit_table[temp & 0xff]) & 0xFFFF;
        data++;
    }

    crc.value = crc_value;
    crc.st = CRC_ST_UPDATE;

    return CRC_ST_SUCCESS;
}


static uint16_t crc_ccit_get_result(void)
{
    uint16_t crc_value;
    if (crc.st != CRC_ST_UPDATE) {
        return 0;
    }

    crc_value = crc.value;
    if (crc.reverse_out == 1) {
        crc_value = u16_bit_reverse(crc_value);
    }
    crc_value = crc_value ^ crc.xor;

    if (crc.endian == 1) {
        crc.value = (crc_value & 0xff) << 8 | (crc_value & 0xff00) >> 8;
    } else {
        crc.value = crc_value;
    }

    /* 状态置为无效 */
    crc.st = CRC_ST_SUCCESS;

    return crc.value;
}

void delay_time(uint32_t times)
{
    printf(".");
}

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

int ca_spi_init(TEEC_Session *sess, struct csi_spi_config *cfg, uint32_t idx)
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
    res = TEEC_InvokeCommand(sess, TA_SPI_INIT, &op, &err_origin);
    if (res != 0) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
    }

    return 0;
}

int ca_spi_uninit(TEEC_Session *sess)
{
    TEEC_Result res;
    uint32_t err_origin;

    /*
     * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
     * called.
     */
    res = TEEC_InvokeCommand(sess, TA_SPI_UNINIT, NULL,  &err_origin);
    if (res != 0) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
    }

    return 0;
}

int ca_spi_send_then_receive(TEEC_Session *sess, const void *sdata, size_t slen, void *rdata, size_t rlen)
{
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t err_origin;

    /* Clear the TEEC_Operation struct */
    memset(&op, 0, sizeof(op));

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = sdata;
    op.params[0].tmpref.size = slen;

    op.params[1].tmpref.buffer = rdata;
    op.params[1].tmpref.size = rlen;

    /*
     * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
     * called.
     */
    res = TEEC_InvokeCommand(sess, TA_SPI_SEND_THEN_RECEIVE, &op, &err_origin);
    if (res != 0) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
    }

    return 0;
}

int ca_spi_send(TEEC_Session *sess, const void *sdata, size_t slen)
{
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t err_origin;

    /* Clear the TEEC_Operation struct */
    memset(&op, 0, sizeof(op));

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = sdata;
    op.params[0].tmpref.size = slen;

    /*
     * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
     * called.
     */
    res = TEEC_InvokeCommand(sess, TA_SPI_CMD_SEND_BYTES, &op, &err_origin);
    if (res != 0) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x",
        res, err_origin);
    }

    return 0;
}

int ca_spi_receive(TEEC_Session *sess, const void *rdata, size_t rlen)
{
    TEEC_Result res;
    TEEC_Operation op;
    uint32_t err_origin;

    /* Clear the TEEC_Operation struct */
    memset(&op, 0, sizeof(op));

    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE, TEEC_NONE);
    op.params[0].tmpref.buffer = rdata;
    op.params[0].tmpref.size = rlen;

    /*
     * TA_HELLO_WORLD_CMD_INC_VALUE is the actual function in the TA to be
     * called.
     */
    res = TEEC_InvokeCommand(sess, TA_SPI_CMD_RECEIVE_BYTES, &op, &err_origin);
    if (res != 0) {
        printf("TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res, err_origin);
    }

    return 0;
}

unsigned char ca_spi_send_one_byte(TEEC_Session *sess, unsigned char sdata)
{
    if (ca_spi_send(sess, &sdata, 1)) {
        printf("%s error\r\n", __func__);
        return 1;
    }

    return 0;
}

unsigned char ca_spi_receive_one_byte(TEEC_Session *sess)
{
    unsigned char rdata;

    if (ca_spi_receive(sess, &rdata, 1)) {
        printf("%s error\r\n", __func__);
        return 1;
    }

    return rdata;
}

unsigned int se_spi_send(TEEC_Session *sess, unsigned char * ucData, unsigned int uiLength)
{
    unsigned int len = 0, i = 0;
    unsigned short crc;

    /* get crc data */
    g_crc.init(CRC_CCIT_KERMIT, 1);
    g_crc.update(ucData, uiLength);
    crc = g_crc.get_result();

    /* SPI master send data */
    spi_io_buf[i++] = 0x00;
    spi_io_buf[i++] = 0x00;
    spi_io_buf[i++] = 0x00;
    spi_io_buf[i++] = 0x55;
    spi_io_buf[i++] = 0xAA;
    spi_io_buf[i++] = 0x00;
    spi_io_buf[i++] = (uiLength >> 8) & 0xff;   /* length */
    spi_io_buf[i++] = uiLength & 0xff;

    memmove(spi_io_buf + i, ucData, uiLength);  /* data */
    i += uiLength;

    spi_io_buf[i++] = (crc >> 8) & 0xff;        /* crc data */
    spi_io_buf[i++] = crc & 0xff;

    for (len = 0; len < 8; len++) {
        ca_spi_send_one_byte(sess, spi_io_buf[len]);
    }

    delay_time(10);

    for (; len < i; len++) {
        ca_spi_send_one_byte(sess, spi_io_buf[len]);
    }

    return 0;
}


unsigned int se_spi_receive(TEEC_Session *sess, unsigned char *ucData, unsigned int *puiLength)
{
    unsigned short crc = 0, crc_result;
    int len = 0, i = 0;
    unsigned char data;

    while (0x00 != ca_spi_receive_one_byte(sess));
    while (0x00 != ca_spi_receive_one_byte(sess));
    while (0x00 != ca_spi_receive_one_byte(sess));
    while (0xAA != ca_spi_receive_one_byte(sess));
    while (0x55 != ca_spi_receive_one_byte(sess));
    if (0x00 != ca_spi_receive_one_byte(sess)) {
        printf("%s %d error\r\n", __func__, __LINE__);
        return 1;
    }

    /* receive data length */
    len = ca_spi_receive_one_byte(sess);
    len <<= 8;
    len |= ca_spi_receive_one_byte(sess);
    if (*puiLength >= len) {
        *puiLength = len;
    } else {
        printf("%s %d error\r\n", __func__, __LINE__);
        return 1;
    }

    /* receive data */
    for (i = 0; i < len; i++) {
        spi_io_buf[i] = ca_spi_receive_one_byte(sess);
        ucData[i] = spi_io_buf[i];
    }

    /* crc data check*/
    g_crc.init(CRC_CCIT_KERMIT, 1);
    g_crc.update(spi_io_buf, len);
    crc_result = g_crc.get_result();

    crc = ca_spi_receive_one_byte(sess);
    crc <<= 8;
    crc |= ca_spi_receive_one_byte(sess);

    if (crc != crc_result) {
        printf("%s %d error\r\n", __func__, __LINE__);
        return 1;
    }

    return 0;
}

int se_spi_transmit(TEEC_Session *sess, unsigned char *sdata, unsigned int slen,
                        unsigned char *rdata, unsigned int *rlen)
{
    if (se_spi_send(sess, sdata, slen) != 0) {
        printf("%s %d error\r\n", __func__, __LINE__);
        return 1;
    }

    if (se_spi_receive(sess, rdata, rlen) != 0) {
        printf("%s %d error\r\n", __func__, __LINE__);
        return 1;
    }

    return 0;
}

static int se_spi_config_get(struct csi_spi_config *cfg)
{
    cfg->speed = SE_SPI_SPEED;
    cfg->format = SE_SPI_FORMAT;
    cfg->mode = CSI_SPI_MODE_MASTER;
    cfg->timeout = CSI_DEFAULT_SPI_POLL_TIMEOUT_MS;

    return 0;
}

static int nor_spi_config_get(struct csi_spi_config *cfg, uint32_t idx)
{
    if (idx == 0) {
        cfg->speed = NOR_SPI_SPEED;
    } else if (idx == 1) {
        cfg->speed = NAND_QSPI_SPEED;
    }
    
    cfg->format = NOR_SPI_FORMAT;
    cfg->mode = CSI_SPI_MODE_MASTER;
    cfg->timeout = CSI_DEFAULT_SPI_POLL_TIMEOUT_MS;

    return 0;
}


int spi_nor_flash_scan(uint32_t spi_idx)
{
    int res;
    TEEC_Context ctx;
    TEEC_Session sess;

    TEEC_UUID uuid = TA_SPI_UUID;
    uint32_t err_origin;

    struct csi_spi_config cfg;
    uint8_t  nor_scan_cmd[1] = {0x9f};
    uint8_t  nor_scan_id[4] = {0x00};

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

    res = nor_spi_config_get(&cfg, spi_idx);
    if (res != TEEC_SUCCESS)
        errx(1, "nor_spi_config_init_get failed with code 0x%x", res);

    res = ca_spi_init(&sess, &cfg, spi_idx);
    if (res != TEEC_SUCCESS)
        errx(1, "nor_spi_init failed with code 0x%x", res);

    res = ca_spi_send_then_receive(&sess, nor_scan_cmd, sizeof(nor_scan_cmd), nor_scan_id, sizeof(nor_scan_id));
    if (res != TEEC_SUCCESS)
        errx(1, "ta_spi_write_then_read failed with code 0x%x", res);

    res = ca_spi_uninit(&sess);
    if (res != TEEC_SUCCESS)
        errx(1, "ta_spi_uninit failed with code 0x%x", res);

    display_hex("nor-flash-id", nor_scan_id, sizeof(nor_scan_id));

    TEEC_CloseSession(&sess);

    TEEC_FinalizeContext(&ctx);

    return 0;
}

int se_hsck2_scan_id(TEEC_Session *sess)
{
    int i;
    int         status;
    uint8_t     command[5] =  {0x00,0xBC,0x00,0x00,0x00}; /* get se version cmd*/
    uint8_t     buf[64] = {0};
    size_t      len = sizeof(buf);

    if (!sess) {
        return -1;
    }

    status = se_spi_transmit(sess, command, sizeof(command), buf, &len);
    if (status) {
        printf("se: probe send command error!\n");
        return -1;
    }

    if (!len) {
        printf("%s %d error\r\n", __func__, __LINE__);
        return -1;
    }

    if (buf[len-2] != 0x90 && buf[len-1] != 0x00) {
        printf("%s %d error\r\n", __func__, __LINE__);
        return -1;
    }

    printf("se information:\n");
    for (i = 0; i < len-2; i++) {
        printf("%X ", buf[i]);
    }
    printf("\n");

    return 0;
}



int se_hsck2_scan(void)
{
    int res;
    TEEC_Context ctx;
    TEEC_Session sess;

    TEEC_UUID uuid = TA_SPI_UUID;
    uint32_t err_origin;

    struct csi_spi_config cfg;
    uint32_t spi_idx = 0;
    uint8_t  nor_scan_cmd[1] = {0x9f};
    uint8_t  nor_scan_id[6] = {0x00};

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
        errx(1, "nor_spi_config_init_get failed with code 0x%x", res);

    res = ca_spi_init(&sess, &cfg, spi_idx);
    if (res != TEEC_SUCCESS)
        errx(1, "nor_spi_init failed with code 0x%x", res);

    res = se_hsck2_scan_id(&sess);
    if (res != TEEC_SUCCESS)
        errx(1, "se_hsck2_scan_id failed with code 0x%x", res);

    TEEC_CloseSession(&sess);

    TEEC_FinalizeContext(&ctx);

    return 0;
}

int qspi_nand_flash_scan(uint32_t qspi_idx)
{
    return spi_nor_flash_scan(qspi_idx);
}

int main(void)
{
    int res;

    printf("qspi_nand_flash_scan start!\n");
    res = qspi_nand_flash_scan(1);
    if (res != TEEC_SUCCESS)
        errx(1, "qspi_nand_flash_scan failed with code 0x%x", res);
    printf("qspi_nand_flash_scan end!\n");

    printf("spi_nor_flash_scan start!\n");
    res = spi_nor_flash_scan(0);
    if (res != TEEC_SUCCESS)
        errx(1, "spi_nor_flash_scan failed with code 0x%x", res);
    printf("spi_nor_flash_scan end!\n");

    // printf("se_hsck2_scan start!\n");
    // res = se_hsck2_scan();
    // if (res != TEEC_SUCCESS)
    //     errx(1, "se_hsck2_scan failed with code 0x%x", res);
    // printf("se_hsck2_scan end!\n");

    return 0;
}
