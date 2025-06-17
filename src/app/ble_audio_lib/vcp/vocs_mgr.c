#include <string.h>
#include "trace.h"
#include "vocs_mgr.h"
#include "bt_types.h"
#include "vocs_def.h"
#include "codec_def.h"
#include "ble_audio_mgr.h"
#include "ble_link_util.h"
#include "vocs.h"

#if LE_AUDIO_VOCS_SUPPORT
static uint8_t vocs_srv_num = 0;
static T_VOCS *p_vocs;

T_VOCS *vocs_find(T_SERVER_ID service_id)
{
    uint8_t  i;

    if (p_vocs == NULL)
    {
        return NULL;
    }

    for (i = 0; i < vocs_srv_num; i++)
    {
        if (p_vocs[i].service_id == service_id)
        {
            return (&p_vocs[i]);
        }
    }

    PROTOCOL_PRINT_ERROR1("[VOCS]vocs_find: failed, service_id 0x%x", service_id);

    return NULL;
}

T_VOCS *vocs_find_by_idx(uint8_t srv_instance_id)
{
    uint8_t  i;

    if (p_vocs == NULL)
    {
        return NULL;
    }

    for (i = 0; i < vocs_srv_num; i++)
    {
        if (p_vocs[i].srv_instance_id == srv_instance_id)
        {
            return (&p_vocs[i]);
        }
    }

    PROTOCOL_PRINT_ERROR1("[VOCS]vocs_find_by_idx: failed, srv_instance_id 0x%x", srv_instance_id);

    return NULL;
}

bool vocs_send_output_desc_notify_all(uint8_t service_id)
{
    T_VOCS *p_entry;
    uint16_t attrib_idx;

    p_entry = vocs_find(service_id);
    if (p_entry == NULL)
    {
        return false;
    }
    attrib_idx = gatt_svc_find_char_index_by_uuid16(p_entry->p_attr_tbl,
                                                    VOCS_UUID_CHAR_AUDIO_OUTPUT_DES,
                                                    p_entry->attr_num);
    ble_audio_send_notify_all(L2C_FIXED_CID_ATT,
                              p_entry->service_id, attrib_idx,
                              p_entry->output_des, p_entry->output_des_len);
    return true;
}

bool vocs_send_audio_location_notify_all(uint8_t service_id)
{
    T_VOCS *p_entry;
    uint8_t data[4];
    uint16_t attrib_idx;

    p_entry = vocs_find(service_id);
    if (p_entry == NULL)
    {
        return false;
    }
    attrib_idx = gatt_svc_find_char_index_by_uuid16(p_entry->p_attr_tbl,
                                                    VOCS_UUID_CHAR_AUDIO_LOCATION,
                                                    p_entry->attr_num);
    LE_UINT32_TO_ARRAY(data, p_entry->audio_location);
    ble_audio_send_notify_all(L2C_FIXED_CID_ATT, p_entry->service_id,
                              attrib_idx, data, 4);
    return true;
}

bool vocs_send_volume_offset_notify_all(uint8_t service_id)
{
    T_VOCS *p_entry;
    uint8_t data[3];
    uint16_t attrib_idx;

    p_entry = vocs_find(service_id);
    if (p_entry == NULL)
    {
        return false;
    }
    attrib_idx = gatt_svc_find_char_index_by_uuid16(p_entry->p_attr_tbl,
                                                    VOCS_UUID_CHAR_VOLUME_OFFSET_STATE,
                                                    p_entry->attr_num);

    data[0] = (uint8_t)p_entry->volume_offset;
    data[1] = (uint8_t)(p_entry->volume_offset >> 8);
    data[2] = p_entry->change_counter;

    ble_audio_send_notify_all(L2C_FIXED_CID_ATT, p_entry->service_id,
                              attrib_idx, data, 3);
    return true;
}

bool vocs_set_param(uint8_t srv_instance_id, T_VOCS_PARAM_SET *p_param)
{
    T_VOCS *p_entry;

    p_entry = vocs_find_by_idx(srv_instance_id);
    if (p_entry == NULL || p_param == NULL)
    {
        return false;
    }

    if (p_param->set_mask & VOCS_VOLUME_OFFSET_STATE_FLAG)
    {
        if (p_entry->param_init == false)
        {
            p_entry->change_counter = p_param->change_counter;
            p_entry->param_init = true;
            p_entry->volume_offset = p_param->volume_offset;
            vocs_send_volume_offset_notify_all(p_entry->service_id);
        }
        else
        {
            if (p_entry->volume_offset != p_param->volume_offset)
            {
                p_entry->volume_offset = p_param->volume_offset;
                p_entry->change_counter++;
                vocs_send_volume_offset_notify_all(p_entry->service_id);
            }
        }
    }

    if (p_param->set_mask & VOCS_AUDIO_LOCATION_FLAG)
    {
        p_entry->audio_location = p_param->audio_location;
        if (p_entry->feature & VOCS_AUDIO_LOCATION_NOTIFY_SUPPORT)
        {
            vocs_send_audio_location_notify_all(p_entry->service_id);
        }
    }
    if (p_param->set_mask & VOCS_AUDIO_OUTPUT_DES_FLAG)
    {
        if (p_param->output_des.p_output_des)
        {
            uint8_t *p_output_des = ble_audio_mem_zalloc(p_param->output_des.output_des_len + 1);
            if (p_output_des)
            {
                if (p_entry->output_des)
                {
                    ble_audio_mem_free(p_entry->output_des);
                }
                p_entry->output_des = p_output_des;

                memcpy(p_entry->output_des, p_param->output_des.p_output_des, p_param->output_des.output_des_len);
                p_entry->output_des_len = p_param->output_des.output_des_len;
                if (p_entry->feature & VOCS_AUDIO_OUTPUT_DES_NOTIFY_SUPPORT)
                {
                    vocs_send_output_desc_notify_all(p_entry->service_id);
                }
            }
            else
            {
                return false;
            }
        }
    }

    PROTOCOL_PRINT_INFO3("[VOCS]vocs_set_param: volume_offset %d, change_counter 0x%01x, audio_location 0x%01x",
                         p_entry->volume_offset,
                         p_entry->change_counter,
                         p_entry->audio_location);
    return true;
}


bool vocs_get_param(uint8_t srv_instance_id, T_VOCS_PARAM_GET *p_param)
{
    T_VOCS *p_entry;

    p_entry = vocs_find_by_idx(srv_instance_id);
    if (p_entry == NULL || p_param == NULL)
    {
        return false;
    }

    p_param->volume_offset = p_entry->volume_offset;
    p_param->change_counter = p_entry->change_counter;
    p_param->audio_location = p_entry->audio_location;
    p_param->output_des.p_output_des = p_entry->output_des;
    p_param->output_des.output_des_len = p_entry->output_des_len;
    return true;
}

static void vocs_write_post_proc(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                 uint16_t attrib_index, uint16_t length,
                                 uint8_t *p_value)
{
    T_VOCS *p_entry;
    PROTOCOL_PRINT_INFO2("[VOCS]vocs_write_post_proc: attrib_index %d, length %x", attrib_index,
                         length);

    p_entry = vocs_find(service_id);
    if (p_entry == NULL)
    {
        return;
    }

    if (attrib_index == gatt_svc_find_char_index_by_uuid16(p_entry->p_attr_tbl,
                                                           VOCS_UUID_CHAR_VOLUME_OFFSET_CP,
                                                           p_entry->attr_num))
    {
        vocs_send_volume_offset_notify_all(service_id);
    }
    else if (attrib_index == gatt_svc_find_char_index_by_uuid16(p_entry->p_attr_tbl,
                                                                VOCS_UUID_CHAR_AUDIO_LOCATION,
                                                                p_entry->attr_num))
    {
        vocs_send_audio_location_notify_all(service_id);
    }
    else if (attrib_index == gatt_svc_find_char_index_by_uuid16(p_entry->p_attr_tbl,
                                                                VOCS_UUID_CHAR_AUDIO_OUTPUT_DES,
                                                                p_entry->attr_num))
    {
        vocs_send_output_desc_notify_all(service_id);
    }

}

T_APP_RESULT vocs_attr_read_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                               uint16_t attrib_index,
                               uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_VOCS *p_entry;

    p_entry = vocs_find(service_id);
    if (p_entry == NULL)
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }
    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_entry->p_attr_tbl,
                                                             attrib_index, p_entry->attr_num);

    PROTOCOL_PRINT_INFO2("[VOCS]vocs_attr_read_cb: attrib_index %d, offset %x", attrib_index, offset);
    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR1("vocs_attr_read_cb Error: attrib_index 0x%x", attrib_index);
        return APP_RESULT_ATTR_NOT_FOUND;
    }
    switch (char_uuid.uu.char_uuid16)
    {
    case VOCS_UUID_CHAR_VOLUME_OFFSET_STATE:
        {
            uint8_t offset_state[3];
            memcpy(offset_state, &p_entry->volume_offset, 2);
            offset_state[2] = p_entry->change_counter;
            if (gatt_svc_read_confirm(conn_handle, cid, service_id,
                                      gatt_svc_find_char_index_by_uuid16(p_entry->p_attr_tbl,
                                                                         VOCS_UUID_CHAR_VOLUME_OFFSET_STATE,
                                                                         p_entry->attr_num),
                                      offset_state, 3, 3, APP_RESULT_SUCCESS))
            {
                cause = APP_RESULT_PENDING;
            }
        }
        break;

    case VOCS_UUID_CHAR_AUDIO_LOCATION:
        {
            uint8_t data[4];
            LE_UINT32_TO_ARRAY(data, p_entry->audio_location);
            if (gatt_svc_read_confirm(conn_handle, cid, service_id,
                                      gatt_svc_find_char_index_by_uuid16(p_entry->p_attr_tbl,
                                                                         VOCS_UUID_CHAR_AUDIO_LOCATION,
                                                                         p_entry->attr_num),
                                      data, 4, 4, APP_RESULT_SUCCESS))
            {
                cause = APP_RESULT_PENDING;
            }
        }
        break;

    case VOCS_UUID_CHAR_AUDIO_OUTPUT_DES:
        {
            if (offset > p_entry->output_des_len)
            {
                cause = APP_RESULT_INVALID_OFFSET;
                break;
            }
            if (gatt_svc_read_confirm(conn_handle, cid, service_id,
                                      gatt_svc_find_char_index_by_uuid16(p_entry->p_attr_tbl,
                                                                         VOCS_UUID_CHAR_AUDIO_OUTPUT_DES,
                                                                         p_entry->attr_num),
                                      p_entry->output_des + offset,
                                      p_entry->output_des_len - offset,
                                      p_entry->output_des_len - offset, APP_RESULT_SUCCESS))
            {
                cause = APP_RESULT_PENDING;
            }
        }
        break;

    default:
        {
            PROTOCOL_PRINT_ERROR1("[VOCS]vocs_attr_read_cb: attrib_index %d not found", attrib_index);
            return APP_RESULT_ATTR_NOT_FOUND;
        }
    }
    return (cause);
}

T_APP_RESULT vocs_attr_write_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                uint16_t attrib_index,
                                T_WRITE_TYPE write_type,
                                uint16_t length, uint8_t *p_value, P_FUN_EXT_WRITE_IND_POST_PROC *p_write_post_proc)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_VOCS *p_entry;

    p_entry = vocs_find(service_id);
    if (p_entry == NULL)
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }
    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_entry->p_attr_tbl,
                                                             attrib_index, p_entry->attr_num);

    PROTOCOL_PRINT_INFO2("[VOCS]vocs_attr_write_cb: attrib_index %d, length %x", attrib_index, length);
    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR1("vocs_attr_write_cb Error: attrib_index 0x%x", attrib_index);
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    if (char_uuid.uu.char_uuid16 == VOCS_UUID_CHAR_VOLUME_OFFSET_CP)
    {
        uint8_t opcode;
        uint8_t change_counter;
        int16_t volume_offset;

        if (length != 4)
        {
            return APP_RESULT_INVALID_VALUE_SIZE;
        }

        opcode = p_value[0];
        change_counter = p_value[1];

        if (opcode != VOCS_CP_SET_VOLUME_OFFSET)
        {
            return (T_APP_RESULT)ATT_ERR_VOCS_OPCODE_NOT_SUPPORT;
        }
        LE_ARRAY_TO_UINT16(volume_offset, p_value + 2);

        if (change_counter != p_entry->change_counter)
        {
            return (T_APP_RESULT)ATT_ERR_VOCS_INVALID_CHANGE_COUNTER;
        }

        if (volume_offset > 255 || volume_offset < -255)
        {
            PROTOCOL_PRINT_INFO2("[VOCS]vocs_attr_write_cb: invalid volume_offset %d, 0x%01x", volume_offset,
                                 volume_offset);
            return (T_APP_RESULT)ATT_ERR_VOCS_VALUE_OUT_OF_RANGE;
        }
        if (p_entry->volume_offset != volume_offset)
        {
            T_VOCS_WRITE_OFFSET_STATE_IND vol_offset;
            vol_offset.srv_instance_id = p_entry->srv_instance_id;
            vol_offset.volume_offset = volume_offset;
            vol_offset.conn_handle = conn_handle;
            cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_VOCS_WRITE_OFFSET_STATE_IND,
                                                         &vol_offset);
            if (APP_RESULT_SUCCESS == cause)
            {
                //FIX TODO this should be set by app audio
                p_entry->volume_offset = volume_offset;
                p_entry->change_counter++;
                *p_write_post_proc = vocs_write_post_proc;
                PROTOCOL_PRINT_INFO2("[VCP][VOCS] vocs_write_post_proc: change_counter 0x%01x, volume_offset %d",
                                     change_counter, volume_offset);
            }
        }
    }
    else if (char_uuid.uu.char_uuid16 == VOCS_UUID_CHAR_AUDIO_LOCATION)
    {
        uint32_t audio_loc;
        if (length != 4)
        {
            return APP_RESULT_INVALID_VALUE_SIZE;
        }
        LE_ARRAY_TO_UINT32(audio_loc, p_value);

        if ((audio_loc | AUDIO_LOCATION_MASK) != AUDIO_LOCATION_MASK)
        {
            return (T_APP_RESULT)ATT_ERR_VOCS_VALUE_OUT_OF_RANGE;
        }
        if (p_entry->audio_location != audio_loc)
        {
            T_VOCS_WRITE_AUDIO_LOCATION_IND chnl_loc;
            chnl_loc.conn_handle = conn_handle;
            chnl_loc.srv_instance_id = p_entry->srv_instance_id;
            chnl_loc.audio_location = audio_loc;
            cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_VOCS_WRITE_AUDIO_LOCATION_IND,
                                                         &chnl_loc);
            if (APP_RESULT_SUCCESS == cause)
            {
                p_entry->audio_location = audio_loc;
                *p_write_post_proc = vocs_write_post_proc;
            }
        }
    }
    else if (char_uuid.uu.char_uuid16 == VOCS_UUID_CHAR_AUDIO_OUTPUT_DES)
    {
        T_VOCS_WRITE_OUTPUT_DES_IND output_des;
        output_des.conn_handle = conn_handle;
        output_des.srv_instance_id = p_entry->srv_instance_id;
        output_des.output_des.p_output_des = p_value;
        output_des.output_des.output_des_len = length;
        cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_VOCS_WRITE_OUTPUT_DES_IND,
                                                     &output_des);
        if (APP_RESULT_SUCCESS == cause)
        {
            uint8_t *p_output_des = ble_audio_mem_zalloc(length + 1);
            if (p_output_des)
            {
                if (p_entry->output_des)
                {
                    ble_audio_mem_free(p_entry->output_des);
                }
                p_entry->output_des = p_output_des;

                memcpy(p_entry->output_des, p_value, length);
                p_entry->output_des_len = length;
                *p_write_post_proc = vocs_write_post_proc;
            }
            else
            {
                return (T_APP_RESULT)ATT_ERR_VOCS_VALUE_OUT_OF_RANGE;
            }
        }
    }
    else
    {
        cause = APP_RESULT_ATTR_NOT_FOUND;
    }
    return (cause);
}

void vocs_cccd_update_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                         uint16_t attrib_index,
                         uint16_t ccc_bits)
{
    PROTOCOL_PRINT_INFO2("[VOCS]vocs_cccd_update_cb: attrib_index %d, ccc_bits 0x%x",
                         attrib_index, ccc_bits);
}

const T_FUN_GATT_EXT_SERVICE_CBS vocs_cbs =
{
    vocs_attr_read_cb,  // Read callback function pointer
    vocs_attr_write_cb, // Write callback function pointer
    vocs_cccd_update_cb  // CCCD update callback function pointer
};

bool vocs_init(uint8_t srv_num, uint8_t *feature)
{
    uint8_t  i = 0;

    p_vocs = ble_audio_mem_zalloc(srv_num * sizeof(T_VOCS));

    if (p_vocs == NULL)
    {
        goto error;
    }

    vocs_srv_num = srv_num;

    for (i = 0; i < vocs_srv_num; i++)
    {
        if (feature[i] & VOCS_AUDIO_LOCATION_WRITE_WITHOUT_RSP_SUPPORT)
        {
            feature[i] |= VOCS_AUDIO_LOCATION_NOTIFY_SUPPORT;
        }
        if (feature[i] & VOCS_AUDIO_OUTPUT_DES_WRITE_WITHOUT_RSP_SUPPORT)
        {
            feature[i] |= VOCS_AUDIO_OUTPUT_DES_NOTIFY_SUPPORT;
        }
        p_vocs[i].service_id = vocs_add_service(&vocs_cbs, &p_vocs[i], feature[i]);
        if (p_vocs[i].service_id == 0xFF)
        {
            goto error;
        }
        p_vocs[i].srv_instance_id = i;
    }

    return true;
error:
    if (p_vocs != NULL)
    {
        if (i != 0)
        {
            while (i-- > 0)
            {
                if (p_vocs[i - 1].p_attr_tbl)
                {
                    ble_audio_mem_free(p_vocs[i - 1].p_attr_tbl);
                }
            }
        }
        ble_audio_mem_free(p_vocs);
        p_vocs = NULL;
    }
    vocs_srv_num = 0;
    return false;
}

#if LE_AUDIO_DEINIT
void vocs_deinit(void)
{
    if (p_vocs)
    {
        for (uint8_t i = 0; i < vocs_srv_num; i++)
        {
            if (p_vocs[i].p_attr_tbl)
            {
                ble_audio_mem_free(p_vocs[i].p_attr_tbl);
            }

            if (p_vocs[i].output_des)
            {
                ble_audio_mem_free(p_vocs[i].output_des);
            }
        }
        vocs_srv_num = 0;
        ble_audio_mem_free(p_vocs);
        p_vocs = NULL;
    }
}
#endif
#endif
