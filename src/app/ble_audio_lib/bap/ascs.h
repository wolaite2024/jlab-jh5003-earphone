#ifndef _ASCS_H_
#define _ASCS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ascs_def.h"
#include "bt_gatt_svc.h"

#if LE_AUDIO_ASCS_SUPPORT

#define ASCS_CHAR_ASE_CP_INDEX                   2
#define ASCS_CHAR_ASE_CP_CCCD_INDEX              3
#define ASCS_CHAR_ASE_INDEX                      5
#define ASCS_CHAR_ASE_CCCD_INDEX                 6

extern T_SERVER_ID ascs_service_id;
extern T_ATTRIB_APPL *p_ascs_attr_tbl;
extern uint16_t ascs_attr_num;

T_SERVER_ID ascs_add_service(const T_FUN_GATT_EXT_SERVICE_CBS *p_srv_ext_cbs);
uint8_t ascs_get_ase_id(uint16_t attrib_index, bool cccd);
bool ascs_send_ase_notify(uint16_t conn_handle, uint16_t cid, uint16_t ase_id, uint8_t *p_data,
                          uint16_t len);
bool ascs_send_ase_cp_notify(uint16_t conn_handle, uint16_t cid, uint8_t *p_data, uint16_t len);

#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
