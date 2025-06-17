/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    mem_config.h
  * @brief   Memory Configuration
  * @date    2017.6.6
  * @version v1.0
  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __MEM_CONFIG__
#define __MEM_CONFIG__

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup MEM_CONFIG Memory Configure
  * @brief Memory configuration for user application
  * @{
  */

/*============================================================================*
 *                        Constants
 *============================================================================*/
/** @defgroup MEM_CONFIG_Exported_Constant Memory Configure Exported Constants
  * @{
  */

/** @brief Maximun global variable size used by user application
    @note  This value could ONLY be changed after changing via config tool
*/
/* --------------------The following macros should not be modified!------------------------- */

#define APP_TRACE_ADR           0x08800000
#define APP_TRACE_SIZE          (4*1024*1024)

#define VECTOR_TABLE_ADDR       (0x00200000) // DATA_RAM_START_ADDR
#define VECTOR_TABLE_SIZE       0x2d0 // extend to 720 bytes because of GPIO/TIM compatible
// original is 0x1c8

#define COMMON1_COMMON0_OFFSET  (0x100000)
/* --------------------The following macros should not be modified! end------------------------- */

/** @brief  common ram*/

#define COMMON_SRAM_CFG_SIZE    (256*1024)

//common0 ram
/* common0 RAM layout:         96K

   1) patch executabel area:              48K
   2) app executabel area:                48K(user define)
*/

//note: common0 size should be 32K align
#define PATCH_RAM_TEXT_SIZE             (80*1024)
#define APP_RAM_TEXT_SIZE               (16*1024)

#define APP_RAM_TEXT_ADDR               (0x00300000 + PATCH_RAM_TEXT_SIZE)
#define ROM_CODE_PAGE_START_ADDRESS     APP_RAM_TEXT_ADDR

//common1 ram -->user define
/* common1 RAM layout:         160K

   1) app global:              7K (user define)
   2) Heap ON:               153K (user define)
   3) Heap OFF:                0K (user define)
*/

#define APP_GLOBAL_SIZE                (16*1024)
#define HEAP_COMMON_OFF_SIZE           (0)
#define HEAP_COMMON_ON_SIZE            (COMMON_SRAM_CFG_SIZE - PATCH_RAM_TEXT_SIZE - APP_RAM_TEXT_SIZE - APP_GLOBAL_SIZE - HEAP_COMMON_OFF_SIZE)



#define APP_GLOBAL_ADDR                (APP_RAM_TEXT_ADDR + APP_RAM_TEXT_SIZE + COMMON1_COMMON0_OFFSET)
#define HEAP_COMMON_ON_ADDR            (APP_GLOBAL_ADDR + APP_GLOBAL_SIZE)
#define HEAP_COMMON_OFF_ADDR           (HEAP_COMMON_ON_ADDR + HEAP_COMMON_ON_SIZE)

#if((PATCH_RAM_TEXT_SIZE + APP_RAM_TEXT_SIZE + APP_GLOBAL_SIZE + HEAP_COMMON_OFF_SIZE) > COMMON_SRAM_CFG_SIZE)
#error "common ram total size error"
#endif
#if((PATCH_RAM_TEXT_SIZE + APP_RAM_TEXT_SIZE)%(32*1024) != 0)
#error "common0 ram size align error"
#endif

#define COMMON_SRAM0_CFG_SIZE      (PATCH_RAM_TEXT_SIZE + APP_RAM_TEXT_SIZE)
#define COMMON_SRAM1_CFG_SIZE      (APP_GLOBAL_SIZE + HEAP_COMMON_ON_SIZE + HEAP_COMMON_OFF_SIZE)
#define CONV_UNCACHEABLE_DATA(data)  (((data)>0x02000000)? +0 : +0x02000000)

#define SHARE_MEMORY_TOTAL_SIZE (512 * 1024)

/* share memory size should be 32K align*/
#define SHARE_MEMORY_HEAP_SIZE   (0 * 1024)
#define SHARE_MEMORY_HEAP_ADDR   (0x00500000)

#define SHARE_MEMORY_GLOBAL_SIZE (0 * 1024)
#define SHARE_MEMORY_GLOBAL_ADDR (SHARE_MEMORY_HEAP_ADDR + SHARE_MEMORY_HEAP_SIZE)

#define SHARE_MEMORY_TEXT_SIZE   (0 * 1024)
#define SHARE_MEMORY_TEXT_ADDR   (SHARE_MEMORY_GLOBAL_ADDR + SHARE_MEMORY_GLOBAL_SIZE)

#if((SHARE_MEMORY_HEAP_SIZE + SHARE_MEMORY_GLOBAL_SIZE + SHARE_MEMORY_TEXT_SIZE ) > SHARE_MEMORY_TOTAL_SIZE)
#error "dsp share memory total size error"
#endif

#if((SHARE_MEMORY_HEAP_SIZE + SHARE_MEMORY_GLOBAL_SIZE + SHARE_MEMORY_TEXT_SIZE)%(32*1024) != 0)
#error "dsp share memory align error"
#endif

/** @} */ /* End of group MEM_CONFIG_Exported_Constents */

#ifdef __cplusplus
}
#endif


/** @} */ /* End of group MEM_CONFIG */

#endif

