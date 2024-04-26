#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <util.h>
#include <se_ta.h>
#include <se_hal.h>
#include <scp_cfg.h>
/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void)
{
    DMSG("has been called");

    return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
    DMSG("has been called");
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
    void *se_handle;
    TEE_Result res;
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;


    res = csi_se_session_open(&se_handle);
    if (res != TEE_SUCCESS) {
        return res;
    }
    /* Unused parameters */
    (void)&params;
    *sess_ctx = (void *)se_handle;

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

static TEE_Result ta_se_init(void *sess_ctx, uint32_t param_types,
                                TEE_Param params[4])
{
    void *se_handle = sess_ctx;

    struct csi_spi_config *cfg;
    uint32_t idx;
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                           TEE_PARAM_TYPE_VALUE_INPUT,
                           TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        DMSG("param types:0x%x is not expect:0x%x\n", param_types, exp_param_types);
        return TEE_ERROR_BAD_PARAMETERS;
    }

    cfg = (struct csi_spi_config *)params[0].memref.buffer;
    idx = params[1].value.a;

    DMSG("format:%d mode:%d speed:%d timeout:%d \n", cfg->format, cfg->mode, cfg->speed, cfg->timeout);

    return csi_se_init(se_handle, cfg, idx);
}

static TEE_Result ta_se_atr(void *sess_ctx)
{
    void *se_handle = sess_ctx;

    return csi_se_chip_atr(se_handle);
}

static TEE_Result ta_se_uninit(void *sess_ctx)
{
    void *se_handle = sess_ctx;

    return csi_se_uninit(se_handle);
}

static TEE_Result ta_se_chip_channel_open(void *sess_ctx)
{
    void *se_handle = sess_ctx;

    return csi_se_chip_channel_open(se_handle);
}

static TEE_Result ta_se_chip_channel_close(void *sess_ctx)
{
    void *se_handle = sess_ctx;

    return csi_se_chip_channel_close(se_handle);
}

static TEE_Result ta_se_chip_reset(void *sess_ctx)
{
    void *se_handle = sess_ctx;

    return csi_se_chip_reset(se_handle);
}

static TEE_Result ta_se_chip_channel_transmit(void *sess_ctx, uint32_t param_types,
                                TEE_Param params[4])
{
    void *se_handle = sess_ctx;
    void *tx_buf;
    void *rx_buf;
    void *rx_len_buf;
    size_t tx_size;
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                           TEE_PARAM_TYPE_MEMREF_OUTPUT,
                           TEE_PARAM_TYPE_MEMREF_OUTPUT,
                           TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        DMSG("param types:0x%x is not expect:0x%x\n", param_types, exp_param_types);
        return TEE_ERROR_BAD_PARAMETERS;
    }

    tx_buf = params[0].memref.buffer;
    tx_size = params[0].memref.size;
    rx_buf = params[1].memref.buffer;
    rx_len_buf = params[2].memref.buffer;

    return csi_se_chip_channel_transmit(se_handle, tx_buf, tx_size, rx_buf, (size_t *)rx_len_buf);
}

static TEE_Result ta_se_chip_mutual_auth(void *sess_ctx, uint32_t param_types,
                                TEE_Param params[4])
{
    void *se_handle = sess_ctx;
    struct csi_scp_config *cfg;

    DMSG("\n");
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                           TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        DMSG("param types:0x%x is not expect:0x%x\n", param_types, exp_param_types);
        return TEE_ERROR_BAD_PARAMETERS;
    }

    cfg = params[0].memref.buffer;

    return csi_se_chip_mutual_auth(se_handle, cfg);
}

static TEE_Result ta_se_chip_get_status(void *sess_ctx, uint32_t param_types,
                                TEE_Param params[4])
{
    void *se_handle = sess_ctx;
    struct csi_status_config *cfg;

    DMSG("\n");
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                           TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        DMSG("param types:0x%x is not expect:0x%x\n", param_types, exp_param_types);
        return TEE_ERROR_BAD_PARAMETERS;
    }

    cfg = params[0].memref.buffer;
    return csi_se_chip_get_status(se_handle, cfg);
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
    case TA_SE_INIT:
        return ta_se_init(sess_ctx, param_types, params);
    case TA_SE_UNINIT:
        return ta_se_uninit(sess_ctx);
    case TA_SE_ATR:
        return ta_se_atr(sess_ctx);
    case TA_SE_CHANNEL_OPEN:
        return ta_se_chip_channel_open(sess_ctx);
    case TA_SE_CHANNEL_CLOSE:
        return ta_se_chip_channel_close(sess_ctx);
    case TA_SE_CHANNEL_TRANSMIT:
        return ta_se_chip_channel_transmit(sess_ctx, param_types, params);
    case TA_SE_RESET:
        return ta_se_chip_reset(sess_ctx);
    case TA_SE_SCP_MUTUAL_AUTH:
        return ta_se_chip_mutual_auth(sess_ctx, param_types, params);
    case TA_SE_SCP_GET_STATUS:
        return ta_se_chip_get_status(sess_ctx, param_types, params);
    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
