/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_XIAOWEI_H_
#define _APP_XIAOWEI_H_
#include "app_flags.h"
#if F_APP_XIAOWEI_FEATURE_SUPPORT
#include "stdint.h"
#include "stdbool.h"
#include "xiaowei_protocol.h"
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_RWS_XIAOWEI App Xiaowei
  * @brief App XIAOWEI
  * @{
  */
extern uint8_t app_exp_bit_rate;/*Expected bit rate of app*/

extern T_XIAOWEI_FVALUE_STORE app_xiaowei_fvalues[XIAOWEI_FVALUE_MAX_NUMBER];

typedef enum
{
    APP_REMOTE_MSG_XIAOWEI_FVALUE             = 0x00,
    APP_REMOTE_MSG_XIAOWEI_FVALUE_MAX_MSG_NUM = 0x01,
} T_APP_XIAOWEI_REMOTE_MSG;

/**
  * @brief  receive data from xiaowei app
  * @return void
  */
void app_xiaowei_receive_data(T_XIAOWEI_CONNECTION_TYPE connection_type, uint8_t *bd_addr,
                              uint8_t conn_id, uint8_t *p_buffer, uint16_t length);

/**
  * @brief  This function can be used to send voice data to xiaowei app
  * @return void
  */
void app_xiaowei_255_send_voice_data_request(T_XIAOWEI_CONNECTION_TYPE connection_type,
                                             uint8_t *bd_addr, uint8_t conn_id, uint8_t *p_buffer, uint16_t length);

/**
  * @brief  This function can be used to wakeup xiaowei app
  * @return void
  */
void app_xiaowei_101_send_wakeup_request(T_XIAOWEI_CONNECTION_TYPE connection_type,
                                         uint8_t *bd_addr,
                                         uint8_t conn_id);

/**
  * @brief  This function can be used to request xiaowei app stop recording
  * @return void
  */
void app_xiaowei_108_send_stop_record_request(T_XIAOWEI_CONNECTION_TYPE connection_type,
                                              uint8_t *bd_addr, uint8_t conn_id);

/**
  * @brief  This function can be used to clear fvalues when factory reset
  * @return void
  */
void app_xiaowei_handle_factory_reset(void);

/**
  * @brief  This function can be used to send fvalues to remote
  * @return true success
  */
bool app_xiaowei_remote_fvalue(T_XIAOWEI_FVALUE_STORE *fvalue, uint8_t length);

/**
  * @brief  This function can be used to init xiaowei protocol, xiaowei device,
  * xiaowei transport and xiaowei roleswap;
  * @return void
  */
void app_xiaowei_init(void);

/** End of APP_RWS_XIAOWEI
* @}
*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
#endif
