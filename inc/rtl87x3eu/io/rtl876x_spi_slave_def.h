/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_spi_slave_def.h
* @brief     SPI Slave Structures Section
* @details
* @author    qinyuan_hu
* @date      2024-06-13
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef RTL876X_SPI_Slave_DEF_H
#define RTL876X_SPI_Slave_DEF_H

#include "rtl876x.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/*============================================================================*
 *                          SPI Defines
 *============================================================================*/
#define SPI_SLAVE_TX_FIFO_SIZE                         (64)
#define SPI_SLAVE_RX_FIFO_SIZE                         (64)
#define SPI_SLAVE_SUPPORT_APH_BRIDGE_FOR_HIGH_SPEED    (1)
#define SPI_SUPPORT_SWAP                               (1)
//#define SPI_SLAVE_SUPPORT_CLOCK_SOURCE_SWITCH          (0)

/*============================================================================*
 *                          SPI_Slave Registers Memory Map
 *============================================================================*/
typedef struct
{
    __IO uint32_t SPI_SLV_CTRL0;            /*!< 0x00 */
    __IO uint32_t RSVD_04;                  /*!< 0x04 */
    __IO uint32_t SPI_SLV_ENR;              /*!< 0x08 */
    __IO uint32_t RSVD_0C[3];               /*!< 0x0C */
    __IO uint32_t SPI_SLV_TXFTLR;           /*!< 0x18 */
    __IO uint32_t SPI_SLV_RXFTLR;           /*!< 0x1C */
    __IO uint32_t SPI_SLV_TXFLR;            /*!< 0x20 */
    __IO uint32_t SPI_SLV_RXFLR;            /*!< 0x24 */
    __IO uint32_t SPI_SLV_SR;               /*!< 0x28 */
    __IO uint32_t SPI_SLV_IMR;              /*!< 0x2C */
    __IO uint32_t SPI_SLV_ISR;              /*!< 0x30 */
    __IO uint32_t SPI_SLV_RISR;             /*!< 0x34 */
    __IO uint32_t SPI_SLV_TXOICR;           /*!< 0x38 */
    __IO uint32_t SPI_SLV_RXOICR;           /*!< 0x3C */
    __IO uint32_t SPI_SLV_RXUICR;           /*!< 0x40 */
    __IO uint32_t SPI_SLV_FAEICR;           /*!< 0x44 */
    __IO uint32_t SPI_SLV_ICR;              /*!< 0x48 */
    __IO uint32_t SPI_SLV_DMACR;            /*!< 0x4C */
    __IO uint32_t SPI_SLV_DMATDLR;          /*!< 0x50 */
    __IO uint32_t SPI_SLV_DMARDLR;          /*!< 0x54 */
    __IO uint32_t SPI_SLV_TXUICR;           /*!< 0x58 */
    __IO uint32_t SPI_SLV_SSRICR;           /*!< 0x5C */
    __IO uint32_t SPI_SLV_DR[1];            /*!< 0x60 */
    __IO uint32_t RSVD_064[38];             /*!< 0x64 */
    __IO uint32_t SPI_SLV_VR;               /*!< 0xFC */
} SPI_Slave_TypeDef;

/*============================================================================*
 *                          SPI_Slave Declaration
 *============================================================================*/

/*============================================================================*
 *                          SPI_Slave Registers and Field Descriptions
 *============================================================================*/
/* 0x00
   3:0     R/W    DFS             4'h7
   5:4     R      reserved5_4     2'b0
   6       R/W    SCPH            1'b0
   7       R/W    SCPOL           1'b0
   9:8     R      reserved9_8     2'b0
   10      R/W    SLV_OE          1'b0
   20:11   R      reserved20_11   10'b0
   21      R/W    TX_BYTE_SWAP    1'b0
   22      R/W    TX_BIT_SWAP     1'b0
   23      R/W    RX_BYTE_SWAP    1'b0
   24      R/W    RX_BIT_SWAP     1'b0
   31:25   R      reserved31_25   7'b0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t DFS: 4;
        const uint32_t reserved_3: 2;
            uint32_t SCPH: 1;
            uint32_t SCPOL: 1;
            const uint32_t reserved_2: 2;
            uint32_t SLV_OE: 1;
            const uint32_t reserved_1: 10;
            uint32_t TX_BYTE_SWAP: 1;
            uint32_t TX_BIT_SWAP: 1;
            uint32_t RX_BYTE_SWAP: 1;
            uint32_t RX_BIT_SWAP: 1;
            const uint32_t reserved_0: 7;
        } b;
    } SPI_SLV_CTRL0_TypeDef;


    /* 0x08
       0       R/W    SPI_EN          1'b0
       31:1    R      reserved8_31_1  31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t SPI_EN: 1;
                const uint32_t reserved_0: 31;
            } b;
        } SPI_SLV_ENR_TypeDef;


    /* 0x18
       5:0     R/W    TFT             6'b0
       31:6    R      reserved18_31_6 26'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t TFT: 6;
                const uint32_t reserved_0: 26;
            } b;
        } SPI_SLV_TXFTLR_TypeDef;


    /* 0x1C
       5:0     R/W    RFT             6'b0
       31:6    R      reserved1c_31_6 26'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t RFT: 6;
                const uint32_t reserved_0: 26;
            } b;
        } SPI_SLV_RXFTLR_TypeDef;


    /* 0x20
       6:0     R      TXTFL           7'b0
       31:7    R      reserved20_31_7 25'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t TXTFL: 7;
                const uint32_t reserved_0: 25;
            } b;
        } SPI_SLV_TXFLR_TypeDef;


    /* 0x24
       6:0     R      RXTFL           7'b0
       31:7    R      reserved24_31_7 25'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t RXTFL: 7;
                const uint32_t reserved_0: 25;
            } b;
        } SPI_SLV_RXFLR_TypeDef;


    /* 0x28
       0       R      BUSY            1'b0
       1       R      TFNF            1'b1
       2       R      TFE             1'b1
       3       R      RFNE            1'b0
       4       R      RFF             1'b0
       5       R      TXE             1'b0
       6       R      reserved28_6    1'b0
       31:7    R      reserved28_31_7 25'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t BUSY: 1;
                const uint32_t TFNF: 1;
                const uint32_t TFE: 1;
                const uint32_t RFNE: 1;
                const uint32_t RFF: 1;
                const uint32_t TXE: 1;
                const uint32_t reserved_1: 1;
                const uint32_t reserved_0: 25;
            } b;
        } SPI_SLV_SR_TypeDef;


    /* 0x2C
       0       R/W    TXEIM           1'b1
       1       R/W    TXOIM           1'b1
       2       R/W    RXUIM           1'b1
       3       R/W    RXOIM           1'b1
       4       R/W    RXFIM           1'b1
       5       R/W    FAEIM           1'b1
       6       R/W    TXUIM           1'b1
       7       R/W    SSRIM           1'b1
       31:8    R      reserved2c_31_8 24'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t TXEIM: 1;
                uint32_t TXOIM: 1;
                uint32_t RXUIM: 1;
                uint32_t RXOIM: 1;
                uint32_t RXFIM: 1;
                uint32_t FAEIM: 1;
                uint32_t TXUIM: 1;
                uint32_t SSRIM: 1;
                const uint32_t reserved_0: 24;
            } b;
        } SPI_SLV_IMR_TypeDef;


    /* 0x30
       0       R      TXEIS           1'b0
       1       R      TXOIS           1'b0
       2       R      RXUIS           1'b0
       3       R      RXOIS           1'b0
       4       R      RXFIS           1'b0
       5       R      FAEIS           1'b0
       6       R      TXUIS           1'b0
       7       R      SSRIS           1'b0
       31:8    R      reserved30_31_8 24'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t TXEIS: 1;
                const uint32_t TXOIS: 1;
                const uint32_t RXUIS: 1;
                const uint32_t RXOIS: 1;
                const uint32_t RXFIS: 1;
                const uint32_t FAEIS: 1;
                const uint32_t TXUIS: 1;
                const uint32_t SSRIS: 1;
                const uint32_t reserved_0: 24;
            } b;
        } SPI_SLV_ISR_TypeDef;


    /* 0x34
       0       R      TXEIR           1'b0
       1       R      TXOIR           1'b0
       2       R      RXUIR           1'b0
       3       R      RXOIR           1'b0
       4       R      RXFIR           1'b0
       5       R      FAEIR           1'b0
       6       R      TXUIR           1'b0
       7       R      SSRIR           1'b0
       31:8    R      reserved34_31_8 24'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t TXEIR: 1;
                const uint32_t TXOIR: 1;
                const uint32_t RXUIR: 1;
                const uint32_t RXOIR: 1;
                const uint32_t RXFIR: 1;
                const uint32_t FAEIR: 1;
                const uint32_t TXUIR: 1;
                const uint32_t SSRIR: 1;
                const uint32_t reserved_0: 24;
            } b;
        } SPI_SLV_RISR_TypeDef;


    /* 0x38
       0       R      TXOICR          1'b0
       31:1    R      reserved38_31_1 31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t TXOICR: 1;
                const uint32_t reserved_0: 31;
            } b;
        } SPI_SLV_TXOICR_TypeDef;


    /* 0x3C
       0       R      RXOICR          1'b0
       31:1    R      reserved3c_31_1 31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t RXOICR: 1;
                const uint32_t reserved_0: 31;
            } b;
        } SPI_SLV_RXOICR_TypeDef;


    /* 0x40
       0       R      RXUICR          1'b0
       31:1    R      reserved40_31_1 31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t RXUICR: 1;
                const uint32_t reserved_0: 31;
            } b;
        } SPI_SLV_RXUICR_TypeDef;


    /* 0x44
       0       R      FAEICR          1'b0
       31:1    R      reserved44_31_1 31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t FAEICR: 1;
                const uint32_t reserved_0: 31;
            } b;
        } SPI_SLV_FAEICR_TypeDef;


    /* 0x48
       0       R      ICR             1'b0
       31:1    R      reserved48_31_1 31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t ICR: 1;
                const uint32_t reserved_0: 31;
            } b;
        } SPI_SLV_ICR_TypeDef;


    /* 0x4C
       0       R/W    RDMAE           1'b0
       1       R/W    TDMAE           1'b0
       31:2    R      reserved4c_31_2 30'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t RDMAE: 1;
                uint32_t TDMAE: 1;
                const uint32_t reserved_0: 30;
            } b;
        } SPI_SLV_DMACR_TypeDef;


    /* 0x50
       5:0     R/W    DMATDL          6'b0
       31:6    R      reserved50_31_6 26'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t DMATDL: 6;
                const uint32_t reserved_0: 26;
            } b;
        } SPI_SLV_DMATDLR_TypeDef;


    /* 0x54
       5:0     R/W    DMARDL          6'b0
       31:6    R      reserved54_31_6 26'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t DMARDL: 6;
                const uint32_t reserved_0: 26;
            } b;
        } SPI_SLV_DMARDLR_TypeDef;


    /* 0x58
       0       R      TXUICR          1'b0
       31:1    R      reserved58_31_1 31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t TXUICR: 1;
                const uint32_t reserved_0: 31;
            } b;
        } SPI_SLV_TXUICR_TypeDef;


    /* 0x5C
       0       R      SSRICR          1'b0
       31:1    R      reserved5c_31_1 31'h0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t SSRICR: 1;
                const uint32_t reserved_0: 31;
            } b;
        } SPI_SLV_SSRICR_TypeDef;


    /* 0x60
       15:0    R/W    DR              16'b0
       31:16   R      reserved60_31_1616'b0
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                uint32_t DR: 16;
                const uint32_t reserved_0: 16;
            } b;
        } SPI_SLV_DR_TypeDef;


    /* 0x64
       31:0    R      SLV_VR          32'b2402260a
    */
    typedef union
        {
            uint32_t d32;
            struct
            {
                const uint32_t SLV_VR: 32;
            } b;
        } SPI_SLV_VR_TypeDef;

    /*============================================================================*
     *                          SPI TYPE/API Wrappers
     *============================================================================*/
//other ic need #define SPI_SLAVE       SPI0_SLAVE

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* RTL876X_SPI_Slave_DEF_H */

    /******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/