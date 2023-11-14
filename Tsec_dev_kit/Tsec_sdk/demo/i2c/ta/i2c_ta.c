/*
 * Copyright (c) 2023, Alibaba Group Holding Limited
 */

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <i2c_ta.h>
#include <i2c_hal.h>

/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void)
{
    return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
                                    TEE_Param __maybe_unused params[4],
                                    void __maybe_unused **sess_ctx)
{
    TEE_Result res;
    void *i2c_handle = NULL;
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        DMSG("param types:0x%x is not expect:0x%x\n", param_types, exp_param_types);
        return TEE_ERROR_BAD_PARAMETERS;
    }

    /* Initialize I2C handle */
    DMSG("has been called\r\n");
    res = csi_i2c_open(&i2c_handle);
    // res = csi_i2c_test();
    if (res != 0) {
        EMSG("csi_i2c_open failed\r\n");
        return res;
    }

    /* Unused parameters */
    (void)&params;
    /* Session context points to gpio_handle */
    *sess_ctx = (void *)i2c_handle;
    /*
     * The DMSG() macro is non-standard, TEE Internal API doesn't
     * specify any means to logging from a TA.
     */
    DMSG("Open I2C Session!\n");
    /* If return value != TEE_SUCCESS the session will not be created. */
    return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
    (void)&sess_ctx; /* Unused parameter */
}

static TEE_Result ta_i2c_init(void *sess_ctx, uint32_t param_types,
                                  TEE_Param params[4])
{
    void *i2c_handle = sess_ctx;
    struct csi_i2c_config *cfg = NULL;
    uint32_t idx = 0;
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                               TEE_PARAM_TYPE_VALUE_INPUT,
                               TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        DMSG("param types:0x%x is not expect:0x%x\n", param_types, exp_param_types);
        return TEE_ERROR_BAD_PARAMETERS;
    }
    /* Retrieve config value from params*/
    cfg = (struct csi_i2c_config *)params[0].memref.buffer;
    idx = params[1].value.a;
    DMSG("I2C controller: %d, mode: %d, speed mode: %d, addr_mode: %d", idx, cfg->mode, cfg->speed,
         cfg->addr_mode);
    return csi_i2c_init(i2c_handle, cfg, idx);
}

static TEE_Result ta_i2c_uninit(void *sess_ctx)
{
    void *i2c_handle = sess_ctx;
    DMSG("Uninit I2C controller\r\n");
    return csi_i2c_uninit(i2c_handle);
}

static TEE_Result ta_csi_i2c_master_send(void *sess_ctx, uint32_t param_types,
                                    TEE_Param params[4])
{
    void *i2c_handle = sess_ctx;
    struct csi_i2c_msg *message = NULL;
    uint8_t *tx_buf = NULL;

    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                           TEE_PARAM_TYPE_MEMREF_INPUT,
                           TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        DMSG("param types:0x%x is not expect:0x%x\n", param_types, exp_param_types);
        return TEE_ERROR_BAD_PARAMETERS;
    }

    tx_buf = params[0].memref.buffer;
    message = (struct csi_i2c_msg *)params[1].memref.buffer;
    

    DMSG("I2C controller send message in Master mode. device addr: %02x, len: %d, offset: %d\r\n", message->addr, message->len, message->offset);
    return csi_i2c_master_send(i2c_handle, tx_buf, message);
}

static TEE_Result ta_csi_i2c_master_receive(void *sess_ctx, uint32_t param_types,
                                    TEE_Param params[4])
{
    void *i2c_handle = sess_ctx;
    struct csi_i2c_msg *message = NULL;
    uint8_t *rx_buf = NULL;

    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_OUTPUT,
                           TEE_PARAM_TYPE_MEMREF_INPUT,
                           TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        DMSG("param types:0x%x is not expect:0x%x\n", param_types, exp_param_types);
        return TEE_ERROR_BAD_PARAMETERS;
    }
    
    rx_buf = params[0].memref.buffer;
    message = (struct csi_i2c_msg *)params[1].memref.buffer;
    DMSG("I2C controller receive message in Master mode. device addr: %02x, len: %d, offset: %d\r\n", message->addr, message->len, message->offset);
    return csi_i2c_master_receive(i2c_handle, rx_buf, message);
}

/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
                                      uint32_t cmd_id,
                                      uint32_t param_types, TEE_Param params[4])
{
    (void)&sess_ctx; /* Unused parameter */

    switch (cmd_id) {
    case TA_I2C_INIT:
        return ta_i2c_init(sess_ctx, param_types, params);

    case TA_I2C_UNINIT:
        return ta_i2c_uninit(sess_ctx);

    case TA_I2C_MASTER_SEND:
        return ta_csi_i2c_master_send(sess_ctx, param_types, params);

    case TA_I2C_MASTER_RECEIVE:
        return ta_csi_i2c_master_receive(sess_ctx, param_types, params);

    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
