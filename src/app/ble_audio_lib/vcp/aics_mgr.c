#include <string.h>
#include "trace.h"
#include "ble_audio_mgr.h"
#include "ble_audio_dm.h"
#include "ble_link_util.h"
#include "aics.h"
#include "ble_audio_mgr.h"
#include "aics_mgr.h"

#if LE_AUDIO_AICS_SUPPORT
typedef struct
{
    uint8_t idx;
    T_SERVER_ID service_id;
    T_AICS_INPUT_STATE input_state;
    T_AICS_GAIN_SETTING_PROP gain_prop;
    T_AUDIO_INPUT_TYPE input_type;
    uint8_t input_status;
    uint16_t input_desc_len;
    uint8_t *input_desc;
} T_AICS;

static T_AICS *p_aics;
static uint8_t aics_srv_num = 0;

static T_AICS *aics_find(T_SERVER_ID service_id)
{
    uint8_t  i;

    if (p_aics == NULL)
    {
        return NULL;
    }

    for (i = 0; i < aics_srv_num; i++)
    {
        if (p_aics[i].service_id == service_id)
        {
            return (&p_aics[i]);
        }
    }

    PROTOCOL_PRINT_ERROR1("[AICS]aics_find: failed, service_id 0x%x", service_id);

    return NULL;
}


static T_AICS *aics_find_by_idx(uint8_t srv_instance_id)
{
    uint8_t  i;

    if (p_aics == NULL)
    {
        return NULL;
    }

    for (i = 0; i < aics_srv_num; i++)
    {
        if (p_aics[i].idx == srv_instance_id)
        {
            return (&p_aics[i]);
        }
    }

    PROTOCOL_PRINT_ERROR1("[AICS]aics_find_by_idx: failed, srv_instance_id 0x%x", srv_instance_id);

    return NULL;
}

static bool aics_send_input_state_notify_all(uint8_t service_id, T_AICS_INPUT_STATE *input_state)
{
    ble_audio_send_notify_all(L2C_FIXED_CID_ATT, service_id, AICS_CHAR_INPUT_STATE_INDEX,
                              (uint8_t *)input_state, sizeof(T_AICS_INPUT_STATE));
    return true;
}

static bool aics_send_input_status_notify_all(uint8_t service_id, uint8_t input_status)
{
    ble_audio_send_notify_all(L2C_FIXED_CID_ATT, service_id, AICS_CHAR_INPUT_STATUS_INDEX,
                              &input_status, 1);
    return true;
}

static bool aics_send_input_desc_notify_all(uint8_t service_id)
{
    T_AICS *p_entry;

    p_entry = aics_find(service_id);
    if (p_entry == NULL)
    {
        return false;
    }

    ble_audio_send_notify_all(L2C_FIXED_CID_ATT, service_id, AICS_CHAR_AUDIO_INPUT_DES_INDEX,
                              p_entry->input_desc, p_entry->input_desc_len);
    return true;
}


bool aics_set_param(uint8_t srv_instance_id, T_AICS_PARAM_TYPE param_type, uint8_t value_len,
                    uint8_t *p_value, bool set_change_counter)
{
    T_AICS *p_entry;
    bool ret = true;

    p_entry = aics_find_by_idx(srv_instance_id);
    if (p_entry == NULL || p_value == NULL)
    {
        return false;
    }

    switch (param_type)
    {
    case AICS_PARAM_INPUT_STATE:
        {
            if (value_len != sizeof(T_AICS_INPUT_STATE))
            {
                ret = false;
            }
            else
            {
                if (set_change_counter)
                {
                    memcpy(&p_entry->input_state, p_value, value_len);
                }
                else
                {
                    T_AICS_INPUT_STATE *p_input_state = (T_AICS_INPUT_STATE *)p_value;
                    if (p_entry->input_state.gain_setting != p_input_state->gain_setting ||
                        p_entry->input_state.mute != p_input_state->mute ||
                        p_entry->input_state.gain_mode != p_input_state->gain_mode)
                    {
                        p_entry->input_state.gain_setting = p_input_state->gain_setting;
                        p_entry->input_state.mute = p_input_state->mute;
                        p_entry->input_state.gain_mode = p_input_state->gain_mode;
                        p_entry->input_state.change_counter++;
                    }
                    else
                    {
                        return true;
                    }
                }
                aics_send_input_state_notify_all(p_entry->service_id, &p_entry->input_state);
            }
        }
        break;

    case AICS_PARAM_GAIN_SETTING_PROP:
        {
            if (value_len != sizeof(T_AICS_GAIN_SETTING_PROP))
            {
                ret = false;
            }
            else
            {
                if (memcmp(&p_entry->gain_prop, p_value, value_len))
                {
                    memcpy(&p_entry->gain_prop, p_value, value_len);
                }
            }
        }
        break;

    case AICS_PARAM_INPUT_TYPE:
        {
            if (value_len != sizeof(uint8_t))
            {
                ret = false;
            }
            else
            {
                if (p_entry->input_type != (T_AUDIO_INPUT_TYPE)p_value[0])
                {
                    p_entry->input_type = (T_AUDIO_INPUT_TYPE)p_value[0];
                }
            }
        }
        break;

    case AICS_PARAM_INPUT_STATUS:
        {
            if (value_len != sizeof(uint8_t))
            {
                ret = false;
            }
            else
            {
                if (p_entry->input_status != p_value[0])
                {
                    p_entry->input_status = p_value[0];
                    aics_send_input_status_notify_all(p_entry->service_id, p_entry->input_status);
                }
            }
        }
        break;
    case AICS_PARAM_INPUT_DES:
        {
            if (p_entry->input_desc_len > 0)
            {
                ble_audio_mem_free(p_entry->input_desc);
                p_entry->input_desc = NULL;

            }
            if (value_len > 0)
            {
                p_entry->input_desc = ble_audio_mem_zalloc(value_len + 1);
                if (p_entry->input_desc == NULL)
                {
                    p_entry->input_desc_len = 0;
                    break;
                }
                memcpy(p_entry->input_desc, p_value, value_len);
            }
            p_entry->input_desc_len = value_len;
            aics_send_input_desc_notify_all(p_entry->service_id);
        }
        break;

    default:
        {
            ret = false;
        }
        break;
    }

    return ret;
}

bool aics_get_param(uint8_t srv_instance_id, T_AICS_PARAM_TYPE param_type, uint8_t *p_value)
{
    T_AICS *p_entry;
    bool ret = true;

    p_entry = aics_find_by_idx(srv_instance_id);
    if (p_entry == NULL || p_value == NULL)
    {
        return false;
    }

    switch (param_type)
    {
    case AICS_PARAM_INPUT_STATE:
        memcpy(p_value, &p_entry->input_state, sizeof(T_AICS_INPUT_STATE));
        break;

    case AICS_PARAM_GAIN_SETTING_PROP:
        memcpy(p_value, &p_entry->gain_prop, sizeof(T_AICS_GAIN_SETTING_PROP));
        break;

    case AICS_PARAM_INPUT_TYPE:
        p_value[0] = p_entry->input_type;
        break;

    case AICS_PARAM_INPUT_STATUS:
        p_value[0] = p_entry->input_status;
        break;

    default:
        {
            ret = false;
        }
        break;
    }

    return ret;
}


T_APP_RESULT aics_handle_audio_input_cp(uint16_t conn_handle, T_AICS *p_entry, uint8_t opcode,
                                        int8_t gain_setting)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_AICS_CP_IND   input_cp_op;
    bool  handle_op = false;

    switch (opcode)
    {
    case AICS_CP_SET_GAIN_SETTING:
        if (gain_setting > p_entry->gain_prop.gain_setting_max
            || gain_setting < p_entry->gain_prop.gain_setting_min)
        {
            PROTOCOL_PRINT_ERROR1("[AICS]AICS_CP_SET_GAIN_SETTING: failed, gain_setting %d", gain_setting);
            cause = (T_APP_RESULT) ATT_ERR_AICS_VALUE_OUT_OF_RANGE;
        }
        else
        {
            if (p_entry->input_state.gain_mode == AICS_GAIN_MODE_AUTOMATIC_ONLY ||
                p_entry->input_state.gain_mode == AICS_GAIN_MODE_AUTOMATIC)
            {
                PROTOCOL_PRINT_ERROR1("[AICS]AICS_CP_SET_GAIN_SETTING: failed, gain_mode %d mis-match",
                                      p_entry->input_state.gain_mode);
                return cause;
            }
            else if (gain_setting != p_entry->input_state.gain_setting)
            {
                input_cp_op.gain_setting = gain_setting;
                handle_op = true;
            }
        }
        break;

    case AICS_CP_UNMUTE:
        if (p_entry->input_state.mute == AICS_MUTE_DISABLED)
        {
            cause = (T_APP_RESULT)ATT_ERR_AICS_MUTE_DISABLED;
        }
        else
        {
            if (p_entry->input_state.mute == AICS_MUTED)
            {
                handle_op = true;
            }
        }
        break;

    case AICS_CP_MUTE:
        if (p_entry->input_state.mute == AICS_MUTE_DISABLED)
        {
            cause = (T_APP_RESULT)ATT_ERR_AICS_MUTE_DISABLED;
        }
        else
        {
            if (p_entry->input_state.mute == AICS_NOT_MUTED)
            {
                handle_op = true;
            }
        }
        break;

    case AICS_CP_SET_MANUAL_GAIN_MODE:
        if (p_entry->input_state.gain_mode < AICS_GAIN_MODE_MANUAL)
        {
            cause = (T_APP_RESULT)ATT_ERR_AICS_GAIN_MODE_CHANGE_NOT_ALLOWED;
        }
        else if (p_entry->input_state.gain_mode == AICS_GAIN_MODE_AUTOMATIC)
        {
            handle_op = true;
        }
        break;

    case AICS_CP_SET_AUTOMATIC_GAIN_MODE:
        if (p_entry->input_state.gain_mode < AICS_GAIN_MODE_MANUAL)
        {
            cause = (T_APP_RESULT)ATT_ERR_AICS_GAIN_MODE_CHANGE_NOT_ALLOWED;
        }
        else if (p_entry->input_state.gain_mode == AICS_GAIN_MODE_MANUAL)
        {
            handle_op = true;
        }
        break;
    default:
        break;
    }

    if (handle_op)
    {
        input_cp_op.conn_handle = conn_handle;
        input_cp_op.srv_instance_id = p_entry->idx;
        input_cp_op.cp_op = (T_AICS_CP_OP)opcode;
        cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_AICS_CP_IND, &input_cp_op);
    }

    //If not success, app should call aics_set_param after operation is done
    if (cause == APP_RESULT_SUCCESS)
    {
        if (opcode == AICS_CP_SET_GAIN_SETTING)
        {
            p_entry->input_state.gain_setting = gain_setting;
        }
        else if (opcode == AICS_CP_UNMUTE)
        {
            p_entry->input_state.mute = AICS_NOT_MUTED;
        }
        else if (opcode == AICS_CP_MUTE)
        {
            p_entry->input_state.mute = AICS_MUTED;
        }
        else if (opcode == AICS_CP_SET_MANUAL_GAIN_MODE)
        {
            p_entry->input_state.gain_mode = AICS_GAIN_MODE_MANUAL;
        }
        else if (opcode == AICS_CP_SET_AUTOMATIC_GAIN_MODE)
        {
            p_entry->input_state.gain_mode = AICS_GAIN_MODE_AUTOMATIC;
        }
        p_entry->input_state.change_counter++;
        aics_send_input_state_notify_all(p_entry->service_id, &p_entry->input_state);
    }

    return cause;
}

static void aics_write_post_proc(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                 uint16_t attrib_index, uint16_t length,
                                 uint8_t *p_value)
{
    T_AICS *p_entry;

    p_entry = aics_find(service_id);
    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR1("[AICS]aics_write_post_proc: entry is NULL service id %d", service_id);
        return;
    }
    aics_send_input_desc_notify_all(service_id);
}

T_APP_RESULT aics_attr_read_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                               uint16_t attrib_index,
                               uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_AICS *p_entry;
    uint8_t index;
    uint8_t *p_data;
    uint16_t length;

    PROTOCOL_PRINT_INFO2("[AICS]aics_attr_read_cb: attrib_index %d, offset %x", attrib_index, offset);

    p_entry = aics_find(service_id);
    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR1("[AICS]aics_attr_read_cb: entry is NULL service id %d", service_id);
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    switch (attrib_index)
    {
    case AICS_CHAR_INPUT_STATE_INDEX:
        {
            index = AICS_CHAR_INPUT_STATE_INDEX;
            p_data = (uint8_t *)&p_entry->input_state;
            length = sizeof(T_AICS_INPUT_STATE);
        }
        break;

    case AICS_CHAR_GAIN_SETTINGS_INDEX:
        {
            index = AICS_CHAR_GAIN_SETTINGS_INDEX;
            p_data = (uint8_t *)&p_entry->gain_prop;
            length = sizeof(T_AICS_GAIN_SETTING_PROP);
        }
        break;

    case AICS_CHAR_INPUT_TYPE_INDEX:
        {
            index = AICS_CHAR_INPUT_TYPE_INDEX;
            p_data = (uint8_t *)&p_entry->input_type;
            length = 1;
        }
        break;

    case AICS_CHAR_INPUT_STATUS_INDEX:
        {
            index = AICS_CHAR_INPUT_STATUS_INDEX;
            p_data = (uint8_t *)&p_entry->input_status;
            length = 1;
        }
        break;

    case AICS_CHAR_AUDIO_INPUT_DES_INDEX:
        {
            index = AICS_CHAR_AUDIO_INPUT_DES_INDEX;
            p_data = (uint8_t *)p_entry->input_desc;
            length = p_entry->input_desc_len;
        }
        break;

    default:
        {
            PROTOCOL_PRINT_ERROR1("[AICS]aics_attr_read_cb: attrib_index %d not found", attrib_index);
            return APP_RESULT_ATTR_NOT_FOUND;
        }
    }

    if (gatt_svc_read_confirm(conn_handle, cid, service_id,
                              index, p_data, length, length, APP_RESULT_SUCCESS))
    {
        cause = APP_RESULT_PENDING;
    }
    return (cause);
}

T_APP_RESULT aics_attr_write_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                uint16_t attrib_index,
                                T_WRITE_TYPE write_type,
                                uint16_t length, uint8_t *p_value, P_FUN_EXT_WRITE_IND_POST_PROC *p_write_post_proc)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_AICS *p_entry;

    PROTOCOL_PRINT_INFO2("[AICS]aics_attr_write_cb: attrib_index %d, length %x", attrib_index, length);

    p_entry = aics_find(service_id);
    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR1("[AICS]aics_attr_write_cb: entry is NULL service id %d", service_id);
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    if (attrib_index == AICS_CHAR_AUDIO_INPUT_CP_INDEX)
    {
        uint8_t opcode;
        uint8_t change_counter;

        opcode = p_value[0];
        if ((opcode == 0) || (opcode > AICS_CP_SET_AUTOMATIC_GAIN_MODE))
        {
            return (T_APP_RESULT)ATT_ERR_AICS_OPCODE_NOT_SUPPORT;
        }

        if (length < 2)
        {
            return APP_RESULT_INVALID_VALUE_SIZE;
        }
        change_counter = p_value[1];

        if (change_counter != p_entry->input_state.change_counter)
        {
            return (T_APP_RESULT)ATT_ERR_AICS_INVALID_CHANGE_COUNTER;
        }
        if ((opcode == AICS_CP_SET_GAIN_SETTING && length != 3)
            || (opcode != AICS_CP_SET_GAIN_SETTING && length != 2))
        {
            return APP_RESULT_INVALID_VALUE_SIZE;
        }
        if (opcode == AICS_CP_SET_GAIN_SETTING)
        {
            cause = aics_handle_audio_input_cp(conn_handle, p_entry, opcode, p_value[2]);
        }
        else
        {
            cause = aics_handle_audio_input_cp(conn_handle, p_entry, opcode, 0);
        }
    }
    else if (attrib_index == AICS_CHAR_AUDIO_INPUT_DES_INDEX)
    {
        T_AICS_WRITE_INPUT_DES_IND input_des_ind;
        input_des_ind.conn_handle = conn_handle;
        input_des_ind.srv_instance_id = p_entry->idx;
        input_des_ind.input_des.input_des_len = length;
        input_des_ind.input_des.p_input_des = p_value;
        cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_AICS_WRITE_INPUT_DES_IND,
                                                     &input_des_ind);
        if (cause == APP_RESULT_SUCCESS)
        {
            if (p_entry->input_desc_len > 0)
            {
                ble_audio_mem_free(p_entry->input_desc);
                p_entry->input_desc = NULL;

            }
            if (length > 0)
            {
                p_entry->input_desc = ble_audio_mem_zalloc(length + 1);
                if (p_entry->input_desc == NULL)
                {
                    p_entry->input_desc_len = 0;
                    return APP_RESULT_APP_ERR;
                }
                memcpy(p_entry->input_desc, p_value, length);
            }
            p_entry->input_desc_len = length;
            *p_write_post_proc = aics_write_post_proc;
        }
    }
    else
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }
    return (cause);
}

void aics_cccd_update_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                         uint16_t attrib_index,
                         uint16_t ccc_bits)
{
    PROTOCOL_PRINT_INFO3("aics_cccd_update_cb: service_id %d, attrib_index %d, ccc_bits 0x%x",
                         service_id, attrib_index, ccc_bits);
}

const T_FUN_GATT_EXT_SERVICE_CBS aics_cbs =
{
    aics_attr_read_cb,  // Read callback function pointer
    aics_attr_write_cb, // Write callback function pointer
    aics_cccd_update_cb  // CCCD update callback function pointer
};

bool aics_init(uint8_t srv_num)
{
    uint8_t  i;

    p_aics = ble_audio_mem_zalloc(srv_num * sizeof(T_AICS));
    aics_srv_num = srv_num;

    if (p_aics == NULL)
    {
        goto error;
    }

    for (i = 0; i < aics_srv_num; i++)
    {
        p_aics[i].service_id = aics_add_service(&aics_cbs);
        if (p_aics[i].service_id == 0xFF)
        {
            goto error;
        }
        p_aics[i].idx = i;
    }

    return true;

error:
    if (p_aics != NULL)
    {
        ble_audio_mem_free(p_aics);
        p_aics = NULL;
    }
    aics_srv_num = 0;
    return false;
}

#if LE_AUDIO_DEINIT
void aics_deinit(void)
{
    if (p_aics != NULL)
    {
        for (uint8_t i = 0; i < aics_srv_num; i++)
        {
            if (p_aics[i].input_desc)
            {
                ble_audio_mem_free(p_aics[i].input_desc);
            }
        }
        ble_audio_mem_free(p_aics);
        p_aics = NULL;
    }
    aics_srv_num = 0;
}
#endif
#endif

