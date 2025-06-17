#ifndef _ASCS_CLIENT_CP_H_
#define _ASCS_CLIENT_CP_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#if LE_AUDIO_ASCS_CLIENT_SUPPORT

#include "gap_cig_mgr.h"

#define ASE_DATA_HEADER_LEN                     2
#define ASE_DATA_CODEC_LEN_OFFSET               (ASE_DATA_HEADER_LEN+sizeof(T_ASE_CODEC_CFG_STATE_DATA) - 1)
#define ASE_DATA_CODEC_CONFIGURED_STATE_MIN_LEN (ASE_DATA_HEADER_LEN+sizeof(T_ASE_CODEC_CFG_STATE_DATA))
#define ASE_DATA_QOS_CONFIGURED_STATE_LEN       (ASE_DATA_HEADER_LEN+sizeof(T_ASE_QOS_CFG_STATE_DATA))
#define ASE_DATA_WITH_METADATA_MIN_LEN          5
#define ASE_DATA_WITH_METADATA_LEN_OFFSET       4

typedef enum
{
    ASE_PENDING_IDLE,
    ASE_PENDING_READ_ASE,
    ASE_PENDING_READ_ASE_ALL,
} T_ASE_PENDING_ACTION;

typedef struct
{
    bool used;
    uint8_t instance_id;

    uint16_t cis_conn_handle;
    bool rec_start_ready_send;
    bool rec_stop_ready_send;
    bool app_handshake;
    bool app_handshake_rec;
    bool release_local;
    uint8_t added_path_direction;

    uint8_t ase_id;
    uint8_t ase_state;
    uint8_t direction;
    uint8_t codec_spec_cfg_len;
    uint8_t metadata_len;
    T_ASE_CODEC_CFG_STATE_DATA codec_state_data;
    T_ASE_QOS_CFG_STATE_DATA qos_state_data;
    uint8_t *p_codec_spec_cfg;
    uint8_t *p_metadata;
} T_ASE_CHAR_ENTRY;

typedef struct
{
    uint8_t snk_ase_num;
    uint8_t src_ase_num;
    uint16_t ase_cp_handle;

    T_ASE_PENDING_ACTION pending_action;
    T_ASE_CHAR_ENTRY ase_data[1];
} T_ASCS_CLIENT_DB;

T_ASE_CHAR_ENTRY *ascs_find_ase_by_cig_cis_id(uint16_t conn_handle, uint8_t cig_id, uint8_t cis_id);
T_ASE_CHAR_ENTRY *ascs_find_ase_by_ase_id(uint16_t conn_handle, uint8_t ase_id);
T_ASCS_CLIENT_DB *ascs_check_link_int(const char *p_func_name, uint16_t conn_handle);
#define ascs_check_link(conn_handle) ascs_check_link_int(__func__, conn_handle)
bool ascs_send_ase_cp(uint16_t conn_handle, uint8_t *p_data, uint16_t len, bool is_req);
void ascs_client_check_state(uint16_t conn_handle, T_ASE_CHAR_ENTRY *p_entry);
T_APP_RESULT ascs_client_cig_cb(uint8_t cig_id, uint8_t cb_type, void *p_cb_data);
void ascs_client_handle_cig_info(uint8_t conn_id, uint16_t cis_conn_handle, uint8_t cb_type,
                                 T_CIG_MGR_CB_DATA *p_data);
T_ASE_CHAR_ENTRY *ase_cp_check_state(uint16_t conn_handle, T_ASE_CP_OP cp_op, uint8_t ase_id);

#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
