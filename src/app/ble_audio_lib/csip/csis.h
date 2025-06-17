#ifndef _CSIS_H_
#define _CSIS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "csis_def.h"
#include "bt_gatt_svc.h"
#include "set_member.h"

#if LE_AUDIO_CSIS_SUPPORT

T_SERVER_ID csis_add_service(const T_FUN_GATT_EXT_SERVICE_CBS *p_srv_ext_cbs, T_SET_MEM *p_entry);

#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
