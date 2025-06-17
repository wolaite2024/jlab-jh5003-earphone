#if C_APP_TILE_FEATURE_SUPPORT
#include <string.h>
#include <trace.h>
#include <gap.h>
#include "app_cfg.h"
#include "app_audio_policy.h"
#include "remote.h"
#include "app_main.h"
#include "app_relay.h"
#include "tile_lib.h"
#include "tile_ble_service.h"
#include "tile_storage.h"
#if F_APP_GATT_SERVER_EXT_API_SUPPORT
#include "gap_conn_le.h"
#endif
const uint8_t GATT_UUID16_TILE_SRV[2]   = { 0xED, 0xFE };

T_SERVER_ID tile_service_id;
uint8_t tile_srv_id;

/**<  Function pointer used to send event to application from tile service. Initiated in tile_ble_service_add_service. */
#if F_APP_GATT_SERVER_EXT_API_SUPPORT
static P_FUN_EXT_SERVER_GENERAL_CB  pfn_tile_ble_service_cb = NULL;
#else
static P_FUN_SERVER_GENERAL_CB pfn_tile_ble_service_cb = NULL;
#endif
/**< @brief  profile/service definition.  */
const T_ATTRIB_APPL tile_ble_service_tbl[] =
{
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        (void *)GATT_UUID16_TILE_SRV,         /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },

    /*******************************************/
    /* <<Characteristic>> demo for write */
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE_NO_RSP), /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                            /* bValueLen     */
        NULL,         /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },
    {
        ATTRIB_FLAG_UUID_128BIT | ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        TILE_TOA_CMD_UUID,
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_WRITE                             /* permissions */
    },

    /********************************************/
    /* <<Characteristic>>, demo for notify */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                          /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_NOTIFY)                 /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_UUID_128BIT | ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        TILE_TOA_RSP_UUID,
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NONE                              /* permissions */
    },
    /**********************************************/

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                 /* flags */
        {                                         /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)          /* permissions */
    },
};

#if F_APP_GATT_SERVER_EXT_API_SUPPORT
/**
 * @brief read characteristic data from service.
 *
 * @param service_id          ServiceID of characteristic data.
 * @param attrib_index        Attribute index of getting characteristic data.
 * @param offset              Used for Blob Read.
 * @param p_length            length of getting characteristic data.
 * @param pp_value            data got from service.
 * @return Profile procedure result
*/
T_APP_RESULT  tile_ble_service_attr_read_cb(uint16_t conn_handle, uint16_t cid,
                                            T_SERVER_ID service_id,
                                            uint16_t attrib_index, uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT  cause  = APP_RESULT_SUCCESS;

    APP_PRINT_ERROR1("tile_ble_service_attr_read_cb, Attr not found, index %d", attrib_index);
    cause = APP_RESULT_ATTR_NOT_FOUND;

    return (cause);
}


void tile_write_post_callback(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                              uint16_t attrib_index,
                              uint16_t length, uint8_t *p_value)
{
    uint8_t conn_id;
    le_get_conn_id_by_handle(conn_handle, &conn_id);
    APP_PRINT_INFO4("tile_write_post_callback: conn_id %d, service_id %d, attrib_index 0x%x, length %d",
                    conn_id, service_id, attrib_index, length);
}
/**
 * @brief write characteristic data from service.
 *
 * @param conn_id
 * @param service_id        ServiceID to be written.
 * @param attrib_index      Attribute index of characteristic.
 * @param length            length of value to be written.
 * @param p_value           value to be written.
 * @return Profile procedure result
*/
T_APP_RESULT tile_ble_service_attr_write_cb(uint16_t conn_handle, uint16_t cid,
                                            T_SERVER_ID service_id,
                                            uint16_t attrib_index, T_WRITE_TYPE write_type, uint16_t length, uint8_t *p_value,
                                            P_FUN_EXT_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    TTILE_CALLBACK_DATA callback_data;
    T_APP_RESULT  cause = APP_RESULT_SUCCESS;
    *p_write_ind_post_proc = tile_write_post_callback;

    uint8_t conn_id;
    le_get_conn_id_by_handle(conn_handle, &conn_id);

    if (TILE_BLE_SERVICE_CHAR_V2_WRITE_INDEX == attrib_index)
    {
        /* Make sure written value size is valid. */
        if (p_value == NULL)
        {
            cause  = APP_RESULT_INVALID_VALUE_SIZE;
        }
        else
        {
            /* Notify Application. */
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.conn_id  = conn_id;
            callback_data.msg_data.write.opcode = TILE_WRITE_V2;
            callback_data.msg_data.write.write_type = write_type;
            callback_data.msg_data.write.len = length;
            callback_data.msg_data.write.p_value = p_value;

            if (pfn_tile_ble_service_cb)
            {
                pfn_tile_ble_service_cb(service_id, (void *)&callback_data);
            }
            APP_PRINT_INFO3("tile_ble_service_attr_write_cb write_type = 0x%x, p_value = %d, length = %d",
                            write_type, *p_value, length);
        }
    }
    else
    {
        APP_PRINT_ERROR2("tile_ble_service_attr_write_cb Error: attrib_index 0x%x, length %d",
                         attrib_index,
                         length);
        cause = APP_RESULT_ATTR_NOT_FOUND;
    }
    return cause;
}

/**
 * @brief update CCCD bits from stack.
 *
 * @param conn_id           connection id.
 * @param service_id          Service ID.
 * @param index          Attribute index of characteristic data.
 * @param cccbits         CCCD bits from stack.
 * @return None
*/
void tile_ble_service_cccd_update_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                     uint16_t index,
                                     uint16_t cccbits)
{
    TTILE_CALLBACK_DATA callback_data;
    bool is_handled = false;
    uint8_t conn_id;
    le_get_conn_id_by_handle(conn_handle, &conn_id);
    callback_data.conn_id = conn_id;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
    APP_PRINT_INFO2("********tile_ble_service_cccd_update_cb: index = %d, cccbits 0x%x", index,
                    cccbits);
    switch (index)
    {
    case TILE_BLE_SERVICE_CHAR_NOTIFY_CCCD_INDEX:
        {
            if (cccbits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                // Enable Notification
                callback_data.msg_data.notification_indification_index = TILE_NOTIFY_INDICATE_V3_ENABLE;
            }
            else
            {
                // Disable Notification
                callback_data.msg_data.notification_indification_index = TILE_NOTIFY_INDICATE_V3_DISABLE;
            }
            is_handled =  true;
        }
        break;

    default:
        break;
    }
    /* If is_handled, notify application. */
    if (pfn_tile_ble_service_cb && (is_handled == true))
    {
        pfn_tile_ble_service_cb(service_id, (void *)&callback_data);
    }
}
#else
/**
 * @brief read characteristic data from service.
 *
 * @param service_id          ServiceID of characteristic data.
 * @param attrib_index        Attribute index of getting characteristic data.
 * @param offset              Used for Blob Read.
 * @param p_length            length of getting characteristic data.
 * @param pp_value            data got from service.
 * @return Profile procedure result
*/
T_APP_RESULT  tile_ble_service_attr_read_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                            uint16_t attrib_index, uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT  cause  = APP_RESULT_SUCCESS;

    APP_PRINT_ERROR1("tile_ble_service_attr_read_cb, Attr not found, index %d", attrib_index);
    cause = APP_RESULT_ATTR_NOT_FOUND;

    return (cause);
}


void tile_write_post_callback(uint8_t conn_id, T_SERVER_ID service_id, uint16_t attrib_index,
                              uint16_t length, uint8_t *p_value)
{
    APP_PRINT_INFO4("tile_write_post_callback: conn_id %d, service_id %d, attrib_index 0x%x, length %d",
                    conn_id, service_id, attrib_index, length);
}
/**
 * @brief write characteristic data from service.
 *
 * @param conn_id
 * @param service_id        ServiceID to be written.
 * @param attrib_index      Attribute index of characteristic.
 * @param length            length of value to be written.
 * @param p_value           value to be written.
 * @return Profile procedure result
*/
T_APP_RESULT tile_ble_service_attr_write_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                            uint16_t attrib_index, T_WRITE_TYPE write_type, uint16_t length, uint8_t *p_value,
                                            P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    TTILE_CALLBACK_DATA callback_data;
    T_APP_RESULT  cause = APP_RESULT_SUCCESS;
    *p_write_ind_post_proc = tile_write_post_callback;
    if (TILE_BLE_SERVICE_CHAR_V2_WRITE_INDEX == attrib_index)
    {
        /* Make sure written value size is valid. */
        if (p_value == NULL)
        {
            cause  = APP_RESULT_INVALID_VALUE_SIZE;
        }
        else
        {
            /* Notify Application. */
            callback_data.msg_type = SERVICE_CALLBACK_TYPE_WRITE_CHAR_VALUE;
            callback_data.conn_id  = conn_id;
            callback_data.msg_data.write.opcode = TILE_WRITE_V2;
            callback_data.msg_data.write.write_type = write_type;
            callback_data.msg_data.write.len = length;
            callback_data.msg_data.write.p_value = p_value;

            if (pfn_tile_ble_service_cb)
            {
                pfn_tile_ble_service_cb(service_id, (void *)&callback_data);
            }
            APP_PRINT_INFO3("tile_ble_service_attr_write_cb write_type = 0x%x, p_value = %d, length = %d",
                            write_type, *p_value, length);
        }
    }
    else
    {
        APP_PRINT_ERROR2("tile_ble_service_attr_write_cb Error: attrib_index 0x%x, length %d",
                         attrib_index,
                         length);
        cause = APP_RESULT_ATTR_NOT_FOUND;
    }
    return cause;
}

/**
 * @brief update CCCD bits from stack.
 *
 * @param conn_id           connection id.
 * @param service_id          Service ID.
 * @param index          Attribute index of characteristic data.
 * @param cccbits         CCCD bits from stack.
 * @return None
*/
void tile_ble_service_cccd_update_cb(uint8_t conn_id, T_SERVER_ID service_id, uint16_t index,
                                     uint16_t cccbits)
{
    TTILE_CALLBACK_DATA callback_data;
    bool is_handled = false;
    callback_data.conn_id = conn_id;
    callback_data.msg_type = SERVICE_CALLBACK_TYPE_INDIFICATION_NOTIFICATION;
    APP_PRINT_INFO2("********tile_ble_service_cccd_update_cb: index = %d, cccbits 0x%x", index,
                    cccbits);
    switch (index)
    {
    case TILE_BLE_SERVICE_CHAR_NOTIFY_CCCD_INDEX:
        {
            if (cccbits & GATT_CLIENT_CHAR_CONFIG_NOTIFY)
            {
                // Enable Notification
                callback_data.msg_data.notification_indification_index = TILE_NOTIFY_INDICATE_V3_ENABLE;
            }
            else
            {
                // Disable Notification
                callback_data.msg_data.notification_indification_index = TILE_NOTIFY_INDICATE_V3_DISABLE;
            }
            is_handled =  true;
        }
        break;

    default:
        break;
    }
    /* If is_handled, notify application. */
    if (pfn_tile_ble_service_cb && (is_handled == true))
    {
        pfn_tile_ble_service_cb(service_id, (void *)&callback_data);
    }
}
#endif

/**
  * @brief send notification of tile notify characteristic value.
  *
  * @param[in] conn_id           connection id
  * @param[in] service_id        service ID of service.
  * @param[in] p_value           characteristic value to notify
  * @param[in] length            characteristic value length to notify
  * @return notification action result
  * @retval 1 true
  * @retval 0 false
  */
bool tile_ble_service_send_v3_notify(uint8_t conn_id, void *p_value, uint16_t length)
{
    APP_PRINT_INFO0("tile_ble_service_send_v3_notify");
    // send notification to client
#if F_APP_GATT_SERVER_EXT_API_SUPPORT
    return server_ext_send_data(le_get_conn_handle(conn_id), L2C_FIXED_CID_ATT,
                                tile_srv_id, TILE_BLE_SERVICE_CHAR_V3_NOTIFY_INDEX, p_value,
                                length, GATT_PDU_TYPE_ANY);
#else
    return server_send_data(conn_id, tile_srv_id, TILE_BLE_SERVICE_CHAR_V3_NOTIFY_INDEX, p_value,
                            length, GATT_PDU_TYPE_ANY);
#endif
}
/**
 * @brief Tile ble Service Callbacks.
*/
#if F_APP_GATT_SERVER_EXT_API_SUPPORT
const T_FUN_GATT_EXT_SERVICE_CBS  tile_ble_service_cbs =
#else
const T_FUN_GATT_SERVICE_CBS tile_ble_service_cbs =
#endif
{
    tile_ble_service_attr_read_cb,  // Read callback function pointer
    tile_ble_service_attr_write_cb, // Write callback function pointer
    tile_ble_service_cccd_update_cb // CCCD update callback function pointer
};

/**
  * @brief Add tile BLE service to the BLE stack database.
  *
  * @param[in]   p_func  Callback when service attribute was read, write or cccd update.
  * @return Service id generated by the BLE stack: @ref T_SERVER_ID.
  * @retval 0xFF Operation failure.
  * @retval others Service id assigned by stack.
  *
  */
T_SERVER_ID tile_ble_service_add_service(void *p_func)
{
#if F_APP_GATT_SERVER_EXT_API_SUPPORT
    if (false == server_ext_add_service(&tile_srv_id,
                                        (uint8_t *)tile_ble_service_tbl,
                                        sizeof(tile_ble_service_tbl),
                                        tile_ble_service_cbs))
    {
        APP_PRINT_ERROR0("tile_ble_service_add_service: fail");
        tile_srv_id = 0xff;
        return tile_srv_id;
    }

    pfn_tile_ble_service_cb = (P_FUN_EXT_SERVER_GENERAL_CB)p_func;
    return tile_srv_id;
#else
    if (false == server_add_service(&tile_srv_id,
                                    (uint8_t *)tile_ble_service_tbl,
                                    sizeof(tile_ble_service_tbl),
                                    tile_ble_service_cbs))
    {
        APP_PRINT_ERROR0("tile_ble_service_add_service: fail");
        tile_srv_id = 0xff;
        return tile_srv_id;
    }

    pfn_tile_ble_service_cb = (P_FUN_SERVER_GENERAL_CB)p_func;
    return tile_srv_id;
#endif
}
#endif
