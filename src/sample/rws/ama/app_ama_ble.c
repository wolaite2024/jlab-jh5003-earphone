#if AMA_FEATURE_SUPPORT

#include "app_cfg.h"
#include "app_ama.h"
#include "app_ama_ble.h"
#include "ama_ble_service.h"
#include "ble_stream.h"
#include "app_relay.h"
#include "trace.h"
#include "gap_conn_le.h"
#include "ble_adv_data.h"
#include "app_ama_timer.h"
#include "string.h"


static const uint8_t GATT_UUID128_AMA_SRV[16] = { 0xFB, 0x34, 0x9B, 0x5F, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0x03, 0xFE, 0x00, 0x00 };
#define  GATT_UUID128_AMA_TX  0x76, 0x30, 0xf8,0xdd,0x90, 0xa3,0x61,0xac,0xa7,0x43,0x05,0x30,0x77,0xb1,0x4e,0xf0
#define  GATT_UUID128_AMA_RX  0x0b, 0x42, 0x82,0x1f,0x64, 0x72,0x2f,0x8a,0xb4,0x4b,0x79,0x18,0x5b,0xa0,0xee,0x2b


typedef struct
{
    void *p_le_ama_adv_handle;
    bool adv_on;
} AMA_BLE_MGR;


typedef struct
{
    bool adv_on;
} AMA_BLE_REMOTE_INFO;


static AMA_BLE_MGR ble_mgr = {.p_le_ama_adv_handle = NULL, .adv_on = true};


typedef enum
{
    ADV_START               = 0x00,
    ADV_STOP                = 0x01,
    REMOTE_MSG_TYPE_MAX     = 0x02,
} REMOTE_MSG_TYPE;


static uint8_t ama_adv_data[31] =
{
    /* Flags */
    0x02,               /* Length for Flags AD Type*/
    GAP_ADTYPE_FLAGS,   /* Flags AD Type Identifier */
    /* Connectable, dual mode */
    GAP_ADTYPE_FLAGS_SIMULTANEOUS_LE_BREDR_HOST | GAP_ADTYPE_FLAGS_SIMULTANEOUS_LE_BREDR_CONTROLLER | GAP_ADTYPE_FLAGS_GENERAL,
    0x03,              /* Length for list of 16-bit Service Class UUID AD Type */
    0x03,              /* List of 16-bit Service Class UUID AD Type Identifier */
    0x03, 0xFE,        /* 16-bit UUID for Alexa */
    0x17,              /* Length for Service Data AD Type */
    0x16,              /* Service Data AD Type Identifier */
    0x03, 0xFE,        /* 16-bit UUID for Alexa */
    0x5D, 0x00,        /* Vendor ID assigned by BT */
    0x01, 0x00,        /* Alexa Built-in Headphones */
    0x00,              /* Color of the Accessory */
    0x03,              /* Classic Bluetooth is discoverable */
    //0x01,              /* Connection on RFCOMM is preferred */
    0x03,              /* Connection on IAP is preferred */
    0x00, 0x00, 0x00,  /* Payload reserved */
};


static T_ATTRIB_APPL ama_attr_tbl[] =
{
    /*--------------------------Alexa Service ---------------------------*/
    /* <<Primary Service>>, .. 0 */
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE),  /* wFlags     */
        {
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),     /* bTypeValue */
        },
        UUID_128BIT_SIZE,                     /* bValueLen     */
        (void *)GATT_UUID128_AMA_SRV,        /* pValueContext */
        GATT_PERM_READ                              /* wPermissions  */
    },
//Alexa Characteristic Tx

    /* <<Characteristic>>, .. 1*/
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* flags */
        {                                        /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_WRITE,            /* characteristic properties */
            //XXXXMJMJ GATT_CHAR_PROP_INDICATE,                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* permissions */
    },

    /* characteristic value ,Alexa service input--- 2*/
    {
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_UUID_128BIT,                   /* flags */
        {                                        /* type_value */
            GATT_UUID128_AMA_TX
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_WRITE_ENCRYPTED_REQ               /* permissions */
    },
//Alexa Characteristic Rx
    /* <<Characteristic>>,  .. 3*/
    {
        ATTRIB_FLAG_VALUE_INCL,                   /* flags */
        {                                        /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_READ,           /* characteristic properties */
        },
        1,                                        /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* permissions */
    },

    /*  characteristic value ,Alexa service output--- 4 */
    {
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_UUID_128BIT,                   /* flags */
        {                                        /* type_value */
            GATT_UUID128_AMA_RX
        },
        0,                                          /* variable size */
        (void *)NULL,
        GATT_PERM_READ_ENCRYPTED_REQ             /* permissions */
    },

    /* client characteristic configuration ----5*/
    {
        (ATTRIB_FLAG_VALUE_INCL |
         ATTRIB_FLAG_CCCD_APPL | ATTRIB_FLAG_CCCD_NO_FILTER),
        {
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT),
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,
        NULL,
        (GATT_PERM_READ | GATT_PERM_WRITE)
    }
};


#if F_APP_GATT_SERVER_EXT_API_SUPPORT
static T_APP_RESULT app_ama_ble_attr_write_cb(uint16_t conn_handle, uint16_t cid,
                                              uint8_t service_id,
                                              uint16_t attrib_index,
                                              T_WRITE_TYPE write_type,
                                              uint16_t length, uint8_t *p_value, P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_BLE_STREAM_EVENT_PARAM ble_stream_param;
    uint8_t conn_id;
    le_get_conn_id_by_handle(conn_handle, &conn_id);
    APP_PRINT_TRACE3("app_ama_ble_attr_write_cb: attrib_index %d, service_id %d, length %d",
                     attrib_index,
                     service_id, length);


    if (p_value == NULL)
    {
        APP_PRINT_ERROR0("app_ama_ble_attr_write_cb: p_value is NULL");
        cause = APP_RESULT_APP_ERR;
        return cause;
    }

    switch (attrib_index)
    {
    default:
        {
            cause = APP_RESULT_ATTR_NOT_FOUND;
        }
        break;

    case AMA_TX_DATA_INDEX:
        {
            ble_stream_param.receive_data_param.conn_id     = conn_id;
            ble_stream_param.receive_data_param.data        = p_value;
            ble_stream_param.receive_data_param.length      = length;
            ble_stream_param.receive_data_param.service_id  = service_id;
            ble_stream_param.receive_data_param.index       = attrib_index;
            ble_stream_event_process(BLE_STREAM_RECEIVE_DATA_EVENT, &ble_stream_param);
        }
        break;
    }

    return cause;
}

static T_APP_RESULT app_ama_ble_attr_read_cb(uint16_t conn_handle, uint16_t cid,
                                             T_SERVER_ID service_id,
                                             uint16_t attrib_index,
                                             uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    uint8_t conn_id;
    le_get_conn_id_by_handle(conn_handle, &conn_id);
    APP_PRINT_TRACE0("app_ama_ble_attr_read_cb");

    return cause;
}

static void app_ama_ble_cccd_update_cb(uint16_t conn_handle, uint16_t cid, uint8_t service_id,
                                       uint16_t index,
                                       uint16_t ccc_bits)
{
    APP_PRINT_TRACE3("app_ama_ble_cccd_update_cb: index %d, service_id %d, ccc_bits 0x%x", index,
                     service_id,
                     ccc_bits);
    T_BLE_STREAM_EVENT_PARAM ble_stream_param;
    uint8_t conn_id;
    le_get_conn_id_by_handle(conn_handle, &conn_id);
    if (index == AMA_RX_DATA_CCCD_INDEX)
    {
        if (GATT_CLIENT_CHAR_CONFIG_NOTIFY & ccc_bits)
        {
            uint16_t mtu = 0;
            T_GAP_CAUSE cause = le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, (void *)&mtu,
                                                  conn_id);
            if (GAP_CAUSE_SUCCESS != cause)
            {
                APP_PRINT_ERROR1("app_ama_ble_cccd_update_cb: get mtu fail, cause is %d", cause);
                return;
            }

            ble_stream_param.write_cccd_param.conn_id    = conn_id;
            ble_stream_param.write_cccd_param.service_id = service_id;
            ble_stream_event_process(BLE_STREAM_WRITE_CCCD_EVENT, &ble_stream_param);
        }
        else if ((GATT_CLIENT_CHAR_CONFIG_NOTIFY & ccc_bits) == 0)
        {

        }
    }

    return;
}
#else
static T_APP_RESULT app_ama_ble_attr_write_cb(uint8_t conn_id, uint8_t service_id,
                                              uint16_t attrib_index,
                                              T_WRITE_TYPE write_type,
                                              uint16_t length, uint8_t *p_value, P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_BLE_STREAM_EVENT_PARAM ble_stream_param;

    APP_PRINT_TRACE3("app_ama_ble_attr_write_cb: attrib_index %d, service_id %d, length %d",
                     attrib_index,
                     service_id, length);


    if (p_value == NULL)
    {
        APP_PRINT_ERROR0("app_ama_ble_attr_write_cb: p_value is NULL");
        cause = APP_RESULT_APP_ERR;
        return cause;
    }

    switch (attrib_index)
    {
    default:
        {
            cause = APP_RESULT_ATTR_NOT_FOUND;
        }
        break;

    case AMA_TX_DATA_INDEX:
        {
            ble_stream_param.receive_data_param.conn_id     = conn_id;
            ble_stream_param.receive_data_param.data        = p_value;
            ble_stream_param.receive_data_param.length      = length;
            ble_stream_param.receive_data_param.service_id  = service_id;
            ble_stream_param.receive_data_param.index       = attrib_index;
            ble_stream_event_process(BLE_STREAM_RECEIVE_DATA_EVENT, &ble_stream_param);
        }
        break;
    }

    return cause;
}

static T_APP_RESULT app_ama_ble_attr_read_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                             uint16_t attrib_index,
                                             uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;

    APP_PRINT_TRACE0("app_ama_ble_attr_read_cb");

    return cause;
}

static void app_ama_ble_cccd_update_cb(uint8_t conn_id, uint8_t service_id, uint16_t index,
                                       uint16_t ccc_bits)
{
    APP_PRINT_TRACE3("app_ama_ble_cccd_update_cb: index %d, service_id %d, ccc_bits 0x%x", index,
                     service_id,
                     ccc_bits);
    T_BLE_STREAM_EVENT_PARAM ble_stream_param;

    if (index == AMA_RX_DATA_CCCD_INDEX)
    {
        if (GATT_CLIENT_CHAR_CONFIG_NOTIFY & ccc_bits)
        {
            uint16_t mtu = 0;
            T_GAP_CAUSE cause = le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, (void *)&mtu,
                                                  conn_id);
            if (GAP_CAUSE_SUCCESS != cause)
            {
                APP_PRINT_ERROR1("app_ama_ble_cccd_update_cb: get mtu fail, cause is %d", cause);
                return;
            }

            ble_stream_param.write_cccd_param.conn_id    = conn_id;
            ble_stream_param.write_cccd_param.service_id = service_id;
            ble_stream_event_process(BLE_STREAM_WRITE_CCCD_EVENT, &ble_stream_param);
        }
        else if ((GATT_CLIENT_CHAR_CONFIG_NOTIFY & ccc_bits) == 0)
        {

        }
    }

    return;
}
#endif

static const T_FUN_GATT_SERVICE_CBS ama_service_cbs =
{
    app_ama_ble_attr_read_cb,
    app_ama_ble_attr_write_cb,
    app_ama_ble_cccd_update_cb,
};


T_SERVER_ID app_ama_ble_ble_service_add(void)
{
    T_SERVER_ID service_id;
#if F_APP_GATT_SERVER_EXT_API_SUPPORT
    if (false == server_ext_add_service(&service_id,
                                        (uint8_t *)ama_attr_tbl,
                                        sizeof(ama_attr_tbl),
                                        &ama_service_cbs))
#else
    if (false == server_add_service(&service_id,
                                    (uint8_t *)ama_attr_tbl,
                                    sizeof(ama_attr_tbl),
                                    ama_service_cbs))
#endif
    {
        service_id = 0xFF;
    }

    APP_PRINT_TRACE1("ama_ble_add_service: service_id %d", service_id);

    return service_id;
}

static void app_ama_ble_send_remote_info(REMOTE_MSG_TYPE msg_type)
{
    AMA_BLE_REMOTE_INFO remote_info = {0};

    remote_info.adv_on = ble_mgr.adv_on;

    APP_PRINT_TRACE4("app_ama_ble_send_remote_info: msg_type %d, adv_on %d, len %d , data %b", msg_type,
                     remote_info.adv_on, sizeof(remote_info), TRACE_BINARY(sizeof(remote_info), &remote_info));

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_AMA_BLE, msg_type,
                                        (uint8_t *)&remote_info,
                                        sizeof(remote_info));
}


int32_t app_ama_ble_get_info_len(void)
{
    return SIZE_ALIGN4((int32_t)sizeof(AMA_BLE_REMOTE_INFO));
}


void app_ama_ble_get_info(uint8_t *buf)
{
    AMA_BLE_REMOTE_INFO *p_info = (AMA_BLE_REMOTE_INFO *)buf;

    p_info->adv_on = ble_mgr.adv_on;

    APP_PRINT_TRACE1("app_ama_ble_get_info: adv_on %d", p_info->adv_on);
}


int32_t app_ama_ble_set_info(uint8_t *buf)
{
    AMA_BLE_REMOTE_INFO *p_info = (AMA_BLE_REMOTE_INFO *)buf;

    ble_mgr.adv_on = p_info->adv_on;

    APP_PRINT_TRACE1("app_ama_ble_set_info: adv_on %d", ble_mgr.adv_on);

    return app_ama_ble_get_info_len();
}


bool app_ama_ble_adv_start(void)
{
    APP_PRINT_INFO0("app_ama_ble_adv_start");

    if (ble_adv_data_add(&ble_mgr.p_le_ama_adv_handle, sizeof(ama_adv_data), ama_adv_data, 0, NULL))
    {
        app_ama_timer_start_le_adv_timer(300 * 1000);
        ble_mgr.adv_on = true;

        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            app_ama_ble_send_remote_info(ADV_START);
        }

        return true;
    }
    else
    {
        return false;
    }
}


bool app_ama_ble_adv_stop(void)
{
    APP_PRINT_INFO0("app_ama_ble_adv_stop");

    if (ble_adv_data_del(ble_mgr.p_le_ama_adv_handle))
    {
        ble_mgr.p_le_ama_adv_handle = NULL;
        ble_mgr.adv_on = false;
        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            app_ama_ble_send_remote_info(ADV_STOP);
        }
        return true;
    }
    else
    {
        return false;
    }
}


bool app_ama_ble_adv_on(void)
{
    return ble_mgr.adv_on;
}


static void app_ama_ble_sync_to_primary(REMOTE_MSG_TYPE msg_type,  AMA_BLE_REMOTE_INFO *p_info)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    APP_PRINT_TRACE2("app_ama_ble_sync_to_primary: msg_type %d, adv_on %d, ", msg_type, p_info->adv_on);

    switch (msg_type)
    {
    case ADV_START:
    case ADV_STOP:
        ble_mgr.adv_on = p_info->adv_on;
        APP_PRINT_TRACE1("app_ama_ble_sync_to_primary: adv_on %d", ble_mgr.adv_on);
        break;

    default:
        break;
    }
}

#if F_APP_ERWS_SUPPORT
static void app_ama_ble_parse(REMOTE_MSG_TYPE msg_type, uint8_t *buf, uint16_t len,
                              T_REMOTE_RELAY_STATUS status)
{
    if (status != REMOTE_RELAY_STATUS_ASYNC_RCVD)
    {
        return;
    }

    if (len != sizeof(AMA_BLE_REMOTE_INFO))
    {
        APP_PRINT_ERROR0("app_ama_ble_parse: len is wrong");
        return;
    }

    APP_PRINT_TRACE3("app_ama_ble_parse: msg_type %d, len %d, data %b", msg_type, len, TRACE_BINARY(len,
                     buf));

    AMA_BLE_REMOTE_INFO info = {0};
    memcpy(&info, buf, len);

    app_ama_ble_sync_to_primary(msg_type, &info);
}
#endif

void app_ama_ble_set_mtu(uint8_t conn_id, uint16_t mtu)
{
    if (extend_app_cfg_const.ama_support && app_ama_transport_supported(AMA_BLE_STREAM))
    {
        T_BLE_STREAM_EVENT_PARAM ble_stream_param;
        ble_stream_param.update_mut_param.conn_id = conn_id;
        ble_stream_param.update_mut_param.mtu     = mtu;
        ble_stream_event_process(BLE_STREAM_UPDATE_MTU_EVENT, &ble_stream_param);
    }
}

T_SERVER_ID app_ama_ble_init(void)
{
    ble_mgr.adv_on = false;
    ble_mgr.p_le_ama_adv_handle = NULL;
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(NULL, (P_APP_PARSE_CBACK)app_ama_ble_parse,
                             APP_MODULE_TYPE_AMA_BLE, REMOTE_MSG_TYPE_MAX);
#endif
    return app_ama_ble_ble_service_add();
}

#endif

