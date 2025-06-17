#include <string.h>
#include "trace.h"
#include "set_member.h"
#include "ble_audio_mgr.h"

#if LE_AUDIO_CSIS_SUPPORT
uint8_t set_mem_num = 0;
T_SET_MEM *p_set_mem = NULL;

bool set_member_init(uint8_t num)
{
    if (num == 0 || p_set_mem != NULL)
    {
        PROTOCOL_PRINT_ERROR0("set_member_init: invalid param");
        return false;
    }
    p_set_mem = ble_audio_mem_zalloc(num * sizeof(T_SET_MEM));
    if (p_set_mem == NULL)
    {
        PROTOCOL_PRINT_ERROR0("set_member_init: allocate mem failed");
        return false;
    }
    set_mem_num = num;
    return true;
}

T_SET_MEM *set_member_allocate_entry(uint8_t sirk[CSIS_SIRK_LEN], T_CSIS_SIRK_TYPE sirk_type,
                                     uint8_t size, uint8_t rank, uint8_t feature, T_ATTR_UUID *p_inc_srv_uuid)
{
    uint8_t i;
    if (p_set_mem == NULL)
    {
        return NULL;
    }
    for (i = 0; i < set_mem_num; i++)
    {
        if (p_set_mem[i].used == false)
        {
            memset(&p_set_mem[i], 0, sizeof(T_SET_MEM));
            memcpy(p_set_mem[i].sirk, sirk, CSIS_SIRK_LEN);
            p_set_mem[i].sirk_type = sirk_type;
            p_set_mem[i].size = size;
            p_set_mem[i].lock = CSIS_UNLOCKED;
            p_set_mem[i].rank = rank;
            p_set_mem[i].feature = feature;
            if (p_inc_srv_uuid)
            {
                memcpy(&p_set_mem[i].inc_srv_uuid, p_inc_srv_uuid, sizeof(T_ATTR_UUID));
            }
            return &p_set_mem[i];
        }
    }
    return NULL;
}

T_SET_MEM *set_member_find_by_uuid(T_ATTR_UUID *p_inc_srv_uuid)
{
    uint8_t  i;
    T_SET_MEM *p_mem = NULL;
    T_ATTR_UUID temp_uuid = {0};

    if (p_set_mem == NULL)
    {
        return NULL;
    }
    if (p_inc_srv_uuid == NULL)
    {
        p_inc_srv_uuid = &temp_uuid;
    }

    for (i = 0; i < set_mem_num; i++)
    {
        if (p_set_mem[i].used && p_set_mem[i].inc_srv_uuid.is_uuid16 == p_inc_srv_uuid->is_uuid16)
        {
            if (p_inc_srv_uuid->is_uuid16)
            {
                if (p_set_mem[i].inc_srv_uuid.p.uuid16 == p_inc_srv_uuid->p.uuid16)
                {
                    p_mem = &p_set_mem[i];
                    break;
                }
            }
            else
            {
                if (memcmp(p_set_mem[i].inc_srv_uuid.p.uuid128, p_inc_srv_uuid->p.uuid128, 16) == 0)
                {
                    p_mem = &p_set_mem[i];
                    break;
                }
            }
        }
    }
    return p_mem;
}

T_SET_MEM *set_member_find_by_srv_id(T_SERVER_ID service_id)
{
    uint8_t  i;

    if (p_set_mem == NULL)
    {
        return NULL;
    }

    for (i = 0; i < set_mem_num; i++)
    {
        if (p_set_mem[i].used
            && (p_set_mem[i].service_id == service_id))
        {
            return (&p_set_mem[i]);
        }
    }

    PROTOCOL_PRINT_ERROR1("set_member_find_by_srv_id: failed, service_id 0x%x", service_id);

    return NULL;
}

#endif

