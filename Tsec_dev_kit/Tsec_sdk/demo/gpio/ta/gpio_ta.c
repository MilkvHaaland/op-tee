/*
 * Copyright (c) 2023, Alibaba Group Holding Limited
 */

#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <gpio_ta.h>
#include <gpio_hal.h>

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
    void *gpio_handle = NULL;
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    /* Initialize GPIO handle */
    res = csi_gpio_init(&gpio_handle);

    if (res != 0) {
        EMSG("csi_gpio_init failed\r\n");
        return res;
    }

    /* Unused parameters */
    (void)&params;
    /* Session context points to gpio_handle */
    *sess_ctx = (void *)gpio_handle;
    /*
     * The DMSG() macro is non-standard, TEE Internal API doesn't
     * specify any means to logging from a TA.
     */
    DMSG("Open GPIO Session!\n");
    /* If return value != TEE_SUCCESS the session will not be created. */
    return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
    TEE_Result res;
    void *gpio_handle = sess_ctx;
    res = csi_gpio_uninit(gpio_handle);

    if (res != 0) {
        EMSG("csi_gpio_uninit failed\r\n");
    }

    DMSG("Close TEE Session!\n");
}

static TEE_Result ta_gpio_request(void *sess_ctx, uint32_t param_types,
                                  TEE_Param params[4])
{
    void *gpio_handle = sess_ctx;
    struct csi_gpio_config *cfg = NULL;
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                               TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    /* Retrieve config value from params*/
    cfg = (struct csi_gpio_config *)params[0].memref.buffer;
    DMSG("GPIO Device: %d, bank: %d, pin: %d", cfg->dev_idx, cfg->bank,
         cfg->offset);
    return csi_gpio_request(gpio_handle, cfg);
}

static TEE_Result ta_gpio_set_value(void *sess_ctx, uint32_t param_types,
                                    TEE_Param params[4])
{
    void *gpio_handle = sess_ctx;
    int val = 0;
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
                               TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    /* Retrieve setting value from params */
    val = params[0].value.a;
    DMSG("Set GPIO pin value to %d\r\n", val);
    return csi_gpio_set_value(gpio_handle, val);
}

static TEE_Result ta_gpio_toggle_value(void *sess_ctx)
{
    void *gpio_handle = sess_ctx;
    DMSG("Toggle GPIO value\r\n");
    return csi_gpio_toggle_value(gpio_handle);
}

static TEE_Result ta_gpio_get_value(void *sess_ctx, uint32_t param_types,
                                    TEE_Param params[4])
{
    void *gpio_handle = sess_ctx;
    int *val = NULL;
    TEE_Result res = TEE_SUCCESS;
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_OUTPUT,
                               TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    /* Retrieve setting value from params */
    val = params[0].memref.buffer;
    res = csi_gpio_get_value(gpio_handle, val);

    if (res != TEE_SUCCESS) {
        return res;
    }

    DMSG("GPIO pin value is: %d", *val);
    return res;
}

static TEE_Result ta_gpio_set_direction(void *sess_ctx, uint32_t param_types,
                                        TEE_Param params[4])
{
    void *gpio_handle = sess_ctx;
    int val = 0;
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
                               TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    /* Retrieve setting value from params */
    val = params[0].value.a;
    DMSG("Set GPIO direction to %s\r\n",
         val == LIGHT_GPIO_DIR_OUT ? "OUTPUT" : "INPUT");
    return csi_gpio_set_direction(gpio_handle, val);
}

static TEE_Result ta_gpio_get_direction(void *sess_ctx, uint32_t param_types,
                                        TEE_Param params[4])
{
    void *gpio_handle = sess_ctx;
    int *val = NULL;
    TEE_Result res = TEE_SUCCESS;
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_OUTPUT,
                               TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE,
                               TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        return TEE_ERROR_BAD_PARAMETERS;
    }

    /* Retrieve setting value from params */
    val = params[0].memref.buffer;
    res = csi_gpio_get_direction(gpio_handle, val);

    if (res != TEE_SUCCESS) {
        return res;
    }

    DMSG("GPIO pin direction is: %s",
         *val == LIGHT_GPIO_DIR_OUT ? "OUTPUT" : "INPUT");
    return res;
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
    case TA_GPIO_REQUEST:
        return ta_gpio_request(sess_ctx, param_types, params);

    case TA_GPIO_SET_VALUE:
        return ta_gpio_set_value(sess_ctx, param_types, params);

    case TA_GPIO_TOGGLE_VALUE:
        return ta_gpio_toggle_value(sess_ctx);

    case TA_GPIO_GET_VALUE:
        return ta_gpio_get_value(sess_ctx, param_types, params);

    case TA_GPIO_SET_DIRECTION:
        return ta_gpio_set_direction(sess_ctx, param_types, params);

    case TA_GPIO_GET_DIRECTION:
        return ta_gpio_get_direction(sess_ctx, param_types, params);

    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
