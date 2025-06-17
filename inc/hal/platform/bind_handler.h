#ifndef __BIND_HANDLER_H__
#define __BIND_HANDLER_H__
#include "stdint.h"
#include "stdbool.h"
/** @defgroup  HAL_BIND_HANDLER    Bind Hanlder
    * @brief This file introduces the bind handler APIs.
    * @{
    */

/*============================================================================*
 *                              Variables
*============================================================================*/


/*============================================================================*
  *                                Functions
  *============================================================================*/

/** @defgroup HAL_BIND_HANDLER_Exported_Functions Bind Handler Exported Functions
    * @brief
    * @{
    */
#ifdef __cplusplus
extern "C" {
#endif
/**
    * @brief  Check bind share key ready status.
    * @warning  This API is only supported in RTL87x3D and RTL87x3E.
    *           It is NOT supported in RTL87x3G and RTL87x3EP.
    * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
    * @return true: the share key exists; false: the share key doesn't exist.
    */
bool check_bind_key_ready_status(void);
/**
    * @brief  Reset bind key.
    * @warning  This API is only supported in RTL87x3D and RTL87x3E.
    *           It is NOT supported in RTL87x3G and RTL87x3EP.
    * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
    * @return 0: success, others: fail.
    */
int reset_bind_key_info(void);
/**
    * @brief  Get bind public key.
    * @warning  This API is only supported in RTL87x3D and RTL87x3E.
    *           It is NOT supported in RTL87x3G and RTL87x3EP.
    * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
    * @param  p_public_key_buf Pointer to the bind public key.
    * @param  public_key_buf_size Specify the size of the bind public key buffer.
    * @return 0: success, others: fail.
    */
int get_bind_public_key(uint8_t *p_public_key_buf, uint32_t public_key_buf_size);

/**
    * @brief  Set peer bind public key.
    * @warning  This API is only supported in RTL87x3D and RTL87x3E.
    *           It is NOT supported in RTL87x3G and RTL87x3EP.
    * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
    * @param  p_public_key_buf Pointer to the peer bind public key.
    * @param  public_key_buf_size Specify the size of the peer bind public key buffer.
    * @return 0: success, others: fail.
    */
int set_peer_bind_public_key(uint8_t *p_public_key_buf, uint32_t public_key_buf_size);
/**
    * @brief  Generate bind share key.
    * @warning  This API is only supported in RTL87x3D and RTL87x3E.
    *           It is NOT supported in RTL87x3G and RTL87x3EP.
    * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
    * @return 0: success, others: fail.
    */
int generate_bind_share_key(void);
/**
    * @brief  Get local bind dynamic authentication data.
    * @warning  This API is only supported in RTL87x3D and RTL87x3E.
    *           It is NOT supported in RTL87x3G and RTL87x3EP.
    * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
    * @param  p_auth_buf Pointer to the dynamic authentication data buffer.
    * @param  auth_buf_size Specify the size of the dynamic authentication data buffer. It should be 24 bytes.
    * @return 0: success, others: fail.
    */
int get_bind_dynamic_auth_data(uint8_t *p_auth_buf, uint32_t auth_buf_size);
/**
    * @brief  Set peer bind dynamic authentication data.
    * @warning  This API is only supported in RTL87x3D and RTL87x3E.
    *           It is NOT supported in RTL87x3G and RTL87x3EP.
    * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
    * @param  p_auth_buf Pointer to the dynamic authentication data buffer.
    * @param  auth_buf_size Specify the size of the dynamic authentication data buffer. It should be 24 bytes.
    * @return 0: success, others: fail.
    */
int set_peer_dynamic_auth_data(uint8_t *p_auth_buf, uint32_t auth_buf_size);
/**
    * @brief  Get local bind encrypted authentication data.
    * @warning  This API is only supported in RTL87x3D and RTL87x3E.
    *           It is NOT supported in RTL87x3G and RTL87x3EP.
    * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
    * @param  p_enc_auth_buf Pointer to the local bind encrypted authentication data.
    * @param  auth_buf_size Specify the size of the local bind encrypted authentication data buffer.
    * @return 0: success, others: fail.
    */
int get_bind_encrypted_auth_data(uint8_t *p_enc_auth_buf, uint32_t auth_buf_size);
/**
    * @brief  Verify peer bind encrypted authentication data.
    * @warning  This API is only supported in RTL87x3D and RTL87x3E.
    *           It is NOT supported in RTL87x3G and RTL87x3EP.
    * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
    * @param  p_peer_enc_auth_buf Pointer to the peer bind encrypted authentication data.
    * @param  auth_buf_size Specify the size of the peer bind encrypted authentication data buffer.
    * @return 0: success, others: fail.
    */
int verify_peer_bind_enc_auth_data(uint8_t *p_peer_enc_auth_buf, uint32_t auth_buf_size);
/**
    * @brief  Get bind share key information to save.
    * @warning  This API is only supported in RTL87x3D and RTL87x3E.
    *           It is NOT supported in RTL87x3G and RTL87x3EP.
    * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
    * @param  p_save_buf Pointer to the bind share key information data buffer to save.
    * @param  save_buf_size Specify the size of the share key information data buffer.
    * @return 0: success, others: fail.
    */
int get_bind_share_key_to_save(uint8_t *p_save_buf, uint32_t save_buf_size);
/**
    * @brief  Restore bind share key information.
    * @warning  This API is only supported in RTL87x3D and RTL87x3E.
    *           It is NOT supported in RTL87x3G and RTL87x3EP.
    * \xrefitem Added_API_2_14_1_0 "Added Since 2.14.1.0" "Added API"
    * @param  p_key_buf Pointer to the bind share key information data buffer to restore.
    * @param  buf_size Specify the size of the share key information data buffer.
    * @return 0: success, others: fail.
    */
int restore_bind_share_key(uint8_t *p_key_buf, uint32_t buf_size);
#ifdef __cplusplus
}
#endif
/** @} */ /* End of group HAL_BIND_HANDLER_Exported_Functions */
/** @} */ /* End of group HAL_BIND_HANDLER */
#endif
