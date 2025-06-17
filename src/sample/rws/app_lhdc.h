/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#ifndef _APP_LHDC_H_
#define _APP_LHDC_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief  lhdc link release.
 *
 * @param  p_link  @ref T_APP_BR_LINK
 * @return void
*/
void app_lhdc_release(T_APP_BR_LINK *p_link);

/**
 * @brief  lhdc link create.
 *
 * @param  p_link  @ref T_APP_BR_LINK
 * @return void
*/
void app_lhdc_create(T_APP_BR_LINK *p_link);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
