/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    aon_wdg_ext.h
  * @brief   This file provides API wrapper for bbpro compatibility..
  * @author  huan_yang
  * @date    2023-4-23
  * @version v1.0
  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __AON_WDG_EXT_H_
#define __AON_WDG_EXT_H_


/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
  * @brief AON Watch Dog (RTC)
  */

/* 0x190 (AON_WDT_CRT)
    01:00    rw  00/01/11: turn on AON Watchdog 10: turn off AON Watchdog                                   2'b10
    02       rw  1: WDG countine count in DLPS 0:WDG stop count in DLPS                                     1'b0
    03       rw  1: reset whole chip            0: reset whole chip - (AON register and RTC)                1'b0
    04       rw  when  reg_aon_wdt_cnt_ctl == 0                                                             1'b0
                 1:relaod counter when exit DLPS 0:not reload counter when exit DLPS
    07:04    rw
    25:08    rw  Set the period of AON watchdog (unit:1/450Hz ~ 1/2.3KHz)                                   8'hff
    31:26    rw
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_aon_wdt_en: 2;            // 01:00
        uint32_t reg_aon_wdt_cnt_ctl: 1;       // 02
        uint32_t reg_aon_wdt_rst_lv_sel: 1;    // 03
        uint32_t reg_aon_wdt_cnt_reload: 1;    // 04
        uint32_t rsvd0: 3;                     // 07:05
        uint32_t reg_aon_wdt_comp: 18;         // 25:08
        uint32_t rsvd1: 6;                     // 31:26
    };
} RTC_AON_WDT_CRT_TYPE;

/* 0x198 & 0x1a4(AON_WDT_CRT)
    00    rw  write_protect 1:enable 0:disable               1'b0
    01    rw  wdt_disable                                    1'b0                                 8'hff
    31:02    rw
*/
typedef struct
{
    uint32_t write_protect : 1;
    uint32_t wdt_disable : 1;
    uint32_t reserved: 30;
} BTAON_FAST_RTC_AON_WDT_CONTROL;
typedef struct
{
    RTC_AON_WDT_CRT_TYPE aon_wdg_crt;
    uint32_t aon_wdg_clr;
    BTAON_FAST_RTC_AON_WDT_CONTROL aon_wdg_wp;
} BTAON_FAST_RTC_AON_WDT;

#define AON_WDG_REG_BASE            0x40000190UL
#define RTC_REG_BASE_ADDR           0x40000100UL
#define AON_WDG1_REG   ((BTAON_FAST_RTC_AON_WDT *)AON_WDG_REG_BASE)
#define AON_WDG2_REG   ((BTAON_FAST_RTC_AON_WDT *)(AON_WDG_REG_BASE + 0x0c))

#define AON_WDG1_REG_OFFSET   ((uint32_t)AON_WDG1_REG - RTC_REG_BASE_ADDR)
#define AON_WDG2_REG_OFFSET   ((uint32_t)AON_WDG2_REG - RTC_REG_BASE_ADDR)
/*AON_WDG1 is the same as charger aon wdg, and can not be used for the project with using charger*/
#define AON_WDG1 (BTAON_FAST_RTC_AON_WDT *)AON_WDG1_REG_OFFSET
#define AON_WDG2 (BTAON_FAST_RTC_AON_WDT *)AON_WDG2_REG_OFFSET

/**
    * @brief  enable aon wdg
    * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
    * @param  wdg: @ref BTAON_FAST_RTC_AON_WDT
    * @return void
    */
void aon_wdg_enable(BTAON_FAST_RTC_AON_WDT *wdg);

/**
    * @brief  disable aon wdg
    * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
    * @param  wdg: @ref BTAON_FAST_RTC_AON_WDT
    * @return none
    */
void aon_wdg_disable(BTAON_FAST_RTC_AON_WDT *wdg);

/**
    * @brief  restart aon wdg
    * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
    * @param  wdg: @ref BTAON_FAST_RTC_AON_WDT
    * @return none
    */
void aon_wdg_kick(BTAON_FAST_RTC_AON_WDT *wdg);

/**
    * @brief  aon wdg is enabled or not
    * \xrefitem Added_API_2_14_0_0 "Added Since 2.14.0.0" "Added API"
    * @param  wdg: @ref BTAON_FAST_RTC_AON_WDT
    * @return true: enabled; false: diabled
    */
bool aon_wdg_is_enable(BTAON_FAST_RTC_AON_WDT *wdg);

#ifdef __cplusplus
}
#endif

#endif
