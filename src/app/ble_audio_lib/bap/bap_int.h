#ifndef _BAP_INT_H_
#define _BAP_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "os_queue.h"
#include "gap_le_types.h"
#include "ble_audio.h"
#include "bap.h"
#include "ascs_def.h"
#include "ble_audio_mgr.h"
#include "bap_unicast_client.h"

#define BAP_DISC_PACS 0x01
#define BAP_DISC_ASCS 0x02
#define BAP_DISC_BASS 0x04

#if LE_AUDIO_ASCS_CLIENT_SUPPORT
typedef struct
{
    bool            used;
    uint8_t         instance_id;
    uint8_t         ase_id;
    T_ASE_STATE     ase_state;
    T_ASE_CP_OP     pending_cp;
    bool            wait_cp;
    uint8_t         direction;
    T_AUDIO_STREAM_SESSION_HANDLE handle;
} T_BAP_ASE_ENTRY;

typedef struct
{
    uint8_t             snk_ase_num;
    uint8_t             src_ase_num;
    T_BAP_ASE_ENTRY     ase_data[];
} T_BAP_ASCS_CB;

uint16_t bap_unicast_client_handle_ascs_msg(uint16_t msg, void *buf, bool *p_handled);
void bap_unicast_client_handle_disconn_msg(T_BLE_AUDIO_LINK *p_link, uint16_t cause);
T_BAP_ASE_ENTRY *bap_find_ase_by_ase_id(uint16_t conn_handle, uint8_t ase_id);
#endif

#if LE_AUDIO_PACS_CLIENT_SUPPORT
typedef struct t_bap_pac_element
{
    struct t_bap_pac_element  *p_next;
    T_BAP_PAC_RECORD           pac_record;
} T_BAP_PAC_ELEMENT;

typedef struct
{
    uint16_t            value_exist;
    uint16_t            read_req;
    uint32_t            snk_audio_loc;
    uint32_t            src_audio_loc;
    uint16_t            snk_sup_context;
    uint16_t            src_sup_context;
    uint16_t            snk_avail_context;
    uint16_t            src_avail_context;

    T_OS_QUEUE          snk_pac_queue;
    T_OS_QUEUE          src_pac_queue;
} T_BAP_PACS_CB;
#endif


#if LE_AUDIO_BAP_SUPPORT
#if (LE_AUDIO_BASS_CLIENT_SUPPORT || LE_AUDIO_PACS_CLIENT_SUPPORT || LE_AUDIO_ASCS_CLIENT_SUPPORT)
typedef struct
{
    uint16_t            conn_handle;
    bool                disc_done;
    uint8_t             disc_flags;
#if LE_AUDIO_BASS_CLIENT_SUPPORT
    uint8_t             brs_char_num;
#endif
#if LE_AUDIO_ASCS_CLIENT_SUPPORT
    T_BAP_ASCS_CB      *p_ascs_cb;
#endif
#if LE_AUDIO_PACS_CLIENT_SUPPORT
    T_BAP_PACS_CB       pacs_cb;
#endif
} T_BAP_LINK_CB;
#endif

typedef struct
{
    uint16_t        role_msk;
    uint8_t         disc_flags;
} T_BAP_DB;

extern T_BAP_DB bap_db;

T_APP_RESULT ble_audio_big_sync_receiver_cb(uint8_t big_handle, uint8_t cb_type,
                                            void *p_cb_data);
T_APP_RESULT broadcast_source_isoc_broadcaster_cb(uint8_t big_handle, uint8_t cb_type,
                                                  void *p_cb_data);
#if (LE_AUDIO_BASS_CLIENT_SUPPORT || LE_AUDIO_PACS_CLIENT_SUPPORT || LE_AUDIO_ASCS_CLIENT_SUPPORT)
#define bap_check_link(conn_handle) bap_check_link_int(__func__, conn_handle)
T_BAP_LINK_CB *bap_check_link_int(const char *p_func_name, uint16_t conn_handle);
void bap_send_discovery_result(T_BAP_LINK_CB *p_bap_cb, uint16_t cause);
#endif
void bap_unicast_client_check_dev_add(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                      T_BLE_AUDIO_DEV_HANDLE dev_handle);
void bap_unicast_client_check_session(uint16_t conn_handle);
#endif

#if LE_AUDIO_BASS_CLIENT_SUPPORT
#define LE_AUDIO_MSG_BASS_CLIENT_DIS_DONE (LE_AUDIO_MSG_GROUP_BASS_CLIENT | LE_AUDIO_MSG_INTERNAL | 0x01)

typedef struct
{
    uint16_t conn_handle;
    bool    is_found;
    bool    load_from_ftl;
    uint8_t brs_char_num;
} T_BASS_CLIENT_DIS_DONE;
bool bass_client_init(void);
void bass_enable_cccd(uint16_t conn_handle);
bool bass_read_brs_value(uint16_t conn_handle, uint8_t instance_id);
#endif
#if LE_AUDIO_BASS_SUPPORT
bool bass_init(uint8_t brs_num);
#endif

#ifdef CONFIG_REALTEK_BLE_USE_UAL_API
void ble_audio_sync_init(void);
#endif

#if LE_AUDIO_ASCS_SUPPORT
void ascs_init(uint8_t snk_ase_num, uint8_t src_ase_num, bool init_gap);
#endif
#if LE_AUDIO_PACS_CLIENT_SUPPORT
bool pacs_client_init(void);
#endif
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
