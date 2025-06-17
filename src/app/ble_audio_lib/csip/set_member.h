#ifndef _SET_MEMBER_H_
#define _SET_MEMBER_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#if LE_AUDIO_CSIS_SUPPORT

#include "bt_gatt_svc.h"
#include "csis_def.h"
#include "sys_timer.h"
#include "bt_gatt_client.h"

typedef struct
{
    bool used;
    T_SERVER_ID service_id;
    uint8_t lock_address_type;
    uint8_t lock_address[GAP_BD_ADDR_LEN];

//  config data
    uint8_t sirk[CSIS_SIRK_LEN];
    T_CSIS_SIRK_TYPE sirk_type;
    uint8_t size;
    T_CSIS_LOCK lock;
    uint8_t rank;

// attribute idx
    uint8_t feature;
    uint8_t sirk_char_idx;
    uint8_t size_char_idx;
    uint8_t lock_char_idx;
    uint8_t rank_char_idx;
    T_ATTRIB_APPL *p_attr_tbl;
    T_ATTR_UUID inc_srv_uuid;
    T_SYS_TIMER_HANDLE timer_handle;
} T_SET_MEM;

extern uint8_t set_mem_num;
extern T_SET_MEM *p_set_mem;

bool set_member_init(uint8_t num);
T_SET_MEM *set_member_allocate_entry(uint8_t sirk[CSIS_SIRK_LEN], T_CSIS_SIRK_TYPE sirk_type,
                                     uint8_t size, uint8_t rank, uint8_t feature, T_ATTR_UUID *p_inc_srv_uuid);

T_SET_MEM *set_member_find_by_srv_id(T_SERVER_ID service_id);
T_SET_MEM *set_member_find_by_uuid(T_ATTR_UUID *p_inc_srv_uuid);

#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
