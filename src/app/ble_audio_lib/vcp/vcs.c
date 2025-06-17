#include <stdint.h>
#include <string.h>
#include "trace.h"
#include "vcs.h"
#include "vocs.h"
#include "aics.h"
#include "vcs_def.h"
#include "ble_audio_mgr.h"

#if LE_AUDIO_VCS_SUPPORT

/**< @brief  profile/service definition.  */
static const T_ATTRIB_APPL vcs_primary_srv_attr[] =
{
    /*----------------- Volume Control Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_VCS),              /* service UUID */
            HI_WORD(GATT_UUID_VCS)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    }
};

#if LE_AUDIO_VOCS_SUPPORT
static T_ATTRIB_APPL vcs_incl_vocs_attr[] =
{
    /** Include Definition*/
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_INCLUDE_MULTI), /**< flags */
        {                                           /**< type_value */
            LO_WORD(GATT_UUID_INCLUDE),
            HI_WORD(GATT_UUID_INCLUDE),
        },
        0,
        ((uint8_t *)&vocs_attr_tbl[0]),
        GATT_PERM_READ                              /**< permissions */
    },
};
#endif

static const T_ATTRIB_APPL vcs_char_attr[] =
{
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
            LO_WORD(VCS_UUID_CHAR_VOLUME_STATE),
            HI_WORD(VCS_UUID_CHAR_VOLUME_STATE)
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
            LO_WORD(VCS_UUID_CHAR_VOLUME_CP),
            HI_WORD(VCS_UUID_CHAR_VOLUME_CP)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_WRITE_AUTHEN_REQ /* permissions */
    },
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
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(VCS_UUID_CHAR_VOLUME_FLAGS),
            HI_WORD(VCS_UUID_CHAR_VOLUME_FLAGS)
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
    }
};

T_SERVER_ID vcs_add_service(const T_FUN_GATT_EXT_SERVICE_CBS *p_srv_ext_cbs,
                            uint8_t vocs_num, uint8_t aics_num, uint8_t *id_array)
{
    T_SERVER_ID service_id;
    uint16_t idx = 0;
    uint16_t table_size = sizeof(vcs_primary_srv_attr);

#if LE_AUDIO_VOCS_SUPPORT
    if (vocs_num != 0)
    {
        for (uint8_t i = 0; i < vocs_num; i++)
        {
            T_VOCS *p_entry = vocs_find_by_idx(i);
            if (p_entry == NULL)
            {
                PROTOCOL_PRINT_ERROR1("vcs_create_table: not find vocs entry %d", i);
                continue;
            }
            table_size += sizeof(T_ATTRIB_APPL);
        }
    }
#endif
#if LE_AUDIO_AICS_SUPPORT
    if (aics_num != 0)
    {
        table_size += (aics_num * sizeof(T_ATTRIB_APPL));
    }
#endif
    table_size += sizeof(vcs_char_attr);
    p_mgr_vcs->p_vcs_attr_tbl = ble_audio_mem_zalloc(table_size);
    if (p_mgr_vcs->p_vcs_attr_tbl == NULL)
    {
        PROTOCOL_PRINT_ERROR0("vcs_create_table: allocate mem failed");
        return 0xff;
    }
    memcpy(p_mgr_vcs->p_vcs_attr_tbl, vcs_primary_srv_attr, sizeof(vcs_primary_srv_attr));
    idx++;
#if LE_AUDIO_VOCS_SUPPORT
    if (vocs_num != 0)
    {
        for (uint8_t i = 0; i < vocs_num; i++)
        {
            T_VOCS *p_entry = vocs_find_by_idx(i);
            vcs_incl_vocs_attr->value_len = 0;
            if (p_entry == NULL)
            {
                PROTOCOL_PRINT_ERROR1("vcs_create_table: not find vocs entry %d", i);
                continue;
            }
            vcs_incl_vocs_attr->p_value_context = (void *)p_entry->p_attr_tbl;
            memcpy(p_mgr_vcs->p_vcs_attr_tbl + idx, vcs_incl_vocs_attr, sizeof(vcs_incl_vocs_attr));
            idx++;
        }
    }
#endif
#if LE_AUDIO_AICS_SUPPORT
    if (aics_num != 0)
    {
        for (uint8_t i = 0; i < aics_num; i++)
        {
            incl_aics_attr->value_len = id_array[i];
            memcpy(p_mgr_vcs->p_vcs_attr_tbl + idx, incl_aics_attr, sizeof(T_ATTRIB_APPL));
            idx++;
        }
    }
#endif
    p_mgr_vcs->char_attr_offset = idx;
    memcpy(p_mgr_vcs->p_vcs_attr_tbl + idx, vcs_char_attr, sizeof(vcs_char_attr));
    p_mgr_vcs->attr_num = table_size / sizeof(T_ATTRIB_APPL);

    if (false == gatt_svc_add(&service_id,
                              (uint8_t *)p_mgr_vcs->p_vcs_attr_tbl,
                              table_size,
                              p_srv_ext_cbs, NULL))
    {
        PROTOCOL_PRINT_ERROR1("vcs_add_service: service_id %d", service_id);
        service_id = 0xff;
    }
#if LE_AUDIO_INIT_DEBUG
    PROTOCOL_PRINT_INFO4("vcs_create_table: vocs_num %d, aics_num %d, table_size %d, char_attr_offset %d",
                         vocs_num, aics_num,
                         table_size,
                         p_mgr_vcs->char_attr_offset);
#endif
    return service_id;
}
#endif
