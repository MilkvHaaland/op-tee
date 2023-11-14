/*
 * Copyright (c) 2023, Alibaba Group Holding Limited
 */

#include <err.h>
#include <stdio.h>
#include <string.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* For the UUID (found in the TA's h-file(s)) */
#include <i2c_ta.h>
#include <i2c_cfg.h>

#define DEFAULT_I2C_CONTROLLER	    0
#define EEPROM_I2C_DEVICE   	    0x50
#define EEPROM_I2C_ADDR_MODE        CSI_I2C_ADDRESS_7BIT
#define EEPROM_I2C_MEM_ADDR_SIZE    CSI_I2C_MEM_ADDR_SIZE_16BIT


static int e2prom_i2c_config_get(struct csi_i2c_config *cfg)
{
    if (!cfg) {
        errx(1, "cfg is NULL pointer\r\n");
    }

    cfg->mode = CSI_I2C_MODE_MASTER;
    cfg->speed = CSI_IC_SPEED_MODE_STANDARD;
    cfg->addr_mode = EEPROM_I2C_ADDR_MODE;
    return 0;
}

static void display_hex(uint8_t *buf, uint32_t len)
{
    int i = 0;
    while (i < len) {
        printf("%02x ", buf[i]);
        i++;
        if (i % 16 == 0) {
            printf("\r\n");
        }
    }
    printf("\r\n");
}

int ca_i2c_init(TEEC_Session *sess, struct csi_i2c_config *cfg, uint32_t idx)
{
    int res;
    TEEC_Operation op;
    uint32_t err_origin;

    if (!cfg) {
        printf("cfg is NULL pointer\r\n");
        return -1;
    }

    /* Clear the TEEC_Operation struct */
    memset(&op, 0, sizeof(op));
    /* Set parameters for TA */
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_VALUE_INPUT, TEEC_NONE,
                                     TEEC_NONE);
    
    op.params[0].tmpref.buffer = cfg;
    op.params[0].tmpref.size = sizeof(struct csi_i2c_config);
    op.params[1].value.a = idx;

    res = TEEC_InvokeCommand(sess, TA_I2C_INIT, &op, &err_origin);

    if (res != 0) {
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res,
             err_origin);
    }

    return 0;
}

int ca_i2c_uninit(TEEC_Session *sess)
{
    int res;
    uint32_t err_origin;

    res = TEEC_InvokeCommand(sess, TA_I2C_UNINIT, NULL, &err_origin);

    if (res != 0) {
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res,
             err_origin);
    }

    return 0;
}

int ca_i2c_master_send(TEEC_Session *sess, uint8_t *tx_buf, struct csi_i2c_msg *message)
{
    int res;
    TEEC_Operation op;
    uint32_t err_origin;
    /* Clear the TEEC_Operation struct */
    memset(&op, 0, sizeof(op));

    /* Set parameters for TA */
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_NONE,
                                     TEEC_NONE);
    if (!message) {
        errx(1, "ca_i2c_master_send failed with null pointer error\r\n");
    }

    op.params[0].tmpref.buffer = tx_buf;
    op.params[0].tmpref.size = message->len;
    op.params[1].tmpref.buffer = message;
    op.params[1].tmpref.size = sizeof(struct csi_i2c_msg);

    res = TEEC_InvokeCommand(sess, TA_I2C_MASTER_SEND, &op, &err_origin);

    if (res != 0) {
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res,
             err_origin);
    }

    return 0;
}

int ca_i2c_master_receive(TEEC_Session *sess, uint8_t *rx_buf, struct csi_i2c_msg *message)
{
    int res;
    TEEC_Operation op;
    uint32_t err_origin;
    /* Clear the TEEC_Operation struct */
    memset(&op, 0, sizeof(op));

    /* Set parameters for TA */
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_MEMREF_TEMP_INPUT, TEEC_NONE,
                                     TEEC_NONE);
    if (!message) {
        errx(1, "ca_i2c_master_receive failed with null pointer error\r\n");
    }
    op.params[0].tmpref.buffer = rx_buf;
    op.params[0].tmpref.size = message->len;
    op.params[1].tmpref.buffer = message;
    op.params[1].tmpref.size = sizeof(struct csi_i2c_msg);
    res = TEEC_InvokeCommand(sess, TA_I2C_MASTER_RECEIVE, &op, &err_origin);

    if (res != 0) {
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res,
             err_origin);
    }

    return 0;
}

int i2c_e2prom_scan(void)
{
    TEEC_Result res;
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_UUID uuid = TA_I2C_UUID;
    uint32_t err_origin;
    struct csi_i2c_config cfg;
    struct csi_i2c_msg message;
    uint8_t rx_buf[32] = {0};
    uint8_t tx_buf[32] = {0x13, 0x99, 0x22};

    message.addr = EEPROM_I2C_DEVICE;
    message.len = 32;
    message.offset = 0;
    message.mem_addr_size = EEPROM_I2C_MEM_ADDR_SIZE;

    /* Initialize a context connecting us to the TEE */
    res = TEEC_InitializeContext(NULL, &ctx);

    if (res != TEEC_SUCCESS) {
        errx(1, "TEEC_InitializeContext failed with code 0x%x", res);
    }

    /*
     * Open a session to the "hello world" TA, the TA will print "hello
     * world!" in the log when the session is created.
     */
    res = TEEC_OpenSession(&ctx, &sess, &uuid,
                           TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);

    if (res != TEEC_SUCCESS) {
        errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
             res, err_origin);
    }

    /* initialize I2C controller */
    res = e2prom_i2c_config_get(&cfg);

    if (res != TEEC_SUCCESS) {
        errx(1, "i2c_config_get failed with code 0x%x", res);
    }

    res = ca_i2c_init(&sess, &cfg, DEFAULT_I2C_CONTROLLER);

    if (res != TEEC_SUCCESS) {
        errx(1, "ca_i2c_init failed with code 0x%x", res);
    }

    
    res = ca_i2c_master_send(&sess, tx_buf, &message);

    if (res != TEEC_SUCCESS) {
        errx(1, "ca_i2c_master_send failed with code 0x%x", res);
    }

    /* read buffer at 0x0 */
    res = ca_i2c_master_receive(&sess, rx_buf, &message);
    if (res != TEEC_SUCCESS) {
        errx(1, "ca_i2c_master_receive failed with code 0x%x", res);
    }

    
    res = ca_i2c_uninit(&sess);
    if (res != TEEC_SUCCESS) {
        errx(1, "ca_i2c_init failed with code 0x%x", res);
    }
   

    printf("send buffer to EEPROM at offset 0x%04x:\r\n", message.offset);
    display_hex(tx_buf, message.len);

    printf("buffer in device EEPROM at offset 0x%04x is:\r\n", message.offset);
    display_hex(rx_buf, message.len);

    /*
     * We're done with the TA, close the session and
     * destroy the context.
     *
     * The TA will print "Goodbye!" in the log when the
     * session is closed.
     */
    TEEC_CloseSession(&sess);
    TEEC_FinalizeContext(&ctx);
    return 0;
}

int main(void)
{
    int res = TEEC_SUCCESS;

    printf("i2c_e2prom_scan start!\r\n");
    res = i2c_e2prom_scan();
    if (res != TEEC_SUCCESS)
        errx(1, "i2c_e2prom_scan failed with code 0x%x", res);
    printf("i2c_e2prom_scan end!\n");

    return res;
}
