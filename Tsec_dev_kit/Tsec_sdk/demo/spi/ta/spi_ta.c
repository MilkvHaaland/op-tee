#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <util.h>
#include <spi_ta.h>
#include <spi_hal.h>

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
    TEE_Result res;
    void *spi_handle;
    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;


    res = csi_spi_open(&spi_handle);
    /* Unused parameters */
    (void)&params;
    *sess_ctx = (void *)spi_handle;

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

static TEE_Result ta_spi_init(void *sess_ctx, uint32_t param_types,
                                TEE_Param params[4])
{
    void *spi_handle = sess_ctx;

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

    return csi_spi_init(spi_handle, cfg, idx);
}

static TEE_Result ta_spi_uninit(void *sess_ctx)
{
    void *spi_handle = sess_ctx;

    return csi_spi_uninit(spi_handle);
}

static TEE_Result ta_spi_send_then_receive(void *sess_ctx, uint32_t param_types,
                                TEE_Param params[4])
{
    void *spi_handle = sess_ctx;
    void *tx_buf;
    void *rx_buf;
    size_t tx_size;
    size_t rx_size;

    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                           TEE_PARAM_TYPE_MEMREF_OUTPUT,
                           TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types) {
        DMSG("param types:0x%x is not expect:0x%x\n", param_types, exp_param_types);
        return TEE_ERROR_BAD_PARAMETERS;
    }

    tx_buf = params[0].memref.buffer;
    tx_size = params[0].memref.size;

    rx_buf = params[1].memref.buffer;
    rx_size = params[1].memref.size;

    return csi_spi_send_then_receive(spi_handle, tx_buf, tx_size, rx_buf, rx_size);
}

static TEE_Result ta_spi_send_bytes (void *sess_ctx, uint32_t param_types,
                                TEE_Param params[4])
{
    void *spi_handle = sess_ctx;
    void *tx_buf;
    size_t tx_size;

    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                           TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

    tx_buf = params[0].memref.buffer;
    tx_size = params[0].memref.size;

    return csi_spi_send(spi_handle, tx_buf, tx_size);
}

static TEE_Result ta_spi_receive_bytes (void *sess_ctx, uint32_t param_types,
                                TEE_Param params[4])
{
    void *spi_handle = sess_ctx;
    void *rx_buf;
    size_t rx_size;

    uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_OUTPUT,
                           TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE,
                           TEE_PARAM_TYPE_NONE);

    if (param_types != exp_param_types)
        return TEE_ERROR_BAD_PARAMETERS;

    rx_buf = params[0].memref.buffer;
    rx_size = params[0].memref.size;

    return csi_spi_receive(spi_handle, rx_buf, rx_size);
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
    case TA_SPI_INIT:
        return ta_spi_init(sess_ctx, param_types, params);
    case TA_SPI_UNINIT:
        return ta_spi_uninit(sess_ctx);
    case TA_SPI_SEND_THEN_RECEIVE:
        return ta_spi_send_then_receive(sess_ctx, param_types, params);
    case TA_SPI_CMD_SEND_BYTES:
        return ta_spi_send_bytes(sess_ctx, param_types, params);
    case TA_SPI_CMD_RECEIVE_BYTES:
        return ta_spi_receive_bytes(sess_ctx, param_types, params);
    default:
        return TEE_ERROR_BAD_PARAMETERS;
    }
}
