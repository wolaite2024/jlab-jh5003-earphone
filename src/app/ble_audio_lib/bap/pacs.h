#ifndef _PACS_H_
#define _PACS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "pacs_def.h"
#include "ble_audio_def.h"
#include "bt_gatt_svc.h"

#if LE_AUDIO_PACS_SUPPORT

#define PACS_CHAR_AUDIO_CONTEXTS_AVA_INDEX            2
#define PACS_CHAR_AUDIO_CONTEXTS_AVA_CCCD_INDEX       3
#define PACS_CHAR_SUPPORTED_AUDIO_CONTEXTS_INDEX      5
#define PACS_CHAR_SUPPORTED_AUDIO_CONTEXTS_CCCD_INDEX 6

typedef struct
{
    T_SERVER_ID service_id;
    uint8_t sink_audio_loc_idx;
    uint8_t sink_audio_loc_cccd_idx;
    uint8_t sink_pac_start_idx;
    uint8_t sink_pac_end_idx;
    uint8_t source_audio_loc_idx;
    uint8_t source_audio_loc_cccd_idx;
    uint8_t source_pac_start_idx;
    uint8_t source_pac_end_idx;
    uint16_t attr_num;
    T_ATTRIB_APPL *p_pacs_attr_tbl;
} T_PACS;

extern T_PACS *p_pacs;
extern uint8_t pacs_sink_audio_locations_char_prop;
extern uint8_t pacs_source_audio_locations_char_prop;
extern uint8_t pacs_supported_audio_contexts_char_prop;

bool pacs_add_service(const T_FUN_GATT_EXT_SERVICE_CBS *p_srv_ext_cbs);

#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
