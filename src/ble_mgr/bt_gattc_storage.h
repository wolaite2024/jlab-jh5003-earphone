#ifndef _BT_GATTC_STORAGE_H_
#define _BT_GATTC_STORAGE_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */
#if CONFIG_REALTEK_BT_GATT_CLIENT_SUPPORT
#include "bt_gatt_client.h"
#include "os_queue.h"
#include "os_timer.h"

#define BT_GATT_CLIENT_BREDR_SUPPORT   1

#define ATTR_DATA_STRUCT_LEN sizeof(T_ATTR_DATA)

#define GATT_SVC_WRITE_CLIENT_SUPPORTED_FEATURES_FLAG  0x80

typedef enum
{
    T_GATTC_CONN_TYPE_LE,
#if BT_GATT_CLIENT_BREDR_SUPPORT
    T_GATTC_CONN_TYPE_BREDR,
#endif
} T_GATTC_CONN_TYPE;

typedef enum
{
    ATTR_TYPE_PRIMARY_SRV_UUID16    = 0x01,
    ATTR_TYPE_PRIMARY_SRV_UUID128   = 0x02,
    ATTR_TYPE_SECONDARY_SRV_UUID16  = 0x03,
    ATTR_TYPE_SECONDARY_SRV_UUID128 = 0x04,
    ATTR_TYPE_INCLUDE_UUID16        = 0x05,
    ATTR_TYPE_INCLUDE_UUID128       = 0x06,
    ATTR_TYPE_CHAR_UUID16           = 0x07,
    ATTR_TYPE_CHAR_UUID128          = 0x08,
    ATTR_TYPE_CCCD_DESC             = 0x09,
} T_ATTR_TYPE;

typedef struct t_spec_gattc_cb
{
    struct t_spec_gattc_cb *p_next;
    T_ATTR_UUID srv_uuid;
    P_FUN_GATT_CLIENT_CB p_fun_cb;
} T_SPEC_GATTC_CB;

typedef struct
{
    uint8_t     attr_type;
    uint8_t     instance_id;
    uint16_t    att_handle;
} T_ATTR_HEADER;

typedef struct
{
    T_ATTR_HEADER hdr;

    uint16_t    end_group_handle;
    uint16_t    uuid16;
} T_ATTR_SRV_UUID16;

typedef struct
{
    T_ATTR_HEADER hdr;

    uint16_t    end_group_handle;
    uint16_t    padding;
    uint8_t     uuid128[16];
} T_ATTR_SRV_UUID128;

typedef struct
{
    T_ATTR_HEADER hdr;

    uint16_t    properties;
    uint16_t    value_handle;
    uint16_t    uuid16;
    uint16_t    end_handle;
} T_ATTR_CHAR_UUID16;

typedef struct
{
    T_ATTR_HEADER hdr;

    uint16_t    properties;
    uint16_t    value_handle;
    uint8_t     uuid128[16];
    uint16_t    end_handle;
    uint16_t    padding;
} T_ATTR_CHAR_UUID128;

typedef struct
{
    T_ATTR_HEADER hdr;
} T_ATTR_CCCD_DESC;

typedef struct
{
    T_ATTR_HEADER hdr;

    uint16_t    start_handle;
    uint16_t    end_handle;
    uint16_t    uuid16;
    uint16_t    padding;
} T_ATTR_INCLUDE_UUID16;

typedef struct
{
    T_ATTR_HEADER hdr;

    uint16_t    start_handle;
    uint16_t    end_handle;
    uint8_t     uuid128[16];
} T_ATTR_INCLUDE_UUID128;

typedef union
{
    T_ATTR_HEADER          hdr;
    T_ATTR_SRV_UUID16      srv_uuid16;
    T_ATTR_SRV_UUID128     srv_uuid128;
    T_ATTR_INCLUDE_UUID16  include_uuid16;
    T_ATTR_INCLUDE_UUID128 include_uuid128;
    T_ATTR_CHAR_UUID16     char_uuid16;
    T_ATTR_CHAR_UUID128    char_uuid128;
    T_ATTR_CCCD_DESC       cccd_desc;
} T_ATTR_DATA;


typedef struct
{
    T_ATTR_CCCD_DESC cccd_descriptor;
    T_ATTR_CHAR_UUID16 char_data;
} T_ATTR_CHAR16_CB;

typedef struct
{
    T_ATTR_CCCD_DESC cccd_descriptor;
    T_ATTR_CHAR_UUID128 char_data;
} T_ATTR_CHAR128_CB;

typedef struct t_attr_srv_cb
{
    struct t_attr_srv_cb   *p_next;
    T_SPEC_GATTC_CB        *p_spec_cb;
    T_ATTR_INCLUDE_UUID16  *p_inc16_table;
    T_ATTR_INCLUDE_UUID128 *p_inc128_table;
    T_ATTR_CHAR16_CB       *p_char16_table;
    T_ATTR_CHAR128_CB      *p_char128_table;
    uint8_t                 inc16_srv_num;
    uint8_t                 inc128_srv_num;
    uint8_t                 char16_num;
    uint8_t                 char128_num;
    T_ATTR_DATA            *p_srv_data;
} T_ATTR_SRV_CB;

typedef struct t_attr_dis_inc
{
    struct t_attr_dis_inc  *p_next;
    T_ATTR_DATA inc_srv;
} T_ATTR_DIS_INC;

typedef struct t_attr_dis_char
{
    struct t_attr_dis_char  *p_next;
    uint16_t    desc_end_handle;
    T_ATTR_CCCD_DESC cccd_descriptor;
    T_ATTR_DATA char_data;
} T_ATTR_DIS_CHAR;

typedef struct t_attr_dis_srv
{
    struct t_attr_dis_srv  *p_next;
    T_SPEC_GATTC_CB      *p_spec_cb;
    uint8_t               inc16_srv_num;
    uint8_t               inc128_srv_num;
    uint8_t               char16_num;
    uint8_t               char128_num;

    T_OS_QUEUE            include_srv_queue; //T_ATTR_DIS_INC
    T_OS_QUEUE            char_queue;        //T_ATTR_DIS_CHAR
    T_ATTR_DATA           srv_data;
} T_ATTR_DIS_SRV;

typedef enum
{
    GATTC_REQ_TYPE_READ    = 0x00,
    GATTC_REQ_TYPE_WRITE   = 0x01,
    GATTC_REQ_TYPE_CCCD    = 0x02,
    GATTC_REQ_TYPE_READ_UUID = 0x03,
} T_GATTC_REQ_TYPE;

typedef struct
{
    T_GATT_WRITE_TYPE write_type;
    uint16_t length;
    uint8_t *p_data;
} T_GATTC_WRITE_REQ;

typedef struct
{
    uint16_t uuid16;
    uint16_t start_handle;
    uint16_t end_handle;
} T_GATTC_READ_UUID;

typedef struct
{
    bool srv_cfg;
    bool cfg_end;
    uint16_t cfg_value;
} T_GATTC_CCCD_CFG;

typedef struct t_gattc_req
{
    struct t_gattc_req  *p_next;
    T_GATTC_REQ_TYPE req_type;
    uint16_t handle;
    uint16_t cid;
    P_FUN_GATT_CLIENT_CB req_cb;
    union
    {
        T_GATTC_WRITE_REQ write;
        T_GATTC_CCCD_CFG  cccd_cfg;
        T_GATTC_READ_UUID read_uuid;
    } p;
} T_GATTC_REQ;

typedef struct
{
    uint16_t        handle;
    uint16_t        cccd_bits;
} T_GATTC_CCCD;

typedef struct t_gattc_channel
{
    struct t_gattc_channel *p_next;
    uint16_t        cid;
    uint16_t        mtu_size;
    bool            is_req;
} T_GATTC_CHANNEL;

typedef struct t_gattc_storage_cb
{
    struct t_gattc_storage_cb *p_next;
    uint8_t conn_id;               //Used for T_GATTC_CONN_TYPE_LE
    T_GAP_CONN_STATE conn_state;   //Used for T_GATTC_CONN_TYPE_LE
    T_GATTC_CONN_TYPE conn_type;
    uint16_t conn_handle;
    P_FUN_GATT_CLIENT_CB p_general_cb;

    //GATTC pending queue
    T_OS_QUEUE gattc_req_list;
    T_OS_QUEUE gattc_write_cmd_list;
    T_OS_QUEUE gattc_wait_rsp_list;
    T_OS_QUEUE channel_list;

    bool     load_from_ftl;

    //service table
    T_GATT_CLIENT_STATE state;
    /*format: service A, service B
    one service:
    service declaration: srv_uuid16 or srv_uuid128
    inc16_srv_num 1 byte;
    inc128_srv_num 1 byte;
    char16_num 1 byte;
    char128_num 1 byte;
    service data: inc16 data, inc128 data, char16 data, char128 data
    */
    uint16_t   srv_table_len;
    uint8_t   *p_srv_table;
    T_OS_QUEUE srv_list;//T_ATTR_SRV_CB

    T_GATT_SERVICE_DATA gatt_svc_data;
    uint8_t gatt_svc_pro_req;

    uint8_t cccd_num;
    T_GATTC_CCCD *p_cccd_table;
    void *p_cccd_timer_handle;
    //T_SYS_TIMER_HANDLE p_cccd_timer_handle;

    //Used for discovery
    T_ATTR_DIS_SRV *p_curr_srv;
    T_OS_QUEUE gattc_dis_list;//T_ATTR_DIS_SRV
} T_GATTC_STORAGE_CB;

extern P_FUN_GATT_STORAGE_CB gatt_storage_cb;
extern uint16_t gattc_dis_mode;
extern T_OS_QUEUE spec_gattc_queue;
extern T_OS_QUEUE bt_gattc_queue;

T_GATTC_STORAGE_CB  *gatt_client_check_link_int(const char *p_func_name, uint16_t conn_handle);
#define gatt_client_check_link(conn_handle) gatt_client_check_link_int(__func__, conn_handle)
//storage
bool gattc_storage_add_primary_service(T_GATTC_STORAGE_CB *p_gattc_cb, bool uuid16,
                                       T_DISCOVERY_RESULT_DATA *p_result_data);
bool gattc_storage_add_secondary_service(T_GATTC_STORAGE_CB *p_gattc_cb);
bool gattc_storage_add_include(T_ATTR_DIS_SRV *p_srv_dis_cb, bool uuid16,
                               T_DISCOVERY_RESULT_DATA *p_result_data);
bool gattc_storage_add_char(T_ATTR_DIS_SRV *p_srv_dis_cb, bool uuid16,
                            T_DISCOVERY_RESULT_DATA *p_result_data);
uint8_t gattc_storage_get_srv_instance_id(T_GATTC_STORAGE_CB *p_gattc_cb, bool is_uuid16,
                                          uint16_t uuid16, uint8_t *p_uuid128);
uint8_t gattc_storage_get_char_instance_id(T_ATTR_DIS_SRV *p_srv_cb, bool is_uuid16,
                                           uint16_t uuid16,
                                           uint8_t *p_uuid128);
bool gattc_storage_srv_list_gen(T_GATTC_STORAGE_CB *p_gattc_cb);
void gattc_storage_srv_list_release(T_GATTC_STORAGE_CB *p_gattc_cb);
void gattc_storage_dis_list_release(T_GATTC_STORAGE_CB *p_gattc_cb);
T_SPEC_GATTC_CB *gatt_spec_client_find_by_uuid(T_ATTR_UUID *p_srv_uuid);
bool gattc_storage_load(T_GATTC_STORAGE_CB *p_gattc_cb, bool database_hash_check);
bool gattc_storage_write(T_GATTC_STORAGE_CB *p_gattc_cb);
//void gattc_storage_dis_print(T_GATTC_STORAGE_CB *p_gattc_cb);
void gattc_storage_print(T_GATTC_STORAGE_CB *p_gattc_cb);

void gattc_set_cccd_data(T_GATTC_STORAGE_CB *p_gattc_cb, uint16_t handle, uint16_t ccc_bits);
uint16_t gattc_get_cccd_data(T_GATTC_STORAGE_CB *p_gattc_cb, uint16_t handle);

uint8_t gattc_storage_get_srv_num(T_GATTC_STORAGE_CB *p_gattc_cb, bool is_uuid16,
                                  uint16_t uuid16, uint8_t *p_uuid128);
T_ATTR_SRV_CB *gattc_storage_find_srv_by_handle(T_GATTC_STORAGE_CB *p_gattc_cb, uint16_t handle);
bool gattc_storage_find_char_desc(T_ATTR_SRV_CB *p_srv_cb, uint16_t handle,
                                  T_GATT_CHAR_TYPE *p_char_type, T_ATTR_DATA *p_char_data);
T_ATTR_SRV_CB *gattc_storage_find_srv_by_uuid(T_GATTC_STORAGE_CB *p_gattc_cb,
                                              T_ATTR_UUID *p_attr_uuid);
T_ATTR_SRV_CB *gattc_storage_find_inc_srv_by_uuid(T_GATTC_STORAGE_CB *p_gattc_cb,
                                                  T_ATTR_SRV_CB *p_inc_srv);
bool gattc_storage_find_char_by_uuid(T_ATTR_SRV_CB *p_srv_cb, T_ATTR_UUID *p_attr_uuid,
                                     T_ATTR_DATA *p_attr_data);
bool gattc_storage_find_char(T_ATTR_SRV_CB *p_srv_cb, T_ATTR_UUID *p_attr_uuid,
                             T_ATTR_CHAR16_CB **pp_char16, T_ATTR_CHAR128_CB **pp_char128);
bool gattc_storage_check_prop(T_GATTC_STORAGE_CB *p_gattc_cb, uint16_t handle,
                              uint16_t properties_bit);
void att_data_covert_to_uuid(T_ATTR_DATA *p_attr_data, T_ATTR_UUID *p_attr_uuid);
#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
