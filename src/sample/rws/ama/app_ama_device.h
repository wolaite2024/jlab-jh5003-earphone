/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_AMA_DEVICE_H_
#define _APP_AMA_DEVICE_H_

#include "btm.h"
#include "common_stream.h"
#include "app_ama.h"
#include "app_ama_transport.h"
#include "ama_stream_process.h"
#include "central.pb.h"


#if AMA_FEATURE_SUPPORT
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @defgroup APP_RWS_AMA App Ama
  * @brief App Ama
  * @{
  */
typedef enum
{
    ALEXA_VA_STATE_IDLE           = 0,
    ALEXA_VA_STATE_ACTIVE_PENDING = 1,
    ALEXA_VA_STATE_ACTIVE         = 2,
    ALEXA_VA_STATE_CANCEL_PENDING = 3,
} T_ALEXA_VA_STATE;


typedef struct APP_AMA_DEVICE
{
    uint8_t             bd_addr[6];
    bool                bt_connected[COMMON_STREAM_BT_TYPE_MAX];
    uint16_t            alexa_eap_session_id;
    T_ALEXA_VA_STATE    alexa_va_state;
    uint8_t             alexa_device_network_conn_status;
    Platform            alexa_device_os_platform;
    uint32_t            alexa_speech_dialog_id;
    uint8_t             alexa_eap_session_status;
    AMA_TRANSPORT       *p_transport;
    AMA_STREAM_PROC     *p_proc;
} APP_AMA_DEVICE;


void app_ama_device_init(void);


APP_AMA_DEVICE *app_ama_device_create(COMMON_STREAM_BT_TYPE bt_type, uint8_t *bd_addr);
bool app_ama_device_delete(COMMON_STREAM_BT_TYPE bt_type, uint8_t *bd_addr);
void app_ama_device_handle_b2s_connected(uint8_t *bd_addr);
void app_ama_device_handle_b2s_disconnected(uint8_t *bd_addr);



void app_ama_device_set_network_conn_status(uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH], uint8_t status);
void app_ama_device_set_os_platform(uint8_t *bd_addr, Platform os_platform);
void app_ama_device_set_va_state(uint8_t *bd_addr, T_ALEXA_VA_STATE state);
uint32_t app_ama_device_get_mtu(uint8_t *bd_addr);
void app_ama_device_va_start(uint8_t *bd_addr);

void app_ama_device_tx(uint8_t *bd_addr, uint8_t *data, uint16_t length);
uint32_t app_ama_device_get_connected_device_num(void);

void app_ama_device_send_all_info(void);

void app_ama_device_ble_disconnect_all(void);


int32_t app_ama_device_get_info_len(void);
void app_ama_device_get_info(uint8_t *buf);

int32_t app_ama_device_set_info(uint8_t *buf);

/** End of APP_RWS_AMA
* @}
*/
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
#endif
