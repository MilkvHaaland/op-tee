/*
 * Copyright (c) 2023, Alibaba Group Holding Limited
 */

#ifndef _SPI_HAL_H_
#define _SPI_HAL_H_

#ifdef __cplusplus
extern "C"{
#endif
#include <tee_api_types.h>
#include "spi_cfg.h"

TEE_Result csi_spi_open(void **spi_handle);

TEE_Result csi_spi_init(void *spi_handle, struct csi_spi_config *config, uint32_t idx);

TEE_Result csi_spi_uninit(void *spi_handle);

TEE_Result csi_spi_send_then_receive(void *spi_handle, const void *txbuf, size_t tx_size, void *rxbuf, size_t rx_size);

TEE_Result csi_spi_send(void *spi_handle, const void *txbuf, size_t size);

TEE_Result csi_spi_receive(void *spi_handle, void *rxbuf, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* _SPI_HAL_H_ */
