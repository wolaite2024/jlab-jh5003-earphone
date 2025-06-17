/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     mcp_mgr.c
* @brief    Media Control service source file.
* @details  Interfaces to access Media Control service.
* @author   cheng_cai
* @date
* @version  v1.0
*********************************************************************************************************
*/
#include "string.h"
#include "trace.h"
#include "ble_audio_mgr.h"
#include "bt_gatt_svc.h"
#include "mcp_mgr_int.h"
#include "ble_link_util.h"
#include "mcp_mgr_obj_str.h"
#include "metadata_def.h"

#if LE_AUDIO_MCP_SERVER_SUPPORT

/********************************************************************************************************
* local static variables defined here, only used in this source file.
********************************************************************************************************/
static T_MCS_CB *p_mcs_mem;
static uint8_t total_mcs_num = 0;

/*----------------- Media Control Service -------------------*/
static const T_ATTRIB_APPL mcs_prim_srv[] =
{
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_GENERIC_MEDIA_CONTROL_SERVICE),              /* service UUID */
            HI_WORD(GATT_UUID_GENERIC_MEDIA_CONTROL_SERVICE)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    }
};

#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
static const T_ATTRIB_APPL mcs_inc_srv[] =
{
    /* <<Include Service>>, .. */
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_INCLUDE_MULTI),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_INCLUDE),
            HI_WORD(GATT_UUID_INCLUDE),
            LO_WORD(OBJECT_TRANS_SERVICE_UUID),              /* service UUID */
            HI_WORD(OBJECT_TRANS_SERVICE_UUID)
        },
        0,                            /* bValueLen     */
        NULL,                              /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    }
};
#endif

/* <<Characteristic>>*/
static const T_ATTRIB_APPL mcs_mpn_mand_opt_attr[] =
{
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
    /*  Media Player Name value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_MEDIA_PLAYER_NAME),
            HI_WORD(MCS_UUID_CHAR_MEDIA_PLAYER_NAME)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ |                             /* permissions */
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)
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
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },
};

static const T_ATTRIB_APPL mcs_tk_chg_mand_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_NOTIFY
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Track Changed value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_TRACK_CHANGED),
            HI_WORD(MCS_UUID_CHAR_TRACK_CHANGED)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_NOTIF_IND_ENCRYPTED_REQ           /* permissions */
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },
};

static const T_ATTRIB_APPL mcs_tk_title_mand_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY)
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Track Title value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_TRACK_TITLE),
            HI_WORD(MCS_UUID_CHAR_TRACK_TITLE)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ |                              /* permissions */
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)
    },
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },
};

static const T_ATTRIB_APPL mcs_tk_dur_mand_opt_attr[] =
{
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
    /*  Track Duration value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_TRACK_DURATION),
            HI_WORD(MCS_UUID_CHAR_TRACK_DURATION)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ |                             /* permissions */
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)
    },
    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },
};

static const T_ATTRIB_APPL mcs_tk_pos_mand_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY |
             GATT_CHAR_PROP_WRITE_NO_RSP |
             GATT_CHAR_PROP_WRITE)
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Track Position value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_TRACK_POSITION),
            HI_WORD(MCS_UUID_CHAR_TRACK_POSITION)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ |
         GATT_PERM_WRITE_ENCRYPTED_REQ |
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)  /* permissions */
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },
};

static const T_ATTRIB_APPL mcs_media_st_mand_attr[] =
{
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
    /*  Media State value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_MEDIA_STATE),
            HI_WORD(MCS_UUID_CHAR_MEDIA_STATE)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ |
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)  /* permissions */
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },
};

static const T_ATTRIB_APPL mcs_cccid_mand_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ                     /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Content Control ID value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_CONTENT_CONTROL_ID),
            HI_WORD(MCS_UUID_CHAR_CONTENT_CONTROL_ID)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_ENCRYPTED_REQ  /* permissions */
    },
};

#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
static const T_ATTRIB_APPL mcs_mpio_id_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Media Player Icon Object ID value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_MEDIA_PLAYER_ICON_OBJECT_ID),
            HI_WORD(MCS_UUID_CHAR_MEDIA_PLAYER_ICON_OBJECT_ID)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_ENCRYPTED_REQ                              /* permissions */
    },
};
#endif

static const T_ATTRIB_APPL mcs_mpi_url_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            GATT_CHAR_PROP_READ
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Media Player Icon URL value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_MEDIA_PLAYER_ICON_URL),
            HI_WORD(MCS_UUID_CHAR_MEDIA_PLAYER_ICON_URL)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_ENCRYPTED_REQ                              /* permissions */
    },
};

static const T_ATTRIB_APPL mcs_pb_speed_opt_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY |
             GATT_CHAR_PROP_WRITE_NO_RSP |
             GATT_CHAR_PROP_WRITE)
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Playback Speed value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_PLAYBACK_SPEED),
            HI_WORD(MCS_UUID_CHAR_PLAYBACK_SPEED)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ |
         GATT_PERM_WRITE_ENCRYPTED_REQ |
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)  /* permissions */
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },
};

static const T_ATTRIB_APPL mcs_sk_speed_opt_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY)
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Seeking Speed value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_SEEKING_SPEED),
            HI_WORD(MCS_UUID_CHAR_SEEKING_SPEED)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ |
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)  /* permissions */
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },
};

#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
static const T_ATTRIB_APPL mcs_ctso_id_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),

            GATT_CHAR_PROP_READ
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Current Track Segments Object ID value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_CURRENT_TRACK_SEGMENTS_OBJECT_ID),
            HI_WORD(MCS_UUID_CHAR_CURRENT_TRACK_SEGMENTS_OBJECT_ID)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_ENCRYPTED_REQ  /* permissions */
    },
};

static const T_ATTRIB_APPL mcs_cto_id_opt_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY |
             GATT_CHAR_PROP_WRITE_NO_RSP |
             GATT_CHAR_PROP_WRITE)
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Current Track Object ID value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_CURRENT_TRACK_OBJECT_ID),
            HI_WORD(MCS_UUID_CHAR_CURRENT_TRACK_OBJECT_ID)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ |
         GATT_PERM_WRITE_ENCRYPTED_REQ |
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)  /* permissions */
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },
};

static const T_ATTRIB_APPL mcs_nto_id_opt_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY |
             GATT_CHAR_PROP_WRITE_NO_RSP |
             GATT_CHAR_PROP_WRITE)
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Next Track Object ID value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID),
            HI_WORD(MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ |
         GATT_PERM_WRITE_ENCRYPTED_REQ |
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)  /* permissions */
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },
};

static const T_ATTRIB_APPL mcs_pgo_id_opt_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY)
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Parent Group Object ID value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_PARENT_GROUP_OBJECT_ID),
            HI_WORD(MCS_UUID_CHAR_PARENT_GROUP_OBJECT_ID)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ |
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)  /* permissions */
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },
};

static const T_ATTRIB_APPL mcs_cgo_id_opt_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY |
             GATT_CHAR_PROP_WRITE_NO_RSP |
             GATT_CHAR_PROP_WRITE)
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Current Group Object ID value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_CURRENT_GROUP_OBJECT_ID),
            HI_WORD(MCS_UUID_CHAR_CURRENT_GROUP_OBJECT_ID)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ |
         GATT_PERM_WRITE_ENCRYPTED_REQ |
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)  /* permissions */
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },
};
#endif
static const T_ATTRIB_APPL mcs_playing_odr_opt_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY |
             GATT_CHAR_PROP_WRITE_NO_RSP |
             GATT_CHAR_PROP_WRITE)
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Playing Order value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_PLAYING_ORDER),
            HI_WORD(MCS_UUID_CHAR_PLAYING_ORDER)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ |
         GATT_PERM_WRITE_ENCRYPTED_REQ |
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)  /* permissions */
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },
};

static const T_ATTRIB_APPL mcs_playing_odr_sup_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),

            GATT_CHAR_PROP_READ                   /* characteristic properties */
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Playing Order Supported value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_PLAYING_ORDERS_SUPPORTED),
            HI_WORD(MCS_UUID_CHAR_PLAYING_ORDERS_SUPPORTED)
        },
        0,                                          /* bValueLen */
        NULL,

        GATT_PERM_READ_ENCRYPTED_REQ  /* permissions */
    },
};

static const T_ATTRIB_APPL mcs_mcp_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY |
             GATT_CHAR_PROP_WRITE_NO_RSP)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Media Control Point value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_MEDIA_CONTROL_POINT),
            HI_WORD(MCS_UUID_CHAR_MEDIA_CONTROL_POINT)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_WRITE_ENCRYPTED_REQ |
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)  /* permissions */
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },
};

static const T_ATTRIB_APPL mcs_mcpo_sup_opt_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_READ |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY)
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Media Control Point Opcodes Supported value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED),
            HI_WORD(MCS_UUID_CHAR_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ |
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)  /* permissions */
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },
};

#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
static const T_ATTRIB_APPL mcs_sro_id_opt_attr[] =
{
    {
        ATTRIB_FLAG_VALUE_INCL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHARACTERISTIC),
            HI_WORD(GATT_UUID_CHARACTERISTIC),
            (GATT_CHAR_PROP_WRITE |                    /* characteristic properties */
             GATT_CHAR_PROP_NOTIFY |
             GATT_CHAR_PROP_WRITE_NO_RSP)
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /*  Search Control Point value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_SEARCH_CONTROL_POINT),
            HI_WORD(MCS_UUID_CHAR_SEARCH_CONTROL_POINT)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_WRITE_ENCRYPTED_REQ |
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)  /* permissions */
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },

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
    /*  Search Results Object ID value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(MCS_UUID_CHAR_SEARCH_RESULTS_OBJECT_ID),
            HI_WORD(MCS_UUID_CHAR_SEARCH_RESULTS_OBJECT_ID)
        },
        0,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ |
         GATT_PERM_NOTIF_IND_ENCRYPTED_REQ)  /* permissions */
    },

    /* client characteristic configuration */
    {
        ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_CCCD_APPL,                   /* flags */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            HI_WORD(GATT_UUID_CHAR_CLIENT_CONFIG),
            LO_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT), /* client char. config. bit field */
            HI_WORD(GATT_CLIENT_CHAR_CONFIG_DEFAULT)
        },
        2,                                          /* bValueLen */
        NULL,
        (GATT_PERM_READ_ENCRYPTED_REQ | GATT_PERM_WRITE_ENCRYPTED_REQ)          /* permissions */
    },
};
#endif

/********************************************************************************************************
* local static function defined here, only used in this source file.
********************************************************************************************************/
static T_MCS_CB *mcs_find_by_srv_id(T_SERVER_ID service_id)
{
    uint8_t  i;

    if (p_mcs_mem == NULL)
    {
        return NULL;
    }

    for (i = 0; i < total_mcs_num; i++)
    {
        if (p_mcs_mem[i].used
            && (p_mcs_mem[i].service_id == service_id))
        {
            return (&p_mcs_mem[i]);
        }
    }

    PROTOCOL_PRINT_ERROR1("mcs_find_by_srv_id: failed, service_id 0x%x", service_id);

    return NULL;
}


static T_MCS_CB *mcs_allocate_entry(T_MCP_SERVER_REG_SRV_PARAM *p_param)
{
    uint8_t i;
    if ((p_mcs_mem == NULL) || (p_param == NULL))
    {
        return NULL;
    }
    for (i = 0; i < total_mcs_num; i++)
    {
        if (p_mcs_mem[i].used == false)
        {
            memset(&p_mcs_mem[i], 0, sizeof(T_MCS_CB));
            p_mcs_mem[i].gmcs = p_param->gmcs;
            p_mcs_mem[i].used = true;
#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
            p_mcs_mem[i].ots_service_id = p_param->ots.ots_service_id;
#else
            p_mcs_mem[i].ots_service_id = 0xFF;
#endif
            p_mcs_mem[i].media_state = MCS_MEDIA_STATE_INACTIVE;
#if LE_AUDIO_OTS_SERV_SUPPORT
            os_queue_init(&p_mcs_mem[i].gp_obj_queue);
#endif
            return &p_mcs_mem[i];
        }
    }
    return NULL;
}


static bool check_media_ctl_point_op_supported(T_MCS_CB *p_entry, uint8_t opcode)
{
    uint32_t supported_val = 0;
    switch (opcode)
    {
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PLAY:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_PLAY;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PAUSE:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_PAUSE;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FAST_REWIND:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_FAST_REWIND;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FAST_FORWARD:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_FAST_FORWARD;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_STOP:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_STOP;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_MOVE_RELATIVE:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_MOVE_RELATIVE;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PREVIOUS_SEGMENT:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_PREVIOUS_SEGMENT;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_NEXT_SEGMENT:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_NEXT_SEGMENT;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FIRST_SEGMENT:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_FIRST_SEGMENT;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_LAST_SEGMENT:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_LAST_SEGMENT;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_SEGMENT:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_GOTO_SEGMENT;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PREVIOUS_TRACK:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_PREVIOUS_TRACK;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_NEXT_TRACK:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_NEXT_TRACK;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FIRST_TRACK:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_FIRST_TRACK;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_LAST_TRACK:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_LAST_TRACK;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_TRACK:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_GOTO_TRACK;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_PREVIOUS_GROUP:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_PREVIOUS_GROUP;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_NEXT_GROUP:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_NEXT_GROUP;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_FIRST_GROUP:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_FIRST_GROUP;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_LAST_GROUP:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_LAST_GROUP;
        break;
    case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_GROUP:
        supported_val = MCS_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED_CHAR_BIT_VALUE_GOTO_GROUP;
        break;
    default:
        return false;
    }

    if (supported_val & p_entry->media_control_point_opcodes_supported)
    {
        return true;
    }

    return false;
}

static bool mcs_send_notify(T_MCS_CB *p_entry, uint16_t conn_handle, uint16_t uuid,
                            uint8_t *p_value, uint16_t len)
{
    T_BLE_AUDIO_LINK *p_link = ble_audio_link_find_by_conn_handle(conn_handle);

    if (p_link)
    {
        uint16_t attrib_idx = 0;
        T_CHAR_UUID char_uuid;
        char_uuid.index = 1;
        char_uuid.uuid_size = UUID_16BIT_SIZE;
        char_uuid.uu.char_uuid16 = uuid;

        if (!p_entry)
        {
            return false;
        }

        if (!gatt_svc_find_char_index_by_uuid(p_entry->p_attr_tbl, char_uuid,
                                              p_entry->attr_num,
                                              &attrib_idx))
        {
            return false;
        }

        if (gatt_svc_send_data(conn_handle, L2C_FIXED_CID_ATT, p_entry->service_id, attrib_idx, p_value,
                               len > (p_link->mtu_size - 3) ? (p_link->mtu_size - 3) : len, GATT_PDU_TYPE_NOTIFICATION))
        {
            return true;
        }
    }

    return false;
}

static bool mcs_send_notify_all(T_MCS_CB *p_entry, uint16_t uuid, uint8_t *p_value, uint16_t len)
{
    bool ret = true;
    uint16_t attrib_idx = 0;
    T_CHAR_UUID char_uuid;
    char_uuid.index = 1;
    char_uuid.uuid_size = UUID_16BIT_SIZE;
    char_uuid.uu.char_uuid16 = uuid;

    if (!p_entry)
    {
        return false;
    }

    if (!gatt_svc_find_char_index_by_uuid(p_entry->p_attr_tbl, char_uuid,
                                          p_entry->attr_num,
                                          &attrib_idx))
    {
        return false;
    }

    PROTOCOL_PRINT_ERROR1("mcs_send_notify_all, attrib_idx: 0x%x", attrib_idx);
    ble_audio_send_notify_all(L2C_FIXED_CID_ATT, p_entry->service_id,
                              attrib_idx, p_value, len);
    return ret;
}
#if LE_AUDIO_OTS_SERV_SUPPORT
//notify all devices except the device which start the operation
static bool mcs_notify_all_exc_own(T_MCS_CB *p_entry, uint16_t conn_handle, uint16_t uuid,
                                   uint8_t *p_value, uint16_t len)
{
    bool ret = true;
    uint8_t i;
    uint16_t attrib_idx = 0;
    T_CHAR_UUID char_uuid;
    char_uuid.index = 1;
    char_uuid.uuid_size = UUID_16BIT_SIZE;
    char_uuid.uu.char_uuid16 = uuid;

    if (!p_entry)
    {
        return false;
    }

    if (!gatt_svc_find_char_index_by_uuid(p_entry->p_attr_tbl, char_uuid,
                                          p_entry->attr_num,
                                          &attrib_idx))
    {
        return false;
    }

    for (i = 0; i < ble_audio_db.acl_link_num; i++)
    {
        if ((ble_audio_db.le_link[i].used == true) &&
            (ble_audio_db.le_link[i].conn_handle != conn_handle) &&
            (ble_audio_db.le_link[i].state == GAP_CONN_STATE_CONNECTED))
        {
            gatt_svc_send_data(ble_audio_db.le_link[i].conn_handle, L2C_FIXED_CID_ATT, p_entry->service_id,
                               attrib_idx,
                               p_value, len > (ble_audio_db.le_link[i].mtu_size - 3) ? (ble_audio_db.le_link[i].mtu_size - 3) :
                               len, GATT_PDU_TYPE_NOTIFICATION);
        }
    }
    return ret;

}
#endif

static void handle_media_ctl_point_opcode(T_MCS_CB *p_entry, uint16_t conn_handle, uint16_t cid,
                                          uint8_t *p_value,
                                          uint16_t len)
{
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;
    uint8_t opcode = *p_value;
    uint8_t result[2] = {opcode, MCS_MEDIA_CONTROL_POINT_NOTIFICATION_RESULT_CODES_SUCCESS};
    T_MCP_SERVER_WRITE_MEDIA_CP_IND mcp_req;
    mcp_req.conn_handle = conn_handle;
    mcp_req.cid = cid;
    mcp_req.service_id = p_entry->service_id;
    mcp_req.opcode = opcode;
    if (!check_media_ctl_point_op_supported(p_entry, opcode))
    {
        result[1] = MCS_MEDIA_CONTROL_POINT_NOTIFICATION_RESULT_CODES_OPCODE_NOT_SUPPORTED;
    }
    else if (p_entry->media_state == MCS_MEDIA_STATE_INACTIVE)
    {
        result[1] = MCS_MEDIA_CONTROL_POINT_NOTIFICATION_RESULT_CODES_MEDIA_PLAYER_INACTIVE;
        cb_result = ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCP_SERVER_WRITE_MEDIA_CP_IND, &mcp_req);
    }
    else
    {
        switch (opcode)
        {
        case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_MOVE_RELATIVE:
            LE_ARRAY_TO_UINT32(mcp_req.param.move_relative_opcode_offset, (p_value + 1));
            break;

        case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_SEGMENT:
            LE_ARRAY_TO_UINT32(mcp_req.param.goto_segment_opcode_n, (p_value + 1));
            break;

        case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_TRACK:
            LE_ARRAY_TO_UINT32(mcp_req.param.goto_track_opcode_n, (p_value + 1));
            break;

        case MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_GROUP:
            LE_ARRAY_TO_UINT32(mcp_req.param.goto_group_opcode_n, (p_value + 1));
            break;

        default:
            break;
        }

        cb_result = ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCP_SERVER_WRITE_MEDIA_CP_IND, &mcp_req);
        if (cb_result != BLE_AUDIO_CB_RESULT_SUCCESS)
        {
            result[1] = MCS_MEDIA_CONTROL_POINT_NOTIFICATION_RESULT_CODES_COMMAND_CANNOT_BE_COMPLETED;
        }
    }

    mcs_send_notify(p_entry, conn_handle, MCS_UUID_CHAR_MEDIA_CONTROL_POINT, result, 2);
}
#if LE_AUDIO_OTS_SERV_SUPPORT
static uint8_t check_scp_only_type(uint8_t *p_value, uint16_t len)
{
    uint16_t read_len = 0;
    while (read_len < len)
    {
        uint16_t type_len = *p_value++;
        read_len += type_len + 1;
        if (read_len > len)
        {
            PROTOCOL_PRINT_ERROR1("check_scp_only_type, the value mis-match len: %d", len);
            return 0xFF;
        }
        uint8_t type = *p_value;
        p_value += type_len;

        if (type == MCS_ONLY_TRACKS_TYPE || type == MCS_ONLY_GROUPS_TYPE)
        {
            return type;
        }
    }
    return 0;
}
#endif
#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
static bool check_scp_type_valide(uint8_t *p_value, uint16_t len)
{
    uint16_t read_len = 0;
    while (read_len < len)
    {
        uint16_t type_len = *p_value++;
        read_len += type_len + 1;
        if (read_len > len)
        {
            PROTOCOL_PRINT_ERROR1("check_scp_type_valide, the value mis-match len: %d", len);
            return false;
        }
        uint8_t type = *p_value;
        p_value += type_len;

        if (type >= MCS_SEARCH_TYPE_MAX)
        {
            PROTOCOL_PRINT_ERROR1("check_scp_type_valide, type valide: 0x%x", type);
            return false;
        }
    }
    return true;
}
#endif
#if LE_AUDIO_OTS_SERV_SUPPORT
static bool filt_gp_obj_by_scp_type(T_MCS_GP_ELEM *p_gp_elem, uint8_t *p_value, uint16_t len)
{
    if (p_gp_elem == NULL)
    {
        return false;
    }
    uint8_t type;
    uint8_t read_len = 0;
    uint8_t value_len;
    while (read_len < len)
    {
        value_len = (*p_value++) - 1;
        read_len += value_len + 2;
        type = *p_value++;

        switch (type)
        {
        case MCS_GROUP_NAME_TYPE:
            {
                if (p_gp_elem->p_gp_name == NULL ||
                    value_len != p_gp_elem->gp_name_len  ||
                    memcmp(p_gp_elem->p_gp_name, p_value, value_len))
                {
                    return false;
                }
            }
            break;
        case MCS_ARTIST_NAME_TYPE:
            {
                if (p_gp_elem->p_artist_name == NULL ||
                    value_len != p_gp_elem->artist_name_len  ||
                    memcmp(p_gp_elem->p_artist_name, p_value, value_len))
                {
                    return false;
                }
            }
            break;

        case MCS_ONLY_GROUPS_TYPE:
            {

            }
            break;

        default:
            PROTOCOL_PRINT_ERROR1("filt_gp_obj_by_scp_type, wrong type :%d", type);
            return false;
        }
        p_value += value_len;
    }
    return true;
}


static bool filt_tk_obj_by_scp_type(T_MCS_TK_ELEM *p_tk_elem, uint8_t *p_value, uint16_t len)
{
    if (p_tk_elem == NULL)
    {
        return false;
    }

    uint8_t type;
    uint8_t read_len = 0;
    uint8_t value_len;
    while (read_len < len)
    {
        value_len = (*p_value++) - 1;
        read_len += value_len + 2;
        type = *p_value++;

        switch (type)
        {
        case MCS_TACK_NAME_TYPE:
            {
                if (p_tk_elem->p_tk_name == NULL ||
                    value_len != p_tk_elem->tk_name_len ||
                    memcmp(p_tk_elem->p_tk_name, p_value, value_len))
                {
                    return false;
                }
            }
            break;

        case MCS_ARTIST_NAME_TYPE:
            {
                if (p_tk_elem->p_artist_name == NULL ||
                    value_len != p_tk_elem->artist_name_len ||
                    memcmp(p_tk_elem->p_artist_name, p_value, value_len))
                {
                    return false;
                }
            }
            break;

        case MCS_ALBUM_NAME_TYPE:
            {
                if (p_tk_elem->p_alb_name == NULL ||
                    value_len != p_tk_elem->alb_name_len ||
                    memcmp(p_tk_elem->p_alb_name, p_value, value_len))
                {
                    return false;
                }
            }
            break;

        case MCS_EARLIEST_YEAR_TYPE:
            {
                if (p_tk_elem->p_elst_year == NULL ||
                    value_len != p_tk_elem->elst_year_len  ||
                    memcmp(p_tk_elem->p_elst_year, p_value, value_len))
                {
                    return false;
                }
            }
            break;

        case MCS_LATEST_YEAR_TYPE:
            {
                if (p_tk_elem->p_latest_year == NULL ||
                    value_len != p_tk_elem->latest_year_len ||
                    memcmp(p_tk_elem->p_latest_year, p_value, value_len))
                {
                    return false;
                }
            }
            break;

        case MCS_GENRE_TYPE:
            {
                if (p_tk_elem->p_genre == NULL ||
                    value_len != p_tk_elem->genre_len ||
                    memcmp(p_tk_elem->p_genre, p_value, value_len))
                {
                    return false;
                }
            }
            break;
        case MCS_ONLY_TRACKS_TYPE:
            break;
        default:
            PROTOCOL_PRINT_ERROR1("filt_tk_obj_by_scp_type, wrong type :%d", type);
            return false;
        }
        p_value += value_len;
    }
    return true;
}
#endif
#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
static bool handle_search_ctl_point_op(T_MCS_CB *p_entry, uint16_t conn_handle,
                                       uint8_t *p_value, uint16_t len)
{
    T_OS_QUEUE *temp_queue = ots_get_obj_queue(p_entry->ots_service_id);
    uint16_t total_len = 0;
    bool  need_ind = false;
    int i, j;
    if (len == 0)
    {
        PROTOCOL_PRINT_ERROR0("handle_search_ctl_point_op, length is 0");
        return false;
    }

    uint8_t *p_res = ble_audio_mem_zalloc(temp_queue->count * (OTS_OBJ_ID_LEN + 1));
    if (p_res == NULL)
    {
        PROTOCOL_PRINT_ERROR0("handle_search_ctl_point_op, alloc buffer fail");
        return false;
    }
    uint8_t only_type = check_scp_only_type(p_value, len);
    if (only_type == 0xFF)
    {
        PROTOCOL_PRINT_ERROR0("handle_search_ctl_point_op, check only type error");
        ble_audio_mem_free(p_res);
        return false;
    }

    if (p_entry->search_res_obj.obj_id != 0)
    {
        //FIX TODO need to update root group value?
        mcs_delete_major_gp(p_entry->service_id, p_entry->search_res_obj.obj_id);
        p_entry->search_res_obj.obj_id = 0;
        need_ind = true;
    }

    for (i = 0; i < p_entry->gp_obj_queue.count; i++)
    {
        T_GP_ELEM *p_gp = (T_GP_ELEM *)os_queue_peek(&p_entry->gp_obj_queue, i);
        if (only_type != MCS_ONLY_TRACKS_TYPE)
        {
            if (filt_gp_obj_by_scp_type(&p_gp->gp_elem, p_value, len))
            {
                p_res[total_len++] = MCS_GROUP_OBJ_TYPE;
                memcpy(&p_res[total_len], (uint8_t *)&p_gp->obj_id, OTS_OBJ_ID_LEN);
                total_len += OTS_OBJ_ID_LEN;
            }
        }
        if (only_type != MCS_ONLY_GROUPS_TYPE)
        {
            for (j = 0; j < p_gp->tr_queue.count; j++)
            {
                T_TK_ELEM *p_tk = (T_TK_ELEM *)os_queue_peek(&p_gp->tr_queue, j);
                if (p_tk && filt_tk_obj_by_scp_type(&p_tk->tk_elem, p_value, len))
                {
                    p_res[total_len++] = MCS_TRACK_OBJ_TYPE;
                    memcpy(&p_res[total_len], (uint8_t *)&p_tk->obj_id, OTS_OBJ_ID_LEN);
                    total_len += OTS_OBJ_ID_LEN;
                }
            }
        }
    }

    PROTOCOL_PRINT_TRACE2("handle_search_ctl_point_op, search result obj id = 0x%x, total_len = %d",
                          (int)p_entry->search_res_obj.obj_id, total_len);

    if (total_len == 0)
    {
        p_entry->search_res_obj.obj_id = 0;
    }
    else
    {
        T_MCS_GP_ELEM gp_elem;
        memset((uint8_t *)&gp_elem, 0, sizeof(T_MCS_GP_ELEM));
        gp_elem.p_gp_name = (uint8_t *)"mcs search res";      //we should use utf-8 format
        gp_elem.gp_name_len = strlen("mcs search res");
        gp_elem.p_gp_value = p_res;
        gp_elem.gp_value_len = total_len;

        p_entry->search_res_obj.obj_id = mcs_create_major_group(p_entry->service_id, &gp_elem,
                                                                p_entry->root_grp_obj_id);

        if (p_entry->search_res_obj.obj_id != 0)
        {
            need_ind = true;
        }
        else
        {
            p_entry->search_res_obj.obj_id = 0;
        }
    }

    if (need_ind)
    {
        T_MCS_SCP_RES scp_res;
        mcs_send_notify(p_entry, conn_handle, MCS_UUID_CHAR_SEARCH_RESULTS_OBJECT_ID,
                        p_entry->search_res_obj.obj_id == 0 ? NULL : (uint8_t *)&p_entry->search_res_obj.obj_id,
                        p_entry->search_res_obj.obj_id == 0 ? 0 : OTS_OBJ_ID_LEN);

        scp_res.service_id = p_entry->service_id;
        scp_res.search_obj_id = p_entry->search_res_obj.obj_id;
        scp_res.p_value = (T_MCS_GP_OBJ *)p_res;
        scp_res.num = total_len / sizeof(T_MCS_GP_OBJ);
        ble_audio_mgr_dispatch(LE_AUDIO_MSG_SCP_RESULT, &scp_res);
    }
    ble_audio_mem_free(p_res);
    return true;
}
#endif

/**
 * @brief read characteristic data from service.
 *
 * @param conn_handle           Connection ID.
 * @param service_id        ServiceID to be read.
 * @param attrib_index      Attribute index of getting characteristic data.
 * @param offset            Offset of characteritic to be read.
 * @param p_length          Length of getting characteristic data.
 * @param pp_value          Pointer to pointer of characteristic value to be read.
 * @return T_APP_RESULT
*/
T_APP_RESULT mcs_attr_read_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                              uint16_t attrib_index,
                              uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_MCS_CB *p_entry = mcs_find_by_srv_id(service_id);
    T_MCP_SERVER_READ_IND read_ind;

    if (p_entry == NULL)
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_entry->p_attr_tbl,
                                                             attrib_index, p_entry->attr_num);
    *p_length = 0;

    PROTOCOL_PRINT_INFO2("mcs_attr_read_cb attrib_index = %d offset %x", attrib_index, offset);

    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR1("mcs_attr_read_cb Error: attrib_index 0x%x", attrib_index);
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    read_ind.conn_handle = conn_handle;
    read_ind.cid = cid;
    read_ind.service_id = service_id;
    read_ind.char_uuid = char_uuid.uu.char_uuid16;
    read_ind.offset = offset;

    switch (char_uuid.uu.char_uuid16)
    {
    case MCS_UUID_CHAR_MEDIA_PLAYER_NAME:
    case MCS_UUID_CHAR_MEDIA_PLAYER_ICON_URL:
    case MCS_UUID_CHAR_TRACK_TITLE:
    case MCS_UUID_CHAR_TRACK_DURATION:
    case MCS_UUID_CHAR_TRACK_POSITION:
    case MCS_UUID_CHAR_PLAYBACK_SPEED:
    case MCS_UUID_CHAR_SEEKING_SPEED:
    case MCS_UUID_CHAR_PLAYING_ORDER:
    case MCS_UUID_CHAR_CONTENT_CONTROL_ID:
        {
            cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCP_SERVER_READ_IND, &read_ind);

            if (cause == BLE_AUDIO_CB_RESULT_SUCCESS)
            {
                cause = (T_APP_RESULT)BLE_AUDIO_CB_RESULT_APP_ERR;
            }
        }
        break;
#if LE_AUDIO_OTS_SERV_SUPPORT
    case MCS_UUID_CHAR_MEDIA_PLAYER_ICON_OBJECT_ID:
        {
            if (p_entry->ply_icon_obj_id != 0)
            {
                *pp_value = (uint8_t *)(&p_entry->ply_icon_obj_id);
                *p_length = OTS_OBJ_ID_LEN;
            }
            else
            {
                *pp_value = NULL;
                *p_length = 0;
            }
        }
        break;

    case MCS_UUID_CHAR_CURRENT_TRACK_SEGMENTS_OBJECT_ID:
        {
            if (p_entry->cur_tkseg_obj_id != 0)
            {
                *pp_value = (uint8_t *)(&p_entry->cur_tkseg_obj_id);
                *p_length = OTS_OBJ_ID_LEN;
            }
            else
            {
                *pp_value = NULL;
                *p_length = 0;
            }
        }
        break;

    case MCS_UUID_CHAR_CURRENT_TRACK_OBJECT_ID:
        {
            if (p_entry->cur_track_obj.obj_id != 0)
            {
                *pp_value = (uint8_t *)(&p_entry->cur_track_obj.obj_id);
                *p_length = OTS_OBJ_ID_LEN;
            }
            else
            {
                *pp_value = NULL;
                *p_length = 0;
            }
        }
        break;

    case MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID:
        {
            if (p_entry->next_track_obj.obj_id != 0)
            {
                *pp_value = (uint8_t *)(&p_entry->next_track_obj.obj_id);
                *p_length = OTS_OBJ_ID_LEN;
            }
            else
            {
                *pp_value = NULL;
                *p_length = 0;
            }
        }
        break;

    case MCS_UUID_CHAR_PARENT_GROUP_OBJECT_ID:
        {
            if (p_entry->parent_group_obj.obj_id != 0)
            {
                *pp_value = (uint8_t *)(&p_entry->parent_group_obj.obj_id);
                *p_length = OTS_OBJ_ID_LEN;
            }
            else
            {
                *pp_value = NULL;
                *p_length = 0;
            }
        }
        break;

    case MCS_UUID_CHAR_CURRENT_GROUP_OBJECT_ID:
        {
            if (p_entry->current_group_obj.obj_id != 0)
            {
                *pp_value = (uint8_t *)(&p_entry->current_group_obj.obj_id);
                *p_length = OTS_OBJ_ID_LEN;
            }
            else
            {
                *pp_value = NULL;
                *p_length = 0;
            }
        }
        break;
    case MCS_UUID_CHAR_SEARCH_RESULTS_OBJECT_ID:
        {
            if (p_entry->search_res_obj.obj_id != 0)
            {
                *pp_value = (uint8_t *)(&p_entry->search_res_obj.obj_id);
                *p_length = OTS_OBJ_ID_LEN;
            }
            else
            {
                *pp_value = NULL;
                *p_length = 0;
            }
        }
        break;
#endif
    case MCS_UUID_CHAR_PLAYING_ORDERS_SUPPORTED:
        {
            cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCP_SERVER_READ_IND, &read_ind);

            *pp_value = (uint8_t *)(&p_entry->play_order_sup);
            *p_length = 2;

            if (cause == BLE_AUDIO_CB_RESULT_PENDING)
            {
                cause = (T_APP_RESULT)BLE_AUDIO_CB_RESULT_APP_ERR;
            }
        }
        break;

    case MCS_UUID_CHAR_MEDIA_STATE:
        {
            cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCP_SERVER_READ_IND, &read_ind);

            *pp_value = &p_entry->media_state;
            *p_length = 1;

            if (cause == BLE_AUDIO_CB_RESULT_PENDING)
            {
                cause = (T_APP_RESULT)BLE_AUDIO_CB_RESULT_APP_ERR;
            }
        }
        break;

    case MCS_UUID_CHAR_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED:
        {
            cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCP_SERVER_READ_IND, &read_ind);

            *pp_value = (uint8_t *)(&p_entry->media_control_point_opcodes_supported);
            *p_length = 4;

            if (cause == BLE_AUDIO_CB_RESULT_PENDING)
            {
                cause = (T_APP_RESULT)BLE_AUDIO_CB_RESULT_APP_ERR;
            }
        }
        break;

    default:
        {
            PROTOCOL_PRINT_ERROR1("mcs_attr_read_cb attrib_index = %d not found", attrib_index);
            cause  = APP_RESULT_ATTR_NOT_FOUND;
        }
        break;
    }
    return (cause);
}

/**
 * @brief update CCCD bits from stack.
 *
 * @param conn_handle           Connection handle.
 * @param service_id        Service ID.
 * @param index             Attribute index of characteristic data.
 * @param ccc_bits          CCCD bits from stack.
 * @return None
*/
void mcs_cccd_update_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id, uint16_t index,
                        uint16_t ccc_bits)
{
    T_MCS_CB *p_entry = mcs_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        return;
    }

    PROTOCOL_PRINT_INFO2("mcs_cccd_update_cb index = %d ccc_bits %x", index, ccc_bits);
    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_entry->p_attr_tbl,
                                                             index, p_entry->attr_num);

    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR1("mcs_cccd_update_cb Error: attrib_index 0x%x", index);
        return;
    }
#if LE_AUDIO_OTS_SERV_SUPPORT
    switch (char_uuid.uu.char_uuid16)
    {
    case MCS_UUID_CHAR_CURRENT_TRACK_OBJECT_ID:
        {
            p_entry->cur_track_obj.char_index = char_uuid.index;
        }
        break;

    case MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID:
        {
            p_entry->next_track_obj.char_index = char_uuid.index;
        }
        break;

    case MCS_UUID_CHAR_PARENT_GROUP_OBJECT_ID:
        {
            p_entry->parent_group_obj.char_index = char_uuid.index;
        }
        break;

    case MCS_UUID_CHAR_CURRENT_GROUP_OBJECT_ID:
        {
            p_entry->current_group_obj.char_index = char_uuid.index;
        }
        break;

    case MCS_UUID_CHAR_SEARCH_RESULTS_OBJECT_ID:
        {
            p_entry->search_res_obj.char_index = char_uuid.index;
        }
        break;
    default:
        {
            break;
        }

    }
#endif
    ble_audio_mgr_send_server_cccd_info(conn_handle, p_entry->p_attr_tbl,
                                        p_entry->service_id, index,
                                        ccc_bits, 0, p_entry->attr_num);
    return;
}

void mcs_write_post_callback(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                             uint16_t attrib_index,
                             uint16_t length, uint8_t *p_value)
{
#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
    uint16_t cb_result = BLE_AUDIO_CB_RESULT_SUCCESS;
    T_MCS_WRITE_REQ write_result;
#endif
    T_MCS_CB *p_entry = mcs_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        return;
    }

    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_entry->p_attr_tbl,
                                                             attrib_index, p_entry->attr_num);

    switch (char_uuid.uu.char_uuid16)
    {
#if LE_AUDIO_OTS_SERV_SUPPORT
    case MCS_UUID_CHAR_CURRENT_TRACK_OBJECT_ID:
        {
            mcs_notify_all_exc_own(p_entry, conn_handle, MCS_UUID_CHAR_CURRENT_TRACK_OBJECT_ID,
                                   (uint8_t *)&p_entry->cur_track_obj.obj_id, OTS_OBJ_ID_LEN);

        }
        break;

    case MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID:
        {
            mcs_notify_all_exc_own(p_entry, conn_handle, MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID,
                                   (uint8_t *)&p_entry->next_track_obj.obj_id, OTS_OBJ_ID_LEN);
        }
        break;

    case MCS_UUID_CHAR_CURRENT_GROUP_OBJECT_ID:
        {
            mcs_notify_all_exc_own(p_entry, conn_handle, MCS_UUID_CHAR_CURRENT_GROUP_OBJECT_ID,
                                   (uint8_t *)&p_entry->current_group_obj.obj_id, OTS_OBJ_ID_LEN);

        }
        break;
#endif
    case MCS_UUID_CHAR_MEDIA_CONTROL_POINT:
        {
            handle_media_ctl_point_opcode(p_entry, conn_handle, cid, p_value, length);
        }
        break;

#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
    case MCS_UUID_CHAR_SEARCH_CONTROL_POINT:
        {
            uint8_t res = SEARCH_POINT_SUCCESS_CODE;
            write_result.value.search_req_cb.p_search_param = p_value;
            write_result.value.search_req_cb.search_param_len = length;
            write_result.conn_handle = conn_handle;
            write_result.service_id = service_id;
            write_result.uuid = char_uuid.uu.char_uuid16;
            cb_result = ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCS_WRITE_REQ_IND, &write_result);
            if (cb_result != BLE_AUDIO_CB_RESULT_SUCCESS)
            {
                res = SEARCH_POINT_FAILURE_CODE;
            }
            if (!check_scp_type_valide(p_value, length))
            {
                res = SEARCH_POINT_FAILURE_CODE;
            }
            mcs_send_notify(p_entry, conn_handle, MCS_UUID_CHAR_SEARCH_CONTROL_POINT, &res, 1);
            if (res == SEARCH_POINT_SUCCESS_CODE)
            {
                handle_search_ctl_point_op(p_entry, conn_handle, p_value, length);
            }
        }
        break;
#endif

    default:
        {
            PROTOCOL_PRINT_ERROR1("mcs_post_callback attrib_index = %d not found", attrib_index);
        }
        break;
    }
}
/**
 * @brief write characteristic data from service.
 *
 * @param conn_handle
 * @param service_id        ServiceID to be written.
 * @param attrib_index      Attribute index of characteristic.
 * @param length            length of value to be written.
 * @param p_value           value to be written.
 * @return Profile procedure result
*/
T_APP_RESULT mcs_attr_write_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                               uint16_t attrib_index, T_WRITE_TYPE write_type,
                               uint16_t length, uint8_t *p_value,
                               P_FUN_EXT_WRITE_IND_POST_PROC *p_write_ind_post_proc)
{
    T_APP_RESULT  cause = APP_RESULT_SUCCESS;
    T_MCS_CB *p_entry = mcs_find_by_srv_id(service_id);
    T_MCS_WRITE_REQ write_result;

    if (p_entry == NULL)
    {
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    PROTOCOL_PRINT_INFO1("mcs_attr_write_cb write_type = 0x%x", write_type);
    *p_write_ind_post_proc = NULL;

    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_entry->p_attr_tbl,
                                                             attrib_index, p_entry->attr_num);

    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR2("mcs_attr_write_cb Error: attrib_index 0x%x, length %d",
                              attrib_index, length);
        return APP_RESULT_ATTR_NOT_FOUND;
    }

    switch (char_uuid.uu.char_uuid16)
    {
    case MCS_UUID_CHAR_TRACK_POSITION:
        {
            if (length != 4)
            {
                cause = APP_RESULT_INVALID_VALUE_SIZE;
            }
            else
            {
                LE_ARRAY_TO_UINT32(write_result.value.pos, p_value);

                write_result.conn_handle = conn_handle;
                write_result.service_id = service_id;
                write_result.uuid = char_uuid.uu.char_uuid16;
                cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCS_WRITE_REQ_IND, &write_result);
            }
        }
        break;

    case MCS_UUID_CHAR_PLAYBACK_SPEED:
        {
            if (length != 1)
            {
                cause = APP_RESULT_INVALID_VALUE_SIZE;
            }
            else
            {
                write_result.value.playback_speed = (int8_t) * p_value;
                write_result.conn_handle = conn_handle;
                write_result.service_id = service_id;
                write_result.uuid = char_uuid.uu.char_uuid16;

                cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCS_WRITE_REQ_IND, &write_result);
            }
        }
        break;

    case MCS_UUID_CHAR_PLAYING_ORDER:
        {
            if (length != 1)
            {
                cause = APP_RESULT_INVALID_VALUE_SIZE;
            }
            else
            {
                uint8_t order = *p_value;

                if ((order >= MCS_SINGLE_ONCE_ORDER) && (order < MCS_PLAYING_ORDER_MAX) &&
                    ((1 << (order - 1)) & p_entry->play_order_sup))
                {
                    write_result.value.playing_order = order;
                    write_result.conn_handle = conn_handle;
                    write_result.service_id = service_id;
                    write_result.uuid = char_uuid.uu.char_uuid16;
                    cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCS_WRITE_REQ_IND, &write_result);
                }
                else
                {
                    cause = APP_RESULT_APP_ERR;
                }
            }
        }
        break;

    case MCS_UUID_CHAR_MEDIA_CONTROL_POINT:
        {
            if (length < 1)
            {
                return APP_RESULT_INVALID_VALUE_SIZE;
            }

            uint8_t opcode = *p_value;
            if ((opcode == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_MOVE_RELATIVE ||
                 opcode == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_SEGMENT ||
                 opcode == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_TRACK ||
                 opcode == MCS_MEDIA_CONTROL_POINT_CHAR_OPCODE_GOTO_GROUP) &&
                length != 5)
            {
                cause = APP_RESULT_INVALID_VALUE_SIZE;
            }
        }
        break;

    case MCS_UUID_CHAR_SEARCH_CONTROL_POINT:
        if (length > 64)
        {
            cause = APP_RESULT_INVALID_VALUE_SIZE;
        }
        break;
#if LE_AUDIO_OTS_SERV_SUPPORT
    //inform APP first, if app return value is success, notify at post callback
    case MCS_UUID_CHAR_CURRENT_TRACK_OBJECT_ID:
        {
            if (length != OTS_OBJ_ID_LEN)
            {
                cause = APP_RESULT_INVALID_VALUE_SIZE;
            }
            else
            {
                write_result.value.obj_id = 0;
                memcpy(&write_result.value.obj_id, p_value, OTS_OBJ_ID_LEN);
                write_result.conn_handle = conn_handle;
                write_result.service_id = service_id;
                write_result.uuid = char_uuid.uu.char_uuid16;
                cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCS_WRITE_REQ_IND, &write_result);
                if (cause == APP_RESULT_SUCCESS)
                {
                    p_entry->cur_track_obj.obj_id = write_result.value.obj_id;
                }
            }
        }
        break;
    case MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID:
        {
            if (length != OTS_OBJ_ID_LEN)
            {
                cause = APP_RESULT_INVALID_VALUE_SIZE;
            }
            else
            {
                write_result.value.obj_id = 0;
                memcpy(&write_result.value.obj_id, p_value, OTS_OBJ_ID_LEN);
                write_result.conn_handle = conn_handle;
                write_result.service_id = service_id;
                write_result.uuid = char_uuid.uu.char_uuid16;
                cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCS_WRITE_REQ_IND, &write_result);
                if (cause == APP_RESULT_SUCCESS)
                {
                    p_entry->next_track_obj.obj_id = write_result.value.obj_id;
                }
            }
        }
        break;
    case MCS_UUID_CHAR_CURRENT_GROUP_OBJECT_ID:
        {
            if (length != OTS_OBJ_ID_LEN)
            {
                cause = APP_RESULT_INVALID_VALUE_SIZE;
            }
            else
            {
                write_result.value.obj_id = 0;
                memcpy(&write_result.value.obj_id, p_value, OTS_OBJ_ID_LEN);
                write_result.conn_handle = conn_handle;
                write_result.service_id = service_id;
                write_result.uuid = char_uuid.uu.char_uuid16;
                cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_MCS_WRITE_REQ_IND, &write_result);
                if (cause == APP_RESULT_SUCCESS)
                {
                    p_entry->current_group_obj.obj_id = write_result.value.obj_id;
                }
            }
        }
        break;
#endif
    default:
        {
            PROTOCOL_PRINT_ERROR1("mcs_attr_read_cb attrib_index = %d not found", attrib_index);
            cause  = APP_RESULT_ATTR_NOT_FOUND;
        }
        break;
    }
    if (cause == APP_RESULT_SUCCESS)
    {
        *p_write_ind_post_proc = mcs_write_post_callback;
    }
    return cause;
}

/**
 * @brief Media Control Service Callbacks.
*/
const T_FUN_GATT_EXT_SERVICE_CBS mcs_cbs =
{
    mcs_attr_read_cb,  // Read callback function pointer
    mcs_attr_write_cb, // Write callback function pointer
    mcs_cccd_update_cb  // CCCD update callback function pointer
};

static T_SERVER_ID mcs_add_service(T_MCS_CB *p_entry, void *p_func)
{
    T_SERVER_ID service_id;
    if (false == gatt_svc_add(&service_id,
                              (uint8_t *)p_entry->p_attr_tbl,
                              p_entry->attr_num * sizeof(T_ATTRIB_APPL),
                              &mcs_cbs, NULL))
    {
        PROTOCOL_PRINT_ERROR1("mcs_add_service fail: service_id %d", service_id);
        return 0xFF;
    }
    p_entry->service_id = service_id;
    return service_id;
}

static bool set_mcs_num_init(uint8_t mcs_num)
{
    if (mcs_num == 0 || p_mcs_mem != NULL)
    {
        PROTOCOL_PRINT_ERROR0("set_mcs_num_init: invalid param");
        return false;
    }
    p_mcs_mem = ble_audio_mem_zalloc(mcs_num * sizeof(T_MCS_CB));
    if (p_mcs_mem == NULL)
    {
        PROTOCOL_PRINT_ERROR0("set_mcs_num_init: allocate mem failed");
        return false;
    }
    total_mcs_num = mcs_num;
    return true;
}

static bool mcs_create_table(T_MCS_CB *p_entry, T_MCP_SERVER_REG_SRV_PARAM *p_param)
{
    uint16_t idx = 0;
    uint16_t attr_tbl_size = 0;

    if ((p_entry == NULL) || (p_param == NULL))
    {
        return false;
    }


    idx += 1;

#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
    T_OTS_SERV_CB *p_ots_entry;

    if ((p_param->ots.include_ots) && p_param->ots.ots_service_id != 0xFF)
    {
        p_ots_entry = ots_get_attr_tbl(p_entry->ots_service_id);
        if (p_ots_entry != NULL)
        {
            idx += 1;
        }
    }
#endif

    idx += 2;
    if ((p_param->gmcs) || (p_param->char_media_player_name.optional_property_notify))
    {
        idx += 1;
    }

#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
    if (p_param->char_media_player_icon_object_id.support)
    {
        idx += 2;
    }
#endif

    if (p_param->char_media_player_icon_url.support)
    {
        idx += 2;
    }
    idx += 3;
    idx += 2;

    if (p_param->char_track_title.optional_property_notify)
    {
        idx += 1;
    }
    idx += 2;

    if (p_param->char_track_duration.optional_property_notify)
    {
        idx += 1;
    }
    idx += 2;

    if (p_param->char_track_position.optional_property_notify)
    {
        idx += 1;
    }

    if (p_param->char_playback_speed.support)
    {
        if (p_param->char_playback_speed.optional_property_notify)
        {
            idx += 1;
        }
        idx += 2;
    }

    if (p_param->char_seeking_speed.support)
    {
        if (p_param->char_seeking_speed.optional_property_notify)
        {
            idx += 1;
        }
        idx += 2;
    }

#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
    if (p_param->char_current_track_object_id.support)
    {
        if (p_param->char_current_track_object_id.char_current_track_object_id_optional_property_notify)
        {
            idx += 1;
        }
        if (p_param->char_current_track_object_id.char_next_track_object_id_optional_property_notify)
        {
            idx += 1;
        }
        if (p_param->char_current_track_object_id.char_parent_group_object_id_optional_property_notify)
        {
            idx += 1;
        }
        if (p_param->char_current_track_object_id.char_current_group_object_id_optional_property_notify)
        {
            idx += 1;
        }
        idx += 10;
    }
#endif

    if (p_param->char_playing_order.support)
    {
        if (p_param->char_playing_order.optional_property_notify)
        {
            idx += 1;
        }
        idx += 2;
    }

    if (p_param->char_playing_orders_supported.support)
    {
        idx += 2;
    }
    idx += 3;

    if (p_param->char_media_control_point.support)
    {
        if (p_param->char_media_control_point.char_media_control_point_opcodes_supported_optional_property_notify)
        {
            idx += 1;
        }
        idx += 5;
    }

#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
    if (p_param->char_search_results_object_id.support)
    {
        idx += 6;
    }
#endif

    idx += 2;
#if LE_AUDIO_INIT_DEBUG
    PROTOCOL_PRINT_TRACE1("mcs_create_table: total idx: %d", idx);
#endif
    p_entry->attr_num = idx;
    attr_tbl_size = idx * sizeof(T_ATTRIB_APPL);
    p_entry->p_attr_tbl = ble_audio_mem_zalloc(attr_tbl_size);

    if (p_entry->p_attr_tbl == NULL)
    {
        PROTOCOL_PRINT_ERROR0("mcs_create_table: allocate p_attr_tbl failed");
        return false;
    }
    idx = 0;
    memcpy(p_entry->p_attr_tbl, (uint8_t *)&mcs_prim_srv, sizeof(T_ATTRIB_APPL));
    if (!p_entry->gmcs)
    {
        p_entry->p_attr_tbl[0].type_value[2] = LO_WORD(GATT_UUID_MEDIA_CONTROL_SERVICE);
        p_entry->p_attr_tbl[0].type_value[3] = HI_WORD(GATT_UUID_MEDIA_CONTROL_SERVICE);
    }
    idx++;

#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
    if ((p_param->ots.include_ots) && p_param->ots.ots_service_id != 0xFF)
    {
        T_OTS_SERV_CB *p_ots_entry = ots_get_attr_tbl(p_entry->ots_service_id);
        if (p_ots_entry != NULL)
        {
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_inc_srv, sizeof(T_ATTRIB_APPL));
            p_entry->p_attr_tbl[idx].p_value_context = (void *)p_ots_entry->p_attr_tbl;
            p_entry->p_attr_tbl[idx].value_len = 0;
            idx += 1;
        }
    }
#endif

    if ((p_param->gmcs) || (p_param->char_media_player_name.optional_property_notify))
    {
        memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_mpn_mand_opt_attr,
               3 * sizeof(T_ATTRIB_APPL));
        idx += 3;
    }
    else
    {
        memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_mpn_mand_opt_attr,
               2 * sizeof(T_ATTRIB_APPL));
        p_entry->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_NOTIFY);
        idx += 2;
    }

#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
    if (p_param->char_media_player_icon_object_id.support)
    {
        memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_mpio_id_opt_attr,
               2 * sizeof(T_ATTRIB_APPL));
        idx += 2;
    }
#endif

    if (p_param->char_media_player_icon_url.support)
    {
        memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_mpi_url_opt_attr,
               2 * sizeof(T_ATTRIB_APPL));
        idx += 2;
    }

    memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_tk_chg_mand_attr,
           3 * sizeof(T_ATTRIB_APPL));
    idx += 3;

    if (p_param->char_track_title.optional_property_notify)
    {
        memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_tk_title_mand_opt_attr,
               3 * sizeof(T_ATTRIB_APPL));
        idx += 3;
    }
    else
    {
        memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_tk_title_mand_opt_attr,
               2 * sizeof(T_ATTRIB_APPL));
        p_entry->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_NOTIFY);
        idx += 2;
    }

    if (p_param->char_track_duration.optional_property_notify)
    {
        memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_tk_dur_mand_opt_attr,
               3 * sizeof(T_ATTRIB_APPL));
        idx += 3;
    }
    else
    {
        memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_tk_dur_mand_opt_attr,
               2 * sizeof(T_ATTRIB_APPL));
        p_entry->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_NOTIFY);
        idx += 2;
    }

    if (p_param->char_track_position.optional_property_notify)
    {
        memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_tk_pos_mand_opt_attr,
               3 * sizeof(T_ATTRIB_APPL));
        idx += 3;
    }
    else
    {
        memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_tk_pos_mand_opt_attr,
               2 * sizeof(T_ATTRIB_APPL));
        p_entry->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_NOTIFY);
        idx += 2;
    }

    if (p_param->char_playback_speed.support)
    {
        if (p_param->char_playback_speed.optional_property_notify)
        {
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_pb_speed_opt_opt_attr,
                   3 * sizeof(T_ATTRIB_APPL));
            idx += 3;
        }
        else
        {
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_pb_speed_opt_opt_attr,
                   2 * sizeof(T_ATTRIB_APPL));
            p_entry->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_NOTIFY);
            idx += 2;
        }
    }

    if (p_param->char_seeking_speed.support)
    {
        if (p_param->char_seeking_speed.optional_property_notify)
        {
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_sk_speed_opt_opt_attr,
                   3 * sizeof(T_ATTRIB_APPL));
            idx += 3;
        }
        else
        {
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_sk_speed_opt_opt_attr,
                   2 * sizeof(T_ATTRIB_APPL));
            p_entry->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_NOTIFY);
            idx += 2;
        }
    }

#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
    if (p_param->char_current_track_object_id.support)
    {
        if (p_param->char_current_track_object_id.char_current_track_object_id_optional_property_notify)
        {
            p_entry->cur_track_obj.char_index = idx + 1;
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_cto_id_opt_opt_attr,
                   3 * sizeof(T_ATTRIB_APPL));
            idx += 3;
        }
        else
        {
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_cto_id_opt_opt_attr,
                   2 * sizeof(T_ATTRIB_APPL));
            p_entry->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_NOTIFY);
            idx += 2;
        }
        memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_ctso_id_opt_attr,
               2 * sizeof(T_ATTRIB_APPL));
        idx += 2;

        if (p_param->char_current_track_object_id.char_next_track_object_id_optional_property_notify)
        {
            p_entry->next_track_obj.char_index = idx + 1;
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_nto_id_opt_opt_attr,
                   3 * sizeof(T_ATTRIB_APPL));
            idx += 3;
        }
        else
        {
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_nto_id_opt_opt_attr,
                   2 * sizeof(T_ATTRIB_APPL));
            p_entry->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_NOTIFY);
            idx += 2;
        }

        if (p_param->char_current_track_object_id.char_parent_group_object_id_optional_property_notify)
        {
            p_entry->parent_group_obj.char_index = idx + 1;
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_pgo_id_opt_opt_attr,
                   3 * sizeof(T_ATTRIB_APPL));
            idx += 3;
        }
        else
        {
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_pgo_id_opt_opt_attr,
                   2 * sizeof(T_ATTRIB_APPL));
            p_entry->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_NOTIFY);
            idx += 2;
        }

        if (p_param->char_current_track_object_id.char_current_group_object_id_optional_property_notify)
        {
            p_entry->current_group_obj.char_index = idx + 1;
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_cgo_id_opt_opt_attr,
                   3 * sizeof(T_ATTRIB_APPL));
            idx += 3;
        }
        else
        {
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_cgo_id_opt_opt_attr,
                   2 * sizeof(T_ATTRIB_APPL));
            p_entry->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_NOTIFY);
            idx += 2;
        }
    }
#endif

    if (p_param->char_playing_order.support)
    {
        if (p_param->char_playing_order.optional_property_notify)
        {
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_playing_odr_opt_opt_attr,
                   3 * sizeof(T_ATTRIB_APPL));
            idx += 3;
        }
        else
        {
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_playing_odr_opt_opt_attr,
                   2 * sizeof(T_ATTRIB_APPL));
            p_entry->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_NOTIFY);
            idx += 2;
        }
    }

    if (p_param->char_playing_orders_supported.support)
    {
        memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_playing_odr_sup_opt_attr,
               2 * sizeof(T_ATTRIB_APPL));
        idx += 2;
    }
    memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_media_st_mand_attr,
           3 * sizeof(T_ATTRIB_APPL));
    idx += 3;

    if (p_param->char_media_control_point.support)
    {
        memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_mcp_opt_attr,
               3 * sizeof(T_ATTRIB_APPL));
        idx += 3;

        if (p_param->char_media_control_point.char_media_control_point_opcodes_supported_optional_property_notify)
        {
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_mcpo_sup_opt_opt_attr,
                   3 * sizeof(T_ATTRIB_APPL));
            idx += 3;
        }
        else
        {
            memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_mcpo_sup_opt_opt_attr,
                   2 * sizeof(T_ATTRIB_APPL));
            p_entry->p_attr_tbl[idx].type_value[2] &= (~GATT_CHAR_PROP_NOTIFY);
            idx += 2;
        }
    }

#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
    if (p_param->char_search_results_object_id.support)
    {
        p_entry->search_res_obj.char_index = idx + 4;
        memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_sro_id_opt_attr,
               6 * sizeof(T_ATTRIB_APPL));
        idx += 6;
    }
#endif

    memcpy((uint8_t *)&p_entry->p_attr_tbl[idx], (uint8_t *)&mcs_cccid_mand_attr,
           2 * sizeof(T_ATTRIB_APPL));
    idx += 2;
#if LE_AUDIO_INIT_DEBUG
    PROTOCOL_PRINT_TRACE2("mcs_create_table: create idx: %d, attr_tbl_size: %d", idx,
                          attr_tbl_size);
#endif
    return true;
}

uint8_t mcp_server_reg_srv(T_MCP_SERVER_REG_SRV_PARAM *p_param)
{
    T_MCS_CB *p_entry = mcs_allocate_entry(p_param);

    if (p_entry == NULL)
    {
        return 0xFF;
    }
    if (mcs_create_table(p_entry, p_param))
    {
        if (mcs_add_service(p_entry, NULL) == 0xFF)
        {
            goto error;
        }
    }
    else
    {
        PROTOCOL_PRINT_ERROR0("mcp_server_reg_srv: create table fail");
        goto error;
    }

    return p_entry->service_id;
error:
    if (p_entry->p_attr_tbl != NULL)
    {
        ble_audio_mem_free(p_entry->p_attr_tbl);
        p_entry->p_attr_tbl = NULL;
    }
    p_entry->used = false;
    return 0xFF;
}


bool mcs_server_init(uint8_t mcs_num)
{
    if (set_mcs_num_init(mcs_num))
    {
        return true;
    }

    return false;
}


bool mcp_server_read_cfm(T_MCP_SERVER_READ_CFM *p_read_cfm)
{
    if (p_read_cfm)
    {
        T_MCS_CB *p_entry = mcs_find_by_srv_id(p_read_cfm->service_id);
        uint16_t attrib_idx;
        T_CHAR_UUID char_uuid;
        char_uuid.index = 1;
        char_uuid.uuid_size = UUID_16BIT_SIZE;
        char_uuid.uu.char_uuid16 = p_read_cfm->char_uuid;

        if (p_entry == NULL)
        {
            PROTOCOL_PRINT_ERROR0("mcp_server_read_cfm, can't find entry");
            return false;
        }

        if (!gatt_svc_find_char_index_by_uuid(p_entry->p_attr_tbl, char_uuid,
                                              p_entry->attr_num, &attrib_idx))
        {
            return false;
        }

        switch (p_read_cfm->char_uuid)
        {
        case MCS_UUID_CHAR_MEDIA_PLAYER_NAME:
            {
                uint8_t data[p_read_cfm->param.media_player_name.media_player_name_len];

                memcpy(data, p_read_cfm->param.media_player_name.p_media_player_name,
                       p_read_cfm->param.media_player_name.media_player_name_len);

                if (p_read_cfm->offset > p_read_cfm->param.media_player_name.media_player_name_len)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset, p_read_cfm->param.media_player_name.media_player_name_len -
                                             p_read_cfm->offset,
                                             p_read_cfm->param.media_player_name.media_player_name_len, (T_APP_RESULT)(p_read_cfm->cause));
            }

        case MCS_UUID_CHAR_MEDIA_PLAYER_ICON_URL:
            {
                uint8_t data[p_read_cfm->param.media_player_icon_url.media_player_icon_url_len];

                memcpy(data, p_read_cfm->param.media_player_icon_url.p_media_player_icon_url,
                       p_read_cfm->param.media_player_icon_url.media_player_icon_url_len);

                if (p_read_cfm->offset > p_read_cfm->param.media_player_icon_url.media_player_icon_url_len)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset, p_read_cfm->param.media_player_icon_url.media_player_icon_url_len -
                                             p_read_cfm->offset,
                                             p_read_cfm->param.media_player_icon_url.media_player_icon_url_len,
                                             (T_APP_RESULT)(p_read_cfm->cause));
            }

        case MCS_UUID_CHAR_TRACK_TITLE:
            {
                uint8_t data[p_read_cfm->param.track_title.track_title_len];

                memcpy(data, p_read_cfm->param.track_title.p_track_title,
                       p_read_cfm->param.track_title.track_title_len);

                if (p_read_cfm->offset > p_read_cfm->param.track_title.track_title_len)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset, p_read_cfm->param.track_title.track_title_len -
                                             p_read_cfm->offset,
                                             p_read_cfm->param.track_title.track_title_len, (T_APP_RESULT)(p_read_cfm->cause));
            }

        case MCS_UUID_CHAR_TRACK_DURATION:
            {
                uint8_t data[4];

                LE_UINT32_TO_ARRAY(data, p_read_cfm->param.track_duration);

                if (p_read_cfm->offset > 4)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset, 4 - p_read_cfm->offset, 4, (T_APP_RESULT)(p_read_cfm->cause));
            }

        case MCS_UUID_CHAR_TRACK_POSITION:
            {
                uint8_t data[4];

                LE_UINT32_TO_ARRAY(data, p_read_cfm->param.track_position);

                if (p_read_cfm->offset > 4)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset, 4 - p_read_cfm->offset, 4, (T_APP_RESULT)(p_read_cfm->cause));
            }

        case MCS_UUID_CHAR_PLAYBACK_SPEED:
            {
                uint8_t data[1];

                data[0] = p_read_cfm->param.playback_speed;

                if (p_read_cfm->offset > 1)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset, 1 - p_read_cfm->offset, 1, (T_APP_RESULT)(p_read_cfm->cause));
            }

        case MCS_UUID_CHAR_SEEKING_SPEED:
            {
                uint8_t data[1];

                data[0] = p_read_cfm->param.seeking_speed;

                if (p_read_cfm->offset > 1)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset, 1 - p_read_cfm->offset, 1, (T_APP_RESULT)(p_read_cfm->cause));
            }

        case MCS_UUID_CHAR_PLAYING_ORDER:
            {
                uint8_t data[1];

                data[0] = p_read_cfm->param.playing_order;

                if (p_read_cfm->offset > 1)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset, 1 - p_read_cfm->offset, 1, (T_APP_RESULT)(p_read_cfm->cause));
            }

        case MCS_UUID_CHAR_CONTENT_CONTROL_ID:
            {
                uint8_t data[1];

                data[0] = p_read_cfm->param.content_control_id;

                if (p_read_cfm->offset > 1)
                {
                    return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid, p_read_cfm->service_id,
                                                 attrib_idx, NULL, 0, 0, APP_RESULT_INVALID_OFFSET);
                }

                return gatt_svc_read_confirm(p_read_cfm->conn_handle, p_read_cfm->cid,
                                             p_read_cfm->service_id,
                                             attrib_idx,
                                             data + p_read_cfm->offset, 1 - p_read_cfm->offset, 1, (T_APP_RESULT)(p_read_cfm->cause));
            }

        default:
            break;
        }
    }

    return false;
}

bool mcp_server_set_param(uint8_t service_id, T_MCP_SERVER_SET_PARAM *p_param)
{
    T_MCS_CB *p_entry = mcs_find_by_srv_id(service_id);

    if (p_entry == NULL || p_param == NULL)
    {
        return false;
    }

    switch (p_param->char_uuid)
    {
    case MCS_UUID_CHAR_PLAYING_ORDERS_SUPPORTED:
        {
            p_entry->play_order_sup = p_param->param.playing_orders_supported;
        }
        break;

    case MCS_UUID_CHAR_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED:
        {
            uint8_t data[4];

            p_entry->media_control_point_opcodes_supported =
                p_param->param.media_control_point_opcodes_supported;

            LE_UINT32_TO_ARRAY(data, p_param->param.media_control_point_opcodes_supported);

            mcs_send_notify_all(p_entry, MCS_UUID_CHAR_MEDIA_CONTROL_POINT_OPCODES_SUPPORTED, data, 4);
        }
        break;

    case MCS_UUID_CHAR_MEDIA_STATE:
        {
            p_entry->media_state = p_param->param.media_state;

            mcs_send_notify_all(p_entry, MCS_UUID_CHAR_MEDIA_STATE, &p_entry->media_state, 1);
        }
        break;

#if LE_AUDIO_MCP_SERVER_SUPPORT_INTERNAL
#if LE_AUDIO_OTS_SERV_SUPPORT
    case MCS_UUID_CHAR_MEDIA_PLAYER_ICON_OBJECT_ID:
        {
            if (p_entry->ply_icon_obj_id != 0)
            {
                delete_ots_object_internal(p_entry->ots_service_id, p_entry->ply_icon_obj_id);
                p_entry->ply_icon_obj_id = 0;
            }

            if (p_info->icon_fmt_len > 0)
            {
                T_OTS_OBJ_PARAM param;
                uint16_t uuid = MEDIA_PLAYER_ICON_OBJ_TYPE;
                param.uuid_size = UUID_16BIT_SIZE;
                param.p_uuid = (uint8_t *)&uuid;
                param.p_name = p_info->p_py_name;
                param.name_len = p_info->py_name_len;
                param.p_value = p_info->p_icon_fmt;
                param.len = p_info->icon_fmt_len;
                param.props = (1 << OBJ_PROP_READ_BIT_NUM);

                T_OBJECT_CB *p_obj = create_ots_object_internal(p_entry->ots_service_id, &param);
                if (p_obj != NULL)
                {
                    p_entry->ply_icon_obj_id = p_obj->obj_id;
                }
            }
        }
        break;

    case MCS_UUID_CHAR_CURRENT_TRACK_OBJECT_ID:
        if (p_tk_info->tk_obj_id != 0)
        {
            T_TK_ELEM *p_tk_elem = mcs_find_tk_by_tk_id(p_entry, p_tk_info->tk_obj_id);

            if (p_tk_elem != NULL)
            {
                if (p_entry->cur_track_obj.obj_id != p_tk_info->tk_obj_id)
                {
                    p_entry->cur_track_obj.obj_id = p_tk_info->tk_obj_id;
                    if (p_entry->cur_track_obj.char_index > 0)
                    {
                        mcs_send_notify_all(p_entry, MCS_UUID_CHAR_CURRENT_TRACK_OBJECT_ID,
                                            (uint8_t *)(&p_entry->cur_track_obj.obj_id), OTS_OBJ_ID_LEN);
                    }
                }
                p_entry->cur_tkseg_obj_id = p_tk_elem->seg_obj_id;
                p_entry->parent_group_obj.obj_id = mcs_find_parent_id_by_gp_id(p_entry, p_tk_elem->gp_obj_id);
                if (p_entry->parent_group_obj.char_index > 0)
                {
                    mcs_send_notify_all(p_entry, MCS_UUID_CHAR_PARENT_GROUP_OBJECT_ID,
                                        (uint8_t *)(&p_entry->parent_group_obj.obj_id), OTS_OBJ_ID_LEN);
                }

                p_entry->current_group_obj.obj_id = p_tk_elem->gp_obj_id;
                if (p_entry->current_group_obj.char_index > 0)
                {
                    mcs_send_notify_all(p_entry, MCS_UUID_CHAR_CURRENT_GROUP_OBJECT_ID,
                                        (uint8_t *)(&p_entry->current_group_obj.obj_id), OTS_OBJ_ID_LEN);
                }
            }
        }
        break;

    case MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID:
        if (p_tk_info->next_tk_obj_id != 0)
        {
            p_entry->next_track_obj.obj_id = p_tk_info->next_tk_obj_id;
            if (p_entry->next_track_obj.char_index > 0)
            {
                mcs_send_notify_all(p_entry, MCS_UUID_CHAR_NEXT_TRACK_OBJECT_ID,
                                    (uint8_t *)(&p_entry->next_track_obj.obj_id), OTS_OBJ_ID_LEN);
            }
        }
        break;
#endif
#endif
    default:
        return false;
    }

    return true;
}

bool mcp_server_send_data(uint8_t service_id, T_MCP_SERVER_SEND_DATA_PARAM *p_param)
{
    T_MCS_CB *p_entry = mcs_find_by_srv_id(service_id);

    if (p_entry == NULL || p_param == NULL)
    {
        return false;
    }

    switch (p_param->char_uuid)
    {
    case MCS_UUID_CHAR_MEDIA_PLAYER_NAME:
        {
            return mcs_send_notify_all(p_entry, MCS_UUID_CHAR_MEDIA_PLAYER_NAME,
                                       p_param->param.media_player_name.p_media_player_name,
                                       p_param->param.media_player_name.media_player_name_len);
        }

    case MCS_UUID_CHAR_TRACK_TITLE:
        {
            return mcs_send_notify_all(p_entry, MCS_UUID_CHAR_TRACK_TITLE,
                                       p_param->param.track_title.p_track_title, p_param->param.track_title.track_title_len);
        }

    case MCS_UUID_CHAR_TRACK_DURATION:
        {
            uint8_t data[4];

            LE_UINT32_TO_ARRAY(data, p_param->param.track_duration);

            return mcs_send_notify_all(p_entry, MCS_UUID_CHAR_TRACK_DURATION, data, 4);
        }

    case MCS_UUID_CHAR_TRACK_POSITION:
        {
            uint8_t data[4];

            LE_UINT32_TO_ARRAY(data, p_param->param.track_position);

            return mcs_send_notify_all(p_entry, MCS_UUID_CHAR_TRACK_POSITION, data, 4);
        }

    case MCS_UUID_CHAR_TRACK_CHANGED:
        {
            return mcs_send_notify_all(p_entry, MCS_UUID_CHAR_TRACK_CHANGED, NULL, 0);
        }

    case MCS_UUID_CHAR_PLAYBACK_SPEED:
        {
            return mcs_send_notify_all(p_entry, MCS_UUID_CHAR_PLAYBACK_SPEED,
                                       (uint8_t *) & (p_param->param.playback_speed), 1);
        }

    case MCS_UUID_CHAR_SEEKING_SPEED:
        {
            return mcs_send_notify_all(p_entry, MCS_UUID_CHAR_SEEKING_SPEED,
                                       (uint8_t *) & (p_param->param.seeking_speed), 1);
        }

    case MCS_UUID_CHAR_PLAYING_ORDER:
        {
            return mcs_send_notify_all(p_entry, MCS_UUID_CHAR_PLAYING_ORDER,
                                       (uint8_t *) & (p_param->param.playing_order), 1);
        }

    default:
        return false;
    }
}

#if LE_AUDIO_OTS_SERV_SUPPORT
uint64_t mcs_create_root_group(uint8_t service_id, T_MCS_GP_ELEM *p_gp_elem)
{
    T_MCS_CB *p_entry = mcs_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR0("mcs_create_root_group, can't find mcs entry");
        return 0;
    }

    p_entry->root_grp_obj_id = mcs_create_root_gp(p_entry, p_gp_elem);
    return p_entry->root_grp_obj_id;
}

uint64_t mcs_create_major_group(uint8_t service_id, T_MCS_GP_ELEM *p_gp_elem,
                                uint64_t parent_obj_id)
{
    T_MCS_CB *p_entry = mcs_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR0("mcs_create_major_group, can't find mcs entry");
        return 0;
    }

    return mcs_create_spec_gp(p_entry, p_gp_elem, parent_obj_id);
}

uint64_t mcs_create_major_track(uint8_t service_id, T_MCS_TK_ELEM *p_tk_elem,
                                uint64_t gp_obj_id, uint8_t *p_id3v2_fmt, uint16_t fmt_len)
{
    T_MCS_CB *p_entry = mcs_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR0("mcs_create_major_track, can't find mcs entry");
        return 0;
    }

    return mcs_create_track(p_entry, p_tk_elem, gp_obj_id, p_id3v2_fmt, fmt_len);
}


void mcs_delete_major_gp(uint8_t service_id, uint64_t gp_obj_id)
{
    T_MCS_CB *p_entry = mcs_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR0("mcs_delete_major_gp, can't find mcs entry");
        return;
    }

    mcs_delete_gp(p_entry, gp_obj_id);
}

void mcs_delete_major_track(uint8_t service_id, uint64_t gp_obj_id, uint64_t tk_obj_id)
{
    T_MCS_CB *p_entry = mcs_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR0("mcs_delete_major_track, can't find mcs entry");
        return;
    }

    mcs_delete_track(p_entry, gp_obj_id, tk_obj_id);
}

void mcs_update_major_gp_value(uint8_t service_id, uint64_t gp_obj_id)
{
    T_MCS_CB *p_entry = mcs_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR0("mcs_update_major_gp_value, can't find mcs entry");
        return;
    }

    mcs_update_gp_value(p_entry, gp_obj_id);
}

void mcs_make_audio_struct_complt(uint8_t service_id)
{
    T_MCS_CB *p_entry = mcs_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR0("mcs_make_audio_struct_complt, can't find mcs entry");
        return;
    }

    mcs_create_objs_complt(p_entry);
}
#endif

#if LE_AUDIO_OTS_SERV_SUPPORT
bool mcs_local_search_op(uint8_t service_id, uint8_t *p_value, uint16_t len)
{
    T_MCS_CB *p_entry = mcs_find_by_srv_id(service_id);

    if (p_entry == NULL)
    {
        PROTOCOL_PRINT_ERROR0("mcs_local_search_op, can't find mcs entry");
        return false;
    }

    T_OS_QUEUE *temp_queue = ots_get_obj_queue(p_entry->ots_service_id);
    uint16_t total_len = 0;
    bool  need_ind = false;
    int i, j;
    if (len == 0)
    {
        p_entry->search_res_obj.obj_id = 0;
        return true;
    }

    uint8_t *p_res = ble_audio_mem_zalloc(temp_queue->count * (OTS_OBJ_ID_LEN + 1));
    if (p_res == NULL)
    {
        PROTOCOL_PRINT_ERROR0("mcs_local_search_op, alloc fail");
        return false;
    }

    uint8_t only_type = check_scp_only_type(p_value, len);
    if (only_type == 0xFF)
    {
        PROTOCOL_PRINT_ERROR0("mcs_local_search_op, check only type error");
        ble_audio_mem_free(p_res);
        return false;
    }

    if (p_entry->search_res_obj.obj_id != 0)
    {
        //FIX TODO need to update root group value?
        mcs_delete_major_gp(p_entry->service_id, p_entry->search_res_obj.obj_id);
        delete_ots_object_internal(p_entry->ots_service_id, p_entry->search_res_obj.obj_id);
        need_ind = true;
    }

    for (i = 0; i < p_entry->gp_obj_queue.count; i++)
    {
        T_GP_ELEM *p_gp = (T_GP_ELEM *)os_queue_peek(&p_entry->gp_obj_queue, i);
        if (only_type != MCS_ONLY_TRACKS_TYPE)
        {
            if (filt_gp_obj_by_scp_type(&p_gp->gp_elem, p_value, len))
            {
                p_res[total_len++] = MCS_GROUP_OBJ_TYPE;
                memcpy(&p_res[total_len], (uint8_t *)&p_gp->obj_id, OTS_OBJ_ID_LEN);
                total_len += OTS_OBJ_ID_LEN;
            }
        }
        if (only_type != MCS_ONLY_GROUPS_TYPE)
        {
            for (j = 0; j < p_gp->tr_queue.count; j++)
            {
                T_TK_ELEM *p_tk = (T_TK_ELEM *)os_queue_peek(&p_gp->tr_queue, j);
                if (p_tk && filt_tk_obj_by_scp_type(&p_tk->tk_elem, p_value, len))
                {
                    p_res[total_len++] = MCS_TRACK_OBJ_TYPE;
                    memcpy(&p_res[total_len], (uint8_t *)&p_tk->obj_id, OTS_OBJ_ID_LEN);
                    total_len += OTS_OBJ_ID_LEN;
                }
            }
        }
    }

    PROTOCOL_PRINT_TRACE2("mcs_local_search_op, search result obj id = 0x%x, total_len = %d",
                          (int)p_entry->search_res_obj.obj_id, total_len);

    if (total_len == 0)
    {
        p_entry->search_res_obj.obj_id = 0;
    }
    else
    {
        T_MCS_GP_ELEM gp_elem = {0};
        gp_elem.p_gp_name = (uint8_t *)"mcs search res";      //we should use utf-8 format
        gp_elem.gp_name_len = strlen("mcs search res") + 1;
        gp_elem.p_gp_value = p_res;
        gp_elem.gp_value_len = total_len;

        p_entry->search_res_obj.obj_id = mcs_create_major_group(p_entry->service_id, &gp_elem,
                                                                p_entry->root_grp_obj_id);

        if (p_entry->search_res_obj.obj_id != 0)
        {
            need_ind = true;
        }
        else
        {
            p_entry->search_res_obj.obj_id = 0;
        }
    }

    if (need_ind)
    {
        T_MCS_SCP_RES scp_res;
        mcs_send_notify_all(p_entry, MCS_UUID_CHAR_SEARCH_RESULTS_OBJECT_ID,
                            p_entry->search_res_obj.obj_id == 0 ? NULL : (uint8_t *)&p_entry->search_res_obj.obj_id,
                            p_entry->search_res_obj.obj_id == 0 ? 0 : OTS_OBJ_ID_LEN);

        scp_res.service_id = p_entry->service_id;
        scp_res.search_obj_id = p_entry->search_res_obj.obj_id;
        scp_res.p_value = (T_MCS_GP_OBJ *)p_res;
        scp_res.num = total_len / sizeof(T_MCS_GP_OBJ);
        ble_audio_mgr_dispatch(LE_AUDIO_MSG_SCP_RESULT, &scp_res);
    }
    ble_audio_mem_free(p_res);
    return true;
}
#endif
#if LE_AUDIO_DEINIT
void mcs_server_deinit(void)
{
    if (p_mcs_mem)
    {
        T_MCS_CB *p_entry;
#if LE_AUDIO_OTS_SERV_SUPPORT
        T_GP_ELEM  *p_gp;
#endif
        for (uint8_t i = 0; i < total_mcs_num; i++)
        {
            p_entry = &p_mcs_mem[i];
#if LE_AUDIO_OTS_SERV_SUPPORT
            while ((p_gp = os_queue_out(&p_entry->gp_obj_queue)) != NULL)
            {
                T_TK_ELEM *p_tk;
                while ((p_tk = os_queue_out(&p_gp->tr_queue)) != NULL)
                {
                    ble_audio_mem_free(p_tk);
                }
                ble_audio_mem_free(p_gp);
            }
#endif
            if (p_entry->p_attr_tbl)
            {
                ble_audio_mem_free(p_entry->p_attr_tbl);
            }
        }
        ble_audio_mem_free(p_mcs_mem);
        p_mcs_mem = NULL;
    }
    total_mcs_num = 0;
}
#endif
#endif

