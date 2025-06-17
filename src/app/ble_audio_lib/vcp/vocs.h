#ifndef _VOCS_H_
#define _VOCS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "vocs_def.h"
#include "bt_gatt_svc.h"

#if LE_AUDIO_VOCS_SUPPORT
typedef struct
{
    bool            param_init;
    uint8_t         srv_instance_id;
    T_SERVER_ID     service_id;
    uint8_t         feature;
    uint8_t         attr_num;
    uint8_t         change_counter;
    uint16_t        output_des_len;
    int16_t         volume_offset;
    uint32_t        audio_location;
    uint8_t         *output_des;
    T_ATTRIB_APPL   *p_attr_tbl;
} T_VOCS;

extern const T_ATTRIB_APPL vocs_attr_tbl[];

T_VOCS *vocs_find_by_idx(uint8_t srv_instance_id);
T_SERVER_ID vocs_add_service(const T_FUN_GATT_EXT_SERVICE_CBS *p_srv_ext_cbs,
                             T_VOCS *p_entry, uint8_t feature);

#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
