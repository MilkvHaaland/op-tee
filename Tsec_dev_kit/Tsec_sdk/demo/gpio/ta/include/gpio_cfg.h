/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2023, Alibaba Group Holding Limited
 */

#ifndef _GPIO_CFG_H_
#define _GPIO_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

enum gpio_idx {
    GPIO_0,
    GPIO_1,
    GPIO_2,
    GPIO_3
};

enum light_gpio_dir {
    LIGHT_GPIO_DIR_IN,
    LIGHT_GPIO_DIR_OUT
};

struct csi_gpio_config {
    unsigned        dev_idx;    /* GPIO device index, range from 0 to 3 */
    int             bank;
    unsigned        offset;
};

#ifdef __cplusplus
}
#endif

#endif /* _GPIO_CFG_H_ */