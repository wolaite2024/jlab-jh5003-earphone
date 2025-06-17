/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_gdma.h
* @brief
* @details
* @author    colin
* @date      2024-06-14
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef __RTL876X_GDMA_H
#define __RTL876X_GDMA_H



#ifdef __cplusplus
extern "C" {
#endif
/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "rtl876x_gdma_def.h"

/** @addtogroup GDMA GDMA
  * @brief GDMA driver module
  * @{
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/

/** @defgroup GDMA_Exported_Constants GDMA Exported Constants
  * @{
  */

/**
 * \defgroup    GDMA_CHANNEL_NUM GDMA CHANNEL NUM
 * \{
 * \ingroup     GDMA_Exported_Constants
 */


#define GDMA_CH_NUM0             (0)
#define GDMA_CH_NUM1             (1)
#define GDMA_CH_NUM2             (2)
#define GDMA_CH_NUM3             (3)
#define GDMA_CH_NUM4             (4)
#define GDMA_CH_NUM5             (5)
#if (CHIP_GDMA_CHANNEL_NUM >= 9)
#define GDMA_CH_NUM6             (6)
#define GDMA_CH_NUM7             (7)
#define GDMA_CH_NUM8             (8)
#endif
#if (CHIP_GDMA_CHANNEL_NUM >= 10)
#define GDMA_CH_NUM9             (9)
#endif
#if (CHIP_GDMA_CHANNEL_NUM >= 12)
#define GDMA_CH_NUM10            (10)
#define GDMA_CH_NUM11            (11)
#endif
#if (CHIP_GDMA_CHANNEL_NUM >= 16)
#define GDMA_CH_NUM12            (12)
#define GDMA_CH_NUM13            (13)
#define GDMA_CH_NUM14            (14)
#define GDMA_CH_NUM15            (15)
#endif
#if (CHIP_GDMA_CHANNEL_NUM >= 24)
#define GDMA_CH_NUM16            (16)
#define GDMA_CH_NUM17            (17)
#define GDMA_CH_NUM18            (18)
#define GDMA_CH_NUM19            (19)
#define GDMA_CH_NUM20            (20)
#define GDMA_CH_NUM21            (21)
#define GDMA_CH_NUM22            (22)
#define GDMA_CH_NUM23            (23)
#endif
#if (CHIP_GDMA_CHANNEL_NUM >= 33)
#define GDMA_CH_NUM24            (24)
#define GDMA_CH_NUM25            (25)
#define GDMA_CH_NUM26            (26)
#define GDMA_CH_NUM27            (27)
#define GDMA_CH_NUM28            (28)
#define GDMA_CH_NUM29            (29)
#define GDMA_CH_NUM30            (30)
#define GDMA_CH_NUM31            (31)
#define GDMA_CH_NUM32            (32)
#endif

#define IS_GDMA_ChannelNum(NUM)  ((NUM) < CHIP_GDMA_CHANNEL_NUM)
#define GDMA_MAX_HP_CH_COUNT (2)
/** End of GDMA_CHANNEL_NUM
  * \}
  */

/**
 * \defgroup   GDMA_Data_Transfer_Direction GDMA Data Transfer Direction
 * \{
 * \ingroup    GDMA_Exported_Constants
 */

typedef enum
{
    GDMA_DIR_MemoryToMemory = 0x0,
    GDMA_DIR_MemoryToPeripheral = 0x1,
    GDMA_DIR_PeripheralToMemory = 0x2,
    GDMA_DIR_PeripheralToPeripheral = 0x3,
    GDMA_DIR_MemoryToPeripheral_FlowCtrl = 0x6, /* NOTE: only support for rom, not support for sdk */
} GDMADirection_TypeDef;

#define IS_GDMA_DIR(DIR) (((DIR) == GDMA_DIR_MemoryToMemory) || \
                          ((DIR) == GDMA_DIR_MemoryToPeripheral) || \
                          ((DIR) == GDMA_DIR_PeripheralToMemory) ||\
                          ((DIR) == GDMA_DIR_MemoryToPeripheral_FlowCtrl) ||\
                          ((DIR) == GDMA_DIR_PeripheralToPeripheral))

/** End of GDMA_Data_Transfer_Direction
  * \}
  */

/**
 * \defgroup    GDMA_Source_Incremented_Mode GDMA Source Incremented Mode
 * \{
 * \ingroup     GDMA_Exported_Constants
 */

typedef enum
{
    DMA_SourceInc_Inc = 0x0,
    DMA_SourceInc_Fix = 0x2,
} GDMASrcInc_TypeDef;

#define IS_GDMA_SourceInc(STATE) (((STATE) == DMA_SourceInc_Inc) || \
                                  ((STATE) == DMA_SourceInc_Fix))

/** End of GDMA_Source_Incremented_Mode
  * \}
  */

/**
 * \defgroup    GDMA_Destination_Incremented_Mode GDMA Destination Incremented Mode
 * \{
 * \ingroup     GDMA_Exported_Constants
 */

typedef enum
{
    DMA_DestinationInc_Inc = 0x0,
    DMA_DestinationInc_Fix = 0x2,
} GDMADestInc_TypeDef;

#define IS_GDMA_DestinationInc(STATE) (((STATE) == DMA_DestinationInc_Inc) || \
                                       ((STATE) == DMA_DestinationInc_Fix))

/** End of GDMA_Destination_Incremented_Mode
  * \}
  */

/**
 * \defgroup    GDMA_Data_Size GDMA Data Size
 * \{
 * \ingroup     GDMA_Exported_Constants
 */

typedef enum
{
    GDMA_DataSize_Byte     = 0x0,
    GDMA_DataSize_HalfWord = 0x1,
    GDMA_DataSize_Word     = 0x2,
} GDMADataSize_TypeDef;

#define IS_GDMA_DATA_SIZE(SIZE) (((SIZE) == GDMA_DataSize_Byte) || \
                                 ((SIZE) == GDMA_DataSize_HalfWord) || \
                                 ((SIZE) == GDMA_DataSize_Word))

/** End of GDMA_Data_Size
  * \}
  */

/**
 * \defgroup    GDMA_Msize GDMA Msize
 * \{
 * \ingroup     GDMA_Exported_Constants
 */

typedef enum
{
    GDMA_Msize_1   = 0x0,
    GDMA_Msize_4   = 0x1,
    GDMA_Msize_8   = 0x2,
    GDMA_Msize_16  = 0x3,
    GDMA_Msize_32  = 0x4,
    GDMA_Msize_64  = 0x5,
    GDMA_Msize_128 = 0x6,
} GDMAMSize_TypeDef;

#define IS_GDMA_MSIZE(SIZE) (((SIZE) == GDMA_Msize_1) || \
                             ((SIZE) == GDMA_Msize_4) || \
                             ((SIZE) == GDMA_Msize_8) || \
                             ((SIZE) == GDMA_Msize_16) || \
                             ((SIZE) == GDMA_Msize_32) || \
                             ((SIZE) == GDMA_Msize_64) || \
                             ((SIZE) == GDMA_Msize_128))

/** End of GDMA_Msize
  * \}
  */

/**
 * \defgroup    GDMA_Status GDMA Operate Return Value
 * \{
 * \ingroup     GDMA_Exported_Constants
 */
typedef enum _GDMA_CMD_RETURN_VAL
{
    GDMA_ENABLE_SUCCESS,
    GDMA_ENABLE_FAIL,
    GDMA_DISABLE_SUCCESS,
    GDMA_DISABLE_FAIL,
    GDMA_DISABLE_ALREADY,
    GDMA_STATUS_NONE,
} GDMA_Status;

/** End of GDMA_Status
  * \}
  */

/** @defgroup DMA_interrupts_definition DMA Interrupts Definition
  * @{
  */

#define GDMA_INT_Transfer               (BIT0)
#define GDMA_INT_Block                  (BIT1)
#define GDMA_INT_Error                  (BIT4)
#if (GDMA_SUPPORT_INT_HAIF_BLOCK == 1)
#define GDMA_INT_Half_Block             (BIT5)
#endif

#define IS_GDMA_CONFIG_IT(IT) ((((IT) & 0xFFFFFFE0) == 0x00) && ((IT) != 0x00))

/** End of Group DMA_interrupts_definition
  * @}
  */

/**
 * \defgroup    GDMA_Multi-block_Mode GDMA Multi-block Mode
 * \{
 * \ingroup     GDMA_Exported_Constants
 */

#define AUTO_RELOAD_WITH_CONTIGUOUS_SAR                            (BIT31)
#define AUTO_RELOAD_WITH_CONTIGUOUS_DAR                            (BIT30)
#define AUTO_RELOAD_TRANSFER                                       (BIT30 | BIT31)
#define LLI_WITH_CONTIGUOUS_SAR                                    (BIT27)
#define LLI_WITH_AUTO_RELOAD_SAR                                   (BIT27 | BIT30)
#define LLI_WITH_CONTIGUOUS_DAR                                    (BIT28)
#define LLI_WITH_AUTO_RELOAD_DAR                                   (BIT28 | BIT31)
#define LLI_TRANSFER                                               (BIT27 | BIT28)

#define IS_GDMA_MULTIBLOCKMODE(MODE) (((MODE) == AUTO_RELOAD_WITH_CONTIGUOUS_SAR) || ((MODE) == AUTO_RELOAD_WITH_CONTIGUOUS_DAR)\
                                      ||((MODE) == AUTO_RELOAD_TRANSFER) || ((MODE) == LLI_WITH_CONTIGUOUS_SAR)\
                                      ||((MODE) == LLI_WITH_AUTO_RELOAD_SAR) || ((MODE) == LLI_WITH_CONTIGUOUS_DAR)\
                                      ||((MODE) == LLI_WITH_AUTO_RELOAD_DAR) || ((MODE) == LLI_TRANSFER))

/** End of GDMA_Multi-block_Mode
  * \}
  */

/**
 * \defgroup    GDMA_Multi-Block_Select_Bit GDMA Multi-Block Select Bit
 * \{
 * \ingroup     GDMA_Exported_Constants
 */

#define AUTO_RELOAD_SELECTED_BIT        (BIT30 | BIT31)
#define LLP_SELECTED_BIT                (BIT27 | BIT28)

/** End of GDMA_Multi-Block_Select_Bit
  * \}
  */

/**
 * \defgroup    GDMA_Suspend_Flag GDMA Suspend Flag
 * \{
 * \ingroup     GDMA_Exported_Constants
 */

#define GDMA_FIFO_STATUS                (BIT(9))
#define GDMA_SUSPEND_TRANSMISSSION      (BIT(8))
#define GDMA_SUSPEND_CMD_STATUS         (BIT(2) | BIT(1))
#define GDMA_SUSPEND_CHANNEL_STATUS     (BIT(0))

/** End of GDMA_Suspend_Flag
  * \}
  */

/** End of Group GDMA_Exported_Constants
  * @}
  */


/*============================================================================*
 *                         Types
 *============================================================================*/
/** \defgroup GDMA_Exported_Types GDMA Exported Types
  * \brief
  * \{
  */

/**
 * \brief       GDMA init structure definition.
 *
 * \ingroup     GDMA_Exported_Types
 */

typedef struct
{
    uint8_t GDMA_ChannelNum;                /*!< Specifies channel number for GDMA. */
    GDMADirection_TypeDef GDMA_DIR;         /*!< Specifies transfer direction. */
    uint32_t GDMA_BufferSize;               /*!< Specifies the buffer size(RTL8762G <= 65535, RTL8772F: GDMA1 is 65535 and GDMA2 is 524287).
                                                 The data unit is equal to the configuration set in DMA_PeripheralDataSize
                                                 or DMA_MemoryDataSize members depending in the transfer direction. */
    GDMASrcInc_TypeDef GDMA_SourceInc;      /*!< Specifies whether the source address
                                                 register is incremented or not. */
    GDMADestInc_TypeDef GDMA_DestinationInc; /*!< Specifies whether the destination address
                                                 register is incremented or not.*/
    GDMADataSize_TypeDef GDMA_SourceDataSize; /*!< Specifies the source data width. */
    GDMADataSize_TypeDef GDMA_DestinationDataSize; /*!< Specifies the destination data width. */
    GDMAMSize_TypeDef GDMA_SourceMsize;     /*!< Specifies items number to be transferred. */
    GDMAMSize_TypeDef GDMA_DestinationMsize;/*!< Specifies items number to be transferred. */
    uint32_t GDMA_SourceAddr;               /*!< Specifies the source base address for GDMA Channelx. */
    uint32_t GDMA_DestinationAddr;          /*!< Specifies the destination base address for GDMA Channelx. */
    uint32_t GDMA_ChannelPriority;          /*!< Specifies the software priority for the GDMA Channelx.
                                                 This parameter can be a value of 0~9(RTL8762G); 0~7 for GDMA1 and 0~15 for GDMA2(RTL8772F). */
    uint32_t GDMA_Multi_Block_Mode;         /*!< Specifies the multi block transfer mode.
                                                 This parameter can be a value of \ref GDMA_Multiblock_Mode. */
    uint32_t GDMA_Multi_Block_Struct;       /*!< Pointer to the first struct of LLI. */
    uint8_t GDMA_Multi_Block_En;            /*!< Enable or disable multi-block function. */
    uint8_t GDMA_SourceHandshake;           /*!< Specifies the handshake index in source.
                                                 This parameter can be a value of \ref GDMA_Handshake_Type. */
    uint8_t GDMA_DestHandshake;             /*!< Specifies the handshake index in Destination.
                                                 This parameter can be a value of \ref GDMA_Handshake_Type. */
#if (GDMA_SUPPORT_GATHER_SCATTER_FUNCTION == 1)
    uint8_t  GDMA_Gather_En;                /*!< Enable or disable Gather function. NOTE:4 bytes ALIGN.*/
    uint32_t GDMA_GatherCount;              /*!< Specifies the GatherCount.NOTE:4 bytes ALIGN.*/
    uint32_t GDMA_GatherInterval;           /*!< Specifies the GatherInterval. */
    uint8_t  GDMA_Scatter_En;               /*!< Enable or disable Scatter function. */
    uint32_t GDMA_ScatterCount;             /*!< Specifies the ScatterCount. */
    uint32_t GDMA_ScatterInterval;          /*!< Specifies the ScatterInterval. */
    uint32_t GDMA_GatherCircularStreamingNum;  /*!< Specifies the GatherCircularStreamingNum. */
    uint32_t GDMA_ScatterCircularStreamingNum; /*!< Specifies the ScatterCircularStreamingNum. */
#endif

} GDMA_InitTypeDef;

/**
 * \brief       GDMA link list item structure definition.
 *
 * \ingroup     GDMA_Exported_Types
 */
typedef struct
{
    __IO uint32_t SAR;
    __IO uint32_t DAR;
    __IO uint32_t LLP;
    __IO uint32_t CTL_LOW;
    __IO uint32_t CTL_HIGH;
} GDMA_LLIDef;

/** End of Group GDMA_Exported_Types
  * @}
  */


/*============================================================================*
 *                         Functions
 *============================================================================*/
/** \defgroup GDMA_Exported_Functions GDMA Exported Functions
  * \brief
  * \{
  */

/**
  * @brief  Enable or Disable GDMA peripheral clock.
  * @param  is_enable:
  *  @args  true:  enable GDMA hw clock
  *  @args  false: disable GDMA hw clock, and deinit all GDMA register
  * @retval None
  */
void GDMA_HwClock(GDMA_TypeDef *GDMAx, bool enable);;

/**
 * rtl876x_gdma.h
 *
 * \brief  Deinitializes the GDMA registers to their default reset values.
 *
 * \param  None.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_gdma_init(void)
 * {
 *     GDMA_DeInit();
 * }
 * \endcode
 */
void GDMA_DeInit(void);

/**
 * rtl876x_gdma.h
 *
 * \brief     Initializes the GDMA Channelx according to the specified
 *            parameters in the GDMA_InitStruct.
 *
 * \param[in] GDMA_Channelx: Where x can be 0 to 11 to select the DMA Channel.
 * \param[in] GDMA_InitStruct: Pointer to a GDMA_InitTypeDef structure that
 *            contains the configuration information for the specified DMA Channel.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * uint8_t uart_tx_dma_ch_num = 0xa5;
 * uint8_t UART_TX_DMA_CHANNEL_NUM = uart_tx_dma_ch_num;
 * #define UART_TX_DMA_CHANNEL    DMA_CH_BASE(uart_tx_dma_ch_num)
 * #define UART_TX_DMA_IRQ        DMA_CH_IRQ(uart_tx_dma_ch_num)
 * uint8_t GDMA_SendBuffer[100];
 *
 * void driver_gdma_init(void)
 * {
 *     uint16_t  strLen = 0;
 *     char *demoStr = "### Welcome to use RealTek Bumblebee ###\r\n";
 *     strLen = strlen(demoStr);
 *     memcpy(GDMA_SendBuffer, demoStr, strLen);
 *
 *     GDMA_InitTypeDef GDMA_InitStruct;
 *     GDMA_StructInit(&GDMA_InitStruct);
 *     GDMA_InitStruct.GDMA_ChannelNum      = UART_TX_DMA_CHANNEL_NUM;
 *     GDMA_InitStruct.GDMA_DIR             = GDMA_DIR_MemoryToPeripheral;
 *     GDMA_InitStruct.GDMA_BufferSize      = strLen;//determine total transfer size
 *     GDMA_InitStruct.GDMA_SourceInc       = DMA_SourceInc_Inc;
 *     GDMA_InitStruct.GDMA_DestinationInc  = DMA_DestinationInc_Fix;
 *     GDMA_InitStruct.GDMA_SourceDataSize  = GDMA_DataSize_Byte;
 *     GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
 *     GDMA_InitStruct.GDMA_SourceMsize      = GDMA_Msize_1;
 *     GDMA_InitStruct.GDMA_DestinationMsize = GDMA_Msize_1;
 *     GDMA_InitStruct.GDMA_SourceAddr      = (uint32_t)GDMA_SendBuffer;
 *     GDMA_InitStruct.GDMA_DestinationAddr = (uint32_t)(&(UART->RB_THR));
 *     GDMA_InitStruct.GDMA_DestHandshake   = GDMA_Handshake_UART0_TX;
 *     GDMA_Init(UART_TX_DMA_CHANNEL, &GDMA_InitStruct);
 * }
 * \endcode
 */
void GDMA_Init(GDMA_ChannelTypeDef *GDMA_Channelx, GDMA_InitTypeDef *GDMA_InitStruct);

/**
 * rtl876x_gdma.h
 *
 * \brief     Fills each GDMA_InitStruct member with its default value.
 *
 * \param[in] GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure which will be initialized.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * uint8_t uart_tx_dma_ch_num = 0xa5;
 * uint8_t UART_TX_DMA_CHANNEL_NUM = uart_tx_dma_ch_num;
 * #define UART_TX_DMA_CHANNEL    DMA_CH_BASE(uart_tx_dma_ch_num)
 * #define UART_TX_DMA_IRQ        DMA_CH_IRQ(uart_tx_dma_ch_num)
 * uint8_t GDMA_SendBuffer[100];
 *
 * void driver_gdma_init(void)
 * {
 *     uint16_t  strLen = 0;
 *     char *demoStr = "### Welcome to use RealTek Bumblebee ###\r\n";
 *     strLen = strlen(demoStr);
 *     memcpy(GDMA_SendBuffer, demoStr, strLen);
 *
 *     GDMA_InitTypeDef GDMA_InitStruct;
 *     GDMA_StructInit(&GDMA_InitStruct);
 *     GDMA_InitStruct.GDMA_ChannelNum      = UART_TX_DMA_CHANNEL_NUM;
 *     GDMA_InitStruct.GDMA_DIR             = GDMA_DIR_MemoryToPeripheral;
 *     GDMA_InitStruct.GDMA_BufferSize      = strLen;//determine total transfer size
 *     GDMA_InitStruct.GDMA_SourceInc       = DMA_SourceInc_Inc;
 *     GDMA_InitStruct.GDMA_DestinationInc  = DMA_DestinationInc_Fix;
 *     GDMA_InitStruct.GDMA_SourceDataSize  = GDMA_DataSize_Byte;
 *     GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
 *     GDMA_InitStruct.GDMA_SourceMsize      = GDMA_Msize_1;
 *     GDMA_InitStruct.GDMA_DestinationMsize = GDMA_Msize_1;
 *     GDMA_InitStruct.GDMA_SourceAddr      = (uint32_t)GDMA_SendBuffer;
 *     GDMA_InitStruct.GDMA_DestinationAddr = (uint32_t)(&(UART->RB_THR));
 *     GDMA_InitStruct.GDMA_DestHandshake   = GDMA_Handshake_UART0_TX;
 *     GDMA_Init(UART_TX_DMA_CHANNEL, &GDMA_InitStruct);
 * }
 * \endcode
 */
void GDMA_StructInit(GDMA_InitTypeDef *GDMA_InitStruct);

/**
 * rtl876x_gdma.h
 *
 * \brief  Enables or disables the selected GDMA channel.
 *
 * \param[in]  GDMA_Channel_Num: GDMA channel number, which can be 0~11.
 * \param[in]  NewState: New state of the selected DMA channel.
 *             This parameter can be one of the following values:
 *             \arg ENABLE: Enable the selected GDMA channel.
 *             \arg DISABLE: Disable the selected GDMA channel.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * uint8_t uart_tx_dma_ch_num = 0xa5;
 * uint8_t UART_TX_DMA_CHANNEL_NUM = uart_tx_dma_ch_num;
 *
 * void driver_gdma_init(void)
 * {
 *     GDMA_Cmd(UART_TX_DMA_CHANNEL_NUM, ENABLE);
 * }
 * \endcode
 */
uint8_t GDMA_Cmd(uint8_t GDMA_ChannelNum, FunctionalState NewState);

/**
 * rtl876x_gdma.h
 *
 * \brief   Enable or disable the specified DMA channelx interrupt source.
 *
 * \param[in] GDMA_Channel_Num: GDMA channel number, which can be 0~11.
 * \param[in] GDMA_IT: Specifies the GDMA interrupt source to be enabled or disabled.
 *            This parameter can be any combination of the following values:
 *            \arg GDMA_INT_Transfer: Transfer complete interrupt source.
 *            \arg GDMA_INT_Block: Block transfer interrupt source.
 *            \arg GDMA_INT_SrcTransfer: Source Transfer interrupt source.
 *            \arg GDMA_INT_DstTransfer: Destination Transfer interruptsource.
 *            \arg GDMA_INT_Error: Transfer error interrupt source.
 * \param[in] NewState: New state of the specified DMA interrupt source.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable the specified DMA channelx interrupt source.
 *            \arg DISABLE: Disable the specified DMA channelx interrupt source.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * uint8_t uart_tx_dma_ch_num = 0xa5;
 * uint8_t UART_TX_DMA_CHANNEL_NUM = uart_tx_dma_ch_num;
 * #define UART_TX_DMA_IRQ        DMA_CH_IRQ(uart_tx_dma_ch_num)
 *
 * void driver_gdma_init(void)
 * {
 *     GDMA_INTConfig(UART_TX_DMA_CHANNEL_NUM, GDMA_INT_Transfer, ENABLE);
 *
 *     NVIC_InitTypeDef NVIC_InitStruct;
 *     NVIC_InitStruct.NVIC_IRQChannel = UART_TX_DMA_IRQ;
 *     NVIC_InitStruct.NVIC_IRQChannelPriority = 3;
 *     NVIC_InitStruct.NVIC_IRQChannelCmd = (FunctionalState)ENABLE;
 *     NVIC_Init(&NVIC_InitStruct);
 *
 *     GDMA_Cmd(UART_TX_DMA_CHANNEL_NUM, ENABLE);
 * }
 * \endcode
 */
void GDMA_INTConfig(uint8_t GDMA_ChannelNum, uint32_t GDMA_IT, FunctionalState NewState);

/**
 * rtl876x_gdma.h
 *
 * \brief  Clear the specified DMA channelx interrupt pending bit.
 *
 * \param[in] GDMA_Channel_Num: GDMA channel number, which can be 0~11.
 *
 * \param[in] GDMA_IT: Specifies the GDMA interrupt source to be enabled or disabled.
 *            This parameter can be any combination of the following values:
 *            \arg GDMA_INT_Transfer: Transfer complete interrupt source.
 *            \arg GDMA_INT_Block: Block transfer interrupt source.
 *            \arg GDMA_INT_SrcTransfer: Source Transfer interrupt source.
 *            \arg GDMA_INT_DstTransfer: Destination Transfer interruptsource.
 *            \arg GDMA_INT_Error: Transfer error interrupt source.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * uint8_t uart_tx_dma_ch_num = 0xa5;
 * uint8_t UART_TX_DMA_CHANNEL_NUM = uart_tx_dma_ch_num;
 *
 * void UART_TX_GDMA_Handler(void)
 * {
 *     GDMA_ClearINTPendingBit(UART_TX_DMA_CHANNEL_NUM, GDMA_INT_Transfer);
 *     //Add user code here.
 * }
 * \endcode
 */
void GDMA_ClearINTPendingBit(uint8_t GDMA_ChannelNum, uint32_t GDMA_IT);

/**
 * rtl876x_gdma.h
 *
 * \brief  Suspend GDMA transmission safe from the source. Please check GDMA FIFO empty to guarnatee without losing data.
 *
 * \param[in] GDMA_Channelx: Where x can be 0 to 11 to select the DMA Channel.
 *
 * \return The result of suspend GDMA transmission.
 * \retval true: suspend GDMA transmission success.
 * \retval false: suspend GDMA transmission failed.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * uint8_t uart_rx_dma_ch_num = 0xa5;
 * #define UART_RX_DMA_CHANNEL_NUM   uart_rx_dma_ch_num
 * #define UART_RX_DMA_CHANNEL       DMA_CH_BASE(uart_rx_dma_ch_num)
 *
 * void gdma_demo(void)
 * {
 *     GDMA_SafeSuspend(UART_RX_DMA_CHANNEL);
 *     //Add user code here.
 * }
 * \endcode
 */
bool GDMA_SafeSuspend(GDMA_ChannelTypeDef *GDMA_Channelx);

/**
 * rtl876x_gdma.h
 *
 * \brief   Get the selected GDMA channel status.
 *
 * \param[in] GDMA_Channel_Num: GDMA channel number, which can be 0~11.
 *
 * \return  The status of GDMA channel.
 * \retval  SET: Channel is be used.
 * \retval  RESET: Channel is free.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void Data_Uart_Handler(void)
 * {
 *     if (UART_GetFlagState(UART, UART_FLAG_RX_IDLE) == SET)
 *     {
 *         UART_INTConfig(UART, UART_INT_IDLE, DISABLE);
 *         if (GDMA_GetChannelStatus(UART_RX_DMA_CHANNEL_NUM))
 *         {
 *             GDMA_SuspendCmd(UART_RX_DMA_CHANNEL, ENABLE);
 *         }
 *     }
 * }
 * \endcode
 */
FlagStatus GDMA_GetChannelStatus(uint8_t GDMA_Channel_Num);

/**
 * rtl876x_gdma.h
 *
 * \brief  Check whether GDMA Channel transfer interrupt is set.
 *
 * \param[in] GDMA_Channel_Num: GDMA channel number, which can be 0~11.
 *
 * \return  Transfer interrupt status, SET or RESET.
 * \retval SET: GDMA Channel transfer interrupt is set.
 * \retval RESET: GDMA Channel transfer interrupt is unset.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * #define LCD_RST                          P4_0
 * #define LCD_DMA_CHANNEL_NUM              lcd_dma_ch_num
 *
 * void lcd_wait_dma_transfer(void)
 * {
 *     while (GDMA_GetTransferINTStatus(LCD_DMA_CHANNEL_NUM) != SET);
 *     GDMA_ClearINTPendingBit(LCD_DMA_CHANNEL_NUM, GDMA_INT_Transfer);
 * }
 * \endcode
 */
ITStatus GDMA_GetTransferINTStatus(uint8_t GDMA_Channel_Num);

/**
 * rtl876x_gdma.h
 *
 * \brief     Clear GDMA Channelx all type interrupt.
 *
 * \param[in] GDMA_Channel_Num: GDMA channel number, which can be 0~11.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * uint8_t uart_tx_dma_ch_num = 0xa5;
 * uint8_t UART_TX_DMA_CHANNEL_NUM = uart_tx_dma_ch_num;
 *
 * void gdma_demo(void)
 * {
 *     GDMA_ClearAllTypeINT(UART_TX_DMA_CHANNEL_NUM);
 * }
 * \endcode
 */
void GDMA_ClearAllTypeINT(uint8_t GDMA_Channel_Num);

/**
 * rtl876x_gdma.h
 *
 * \brief     Set GDMA transmission source address.
 *
 * \param[in] GDMA_Channelx: Where x can be 0 to 11 to select the DMA Channel.
 * \param[in] Address: Source address.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * uint8_t console_tx_dma_ch_num = 0xa5;
 * #define UART_TX_DMA_CHANNEL         DMA_CH_BASE(console_tx_dma_ch_num)
 *
 * void gdma_demo(void)
 * {
 *     uint32_t data_buf[10] = {0};
 *     uint32_t data_buf_size = 4095;
 *     GDMA_SetSourceAddress(UART_TX_DMA_CHANNEL, (uint32_t)data_buf);
 *     GDMA_SetBufferSize(UART_TX_DMA_CHANNEL, data_buf_size);
 * }
 * \endcode
 */
void GDMA_SetSourceAddress(GDMA_ChannelTypeDef *GDMA_Channelx, uint32_t Address);

/**
 * rtl876x_gdma.h
 *
 * \brief     Set GDMA transmission destination address.
 *
 * \param[in] GDMA_Channelx: Where x can be 0 to 11 to select the DMA Channel.
 * \param[in] Address: Destination address.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * uint8_t console_rx_dma_ch_num = 0xa5;
 * #define UART_RX_DMA_CHANNEL    DMA_CH_BASE(console_rx_dma_ch_num)
 *
 * void gdma_demo(void)
 * {
 *     uint32_t data_buf[10] = {0};
 *     GDMA_SetDestinationAddress(UART_RX_DMA_CHANNEL, (uint32_t)data_buf);
 * }
 * \endcode
 */
void GDMA_SetDestinationAddress(GDMA_ChannelTypeDef *GDMA_Channelx, uint32_t Address);

/**
 * rtl876x_gdma.h
 *
 * \brief     Set GDMA buffer size.
 *
 * \param[in] GDMA_Channelx: Where x can be 0 to 11 to select the DMA Channel.
 * \param[in] buffer_size: Set GDMA BufferSize, max 1048575.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * uint8_t console_tx_dma_ch_num = 0xa5;
 * #define UART_TX_DMA_CHANNEL         DMA_CH_BASE(console_tx_dma_ch_num)
 *
 * void gdma_demo(void)
 * {
 *     uint32_t data_buf_size = 4095;
 *     GDMA_SetBufferSize(UART_TX_DMA_CHANNEL, data_buf_size);
 * }
 * \endcode
 */
void GDMA_SetBufferSize(GDMA_ChannelTypeDef *GDMA_Channelx, uint32_t buffer_size);

/**
 * \brief  Get GDMA source address.
 *
 * \param[in] GDMA_Channelx: Select the GDMA peripheral. \ref GDMA_Declaration
 *
 * \return Source address.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void gdma_demo(void)
 * {
 *     uint32_t address = GDMA_GetSrcTransferAddress(GDMA2_Channel0);
 * }
 * \endcode
 */
uint32_t GDMA_GetSrcTransferAddress(GDMA_ChannelTypeDef *GDMA_Channelx);

/**
 * \brief  Get GDMA destination address.
 *
 * \param[in] GDMA_Channelx: Select the GDMA peripheral. \ref GDMA_Declaration
 *
 * \return Destination address.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void gdma_demo(void)
 * {
 *     uint32_t address = GDMA_GetDstTransferAddress(GDMA2_Channel0);
 * }
 * \endcode
 */
uint32_t GDMA_GetDstTransferAddress(GDMA_ChannelTypeDef *GDMA_Channelx);

/**
 * rtl876x_gdma.h
 *
 * \brief     Suspend GDMA transmission from the source. Please check GDMA FIFO empty to guarnatee without losing data.
 *
 * \param[in] GDMA_Channelx: Where x can be 0 to 11 to select the DMA Channel.
 * \param[in] NewState: Enable or disable suspend GDMA transmission.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable suspend GDMA transmission.
 *            \arg DISABLE: Disable suspend GDMA transmission.
 *
 * \return    None.
 *
 * \note      To prevent data loss, it is necessary to check whether FIFO data transmission is completed
 *            after suspend, and judge by checking whether GDMA FIFO is empty.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void Data_Uart_Handler(void)
 * {
 *         //restart dma receive
 *         GDMA_SetDestinationAddress(UART_RX_DMA_CHANNEL, (uint32_t)uart_receive_buf);
 *         GDMA_SuspendCmd(UART_RX_DMA_CHANNEL, DISABLE);
 *         GDMA_Cmd(UART_RX_DMA_CHANNEL_NUM, ENABLE);
 *
 *         UART_INTConfig(UART0, UART_INT_IDLE, ENABLE);
 *     }
 * }
 * \endcode
 */
void GDMA_SuspendCmd(GDMA_ChannelTypeDef *GDMA_Channelx, FunctionalState NewState);

/**
 * rtl876x_gdma.h
 *
 * \brief     Check whether GDMA FIFO is empty.
 *
 * \param[in] GDMA_Channelx: Where x can be 0 to 11 to select the DMA Channel.
 *
 * \return The status of GDMA FIFO.
 * \retval SET: GDMA FIFO is empty.
 * \retval RESET: GDMA FIFO is not empty.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void Data_Uart_Handler(void)
 * {
 *     uint16_t rx_count;
 *     if (UART_GetFlagState(UART, UART_FLAG_RX_IDLE) == SET)
 *     {
 *         UART_INTConfig(UART, UART_INT_IDLE, DISABLE);
 *         if (GDMA_GetChannelStatus(UART_RX_DMA_CHANNEL_NUM))
 *         {
 *             GDMA_SuspendCmd(UART_RX_DMA_CHANNEL, ENABLE);
 *         }
 *         while (GDMA_GetFIFOStatus(UART_RX_DMA_CHANNEL) != SET);
 *     }
 * }
 * \endcode
 */
FlagStatus GDMA_GetFIFOStatus(GDMA_ChannelTypeDef *GDMA_Channelx);

/**
 * rtl876x_gdma.h
 *
 * \brief     Get GDMA transfer data length.
 *
 * \param[in] GDMA_Channelx: Where x can be 0 to 11 to select the DMA Channel.
 *
 * \return    GDMA transfer data length.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void Data_Uart_Handler(void)
 * {
 *     uint16_t rx_count;
 *     if (UART_GetFlagState(UART, UART_FLAG_RX_IDLE) == SET)
 *     {
 *         //read DMA total transfer count
 *         rx_count = GDMA_GetTransferLen(UART_RX_DMA_CHANNEL);
 *         APP_PRINT_INFO1("rx_count:%d", rx_count);
 *     }
 * }
 * \endcode
 */
uint16_t GDMA_GetTransferLen(GDMA_ChannelTypeDef *GDMA_Channelx);

/**
 * rtl876x_gdma.h
 *
 * \brief     Set GDMA LLP stucture address.
 *
 * \param[in] GDMA_Channelx: Only for GDMA_Channel0&2.
 * \param[in] Address: The address of LLP stucture.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void gdma_demo(void)
 * {
 *     GDMA_LLIDef GDMA_LLIStruct[4000];
 *     GDMA_SetLLPAddress(GDMA2_Channel0,(uint32_t)GDMA_LLIStruct);
 * }
 * \endcode
 */
void GDMA_SetLLPAddress(GDMA_ChannelTypeDef *GDMA_Channelx, uint32_t Address);

/**
 * \brief     Get GDMA Channelx of the specified DMA Channel Number.
 *
 * \param[in]  GDMA_ChannelNum: Select the GDMA channel number. \ref GDMA_CHANNEL_NUM
 *
 * \return    GDMA_Channelx.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_gdma_init(void)
 * {
 *     uint8_t channel_num = GDMA_ChannelNum;
 *     GDMA_ChannelTypeDef *GDMA_Channelx = GDMA_GetGDMAChannelx(GDMA_ChannelNum);
 * }
 * \endcode
 */
GDMA_ChannelTypeDef *GDMA_GetGDMAChannelx(uint8_t GDMA_ChannelNum);
#define DMA_CH_BASE(ChNum) (GDMA_GetGDMAChannelx(ChNum))

/**
 * rtl876x_gdma.h
 *
 * \brief  Check GDMA suspend channel status.
 *
 * \param[in] GDMA_Channelx: Where x can be 0 to 12 to select the DMA Channel.
 *
 * \return GDMA suspend channel status.
 * \retval SET: The GDMA suspend channel is inactive.
 * \retval RESET: The GDMA suspend channel is active.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void gdma_demo(void)
 * {
 *     //add user code here.
 *     while (GDMA_GetSuspendChannelStatus(GDMA_Channel0) != SET);
 * }
 * \endcode
 */
FlagStatus GDMA_GetSuspendChannelStatus(GDMA_ChannelTypeDef *GDMA_Channelx);

/**
 * rtl876x_gdma.h
 *
 * \brief  Check GDMA suspend command status.
 *
 * \param[in] GDMA_Channelx: Where x can be 0 to 12 to select the DMA Channel.
 *
 * \return GDMA suspend command status.
 * \retval SET: The GDMA channel is suspended.
 * \retval RESET: The GDMA channel is not suspended.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void gdma_demo(void)
 * {
 *     FlagStatus flag_status = GDMA_GetSuspendCmdStatus(GDMA2_Channel0);
 * }
 * \endcode
 */
FlagStatus GDMA_GetSuspendCmdStatus(GDMA_ChannelTypeDef *GDMA_Channelx);

/**
 * rtl876x_gdma.h
 *
 * \brief  Update GDMA LLP mode in multi-block.
 *
 * \param[in] GDMA_Channelx: Where x can be 0 to 12 to select the DMA Channel.
 * \param[in] GDMA LLP mode
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void gdma_demo(void)
 * {
 *     GDMA_SetLLPMode(LLI_TRANSFER);
 * }
 * \endcode
 */
void GDMA_SetLLPMode(GDMA_ChannelTypeDef *GDMA_Channelx, uint32_t mode);

#ifdef __cplusplus
}
#endif

#endif /*__RTL8762X_GDMA_H*/

/** @} */ /* End of group GDMA_Exported_Functions */
/** @} */ /* End of group GDMA */


/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/

