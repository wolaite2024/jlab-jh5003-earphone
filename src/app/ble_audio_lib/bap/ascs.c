#include <stdint.h>
#include <string.h>
#include "trace.h"
#include "ascs.h"
#include "ascs_def.h"
#include "ase_sm.h"
#include "ble_link_util.h"

#if LE_AUDIO_ASCS_SUPPORT
T_SERVER_ID ascs_service_id;
T_ATTRIB_APPL *p_ascs_attr_tbl = NULL;
uint16_t ascs_attr_num = 0;

/**< @brief  profile/service definition.  */
static const T_ATTRIB_APPL ascs_attr_tbl[] =
{
    /*----------------- Published Audio Capabilities Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_ASCS),              /* service UUID */
            HI_WORD(GATT_UUID_ASCS)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_WRITE_NO_RSP |                   /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* ASE Control Point value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(ASCS_UUID_CHAR_ASE_CONTROL_POINT),
            HI_WORD(ASCS_UUID_CHAR_ASE_CONTROL_POINT)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_WRITE_AUTHEN_REQ | GATT_PERM_NOTIF_IND_AUTHEN_REQ /* permissions */
    },
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ)          /* permissions */
    },
};

static const T_ATTRIB_APPL ase_attr_tbl[] =
{
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* ASE value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(ASCS_UUID_CHAR_SNK_ASE),
            HI_WORD(ASCS_UUID_CHAR_SNK_ASE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_NOTIF_IND_AUTHEN_REQ /* permissions */
    }
    ,
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            /* NOTE: this value has an instantiation for each client, a write to */
            /* this attribute does not modify this default value:                */
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ)          /* permissions */
    },
};

uint16_t create_ascs_table(void)
{
    uint8_t i, idx = 0;
    uint8_t ase_num = ble_audio_db.src_ase_num + ble_audio_db.snk_ase_num;
    uint16_t attr_tbl_size = 0;

    attr_tbl_size = sizeof(ascs_attr_tbl);

    if (ase_num < 1 || ase_num > ASCS_AES_CHAR_MAX_NUM)
    {
        return 0;
    }

    attr_tbl_size += ase_num * 3 * sizeof(T_ATTRIB_APPL);
    p_ascs_attr_tbl = ble_audio_mem_zalloc(attr_tbl_size);
    ascs_attr_num = attr_tbl_size / sizeof(T_ATTRIB_APPL);

    if (p_ascs_attr_tbl == NULL)
    {
        PROTOCOL_PRINT_ERROR0("create_ascs_table: calloc table fail");
        return 0;
    }
    memcpy(p_ascs_attr_tbl, ascs_attr_tbl, sizeof(ascs_attr_tbl));
    idx += 4;
    for (i = 0; i < ble_audio_db.snk_ase_num; i++)
    {
        memcpy(p_ascs_attr_tbl + idx, ase_attr_tbl, sizeof(ase_attr_tbl));
        idx += 3;
    }

    for (i = 0; i < ble_audio_db.src_ase_num; i++)
    {
        memcpy(p_ascs_attr_tbl + idx, ase_attr_tbl, sizeof(ase_attr_tbl));
        p_ascs_attr_tbl[idx + 1].type_value[0] = LO_WORD(ASCS_UUID_CHAR_SRC_ASE);
        p_ascs_attr_tbl[idx + 1].type_value[1] = HI_WORD(ASCS_UUID_CHAR_SRC_ASE);
        idx += 3;
    }
    return attr_tbl_size;
}


uint8_t ascs_get_ase_id(uint16_t attrib_index, bool cccd)
{
    uint8_t ase_id = 0;
    uint16_t start_handle = ASCS_CHAR_ASE_INDEX;
    if (cccd)
    {
        start_handle = ASCS_CHAR_ASE_CCCD_INDEX;
    }
    if (attrib_index >= start_handle)
    {
        uint16_t index = attrib_index - start_handle;
        ase_id = index / 3 + 1;
        if (index % 3 != 0)
        {
            ase_id = 0;
        }
    }
    return ase_id;
}

bool ascs_send_ase_notify(uint16_t conn_handle, uint16_t cid, uint16_t ase_id, uint8_t *p_data,
                          uint16_t len)
{
    uint16_t attrib_idx;
    T_BLE_AUDIO_LINK *p_link = NULL;

    if (p_data == NULL || ase_id > (ble_audio_db.snk_ase_num + ble_audio_db.src_ase_num) || ase_id == 0)
    {
        return false;
    }

    p_link = ble_audio_link_find_by_conn_handle(conn_handle);
    if (p_link == NULL)
    {
        return false;
    }
    if (len > p_link->mtu_size - 3)
    {
        len = p_link->mtu_size - 3;
    }
    attrib_idx = ASCS_CHAR_ASE_INDEX + 3 * (ase_id - 1);
    return gatt_svc_send_data(conn_handle, cid, ascs_service_id,
                              attrib_idx, p_data,
                              len, GATT_PDU_TYPE_NOTIFICATION);
}

bool ascs_send_ase_cp_notify(uint16_t conn_handle, uint16_t cid, uint8_t *p_data, uint16_t len)
{
    if (p_data == NULL)
    {
        return false;
    }
    PROTOCOL_PRINT_INFO0("ascs_send_ase_cp_notify");
    return gatt_svc_send_data(conn_handle, cid, ascs_service_id, ASCS_CHAR_ASE_CP_INDEX, p_data,
                              len, GATT_PDU_TYPE_NOTIFICATION);
}

T_SERVER_ID ascs_add_service(const T_FUN_GATT_EXT_SERVICE_CBS *p_srv_ext_cbs)
{
    T_SERVER_ID service_id;
    uint16_t attr_tbl_size;

    if (p_ascs_attr_tbl != NULL)
    {
        PROTOCOL_PRINT_ERROR0("ascs_add_service: already register");
        return ascs_service_id;
    }

    attr_tbl_size = create_ascs_table();
    if (attr_tbl_size == 0)
    {
        PROTOCOL_PRINT_ERROR0("ascs_add_service: create table fail");
        service_id = 0xff;
        goto error;
    }

    if (false == gatt_svc_add(&service_id,
                              (uint8_t *)p_ascs_attr_tbl,
                              attr_tbl_size,
                              p_srv_ext_cbs, NULL))
    {
        PROTOCOL_PRINT_ERROR1("ascs_add_service: service_id %d", service_id);
        service_id = 0xff;
        goto error;
    }
    ascs_service_id = service_id;
    return service_id;
error:
    if (p_ascs_attr_tbl)
    {
        ble_audio_mem_free(p_ascs_attr_tbl);
        p_ascs_attr_tbl = NULL;
    }
    return 0xFF;
}
#endif
