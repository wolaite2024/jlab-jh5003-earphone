/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_sdio.h
* @brief     The header file of SDIO driver.
* @details
* @author   elliot chen
* @date     2024-07-18
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _RTL876x_SDIO_H_
#define _RTL876x_SDIO_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "rtl876x.h"

/* Peripheral: SDIO */
/* Description: SDIO register defines */
#define SDIO_SYS_REG                    0x4005D000
/* Register: TF_MODE --------------------------------------------------*/
/* Description: Transfer mode Register. Offset: 0x0C. Address: 0x4005C00CUL. */
/* TF_MODE[5] :SDIO_MULTI_BLOCK. */
#define SDIO_MULTI_BLOCK_EN                 BIT(5)
#define SDIO_AUTO_CMD12_EN                  BIT(2)
#define SDIO_BLOCK_COUNT_EN                 BIT(1)

/* Register: PRESENT_STATE --------------------------------------------------*/
/* Description: Present State Register. Offset: 0x24. Address: 0x4005C024UL. */
/* PRESENT_STATE[1] :SDIO_DAT_INHIBIT. */
#define SDIO_DAT_INHIBIT                    BIT(1)
/* PRESENT_STATE[0] :SDIO_CMD_INHIBIT. */
#define SDIO_CMD_INHIBIT                    BIT(0)

/* Register: PWR_CTRL ------------------------------------------------------*/
/* Description: Power control register. Offset: 0x29. Address: 0x4005C029UL. */
/* PWR_CTRL[3:1] :SDIO_BUS_VOLTAGE. */
#define SDIO_BUS_VOLTAGE_POS                (1UL)
#define SDIO_BUS_VOLTAGE_MASK               (0x1UL << SDIO_BUS_VOLTAGE_POS)
#define SDIO_BUS_VOLTAGE_CLR                (~SDIO_BUS_VOLTAGE_MASK)
#define SDIO_VOLT_3_3V                      ((uint32_t)7 << SDIO_BUS_VOLTAGE_POS)
#define SDIO_VOLT_3_0V                      ((uint32_t)6 << SDIO_BUS_VOLTAGE_POS)
#define SDIO_VOLT_1_8V                      ((uint32_t)5 << SDIO_BUS_VOLTAGE_POS)
/* PWR_CTRL[0] :SDIO_BUS_PWR. 0x1: Power on. 0x0: Power down. */
#define SDIO_BUS_PWR_POS                    (0UL)
#define SDIO_BUS_PWR_MASK                   (0x1UL << SDIO_BUS_PWR_POS)
#define SDIO_BUS_PWR_CLR                    (~SDIO_BUS_PWR_MASK)

/* Register: CLK_CTRL -------------------------------------------------------*/
/* Description: clock control register. Offset: 0x2C. Address: 0x4005C02CUL. */
/* CLK_CTRL[1] :SDIO_INTERNAL_CLOCK_STABLE. 0x1: Enable. 0x0: Disable. */
#define SDIO_INTERNAL_CLK_STABLE_POS        (1UL)
#define SDIO_INTERNAL_CLK_STABLE_MASK       (0x1UL << SDIO_INTERNAL_CLK_STABLE_POS)
#define SDIO_INTERNAL_CLK_STABLE_CLR        (~SDIO_INTERNAL_CLK_STABLE_MASK)
/* CLK_CTRL[0] :SDIO_INTERNAL_CLOCK. 0x1: Enable. 0x0: Disable. */
#define SDIO_INTERNAL_CLOCK_POS             (0UL)
#define SDIO_INTERNAL_CLOCK_MASK            (0x1UL << SDIO_INTERNAL_CLOCK_POS)
#define SDIO_INTERNAL_CLOCK_CLR             (~SDIO_INTERNAL_CLOCK_MASK)

/* Register: SW_RESET -------------------------------------------------------*/
/* Description: Software reset register. Offset: 0x2F. Address: 0x4005C02FUL. */
/* SW_RESET[0] :SDIO_SF_RESET. 0x1: Reset. 0x0: Work. */
#define SDIO_SF_RESET_POS                   (0UL)
#define SDIO_SF_RESET_MASK                  (0x1UL << SDIO_SF_RESET_POS)
#define SDIO_SF_RESET_CLR                   (~SDIO_SF_RESET_MASK)

/* Register: CAPABILITIES_L ---------------------------------------------------*/
/* Description: Capabilities Register. Offset: 0x40. Address: 0x4005C040UL. */
#define SDIO_CAPA_TIMEOUT_CLK_UNIT          BIT(7)
#define SDIO_CAPA_ADMA2_SUPPORT             BIT(19)
#define SDIO_CAPA_HIGH_SPEED_SUPPORT        BIT(21)
#define SDIO_CAPA_VOLT_SUPPORT_33V          BIT(24)
#define SDIO_CAPA_VOLT_SUPPORT_30V          BIT(25)
#define SDIO_CAPA_VOLT_SUPPORT_18V          BIT(26)


/** @addtogroup 87x3d_SDIO SDIO
  * @brief SDIO driver module.
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @defgroup SDIO_Exported_Types SDIO Exported Types
  * @{
  */

/**
 * @brief SDIO initialize parameters.
 *
 *
 */
typedef struct
{
    uint32_t SDIO_ClockDiv;         /*!< Specifies the clock frequency of the SDIO controller.
                                    This parameter can be a value of @ref SDIO_ClockDiv. */

    uint32_t SDIO_TimeOut;          /*!< Specifies whether the SDIO time out.
                                    This parameter must range from 0 to 14. */

    uint32_t SDIO_BusWide;          /*!< Specifies the SDIO bus width. */

} SDIO_InitTypeDef;

/**
 * @brief SDIO command initialize parameters.
 *
 *
 */
typedef struct
{
    uint32_t SDIO_Argument;         /*!< Specifies the command argument. */

    uint32_t SDIO_CmdIndex;         /*!< Specifies the command number.
                                    This parameter can be a value of 0 to 63. */

    uint32_t SDIO_CmdType;          /*!< Specifies the command type.
                                    This parameter can be a value of @ref SDIO_CMD_TYPE. */

    uint32_t SDIO_DataPresent;      /*!< Specifies whether to present data using DAT line.
                                    This parameter can be a value of @ref DATA_PRESENT_SEL. */

    uint32_t SDIO_CmdIdxCheck;      /*!< Specifies whether the host controller shall check index field.
                                    This parameter can be a value of ENABLE or DISABLE. */

    uint32_t SDIO_CmdCrcCheck;      /*!< Specifies whether the host controller shall check crc field.
                                    This parameter can be a value of ENABLE or DISABLE. */

    uint32_t SDIO_ResponseType;     /*!< Specifies response type.
                                    This parameter can be a value of @ref SDIO_RSP_TYPE. */

} SDIO_CmdInitTypeDef;

/**
 * @brief SDIO data transmission initialize parameters.
 *
 *
 */
typedef struct
{
    uint32_t SDIO_Address;      /*!< Specifies the start address of the Descriptor table.
                                    This parameter can be a value 4 aligned address. */

    uint32_t SDIO_BlockSize;    /*!< Specifies the block size of data trnasfers for CMD17,
                                    CMD18, CMD24, CMD25 and CMD53, in the case of high capacity
                                    memory card, the size of data block is fixed to 512 bytes.
                                    This parameter can be a value of 0 to 0x800. */

    uint32_t SDIO_BlockCount;   /*!< Specifies the block count for current transfer.
                                    This parameter can be a value of 0 to 0xffff. */

    uint32_t SDIO_TransferDir;  /*!< Specifies the data transfer direction of DAT line data transfers.
                                    This parameter can be a value of @ref IS_SDIO_TRANSFER_DIR. */

    uint32_t SDIO_TransferMode; /*!< Specifies the transfer mode. */

    uint32_t SDIO_DMAEn;        /*!< Specifies whether to enable DMA functionlity.
                                    This parameter can be a value of ENABLE or DISABLE. */

} SDIO_DataInitTypeDef;

/**
 * @brief ADMA2 descriptor attribute initialize parameters.
 *
 *
 */
typedef struct
{
    uint16_t SDIO_Valid: 1;
    uint16_t SDIO_End: 1;
    uint16_t SDIO_Int: 1;
    uint16_t SDIO_Rsvd1: 1;
    uint16_t SDIO_Act1: 1;
    uint16_t SDIO_Act2: 1;
    uint16_t SDIO_Rsvd2: 10;
} ADMA2_AttrTypedef;

/**
 * @brief ADMA2 descriptor table initialize parameters.
 *
 *
 */
typedef struct
{
    ADMA2_AttrTypedef SDIO_Attribute;
    uint16_t SDIO_Length;
    uint32_t SDIO_Address;
} SDIO_ADMA2TypeDef;

/** End of group SDIO_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/

/** @defgroup SDIO_Exported_Constants SDIO Exported Constants
  * @{
  */

#define IS_SDIO_PERIPH(PERIPH) ((PERIPH) == SDIO)

/** @defgroup SDIO_Pin_Group SDIO Pin Group
  * @{
  */

#define SDIO_PinGroup_0                     ((uint32_t)0x00000000)
#define SDIO_PinGroup_1                     ((uint32_t)0x00000001)

#define IS_SDIO_PINGROUP_IDX(IDX) (((IDX) == SDIO_PinGroup_0) || ((IDX) == SDIO_PinGroup_1))

/** End of group SDIO_Pin_Group
  * @}
  */

/** @defgroup SDIO_Power_State SDIO Power State
  * @{
  */

#define SDIO_PowerState_OFF                 ((uint32_t)0x00000000)
#define SDIO_PowerState_ON                  ((uint32_t)0x00000001)

#define IS_SDIO_POWER_STATE(STATE) (((STATE) == SDIO_PowerState_OFF) || ((STATE) == SDIO_PowerState_ON))

/** End of group SDIO_Power_State
  * @}
  */

/** @defgroup SDIO_ClockDiv SDIO Clock Div
  * @{
  */

#define SDIO_CLOCK_DIV_1                    ((uint32_t)((0x0) << 8))
#define SDIO_CLOCK_DIV_2                    ((uint32_t)((0x1) << 8))
#define SDIO_CLOCK_DIV_4                    ((uint32_t)((0x2) << 8))
#define SDIO_CLOCK_DIV_8                    ((uint32_t)((0x4) << 8))
#define SDIO_CLOCK_DIV_16                   ((uint32_t)((0x8) << 8))
#define SDIO_CLOCK_DIV_32                   ((uint32_t)((0x10) << 8))
#define SDIO_CLOCK_DIV_64                   ((uint32_t)((0x20) << 8))
#define SDIO_CLOCK_DIV_128                  ((uint32_t)((0x40) << 8))
#define SDIO_CLOCK_DIV_256                  ((uint32_t)((0x80) << 8))

#define IS_SDIO_CLOCK_DIV(CTRL)         (((CTRL) == SDIO_CLOCK_DIV_1) || \
                                         ((CTRL) == SDIO_CLOCK_DIV_2) || \
                                         ((CTRL) == SDIO_CLOCK_DIV_4) || \
                                         ((CTRL) == SDIO_CLOCK_DIV_8) || \
                                         ((CTRL) == SDIO_CLOCK_DIV_16) || \
                                         ((CTRL) == SDIO_CLOCK_DIV_32) || \
                                         ((CTRL) == SDIO_CLOCK_DIV_64) || \
                                         ((CTRL) == SDIO_CLOCK_DIV_128) || \
                                         ((CTRL) == SDIO_CLOCK_DIV_256))

/** End of group SDIO_ClockDiv
  * @}
  */

/** @defgroup SDIO_BusWide SDIO Bus Wide
  * @{
  */

#define SDIO_BusWide_1b                     ((uint32_t)0x00)
#define SDIO_BusWide_4b                     ((uint32_t)0x02)

#define IS_SDIO_BUS_WIDE(WIDE) (((WIDE) == SDIO_BusWide_1b) || ((WIDE) == SDIO_BusWide_4b))

/** End of group SDIO_BusWide
  * @}
  */

/** @defgroup SDIO_TimeOut SDIO Timeout
  * @{
  */

#define IS_SDIO_TIME_OUT(TIME) ((TIME) <= 0x0E)

/** End of group SDIO_TimeOut
  * @}
  */

/** @defgroup SDIO_Block_Size SDIO Block Size
  * @{
  */

#define IS_SDIO_BLOCK_SIZE(SIZE)            ((SIZE) <= 0x800)

/** End of group SDIO_Block_Size
  * @}
  */

/** @defgroup SDIO_Block_COUNT SDIO Block Count
  * @{
  */

#define IS_SDIO_BLOCK_COUNT(COUNT)          ((COUNT) <= 0xFFFF)

/** End of group SDIO_Block_COUNT
  * @}
  */

/** @defgroup SDIO_Transfer_Direction SDIO Transfer Direction
  * @{
  */

#define SDIO_TransferDir_READ               ((uint32_t)1 << 4)
#define SDIO_TransferDir_WRITE              ((uint32_t)0x00)
#define IS_SDIO_TRANSFER_DIR(DIR) (((DIR) == SDIO_TransferDir_READ) || \
                                   ((DIR) == SDIO_TransferDir_WRITE))
/** End of group SDIO_Transfer_Direction
  * @}
  */

/** @defgroup SDIO_Command_Index SDIO Command Index
  * @{
  */

#define IS_SDIO_CMD_INDEX(INDEX)            ((INDEX) < 0x40)

/** End of group SDIO_Command_Index
  * @}
  */

/** @defgroup SDIO_CMD_TYPE SDIO CMD Type
  * @{
  */

typedef enum
{
    NORMAL,
    SUSPEND,
    RESUME,
    ABORT
} SDIO_CMD_TYPE;

/** End of group SDIO_CMD_TYPE
  * @}
  */

/** @defgroup SDIO_DATA_PRESENT_SEL SDIO Data Present Select
  * @{
  */

typedef enum
{
    NO_DATA,
    WITH_DATA
} DATA_PRESENT_SEL;

/** End of group SDIO_DATA_PRESENT_SEL
  * @}
  */

/** @defgroup SDIO_RSP_TYPE SDIO Response Type
  * @{
  */

typedef enum
{
    SDIO_Response_No,
    RSP_LEN_136,
    RSP_LEN_48,
    RSP_LEN_48_CHK_BUSY
} SDIO_RSP_TYPE;

/** End of group SDIO_RSP_TYPE
  * @}
  */

/** @defgroup SDIO_Interrupts_Definition SDIO Interrupt Definition
  * @{
  */
#define SDIO_INT_ERROR                      ((uint32_t)1 << 15)
#define SDIO_INT_CARD                       ((uint32_t)1 << 8)
#define SDIO_CARD_REMOVAL                   ((uint32_t)1 << 7)
#define SDIO_CARD_INSERT                    ((uint32_t)1 << 6)
#define SDIO_BUF_READ_RDY                   ((uint32_t)1 << 5)
#define SDIO_BUF_WRITE_RDY                  ((uint32_t)1 << 4)
#define SDIO_INT_DMA                        ((uint32_t)1 << 3)
#define SDIO_BLOCK_GAP_EVENT                ((uint32_t)1 << 2)
#define SDIO_INT_TF_CMPL                    ((uint32_t)1 << 1)
#define SDIO_INT_CMD_CMPL                   ((uint32_t)1 << 0)

#define IS_SDIO_INT(INT) (((INT)  == SDIO_INT_ERROR)        || \
                          ((INT)  == SDIO_INT_CARD)       || \
                          ((INT)  == SDIO_INT_DMA)        || \
                          ((INT)  == SDIO_INT_TF_CMPL)    || \
                          ((INT)  == SDIO_INT_CMD_CMPL))

/** End of group SDIO_Interrupts_Definition
  * @}
  */

/** @defgroup SDIO_Error_Interrupts_Definition SDIO Error Interrupt Definition
  * @{
  */
#define SDIO_INT_VENDOR_SPECIFIC_ERR        ((uint32_t)0xF << 12)
#define SDIO_INT_ADMA_ERR                   ((uint32_t)1 << 9)
#define SDIO_INT_AUTO_CMD12_ERR             ((uint32_t)1 << 8)
#define SDIO_INT_CURRENT_LIMIT_ERR          ((uint32_t)1 << 7)
#define SDIO_INT_DAT_END_BIT_ERR            ((uint32_t)1 << 6)
#define SDIO_INT_DAT_CRC_ERR                ((uint32_t)1 << 5)
#define SDIO_INT_DAT_TIMEOUT_ERR            ((uint32_t)1 << 4)
#define SDIO_INT_CMD_INDEX_ERR              ((uint32_t)1 << 3)
#define SDIO_INT_CMD_END_BIT_ERR            ((uint32_t)1 << 2)
#define SDIO_INT_CMD_CRC_ERR                ((uint32_t)1 << 1)
#define SDIO_INT_CMD_TIMEOUT_ERR            ((uint32_t)1 << 0)

#define IS_SDIO_ERR_INT(INT) (((INT)  == SDIO_INT_VENDOR_SPECIFIC_ERR)  || \
                              ((INT)  == SDIO_INT_ADMA_ERR)           || \
                              ((INT)  == SDIO_INT_AUTO_CMD12_ERR)     || \
                              ((INT)  == SDIO_INT_CURRENT_LIMIT_ERR)  || \
                              ((INT)  == SDIO_INT_DAT_END_BIT_ERR)    || \
                              ((INT)  == SDIO_INT_DAT_CRC_ERR)        || \
                              ((INT)  == SDIO_INT_DAT_TIMEOUT_ERR)    || \
                              ((INT)  == SDIO_INT_CMD_INDEX_ERR)      || \
                              ((INT)  == SDIO_INT_CMD_END_BIT_ERR)    || \
                              ((INT)  == SDIO_INT_CMD_CRC_ERR)        || \
                              ((INT)  == SDIO_INT_CMD_TIMEOUT_ERR))

/** End of group SDIO_Error_Interrupts_Definition
  * @}
  */

/** @defgroup SDIO_Wake_Up_Definition SDIO Wake Up Definition
  * @{
  */

#define SDIO_WAKE_UP_SDCARD_REMOVAL         ((uint32_t)1 << 2)
#define SDIO_WAKE_UP_SDCARD_INSERT          ((uint32_t)1 << 1)
#define SDIO_WAKE_UP_SDCARD_INTR            ((uint32_t)1 << 0)


#define IS_SDIO_WAKE_UP(EVT) (((EVT)  == SDIO_WAKE_UP_SDCARD_REMOVAL)   || \
                              ((EVT)  == SDIO_WAKE_UP_SDCARD_INSERT)  || \
                              ((EVT)  == SDIO_WAKE_UP_SDCARD_INTR))

/** End of group SDIO_Wake_Up_Definition
  * @}
  */

/** @defgroup SDIO_Flag SDIO Flag
  * @{
  */

#define SDIO_FLAG_BUF_READ_EN               ((uint32_t)1 << 11)
#define SDIO_FLAG_WRITE_BUF_EN              ((uint32_t)1 << 10)
#define SDIO_FLAG_READ_TF_ACTIVE            ((uint32_t)1 << 9)
#define SDIO_FLAG_WRITE_TF_ACTIVE           ((uint32_t)1 << 8)
#define SDIO_FLAG_DAT_LINE_ACTIVE           ((uint32_t)1 << 2)
#define SDIO_FLAG_CMD_DAT_INHIBIT           ((uint32_t)1 << 1)
#define SDIO_FLAG_CMD_INHIBIT               ((uint32_t)1 << 0)

#define IS_SDIO_FLAG(FLAG) (((FLAG)  == SDIO_FLAG_BUF_READ_EN)      || \
                            ((FLAG)  == SDIO_FLAG_WRITE_BUF_EN)     || \
                            ((FLAG)  == SDIO_FLAG_READ_TF_ACTIVE)   || \
                            ((FLAG)  == SDIO_FLAG_WRITE_TF_ACTIVE)  || \
                            ((FLAG)  == SDIO_FLAG_DAT_LINE_ACTIVE)  || \
                            ((FLAG)  == SDIO_FLAG_CMD_DAT_INHIBIT)  || \
                            ((FLAG)  == SDIO_FLAG_CMD_INHIBIT))

/** End of group SDIO_Flag
  * @}
  */

/** @defgroup SDIO_RSP_REG SDIO Response Register
  * @{
  */

#define SDIO_RSP0                          ((uint32_t)0x00000000)
#define SDIO_RSP2                          ((uint32_t)0x00000004)
#define SDIO_RSP4                          ((uint32_t)0x00000008)
#define SDIO_RSP6                          ((uint32_t)0x0000000C)

/** End of group SDIO_RSP_REG
  * @}
  */


/** @defgroup SDIO_MODE_FLAG SDIO Mode Flag
  * @{
  */

#define SDIO_FLAG_IDLE    0x00
#define SDIO_FLAG_READ    0x01
#define SDIO_FLAG_WRITE   0x02

/** End of group SDIO_MODE_FLAG
  * @}
  */


/** End of group SDIO_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/

/** @defgroup SDIO_Exported_Functions SDIO Exported Functions
  * @{
  */

/**
 * rtl876x_sdio.h
 *
 * \brief  Enable or disable the SDIO output pin.
 *
 * \param[in] NewState: New state of the SDIO output pin.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable the SDIO output pin.
 *            \arg DISABLE: Disable the SDIO output pin.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void Board_SD_Init(void)
 * {
 *     SDIO_PinOutputCmd(ENABLE);
 * }
 * \endcode
 */
void SDIO_PinOutputCmd(FunctionalState NewState);

/**
 * rtl876x_sdio.h
 *
 * \brief  Select the SDIO output pin group.
 *
 * \param[in] SDIO_PinGroupType: SDIO output pin group.
 *            This parameter can be one of the following values:
 *            \arg SDIO_PinGroup_0 : SDH_CLK(P1_2), SDH_CMD(P1_3), SDH_WT_PROT(P3_0),SDH_CD(P3_1),
 *                 SDH_D0(P1_4), SDH_D1(P1_5), SDH_D2(P1_6), SDH_D3(P1_7).
 *            \arg SDIO_PinGroup_1:  SDH_CLK(P4_2), SDH_CMD(P4_3), SDH_WT_PROT(P6_0),SDH_CD(P6_1),
 *                 SDH_D0(P4_4), SDH_D1(P4_5), SDH_D2(P4_6), SDH_D3(P4_7).
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void Board_SD_Init(void)
 * {
 *     //Configure SD pin group and enable it
 *     SDIO_PinGroupConfig(SDIO_PinGroup_0);
 *     SDIO_PinOutputCmd(ENABLE);
 * }
 * \endcode
 */
void SDIO_PinGroupConfig(uint32_t SDIO_PinGroupType);

/**
 * rtl876x_sdio.h
 *
 * \brief  Software reset host controller.
 *
 * \param  None.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SD_InitHost(void)
 * {
 *     SDIO_SoftwareReset();
 * }
 * \endcode
 */
void SDIO_SoftwareReset(void);

/**
 * rtl876x_sdio.h
 *
 * \brief  Enable or disable the SDIO internal clock.
 *
 * \param[in] NewState: New state of the SDIO internal clock.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable the SDIO internal clock.
 *            \arg DISABLE: Disable the SDIO internal clock.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SD_InitHost(void)
 * {
 *     SDIO_InternalClockCmd(ENABLE);
 * }
 * \endcode
 */
void SDIO_InternalClockCmd(FunctionalState NewState);

/**
 * rtl876x_sdio.h
 *
 * \brief  Get the status of software reset host controller.
 *
 * \param  None.
 *
 * \return The new state of SDIO software reset progress.
 * \retval SET: The software reset status is set.
 * \retval RESET: The software reset status is unset.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SD_InitHost(void)
 * {
 *     SDIO_SoftwareReset();
 *     while (SDIO_GetSoftwareResetStatus() == SET)
 *     {
 *       //add user code here.
 *     }
 * }
 * \endcode
 */
FlagStatus SDIO_GetSoftwareResetStatus(void);

/**
 * rtl876x_sdio.h
 *
 * \brief  Get the status of SDIO internal clock.
 *
 * \param  None.
 *
 * \return The new state of SDIO internal clock ready.
 * \retval SET: The SDIO internal clock ready status is set.
 * \retval RESET: The SDIO internal clock ready status is unset.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_init(void)
 * {
 *     SDIO_InternalClockCmd(ENABLE);
 *     while (SDIO_GetInternalClockStatus() == RESET)
 *     {
 *       //add user code here
 *     }
 * }
 * \endcode
 */
FlagStatus SDIO_GetInternalClockStatus(void);

/**
 * rtl876x_sdio.h
 *
 * \brief  Check whether the specified SDIO flag is set.
 *
 * \param[in] SDIO_FLAG: Specifies the SDIO flag to check.
 *            This parameter can be one of the following values:
 *            \arg SDIO_FLAG_BUF_READ_EN: Buffer read enable, this status is used for non-DMA read transfer.
 *                 SET: Have data in buffer when block data is ready in the buffer.
 *                 RESET: No Data in buffer when block data is ready in the buffer.
 *                 Supplement: A change of this bit from 0 to 1 occurs when block data is ready
 *                 in the buffer and generates the Buffer Read Interrupt.
 *            \arg SDIO_FLAG_WRITE_BUF_EN: Write buffer enable, this status is used for non-DMA write transfer.
 *                 SET: Can write in non-DMA write transfer.
 *                 RESET: Can not write in non-DMA write transfer.
 *                 Supplement: A change of this bit from 0 to 1 occurs when top of block data can be written to the buffer
 *                 and generates the Buffer Write Ready Interrupt.
 *            \arg SDIO_FLAG_READ_TF_ACTIVE: Read transfer active status.
 *                 SET: Transfer data in a read transfer.
 *                 RESET: No valid data in a read transfer.
 *            \arg SDIO_FLAG_WRITE_TF_ACTIVE: Write transfer active status.
 *                 SET: Transfer data in a write transfer.
 *                 RESET: No valid data in a write transfer.
 *            \arg SDIO_FLAG_DAT_LINE_ACTIVE: DAT line active status.
 *                 SET: DAT line active.
 *                 RESET: DAT line inactive.
 *            \arg SDIO_FLAG_CMD_DAT_INHIBIT: Command inhibit(DAT) status.
 *                 SET: Can not issue command which uses the DAT line.
 *                 RESET: Can issue command which uses the DAT line.
 *            \arg SDIO_FLAG_CMD_INHIBIT: Command inhibit(CMD) status.
 *                 SET: Can not issue command which uses the CMD line.
 *                 RESET: Can issue command which uses the CMD line.
 *
 * \return The new state of SDIO_FLAG.
 * \retval SET: The specified SDIO flag is set.
 * \retval RESET: The specified SDIO flag is unset.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SD_WaitCmdLineIdle(void)
 * {
 *     //Check command inhibit(CMD) status
 *     while (SDIO_GetFlagStatus(SDIO_FLAG_CMD_INHIBIT) == SET)
 *     {
 *         //add user code here.
 *     }
 * }
 * \endcode
 */
FlagStatus SDIO_GetFlagStatus(uint32_t SDIO_FLAG);

/**
 * rtl876x_sdio.h
 *
 * \brief  Set the SD bus power status of the controller.
 *
 * \param[in] SDIO_PowerState: New state of the bus power state.
 *            This parameter can be one of the following values:
 *            \arg SDIO_PowerState_ON: Enable setting the SD bus power.
 *            \arg SDIO_PowerState_OFF: Disable setting the SD bus power.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SD_InitHost(void)
 * {
 *     //Enable SD bus power.
 *     SDIO_SetBusPower(SDIO_PowerState_ON);
 * }
 * \endcode
 */
void SDIO_SetBusPower(uint32_t SDIO_PowerState);

/**
 * rtl876x_sdio.h
 *
 * \brief  Initializes the SDIO peripheral according to the specified
 *         parameters in the SDIO_InitStruct.
 *
 * \param[in] SDIO_InitStruct: Pointer to a SDIO_InitTypeDef structure that
 *            contains the configuration information for the specified SDIO peripheral.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SD_InitHost(void)
 * {
 *     //Initialize the SDIO host peripheral
 *     SDIO_InitTypeDef SDIO_InitStructure;
 *     SDIO_StructInit(&SDIO_InitStructure);
 *     SDIO_Init(&SDIO_InitStructure);
 * }
 * \endcode
 */
void SDIO_Init(SDIO_InitTypeDef *SDIO_InitStruct);

/**
 * rtl876x_sdio.h
 *
 * \brief  Fills each SDIO_InitStruct member with its default value.
 *
 * \note   The default settings for the SDIO_InitStruct member are shown in the following table:
 *         | SDIO_InitStruct member | Default value           |
 *         |:----------------------:|:-----------------------:|
 *         | SDIO_TimeOut           | 0x0E                    |
 *         | SDIO_BusWide           | \ref SDIO_BusWide_1b    |
 *         | SDIO_ClockDiv          | \ref SDIO_CLOCK_DIV_256 |
 *
 * \param[in] SDIO_InitStruct: Pointer to an SDIO_InitTypeDef structure which will be initialized.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SD_InitHost(void)
 * {
 *     SDIO_InitTypeDef SDIO_InitStructure;
 *     SDIO_StructInit(&SDIO_InitStructure);
 *     SDIO_InitStructure.SDIO_TimeOut  = 0x0E;
 *     SDIO_InitStructure.SDIO_BusWide  = SDIO_BusWide_1b;
 *     SDIO_InitStructure.SDIO_ClockDiv = SDIO_CLOCK_DIV_128;
 *     SDIO_Init(&SDIO_InitStructure);
 * }
 * \endcode
 */
void SDIO_StructInit(SDIO_InitTypeDef *SDIO_InitStruct);

/**
 * rtl876x_sdio.h
 *
 * \brief  Initializes the SDIO data path according to the specified
 *         parameters in the SDIO_DataInitStruct.
 *
 * \param[in] SDIO_DataInitStruct: Pointer to a SDIO_DataInitTypeDef
 *            structure that contains the configuration information for the SDIO command.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SDIO_data_init(void)
 * {
 *     SDIO_DataStructInit(&SDIO_DataInitStruct);
 *     SDIO_DataInitStruct.SDIO_Address        = (uint32_t)SDIO_ADMA2_DescTab;
 *     SDIO_DataInitStruct.SDIO_BlockCount     = 1;
 *     SDIO_DataInitStruct.SDIO_BlockSize      = blockSize;
 *     SDIO_DataInitStruct.SDIO_TransferDir    = SDIO_TransferDir_READ;
 *     SDIO_DataInitStruct.SDIO_DMAEn          = ENABLE;
 *     SDIO_DataConfig(&SDIO_DataInitStruct);
 * }
 * \endcode
 */
void SDIO_DataConfig(SDIO_DataInitTypeDef *SDIO_DataInitStruct);

/**
 * rtl876x_sdio.h
 *
 * \brief  Fills each SDIO_DataInitStruct member with its default value.
 *
 * \param[in] SDIO_DataInitStruct: pointer to an SDIO_DataInitTypeDef structure
 *            which will be initialized.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SDIO_data_init(void)
 * {
 *     SDIO_DataStructInit(&SDIO_DataInitStruct);
 *     SDIO_DataInitStruct.SDIO_Address        = (uint32_t)SDIO_ADMA2_DescTab;
 *     SDIO_DataInitStruct.SDIO_BlockCount     = 1;
 *     SDIO_DataInitStruct.SDIO_BlockSize      = blockSize;
 *     SDIO_DataInitStruct.SDIO_TransferDir    = SDIO_TransferDir_READ;
 *     SDIO_DataInitStruct.SDIO_DMAEn          = ENABLE;
 *     SDIO_DataConfig(&SDIO_DataInitStruct);
 * }
 * \endcode
 */
void SDIO_DataStructInit(SDIO_DataInitTypeDef *SDIO_DataInitStruct);

/**
 * rtl876x_sdio.h
 *
 * \brief  Configure SDIO clock.
 *
 * \param[in]  clock_div: Value of SDIO clock divider, which can be 1/2/4/8/16/32/64/128/256.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_init(void)
 * {
 *     SDIO_SetClock(1);
 * }
 * \endcode
 */
void SDIO_SetClock(uint32_t clock_div);

/**
 * rtl876x_sdio.h
 *
 * \brief  Configure SDIO bus wide.
 *
 * \param[in] SDIO_BusWide: Value of SDIO bus wide.
 *            This parameter can be one of the following values:
 *            \arg SDIO_BusWide_1b: 1-bit mode.
 *            \arg SDIO_BusWide_4b: 4-bit mode.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_init(void)
 * {
 *     SDIO_SetBusWide(SDIO_BusWide_1b);
 * }
 * \endcode
 */
void SDIO_SetBusWide(uint32_t SDIO_BusWide);

/**
 * rtl876x_sdio.h
 *
 * \brief  Set start address of the descriptor table.
 *
 * \param[in] address: Start address of the descriptor table.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_init(void)
 * {
 *     uint32_t address = 0x12;
 *     SDIO_SetSystemAddress(address);
 * }
 * \endcode
 */
void SDIO_SetSystemAddress(uint32_t address);

/**
 * rtl876x_sdio.h
 *
 * \brief  Set SDIO block size.
 *
 * \param[in] BlockSize: The SDIO data block size.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_init(void)
 * {
 *     uint32_t blocksize = 8;
 *     SDIO_SetBlockSize(blocksize);
 * }
 * \endcode
 */
void SDIO_SetBlockSize(uint32_t BlockSize);

/**
 * rtl876x_sdio.h
 *
 * \brief  Set SDIO block count.
 *
 * \param[in] BlockCount: The SDIO block count.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_init(void)
 * {
 *     uint32_t blockcount = 1;
 *     SDIO_SetBlockCount(blockcount);
 * }
 * \endcode
 */
void SDIO_SetBlockCount(uint32_t BlockCount);

/**
 * rtl876x_sdio.h
 *
 * \brief  Initializes the SDIO Command according to the specified
 *         parameters in the SDIO_CmdInitStruct and send the command.
 *
 * \param[in] SDIO_CmdInitStruct: pointer to a SDIO_CmdInitTypeDef
 *            structure that contains the configuration information for the SDIO command.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_SendCommand(void)
 * {
 *     SDIO_CmdInitTypeDef SDIO_CmdInitStructure;
 *     SDIO_CmdInitStructure.SDIO_Argument     = 0;
 *     SDIO_CmdInitStructure.SDIO_CmdIndex     = SDIO_GO_IDLE_STATE;
 *     SDIO_CmdInitStructure.SDIO_CmdType      = NORMAL;
 *     SDIO_CmdInitStructure.SDIO_DataPresent  = NO_DATA;
 *     SDIO_CmdInitStructure.SDIO_CmdIdxCheck  = DISABLE;
 *     SDIO_CmdInitStructure.SDIO_CmdCrcCheck  = DISABLE;
 *     SDIO_CmdInitStructure.SDIO_ResponseType = SDIO_Response_No;
 *     SDIO_SendCommand(&SDIO_CmdInitStructure);
 * }
 * \endcode
 */
void SDIO_SendCommand(SDIO_CmdInitTypeDef *SDIO_CmdInitStruct);

/**
 * rtl876x_sdio.h
 *
 * \brief  Fills each SDIO_CmdInitStruct member with its default value.
 *
 * \param[in] SDIO_CmdInitStruct: pointer to an SDIO_CmdInitTypeDef structure which
 *            will be initialized.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_SendCommand(void)
 * {
 *     SDIO_CmdInitTypeDef SDIO_CmdInitStruct;
 *     SDIO_CmdInitStruct.SDIO_Argument     = 0;
 *     SDIO_CmdInitStruct.SDIO_CmdIndex     = SDIO_GO_IDLE_STATE;
 *     SDIO_CmdInitStruct.SDIO_CmdType      = NORMAL;
 *     SDIO_CmdInitStruct.SDIO_DataPresent  = NO_DATA;
 *     SDIO_CmdInitStruct.SDIO_CmdIdxCheck  = DISABLE;
 *     SDIO_CmdInitStruct.SDIO_CmdCrcCheck  = DISABLE;
 *     SDIO_CmdInitStruct.SDIO_ResponseType = SDIO_Response_No;
 *     SDIO_CmdStructInit(&SDIO_CmdInitStruct);
 * }
 * \endcode
 */
void SDIO_CmdStructInit(SDIO_CmdInitTypeDef *SDIO_CmdInitStruct);

/**
 * rtl876x_sdio.h
 *
 * \brief  Read data through the SDIO peripheral.
 *
 * \param  None.
 *
 * \return The value of the received data.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_demo(void)
 * {
 *     uint32_t data = 0;
 *     data = SDIO_ReadData();
 * }
 * \endcode
 */
uint32_t SDIO_ReadData(void);

/**
 * rtl876x_sdio.h
 *
 * \brief  Write data through the SDIO peripheral.
 *
 * \param[in] Data: Data to be transmitted.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_demo(void)
 * {
 *     uint32_t data[10] = {0x11, 0x22};
 *     SDIO_WriteData(data);
 * }
 * \endcode
 */
void SDIO_WriteData(uint32_t Data);

/**
 * rtl876x_sdio.h
 *
 * \brief  Enable or disable the specified SDIO interrupt status.
 *
 * \param[in] SDIO_INTStatus: Specifies the SDIO interrupt status to be enabled or disabled.
 *            This parameter can be one of the following values:
 *            \arg SDIO_INT_ERROR: Error interrupt signal caused by any type error in error interrupt status register.
 *            \arg SDIO_INT_CARD: Card interrupt status.
 *            \arg SDIO_INT_DMA: DMA interrupt status. In case of ADMA, by setting int field
 *                 in the descriptor table, host controller generates this interrupt.
 *            \arg SDIO_INT_TF_CMPL: Transfer complete interrupt status. Set when a read/write transfer
 *                 and a command with busy is complete.
 *            \arg SDIO_INT_CMD_CMPL: Command complete interrupt status. Set when get the end bit
 *                 of command response(Except Auto CMD12).
 *            \arg SDIO_BLOCK_GAP_EVENT: Set when stop at Block Gap Request.
 * \param[in] newState: New state of the specified SDIO interrupt status.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable the specified SDIO interrupt status.
 *            \arg DISABLE: Disable the specified SDIO interrupt status.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_config(void)
 * {
 *     SDIO_INTStatusConfig(SDIO_INT_CMD_CMPL, ENABLE);
 * }
 * \endcode
 */
void SDIO_INTStatusConfig(uint32_t SDIO_INTStatus, FunctionalState newState);

/**
 * rtl876x_sdio.h
 *
 * \brief  Enable or disable the specified SDIO interrupts.
 *
 * \param[in] SDIO_INT: specifies the SDIO interrupts sources to be enabled or disabled.
 *            This parameter can be one of the following values:
 *            \arg SDIO_INT_ERROR: Error interrupt signal caused by any type error in error interrupt status register.
 *            \arg SDIO_INT_CARD: Card interrupt status.
 *            \arg SDIO_INT_DMA: DMA interrupt status. In case of ADMA, by setting int field
 *                 in the descriptor table, host controller generates this interrupt.
 *            \arg SDIO_INT_TF_CMPL: Transfer complete interrupt status. Set when a read/write transfer
 *                 and a command with busy is complete.
 *            \arg SDIO_INT_CMD_CMPL: Command complete interrupt status. Set when get the end bit
 *                 of command response(Except Auto CMD12).
 *            \arg SDIO_BLOCK_GAP_EVENT: Set when stop at Block Gap Request.
 * \param[in] NewState: New state of the specified SDIO interrupts.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable the specified SDIO interrupts.
 *            \arg DISABLE: Disable the specified SDIO interrupts.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_config(void)
 * {
 *     SDIO_INTConfig(SDIO_INT_CMD_CMPL, ENABLE);
 * }
 * \endcode
 */
void SDIO_INTConfig(uint32_t SDIO_INT, FunctionalState newState);

/**
 * rtl876x_sdio.h
 *
 * \brief  Enable or disable the specified SDIO error interrupt status.
 *
 * \param[in] SDIO_INTStatus: Specifies the SDIO error interrupts status to be enabled or disabled.
 *            This parameter can be one or any combination of the following values:
 *            \arg SDIO_INT_VENDOR_SPECIFIC_ERR: Additional status bits can be defined in this register by the vendor.
 *            \arg SDIO_INT_ADMA_ERR: Set when the host controller detects errors during ADMA based data transfer.
 *            \arg SDIO_INT_AUTO_CMD12_ERR: Set when detecting that one of this bits D00-D04 in Auto CMD12
 *                 Error Status register has changed from 0 to 1.
 *            \arg SDIO_INT_CURRENT_LIMIT_ERR: Current limit error, by setting the SD Bus Power bit in the Power Control register,
 *                 the Host Controller is requested to supply power for the SD Bus.
 *            \arg SDIO_INT_DAT_END_BIT_ERR: Set when detecting 0 at the end bit position of read data
 *                 which uses the DAT line or at the end bit position of the CRC Status.
 *            \arg SDIO_INT_DAT_CRC_ERR: Set when detecting a CRC error in data transfer through the DAT line.
 *            \arg SDIO_INT_DAT_TIMEOUT_ERR: Set when detecting one of following timeout conditions:
 *                 Busy timeout for R1b,R5b type; Busy timeout after Write CRC status;
 *                 Write CRC Status timeout; Read Data timeout.
 *            \arg SDIO_INT_CMD_INDEX_ERR: Set if the command index error occurs in response to a command.
 *            \arg SDIO_INT_CMD_END_BIT_ERR: Set when detecting that the end bit of command response is 0.
 *            \arg SDIO_INT_CMD_CRC_ERR: Set when detecting a CRC error in the command response.
 *            \arg SDIO_INT_CMD_TIMEOUT_ERR: Set if no response is returned within 64 SDCLK cycles from
 *                 the end bit of command.
 * \param[in] newState: new state of the specified SDIO error interrupt status.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable the specified SDIO error interrupt status.
 *            \arg DISABLE: Disable the specified SDIO error interrupt status.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_error_config(void)
 * {
 *     SDIO_ErrrorINTStatusConfig(SDIO_INT_CMD_TIMEOUT_ERR | SDIO_INT_CMD_CRC_ERR, ENABLE);
 * }
 * \endcode
 */
void SDIO_ErrrorINTStatusConfig(uint32_t SDIO_INTStatus, FunctionalState newState);

/**
 * rtl876x_sdio.h
 *
 * \brief  Enable or disable the specified SDIO error interrupts.
 *
 * \param[in] SDIO_INT: specifies the SDIO error interrupts sources to be enabled or disabled.
 *            This parameter can be one of the following values:
 *            \arg SDIO_INT_VENDOR_SPECIFIC_ERR: Additional status bits can be defined in this register by the vendor.
 *            \arg SDIO_INT_ADMA_ERR: Set when the host controller detects errors during ADMA based data transfer.
 *            \arg SDIO_INT_AUTO_CMD12_ERR: Set when detecting that one of this bits D00-D04 in Auto CMD12
 *                 Error Status register has changed from 0 to 1.
 *            \arg SDIO_INT_CURRENT_LIMIT_ERR: Current limit error, by setting the SD Bus Power bit in the Power Control register,
 *                 the Host Controller is requested to supply power for the SD Bus.
 *            \arg SDIO_INT_DAT_END_BIT_ERR: Set when detecting 0 at the end bit position of read data
 *                 which uses the DAT line or at the end bit position of the CRC Status.
 *            \arg SDIO_INT_DAT_CRC_ERR: Set when detecting a CRC error in data transfer through the DAT line.
 *            \arg SDIO_INT_DAT_TIMEOUT_ERR: Set when detecting one of following timeout conditions:
 *                 Busy timeout for R1b,R5b type; Busy timeout after Write CRC status;
 *                 Write CRC Status timeout; Read Data timeout.
 *            \arg SDIO_INT_CMD_INDEX_ERR: Set if the command index error occurs in response to a command.
 *            \arg SDIO_INT_CMD_END_BIT_ERR: Set when detecting that the end bit of command response is 0.
 *            \arg SDIO_INT_CMD_CRC_ERR: Set when detecting a CRC error in the command response.
 *            \arg SDIO_INT_CMD_TIMEOUT_ERR: Set if no response is returned within 64 SDCLK cycles from
 *                 the end bit of command.
 * \param[in] NewState: New state of the specified SDIO error interrupts.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable the specified SDIO error interrupts.
 *            \arg DISABLE: Disable the specified SDIO error interrupts.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_error_config(void)
 * {
 *     SDIO_ErrrorINTConfig(SDIO_INT_CMD_CRC_ERR, ENABLE);
 * }
 * \endcode
 */
void SDIO_ErrrorINTConfig(uint32_t SDIO_INT, FunctionalState newState);

/**
 * rtl876x_sdio.h
 *
 * \brief  Enable or disable the specified SDIO wake up event.
 *
 * \param[in] SDIO_WakeUp: Specifies the SDIO wake up sources to be enabled or disabled.
 *            This parameter can be one of the following values:
 *            \arg SDIO_WAKE_UP_SDCARD_REMOVAL: Wakeup Event Enable On SD Card Removal. This bit enables
 *                 wakeup event via Card Removal assertion in the Normal Interrupt Status register.
 *            \arg SDIO_WAKE_UP_SDCARD_INSERT: Wakeup Event Enable On SD Card Insertion. This bit enables
 *                 wakeup event via Card Insertion assertion in the Normal Interrupt Status register.
 *            \arg SDIO_WAKE_UP_SDCARD_INTR: Wakeup Event Enable On Card Interruptis bit enables
 *                 wakeup event via Card Interrupt assertion in the Normal Interrupt Status register.
 * \param[in] NewState: New state of the SDIO wake up event.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable the specified SDIO wake up event.
 *            \arg DISABLE: Disable the specified SDIO wake up event.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_wakeup_config(void)
 * {
 *     SDIO_WakeUpConfig(SDIO_WAKE_UP_SDCARD_REMOVAL, ENABLE);
 * }
 * \endcode
 */
void SDIO_WakeUpConfig(uint32_t SDIO_WakeUp, FunctionalState newState);

/**
 * rtl876x_sdio.h
 *
 * \brief  Check whether the specified SDIO interrupt is set.
 *
 * \param[in] SDIO_INT: specifies the interrupt to check.
 *            This parameter can be one of the following values:
 *            \arg SDIO_INT_ERROR: Error interrupt signal caused by any type error in error interrupt status register.
 *            \arg SDIO_INT_CARD: Card interrupt status.
 *            \arg SDIO_INT_DMA: DMA interrupt status. In case of ADMA, by setting int field
 *                 in the descriptor table, host controller generates this interrupt.
 *            \arg SDIO_INT_TF_CMPL: Transfer complete interrupt status. Set when a read/write transfer
 *                 and a command with busy is complete.
 *            \arg SDIO_INT_CMD_CMPL: Command complete interrupt status. Set when get the end bit
 *                 of command response(Except Auto CMD12).
 *            \arg SDIO_BLOCK_GAP_EVENT: Set when stop at Block Gap Request.
 *
 * \return The new state of ITStatus.
 * \retval SET: The specified SDIO interrupt is set.
 * \retval RESET: The specified SDIO interrupt is unset.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_handler(void)
 * {
 *     if (SDIO_GetINTStatus(SDIO_INT_CMD_CMPL) == SET)
 *     {
 *         //add user code here.
 *     }
 * }
 * \endcode
 */
ITStatus SDIO_GetINTStatus(uint32_t SDIO_INT);

/**
 * rtl876x_sdio.h
 *
 * \brief  Check whether the specified SDIO error interrupt is set.
 *
 * \param[in] SDIO_INT: Specifies the error interrupt to check.
 *            This parameter can be one of the following values:
 *            \arg SDIO_INT_VENDOR_SPECIFIC_ERR: Additional status bits can be defined in this register by the vendor.
 *            \arg SDIO_INT_ADMA_ERR: Set when the host controller detects errors during ADMA based data transfer.
 *            \arg SDIO_INT_AUTO_CMD12_ERR: Set when detecting that one of this bits D00-D04 in Auto CMD12
 *                 Error Status register has changed from 0 to 1.
 *            \arg SDIO_INT_CURRENT_LIMIT_ERR: Current limit error, by setting the SD Bus Power bit in the Power Control register,
 *                 the Host Controller is requested to supply power for the SD Bus.
 *            \arg SDIO_INT_DAT_END_BIT_ERR: Set when detecting 0 at the end bit position of read data
 *                 which uses the DAT line or at the end bit position of the CRC Status.
 *            \arg SDIO_INT_DAT_CRC_ERR: Set when detecting a CRC error in data transfer through the DAT line.
 *            \arg SDIO_INT_DAT_TIMEOUT_ERR: Set when detecting one of following timeout conditions:
 *                 Busy timeout for R1b,R5b type; Busy timeout after Write CRC status;
 *                 Write CRC Status timeout; Read Data timeout.
 *            \arg SDIO_INT_CMD_INDEX_ERR: Set if the command index error occurs in response to a command.
 *            \arg SDIO_INT_CMD_END_BIT_ERR: Set when detecting that the end bit of command response is 0.
 *            \arg SDIO_INT_CMD_CRC_ERR: Set when detecting a CRC error in the command response.
 *            \arg SDIO_INT_CMD_TIMEOUT_ERR: Set if no response is returned within 64 SDCLK cycles from
 *                 the end bit of command.
 *
 * \return The new state of ITStatus.
 * \retval SET: The specified SDIO error interrupt is set.
 * \retval RESET: The specified SDIO error interrupt is unset.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_handler(void)
 * {
 *     if (SDIO_GetErrorINTStatus(SDIO_INT_CMD_TIMEOUT_ERR) == SET)
 *     {
 *         //add user code here.
 *     }
 * }
 * \endcode
 */
ITStatus SDIO_GetErrorINTStatus(uint32_t SDIO_INT);

/**
 * rtl876x_sdio.h
 *
 * \brief  Clear the specified SDIO interrupt pending bits.
 *
 * \param[in] SDIO_INT: Specifies the interrupt pending bit to clear.
 *            This parameter can be one of the following values:
 *            \arg SDIO_INT_ERROR: Error interrupt signal caused by any type error in error interrupt status register.
 *            \arg SDIO_INT_CARD: Card interrupt status.
 *            \arg SDIO_INT_DMA: DMA interrupt status. In case of ADMA, by setting int field
 *                 in the descriptor table, host controller generates this interrupt.
 *            \arg SDIO_INT_TF_CMPL: Transfer complete interrupt status. Set when a read/write transfer
 *                 and a command with busy is complete.
 *            \arg SDIO_INT_CMD_CMPL: Command complete interrupt status. Set when get the end bit
 *                 of command response(Except Auto CMD12).
 *            \arg SDIO_BLOCK_GAP_EVENT: Set when stop at Block Gap Request.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_handler(void)
 * {
 *     if (SDIO_GetINTStatus(SDIO_INT_CMD_CMPL) == SET)
 *     {
 *         SDIO_ClearINTPendingBit(SDIO_INT_CMD_CMPL);
 *     }
 * }
 * \endcode
 */
void SDIO_ClearINTPendingBit(uint32_t SDIO_INT);

/**
 * rtl876x_sdio.h
 *
 * \brief  Clear the specified SDIO error interrupt pending bits.
 *
 * \param[in] SDIO_INT: Specifies the SDIO error interrupt pending bit to clear.
 *            This parameter can be one of the following values:
 *            \arg SDIO_INT_VENDOR_SPECIFIC_ERR: Additional status bits can be defined in this register by the vendor.
 *            \arg SDIO_INT_ADMA_ERR: Set when the host controller detects errors during ADMA based data transfer.
 *            \arg SDIO_INT_AUTO_CMD12_ERR: Set when detecting that one of this bits D00-D04 in Auto CMD12
 *                 Error Status register has changed from 0 to 1.
 *            \arg SDIO_INT_CURRENT_LIMIT_ERR: Current limit error, by setting the SD Bus Power bit in the Power Control register,
 *                 the Host Controller is requested to supply power for the SD Bus.
 *            \arg SDIO_INT_DAT_END_BIT_ERR: Set when detecting 0 at the end bit position of read data
 *                 which uses the DAT line or at the end bit position of the CRC Status.
 *            \arg SDIO_INT_DAT_CRC_ERR: Set when detecting a CRC error in data transfer through the DAT line.
 *            \arg SDIO_INT_DAT_TIMEOUT_ERR: Set when detecting one of following timeout conditions:
 *                 Busy timeout for R1b,R5b type; Busy timeout after Write CRC status;
 *                 Write CRC Status timeout; Read Data timeout.
 *            \arg SDIO_INT_CMD_INDEX_ERR: Set if the command index error occurs in response to a command.
 *            \arg SDIO_INT_CMD_END_BIT_ERR: Set when detecting that the end bit of command response is 0.
 *            \arg SDIO_INT_CMD_CRC_ERR: Set when detecting a CRC error in the command response.
 *            \arg SDIO_INT_CMD_TIMEOUT_ERR: Set if no response is returned within 64 SDCLK cycles from
 *                 the end bit of command.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_handler(void)
 * {
 *     if (SDIO_GetErrorINTStatus(SDIO_INT_CMD_TIMEOUT_ERR) == SET)
 *     {
 *         SDIO_ClearErrorINTPendingBit(SDIO_INT_CMD_TIMEOUT_ERR);
 *     }
 * }
 * \endcode
 */
void SDIO_ClearErrorINTPendingBit(uint32_t SDIO_INT);

/**
 * rtl876x_sdio.h
 *
 * \brief  Return response received from the card for the last command.
 *
 * \param[in] SDIO_RSP: Specifies the SDIO response register.
 *            This parameter can be one of the following values:
 *            \arg SDIO_RSP0: Response Register 0~1.
 *            \arg SDIO_RSP2: Response Register 2~3.
 *            \arg SDIO_RSP4: Response Register 4~5.
 *            \arg SDIO_RSP6: Response Register 6~7.
 *
 * \return value of SDIO_RESP.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_init(void)
 * {
 *     uint32_t response = SDIO_GetResponse(SDIO_RSP0);
 * }
 * \endcode
 */
uint32_t SDIO_GetResponse(uint32_t SDIO_RSP);


/**
 * rtl876x_sdio.h
 *
 * \brief  Select the SDIO data bus pin.
 *
 * \param[in] SDIO_BusWide: Value of SDIO bus wide.
 *            This parameter can be one of the following values:
 *            \arg SDIO_BusWide_1b: 1-bit mode.
 *            \arg SDIO_BusWide_4b: 4-bit mode.
 * \param[in] NewState: New state of the SDIO data pin.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable the SDIO data bus pin.
 *            \arg DISABLE: Disable the SDIO data bus pin.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_init(void)
 * {
 *     SDIO_DataPinConfig(SDIO_BusWide_1b, ENABLE);
 * }
 * \endcode
 */
void SDIO_DataPinConfig(uint32_t SDIO_BusWide, FunctionalState NewState);


/**
 * rtl876x_sdio.h
 *
 * \brief  Set the flag for SDIO operating.
 *
 * \param[in] sdio_mode: Specifies the Current SDIO mode.
 *            This parameter can be one of the following values:
 *            \arg SDIO_FLAG_IDLE: Set the Current SDIO mode to idle.
 *            \arg SDIO_FLAG_READ: Set the Current SDIO mode to read.
 *            \arg SDIO_FLAG_WRITE: Set the Current SDIO mode to write.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_init(void)
 * {
 *     sdio_set_flag(SDIO_FLAG_READ);
 * }
 * \endcode
 */
extern void sdio_set_flag(uint32_t sdio_mode);


/**
 * rtl876x_sdio.h
 *
 * \brief  Get the SDIO busy flag.
 *
 * \param  None.
 *
 * \return Indicate if SDIO is busy.
 * \retval true   The SDIO is busy.
 * \retval false  The SDIO is not busy.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void sdio_demo(void)
 * {
 *     sdio_get_busy_flag();
 * }
 * \endcode
 */
extern bool sdio_get_busy_flag(void);

/**
 * rtl876x_sdio.h
 *
 * \brief  Select bus delay through the SDIO peripheral.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API
 *
 * \param[in] SDIO_BusWide: Value of SDIO bus wide.
 *            This parameter can be one of the following values:
 *            \arg SDIO_BusWide_1b: 1-bit mode.
 *            \arg SDIO_BusWide_4b: 4-bit mode.
 * \param[in] SDIO_BusDelay: bus delay time(unit ns), range from 0 to 15.
 *
 * \return None.
 */
void SDIO_BusDelaySel(uint32_t SDIO_BusWide, uint8_t SDIO_BusDelay);

#ifdef __cplusplus
}
#endif

#endif /* _RTL876x_SDIO_H_ */

/** @} */ /* End of group SDIO_Exported_Functions */
/** @} */ /* End of group 87x3d_SDIO */

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/

