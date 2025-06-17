#ifndef _ASE_HANDLE_CP_H_
#define _ASE_HANDLE_CP_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ascs_def.h"
#include "ase_sm.h"

// ASE CP minimum possible total length
#define ASE_CP_OP_MIN_LEN 3
#define ASE_CP_OP_CONFIG_CODEC_MIN_LEN        sizeof(T_ASE_CP_CODEC_CFG_ARRAY_PARAM)

#if (LE_AUDIO_ASCS_SUPPORT | LE_AUDIO_ASCS_CLIENT_SUPPORT)
bool ase_cp_check_sm_state(uint8_t ase_state, uint8_t direction, T_ASE_CP_OP cp_op, bool is_client);
#endif
#if LE_AUDIO_ASCS_SUPPORT
T_ASCS_ASE_CP *ase_check_cp_data(uint16_t conn_handle, uint16_t cid, uint8_t *p_data, uint16_t len);
#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
