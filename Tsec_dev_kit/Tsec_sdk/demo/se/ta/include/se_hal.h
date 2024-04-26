/*
 * Copyright (c) 2023, Alibaba Group Holding Limited
 */

#ifndef _SE_HAL_H_
#define _SE_HAL_H_

#ifdef __cplusplus
extern "C"{
#endif
#include <tee_api_types.h>
#include "spi_cfg.h"
#include "scp_cfg.h"

TEE_Result csi_se_session_open(void **se_handle);

TEE_Result csi_se_init(void *se_handle, struct csi_spi_config *config, uint32_t idx);

TEE_Result csi_se_uninit(void *se_handle);

TEE_Result csi_se_chip_atr(void *se_handle);

TEE_Result csi_se_chip_channel_open(void *se_handle);

TEE_Result csi_se_chip_channel_close(void *se_handle);

TEE_Result csi_se_chip_channel_transmit(void *se_handle, const void *txbuf, size_t tx_size, void *rxbuf, size_t *rx_size);

TEE_Result csi_se_chip_power_down(void *se_handle);

TEE_Result csi_se_chip_reset(void *se_handle);

TEE_Result csi_se_chip_mutual_auth(void *se_handle, struct csi_scp_config *scp_config);
TEE_Result csi_se_chip_get_status(void *se_handle, struct csi_status_config *get_status_config);

#ifdef __cplusplus
}
#endif

#endif /* _SE_HAL_H_ */
