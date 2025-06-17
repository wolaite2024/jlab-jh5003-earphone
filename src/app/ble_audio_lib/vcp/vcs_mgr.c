#include "trace.h"
#include "vocs_mgr.h"
#include "bt_types.h"
#include "vcs_def.h"
#include "codec_def.h"
#include "ble_audio_mgr.h"
#include "ble_link_util.h"
#include "vcs.h"
#include "vcs_mgr.h"

#if LE_AUDIO_VCS_SUPPORT
T_MGR_VCS *p_mgr_vcs = NULL;

static bool vcs_send_volume_state_notify_all(uint16_t cid)
{
    uint8_t data[3];

    data[0] = p_mgr_vcs->volume_state.volume_setting;
    data[1] = p_mgr_vcs->volume_state.mute;
    data[2] = p_mgr_vcs->volume_state.change_counter;

    ble_audio_send_notify_all(cid, p_mgr_vcs->service_id,
                              (VCS_CHAR_VOLUME_STATE_INDEX + p_mgr_vcs->char_attr_offset),
                              data, 3);

    return true;
}

static bool vcs_send_volume_flags_notify_all(uint16_t cid)
{
    ble_audio_send_notify_all(cid, p_mgr_vcs->service_id,
                              (VCS_CHAR_VOLUME_FLAGS_INDEX + p_mgr_vcs->char_attr_offset),
                              &p_mgr_vcs->volume_flags, sizeof(uint8_t));
    return true;
}

static T_APP_RESULT vcs_handle_volume_cp(uint16_t conn_handle, uint8_t opcode,
                                         uint8_t volume_setting)
{
    T_VCS_VOLUME_CP_IND volume_state;
    p_mgr_vcs->write_flags = 0;
    T_APP_RESULT app_result = APP_RESULT_SUCCESS;
    volume_state.conn_handle = conn_handle;
    volume_state.volume_setting = p_mgr_vcs->volume_state.volume_setting;
    volume_state.mute = p_mgr_vcs->volume_state.mute;
    volume_state.cp_op = (T_VCS_CP_OP)opcode;

    PROTOCOL_PRINT_INFO1("vcs_handle_volume_cp: opcode 0x%x", opcode);
    switch (opcode)
    {
    case VCS_CP_RELATIVE_VOLUME_DOWN:
    case VCS_CP_UNMUTE_RELATIVE_VOLUME_DOWN:
        if (volume_state.volume_setting >= p_mgr_vcs->step_size)
        {
            volume_state.volume_setting -= p_mgr_vcs->step_size;
        }
        else
        {
            volume_state.volume_setting = 0;
        }
        if (opcode == VCS_CP_UNMUTE_RELATIVE_VOLUME_DOWN)
        {
            volume_state.mute = VCS_NOT_MUTED;
        }
        break;

    case VCS_CP_RELATIVE_VOLUME_UP:
    case VCS_CP_UNMUTE_RELATIVE_VOLUME_UP:
        {
            volume_state.volume_setting = ((volume_state.volume_setting + p_mgr_vcs->step_size) <
                                           VCS_VOLUME_SETTING_MAX) ?
                                          (volume_state.volume_setting + p_mgr_vcs->step_size) : VCS_VOLUME_SETTING_MAX;

            if (opcode == VCS_CP_UNMUTE_RELATIVE_VOLUME_UP)
            {
                volume_state.mute = VCS_NOT_MUTED;
            }
        }
        break;

    case VCS_CP_SET_ABSOLUTE_VOLUME:
        volume_state.volume_setting = volume_setting;
        break;

    case VCS_CP_UNMUTE:
        volume_state.mute = VCS_NOT_MUTED;
        break;

    case VCS_CP_MUTE:
        volume_state.mute = VCS_MUTED;
        break;

    default:
        break;
    }

    app_result = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_VCS_VOLUME_CP_IND,
                                                      &volume_state);

    if (app_result != APP_RESULT_SUCCESS)
    {
        return app_result;
    }

    if (p_mgr_vcs->volume_state.volume_setting != volume_state.volume_setting)
    {
        p_mgr_vcs->write_flags |= VCS_WRITE_FLAGS_VOLUME_SETTING;
        if ((p_mgr_vcs->volume_flags & VCS_VOLUME_SETTING_PERSISTED_FLAG) == VCS_RESET_VOLUME_SETTING)
        {
            p_mgr_vcs->volume_flags |= VCS_USER_SET_VOLUME_SETTING;
            p_mgr_vcs->write_flags |= VCS_WRITE_FLAGS_VOLUME_FLAGS;
        }
    }
    if (p_mgr_vcs->volume_state.mute != volume_state.mute)
    {
        p_mgr_vcs->write_flags |= VCS_WRITE_FLAGS_MUTE;
    }
    if (p_mgr_vcs->write_flags != 0)
    {
        p_mgr_vcs->volume_state.volume_setting = volume_state.volume_setting;
        p_mgr_vcs->volume_state.mute = volume_state.mute;
        p_mgr_vcs->volume_state.change_counter++;
    }

    return app_result;
}


bool vcs_set_param(T_VCS_PARAM *p_param)
{
    if (p_param == NULL || p_mgr_vcs == NULL)
    {
        return false;
    }

    if (p_mgr_vcs->param_init == false)
    {
        p_mgr_vcs->volume_state.change_counter = p_param->change_counter;
        p_mgr_vcs->volume_state.volume_setting = p_param->volume_setting;
        p_mgr_vcs->volume_state.mute = p_param->mute;
        p_mgr_vcs->step_size = p_param->step_size;
        p_mgr_vcs->volume_flags = p_param->volume_flags;
        p_mgr_vcs->param_init = true;
        vcs_send_volume_state_notify_all(L2C_FIXED_CID_ATT);
        vcs_send_volume_flags_notify_all(L2C_FIXED_CID_ATT);
    }
    else
    {
        if (p_mgr_vcs->volume_state.mute != p_param->mute ||
            p_mgr_vcs->volume_state.volume_setting != p_param->volume_setting)
        {
            p_mgr_vcs->volume_state.change_counter++;
            p_mgr_vcs->volume_state.volume_setting = p_param->volume_setting;
            p_mgr_vcs->volume_state.mute = p_param->mute;
            p_mgr_vcs->step_size = p_param->step_size;
            vcs_send_volume_state_notify_all(L2C_FIXED_CID_ATT);
        }

        if (p_mgr_vcs->volume_flags != p_param->volume_flags)
        {
            p_mgr_vcs->volume_flags = p_param->volume_flags;
            vcs_send_volume_flags_notify_all(L2C_FIXED_CID_ATT);
        }
    }

    PROTOCOL_PRINT_INFO5("vcs_set_param: volume_setting 0x%01x, mute 0x%01x, change_counter 0x%01x, volume_flags 0x%01x, step_size 0x%01x",
                         p_mgr_vcs->volume_state.volume_setting,
                         p_mgr_vcs->volume_state.mute,
                         p_mgr_vcs->volume_state.change_counter,
                         p_mgr_vcs->volume_flags,
                         p_mgr_vcs->step_size);
    return true;
}

static void vcs_write_post_proc(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                uint16_t attrib_index, uint16_t length,
                                uint8_t *p_value)
{
    //FIX TODO change notify may should set by audio app
    if (attrib_index == (p_mgr_vcs->char_attr_offset + VCS_CHAR_VOLUME_CP_INDEX))
    {
        if ((p_mgr_vcs->write_flags & VCS_WRITE_FLAGS_MUTE) ||
            (p_mgr_vcs->write_flags & VCS_WRITE_FLAGS_VOLUME_SETTING))
        {
            vcs_send_volume_state_notify_all(cid);
        }
        if (p_mgr_vcs->write_flags & VCS_WRITE_FLAGS_VOLUME_FLAGS)
        {
            vcs_send_volume_flags_notify_all(cid);
        }
        p_mgr_vcs->write_flags = 0;
    }
}

bool vcs_get_param(T_VCS_PARAM *p_param)
{
    if (p_param == NULL || p_mgr_vcs == NULL)
    {
        return false;
    }
    p_param->volume_setting = p_mgr_vcs->volume_state.volume_setting;
    p_param->mute = p_mgr_vcs->volume_state.mute;
    p_param->change_counter = p_mgr_vcs->volume_state.change_counter;
    p_param->volume_flags = p_mgr_vcs->volume_flags;
    p_param->step_size = p_mgr_vcs->step_size;
    return true;
}

T_APP_RESULT vcs_attr_read_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                              uint16_t attrib_index,
                              uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;

    PROTOCOL_PRINT_INFO2("vcs_attr_read_cb: attrib_index %d, offset %x", attrib_index, offset);
    attrib_index -= p_mgr_vcs->char_attr_offset;

    switch (attrib_index)
    {
    case VCS_CHAR_VOLUME_STATE_INDEX:
        {
            uint8_t volume_state[3];
            volume_state[0] = p_mgr_vcs->volume_state.volume_setting;
            volume_state[1] = p_mgr_vcs->volume_state.mute;
            volume_state[2] = p_mgr_vcs->volume_state.change_counter;
            if (gatt_svc_read_confirm(conn_handle, cid, service_id,
                                      attrib_index,
                                      volume_state, 3, 3, APP_RESULT_SUCCESS))
            {
                cause = APP_RESULT_PENDING;
            }
        }
        break;

    case VCS_CHAR_VOLUME_FLAGS_INDEX:
        {
            if (gatt_svc_read_confirm(conn_handle, cid, service_id,
                                      attrib_index,
                                      &p_mgr_vcs->volume_flags, 1, 1, APP_RESULT_SUCCESS))
            {
                cause = APP_RESULT_PENDING;
            }
        }
        break;

    default:
        {
            PROTOCOL_PRINT_ERROR1("vcs_attr_read_cb: attrib_index %d not found", attrib_index);
            return APP_RESULT_ATTR_NOT_FOUND;
        }
    }
    return (cause);
}

T_APP_RESULT vcs_attr_write_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                               uint16_t attrib_index,
                               T_WRITE_TYPE write_type,
                               uint16_t length, uint8_t *p_value, P_FUN_EXT_WRITE_IND_POST_PROC *p_write_post_proc)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;

    PROTOCOL_PRINT_INFO2("vcs_attr_write_cb: attrib_index %d, length %x", attrib_index, length);

    attrib_index -= p_mgr_vcs->char_attr_offset;

    if (attrib_index == VCS_CHAR_VOLUME_CP_INDEX)
    {
        uint8_t opcode;
        uint8_t change_counter;
        uint8_t volume_setting = 0;

        if (length < 2)
        {
            return APP_RESULT_INVALID_VALUE_SIZE;
        }
        opcode = p_value[0];
        change_counter = p_value[1];
        if (opcode > VCS_CP_MUTE)
        {
            return (T_APP_RESULT)ATT_ERR_VCS_OPCODE_NOT_SUPPORT;
        }

        if ((opcode == VCS_CP_SET_ABSOLUTE_VOLUME && length != 3)
            || (opcode != VCS_CP_SET_ABSOLUTE_VOLUME && length != 2))
        {
            return APP_RESULT_INVALID_VALUE_SIZE;
        }

        if (opcode == VCS_CP_SET_ABSOLUTE_VOLUME)
        {
            volume_setting = p_value[2];
        }

        if (change_counter != p_mgr_vcs->volume_state.change_counter)
        {
            return (T_APP_RESULT)ATT_ERR_VCS_INVALID_CHANGE_COUNTER;
        }

        cause = vcs_handle_volume_cp(conn_handle, opcode,
                                     volume_setting);

        if (cause == APP_RESULT_SUCCESS)
        {
            *p_write_post_proc = vcs_write_post_proc;
        }
    }
    else
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    return (cause);
}

void vcs_cccd_update_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                        uint16_t attrib_index,
                        uint16_t ccc_bits)
{
    PROTOCOL_PRINT_INFO2("vcs_cccd_update_cb: attrib_index %d, ccc_bits 0x%x", attrib_index, ccc_bits);
    ble_audio_mgr_send_server_cccd_info(conn_handle, p_mgr_vcs->p_vcs_attr_tbl, service_id,
                                        attrib_index,
                                        ccc_bits, 0, p_mgr_vcs->attr_num);
}

const T_FUN_GATT_EXT_SERVICE_CBS vcs_cbs =
{
    vcs_attr_read_cb,  // Read callback function pointer
    vcs_attr_write_cb, // Write callback function pointer
    vcs_cccd_update_cb  // CCCD update callback function pointer
};

bool vcs_init(uint8_t vocs_num, uint8_t aics_num, uint8_t *id_array)
{
    p_mgr_vcs = ble_audio_mem_zalloc(sizeof(T_MGR_VCS));

    if (p_mgr_vcs == NULL)
    {
        goto error;
    }
    p_mgr_vcs->service_id = vcs_add_service(&vcs_cbs, vocs_num, aics_num, id_array);

    if (p_mgr_vcs->service_id == 0xFF)
    {
        goto error;
    }
    return true;
error:
    if (p_mgr_vcs != NULL)
    {
        ble_audio_mem_free(p_mgr_vcs);
        p_mgr_vcs = NULL;
    }
    return false;
}

#if LE_AUDIO_DEINIT
void vcs_deinit(void)
{
    if (p_mgr_vcs != NULL)
    {
        if (p_mgr_vcs->p_vcs_attr_tbl)
        {
            ble_audio_mem_free(p_mgr_vcs->p_vcs_attr_tbl);
        }
        ble_audio_mem_free(p_mgr_vcs);
        p_mgr_vcs = NULL;
    }
}
#endif
#endif
