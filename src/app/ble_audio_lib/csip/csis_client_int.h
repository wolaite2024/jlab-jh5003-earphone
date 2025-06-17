#ifndef _CSIS_CLIENT_INT_H_
#define _CSIS_CLIENT_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "csis_def.h"
#include "bt_gatt_client.h"
#include "csis_client.h"

#if LE_AUDIO_CSIS_CLIENT_SUPPORT

/**
 * \defgroup    LEA_GAF_CSIP CSIP Profile
 *
 * \brief   Coordinated Set Identification Profile.
 */

/**
 * csis_client.h
 *
 * \brief  CSIS Client Database.
 *
 * \ingroup LEA_GAF_CSIP
 */
typedef struct
{
    uint8_t             cur_srv_idx;
    uint8_t             cur_read_flags;
    uint8_t             cur_read_req_flags;
    uint8_t             cur_size;
    T_CSIS_LOCK         cur_lock;
    uint8_t             cur_rank;
    T_CSIS_SIRK_TYPE    sirk_type;
    uint8_t             cur_sirk[CSIS_SIRK_LEN];
} T_CSIS_CLIENT_DB;

/**
 * csis_client.h
 *
 * \brief  oordinated Set Identification Service Characteristic Type.
 *
 * \ingroup LEA_GAF_CSIP
 */
typedef enum
{
    CSIS_CHAR_LOCK,
    CSIS_CHAR_SIRK,
    CSIS_CHAR_SIZE,
    CSIS_CHAR_RANK,
} T_CSIS_CHAR_TYPE;

/**
 * csis_client.h
 *
 * \brief  Read coordinated set identification service characteristic value.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  srv_instance_id  Service id.
 * \param[in]  type             CSIS characteristic type: @ref T_CSIS_CHAR_TYPE.
 * \param[in]  p_req_cb         Client callback function.
 *
 * \return         The result of read coordinated set identification service characteristic value.
 * \retval true    Read coordinated set identification service characteristic value success.
 * \retval false   Read coordinated set identification service characteristic value failed.
 *
 * \ingroup LEA_GAF_CSIP
 */
bool csis_read_char_value(uint16_t conn_handle, uint8_t srv_instance_id, T_CSIS_CHAR_TYPE type,
                          P_FUN_GATT_CLIENT_CB p_req_cb);

/**
 * csis_client.h
 *
 * \brief  Send member lock request.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  srv_instance_id  Service id.
 * \param[in]  lock             CSIS Lock State: @ref T_CSIS_LOCK.
 * \param[in]  p_req_cb         Client callback function.
 *
 * \return         The result of send member lock request.
 * \retval true    Send member lock request success.
 * \retval false   Send member lock request failed.
 *
 * \ingroup LEA_GAF_CSIP
 */
bool csis_send_lock_req(uint16_t conn_handle, uint8_t srv_instance_id, T_CSIS_LOCK lock,
                        P_FUN_GATT_CLIENT_CB p_req_cb);

/**
 * csis_client.h
 *
 * \brief  Initialize coordinated set identification service client.
 *
 * \param[in]  app_cb      Client callback function.
 *
 * \return         The result of initialize coordinated set identification service client.
 * \retval true    Initialize coordinated set identification service client success.
 * \retval false   Initialize coordinated set identification service client failed.
 *
 * \ingroup LEA_GAF_CSIP
 */
bool csis_client_init(P_FUN_GATT_CLIENT_CB app_cb);

#endif
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
