#if DMA_FEATURE_SUPPORT
#include "app_dma_device.h"
#include "app_dma_record.h"
#include "dma_service_api.h"
#include "dma.pb-c.h"
#include "vector.h"
#include "string.h"
#include "trace.h"
#include "app_dma_rfc.h"
#include "app_dma_ble.h"
#include "bt_avrcp.h"
#include "app_link_util.h"
#include "dma_state_feature.h"


#define TRANSPORT__BLUETOOTH_MAX (TRANSPORT__BLUETOOTH_IAP + 1)


typedef struct
{
    bool is_connected;
    uint8_t bd_addr[6];
    uint8_t conn_id;
    uint32_t mtu;
    Transport transport_type;
    DMA_SERVICE_HDL dma_serv_hdl;
} APP_DMA_DEVICE;


struct
{
    VECTOR dev_vector;
    APP_DMA_DEVICE *recording_dev;
} app_dma_device_mgr = {.dev_vector = NULL, .recording_dev = NULL};



static bool app_dma_device_tx(APP_DMA_DEVICE *p_dev, uint8_t *data, uint32_t len)
{
    APP_PRINT_TRACE2("app_dma_device_tx: p_dev %p, len %d", p_dev, len);

    //ToDo
    switch (p_dev->transport_type)
    {
    case TRANSPORT__BLUETOOTH_LOW_ENERGY:
        app_dma_ble_write(p_dev->conn_id, data, len);
        break;

    case TRANSPORT__BLUETOOTH_RFCOMM:
        app_dma_device_media_ctrl(p_dev->bd_addr, data, len);
        break;

    case TRANSPORT__BLUETOOTH_IAP:
        break;

    default:
        break;
    }

    return true;
}



static uint32_t app_dma_device_get_mtu(APP_DMA_DEVICE *p_dev)
{
    return p_dev->mtu;
}

static bool app_dma_device_start_record(APP_DMA_DEVICE *p_dev)
{
    if (app_dma_device_mgr.recording_dev != NULL)
    {
        APP_PRINT_ERROR1("app_dma_device start_record: already recording now, p_dev %p", p_dev);
        return false;
    }

    //ToDo: start record
    app_dma_record_start();
    return true;
}


static bool app_dma_device_stop_record(APP_DMA_DEVICE *p_dev)
{
    if (p_dev != app_dma_device_mgr.recording_dev)
    {
        APP_PRINT_ERROR2("app_dma_device stop_record: target addr %s, recording addr %s", p_dev,
                         app_dma_device_mgr.recording_dev);
        return false;
    }

    //ToDo: stop recording
    app_dma_record_stop();
    app_dma_device_mgr.recording_dev = NULL;

    return true;
}


static State app_dma_device_get_bt_state(uint32_t feature)
{
    State state = STATE__INIT;

    switch (feature)
    {

    }

    return state;
}


static State app_dma_device_get_ota_state(uint32_t feature)
{
    State state = STATE__INIT;

    return state;
}


static State app_dma_device_get_tws_state(uint32_t feature)
{
    State state = STATE__INIT;

    return state;
}


static State app_dma_device_get_state_by_group(uint32_t feature)
{
    State state = STATE__INIT;

    if (DMA_A2DP_ENABLED <= feature && feature <= DMA_BT_CLASSIC_DISCOVERABLE)
    {
        state = app_dma_device_get_bt_state(feature);
    }
    else if ((DMA_SILENT_OTA_OFFSET <= feature && feature <= DMA_OTA_BIN_IDX)
             || feature == DMA_OTA_CTRL_BY_SCENE
             || feature == DMA_OTA_INFORM
             || feature == DMA_LEFT_BUD_FW_VERSION_INFORM
             || feature == DMA_LIGHT_BUD_FW_VERSION_INFORM)
    {
        state = app_dma_device_get_ota_state(feature);
    }
    else if ((DMA_LEFT_IS_ACTIVATED <= feature && DMA_RIGHT_TAP_SETTING <= DMA_LEFT_IS_ACTIVATED)
             || feature == DMA_MS_SL_SW_CHK
             || feature == DMA_MS_SL_SW_CHK_W_BOX_CLOSE
             || feature == DMA_RSSI_SW_ROLE)
    {
        state = app_dma_device_get_tws_state(feature);
    }
    else if (feature <= DMA_RIGHT_BUD_SEARCH_TONING)
    {

    }


    return state;
}


static State app_dma_device_get_state(APP_DMA_DEVICE *p_dev, uint32_t feature)
{
    State state = STATE__INIT;
    state.feature = feature;

    switch (feature)
    {
    case DMA_ALIVE:
        state.boolean = true;
        break;

    case DMA_SIGN_VERIFY:
        state.boolean = false;
        break;

    case DMA_AUX_CONNECTED:
        state.boolean = false;
        break;

    case DMA_DEVICE_THEME:
        break;

    case DMA_REMAIN_BAT_LEVEL:
        break;

    case DMA_EAR_DETECTION:
        break;

    case DMA_SERIAL_NUM_AUTH:
        break;

    case DMA_CONTINUE_ASR:
        break;

    default:
        {
            state = app_dma_device_get_state_by_group(feature);
        }
        break;
    }

    return state;
}


static bool app_dma_device_send_at_cmd(APP_DMA_DEVICE *p_dev, char *at_cmd)
{
    //ToDo

    return true;
}


static bool app_dma_device_media_ctrl(APP_DMA_DEVICE *p_dev, MediaControl media_ctrl)
{
    //ToDo
    APP_PRINT_TRACE1("app_dma_media_ctrl: media_ctrl %d", media_ctrl);


    switch (media_ctrl)
    {
    case MEDIA_CONTROL__PLAY:
        bt_avrcp_play(p_dev->bd_addr);
        break;

    case MEDIA_CONTROL__PAUSE:
        bt_avrcp_pause(p_dev->bd_addr);
        break;

    case MEDIA_CONTROL__NEXT:
        bt_avrcp_forward(p_dev->bd_addr);
        break;

    case MEDIA_CONTROL__PREVIOUS:
        bt_avrcp_backward(p_dev->bd_addr);
        break;

    case MEDIA_CONTROL__ABSOLUTE_VOLUME:
        {
            T_APP_BR_LINK *p_link = NULL;
            p_link = app_link_find_br_link(p_dev->bd_addr);

            if (p_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PLAYING)
            {
                bt_avrcp_pause(p_dev->bd_addr);
            }
            else if (p_link->avrcp_play_status == BT_AVRCP_PLAY_STATUS_PAUSED)
            {
                bt_avrcp_play(p_dev->bd_addr);
            }
            else
            {
                APP_PRINT_ERROR1("app_dma_device_media_ctrl: ERROR avrcp_play_status %d",
                                 p_link->avrcp_play_status);
            }
        }
        break;
    default:
        break;
    }


    return true;
}


void app_dma_device_create(Transport transport_type, uint8_t *bd_addr, uint8_t conn_id)
{
    APP_DMA_DEVICE *p_dev = NULL;

    p_dev = dma_malloc(sizeof(APP_DMA_DEVICE));
    if (p_dev == NULL)
    {
        return;
    }

    switch (transport_type)
    {
    case TRANSPORT__BLUETOOTH_LOW_ENERGY:
        memset(p_dev->bd_addr, 0xff, 6);
        p_dev->conn_id = conn_id;
        break;

    case TRANSPORT__BLUETOOTH_RFCOMM:
        memcpy(p_dev->bd_addr, bd_addr, 6);
        p_dev->conn_id = DMA_DEFAULT_CONN_ID;
        break;

    case TRANSPORT__BLUETOOTH_IAP:
        break;

    default:
        break;
    }

    p_dev->transport_type = transport_type;
    p_dev->is_connected = false;

    DMA_SERVICE_CBS cbs = {0};
    cbs.service_tx = (DMA_SERVICE_TX)app_dma_device_tx;
    cbs.get_mtu = (DMA_SERVICE_GET_MTU)app_dma_device_get_mtu;
    cbs.start_record = (DMA_SERVICE_START_RECORD)app_dma_device_start_record;
    cbs.stop_record = (DMA_SERVICE_STOP_RECORD)app_dma_device_stop_record;
    cbs.get_state = (DMA_SERVICE_GET_STATE)app_dma_device_get_state;
    cbs.send_at_cmd = (DMA_SERVICE_SEND_AT_CMD)app_dma_device_send_at_cmd;
    cbs.media_ctrl = (DMA_SERVICE_MEDIA_CTRL)app_dma_device_media_ctrl;
    p_dev->dma_serv_hdl = dma_service_create(p_dev, bd_addr, cbs);

    APP_PRINT_TRACE4("app_dma_device_create: transport_type %d, bd_addr %s, conn_id %d, dma_serv_hdl %p",
                     transport_type, TRACE_BDADDR(bd_addr), conn_id, p_dev->dma_serv_hdl);

    vector_add(app_dma_device_mgr.dev_vector, p_dev);
}


static bool app_dma_device_match_bd_addr(APP_DMA_DEVICE *p_dev, uint8_t *bd_addr)
{
    if (p_dev->transport_type != TRANSPORT__BLUETOOTH_RFCOMM)
    {
        return false;
    }

    if (memcmp(p_dev->bd_addr, bd_addr, 6) != 0)
    {
        return false;
    }

    return true;
}


static bool app_dma_device_match_conn_id(APP_DMA_DEVICE *p_dev, uint8_t *p_conn_id)
{
    if (p_dev->transport_type != TRANSPORT__BLUETOOTH_LOW_ENERGY)
    {
        return false;
    }

    if (p_dev->conn_id != *p_conn_id)
    {
        return false;
    }

    return true;
}


void app_dma_device_delete(uint8_t *bd_addr)
{
    APP_DMA_DEVICE *p_dev = NULL;
    p_dev = vector_search(app_dma_device_mgr.dev_vector, (V_PREDICATE)app_dma_device_match_bd_addr,
                          bd_addr);
    if (p_dev == NULL)
    {
        return;
    }

    vector_remove(app_dma_device_mgr.dev_vector, p_dev);

    dma_service_delete(p_dev->dma_serv_hdl);

    dma_free(p_dev);
}


static APP_DMA_DEVICE *search_dev(uint8_t *bd_addr, uint8_t conn_id)
{
    APP_DMA_DEVICE *p_dev = NULL;

    if (bd_addr != NULL)
    {
        p_dev = vector_search(app_dma_device_mgr.dev_vector, (V_PREDICATE)app_dma_device_match_bd_addr,
                              bd_addr);
    }
    else
    {
        p_dev = vector_search(app_dma_device_mgr.dev_vector, (V_PREDICATE)app_dma_device_match_conn_id,
                              &conn_id);
    }

    return p_dev;
}


void app_dma_device_handle_b2s_connected(uint8_t *bd_addr)
{
    app_dma_device_create(TRANSPORT__BLUETOOTH_RFCOMM, bd_addr, 0xff);
}



void app_dma_device_handle_transport_evt(APP_DMA_TRANSPORT_EVT evt,
                                         APP_DMA_TRANSPORT_PARAM *p_param)
{
    APP_DMA_DEVICE *p_dev = NULL;

    APP_PRINT_TRACE3("app_dma_device_handle_transport_evt: evt %d, bd_addr %s, conn_id %d", evt,
                     TRACE_BDADDR(p_param->bd_addr), p_param->conn_id);

    p_dev = search_dev(p_param->bd_addr, p_param->conn_id);
    if (p_dev == NULL)
    {
        return;
    }

    switch (evt)
    {
    case APP_DMA_CONN_CMPL:
        if (p_dev->is_connected == false)
        {
            p_dev->is_connected = true;
            p_dev->mtu = p_param->conn_cmpl.mtu;
            dma_service_connected(p_dev->dma_serv_hdl);
        }
        break;

    case APP_DMA_DISC_CMPL:
        if (p_dev->is_connected == true)
        {
            APP_PRINT_TRACE0("app_dma_device_handle_transport_evt: device disc");
            p_dev->is_connected = false;
        }
        break;

    case APP_DMA_DATA_IND:
        if (p_dev->is_connected == false)
        {
            APP_PRINT_TRACE0("app_dma_device_handle_transport_evt: device not connected");
        }
        dma_service_receive_data(p_dev->dma_serv_hdl, p_param->data_ind.data, p_param->data_ind.data_len);
        break;

    default:
        break;
    }
}


void app_dma_device_init(void)
{
    app_dma_device_mgr.dev_vector = vector_create(0xff);

    APP_PRINT_TRACE1("app_dma_device_init: dev_vector %p", app_dma_device_mgr.dev_vector);

    app_dma_ble_init(app_dma_device_handle_transport_evt);
    app_dma_rfc_init(app_dma_device_handle_transport_evt);
}

#endif
