#include "trace.h"
#include "ble_audio_mgr.h"
#include "os_queue.h"
#include "audio_stream_session_int.h"
#include "ble_audio_group_int.h"

#if LE_AUDIO_ASCS_CLIENT_SUPPORT

T_OS_QUEUE audio_session_queue;
uint8_t cig_id_free = 0x01;
uint8_t cis_id_free = 0x01;

uint8_t audio_stream_get_free_cig_id(void)
{
    uint8_t cig_id_max = CIG_ID_MAX;
    uint8_t cig_id_curr;
    bool found = false;
    for (cig_id_curr = cig_id_free; cig_id_curr < cig_id_max; cig_id_curr++)
    {
        if (audio_stream_session_find_by_cig_id(cig_id_curr) == NULL)
        {
            found = true;
            break;
        }
    }
    if (found == true)
    {
        cig_id_free = cig_id_curr + 1;
        if (cig_id_free == cig_id_max)
        {
            cig_id_free = 0x01;
        }
        return cig_id_curr;
    }
    return 0;
}

uint8_t audio_stream_get_free_cis_id(void)
{
    uint8_t cis_id_curr = cis_id_free;
    cis_id_free++;
    if (cis_id_free == CIS_ID_MAX)
    {
        cis_id_free = 0x01;
    }
    return cis_id_curr;
}

bool audio_stream_session_check_int(const char *p_func_name,
                                    T_AUDIO_STREAM_SESSION_HANDLE handle)
{
    if (handle != NULL)
    {
        if (os_queue_search(&audio_session_queue, handle) == true)
        {
            return true;
        }
    }
    PROTOCOL_PRINT_ERROR2("audio_stream_session_check:failed, %s, handle %p",
                          TRACE_STRING(p_func_name), handle);
    return false;
}

T_AUDIO_STREAM_SESSION_HANDLE audio_stream_session_allocate(T_BLE_AUDIO_GROUP_HANDLE group_handle)
{
    T_AUDIO_STREAM_SESSION *p_audio_session;
    if (ble_audio_group_handle_check(group_handle) == false)
    {
        return NULL;
    }

    p_audio_session = ble_audio_mem_zalloc(sizeof(T_AUDIO_STREAM_SESSION));

    if (p_audio_session == NULL)
    {
        return NULL;
    }
    p_audio_session->group_handle = group_handle;
    ble_audio_group_session(group_handle, true);
    PROTOCOL_PRINT_INFO2("audio_stream_session_allocate: session_handle %p, group_handle %p",
                         p_audio_session, group_handle);

    p_audio_session->cig_id = audio_stream_get_free_cig_id();
    os_queue_in(&audio_session_queue, p_audio_session);
    return p_audio_session;
}

bool audio_stream_session_release(T_AUDIO_STREAM_SESSION_HANDLE handle)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    if (audio_stream_session_check(handle) == false)
    {
        return false;
    }
    PROTOCOL_PRINT_INFO0("audio_stream_session_release");
    if (p_audio_session->server)
    {
        for (uint8_t i = 0; i < p_audio_session->server_num; i++)
        {
            for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
            {
                T_BAP_ASE_ENTRY *p_ase_entry = p_audio_session->server[i].ase_info[j].p_ase_entry;
                if (p_ase_entry)
                {
                    p_ase_entry->used = false;
                    p_ase_entry->handle = NULL;
                }
                if (p_audio_session->server[i].ase_info[j].p_metadata)
                {
                    ble_audio_mem_free(p_audio_session->server[i].ase_info[j].p_metadata);
                }
            }
            if (p_audio_session->server[i].ase_info)
            {
                ble_audio_mem_free(p_audio_session->server[i].ase_info);
            }
        }
        ble_audio_mem_free(p_audio_session->server);
    }
    if (p_audio_session->p_session_timer_handle)
    {
        sys_timer_delete(p_audio_session->p_session_timer_handle);
    }
    ble_audio_group_session(p_audio_session->group_handle, false);
    os_queue_delete(&audio_session_queue, p_audio_session);
    ble_audio_mem_free(p_audio_session);
    return true;
}

T_AUDIO_STREAM_SESSION_HANDLE audio_stream_session_find_by_cig_id(uint8_t cig_id)
{
    uint8_t i;
    T_AUDIO_STREAM_SESSION *p_audio_session;
    for (i = 0; i < audio_session_queue.count; i++)
    {
        p_audio_session = (T_AUDIO_STREAM_SESSION *)os_queue_peek(&audio_session_queue, i);
        if (p_audio_session != NULL)
        {
            if (p_audio_session->cig_id == cig_id)
            {
                return p_audio_session;
            }
        }
    }
    return NULL;
}

T_BLE_AUDIO_GROUP_HANDLE audio_stream_session_get_group(T_AUDIO_STREAM_SESSION_HANDLE handle)
{
    T_AUDIO_STREAM_SESSION *p_audio_session = (T_AUDIO_STREAM_SESSION *)handle;
    if (audio_stream_session_check(handle) == false)
    {
        return false;
    }
    return p_audio_session->group_handle;
}

#if LE_AUDIO_DEINIT
void audio_stream_session_deinit(void)
{
    T_AUDIO_STREAM_SESSION *p_audio_session;
    while ((p_audio_session = os_queue_out(&audio_session_queue)) != NULL)
    {
        if (p_audio_session->p_session_timer_handle)
        {
            sys_timer_delete(p_audio_session->p_session_timer_handle);
        }

        if (p_audio_session->p_cis_disc_timer_handle)
        {
            sys_timer_delete(p_audio_session->p_cis_disc_timer_handle);
        }

        if (p_audio_session->server)
        {
            for (uint8_t i = 0; i < p_audio_session->server_num; i++)
            {
                for (uint8_t j = 0; j < p_audio_session->server[i].ase_num; j++)
                {
                    if (p_audio_session->server[i].ase_info[j].p_metadata)
                    {
                        ble_audio_mem_free(p_audio_session->server[i].ase_info[j].p_metadata);
                    }
                }
                if (p_audio_session->server[i].ase_info)
                {
                    ble_audio_mem_free(p_audio_session->server[i].ase_info);
                }
            }
            ble_audio_mem_free(p_audio_session->server);
        }

        ble_audio_mem_free(p_audio_session);
    }
    cig_id_free = 0x01;
    cis_id_free = 0x01;
}
#endif
#endif
