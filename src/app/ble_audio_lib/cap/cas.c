#include <string.h>
#include "trace.h"
#include "bt_gatt_svc.h"
#include "cap.h"
#include "cap_int.h"
#include "ble_audio_mgr.h"

#if LE_AUDIO_CAS_SUPPORT
T_ATTRIB_APPL *p_cas_srv_tbl = NULL;
/**< @brief  profile/service definition.  */
static const T_ATTRIB_APPL cas_attr_tbl[] =
{
    /*----------------- Published Audio Capabilities Service -------------------*/
    /* <<Primary Service>>, .. */
    {
        (ATTRIB_FLAG_VALUE_INCL | ATTRIB_FLAG_LE),   /* flags     */
        {                                           /* type_value */
            LO_WORD(GATT_UUID_PRIMARY_SERVICE),
            HI_WORD(GATT_UUID_PRIMARY_SERVICE),
            LO_WORD(GATT_UUID_CAS),              /* service UUID */
            HI_WORD(GATT_UUID_CAS)
        },
        UUID_16BIT_SIZE,                            /* bValueLen     */
        NULL,                                       /* p_value_context */
        GATT_PERM_READ                              /* permissions  */
    },
    {
        (ATTRIB_FLAG_VOID | ATTRIB_FLAG_INCLUDE_MULTI),   /**< flags */
        {                                           /**< type_value */
            LO_WORD(GATT_UUID_INCLUDE),
            HI_WORD(GATT_UUID_INCLUDE),
        },
        0,
        NULL,
        GATT_PERM_READ                              /**< permissions */
    }
};

const T_FUN_GATT_EXT_SERVICE_CBS cas_cbs =
{
    NULL,  // Read callback function pointer
    NULL, // Write callback function pointer
    NULL  // CCCD update callback function pointer
};

T_SERVER_ID cas_add_service(T_ATTRIB_APPL *p_csis_tbl)
{
    T_SERVER_ID service_id;
    uint16_t length = sizeof(T_ATTRIB_APPL);
    if (p_cas_srv_tbl)
    {
        goto error;
    }

    p_cas_srv_tbl = ble_audio_mem_zalloc(sizeof(cas_attr_tbl));
    if (p_cas_srv_tbl == NULL)
    {
        goto error;
    }
    memcpy(p_cas_srv_tbl, cas_attr_tbl, sizeof(cas_attr_tbl));
    if (p_csis_tbl)
    {
        p_cas_srv_tbl[1].p_value_context = p_csis_tbl;
        length += sizeof(T_ATTRIB_APPL);
    }

    if (false == gatt_svc_add(&service_id,
                              (uint8_t *)p_cas_srv_tbl,
                              length,
                              &cas_cbs, NULL))
    {
        goto error;
    }
    return service_id;
error:
    PROTOCOL_PRINT_ERROR0("cas_add_service: failed");
    return 0xff;
}

bool cas_init(uint8_t *sirk, T_CSIS_SIRK_TYPE sirk_type, uint8_t size, uint8_t rank,
              uint8_t feature)
{
    T_ATTRIB_APPL *p_csis_tbl = NULL;
    bool csis_reg = false;
#if LE_AUDIO_CSIS_SUPPORT
    T_SERVER_ID csis_id = 0xFF;

    if (sirk != NULL)
    {
        T_ATTR_UUID srv_uuid = {0};
        srv_uuid.is_uuid16 = true;
        srv_uuid.p.uuid16 = GATT_UUID_CAS;
        csis_id = csis_reg_srv(sirk, sirk_type, size, rank, feature, &srv_uuid);
        if (csis_id != 0xFF)
        {
            if (csis_get_srv_tbl(csis_id, &p_csis_tbl))
            {
#if LE_AUDIO_INIT_DEBUG
                PROTOCOL_PRINT_INFO0("cas_init: get csis table success");
#endif
                csis_reg = true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
#endif
    if (cas_add_service(csis_reg ? p_csis_tbl : NULL) == 0xff)
    {
        return false;
    }
    return true;
}
#if LE_AUDIO_DEINIT
void cas_deinit(void)
{
    if (p_cas_srv_tbl)
    {
        ble_audio_mem_free(p_cas_srv_tbl);
        p_cas_srv_tbl = NULL;
    }
}
#endif
#endif
