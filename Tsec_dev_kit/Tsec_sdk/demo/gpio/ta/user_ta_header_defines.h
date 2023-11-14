/*
 * Copyright (c) 2023, Alibaba Group Holding Limited
 */

/*
 * The name of this file must not be modified
 */

#ifndef USER_TA_HEADER_DEFINES_H
#define USER_TA_HEADER_DEFINES_H

/* To get the TA UUID definition */
#include <gpio_ta.h>

#define TA_UUID				TA_GPIO_UUID

/*
 * TA properties: multi-instance TA, no specific attribute
 * TA_FLAG_EXEC_DDR is meaningless but mandated.
 */
#define TA_FLAGS			TA_FLAG_EXEC_DDR

/* Provisioned stack size */
#define TA_STACK_SIZE			(2 * 1024)

/* Provisioned heap size for TEE_Malloc() and friends */
#define TA_DATA_SIZE			(32 * 1024)

/* The gpd.ta.version property */
#define TA_VERSION	"1.0"

/* The gpd.ta.description property */
#define TA_DESCRIPTION	"Example of OP-TEE GPIO Trusted Application"

/* Extra properties */
#define TA_CURRENT_TA_EXT_PROPERTIES \
    { "org.alibaba.optee.examples.gpio.description", \
	USER_TA_PROP_TYPE_STRING, \
        "Example of TA using GPIO" }, \
    { "org.alibaba.optee.examples.gpio.version", \
	USER_TA_PROP_TYPE_U32, &(const uint32_t){ 0x0010 } }

#endif /* USER_TA_HEADER_DEFINES_H */
