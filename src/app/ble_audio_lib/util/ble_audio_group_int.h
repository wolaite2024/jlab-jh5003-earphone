#ifndef _BLE_AUDIO_GROUP_INT_H_
#define _BLE_AUDIO_GROUP_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "gap_conn_le.h"
#include "csis_def.h"
#include "os_queue.h"
#include "ble_audio_group.h"
#include "ble_link_util.h"
#include "csis_client.h"

#if LE_AUDIO_GROUP_SUPPORT

typedef struct t_le_audio_dev
{
    struct t_le_audio_dev    *p_next;
    bool                      is_used;
    T_GAP_REMOTE_ADDR_TYPE    addr_type;
    uint8_t                   bd_addr[6];
    bool                      is_rpa;
    uint8_t                   identity_addr_type;
    uint8_t                   identity_addr[6];
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
    uint8_t                   srv_instance_id;
    uint8_t                   char_exit;
    bool                      lock_obtained;
    T_CSIS_LOCK               lock;
    uint8_t                   rank;
#endif
    T_BLE_AUDIO_LINK         *p_link;
} T_LE_AUDIO_DEV;

#if LE_AUDIO_CSIS_CLIENT_SUPPORT
typedef struct
{
    bool                     rank_sorted;
    uint8_t                  char_exit;
    T_SET_COORDINATOR_LOCK   lock_state;
    bool                     lock_req;
    uint8_t                  size;
    T_CSIS_SIRK_TYPE         sirk_type;
    uint8_t                  sirk[CSIS_SIRK_LEN];
    uint16_t                 serv_uuid;
} T_CSIP_CB;
#endif

typedef struct t_le_audio_group
{
    struct t_le_audio_group *p_next;
    uint8_t                  conn_dev_num;
#if LE_AUDIO_ASCS_CLIENT_SUPPORT
    uint8_t                  session_num;
#endif
    P_FUN_AUDIO_GROUP_CB     p_group_cb;
    T_OS_QUEUE               dev_queue;
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
    T_CSIP_CB               *p_csip_cb;
#endif
#if LE_AUDIO_CAP_SUPPORT
    void                    *p_cap;
#endif
} T_LE_AUDIO_GROUP;

extern T_OS_QUEUE audio_group_queue;
void ble_audio_group_init(void);
void ble_audio_group_print(T_LE_AUDIO_GROUP *p_audio_group);
bool ble_audio_group_session(T_BLE_AUDIO_GROUP_HANDLE group_handle, bool is_add);
T_BLE_AUDIO_GROUP_HANDLE ble_audio_group_allocate_by_dev_num(uint8_t dev_num);
bool ble_audio_group_handle_check_int(const char *p_func_name,
                                      T_BLE_AUDIO_GROUP_HANDLE group_handle);
bool ble_audio_dev_handle_check_int(const char *p_func_name, T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                    T_BLE_AUDIO_DEV_HANDLE dev_handle);
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
T_CSIP_CB *set_coordinator_get_cb_int(const char *p_func_name,
                                      T_LE_AUDIO_GROUP *p_coor_set);
#define set_coordinator_get_cb(p_coor_set) set_coordinator_get_cb_int(__func__, p_coor_set)
#endif
#define ble_audio_group_handle_check(group_handle) ble_audio_group_handle_check_int(__func__, group_handle)
#define ble_audio_dev_handle_check(group_handle, dev_handle) ble_audio_dev_handle_check_int(__func__, group_handle, dev_handle)

void ble_audio_group_send_msg(T_AUDIO_GROUP_MSG msg, T_BLE_AUDIO_GROUP_HANDLE group_handle,
                              uint8_t *p_data, uint8_t data_len, bool direct);
#endif
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
