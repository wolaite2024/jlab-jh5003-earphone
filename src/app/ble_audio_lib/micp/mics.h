#ifndef _MICS_H_
#define _MICS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "mics_def.h"
#include "bt_gatt_svc.h"

#if LE_AUDIO_MICS_SUPPORT
typedef struct
{
    T_SERVER_ID    service_id;
    T_MICS_MUTE    mic_mute;
    uint8_t        attr_num;
    T_ATTRIB_APPL *p_mics_attr_tbl;
} T_MGR_MICS;

extern T_MGR_MICS *p_mgr_mics;


T_SERVER_ID mics_add_service(const T_FUN_GATT_EXT_SERVICE_CBS *p_srv_ext_cbs,
                             uint8_t aics_num, uint8_t *id_array);

#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
