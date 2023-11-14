/*
 * Copyright (c) 2023, Alibaba Group Holding Limited
 */

#include <err.h>
#include <stdio.h>
#include <string.h>

/* OP-TEE TEE client API (built by optee_client) */
#include <tee_client_api.h>

/* For the UUID (found in the TA's h-file(s)) */
#include <gpio_ta.h>
#include <gpio_cfg.h>

#define TEST_GPIO_DEV	GPIO_0
#define TEST_GPIO_PIN	29
#define TEST_GPIO_BANK	0	/* Fixed value */

static int gpio_config_get(struct csi_gpio_config *cfg)
{
    if (!cfg) {
        errx(1, "cfg is NULL pointer\r\n");
    }

    cfg->dev_idx = TEST_GPIO_DEV;
    cfg->offset = TEST_GPIO_PIN;
    cfg->bank = TEST_GPIO_BANK;
    return 0;
}

int ca_gpio_request(TEEC_Session *sess, struct csi_gpio_config *cfg)
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
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_INPUT, TEEC_NONE, TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = cfg;
    op.params[0].tmpref.size = sizeof(struct csi_gpio_config);
    res = TEEC_InvokeCommand(sess, TA_GPIO_REQUEST, &op, &err_origin);

    if (res != 0) {
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res,
             err_origin);
    }

    return 0;
}

int ca_gpio_set_value(TEEC_Session *sess, int value)
{
    int res;
    TEEC_Operation op;
    uint32_t err_origin;
    /* Clear the TEEC_Operation struct */
    memset(&op, 0, sizeof(op));
    /* Set parameters for TA */
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].value.a = value;
    res = TEEC_InvokeCommand(sess, TA_GPIO_SET_VALUE, &op, &err_origin);

    if (res != 0) {
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res,
             err_origin);
    }

    return 0;
}

int ca_gpio_toggle_value(TEEC_Session *sess)
{
    int res;
    uint32_t err_origin;
    res = TEEC_InvokeCommand(sess, TA_GPIO_TOGGLE_VALUE, NULL, &err_origin);

    if (res != 0) {
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res,
             err_origin);
    }

    return 0;
}

int ca_gpio_get_value(TEEC_Session *sess, int *value)
{
    int res;
    TEEC_Operation op;
    uint32_t err_origin;
    /* Clear the TEEC_Operation struct */
    memset(&op, 0, sizeof(op));
    /* Set parameters for TA */
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = value;
    op.params[0].tmpref.size = sizeof(int);
    res = TEEC_InvokeCommand(sess, TA_GPIO_GET_VALUE, &op, &err_origin);

    if (res != 0) {
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res,
             err_origin);
    }

    return 0;
}

int ca_gpio_set_direction(TEEC_Session *sess, int value)
{
    int res;
    TEEC_Operation op;
    uint32_t err_origin;
    /* Clear the TEEC_Operation struct */
    memset(&op, 0, sizeof(op));
    /* Set parameters for TA */
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_VALUE_INPUT, TEEC_NONE, TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].value.a = value;
    res = TEEC_InvokeCommand(sess, TA_GPIO_SET_DIRECTION, &op, &err_origin);

    if (res != 0) {
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res,
             err_origin);
    }

    return 0;
}

int ca_gpio_get_direction(TEEC_Session *sess, int *value)
{
    int res;
    TEEC_Operation op;
    uint32_t err_origin;
    /* Clear the TEEC_Operation struct */
    memset(&op, 0, sizeof(op));
    /* Set parameters for TA */
    op.paramTypes = TEEC_PARAM_TYPES(TEEC_MEMREF_TEMP_OUTPUT, TEEC_NONE, TEEC_NONE,
                                     TEEC_NONE);
    op.params[0].tmpref.buffer = value;
    op.params[0].tmpref.size = sizeof(int);
    res = TEEC_InvokeCommand(sess, TA_GPIO_GET_DIRECTION, &op, &err_origin);

    if (res != 0) {
        errx(1, "TEEC_InvokeCommand failed with code 0x%x origin 0x%x", res,
             err_origin);
    }

    return 0;
}

int main(void)
{
    TEEC_Result res;
    TEEC_Context ctx;
    TEEC_Session sess;
    TEEC_UUID uuid = TA_GPIO_UUID;
    uint32_t err_origin;
    struct csi_gpio_config cfg;
    int value = -1;
    int direction = -1;
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

    /* request GPIO0_29 */
    res = gpio_config_get(&cfg);

    if (res != TEEC_SUCCESS) {
        errx(1, "gpio_config_get failed with code 0x%x", res);
    }

    res = ca_gpio_request(&sess, &cfg);

    if (res != TEEC_SUCCESS) {
        errx(1, "ca_gpio_request failed with code 0x%x", res);
    }

    /* get GPIO0_29 direction */
    res = ca_gpio_get_direction(&sess, &direction);

    if (res != TEEC_SUCCESS) {
        errx(1, "ca_gpio_get_direction failed with code 0x%x", res);
    }

    // printf("GPIO%d_%d direction: %s\r\n", cfg.dev_idx, cfg.offset, direction == LIGHT_GPIO_DIR_OUT ? "OUTPUT" : "INPUT");
    /* reverse GPIO0_29 direction */
    direction = !direction;
    res = ca_gpio_set_direction(&sess, direction);

    if (res != TEEC_SUCCESS) {
        errx(1, "ca_gpio_set_direction failed with code 0x%x", res);
    }

    /* get GPIO0_29 direction */
    res = ca_gpio_get_direction(&sess, &direction);

    if (res != TEEC_SUCCESS) {
        errx(1, "ca_gpio_get_direction failed with code 0x%x", res);
    }

    /* get GPIO0_29 value */
    res = ca_gpio_get_value(&sess, &value);

    if (res != TEEC_SUCCESS) {
        errx(1, "ca_gpio_get_value failed with code 0x%x", res);
    }

    /* Toggle GPIO0_29 value */
    res = ca_gpio_toggle_value(&sess);

    if (res != TEEC_SUCCESS) {
        errx(1, "ca_gpio_toggle_value failed with code 0x%x", res);
    }

    /* get GPIO0_29 value */
    res = ca_gpio_get_value(&sess, &value);

    if (res != TEEC_SUCCESS) {
        errx(1, "ca_gpio_get_value failed with code 0x%x", res);
    }

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
