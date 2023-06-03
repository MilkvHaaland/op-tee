/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2023, Alibaba Group Holding Limited
 */

#ifndef _FCE_HAL_H_
#define _FCE_HAL_H_

#ifdef __cplusplus
extern "C"{
#endif
#include <tee_api_types.h>
#include "fce_cfg.h"

enum FCE_EVENT {
    FCE_EVENT_COMPLETE = 0,
    FCE_EVENT_PAUSE,
    FCE_EVENT_BUSERR,
    FCE_EVENT_TIMEOUT,
    FCE_EVENT_UNDEFMSG,
    FCE_EVENT_SOCKERR,
    FCE_EVENT_OTHER
};

#define BUFFER_SIZE (4*1024)

typedef void *csi_fce_handle_t;
typedef void *csi_fce_sock_handle_t;
typedef void *csi_fce_libbuf_handle_t;

TEE_Result csi_fce_open(csi_fce_handle_t *fce_handle, const char *name);
TEE_Result csi_fce_close(csi_fce_handle_t fce_handle);
TEE_Result csi_fce_query_result_cnt(csi_fce_handle_t fce_handle, unsigned int *cnt);
TEE_Result csi_fce_set_result_cnt(csi_fce_handle_t fce_handle, unsigned int *cnt);
TEE_Result csi_fce_set_attr(csi_fce_handle_t fce_handle, struct fce_cfg *cfg);
TEE_Result csi_fce_get_attr(csi_fce_handle_t fce_handle, struct fce_cfg *cfg);
TEE_Result csi_fce_do_compare(csi_fce_handle_t fce_handle, struct target_vector *target_vect);
TEE_Result csi_fce_get_result(csi_fce_handle_t fce_handle, struct top_result *top_buf);
TEE_Result csi_fce_suspend(csi_fce_handle_t fce_handle);
TEE_Result csi_fce_resume(csi_fce_handle_t fce_handle);
TEE_Result csi_fce_stop(csi_fce_handle_t fce_handle);

csi_fce_libbuf_handle_t csi_fce_create_featurelib(csi_fce_handle_t fce_handle, unsigned int size);
TEE_Result csi_fce_release_featurelib(csi_fce_handle_t fce_handle, csi_fce_libbuf_handle_t handle);
TEE_Result csi_fce_set_featurelib(csi_fce_handle_t fce_handle, struct fce_lib_buf_info *info);

#ifdef __cplusplus
}
#endif

#endif // #ifndef _FCE_HAL_H_
