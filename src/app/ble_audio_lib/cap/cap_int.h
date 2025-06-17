#ifndef _CAP_INT_H_
#define _CAP_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "bt_gatt_svc.h"
#include "os_queue.h"
#include "gap_le_types.h"
#include "ble_audio.h"
#include "csis_def.h"
#include "ble_audio_group_int.h"

#if LE_AUDIO_CAP_SUPPORT
typedef struct
{
    bool new_group;
    T_BLE_AUDIO_GROUP_HANDLE group_handle;
    T_BLE_AUDIO_DEV_HANDLE dev_handle;
} T_CAP_SET_MEM_ADD;
#define LE_AUDIO_MSG_CAP_SET_MEM_ADD     (LE_AUDIO_MSG_GROUP_CAP | LE_AUDIO_MSG_INTERNAL | 0x00)

uint16_t cap_msg_cb(uint16_t msg, void *buf, bool *p_handled);
uint16_t bap_msg_cb(uint16_t msg, void *buf, bool *p_handled);
#define cap_check_group_state(p_group, csis_check) cap_check_group_state_int(__func__, p_group, csis_check)
T_LE_AUDIO_CAUSE cap_check_group_state_int(const char *p_func_name,
                                           T_BLE_AUDIO_GROUP_HANDLE group_handle, bool csis_check);
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
bool set_coordinator_init(void);
#endif
#if LE_AUDIO_CAS_SUPPORT
bool cas_init(uint8_t *sirk, T_CSIS_SIRK_TYPE sirk_type, uint8_t size, uint8_t rank,
              uint8_t feature);
#endif
#if LE_AUDIO_MCP_CLIENT_SUPPORT
bool mcs_client_init(void);
bool ots_client_init(void);
#endif
#if LE_AUDIO_CSIS_SUPPORT
bool csis_init(uint8_t set_mem_num);
bool csis_get_srv_tbl(uint8_t service_id, T_ATTRIB_APPL **pp_srv_tbl);
uint8_t csis_reg_srv(uint8_t *sirk, T_CSIS_SIRK_TYPE sirk_type, uint8_t size, uint8_t rank,
                     uint8_t feature, T_ATTR_UUID *p_inc_srv_uuid);
#endif
#if LE_AUDIO_MCP_SERVER_SUPPORT
bool mcs_server_init(uint8_t mcs_num);
#if LE_AUDIO_OTS_SERV_SUPPORT
bool ots_server_init(uint8_t ots_num);
#endif
#endif
#if LE_AUDIO_CCP_CLIENT_SUPPORT
bool ccp_client_init(void);
#endif
#if LE_AUDIO_CCP_SERVER_SUPPORT
bool ccp_server_init(uint8_t tbs_num);
#endif
#if LE_AUDIO_VCS_CLIENT_SUPPORT
typedef enum
{
    VCS_CHECK_PREFER_VOLUME,
    VCS_CHECK_PREFER_MUTE,
    VCS_CHECK_VOLUME_IS_EQUAL,
    VCS_CHECK_MUTE_IS_EQUAL,
} T_VCS_CHECK_TYPE;

#if LE_AUDIO_GROUP_SUPPORT
bool vcs_check_group_data(T_LE_AUDIO_GROUP *p_group, T_VCS_CHECK_TYPE type, uint8_t data);
#endif
#endif

#if LE_AUDIO_MICS_CLIENT_SUPPORT
typedef enum
{
    MICS_CHECK_PREFER_MUTE,
    MICS_CHECK_MUTE_IS_EQUAL,
} T_MICS_CHECK_TYPE;
#if LE_AUDIO_GROUP_SUPPORT
bool mics_check_group_data(T_LE_AUDIO_GROUP *p_group, T_MICS_CHECK_TYPE type, uint8_t data);
#endif
#endif
#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
