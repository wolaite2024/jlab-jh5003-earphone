#include <stdint.h>
#include <string.h>
#include "trace.h"
#include "bt_gatt_svc.h"
#include "pacs.h"
#include "pacs_def.h"
#include "pac_mgr.h"
#include "ble_audio_mgr.h"

#if LE_AUDIO_PACS_SUPPORT

/**< @brief  profile/service definition.  */
static const T_ATTRIB_APPL pacs_tbl_srv[] =
{
    /*----------------- Published Audio Capabilities Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_PACS),              /* service UUID */
            HI_WORD(GATT_UUID_PACS)
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
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* Audio Contexts Availability value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(PACS_UUID_CHAR_AUDIO_AVAILABILITY_CONTEXTS),
            HI_WORD(PACS_UUID_CHAR_AUDIO_AVAILABILITY_CONTEXTS)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ                   /* permissions */
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
        ATTRIB_FLAG_VOID,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        &pacs_supported_audio_contexts_char_prop,
        GATT_PERM_READ                              /* permissions */
    },
    /* Audio Contexts Availability value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(PACS_UUID_CHAR_SUPPORTED_AUDIO_CONTEXTS),
            HI_WORD(PACS_UUID_CHAR_SUPPORTED_AUDIO_CONTEXTS)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ                   /* permissions */
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

static const T_ATTRIB_APPL pacs_tbl_sink_pac[] =
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
    /* Sink PAC value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(PACS_UUID_CHAR_SINK_PAC),
            HI_WORD(PACS_UUID_CHAR_SINK_PAC)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ                   /* permissions */
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
    }
};

static const T_ATTRIB_APPL pacs_tbl_sink_audio_locations[] =
{
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VOID,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        &pacs_sink_audio_locations_char_prop,
        GATT_PERM_READ                              /* permissions */
    },
    /* Sink audio loactions value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(PACS_UUID_CHAR_SINK_AUDIO_LOCATIONS),
            HI_WORD(PACS_UUID_CHAR_SINK_AUDIO_LOCATIONS)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ                   /* permissions */
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
    }
};

static const T_ATTRIB_APPL pacs_tbl_source_pac[] =
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
    /* Source PAC value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(PACS_UUID_CHAR_SOURCE_PAC),
            HI_WORD(PACS_UUID_CHAR_SOURCE_PAC)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ                   /* permissions */
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
    }
};

static const T_ATTRIB_APPL pacs_tbl_source_audio_locations[] =
{
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VOID,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        &pacs_source_audio_locations_char_prop,
        GATT_PERM_READ                              /* permissions */
    },
    /* Source audio locations value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(PACS_UUID_CHAR_SOURCE_AUDIO_LOCATIONS),
            HI_WORD(PACS_UUID_CHAR_SOURCE_AUDIO_LOCATIONS)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ                   /* permissions */
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
    }
};

uint16_t pacs_create_table(void)
{
    uint16_t idx = 0;
    uint16_t attrib_num = 0;
    T_PAC_RECORD_LIST *p_loop;
    uint16_t pacs_attr_tbl_size = 0;

    if (pacs_supported_audio_contexts_char_prop & GATT_CHAR_PROP_NOTIFY)
    {
        attrib_num += 7;
    }
    else
    {
        attrib_num += 6;
    }
    attrib_num += pac_char_attrib_num;
    if (sink_pac_num != 0 && pacs_sink_audio_locations_char_prop != 0)
    {
        attrib_num += 2;
        if (pacs_sink_audio_locations_char_prop & GATT_CHAR_PROP_NOTIFY)
        {
            attrib_num++;
        }
    }
    if (source_pac_num != 0 && pacs_source_audio_locations_char_prop != 0)
    {
        attrib_num += 2;
        if (pacs_source_audio_locations_char_prop & GATT_CHAR_PROP_NOTIFY)
        {
            attrib_num++;
        }
    }
    pacs_attr_tbl_size = attrib_num * sizeof(T_ATTRIB_APPL);
    p_pacs->p_pacs_attr_tbl = ble_audio_mem_zalloc(pacs_attr_tbl_size);
    p_pacs->attr_num = attrib_num;
    if (p_pacs->p_pacs_attr_tbl == NULL)
    {
        PROTOCOL_PRINT_ERROR0("pacs_create_table: p_pacs_attr_tbl allocate failed");
        return 0;
    }
#if LE_AUDIO_INIT_DEBUG
    PROTOCOL_PRINT_INFO3("pacs_create_table: sizeof(T_ATTRIB_APPL) %d, attrib_num %d, pacs_attr_tbl_size %d",
                         sizeof(T_ATTRIB_APPL), p_pacs->attr_num, pacs_attr_tbl_size);
#endif
    if (pacs_supported_audio_contexts_char_prop & GATT_CHAR_PROP_NOTIFY)
    {
        attrib_num = 7;
        memcpy(p_pacs->p_pacs_attr_tbl, pacs_tbl_srv, attrib_num * sizeof(T_ATTRIB_APPL));

    }
    else
    {
        attrib_num = 6;
        memcpy(p_pacs->p_pacs_attr_tbl, pacs_tbl_srv, attrib_num * sizeof(T_ATTRIB_APPL));
    }
    idx += attrib_num;

    if (sink_pac_num != 0)
    {
        if (pacs_sink_audio_locations_char_prop & GATT_CHAR_PROP_NOTIFY)
        {
            memcpy(p_pacs->p_pacs_attr_tbl + idx, pacs_tbl_sink_audio_locations, 3 * sizeof(T_ATTRIB_APPL));
            p_pacs->sink_audio_loc_idx = idx + 1;
            p_pacs->sink_audio_loc_cccd_idx = idx + 2;
            p_pacs->sink_pac_start_idx = idx + 3;
            if (pacs_sink_audio_locations_char_prop & GATT_CHAR_PROP_WRITE)
            {
                p_pacs->p_pacs_attr_tbl[p_pacs->sink_audio_loc_idx].permissions |= GATT_PERM_WRITE_AUTHEN_REQ;
            }
            idx += 3;
        }
        else
        {
            if (pacs_sink_audio_locations_char_prop == 0)
            {
                p_pacs->sink_audio_loc_idx = 0xff;
                p_pacs->sink_audio_loc_cccd_idx = 0xff;
                p_pacs->sink_pac_start_idx = idx;
            }
            else
            {
                memcpy(p_pacs->p_pacs_attr_tbl + idx, pacs_tbl_sink_audio_locations, 2 * sizeof(T_ATTRIB_APPL));
                p_pacs->sink_audio_loc_idx = idx + 1;
                p_pacs->sink_audio_loc_cccd_idx = 0xff;
                p_pacs->sink_pac_start_idx = idx + 2;
                if (pacs_sink_audio_locations_char_prop & GATT_CHAR_PROP_WRITE)
                {
                    p_pacs->p_pacs_attr_tbl[p_pacs->sink_audio_loc_idx].permissions |= GATT_PERM_WRITE_AUTHEN_REQ;
                }
                idx += 2;
            }

        }
        p_loop = p_pac_list;
        while (p_loop != NULL)
        {
            if (p_loop->pac_record.direction == SERVER_AUDIO_SINK)
            {
                if (p_loop->pac_record.notify)
                {
                    memcpy(p_pacs->p_pacs_attr_tbl + idx, pacs_tbl_sink_pac, sizeof(pacs_tbl_sink_pac));
                    p_loop->pac_record.attrib_idx = idx + 1;
                    p_loop->pac_record.attrib_cccd_idx = idx + 2;
                    p_pacs->p_pacs_attr_tbl[idx].type_value[2] = (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY);
                    idx += 3;
                }
                else
                {
                    memcpy(p_pacs->p_pacs_attr_tbl + idx, pacs_tbl_sink_pac, 2 * sizeof(T_ATTRIB_APPL));
                    p_loop->pac_record.attrib_idx = idx + 1;
                    p_loop->pac_record.attrib_cccd_idx = 0xff;
                    p_pacs->p_pacs_attr_tbl[idx].type_value[2] = (GATT_CHAR_PROP_READ);
                    idx += 2;
                }
            }
            p_loop = p_loop->next;
        }
        p_pacs->sink_pac_end_idx = idx - 1;
    }
    if (source_pac_num != 0)
    {
        if (pacs_source_audio_locations_char_prop & GATT_CHAR_PROP_NOTIFY)
        {
            memcpy(p_pacs->p_pacs_attr_tbl + idx, pacs_tbl_source_audio_locations, 3 * sizeof(T_ATTRIB_APPL));
            p_pacs->source_audio_loc_idx = idx + 1;
            p_pacs->source_audio_loc_cccd_idx = idx + 2;
            p_pacs->source_pac_start_idx = idx + 3;
            if (pacs_source_audio_locations_char_prop & GATT_CHAR_PROP_WRITE)
            {
                p_pacs->p_pacs_attr_tbl[p_pacs->source_audio_loc_idx].permissions |= GATT_PERM_WRITE_AUTHEN_REQ;
            }
            idx += 3;
        }
        else
        {
            if (pacs_source_audio_locations_char_prop == 0)
            {
                p_pacs->source_audio_loc_idx = 0xff;
                p_pacs->source_audio_loc_cccd_idx = 0xff;
                p_pacs->source_pac_start_idx = idx;
            }
            else
            {
                memcpy(p_pacs->p_pacs_attr_tbl + idx, pacs_tbl_source_audio_locations, 2 * sizeof(T_ATTRIB_APPL));
                p_pacs->source_audio_loc_idx = idx + 1;
                p_pacs->source_audio_loc_cccd_idx = 0xff;
                p_pacs->source_pac_start_idx = idx + 2;
                if (pacs_source_audio_locations_char_prop & GATT_CHAR_PROP_WRITE)
                {
                    p_pacs->p_pacs_attr_tbl[p_pacs->source_audio_loc_idx].permissions |= GATT_PERM_WRITE_AUTHEN_REQ;
                }
                idx += 2;
            }
        }
        p_loop = p_pac_list;
        while (p_loop != NULL)
        {
            if (p_loop->pac_record.direction == SERVER_AUDIO_SOURCE)
            {
                if (p_loop->pac_record.notify)
                {
                    memcpy(p_pacs->p_pacs_attr_tbl + idx, pacs_tbl_source_pac, sizeof(pacs_tbl_source_pac));
                    p_loop->pac_record.attrib_idx = idx + 1;
                    p_loop->pac_record.attrib_cccd_idx = idx + 2;
                    p_pacs->p_pacs_attr_tbl[idx].type_value[2] = (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY);
                    idx += 3;
                }
                else
                {
                    memcpy(p_pacs->p_pacs_attr_tbl + idx, pacs_tbl_source_pac, 2 * sizeof(T_ATTRIB_APPL));
                    p_loop->pac_record.attrib_idx = idx + 1;
                    p_loop->pac_record.attrib_cccd_idx = 0xff;
                    p_pacs->p_pacs_attr_tbl[idx].type_value[2] = (GATT_CHAR_PROP_READ);
                    idx += 2;
                }
            }
            p_loop = p_loop->next;
        }
        p_pacs->source_pac_end_idx = idx - 1;
    }
#if LE_AUDIO_INIT_DEBUG
    PROTOCOL_PRINT_INFO5("pacs_create_table: sink num %d, sink_audio_loc_idx %d, sink_audio_loc_cccd_idx %d, sink_pac_start_idx %d, sink_pac_end_idx %d",
                         sink_pac_num,
                         p_pacs->sink_audio_loc_idx,
                         p_pacs->sink_audio_loc_cccd_idx,
                         p_pacs->sink_pac_start_idx,
                         p_pacs->sink_pac_end_idx);
    PROTOCOL_PRINT_INFO5("pacs_create_table:source num %d, source_audio_loc_idx %d, source_audio_loc_cccd_idx %d, source_pac_start_idx %d, source_pac_end_idx %d",
                         source_pac_num,
                         p_pacs->source_audio_loc_idx,
                         p_pacs->source_audio_loc_cccd_idx,
                         p_pacs->source_pac_start_idx,
                         p_pacs->source_pac_end_idx);
#endif
    return pacs_attr_tbl_size;
}

bool pacs_add_service(const T_FUN_GATT_EXT_SERVICE_CBS *p_srv_ext_cbs)
{
    T_SERVER_ID service_id;
    uint16_t pacs_attr_tbl_size = pacs_create_table();

    if (pacs_attr_tbl_size == 0)
    {
        ble_audio_mem_free(p_pacs);
        p_pacs = NULL;
        return false;
    }
    if (false == gatt_svc_add(&service_id,
                              (uint8_t *)p_pacs->p_pacs_attr_tbl,
                              pacs_attr_tbl_size,
                              p_srv_ext_cbs, NULL))
    {
        PROTOCOL_PRINT_ERROR1("pacs_add_service: service_id %d", service_id);
        ble_audio_mem_free(p_pacs->p_pacs_attr_tbl);
        ble_audio_mem_free(p_pacs);
        p_pacs = NULL;
        return false;
    }
    p_pacs->service_id = service_id;
    return true;
}
#endif
