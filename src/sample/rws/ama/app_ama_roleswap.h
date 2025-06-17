/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef __AMA_ROLESWAP_H__
#define __AMA_ROLESWAP_H__


#include "ama_service_api.h"


#include "app_ama_transport.h"
#include "iap_stream.h"
#include "spp_stream.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup APP_RWS_AMA App Ama
  * @brief
  * @{
  */


/**
 * \defgroup    AMA_ROLESWAP       AMA roleswap
 *
 * \brief   Provide AMA roleswap related functions.
 *
 */


void app_ama_roleswap_init(void);

void app_ama_roleswap_handle_engage_role_decided(void);

void app_ama_roleswap_handle_role_swap_success(T_REMOTE_SESSION_ROLE device_role);

void app_ama_roleswap_handle_role_swap_fail(T_REMOTE_SESSION_ROLE device_role);

void app_ama_roleswap_handle_remote_conn_cmpl(void);

void app_ama_roleswap_handle_remote_disconn_cmpl(void);
/** @} End of APP_RWS_AMA */

#ifdef __cplusplus
}
#endif    /*  __cplusplus */
#endif    /*  __AMA_ROLESWAP_H__*/

