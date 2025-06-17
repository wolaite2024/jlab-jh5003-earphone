#include <stdint.h>
#include <string.h>
#include "trace.h"
#include "vocs.h"
#include "vocs_mgr.h"
#include "vocs_def.h"
#include "ble_audio_def.h"
#include "ble_audio_mgr.h"

#if LE_AUDIO_VOCS_SUPPORT
/**< @brief  profile/service definition.  */
const T_ATTRIB_APPL vocs_attr_tbl[] =
{
    /*----------------- Published Audio Capabilities Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_SECONDARY_SERVICE),
            HI_WORD(GATT_UUID_SECONDARY_SERVICE),
            LO_WORD(GATT_UUID_VOCS),              /* service UUID */
            HI_WORD(GATT_UUID_VOCS)
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
            (GATT_CHAR_PROP_READ |                  /* characteristic properties */
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
            LO_WORD(VOCS_UUID_CHAR_VOLUME_OFFSET_STATE),
            HI_WORD(VOCS_UUID_CHAR_VOLUME_OFFSET_STATE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_NOTIF_IND_AUTHEN_REQ /* permissions */
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
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE)                    /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(VOCS_UUID_CHAR_VOLUME_OFFSET_CP),
            HI_WORD(VOCS_UUID_CHAR_VOLUME_OFFSET_CP)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_WRITE_AUTHEN_REQ /* permissions */
    },
};

const T_ATTRIB_APPL vocs_audio_loc_attr_tbl[] =
{
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE_NO_RSP |
             GATT_CHAR_PROP_NOTIFY) /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(VOCS_UUID_CHAR_AUDIO_LOCATION),
            HI_WORD(VOCS_UUID_CHAR_AUDIO_LOCATION)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ /* permissions */
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

const T_ATTRIB_APPL vocs_audio_output_des_attr_tbl[] =
{
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE_NO_RSP |
             GATT_CHAR_PROP_NOTIFY)                    /* characteristic properties */
        },
        1,                                          /* bValueLen */
        NULL,
        (GATT_CHAR_PROP_READ)                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_ASCII_Z, /* flags */
        {                                           /* type_value */
            LO_WORD(VOCS_UUID_CHAR_AUDIO_OUTPUT_DES),
            HI_WORD(VOCS_UUID_CHAR_AUDIO_OUTPUT_DES)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ /* permissions */
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

static bool vocs_create_table(T_VOCS *p_entry, uint8_t feature)
{
    uint16_t idx = 0;
    uint16_t attr_tbl_size = 0;

    attr_tbl_size += sizeof(vocs_attr_tbl);
    attr_tbl_size += 4 * sizeof(T_ATTRIB_APPL);

    if (feature & VOCS_AUDIO_LOCATION_NOTIFY_SUPPORT)
    {
        attr_tbl_size += sizeof(T_ATTRIB_APPL);
    }

    if (feature & VOCS_AUDIO_OUTPUT_DES_NOTIFY_SUPPORT)
    {
        attr_tbl_size += sizeof(T_ATTRIB_APPL);
    }

    p_entry->p_attr_tbl = ble_audio_mem_zalloc(attr_tbl_size);

    if (p_entry->p_attr_tbl == NULL)
    {
        PROTOCOL_PRINT_ERROR0("[VOCS]vocs_create_table: allocate mem failed!");
        return false;
    }

    p_entry->attr_num = attr_tbl_size / sizeof(T_ATTRIB_APPL);
    memcpy(p_entry->p_attr_tbl + idx, vocs_attr_tbl, sizeof(vocs_attr_tbl));
    idx += (sizeof(vocs_attr_tbl) / sizeof(T_ATTRIB_APPL));

    if (feature & VOCS_AUDIO_LOCATION_NOTIFY_SUPPORT)
    {
        memcpy(p_entry->p_attr_tbl + idx, vocs_audio_loc_attr_tbl, sizeof(vocs_audio_loc_attr_tbl));
        idx += (sizeof(vocs_audio_loc_attr_tbl) / sizeof(T_ATTRIB_APPL));
    }
    else
    {
        memcpy(p_entry->p_attr_tbl + idx, vocs_audio_loc_attr_tbl, sizeof(T_ATTRIB_APPL) * 2);
        p_entry->p_attr_tbl[idx].type_value[2] = GATT_CHAR_PROP_READ;
        p_entry->p_attr_tbl[idx + 1].type_value[2] = GATT_PERM_READ_AUTHEN_REQ;
        idx += 2;
    }

    if (feature & VOCS_AUDIO_OUTPUT_DES_NOTIFY_SUPPORT)
    {
        memcpy(p_entry->p_attr_tbl + idx, vocs_audio_output_des_attr_tbl,
               sizeof(vocs_audio_output_des_attr_tbl));
        idx += (sizeof(vocs_audio_output_des_attr_tbl) / sizeof(T_ATTRIB_APPL));
    }
    else
    {
        memcpy(p_entry->p_attr_tbl + idx, vocs_audio_output_des_attr_tbl, sizeof(T_ATTRIB_APPL) * 2);
        p_entry->p_attr_tbl[idx].type_value[2] = GATT_CHAR_PROP_READ;
        p_entry->p_attr_tbl[idx + 1].type_value[2] = GATT_PERM_READ_AUTHEN_REQ;
        idx += 2;
    }

    return true;
}

T_SERVER_ID vocs_add_service(const T_FUN_GATT_EXT_SERVICE_CBS *p_srv_ext_cbs,
                             T_VOCS *p_entry, uint8_t feature)
{
    uint8_t  service_id;

    if (vocs_create_table(p_entry, feature) == false)
    {
        goto error;
    }

    if (false == gatt_svc_add(&service_id,
                              (uint8_t *)p_entry->p_attr_tbl,
                              p_entry->attr_num * sizeof(T_ATTRIB_APPL),
                              p_srv_ext_cbs, NULL))
    {
        goto error;
    }

    p_entry->feature = feature;
    return service_id;

error:
    PROTOCOL_PRINT_ERROR0("[VOCS]vocs_add_service: failed");
    if (p_entry->p_attr_tbl)
    {
        ble_audio_mem_free(p_entry->p_attr_tbl);
        p_entry->attr_num = 0;
    }
    return 0xff;
}
#endif
