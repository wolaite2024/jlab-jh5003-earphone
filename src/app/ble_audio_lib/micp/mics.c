#include <string.h>
#include "trace.h"
#include "aics.h"
#include "mics_def.h"
#include "mics.h"
#include "bt_gatt_svc.h"
#include "ble_audio_mgr.h"

#if LE_AUDIO_MICS_SUPPORT

/**< @brief  profile/service definition.  */
static const T_ATTRIB_APPL mics_primary_srv_attr[] =
{
    /*----------------- Published Audio Capabilities Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_MICS),              /* service UUID */
            HI_WORD(GATT_UUID_MICS)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    }
};

static const T_ATTRIB_APPL mics_char_attr[] =
{
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                  /* characteristic properties */
             GATT_CHAR_PROP_WRITE |
             GATT_CHAR_PROP_NOTIFY)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MICS_UUID_CHAR_MUTE),
            HI_WORD(MICS_UUID_CHAR_MUTE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ |
        GATT_PERM_WRITE_AUTHEN_REQ |
        GATT_PERM_NOTIF_IND_AUTHEN_REQ /* permissions */
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
    }
};

T_SERVER_ID mics_add_service(const T_FUN_GATT_EXT_SERVICE_CBS *p_srv_ext_cbs, uint8_t aics_num,
                             uint8_t *aics_id_array)
{
    T_SERVER_ID service_id;
    uint16_t idx = 0;
    uint16_t table_size = sizeof(mics_primary_srv_attr);

#if LE_AUDIO_AICS_SUPPORT
    if (aics_num != 0)
    {
        table_size += (aics_num * sizeof(T_ATTRIB_APPL));
    }
#endif
    table_size += sizeof(mics_char_attr);
    p_mgr_mics->p_mics_attr_tbl = ble_audio_mem_zalloc(table_size);
    if (p_mgr_mics->p_mics_attr_tbl == NULL)
    {
        PROTOCOL_PRINT_ERROR0("mics_create_table: allocate mem failed");
        goto error;
    }
    p_mgr_mics->attr_num += 4;
    memcpy(p_mgr_mics->p_mics_attr_tbl, mics_primary_srv_attr, sizeof(mics_primary_srv_attr));
    idx++;
#if LE_AUDIO_AICS_SUPPORT
    if (aics_num != 0)
    {
        for (uint8_t i = 0; i < aics_num; i++)
        {
            incl_aics_attr->value_len = aics_id_array[i];
            memcpy(p_mgr_mics->p_mics_attr_tbl + idx, incl_aics_attr, sizeof(T_ATTRIB_APPL));
            idx++;
        }
        p_mgr_mics->attr_num += aics_num;
    }
#endif
    memcpy(p_mgr_mics->p_mics_attr_tbl + idx, mics_char_attr, sizeof(mics_char_attr));


    if (false == gatt_svc_add(&service_id,
                              (uint8_t *)p_mgr_mics->p_mics_attr_tbl,
                              table_size,
                              p_srv_ext_cbs, NULL))
    {
        PROTOCOL_PRINT_ERROR1("mics_add_service: service_id %d", service_id);
        service_id = 0xff;
        goto error;
    }
#if LE_AUDIO_INIT_DEBUG
    PROTOCOL_PRINT_INFO3("mics_create_table: aics_num %d, table_size %d, idx %d",
                         aics_num, table_size, idx);
#endif
    return service_id;
error:
    return 0xFF;
}
#endif
