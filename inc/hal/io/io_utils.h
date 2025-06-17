/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      io_utils.h
* @brief
* @details
* @author
* @date
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef __IO_UTILS_H
#define __IO_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Types
*============================================================================*/
/** @addtogroup IO_UTILS IO Utils
  * @brief IO Utils driver module.
  * @{
  */

/** @defgroup IO_UTILS_Exported_Types IO Utils Exported Types
  * @{
  */
typedef enum
{
    RESET = 0,
    SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
    DISABLE = 0,
    ENABLE = !DISABLE
} FunctionalState;

#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))
//typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;

/** End of group IO_UTILS_Exported_Types
  * @}
  */

/*============================================================================*
 *                              Constants
*============================================================================*/
/** @defgroup IO_UTILS_Exported_Constants IO Utils Exported Constants
  * @{
  */

//Add by Vendor
#define LITTLE_ENDIAN                        0
#define BIG_ENDIAN                           1
#define SYSTEM_ENDIAN                        LITTLE_ENDIAN

#define SWAP32(x) ((uint32_t)(                         \
                                                       (((uint32_t)(x) & (uint32_t)0x000000ff) << 24) |            \
                                                       (((uint32_t)(x) & (uint32_t)0x0000ff00) <<  8) |            \
                                                       (((uint32_t)(x) & (uint32_t)0x00ff0000) >>  8) |            \
                                                       (((uint32_t)(x) & (uint32_t)0xff000000) >> 24)))

#define WAP16(x) ((uint16_t)(                         \
                                                      (((uint16_t)(x) & (uint16_t)0x00ff) <<  8) |            \
                                                      (((uint16_t)(x) & (uint16_t)0xff00) >>  8)))

#if SYSTEM_ENDIAN == LITTLE_ENDIAN
#ifndef rtk_le16_to_cpu
#define rtk_cpu_to_le32(x)      ((uint32_t)(x))
#define rtk_le32_to_cpu(x)      ((uint32_t)(x))
#define rtk_cpu_to_le16(x)      ((uint16_t)(x))
#define rtk_le16_to_cpu(x)      ((uint16_t)(x))
#define rtk_cpu_to_be32(x)      SWAP32((x))
#define rtk_be32_to_cpu(x)      SWAP32((x))
#define rtk_cpu_to_be16(x)      WAP16((x))
#define rtk_be16_to_cpu(x)      WAP16((x))
#endif

#elif SYSTEM_ENDIAN == BIG_ENDIAN
#ifndef rtk_le16_to_cpu
#define rtk_cpu_to_le32(x)      SWAP32((x))
#define rtk_le32_to_cpu(x)      SWAP32((x))
#define rtk_cpu_to_le16(x)      WAP16((x))
#define rtk_le16_to_cpu(x)      WAP16((x))
#define rtk_cpu_to_be32(x)      ((uint32_t)(x))
#define rtk_be32_to_cpu(x)      ((uint32_t)(x))
#define rtk_cpu_to_be16(x)      ((uint16_t)(x))
#define rtk_be16_to_cpu(x)      ((uint16_t)(x))
#endif
#endif

#define HAL_READ32(base, addr)            \
    rtk_le32_to_cpu(*((volatile uint32_t *)(base + addr)))

#define HAL_WRITE32(base, addr, value32)  \
    ((*((volatile uint32_t *)(base + addr))) = rtk_cpu_to_le32(value32))

#define HAL_UPDATE32(addr, mask, value32)  \
    HAL_WRITE32(0, addr, (HAL_READ32(0, addr) & ~(mask)) | ((value32) & (mask)))

#define HAL_READ16(base, addr)            \
    rtk_le16_to_cpu(*((volatile uint16_t *)(base + addr)))

#define HAL_WRITE16(base, addr, value)  \
    ((*((volatile uint16_t *)(base + addr))) = rtk_cpu_to_le16(value))

#define HAL_UPDATE16(addr, mask, value16)  \
    HAL_WRITE16(0, addr, (HAL_READ16(0, addr) & ~(mask)) | ((value16) & (mask)))

#define HAL_READ8(base, addr)            \
    (*((volatile uint8_t *)(base + addr)))

#define HAL_WRITE8(base, addr, value)  \
    ((*((volatile uint8_t *)(base + addr))) = value)

#define HAL_UPDATE8(addr, mask, value8)  \
    HAL_WRITE8(0, addr, (HAL_READ8(0, addr) & ~(mask)) | ((value8) & (mask)))

#define BIT0        0x00000001
#define BIT1        0x00000002
#define BIT2        0x00000004
#define BIT3        0x00000008
#define BIT4        0x00000010
#define BIT5        0x00000020
#define BIT6        0x00000040
#define BIT7        0x00000080
#define BIT8        0x00000100
#define BIT9        0x00000200
#define BIT10       0x00000400
#define BIT11       0x00000800
#define BIT12       0x00001000
#define BIT13       0x00002000
#define BIT14       0x00004000
#define BIT15       0x00008000
#define BIT16       0x00010000
#define BIT17       0x00020000
#define BIT18       0x00040000
#define BIT19       0x00080000
#define BIT20       0x00100000
#define BIT21       0x00200000
#define BIT22       0x00400000
#define BIT23       0x00800000
#define BIT24       0x01000000
#define BIT25       0x02000000
#define BIT26       0x04000000
#define BIT27       0x08000000
#define BIT28       0x10000000
#define BIT29       0x20000000
#define BIT30       0x40000000
#define BIT31       0x80000000

#define BIT(_n)     (uint32_t)(1U << (_n))
#define BIT64(_n)   (1ULL << (_n))

/** End of group IO_UTILS_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup IO_UTILS_Exported_Functions IO Utils Exported Functions
  * @{
  */
/* Uncomment the line below to expanse the 'assert_param' macro in the
   Standard Peripheral Library drivers code */
//#define USE_FULL_ASSERT

#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports
  *         the name of the source file and the source line number of the call
  *         that failed. If expr is true, it returns no value.
  * @return None.
  */
#define assert_param(expr) ((expr) ? (void)0 : io_assert_failed((uint8_t *)__FILE__, __LINE__))
void io_assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

/** @} */ /* End of group IO_UTILS_Exported_Functions */

#ifdef __cplusplus
}
#endif

#endif /*__IO_UTILS_H*/


/** @} */ /* End of group IO_UTILS */
