
/**
*****************************************************************************************
*     Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file    rtl876x.h
  * @brief   CMSIS Cortex-M4 Peripheral Access Layer Header File for
  *          RTL876X from Realtek Semiconductor.
  * @date    2016.3.3
  * @version v1.0

 * @date     3. March 2022
 *
 * @note     Generated with SVDConv Vx.xxp
 *           from CMSIS SVD File 'RTL876X.xml' Version x.xC,
 *
 * @par      Copyright (c) 2022 Realtek Semiconductor. All Rights Reserved.
 *
 *           The information contained herein is property of Realtek Semiconductor.
 *           Terms and conditions of usage are described in detail in Realtek
 *           SEMICONDUCTOR STANDARD SOFTWARE LICENSE AGREEMENT.
 *
 *           Licensees are granted free, non-transferable use of the information. NO
 *           WARRANTY of ANY KIND is provided. This heading must NOT be removed from
 *           the file.
 *
 *

  * *************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2022 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

#ifndef RTL876X_H
#define RTL876X_H
#include "compiler_abstraction.h"
#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup 87x3d_RTL876X Rtl876x
  * @brief   CMSIS Cortex-M4 peripheral access layer header file for
  *          RTL876X from Realtek Semiconductor
  * @{
  */


/** @defgroup 87x3d_Configuration_of_CMSIS Configuration of CMSIS
  * @brief   Configuration of the cm4 Processor and Core Peripherals
  * @{
  */
/* ----------------Configuration of the cm4 Processor and Core Peripherals---------------- */
#define __CM4_REV                      0x0001U    //!< Core revision r0p1
#define __MPU_PRESENT                  1          //!< MPU present or not
#define __FPU_PRESENT                  1          //!< FPU present
#define __NVIC_PRIO_BITS               3          //!< Number of Bits used for Priority Levels
#define __Vendor_SysTickConfig         0          //!< Set to 1 if different SysTick Config is used
/** @} */ /* End of group 87x3d_Configuration_of_CMSIS */

/*============================================================================*
 *                               Header Files
*============================================================================*/
#include "irqn_def.h"
#include "core_cm4.h"
#include "io_utils.h"
#include "pin_def.h"
#include "indirect_access.h"

/* ================================================================================ */
/* ================    Peripheral Registers Structures Section     ================ */
/* ================================================================================ */

/** @defgroup 87x3d_RTL876X_Peripheral_Registers_Structures RTL876X Register Structure
  * @{
  */

/* ================================================================================ */
/* ================                      UART                      ================ */
/* ================================================================================ */

/**
  * @brief Universal Asynchronous Receiver/Transmitter, version 1.0. (UART)
  */

typedef struct                                      /*!< UART Structure                                                        */
{
    __IO  uint32_t DLL;              /*!< offset 0x00, UART Divisor Latch (LS) Register */
    __IO  uint32_t DLH_INTCR;        /*!< offset 0x04, UART Divisor Latch High Register or
                                                            Interrupt Control Register */
    __IO  uint32_t INTID_FCR;        /*!< offset 0x08, UART Interrupt Identification Register or
                                                            FIFO Control Register*/
    __IO  uint32_t LCR;              /*!< offset 0x0C, UART Line Control Register */
    __IO  uint32_t MCR;              /*!< offset 0x10, UART Modem Control Register */
    __I   uint32_t LSR;              /*!< offset 0x14, UART Line Status Register */
    __I   uint32_t MSR;              /*!< offset 0x18, UART Modem Status Register */
    __IO  uint32_t SPR;              /*!< offset 0x1C, UART Scratch Pad Register */
    __IO  uint32_t STSR;             /*!< offset 0x20, UART STS Register */
    __IO  uint32_t RB_THR;           /*!< offset 0x24, UART Receiver Buffer Register or
                                                            Transmitter Holding Register */

    __IO  uint32_t MISCR;            /*!< offset 0x28, UART MISC Control Register */
    __IO  uint32_t TXPLSR;           /*!< offset 0x2C, UART TXPLSR */
    __IO  uint32_t RSVD0;            /*!< offset 0x30, Reserved */
    __IO  uint32_t BaudMONR;         /*!< offset 0x34, Reserved */
    __IO  uint32_t RSVD1;            /*!< offset 0x38, Reserved */
    __I   uint32_t DBG_UART;         /*!< offset 0x3C, Reserved */
    __IO  uint32_t RX_IDLE_INTTCR;   /*!< offset 0x40, UART RXIDLE_TIMEOUT */
    __IO  uint32_t RX_IDLE_SR;       /*!< offset 0x44, UART_VENDORISR */
    __IO  uint32_t RXIDLE_INTCR;     /*!< offset 0x48, UART_VENDORIE */
    __I   uint32_t FIFO_LEVEL;       /*!< offset 0x4C, UART_TRXFIFO_WL */
    __IO  uint32_t REG_INT_MASK;     /*!< offset 0x50, UART Interrupt Mask Register*/
    __I   uint32_t REG_TXDONE_INT;   /*!< offset 0x54, Reserved */
    __I   uint32_t REG_TX_THD_INT;   /*!< offset 0x58, Reserved */
} UART_TypeDef;

typedef enum
{
    BAUD_RATE_1200,
    BAUD_RATE_9600,
    BAUD_RATE_14400,
    BAUD_RATE_19200,
    BAUD_RATE_28800,
    BAUD_RATE_38400,
    BAUD_RATE_57600,
    BAUD_RATE_76800,
    BAUD_RATE_115200,
    BAUD_RATE_128000,
    BAUD_RATE_153600,
    BAUD_RATE_230400,
    BAUD_RATE_460800,
    BAUD_RATE_500000,
    BAUD_RATE_921600,
    BAUD_RATE_1000000,
    BAUD_RATE_1382400,
    BAUD_RATE_1444400,
    BAUD_RATE_1500000,
    BAUD_RATE_1843200,
    BAUD_RATE_2000000,
    BAUD_RATE_3000000,
    BAUD_RATE_4000000,
    BAUD_RATE_6000000
} UartBaudRate_TypeDef;
/* ================================================================================ */
/* ================                 2WIRE_SPI                      ================ */
/* ================================================================================ */

/**
  * @brief 2wire spi, mostly used with mouse sensor. (2WIRE_SPI)
  */

typedef struct                                      /*!< 3WIRE_SPI Structure                                             */
{
    __IO  uint32_t
    RSVD0[12];                           /*!< 0x00 -- 0x2C for Q-decode                                       */
    __IO  uint32_t
    CFGR;                                 /*!< 0x30                                           */
    __IO  uint32_t CR;
    __IO  uint32_t INTCR;
    __I   uint32_t SR;
    __IO  uint32_t
    RD0;                                 /*!< 0x40                                           */
    __IO  uint32_t RD1;
    __IO  uint32_t RD2;
    __IO  uint32_t RD3;
} SPI3WIRE_TypeDef;

/* ================================================================================ */
/* ================                 IR                             ================ */
/* ================================================================================ */

/**
  * @brief IR.
  */

typedef struct                                      /*!< IR Structure  */
{
    __IO  uint32_t CLK_DIV;                 /*!< 0x00 */
    __IO  uint32_t TX_CONFIG;               /*!< 0x04 */
    __IO  uint32_t TX_SR;                   /*!< 0x08 */
    __IO  uint32_t RESERVER;                /*!< 0x0c */
    __IO  uint32_t TX_INT_CLR;              /*!< 0x10 */
    __IO  uint32_t TX_FIFO;                 /*!< 0x14 */
    __IO  uint32_t RX_CONFIG;               /*!< 0x18 */
    __IO  uint32_t RX_SR;                   /*!< 0x1c */
    __IO  uint32_t RX_INT_CLR;              /*!< 0x20 */
    __IO  uint32_t RX_CNT_INT_SEL;          /*!< 0x24 */
    __IO  uint32_t RX_FIFO;                 /*!< 0x28 */
    __IO  uint32_t IR_VERSION;              /*!< 0x2c */
    __IO  uint32_t REVD[4];
    __IO  uint32_t TX_TIME;                 /*!< 0x40 */
    __IO  uint32_t RX_TIME;                 /*!< 0x44 */
    __IO  uint32_t IR_TX_COMPE;             /*!< 0x48 */
} IR_TypeDef;

/* ================================================================================ */
/* ================                       SPI                      ================ */
/* ================================================================================ */

/**
  * @brief SPI master 0. (SPI)
  */
typedef struct                                          /*!< SPI Structure  */
{
    __IO  uint32_t CTRLR0;                /*!< 0x00, Control Register 0 */
    __IO  uint32_t CTRLR1;                /*!< 0x04, Control Register 1 */
    __IO  uint32_t SSIENR;                /*!< 0x08, SSI Enable Register */
    __IO  uint32_t    RSVD_0C;            /*!< 0x0C, Microwire Control Register */
    __IO  uint32_t SER;                   /*!< 0x10, Slave Enable Register */
    __IO  uint32_t BAUDR;                 /*!< 0x14, Baud Rate Select */
    __IO  uint32_t TXFTLR;                /*!< 0x18, Transmit FIFO Threshold Level */
    __IO  uint32_t RXFTLR;                /*!< 0x1C, Receive FIFO Threshold Level */
    __I   uint32_t TXFLR;                 /*!< 0x20, Transmit FIFO Level Register */
    __I   uint32_t RXFLR;                 /*!< 0x24, Receive FIFO Level Register */
    __I   uint32_t SR;                    /*!< 0x28, Status Register */
    __IO  uint32_t IMR;                   /*!< 0x2C, Interrupt Mask Register */
    __I   uint32_t ISR;                   /*!< 0x30, Interrupt Status Register */
    __I   uint32_t RISR;                  /*!< 0x34, Raw Interrupt Status Register */
    __I   uint32_t TXOICR;                /*!< 0x38, Transmit FIFO Overflow Interrupt Clear Register */
    __I   uint32_t RXOICR;                /*!< 0x3C, Receive FIFO Overflow Interrupt Clear Register */
    __I   uint32_t RXUICR;                /*!< 0x40, Receive FIFO Underflow Interrupt Clear Register */
    __I  uint32_t  FAEICR;                /*!< 0x44, Multi-Master Interrupt Clear Register */
    __I   uint32_t ICR;                   /*!< 0x48, Interrupt Clear Register */
    __IO  uint32_t DMACR;                 /*!< 0x4C, DMA Control Register */
    __IO  uint32_t DMATDLR;               /*!< 0x50, DMA Transmit Data Level */
    __IO  uint32_t DMARDLR;               /*!< 0x54, DMA Receive Data Level */
    __I   uint32_t TXUICR;                /*!< 0x58, Identification Register */
    __I   uint32_t SSRICR;                /*!< 0x5C, coreKit version ID register */
    __IO  uint32_t DR[36];                /*!< 0x60 - 0xEC, Data Register */
    __IO  uint32_t RX_SAMPLE_DLY;         /*!< 0xF0, Rx Sample Delay Register*/
} SPI_TypeDef;

/* ================================================================================ */
/* ================                      SD host Interface                    ================= */
/* ================================================================================ */

typedef struct
{
    __IO uint32_t SDMA_SYS_ADDR;      /*!< 0x00 */
    __IO uint16_t BLK_SIZE;           /*!< 0x04 */
    __IO uint16_t BLK_CNT;            /*!< 0x06 */
    __IO uint32_t ARG;                /*!< 0x08 */
    __IO uint16_t TF_MODE;            /*!< 0x0C */
    __IO uint16_t CMD;                /*!< 0x0E */
    __I  uint32_t RSP0;               /*!< 0x10 */
    __I  uint32_t RSP2;               /*!< 0x14 */
    __I  uint32_t RSP4;               /*!< 0x18 */
    __I  uint32_t RSP6;               /*!< 0x1C */
    __IO uint32_t BUF_DATA_PORT;      /*!< 0x20 */
    __I  uint32_t PRESENT_STATE;      /*!< 0x24 */
    __IO uint8_t  HOST_CTRL;          /*!< 0x28 */
    __IO uint8_t  PWR_CTRL;           /*!< 0x29 */
    __IO uint8_t  BLK_GAP_CTRL;       /*!< 0x2A */
    __IO uint8_t  WAKEUP_CTRL;        /*!< 0x2B */
    __IO uint16_t CLK_CTRL;           /*!< 0x2C */
    __IO uint8_t  TIMEOUT_CTRL;       /*!< 0x2E */
    __IO uint8_t  SW_RESET;           /*!< 0x2F */
    __IO uint16_t NORMAL_INTR_SR;     /*!< 0x30 */
    __IO uint16_t ERR_INTR_SR;        /*!< 0x32 */
    __IO uint16_t NORMAL_INTR_SR_EN;  /*!< 0x34 */
    __IO uint16_t ERR_INTR_SR_EN;     /*!< 0x36 */
    __IO uint16_t NORMAL_INTR_SIG_EN; /*!< 0x38 */
    __IO uint16_t ERR_INTR_SIG_EN;    /*!< 0x3A */
    __I  uint16_t CMD12_ERR_SR;       /*!< 0x3C */
    __I  uint16_t RSVD0;              /*!< 0x3E */
    __IO uint32_t CAPABILITIES_L;     /*!< 0x40 */
    __IO uint32_t CAPABILITIES_H;     /*!< 0x44 */
    __I  uint32_t RSVD1[4];           /*!< 0x48 */
    __IO uint32_t ADMA_SYS_ADDR_L;    /*!< 0x58 */
    __IO uint32_t ADMA_SYS_ADDR_H;    /*!< 0x5C */
    __I  uint32_t RSVD2[1000];        /*!< 0x60 */
    __IO uint8_t  CAPABILITY_CTRL;    /*!< 0x1000 */
    __IO uint8_t  SYSTEM_CTRL;        /*!< 0x1001 */
    __IO uint8_t  LOOPBACK_CTRL;      /*!< 0x1002 */
    __I  uint8_t  DEBUG_INFO;         /*!< 0x1003 */
    __I  uint8_t  DEBUG_INFO2;        /*!< 0x1004 */
    __I  uint8_t  RSVD3[3];           /*!< 0x1005 */
    __IO uint32_t DMA_CTRL;           /*!< 0x1008 */
    __I  uint32_t RSVD4;              /*!< 0x100C */
    __IO uint32_t BUS_DLY_SEL;        /*!< 0x1010 */
} SDIO_TypeDef;

/* ================================================================================ */
/* ================                      I2C                      ================= */
/* ================================================================================ */

/**
  * @brief I2C
  */
typedef struct
{
    __IO uint32_t IC_CON;                 /*!< offset 0x00, I2C Control Register */
    __IO uint32_t IC_TAR;                 /*!< offset 0x04, I2C Target Address Register */
    __IO uint32_t IC_SAR;                 /*!< offset 0x08, I2C Slave Address Register */
    __IO uint32_t IC_HS_MADDR;            /*!< offset 0x0C, I2C HS Master Mode Code Address */
    __IO uint32_t IC_DATA_CMD;            /*!< offset 0x10, I2C Rx/Tx Data Buffer and
                                                            Command Register */
    __IO uint32_t IC_SS_SCL_HCNT;         /*!< offset 0x14, Standard speed I2C Clock
                                                            SCL High Count */
    __IO uint32_t IC_SS_SCL_LCNT;         /*!< offset 0x18, Standard speed I2C Clock
                                                            SCL Low Count */
    __IO uint32_t IC_FS_SCL_HCNT;         /*!< offset 0x1C, Fast speed I2C Clock SCL High Count */
    __IO uint32_t IC_FS_SCL_LCNT;         /*!< offset 0x20, Fast speed I2C Clock SCL Low Count */
    __IO uint32_t IC_HS_SCL_HCNT;         /*!< offset 0x24, High speed I2C Clock SCL High Count */
    __IO uint32_t IC_HS_SCL_LCNT;         /*!< offset 0x28, High speed I2C Clock SCL Low Count */
    __I  uint32_t IC_INTR_STAT;           /*!< offset 0x2C, I2C Interrupt Status Register */
    __IO uint32_t IC_INTR_MASK;           /*!< offset 0x30, I2C Interrupt Mask Register  */
    __I  uint32_t IC_RAW_INTR_STAT;       /*!< offset 0x34, I2C Raw Interrupt Status Register */
    __IO uint32_t IC_RX_TL;               /*!< offset 0x38, I2C Receive FIFO Threshold Level */
    __IO uint32_t IC_TX_TL;               /*!< offset 0x3C, I2C Transmit FIFO Threshold Register */
    __I  uint32_t IC_CLR_INTR;            /*!< offset 0x40, I2C clear interrupt status */
    __I  uint32_t IC_CLR_RX_UNDER;        /*!< offset 0x44, IC_CLR_RX_UNDER */
    __I  uint32_t IC_CLR_RX_OVER;         /*!< offset 0x48, IC_CLR_RX_OVER */
    __I  uint32_t IC_CLR_TX_OVER;         /*!< offset 0x4C, IC_CLR_TX_OVER */
    __I  uint32_t IC_CLR_RD_REQ;          /*!< offset 0x50, IC_CLR_RD_REQ */
    __I  uint32_t IC_CLR_TX_ABRT;         /*!< offset 0x54, IC_CLR_TX_ABRT */
    __I  uint32_t IC_CLR_RX_DONE;         /*!< offset 0x58, IC_CLR_RX_DONE */
    __I  uint32_t IC_CLR_ACTIVITY;        /*!< offset 0x5C, IC_CLR_ACTIVITY */
    __I  uint32_t IC_CLR_STOP_DET;        /*!< offset 0x60, IC_CLR_STOP_DET */
    __I  uint32_t IC_CLR_START_DET;       /*!< offset 0x64, IC_CLR_START_DET */
    __I  uint32_t IC_CLR_GEN_CALL;        /*!< offset 0x68, IC_CLR_GEN_CALL */
    __IO uint32_t IC_ENABLE;              /*!< offset 0x6C, IC_ENABLE */
    __I  uint32_t IC_STATUS;              /*!< offset 0x70, I2C Status Register */
    __I  uint32_t IC_TXFLR;               /*!< offset 0x74, I2C Transmit FIFO Level Register */
    __I  uint32_t IC_RXFLR;               /*!< offset 0x78, I2C Receive FIFO Level Register  */
    __IO uint32_t IC_SDA_HOLD;            /*!< offset 0x7C, I2C SDA Hold Time Length Register */
    __I  uint32_t IC_TX_ABRT_SOURCE;      /*!< offset 0x80, I2C Transmit Abort Source Register  */
    __IO uint32_t IC_SLV_DATA_NACK_ONLY;  /*!< offset 0x84, Generate Slave Data NACK Register */
    __IO uint32_t IC_DMA_CR;              /*!< offset 0x88, DMA Control Register */
    __IO uint32_t IC_DMA_TDLR;            /*!< offset 0x8C, DMA Transmit Data Level Register */
    __IO uint32_t IC_DMA_RDLR;            /*!< offset 0x90, DMA Receive Data Level Register */
    __IO uint32_t IC_SDA_SETUP;           /*!< offset 0x94, I2C SDA Setup Register */
    __IO uint32_t IC_ACK_GENERAL_CALL;    /*!< offset 0x98, I2C ACK General Call Register */
    __IO uint32_t IC_ENABLE_STATUS;       /*!< offset 0x9C, I2C Enable Status Register */
    __IO uint32_t IC_FS_SPKLEN;           /*!< offset 0xA0, I2C SS and FS Spike Suppression
                                                            Limit Register */
} I2C_TypeDef;

/* ================================================================================ */
/* ================                       ADC                      ================ */
/* ================================================================================ */

/**
  * @brief Analog to digital converter. (ADC)
  */

typedef struct
{
    __O  uint32_t FIFO;                   /*!< offset 0x00 */
    __IO uint32_t CR;                     /*!< offset 0x04 */
    __IO uint32_t SCHCR;                  /*!< offset 0x08 */
    __IO uint32_t INTCR;                  /*!< offset 0x0C */
    __IO uint32_t SCHTAB0;                /*!< offset 0x10 */
    __IO uint32_t SCHTAB1;                /*!< offset 0x14 */
    __IO uint32_t SCHTAB2;                /*!< offset 0x18 */
    __IO uint32_t SCHTAB3;                /*!< offset 0x1C */
    __IO uint32_t SCHTAB4;                /*!< offset 0x20 */
    __IO uint32_t SCHTAB5;                /*!< offset 0x24 */
    __IO uint32_t SCHTAB6;                /*!< offset 0x28 */
    __IO uint32_t SCHTAB7;                /*!< offset 0x2C */
    __IO uint32_t SCHD0;                  /*!< offset 0x30 */
    __IO uint32_t SCHD1;                  /*!< offset 0x34 */
    __IO uint32_t SCHD2;                  /*!< offset 0x38 */
    __IO uint32_t SCHD3;                  /*!< offset 0x3C */
    __IO uint32_t SCHD4;                  /*!< offset 0x40 */
    __IO uint32_t SCHD5;                  /*!< offset 0x44 */
    __IO uint32_t SCHD6;                  /*!< offset 0x48 */
    __IO uint32_t SCHD7;                  /*!< offset 0x4C */
    __IO uint32_t PWRDLY;                 /*!< offset 0x50 */
    __IO uint32_t DATCLK;                 /*!< offset 0x54 */
    __IO uint32_t ANACTL;                 /*!< offset 0x58 */
    __IO uint32_t REG_5C_CLK;             /*!< offset 0x5C */
    __IO uint32_t RTLVER;                 /*!< offset 0x60 */
} ADC_TypeDef;

/* ================================================================================ */
/* ================                      TIM                      ================ */
/* ================================================================================ */

/**
  * @brief TIM
  */
typedef struct
{
    __IO uint32_t LoadCount;              /*!< 0x00, TimerN Load Count Register */
    __I  uint32_t CurrentValue;           /*!< 0x04, TimerN Current Value Register */
    __IO uint32_t ControlReg;             /*!< 0x08, TimerN Control Register */
    __I  uint32_t EOI;                    /*!< 0x0C, TimerN End-of-Interrupt Register */
    __I  uint32_t IntStatus;              /*!< 0x10, TimerN Interrupt Status Register */
} TIM_TypeDef;

typedef struct
{
    __I uint32_t TimersIntStatus;        /*!< 0xa0, Timers Interrupt Status Register */
    __I uint32_t TimersEOI;              /*!< 0xa4, Timers End-of-Interrupt Register */
    __I uint32_t TimersRawIntStatus;     /*!< 0xa8, Timers Raw Interrupt Status Register */
} TIM_ChannelsTypeDef;

/* ================================================================================ */
/* ================                      GDMA                      ================ */
/* ================================================================================ */
/**
  * @brief GDMA interrupt registers
  */
typedef struct
{
    __I  uint32_t RAW_TFR;                /*!< 0x2C0*/
    __I  uint32_t RSVD0;
    __I  uint32_t RAW_BLOCK;              /*!< 0x2C8*/
    __I  uint32_t RSVD1;
    __I  uint32_t RAW_SRC_TRAN;           /*!< 0x2D0*/
    __I  uint32_t RSVD2;
    __I  uint32_t RAW_DST_TRAN;           /*!< 0x2D8*/
    __I  uint32_t RSVD3;
    __I  uint32_t RAW_ERR;                /*!< 0x2E0*/
    __I  uint32_t RSVD4;

    __I  uint32_t STATUS_TFR;             /*!< 0x2E8*/
    __I  uint32_t RSVD5;
    __I  uint32_t STATUS_BLOCK;           /*!< 0x2F0*/
    __I  uint32_t RSVD6;
    __I  uint32_t STATUS_SRC_TRAN;        /*!< 0x2F8*/
    __I  uint32_t RSVD7;
    __I  uint32_t STATUS_DST_TRAN;        /*!< 0x300*/
    __I  uint32_t RSVD8;
    __I  uint32_t STATUS_ERR;             /*!< 0x308*/
    __I  uint32_t RSVD9;

    __IO uint32_t MASK_TFR;               /*!< 0x310*/
    __I  uint32_t RSVD10;
    __IO uint32_t MASK_BLOCK;             /*!< 0x318*/
    __I  uint32_t RSVD11;
    __IO uint32_t MASK_SRC_TRAN;          /*!< 0x320*/
    __I  uint32_t RSVD12;
    __IO uint32_t MASK_DST_TRAN;          /*!< 0x328*/
    __I  uint32_t RSVD13;
    __IO uint32_t MASK_ERR;               /*!< 0x330*/
    __I  uint32_t RSVD14;

    __O  uint32_t CLEAR_TFR;              /*!< 0x338*/
    __I  uint32_t RSVD15;
    __O  uint32_t CLEAR_BLOCK;            /*!< 0x340*/
    __I  uint32_t RSVD16;
    __O  uint32_t CLEAR_SRC_TRAN;         /*!< 0x348*/
    __I  uint32_t RSVD17;
    __O  uint32_t CLEAR_DST_TRAN;         /*!< 0x350*/
    __I  uint32_t RSVD18;
    __O  uint32_t CLEAR_ERR;              /*!< 0x358*/
    __I  uint32_t RSVD19;
    __O  uint32_t StatusInt;              /*!< 0x360, reserved in RTK DMAC IP*/
    __I  uint32_t RSVD191;

    __IO uint32_t ReqSrcReg;              /*!< 0x368, reserved in RTK DMAC IP*/
    __I  uint32_t RSVD20;
    __IO uint32_t ReqDstReg;              /*!< 0x370, reserved in RTK DMAC IP*/
    __I  uint32_t RSVD21;
    __IO uint32_t SglReqSrcReg;           /*!< 0x378, reserved in RTK DMAC IP*/
    __I  uint32_t RSVD22;
    __IO uint32_t SglReqDstReg;           /*!< 0x380, reserved in RTK DMAC IP*/
    __I  uint32_t RSVD23;
    __IO uint32_t LstSrcReg;              /*!< 0x388, reserved in RTK DMAC IP*/
    __I  uint32_t RSVD24;
    __IO uint32_t LstDstReg;              /*!< 0x390, reserved in RTK DMAC IP*/
    __I  uint32_t RSVD25;

    __IO uint32_t DmaCfgReg;              /*!< 0x398 */
    __I  uint32_t RSVD26;
    __IO uint32_t ChEnReg;                /*!< 0x3A0 */
    __I  uint32_t RSVD27;
    __I  uint32_t DmaIdReg;               /*!< 0x3A8 */
    __I  uint32_t RSVD28;
    __IO uint32_t DmaTestReg;             /*!< 0x3B0 */
    __IO uint32_t DmaOsNum;               /*!< 0x3B8 */
    __I  uint32_t RSVD30;
} GDMA_TypeDef;

typedef struct
{
    __IO uint32_t SAR;                     /*!< 0x00 */
    __I  uint32_t RSVD0;
    __IO uint32_t DAR;                     /*!< 0x08 */
    __I  uint32_t RSVD1;
    __IO uint32_t LLP;                     /*!< 0x10 */
    __I  uint32_t RSVD2;
    __IO uint32_t CTL_LOW;                 /*!< 0x18 */
    __IO uint32_t CTL_HIGH;                /*!< 0x1C */
    __IO uint32_t SSTAT;                   /*!< 0x20 */
    __I  uint32_t RSVD4;
    __IO uint32_t DSTAT;                   /*!< 0x28 */
    __I  uint32_t RSVD5;
    __IO uint32_t SSTATAR;                 /*!< 0x30 */
    __I  uint32_t RSVD6;
    __IO uint32_t DSTATAR;                 /*!< 0x38 */
    __I  uint32_t RSVD7;
    __IO uint32_t CFG_LOW;                 /*!< 0x40 */
    __IO uint32_t CFG_HIGH;
    __IO uint32_t SGR_LOW;                 /*!< 0x48 */
    __IO uint32_t SGR_HIGH;
    __IO uint32_t DSR_LOW;                 /*!< 0x50 */
    __IO uint32_t DSR_HIGH;
} GDMA_ChannelTypeDef;

/** @defgroup GDMA_CH_NUM GDMA channel number defination.
  * @{
  */

#define GDMA0_CH_NUM0                       0   //!< The number of GDMA_0 channel 0.
#define GDMA0_CH_NUM1                       1   //!< The number of GDMA_0 channel 1.
#define GDMA0_CH_NUM2                       2   //!< The number of GDMA_0 channel 2.
#define GDMA0_CH_NUM3                       3   //!< The number of GDMA_0 channel 3.
#define GDMA0_CH_NUM4                       4   //!< The number of GDMA_0 channel 4.
#define GDMA0_CH_NUM5                       5   //!< The number of GDMA_0 channel 5.
#define GDMA0_CH_NUM6                       6   //!< The number of GDMA_0 channel 6.
#define GDMA0_CH_NUM7                       7   //!< The number of GDMA_0 channel 7.
#define GDMA0_CH_NUM8                       8   //!< The number of GDMA_0 channel 8.
#define GDMA0_CH_NUM9                       9   //!< The number of GDMA_0 channel 9.
#define GDMA0_CH_NUM10                      10  //!< The number of GDMA_0 channel 10.
#define GDMA0_CH_NUM11                      11  //!< The number of GDMA_0 channel 11.
#define GDMA0_CH_NUM12                      12  //!< The number of GDMA_0 channel 12.
#define GDMA0_CH_NUM13                      13  //!< The number of GDMA_0 channel 13.
#define GDMA0_CH_NUM14                      14  //!< The number of GDMA_0 channel 14.
#define GDMA0_CH_NUM15                      15  //!< The number of GDMA_0 channel 15.

#define GDMA1_CH_NUM0                       16  //!< The number of GDMA_1 channel 0.
#define GDMA1_CH_NUM1                       17  //!< The number of GDMA_1 channel 1.
#define GDMA1_CH_NUM2                       18  //!< The number of GDMA_1 channel 2.
#define GDMA1_CH_NUM3                       19  //!< The number of GDMA_1 channel 3.
#define GDMA1_CH_NUM4                       20  //!< The number of GDMA_1 channel 4.
#define GDMA1_CH_NUM5                       21  //!< The number of GDMA_1 channel 5.
#define GDMA1_CH_NUM6                       22  //!< The number of GDMA_1 channel 6.
#define GDMA1_CH_NUM7                       23  //!< The number of GDMA_1 channel 7.

/** End of group GDMA_Declaration
  * @}
  */

/* ================================================================================ */
/* ================                       RTC                      ================ */
/* ================================================================================ */

/**
  * @brief Real time counter 0. (RTC)
  */

typedef struct
{
    __IO uint32_t CR0;                    /*!< 0x00  */
    __IO uint32_t INT_MASK;               /*!< 0x04  */
    __IO uint32_t INT_SR;                 /*!< 0x08  */
    __IO uint32_t PRESCALER;              /*!< 0x0C  */
    __IO uint32_t COMP0;                  /*!< 0x10  */
    __IO uint32_t COMP1;                  /*!< 0x14  */
    __IO uint32_t COMP2;                  /*!< 0x18  */
    __IO uint32_t COMP3;                  /*!< 0x1C  */
    __IO uint32_t COMP0GT;                /*!< 0x20  */
    __IO uint32_t COMP1GT;                /*!< 0x24  */
    __IO uint32_t COMP2GT;                /*!< 0x28  */
    __IO uint32_t COMP3GT;                /*!< 0x2C  */
    __I  uint32_t CNT;                    /*!< 0x30  */
    __I  uint32_t PRESCALE_CNT;           /*!< 0x34  */
    __IO uint32_t PRESCALE_CMP;           /*!< 0x38  */
    __IO uint32_t BACKUP;                 /*!< 0x3C  */
} RTC_TypeDef;

typedef struct
{
    __IO uint32_t CR0;                    /*!< 0x00 */
    __IO uint32_t COMP0;                  /*!< 0x04 */
    __IO uint32_t COMP1;                  /*!< 0x08 */
    __IO uint32_t COMP0GT;                /*!< 0x0C */
    __IO uint32_t COMP1GT;                /*!< 0x10 */
    __IO uint32_t BACKUP;                 /*!< 0x14 */
    __I  uint32_t CNT;                    /*!< 0x18 */
    __IO uint32_t INT_CLR;                /*!< 0x1C */
    __IO uint32_t INT_SR;                 /*!< 0x20 */
} PF_RTC_TypeDef;

/**
  * @brief Real time for lp (RTC)
  */
typedef struct
{
    __IO uint32_t LPC_CR0;
    __IO uint32_t LPC_SR;
    __IO uint32_t LPC_CMP_LOAD;
    __IO uint32_t LPC_CMP_CNT;
    __IO uint32_t LPC_SR_IN_SLEEP_MODE;
} LPC_TypeDef;

/**
  * @brief SLEEP MODE LED (RTC)
  */

typedef struct
{
    __IO uint32_t S_LED_CR;         //!<0x00
    __IO uint32_t S_LED_CH0_CR0;    //!<0x04
    __IO uint32_t S_LED_CH0_CR1;    //!<0x08
    __IO uint32_t S_LED_CH0_P1_CR;  //!<0x0C
    __IO uint32_t S_LED_CH0_P2_CR;  //!<0x10
    __IO uint32_t S_LED_CH0_P3_CR;  //!<0x14
    __IO uint32_t S_LED_CH0_P4_CR;  //!<0x18
    __IO uint32_t S_LED_CH0_P5_CR;  //!<0x1C
    __IO uint32_t S_LED_CH0_P6_CR;  //!<0x20
    __IO uint32_t S_LED_CH0_P7_CR;  //!<0x24
    __IO uint32_t S_LED_CH0_P8_CR;  //!<0x28

    __IO uint32_t RSV0;             //!<0x2C

    __IO uint32_t S_LED_CH1_CR0;    //!<0x30
    __IO uint32_t S_LED_CH1_CR1;    //!<0x34
    __IO uint32_t S_LED_CH1_P1_CR;  //!<0x38
    __IO uint32_t S_LED_CH1_P2_CR;  //!<0x3C
    __IO uint32_t S_LED_CH1_P3_CR;  //!<0x40
    __IO uint32_t S_LED_CH1_P4_CR;  //!<0x44
    __IO uint32_t S_LED_CH1_P5_CR;  //!<0x48
    __IO uint32_t S_LED_CH1_P6_CR;  //!<0x4C
    __IO uint32_t S_LED_CH1_P7_CR;  //!<0x50
    __IO uint32_t S_LED_CH1_P8_CR;  //!<0x54

    __IO uint32_t RSV1;             //!<0x58
    __IO uint32_t RSV2;             //!<0x5C

    __IO uint32_t S_LED_CH2_CR0;    //!<0x60
    __IO uint32_t S_LED_CH2_CR1;    //!<0x64
    __IO uint32_t S_LED_CH2_P1_CR;  //!<0x68
    __IO uint32_t S_LED_CH2_P2_CR;  //!<0x6C
    __IO uint32_t S_LED_CH2_P3_CR;  //!<0x70
    __IO uint32_t S_LED_CH2_P4_CR;  //!<0x74
    __IO uint32_t S_LED_CH2_P5_CR;  //!<0x78
    __IO uint32_t S_LED_CH2_P6_CR;  //!<0x7C
    __IO uint32_t S_LED_CH2_P7_CR;  //!<0x80
    __IO uint32_t S_LED_CH2_P8_CR;  //!<0x84
} RTC_LED_TypeDef;

/* ================================================================================ */
/* ================                      QDEC                      ================ */
/* ================================================================================ */

/**
  * @brief Rotary decoder. (QDEC)
  */

typedef struct                                      /*!< QDEC Structure           */
{
    __IO uint32_t   REG_DIV;                       /*!< 0x00 */
    __IO uint32_t   REG_CR_X;                      /*!< 0x04 */
    __IO uint32_t   REG_SR_X;                      /*!< 0x08 */
    __IO uint32_t   REG_CR_Y;                      /*!< 0x0C */
    __IO uint32_t   REG_SR_Y;                      /*!< 0x10 */
    __IO uint32_t   REG_CR_Z;                      /*!< 0x14 */
    __IO uint32_t   REG_SR_Z;                      /*!< 0x18 */
    __IO uint32_t   INT_MASK;                      /*!< 0x1C, Reserved */
    __IO uint32_t   INT_SR;                        /*!< 0x20 */
    __IO uint32_t   INT_CLR;                       /*!< 0x24 */
    __IO uint32_t   REG_DBG;                       /*!< 0x28 */
    __IO uint32_t   REG_VERSION;                   /*!< 0x2C  */
} QDEC_TypeDef;

/* ================================================================================ */
/* ================                     Watch Dog                     ================ */
/* ================================================================================ */

/**
  * @brief Watch Dog Structure. (WDG)
  */

typedef struct
{
    __IO uint32_t WDG_CTL;                          /**< 0x00  */
}   WDG_TypeDef;

/* ================================================================================ */
/* ================            System Block Control            ================ */
/* ================================================================================ */

/**
  * @brief System Block Control. (SYS_BLKCTRL)
  */

typedef struct
{
    /* 0x0200       0x4000_0200
        0       R/W    r_cpu_slow_en                   1'h0
        1       R/W    r_cpu_slow_opt_wfi              1'h0
        2       R/W    r_cpu_slow_opt_dsp              1'h0
        3       R/W    r_dsp_slow_en                   1'h0
        4       R/W    r_dsp_slow_opt_dsp              1'h0
        5       R/W    r_bus2dspram_fast_clk_en        1'h1
        6       R/W    r_clk_cpu_f1m_en                1'h0
        7       R/W    r_clk_cpu_32k_en                1'h0
        8       R/W    r_aon_rd_opt                    1'h0
        9       R/W    r_pll_clk_src_sel               1'h0
        10      R/W    r_dsp_auto_slow_filter_en       1'h1
        18:11   R/W    r_bus2dspram_ext_num            8'h3
        25:19   R/W    r_bt_ahb_wait_cnt               7'hc
        26      R/W    r_cpu_slow_opt_at_tx            1'h1
        27      R/W    r_cpu_slow_opt_at_rx            1'h1
        31:28   R/W    r_cpu_low_rate_valid_num        4'h3
    */
    union
    {
        __IO uint32_t REG_PERION_REG_SYS_CLK_SEL;
        struct
        {
            __IO uint32_t r_cpu_slow_en: 1;
            __IO uint32_t r_cpu_slow_opt_wfi: 1;
            __IO uint32_t r_cpu_slow_opt_dsp: 1;
            __IO uint32_t r_dsp_slow_en: 1;
            __IO uint32_t r_dsp_slow_opt_dsp: 1;
            __IO uint32_t r_bus2dspram_fast_clk_en: 1;
            __IO uint32_t r_clk_cpu_f1m_en: 1;
            __IO uint32_t r_clk_cpu_32k_en: 1;
            __IO uint32_t r_aon_rd_opt: 1;
            __IO uint32_t r_dsp_pll_src_sel1: 1;
            __IO uint32_t r_dsp_auto_slow_filter_en: 1;
            __IO uint32_t r_bus2dspram_ext_num: 8;
            __IO uint32_t r_bt_ahb_wait_cnt: 7;
            __IO uint32_t r_cpu_slow_opt_at_tx: 1;
            __IO uint32_t r_cpu_slow_opt_at_rx: 1;
            __IO uint32_t r_cpu_low_rate_valid_num: 4;
        } BITS_200;
    } u_200;

    /* 0x0204       0x4000_0204
        3:0     R      RF_RL_ID                        4'h0
        7:4     R      RF_RTL_ID                       4'h0
        8       R/W    r_flash3_pll_src_sel            1'h0
        9       R/W    r_btaon_acc_no_block            1'h0
        10      R/W    r_psram_pll_src_sel             1'h0
        11      R/W    r_flash2_pll_src_sel            1'h0
        12      R/W    r_flash1_pll_src_sel            1'h0
        13      R/W    r_flash_pll_src_sel             1'h0
        14      R/W    r_dsp_pll_src_sel               1'h0
        15      R/W    bzdma_autoslow_eco_disable      1'h0
        16      R/W    r_cpu_auto_slow_opt2            1'h0
        17      R/W    r_cpu_auto_slow_opt1            1'h0
        18      R/W    DUMMY                           1'h0
        19      R/W    r_cpu_slow_opt_dma              1'h0
        20      R/W    r_cpu_slow_opt_sdio             1'h0
        21      R/W    r_cpu_slow_opt_usb              1'h0
        22      R/W    r_cpu_slow_opt_bt_sram_1        1'h0
        23      R/W    r_cpu_slow_opt_bt_sram_2        1'h0
        24      R/W    r_dsp_slow_opt_dspram_wbuf      1'h0
        25      R/W    r_cpu_slow_opt_dspram_wbuf      1'h0
        26      R/W    r_dsp_slow_opt_at_tx            1'h1
        27      R/W    r_dsp_slow_opt_at_rx            1'h1
        31:28   R/W    r_dsp_low_rate_valid_num        4'h3
    */
    union
    {
        __IO uint32_t REG_PERION_REG_DSP_CLK_MISC;
        struct
        {
            __I uint32_t RF_RL_ID: 4;
            __I uint32_t RF_RTL_ID: 4;
            __IO uint32_t r_flash3_pll_src_sel: 1;
            __IO uint32_t r_btaon_acc_no_block: 1;
            __IO uint32_t r_psram_pll_src_sel: 1;
            __IO uint32_t r_flash2_pll_src_sel: 1;
            __IO uint32_t r_flash1_pll_src_sel: 1;
            __IO uint32_t r_flash_pll_src_sel: 1;
            __IO uint32_t r_dsp_pll_src_sel: 1;
            __IO uint32_t bzdma_autoslow_eco_disable: 1;
            __IO uint32_t r_cpu_auto_slow_opt2: 1;
            __IO uint32_t r_cpu_auto_slow_opt1: 1;
            __IO uint32_t RESERVED_0: 1;
            __IO uint32_t r_cpu_slow_opt_dma: 1;
            __IO uint32_t r_cpu_slow_opt_sdio: 1;
            __IO uint32_t r_cpu_slow_opt_usb: 1;
            __IO uint32_t r_cpu_slow_opt_bt_sram_1: 1;
            __IO uint32_t r_cpu_slow_opt_bt_sram_2: 1;
            __IO uint32_t r_dsp_slow_opt_dspram_wbuf: 1;
            __IO uint32_t r_cpu_slow_opt_dspram_wbuf: 1;
            __IO uint32_t r_dsp_slow_opt_at_tx: 1;
            __IO uint32_t r_dsp_slow_opt_at_rx: 1;
            __IO uint32_t r_dsp_low_rate_valid_num: 4;
        } BITS_204;
    } u_204;

    /* 0x0208       0x4000_0208
        3:0     R/W    r_cpu_div_sel                   4'h4
        7:4     R/W    r_cpu_div_sel_slow              4'h4
        8       R/W    r_cpu_div_en                    1'h1
        9       R/W    r_CPU_CLK_SRC_EN                1'h1
        10      R/W    r_cpu_auto_slow_filter_en       1'h1
        11      R/W    r_cpu_slow_opt_spi2             1'h0
        12      R/W    r_cpu_pll_clk_cg_en             1'h1
        13      R/W    r_cpu_xtal_clk_cg_en            1'h1
        14      R/W    r_cpu_osc40_clk_cg_en           1'h1
        15      R/W    r_cpu_div_en_slow               1'h1
        19:16   R/W    r_dsp_div_sel                   4'h0
        23:20   R/W    r_dsp_div_sel_slow              4'h0
        24      R/W    r_dsp_div_en                    1'h0
        25      R/W    r_DSP_CLK_SRC_EN                1'h1
        26      R/W    r_dsp_clk_src_sel_1             1'h1
        27      R/W    r_dsp_clk_src_sel_0             1'h0
        28      R/W    r_dsp_pll_clk_cg_en             1'h0
        29      R/W    r_dsp_xtal_clk_cg_en            1'h0
        30      R/W    r_dsp_osc40_clk_cg_en           1'h0
        31      R/W    r_dsp_div_en_slow               1'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_SYS_CLK_SEL_1;
        struct
        {
            __IO uint32_t r_cpu_div_sel: 4;
            __IO uint32_t r_cpu_div_sel_slow: 4;
            __IO uint32_t r_cpu_div_en: 1;
            __IO uint32_t r_CPU_CLK_SRC_EN: 1;
            __IO uint32_t r_cpu_auto_slow_filter_en: 1;
            __IO uint32_t r_cpu_slow_opt_spi2: 1;
            __IO uint32_t r_cpu_pll_clk_cg_en: 1;
            __IO uint32_t r_cpu_xtal_clk_cg_en: 1;
            __IO uint32_t r_cpu_osc40_clk_cg_en: 1;
            __IO uint32_t r_cpu_div_en_slow: 1;
            __IO uint32_t r_dsp_div_sel: 4;
            __IO uint32_t r_dsp_div_sel_slow: 4;
            __IO uint32_t r_dsp_div_en: 1;
            __IO uint32_t r_DSP_CLK_SRC_EN: 1;
            __IO uint32_t r_dsp_clk_src_sel_1: 1;
            __IO uint32_t r_dsp_clk_src_sel_0: 1;
            __IO uint32_t r_dsp_pll_clk_cg_en: 1;
            __IO uint32_t r_dsp_xtal_clk_cg_en: 1;
            __IO uint32_t r_dsp_osc40_clk_cg_en: 1;
            __IO uint32_t r_dsp_div_en_slow: 1;
        } BITS_208;
    } u_208;

    /* 0x020C       0x4000_020c
        3:0     R/W    r_flash_div_sel                 4'h0
        4       R/W    r_flash_div_en                  1'h0
        5       R/W    r_FLASH_CLK_SRC_EN              1'h1
        6       R/W    r_flash_clk_src_sel_1           1'h0
        7       R/W    r_flash_clk_src_sel_0           1'h0
        8       R/W    r_flash_mux_1_clk_cg_en         1'h1
        9       R/W    r_rng_sfosc_sel                 1'h0
        12:10   R/W    r_rng_sfosc_div_sel             3'h0
        15:13   R/W    DUMMY                           3'h0
        18:16   R/W    r_40m_div_sel                   3'h0
        19      R/W    DUMMY                           1'h0
        20      R/W    r_40m_div_en                    1'h0
        21      R/W    r_CLK_40M_DIV_CG_EN             1'h1
        22      R/W    r_CLK_40M_SRC_EN                1'h1
        23      R/W    r_40m_clk_src_sel_1             1'h0
        24      R/W    r_40m_clk_src_sel_0             1'h1
        25      R/W    r_f40m_pll_clk_cg_en            1'h0
        26      R/W    r_CLK_40M_SRC_DIV_EN            1'h1
        27      R/W    r_CLK_20M_SRC_EN                1'h0
        28      R/W    r_CLK_10M_SRC_EN                1'h1
        29      R/W    r_CLK_5M_SRC_EN                 1'h0
        30      R/W    r_CLK_1M_SRC_EN                 1'h0
        31      R/W    DUMMY                           1'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_SYS_CLK_SEL_2;
        struct
        {
            __IO uint32_t r_flash_div_sel: 4;
            __IO uint32_t r_flash_div_en: 1;
            __IO uint32_t r_FLASH_CLK_SRC_EN: 1;
            __IO uint32_t r_flash_clk_src_sel_1: 1;
            __IO uint32_t r_flash_clk_src_sel_0: 1;
            __IO uint32_t r_flash_mux_1_clk_cg_en: 1;
            __IO uint32_t r_rng_sfosc_sel: 1;
            __IO uint32_t r_rng_sfosc_div_sel: 3;
            __IO uint32_t RESERVED_2: 3;
            __IO uint32_t r_40m_div_sel: 3;
            __IO uint32_t RESERVED_1: 1;
            __IO uint32_t r_40m_div_en: 1;
            __IO uint32_t r_CLK_40M_DIV_CG_EN: 1;
            __IO uint32_t r_CLK_40M_SRC_EN: 1;
            __IO uint32_t r_40m_clk_src_sel_1: 1;
            __IO uint32_t r_40m_clk_src_sel_0: 1;
            __IO uint32_t r_f40m_pll_clk_cg_en: 1;
            __IO uint32_t r_CLK_40M_SRC_DIV_EN: 1;
            __IO uint32_t r_CLK_20M_SRC_EN: 1;
            __IO uint32_t r_CLK_10M_SRC_EN: 1;
            __IO uint32_t r_CLK_5M_SRC_EN: 1;
            __IO uint32_t r_CLK_1M_SRC_EN: 1;
            __IO uint32_t RESERVED_0: 1;
        } BITS_20C;
    } u_20C;

    /* 0x0210       0x4000_0210
        0       R/W    DUMMY                           1'h0
        1       R/W    DUMMY                           1'h0
        2       R/W    BIT_SOC_BTBUS_EN                1'h1
        3       R/W    BIT_SOC_FLASH1_EN               1'h0
        4       R/W    BIT_SOC_FLASH_EN                1'h1
        5       R/W    BIT_SOC_FLASH2_EN               1'h0
        6       R/W    BIT_SOC_FLASH3_EN               1'h0
        11:7    R/W    DUMMY                           6'h0
        12      R/W    BIT_SOC_LOG_UART_EN             1'h0
        13      R/W    BIT_SOC_GDMA0_EN                1'h0
        14      R/W    BIT_SOC_SDH_EN                  1'h0
        15      R/W    BIT_SOC_USB_EN                  1'h0
        16      R/W    BIT_SOC_GTIMERA_EN              1'h0
        17      R/W    BIT_SOC_GTIMERB_EN              1'h0
        18      R/W    BIT_SOC_SWR_SS_EN               1'h0
        19      R/W    BIT_SOC_ASRC2_EN                1'h0
        31:20   R/W    DUMMY                           12'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_SOC_FUNC_EN;
        struct
        {
            __IO uint32_t RESERVED_3: 1;
            __IO uint32_t RESERVED_2: 1;
            __IO uint32_t BIT_SOC_BTBUS_EN: 1;
            __IO uint32_t BIT_SOC_FLASH1_EN: 1;
            __IO uint32_t BIT_SOC_FLASH_EN: 1;
            __IO uint32_t BIT_SOC_FLASH2_EN: 1;
            __IO uint32_t BIT_SOC_FLASH3_EN: 1;
            __IO uint32_t RESERVED_1: 5;
            __IO uint32_t BIT_SOC_LOG_UART_EN: 1;
            __IO uint32_t BIT_SOC_GDMA0_EN: 1;
            __IO uint32_t BIT_SOC_SDH_EN: 1;
            __IO uint32_t BIT_SOC_USB_EN: 1;
            __IO uint32_t BIT_SOC_GTIMERA_EN: 1;
            __IO uint32_t BIT_SOC_GTIMERB_EN: 1;
            __IO uint32_t BIT_SOC_SWR_SS_EN: 1;
            __IO uint32_t BIT_SOC_ASRC2_EN: 1;
            __IO uint32_t RESERVED_0: 12;
        } BITS_210;
    } u_210;

    /* 0x0214       0x4000_0214
        31:0    R      RSVD                            32'h0
    */
    __IO uint32_t REG_PERION_REG_0x0214;

    /* 0x0218       0x4000_0218
        0       R/W    BIT_PERI_UART0_EN               1'h0
        1       R/W    BIT_PERI_UART2_EN               1'h0
        2       R/W    BIT_PERI_AES_EN                 1'h0
        3       R/W    BIT_PERI_RNG_EN                 1'h0
        4       R/W    BIT_PERI_UART3_EN               1'h0
        5       R/W    BIT_PERI_DSP2_WDT_EN            1'h0
        6       R/W    BIT_PERI_DSP_TIMER_EN           1'h0
        7       R/W    BIT_PERI_SPI2_EN                1'h0
        8       R/W    BIT_PERI_SPI0_EN                1'h0
        9       R/W    BIT_PERI_SPI1_EN                1'h0
        10      R/W    BIT_PERI_IR_EN                  1'h0
        11      R/W    BIT_PERI_DSP_DMA_EN             1'h0
        12      R/W    BIT_PERI_DSP_PERI_EN            1'h0
        13      R/W    BIT_PERI_DSP_SYS_RAM_EN         1'h0
        14      R/W    BIT_PERI_DSP_BUS_EN             1'h0
        15      R/W    BIT_PERI_I2C2_EN                1'h0
        16      R/W    BIT_PERI_I2C0_EN                1'h0
        17      R/W    BIT_PERI_I2C1_EN                1'h0
        18      R/W    BIT_PERI_QDECODE_EN             1'h0
        19      R/W    BIT_PERI_KEYSCAN_EN             1'h0
        20      R/W    BIT_PERI_DSP2_CORE_EN           1'h0
        21      R/W    BIT_PERI_DSP2_MEM_EN            1'h0
        22      R/W    BIT_PERI_PSRAM_EN               1'h0
        23      R/W    BIT_PERI_RSA_EN                 1'h0
        24      R/W    BIT_PERI_SPI2W_EN               1'h0
        25      R/W    BIT_DSP_CORE_EN                 1'h1
        26      R/W    BIT_DSP_H2D_D2H                 1'h1
        27      R/W    BIT_DSP_MEM                     1'h1
        28      R/W    BIT_ASRC_EN                     1'h0
        29      R/W    BIT_DSP_WDT_EN                  1'h0
        30      R/W    BIT_EFUSE_EN                    1'h0
        31      R/W    BIT_SHA256_EN                   1'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_SOC_PERI_FUNC0_EN;
        struct
        {
            __IO uint32_t BIT_PERI_UART0_EN: 1;
            __IO uint32_t BIT_PERI_UART2_EN: 1;
            __IO uint32_t BIT_PERI_AES_EN: 1;
            __IO uint32_t BIT_PERI_RNG_EN: 1;
            __IO uint32_t BIT_PERI_UART3_EN: 1;
            __IO uint32_t BIT_PERI_DSP2_WDT_EN: 1;
            __IO uint32_t BIT_PERI_DSP_TIMER_EN: 1;
            __IO uint32_t BIT_PERI_SPI2_EN: 1;
            __IO uint32_t BIT_PERI_SPI0_EN: 1;
            __IO uint32_t BIT_PERI_SPI1_EN: 1;
            __IO uint32_t BIT_PERI_IR_EN: 1;
            __IO uint32_t BIT_PERI_DSP_DMA_EN: 1;
            __IO uint32_t BIT_PERI_DSP_PERI_EN: 1;
            __IO uint32_t BIT_PERI_DSP_SYS_RAM_EN: 1;
            __IO uint32_t BIT_PERI_DSP_BUS_EN: 1;
            __IO uint32_t BIT_PERI_I2C2_EN: 1;
            __IO uint32_t BIT_PERI_I2C0_EN: 1;
            __IO uint32_t BIT_PERI_I2C1_EN: 1;
            __IO uint32_t BIT_PERI_QDECODE_EN: 1;
            __IO uint32_t BIT_PERI_KEYSCAN_EN: 1;
            __IO uint32_t BIT_PERI_DSP2_CORE_EN: 1;
            __IO uint32_t BIT_PERI_DSP2_MEM_EN: 1;
            __IO uint32_t BIT_PERI_PSRAM_EN: 1;
            __IO uint32_t BIT_PERI_RSA_EN: 1;
            __IO uint32_t BIT_PERI_SPI2W_EN: 1;
            __IO uint32_t BIT_DSP_CORE_EN: 1;
            __IO uint32_t BIT_DSP_H2D_D2H: 1;
            __IO uint32_t BIT_DSP_MEM: 1;
            __IO uint32_t BIT_ASRC_EN: 1;
            __IO uint32_t BIT_DSP_WDT_EN: 1;
            __IO uint32_t BIT_EFUSE_EN: 1;
            __IO uint32_t BIT_SHA256_EN: 1;
        } BITS_218;
    } u_218;

    /* 0x021C       0x4000_021c
        0       R/W    BIT_PERI_ADC_EN                 1'h0
        7:1     R/W    DUMMY                           7'h0
        8       R/W    BIT_PERI_GPIOA_EN               1'h0
        9       R/W    BIT_PERI_GPIOB_EN               1'h0
        10      R/W    BIT_PERI_GPIOC_EN               1'h0
        11      R/W    BIT_PERI_APSRAM_EN              1'h0
        12      R/W    BIT_PERI_ECC_EN                 1'h0
        13      R/W    BIT_CKE_ECC_EN                  1'h0
        31 :14  R/W    DUMMY                           18'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_SOC_PERI_FUNC1_EN;
        struct
        {
            __IO uint32_t BIT_PERI_ADC_EN: 1;
            __IO uint32_t RESERVED_1: 7;
            __IO uint32_t BIT_PERI_GPIOA_EN: 1;
            __IO uint32_t BIT_PERI_GPIOB_EN: 1;
            __IO uint32_t BIT_PERI_GPIOC_EN: 1;
            __IO uint32_t BIT_PERI_APSRAM_EN: 1;
            __IO uint32_t BIT_PERI_ECC_EN: 1;
            __IO uint32_t BIT_CKE_ECC_EN: 1;
            __IO uint32_t RESERVED_0: 18;
        } BITS_21C;
    } u_21C;

    /* 0x0220       0x4000_0220
        0       R/W    BIT_PERI_AUDIO_EN               1'h0
        1       R/W    BIT_PERI_SPORT0_EN              1'h0
        2       R/W    BIT_PERI_SPORT1_EN              1'h0
        3       R/W    BIT_PERI_SPDIF_EN               1'h0
        4       R/W    BIT_CKE_AUDIO_EN                1'h0
        5       R/W    BIT_CKE_SPORT0_EN               1'h0
        6       R/W    BIT_CKE_SPORT1_EN               1'h0
        7       R/W    BIT_CKE_SPDIF_EN                1'h0
        8       R/W    BIT_CKE_SPORT40M_EN             1'h0
        9       R/W    BIT_CKE_AUDIOREG_EN             1'h1
        10      R/W    BIT_PERI_SPORT2_EN              1'h0
        11      R/W    BIT_PERI_SPORT3_EN              1'h0
        12      R/W    BIT_CKE_SPORT2_EN               1'h0
        13      R/W    BIT_CKE_SPORT3_EN               1'h0
        31:14   R/W    DUMMY                           18'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_SOC_AUDIO_IF_EN;
        struct
        {
            __IO uint32_t BIT_PERI_AUDIO_EN: 1;
            __IO uint32_t BIT_PERI_SPORT0_EN: 1;
            __IO uint32_t BIT_PERI_SPORT1_EN: 1;
            __IO uint32_t BIT_PERI_SPDIF_EN: 1;
            __IO uint32_t BIT_CKE_AUDIO_EN: 1;
            __IO uint32_t BIT_CKE_SPORT0_EN: 1;
            __IO uint32_t BIT_CKE_SPORT1_EN: 1;
            __IO uint32_t BIT_CKE_SPDIF_EN: 1;
            __IO uint32_t BIT_CKE_SPORT40M_EN: 1;
            __IO uint32_t BIT_CKE_AUDIOREG_EN: 1;
            __IO uint32_t BIT_PERI_SPORT2_EN: 1;
            __IO uint32_t BIT_PERI_SPORT3_EN: 1;
            __IO uint32_t BIT_CKE_SPORT2_EN: 1;
            __IO uint32_t BIT_CKE_SPORT3_EN: 1;
            __IO uint32_t RESERVED_0: 18;
        } BITS_220;
    } u_220;

    /* 0x0224       0x4000_0224
        31:0    R/W    DUMMY                           32'h44
    */
    __IO uint32_t REG_PERION_REG_SOC_AUDIO_CLK_CTRL_A;

    /* 0x0228       0x4000_0228
        31:0    R/W    DUMMY                           32h48001
    */
    __IO uint32_t REG_PERION_REG_SOC_AUDIO_CLK_CTRL_B;

    /* 0x022C       0x4000_022c
        0       R/W    r_AUDIO_XTAL_SEL                1'h0
        31:1    R/W    DUMMY                           31'h72
    */
    union
    {
        __IO uint32_t REG_PERION_REG_SOC_AUDIO_CLK_CTRL_C;
        struct
        {
            __IO uint32_t r_AUDIO_XTAL_SEL: 1;
            __IO uint32_t RESERVED_0: 31;
        } BITS_22C;
    } u_22C;

    /* 0x0230       0x4000_0230
        0       R/W    BIT_CKE_HWSPI                   1'h1
        1       R/W    BIT_CKE_CORDIC                  1'h0
        2       R/W    BIT_SOC_CKE_PLFM                1'h1
        3       R/W    BIT_CKE_EFC                     1'h1
        4       R/W    BIT_CKE_BUS_RAM_SLP             1'h0
        5       R/W    BIT_CKE_BT_VEN                  1'h1
        6       R/W    BIT_SOC_ACTCK_VENDOR_REG_EN     1'h0
        7       R/W    BIT_SOC_SLPCK_VENDOR_REG_EN     1'h0
        8       R/W    BIT_SOC_ACTCK_FLASH_EN          1'h1
        9       R/W    BIT_SOC_SLPCK_FLASH_EN          1'h0
        10      R/W    BIT_SOC_ACTCK_UART2_EN          1'h0
        11      R/W    BIT_SOC_SLPCK_UART2_EN          1'h0
        12      R/W    BIT_SOC_ACTCK_UART1_EN          1'h0
        13      R/W    BIT_SOC_SLPCK_UART1_EN          1'h0
        14      R/W    BIT_SOC_ACTCK_TIMER0_EN         1'h0
        15      R/W    BIT_SOC_SLPCK_TIMER0_EN         1'h0
        16      R/W    BIT_SOC_ACTCK_GDMA0_EN          1'h0
        17      R/W    BIT_SOC_SLPCK_GDMA0_EN          1'h0
        18      R/W    BIT_SOC_ACTCK_UART3_EN          1'h0
        19      R/W    BIT_SOC_SLPCK_UART3_EN          1'h0
        20      R/W    BIT_SOC_ACTCK_TIMER1_EN         1'h0
        21      R/W    BIT_SOC_SLPCK_TIMER1_EN         1'h0
        22      R/W    BIT_SOC_ACTCK_GPIOB_EN          1'h0
        23      R/W    BIT_SOC_SLPCK_GPIOB_EN          1'h0
        24      R/W    BIT_SOC_ACTCK_GPIOA_EN          1'h0
        25      R/W    BIT_SOC_SLPCK_GPIOA_EN          1'h0
        26      R/W    BIT_SOC_ACTCK_SDH_EN            1'h0
        27      R/W    BIT_SOC_SLPCK_SDH_EN            1'h0
        28      R/W    BIT_SOC_ACTCK_USB_EN            1'h0
        29      R/W    BIT_SOC_SLPCK_USB_EN            1'h0
        30      R/W    BIT_CKE_COM_RAM                 1'h1
        31      R/W    BIT_CKE_BUS_COM_RAM_SLP         1'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_PESOC_CLK_CTRL;
        struct
        {
            __IO uint32_t BIT_CKE_HWSPI: 1;
            __IO uint32_t BIT_CKE_CORDIC: 1;
            __IO uint32_t BIT_SOC_CKE_PLFM: 1;
            __IO uint32_t BIT_CKE_EFC: 1;
            __IO uint32_t BIT_CKE_BUS_RAM_SLP: 1;
            __IO uint32_t BIT_CKE_BT_VEN: 1;
            __IO uint32_t BIT_SOC_ACTCK_VENDOR_REG_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_VENDOR_REG_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_FLASH_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_FLASH_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_UART2_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_UART2_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_UART1_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_UART1_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_TIMER0_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_TIMER0_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_GDMA0_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_GDMA0_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_UART3_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_UART3_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_TIMER1_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_TIMER1_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_GPIOB_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_GPIOB_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_GPIOA_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_GPIOA_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_SDH_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_SDH_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_USB_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_USB_EN: 1;
            __IO uint32_t BIT_CKE_COM_RAM: 1;
            __IO uint32_t BIT_CKE_BUS_COM_RAM_SLP: 1;
        } BITS_230;
    } u_230;

    /* 0x0234       0x4000_0234
        0       R/W    BIT_SOC_ACTCK_UART0_DATA_EN     1'h0
        1       R/W    BIT_SOC_SLPCK_UART0_DATA_EN     1'h0
        2       R/W    BIT_SOC_ACTCK_UART1_HCI_EN      1'h0
        3       R/W    BIT_SOC_SLPCK_UART1_HCI_EN      1'h0
        4       R/W    BIT_CKE_MODEM                   1'h1
        5       R/W    BIT_CKE_CAL32K                  1'h0
        6       R/W    BIT_CKE_SWR_SS                  1'h0
        7       R/W    BIT_CKE_RSA                     1'h0
        8       R/W    BIT_CKE_RNG                     1'h0
        9       R/W    BIT_CKE_PSRAM                   1'h1
        10      R/W    BIT_CKE_DSP_TIMER               1'h0
        11      R/W    BIT_CKE_DSP_SYNC                1'h1
        12      R/W    BIT_CKE_DSP2_WDT                1'h0
        13      R/W    BIT_CKE_ASRC2                   1'h0
        14      R/W    BIT_SOC_ACTCK_SPI2_EN           1'h0
        15      R/W    BIT_SOC_SLPCK_SPI2_EN           1'h0
        16      R/W    BIT_SOC_ACTCK_SPI0_EN           1'h0
        17      R/W    BIT_SOC_SLPCK_SPI0_EN           1'h0
        18      R/W    BIT_SOC_ACTCK_SPI1_EN           1'h0
        19      R/W    BIT_SOC_SLPCK_SPI1_EN           1'h0
        20      R/W    BIT_SOC_ACTCK_IR_EN             1'h0
        21      R/W    BIT_SOC_SLPCK_IR_EN             1'h0
        22      R/W    BIT_SOC_ACTCK_SHA_EN            1'h0
        23      R/W    BIT_SOC_SLPCK_SHA_EN            1'h0
        24      R/W    BIT_SOC_ACTCK_FLASH1_EN         1'h0
        25      R/W    BIT_SOC_SLPCK_FLASH1_EN         1'h0
        26      R/W    BIT_SOC_ACTCK_FLASH2_EN         1'h0
        27      R/W    BIT_SOC_SLPCK_FLASH2_EN         1'h0
        28      R/W    BIT_SOC_ACTCK_FLASH3_EN         1'h0
        29      R/W    BIT_SOC_SLPCK_FLASH3_EN         1'h0
        30      R/W    BIT_CKE_COM2_RAM                1'h0
        31      R/W    BIT_CKE_BUS_COM2_RAM_SLP        1'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_PESOC_PERI_CLK_CTRL0;
        struct
        {
            __IO uint32_t BIT_SOC_ACTCK_UART0_DATA_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_UART0_DATA_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_UART1_HCI_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_UART1_HCI_EN: 1;
            __IO uint32_t BIT_CKE_MODEM: 1;
            __IO uint32_t BIT_CKE_CAL32K: 1;
            __IO uint32_t BIT_CKE_SWR_SS: 1;
            __IO uint32_t BIT_CKE_RSA: 1;
            __IO uint32_t BIT_CKE_RNG: 1;
            __IO uint32_t BIT_CKE_PSRAM: 1;
            __IO uint32_t BIT_CKE_DSP_TIMER: 1;
            __IO uint32_t BIT_CKE_DSP_SYNC: 1;
            __IO uint32_t BIT_CKE_DSP2_WDT: 1;
            __IO uint32_t BIT_CKE_ASRC2: 1;
            __IO uint32_t BIT_SOC_ACTCK_SPI2_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_SPI2_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_SPI0_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_SPI0_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_SPI1_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_SPI1_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_IR_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_IR_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_SHA_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_SHA_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_FLASH1_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_FLASH1_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_FLASH2_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_FLASH2_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_FLASH3_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_FLASH3_EN: 1;
            __IO uint32_t BIT_CKE_COM2_RAM: 1;
            __IO uint32_t BIT_CKE_BUS_COM2_RAM_SLP: 1;
        } BITS_234;
    } u_234;

    /* 0x0238       0x4000_0238
        0       R/W    BIT_SOC_ACTCK_I2C0_EN           1'h0
        1       R/W    BIT_SOC_SLPCK_I2C0_EN           1'h0
        2       R/W    BIT_SOC_ACTCK_I2C1_EN           1'h0
        3       R/W    BIT_SOC_SLPCK_I2C1_EN           1'h0
        4       R/W    BIT_SOC_ACTCK_QDECODE_EN        1'h0
        5       R/W    BIT_SOC_SLPCK_QDECODE_EN        1'h0
        6       R/W    BIT_SOC_ACTCK_KEYSCAN_EN        1'h0
        7       R/W    BIT_SOC_SLPCK_KEYSCAN_EN        1'h0
        8       R/W    BIT_SOC_ACTCK_AES_EN            1'h0
        9       R/W    BIT_SOC_SLPCK_AES_EN            1'h0
        10      R/W    BIT_SOC_ACTCK_I2C2_EN           1'h0
        11      R/W    BIT_SOC_SLPCK_I2C2_EN           1'h0
        12      R/W    BIT_SOC_ACTCKE_DSP_DMA          1'h0
        13      R/W    BIT_SOC_ACTCKE_DSP_PERI         1'h0
        14      R/W    BIT_SOC_ACTCKE_DSP_SYS_RAM      1'h0
        15      R/W    BIT_SOC_ACTCKE_DSP_BUS          1'h0
        16      R/W    BIT_SOC_ACTCK_SPI_3WIRE_EN      1'h0
        17      R/W    BIT_SOC_SLPCK_SPI_3WIRE_EN      1'h0
        18      R/W    BIT_SOC_ACTCKE_DSP2_MEM         1'h0
        19      R/W    BIT_SOC_ACTCKE_DSP2             1'h0
        20      R/W    BIT_SOC_ACTCKE_ASRC             1'h0
        21      R/W    BIT_SOC_SLPCKE_ASRC             1'h0
        22      R/W    BIT_SOC_ACTCKE_DSP_MEM          1'h0
        23      R/W    BIT_SOC_SLPCKE_DSP_MEM          1'h0
        24      R/W    BIT_SOC_ACTCK_ADC_EN            1'h0
        25      R/W    BIT_SOC_SLPCK_ADC_EN            1'h0
        26      R/W    BIT_SOC_ACTCKE_H2D_D2H          1'h0
        27      R/W    BIT_SOC_SLPCKE_H2D_D2H          1'h0
        28      R/W    BIT_SOC_ACTCKE_DSP              1'h0
        29      R/W    BIT_SOC_SLPCKE_DSP              1'h0
        30      R/W    BIT_SOC_CKE_DSP_WDT             1'h0
        31      R/W    BIT_SOC_CLK_EFUSE               1'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_PESOC_PERI_CLK_CTRL1;
        struct
        {
            __IO uint32_t BIT_SOC_ACTCK_I2C0_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_I2C0_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_I2C1_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_I2C1_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_QDECODE_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_QDECODE_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_KEYSCAN_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_KEYSCAN_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_AES_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_AES_EN: 1;
            __IO uint32_t BIT_SOC_ACTCK_I2C2_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_I2C2_EN: 1;
            __IO uint32_t BIT_SOC_ACTCKE_DSP_DMA: 1;
            __IO uint32_t BIT_SOC_ACTCKE_DSP_PERI: 1;
            __IO uint32_t BIT_SOC_ACTCKE_DSP_SYS_RAM: 1;
            __IO uint32_t BIT_SOC_ACTCKE_DSP_BUS: 1;
            __IO uint32_t BIT_SOC_ACTCK_SPI_3WIRE_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_SPI_3WIRE_EN: 1;
            __IO uint32_t BIT_SOC_ACTCKE_DSP2_MEM: 1;
            __IO uint32_t BIT_SOC_ACTCKE_DSP2: 1;
            __IO uint32_t BIT_SOC_ACTCKE_ASRC: 1;
            __IO uint32_t BIT_SOC_SLPCKE_ASRC: 1;
            __IO uint32_t BIT_SOC_ACTCKE_DSP_MEM: 1;
            __IO uint32_t BIT_SOC_SLPCKE_DSP_MEM: 1;
            __IO uint32_t BIT_SOC_ACTCK_ADC_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_ADC_EN: 1;
            __IO uint32_t BIT_SOC_ACTCKE_H2D_D2H: 1;
            __IO uint32_t BIT_SOC_SLPCKE_H2D_D2H: 1;
            __IO uint32_t BIT_SOC_ACTCKE_DSP: 1;
            __IO uint32_t BIT_SOC_SLPCKE_DSP: 1;
            __IO uint32_t BIT_SOC_CKE_DSP_WDT: 1;
            __IO uint32_t BIT_SOC_CLK_EFUSE: 1;
        } BITS_238;
    } u_238;

    /* 0x023C       0x4000_023c
        0       R/W    r_epalna_od                     1'h0
        31:1    R/W    DUMMY                           31'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_0x023C;
        struct
        {
            __IO uint32_t r_epalna_od: 1;
            __IO uint32_t RESERVED_0: 31;
        } BITS_23C;
    } u_23C;

    /* 0x0240       0x4000_0240
        0       R/W    BIT_SOC_ACTCK_GPIOC_EN          1'h0
        1       R/W    BIT_SOC_SLPCK_GPIOC_EN          1'h0
        7:2     R/W    DUMMY                           6'h0
        8       R/W    BIT_SOC_CK_APSRAM_EN            1'h0
        31:9    R/W    DUMMY                           23'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_0x0240;
        struct
        {
            __IO uint32_t BIT_SOC_ACTCK_GPIOC_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_GPIOC_EN: 1;
            __IO uint32_t RESERVED_1: 6;
            __IO uint32_t BIT_SOC_CK_APSRAM_EN: 1;
            __IO uint32_t RESERVED_0: 23;
        } BITS_240;
    } u_240;

    /* 0x0244       0x4000_0244
        0       R/W    BIT_SOC_ACTCK_BTBUS_EN          1
        1       R/W    BIT_SOC_SLPCK_BTBUS_EN          1
        31:2    R/W    DUMMY                           30'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_PESOC_COM_CLK_CTRL1_REG_OFF_MEM_PWR_CRTL;
        struct
        {
            __IO uint32_t BIT_SOC_ACTCK_BTBUS_EN: 1;
            __IO uint32_t BIT_SOC_SLPCK_BTBUS_EN: 1;
            __IO uint32_t RESERVED_0: 30;
        } BITS_244;
    } u_244;

    /* 0x0248       0x4000_0248
        0       R/W    r_cpu_auto_slow_force_update    0
        1       R/W    r_mcu_fft1_cken                 1'h0
        2       R/W    r_mcu_fft2_cken                 1'h0
        3       R/W    r_mcu_fft1_fen                  1'h0
        4       R/W    r_mcu_fft2_fen                  1'h0
        7:5     R/W    DUMMY                           3'h0
        10:8    R/W    r_swr_ss_div_sel                3'h0
        15:11   R/W    DUMMY                           5'h0
        16      R/W    r_phy_gpio1_reg_1               1'h0
        17      R/W    r_phy_gpio1_sel0                1'h0
        19:18   R/W    r_phy_gpio1_sel1                2'h0
        21:20   R/W    r_phy_gpio1_sel2                2'h0
        22      R/W    r_phy_gpio2_reg_2               1'h0
        23      R/W    r_phy_gpio2_sel0                1'h0
        25:24   R/W    r_phy_gpio2_sel1                2'h0
        27:26   R/W    r_phy_gpio2_sel2                2'h0
        28      R/W    DUMMY                           1'h0
        29      R/W    r_clk_dsp1_en                   1'h1
        30      R/W    r_clk_dsp2_en                   1'h1
        31      R/W    r_clk_dspb_en                   1'h1
    */
    union
    {
        __IO uint32_t REG_PERION_REG_0x0248;
        struct
        {
            __IO uint32_t r_cpu_auto_slow_force_update: 1;
            __IO uint32_t r_mcu_fft1_cken: 1;
            __IO uint32_t r_mcu_fft2_cken: 1;
            __IO uint32_t r_mcu_fft1_fen: 1;
            __IO uint32_t r_mcu_fft2_fen: 1;
            __IO uint32_t RESERVED_2: 3;
            __IO uint32_t r_swr_ss_div_sel: 3;
            __IO uint32_t RESERVED_1: 5;
            __IO uint32_t r_phy_gpio1_reg_1: 1;
            __IO uint32_t r_phy_gpio1_sel0: 1;
            __IO uint32_t r_phy_gpio1_sel1: 2;
            __IO uint32_t r_phy_gpio1_sel2: 2;
            __IO uint32_t r_phy_gpio2_reg_2: 1;
            __IO uint32_t r_phy_gpio2_sel0: 1;
            __IO uint32_t r_phy_gpio2_sel1: 2;
            __IO uint32_t r_phy_gpio2_sel2: 2;
            __IO uint32_t RESERVED_0: 1;
            __IO uint32_t r_clk_dsp1_en: 1;
            __IO uint32_t r_clk_dsp2_en: 1;
            __IO uint32_t r_clk_dspb_en: 1;
        } BITS_248;
    } u_248;

    /* 0x024C       0x4000_024c
        31:0    R/W    r_slow_debug_sel_1_0            32'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_SLOW_DEBUG_1_0;
        struct
        {
            __IO uint32_t r_slow_debug_sel_1_0: 32;
        } BITS_24C;
    } u_24C;

    /* 0x0250       0x4000_0250
        31:0    R/W    r_slow_debug_sel_1_1            32'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_SLOW_DEBUG_1_1;
        struct
        {
            __IO uint32_t r_slow_debug_sel_1_1: 32;
        } BITS_250;
    } u_250;

    /* 0x0254       0x4000_0254
        0       R/W    DSP_RUN_STALL                   1'h1
        1       R/W    DSP_STAT_VECTOR_SEL             1'h0
        2       R/W    reg_bypass_pipe                 1'h0
        3       R/W    DUMMY                           1'h0
        4       R/W    DSP2_RUN_STALL                  1'h1
        5       R/W    DSP2_STAT_VECTOR_SEL            1'h0
        15:6    R/W    DUMMY                           10'h0
        19:16   R/W    r_cpu_low_rate_valid_num1       4'h3
        23:20   R/W    r_dsp_low_rate_valid_num1       4'h3
        24      R/W    r_cpu_auto_slow_filter1_en      1'h1
        25      R/W    r_dsp_auto_slow_filter1_en      1'h1
        31:26   R/W    DUMMY                           6'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_DSP_TOP_CTRL;
        struct
        {
            __IO uint32_t DSP_RUN_STALL: 1;
            __IO uint32_t DSP_STAT_VECTOR_SEL: 1;
            __IO uint32_t reg_bypass_pipe: 1;
            __IO uint32_t RESERVED_2: 1;
            __IO uint32_t DSP2_RUN_STALL: 1;
            __IO uint32_t DSP2_STAT_VECTOR_SEL: 1;
            __IO uint32_t RESERVED_1: 10;
            __IO uint32_t r_cpu_low_rate_valid_num1: 4;
            __IO uint32_t r_dsp_low_rate_valid_num1: 4;
            __IO uint32_t r_cpu_auto_slow_filter1_en: 1;
            __IO uint32_t r_dsp_auto_slow_filter1_en: 1;
            __IO uint32_t RESERVED_0: 6;
        } BITS_254;
    } u_254;

    /* 0x0258       0x4000_0258
        31:0    R/W    r_slow_debug_sel_2_0            32'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_SLOW_DEBUG_2_0;
        struct
        {
            __IO uint32_t r_slow_debug_sel_2_0: 32;
        } BITS_258;
    } u_258;

    /* 0x025C       0x4000_025c
        31:0    R/W    r_slow_debug_sel_2_1            32'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_SLOW_DEBUG_2_1;
        struct
        {
            __IO uint32_t r_slow_debug_sel_2_1: 32;
        } BITS_25C;
    } u_25C;

    /* 0x0260       0x4000_0260
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_REG_0x0260;

    /* 0x0264       0x4000_0264
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_REG_0x0264;

    /* 0x0268       0x4000_0268
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_REG_0x0268;

    /* 0x026C       0x4000_026c
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_REG_0x026C;

    /* 0x0270       0x4000_0270
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_REG_0x0270;

    /* 0x0274       0x4000_0274
        19:0    R/W    r_dss_data_in                   20'h0
        22:20   R/W    r_dss_ro_sel                    3'h0
        23      R/W    r_dss_wire_sel                  0
        24      R/W    r_dss_clk_en                    0
        25      R/W    r_dss_speed_en                  0
        26      R/W    r_FEN_DSS                       0
        31:27   R/W    DUMMY                           5'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_DSS_CTRL;
        struct
        {
            __IO uint32_t r_dss_data_in: 20;
            __IO uint32_t r_dss_ro_sel: 3;
            __IO uint32_t r_dss_wire_sel: 1;
            __IO uint32_t r_dss_clk_en: 1;
            __IO uint32_t r_dss_speed_en: 1;
            __IO uint32_t r_FEN_DSS: 1;
            __IO uint32_t RESERVED_0: 5;
        } BITS_274;
    } u_274;

    /* 0x0278       0x4000_0278
        19:0    R      bset_dss_count_out              20'h0
        20      R      bset_dss_wsort_go               0
        21      R      bset_dss_ready                  0
        31:22   R      RSVD                            10'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_BEST_DSS_RD;
        struct
        {
            __I uint32_t bset_dss_count_out: 20;
            __I uint32_t bset_dss_wsort_go: 1;
            __I uint32_t bset_dss_ready: 1;
            __I uint32_t RESERVED_0: 10;
        } BITS_278;
    } u_278;

    /* 0x027C       0x4000_027c
        19:0    R      dss_ir_count_out                20'h0
        20      R      dss_ir_wsort_go                 0
        21      R      dss_ir_ready                    0
        31:22   R      RSVD                            10'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_WORST_DSS_RD;
        struct
        {
            __I uint32_t dss_ir_count_out: 20;
            __I uint32_t dss_ir_wsort_go: 1;
            __I uint32_t dss_ir_ready: 1;
            __I uint32_t RESERVED_0: 10;
        } BITS_27C;
    } u_27C;

    /* 0x0280       0x4000_0280
        31:0    R/W    DUMMY                           32'h0
    */
    __IO uint32_t REG_PERION_REG_0x0280;

    /* 0x0284       0x4000_0284
        31:0    R/W    DUMMY                           32'h0
    */
    __IO uint32_t REG_PERION_DUMMY_2;

    /* 0x0288       0x4000_0288
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_DUMMY_3;

    /* 0x028C       0x4000_028c
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_DUMMY_4;

    /* 0x0290       0x4000_0290
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_DUMMY_5;

    /* 0x0294       0x4000_0294
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_DUMMY_6;

    /* 0x0298       0x4000_0298
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_DUMMY_7;

    /* 0x029C       0x4000_029c
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_DUMMY_8;

    /* 0x02A0       0x4000_02a0
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_DUMMY_9;

    /* 0x02A4       0x4000_02a4
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_DUMMY_10;

    /* 0x02A8       0x4000_02a8
        3:0     R/W    DUMMY                           4'h1
        4       R/W    PMUX_SPIC_MON_EN                1'b0
        5       R/W    DUMMY                           1'h0
        6       R/W    bypass_dcd_dbnc                 1'b0
        7       R/W    DUMMY                           1'b0
        12:8    R/W    PMUX_1_WIRE_UART_EN             5'b0
        15:13   R/W    DUMMY                           3'b0
        30:16   R/W    DUMMY                           15'h0002
        31      R/W    bypass_non_std_det              1'b0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_TEST_MODE;
        struct
        {
            __IO uint32_t RESERVED_4: 4;
            __IO uint32_t PMUX_SPIC_MON_EN: 1;
            __IO uint32_t RESERVED_3: 1;
            __IO uint32_t bypass_dcd_dbnc: 1;
            __IO uint32_t RESERVED_2: 1;
            __IO uint32_t uart0_one_wire: 1;
            __IO uint32_t RSVD1: 1;
            __IO uint32_t uart3_one_wire: 1;
            __IO uint32_t uart1_one_wire: 1;
            __IO uint32_t uart2_one_wire: 1;
            __IO uint32_t RESERVED_1: 3;
            __IO uint32_t RESERVED_0: 15;
            __IO uint32_t bypass_non_std_det: 1;
        } BITS_2A8;
    } u_2A8;

    /* 0x02AC       0x4000_02ac
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_DUMMY_11;

    /* 0x02B0       0x4000_02b0
        31:0    R/W    DUMMY                           32'h0
    */
    __IO uint32_t REG_PERION_DUMMY_12;

    /* 0x02B4       0x4000_02b4
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_DUMMY_13;

    /* 0x02B8       0x4000_02b8
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_DUMMY_14;

    /* 0x02BC       0x4000_02bc
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_DUMMY_15;

    /* 0x02C0       0x4000_02c0
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_DUMMY_16;

    /* 0x02C4       0x4000_02c4
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_DUMMY_17;

    /* 0x02C8       0x4000_02c8
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_DUMMY_18;

    /* 0x02CC       0x4000_02cc
        31:0    R      RSVD                            32'hEAEAEAEA
    */
    __IO uint32_t REG_PERION_DUMMY_19;

    /* 0x02D0       0x4000_02d0
        3:0     R/W    r_flash1_div_sel                4'h0
        4       R/W    r_flash1_div_en                 1'h0
        5       R/W    r_FLASH1_CLK_SRC_EN             1'h0
        6       R/W    r_flash1_clk_src_sel_1          1'h0
        7       R/W    r_flash1_clk_src_sel_0          1'h0
        8       R/W    r_flash1_mux_1_clk_cg_en        1'h0
        12:9    R/W    r_flash2_div_sel                4'h0
        13      R/W    r_flash2_div_en                 1'h0
        14      R/W    r_FLASH2_CLK_SRC_EN             1'h0
        15      R/W    r_flash2_clk_src_sel_1          1'h0
        16      R/W    r_flash2_clk_src_sel_0          1'h0
        17      R/W    r_flash2_mux_1_clk_cg_en        1'h0
        21:18   R/W    r_flash3_div_sel                4'h0
        22      R/W    r_flash3_div_en                 1'h0
        23      R/W    r_FLASH3_CLK_SRC_EN             1'h0
        24      R/W    r_flash3_clk_src_sel_1          1'h0
        25      R/W    r_flash3_clk_src_sel_0          1'h0
        26      R/W    r_flash3_mux_1_clk_cg_en        1'h0
        31:27   R/W    DUMMY                           5'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_FLASH_CLK_CTRL;
        struct
        {
            __IO uint32_t r_flash1_div_sel: 4;
            __IO uint32_t r_flash1_div_en: 1;
            __IO uint32_t r_FLASH1_CLK_SRC_EN: 1;
            __IO uint32_t r_flash1_clk_src_sel_1: 1;
            __IO uint32_t r_flash1_clk_src_sel_0: 1;
            __IO uint32_t r_flash1_mux_1_clk_cg_en: 1;
            __IO uint32_t r_flash2_div_sel: 4;
            __IO uint32_t r_flash2_div_en: 1;
            __IO uint32_t r_FLASH2_CLK_SRC_EN: 1;
            __IO uint32_t r_flash2_clk_src_sel_1: 1;
            __IO uint32_t r_flash2_clk_src_sel_0: 1;
            __IO uint32_t r_flash2_mux_1_clk_cg_en: 1;
            __IO uint32_t r_flash3_div_sel: 4;
            __IO uint32_t r_flash3_div_en: 1;
            __IO uint32_t r_FLASH3_CLK_SRC_EN: 1;
            __IO uint32_t r_flash3_clk_src_sel_1: 1;
            __IO uint32_t r_flash3_clk_src_sel_0: 1;
            __IO uint32_t r_flash3_mux_1_clk_cg_en: 1;
            __IO uint32_t RESERVED_0: 5;
        } BITS_2D0;
    } u_2D0;

    /* 0x02D4       0x4000_02d4
        3:0     R/W    r_psram_div_sel                 4'h0
        4       R/W    r_psram_div_en                  1'h0
        5       R/W    r_PSRAM_CLK_SRC_EN              1'h0
        6       R/W    r_psram_clk_src_sel_1           1'h0
        7       R/W    r_psram_clk_src_sel_0           1'h0
        8       R/W    r_psram_mux_1_clk_cg_en         1'h0
        9       R/W    r_psram_pinmux_sel              1'h0
        31:10   R      RSVD                            22'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_PSRAM_CLK_CTRL;
        struct
        {
            __IO uint32_t r_psram_div_sel: 4;
            __IO uint32_t r_psram_div_en: 1;
            __IO uint32_t r_PSRAM_CLK_SRC_EN: 1;
            __IO uint32_t r_psram_clk_src_sel_1: 1;
            __IO uint32_t r_psram_clk_src_sel_0: 1;
            __IO uint32_t r_psram_mux_1_clk_cg_en: 1;
            __IO uint32_t r_psram_pinmux_sel: 1;
            __I uint32_t RESERVED_0: 22;
        } BITS_2D4;
    } u_2D4;

    /* 0x02D8       0x4000_02d8
        3:0     R/W    r_nna_div_sel                   4'h0
        4       R/W    r_nna_div_en                    1'h0
        5       R/W    r_NNA_CLK_SRC_EN                1'h0
        6       R/W    r_nna_clk_src_sel_1             1'h0
        7       R/W    r_nna_clk_src_sel_0             1'h0
        8       R/W    r_nna_mux_clk_cg_en             1'h0
        9       R/W    r_nna_pll_src_sel_0             1'h0
        10      R/W    r_CKE_NNA                       1'h0
        11      R/W    r_FEN_NNA                       1'h0
        12      R/W    r_nna_pll_src_sel_1             1'h0
        15:13   R/W    RSVD                            3'h0
        16      R/W    r_nna_interrupt_in_mcu          1'h0
        31:17   R/W    RSVD                            15'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_0x02D8;
        struct
        {
            __IO uint32_t r_nna_div_sel: 4;
            __IO uint32_t r_nna_div_en: 1;
            __IO uint32_t r_NNA_CLK_SRC_EN: 1;
            __IO uint32_t r_nna_clk_src_sel_1: 1;
            __IO uint32_t r_nna_clk_src_sel_0: 1;
            __IO uint32_t r_nna_mux_clk_cg_en: 1;
            __IO uint32_t r_nna_pll_src_sel_0: 1;
            __IO uint32_t r_CKE_NNA: 1;
            __IO uint32_t r_FEN_NNA: 1;
            __IO uint32_t r_nna_pll_src_sel_1: 1;
            __IO uint32_t RESERVED_1: 3;
            __IO uint32_t r_nna_interrupt_in_mcu: 1;
            __IO uint32_t RESERVED_0: 15;
        } BITS_2D8;
    } u_2D8;

    /* 0x02DC       0x4000_02dc
        0       R/W    r_SPIC_OPI_EN                   1'b0
        1       R/W    r_SPIC_QPI_EN                   1'b0
        2       R/W    r_clko_sel                      1'b0
        3       R/W    r_inv_clk_oe                    1'b0
        11:4    R/W    r_spi_dqs_dly                   8'b0
        19:12   R/W    r_clko_dly_sel                  8'b0
        20      R/W    r_fetch_sclk_phase              1'b0
        21      R/W    r_fetch_sclk_phase_2            1'b0
        22      R/W    r_ds_sel                        1'b0
        23      R/W    r_ds_dtr                        1'b0
        24      R/W    r_data_phase_sel                1'b0
        25      R/W    r_pos_data_order                1'b0
        26      R/W    r_spi_clk_sel                   1'b0
        28:27   R/W    r_opi_phy_dbg_sel               2'b0
        30:29   R/W    r_spi_qpi_clk_sel               2'b0
        31      R/W    r_fetch_spic_clk_phase          1'b0
    */
    union
    {
        __IO uint32_t REG_PERION_PSRAM_PHY_Control;
        struct
        {
            __IO uint32_t r_SPIC_OPI_EN: 1;
            __IO uint32_t r_SPIC_QPI_EN: 1;
            __IO uint32_t r_clko_sel: 1;
            __IO uint32_t r_inv_clk_oe: 1;
            __IO uint32_t r_spi_dqs_dly: 8;
            __IO uint32_t r_clko_dly_sel: 8;
            __IO uint32_t r_fetch_sclk_phase: 1;
            __IO uint32_t r_fetch_sclk_phase_2: 1;
            __IO uint32_t r_ds_sel: 1;
            __IO uint32_t r_ds_dtr: 1;
            __IO uint32_t r_data_phase_sel: 1;
            __IO uint32_t r_pos_data_order: 1;
            __IO uint32_t r_spi_clk_sel: 1;
            __IO uint32_t r_opi_phy_dbg_sel: 2;
            __IO uint32_t r_spi_qpi_clk_sel: 2;
            __IO uint32_t r_fetch_spic_clk_phase: 1;
        } BITS_2DC;
    } u_2DC;

    /* 0x02E0       0x4000_02e0
        0       R/W    det_enable                      0
        1       R/W    vbus_det                        0
        2       R/W    skip_sec_detection              0
        3       R/W    DUMMY                           0
        7:4     R/W    DUMMY                           4'hc
        8       R      sdp_op5a                        0
        9       R      dcp_cdp_1p5a                    0
        10      R      cdp_det                         0
        11      R      dcp_det                         0
        12      R      others_op5a                     0
        13      R      DUMMY                           0
        14      R      DUMMY                           0
        15      R      DUMMY                           0
        16      R      det_is_done                     0
        17      R      DUMMY                           0
        18      R      DUMMY                           0
        31:19   R/W    DUMMY                           13'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_SOC_CHARGER_DET_A;
        struct
        {
            __IO uint32_t det_enable: 1;
            __IO uint32_t vbus_det: 1;
            __IO uint32_t skip_sec_detection: 1;
            __IO uint32_t RESERVED_7: 1;
            __IO uint32_t RESERVED_6: 4;
            __I uint32_t sdp_op5a: 1;
            __I uint32_t dcp_cdp_1p5a: 1;
            __I uint32_t cdp_det: 1;
            __I uint32_t dcp_det: 1;
            __I uint32_t others_op5a: 1;
            __I uint32_t RESERVED_5: 1;
            __I uint32_t RESERVED_4: 1;
            __I uint32_t RESERVED_3: 1;
            __I uint32_t det_is_done: 1;
            __I uint32_t RESERVED_2: 1;
            __I uint32_t RESERVED_1: 1;
            __IO uint32_t RESERVED_0: 13;
        } BITS_2E0;
    } u_2E0;

    /* 0x02E4       0x4000_02e4
        0       R/W    idp_src_en                    1'b0
        1       R/W    rdm_dwn_en                    1'b0
        2       R/W    vdp_src_en                    1'b0
        3       R/W    idm_snk_en                    1'b0
        4       R/W    vdm_src_en                    1'b0
        5       R/W    idp_snk_en                    1'b0
        6       R/W    non_std_det_en                1'b0
        7       R/W    pow_L                             1'b0
        8       R/W    fw_ctrl_mode                  1'b0
            15:9    R/W    DUMMY15_9                     7'h0
            19:16   R      VDP_COMP_L                    4'h0
        20      R      VDP_LT_VLOW_L                 1'b0
                24:21   R      VDM_COMP_L                    4'h0
        25      R      VDM_LT_VLOW_L                 1'b0
            31:26   R/W    DUMMY31_26                    6'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_SOC_CHARGER_DET_B;
        struct
        {
            __IO uint32_t idp_src_en: 1;
            __IO uint32_t rdm_dwn_en: 1;
            __IO uint32_t vdp_src_en: 1;
            __IO uint32_t idm_snk_en: 1;
            __IO uint32_t vdm_src_en: 1;
            __IO uint32_t idp_snk_en: 1;
            __IO uint32_t non_std_det_en: 1; //???
            __IO uint32_t pow_L: 1;
            __IO uint32_t fw_ctrl_mode: 1;
            __IO uint32_t DUMMY15_9: 7;
            __I uint32_t VDP_COMP_L: 4;
            __I uint32_t VDP_LT_VLOW_L: 1;
            __I uint32_t VDM_COMP_L: 4;
            __I uint32_t VDM_LT_VLOW_L: 1;
            __I uint32_t DUMMY31_26: 6;
        } BITS_2E4;
    } u_2E4;

    /* 0x02E8       0x4000_02e8
        0       R/W    CORE_FPWMDIG_H                  0
        3:1     R/W    CORE_SWRCORE_ENDH               3'b100
        6:4     R/W    CORE_SWRCORE_ENDP               3'b100
        8:7     R/W    CORE_SWRCORE_MNSZ               2'b11
        10:9    R/W    CORE_SWRCORE_MPSZ               2'b11
        16:11   R/W    CORE_SWRCORE_SAW_FREQ           6'b100000
        19:17   R/W    CORE_SWRCORE_VREFOCP_H          3'b100
        24:20   R/W    DUMMY                           5'b00000
        31:25   R/W    CORE_VOUT_TUNE_LDO_H            7'b0101011
    */
    union
    {
        __IO uint32_t REG_PERION_CORE_SWR_PAR_TAB_0;
        struct
        {
            __IO uint32_t CORE_FPWMDIG_H: 1;
            __IO uint32_t CORE_SWRCORE_ENDH: 3;
            __IO uint32_t CORE_SWRCORE_ENDP: 3;
            __IO uint32_t CORE_SWRCORE_MNSZ: 2;
            __IO uint32_t CORE_SWRCORE_MPSZ: 2;
            __IO uint32_t CORE_SWRCORE_SAW_FREQ: 6;
            __IO uint32_t CORE_SWRCORE_VREFOCP_H: 3;
            __IO uint32_t RESERVED_0: 5;
            __IO uint32_t CORE_VOUT_TUNE_LDO_H: 7;
        } BITS_2E8;
    } u_2E8;

    /* 0x02EC       0x4000_02ec
        0       R/W    CORE_FPWMDIG_H                  0
        3:1     R/W    CORE_SWRCORE_ENDH               3'b100
        6:4     R/W    CORE_SWRCORE_ENDP               3'b100
        8:7     R/W    CORE_SWRCORE_MNSZ               2'b11
        10:9    R/W    CORE_SWRCORE_MPSZ               2'b11
        16:11   R/W    CORE_SWRCORE_SAW_FREQ           6'b100000
        19:17   R/W    CORE_SWRCORE_VREFOCP_H          3'b100
        24:20   R/W    DUMMY                           5'b00000
        31:25   R/W    CORE_VOUT_TUNE_LDO_H            7'b0101011
    */
    union
    {
        __IO uint32_t REG_PERION_CORE_SWR_PAR_TAB_4;
        struct
        {
            __IO uint32_t CORE_FPWMDIG_H: 1;
            __IO uint32_t CORE_SWRCORE_ENDH: 3;
            __IO uint32_t CORE_SWRCORE_ENDP: 3;
            __IO uint32_t CORE_SWRCORE_MNSZ: 2;
            __IO uint32_t CORE_SWRCORE_MPSZ: 2;
            __IO uint32_t CORE_SWRCORE_SAW_FREQ: 6;
            __IO uint32_t CORE_SWRCORE_VREFOCP_H: 3;
            __IO uint32_t RESERVED_0: 5;
            __IO uint32_t CORE_VOUT_TUNE_LDO_H: 7;
        } BITS_2EC;
    } u_2EC;

    /* 0x02F0       0x4000_02f0
        0       R/W    CORE_FPWMDIG_H                  0
        3:1     R/W    CORE_SWRCORE_ENDH               3'b100
        6:4     R/W    CORE_SWRCORE_ENDP               3'b100
        8:7     R/W    CORE_SWRCORE_MNSZ               2'b11
        10:9    R/W    CORE_SWRCORE_MPSZ               2'b11
        16:11   R/W    CORE_SWRCORE_SAW_FREQ           6'b100000
        19:17   R/W    CORE_SWRCORE_VREFOCP_H          3'b100
        24:20   R/W    DUMMY                           5'b00000
        31:25   R/W    CORE_VOUT_TUNE_LDO_H            7'b0101011
    */
    union
    {
        __IO uint32_t REG_PERION_CORE_SWR_PAR_TAB_5;
        struct
        {
            __IO uint32_t CORE_FPWMDIG_H: 1;
            __IO uint32_t CORE_SWRCORE_ENDH: 3;
            __IO uint32_t CORE_SWRCORE_ENDP: 3;
            __IO uint32_t CORE_SWRCORE_MNSZ: 2;
            __IO uint32_t CORE_SWRCORE_MPSZ: 2;
            __IO uint32_t CORE_SWRCORE_SAW_FREQ: 6;
            __IO uint32_t CORE_SWRCORE_VREFOCP_H: 3;
            __IO uint32_t RESERVED_0: 5;
            __IO uint32_t CORE_VOUT_TUNE_LDO_H: 7;
        } BITS_2F0;
    } u_2F0;

    /* 0x02F4       0x4000_02f4
        0       R/W    CORE_FPWMDIG_H                  0
        3:1     R/W    CORE_SWRCORE_ENDH               3'b100
        6:4     R/W    CORE_SWRCORE_ENDP               3'b100
        8:7     R/W    CORE_SWRCORE_MNSZ               2'b11
        10:9    R/W    CORE_SWRCORE_MPSZ               2'b11
        16:11   R/W    CORE_SWRCORE_SAW_FREQ           6'b100000
        19:17   R/W    CORE_SWRCORE_VREFOCP_H          3'b100
        24:20   R/W    DUMMY                           5'b00000
        31:25   R/W    CORE_VOUT_TUNE_LDO_H            7'b0101011
    */
    union
    {
        __IO uint32_t REG_PERION_CORE_SWR_PAR_TAB_6;
        struct
        {
            __IO uint32_t CORE_FPWMDIG_H: 1;
            __IO uint32_t CORE_SWRCORE_ENDH: 3;
            __IO uint32_t CORE_SWRCORE_ENDP: 3;
            __IO uint32_t CORE_SWRCORE_MNSZ: 2;
            __IO uint32_t CORE_SWRCORE_MPSZ: 2;
            __IO uint32_t CORE_SWRCORE_SAW_FREQ: 6;
            __IO uint32_t CORE_SWRCORE_VREFOCP_H: 3;
            __IO uint32_t RESERVED_0: 5;
            __IO uint32_t CORE_VOUT_TUNE_LDO_H: 7;
        } BITS_2F4;
    } u_2F4;

    /* 0x02F8       0x4000_02f8
        0       R/W    CORE_FPWMDIG_H                  0
        3:1     R/W    CORE_SWRCORE_ENDH               3'b100
        6:4     R/W    CORE_SWRCORE_ENDP               3'b100
        8:7     R/W    CORE_SWRCORE_MNSZ               2'b11
        10:9    R/W    CORE_SWRCORE_MPSZ               2'b11
        16:11   R/W    CORE_SWRCORE_SAW_FREQ           6'b100000
        19:17   R/W    CORE_SWRCORE_VREFOCP_H          3'b100
        24:20   R/W    DUMMY                           5'b00000
        31:25   R/W    CORE_VOUT_TUNE_LDO_H            7'b0101011
    */
    union
    {
        __IO uint32_t REG_PERION_CORE_SWR_PAR_TAB_7;
        struct
        {
            __IO uint32_t CORE_FPWMDIG_H: 1;
            __IO uint32_t CORE_SWRCORE_ENDH: 3;
            __IO uint32_t CORE_SWRCORE_ENDP: 3;
            __IO uint32_t CORE_SWRCORE_MNSZ: 2;
            __IO uint32_t CORE_SWRCORE_MPSZ: 2;
            __IO uint32_t CORE_SWRCORE_SAW_FREQ: 6;
            __IO uint32_t CORE_SWRCORE_VREFOCP_H: 3;
            __IO uint32_t RESERVED_0: 5;
            __IO uint32_t CORE_VOUT_TUNE_LDO_H: 7;
        } BITS_2F8;
    } u_2F8;

    /* 0x02FC       0x4000_02fc
        2:0     R/W    CORE_SWRCORE_ZCDIB_0            3'b100
        3       R/W    CORE_EN_EAIqX4_0                1
        4       R/W    CORE_ENEAIqX2_H_0               1
        5       RW     DUMMY                           0
        8:6     R/W    CORE_SWRCORE_ZCDIB_4            3'b100
        9       R/W    CORE_EN_EAIqX4_4                1
        10      R/W    CORE_ENEAIqX2_H_4               1
        11      R/W    DUMMY                           0
        14:12   R/W    CORE_SWRCORE_ZCDIB_5            3'b100
        15      R/W    CORE_EN_EAIqX4_5                1
        16      R/W    CORE_ENEAIqX2_H_5               1
        17      R/W    DUMMY                           0
        20:18   R/W    CORE_SWRCORE_ZCDIB_6            3'b100
        21      R/W    CORE_EN_EAIqX4_6                1
        22      R/W    CORE_ENEAIqX2_H_6               1
        23      R/W    DUMMY                           0
        26:24   R/W    CORE_SWRCORE_ZCDIB_7            3'b100
        27      R/W    CORE_EN_EAIqX4_7                1
        28      R/W    CORE_ENEAIqX2_H_7               1
        31:29   R/W    DUMMY                           3'b000
    */
    union
    {
        __IO uint32_t REG_PERION_CORE_SWR_PAR_TAB_ext;
        struct
        {
            __IO uint32_t CORE_SWRCORE_ZCDIB_0: 3;
            __IO uint32_t CORE_EN_EAIqX4_0: 1;
            __IO uint32_t CORE_ENEAIqX2_H_0: 1;
            __IO uint32_t RESERVED_4: 1;
            __IO uint32_t CORE_SWRCORE_ZCDIB_4: 3;
            __IO uint32_t CORE_EN_EAIqX4_4: 1;
            __IO uint32_t CORE_ENEAIqX2_H_4: 1;
            __IO uint32_t RESERVED_3: 1;
            __IO uint32_t CORE_SWRCORE_ZCDIB_5: 3;
            __IO uint32_t CORE_EN_EAIqX4_5: 1;
            __IO uint32_t CORE_ENEAIqX2_H_5: 1;
            __IO uint32_t RESERVED_2: 1;
            __IO uint32_t CORE_SWRCORE_ZCDIB_6: 3;
            __IO uint32_t CORE_EN_EAIqX4_6: 1;
            __IO uint32_t CORE_ENEAIqX2_H_6: 1;
            __IO uint32_t RESERVED_1: 1;
            __IO uint32_t CORE_SWRCORE_ZCDIB_7: 3;
            __IO uint32_t CORE_EN_EAIqX4_7: 1;
            __IO uint32_t CORE_ENEAIqX2_H_7: 1;
            __IO uint32_t RESERVED_0: 3;
        } BITS_2FC;
    } u_2FC;

    /* 0x0300       0x4000_0300
        7:0     R/W    r_PON_DLYSEL_SPIM0              8'h0
        15:8    R/W    r_PON_DLYSEL_SPIM1              8'h0
        23:16   R/W    DUMMY                           8'h0
        24      R/W    r_flash0_clk_inv_en             1'b0
        25      R/W    r_flash1_clk_inv_en             1'b0
        27:26   R/W    r_flash0_phy_clk_sel            2'h0
        29:28   R/W    r_flash1_phy_clk_sel            2'h0
        31:30   R/W    DUMMY                           2'h0
    */
    union
    {
        __IO uint32_t REG_PERION_PON_PERI_DLYSEL_CTRL;
        struct
        {
            __IO uint32_t r_PON_DLYSEL_SPIM0: 8;
            __IO uint32_t r_PON_DLYSEL_SPIM1: 8;
            __IO uint32_t RESERVED_1: 8;
            __IO uint32_t r_flash0_clk_inv_en: 1;
            __IO uint32_t r_flash1_clk_inv_en: 1;
            __IO uint32_t r_flash0_phy_clk_sel: 2;
            __IO uint32_t r_flash1_phy_clk_sel: 2;
            __IO uint32_t RESERVED_0: 2;
        } BITS_300;
    } u_300;

    /* 0x0304       0x4000_0304
        15:0    R/W    DUMMY                           16'h0001
        31:16   R/W    DUMMY                           16'h1FC0
    */
    union
    {
        __IO uint32_t REG_PERION_DUMMY_20;
        struct
        {
            __IO uint32_t RESERVED_1: 16;
            __IO uint32_t RESERVED_0: 16;
        } BITS_304;
    } u_304;

    /* 0x0308       0x4000_0308
        0       R/W    r_PON_SPIO_MST                  1'b0
        3:1     R/W    DUMMY                           3'b000
        4       R/W    r_PON_SPI2_BRG_EN               1'b0
        5       R/W    r_PON_SPI2_H2S_BRG_EN           1'b0
        6       R/W    r_PON_SPI1_BRG_EN               1'b0
        7       R/W    r_PON_SPI1_H2S_BRG_EN           1'b0
        8       R/W    r_PON_SPI0_BRG_EN               1'b0
        9       R/W    r_PON_SPI0_H2S_BRG_EN           1'b0
        31:10   R/W    DUMMY                           22'h0
    */
    union
    {
        __IO uint32_t REG_PERION_PON_SPI_FUNC_CTRL;
        struct
        {
            __IO uint32_t r_PON_SPIO_MST: 1;
            __IO uint32_t RESERVED_1: 3;
            __IO uint32_t r_PON_SPI2_BRG_EN: 1;
            __IO uint32_t r_PON_SPI2_H2S_BRG_EN: 1;
            __IO uint32_t r_PON_SPI1_BRG_EN: 1;
            __IO uint32_t r_PON_SPI1_H2S_BRG_EN: 1;
            __IO uint32_t r_PON_SPI0_BRG_EN: 1;
            __IO uint32_t r_PON_SPI0_H2S_BRG_EN: 1;
            __IO uint32_t RESERVED_0: 22;
        } BITS_308;
    } u_308;

    /* 0x030C       0x4000_030c
        7:0     R/W    r_PON_DLYSEL_SPIM2              8'h0
        15:8    R/W    r_PON_DLYSEL_SPIM3              8'h0
        23:16   R/W    r_PON_DLYSEL_SPIM4              8'h0
        24      R/W    r_flash2_clk_inv_en             1'b0
        25      R/W    r_flash3_clk_inv_en             1'b0
        26      R/W    r_flash4_clk_inv_en             1'b0
        28:27   R/W    r_flash2_phy_clk_sel            2'h0
        30:29   R/W    r_flash3_phy_clk_sel            2'h0
        31      R/W    DUMMY                           1'b0
    */
    union
    {
        __IO uint32_t REG_PERION_PON_PERI_DLYSEL_CTRL_2;
        struct
        {
            __IO uint32_t r_PON_DLYSEL_SPIM2: 8;
            __IO uint32_t r_PON_DLYSEL_SPIM3: 8;
            __IO uint32_t r_PON_DLYSEL_SPIM4: 8;
            __IO uint32_t r_flash2_clk_inv_en: 1;
            __IO uint32_t r_flash3_clk_inv_en: 1;
            __IO uint32_t r_flash4_clk_inv_en: 1;
            __IO uint32_t r_flash2_phy_clk_sel: 2;
            __IO uint32_t r_flash3_phy_clk_sel: 2;
            __IO uint32_t RESERVED_0: 1;
        } BITS_30C;
    } u_30C;

    __IO uint32_t RSVD_0x310[4];

    /* 0x0320       0x4000_0320
        15:0    R/W    DUMMY                           16'h0
        31:16   R/W    DUMMY                           16'h0
    */
    union
    {
        __IO uint32_t REG_PERION_DUMMY_21;
        struct
        {
            __IO uint32_t RESERVED_1: 16;
            __IO uint32_t RESERVED_0: 16;
        } BITS_320;
    } u_320;

    /* 0x0324       0x4000_0324
        31:0    R/W    r_swr_ss_lut_0                  32'h0
    */
    union
    {
        __IO uint32_t REG_PERION_SWR_SS_LUT_0;
        struct
        {
            __IO uint32_t r_swr_ss_lut_0: 32;
        } BITS_324;
    } u_324;

    /* 0x0328       0x4000_0328
        31:0    R/W    r_swr_ss_lut_1                  32'h0
    */
    union
    {
        __IO uint32_t REG_PERION_SWR_SS_LUT_1;
        struct
        {
            __IO uint32_t r_swr_ss_lut_1: 32;
        } BITS_328;
    } u_328;

    /* 0x032C       0x4000_032c
        3:0     R/W    DUMMY                           4'h8
        15:4    R/W    DUMMY                           12'h090
        31:16   R/W    r_swr_ss_config                 16'h0001
    */
    union
    {
        __IO uint32_t REG_PERION_RG0X_40M, _SWR_SS_CONFIG;
        struct
        {
            __IO uint32_t RESERVED_1: 4;
            __IO uint32_t RESERVED_0: 12;
            __IO uint32_t r_swr_ss_config: 16;
        } BITS_32C;
    } u_32C;

    /* 0x0330       0x4000_0330
        0       RW     rst_n_aac                       1'b0
        1       RW     Offset_plus                     1'b0
        7:2     RW     XAAC_GM_offset                  6'h0
        8       RW     GM_STEP                         1'b0
        14:9    RW     GM_INIT                         6'h3F
        17:15   RW     XTAL_CLK_SET                    3'b101
        23:18   RW     GM_STUP                         6'h3F
        29:24   RW     GM_MANUAL                       6'h1F
        30      RW     r_EN_XTAL_AAC_DIGI              1'b0
        31      RW     r_EN_XTAL_AAC_TRIG              1'b0
    */
    union
    {
        __IO uint32_t REG_PERION_AAC_CTRL_0;
        struct
        {
            __IO uint32_t rst_n_aac: 1;
            __IO uint32_t Offset_plus: 1;
            __IO uint32_t XAAC_GM_offset: 6;
            __IO uint32_t GM_STEP: 1;
            __IO uint32_t GM_INIT: 6;
            __IO uint32_t XTAL_CLK_SET: 3;
            __IO uint32_t GM_STUP: 6;
            __IO uint32_t GM_MANUAL: 6;
            __IO uint32_t r_EN_XTAL_AAC_DIGI: 1;
            __IO uint32_t r_EN_XTAL_AAC_TRIG: 1;
        } BITS_330;
    } u_330;

    /* 0x0334       0x4000_0334
        0       R      XAAC_BUSY                       1'b0
        1       R      XAAC_READY                      1'b0
        7:2     R      XTAL_GM_OUT                     6'h1F
        11:8    R      xaac_curr_state                 4'h0
        12      R/W    EN_XTAL_AAC_GM                  1'b0
        13      R/W    EN_XTAL_AAC_PKDET               1'b0
        14      R      XTAL_PKDET_OUT                  1'b0
        15      R/W    DUMMY                           1'b0
        31:16   R/W    DUMMY                           16'h0129
    */
    union
    {
        __IO uint32_t REG_PERION_AAC_CTRL_1;
        struct
        {
            __I uint32_t XAAC_BUSY: 1;
            __I uint32_t XAAC_READY: 1;
            __I uint32_t XTAL_GM_OUT: 6;
            __I uint32_t xaac_curr_state: 4;
            __IO uint32_t EN_XTAL_AAC_GM: 1;
            __IO uint32_t EN_XTAL_AAC_PKDET: 1;
            __I uint32_t XTAL_PKDET_OUT: 1;
            __IO uint32_t RESERVED_1: 1;
            __IO uint32_t RESERVED_0: 16;
        } BITS_334;
    } u_334;

    /* 0x0338       0x4000_0338
        0       R/W    disable_pll_pre_gating          1'b0
        17:1    R/W    DUMMY                           16'h0
        19:18   R      RSVD                            2'h0
        20      R      BT_READY_PLL3                   1'h0
        21      R      BT_READY_PLL4                   1'h0
        23:22   R      RSVD                            2'h0
        24      R      BT_READY_PLL                    1'b0
        25      R      XTAL_OK                         1'b0
        26      R      OSC32K_OK                       1'b0
        27      R      XTAL32K_OK                      1'b0
        28      R      BT_READY_PLL2                   1'b0
        31:29   R      BT_DBGOUT_AFESDM_PLL2[2:0]      3'b000
    */
    union
    {
        __IO uint32_t REG_PERION_0x338;
        struct
        {
            __IO uint32_t disable_pll_pre_gating: 1;
            __IO uint32_t RESERVED_2: 17;
            __I uint32_t RESERVED_1: 2;
            __I uint32_t BT_READY_PLL3: 1;
            __I uint32_t BT_READY_PLL4: 1;
            __I uint32_t RESERVED_0: 2;
            __I uint32_t BT_READY_PLL: 1;
            __I uint32_t XTAL_OK: 1;
            __I uint32_t OSC32K_OK: 1;
            __I uint32_t XTAL32K_OK: 1;
            __I uint32_t BT_READY_PLL2: 1;
            __I uint32_t BT_DBGOUT_AFESDM_PLL2_2_0: 3;
        } BITS_338;
    } u_338;

    /* 0x033C       0x4000_033c
        0       R/W    resetn                          1'b0
        1       R/W    EN_XTAL_PDCK_DIGI               1'b0
        2       R/W    PDCK_SEARCH_MODE                1'b0
        4:3     R/W    PDCK_WAIT_CYC[1:0]              2'b01
        9:5     R/W    VREF_MANUAL[4:0]                5'h1F
        14:10   R/W    VREF_INIT[4:0]                  5'h1F
        16:15   R/W    XTAL_PDCK_UNIT[1:0]             2'b01
        21:17   R/W    XPDCK_VREF_SEL[4:0]             5'h2
        22      R/W    PDCK_LPOW                       1'b0
        27:23   R/W    DUMMY                           5'h0
        31:28   R      pdck_state[3:0]                 4'h0
    */
    union
    {
        __IO uint32_t REG_PERION_XTAL_PDCK;
        struct
        {
            __IO uint32_t resetn: 1;
            __IO uint32_t EN_XTAL_PDCK_DIGI: 1;
            __IO uint32_t PDCK_SEARCH_MODE: 1;
            __IO uint32_t PDCK_WAIT_CYC_1_0: 2;
            __IO uint32_t VREF_MANUAL_4_0: 5;
            __IO uint32_t VREF_INIT_4_0: 5;
            __IO uint32_t XTAL_PDCK_UNIT_1_0: 2;
            __IO uint32_t XPDCK_VREF_SEL_4_0: 5;
            __IO uint32_t PDCK_LPOW: 1;
            __IO uint32_t RESERVED_0: 5;
            __I uint32_t pdck_state_3_0: 4;
        } BITS_33C;
    } u_33C;

    /* 0x0340       0x4000_0340
        0       R/W    iso_int_out_en                  1'b0
        1       R/W    r_usb_clk_src_sel               1'h0
        2       R/W    r_usb_clk_div_en                1'h0
        5:3     R/W    r_usb_clk_div_sel               1'h0
        8:6     R/W    DUMMY                           3'h0
        9       R/W    r_sdio_clk_src_sel              1'h0
        10      R/W    r_sdio_clk_div_en               1'h0
        13:11   R/W    r_sdio_clk_div_sel              1'h0
        14      R/W    r_spi0_src_clk_sel              1'h0
        15      R/W    r_DSP2_CLK_SEL                  1'h0
        16      R/W    r_DSP2_CLK_SRC_EN               1'h0
        18:17   R/W    r_DSP2_CLK_DIV_SEL              2'h0
        19      R/W    r_DSP1_CLK_SRC_EN               1'h0
        31:20   R/W    DUMMY                           12'h0
    */
    union
    {
        __IO uint32_t REG_PERION_0x340;
        struct
        {
            __IO uint32_t iso_int_out_en: 1;
            __IO uint32_t r_usb_clk_src_sel: 1;
            __IO uint32_t r_usb_clk_div_en: 1;
            __IO uint32_t r_usb_clk_div_sel: 3;
            __IO uint32_t RESERVED_1: 3;
            __IO uint32_t r_sdio_clk_src_sel: 1;
            __IO uint32_t r_sdio_clk_div_en: 1;
            __IO uint32_t r_sdio_clk_div_sel: 3;
            __IO uint32_t r_spi0_src_clk_sel: 1;
            __IO uint32_t r_DSP2_CLK_SEL: 1;
            __IO uint32_t r_DSP2_CLK_SRC_EN: 1;
            __IO uint32_t r_DSP2_CLK_DIV_SEL: 2;
            __IO uint32_t r_DSP1_CLK_SRC_EN: 1;
            __IO uint32_t RESERVED_0: 12;
        } BITS_340;
    } u_340;

    /* 0x0344       0x4000_0344
        7:0     R/W    dbnc_cnt_limit                  8'h1
        11:8    R/W    dbnc_div_sel                    4'h1
        12      R/W    dbnc_div_en                     1'b0
        15:13   R/W    DUMMY                           3'b000
        23:16   R/W    dbnc1_cnt_limit                 8'h1
        27:24   R/W    dbnc1_div_sel                   4'h1
        28      R/W    dbnc1_div_en                    1'b0
        31:29   R/W    DUMMY                           3'b000
    */
    union
    {
        __IO uint32_t REG_PERION_PON_GPIO_DBNC_CTRL;
        struct
        {
            __IO uint32_t dbnc_cnt_limit: 8;
            __IO uint32_t dbnc_div_sel: 4;
            __IO uint32_t dbnc_div_en: 1;
            __IO uint32_t RESERVED_1: 3;
            __IO uint32_t dbnc1_cnt_limit: 8;
            __IO uint32_t dbnc1_div_sel: 4;
            __IO uint32_t dbnc1_div_en: 1;
            __IO uint32_t RESERVED_0: 3;
        } BITS_344;
    } u_344;

    /* 0x0348       0x4000_0348
        2:0     R/W    r_timer_div_sel                 3'b000
        3       R/W    r_timer_div_en                  1'b0
        4       R/W    r_timer_cg_en                   1'b0
        5       R/W    r_timer_clk_src_sel_0           1'b1
        6       R/W    r_timer_clk_src_sel_1           1'b0
        7       R/W    r_timer_mux_1_clk_cg_en         1'b0
        10:8    R/W    BIT_PERI_GT8_CLK_DIV            3'b000
        13:11   R/W    BIT_PERI_GT9_CLK_DIV            3'b000
        16:14   R/W    BIT_PERI_GT10_CLK_DIV           3'b000
        19:17   R/W    BIT_PERI_GT11_CLK_DIV           3'b000
        22:20   R/W    BIT_PERI_GT12_CLK_DIV           3'b000
        25:23   R/W    BIT_PERI_GT13_CLK_DIV           3'b000
        28:26   R/W    BIT_PERI_GT14_CLK_DIV           3'b000
        31:29   R/W    BIT_PERI_GT15_CLK_DIV           3'b000
    */
    union
    {
        __IO uint32_t REG_PERION_REG_PERI_GTIMER_CLK_CTRL;
        struct
        {
            __IO uint32_t r_timer_div_sel: 3;
            __IO uint32_t r_timer_div_en: 1;
            __IO uint32_t r_timer_cg_en: 1;
            __IO uint32_t r_timer_clk_src_sel_0: 1;
            __IO uint32_t r_timer_clk_src_sel_1: 1;
            __IO uint32_t r_timer_mux_1_clk_cg_en: 1;
            __IO uint32_t BIT_PERI_GT8_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT9_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT10_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT11_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT12_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT13_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT14_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT15_CLK_DIV: 3;
        } BITS_348;
    } u_348;

    /* 0x034C       0x4000_034c
        7:0     R/W    dbnc_cnt_limit                  8'h0
        11:8    R/W    dbnc_div_sel                    4'h0
        12      R/W    dbnc_div_en                     1'h0
        31:13   R/W    DUMMY                           19'h0
    */
    union
    {
        __IO uint32_t REG_PERION_PON_GPIOC_DBNC_CTRL;
        struct
        {
            __IO uint32_t dbnc_cnt_limit: 8;
            __IO uint32_t dbnc_div_sel: 4;
            __IO uint32_t dbnc_div_en: 1;
            __IO uint32_t RESERVED_0: 19;
        } BITS_34C;
    } u_34C;

    __IO uint32_t RSVD_0x350[3];

    /* 0x035C       0x4000_035c
        2:0     R/W    BIT_PERI_GT5_CLK_DIV            3'b000
        5:3     R/W    BIT_PERI_GT6_CLK_DIV            3'b000
        8:6     R/W    BIT_PERI_GT7_CLK_DIV            3'b000
        10:9    R/W    BIT_PERI_UART0_CLK_DIV          2'b00
        12:11   R/W    BIT_PERI_UART1_CLK_DIV          2'b00
        14:13   R/W    BIT_PERI_UART2_CLK_DIV          2'b00
        16:15   R/W    BIT_PERI_I2C0_CLK_DIV           2'b00
        18:17   R/W    BIT_PERI_I2C1_CLK_DIV           2'b00
        21:19   R/W    BIT_PERI_SPI0_CLK_DIV           3'b000
        24:22   R/W    BIT_PERI_SPI1_CLK_DIV           3'b000
        27:25   R/W    BIT_PERI_SPI2_CLK_DIV           3'b000
        29:28   R/W    BIT_PERI_I2C2_CLK_DIV           2'b00
        31:30   R/W    BIT_PERI_UART3_CLK_DIV          2'b00
    */
    union
    {
        __IO uint32_t REG_PERION_REG_PERI_GTIMER_CLK_SRC1;
        struct
        {
            __IO uint32_t BIT_PERI_GT5_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT6_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT7_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_UART0_CLK_DIV: 2;
            __IO uint32_t BIT_PERI_UART1_CLK_DIV: 2;
            __IO uint32_t BIT_PERI_UART2_CLK_DIV: 2;
            __IO uint32_t BIT_PERI_I2C0_CLK_DIV: 2;
            __IO uint32_t BIT_PERI_I2C1_CLK_DIV: 2;
            __IO uint32_t BIT_PERI_SPI0_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_SPI1_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_SPI2_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_I2C2_CLK_DIV: 2;
            __IO uint32_t BIT_PERI_UART3_CLK_DIV: 2;
        } BITS_35C;
    } u_35C;

    /* 0x0360       0x4000_0360
        0       R/W    BIT_PERI_GT3_CLK_SEL            1'b0
        1       R/W    BIT_PERI_GT5_CLK_SEL            1'b0
        2       R/W    BIT_PERI_GT7_CLK_SEL            1'b0
        3       R/W    BIT_PERI_GT9_CLK_SEL            1'b0
        4       R/W    BIT_PERI_GT11_CLK_SEL           1'b0
        7:5     R/W    DUMMY                           3'b000
        8       R/W    BIT_TIMER_CLK_32K_EN            1'b1
        9       R/W    BIT_TIMER_CLK_f40M_EN           1'b1
        10      R/W    r_timer_apb_clk_disable         1'b0
        11      R/W    DUMMY                           1'b0
        12      R/W    r_clk_timer_f1m_en              1'b0
        15:13   R/W    r_clk_cpu_tick_div_sel          3'b000
        18:16   R/W    BIT_PERI_GT0_CLK_DIV            3'b000
        21:19   R/W    BIT_PERI_GT1_CLK_DIV            3'b000
        24:22   R/W    BIT_PERI_GT2_CLK_DIV            3'b000
        27:25   R/W    BIT_PERI_GT3_CLK_DIV            3'b000
        30:28   R/W    BIT_PERI_GT4_CLK_DIV            3'b000
        31      R/W    DUMMY                           1'b0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_PERI_GTIMER_CLK_SRC0;
        struct
        {
            __IO uint32_t BIT_PERI_GT3_CLK_SEL: 1;
            __IO uint32_t BIT_PERI_GT5_CLK_SEL: 1;
            __IO uint32_t BIT_PERI_GT7_CLK_SEL: 1;
            __IO uint32_t BIT_PERI_GT9_CLK_SEL: 1;
            __IO uint32_t BIT_PERI_GT11_CLK_SEL: 1;
            __IO uint32_t RESERVED_2: 3;
            __IO uint32_t BIT_TIMER_CLK_32K_EN: 1;
            __IO uint32_t BIT_TIMER_CLK_f40M_EN: 1;
            __IO uint32_t r_timer_apb_clk_disable: 1;
            __IO uint32_t RESERVED_1: 1;
            __IO uint32_t r_clk_timer_f1m_en: 1;
            __IO uint32_t r_clk_cpu_tick_div_sel: 3;
            __IO uint32_t BIT_PERI_GT0_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT1_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT2_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT3_CLK_DIV: 3;
            __IO uint32_t BIT_PERI_GT4_CLK_DIV: 3;
            __IO uint32_t RESERVED_0: 1;
        } BITS_360;
    } u_360;

    /* 0x0364       0x4000_0364
        7:0     R/W    dead_zone_size                  8'h0
        8       R/W    emg_stop                        1'h0
        10:9    RW     stop_state                      2'h0
        11      R/W    DUMMY                           1'h0
        12      R/W    dead_zone_en                    1'h0
        13      R/W    pwm_n_r_new                     1'h0
        28:14   R/W    DUMMY                           15'h0
        29      R/W    r_mcu_clk_dsp_timer_32k_en      1'h0
        30      R/W    r_mcu_clk_dsp_timer_f1m_en      1'h0
        31      R/W    r_mcu_clk_dsp_timer_5m_en       1'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_PERI_PWM0_DZONE_CTRL;
        struct
        {
            __IO uint32_t dead_zone_size: 8;
            __IO uint32_t emg_stop: 1;
            __IO uint32_t stop_state: 2;
            __IO uint32_t RESERVED_1: 1;
            __IO uint32_t dead_zone_en: 1;
            __IO uint32_t pwm_n_r_new: 1;
            __IO uint32_t RESERVED_0: 15;
            __IO uint32_t r_mcu_clk_dsp_timer_32k_en: 1;
            __IO uint32_t r_mcu_clk_dsp_timer_f1m_en: 1;
            __IO uint32_t r_mcu_clk_dsp_timer_5m_en: 1;
        } BITS_364;
    } u_364;

    /* 0x0368       0x4000_0368
        7:0     R/W    dead_zone_size                  8'h0
        8       R/W    emg_stop                        1'h0
        10:9    RW     stop_state                      2'h0
        11      R/W    DUMMY                           1'h0
        12      R/W    dead_zone_en                    1'h0
        13      R/W    pwm_n_r_new                     1'h0
        15:14   R/W    DUMMY                           2'h0
        31:16   R/W    DUMMY                           16'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_PERI_PWM1_DZONE_CTRL;
        struct
        {
            __IO uint32_t dead_zone_size: 8;
            __IO uint32_t emg_stop: 1;
            __IO uint32_t stop_state: 2;
            __IO uint32_t RESERVED_2: 1;
            __IO uint32_t dead_zone_en: 1;
            __IO uint32_t pwm_n_r_new: 1;
            __IO uint32_t RESERVED_1: 2;
            __IO uint32_t RESERVED_0: 16;
        } BITS_368;
    } u_368;

    /* 0x036C       0x4000_036c
        0       R/W1O  otp_reg_otp_w1o_rp_all          0
        1       R/W1O  otp_reg_w1o_wp_all              0
        2       R/W    otp_reg_key_autoload_en         0
        3       R/W1O  otp_reg_w1o_key_auto_load_lock  0
        4       R      otp_reg_key_auto_load_rdy       0
        5       R      otp_reg_flag_key2_not_burned    0
        6       R      otp_reg_flag_key2_sanity_pass   0
        7       R      otp_reg_flag_key3_not_burned    0
        8       R      otp_reg_flag_key3_sanity_pass   0
        9       R      otp_reg_flag_key4_not_burned    0
        10      R      otp_reg_flag_key4_sanity_pass   0
        31:11   R/W    DUMMY                           21'h0
    */
    union
    {
        __IO uint32_t REG_PERION_otp_key_control_otp_protection;
        struct
        {
            __IO uint32_t otp_reg_otp_w1o_rp_all: 1;
            __IO uint32_t otp_reg_w1o_wp_all: 1;
            __IO uint32_t otp_reg_key_autoload_en: 1;
            __IO uint32_t otp_reg_w1o_key_auto_load_lock: 1;
            __I uint32_t otp_reg_key_auto_load_rdy: 1;
            __I uint32_t otp_reg_flag_key2_not_burned: 1;
            __I uint32_t otp_reg_flag_key2_sanity_pass: 1;
            __I uint32_t otp_reg_flag_key3_not_burned: 1;
            __I uint32_t otp_reg_flag_key3_sanity_pass: 1;
            __I uint32_t otp_reg_flag_key4_not_burned: 1;
            __I uint32_t otp_reg_flag_key4_sanity_pass: 1;
            __IO uint32_t RESERVED_0: 21;
        } BITS_36C;
    } u_36C;

    /* 0x0370       0x4000_0370
        31:0    R/W1O  DUMMY                           32'h0
    */
    __IO uint32_t REG_PERION_efuse_protection;

    /* 0x0374       0x4000_0374
        31:0    R/W1O  DUMMY                           32'h0
    */
    __IO uint32_t REG_PERION_efuse_protection_2;

    /* 0x0378       0x4000_0378
        31:0    R/W1O  DUMMY                           32'h0
    */
    __IO uint32_t REG_PERION_efuse_protection_3;

    /* 0x037C       0x4000_037c
        31:0    R/W1O  DUMMY                           32'h0
    */
    __IO uint32_t REG_PERION_efuse_protection_4;

    /* 0x0380       0x4000_0380
        6:0     R/W    CORE_VOUT_TUNE_PWM_H_0          7'h2B
        7       R/W    DUMMY                           0
        14:8    R/W    CORE_VREFPFM_H_0                7'h30
        31:15   R/W    DUMMY                           17'h0
    */
    union
    {
        __IO uint32_t REG_PERION_CORE_SWR_PAR_TAB_0;
        struct
        {
            __IO uint32_t CORE_VOUT_TUNE_PWM_H_0: 7;
            __IO uint32_t RESERVED_1: 1;
            __IO uint32_t CORE_VREFPFM_H_0: 7;
            __IO uint32_t RESERVED_0: 17;
        } BITS_380;
    } u_380;

    /* 0x0384       0x4000_0384
        6:0     R/W    CORE_VOUT_TUNE_PWM_H_4          7'h2B
        7       R/W    DUMMY                           0
        14:8    R/W    CORE_VOUT_TUNE_PWM_H_5          7'h2B
        15      R/W    DUMMY                           0
        22:16   R/W    CORE_VOUT_TUNE_PWM_H_6          7'h2B
        23      R/W    DUMMY                           0
        30:24   R/W    CORE_VOUT_TUNE_PWM_H_7          7'h2B
        31      R/W    DUMMY                           0
    */
    union
    {
        __IO uint32_t REG_PERION_CORE_SWR_PAR_TAB_4_5_6_7;
        struct
        {
            __IO uint32_t CORE_VOUT_TUNE_PWM_H_4: 7;
            __IO uint32_t RESERVED_3: 1;
            __IO uint32_t CORE_VOUT_TUNE_PWM_H_5: 7;
            __IO uint32_t RESERVED_2: 1;
            __IO uint32_t CORE_VOUT_TUNE_PWM_H_6: 7;
            __IO uint32_t RESERVED_1: 1;
            __IO uint32_t CORE_VOUT_TUNE_PWM_H_7: 7;
            __IO uint32_t RESERVED_0: 1;
        } BITS_384;
    } u_384;

    /* 0x0388       0x4000_0388
        6:0     R/W    CORE_VREFPFM_H_4                7'h37
        7       R/W    DUMMY                           0
        14:8    R/W    CORE_VREFPFM_H_5                7'h37
        15      RW     DUMMY                           0
        22:16   R/W    CORE_VREFPFM_H_6                7'h37
        23      R/W    DUMMY                           0
        30:24   R/W    CORE_VREFPFM_H_7                7'h37
        31      RW     DUMMY                           0
    */
    union
    {
        __IO uint32_t REG_PERION_CORE_SWR_PAR_TAB_4_5_6_7;
        struct
        {
            __IO uint32_t CORE_VREFPFM_H_4: 7;
            __IO uint32_t RESERVED_3: 1;
            __IO uint32_t CORE_VREFPFM_H_5: 7;
            __IO uint32_t RESERVED_2: 1;
            __IO uint32_t CORE_VREFPFM_H_6: 7;
            __IO uint32_t RESERVED_1: 1;
            __IO uint32_t CORE_VREFPFM_H_7: 7;
            __IO uint32_t RESERVED_0: 1;
        } BITS_388;
    } u_388;

    /* 0x038C       0x4000_038c
        31:0    R/W    r_swr_ss_lut_2                  32'h0
    */
    union
    {
        __IO uint32_t REG_PERION_SWR_SS_LUT_2;
        struct
        {
            __IO uint32_t r_swr_ss_lut_2: 32;
        } BITS_38C;
    } u_38C;

    /* 0x0390       0x4000_0390
        31:0    R/W    r_swr_ss_lut_3                  32'h0
    */
    union
    {
        __IO uint32_t REG_PERION_SWR_SS_LUT_3;
        struct
        {
            __IO uint32_t r_swr_ss_lut_3: 32;
        } BITS_390;
    } u_390;

    /* 0x0394       0x4000_0394
        31:0    R/W    r_swr_ss_lut_4                  32'h0
    */
    union
    {
        __IO uint32_t REG_PERION_SWR_SS_LUT_4;
        struct
        {
            __IO uint32_t r_swr_ss_lut_4: 32;
        } BITS_394;
    } u_394;

    /* 0x0398       0x4000_0398
        31:0    R/W    r_swr_ss_lut_5                  32'h0
    */
    union
    {
        __IO uint32_t REG_PERION_SWR_SS_LUT_5;
        struct
        {
            __IO uint32_t r_swr_ss_lut_5: 32;
        } BITS_398;
    } u_398;

    __IO uint32_t RSVD_0x39c[21];

    /* 0x03F0       0x4000_03f0
        1:0     R/W    occ_0_dbg_en                    2'h0
        3:2     R/W    occ_1_dbg_en                    2'h0
        5:4     R/W    occ_2_dbg_en                    2'h0
        7:6     R/W    occ_3_dbg_en                    2'h0
        9:8     R/W    occ_4_dbg_en                    2'h0
        15:10   R/W    DUMMY                           6'h0
        21:16   R/W    occ_dbg_sel                     6'h0
        31:22   R/W    DUMMY                           10'h0
    */
    union
    {
        __IO uint32_t REG_PERION_0x3F0;
        struct
        {
            __IO uint32_t occ_0_dbg_en: 2;
            __IO uint32_t occ_1_dbg_en: 2;
            __IO uint32_t occ_2_dbg_en: 2;
            __IO uint32_t occ_3_dbg_en: 2;
            __IO uint32_t occ_4_dbg_en: 2;
            __IO uint32_t RESERVED_1: 6;
            __IO uint32_t occ_dbg_sel: 6;
            __IO uint32_t RESERVED_0: 10;
        } BITS_3F0;
    } u_3F0;

    /* 0x03F4       0x4000_03f4
        31:0    R/W    DUMMY                           32'h0
    */
    __IO uint32_t REG_PERION_0x3F4;

    __IO uint32_t RSVD_0x3f8[1];

    /* 0x03FC       0x4000_03fc
        14:0    R/W    DUMMY                           14'h0
        15      R      CABLE_CONNECT                   0
        23:16   R      SLB_PACKET_NUM                  8'h0
        31:24   R      SLB_ERR_NUM                     8'h0
    */
    union
    {
        __IO uint32_t REG_PERION_REG_PERION_0x3FC;
        struct
        {
            __IO uint32_t RESERVED_0: 15;
            __I uint32_t CABLE_CONNECT: 1;
            __I uint32_t SLB_PACKET_NUM: 8;
            __I uint32_t SLB_ERR_NUM: 8;
        } BITS_3FC;
    } u_3FC;


} SYS_BLKCTRL_TypeDef;

/* ================================================================================ */
/* ================                   Peripheral                   ================ */
/* ================================================================================ */

/**
  * @brief Peripheral Structure. (Peripheral)
  */
typedef struct
{
    uint32_t RSVD0[134];                       /**< Reserved */
    __IO uint32_t    PERI_FUNC0_EN;            /**< 0x218 */
    __IO uint32_t    PERI_FUNC1_EN;            /**< 0x21C */
    __IO uint32_t    PERI_BD_FUNC0_EN;         /**< 0x220 */
    uint32_t RSVD1[3];                         /**< Reserved */
    __IO uint32_t    PERI_CLK_CTRL;            /**< 0x230 */
    __IO uint32_t    PERI_CLK_CTRL0;           /**< 0x234 */
    __IO uint32_t    PERI_CLK_CTRL1;           /**< 0x238 */
} PHERIPHERIAL_TypeDef;

/* ================================================================================ */
/* ================              Peripheral Interrupt              ================ */
/* ================================================================================ */

/**
  * @brief Peripheral Interrupt Structure. (Peripheral Interrupt)
  */
typedef struct
{
    __IO uint32_t RSVD0;       /**< 0x00 */
    __IO uint32_t STATUS;      /**< 0x04 */
    __IO uint32_t MODE;        /**< 0x08, 0: high level active, 1: edge trigger */
    __IO uint32_t EN;          /**< 0x0C */
    __IO uint32_t RSVD1;       /**< 0x10 */
    __IO uint32_t RSVD2;       /**< 0x14 */
    __IO uint32_t EDGE_MODE;   /**< 0x18, 0: rising edge (HW default), 1: both edge */
} PERI_INT_TypeDef;

/*
0x00E0  OTP R range protection
    [31]    W1O 1'b0    reg_otp_rp_en   enable R range protection for otp                       Y
    [30]    W1O 1'b0    reg_otp_rp_lock lock reg_otp_rp_start_addr and reg_otp_rp_end_addr      Y
    [29:26] R   4'd0    RSVD
    [25:13] R/W 13'd0   reg_otp_rp_end_addr R range protection end address                      Y
    [12:0]  R/W 13'd0   reg_otp_rp_start_addr   R range protection start address                Y
0x00E4  OTP W range protection
    [31]    W1O 1'b0    reg_otp_wp_en   enable W range protection for otp                       Y
    [30]    W1O 1'b0    reg_otp_wp_lock lock reg_otp_wp_start_addr and reg_otp_wp_end_addr      Y
    [29:26] R   4'd0    RSVD
    [25:13] R/W 13'd0   reg_otp_wp_end_addr W range protection end address                      Y
    [12:0]  R/W 13'd0   reg_otp_wp_start_addr   W range protection start address                Y
*/
typedef struct
{
    union
    {
        __IO uint32_t REG_OTP_R_range_protection;
        struct
        {
            __IO uint32_t reg_otp_rp_start_addr: 13;
            __IO uint32_t reg_otp_rp_end_addr: 13;
            __I  uint32_t RSVD: 4;
            __IO uint32_t reg_otp_rp_lock: 1;
            __IO uint32_t reg_otp_rp_en: 1;
        } BITS_00E0;
    } u_00E0;
    union
    {
        __IO uint32_t REG_OTP_W_range_protection;
        struct
        {
            __IO uint32_t reg_otp_wp_start_addr: 13;
            __IO uint32_t reg_otp_wp_end_addr: 13;
            __I  uint32_t RSVD: 4;
            __IO uint32_t reg_otp_wp_lock: 1;
            __IO uint32_t reg_otp_wp_en: 1;
        } BITS_00E4;
    } u_00E4;
} OTP_RANG_PROTECT_TypeDef;

/**
  * @brief SoC_VENDOR Structure. (SoC_VENDOR)
  */

typedef struct
{
    /* 0x00
    15:0    R/W    wdt_divfactor                       16'h1
    23:16   R/W    wdt_en_byte                         8'h0
    24      R/WAC  wdt_clear                           0
    28:25   R/W    Cnt_limit                           4'h0
    29      R/W    wdtaon_en                           1
    30      R/W    wdt_mode                            0
    31      R/W1C  wdt_to                              0
    */
    union
    {
        __IO uint32_t REGWATCH_DOG_TIMER;       /*!< 0x00 */
        struct
        {
            __IO uint32_t wdt_divfactor: 16;    /* Wdt is count with 32.768KHz/(divfactor+1).
                                                   Minimum dividing factor is 1.*/
            __IO uint32_t wdt_en_byte: 8;       /* Set 0xA5 to enable watch dog timer */
            __IO uint32_t wdt_clear: 1;
            __IO uint32_t Cnt_limit: 4;         /* 0: 0x001,1: 0x003,2: 0x007,3: 0x00F,4: 0x01F,
                                                   5: 0x03F,6: 0x07F,7: 0x0FF,8: 0x1FF,9: 0x3FF,
                                                   10: 0x7FF, 11~15: 0xFFF */
            __IO uint32_t wdtaon_en: 1;         /* {Wdt_mode,wdtaon_en} == 00 interrupt cpu,
                                                   10 reset core domain,
                                                   01 reset whole chip except aon reg,
                                                   11 reset whole chip*/
            __IO uint32_t wdt_mode: 1;
            __IO uint32_t wdt_to: 1;            /* Watch dog timer timeout:
                                            when bit 30 = 1 (reset mode) --- 1 cycle pulse
                                            when bit 30 = 0 (interrupt mode) ---- Write 1 clear */

        };
    };

    /* 0x04
    0       R/W1C  spic0_intr                          0
    1       R/W1C  spic1_intr                          0
    2       R/W1C  spic2_intr                          0
    3       R/W1C  spic3_intr                          0
    6:4     R      DUMMY                               7'h0
    7       R/W1C  spdif_intr_rx_r                     0
    8       R/W1C  spdif_intr_tx_r                     0
    9       R/W1C  MBIAS_MBF_DET_L                     0
    10      R/W1C  spi2w_intr                          0
    11      R/W1C  lpcomp_int_r                        0
    12      R/W1C  MBIAS_VBAT_DET_L                    0
    13      R/W1C  MBIAS_ADP_DET_L                     0
    14      R/W1C  HW_ASRC_ISR1                        0
    15      R/W1C  HW_ASRC_ISR2                        0
    16      R/W1C  vb_intr                             0
    17      R/W1C  mailbox_intr                        0
    18      R/W1C  dsp_wdt_to_mcu_intr                 0
    19      R/W1C  tdm0_intr_tx                        0
    20      R/W1C  tdm0_intr_rx                        0
    21      R/W1C  tdm1_intr_tx                        0
    22      R/W1C  tdm1_intr_rx                        0
    23      R/W1C  vad_intr                            0
    24      R/W1C  anc_intr                            0
    25      R/W1C  sp0_intr_tx                         0
    26      R/W1C  sp0_intr_rx                         0
    27      R/W1C  sp1_intr_tx                         0
    28      R/W1C  sp1_intr_rx                         0
    29      R/W1C  utmi_suspend_n                      0
    30      R      DUMMY                               0
    31      R/W1C  trng_int                            0
    */
    union
    {
        __IO uint32_t REG_LOW_PRI_INT_STATUS;   /*!< 0x04, default: 0x0 */
        struct
        {
            __IO uint32_t spic0_intr: 1;
            __IO uint32_t spic1_intr: 1;
            __IO uint32_t spic2_intr: 1;
            __IO uint32_t spic3_intr: 1;
            __I  uint32_t RESERVED_1: 3;
            __IO uint32_t spdif_intr_rx_r: 1;
            __IO uint32_t spdif_intr_tx_r: 1;
            __IO uint32_t MBIAS_MBF_DET_L: 1;
            __IO uint32_t spi2w_intr: 1;
            __IO uint32_t lpcomp_int_r: 1;
            __IO uint32_t MBIAS_VBAT_DET_L: 1;
            __IO uint32_t MBIAS_ADP_DET_L: 1;
            __IO uint32_t HW_ASRC_ISR1: 1;
            __IO uint32_t HW_ASRC_ISR2: 1;
            __IO uint32_t vb_intr: 1;
            __IO uint32_t mailbox_intr: 1;
            __IO uint32_t dsp_wdt_to_mcu_intr: 1;
            __IO uint32_t tdm0_intr_tx: 1;
            __IO uint32_t tdm0_intr_rx: 1;
            __IO uint32_t tdm1_intr_tx: 1;
            __IO uint32_t tdm1_intr_rx: 1;
            __IO uint32_t vad_intr: 1;
            __IO uint32_t anc_intr: 1;
            __IO uint32_t sp0_intr_tx: 1;
            __IO uint32_t sp0_intr_rx: 1;
            __IO uint32_t sp1_intr_tx: 1;
            __IO uint32_t sp1_intr_rx: 1;
            __IO uint32_t utmi_suspend_n: 1;
            __I  uint32_t RESERVED_0: 1;
            __IO uint32_t trng_int: 1;
        };
    };

    /* 0x08
    31:0    R/W    int_mode                            32'hffffffff
    */
    union
    {
        __IO uint32_t REG_LOW_PRI_INT_MODE;         /*!< 0x08, default: 0xFFFFFFFF
                                                     0: high level active, 1: rising edge trigger
                                                     */
        struct
        {
            __IO uint32_t int_mode: 32;
        };
    };

    /* 0x0C
    31:0    R/W    int_en                              32'h0
    */
    union
    {
        __IO uint32_t REG_LOW_PRI_INT_EN;           /*!< 0x0C, default: 0x0 */
        struct
        {
            __IO uint32_t int_en: 32;
        };
    };

    /* 0x10
    0       R      timer_intr1andtimer_intr0           0
    1       R      bluewiz_intr_r                      0
    2       R      hci_dma_intr                        0
    3       R      btverdor_reg_intr                   0
    31:4    R      RSVD                                28'h0
    */
    union
    {
        __I uint32_t BT_MAC_interrupt;         /*!< 0x10 */
        struct
        {
            __I  uint32_t timer_intr1_and_timer_intr0: 1;
            __I  uint32_t bluewiz_intr_r: 1;
            __I  uint32_t hci_dma_intr: 1;
            __I  uint32_t btverdor_reg_intr: 1;
            __I  uint32_t RSVD: 28;
        };
    };

    /* 0x14
    0       R      otg_intr                            0
    1       R      sdiohost_intr                       0
    15:2    R      RSVD                                nan
    16      R      rxi300_intr                         0
    17      R      rdp_intr                            0
    18      R      mcu_ram_err_int                     0
    22:19   R      RSVD                                nan
    23      R/W    cm4_pc_sel                          1
    24      R/W    rxi300_intr_en                      1
    25      R/W    rdp_intr_en                         0
    26      R/W    mcu_ram_err_int_en                  0
    27      R/W    mcu_ram_err_int_clr                 0
    31:28   R      RSVD                                nan
    */
    union
    {
        __IO uint32_t INT_2ND_LVL;              /*!< 0x14 */
        struct
        {
            __I  uint32_t otg_intr: 1;
            __I  uint32_t sdio_host_intr: 1;
            __I  uint32_t RSVD_2: 14;
            __I  uint32_t rxi300_intr: 1;
            __I  uint32_t rdp_intr: 1;
            __I  uint32_t mcu_ram_err_int: 1;
            __I  uint32_t RSVD_1: 4;
            __IO uint32_t cm4_pc_sel: 1;
            __IO uint32_t rxi300_intr_en: 1;
            __IO uint32_t rdp_intr_en: 1;
            __IO uint32_t mcu_ram_err_int_en: 1;
            __IO uint32_t mcu_ram_err_int_clr: 1;
            __I  uint32_t RSVD_0: 4;
        };
    };

    /* 0x18
    31:0    RW     int_pol                             0
    */
    union
    {
        __IO uint32_t Interrupt_edge_option;        /*!< 0x18, default: 0x0
                                                     0:rising edge (HW default),1:both edge
                                                     */
        struct
        {
            __IO uint32_t int_pol: 32;
        };
    };

} SoC_VENDOR_REG_TypeDef;

/* ================================================================================ */
/* ================                    Key Scan                    ================ */
/* ================================================================================ */

/**
  * @brief Key Scan. (KeyScan)
  */

typedef struct                                      /*!< KeyScan Structure */
{

    __IO uint32_t CLKDIV;               /*!< 0x00 */
    __IO uint32_t TIMERCR;              /*!< 0x04 */
    __IO uint32_t CR;                   /*!< 0x08 */
    __IO uint32_t COLCR;                /*!< 0x0C */
    __IO uint32_t ROWCR;                /*!< 0x10 */
    __I  uint32_t FIFODATA;             /*!< 0x14 */
    __IO uint32_t INTMASK;              /*!< 0x18 */
    __IO uint32_t INTCLR;               /*!< 0x1C */
    __I  uint32_t STATUS;               /*!< 0x20 */

} KEYSCAN_TypeDef;

/* ======================================================== */
/* ================                      GPIO                       ================ */
/* ======================================================== */

/**
  * @brief General purpose input and output structure. (GPIO)
  */

typedef struct
{
    __IO uint32_t DATAOUT;              /*!< offset 0x00, Port A Data Register */
    __IO uint32_t DATADIR;              /*!< offset 0x04, Port A Data Direction Register */
    __IO uint32_t DATASRC;              /*!< offset 0x08, Port A Data Source*/
    uint32_t RSVD[9];                   /*!< offset 0x0C ~ 0x2C, Reserved */
    __IO uint32_t INTEN;                /*!< offset 0x30, Interrupt enable */
    __IO uint32_t INTMASK;              /*!< offset 0x34, Interrupt mask */
    __IO uint32_t INTTYPE;              /*!< offset 0x38, Interrupt level */
    __IO uint32_t INTPOLARITY;          /*!< offset 0x3C, Interrupt polarity */
    __IO uint32_t INTSTATUS;            /*!< offset 0x40, Interrupt status */
    __IO uint32_t RAWINTSTATUS;         /*!< offset 0x44, Raw interrupt status */
    __IO uint32_t DEBOUNCE;             /*!< offset 0x48, Debounce enable register */
    __O  uint32_t INTCLR;               /*!< offset 0x4C, Clear interrupt */
    __I  uint32_t DATAIN;               /*!< offset 0x50, External Port A */
    uint32_t RSVD1[3];                  /*!< offset 0x54 ~ 0x5C, Reserved */
    __IO uint32_t LSSYNC;               /*!< offset 0x00, Synchronization level */
    __I  uint32_t IDCODE;               /*!< offset 0x00, GPIO ID code */
    __IO uint32_t INTBOTHEDGE;          /*!< offset 0x00, Interrupt both edge type */
} GPIO_TypeDef;

/* ======================================================= */
/* ================                      PWM                     ================ */
/* ======================================================= */

/**
  * @brief PWM
  */
typedef struct
{
    __IO uint32_t CR;                   /*!< 0x00*/
} PWM_TypeDef;


/* ======================================================= */
/* ================                 Peri clock reg               ================ */
/* ======================================================= */

/**
  * @brief PWM
  */
typedef struct
{
    __IO uint32_t CLKSELE;                    /*!< 0x348*/
    uint32_t RSVD[4];
    __IO uint32_t CLK_SRCL;
    __IO uint32_t CLK_SRCH;                   /*!< 0x360*/
    __IO uint32_t PWM0_CTRL_L;
    __IO uint32_t PWM0_CTRL_H;
    __IO uint32_t PWM1_CTRL_L;
    __IO uint32_t PWM1_CTRL_H;                /*!< 0x370*/
    __IO uint32_t PWM2_CTRL_L;
    __IO uint32_t PWM2_CTRL_H;
    __IO uint32_t PWM3_CTRL_L;
    __IO uint32_t PWM3_CTRL_H;                /*!< 0x380*/
    __IO uint32_t TIM_EVT_CTRL;

} Peri_ClockGate_TypeDef;

/* ================================================================================ */
/* ================                      CACHE                     ================ */
/* ================================================================================ */

/**
  * @brief cache for flash
  */
typedef struct
{
    __IO uint32_t CACHE_ENABLE;          /*!< SPIC cache Enable Register, Address Offset: 0x00*/
    __IO uint32_t FLUSH;                 /*!< Cache Flush register, Address Offset: 0x04*/
    __IO uint32_t INTR;                  /*!< Cache Interrupt register, Address Offset: 0x08*/
    __IO uint32_t RST_CNT;               /*!< Cache Reset Counter register, Address Offset: 0x0C*/
    __IO uint32_t RD_EVT_CNT;            /*!< Cache Read Event Counter register, Address Offset: 0x10*/
    __IO uint32_t HIT_EVT_CNT;           /*!< Cache HIT Event Counter register, Address Offset: 0x14*/
    __IO uint32_t HIT_LSTW_EVT_CNT;      /*!< Cache Hit lastway event counter register, Offset: 0x18*/
    __IO uint32_t RD_PND_CNT;            /*!< Cache Read pending counter register, Offset: 0x1c*/
    __IO uint32_t RSVD[4];               /*!< 0x20~0x2C */
    __IO uint32_t HITWAYBUF_CTRL;        /*!< Hit way buffer control, Address Offset: 0x30 */
    __IO uint32_t CACHE_RAM_CTTRL;       /*!< icache twoway control, Address Offset: 0x34*/
} CACHE_TypeDef;

/* ================================================================================ */
/* ================                      SPIC                      ================ */
/* ================================================================================ */
typedef struct
{
    __IO uint32_t CTRLR0;               /*!< Control reg 0,                         offset: 0x000 */
    __IO uint32_t RX_NDF;               /*!< User mode rx data data frame counter,  offset: 0x004 */
    __IO uint32_t SSIENR;               /*!< Enable reg,                            offset: 0x008 */
    __IO uint32_t MWCR;                 /*!< N/A,                                   offset: 0x00C */
    __IO uint32_t SER;                  /*!< Slave enable reg,                      offset: 0x010 */
    __IO uint32_t BAUDR;                /*!< Baudrate select reg,                   offset: 0x014 */
    __IO uint32_t TXFTLR;               /*!< Tx FIFO threshold level,               offset: 0x018 */
    __IO uint32_t RXFTLR;               /*!< Rx FIFO threshold level,               offset: 0x01C */
    __IO uint32_t TXFLR;                /*!< Tx FIFO level reg,                     offset: 0x020 */
    __IO uint32_t RXFLR;                /*!< Rx FIFO level reg,                     offset: 0x024 */
    __IO uint32_t SR;                   /*!< Status reg,                            offset: 0x028 */
    __IO uint32_t IMR;                  /*!< Interrupt mask reg,                    offset: 0x02C */
    __IO uint32_t ISR;                  /*!< Interrupt status reg,                  offset: 0x030 */
    __IO uint32_t RISR;                 /*!< Raw interrupt status reg,              offset: 0x034 */
    __IO uint32_t TXOICR;               /*!< Tx FIFO overflow interrupt clear reg,  offset: 0x038 */
    __IO uint32_t RXOICR;               /*!< Rx FIFO overflow interrupt clear reg,  offset: 0x03C */
    __IO uint32_t RXUICR;               /*!< Rx FIFO underflow interrupt clear reg, offset: 0x040 */
    __IO uint32_t MSTICR;               /*!< Master error interrupt clear reg,      offset: 0x044 */
    __IO uint32_t ICR;                  /*!< Interrupt clear reg,                   offset: 0x048 */
    __IO uint32_t DMACR;                /*!< DMA control reg,                       offset: 0x04C */
    __IO uint32_t DMATDLR;              /*!< DMA transimit data level reg,          offset: 0x050 */
    __IO uint32_t DMARDLR;              /*!< DMA revceive data level reg,           offset: 0x054 */
    __IO uint32_t IDR;                  /*!< Identiation reg,                       offset: 0x058 */
    __IO uint32_t SPIC_VERSION;         /*!< Version ID reg,                        offset: 0x05C */
    union
    {
        __IO uint8_t  BYTE;
        __IO uint16_t HALF;
        __IO uint32_t WORD;
    } DR[16];                           /*!< Data reg,                              offset: 0x060 */
    __IO uint32_t DM_DR[16];            /*!< Data mask data register,               offset: 0x0A0 */
    __IO uint32_t READ_FAST_SINGLE;     /*!< Fast read data cmd of flash,           offset: 0x0E0 */
    __IO uint32_t READ_DUAL_DATA;       /*!< Dual output read cmd of flash,         offset: 0x0E4 */
    __IO uint32_t READ_DUAL_ADDR_DATA;  /*!< Dual I/O read cmd of flash,            offset: 0x0E8 */
    __IO uint32_t READ_QUAD_DATA;       /*!< Quad output read cmd of flash,         offset: 0x0EC */
    __IO uint32_t READ_QUAD_ADDR_DATA;  /*!< Quad I/O read cmd of flash,            offset: 0x0F0 */
    __IO uint32_t WRITE_SINGLE;         /*!< Page program cmd of flash,             offset: 0x0F4 */
    __IO uint32_t WRITE_DUAL_DATA;      /*!< Dual data input program cmd of flash,  offset: 0x0F8 */
    __IO uint32_t WRITE_DUAL_ADDR_DATA; /*!< Dual addr & data program cmd of flash, offset: 0x0FC */
    __IO uint32_t WRITE_QUAD_DATA;      /*!< Quad data input program cmd of flash,  offset: 0x100 */
    __IO uint32_t WRITE_QUAD_ADDR_DATA; /*!< Quad addr & data program cmd of flash, offset: 0x104 */
    __IO uint32_t WRITE_ENABLE;         /*!< Write enabe cmd of flash,              offset: 0x108 */
    __IO uint32_t READ_STATUS;          /*!< Read status cmd of flash,              offset: 0x10C */
    __IO uint32_t CTRLR2;               /*!< Control reg 2,                         offset: 0x110 */
    __IO uint32_t FBAUDR;               /*!< Fast baudrate select,                  offset: 0x114 */
    __IO uint32_t USER_LENGTH;          /*!< Addr length reg,                       offset: 0x118 */
    __IO uint32_t AUTO_LENGTH;          /*!< Auto addr length reg,                  offset: 0x11C */
    __IO uint32_t VALID_CMD;            /*!< Valid cmd reg,                         offset: 0x120 */
    __IO uint32_t FLASH_SIZE_R;         /*!< Flash size reg,                        offset: 0x124 */
    __IO uint32_t FLUSH_FIFO;           /*!< Flush FIFO reg,                        offset: 0x128 */
    __IO uint32_t DUM_BYTE;             /*!< Dummy byte value,                      offset: 0x12C */
    __IO uint32_t TX_NDF;               /*!< Tx NDF,                                offset: 0x130 */
    __IO uint32_t DEVICE_INFO;          /*!< Device info,                           offset: 0x134 */
    __IO uint32_t TPR0;                 /*!< Timing parameters,                     offset: 0x138 */
    __IO uint32_t AUTO_LENGTH2;         /*!< Auto addr length reg 2,                offset: 0x13C */
} SPIC_TypeDef;

/* ================================================================================ */
/* ================                      PSRAM                     ================ */
/* ================================================================================ */
typedef struct
{
    __IO uint32_t CCR;              /* Configuration control register,          Address offset: 0x00  */
    __IO uint32_t DCR;              /* Device configuration control register,   Address offset: 0x04  */
    __IO uint32_t IOCR0;            /* I/O configuration control regsiter0,     Address offset: 0x08  */
    __IO uint32_t CSR;              /* Controller status register,              Address offset: 0x0c  */
    __IO uint32_t DRR;              /* Device refresh/power-up register,        Address offset: 0x10  */
    __IO uint32_t RSVD0[4];         /* Reserved 0,                              Address offset: 0x14  */
    __IO uint32_t CMD_DPIN_NDGE;    /* Device cmd/addr pin register (NEDGE),    Address offset: 0x24  */
    __IO uint32_t CMD_DPIN;         /* Device cmd/addr pin regsiter (PEDGE),    Address offset: 0x28  */
    __IO uint32_t CR_TDPIN;         /* Tie DPIN register (sw ctrl dfi_reset_n), Address offset: 0x2c  */
    __IO uint32_t MR_INFO;          /* Mode latency information regster,        Address offset: 0x30  */
    __IO uint32_t MR0;              /* Device CR0 register,                     Address offset: 0x34  */
    __IO uint32_t MR1;              /* Device CR1 register,                     Address offset: 0x38  */
    __IO uint32_t RSVD1[9];         /* Reserved 1,                              Address offset: 0x3c  */
    __IO uint32_t DPDRI;            /* DPIN data index register,                Address offset: 0x60  */
    __IO uint32_t DPDR;             /* DPIN data register,                      Address offset: 0x64  */
    __IO uint32_t RSVD2[35];        /* Reserved 2,                              Address offset: 0x68  */
    __IO uint32_t PCTL_SVN_ID;      /* PSRAM_LPC_CTRL version number,           Address offset: 0xf4  */
    __IO uint32_t PCTL_IDR;         /* PSRAM_LPC_CTRL identification register,  Address offset: 0xf8  */
    __IO uint32_t RSVD3[193];       /* Reserved 3,                              Address offset: 0xfc  */
    __IO uint32_t USER0_INDEX;      /* User extended index,                     Address offset: 0x400 */
    __IO uint32_t USER0_DATA;       /* User extended data,                      Address offset: 0x404 */
} PSRAMC_TypeDef;
/** @} */ /* End of group 87x3d_RTL876X_Peripheral_Registers_Structures */

/*============================================================================*
 *                              Macros
 *============================================================================*/

/** @defgroup 87x3d_RTL876X_Exported_Macros RTL876X  Exported Macros
    * @brief
  * @{
  */
/* ================================================================================ */
/* ================              Peripheral memory map             ================ */
/* ================================================================================ */
#define DATA_SRAM_BASE              0x00200000UL /*S2*/
#define BUFFER_SRAM_BASE            0x00280000UL /*S3*/
#define COMMON_SRAM0_BASE           0x00300000UL /*S11*/
#define COMMON_SRAM1_BASE           0x00400000UL /*S12*/
#define COMMON_SRAM2_BASE           0x00500000UL /*S18*/
#define DSP_SRAM_BASE               0x00600000UL /*S8*/
#define SPI_FLASH0_CACHE_BASE       0x02000000UL /*S1*/
#define SPI_FLASH0_BASE             0x04000000UL /*S1*/
#define SPI_FLASH1_BASE             0x06000000UL /*S14*/
#define SPI_FLASH2_BASE             0x08000000UL /*S16*/
#define SPI_FLASH3_BASE             0x10000000UL /*S17*/
#define SYSTEM_REG_BASE             0x40000000UL /*S4-0*/
#define PERIPH_REG_BASE             0x40000000UL /*S4-0*/
#define RTC_REG_BASE                0x40000100UL
#define PF_RTC_REG_BASE             0x40000140UL
#define RTC_LP_REG_BASE             0x40000180UL
#define LPC_REG_BASE                0x40000180UL
#define SYSBLKCTRL_REG_BASE         0x40000200UL
#define SPIC_DLY_CTRL_BASE          0x40000300UL
#define PERICLKGAT_REG_BASE         0x40000348UL
#define PWM0_REG_BASE               0x40000364UL
#define PWM1_REG_BASE               0x40000368UL
#define PWM2_REG_BASE               0x40000374UL
#define PWM3_REG_BASE               0x4000037CUL
#define RTC_LED_REG_BASE            0x40000400UL
#define AUTO_SWITCH_REG_BASE        0x40000800UL
#define GPIOA_REG_BASE              0x40001000UL /*S4-1*/
#define GPIOB_REG_BASE              0x40001800UL /*S4-2*/
#define TIMER0_REG_BASE             0x40002000UL /*S4-3*/
#define TIM0_REG_BASE               0x40002000UL
#define TIM1_REG_BASE               0x40002014UL
#define TIM2_REG_BASE               0x40002028UL
#define TIM3_REG_BASE               0x4000203CUL
#define TIM4_REG_BASE               0x40002050UL
#define TIM5_REG_BASE               0x40002064UL
#define TIM6_REG_BASE               0x40002078UL
#define TIM7_REG_BASE               0x4000208CUL
#define TIM8_REG_BASE               0x40282000UL
#define TIM9_REG_BASE               0x40282014UL
#define TIM10_REG_BASE              0x40282028UL
#define TIM11_REG_BASE              0x4028203CUL
#define TIM12_REG_BASE              0x40282050UL
#define TIM13_REG_BASE              0x40282064UL
#define TIM14_REG_BASE              0x40282078UL
#define TIM15_REG_BASE              0x4028208CUL
#define TIMA_CHANNELS_REG_BASE      0x400020A0UL
#define TIMB_CHANNELS_REG_BASE      0x402820A0UL
#define QDEC_REG_BASE               0x40004000UL /*S4-4*/
#define SPI2WIRE_REG_BASE           0x40004000UL
#define KEYSCAN_REG_BASE            0x40005000UL /*S4-5*/
#define VENDOR_REG_BASE             0x40006000UL /*S4-6*/
#define WDG_REG_BASE                0x40006000UL
#define TRNG_REG_BASE               0x40006200UL
#define CACHE_REG_BASE              0x40007000UL /*S4-7*/
#define RXI300_MCU_REG_BASE         0x40008000UL /*S4-8*/
#define GPIOC_REG_BASE              0x40008800UL /*S4-9*/
#define ADC_REG_BASE                0x40010000UL /*S5-0*/
#define UART0_REG_BASE              0x40011000UL /*S5-1*/
#define UART1_REG_BASE              0x40011400UL /*S5-2*/
#define UART2_REG_BASE              0x40011800UL /*S5-3*/
#define CRYPTO0_REG_BASE            0x40011C00UL /*S5-4*/
#define HW_AES_REG_BASE             0x40012000UL /*S5-5*/
#define IR_RC_REG_BASE              0x40013000UL /*S5-6*/
#define CRYPTO1_REG_BASE            0x40014000UL /*S5-7*/
#define ERR_CORR_CODE_REG_BASE      0x40015000UL /*S5-8*/
#define SPI0_REG_BASE               0x40028000UL /*S15-0*/
#define SPI1_REG_BASE               0x40028400UL /*S15-1*/
#define SPI2_REG_BASE               0x40028800UL /*S15-2*/
#define I2C0_REG_BASE               0x40029000UL /*S15-3*/
#define I2C1_REG_BASE               0x40029400UL /*S15-4*/
#define I2C2_REG_BASE               0x40029800UL /*S15-5*/
#define H2D_D2H_REG_BASE            0x40280000UL /*S6-1*/
#define UART3_REG_BASE              0x40281000UL /*S6-2*/
#define TIMER1_REG_BASE             0x40282000UL /*S6-3*/
#define GDMA_CHANNEL_REG_BASE       0x40283000UL /*S6-4(RXI_350)*/
#define BT_BB_REG_BASE              0x40050000UL /*S7-0(bluewiz)*/
#define BT_LE_REG_BASE              0x40051000UL
#define HCI_DMA_REG_BASE            0x40055000UL /*S7-1*/
#define HCI_UART_REG_BASE           0x40056000UL /*S7-2*/
#define USB_OTG_CFG_BASE            0x400C0000UL /*S7-3*/
#define BT_VENDOR_REG_BASE          0x40058000UL /*S7-4*/
#define SDIO_REG_BASE               0x4005C000UL /*S7-5*/
#define TDM0_REG_BASE               0x40400000UL /*S9-0*/
#define TDM2_REG_BASE               0x40404000UL /*S9-1*/
#define TDM1_REG_BASE               0x40500000UL /*S10-0*/
#define TDM3_REG_BASE               0x40504000UL /*S10-1*/
#define SPDIF_REG_BASE              0x40508000UL /*S10-2*/
#define SPIC_FLASH0_REG_BASE        0x40080000UL /*S1 */
#define SPIC_FLASH1_REG_BASE        0x40084000UL /*S14*/
#define SPIC_FLASH2_REG_BASE        0x40088000UL /*S16*/
#define SPIC_FLASH3_REG_BASE        0x4008C000UL /*S17*/
#define SPI2_HS_REG_BASE            0x40100000UL /*S13-0*/
#define SPI1_HS_REG_BASE            0x40100400UL /*S13-1*/
#define SPI0_HS_REG_BASE            0x40100800UL /*S13-2*/
#define SPI_CODEC_REG_BASE          0x40200000UL /*S6-0*/
#define VADBUF_REG_BASE             0x40300000UL /*S20*/
#define DSP_BUS_MEM_BASE            0xA0000000UL /*S19-0*/
#define DSP_BUS_PERI_BASE           0xB0000000UL /*S19-1*/
#define DSP_BUS_SPIC_MEM_BASE       0xC0000000UL /*S19-2*/
#define DSP_MAIN_MEM1_BASE          0xA4000000UL
#define DSP_MAIN_MEM2_BASE          0xA4800000UL
#define DSP_PSRAM_MEM_BASE          0xA6000000UL
#define DSP_SPIC_MEM_BASE           0xC0000000UL
#define DSP_HIFI4_S_BASE            0xB2000000UL
#define DSP_PERI_REG_BASE           0xBE000000UL
#define DSP_DMA_REG_BASE            0xBE100000UL
#define DSP_RXI300_REG_BASE         0xBE200000UL
#define DSP_TIMER_REG_BASE          0xBE201000UL
#define DSP_PSRAMC_REG_BASE         0xBE204000UL
#define DSP_SPIC_REG_BASE           0xBE208000UL
#define DSP_TDM0_REG_BASE           0xBE300000UL
#define DSP_TDM2_REG_BASE           0xBE304000UL
#define DSP_TDM1_REG_BASE           0xBE400000UL
#define DSP_TDM3_REG_BASE           0xBE404000UL
#define DSP_SPDIF_REG_BASE          0xBE408000UL
#define DSP_NNA_REG_BASE            0xBE40C000UL
#define DSP_VADBUF_REG_BASE         0xBE500000UL

#define GDMA1_CHANNEL_REG_BASE      0xBE100000UL
#define GDMA_REG_BASE               (GDMA_CHANNEL_REG_BASE + 0x02c0)
#define GDMA_Channel0_BASE          (GDMA_CHANNEL_REG_BASE + 0x0000)
#define GDMA_Channel1_BASE          (GDMA_CHANNEL_REG_BASE + 0x0058)
#define GDMA_Channel2_BASE          (GDMA_CHANNEL_REG_BASE + 0x00b0)
#define GDMA_Channel3_BASE          (GDMA_CHANNEL_REG_BASE + 0x0108)
#define GDMA_Channel4_BASE          (GDMA_CHANNEL_REG_BASE + 0x0160)
#define GDMA_Channel5_BASE          (GDMA_CHANNEL_REG_BASE + 0x01b8)
#define GDMA_Channel6_BASE          (GDMA_CHANNEL_REG_BASE + 0x0210)
#define GDMA_Channel7_BASE          (GDMA_CHANNEL_REG_BASE + 0x0268)
#define GDMA_Channel8_BASE          (GDMA_CHANNEL_REG_BASE + 0x0400)
#define GDMA_Channel9_BASE          (GDMA_CHANNEL_REG_BASE + 0x0458)
#define GDMA_Channel10_BASE         (GDMA_CHANNEL_REG_BASE + 0x04b0)
#define GDMA_Channel11_BASE         (GDMA_CHANNEL_REG_BASE + 0x0508)
#define GDMA_Channel12_BASE         (GDMA_CHANNEL_REG_BASE + 0x0560)
#define GDMA_Channel13_BASE         (GDMA_CHANNEL_REG_BASE + 0x05b8)
#define GDMA_Channel14_BASE         (GDMA_CHANNEL_REG_BASE + 0x0610)
#define GDMA_Channel15_BASE         (GDMA_CHANNEL_REG_BASE + 0x0668)

#define GDMA1_REG_BASE              (GDMA1_CHANNEL_REG_BASE + 0x02c0)
#define GDMA1_Channel0_BASE         (GDMA1_CHANNEL_REG_BASE + 0x0000)
#define GDMA1_Channel1_BASE         (GDMA1_CHANNEL_REG_BASE + 0x0058)
#define GDMA1_Channel2_BASE         (GDMA1_CHANNEL_REG_BASE + 0x00b0)
#define GDMA1_Channel3_BASE         (GDMA1_CHANNEL_REG_BASE + 0x0108)
#define GDMA1_Channel4_BASE         (GDMA1_CHANNEL_REG_BASE + 0x0160)
#define GDMA1_Channel5_BASE         (GDMA1_CHANNEL_REG_BASE + 0x01b8)
#define GDMA1_Channel6_BASE         (GDMA1_CHANNEL_REG_BASE + 0x0210)
#define GDMA1_Channel7_BASE         (GDMA1_CHANNEL_REG_BASE + 0x0268)

#define GPIOA_DMA_PORT_ADDR         0x40011600UL /* rtl87x3d */
#define GPIOB_DMA_PORT_ADDR         0x40011A00UL /* rtl87x3d */

#define SPI0_MASTER_MODE_REG        *((volatile uint32_t *)0x40000308UL)
#define SPI0_MASTER_MODE_BIT        BIT(8)

/** @brief clock divider for peripheral */
#define SYSTEM_CLK_CTRL             *((volatile uint32_t *)0x4000020CUL)
#define CLK_SOURCE_REG_0            *((volatile uint32_t *)0x40000348UL)
#define CLK_SOURCE_REG_1            *((volatile uint32_t *)0x4000035CUL)
#define CLK_SOURCE_REG_2            *((volatile uint32_t *)0x40000360UL)

/* TIM Load Count 2 register for PWM */
#define TIMER0_LOAD_COUNT2          *((volatile uint32_t *)0x400020b0UL)
#define TIMER1_LOAD_COUNT2          *((volatile uint32_t *)0x400020b4UL)
#define TIMER2_LOAD_COUNT2          *((volatile uint32_t *)0x400020b8UL)
#define TIMER3_LOAD_COUNT2          *((volatile uint32_t *)0x400020bCUL)
#define TIMER4_LOAD_COUNT2          *((volatile uint32_t *)0x400020C0UL)
#define TIMER5_LOAD_COUNT2          *((volatile uint32_t *)0x400020c4UL)
#define TIMER6_LOAD_COUNT2          *((volatile uint32_t *)0x400020c8UL)
#define TIMER7_LOAD_COUNT2          *((volatile uint32_t *)0x400020ccUL)
#define TIMER8_LOAD_COUNT2          *((volatile uint32_t *)0x402820b0UL)
#define TIMER9_LOAD_COUNT2          *((volatile uint32_t *)0x402820b4UL)
#define TIMER10_LOAD_COUNT2         *((volatile uint32_t *)0x402820b8UL)
#define TIMER11_LOAD_COUNT2         *((volatile uint32_t *)0x402820bCUL)
#define TIMER12_LOAD_COUNT2         *((volatile uint32_t *)0x402820c0UL)
#define TIMER13_LOAD_COUNT2         *((volatile uint32_t *)0x402820c4UL)
#define TIMER14_LOAD_COUNT2         *((volatile uint32_t *)0x402820c8UL)
#define TIMER15_LOAD_COUNT2         *((volatile uint32_t *)0x402820ccUL)

/* TIM Private Defines */
#define TIMER_PWM2_CR               *((volatile uint32_t *)0x40000364UL)
#define TIMER_PWM3_CR               *((volatile uint32_t *)0x40000368UL)

/** @brief spdif reg */
#define SPDIF_REG_04                *((volatile uint32_t *)(SPDIF_REG_BASE + 0x0004))

#define PERI_CLOCKGATE_REG_BASE     0x40000348UL

#define REG_PEON_SYS_CLK_SEL        0x0200
#define REG_PEON_SYS_CLK_SEL_2      0x0208
#define REG_PEON_SYS_CLK_SEL_3      0x020C
#define REG_SOC_FUNC_EN             0x0210
#define REG_SOC_HCI_COM_FUNC_EN     0x0214
#define REG_SOC_PERI_FUNC0_EN       0x0218
#define REG_SOC_PERI_FUNC1_EN       0x021C
#define REG_SOC_AUDIO_IF_EN         0x0220
#define REG_SOC_AUDIO_CLK_CTRL_A    0x0224
#define REG_SOC_AUDIO_CLK_CTRL_B    0x0228
#define REG_PESOC_CLK_CTRL          0x0230
#define REG_PESOC_PERI_CLK_CTRL0    0x0234
#define REG_PESOC_PERI_CLK_CTRL1    0x0238
#define REG_PESOC_COM_CLK_CTRL1     0x0244
#define REG_DSS_CTRL                0x0274
#define REG_BEST_DSS_RD             0x0278
#define REG_WORST_DSS_RD            0x027C
#define REG_GPIOA0_3                0x0280
#define REG_GPIOA4_7                0x0284
#define REG_GPIOB0_3                0x0288
#define REG_GPIOB4_7                0x028C
#define REG_GPIOC0_3                0x0290
#define REG_GPIOC4_7                0x0294
#define REG_GPIOD0_3                0x0298
#define REG_GPIOD4_7                0x029C
#define REG_GPIOE0_3                0x02A0
#define REG_GPIOE4_7                0x02A4
#define REG_TEST_MODE               0x02A8
#define REG_GPIOF0_3                0x02AC
#define REG_GPIOF4_7                0x02B0
#define REG_GPIOG0                  0x02B4
#define REG_GPIOH0_3                0x02B8
#define REG_GPIOH4_7                0x02BC
#define REG_GPIOI0_3                0x02C0
#define REG_GPIOI4_7                0x02C4
#define REG_GPIOJ0_3                0x02C8
#define REG_PINMUX_DIGI_MODE_EN     0x02CC
#define REG_SOC_CHARGER_DET_A       0x02E0
#define REG_SOC_CHARGER_DET_B       0x02E4
#define CORE_SWR_PAR_TAB_0          0x02E8
#define CORE_SWR_PAR_TAB_4          0x02EC
#define CORE_SWR_PAR_TAB_5          0x02F0
#define CORE_SWR_PAR_TAB_6          0x02F4
#define CORE_SWR_PAR_TAB_7          0x02F8
#define CORE_SWR_PAR_TAB_ext        0x02FC
#define PON_PERI_DLYSEL_CTRL        0x0300
#define REG_ANAPAR_PLL1_0           0x0320
#define REG_ANAPAR_PLL3_2           0x0324
#define REG_ANAPAR_PLL5_4           0x0328
#define RG0X_40M                    0x032C
#define REG_XTAL_PLL_READY          0x0338

#define GPIO_OUTPUT_OFFSET          0x00
#define GPIO_DIRECTION_OFFSET       0x04

/** @brief AON PAD AREA */
#define REG_PAD_WKEN_ADDRESS        0x20
#define REG_PAD_WK_CTRL_ADDRESS     0x12d
#define REG_PAD_WKPOL_ADDRESS       0x25
#define REG_PAD_O_ADDRESS           0x2A
#define REG_AON_PAD_E_ADDRESS       0x2F
#define REG_AON_PAD_S_ADDRESS       0x34
#define REG_AON_PAD_PU_ADDRESS      0x39
#define REG_AON_PAD_PD_ADDRESS      0x3E
#define REG_AON_PAD_PWRON_ADDRESS   0x4C

#define BIT_WK_INTEN                (1U << 7) /**< BIT7 */
#define BIT_WK_FLAG_GPIO            (1U << 6) /**< BIT6 */

#define BIT_PERI_KEYSCAN_EN         (1U << 19) /**< BIT19 */
#define BIT_SOC_ACTCK_KEYSCAN_EN    (1U << 6) /**< BIT6 */

/* =========================== RXI300 section ===========================*/
typedef struct
{
    __I uint32_t NAME;
    __I uint32_t VER;
    __I uint32_t REV;
    __I uint32_t INST;
    __I uint32_t IMPL_Y;
    __I uint32_t IMPL_D;
    __I uint32_t DEV;
    __I uint32_t PRO_NUM;
    __I uint32_t rsvd0[120];
    union
    {
        __I uint32_t ELR_i_PLD0;
        struct
        {
            __I uint32_t ERR_SRC: 8;
            __I uint32_t ERR_CMD: 3;
            __I uint32_t ERR_BSTTYPE: 3;
            __I uint32_t rsvd: 2;
            __I uint32_t ERR_BSTLEN: 8;
            __I uint32_t ERR_BSTINDEX: 8;
        } BITS_200;
    } u_200;
    union
    {
        __I uint32_t ELR_i_PLD1;
        struct
        {
            __I uint32_t rsvd0: 16;
            __I uint32_t ERR_SIZE: 3;
            __I uint32_t rsvd1: 4;
            __I uint32_t ERR_PROT: 3;
            __I uint32_t ERR_Cache: 4;
            __I uint32_t ERR_Lock: 2;
        } BITS_204;
    } u_204;
    union
    {
        __I uint32_t ELR_i_ID;
        struct
        {
            __I uint32_t ERR_ID;
        } BITS_208;
    } u_208;
    union
    {
        __I uint32_t ELR_i_ADR0;
        struct
        {
            __I uint32_t ERR_ADR0;
        } BITS_20C;
    } u_20C;
    union
    {
        __I uint32_t ELR_i_ADR1;
        struct
        {
            __I uint32_t ERR_ADR1;
        } BITS_210;
    } u_210;
    __I uint32_t rsvd1[7];
    union
    {
        __I uint32_t ELR_i_CODE;
        struct
        {
            __I uint32_t rsvd: 24;
            __I uint32_t ELR_CODE: 8;
        } BITS_230;
    } u_230;
    __I uint32_t rsvd2[2];
    union
    {
        __IO uint32_t ELR_i_INTR_CLR;
        struct
        {
            __IO uint32_t ELR_INTR_CLR: 1;
            __IO uint32_t rsvd: 31;
        } BITS_23C;
    } u_23C;
} RXI300_Typedef;

/* ================================================================================ */
/* ================             Auto Switch declaration            ================ */
/* ================================================================================ */
#define AUTO_SWITCH_REG0X(table_index)  (0x00 + (table_index << 2))
#define AUTO_SWITCH_REG1X(table_index)  (0x14 + (table_index << 2))
#define AUTO_SWITCH_REG2X(table_index)  (0x28 + (table_index << 2))
#define AUTO_SWITCH_REG3X(table_index)  (0x3c + (table_index << 2))
#define AUTO_SWITCH_REG4X(table_index)  (0x50 + (table_index << 2))
#define AUTO_SWITCH_REG5X(table_index)  (0x64 + (table_index << 2))

typedef union _TAB_AUTO0X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AUTO_SW_SWR_CORE_ZCDQ_TUNE_FORCECODE2: 8;
        uint32_t AUTO_SW_SWR_CORE_ZCDQ_TUNE_FORCECODE1: 8;
        uint32_t AUTO_SW_SWR_CORE_DUMMY: 8;
        uint32_t AUTO_SW_SWR_CORE_PFM1POS_PON_OVER: 8;
    };
} TAB_AUTO0X_TYPE;

typedef union _TAB_AUTO1X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AUTO_SW_SWR_CORE_PFMCCM1POS_PON_OVER: 8;
        uint32_t AUTO_SW_SWR_CORE_PFM300to600K_TUNE_CCOT_MANU_CODE: 5;
        uint32_t AUTO_SW_SWR_CORE_PFM300to600K_SEL_NI_ON: 1;
        uint32_t AUTO_SW_SWR_CORE_PFM300to600K_TUNE_CCOT_FORCE: 5;
        uint32_t AUTO_SW_SWR_CORE_PFM300to600K_TUNE_PFM_COMP_SAMPLE_CYC: 2;
        uint32_t AUTO_SW_SWR_CORE_PFM300to600K_EN_VDROP_DET: 1;
        uint32_t AUTO_SW_SWR_CORE_DUMMY: 10;
    };
} TAB_AUTO1X_TYPE;

typedef union _TAB_AUTO2X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AUTO_SW_SWR_CORE_PFM300to600K2_TUNE_CCOT_MANU_CODE: 5;
        uint32_t AUTO_SW_SWR_CORE_PFM300to600K2_SEL_NI_ON: 1;
        uint32_t AUTO_SW_SWR_CORE_PFM300to600K2_TUNE_CCOT_FORCE: 5;
        uint32_t AUTO_SW_SWR_CORE_PFM300to600K2_TUNE_PFM_COMP_SAMPLE_CYC: 2;
        uint32_t AUTO_SW_SWR_CORE_PFM300to600K2_EN_VDROP_DET: 1;
        uint32_t AUTO_SW_SWR_CORE_DUMMY: 18;
    };
} TAB_AUTO2X_TYPE;

typedef union _TAB_AUTO3X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AUTO_SW_SWR_CORE_EN_TAB0_AUTO_SW_SWR_SHORT: 1;
        uint32_t AUTO_SW_SWR_CORE_SEL_FOLLOWCTRL1: 1;
        uint32_t AUTO_SW_SWR_CORE_TUNE_VDIV1: 8;
        uint32_t AUTO_SW_SWR_CORE_POW_SAW: 1;
        uint32_t AUTO_SW_SWR_CORE_FPWM2: 1;
        uint32_t AUTO_SW_SWR_CORE_FPWM1: 1;
        uint32_t AUTO_SW_SWR_CORE_EN_SAW_RAMP2: 1;
        uint32_t AUTO_SW_SWR_CORE_EN_SAW_RAMP1: 1;
        uint32_t AUTO_SW_SWR_CORE_TUNE_OCP_RES: 2;
        uint32_t AUTO_SW_SWR_CORE_SEL_OCP_TABLE: 1;
        uint32_t AUTO_SW_SWR_CORE_DUMMY0: 2;
        uint32_t AUTO_SW_SWR_CORE_EN_DMYLOAD2: 1;
        uint32_t AUTO_SW_SWR_CORE_EN_DMYLOAD1: 1;
        uint32_t AUTO_SW_SWR_CORE_X4_PFM_COMP_IB_OUT1: 1;
        uint32_t AUTO_SW_SWR_CORE_X4_PFM_COMP_IB_OUT2: 1;
        uint32_t AUTO_SW_SWR_CORE_EN_PFM_FORCE_OFF_TO_ZCD1: 1;
        uint32_t AUTO_SW_SWR_CORE_EN_PFM_FORCE_OFF_TO_ZCD2: 1;
        uint32_t AUTO_SW_SWR_CORE_TUNE_POS_VREFOCPPFM1: 3;
        uint32_t AUTO_SW_SWR_CORE_DUMMY1: 3;
    };
} TAB_AUTO3X_TYPE;

typedef union _TAB_AUTO4X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AUTO_SW_SWR_AUDIO_PFM300to600K_TUNE_CCOT_MANU_CODE: 5;
        uint32_t AUTO_SW_SWR_AUDIO_PFM300to600K_SEL_NI_ON: 1;
        uint32_t AUTO_SW_SWR_AUDIO_PFM300to600K_TUNE_CCOT_FORCE: 5;
        uint32_t AUTO_SW_SWR_AUDIO_PFM300to600K_TUNE_PFM_COMP_SAMPLE_CYC: 2;
        uint32_t AUTO_SW_SWR_AUDIO_PFM300to600K_EN_VDROP_DET: 1;
        uint32_t AUTO_SW_SWR_AUDIO_DUMMY0: 7;
        uint32_t AUTO_SW_SWR_AUDIO_PFMPOS_PON_OVER: 8;
        uint32_t AUTO_SW_SWR_AUDIO_DUMMY1: 3;
    };
} TAB_AUTO4X_TYPE;

typedef union _TAB_AUTO5X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AUTO_SW_XTAL_SEL_MODE: 3;
        uint32_t AUTO_SW_XTAL_EN_SUPPLY_MODE: 1;
        uint32_t AUTO_SW_SWR_AUDIO_TUNE_VDIV: 8;
        uint32_t AUTO_SW_SWR_AUDIO_ZCDQ_TUNE_FORCECODE1: 8;
        uint32_t AUTO_SW_SWR_AUDIO_TUNE_OCP_RES: 2;
        uint32_t AUTO_SW_SWR_AUDIO_SEL_OCP2: 1;
        uint32_t AUTO_SW_SWR_AUDIO_SEL_SOFTSTART: 1;
        uint32_t AUTO_SW_SWR_AUDIO_EN_PFM_FORCE_OFF_TO_ZCD: 1;
        uint32_t AUTO_SW_SWR_AUDIO_DUMMY: 7;
    };
} TAB_AUTO5X_TYPE;

/* ================================================================================ */
/* ================             Peripheral declaration             ================ */
/* ================================================================================ */
/* System */
#define SoC_VENDOR                      ((SoC_VENDOR_REG_TypeDef   *) VENDOR_REG_BASE)

#define PERIPH                          ((PHERIPHERIAL_TypeDef  *) PERIPH_REG_BASE)
#define SYSBLKCTRL                      ((SYS_BLKCTRL_TypeDef   *) SYSBLKCTRL_REG_BASE)
#define PERIPHINT                       ((PERI_INT_TypeDef      *) VENDOR_REG_BASE)
#define OTP_RANG_PROT                   ((OTP_RANG_PROTECT_TypeDef *) (VENDOR_REG_BASE + 0x00E0))

/* IO */
#define KEYSCAN                         ((KEYSCAN_TypeDef       *) KEYSCAN_REG_BASE)
#define GPIOA                           ((GPIO_TypeDef          *) GPIOA_REG_BASE)
#define GPIOB                           ((GPIO_TypeDef          *) GPIOB_REG_BASE)
#define GPIOC                           ((GPIO_TypeDef          *) GPIOC_REG_BASE)

/** @defgroup 87x3d_QDEC_Declaration QDEC Declaration
  * @{
  */
#define QDEC                            ((QDEC_TypeDef          *) QDEC_REG_BASE)
/** End of group 87x3d_QDEC_Declaration
  * @}
  */

/** @defgroup 87x3d_I2C_Declaration I2C Declaration
  * @{
  */
#define I2C0                            ((I2C_TypeDef           *) I2C0_REG_BASE)
#define I2C1                            ((I2C_TypeDef           *) I2C1_REG_BASE)
#define I2C2                            ((I2C_TypeDef           *) I2C2_REG_BASE)
/** End of group 87x3d_I2C_Declaration
  * @}
  */

/** @defgroup 87x3d_SPI_Declaration SPI Declaration
  * @{
  */
#define SPI0                            ((SPI_TypeDef           *) SPI0_REG_BASE)
#define SPI0_HS                         ((SPI_TypeDef           *) SPI0_HS_REG_BASE)
#define SPI1                            ((SPI_TypeDef           *) SPI1_REG_BASE)
#define SPI2                            ((SPI_TypeDef           *) SPI2_REG_BASE)
/** End of group 87x3d_SPI_Declaration
  * @}
  */

#define TIM0                            ((TIM_TypeDef           *) TIM0_REG_BASE)
#define TIM1                            ((TIM_TypeDef           *) TIM1_REG_BASE)
#define TIM2                            ((TIM_TypeDef           *) TIM2_REG_BASE)
#define TIM3                            ((TIM_TypeDef           *) TIM3_REG_BASE)
#define TIM4                            ((TIM_TypeDef           *) TIM4_REG_BASE)
#define TIM5                            ((TIM_TypeDef           *) TIM5_REG_BASE)
#define TIM6                            ((TIM_TypeDef           *) TIM6_REG_BASE)
#define TIM7                            ((TIM_TypeDef           *) TIM7_REG_BASE)

#define TIM8                            ((TIM_TypeDef           *) TIM8_REG_BASE)
#define TIM9                            ((TIM_TypeDef           *) TIM9_REG_BASE)
#define TIM10                           ((TIM_TypeDef           *) TIM10_REG_BASE)
#define TIM11                           ((TIM_TypeDef           *) TIM11_REG_BASE)
#define TIM12                           ((TIM_TypeDef           *) TIM12_REG_BASE)
#define TIM13                           ((TIM_TypeDef           *) TIM13_REG_BASE)
#define TIM14                           ((TIM_TypeDef           *) TIM14_REG_BASE)
#define TIM15                           ((TIM_TypeDef           *) TIM15_REG_BASE)

#define PWM0_PN                         ((PWM_TypeDef           *) PWM0_REG_BASE)
#define PWM1_PN                         ((PWM_TypeDef           *) PWM1_REG_BASE)
#define PWM2                            ((PWM_TypeDef           *) PWM2_REG_BASE)
#define PWM3                            ((PWM_TypeDef           *) PWM3_REG_BASE)

#define TIMA_CHANNELS                   ((TIM_ChannelsTypeDef   *)TIMA_CHANNELS_REG_BASE)
#define TIMB_CHANNELS                   ((TIM_ChannelsTypeDef   *)TIMB_CHANNELS_REG_BASE)

#define GDMA_BASE                       ((GDMA_TypeDef          *) GDMA_REG_BASE)
#define GDMA1_BASE                      ((GDMA_TypeDef          *) GDMA1_REG_BASE)  //reserved for DSP's GDMA

/** @defgroup GDMA_Declaration GDMA Declaration
  * @{
  */
#define GDMA_Channel0                   ((GDMA_ChannelTypeDef   *) GDMA_Channel0_BASE)
#define GDMA_Channel1                   ((GDMA_ChannelTypeDef   *) GDMA_Channel1_BASE)
#define GDMA_Channel2                   ((GDMA_ChannelTypeDef   *) GDMA_Channel2_BASE)
#define GDMA_Channel3                   ((GDMA_ChannelTypeDef   *) GDMA_Channel3_BASE)
#define GDMA_Channel4                   ((GDMA_ChannelTypeDef   *) GDMA_Channel4_BASE)
#define GDMA_Channel5                   ((GDMA_ChannelTypeDef   *) GDMA_Channel5_BASE)
#define GDMA_Channel6                   ((GDMA_ChannelTypeDef   *) GDMA_Channel6_BASE)
#define GDMA_Channel7                   ((GDMA_ChannelTypeDef   *) GDMA_Channel7_BASE)
#define GDMA_Channel8                   ((GDMA_ChannelTypeDef   *) GDMA_Channel8_BASE)
#define GDMA_Channel9                   ((GDMA_ChannelTypeDef   *) GDMA_Channel9_BASE)
#define GDMA_Channel10                  ((GDMA_ChannelTypeDef   *) GDMA_Channel10_BASE)
#define GDMA_Channel11                  ((GDMA_ChannelTypeDef   *) GDMA_Channel11_BASE)
#define GDMA_Channel12                  ((GDMA_ChannelTypeDef   *) GDMA_Channel12_BASE)
#define GDMA_Channel13                  ((GDMA_ChannelTypeDef   *) GDMA_Channel13_BASE)
#define GDMA_Channel14                  ((GDMA_ChannelTypeDef   *) GDMA_Channel14_BASE)
#define GDMA_Channel15                  ((GDMA_ChannelTypeDef   *) GDMA_Channel15_BASE)
#define GDMA1_Channel0                  ((GDMA_ChannelTypeDef   *) GDMA1_Channel0_BASE)
#define GDMA1_Channel1                  ((GDMA_ChannelTypeDef   *) GDMA1_Channel1_BASE)
#define GDMA1_Channel2                  ((GDMA_ChannelTypeDef   *) GDMA1_Channel2_BASE)
#define GDMA1_Channel3                  ((GDMA_ChannelTypeDef   *) GDMA1_Channel3_BASE)
#define GDMA1_Channel4                  ((GDMA_ChannelTypeDef   *) GDMA1_Channel4_BASE)
#define GDMA1_Channel5                  ((GDMA_ChannelTypeDef   *) GDMA1_Channel5_BASE)
#define GDMA1_Channel6                  ((GDMA_ChannelTypeDef   *) GDMA1_Channel6_BASE)
#define GDMA1_Channel7                  ((GDMA_ChannelTypeDef   *) GDMA1_Channel7_BASE)
/** End of group GDMA_Declaration
  * @}
  */

#define ADC                             ((ADC_TypeDef           *) ADC_REG_BASE)

/** @defgroup 87x3d_UART_Declaration UART Declaration
  * @{
  */
#define UART0                           ((UART_TypeDef          *) UART0_REG_BASE)
#define UART1                           ((UART_TypeDef          *) UART1_REG_BASE)
#define UART2                           ((UART_TypeDef          *) UART2_REG_BASE)
#define UART3                           ((UART_TypeDef          *) UART3_REG_BASE)
/** End of group 87x3d_UART_Declaration
  * @}
  */

#define SPIC0                           ((SPIC_TypeDef          *) SPIC_FLASH0_REG_BASE)
#define SPIC1                           ((SPIC_TypeDef          *) SPIC_FLASH1_REG_BASE)
#define SPIC2                           ((SPIC_TypeDef          *) SPIC_FLASH2_REG_BASE)
#define SPIC3                           ((SPIC_TypeDef          *) SPIC_FLASH3_REG_BASE)
#define SPIC4                           ((SPIC_TypeDef          *) DSP_SPIC_REG_BASE)
#define PSRAMC                          ((PSRAMC_TypeDef        *) DSP_PSRAMC_REG_BASE)

#define SPI3WIRE                        ((SPI3WIRE_TypeDef      *) SPI2WIRE_REG_BASE)
#define IR                              ((IR_TypeDef            *) IR_RC_REG_BASE)
#define RTC                             ((RTC_TypeDef           *) RTC_REG_BASE)
#define PF_RTC                          ((PF_RTC_TypeDef        *) PF_RTC_REG_BASE)
#define RTC_LED                         ((RTC_LED_TypeDef       *) RTC_LED_REG_BASE)
#define LPC                             ((LPC_TypeDef           *) LPC_REG_BASE)
#define HWAES                           ((HW_AES_TypeDef        *) HW_AES_REG_BASE)
#define WDG                             ((WDG_TypeDef           *) WDG_REG_BASE)
#define CACHE                           ((CACHE_TypeDef         *) CACHE_REG_BASE)
#define SDIO                            ((SDIO_TypeDef          *) SDIO_REG_BASE)
#define SDIO0                           SDIO
#define HWSHA256                        ((HW_SHA256_TypeDef     *) CRYPTO0_REG_BASE)
#define CLK_GATE                        ((Peri_ClockGate_TypeDef*) PERI_CLOCKGATE_REG_BASE)
#define RXI300                          ((RXI300_Typedef        *) RXI300_MCU_REG_BASE)

// Wrapper for compatible
#define PERIPHERAL_1ST_IRQ              RESERVED0_IRQn

#define GPIO_B_0_7_IRQn                 GPIO_B_0to7_IRQn
#define GPIO_B_8_15_IRQn                GPIO_B_8to15_IRQn
#define GPIO_B_16_23_IRQn               GPIO_B_16to23_IRQn
#define GPIO_B_24_31_IRQn               GPIO_B_24to31_IRQn
#define GPIO0_IRQn                      GPIO_A0_IRQn
#define GPIO1_IRQn                      GPIO_A1_IRQn
#define GPIO2_IRQn                      GPIO_A_2_7_IRQn
#define GPIO3_IRQn                      GPIO_A_2_7_IRQn
#define GPIO4_IRQn                      GPIO_A_2_7_IRQn
#define GPIO5_IRQn                      GPIO_A_2_7_IRQn
#define GPIO6_IRQn                      GPIO_A_2_7_IRQn
#define GPIO7_IRQn                      GPIO_A_2_7_IRQn
#define GPIO8_IRQn                      GPIO_A_8_15_IRQn
#define GPIO9_IRQn                      GPIO_A_9_IRQn
#define GPIO10_IRQn                     GPIO_A_8_15_IRQn
#define GPIO11_IRQn                     GPIO_A_8_15_IRQn
#define GPIO12_IRQn                     GPIO_A_8_15_IRQn
#define GPIO13_IRQn                     GPIO_A_8_15_IRQn
#define GPIO14_IRQn                     GPIO_A_8_15_IRQn
#define GPIO15_IRQn                     GPIO_A_8_15_IRQn
#define GPIO16_IRQn                     GPIO_A_16_IRQn
#define GPIO17_IRQn                     GPIO_A_16_23_IRQn
#define GPIO18_IRQn                     GPIO_A_16_23_IRQn
#define GPIO19_IRQn                     GPIO_A_16_23_IRQn
#define GPIO20_IRQn                     GPIO_A_16_23_IRQn
#define GPIO21_IRQn                     GPIO_A_16_23_IRQn
#define GPIO22_IRQn                     GPIO_A_16_23_IRQn
#define GPIO23_IRQn                     GPIO_A_16_23_IRQn
#define GPIO24_IRQn                     GPIO_A_24_31_IRQn
#define GPIO25_IRQn                     GPIO_A_24_31_IRQn
#define GPIO26_IRQn                     GPIO_A_24_31_IRQn
#define GPIO27_IRQn                     GPIO_A_24_31_IRQn
#define GPIO28_IRQn                     GPIO_A_24_31_IRQn
#define GPIO29_IRQn                     GPIO_A_24_31_IRQn
#define GPIO30_IRQn                     GPIO_A_24_31_IRQn
#define GPIO31_IRQn                     GPIO_A_24_31_IRQn

#define GPIO32_IRQn                     GPIO_B_0to7_IRQn
#define GPIO33_IRQn                     GPIO_B_0to7_IRQn
#define GPIO34_IRQn                     GPIO_B_0to7_IRQn
#define GPIO35_IRQn                     GPIO_B_0to7_IRQn
#define GPIO36_IRQn                     GPIO_B_0to7_IRQn
#define GPIO37_IRQn                     GPIO_B_0to7_IRQn
#define GPIO38_IRQn                     GPIO_B_0to7_IRQn
#define GPIO39_IRQn                     GPIO_B_0to7_IRQn
#define GPIO40_IRQn                     GPIO_B_8to15_IRQn
#define GPIO41_IRQn                     GPIO_B_8to15_IRQn
#define GPIO42_IRQn                     GPIO_B_8to15_IRQn
#define GPIO43_IRQn                     GPIO_B_8to15_IRQn
#define GPIO44_IRQn                     GPIO_B_8to15_IRQn
#define GPIO45_IRQn                     GPIO_B_8to15_IRQn
#define GPIO46_IRQn                     GPIO_B_8to15_IRQn
#define GPIO47_IRQn                     GPIO_B_8to15_IRQn
#define GPIO48_IRQn                     GPIO_B_16to23_IRQn
#define GPIO49_IRQn                     GPIO_B_16to23_IRQn
#define GPIO50_IRQn                     GPIO_B_16to23_IRQn
#define GPIO51_IRQn                     GPIO_B_16to23_IRQn
#define GPIO52_IRQn                     GPIO_B_16to23_IRQn
#define GPIO53_IRQn                     GPIO_B_16to23_IRQn
#define GPIO54_IRQn                     GPIO_B_16to23_IRQn
#define GPIO55_IRQn                     GPIO_B_16to23_IRQn
#define GPIO56_IRQn                     GPIO_B_24to31_IRQn
#define GPIO57_IRQn                     GPIO_B_24to31_IRQn
#define GPIO58_IRQn                     GPIO_B_24to31_IRQn
#define GPIO59_IRQn                     GPIO_B_24to31_IRQn
#define GPIO60_IRQn                     GPIO_B_24to31_IRQn
#define GPIO61_IRQn                     GPIO_B_24to31_IRQn
#define GPIO62_IRQn                     GPIO_B_24to31_IRQn
#define GPIO63_IRQn                     GPIO_B_24to31_IRQn
#define GPIO64_IRQn                     GPIO_C_0to7_IRQn
#define GPIO65_IRQn                     GPIO_C_0to7_IRQn
#define GPIO66_IRQn                     GPIO_C_0to7_IRQn
#define GPIO67_IRQn                     GPIO_C_0to7_IRQn
#define GPIO68_IRQn                     GPIO_C_0to7_IRQn
#define GPIO69_IRQn                     GPIO_C_0to7_IRQn
#define GPIO70_IRQn                     GPIO_C_0to7_IRQn
#define GPIO71_IRQn                     GPIO_C_0to7_IRQn
#define GPIO72_IRQn                     GPIO_C_8to15_IRQn
#define GPIO73_IRQn                     GPIO_C_8to15_IRQn
#define GPIO74_IRQn                     GPIO_C_8to15_IRQn
#define GPIO75_IRQn                     GPIO_C_8to15_IRQn
#define GPIO76_IRQn                     GPIO_C_8to15_IRQn
#define GPIO77_IRQn                     GPIO_C_8to15_IRQn
#define GPIO78_IRQn                     GPIO_C_8to15_IRQn
#define GPIO79_IRQn                     GPIO_C_8to15_IRQn
#define GPIO80_IRQn                     GPIO_C_16_IRQn

#define LOG_UART_REG_BASE               UART1_REG_BASE
#define UART_REG_BASE                   UART0_REG_BASE

#define GPIO                            GPIOA
#define UART                            UART0
#define LOG_UART                        UART1
#define LOG_UART1                       UART3

/** @} */ /* End of group 87x3d_RTL876X_Exported_Macros */

/** @} */ /* End of group 87x3d_RTL876X */

#ifdef __cplusplus
}
#endif
#endif  /* RTL876X_H */
