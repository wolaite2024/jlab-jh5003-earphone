/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_3wire_spi.h
* @brief     This file provides all the 3-wire SPI firmware functions.
* @details
* @author    elliot chen
* @date      2024-07-18
* @version   v1.0
* *********************************************************************************************************
*/


#ifndef _RTL876x_3WIRE_SPI_H_
#define _RTL876x_3WIRE_SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"

/* Peripheral: SPI3WIRE */
/* Description: SPI3WIRE register defines */

/* Register: CFGR ------------------------------------------------------------*/
/* Description: SPI3WIRE configuration register. Offset: 0x30. Address: 0x40004030. */

/* CFGR[31] :SPI3WIRE_SSI_EN. 0x1: enable SPI3WIRE mode. 0x0: disable SPI3WIRE mode. */
#define SPI3WIRE_SSI_EN_Pos                 (31UL)
#define SPI3WIRE_SSI_EN_Msk                 (0x1UL << SPI3WIRE_SSI_EN_Pos)
#define SPI3WIRE_SSI_EN_CLR                 (~SPI3WIRE_SSI_EN_Msk)
/* CFGR[30] :SPI3WIRE_SSI_CS_EN. 0x1: enable CS signal. 0x0: disable CS signal. */
#define SPI3WIRE_SSI_CS_EN_Pos              (30UL)
#define SPI3WIRE_SSI_CS_EN_Msk              (0x1UL << SPI3WIRE_SSI_CS_EN_Pos)
#define SPI3WIRE_SSI_CS_EN_CLR              (~SPI3WIRE_SSI_CS_EN_Msk)
/* CFGR[29] :SPI3WIRE_SSI_END_EXT_EN. 0x1: extend mode. 0x0: normal mode. */
/* Note: Using this control bit can extend the timing window for SPI output enable = 0, The extend time is 1/(2*SPI_CLK) */
#define SPI3WIRE_SSI_END_EXT_EN_Pos         (29UL)
#define SPI3WIRE_SSI_END_EXT_EN_Msk         (0x1UL << SPI3WIRE_SSI_END_EXT_EN_Pos)
#define SPI3WIRE_SSI_END_EXT_EN_CLR         (~SPI3WIRE_SSI_END_EXT_EN_Msk)
/* CFGR[28] :SPI3WIRE_SSI_OE_DLY_EN. 0x1: enable. 0x0: disable. */
/* Note: Extend 1T of ssi_OE(output-> intput switch delay 4T->5T  ) */
#define SPI3WIRE_SSI_OE_DLY_EN_Pos          (28UL)
#define SPI3WIRE_SSI_OE_DLY_EN_Msk          (0x1UL << SPI3WIRE_SSI_OE_DLY_EN_Pos)
#define SPI3WIRE_SSI_OE_DLY_EN_CLR          (~SPI3WIRE_SSI_OE_DLY_EN_Msk)
/* CFGR[21] :SPI3WIRE_SSI_RESYNC_TIME. */
/* output n*T low pulse, write 1 to 0x38 bit[31] to output;The T time is 1/(2*ssi_CLK) */
#define SPI3WIRE_SSI_RESYNC_TIME_Pos        (21UL)
#define SPI3WIRE_SSI_RESYNC_TIME_Msk        (0xfUL << SPI3WIRE_SSI_RESYNC_TIME_Pos)
#define SPI3WIRE_SSI_RESYNC_TIME_CLR        (~SPI3WIRE_SSI_RESYNC_TIME_Msk)
/* CFGR[20] :SPI3WIRE_SSI_BURST_READ_EN. 0x1: enable burst read. 0x0: disable. */
#define SPI3WIRE_SSI_BURST_READ_EN_Pos      (20UL)
#define SPI3WIRE_SSI_BURST_READ_EN_Msk      (0x1UL << SPI3WIRE_SSI_BURST_READ_EN_Pos)
#define SPI3WIRE_SSI_BURST_READ_EN_CLR      (~SPI3WIRE_SSI_BURST_READ_EN_Msk)
/* CFGR[16] :SPI3WIRE_SSI_BURST_READ_NUM. The total number of read data bytes in burst mode */
#define SPI3WIRE_SSI_BURST_READ_NUM_Pos     (16UL)
#define SPI3WIRE_SSI_BURST_READ_NUM_Msk     (0xfUL << SPI3WIRE_SSI_BURST_READ_NUM_Pos)
#define SPI3WIRE_SSI_BURST_READ_NUM_CLR     (~SPI3WIRE_SSI_BURST_READ_NUM_Msk)
/* CFGR[8] :SPI3WIRE_SSI_DIV_NUM. The divider number to generate 2x SPI_CLK */
#define SPI3WIRE_SSI_DIV_NUM_Pos            (8UL)
#define SPI3WIRE_SSI_DIV_NUM_Msk            (0xffUL << SPI3WIRE_SSI_DIV_NUM_Pos)
#define SPI3WIRE_SSI_DIV_NUM_CLR            (~SPI3WIRE_SSI_DIV_NUM_Msk)
/* CFGR[3] :SPI3WIRE_SSI_DLY_CYCLE. The delay time from the end of address phase to the start of read data phase. */
#define SPI3WIRE_SSI_DLY_CYCLE_Pos          (3UL)
#define SPI3WIRE_SSI_DLY_CYCLE_Msk          (0x1fUL << SPI3WIRE_SSI_DLY_CYCLE_Pos)
#define SPI3WIRE_SSI_DLY_CYCLE_CLR          (~SPI3WIRE_SSI_DLY_CYCLE_Msk)
/* CFGR[1] :SPI3WIRE_SSI_INT_MASK. 0x1: mask. 0x0: no mask. */
#define SPI3WIRE_SSI_INT_MASK_Pos           (1UL)
#define SPI3WIRE_SSI_INT_MASK_Msk           (0x1UL << SPI3WIRE_SSI_INT_MASK_Pos)
#define SPI3WIRE_SSI_INT_MASK_CLR           (~SPI3WIRE_SSI_INT_MASK_Msk)
/* CFGR[0] :SPI3WIRE_SSI_INT_EN. 0x1: enable. 0x0: disable. */
#define SPI3WIRE_SSI_INT_EN_Pos             (0UL)
#define SPI3WIRE_SSI_INT_EN_Msk             (0x1UL << SPI3WIRE_SSI_INT_EN_Pos)
#define SPI3WIRE_SSI_INT_EN_CLR             (~SPI3WIRE_SSI_INT_EN_Msk)

/* Register: CR ------------------------------------------------------------*/
/* Description: SPI3WIRE control register. Offset: 0x34. Address: 0x40004034. */

/* CR[15] :SPI3WIRE_RW_MODE. 0x1: write mode. 0x0: read mode. */
#define SPI3WIRE_RW_MODE_Pos                (15UL)
#define SPI3WIRE_RW_MODE_Msk                (0x1UL << SPI3WIRE_RW_MODE_Pos)
#define SPI3WIRE_RW_MODE_CLR                (~SPI3WIRE_RW_MODE_Msk)
/* CR[8] :SPI3WIRE_ADDRESS. */
#define SPI3WIRE_ADDRESS_Pos                (8UL)
#define SPI3WIRE_ADDRESS_Msk                (0x7fUL << SPI3WIRE_ADDRESS_Pos)
#define SPI3WIRE_ADDRESS_CLR                (~SPI3WIRE_ADDRESS_Msk)
/* CR[0] :SPI3WIRE_DATA. write data ; in read mode, this data byte is useless */
#define SPI3WIRE_DATA_Pos                   (0UL)
#define SPI3WIRE_DATA_Msk                   (0xffUL << SPI3WIRE_ADDRESS_Pos)
#define SPI3WIRE_DATA_CLR                   (~SPI3WIRE_ADDRESS_Msk)

/* Register: INTCR ------------------------------------------------------------*/
/* Description: SPI3WIRE interrupt clear register. Offset: 0x38. Address: 0x40004038. */

/* INTCR[31] :SPI3WIRE_RESYNV_EN. 0x1: trriger resync signal. 0x0: disable resync signal. */
#define SPI3WIRE_RESYNV_EN_Pos              (31UL)
#define SPI3WIRE_RESYNV_EN_Msk              (0x1UL << SPI3WIRE_RESYNV_EN_Pos)
#define SPI3WIRE_RESYNV_EN_CLR              (~SPI3WIRE_RESYNV_EN_Msk)
/* INTCR[2] :SPI3WIRE_RD_DATA_CLEAR. 0x1: write clear. */
#define SPI3WIRE_RD_DATA_CLEAR_Pos          (2UL)
#define SPI3WIRE_RD_DATA_CLEAR_Msk          (0x1UL << SPI3WIRE_RD_DATA_CLEAR_Pos)
/* INTCR[1] :SPI3WIRE_RD_NUM_CLEAR. 0x1: write clear. */
#define SPI3WIRE_RD_NUM_CLEAR_Pos           (1UL)
#define SPI3WIRE_RD_NUM_CLEAR_Msk           (0x1UL << SPI3WIRE_RD_NUM_CLEAR_Pos)
/* INTCR[0] :SPI3WIRE_INT_CLEAR. 0x1: write clear. */
#define SPI3WIRE_INT_CLEAR_Pos              (0UL)
#define SPI3WIRE_INT_CLEAR_Msk              (0x1UL << SPI3WIRE_INT_CLEAR_Pos)

#define SPI3WIRE_FIFO_INT_ALL_CLR           (SPI3WIRE_RD_DATA_CLEAR_Msk | \
                                             SPI3WIRE_RD_NUM_CLEAR_Msk | \
                                             SPI3WIRE_INT_CLEAR_Msk)

/* Register: SR ------------------------------------------------------------*/
/* Description: SPI3WIRE status register. Offset: 0x3C. Address: 0x4000403C. */

/* SR[6] :SPI3WIRE_RESYNV_BUSY. 0x1: Resync busy. 0x0: Resync idle. */
#define SPI3WIRE_RESYNV_BUSY_Pos                (6UL)
#define SPI3WIRE_RESYNV_BUSY_Msk                (0x1UL << SPI3WIRE_RESYNV_BUSY_Pos)
#define SPI3WIRE_RESYNV_BUSY_CLR                (~SPI3WIRE_RESYNV_BUSY_Msk)
/* SR[5] :SPI3WIRE_INT_STATUS. 0x1: Interrupt. 0x0: No interrupt. */
#define SPI3WIRE_INT_STATUS_Pos                 (5UL)
#define SPI3WIRE_INT_STATUS_Msk                 (0x1UL << SPI3WIRE_INT_STATUS_Pos)
#define SPI3WIRE_INT_STATUS_CLR                 (~SPI3WIRE_INT_STATUS_Msk)
/* SR[4] :SPI3WIRE_BUSY. 0x1: Busy. 0x0:Idle. */
#define SPI3WIRE_BUSY_Pos                       (4UL)
#define SPI3WIRE_BUSY_Msk                       (0x1UL << SPI3WIRE_BUSY_Pos)
#define SPI3WIRE_BUSY_CLR                       (~SPI3WIRE_BUSY_Msk)
/* SR[0] :SPI3WIRE_RDATA_NUM. The total number of data byte in each SPI read transaction */
#define SPI3WIRE_RDATA_NUM_Pos                  (0UL)
#define SPI3WIRE_RDATA_NUM_Msk                  (0xfUL << SPI3WIRE_RDATA_NUM_Pos)
#define SPI3WIRE_RDATA_NUM_CLR                  (~SPI3WIRE_RDATA_NUM_Msk)


/** @addtogroup 87x3d_THREE_WIRE_SPI 3-Wire SPI
  * @brief 3-Wire SPI driver module.
  * @{
  */

/*============================================================================*
 *                         Types
 *============================================================================*/


/** @defgroup THREE_WIRE_SPI_Exported_Types 3-Wire SPI Exported Types
  * @{
  */

/**
 * @brief 3-Wire SPI initialize parameters.
 *
 *
 */
typedef struct
{
    uint32_t SPI3WIRE_SysClock;     /**< Specify system clock.
                                         This parameter must be 20MHz. */

    uint32_t SPI3WIRE_Speed;        /*!< Specifies 3-wire SPI clock.
                                         This parameter can be a value of 20/256MHz ~ 2MHz. */

    uint32_t SPI3WIRE_Mode;         /*!< Specifies 3-wire SPI operation mode.
                                         This parameter can be a value of @ref THREE_WIRE_SPI_mode. */

    uint32_t SPI3WIRE_ReadDelay;    /*!< Specifies the delay time from the end of address phase to the start of read data phase.
                                         This parameter can be a value of 0x0 to 0x1f.
                                         delay time = (SPI3WIRE_ReadDelay+1)/(2*SPI3WIRE_Speed). */

    uint32_t SPI3WIRE_OutputDelay;  /*!< Specifies 3-wire SPI output delay 1T or not.
                                         This parameter can be a value of @ref THREE_WIRE_SPI_OE_delay_config. */

    uint32_t SPI3WIRE_ExtMode;      /*!< Specifies extended timing window for 3-wire SPI output enable = 0.
                                         This parameter can be a value of @ref THREE_WIRE_SPI_end_extend_mode. */
} SPI3WIRE_InitTypeDef;

/** End of THREE_WIRE_SPI_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/

/** @defgroup THREE_WIRE_SPI_Exported_Constants 3-Wire SPI Exported Constants
  * @{
  */

/** @defgroup THREE_WIRE_SPI_mode 3-Wire SPI Mode
  * @{
  */

#define SPI3WIRE_3WIRE_MODE                     ((uint32_t)(1 << SPI3WIRE_SSI_CS_EN_Pos)) //!< 3-Wire mode, enable CS.
#define SPI3WIRE_2WIRE_MODE                     ((uint32_t)(0 << SPI3WIRE_SSI_CS_EN_Pos)) //!< 2-Wire mode, disable CS.

#define IS_SPI3WIRE_MODE(MODE) ((MODE) == SPI3WIRE_2WIRE_MODE || (MODE) == SPI3WIRE_3WIRE_MODE)

/** End of THREE_WIRE_SPI_mode
  * @}
  */

/** @defgroup THREE_WIRE_SPI_OE_delay_config 3-Wire SPI Output Delay
  * @{
  */

#define SPI3WIRE_OE_DELAY_1T                     ((uint32_t)(1 << SPI3WIRE_SSI_OE_DLY_EN_Pos))  //!< 3-Wire SPI output delay 1T.
#define SPI3WIRE_OE_DELAY_NONE                   ((uint32_t)(0 << SPI3WIRE_SSI_OE_DLY_EN_Pos))  //!< 3-Wire SPI output has no delay of 1T.

#define IS_SPI3WIRE_OE_DELAY_CFG(CFG) ((CFG) == SPI3WIRE_OE_DELAY_1T || (CFG) == SPI3WIRE_OE_DELAY_NONE)

/** End of THREE_WIRE_SPI_OE_delay_config
  * @}
  */


/** @defgroup THREE_WIRE_SPI_end_extend_mode 3-Wire SPI Extend Mode
  * @{
  */

#define SPI3WIRE_EXTEND_MODE                     ((uint32_t)(1 << SPI3WIRE_SSI_END_EXT_EN_Pos))  //!< 3-Wire SPI is in extend mode.
#define SPI3WIRE_NORMAL_MODE                     ((uint32_t)(0 << SPI3WIRE_SSI_END_EXT_EN_Pos))  //!< 3-Wire SPI is in normal mode.

#define IS_SPI3WIRE_END_EXTEND_MODE(MODE) ((MODE) == SPI3WIRE_EXTEND_MODE || (MODE) == SPI3WIRE_NORMAL_MODE)

/** End of THREE_WIRE_SPI_end_extend_mode
  * @}
  */

/** @defgroup THREE_WIRE_SPI_read_cycle_delay 3-Wire SPI Read Delay Cycle
  * @{
  */

#define IS_SPI3WIRE_READ_CYCLE_DELAY(DELAY) (DELAY <= 0x1f)   //!< 3-Wire SPI read delay cycle is between 0 and 0x1f.

/** End of THREE_WIRE_SPI_read_cycle_delay
  * @}
  */


/** @defgroup THREE_WIRE_SPI_FLAG 3-Wire SPI Flag
  * @{
  */

#define SPI3WIRE_FLAG_BUSY                       ((uint32_t)SPI3WIRE_BUSY_Msk)          //!< 3-Wire SPI busy status flag.
#define SPI3WIRE_FLAG_INT_IND                    ((uint32_t)SPI3WIRE_INT_STATUS_Msk)    //!< 3-Wire SPI interrupt status flag.
#define SPI3WIRE_FLAG_RESYNC_BUSY                ((uint32_t)SPI3WIRE_RESYNV_BUSY_Msk)   //!< 3-Wire SPI resync busy status flag.

#define IS_SPI3WIRE_FLAG(FLAG) (((FLAG) == SPI3WIRE_FLAG_BUSY) || \
                                ((FLAG) == SPI3WIRE_FLAG_INT_IND) || \
                                ((FLAG) == SPI3WIRE_FLAG_RESYNC_BUSY))

/** End of THREE_WIRE_SPI_FLAG
  * @}
  */

/** @defgroup THREE_WIRE_SPI_interrupt_definition 3-Wire SPI Interrupt Definition
  * @{
  */

#define SPI3WIRE_INT_BIT                          ((uint32_t)SPI3WIRE_SSI_INT_EN_Msk)     //!< When reading state ends, 3-wire SPI interrupt will be triggered.

#define IS_SPI3WIRE_INT(INT) ((INT) == SPI3WIRE_INT_BIT)

/** End of THREE_WIRE_SPI_interrupt_definition
  * @}
  */

/** End of THREE_WIRE_SPI_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup THREE_WIRE_SPI_Exported_Functions 3-Wire SPI Exported Functions
  * @{
  */

/**
 * rtl876x_3wire_spi.h
 *
 * \brief   Disable the 3-wire SPI peripheral clock, and restore registers to their default values.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void Driver_SPI3WIRE_Init(void)
 * {
 *     SPI3WIRE_DeInit();
 * }
 * \endcode
 */
void SPI3WIRE_DeInit(void);

/**
 * rtl876x_3wire_spi.h
 *
 * \brief   Initializes the 3-wire SPI peripheral according to the specified
 *          parameters in the SPI3WIRE_InitStruct.
 *
 * \param[in] SPI3WIRE_InitStruct: Pointer to a SPI3WIRE_InitTypeDef structure that
 *            contains the configuration information for the specified 3-wire SPI peripheral.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void Driver_SPI3WIRE_Init(void)
 * {
 *     RCC_PeriphClockCmd(APBPeriph_SPI2W, APBPeriph_SPI2W_CLOCK, ENABLE);
 *
 *     SPI3WIRE_InitTypeDef  SPI3WIRE_InitStruct;
 *     SPI3WIRE_StructInit(&SPI3WIRE_InitStruct);
 *
 *     SPI3WIRE_InitStruct.SPI3WIRE_SysClock    = 20000000;//20M
 *     SPI3WIRE_InitStruct.SPI3WIRE_Speed       = 1000000;
 *     SPI3WIRE_InitStruct.SPI3WIRE_Mode        = SPI3WIRE_2WIRE_MODE;
 *     SPI3WIRE_InitStruct.SPI3WIRE_ReadDelay   = 0x5;
 *     SPI3WIRE_InitStruct.SPI3WIRE_OutputDelay = SPI3WIRE_OE_DELAY_1T;
 *     SPI3WIRE_InitStruct.SPI3WIRE_ExtMode     = SPI3WIRE_NORMAL_MODE;
 *
 *     SPI3WIRE_Init(&SPI3WIRE_InitStruct);
 * }
 * \endcode
 */
void SPI3WIRE_Init(SPI3WIRE_InitTypeDef *SPI3WIRE_InitStruct);

/**
 * rtl876x_3wire_spi.h
 *
 * \brief   Fills each SPI3WIRE_InitStruct member with its default value.
 *
 * \note   The default settings for the SPI3WIRE_InitStruct member are shown in the following table:
 *         | SPI3WIRE_InitStruct member | Default value                |
 *         |:--------------------------:|:----------------------------:|
 *         | SPI3WIRE_SysClock          | 20000000                     |
 *         | SPI3WIRE_Speed             | 1000000                      |
 *         | SPI3WIRE_Mode              | \ref SPI3WIRE_2WIRE_MODE     |
 *         | SPI3WIRE_ReadDelay         | 0                            |
 *         | SPI3WIRE_OutputDelay       | \ref SPI3WIRE_OE_DELAY_1T    |
 *         | SPI3WIRE_ExtMode           | \ref SPI3WIRE_NORMAL_MODE    |
 *
 * \param[in] SPI3WIRE_InitStruct: Pointer to an SPI3WIRE_InitTypeDef structure which will be initialized.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void Driver_SPI3WIRE_Init(void)
 * {
 *     RCC_PeriphClockCmd(APBPeriph_SPI2W, APBPeriph_SPI2W_CLOCK, ENABLE);
 *
 *     SPI3WIRE_InitTypeDef  SPI3WIRE_InitStruct;
 *     SPI3WIRE_StructInit(&SPI3WIRE_InitStruct);
 *
 *     SPI3WIRE_InitStruct.SPI3WIRE_SysClock    = 20000000;//20M
 *     SPI3WIRE_InitStruct.SPI3WIRE_Speed       = 1000000;
 *     SPI3WIRE_InitStruct.SPI3WIRE_Mode        = SPI3WIRE_2WIRE_MODE;
 *     SPI3WIRE_InitStruct.SPI3WIRE_ReadDelay   = 0x5;
 *     SPI3WIRE_InitStruct.SPI3WIRE_OutputDelay = SPI3WIRE_OE_DELAY_1T;
 *     SPI3WIRE_InitStruct.SPI3WIRE_ExtMode     = SPI3WIRE_NORMAL_MODE;
 *
 *     SPI3WIRE_Init(&SPI3WIRE_InitStruct);
 * }
 * \endcode
 */
void SPI3WIRE_StructInit(SPI3WIRE_InitTypeDef *SPI3WIRE_InitStruct);

/**
 * rtl876x_3wire_spi.h
 *
 * \brief   Configure resync signal time value.
 *
 * \param[in] value: Resync signal time value whose uint is 1/(2*SPI3WIRE_Speed).
 *            This parameter can be only be a value of 0x0 ~ 0xf.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SPI3WIRE_DemoCode(void)
 * {
 *     //Send resync time
 *     SPI3WIRE_SetResyncTime(2);
 *     SPI3WIRE_ResyncSignalCmd(ENABLE);
 *     while (SPI3WIRE_GetFlagStatus(SPI3WIRE_FLAG_RESYNC_BUSY) == SET);
 *     SPI3WIRE_ResyncSignalCmd(DISABLE);
 * }
 * \endcode
 */
void SPI3WIRE_SetResyncTime(uint32_t value);

/**
 * rtl876x_3wire_spi.h
 *
 * \brief   Enable or disable sending the resync signal. Must send when 3-wire SPI is disable.
 *
 * \param[in] NewState: New state of the 3-wire SPI peripheral.
 *            This parameter can be one of the following values:
 *            - ENABLE: Trigger resync signal.
 *            - DISABLE: Disable resync signal.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SPI3WIRE_DemoCode(void)
 * {
 *     SPI3WIRE_ResyncSignalCmd(ENABLE);
 *     while (SPI3WIRE_GetFlagStatus(SPI3WIRE_FLAG_RESYNC_BUSY) == SET);
 *     SPI3WIRE_ResyncSignalCmd(DISABLE);
 * }
 * \endcode
 */
void SPI3WIRE_ResyncSignalCmd(FunctionalState NewState);

/**
 * rtl876x_3wire_spi.h
 *
 * \brief   Enable or disable the specified 3-wire SPI peripheral.
 *
 * \param[in] NewState: New state of the 3-wire SPI peripheral.
 *            This parameter can be one of the following values:
 *            - ENABLE: Enable the specified 3-wire SPI peripheral.
 *            - DISABLE: Disable the specified 3-wire SPI peripheral.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void Driver_SPI3WIRE_Init(void)
 * {
 *     SPI3WIRE_Cmd(ENABLE);
 * }
 * \endcode
 */
void SPI3WIRE_Cmd(FunctionalState NewState);

/**
 * rtl876x_3wire_spi.h
 *
 * \brief   Get total number of data byte in each SPI reading.
 *
 * \return  The total number of data byte in each SPI reading.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SPI3WIRE_SingleRead(void)
 * {
 *     //wait until received data
 *     while (SPI3WIRE_GetRxDataLen() == 0);
 *     SPI3WIRE_ResyncSignalCmd(DISABLE);
 * }
 * \endcode
 */
uint8_t SPI3WIRE_GetRxDataLen(void);

/**
 * rtl876x_3wire_spi.h
 *
 * \brief   Clear read data number status.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SPI3WIRE_SingleRead(void)
 * {
 *     //Clear Receive data length
 *     SPI3WIRE_ClearRxDataLen();
 *     SPI3WIRE_StartRead(0x20, 1);
 * }
 * \endcode
 */
void SPI3WIRE_ClearRxDataLen(void);

/**
 * rtl876x_3wire_spi.h
 *
 * \brief   Clear all read data registers.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SPI3WIRE_demo(void)
 * {
 *     SPI3WIRE_ClearRxFIFO();
 * }
 * \endcode
 */
void SPI3WIRE_ClearRxFIFO(void);

/**
 * rtl876x_3wire_spi.h
 *
 * \brief   Start to write data through 3-wire SPI perpherial.
 *
 * \param[in] address: Address of register which need to write data.
 * \param[in] data: Data which need to write.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SPI3WIRE_SingleWrite(void)
 * {
 *     //Write data
 *     SPI3WIRE_StartWrite(0x06, (1 << 7));
 * }
 * \endcode
 */
void SPI3WIRE_StartWrite(uint8_t address, uint8_t data);

/**
 * rtl876x_3wire_spi.h
 *
 * \brief    Start to read data through 3-wire SPI perpherial.
 *
 * \param[in] address: Address of register which need to read.
 * \param[in] len: Number of data to read.
 *            This parameter can be only be a value of 0x1 ~ 0xf.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SPI3WIRE_SingleRead(void)
 * {
 *     //Clear Receive data length
 *     SPI3WIRE_ClearRxDataLen();
 *     SPI3WIRE_StartRead(0x20, 1);
 *
 *     //wait until received data
 *     while (SPI3WIRE_GetRxDataLen() == 0);
 *     SPI3WIRE_ResyncSignalCmd(DISABLE);
 * }
 * \endcode
 */
void SPI3WIRE_StartRead(uint8_t address, uint32_t len);

/**
 * rtl876x_3wire_spi.h
 *
 * \brief    Read data through 3-wire SPI perpherial.
 *
 * \param[out] pBuf: Buffer to store the read data.
 * \param[in] readNum: Number of data to read.
 *            This parameter can be only be a value of 0x1 ~ 0xf.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * uint8_t SPI3WIRE_SingleRead()
 * {
 *     //Read data
 *     SPI3WIRE_ReadBuf(&reg_value, 1);
 *     return reg_value;
 * }
 * \endcode
 */
void SPI3WIRE_ReadBuf(uint8_t *pBuf, uint8_t readNum);

/**
 * rtl876x_3wire_spi.h
 *
 * \brief   Get the specified 3-wire SPI flag status.
 *
 * \param[in] SPI3WIRE_FLAG: Specify the flags to check, refer to THREE_WIRE_SPI_FLAG.
 *            This parameter can be one of the following values:
 *            - SPI3WIRE_FLAG_BUSY: 3-Wire SPI busy status flag.
 *            - SPI3WIRE_FLAG_INT_IND: 3-Wire SPI interrupt status flag.
 *            - SPI3WIRE_FLAG_RESYNC_BUSY: 3-Wire SPI resync busy status flag.
 *
 * \return The new state of 3-wire SPI flag.
 * \retval SET: The specified 3-wire SPI flag status is pending.
 * \retval RESET: The specified 3-wire SPI flag status is not pending.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SPI3WIRE_SingleWrite(void)
 * {
 *     uint32_t timeout = 0;
 *
 *     //Check SPI busy or not
 *     while (SPI3WIRE_GetFlagStatus(SPI3WIRE_FLAG_BUSY) == SET)
 *     {
 *         //add user code here.
 *     }
 * }
 * \endcode
 */
FlagStatus SPI3WIRE_GetFlagStatus(uint32_t SPI3WIRE_FLAG);

/**
 * rtl876x_3wire_spi.h
 *
 * \brief   Enables or disables the specified 3-wire SPI interrupt.
 *
 * \param[in] SPI3WIRE_INT: Specifies the 3-wire SPI interrupt source to be enabled or disabled, refer to \ref THREE_WIRE_SPI_interrupt_definition.
 *            This parameter can be only be the following value:
 *            - SPI3WIRE_INT_BIT: 3-Wire SPI interrupt source.
 * \param[in]  newState: New state of the specified 3-wire SPI interrupt.
 *             This parameter can be one of the following values:
 *             - ENABLE: Enable the interrupt of 3-wire SPI.
 *             - DISABLE: Disable the interrupt of 3-wire SPI.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SPI3WIRE_init(void)
 * {
 *     SPI3WIRE_INTConfig(SPI3WIRE_INT_BIT, ENABLE);
 * }
 * \endcode
 */
void SPI3WIRE_INTConfig(uint32_t SPI3WIRE_INT, FunctionalState newState);

/**
 * rtl876x_3wire_spi.h
 *
 * \brief   Clears the 3-wire SPI interrupt pending bit.
 *
 * \param[in] SPI3WIRE_INT: Specifies the 3-wire SPI interrupt source, refer to THREE_WIRE_SPI_interrupt_definition.
 *            This parameter can be only be the following value:
 *            - SPI3WIRE_INT_BIT: 3-Wire SPI interrupt source.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SPI3WIRE_init(void)
 * {
 *     SPI3WIRE_ClearINTPendingBit(SPI3WIRE_INT_BIT);
 * }
 * \endcode
 */
void SPI3WIRE_ClearINTPendingBit(uint32_t SPI3WIRE_INT);

#ifdef __cplusplus
}
#endif

#endif /* _RTL8762_3WIRE_SPI_H_ */

/** @} */ /* End of group THREE_WIRE_SPI_Exported_Functions */
/** @} */ /* End of group 87x3d_THREE_WIRE_SPI */

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/

