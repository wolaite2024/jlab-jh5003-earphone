#ifndef _ASE_SM_H_
#define _ASE_SM_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ascs_def.h"
#include "gap.h"
#include "codec_def.h"
#include "ascs_mgr.h"

#if LE_AUDIO_ASCS_SUPPORT

typedef enum
{
    ASE_SM_CHECK_DISABLE = 0x00,
    ASE_SM_CHECK_ENABLE  = 0x01,
    ASE_SM_CHECK_RELEASE = 0x02,
} T_ASE_SM_CHECK_TYPE;

typedef struct
{
    uint16_t conn_handle;
    uint16_t cis_conn_handle;
    bool rec_start_ready;
    bool rec_stop_ready;
    bool app_handshake;
    bool app_handshake_rec;
    uint8_t added_path_direction;

    uint8_t ase_id;
    uint8_t ase_state;
    uint8_t direction;
    uint8_t codec_spec_cfg_len;
    uint8_t metadata_len;
    uint16_t min_transport_latency;
    T_ASE_CODEC_CFG_STATE_DATA codec_state_data;
    T_ASE_QOS_CFG_STATE_DATA qos_state_data;
    T_CODEC_CFG codec_cfg;
    uint8_t *p_codec_spec_cfg;
    uint8_t *p_metadata;
} T_ASE_ENTRY;

typedef struct
{
    uint16_t conn_handle;
    uint8_t number_of_ase;
    uint8_t ase_id[ASCS_AES_CHAR_MAX_NUM];
} T_ASCS_CP_ASE_ID_DATA;

typedef union
{
    T_ASCS_CP_CONFIG_CODEC_IND config_codec;
    T_ASCS_CP_CONFIG_QOS_IND config_qos;
    T_ASCS_CP_ENABLE_IND enable;
    T_ASCS_CP_ASE_ID_DATA ase_id_data;
    T_ASCS_CP_DISABLE_IND disable;
    T_ASCS_CP_UPDATE_METADATA_IND update_metadata;
} T_ASE_CP_DATA;

typedef struct
{
    T_ASE_CP_OP op;
    uint8_t number_of_ase;
    T_ASE_CP_DATA data;
} T_ASCS_ASE_CP;

void ascs_check_current_state(uint16_t conn_handle, T_ASE_ENTRY *p_entry);
bool ase_sm_check_iosch_status(T_ASE_SM_CHECK_TYPE type, T_ASE_ENTRY *p_entry);
T_ASE_ENTRY *ase_find_by_ase_id(uint16_t conn_handle, uint8_t ase_id);
T_ASE_ENTRY *ase_find_by_cig_cis(uint16_t conn_handle, T_AUDIO_DIRECTION direction, uint8_t cig_id,
                                 uint8_t cis_id);
bool ase_sm_link_init(uint16_t conn_handle);
void ase_sm_link_free(uint16_t conn_handle);
bool ase_gen_char_data(uint16_t conn_handle, uint8_t ase_id, uint8_t  **pp_data,
                       uint16_t *p_data_len);
bool ase_sm_send_notify(uint16_t conn_handle, T_ASE_ENTRY *p_entry);
bool ase_action_cfg_codec(uint16_t conn_handle, T_ASE_ENTRY *p_entry,
                          T_ASE_CP_CODEC_CFG_ARRAY_PARAM *p_param, uint8_t *p_codec_data,
                          bool is_client);
bool ase_action_cfg_qos(uint16_t conn_handle, T_ASE_ENTRY *p_entry,
                        T_ASE_CP_QOS_CFG_ARRAY_PARAM *p_param);
bool ase_action_enable(uint16_t conn_handle, T_ASE_ENTRY *p_entry,
                       uint8_t metadata_len, uint8_t *p_metadata);
bool ase_action_rec_stop_ready(uint16_t conn_handle, T_ASE_ENTRY *p_entry);
bool ase_action_rec_start_ready(uint16_t conn_handle, T_ASE_ENTRY *p_entry);
bool ase_action_disable(uint16_t conn_handle, T_ASE_ENTRY *p_entry, bool is_client);
bool ase_action_update_metadata(uint16_t conn_handle, T_ASE_ENTRY *p_entry,
                                uint8_t metadata_len, uint8_t *p_metadata, bool is_client);
bool ase_action_release(uint16_t conn_handle, T_ASE_ENTRY *p_entry, bool is_client);
bool ase_action_released(uint16_t conn_handle, T_ASE_ENTRY *p_entry);
bool ase_switch_to_streaming(uint16_t conn_handle, T_ASE_ENTRY *p_entry);
bool ase_switch_to_qos_cfg(uint16_t conn_handle, T_ASE_ENTRY *p_entry);

void ascs_change_state(T_ASE_ENTRY *p_entry, uint8_t ase_state);

#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
