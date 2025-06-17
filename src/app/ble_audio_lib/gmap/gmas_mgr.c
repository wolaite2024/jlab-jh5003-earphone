#include <stdint.h>
#include <string.h>
#include "trace.h"
#include "bt_types.h"
#include "ble_audio_dm.h"
#include "ble_audio_mgr.h"
#include "gmas_mgr.h"

/* base on GMAS_v1.0 */
#if LE_AUDIO_GMAS_SUPPORT

typedef struct
{
    T_SERVER_ID       service_id;
    uint8_t           gmap_role;
    uint16_t          attr_num;
    T_ATTRIB_APPL     *p_attr_tbl;
} T_GMAS;

T_GMAS *p_gmas = NULL;

/**< @brief  profile/service definition.  */
const T_ATTRIB_APPL gmas_attr_tbl[] =
{
    /*----------------- Gaming Audio Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),  /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_GMAS),                /* service UUID */
            HI_WORD(GATT_UUID_GMAS)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },
    /* <<Characteristic>>, ..GMAP Role */
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
    /* GMAP Role value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GMAS_UUID_CHAR_GMAP_ROLE),
            HI_WORD(GMAS_UUID_CHAR_GMAP_ROLE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    }
};

const T_ATTRIB_APPL ugg_features_char_tbl[] =
{
    /* <<Characteristic>>, .. UGG Features */
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
    /* UGG Features value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GMAS_UUID_CHAR_UGG_FEATURES),
            HI_WORD(GMAS_UUID_CHAR_UGG_FEATURES)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    }
};

const T_ATTRIB_APPL ugt_features_char_tbl[] =
{
    /* <<Characteristic>>, .. UGT Features */
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
    /* UGT Features value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GMAS_UUID_CHAR_UGT_FEATURES),
            HI_WORD(GMAS_UUID_CHAR_UGT_FEATURES)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    }
};

const T_ATTRIB_APPL bgs_features_char_tbl[] =
{
    /* <<Characteristic>>, .. BGS Features */
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
    /* BGS Features value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GMAS_UUID_CHAR_BGS_FEATURES),
            HI_WORD(GMAS_UUID_CHAR_BGS_FEATURES)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    }
};

const T_ATTRIB_APPL bgr_features_char_tbl[] =
{
    /* <<Characteristic>>, .. BGR Features */
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
    /* BGR Features value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(GMAS_UUID_CHAR_BGR_FEATURES),
            HI_WORD(GMAS_UUID_CHAR_BGR_FEATURES)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    }
};

bool gmas_features_read_confirm(uint16_t conn_handle, uint16_t cid,
                                uint16_t char_uuid, uint8_t features)
{
    bool ret = true;
    switch (char_uuid)
    {
    case GMAS_UUID_CHAR_UGG_FEATURES:
        {
            if (!(p_gmas->gmap_role & GMAP_ROLE_CHAR_BIT_VALUE_UGG))
            {
                ret = false;
            }
        }
        break;

    case GMAS_UUID_CHAR_UGT_FEATURES:
        {
            if (!(p_gmas->gmap_role & GMAP_ROLE_CHAR_BIT_VALUE_UGT))
            {
                ret = false;
            }
        }
        break;

    case GMAS_UUID_CHAR_BGS_FEATURES:
        {
            if (!(p_gmas->gmap_role & GMAP_ROLE_CHAR_BIT_VALUE_BGS))
            {
                ret = false;
            }
        }
        break;

    case GMAS_UUID_CHAR_BGR_FEATURES:
        {
            if (!(p_gmas->gmap_role & GMAP_ROLE_CHAR_BIT_VALUE_BGR))
            {
                ret = false;
            }
        }
        break;

    default:
        {
            ret = false;
        }
        break;
    }

    if (ret)
    {
        uint16_t attrib_idx = gatt_svc_find_char_index_by_uuid16(p_gmas->p_attr_tbl,
                                                                 char_uuid,
                                                                 p_gmas->attr_num);

        return gatt_svc_read_confirm(conn_handle, cid, p_gmas->service_id,
                                     attrib_idx, &features, sizeof(uint8_t),
                                     sizeof(uint8_t), APP_RESULT_SUCCESS);
    }
    else
    {
        PROTOCOL_PRINT_ERROR1("gmas_features_read_confirm fail: char_uuid 0x%x not support", char_uuid);
        return ret;
    }
}

static T_APP_RESULT gmas_attr_read_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                      uint16_t attrib_index,
                                      uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;
    T_GMAS_READ_IND read_ind = {0};
    PROTOCOL_PRINT_INFO2("gmas_attr_read_cb: attrib_index %d, offset %x", attrib_index, offset);

    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(p_gmas->p_attr_tbl, attrib_index,
                                                             p_gmas->attr_num);
    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR1("gmas_attr_read_cb Error: attrib_index 0x%x", attrib_index);
        return APP_RESULT_ATTR_NOT_FOUND;
    }
    *p_length = 0;

    read_ind.conn_handle = conn_handle;
    read_ind.cid = cid;
    read_ind.char_uuid = char_uuid.uu.char_uuid16;

    if (char_uuid.uu.char_uuid16 == GMAS_UUID_CHAR_GMAP_ROLE)
    {
        *p_length = sizeof(uint8_t);
        *pp_value = &p_gmas->gmap_role;

        PROTOCOL_PRINT_TRACE1("gmas_attr_read_cb: read GMAP role 0x%x", p_gmas->gmap_role);
        return cause;
    }

    switch (char_uuid.uu.char_uuid16)
    {
    case GMAS_UUID_CHAR_UGG_FEATURES:
        {
            if (!(p_gmas->gmap_role & GMAP_ROLE_CHAR_BIT_VALUE_UGG))
            {
                cause = APP_RESULT_ATTR_NOT_FOUND;
            }
        }
        break;
    case GMAS_UUID_CHAR_UGT_FEATURES:
        {
            if (!(p_gmas->gmap_role & GMAP_ROLE_CHAR_BIT_VALUE_UGT))
            {
                cause = APP_RESULT_ATTR_NOT_FOUND;
            }
        }
        break;
    case GMAS_UUID_CHAR_BGS_FEATURES:
        {
            if (!(p_gmas->gmap_role & GMAP_ROLE_CHAR_BIT_VALUE_BGS))
            {
                cause = APP_RESULT_ATTR_NOT_FOUND;
            }
        }
        break;
    case GMAS_UUID_CHAR_BGR_FEATURES:
        {
            if (!(p_gmas->gmap_role & GMAP_ROLE_CHAR_BIT_VALUE_BGR))
            {
                cause = APP_RESULT_ATTR_NOT_FOUND;
            }
        }
        break;
    default:
        {
            PROTOCOL_PRINT_ERROR1("gmas_attr_read_cb attrib_index = 0x%x not found", attrib_index);
            cause = APP_RESULT_ATTR_NOT_FOUND;
        }
        break;
    }

    if (cause == APP_RESULT_SUCCESS)
    {
        cause = (T_APP_RESULT)ble_audio_mgr_dispatch(LE_AUDIO_MSG_GMAS_READ_IND, &read_ind);

        if (cause == BLE_AUDIO_CB_RESULT_SUCCESS && char_uuid.uu.char_uuid16 != GMAS_UUID_CHAR_GMAP_ROLE)
        {
            cause = (T_APP_RESULT)BLE_AUDIO_CB_RESULT_PENDING;
        }
    }

    return (cause);
}

bool gmas_create_table(uint8_t gmap_role)
{
    uint16_t idx = 3;
    uint16_t attr_tbl_size = 0;

    if (gmap_role & GMAP_ROLE_CHAR_BIT_VALUE_UGG)
    {
        idx += 2;
    }

    if (gmap_role & GMAP_ROLE_CHAR_BIT_VALUE_UGT)
    {
        idx += 2;
    }

    if (gmap_role & GMAP_ROLE_CHAR_BIT_VALUE_BGS)
    {
        idx += 2;
    }

    if (gmap_role & GMAP_ROLE_CHAR_BIT_VALUE_BGR)
    {
        idx += 2;
    }
#if LE_AUDIO_INIT_DEBUG
    PROTOCOL_PRINT_TRACE1("[GMAS]gmas_create_table: total idx: %d", idx);
#endif
    p_gmas->attr_num = idx;
    attr_tbl_size = idx * sizeof(T_ATTRIB_APPL);
    p_gmas->p_attr_tbl = ble_audio_mem_zalloc(attr_tbl_size);
    if (p_gmas->p_attr_tbl == NULL)
    {
        PROTOCOL_PRINT_ERROR0("[GMAS]gmas_create_table: allocate p_attr_tbl failed");
        return false;
    }

    idx = 0;

    memcpy(p_gmas->p_attr_tbl, gmas_attr_tbl, sizeof(gmas_attr_tbl));
    idx += 3;

    if (gmap_role & GMAP_ROLE_CHAR_BIT_VALUE_UGG)
    {
        memcpy(&p_gmas->p_attr_tbl[idx], ugg_features_char_tbl, sizeof(ugg_features_char_tbl));
        idx += 2;
    }

    if (gmap_role & GMAP_ROLE_CHAR_BIT_VALUE_UGT)
    {
        memcpy(&p_gmas->p_attr_tbl[idx], ugt_features_char_tbl, sizeof(ugt_features_char_tbl));
        idx += 2;
    }

    if (gmap_role & GMAP_ROLE_CHAR_BIT_VALUE_BGS)
    {
        memcpy(&p_gmas->p_attr_tbl[idx], bgs_features_char_tbl, sizeof(bgs_features_char_tbl));
        idx += 2;
    }

    if (gmap_role & GMAP_ROLE_CHAR_BIT_VALUE_BGR)
    {
        memcpy(&p_gmas->p_attr_tbl[idx], bgr_features_char_tbl, sizeof(bgr_features_char_tbl));
    }

    return true;
}

const T_FUN_GATT_EXT_SERVICE_CBS gmas_cbs =
{
    gmas_attr_read_cb,  // Read callback function pointer
    NULL,               // Write callback function pointer
    NULL                // CCCD update callback function pointer
};

bool gmas_init(uint8_t gmap_role)
{
    uint8_t error_id = 0;
    T_SERVER_ID service_id;

    if (p_gmas)
    {
        error_id = 1;
        goto error;
    }

    p_gmas = ble_audio_mem_zalloc(sizeof(T_GMAS));

    if (p_gmas == NULL)
    {
        error_id = 2;
        goto error;
    }

    if (gmas_create_table(gmap_role))
    {
        if (false == gatt_svc_add(&service_id,
                                  (uint8_t *)p_gmas->p_attr_tbl,
                                  p_gmas->attr_num * sizeof(T_ATTRIB_APPL),
                                  &gmas_cbs, NULL))
        {
            error_id = 3;
            goto error;
        }

        p_gmas->service_id = service_id;
        p_gmas->gmap_role = gmap_role;
    }
    else
    {
        error_id = 4;
        goto error;
    }

    return true;

error:
    PROTOCOL_PRINT_ERROR1("gmas_init: fail, error_id %d", error_id);

    if (p_gmas != NULL)
    {
        if (p_gmas->p_attr_tbl != NULL)
        {
            ble_audio_mem_free(p_gmas->p_attr_tbl);
        }
        ble_audio_mem_free(p_gmas);
    }

    return false;
}

#if LE_AUDIO_DEINIT
void gmas_deinit(void)
{
    if (p_gmas)
    {
        if (p_gmas->p_attr_tbl != NULL)
        {
            ble_audio_mem_free(p_gmas->p_attr_tbl);
        }
        ble_audio_mem_free(p_gmas);
        p_gmas = NULL;
    }
}
#endif
#endif
