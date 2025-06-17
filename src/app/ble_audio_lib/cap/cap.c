#include <string.h>
#include "trace.h"
#include "ble_audio_mgr.h"
#include "cap.h"
#include "ble_audio_dm.h"
#include "ble_link_util.h"
#include "csis_client_int.h"
#include "vcs_client.h"
#include "volume_ctrl_int.h"
#include "csis_client.h"
#include "cap_int.h"
#include "mics_client.h"
#include "ble_audio_group_int.h"

#if LE_AUDIO_CAP_SUPPORT
#define CAP_VCP_FLAG   0x01
#define CAP_MICP_FLAG  0x02
#define CAP_CAS_FLAG   0x04
#define CAP_CSIS_FLAG  0x08

#define VCS_SET_VOLUME   0x01
#define VCS_SET_MUTE     0x02
#define VCS_SET_MIC_MUTE 0x04

#define CAP_DEBUG      0

typedef struct
{
    uint8_t               param_set_mask;
#if LE_AUDIO_VCS_CLIENT_SUPPORT
    //vcs
    T_VCS_CP_OP           vcs_op;
    uint8_t               vcs_update_mask; //T_BLE_AUDIO_LINK: id
    uint8_t               prefer_volume;
    uint8_t               prefer_mute;
#endif
#if LE_AUDIO_MICS_CLIENT_SUPPORT
    //mics
    uint8_t               mic_update_mask;
    T_MICS_MUTE           prefer_mic_mute;
#endif
} T_CAP_PROC_DB;

typedef struct
{
    uint8_t  cap_role;
    uint8_t  cap_init_flags;
    uint16_t cap_discov_flags;
} T_CAP_DB;

static T_CAP_DB cap_db;

#if CAP_DEBUG
#define cap_print_proc_db(idx, p_cap_proc_db, cause) cap_print_proc_db_int(__func__, idx, p_cap_proc_db, cause)
void cap_print_proc_db_int(const char *p_func_name, uint16_t idx, T_CAP_PROC_DB *p_cap_proc_db,
                           uint8_t cause)
{
    if (p_cap_proc_db)
    {
        PROTOCOL_PRINT_INFO4("[CAP][VCP] cap_print_proc_db : %s, idx %d, param_set_mask 0x%x, cause 0x%x",
                             TRACE_STRING(p_func_name), idx, p_cap_proc_db->param_set_mask, cause);
#if (LE_AUDIO_VCS_CLIENT_SUPPORT && LE_AUDIO_MICS_CLIENT_SUPPORT)
        PROTOCOL_PRINT_INFO6("[CAP][VCP] cap_print_proc_db : vcs_op %d, vcs_update_mask 0x%x, prefer_volume %d, prefer_mute %d, mic_update_mask 0x%x, prefer_mic_mute %d",
                             p_cap_proc_db->vcs_op, p_cap_proc_db->vcs_update_mask,
                             p_cap_proc_db->prefer_volume, p_cap_proc_db->prefer_mute,
                             p_cap_proc_db->mic_update_mask, p_cap_proc_db->prefer_mic_mute);
#endif
    }
    else
    {
        PROTOCOL_PRINT_INFO2("[CAP][VCP] cap_print_proc_db : %s, idx %d, p_cap_proc_db = NULL",
                             TRACE_STRING(p_func_name), idx);
    }
}
#endif

#if LE_AUDIO_GROUP_SUPPORT
T_LE_AUDIO_CAUSE cap_check_group_state_int(const char *p_func_name,
                                           T_BLE_AUDIO_GROUP_HANDLE group_handle, bool csis_check)
{
    T_LE_AUDIO_CAUSE cause = LE_AUDIO_CAUSE_SUCCESS;
    T_LE_AUDIO_GROUP *p_group = (T_LE_AUDIO_GROUP *)group_handle;
    if (ble_audio_group_handle_check(group_handle) == false)
    {
        cause = LE_AUDIO_CAUSE_INVALID_GROUP;
        goto failed;
    }
    if (p_group->conn_dev_num == 0)
    {
        cause = LE_AUDIO_CAUSE_NO_CONN;
        goto failed;
    }
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
    if (p_group->p_csip_cb == NULL)
    {
        if (csis_check)
        {
            cause = LE_AUDIO_CAUSE_INVALID_CSIS_UNKNOWN;
            goto failed;
        }
    }
    else
    {
        if (csis_check)
        {
            if (p_group->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_LOCK ||
                p_group->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_UNLOCK ||
                p_group->p_csip_cb->lock_state == SET_COORDINATOR_LOCK_UNKNOWN ||
                p_group->p_csip_cb->lock_state == SET_COORDINATOR_LOCK_DENIED)
            {
                cause = LE_AUDIO_CAUSE_INVALID_CSIS_LOCK_STATE;
                goto failed;
            }
        }
    }
#endif
    PROTOCOL_PRINT_INFO2("[CAP][VCP] cap_check_group_state: %s, group_handle %p",
                         TRACE_STRING(p_func_name), p_group);
    return cause;
failed:
    PROTOCOL_PRINT_ERROR3("[CAP][VCP] cap_check_group_state:failed, %s, group_handle %p, cause %d",
                          TRACE_STRING(p_func_name), p_group, cause);
    return cause;
}
#endif

#if LE_AUDIO_CSIS_CLIENT_SUPPORT
static T_CAP_PROC_DB *cap_find_or_alloc_proc_cb(T_LE_AUDIO_GROUP *p_group)
{
    if (p_group != NULL && p_group->p_csip_cb != NULL &&
        p_group->p_csip_cb->serv_uuid == GATT_UUID_CAS)
    {
        //PROTOCOL_PRINT_INFO2("[CAP][VCP] cap_find_or_alloc_proc_cb: p_group %p, p_group->p_cap %p", p_group, p_group->p_cap);
        if (p_group->p_cap == NULL)
        {
            p_group->p_cap = ble_audio_mem_zalloc(sizeof(T_CAP_PROC_DB));
#if LE_AUDIO_VCS_CLIENT_SUPPORT
            if (p_group->p_cap)
            {
                T_CAP_PROC_DB *p_cap_cb = (T_CAP_PROC_DB *)p_group->p_cap;
                p_cap_cb->vcs_op = VCS_CP_MAX;
            }
#endif
        }
        if (p_group->p_cap)
        {
            return p_group->p_cap;
        }
    }
    return NULL;
}

T_CAP_PROC_DB *cap_find_group(T_BLE_AUDIO_LINK *p_link, T_LE_AUDIO_GROUP **pp_group,
                              T_LE_AUDIO_DEV **pp_set_member)
{
    T_CAP_PROC_DB *p_cap_db = NULL;
    T_LE_AUDIO_GROUP *p_group;
    T_BLE_AUDIO_DEV_HANDLE dev_handle;
    p_group = set_coordinator_find_by_addr(p_link->remote_bd, p_link->remote_bd_type, GATT_UUID_CAS,
                                           &dev_handle);
    if (p_group)
    {
        p_cap_db = cap_find_or_alloc_proc_cb(p_group);
        if (p_cap_db)
        {
            *pp_set_member = dev_handle;
            *pp_group = p_group;
        }
    }
    return p_cap_db;
}

#if LE_AUDIO_VCS_CLIENT_SUPPORT
T_LE_AUDIO_CAUSE cap_vcs_change_mute_int(T_LE_AUDIO_GROUP *p_group, T_CAP_PROC_DB *p_cap_db,
                                         uint8_t mute)
{
    T_LE_AUDIO_CAUSE cause = LE_AUDIO_CAUSE_SUCCESS;
    T_LE_AUDIO_DEV *p_set_member;

    if (p_cap_db)
    {
        p_cap_db->param_set_mask |= VCS_SET_MUTE;
        p_cap_db->prefer_mute = mute;
        if (p_cap_db->vcs_op != VCS_CP_MAX &&
            p_cap_db->vcs_op != VCS_CP_UNMUTE &&
            p_cap_db->vcs_op != VCS_CP_MUTE)
        {
            PROTOCOL_PRINT_ERROR1("[CAP][VCP] cap_vcs_change_mute_int: invalid vcs_op %d", p_cap_db->vcs_op);
            return cause;
        }
    }
    if (p_group->p_csip_cb != NULL &&
        (p_group->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_LOCK ||
         p_group->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_UNLOCK ||
         p_group->p_csip_cb->lock_state == SET_COORDINATOR_LOCK_UNKNOWN ||
         p_group->p_csip_cb->lock_state == SET_COORDINATOR_LOCK_DENIED))
    {
        return cause;
    }
#if CAP_DEBUG
    cap_print_proc_db(1, p_cap_db, 0);
#endif
    for (int i = 0; i < p_group->dev_queue.count; i++)
    {
        p_set_member = (T_LE_AUDIO_DEV *)os_queue_peek(&p_group->dev_queue, i);
        if (p_set_member != NULL && p_set_member->p_link &&
            p_set_member->p_link->state == GAP_CONN_STATE_CONNECTED)
        {
            if (p_cap_db && (p_cap_db->vcs_update_mask & (1 << (p_set_member->p_link->id))) != 0)
            {
                continue;
            }
            if ((p_set_member->p_link->cap_discov_flags & CAP_VCP_FLAG) == 0)
            {
                continue;
            }
            T_VOLUME_STATE data;
            if (vcs_get_volume_state(p_set_member->p_link->conn_handle, &data))
            {
                if (data.mute != mute)
                {
                    if (mute == VCS_NOT_MUTED)
                    {
                        if (vcs_write_cp(p_set_member->p_link->conn_handle, VCS_CP_UNMUTE, NULL))
                        {
                            if (p_cap_db)
                            {
                                p_cap_db->vcs_op = VCS_CP_UNMUTE;
                                p_cap_db->vcs_update_mask |= (1 << (p_set_member->p_link->id));
                            }
                        }
                    }
                    else
                    {
                        if (vcs_write_cp(p_set_member->p_link->conn_handle, VCS_CP_MUTE, NULL))
                        {
                            if (p_cap_db)
                            {
                                p_cap_db->vcs_op = VCS_CP_MUTE;
                                p_cap_db->vcs_update_mask |= (1 << (p_set_member->p_link->id));
                            }
                        }
                    }
                }
            }
        }
    }
#if CAP_DEBUG
    cap_print_proc_db(2, p_cap_db, cause);
#endif
    return cause;
}

T_LE_AUDIO_CAUSE cap_vcs_change_mute(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                     uint8_t mute)
{
    T_LE_AUDIO_CAUSE cause;
    T_LE_AUDIO_GROUP *p_group = (T_LE_AUDIO_GROUP *)group_handle;
    T_CAP_PROC_DB *p_cap_db = NULL;
    cause = cap_check_group_state(group_handle, false);
    if (mute > VCS_MUTED)
    {
        cause = LE_AUDIO_CAUSE_INVALID_PARAM;
    }
    if (cause == LE_AUDIO_CAUSE_SUCCESS)
    {
        p_cap_db = cap_find_or_alloc_proc_cb(p_group);

        if (vcs_check_group_data(p_group, VCS_CHECK_PREFER_MUTE, mute))
        {
            return cause;
        }
        cause = cap_vcs_change_mute_int(p_group, p_cap_db, mute);
    }
    PROTOCOL_PRINT_INFO3("[CAP][VCP] cap_vcs_change_mute: group_handle %p, mute %d, cause 0x%x",
                         group_handle, mute, cause);
    return cause;
}

T_LE_AUDIO_CAUSE cap_vcs_change_volume_int(T_LE_AUDIO_GROUP *p_group, T_CAP_PROC_DB *p_cap_db,
                                           uint8_t volume_setting)
{
    T_LE_AUDIO_CAUSE cause = LE_AUDIO_CAUSE_SUCCESS;
    T_LE_AUDIO_DEV *p_set_member;
    T_VCS_VOLUME_CP_PARAM param;

    if (p_cap_db)
    {
        p_cap_db->param_set_mask |= VCS_SET_VOLUME;
        p_cap_db->prefer_volume = volume_setting;
        if (p_cap_db->vcs_op != VCS_CP_MAX &&
            p_cap_db->vcs_op != VCS_CP_SET_ABSOLUTE_VOLUME)
        {
            PROTOCOL_PRINT_ERROR1("[CAP][VCP] cap_vcs_change_volume_int: invalid vcs_op %d", p_cap_db->vcs_op);
            return cause;
        }
    }
    if (p_group->p_csip_cb != NULL &&
        (p_group->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_LOCK ||
         p_group->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_UNLOCK ||
         p_group->p_csip_cb->lock_state == SET_COORDINATOR_LOCK_UNKNOWN ||
         p_group->p_csip_cb->lock_state == SET_COORDINATOR_LOCK_DENIED))
    {
        return cause;
    }
    param.volume_setting = volume_setting;
#if CAP_DEBUG
    cap_print_proc_db(11, p_cap_db, 0);
#endif
    for (int i = 0; i < p_group->dev_queue.count; i++)
    {
        p_set_member = (T_LE_AUDIO_DEV *)os_queue_peek(&p_group->dev_queue, i);
        if (p_set_member != NULL && p_set_member->p_link &&
            p_set_member->p_link->state == GAP_CONN_STATE_CONNECTED)
        {
            if (p_cap_db && (p_cap_db->vcs_update_mask & (1 << (p_set_member->p_link->id))) != 0)
            {
                continue;
            }
            if ((p_set_member->p_link->cap_discov_flags & CAP_VCP_FLAG) == 0)
            {
                continue;
            }
            T_VOLUME_STATE data;
            if (vcs_get_volume_state(p_set_member->p_link->conn_handle, &data))
            {
                if (data.volume_setting != volume_setting)
                {
                    if (vcs_write_cp(p_set_member->p_link->conn_handle, VCS_CP_SET_ABSOLUTE_VOLUME,
                                     &param))
                    {
                        if (p_cap_db)
                        {
                            p_cap_db->vcs_op = VCS_CP_SET_ABSOLUTE_VOLUME;
                            p_cap_db->vcs_update_mask |= (1 << (p_set_member->p_link->id));
                        }
                    }
                }
            }
        }
    }
#if CAP_DEBUG
    cap_print_proc_db(12, p_cap_db, cause);
#endif
    return cause;
}

T_LE_AUDIO_CAUSE cap_vcs_change_volume(T_BLE_AUDIO_GROUP_HANDLE group_handle,
                                       uint8_t volume_setting)
{
    T_LE_AUDIO_CAUSE cause;
    T_LE_AUDIO_GROUP *p_group = (T_LE_AUDIO_GROUP *)group_handle;
    T_CAP_PROC_DB *p_cap_db = NULL;
    cause = cap_check_group_state(group_handle, false);
    if (cause == LE_AUDIO_CAUSE_SUCCESS)
    {
        p_cap_db = cap_find_or_alloc_proc_cb(p_group);

        if (vcs_check_group_data(p_group, VCS_CHECK_PREFER_VOLUME, volume_setting))
        {
            return cause;
        }
        cause = cap_vcs_change_volume_int(p_group, p_cap_db, volume_setting);
    }
    PROTOCOL_PRINT_INFO3("[CAP][VCP] cap_vcs_change_volume: group_handle %p, volume_setting %d, cause 0x%x",
                         group_handle, volume_setting, cause);
    return cause;
}
#endif

#if LE_AUDIO_MICS_CLIENT_SUPPORT
T_LE_AUDIO_CAUSE cap_mics_change_mute_int(T_LE_AUDIO_GROUP *p_group, T_CAP_PROC_DB *p_cap_db,
                                          T_MICS_MUTE mic_mute)
{
    T_LE_AUDIO_CAUSE cause = LE_AUDIO_CAUSE_SUCCESS;
    T_LE_AUDIO_DEV *p_set_member;

    if (p_cap_db)
    {
        p_cap_db->param_set_mask |= VCS_SET_MIC_MUTE;
        p_cap_db->prefer_mic_mute = mic_mute;
    }
    if (p_group->p_csip_cb != NULL &&
        (p_group->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_LOCK ||
         p_group->p_csip_cb->lock_state == SET_COORDINATOR_WAIT_FOR_UNLOCK ||
         p_group->p_csip_cb->lock_state == SET_COORDINATOR_LOCK_UNKNOWN ||
         p_group->p_csip_cb->lock_state == SET_COORDINATOR_LOCK_DENIED))
    {
        return cause;
    }
#if CAP_DEBUG
    cap_print_proc_db(1, p_cap_db, 0);
#endif
    for (int i = 0; i < p_group->dev_queue.count; i++)
    {
        p_set_member = (T_LE_AUDIO_DEV *)os_queue_peek(&p_group->dev_queue, i);
        if (p_set_member != NULL && p_set_member->p_link &&
            p_set_member->p_link->state == GAP_CONN_STATE_CONNECTED)
        {
            if (p_cap_db && (p_cap_db->mic_update_mask & (1 << (p_set_member->p_link->id))) != 0)
            {
                continue;
            }
            if ((p_set_member->p_link->cap_discov_flags & CAP_MICP_FLAG) == 0)
            {
                continue;
            }
            T_MICS_MUTE curr_mute = MICS_DISABLED;
            if (mics_get_mute_value(p_set_member->p_link->conn_handle, &curr_mute))
            {
                if (curr_mute == MICS_DISABLED)
                {
                    PROTOCOL_PRINT_ERROR1("[CAP][VCP] cap_change_csis_mic_mute: conn_handle 0x%x the remote mute is disabled, skip this member",
                                          p_set_member->p_link->conn_handle);
                    continue;
                }
                if (curr_mute != mic_mute)
                {
                    if (mics_write_mute_value(p_set_member->p_link->conn_handle, (T_MICS_MUTE)mic_mute))
                    {
                        if (p_cap_db)
                        {
                            p_cap_db->mic_update_mask |= (1 << (p_set_member->p_link->id));
                        }
                    }

                }
            }
        }
    }
#if CAP_DEBUG
    cap_print_proc_db(2, p_cap_db, cause);
#endif
    return cause;
}

T_LE_AUDIO_CAUSE cap_mics_change_mute(T_BLE_AUDIO_GROUP_HANDLE group_handle, T_MICS_MUTE mic_mute)
{
    T_LE_AUDIO_CAUSE cause;
    T_LE_AUDIO_GROUP *p_group = (T_LE_AUDIO_GROUP *)group_handle;
    T_CAP_PROC_DB *p_cap_db = NULL;
    cause = cap_check_group_state(group_handle, false);
    if (cause == LE_AUDIO_CAUSE_SUCCESS)
    {
        p_cap_db = cap_find_or_alloc_proc_cb(p_group);
        if (mics_check_group_data(p_group, MICS_CHECK_PREFER_MUTE, mic_mute))
        {
            return cause;
        }
        cause = cap_mics_change_mute_int(p_group, p_cap_db, mic_mute);
    }
    PROTOCOL_PRINT_INFO3("[CAP][VCP] cap_mics_change_mute: group_handle %p, mic_mute %d, cause 0x%x",
                         group_handle, mic_mute, cause);
    return cause;
}
#endif
#endif

#if (LE_AUDIO_CAS_CLIENT_SUPPORT || LE_AUDIO_VCS_CLIENT_SUPPORT ||LE_AUDIO_MICS_CLIENT_SUPPORT)
static void cap_check_disc_done(T_BLE_AUDIO_LINK *p_link, uint16_t conn_handle, uint8_t flags)
{
    PROTOCOL_PRINT_INFO2("[CAP][VCP] cap_check_disc_done: cap_init_flags 0x%x, flags 0x%x",
                         p_link->cap_init_flags, flags);
    if (p_link->cap_init_flags & flags)
    {
        p_link->cap_init_flags &= ~flags;
        if (p_link->cap_init_flags == 0)
        {
            T_CAP_DIS_DONE dis_done = {0};

            dis_done.conn_handle = conn_handle;
            dis_done.load_from_ftl = gatt_client_is_load_from_ftl(conn_handle);
            if (p_link->cap_discov_flags & CAP_CAS_FLAG)
            {
                dis_done.cas_is_found = true;
                if (p_link->cap_discov_flags & CAP_CSIS_FLAG)
                {
                    dis_done.cas_inc_csis = true;
                }
            }
            if (p_link->cap_discov_flags & CAP_VCP_FLAG)
            {
                dis_done.vcs_is_found = true;
            }
            if (p_link->cap_discov_flags & CAP_MICP_FLAG)
            {
                dis_done.mics_is_found = true;
            }
            ble_audio_mgr_dispatch(LE_AUDIO_MSG_CAP_DIS_DONE, &dis_done);
        }
    }
}
#endif

#if LE_AUDIO_CSIS_CLIENT_SUPPORT
#if LE_AUDIO_VCS_CLIENT_SUPPORT
void cap_check_vcs_data(T_LE_AUDIO_GROUP *p_group, T_CAP_PROC_DB *p_cap_db, bool set_volume,
                        bool set_mute)
{
    PROTOCOL_PRINT_INFO3("[CAP][VCP] cap_check_vcs_data: group_handle %d, set_volume %d, set_mute %d",
                         p_group, set_volume, set_mute);
    if (set_volume == true && (p_cap_db->param_set_mask & VCS_SET_VOLUME))
    {
        cap_vcs_change_volume_int(p_group, p_cap_db, p_cap_db->prefer_volume);
    }
    if (set_mute == true && (p_cap_db->param_set_mask & VCS_SET_MUTE))
    {
        cap_vcs_change_mute_int(p_group, p_cap_db, p_cap_db->prefer_mute);
    }
}
#endif

#if (LE_AUDIO_VCS_CLIENT_SUPPORT || LE_AUDIO_MICS_CLIENT_SUPPORT || LE_AUDIO_CSIS_CLIENT_SUPPORT)
void cap_handle_new_dev(T_LE_AUDIO_GROUP *p_group, T_CAP_PROC_DB *p_cap_db,
                        T_BLE_AUDIO_LINK *p_link, bool is_vcs, bool is_mics)
{
#if CAP_DEBUG
    cap_print_proc_db(2, p_cap_db, 0);
#endif
    if (p_group->conn_dev_num == 1)
    {
#if LE_AUDIO_VCS_CLIENT_SUPPORT
        if (is_vcs)
        {
            T_VOLUME_STATE data;
            if (vcs_get_volume_state(p_link->conn_handle, &data) == false)
            {
                return;
            }
            if ((p_cap_db->param_set_mask & VCS_SET_VOLUME) == 0)
            {
                p_cap_db->param_set_mask |= VCS_SET_VOLUME;
                p_cap_db->prefer_volume = data.volume_setting;
            }
            if ((p_cap_db->param_set_mask & VCS_SET_MUTE) == 0)
            {
                p_cap_db->param_set_mask |= VCS_SET_MUTE;
                p_cap_db->prefer_mute = data.mute;
            }
        }
#endif
#if LE_AUDIO_MICS_CLIENT_SUPPORT
        if (is_mics)
        {
            if ((p_cap_db->param_set_mask & VCS_SET_MIC_MUTE) == 0)
            {
                T_MICS_MUTE mics_mute;
                if (mics_get_mute_value(p_link->conn_handle, &mics_mute))
                {
                    p_cap_db->param_set_mask |= VCS_SET_MIC_MUTE;
                    p_cap_db->prefer_mic_mute = mics_mute;
                }
            }
        }
#endif
    }
    else if (p_group->conn_dev_num > 1)
    {
#if LE_AUDIO_VCS_CLIENT_SUPPORT
        if (is_vcs)
        {
            bool volume_is_equal = vcs_check_group_data(p_group, VCS_CHECK_VOLUME_IS_EQUAL, 0);
            bool mute_is_equal = vcs_check_group_data(p_group, VCS_CHECK_MUTE_IS_EQUAL, 0);
            if (volume_is_equal == false ||
                mute_is_equal == false)
            {
                cap_check_vcs_data(p_group, p_cap_db, !volume_is_equal, !mute_is_equal);
            }
        }
#endif
#if LE_AUDIO_MICS_CLIENT_SUPPORT
        if (is_mics)
        {
            if (mics_check_group_data(p_group, MICS_CHECK_MUTE_IS_EQUAL, 0) == false)
            {
                cap_mics_change_mute_int(p_group, p_cap_db, p_cap_db->prefer_mic_mute);
            }
        }
#endif
    }
#if CAP_DEBUG
    cap_print_proc_db(3, p_cap_db, 0);
#endif
}
#endif
#endif

#if (LE_AUDIO_BASS_CLIENT_SUPPORT || LE_AUDIO_PACS_CLIENT_SUPPORT || LE_AUDIO_ASCS_CLIENT_SUPPORT||LE_AUDIO_CSIS_CLIENT_SUPPORT ||LE_AUDIO_VCS_CLIENT_SUPPORT ||LE_AUDIO_MICS_CLIENT_SUPPORT)
uint16_t cap_msg_cb(uint16_t msg, void *buf, bool *p_handled)
{
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
    T_LE_AUDIO_GROUP *p_group = NULL;
    T_CAP_PROC_DB *p_cap_db = NULL;
    T_LE_AUDIO_DEV *p_set_member;
#endif

    if ((msg & LE_AUDIO_MSG_INTERNAL) == LE_AUDIO_MSG_INTERNAL)
    {
        *p_handled = true;
    }
#if (LE_AUDIO_BASS_CLIENT_SUPPORT || LE_AUDIO_PACS_CLIENT_SUPPORT || LE_AUDIO_ASCS_CLIENT_SUPPORT)
    uint16_t msg_group;
    msg_group = msg & 0xff00;
    if (msg_group == LE_AUDIO_MSG_GROUP_BASS_CLIENT ||
        msg_group == LE_AUDIO_MSG_GROUP_PACS_CLIENT ||
        msg_group == LE_AUDIO_MSG_GROUP_ASCS_CLIENT)
    {
        return bap_msg_cb(msg, buf, p_handled);
    }
#endif
    switch (msg)
    {
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
    case LE_AUDIO_MSG_CSIS_CLIENT_LOCK_STATE:
        {
            T_CSIS_CLIENT_LOCK_STATE *p_lock_state = (T_CSIS_CLIENT_LOCK_STATE *)buf;
            p_group = (T_LE_AUDIO_GROUP *)p_lock_state->group_handle;
            if (p_group != NULL && p_group->p_cap != NULL)
            {
                p_cap_db = p_group->p_cap;
                PROTOCOL_PRINT_INFO3("[CAP][VCP] LE_AUDIO_MSG_CSIS_CLIENT_LOCK_STATE: group_handle %p, event %d, lock_state 0x%x",
                                     p_group, p_lock_state->event, p_lock_state->lock_state);
                if (p_cap_db->param_set_mask != 0)
                {
                    if (p_lock_state->lock_state == SET_COORDINATOR_UNLOCK ||
                        p_lock_state->lock_state == SET_COORDINATOR_LOCK_GRANTED)
                    {
#if CAP_DEBUG
                        cap_print_proc_db(1, p_cap_db, 0);
#endif
#if LE_AUDIO_VCS_CLIENT_SUPPORT
                        cap_check_vcs_data(p_group, p_cap_db, true, true);
#endif
#if LE_AUDIO_MICS_CLIENT_SUPPORT
                        if (p_cap_db->param_set_mask & VCS_SET_MIC_MUTE)
                        {
                            cap_mics_change_mute_int(p_group, p_cap_db, p_cap_db->prefer_mic_mute);
                        }
#endif
                    }
                }
            }
        }
        break;
    case LE_AUDIO_MSG_CAP_SET_MEM_ADD:
        {
            T_CAP_SET_MEM_ADD *p_mem = (T_CAP_SET_MEM_ADD *)buf;
            PROTOCOL_PRINT_INFO3("[CAP][VCP] LE_AUDIO_MSG_CAP_SET_MEM_ADD: new_group %d, group_handle %p, dev_handle %p",
                                 p_mem->new_group, p_mem->group_handle, p_mem->dev_handle);
            p_group = (T_LE_AUDIO_GROUP *)p_mem->group_handle;
            p_set_member = (T_LE_AUDIO_DEV *)p_mem->dev_handle;
            if (p_mem->new_group)
            {
                cap_find_or_alloc_proc_cb(p_group);
            }
            if (p_group->p_cap)
            {
                if (p_set_member->p_link != NULL &&
                    p_set_member->p_link->state == GAP_CONN_STATE_CONNECTED)
                {
                    cap_handle_new_dev(p_group, p_group->p_cap, p_set_member->p_link, true, true);
                }
            }
        }
        break;
#endif
#if LE_AUDIO_VCS_CLIENT_SUPPORT
    case LE_AUDIO_MSG_VCS_CLIENT_DIS_DONE:
        {
            T_VCS_CLIENT_DIS_DONE *p_dis_done = (T_VCS_CLIENT_DIS_DONE *)buf;
            PROTOCOL_PRINT_INFO3("[CAP][VCP] LE_AUDIO_MSG_VCS_CLIENT_DIS_DONE: conn_handle 0x%x, is_found %d, type_exist 0x%x",
                                 p_dis_done->conn_handle, p_dis_done->is_found, p_dis_done->type_exist);
            T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(p_dis_done->conn_handle);
            if (p_link)
            {
                if (p_dis_done->is_found)
                {
                    p_link->cap_discov_flags |= CAP_VCP_FLAG;
                }
                cap_check_disc_done(p_link, p_dis_done->conn_handle, CAP_VCP_FLAG);
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
                if (p_dis_done->is_found)
                {
                    p_cap_db = cap_find_group(p_link, &p_group, &p_set_member);
                    if (p_cap_db == NULL)
                    {
                        break;
                    }
                    cap_handle_new_dev(p_group, p_cap_db, p_link, true, false);
                }
#endif
            }
        }
        break;

    case LE_AUDIO_MSG_VCS_CLIENT_VOLUME_STATE_DATA:
        {
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
            T_VCS_CLIENT_VOLUME_STATE_DATA *p_notify_data = (T_VCS_CLIENT_VOLUME_STATE_DATA *)buf;
            T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(p_notify_data->conn_handle);
            if (p_link == NULL)
            {
                break;
            }
            p_cap_db = cap_find_group(p_link, &p_group, &p_set_member);
            if (p_cap_db == NULL)
            {
                break;
            }
            if (p_notify_data->type == VCS_CHAR_VOLUME_STATE)
            {
                bool set_volume = false;
                bool set_mute = false;
#if CAP_DEBUG
                cap_print_proc_db(3, p_cap_db, 0);
#endif
                if ((p_cap_db->vcs_update_mask & (1 << (p_link->id))) != 0)
                {
                    p_cap_db->vcs_update_mask &= ~(1 << (p_link->id));
                    if (p_cap_db->vcs_update_mask == 0)
                    {
                        if (p_cap_db->vcs_op == VCS_CP_SET_ABSOLUTE_VOLUME)
                        {
                            if (vcs_check_group_data(p_group, VCS_CHECK_PREFER_VOLUME, p_cap_db->prefer_volume))
                            {
                                p_cap_db->vcs_op = VCS_CP_MAX;
                                set_mute = true;
                            }
                            else
                            {
                                set_volume = true;
                            }
                        }
                        else if (p_cap_db->vcs_op == VCS_CP_UNMUTE || p_cap_db->vcs_op == VCS_CP_MUTE)
                        {
                            if (vcs_check_group_data(p_group, VCS_CHECK_PREFER_MUTE, p_cap_db->prefer_mute))
                            {
                                p_cap_db->vcs_op = VCS_CP_MAX;
                                set_volume = true;
                            }
                            else
                            {
                                set_mute = true;
                            }
                        }
                    }
                }
                else
                {
                    T_VOLUME_STATE data;
                    if (vcs_get_volume_state(p_link->conn_handle, &data))
                    {
                        p_cap_db->param_set_mask |= VCS_SET_VOLUME;
                        p_cap_db->param_set_mask |= VCS_SET_MUTE;
                        p_cap_db->prefer_volume = data.volume_setting;
                        p_cap_db->prefer_mute = data.mute;
                    }
                    if (p_group->conn_dev_num > 1)
                    {
                        if (vcs_check_group_data(p_group, VCS_CHECK_VOLUME_IS_EQUAL, 0) == false)
                        {
                            set_volume = true;
                        }
                        if (vcs_check_group_data(p_group, VCS_CHECK_MUTE_IS_EQUAL, 0) == false)
                        {
                            set_mute = true;
                        }
                    }
                }
                if (set_volume || set_mute)
                {
                    cap_check_vcs_data(p_group, p_cap_db, set_volume, set_mute);
                }
#if CAP_DEBUG
                cap_print_proc_db(4, p_cap_db, 0);
#endif
            }
#endif
        }
        break;
#endif
#if LE_AUDIO_MICS_CLIENT_SUPPORT
    case LE_AUDIO_MSG_MICS_CLIENT_DIS_DONE:
        {
            T_MICS_CLIENT_DIS_DONE *p_dis_done = (T_MICS_CLIENT_DIS_DONE *)buf;
            PROTOCOL_PRINT_INFO4("[CAP][VCP] LE_AUDIO_MSG_MICS_CLIENT_DIS_DONE: conn_handle 0x%x, is_found %d, load_from_ftl 0x%x, mic_mute %d",
                                 p_dis_done->conn_handle, p_dis_done->is_found, p_dis_done->load_from_ftl, p_dis_done->mic_mute);
            T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(p_dis_done->conn_handle);
            if (p_link)
            {
                if (p_dis_done->is_found)
                {
                    p_link->cap_discov_flags |= CAP_MICP_FLAG;
                }
                cap_check_disc_done(p_link, p_dis_done->conn_handle, CAP_MICP_FLAG);
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
                if (p_dis_done->is_found)
                {
                    p_cap_db = cap_find_group(p_link, &p_group, &p_set_member);
                    if (p_cap_db == NULL)
                    {
                        break;
                    }
                    cap_handle_new_dev(p_group, p_cap_db, p_link, false, true);
                }
#endif
            }
        }
        break;

    case LE_AUDIO_MSG_MICS_CLIENT_NOTIFY:
        {
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
            T_MICS_CLIENT_NOTIFY *p_notify_data = (T_MICS_CLIENT_NOTIFY *)buf;
            bool set_mic_mute = false;
            T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(p_notify_data->conn_handle);
            if (p_link == NULL)
            {
                break;
            }
            p_cap_db = cap_find_group(p_link, &p_group, &p_set_member);
            if (p_cap_db == NULL)
            {
                break;
            }
#if CAP_DEBUG
            cap_print_proc_db(7, p_cap_db, 0);
#endif

            if ((p_cap_db->mic_update_mask & (1 << (p_link->id))) != 0)
            {
                p_cap_db->mic_update_mask &= ~(1 << (p_link->id));
                if (p_cap_db->mic_update_mask == 0)
                {
                    if (mics_check_group_data(p_group, MICS_CHECK_PREFER_MUTE, p_cap_db->prefer_mic_mute) == false)
                    {
                        set_mic_mute = true;
                    }
                }
            }
            else
            {
                p_cap_db->param_set_mask |= VCS_SET_MIC_MUTE;
                p_cap_db->prefer_mic_mute = p_notify_data->mic_mute;
                if (p_group->conn_dev_num > 1)
                {
                    if (mics_check_group_data(p_group, MICS_CHECK_MUTE_IS_EQUAL, 0) == false)
                    {
                        set_mic_mute = true;
                    }
                }
            }
            if (set_mic_mute)
            {
                cap_mics_change_mute_int(p_group, p_cap_db, p_cap_db->prefer_mic_mute);
            }

#if CAP_DEBUG
            cap_print_proc_db(8, p_cap_db, 0);
#endif
#endif
        }
        break;
#endif
    default:
        break;

    }
    return cb_result;
}
#endif

#if LE_AUDIO_CAS_CLIENT_SUPPORT
static T_APP_RESULT cap_cas_callback(uint16_t conn_handle, T_GATT_CLIENT_EVENT type, void *p_data)
{
    T_APP_RESULT  result = APP_RESULT_SUCCESS;
    T_GATT_CLIENT_DATA *p_client_cb_data = (T_GATT_CLIENT_DATA *)p_data;

    switch (type)
    {
    case GATT_CLIENT_EVENT_DIS_DONE:
        {
            T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);
            if (p_link == NULL)
            {
                return APP_RESULT_APP_ERR;
            }
            if (p_client_cb_data->dis_done.is_found)
            {
                p_link->cap_discov_flags |= CAP_CAS_FLAG;
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
                T_ATTR_UUID srv_uuid;
                T_ATTR_UUID inc_uuid;
                T_ATTR_INSTANCE attr_instance;

                srv_uuid.is_uuid16 = true;
                srv_uuid.instance_id = 0;
                srv_uuid.p.uuid16 = GATT_UUID_CAS;
                inc_uuid.is_uuid16 = true;
                inc_uuid.instance_id = 0;
                inc_uuid.p.uuid16 = GATT_UUID_CSIS;
                if (gatt_client_find_include_srv_by_primary(conn_handle, &srv_uuid, &inc_uuid, &attr_instance))
                {
                    if (attr_instance.instance_num == 1)
                    {
                        PROTOCOL_PRINT_INFO0("[CAP][VCP] cap_cas_callback: support csis");
                        p_link->cap_discov_flags |= CAP_CSIS_FLAG;
                        set_coordinator_read_chars(conn_handle, attr_instance.instance_id[0]);

                    }
                }
#endif
            }
            cap_check_disc_done(p_link, conn_handle, CAP_CAS_FLAG);
        }
        break;

    default:
        break;
    }

    return result;
}
#endif

static void cap_dm_cback(T_BLE_DM_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BLE_DM_EVENT_PARAM *param = (T_BLE_DM_EVENT_PARAM *)event_buf;

    switch (event_type)
    {
    case BLE_DM_EVENT_CONN_STATE:
        {
            T_BLE_AUDIO_LINK *p_link = NULL;
            p_link = ble_audio_link_find_by_conn_id(param->conn_state.conn_id);
            if (p_link == NULL)
            {
                PROTOCOL_PRINT_ERROR1("[CAP][VCP] cap_dm_cback: not find the link, conn_id %d",
                                      param->conn_state.conn_id);
                break;
            }
            if (param->conn_state.state == GAP_CONN_STATE_CONNECTED)
            {
                p_link->cap_init_flags = cap_db.cap_init_flags;
            }
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
            else if (param->conn_state.state == GAP_CONN_STATE_DISCONNECTED)
            {
                T_LE_AUDIO_GROUP *p_audio_group;
                T_LE_AUDIO_DEV *p_dev;

                for (uint8_t i = 0; i < audio_group_queue.count; i++)
                {
                    p_audio_group = os_queue_peek(&audio_group_queue, i);
                    p_dev = ble_audio_group_find_dev(p_audio_group, p_link->remote_bd, p_link->remote_bd_type);
                    if (p_dev)
                    {
                        T_CAP_PROC_DB *p_cap_db = p_audio_group->p_cap;
                        if (p_cap_db)
                        {
#if LE_AUDIO_VCS_CLIENT_SUPPORT
                            p_cap_db->vcs_update_mask &= ~(1 << (p_link->id));
                            if (p_cap_db->vcs_update_mask == 0)
                            {
                                p_cap_db->vcs_op = VCS_CP_MAX;
                            }
#endif
#if LE_AUDIO_MICS_CLIENT_SUPPORT
                            p_cap_db->mic_update_mask &= ~(1 << (p_link->id));
#endif
#if CAP_DEBUG
                            cap_print_proc_db(10, p_cap_db, 0);
#endif
                            if (p_audio_group->conn_dev_num == 0)
                            {
                                ble_audio_mem_free(p_cap_db);
                                p_audio_group->p_cap = NULL;
                            }
                        }

                    }
                }
            }
#endif
        }
        break;

    default:
        break;
    }
}

bool cap_init(T_CAP_INIT_PARAMS *p_param)
{
    uint8_t err_idx = 0;
    if (p_param == NULL)
    {
        err_idx = 1;
        goto failed;
    }

    PROTOCOL_PRINT_INFO2("[CAP][VCP] cap_init: cap_role 0x%x, %b",
                         p_param->cap_role,
                         TRACE_BINARY(sizeof(T_CAP_INIT_PARAMS), p_param));

    memset((uint8_t *)&cap_db, 0, sizeof(T_CAP_DB));

#if LE_AUDIO_CAS_CLIENT_SUPPORT
    if (p_param->cas_client)
    {
        T_ATTR_UUID srv_uuid = {0};
        srv_uuid.is_uuid16 = true;
        srv_uuid.p.uuid16 = GATT_UUID_CAS;
        cap_db.cap_init_flags |= CAP_CAS_FLAG;
        gatt_client_spec_register(&srv_uuid, cap_cas_callback);
    }
#endif
#if LE_AUDIO_CSIS_CLIENT_SUPPORT
    if (p_param->csip_set_coordinator)
    {
        if (set_coordinator_init() == false)
        {
            err_idx = 2;
            goto failed;
        }
    }
#endif
#if LE_AUDIO_CSIS_SUPPORT
    if (p_param->csis_num)
    {
        if (csis_init(p_param->csis_num) == false)
        {
            err_idx = 2;
            goto failed;
        }
    }
#endif
#if LE_AUDIO_CAS_SUPPORT
    if (p_param->cas.enable)
    {
        if (cas_init(p_param->cas.csis_sirk, p_param->cas.csis_sirk_type, p_param->cas.csis_size,
                     p_param->cas.csis_rank, p_param->cas.csis_feature) == false)
        {
            err_idx = 3;
            goto failed;
        }
    }
#endif
#if LE_AUDIO_VCS_CLIENT_SUPPORT
    if (p_param->vcp_micp.vcp_vcs_client)
    {
        if (vcs_client_init() == false)
        {
            err_idx = 4;
            goto failed;
        }
        cap_db.cap_init_flags |= CAP_VCP_FLAG;
    }
#endif
#if LE_AUDIO_VOCS_CLIENT_SUPPORT
    if (p_param->vcp_micp.vcp_vocs_client)
    {
        if (vocs_client_init(p_param->vcp_micp.vcp_vocs_cfg) == false)
        {
            err_idx = 5;
            goto failed;
        }
    }
#endif
#if LE_AUDIO_AICS_CLIENT_SUPPORT
    if (p_param->vcp_micp.vcp_aics_client)
    {
        if (aics_client_init(p_param->vcp_micp.vcp_aics_cfg) == false)
        {
            err_idx = 6;
            goto failed;
        }
    }
#endif
#if LE_AUDIO_MICS_CLIENT_SUPPORT
    if (p_param->vcp_micp.micp_mic_controller)
    {
        if (mics_client_init() == false)
        {
            err_idx = 7;
            goto failed;
        }
        cap_db.cap_init_flags |= CAP_MICP_FLAG;
    }
#endif
#if LE_AUDIO_CCP_CLIENT_SUPPORT
    if (p_param->ccp_call_control_client)
    {
        if (ccp_client_init() == false)
        {
            err_idx = 8;
            goto failed;
        }
    }
#endif
#if LE_AUDIO_CCP_SERVER_SUPPORT
    if (p_param->tbs.ccp_call_control_server)
    {
        if (ccp_server_init(p_param->tbs.tbs_num) == false)
        {
            err_idx = 9;
            goto failed;
        }
    }
#endif
#if LE_AUDIO_MCP_CLIENT_SUPPORT
    if (p_param->mcp_media_control_client)
    {
        if (mcs_client_init() == false)
        {
            err_idx = 10;
            goto failed;
        }
#if LE_AUDIO_OTS_CLIENT_SUPPORT
        if (ots_client_init() == false)
        {
            err_idx = 11;
            goto failed;
        }
#endif
    }
#endif
#if LE_AUDIO_MCP_SERVER_SUPPORT
    if (p_param->mcs.mcp_media_control_server)
    {
        if (p_param->mcs.mcs_num)
        {
            if (mcs_server_init(p_param->mcs.mcs_num) == false)
            {
                err_idx = 12;
                goto failed;
            }
        }
#if LE_AUDIO_OTS_SERV_SUPPORT
        if (p_param->mcs.ots_num)
        {
            if (ots_server_init(p_param->mcs.ots_num) == false)
            {
                err_idx = 13;
                goto failed;
            }
        }
#endif
    }
#endif
#if (LE_AUDIO_BASS_CLIENT_SUPPORT || LE_AUDIO_PACS_CLIENT_SUPPORT || LE_AUDIO_ASCS_CLIENT_SUPPORT||LE_AUDIO_CSIS_CLIENT_SUPPORT ||LE_AUDIO_VCS_CLIENT_SUPPORT ||LE_AUDIO_MICS_CLIENT_SUPPORT)
    ble_audio_reg_cap_cb(cap_msg_cb);
#endif
    ble_dm_cback_register(cap_dm_cback);
    cap_db.cap_role = p_param->cap_role;
#if LE_AUDIO_INIT_DEBUG
    PROTOCOL_PRINT_INFO1("[CAP][VCP] cap_init: cap_init_flags 0x%x", cap_db.cap_init_flags);
#endif
    return true;
failed:
    PROTOCOL_PRINT_ERROR1("[CAP][VCP] cap_init: failed, err_idx %d", err_idx);
    return false;
}

#if LE_AUDIO_DEINIT
void cap_deinit(void)
{
    memset(&cap_db, 0, sizeof(cap_db));
}
#endif
#endif

