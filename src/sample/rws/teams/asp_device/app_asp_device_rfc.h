/**
 * @file   app_asp_device_rfc.h
 * @brief  struct and interface about asp api for app
 * @author leon
 * @date   2020.9.3
 * @version 1.0
 * @par Copyright (c):
         Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _APP_ASP_DEVICE_RFC_H_
#define _APP_ASP_DEVICE_RFC_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if F_APP_TEAMS_FEATURE_SUPPORT
/** @defgroup RWS_APP RWS APP
  * @brief Provides rws app interfaces.
  * @{
  */

/** @defgroup APP_ASP_DEVICE_RFC APP ASP DEVICE RFC module init
  * @brief Provides App Asp Device Rfc profile interfaces.
  * @{
  */


/**
 *  @brief function app asp rfc module init
 *  @note  task init call this function, the rfcomm channel would init completly
 */
void app_asp_device_rfc_init(void);

/** End of APP_ASP_DEVICE_RFC
* @}
*/

/** End of RWS_APP
* @}
*/
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_ASP_DEVICE_H_ */
