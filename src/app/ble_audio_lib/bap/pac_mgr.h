#ifndef _PAC_MGR_H_
#define _PAC_MGR_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "gatt.h"
#include "bt_gatt_svc.h"
#include "ble_audio_def.h"
#include "codec_def.h"
#include "pacs.h"

#if LE_AUDIO_PACS_SUPPORT

#define PAC_RECORD_INVALID_ID 0xff

typedef struct
{
    uint8_t pac_id;
    bool notify;
    T_AUDIO_DIRECTION direction;
    uint8_t  pac_data_len;
    uint16_t attrib_idx;
    uint16_t attrib_cccd_idx;
    uint8_t *pac_data;
} T_PAC_RECORD;

typedef struct _T_PAC_RECORD_LIST
{
    struct _T_PAC_RECORD_LIST *next;
    T_PAC_RECORD pac_record;
} T_PAC_RECORD_LIST;

extern T_PAC_RECORD_LIST *p_pac_list;
extern uint8_t pac_list_num;
extern uint8_t sink_pac_num;
extern uint8_t source_pac_num;
extern uint8_t pac_char_attrib_num;

bool pacs_gen_pac_records(uint16_t attrib_index, uint8_t **pp_data,
                          uint16_t *p_data_len);
bool pac_update(uint8_t pac_id, const uint8_t *pac_data, uint8_t pac_data_len);
T_PAC_RECORD *pac_find_by_id(uint8_t pac_id);
T_PAC_RECORD *pac_find_by_index(uint16_t attrib_index, bool cccd);
T_PAC_RECORD *pac_find_by_codec_data(uint8_t direction, uint8_t *codec_id, T_CODEC_CFG *p_cfg);
void pac_print_info(void);
#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
