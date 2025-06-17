#include <string.h>
#include "aics.h"
#include "aics_def.h"

#if LE_AUDIO_AICS_SUPPORT
/**< @brief  profile/service definition.  */
const T_ATTRIB_APPL aics_attr_tbl[] =
{
    /*----------------- Audio Input Control Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_SECONDARY_SERVICE),
            HI_WORD(GATT_UUID_SECONDARY_SERVICE),
            LO_WORD(GATT_UUID_AICS),              /* service UUID */
            HI_WORD(GATT_UUID_AICS)
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
            LO_WORD(AICS_UUID_CHAR_INPUT_STATE),
            HI_WORD(AICS_UUID_CHAR_INPUT_STATE)
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
            (GATT_CHAR_PROP_READ)                    /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL, /* flags */
        {                                           /* type_value */
            LO_WORD(AICS_UUID_CHAR_GAIN_SETTING_PROP),
            HI_WORD(AICS_UUID_CHAR_GAIN_SETTING_PROP)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ /* permissions */
    },
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ)                    /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL, /* flags */
        {                                           /* type_value */
            LO_WORD(AICS_UUID_CHAR_INPUT_TYPE),
            HI_WORD(AICS_UUID_CHAR_INPUT_TYPE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ /* permissions */
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
            LO_WORD(AICS_UUID_CHAR_INPUT_STATUS),
            HI_WORD(AICS_UUID_CHAR_INPUT_STATUS)
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
            LO_WORD(AICS_UUID_CHAR_INPUT_CP),
            HI_WORD(AICS_UUID_CHAR_INPUT_CP)
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
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE_NO_RSP |    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY)
            //fixme, optional properties, write without response
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL | ATTRIB_FLAG_ASCII_Z, /* flags */
        {                                           /* type_value */
            LO_WORD(AICS_UUID_CHAR_INPUT_DES),
            HI_WORD(AICS_UUID_CHAR_INPUT_DES)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ |
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


T_ATTRIB_APPL incl_aics_attr[] =
{
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_INCLUDE_MULTI),                   /**< flags */
        {                                           /**< type_value */
            LO_WORD(GATT_UUID_INCLUDE),
            HI_WORD(GATT_UUID_INCLUDE),
        },
        0,
        ((uint8_t *)&aics_attr_tbl[0]),
        GATT_PERM_READ                              /**< permissions */
    }
};

T_SERVER_ID aics_add_service(const T_FUN_GATT_EXT_SERVICE_CBS *p_srv_ext_cbs)
{
    uint8_t  service_id;

    if (false == gatt_svc_add(&service_id,
                              (uint8_t *)aics_attr_tbl,
                              sizeof(aics_attr_tbl),
                              p_srv_ext_cbs, NULL))
    {
        return 0xff;
    }
    return service_id;
}
#endif
