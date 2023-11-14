/*
 * Copyright (c) 2023, Alibaba Group Holding Limited
 */

#ifndef _I2C_HAL_H_
#define _I2C_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <tee_api_types.h>
#include "i2c_cfg.h"

TEE_Result csi_i2c_open(void **i2c_phandle);
TEE_Result csi_i2c_init(void *i2c_handle, struct csi_i2c_config *config, uint32_t idx);
TEE_Result csi_i2c_uninit(void *i2c_handle);
TEE_Result csi_i2c_master_send(void *i2c_handle, uint8_t *tx_buf, struct csi_i2c_msg *message);
TEE_Result csi_i2c_master_receive(void *i2c_handle, uint8_t *rx_buf, struct csi_i2c_msg *message);

#ifdef __cplusplus
}
#endif

#endif /* _I2C_HAL_H_ */