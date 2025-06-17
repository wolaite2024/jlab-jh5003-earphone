/**
 * @file   app_teams_util.h
 * @brief  struct and interface about teams rws for app
 * @author leon
 * @date   2021.10.15
 * @version 1.0
 * @par Copyright (c):
         Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _APP_TEAMS_UTIL_H_
#define _APP_TEAMS_UTIL_H_


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if F_APP_TEAMS_FEATURE_SUPPORT
/** @defgroup RWS_APP RWS APP
  * @brief Provides rws app interfaces.
  * @{
  */

/** @defgroup APP_TEAMS_UTIL APP TEAMS UTIL module init
  * @brief Provides teams rws profile interfaces.
  * @{
  */

/**
 *  @brief function app teams util module init
 *  @note  task init call this function, the rfcomm channel would init completly
 */
void app_teams_util_init(void);

/** End of APP_TEAMS_UTIL
* @}
*/

/** End of RWS_APP
* @}
*/
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_TEAMS_UTIL_H_ */
