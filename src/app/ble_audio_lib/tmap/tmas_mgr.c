#include <stdint.h>
#include "trace.h"
#include "bt_types.h"
#include "ble_audio_dm.h"
#include "ble_audio_mgr.h"
#include "tmas_mgr.h"

#if LE_AUDIO_TMAS_SUPPORT
typedef struct
{
    T_SERVER_ID service_id;
    uint16_t    role;
} T_TMAS;

static T_TMAS *p_tmas;

/**< @brief  profile/service definition.  */
static const T_ATTRIB_APPL tmas_attr_tbl[] =
{
    /*----------------- Telephony and Media Audio Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_TMAS),              /* service UUID */
            HI_WORD(GATT_UUID_TMAS)
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
            GATT_CHAR_PROP_READ
            /* characteristic UUID not needed here, is UUID of next attrib. */
        },
        1,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ                              /* permissions */
    },
    /* TMAP role value */
    {
        ATTRIB_FLAG_VALUE_APPL,                     /* flags */
        {                                           /* type_value */
            LO_WORD(TMAS_UUID_CHAR_ROLE),
            HI_WORD(TMAS_UUID_CHAR_ROLE)
        },
        0,                                          /* bValueLen */
        NULL,
        GATT_PERM_READ_AUTHEN_REQ /* permissions */
    }
};

static T_APP_RESULT tmas_attr_read_cb(uint16_t conn_handle, uint16_t cid, T_SERVER_ID service_id,
                                      uint16_t attrib_index,
                                      uint16_t offset, uint16_t *p_length, uint8_t **pp_value)
{
    T_APP_RESULT cause = APP_RESULT_SUCCESS;

    PROTOCOL_PRINT_INFO2("tmas_attr_read_cb: attrib_index %d, offset %x", attrib_index, offset);

    T_CHAR_UUID char_uuid = gatt_svc_find_char_uuid_by_index(tmas_attr_tbl, attrib_index,
                                                             sizeof(tmas_attr_tbl) / sizeof(T_ATTRIB_APPL));
    if (char_uuid.uuid_size != UUID_16BIT_SIZE)
    {
        PROTOCOL_PRINT_ERROR1("tmas_attr_read_cb Error: attrib_index 0x%x", attrib_index);
        return APP_RESULT_ATTR_NOT_FOUND;
    }
    *p_length = 0;
    switch (char_uuid.uu.char_uuid16)
    {
    case TMAS_UUID_CHAR_ROLE:
        {
            *pp_value = (uint8_t *)&p_tmas->role;
            *p_length = 2;
        }
        break;
    default:
        cause = APP_RESULT_ATTR_NOT_FOUND;
        break;
    }

    return (cause);
}

const T_FUN_GATT_EXT_SERVICE_CBS tmas_cbs =
{
    tmas_attr_read_cb,  // Read callback function pointer
    NULL, // Write callback function pointer
    NULL  // CCCD update callback function pointer
};

bool tmas_init(uint16_t role)
{
    T_SERVER_ID service_id;

    if (p_tmas)
    {
        goto error;
    }

    p_tmas = ble_audio_mem_zalloc(sizeof(T_TMAS));

    if (p_tmas == NULL)
    {
        goto error;
    }

    if (false == gatt_svc_add(&service_id,
                              (uint8_t *)tmas_attr_tbl,
                              sizeof(tmas_attr_tbl),
                              &tmas_cbs, NULL))
    {
        goto error;
    }
    p_tmas->service_id = service_id;
    p_tmas->role = role;
    return true;
error:
    if (p_tmas)
    {
        ble_audio_mem_free(p_tmas);
        p_tmas = NULL;
    }
    PROTOCOL_PRINT_ERROR0("tmas_init: fail!");
    return false;
}

#if LE_AUDIO_DEINIT
void tmas_deinit(void)
{
    if (p_tmas)
    {
        ble_audio_mem_free(p_tmas);
        p_tmas = NULL;
    }
}
#endif
#endif
