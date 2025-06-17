#ifndef _BLE_AUDIO_MGR_H_
#define _BLE_AUDIO_MGR_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "gap_conn_le.h"
#include "os_queue.h"
#include "ase_sm.h"
#include "bt_gatt_svc.h"
#include "ble_audio.h"
#include "bt_gatt_client.h"
#include "bt_bond_mgr.h"
#include "ble_audio_mem.h"
#include "ble_audio_int.h"
#include "ots_client.h"

#define LE_AUDIO_DEINIT   0
#define LE_AUDIO_IO_MESSAGE_HANDLER LE_AUDIO_GROUP_SUPPORT

#define BLE_AUDIO_GROUP 2
#define BLE_AUDIO_ID_MAX 3


/* event handler function type */
typedef void(*P_BLE_AUDIO_EVT_HDLR)(void *p_msg, uint16_t event);
typedef uint16_t(*P_FUN_BLE_AUDIO_INT_CB)(T_LE_AUDIO_MSG msg, void *buf, bool *p_handled);

typedef struct
{
    bool        used;
    T_SERVER_ID service_id;
    uint16_t    cccd_cfg[1];
    uint16_t    cccd_pending[1];
} T_LE_SRV_CFG;

typedef struct
{
    bool                used;
    T_GAP_CONN_STATE    state;
    uint8_t             id;
    uint8_t             conn_id;
    uint16_t            conn_handle;
    uint16_t            mtu_size;
#if LE_AUDIO_BASS_SUPPORT
    uint8_t             past_mode;
#endif
    uint8_t             remote_bd_type;
    uint8_t             remote_bd[GAP_BD_ADDR_LEN];  /* current remote BD */
    bool                is_rpa;
    uint8_t             identity_addr_type;
    uint8_t             identity_addr[6];
    uint8_t             remote_feats[8];
#if LE_AUDIO_MICS_CLIENT_SUPPORT
    uint8_t             mics_mic_value;
#endif
#if LE_AUDIO_CAP_SUPPORT
    uint8_t             cap_init_flags;
    uint8_t             cap_discov_flags;
#endif
    uint8_t             srv_cfg_num;
    T_LE_SRV_CFG        *srv_cfg_tbl;

// for ASCS
#if LE_AUDIO_ASCS_SUPPORT
    void               *p_ase_entry;
#endif
#if LE_AUDIO_PACS_CLIENT_SUPPORT
    void               *p_pacs_client;
#endif
#if LE_AUDIO_BASS_CLIENT_SUPPORT
    void               *p_bass_client;
#endif
#if LE_AUDIO_ASCS_CLIENT_SUPPORT
    void               *p_ascs_client;
#endif
#if LE_AUDIO_VCS_CLIENT_SUPPORT
    void               *p_vcs_client;
#endif
#if LE_AUDIO_VOCS_CLIENT_SUPPORT
    void               *p_vocs_client;
#endif
#if LE_AUDIO_AICS_CLIENT_SUPPORT
    void               *p_aics_client;
#endif
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
    void               *p_csis_client;
#endif
#if LE_AUDIO_MCP_CLIENT_SUPPORT
    void               *p_mcs_client;
#endif
#if LE_AUDIO_OTS_CLIENT_SUPPORT
    void               *p_ots_client;
#endif

#if LE_AUDIO_CCP_CLIENT_SUPPORT
    void                *p_ccp_client;
#endif
#if LE_AUDIO_HAS_CLIENT_SUPPORT
    void                *p_has_client;
#endif
#if LE_AUDIO_BAP_SUPPORT
    void                *p_bap_cb;
#endif
} T_BLE_AUDIO_LINK;

typedef struct
{
    T_BLE_AUDIO_LINK        *le_link;
    T_OS_QUEUE              cback_list;
#if (LE_AUDIO_BASS_CLIENT_SUPPORT || LE_AUDIO_PACS_CLIENT_SUPPORT || LE_AUDIO_ASCS_CLIENT_SUPPORT||LE_AUDIO_CSIS_CLIENT_SUPPORT ||LE_AUDIO_VCS_CLIENT_SUPPORT ||LE_AUDIO_MICS_CLIENT_SUPPORT)
    P_FUN_BLE_AUDIO_INT_CB  p_cap_cb;
#endif

    const T_BT_BOND_MGR     *bond_mgr;
#if LE_AUDIO_IO_MESSAGE_HANDLER
    void                    *evt_queue_handle;
    void                    *io_queue_handle;
    P_BLE_AUDIO_EVT_HDLR    hdl_reg[BLE_AUDIO_ID_MAX]; /* registration structures */
    bool                    is_reg[BLE_AUDIO_ID_MAX];       /* registration structures */
    uint16_t                io_event_type;
#endif
    T_GAP_DEV_STATE         dev_state;
    uint8_t                 acl_link_num;

#if LE_AUDIO_ASCS_SUPPORT
    uint8_t    snk_ase_num;
    uint8_t    src_ase_num;
#endif

#if LE_AUDIO_PACS_SUPPORT
    uint16_t   sink_supported_contexts;
    uint16_t   source_supported_contexts;

    uint32_t sink_audio_location;
    uint32_t source_audio_location;
#endif
} T_BLE_AUDIO_DB;

extern T_BLE_AUDIO_DB ble_audio_db;
void ble_audio_reg_cap_cb(P_FUN_BLE_AUDIO_INT_CB p_bap_cb);
bool ble_audio_send_msg_to_app(uint16_t subtype, void *buf);
uint16_t ble_audio_mgr_dispatch(uint16_t msg, void *buf);
void ble_audio_mgr_send_server_cccd_info(uint16_t conn_handle, const T_ATTRIB_APPL *p_srv,
                                         T_SERVER_ID service_id, uint16_t attrib_index,
                                         uint16_t ccc_bits, uint32_t param, uint16_t attr_num);
void ble_audio_mgr_dispatch_client_attr_cccd_info(uint16_t conn_handle,
                                                  T_GATT_CLIENT_CCCD_CFG *p_cfg);
void ble_audio_handle_register(uint8_t id, const P_BLE_AUDIO_EVT_HDLR p_reg);

bool vocs_init(uint8_t srv_num, uint8_t *feature);
bool aics_init(uint8_t srv_num);
bool vcs_init(uint8_t vocs_num, uint8_t aics_num, uint8_t *id_array);
bool mics_init(uint8_t aics_num, uint8_t *id_array);


#if LE_AUDIO_DEINIT
#define LE_AUDIO_DEINIT_FLAG 0x01
extern uint8_t ble_audio_deinit_flow;

void ble_audio_link_deinit(void);
void audio_stream_session_deinit(void);
void ble_dm_deinit(void);
void ble_audio_group_deinit(void);
#if LE_AUDIO_ASCS_CLIENT_SUPPORT
void ascs_client_deinit(void);
#endif
#if LE_AUDIO_ASCS_SUPPORT
void ascs_deinit(void);
#endif
#if LE_AUDIO_BAP_SUPPORT
void bap_role_deinit(void);
#endif
#if LE_AUDIO_BASS_CLIENT_SUPPORT
void bass_client_deinit(void);
#endif
#if LE_AUDIO_BASS_SUPPORT
void bass_deinit(void);
#endif
#if LE_AUDIO_PACS_CLIENT_SUPPORT
void pacs_client_deinit(void);
#endif
#if LE_AUDIO_PACS_SUPPORT
void pacs_deinit(void);
#endif
#if LE_AUDIO_CAS_SUPPORT
void cas_deinit(void);
#endif
#if LE_AUDIO_CAP_SUPPORT
void cap_deinit(void);
#endif
#if LE_AUDIO_CSIS_SUPPORT
void csis_deinit(void);
#endif
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
void csis_client_deinit(void);
void set_coordinator_deinit(void);
#endif
#if LE_AUDIO_HAS_SUPPORT
void has_deinit(void);
#endif
#if LE_AUDIO_HAS_CLIENT_SUPPORT
void has_client_deinit(void);
#endif
#if LE_AUDIO_MICS_SUPPORT
void mics_deinit(void);
#endif
#if LE_AUDIO_TMAS_SUPPORT
void tmas_deinit(void);
#endif
#if LE_AUDIO_GMAS_SUPPORT
void gmas_deinit(void);
#endif
#if LE_AUDIO_AICS_SUPPORT
void aics_deinit(void);
#endif
#if LE_AUDIO_AICS_CLIENT_SUPPORT
void aics_client_deinit(void);
#endif
#if LE_AUDIO_VCS_SUPPORT
void vcs_deinit(void);
#endif
#if LE_AUDIO_VCS_CLIENT_SUPPORT
void vcs_client_deinit(void);
#endif
#if LE_AUDIO_VOCS_SUPPORT
void vocs_deinit(void);
#endif
#if LE_AUDIO_VOCS_CLIENT_SUPPORT
void vocs_client_deinit(void);
#endif
#if LE_AUDIO_BASE_DATA_GENERATE
void base_data_gen_deinit(void);
#endif
#if LE_AUDIO_BASE_DATA_PARSE
void base_data_parse_deinit(void);
#endif
#if (LE_AUDIO_BROADCAST_SINK_ROLE | LE_AUDIO_BROADCAST_ASSISTANT_ROLE)
void ble_audio_sync_deinit(void);
#endif
#if LE_AUDIO_BROADCAST_SOURCE_ROLE
void broadcast_source_deinit(void);
#endif

#if LE_AUDIO_CCP_SERVER_SUPPORT
void ccp_server_deinit(void);
#endif
#if LE_AUDIO_CCP_CLIENT_SUPPORT
void ccp_client_deinit(void);
#endif
#if (LE_AUDIO_OTS_CLIENT_SUPPORT|LE_AUDIO_OTS_SERV_SUPPORT)
void le_coc_deinit(void);
#endif
#if LE_AUDIO_OTS_SERV_SUPPORT
void ots_server_deinit(void);
#endif
#if LE_AUDIO_OTS_CLIENT_SUPPORT
void ots_client_deinit(void);
#endif
#if LE_AUDIO_MCP_SERVER_SUPPORT
void mcs_server_deinit(void);
#endif
#if LE_AUDIO_MCP_CLIENT_SUPPORT
void mcs_client_deinit(void);
#endif
#if LE_AUDIO_MEM_DUMP
void ble_audio_mem_deinit(void);
#endif
#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
