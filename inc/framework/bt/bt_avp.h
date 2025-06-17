/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _BT_AVP_H_
#define _BT_AVP_H_


#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum t_bt_avp_event
{
    BT_AVP_EVENT_CONN_CMPL       = 0x00,
    BT_AVP_EVENT_DISCONN_CMPL    = 0x01,
    BT_AVP_EVENT_DATA_IND        = 0x02,
} T_BT_AVP_EVENT;

typedef struct t_bt_avp_event_conn_cmpl
{
    uint8_t bd_addr[6];
} T_BT_AVP_EVENT_CONN_CMPL;

typedef struct t_bt_avp_event_disconn_cmpl
{
    uint8_t bd_addr[6];
} T_BT_AVP_EVENT_DISCONN_CMPL;

typedef struct t_bt_avp_event_data_ind
{
    uint8_t    bd_addr[6];
    uint8_t   *data;
    uint16_t   len;
} T_BT_AVP_EVENT_DATA_IND;

typedef union t_bt_avp_event_param
{
    T_BT_AVP_EVENT_CONN_CMPL       conn_cmpl;
    T_BT_AVP_EVENT_DISCONN_CMPL    disconn_cmpl;
    T_BT_AVP_EVENT_DATA_IND        data_ind;
} T_BT_AVP_EVENT_PARAM;

typedef void (* P_BT_AVP_CBACK)(T_BT_AVP_EVENT  event_type,
                                void           *event_buf,
                                uint16_t        buf_len);

bool bt_avp_init(P_BT_AVP_CBACK cback);

void bt_avp_deinit(void);

bool bt_avp_connect_req(uint8_t bd_addr[6]);

bool bt_avp_disconnect_req(uint8_t bd_addr[6]);

bool bt_avp_data_send(uint8_t   bd_addr[6],
                      uint8_t  *data,
                      uint16_t  data_len,
                      bool      flush);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
