/**
*****************************************************************************************
*     Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    debug_monitor.h
  * @brief   Debug Monitor
  * @author  Henry_Huang
  * @date    2020-09-18
  * @version v1.0
  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2020 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __DEBUG_MONITOR_H
#define __DEBUG_MONITOR_H

/*============================================================================*
 *                               Header Files
*============================================================================*/

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WATCH_POINT0_FUNCTION (FUNCTION_CYCLE_COUNTER       | FUNCTION_INSTR_ADDR          | FUNCTION_DADDR_RW            | \
                               FUNCTION_DADDR_W             | FUNCTION_DADDR_R             | FUNCTION_DADDR_WITH_VALUE_RW | \
                               FUNCTION_DADDR_WITH_VALUE_W  | FUNCTION_DADDR_WITH_VALUE_R  | FUNCTION_DISABLED)

#define WATCH_POINT1_FUNCTION (FUNCTION_INSTR_ADDR          | FUNCTION_INSTR_ADDR_LIM      |  FUNCTION_DADDR_RW           | \
                               FUNCTION_DADDR_W             | FUNCTION_DADDR_R             |  FUNCTION_DADDR_LIM          | \
                               FUNCTION_DVAL_RW             | FUNCTION_DVAL_W              |  FUNCTION_DVAL_R             | \
                               FUNCTION_DVAL_LINK           | FUNCTION_DADDR_WITH_VALUE_RW | FUNCTION_DADDR_WITH_VALUE_W  | \
                               FUNCTION_DADDR_WITH_VALUE_R  | FUNCTION_DISABLED)

#define WATCH_POINT2_FUNCTION (FUNCTION_INSTR_ADDR          | FUNCTION_DADDR_RW            |  FUNCTION_DADDR_W            | \
                               FUNCTION_DADDR_R             | FUNCTION_DADDR_WITH_VALUE_RW | FUNCTION_DADDR_WITH_VALUE_W  | \
                               FUNCTION_DADDR_WITH_VALUE_R  | FUNCTION_DISABLED)

#define WATCH_POINT3_FUNCTION (FUNCTION_INSTR_ADDR          | FUNCTION_INSTR_ADDR_LIM      |  FUNCTION_DADDR_RW           | \
                               FUNCTION_DADDR_W             | FUNCTION_DADDR_R             |  FUNCTION_DADDR_LIM          | \
                               FUNCTION_DVAL_RW             | FUNCTION_DVAL_W              |  FUNCTION_DVAL_R             | \
                               FUNCTION_DVAL_LINK           | FUNCTION_DADDR_WITH_VALUE_RW | FUNCTION_DADDR_WITH_VALUE_W  | \
                               FUNCTION_DADDR_WITH_VALUE_R  | FUNCTION_DISABLED)

#define WATCH_POINT4_FUNCTION (FUNCTION_INSTR_ADDR          | FUNCTION_DADDR_RW            |  FUNCTION_DADDR_W            | \
                               FUNCTION_DADDR_R             | FUNCTION_DADDR_WITH_VALUE_RW | FUNCTION_DADDR_WITH_VALUE_W  | \
                               FUNCTION_DADDR_WITH_VALUE_R  | FUNCTION_DISABLED)

#define WATCH_POINT5_FUNCTION (FUNCTION_INSTR_ADDR          | FUNCTION_INSTR_ADDR_LIM      | FUNCTION_DADDR_RW            | \
                               FUNCTION_DADDR_W             | FUNCTION_DADDR_R             | FUNCTION_DADDR_LIM           | \
                               FUNCTION_DADDR_WITH_VALUE_RW | FUNCTION_DADDR_WITH_VALUE_W  | FUNCTION_DADDR_WITH_VALUE_R  | \
                               FUNCTION_DISABLED)

#define WATCH_POINT6_FUNCTION (FUNCTION_INSTR_ADDR          | FUNCTION_DADDR_RW            |  FUNCTION_DADDR_W            | \
                               FUNCTION_DADDR_R             | FUNCTION_DADDR_WITH_VALUE_RW | FUNCTION_DADDR_WITH_VALUE_W  | \
                               FUNCTION_DADDR_WITH_VALUE_R  | FUNCTION_DISABLED)

#define WATCH_POINT7_FUNCTION (FUNCTION_INSTR_ADDR          | FUNCTION_INSTR_ADDR_LIM      | FUNCTION_DADDR_RW            | \
                               FUNCTION_DADDR_W             | FUNCTION_DADDR_R             | FUNCTION_DADDR_LIM           | \
                               FUNCTION_DADDR_WITH_VALUE_RW | FUNCTION_DADDR_WITH_VALUE_W  | FUNCTION_DADDR_WITH_VALUE_R  | \
                               FUNCTION_DISABLED)

#define GET_WATCH_POINT_OFFSET(watch_point) (((watch_point) - WATCH_POINT0)*16)
#define GET_REG_FUNCTION(function) (31 - __builtin_clz((function)))

typedef enum
{
    WATCH_POINT0 = 0,
    WATCH_POINT1 = 1,
    WATCH_POINT2 = 2,
    WATCH_POINT3 = 3,
    WATCH_POINT4 = 4,
    WATCH_POINT5 = 5,
    WATCH_POINT6 = 6,
    WATCH_POINT7 = 7,
    WATCH_POINT_MAX
} WATCH_POINT_TYPE;

/* DWT Config*/
typedef enum
{
    BYTE        = 0,
    HALFWORD    = 1,
    WORD        = 2,
    WATCH_SIZE_MAX
} DWT_WATCH_SIZE;

typedef enum
{
    TRIGGER_ONLY = 0,
    DEBUG_EVENT  = 1,
    ACTION_MAX
} DWT_ACTION_TYPE;


typedef enum
{
    FUNCTION_DISABLED             = 0x1,
    FUNCTION_CYCLE_COUNTER        = 0x2,
    FUNCTION_INSTR_ADDR           = 0x4,
    FUNCTION_INSTR_ADDR_LIM       = 0x8,
    FUNCTION_DADDR_RW             = 0x10,
    FUNCTION_DADDR_W              = 0x20,
    FUNCTION_DADDR_R              = 0x40,
    FUNCTION_DADDR_LIM            = 0x80,
    FUNCTION_DVAL_RW              = 0x100,
    FUNCTION_DVAL_W               = 0x200,
    FUNCTION_DVAL_R               = 0x400,
    FUNCTION_DVAL_LINK            = 0x800,
    FUNCTION_DADDR_WITH_VALUE_RW  = 0x1000,
    FUNCTION_DADDR_WITH_VALUE_W   = 0x2000,
    FUNCTION_DADDR_WITH_VALUE_R   = 0x4000,
} DWT_FUNCTION_TYPE;

typedef struct
{
    WATCH_POINT_TYPE watch_point;
    uint32_t watch_address;
    DWT_WATCH_SIZE watch_size;
    DWT_ACTION_TYPE action_type;
    DWT_FUNCTION_TYPE access_func;
} WATCH_POINT_CONFIG_TYPE;

void enable_debug_monitor(void);

void watch_point_setting(WATCH_POINT_CONFIG_TYPE watch_point_cfg);

#endif  /* __DEBUG_MONITOR_H */
