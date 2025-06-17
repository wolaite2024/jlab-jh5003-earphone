#ifndef _APP_AMA_TRANSPORTS_H_
#define _APP_AMA_TRANSPORTS_H_


#include "ama_service_api.h"


#include "common_stream.h"
#include "spp_stream.h"
#include "ble_stream.h"
#include "iap_stream.h"

#define AMA_STREAM_INVALID_CONN_ID  (0xff)


#if AMA_FEATURE_SUPPORT
#define AMA_SPP_STREAM_NUM  (2)
#define AMA_BLE_STREAM_NUM  (2)
#define AMA_IAP_STREAM_NUM  (2)
#else
#define AMA_SPP_STREAM_NUM  (0)
#define AMA_BLE_STREAM_NUM  (0)
#define AMA_IAP_STREAM_NUM  (0)
#endif


/**  @brief  Transport mode */
typedef enum
{
    AMA_BLE_STREAM,
    AMA_SPP_STREAM,
    AMA_IAP_STREAM,
    AMA_MAX_STREAM
} AMA_STREAM_TYPE;


struct _AMA_TRANSPORT
{
    bool            transport_connected;
    uint8_t         bd_addr[BLUETOOTH_ADDR_LENGTH];
    AMA_STREAM_TYPE active_stream_type;
    COMMON_STREAM   streams[AMA_MAX_STREAM];
    uint8_t         conn_id;
};


typedef struct _AMA_TRANSPORT AMA_TRANSPORT;


typedef enum
{
    AMA_TRANSPORT_CONNECTED_EVT,
    AMA_TRANSPORT_DISCONNECTED_EVT,
    AMA_TRANSPORT_DATA_RECEIVED,
} AMA_TRANSPORT_EVT;


typedef struct
{
    uint8_t *bd_addr;
    uint8_t *buf;
    uint32_t length;
} AMA_TRANSPORT_CB_PARAM;


typedef void (*AMA_TRANSPORT_CB)(AMA_TRANSPORT_EVT evt, AMA_TRANSPORT_CB_PARAM *p_param);


T_AMA_BT_INFO  app_ama_transport_get_bt_info(uint8_t *bd_addr);
bool app_ama_transport_supported(AMA_STREAM_TYPE stream_type);



bool app_ama_transport_init(AMA_TRANSPORT_CB cb);


COMMON_STREAM app_ama_transport_stream_create(AMA_STREAM_TYPE stream_type, uint8_t *pub_addr,
                                              uint8_t conn_id);
bool app_ama_transport_stream_delete(AMA_STREAM_TYPE stream_type, uint8_t *pub_addr,
                                     uint8_t conn_id);

void app_ama_transport_tx(AMA_TRANSPORT *p_transport, uint8_t *data, uint16_t length);

AMA_TRANSPORT *app_ama_transport_create(uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH]);
void app_ama_transport_delete(uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH]);
bool app_ama_transport_delete_for_temp_ble(void);


void app_ama_transport_send_all_info(void);


uint32_t app_ama_transport_get_mtu(AMA_TRANSPORT *p_transport);
void app_ama_transport_select_active_stream(AMA_TRANSPORT *p_transport,
                                            AMA_STREAM_TYPE select_stream_type);
AMA_TRANSPORT *app_ama_transport_find_transport_by_bd_addr(uint8_t bd_addr[BLUETOOTH_ADDR_LENGTH]);


int32_t app_ama_transport_get_info_len(void);
void app_ama_transport_get_info(uint8_t *buf);
int32_t app_ama_transport_set_info(uint8_t *buf);

#endif

