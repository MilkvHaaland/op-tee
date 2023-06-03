/*
 * Copyright (C) 2019-2022 Alibaba Group Holding Limited
 */
#ifndef __PTA_MISC_H
#define __PTA_MISC_H

//#include <util.h>

enum tee_plat_lc_e {
    TEE_PLAT_LC_INIT = 0,
	TEE_PLAT_LC_DEV,
	TEE_PLAT_LC_OEM,
	TEE_PLAT_LC_PRO,
	TEE_PLAT_LC_RMA,
	TEE_PLAT_LC_MAX,
};

/*
 * Interface to the pseudo TA, which is provides misc. auxiliary services,
 * extending existing GlobalPlatform Core API
 */

#define TA_MISC_UUID	\
        { 0x33fbd23e, 0x5cbf, 0x4e87, \
                { 0x99, 0x8c, 0xf4, 0x5c, 0x11, 0x7f, 0x24, 0x4a} }
/*
 * Update life cycle
 *
 * [in]	    value[0].a: Life cycle which is need to be update
 * [in]	    value[0].b: Must be 0
 */
#define PTA_MISC_UPDATE_LC	1

/*
 * Get life cycle
 *
 * [out]    value[0].a: Address upper 32-bits
 * [out]    value[0].b: Address lower 32-bits
 */
#define PTA_MISC_GET_LC		2

/*
 * Read back life cycle which is just set. In some platform, life cycle would be
 * effected atfer system reboot, and this can be used to check whether life cycle is
 * set correctly before system reboot
 *
 * [in]	    value[0].a: life
 * [in]	    value[0].b: Must be 0
 */
#define PTA_MISC_READBACK_LC 	3

/*
 * Write bytes to efuse 
 *
 * [in]	    value[0].a: offset
 * [in]	    value[0].b: count
 * [out]	memref[1].buffer: buffer to store efuse
 * [out]	memref[1].size: size of buffer to store efuse
 */
#define PTA_MISC_EFUSE_READ	    4

/*
 * Write bytes to efuse 
 *
 * [in]	    value[0].a: offset
 * [in]	    value[0].b: count
 * [in]	memref[1].buffer: buffer to store efuse
 * [in]	memref[1].size: size of buffer to store efuse
 */
#define PTA_MISC_EFUSE_WRITE	    5

#endif /* __PTA_MISC_H */
