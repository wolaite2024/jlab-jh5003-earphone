/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     ots_coc.h
  * @brief    Head file for Object transfer Service.
  * @details  mcs data structs and external functions declaration.
  * @author   cheng_cai
  * @date
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _OTS_COC_H_
#define _OTS_COC_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "os_queue.h"
#include "gap_credit_based_conn.h"
#include "bt_gatt_svc.h"


#if (LE_AUDIO_MCP_CLIENT_SUPPORT | LE_AUDIO_MCP_SERVER_SUPPORT)

#define OTS_LE_COC_CHAN_MAX_NUM           2
#define OTS_COC_CREDIT_NUM                6
#define OTS_COC_MTU_SIZE                  521

typedef T_APP_RESULT(*P_FUN_LE_COC_OTS_CB)(uint8_t coc_type, void *p_coc_data, uint8_t id);

typedef struct t_coc_link_cb
{
    struct t_coc_link_cb     *p_next;
    uint8_t                   conn_id;
    uint8_t                   id;
    bool                      is_serv;
    uint16_t                  cid;

    P_FUN_LE_COC_OTS_CB       app_callback;
} T_COC_LINK_CB;


typedef struct
{
    bool              coc_connecting;
    T_OS_QUEUE        coc_queue;
} T_COC_CB;


bool le_coc_register_cback(uint8_t conn_id, P_FUN_LE_COC_OTS_CB app_callback, uint8_t id,
                           bool is_server);
void le_coc_disconnect(int16_t cid);
void le_coc_enable(void);
#endif
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _OTS_COC_H_ */
