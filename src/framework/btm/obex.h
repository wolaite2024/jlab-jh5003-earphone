/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _OBEX_H_
#define _OBEX_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define SRM_DISABLE             0x00
#define SRM_WAIT                0x01
#define SRM_ENABLE              0x02

/***********obex header id definition**********/
#define OBEX_HI_COUNT               0xC0
#define OBEX_HI_NAME                0x01
#define OBEX_HI_TYPE                0x42
#define OBEX_HI_LENGTH              0xC3
#define OBEX_HI_TIME1               0x44
#define OBEX_HI_TIME2               0xC4
#define OBEX_HI_DESCRIPTION         0x05
#define OBEX_HI_TARGET              0x46
#define OBEX_HI_HTTP                0x47
#define OBEX_HI_BODY                0x48
#define OBEX_HI_END_BODY            0x49
#define OBEX_HI_WHO                 0x4A
#define OBEX_HI_CONNECTION_ID       0xCB
#define OBEX_HI_APP_PARAMETER       0x4C
#define OBEX_HI_AUTHEN_CHALLENGE    0x4D
#define OBEX_HI_AUTHEN_RSP          0x4E
#define OBEX_HI_OBJECT_CLASS        0x4F
#define OBEX_HI_SSN                 0x93
#define OBEX_HI_SRM                 0x97
#define OBEX_HI_SRMP                0x98

#define OBEX_HDR_PUT_LEN                0x03
#define OBEX_HDR_CONNECT_LEN            0x07
#define OBEX_HDR_PREFIXED_LEN_BASE      0x03
#define OBEX_HDR_CONN_ID_LEN            0x05
#define OBEX_HDR_AUTHEN_CHALLENGE_LEN   0x18
#define OBEX_HDR_AUTHEN_RSP_LEN         0x15
#define OBEX_HDR_SRM_LEN                0x02
#define OBEX_HDR_ABORT_LEN              0x03
#define OBEX_HDR_NAME_LEN               0x03
#define OBEX_HDR_BODY_LEN               0x03
#define OBEX_HDR_TYPE_LEN               0x03
#define OBEX_HDR_LENGTH_LEN             0x05

/*********response code definition*********/
/* For OBEX version 1.0 final bit is always set */
#define OBEX_RSP_CONTINUE               0x90
#define OBEX_RSP_SUCCESS                0xA0
#define OBEX_RSP_ACCEPT                 0xA2
#define OBEX_RSP_BAD_REQUEST            0xC0
#define OBEX_RSP_UNAUTHORIZED           0xC1
#define OBEX_RSP_FORBIDDEN              0xC3
#define OBEX_RSP_NOT_FOUND              0xC4
#define OBEX_RSP_NOT_ACCEPTABLE         0xC6
#define OBEX_RSP_PRECONDITION_FAIL      0xCC
#define OBEX_RSP_NOT_IMPLEMENT          0xD1
#define OBEX_RSP_SERVICE_UNAVAILABLE    0xD3

/*********get object status definition*********/
#define OBEX_GET_OBJECT_BODY            0x00
#define OBEX_GET_OBJECT_END_BODY        0x01
#define OBEX_GET_OBJECT_APP_PARA        0x02
#define OBEX_GET_OBJECT_FAIL            0x03

typedef void(*P_OBEX_CBACK)(uint8_t msg_type, void *p_msg);

typedef void *T_OBEX_HANDLE;

typedef enum t_obex_msg
{
    OBEX_MSG_CONN_IND      = 0x00,
    OBEX_MSG_CONN_CMPL     = 0x01,
    OBEX_MSG_DISCONN       = 0x02,
    OBEX_MSG_SET_PATH_DONE = 0x03,
    OBEX_MSG_PUT_DONE      = 0x04,
    OBEX_MSG_PUT_DATA_RSP  = 0x05,
    OBEX_MSG_GET_OBJECT    = 0x06,
    OBEX_MSG_REMOTE_PUT    = 0x07,
    OBEX_MSG_ABORT_DONE    = 0x08,
} T_OBEX_MSG;

typedef enum t_obex_role
{
    OBEX_ROLE_CLIENT = 0x00,
    OBEX_ROLE_SERVER = 0x01,
} T_OBEX_ROLE;

typedef enum t_obex_state
{
    OBEX_STATE_DISCONNECTED    = 0x00,
    OBEX_STATE_RFC_CONNECTED   = 0x01,
    OBEX_STATE_L2C_CONNECTED   = 0x02,
    OBEX_STATE_OBEX_CONNECTING = 0x03,
    OBEX_STATE_CONNECTED       = 0x04,
    OBEX_STATE_IDLE            = 0x05,
    OBEX_STATE_DISCONNECT      = 0x06,
    OBEX_STATE_GET             = 0x07,
    OBEX_STATE_PUT             = 0x08,
    OBEX_STATE_SET_PATH        = 0x09,
    OBEX_STATE_ABORT           = 0x0a,
} T_OBEX_STATE;

typedef struct t_obex_conn_rsp_data
{
    T_OBEX_HANDLE     handle;
    uint8_t           bd_addr[6];
} T_OBEX_CONN_RSP_DATA;

typedef struct t_obex_conn_ind_data
{
    T_OBEX_HANDLE     handle;
    uint8_t           bd_addr[6];
} T_OBEX_CONN_IND_DATA;

typedef struct t_obex_conn_cmpl_data
{
    T_OBEX_HANDLE     handle;
    uint8_t           bd_addr[6];
    uint16_t          max_pkt_len;
    bool              obex_over_l2c;
} T_OBEX_CONN_CMPL_DATA;

typedef struct t_obex_disconn_cmpl_data
{
    T_OBEX_HANDLE     handle;
    uint8_t           bd_addr[6];
    uint16_t          cause;
} T_OBEX_DISCONN_CMPL_DATA;

typedef struct t_obex_set_path_cmpl_data
{
    T_OBEX_HANDLE     handle;
    uint8_t           bd_addr[6];
    uint8_t           cause;
} T_OBEX_SET_PATH_CMPL_DATA;

typedef struct t_obex_put_data_rsp
{
    T_OBEX_HANDLE     handle;
    uint8_t           bd_addr[6];
} T_OBEX_PUT_DATA_RSP;

typedef struct t_obex_put_cmpl_data
{
    T_OBEX_HANDLE     handle;
    uint8_t           bd_addr[6];
    uint8_t           cause;
    uint8_t          *p_name;
    uint16_t          name_len;
    uint8_t           srm_status;
} T_OBEX_PUT_CMPL_DATA;

typedef struct t_obex_get_object_cmpl_data
{
    T_OBEX_HANDLE     handle;
    uint8_t           bd_addr[6];
    uint8_t          *p_data;
    uint16_t          data_len;
    uint8_t           rsp_code;
    uint8_t           srm_status;
} T_OBEX_GET_OBJECT_CMPL_DATA;

typedef struct t_obex_remote_put_data
{
    T_OBEX_HANDLE     handle;
    uint8_t           bd_addr[6];
    uint8_t          *p_data;
    uint16_t          data_len;
    uint8_t           srm_status;
} T_OBEX_REMOTE_PUT_DATA;

typedef struct t_obex_abort_cmpl_data
{
    T_OBEX_HANDLE     handle;
    uint8_t           bd_addr[6];
    uint8_t           cause;
} T_OBEX_ABORT_CMPL_DATA;

typedef struct t_obex_roleswap_info
{
    uint32_t            conn_id;
    uint16_t            cid;
    uint16_t            local_max_pkt_len;
    uint16_t            remote_max_pkt_len;
    T_OBEX_ROLE         role;
    T_OBEX_STATE        state;
    uint16_t            psm;
    uint16_t            data_offset;
    uint8_t             rfc_dlci;
} T_OBEX_ROLESWAP_INFO;

bool obex_register_over_rfc(uint8_t      chann_num,
                            P_OBEX_CBACK callback);

bool obex_unregister_over_rfc(P_OBEX_CBACK callback);

bool obex_register_over_l2c(uint16_t     psm,
                            P_OBEX_CBACK callback);

bool obex_unregister_over_l2c(P_OBEX_CBACK callback);

bool obex_conn_req_over_rfc(uint8_t        bd_addr[6],
                            uint8_t       *p_param,
                            uint8_t        server_chann,
                            P_OBEX_CBACK   cback,
                            T_OBEX_HANDLE *p_handle);

bool obex_conn_req_over_l2c(uint8_t        bd_addr[6],
                            uint8_t       *p_param,
                            uint16_t       l2c_psm,
                            P_OBEX_CBACK   cback,
                            T_OBEX_HANDLE *p_handle);

bool obex_conn_cfm(T_OBEX_HANDLE  handle,
                   bool           accept,
                   uint8_t       *p_cfm_data,
                   uint16_t       data_len);

bool obex_disconn_req(T_OBEX_HANDLE handle);

bool obex_set_path(T_OBEX_HANDLE  handle,
                   uint8_t        flag,
                   const uint8_t *path_ptr,
                   uint16_t       path_len);

bool obex_put_data(T_OBEX_HANDLE  handle,
                   const uint8_t *header_ptr,
                   uint16_t       header_len,
                   bool           more_data,
                   uint8_t       *body_ptr,
                   uint16_t       body_len,
                   bool           ack);

bool obex_get_object(T_OBEX_HANDLE  handle,
                     uint8_t       *content_ptr,
                     uint16_t       content_len);

bool obex_send_rsp(T_OBEX_HANDLE handle,
                   uint8_t       rsp_code);

bool obex_get_object_continue(T_OBEX_HANDLE handle);

bool obex_abort(T_OBEX_HANDLE handle);

bool obex_find_hdr_in_pkt(uint8_t   *p_pkt,
                          uint16_t   pkt_len,
                          uint8_t    tgt_hdr,
                          uint8_t  **pp_tgt,
                          uint16_t  *p_tgt_len);

bool obex_find_value_in_app_param(uint8_t   *p_param,
                                  uint16_t   param_len,
                                  uint8_t    tag,
                                  uint8_t  **pp_value,
                                  uint16_t  *p_value_len);

bool obex_roleswap_info_get(T_OBEX_HANDLE         handle,
                            T_OBEX_ROLESWAP_INFO *p_info);

bool obex_roleswap_info_set(uint8_t               bd_addr[6],
                            P_OBEX_CBACK          cback,
                            T_OBEX_ROLESWAP_INFO *p_info,
                            T_OBEX_HANDLE        *p_handle);

bool obex_roleswap_info_del(T_OBEX_HANDLE handle);

uint8_t obex_service_id_get(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _OBEX_H_ */
