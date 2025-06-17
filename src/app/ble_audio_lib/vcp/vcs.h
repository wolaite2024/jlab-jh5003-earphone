#ifndef _VCS_H_
#define _VCS_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "vcs_def.h"
#include "bt_gatt_svc.h"

#if LE_AUDIO_VCS_SUPPORT

#define VCS_WRITE_FLAGS_VOLUME_SETTING 0x01
#define VCS_WRITE_FLAGS_MUTE           0x02
#define VCS_WRITE_FLAGS_VOLUME_FLAGS   0x04

#define VCS_CHAR_VOLUME_STATE_INDEX                   1
#define VCS_CHAR_VOLUME_STATE_CCCD_INDEX              2
#define VCS_CHAR_VOLUME_CP_INDEX                      4
#define VCS_CHAR_VOLUME_FLAGS_INDEX                   6
#define VCS_CHAR_VOLUME_FLAGS_CCCD_INDEX              7

typedef struct
{
    bool           param_init;
    T_SERVER_ID    service_id;
    uint8_t        volume_flags;
    uint8_t        write_flags;
    uint8_t        step_size;
    T_VOLUME_STATE volume_state;
    uint8_t        char_attr_offset;
    uint8_t        attr_num;
    T_ATTRIB_APPL *p_vcs_attr_tbl;
} T_MGR_VCS;

extern T_MGR_VCS *p_mgr_vcs;

T_SERVER_ID vcs_add_service(const T_FUN_GATT_EXT_SERVICE_CBS *p_srv_ext_cbs,
                            uint8_t vocs_num, uint8_t aics_num, uint8_t *id_array);

#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
