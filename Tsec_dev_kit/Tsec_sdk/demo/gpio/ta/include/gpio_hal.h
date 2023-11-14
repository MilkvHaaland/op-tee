/*
 * Copyright (c) 2023, Alibaba Group Holding Limited
 */

#ifndef _GPIO_HAL_H_
#define _GPIO_HAL_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <tee_api_types.h>
#include "gpio_cfg.h"

TEE_Result csi_gpio_init(void **gpio_phandle);
TEE_Result csi_gpio_uninit(void *gpio_handle);
TEE_Result csi_gpio_request(void *gpio_handle, struct csi_gpio_config *config);
TEE_Result csi_gpio_set_value(void *gpio_handle, int val);
TEE_Result csi_gpio_toggle_value(void *gpio_handle);
TEE_Result csi_gpio_get_value(void *gpio_handle, int *val);
TEE_Result csi_gpio_set_direction(void *gpio_handle, int val);
TEE_Result csi_gpio_get_direction(void *gpio_handle, int *val);

#ifdef __cplusplus
}
#endif

#endif /* _GPIO_HAL_H_ */
