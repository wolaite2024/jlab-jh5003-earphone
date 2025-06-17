#ifndef _AICS_H_
#define _AICS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "aics_def.h"
#include "bt_gatt_svc.h"

#if LE_AUDIO_AICS_SUPPORT

#define AICS_CHAR_INPUT_STATE_INDEX               2
#define AICS_CHAR_INPUT_STATE_CCCD_INDEX          3
#define AICS_CHAR_GAIN_SETTINGS_INDEX             5
#define AICS_CHAR_INPUT_TYPE_INDEX                7
#define AICS_CHAR_INPUT_STATUS_INDEX              9
#define AICS_CHAR_INPUT_STATUS_CCCD_INDEX         10
#define AICS_CHAR_AUDIO_INPUT_CP_INDEX            12
#define AICS_CHAR_AUDIO_INPUT_DES_INDEX           14
#define AICS_CHAR_AUDIO_INPUT_DES_CCCD_INDEX      15

extern T_ATTRIB_APPL incl_aics_attr[];

T_SERVER_ID aics_add_service(const T_FUN_GATT_EXT_SERVICE_CBS *p_srv_ext_cbs);

#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
