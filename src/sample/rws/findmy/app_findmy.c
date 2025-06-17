/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_findmy.c
   * @brief     This file handles findmy init.
   * @author    cen
   * @date      2022-11-07
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#if F_APP_FINDMY_FEATURE_SUPPORT
#include "fmna_connection_platform.h"
#include "fmna_state_machine.h"
#include "fmna_import_includes.h"
#include "fmna_motion_detection.h"
#include "fmna_crypto.h"
#include "fmna_connection.h"
#include "fmna_sound_platform.h"

/*============================================================================*
 *                              Functions
 *============================================================================*/

void app_findmy_init(void)
{
    fmna_version_init();
    fmna_crypto_init();
    fmna_gatt_init();
    fmna_motion_detection_init();
    fmna_sound_platform_init();
}

#endif
