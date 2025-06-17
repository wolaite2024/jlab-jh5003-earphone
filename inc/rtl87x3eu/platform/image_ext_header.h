/**
  ******************************************************************************
  * @file    patch_header_check.h
  * @author
  * @version V0.0.1
  * @date    2017-09-01
  * @brief   This file contains all the functions regarding patch header check.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef _IMAGE_EXT_HEADER_H_
#define _IMAGE_EXT_HEADER_H_
#include <stdbool.h>    //for bool
#include <stdint.h>   //for uint* type
/** @addtogroup  FLASH_DEVICE    Flash Device
    * @{
    */
/*============================================================================*
  *                                   Constants
  *============================================================================*/

typedef void (*APP_PRE_INIT)(void);

typedef union
{
    struct
    {
        APP_PRE_INIT app_pre_init;
    } app_ext_info;
    struct
    {
        uint8_t comiple_stamp[16];
    } upper_ext_info;
} T_EXTRA_INFO_FORMAT;


#endif // _IMAGE_EXT_HEADER_H_
