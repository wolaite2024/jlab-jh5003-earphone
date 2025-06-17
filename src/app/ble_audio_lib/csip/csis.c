#include <stdint.h>
#include <string.h>
#include "trace.h"
#include "csis.h"
#include "csis_def.h"
#include "set_member.h"
#include "gap_conn_le.h"
#include "ble_audio_ual.h"
#include "ble_link_util.h"

#if LE_AUDIO_CSIS_SUPPORT

/**< @brief  profile/service definition.  */
const T_ATTRIB_APPL csis_attr_tbl[] =
{
    /*----------------- Published Audio Capabilities Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_CSIS),              /* service UUID */
            HI_WORD(GATT_UUID_CSIS)
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
            (GATT_CHAR_PROP_READ)                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(CSIS_UUID_CHAR_SIRK),
            HI_WORD(CSIS_UUID_CHAR_SIRK)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ /* permissions */
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

const T_ATTRIB_APPL csis_size_char_tbl[] =
{
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ)                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(CSIS_UUID_CHAR_SIZE),
            HI_WORD(CSIS_UUID_CHAR_SIZE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ /* permissions */
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

const T_ATTRIB_APPL csis_lock_char_tbl[] =
{
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE |
             GATT_CHAR_PROP_NOTIFY)                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(CSIS_UUID_CHAR_LOCK),
            HI_WORD(CSIS_UUID_CHAR_LOCK)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ  /* permissions */
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

const T_ATTRIB_APPL csis_rank_char_tbl[] =
{
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ)                  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(CSIS_UUID_CHAR_RANK),
            HI_WORD(CSIS_UUID_CHAR_RANK)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ /* permissions */
    },
};

uint16_t csis_create_table(T_SET_MEM *p_entry)
{
    uint16_t idx = 0;
    uint16_t table_size = 0;

    if (p_entry->feature & SET_MEMBER_SIRK_NOTIFY_SUPPORT)
    {
        table_size += 4 * sizeof(T_ATTRIB_APPL);
    }
    else
    {
        table_size += 3 * sizeof(T_ATTRIB_APPL);
    }

    if (p_entry->feature & SET_MEMBER_LOCK_EXIST)
    {
        table_size += sizeof(csis_lock_char_tbl);
    }

    if (p_entry->feature & SET_MEMBER_RANK_EXIST)
    {
        table_size += sizeof(csis_rank_char_tbl);
    }

    if (p_entry->feature & SET_MEMBER_SIZE_EXIST)
    {
        if (p_entry->feature & SET_MEMBER_SIZE_NOTIFY_SUPPORT)
        {
            table_size += 3 * sizeof(T_ATTRIB_APPL);
        }
        else
        {
            table_size += 2 * sizeof(T_ATTRIB_APPL);
        }
    }
    p_entry->p_attr_tbl = ble_audio_mem_zalloc(table_size);
    if (p_entry->p_attr_tbl == NULL)
    {
        PROTOCOL_PRINT_ERROR0("csis_create_table: allocate mem failed");
        return 0;
    }

    if (p_entry->feature & SET_MEMBER_SIRK_NOTIFY_SUPPORT)
    {
        memcpy(p_entry->p_attr_tbl + idx, csis_attr_tbl, 4 * sizeof(T_ATTRIB_APPL));
        p_entry->p_attr_tbl[1].type_value[2] = (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY);
        idx += 4;
    }
    else
    {
        memcpy(p_entry->p_attr_tbl + idx, csis_attr_tbl, 3 * sizeof(T_ATTRIB_APPL));
        idx += 3;
    }
    p_entry->sirk_char_idx = 2;
    if (p_entry->feature & SET_MEMBER_LOCK_EXIST)
    {
        p_entry->lock_char_idx = idx + 1;
        memcpy(p_entry->p_attr_tbl + idx, csis_lock_char_tbl, sizeof(csis_lock_char_tbl));
        idx += 3;

    }

    if (p_entry->feature & SET_MEMBER_RANK_EXIST)
    {
        p_entry->rank_char_idx = idx + 1;
        memcpy(p_entry->p_attr_tbl + idx, csis_rank_char_tbl, sizeof(csis_rank_char_tbl));
        idx += 2;

    }

    if (p_entry->feature & SET_MEMBER_SIZE_EXIST)
    {
        p_entry->size_char_idx = idx + 1;
        if (p_entry->feature & SET_MEMBER_SIZE_NOTIFY_SUPPORT)
        {
            memcpy(p_entry->p_attr_tbl + idx, csis_size_char_tbl, 3 * sizeof(T_ATTRIB_APPL));
            p_entry->p_attr_tbl[idx].type_value[2] = (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY);
            idx += 3;
        }
        else
        {
            memcpy(p_entry->p_attr_tbl + idx, csis_size_char_tbl, 2 * sizeof(T_ATTRIB_APPL));
            idx += 2;
        }
    }

    return table_size;
}

T_SERVER_ID csis_add_service(const T_FUN_GATT_EXT_SERVICE_CBS *p_srv_ext_cbs, T_SET_MEM *p_entry)
{
    uint16_t table_size = 0;

    if (p_entry == NULL)
    {
        return 0xff;
    }
    table_size = csis_create_table(p_entry);

    if (table_size != 0)
    {
        if (false == gatt_svc_add(&p_entry->service_id,
                                  (uint8_t *)p_entry->p_attr_tbl,
                                  table_size,
                                  p_srv_ext_cbs, NULL))
        {
            goto error;
        }
    }
    else
    {
        goto error;
    }
    p_entry->used = true;
    PROTOCOL_PRINT_INFO5("csis_add_service: service_id %d, table_size %d, lock_char_idx %d, rank_char_idx %d, size_char_idx %d",
                         p_entry->service_id,
                         table_size,
                         p_entry->lock_char_idx,
                         p_entry->rank_char_idx,
                         p_entry->size_char_idx);
    return p_entry->service_id;

error:
    if (p_entry->p_attr_tbl != NULL)
    {
        ble_audio_mem_free(p_entry->p_attr_tbl);
        p_entry->p_attr_tbl = NULL;
    }
    PROTOCOL_PRINT_ERROR0("csis_add_service: failed");
    return 0xff;
}
#endif
