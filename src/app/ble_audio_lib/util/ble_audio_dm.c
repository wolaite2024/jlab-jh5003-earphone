#include "trace.h"
#include "ble_audio_dm.h"
#include "os_queue.h"
#include "ble_audio_mgr.h"

typedef struct t_ble_dm_db
{
    T_OS_QUEUE  cback_list;         /* device manager callback list */
} T_BLE_DM_DB;

typedef struct t_ble_dm_cback_item
{
    struct t_ble_dm_cback_item *p_next;
    P_BLE_DM_CBACK              cback;
} T_BLE_DM_CBACK_ITEM;

T_BLE_DM_DB ble_dm_db;

bool ble_dm_cback_register(P_BLE_DM_CBACK cback)
{
    T_BLE_DM_CBACK_ITEM *p_item;

    p_item = (T_BLE_DM_CBACK_ITEM *)ble_dm_db.cback_list.p_first;
    while (p_item != NULL)
    {
        /* Sanity check if callback already registered */
        if (p_item->cback == cback)
        {
            return true;
        }

        p_item = p_item->p_next;
    }

    p_item = ble_audio_mem_zalloc(sizeof(T_BLE_DM_CBACK_ITEM));
    if (p_item != NULL)
    {
        p_item->cback = cback;
        os_queue_in(&ble_dm_db.cback_list, p_item);
        return true;
    }

    return false;
}

bool ble_dm_cback_unregister(P_BLE_DM_CBACK cback)
{
    T_BLE_DM_CBACK_ITEM *p_item;
    bool             ret = false;

    p_item = (T_BLE_DM_CBACK_ITEM *)ble_dm_db.cback_list.p_first;
    while (p_item != NULL)
    {
        if (p_item->cback == cback)
        {
            os_queue_delete(&ble_dm_db.cback_list, p_item);
            ble_audio_mem_free(p_item);
            ret = true;
            break;
        }

        p_item = p_item->p_next;
    }

    return ret;
}

bool ble_dm_event_post(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_CBACK_ITEM *p_item;
#if LE_AUDIO_DEINIT
    if (ble_audio_deinit_flow != 0)
    {
        return true;
    }
#endif
    PROTOCOL_PRINT_INFO1("ble_dm_event_post: event_type 0x%04x", event_type);

    p_item = (T_BLE_DM_CBACK_ITEM *)ble_dm_db.cback_list.p_first;
    while (p_item != NULL)
    {
        p_item->cback(event_type, event_buf, buf_len);
        p_item = p_item->p_next;
    }

    return true;
}

#if LE_AUDIO_DEINIT
void ble_dm_deinit(void)
{
    T_BLE_DM_CBACK_ITEM *p_item;
    while ((p_item = os_queue_out(&ble_dm_db.cback_list)) != NULL)
    {
        ble_audio_mem_free(p_item);
    }
}
#endif
