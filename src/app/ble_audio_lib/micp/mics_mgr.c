#include "trace.h"
#include "vocs_mgr.h"
#include "bt_types.h"
#include "codec_def.h"
#include "ble_audio_mgr.h"
#include "ble_link_util.h"
#include "mics.h"
#include "mics_mgr.h"
#include "mics_def.h"

#if LE_AUDIO_MICS_SUPPORT

T_MGR_MICS *p_mgr_mics = NULL;

static bool mics_send_mute_value_notify_all()
{
    uint16_t attrib_idx = gatt_svc_find_char_index_by_uuid16(p_mgr_mics->p_mics_attr_tbl,
                                                             MICS_UUID_CHAR_MUTE,
                                                             p_mgr_mics->attr_num);
    ble_audio_send_notify_all(L2C_FIXED_CID_ATT, p_mgr_mics->service_id, attrib_idx,
                              &p_mgr_mics->mic_mute,
                              1);
    return true;
}

bool mics_set_param(T_MICS_PARAM *p_param)
{
    if (p_param == NULL || p_mgr_mics == NULL)
    {
        return false;
    }
    if (p_mgr_mics->mic_mute == p_param->mic_mute)
    {
        return true;
    }

    p_mgr_mics->mic_mute = p_param->mic_mute;
    return mics_send_mute_value_notify_all();
}

bool mics_get_param(T_MICS_PARAM *p_param)
{
    if (p_param == NULL || p_mgr_mics == NULL)
    {
        return false;
    }
    p_param->mic_mute = p_mgr_mics->mic_mute;
    return true;
}

static void mics_write_post_proc(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                 uint16_t attrib_index, uint16_t length,
                                 uint8_t *p_value)
{
    uint16_t attrib_idx = gatt_svc_find_char_index_by_uuid16(p_mgr_mics->p_mics_attr_tbl,
                                                             MICS_UUID_CHAR_MUTE,
                                                             p_mgr_mics->attr_num);
    if (attrib_index == attrib_idx)
    {
        p_mgr_mics->mic_mute = (T_MICS_MUTE) * p_value;
        mics_send_mute_value_notify_all();
    }
}

T_APP_RESULT mics_attr_read_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                               uint16_t attrib_index,
                               uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_mgr_mics->p_mics_attr_tbl,
                                                             attrib_index, p_mgr_mics->attr_num);
    *p_length = 0;

    PROTOCOL_PRINT_INFO2("mics_attr_read_cb: attrib_index %d, offset %x", attrib_index, offset);

    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR1("mics_attr_read_cb Error: attrib_index 0x%x", attrib_index);
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    if (char_uuid.uu.char_uuid16 == MICS_UUID_CHAR_MUTE)
    {
        if (gatt_svc_read_confirm(conn_handle, cid,
                                  service_id, attrib_index,
                                  &p_mgr_mics->mic_mute, 1, 1, APP_RESULT_SUCCESS))
        {
            cause = APP_RESULT_PENDING;
        }
    }
    else
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    return (cause);
}

T_APP_RESULT mics_attr_write_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                uint16_t attrib_index,
                                T_WRITE_TYPE write_type,
                                uint16_t length, uint8_t *p_value, P_FUN_EXT_WRITE_IND_POST_PROC *p_write_post_proc)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_mgr_mics->p_mics_attr_tbl,
                                                             attrib_index, p_mgr_mics->attr_num);

    PROTOCOL_PRINT_INFO2("mics_attr_write_cb: attrib_index %d, length %x", attrib_index, length);
    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR1("mics_attr_write_cb Error: attrib_index 0x%x", attrib_index);
        return APP_RESULT_ATTR_NOT_FOUND;
    }
    if (char_uuid.uu.char_uuid16 == MICS_UUID_CHAR_MUTE)
    {
        T_MICS_WRITE_MUTE_IND mute_ind;

        mute_ind.mic_mute = (T_MICS_MUTE) * p_value;

        if (length < 1)
        {
            return APP_RESULT_INVALID_VALUE_SIZE;
        }
        if (p_mgr_mics->mic_mute == MICS_DISABLED)
        {
            return (T_APP_RESULT)ATT_ERR_MICS_MUTE_DISABLED_ERROR;
        }
        else if (mute_ind.mic_mute > MICS_MUTED)
        {
            return (T_APP_RESULT)(ATT_ERR | ATT_ERR_VALUE_NOT_ALLOWED);
        }

        cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_MICS_WRITE_MUTE_IND, &mute_ind);
        if (cause == APP_RESULT_SUCCESS)
        {
            *p_write_post_proc = mics_write_post_proc;
        }
    }
    else
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    return (cause);
}

void mics_cccd_update_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                         uint16_t attrib_index,
                         uint16_t ccc_bits)
{
    PROTOCOL_PRINT_INFO2("mics_cccd_update_cb: attrib_index %d, ccc_bits 0x%x", attrib_index, ccc_bits);
    if (p_mgr_mics == NULL)
    {
        return;
    }
    ble_audio_mgr_send_server_cccd_info(conn_handle, p_mgr_mics->p_mics_attr_tbl, service_id,
                                        attrib_index, ccc_bits,
                                        0, p_mgr_mics->attr_num);
    return;
}

const T_FUN_GATT_EXT_SERVICE_CBS mics_cbs =
{
    mics_attr_read_cb,  // Read callback function pointer
    mics_attr_write_cb, // Write callback function pointer
    mics_cccd_update_cb  // CCCD update callback function pointer
};

bool mics_init(uint8_t aics_num, uint8_t *id_array)
{
    if (p_mgr_mics)
    {
        return true;
    }
    p_mgr_mics = ble_audio_mem_zalloc(sizeof(T_MGR_MICS));

    if (p_mgr_mics == NULL)
    {
        goto error;
    }
    p_mgr_mics->service_id = mics_add_service(&mics_cbs, aics_num, id_array);

    if (p_mgr_mics->service_id == 0xFF)
    {
        goto error;
    }

    return true;
error:
    if (p_mgr_mics != NULL)
    {
        if (p_mgr_mics->p_mics_attr_tbl)
        {
            ble_audio_mem_free(p_mgr_mics->p_mics_attr_tbl);
        }
        ble_audio_mem_free(p_mgr_mics);
        p_mgr_mics = NULL;
    }
    return false;
}

#if LE_AUDIO_DEINIT
void mics_deinit(void)
{
    if (p_mgr_mics != NULL)
    {
        if (p_mgr_mics->p_mics_attr_tbl)
        {
            ble_audio_mem_free(p_mgr_mics->p_mics_attr_tbl);
        }
        ble_audio_mem_free(p_mgr_mics);
        p_mgr_mics = NULL;
    }

}
#endif
#endif
