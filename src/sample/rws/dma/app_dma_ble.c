#if DMA_FEATURE_SUPPORT
#include "app_dma_ble.h"
#include "stdint.h"
#include "gap_conn_le.h"
#include "gatt.h"
#include "stdlib.h"
#if F_APP_GATT_SERVER_EXT_API_SUPPORT
#include <profile_server_ext.h>
#else
#include <profile_server.h>
#endif
#include "trace.h"
#include "string.h"
#include "app_cfg.h"
#include "ble_adv_data.h"



typedef enum
{
    DMA_SERVICE_DCLR_IDX        = 0,
    DMA_TX_CHAR_DCLR_IDX        = 1,
    DMA_TX_CHAR_VAL_DCLR_IDX    = 2,
    DMA_RX_CHAR_DCLR_IDX        = 3,
    DMA_RX_CHAR_VAL_DCLR_IDX    = 4,
    DMA_RX_CHAR_CCCD_IDX        = 5,
} DMA_ATTR_IDX;


struct
{
    APP_DMA_TRANSPORT_CB cb;
    T_SERVER_ID service_id;
    void *adv_handler;
} app_dma_ble_mgr = {.cb = NULL, .service_id = 0xff, .adv_handler = NULL};



#define DMA_SERVICE_CLS_UUID    0x00, 0x00, 0xFD, 0xC2, 0x00, 0x00, 0x10, 0x00,\
    0x80, 0x00, 0x00, 0x80, 0x5F, 0x9B, 0x34, 0xFB,

#define DMA_TX_UUID     0xB8, 0x4A, 0xC9, 0xC6, 0x29, 0xC5, 0x46, 0xD4,\
    0xBB, 0xA1, 0x9D, 0x53, 0x47, 0x84, 0x33, 0x0F

#define DMA_RX_UUID     0xC2, 0xE7, 0x58, 0xB9, 0x0E, 0x78, 0x41, 0xE0,\
    0xB0, 0xCB, 0x98, 0xA5, 0x93, 0x19, 0x3F, 0xC5




#define DMA_SERVICE_DCLR                                                \
    {                                                                       \
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_LE),  /* wFlags     */              \
        {                                                                   \
                                                                            LO_WORD(GATT_UUID_PRIMARY_SERVICE),                             \
                                                                            HI_WORD(GATT_UUID_PRIMARY_SERVICE),     /* bTypeValue */        \
        },                                                                  \
        UUID_128BIT_SIZE,                     /* bValueLen     */           \
        (void *)((uint8_t[]){DMA_SERVICE_CLS_UUID}),                        \
        GATT_PERM_READ                                                      \
    }


#define DMA_TX_CHAR_DCLR                                                \
    {                                                                       \
        ATTRIB_FLAG_VALUE_INCL,                                         \
        {                                                               \
                                                                        LO_WORD(GATT_UUID_CHARACTERISTIC),                          \
                                                                        HI_WORD(GATT_UUID_CHARACTERISTIC),                          \
                                                                        GATT_CHAR_PROP_WRITE                                        \
        },                                                              \
        1,                                        /* bValueLen */       \
        NULL,                                                           \
        GATT_PERM_READ                            /* permissions */     \
    }


#define DMA_TX_CHAR_VAL_DCLR                                            \
    {                                                                       \
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_UUID_128BIT,                   \
        {                                                                   \
                                                                            DMA_TX_UUID                                             \
        },                                                                  \
        0,                                          /* variable size */     \
        (void *)NULL,                                                       \
        GATT_PERM_WRITE_ENCRYPTED_REQ               /* permissions */       \
    }


#define DMA_RX_CHAR_DCLR                                                \
    {                                                                       \
        ATTRIB_FLAG_VALUE_INCL,                                             \
        {                                                                   \
                                                                            LO_WORD(GATT_UUID_CHARACTERISTIC),                              \
                                                                            HI_WORD(GATT_UUID_CHARACTERISTIC),                              \
                                                                            GATT_CHAR_PROP_NOTIFY | GATT_CHAR_PROP_READ,                    \
        },                                                                  \
        1,                                                                  \
        NULL,                                                               \
        GATT_PERM_READ                                                      \
    }



#define DMA_RX_CHAR_VAL_DCLR                                                \
    {                                                                       \
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_UUID_128BIT,                   \
        {                                                                   \
                                                                            DMA_RX_UUID                                                     \
        },                                                                  \
        0,                                                                  \
        (void *)NULL,                                                       \
        GATT_PERM_READ_ENCRYPTED_REQ                                        \
    }



#define DMA_RX_CHAR_CCCD                                                \
    {                                                                       \
        (ATTRIB_FLAG_VALUE_INCL |                                           \
         ATTRIB_FLAG_CCCD_APPL | ATTRIB_FLAG_CCCD_NO_FILTER),               \
        {                                                                   \
                                                                            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),                          \
                                                                            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),                          \
                                                                            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT),                       \
                                                                            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)                        \
        },                                                                  \
        2,                                                                  \
        NULL,                                                               \
        (GATT_PERM_READ | GATT_PERM_WRITE)                                  \
    }


#define DMA_ADV_ADDR_OFFSET         (3 + 4 + 6)

static uint8_t dma_adv_data[31] =
{
    0x02,
    GAP_ADTYPE_FLAGS,
    GAP_ADTYPE_FLAGS_BREDR_NOT_SUPPORTED | GAP_ADTYPE_FLAGS_GENERAL,

    0x03,              /* Length for list of 16-bit Service Class UUID AD Type */
    0x03,              /* List of 16-bit Service Class UUID AD Type Identifier */
    0xC2, 0xFD,        /* 16-bit UUID for DMA */

    0x0E,
    GAP_ADTYPE_MANUFACTURER_SPECIFIC,
    0x5D, 0x00, //Manufacture id
    0xFD, 0xC2, //Magic number
    0x55, 0x55, 0x55, 0x55, 0x55, 0x55, //Public Address
    0xff,       // pairing pop-up windows
    0x01,       // device type = headphone
    0x01        // C-version headphone
};



static const T_ATTRIB_APPL dma_attr_tbl[] =
{
    [DMA_SERVICE_DCLR_IDX]      = DMA_SERVICE_DCLR,
    [DMA_TX_CHAR_DCLR_IDX]      = DMA_TX_CHAR_DCLR,
    [DMA_TX_CHAR_VAL_DCLR_IDX]  = DMA_TX_CHAR_VAL_DCLR,
    [DMA_RX_CHAR_DCLR_IDX]      = DMA_RX_CHAR_DCLR,
    [DMA_RX_CHAR_VAL_DCLR_IDX]  = DMA_RX_CHAR_VAL_DCLR,
    [DMA_RX_CHAR_CCCD_IDX]      = DMA_RX_CHAR_CCCD,
};

#if F_APP_GATT_SERVER_EXT_API_SUPPORT
static T_APP_RESULT app_dma_ble_attr_write_cb(uint16_t conn_handle, uint16_t cid,
                                              uint8_t service_id,
                                              uint16_t attrib_index,
                                              T_WRITE_TYPE write_type,
                                              uint16_t length, uint8_t *p_value, P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc)

{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    uint8_t conn_id;
    le_get_conn_id_by_handle(conn_handle, &conn_id);
    switch (attrib_index)
    {
    case DMA_TX_CHAR_VAL_DCLR_IDX:
        {
            APP_DMA_TRANSPORT_PARAM param = {0};
            param.conn_id = conn_id;
            param.bd_addr = NULL;
            param.data_ind.data = p_value;
            param.data_ind.data_len = length;
            app_dma_ble_mgr.cb(APP_DMA_DATA_IND, &param);
        }
        break;

    default:
        break;
    }

    return cause;
}


static T_APP_RESULT app_dma_ble_attr_read_cb(uint16_t conn_handle, uint16_t cid,
                                             T_SERVER_ID service_id,
                                             uint16_t attrib_index,
                                             uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    uint8_t conn_id;
    le_get_conn_id_by_handle(conn_handle, &conn_id);
    APP_PRINT_TRACE0("app_dma_ble_attr_read_cb");

    return cause;
}


static void app_dma_ble_cccd_update_cb(uint16_t conn_handle, uint16_t cid, uint8_t service_id,
                                       uint16_t index,
                                       uint16_t ccc_bits)
{
    APP_PRINT_TRACE3("ama_cccd_update_cb: index %d, service_id %d, ccc_bits 0x%x", index,
                     service_id,
                     ccc_bits);
    uint8_t conn_id;
    le_get_conn_id_by_handle(conn_handle, &conn_id);
    if (index == DMA_RX_CHAR_CCCD_IDX)
    {
        if (GATT_CLIENT_CHAR_CONFIG_NOTIFY & ccc_bits)
        {
            uint16_t mtu = 0;
            T_GAP_CAUSE cause = le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, (void *)&mtu,
                                                  conn_id);

            APP_DMA_TRANSPORT_PARAM param = {0};
            param.conn_id = conn_id;
            param.bd_addr = NULL;
            param.conn_cmpl.mtu = mtu;
            app_dma_ble_mgr.cb(APP_DMA_CONN_CMPL, &param);
        }
    }
}
#else
static T_APP_RESULT app_dma_ble_attr_write_cb(uint8_t conn_id, uint8_t service_id,
                                              uint16_t attrib_index,
                                              T_WRITE_TYPE write_type,
                                              uint16_t length, uint8_t *p_value, P_FUN_WRITE_IND_POST_PROC *p_write_ind_post_proc)

{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;

    switch (attrib_index)
    {
    case DMA_TX_CHAR_VAL_DCLR_IDX:
        {
            APP_DMA_TRANSPORT_PARAM param = {0};
            param.conn_id = conn_id;
            param.bd_addr = NULL;
            param.data_ind.data = p_value;
            param.data_ind.data_len = length;
            app_dma_ble_mgr.cb(APP_DMA_DATA_IND, &param);
        }
        break;

    default:
        break;
    }

    return cause;
}


static T_APP_RESULT app_dma_ble_attr_read_cb(uint8_t conn_id, T_SERVER_ID service_id,
                                             uint16_t attrib_index,
                                             uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;

    APP_PRINT_TRACE0("app_dma_ble_attr_read_cb");

    return cause;
}


static void app_dma_ble_cccd_update_cb(uint8_t conn_id, uint8_t service_id, uint16_t index,
                                       uint16_t ccc_bits)
{
    APP_PRINT_TRACE3("ama_cccd_update_cb: index %d, service_id %d, ccc_bits 0x%x", index,
                     service_id,
                     ccc_bits);

    if (index == DMA_RX_CHAR_CCCD_IDX)
    {
        if (GATT_CLIENT_CHAR_CONFIG_NOTIFY & ccc_bits)
        {
            uint16_t mtu = 0;
            T_GAP_CAUSE cause = le_get_conn_param(GAP_PARAM_CONN_MTU_SIZE, (void *)&mtu,
                                                  conn_id);

            APP_DMA_TRANSPORT_PARAM param = {0};
            param.conn_id = conn_id;
            param.bd_addr = NULL;
            param.conn_cmpl.mtu = mtu;
            app_dma_ble_mgr.cb(APP_DMA_CONN_CMPL, &param);
        }
    }
}
#endif

void app_dma_ble_write(uint8_t conn_id, uint8_t *data, uint32_t data_len)
{
#if F_APP_GATT_SERVER_EXT_API_SUPPORT
    server_ext_send_data(le_get_conn_handle(conn_id), L2C_FIXED_CID_ATT,
                         app_dma_ble_mgr.service_id, DMA_TX_CHAR_VAL_DCLR_IDX,
                         data,
                         data_len,
                         GATT_PDU_TYPE_ANY);
#else
    server_send_data(conn_id, app_dma_ble_mgr.service_id, DMA_TX_CHAR_VAL_DCLR_IDX,
                     data,
                     data_len,
                     GATT_PDU_TYPE_ANY);
#endif
}


void app_dma_ble_adv_start(void)
{
    memcpy(&dma_adv_data[DMA_ADV_ADDR_OFFSET], app_cfg_nv.bud_local_addr, 6);

    APP_PRINT_TRACE1("app_dma_ble_adv_start: adv_data %b", TRACE_BINARY(sizeof(dma_adv_data),
                                                                        dma_adv_data));

    ble_adv_data_add(&app_dma_ble_mgr.adv_handler, sizeof(dma_adv_data), dma_adv_data, 0, NULL);

}


void app_dma_ble_adv_stop(void)
{
    APP_PRINT_INFO0("app_dma_ble_adv_stop");
    ble_adv_data_del(app_dma_ble_mgr.adv_handler);
}



void app_dma_ble_init(APP_DMA_TRANSPORT_CB cb)
{
    T_FUN_GATT_SERVICE_CBS ble_cbs = {0};

    ble_cbs.cccd_update_cb = app_dma_ble_cccd_update_cb;
    ble_cbs.read_attr_cb = app_dma_ble_attr_read_cb;
    ble_cbs.write_attr_cb = app_dma_ble_attr_write_cb;
#if F_APP_GATT_SERVER_EXT_API_SUPPORT
    server_ext_add_service(&app_dma_ble_mgr.service_id, (uint8_t *)dma_attr_tbl, sizeof(dma_attr_tbl),
                           &ble_cbs);
#else
    server_add_service(&app_dma_ble_mgr.service_id, (uint8_t *)dma_attr_tbl, sizeof(dma_attr_tbl),
                       ble_cbs);
#endif
}

#endif
