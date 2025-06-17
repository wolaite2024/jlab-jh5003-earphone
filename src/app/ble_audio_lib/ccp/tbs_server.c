#include <stdint.h>
#include <string.h>
#include "trace.h"
#include "tbs_server.h"
#include "ccp_def.h"
#include "gap_conn_le.h"
#include "ble_audio_mgr.h"
#include "ble_link_util.h"
#include "metadata_def.h"

#if LE_AUDIO_CCP_SERVER_SUPPORT
/**< @brief  profile/service definition.  */
const T_ATTRIB_APPL gtbs_attr_tbl[] =
{
    /*----------------- telepone bearer Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_GENERIC_TELEPHONE_BEARER_SERVICE),              /* service UUID */
            HI_WORD(GATT_UUID_GENERIC_TELEPHONE_BEARER_SERVICE)
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
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY)   /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                            /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TBS_UUID_CHAR_BEARER_PROVIDER_NAME),
            HI_WORD(TBS_UUID_CHAR_BEARER_PROVIDER_NAME)
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
            LO_WORD(TBS_UUID_CHAR_BEARER_UCI),
            HI_WORD(TBS_UUID_CHAR_BEARER_UCI)
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
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY)   /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TBS_UUID_CHAR_BEARER_TECHNOLOGY),
            HI_WORD(TBS_UUID_CHAR_BEARER_TECHNOLOGY)
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

    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY) /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TBS_UUID_CHAR_BEARER_LIST_CURRENT_CALLS),
            HI_WORD(TBS_UUID_CHAR_BEARER_LIST_CURRENT_CALLS)
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
            LO_WORD(TBS_UUID_CHAR_CONTENT_CONTROL_ID),
            HI_WORD(TBS_UUID_CHAR_CONTENT_CONTROL_ID)
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
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY) /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TBS_UUID_CHAR_STATUS_FLAGS),
            HI_WORD(TBS_UUID_CHAR_STATUS_FLAGS)
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

    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY) /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TBS_UUID_CHAR_CALL_STATE),
            HI_WORD(TBS_UUID_CHAR_CALL_STATE)
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

    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE | GATT_CHAR_PROP_NOTIFY |
             GATT_CHAR_PROP_WRITE_NO_RSP) /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                             /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TBS_UUID_CHAR_CALL_CONTROL_POINT),
            HI_WORD(TBS_UUID_CHAR_CALL_CONTROL_POINT)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_WRITE_AUTHEN_REQ /* permissions */
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
            (GATT_CHAR_PROP_READ) /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TBS_UUID_CHAR_CALL_CONTROL_POINT_OPTIONAL_OPCODES),
            HI_WORD(TBS_UUID_CHAR_CALL_CONTROL_POINT_OPTIONAL_OPCODES)
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
            (GATT_CHAR_PROP_NOTIFY)                 /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                             /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TBS_UUID_CHAR_TERMINATION_REASON),
            HI_WORD(TBS_UUID_CHAR_TERMINATION_REASON)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NONE /* permissions */
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
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY) /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                             /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TBS_UUID_CHAR_INCOMING_CALL),
            HI_WORD(TBS_UUID_CHAR_INCOMING_CALL)
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

const T_ATTRIB_APPL bearer_signal_char_tbl[] =
{
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY) /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                             /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TBS_UUID_CHAR_BEARER_SIGNAL_STRENGTH),
            HI_WORD(TBS_UUID_CHAR_BEARER_SIGNAL_STRENGTH)
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

    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_WRITE |
             GATT_CHAR_PROP_WRITE_NO_RSP) /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                             /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TBS_UUID_CHAR_BEARER_SIGNAL_STRENGTH_REPORTING_INTERVAL),
            HI_WORD(TBS_UUID_CHAR_BEARER_SIGNAL_STRENGTH_REPORTING_INTERVAL)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ | GATT_PERM_WRITE_AUTHEN_REQ /* permissions */
    },
};

const T_ATTRIB_APPL bearer_uri_schemes_supported_list_char_tbl[] =
{
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY)  /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TBS_UUID_CHAR_BEARER_URI_SCHEMES_SUPPORTED_LIST),
            HI_WORD(TBS_UUID_CHAR_BEARER_URI_SCHEMES_SUPPORTED_LIST)
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

const T_ATTRIB_APPL incoming_call_tg_uri_char_tbl[] =
{
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY) /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                             /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TBS_UUID_CHAR_INCOMING_CALL_TARGET_BEARER_URI),
            HI_WORD(TBS_UUID_CHAR_INCOMING_CALL_TARGET_BEARER_URI)
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

const T_ATTRIB_APPL call_friendly_name_char_tbl[] =
{
    /* <<Characteristic>>, .. */
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ | GATT_CHAR_PROP_NOTIFY) /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                             /* permissions */
    },
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TBS_UUID_CHAR_CALL_FRIENDLY_NAME),
            HI_WORD(TBS_UUID_CHAR_CALL_FRIENDLY_NAME)
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


T_CALL_STATE *tbs_get_call_state(T_TBS_CB *p_entry, uint8_t call_idx)
{
    int i;
    T_CALL_STATE *p_call_state = NULL;
    for (i = 0; i < p_entry->call_state_queue.count; i++)
    {
        p_call_state = (T_CALL_STATE *)os_queue_peek(&p_entry->call_state_queue, i);
        if (call_idx == p_call_state->call_idx)
        {
            return p_call_state;
        }
    }
    return NULL;
}

uint16_t tbs_calculate_call_list_size(T_TBS_CB *p_entry)
{
    int i;
    uint16_t total_size = 0;
    T_CALL_STATE *p_call_state;
    for (i = 0; i < p_entry->call_state_queue.count; i++)
    {
        p_call_state = (T_CALL_STATE *)os_queue_peek(&p_entry->call_state_queue, i);
        total_size += 4 + p_call_state->uri_len;
    }
    return total_size;
}

bool tbs_send_notify(T_TBS_CB *p_entry, uint16_t conn_handle, uint16_t attrib_idx,
                     uint8_t *p_data, uint16_t data_len)
{
    PROTOCOL_PRINT_INFO3("[CCP]tbs_send_notify: conn_handle 0x%x, attrib_idx 0x%x, data_len %d",
                         conn_handle, attrib_idx, data_len);

    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);

    if (p_entry == NULL || p_link == NULL ||
        (data_len != 0 && p_data == NULL))
    {
        return false;
    }

    if (data_len > (p_link->mtu_size - 3))
    {
        data_len = p_link->mtu_size - 3;
    }
    return gatt_svc_send_data(conn_handle, L2C_FIXED_CID_ATT, p_entry->service_id, attrib_idx,
                              p_data,
                              data_len, GATT_PDU_TYPE_NOTIFICATION);

}

T_APP_RESULT tbs_attr_read_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                              uint16_t attrib_index,
                              uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_TBS_CB *p_entry = tbs_find_by_srv_id(service_id);
    T_CCP_SERVER_READ_IND read_ind;

    if (p_entry == NULL)
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_entry->p_attr_tbl, attrib_index,
                                                             p_entry->attr_num);

    *p_length = 0;
    PROTOCOL_PRINT_INFO2("[CCP]tbs_attr_read_cb: attrib_index %d, offset %x", attrib_index, offset);

    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR1("[CCP]tbs_attr_read_cb Error: attrib_index 0x%x", attrib_index);
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    read_ind.conn_handle = conn_handle;
    read_ind.cid = cid;
    read_ind.service_id = service_id;
    read_ind.char_uuid = char_uuid.uu.char_uuid16;
    read_ind.offset = offset;

    switch (char_uuid.uu.char_uuid16)
    {
    case TBS_UUID_CHAR_BEARER_PROVIDER_NAME:
    case TBS_UUID_CHAR_BEARER_UCI:
    case TBS_UUID_CHAR_BEARER_TECHNOLOGY:
    case TBS_UUID_CHAR_BEARER_URI_SCHEMES_SUPPORTED_LIST:
    case TBS_UUID_CHAR_BEARER_SIGNAL_STRENGTH:
    case TBS_UUID_CHAR_BEARER_SIGNAL_STRENGTH_REPORTING_INTERVAL:
    case TBS_UUID_CHAR_CONTENT_CONTROL_ID:
    case TBS_UUID_CHAR_STATUS_FLAGS:
        {
            cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_CCP_SERVER_READ_IND, &read_ind);

            if (cause == BLE_AUDIO_CB_RESULT_SUCCESS)
            {
                cause = (T_APP_RESULT)BLE_AUDIO_CB_RESULT_APP_ERR;
            }
        }
        break;

    case TBS_UUID_CHAR_CALL_CONTROL_POINT_OPTIONAL_OPCODES:
        cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_CCP_SERVER_READ_IND, &read_ind);

        *pp_value = (uint8_t *)&p_entry->call_control_point_optional_opcodes;
        *p_length = 2;

        if (cause == BLE_AUDIO_CB_RESULT_PENDING)
        {
            cause = (T_APP_RESULT)BLE_AUDIO_CB_RESULT_APP_ERR;
        }
        break;

    case TBS_UUID_CHAR_BEARER_LIST_CURRENT_CALLS:
        {
            int i;
            T_CALL_STATE *p_call_state;
            uint16_t call_state_list_size = tbs_calculate_call_list_size(p_entry);
            uint8_t read_data[call_state_list_size];
            uint16_t idx = 0;

            cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_CCP_SERVER_READ_IND, &read_ind);

            if (offset > call_state_list_size)
            {
                cause = APP_RESULT_INVALID_OFFSET;
                break;
            }

            if (cause != APP_RESULT_SUCCESS)
            {
                if (cause == BLE_AUDIO_CB_RESULT_PENDING)
                {
                    cause = (T_APP_RESULT)BLE_AUDIO_CB_RESULT_APP_ERR;
                }

                break;
            }

            for (i = 0; i < p_entry->call_state_queue.count; i++)
            {
                p_call_state = (T_CALL_STATE *)os_queue_peek(&p_entry->call_state_queue, i);
                read_data[idx++] = 3 + p_call_state->uri_len;
                read_data[idx++] = p_call_state->call_idx;
                read_data[idx++] = p_call_state->call_state;
                read_data[idx++] = p_call_state->call_flags;
                memcpy(&read_data[idx], p_call_state->call_uri, p_call_state->uri_len);
                idx += p_call_state->uri_len;
            }

            if (gatt_svc_read_confirm(conn_handle, cid, service_id, attrib_index,
                                      read_data + offset, call_state_list_size - offset,
                                      call_state_list_size, APP_RESULT_SUCCESS))
            {
                cause = APP_RESULT_PENDING;
            }
            else
            {
                cause = APP_RESULT_APP_ERR;
            }
        }
        break;

    case TBS_UUID_CHAR_INCOMING_CALL_TARGET_BEARER_URI:
        {
            T_CALL_STATE *p_call_state = tbs_get_call_state(p_entry, p_entry->in_call_tg_uri_cb.call_index);

            if (p_call_state == NULL)
            {
                return (T_APP_RESULT)BLE_AUDIO_CB_RESULT_APP_ERR;
            }

            cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_CCP_SERVER_READ_IND, &read_ind);

            if (offset > (1 + p_entry->in_call_tg_uri_cb.incoming_call_target_uri_len))
            {
                cause = APP_RESULT_INVALID_OFFSET;
                break;
            }

            if (cause != APP_RESULT_SUCCESS)
            {
                if (cause == BLE_AUDIO_CB_RESULT_PENDING)
                {
                    cause = (T_APP_RESULT)BLE_AUDIO_CB_RESULT_APP_ERR;
                }

                break;
            }

            uint8_t read_data[1 + p_entry->in_call_tg_uri_cb.incoming_call_target_uri_len];
            read_data[0] = p_entry->in_call_tg_uri_cb.call_index;
            memcpy(&read_data[1], p_entry->in_call_tg_uri_cb.p_incoming_call_target_uri,
                   p_entry->in_call_tg_uri_cb.incoming_call_target_uri_len);

            if (gatt_svc_read_confirm(conn_handle, cid, service_id, attrib_index,
                                      read_data + offset, sizeof(read_data) - offset,
                                      sizeof(read_data), APP_RESULT_SUCCESS))
            {
                cause = APP_RESULT_PENDING;
            }
            else
            {
                cause = APP_RESULT_APP_ERR;
            }
        }
        break;

    case TBS_UUID_CHAR_CALL_STATE:
        {
            int i;
            T_CALL_STATE *p_call_state;
            uint8_t read_data[3 * p_entry->call_state_queue.count];
            uint16_t idx = 0;

            cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_CCP_SERVER_READ_IND, &read_ind);

            if (offset > (3 * p_entry->call_state_queue.count))
            {
                cause = APP_RESULT_INVALID_OFFSET;
                break;
            }

            if (cause != APP_RESULT_SUCCESS)
            {
                if (cause == BLE_AUDIO_CB_RESULT_PENDING)
                {
                    cause = (T_APP_RESULT)BLE_AUDIO_CB_RESULT_APP_ERR;
                }

                break;
            }

            for (i = 0; i < p_entry->call_state_queue.count; i++)
            {
                p_call_state = (T_CALL_STATE *)os_queue_peek(&p_entry->call_state_queue, i);
                read_data[idx++] = p_call_state->call_idx;
                read_data[idx++] = p_call_state->call_state;
                read_data[idx++] = p_call_state->call_flags;
            }
            if (gatt_svc_read_confirm(conn_handle, cid, service_id, attrib_index,
                                      read_data + offset, sizeof(read_data) - offset,
                                      sizeof(read_data), APP_RESULT_SUCCESS))
            {
                cause = APP_RESULT_PENDING;
            }
            else
            {
                cause = APP_RESULT_APP_ERR;
            }
        }
        break;

    case TBS_UUID_CHAR_INCOMING_CALL:
        {
            T_CALL_STATE *p_call_state = tbs_get_call_state(p_entry, p_entry->incoming_call_idx);
            if (p_call_state == NULL)
            {
                return (T_APP_RESULT)BLE_AUDIO_CB_RESULT_APP_ERR;
            }

            cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_CCP_SERVER_READ_IND, &read_ind);

            if (offset > (1 + p_call_state->uri_len))
            {
                cause = APP_RESULT_INVALID_OFFSET;
                break;
            }

            if (cause != APP_RESULT_SUCCESS)
            {
                if (cause == BLE_AUDIO_CB_RESULT_PENDING)
                {
                    cause = (T_APP_RESULT)BLE_AUDIO_CB_RESULT_APP_ERR;
                }

                break;
            }

            uint8_t read_data[1 + p_call_state->uri_len];
            read_data[0] = p_entry->incoming_call_idx;
            memcpy(&read_data[1], p_call_state->call_uri, p_call_state->uri_len);
            if (gatt_svc_read_confirm(conn_handle, cid, service_id, attrib_index,
                                      read_data + offset, sizeof(read_data) - offset,
                                      sizeof(read_data), APP_RESULT_SUCCESS))
            {
                cause = APP_RESULT_PENDING;
            }
            else
            {
                cause = APP_RESULT_APP_ERR;
            }
        }
        break;

    case TBS_UUID_CHAR_CALL_FRIENDLY_NAME:
        {
            T_CALL_STATE *p_call_state = tbs_get_call_state(p_entry, p_entry->call_fri_name_cb.call_index);

            if (p_call_state == NULL)
            {
                return (T_APP_RESULT)BLE_AUDIO_CB_RESULT_APP_ERR;
            }

            cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_CCP_SERVER_READ_IND, &read_ind);

            if (offset > (1 + p_entry->call_fri_name_cb.friendly_name_len))
            {
                cause = APP_RESULT_INVALID_OFFSET;
                break;
            }

            if (cause != APP_RESULT_SUCCESS)
            {
                if (cause == BLE_AUDIO_CB_RESULT_PENDING)
                {
                    cause = (T_APP_RESULT)BLE_AUDIO_CB_RESULT_APP_ERR;
                }

                break;
            }

            uint8_t read_data[1 + p_entry->call_fri_name_cb.friendly_name_len];
            read_data[0] = p_entry->call_fri_name_cb.call_index;
            memcpy(&read_data[1], p_entry->call_fri_name_cb.p_friendly_name,
                   p_entry->call_fri_name_cb.friendly_name_len);

            if (gatt_svc_read_confirm(conn_handle, cid, service_id, attrib_index,
                                      read_data + offset, sizeof(read_data) - offset,
                                      sizeof(read_data), APP_RESULT_SUCCESS))
            {
                cause = APP_RESULT_PENDING;
            }
            else
            {
                cause = APP_RESULT_APP_ERR;
            }
        }
        break;

    default:
        cause =  APP_RESULT_ATTR_NOT_FOUND;
        break;
    }
    return (cause);
}

T_APP_RESULT tbs_attr_write_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                               uint16_t attrib_index,
                               T_WRITE_TYPE write_type,
                               uint16_t length, uint8_t *p_value, P_FUN_EXT_WRITE_IND_POST_PROC *p_write_post_proc)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_TBS_CB *p_entry = tbs_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    *p_write_post_proc = NULL;

    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_entry->p_attr_tbl, attrib_index,
                                                             p_entry->attr_num);

    PROTOCOL_PRINT_INFO1("[CCP]tbs_attr_write_cb: attrib_index %d, offset %x", attrib_index);

    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR1("[CCP]tbs_attr_write_cb Error: attrib_index 0x%x", attrib_index);
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    switch (char_uuid.uu.char_uuid16)
    {
    case TBS_UUID_CHAR_BEARER_SIGNAL_STRENGTH_REPORTING_INTERVAL:
        {
            if (length != 1)
            {
                cause = APP_RESULT_INVALID_VALUE_SIZE;
            }
            else
            {
                T_CCP_WRITE_RPT_INT rpt_int_cb = {0};

                rpt_int_cb.conn_handle = conn_handle;
                rpt_int_cb.cid = cid;
                rpt_int_cb.service_id = service_id;
                rpt_int_cb.rpt_int = *p_value;

                cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_CCP_WRITE_RPT_INT, &rpt_int_cb);
            }
        }
        break;

    case TBS_UUID_CHAR_CALL_CONTROL_POINT:
        {
            if (length < 1)
            {
                return APP_RESULT_INVALID_VALUE_SIZE;
            }

            uint8_t opcode = *p_value;
            if (opcode != TBS_CALL_CONTROL_POINT_CHAR_OPCODE_ORIGINATE &&
                opcode != TBS_CALL_CONTROL_POINT_CHAR_OPCODE_JOIN)
            {
                if (length != 2)
                {
                    return APP_RESULT_INVALID_VALUE_SIZE;
                }
            }

            *p_write_post_proc = ccp_write_post_proc;
        }
        break;

    default:
        {
            cause = APP_RESULT_ATTR_NOT_FOUND;
        }
        break;
    }

    return (cause);
}

void tbs_cccd_update_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                        uint16_t attrib_index,
                        uint16_t ccc_bits)
{
    T_TBS_CB *p_entry = tbs_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        return;
    }
    ble_audio_mgr_send_server_cccd_info(conn_handle, p_entry->p_attr_tbl, service_id, attrib_index,
                                        ccc_bits, 0, p_entry->attr_num);
    return;
}


const T_FUN_GATT_EXT_SERVICE_CBS tbs_cbs =
{
    tbs_attr_read_cb,  // Read callback function pointer
    tbs_attr_write_cb, // Write callback function pointer
    tbs_cccd_update_cb  // CCCD update callback function pointer
};

static bool tbs_create_table(T_TBS_CB *p_entry, T_CCP_SERVER_REG_SRV_PARAM *p_param)
{
    uint16_t idx = 0;
    uint16_t attr_tbl_size = sizeof(gtbs_attr_tbl);

    if (p_param->char_bearer_uri_schemes_supported_list.optional_property_notify)
    {
        attr_tbl_size += sizeof(bearer_uri_schemes_supported_list_char_tbl);
    }
    else
    {
        attr_tbl_size += (sizeof(bearer_uri_schemes_supported_list_char_tbl) - sizeof(T_ATTRIB_APPL));
    }

    if (p_param->char_bearer_signal_strength.support)
    {
        attr_tbl_size += sizeof(bearer_signal_char_tbl);
    }

    if (p_param->char_incoming_call_target_bearer_uri.support)
    {
        attr_tbl_size += sizeof(incoming_call_tg_uri_char_tbl);
    }

    if (p_param->char_call_friendly_name.support)
    {
        attr_tbl_size += sizeof(call_friendly_name_char_tbl);
    }

    p_entry->p_attr_tbl = ble_audio_mem_zalloc(attr_tbl_size);

    if (p_entry->p_attr_tbl == NULL)
    {
        PROTOCOL_PRINT_ERROR0("[CCP]tbs_create_table: allocate mem failed");
        return false;
    }

    memcpy(p_entry->p_attr_tbl + idx, gtbs_attr_tbl, sizeof(gtbs_attr_tbl));
    idx += (sizeof(gtbs_attr_tbl) / sizeof(T_ATTRIB_APPL));

    if (!p_entry->gtbs)
    {
        p_entry->p_attr_tbl[0].type_value[2] = LO_WORD(GATT_UUID_TELEPHONE_BEARER_SERVICE);
        p_entry->p_attr_tbl[0].type_value[3] = HI_WORD(GATT_UUID_TELEPHONE_BEARER_SERVICE);
    }

    if (p_param->char_bearer_uri_schemes_supported_list.optional_property_notify)
    {
        memcpy(p_entry->p_attr_tbl + idx, bearer_uri_schemes_supported_list_char_tbl,
               sizeof(bearer_uri_schemes_supported_list_char_tbl));
        idx += (sizeof(bearer_uri_schemes_supported_list_char_tbl) / sizeof(T_ATTRIB_APPL));
    }
    else
    {
        memcpy(p_entry->p_attr_tbl + idx, bearer_uri_schemes_supported_list_char_tbl,
               (sizeof(bearer_uri_schemes_supported_list_char_tbl) - sizeof(T_ATTRIB_APPL)));

        p_entry->p_attr_tbl[idx].type_value[2] = GATT_CHAR_PROP_READ;

        idx += ((sizeof(bearer_uri_schemes_supported_list_char_tbl) / sizeof(T_ATTRIB_APPL)) - 1);
    }

    if (p_param->char_bearer_signal_strength.support)
    {
        memcpy(p_entry->p_attr_tbl + idx, bearer_signal_char_tbl, sizeof(bearer_signal_char_tbl));
        idx += (sizeof(bearer_signal_char_tbl) / sizeof(T_ATTRIB_APPL));
    }

    if (p_param->char_incoming_call_target_bearer_uri.support)
    {
        memcpy(p_entry->p_attr_tbl + idx, incoming_call_tg_uri_char_tbl,
               sizeof(incoming_call_tg_uri_char_tbl));
        idx += (sizeof(incoming_call_tg_uri_char_tbl) / sizeof(T_ATTRIB_APPL));
    }

    if (p_param->char_call_friendly_name.support)
    {
        memcpy(p_entry->p_attr_tbl + idx, call_friendly_name_char_tbl, sizeof(call_friendly_name_char_tbl));
        idx += (sizeof(call_friendly_name_char_tbl) / sizeof(T_ATTRIB_APPL));
    }

    p_entry->attr_num = idx;
#if LE_AUDIO_INIT_DEBUG
    PROTOCOL_PRINT_INFO2("[CCP]tbs_create_table: idx %d, attr_tbl_size %d", idx,
                         attr_tbl_size);
#endif
    return true;
}

T_SERVER_ID tbs_add_service(T_TBS_CB *p_entry, T_CCP_SERVER_REG_SRV_PARAM *p_param)
{
    if ((p_entry == NULL) || (p_param == NULL))
    {
        PROTOCOL_PRINT_ERROR0("[CCP]tbs_add_service: entry is NULL");
        return 0xFF;
    }

    if (tbs_create_table(p_entry, p_param))
    {
        if (false == gatt_svc_add(&p_entry->service_id,
                                  (uint8_t *)p_entry->p_attr_tbl,
                                  p_entry->attr_num * sizeof(T_ATTRIB_APPL),
                                  &tbs_cbs, NULL))
        {
            goto error;
        }
    }
    else
    {
        goto error;
    }

    PROTOCOL_PRINT_INFO2("[CCP]tbs_add_service: service_id %d, attr_num %d",
                         p_entry->service_id,
                         p_entry->attr_num);
    return p_entry->service_id;

error:
    if (p_entry->p_attr_tbl != NULL)
    {
        ble_audio_mem_free(p_entry->p_attr_tbl);
        p_entry->p_attr_tbl = NULL;
    }

    p_entry->used = false;

    PROTOCOL_PRINT_ERROR0("[CCP]tbs_add_service: failed");

    return 0xFF;
}

#endif
