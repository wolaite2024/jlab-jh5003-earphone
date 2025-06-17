/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl_ethernet.h
* \brief    The header file of the peripheral ETH driver.
* \details  This file provides all ETH firmware functions.
* \author   colin
* \date     2024-06-28
* \version  v1.0
* *********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef RTL_ETHERNET_H_
#define RTL_ETHERNET_H_

#ifdef  __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "rtl_ethernet_def.h"
#include "rtl_ethernet_cpu_reg.h"

/*============================================================================*
 *                          Private Macros
 *============================================================================*/

/// Defines the MAC address length
#define MAC_ADDR_LEN                    6

/// Defines the ethernet max tx packet size
#define ETH_MAX_TX_PACKET_SIZE                          1514

/// Defines the ethernet receive packet first desc data offset
#define ETH_RX_PACKET_FIRST_DESC_DATA_OFFSET            2

#define ETH_CPU_ETH_0X130_DEFAULT_VALUE 0x01010100UL
#define ETH_CPU_ETH_0X138_DEFAULT_VALUE 0x31000000UL
#define ETH_CPU_ETH_0X134_DEFAULT_VALUE 0x40081000UL

//bit definition of tx descriptor status register
#define ETH_TX_DESC_OWN                 BIT31
#define ETH_TX_DESC_EOR                 BIT30
#define ETH_TX_DESC_FS                  BIT29
#define ETH_TX_DESC_LS                  BIT28
#define ETH_TX_DESC_CRC                 BIT23

//bit definition of rx descriptor status register
#define ETH_RX_DESC_OWN                 BIT31
#define ETH_RX_DESC_EOR                 BIT30
#define ETH_RX_DESC_FS                  BIT29
#define ETH_RX_DESC_LS                  BIT28

/** \defgroup ETHERNET        ETHERNET
  * \brief
  * \{
  */
/*============================================================================*
 *                         Constants
 *============================================================================*/
/** \defgroup ETH_Exported_Constants ETH Exported Constants
  * \brief
  * \{
  */

typedef enum
{
    ETH_LINK_UP = 0,
    ETH_LINK_DOWN = 1
} ETHLinkStatus_Typedef;

typedef enum
{
    ETH_HALF_DUPLEX = 0,
    ETH_FULL_DUPLEX = 1
} ETHDuplexMode_Typedef;

typedef enum
{
    ETH_SPEED_100M = 0x0,
    ETH_SPEED_10M = 0x1,
    ETH_SPEED_NOT_SUPPORT = 0x2,
    ETH_SPEED_NOT_ALLOWED = 0x3,
} ETHLinkSpeed_Typedef;

typedef enum
{
    ETH_STATUS_OK,
    ETH_STATUS_ERROR,
} ETHErrorStatus_Typedef;

typedef enum
{
    ETH_RX_INVALID_PARAMETER,
    ETH_RX_NO_DATA,
    ETH_RX_FRAME_ERROR,
    ETH_RX_FRAME_SUCCESS,
    ETH_RX_COPY_DATA_LEN_ERROR,
    ETH_RX_COPY_DATA_SUCCESS,
} ETHReceiveFrameStatus_Typedef;

typedef enum
{
    ETH_TX_INVALID_PARAMETER,
    ETH_TX_PACKET_SIZE_OVERFLOW,
    ETH_TX_DSC_IS_FULL,
    ETH_TX_FRAME_SUCCESS,
    ETH_TX_COPY_DATA_SUCCESS,
} ETHSendFrameStatus_Typedef;

typedef enum
{
    PHY_REG_READ_MODE = 0x00,
    PHY_REG_WRITE_MODE = 0x01,
} PHYRegOperationMode_Typedef;

typedef enum
{
    ETH_TX_CHANGE_ON_FALLING_EDGE = 0x00,
    ETH_TX_CHANGE_ON_RISING_EDGE = 0x01,
} ETHTxRefclkPhase_TypeDef;

typedef enum
{
    ETH_RX_SAMPLE_ON_FALLING_EDGE = 0x0,
    ETH_RX_SAMPLE_ON_RISING_EDGE = 0x1,
} ETHRxRefclkPhase_TypeDef;

typedef enum
{
    ETH_PHY_NORMAL_MODE = 0x0,
    ETH_PHY_LOOPBACK_MODE = 0x1,
} ETHPhyMode_TypeDef;

typedef enum
{
    ETH_NORMAL_MODE = 0,
    ETH_LOOPBACK_R2T_MODE = 1,
    ETH_LOOPBACK_T2R_MODE = 3,
} ETHMode_TypeDef;

typedef enum
{
    ETH_LOOPBACK_FORCE_SPEED_100M = 0x0,
    ETH_LOOPBACK_FORCE_SPEED_10M = 0x1,
} ETHForceSpeed_Typedef;

typedef enum
{
    ETH_IFG_0 = 0,
    ETH_IFG_1 = 1,
    ETH_IFG_2 = 2,
    ETH_IFG_3 = 3,// 9.6 us for 10 Mbps, 960 ns for 100 Mbps
    ETH_IFG_4 = 4,
    ETH_IFG_5 = 5,
    ETH_IFG_6 = 6,
    ETH_IFG_7 = 7,
} ETHInterFrameGapTime_TypeDef;

typedef enum
{
    ETH_RX_CONFIG_AAP = BIT0,
    ETH_RX_CONFIG_APM = BIT1,
    ETH_RX_CONFIG_AM = BIT2,
    ETH_RX_CONFIG_AB = BIT3,
    ETH_RX_CONFIG_AR = BIT4,
    ETH_RX_CONFIG_AER = BIT5,
    ETH_RX_CONFIG_AFC = BIT6,
    ETH_RX_CONFIG_HOME_PNA = BIT7,
} ETHRxConfig_TypeDef;

typedef enum
{
    ETH_RX_JUMBO_DISABLE = 0x0,
    ETH_RX_JUMBO_ENABLE = 0x1,
} ETHRxJumboConfig_TypeDef;

typedef enum
{
    ETH_TX_THRESHOLD_128B = 0,
    ETH_TX_THRESHOLD_256B = 1,
    ETH_TX_THRESHOLD_512B = 2,
    ETH_TX_THRESHOLD_1024B = 3,
} ETHTxThreshold_TypeDef;

typedef enum
{
    ETH_RX_THRESHOLD_1024B = 0,
    ETH_RX_THRESHOLD_128B = 1,
    ETH_RX_THRESHOLD_256B = 2,
    ETH_RX_THRESHOLD_512B = 3,
} ETHRxThreshold_TypeDef;

typedef enum
{
    ETH_TX_TRIGGER_LEVEL_1_PKT = 0,
    ETH_TX_TRIGGER_LEVEL_4_PKTS = 1,
    ETH_TX_TRIGGER_LEVEL_8_PKTS = 2,
    ETH_TX_TRIGGER_LEVEL_12_PKTS = 3,
    ETH_TX_TRIGGER_LEVEL_16_PKTS = 4,
    ETH_TX_TRIGGER_LEVEL_20_PKTS = 5,
    ETH_TX_TRIGGER_LEVEL_24_PKTS = 6,
    ETH_TX_TRIGGER_LEVEL_28_PKTS = 7,
} ETHTxTriggerLevel_TypeDef;

typedef enum
{
    ETH_RX_TRIGGER_LEVEL_1_PKT = 0,
    ETH_RX_TRIGGER_LEVEL_4_PKTS = 1,
    ETH_RX_TRIGGER_LEVEL_8_PKTS = 2,
    ETH_RX_TRIGGER_LEVEL_12_PKTS = 3,
    ETH_RX_TRIGGER_LEVEL_16_PKTS = 4,
    ETH_RX_TRIGGER_LEVEL_20_PKTS = 5,
    ETH_RX_TRIGGER_LEVEL_24_PKTS = 6,
    ETH_RX_TRIGGER_LEVEL_28_PKTS = 7,
} ETHRxTriggerLevel_TypeDef;

//RTL8201FR TX Setup time
typedef enum
{
    ETH_PHY_TX_SETUP_TIME_6NS,
    ETH_PHY_TX_SETUP_TIME_8NS,
    ETH_PHY_TX_SETUP_TIME_10NS, //default 10ns_0x1111
    ETH_PHY_TX_SETUP_TIME_12NS,
    ETH_PHY_TX_SETUP_TIME_14NS,
    ETH_PHY_TX_SETUP_TIME_16NS,
    ETH_PHY_TX_SETUP_TIME_18NS,
    ETH_PHY_TX_SETUP_TIME_NUM,
} ETHPhyTxSetupTime_TypeDef;

//RTL8201FR RX Setup time
typedef enum
{
    ETH_PHY_RX_SETUP_TIME_8NS,
    ETH_PHY_RX_SETUP_TIME_10NS, //default 10ns_0x1111
    ETH_PHY_RX_SETUP_TIME_12NS,
    ETH_PHY_RX_SETUP_TIME_14NS,
    ETH_PHY_RX_SETUP_TIME_16NS,
    ETH_PHY_RX_SETUP_TIME_18NS,
    ETH_PHY_RX_SETUP_TIME_NUM,
} ETHPhyRxSetupTime_TypeDef;

typedef enum
{
    ETH_INT_ROK = BIT0,
    ETH_INT_CNT_WRAP = BIT1,
    ETH_INT_RER_RUNT = BIT2,
    ETH_INT_RER_OVF = BIT4,
    ETH_INT_RDU = BIT5,
    ETH_INT_TOK = BIT6,
    ETH_INT_TER = BIT7,
    ETH_INT_LINK_CHANGE = BIT8,
    ETH_INT_TDU = BIT9,
    ETH_INT_SW_INT = BIT10,
    ETH_INT_RDU2 = BIT11,
    ETH_INT_RDU3 = BIT12,
    ETH_INT_RDU4 = BIT13,
    ETH_INT_RDU5 = BIT14,
    ETH_INT_RDU6 = BIT15,
} ETHINT_TypeDef;

#define IS_ETH_INT       (status)     (((status) == ETH_INT_ROK) || \
                                       ((status) == ETH_INT_CNT_WRAP) || \
                                       ((status) == ETH_INT_RER_RUNT) || \
                                       ((status) == ETH_INT_RER_OVF) || \
                                       ((status) == ETH_INT_RDU) || \
                                       ((status) == ETH_INT_TOK) || \
                                       ((status) == ETH_INT_TER) || \
                                       ((status) == ETH_INT_LINK_CHANGE) || \
                                       ((status) == ETH_INT_TDU) || \
                                       ((status) == ETH_INT_SW_INT) || \
                                       ((status) == ETH_INT_RDU2) || \
                                       ((status) == ETH_INT_RDU3) || \
                                       ((status) == ETH_INT_RDU4) || \
                                       ((status) == ETH_INT_RDU5) || \
                                       ((status) == ETH_INT_RDU6))


/**
  \brief  The structure of Tx descriptor.
*/
typedef struct
{
    uint32_t dw1;    // offset 0
    uint32_t addr;   // offset 4
    uint32_t dw2;    // offset 8
    uint32_t dw3;    // offset 12
    uint32_t dw4;    // offset 16
} ETHTxDesc_TypeDef;

/**
  \brief  The structure of Rx descriptor.
*/
typedef struct
{
    uint32_t dw1;    // offset 0
    uint32_t addr;   // offset 4
    uint32_t dw2;    // offset 8
    uint32_t dw3;    // offset 12
} ETHRxDesc_TypeDef;

// the size (unit: Bytes) of each Tx descriptor
#define ETH_TX_DESC_SIZE               sizeof(ETHTxDesc_TypeDef)
// the size (unit: Bytes) of each Rx descriptor
#define ETH_RX_DESC_SIZE               sizeof(ETHRxDesc_TypeDef)

/** End of ETH_Exported_Constants
  * \}
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** \defgroup ETH_Exported_Types ETH Exported Types
  * \brief
  * \{
  */

/**
 * \brief       ETH init structure definition.
 *
 * \ingroup     ETH_Exported_Types
 */
typedef struct
{
    ETHTxRefclkPhase_TypeDef ETH_TxRefclkPhase;
    ETHRxRefclkPhase_TypeDef ETH_RxRefclkPhase;
    ETHPhyMode_TypeDef ETH_PhyMode;
    ETHMode_TypeDef ETH_Mode;
    ETHInterFrameGapTime_TypeDef ETH_InterFrameGapTime;
    uint8_t ETH_MacAddr[6];
    uint8_t ETH_ReceiveConfig;
    ETHRxJumboConfig_TypeDef ETH_RxJumboConfig;
    ETHTxThreshold_TypeDef ETH_TxThreshold;
    ETHRxThreshold_TypeDef ETH_RxThreshold;
    ETHTxTriggerLevel_TypeDef ETH_TxTriggerLevel;
    ETHRxTriggerLevel_TypeDef ETH_RxTriggerLevel;
    ETHPhyTxSetupTime_TypeDef ETH_PhyTxSetupTime;
    ETHPhyRxSetupTime_TypeDef ETH_PhyRxSetupTime;
    uint8_t ETH_TxDescNum;
    uint8_t ETH_RxDescNum;
    volatile ETHTxDesc_TypeDef *ETH_TxDesc;
    volatile ETHRxDesc_TypeDef *ETH_RxDesc;
    volatile uint8_t *ETH_TxPktBuf;
    volatile uint8_t *ETH_RxPktBuf;
    uint8_t ETH_TxDescCurrentNum;
    uint8_t ETH_RxDescCurrentNum;
    uint8_t ETH_RxFrameStartDescIdx;
    uint32_t ETH_RxFrameLen;
    uint32_t ETH_RxSegmentCount;
    uint16_t ETH_TxAllocBufSize;
    uint16_t ETH_RxAllocBufSize;
    uint16_t ETH_TxBufSize;
    uint16_t ETH_RxBufSize;
    ETHForceSpeed_Typedef ETH_ForceSpeed;
} ETH_InitTypeDef;

/**
 * \brief       Ethernet pyh operations.
 *
 * \ingroup     ETH_Exported_Types
 */
typedef struct
{
    bool (*ETH_ResetPHY)(void);                                 /*!< Specifies eth reset phy api. */
    bool (*ETH_WaitPHYLinkUp)(
        void);                            /*!< Specifies eth wait phy link up api.*/
    bool (*ETH_PHYConfig)(ETH_InitTypeDef *ETH_InitStruct);     /*!< Specifies eth config phy api.*/
    bool (*ETH_PHYLoopback)(ETH_InitTypeDef
                            *ETH_InitStruct);   /*!< Specifies eth phy config loopback.*/
} ETHPHYOps_TypeDef;

/** End of ETH_Exported_Types
  * \}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** \defgroup ETH_Exported_Functions ETH Exported Functions
  * \brief
  * \{
  */
/**
  * \brief  Init PHY operations.
  * \param  phy_ops: PHY operations.
  * \return none.
  */
void ETH_PHYOpsInit(ETHPHYOps_TypeDef *phy_ops);

/**
  * \brief  Write PHY register.
  * \param  phy_addr: PHY address.
  * \param  address: PHY's register address.
  * \param  data: The data to be written to the above register address.
  * \return The result of the write operation.
  *         true: write success
  *         false: write failed
  */
bool ETH_WritePHYRegister(uint8_t phy_addr, uint8_t reg_address, uint16_t reg_data);

/**
  * \brief  Read PHY register.
  * \param  phy_addr: PHY address.
  * \param  address: PHY's register address.
  * \return The result of the read register data.
  */
uint16_t ETH_ReadPHYRegister(uint8_t phy_addr, uint8_t reg_address);

/**
  * \brief  Set the Tx/Rx descriptor number.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  tx_desc_no: The specified Tx descriptor number.
  * \param  rx_desc_no: The specified Rx descriptor number.
  * \return ETHErrorStatus_Typedef: The status of ETH. \ref ETHErrorStatus_Typedef.
  */
ETHErrorStatus_Typedef ETH_SetDescNum(ETH_InitTypeDef *ETH_InitStruct, uint8_t tx_desc_no,
                                      uint8_t rx_desc_no);

/**
  * \brief  Set the start address of Tx/Rx descriptor ring.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  tx_desc: The start address of Tx descriptor ring.
  * \param  rx_desc: The start address of Rx descriptor ring.
  * \return ETHErrorStatus_Typedef: The status of ETH. \ref ETHErrorStatus_Typedef.
  */
ETHErrorStatus_Typedef ETH_SetDescAddr(ETH_InitTypeDef *ETH_InitStruct, uint8_t *tx_desc,
                                       uint8_t *rx_desc);

/**
  * \brief  Set the start address of Tx/Rx packet buffer.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  tx_pkt_buf: The start address of Tx packet buffer.
  * \param  rx_pkt_buf: The start address of Rx packet buffer.
  * \return ETHErrorStatus_Typedef: The status of ETH. \ref ETHErrorStatus_Typedef.
  */
ETHErrorStatus_Typedef ETH_SetPktBuf(ETH_InitTypeDef *ETH_InitStruct, uint8_t *tx_pkt_buf,
                                     uint8_t *rx_pkt_buf);

/**
  * \brief  Set the ethernet MAC address.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  addr: The specified MAC address.
  * \return ETHErrorStatus_Typedef: The status of ETH. \ref ETHErrorStatus_Typedef.
  */
ETHErrorStatus_Typedef ETH_SetMacAddr(ETH_InitTypeDef *ETH_InitStruct, uint8_t *addr);


/**
  * \brief  Get the ethernet MAC address.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  addr: The buffer of MAC address.
  * \return ETHErrorStatus_Typedef: The status of ETH. \ref ETHErrorStatus_Typedef.
  */
ETHErrorStatus_Typedef ETH_GetMacAddr(ETH_InitTypeDef *ETH_InitStruct, uint8_t *addr);

/**
  * \brief  Set buffer size.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  tx_alloc_buf_size: Allocated tx buffer size.
  * \param  rx_alloc_buf_size: Allocated rx buffer size.
  * \param  tx_buf_size: The tx buffer size that can be used actually.
  * \param  rx_buf_size: The rx buffer size that can be used actually.
  * \return ETHErrorStatus_Typedef: The status of ETH. \ref ETHErrorStatus_Typedef.
  */
ETHErrorStatus_Typedef ETH_SetBufSize(ETH_InitTypeDef *ETH_InitStruct, uint16_t tx_alloc_buf_size,
                                      uint16_t rx_alloc_buf_size, uint16_t tx_buf_size, uint16_t rx_buf_size);

/**
  * @brief  Init ETH clock.
  * @param  None.
  * @return None.
  */
void ETH_ClkInit(void);

/**
  * \brief  Initialize the ethernet MAC controller and PHY RTL8201FR.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \return ETHErrorStatus_Typedef: The status of ETH. \ref ETHErrorStatus_Typedef.
  */
ETHErrorStatus_Typedef ETH_Init(ETH_InitTypeDef *ETH_InitStruct);

/**
  * \brief  Initialize ETH_InitTypeDef.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \return None.
  */
void ETH_StructInit(ETH_InitTypeDef *ETH_InitStruct);

/**
  * \brief  Get ethernet link speed.
  * \return ETHLinkSpeed_Typedef: ethernet link speed. \ref ETHLinkSpeed_Typedef.
  */
ETHLinkSpeed_Typedef ETH_GetLinkSpeed(void);

/**
  * \brief  Get ethernet duplex mode.
  * \return ETHDuplexMode_Typedef: ethernet duplex mode. \ref ETHDuplexMode_Typedef.
  */
ETHDuplexMode_Typedef ETH_GetDuplexMode(void);

/**
  * \brief  Enable ethernet RX.
  * \param  None.
  * \return None.
  */
void ETH_EnableRx(void);

/**
  * \brief  Close the clock of ETH and disable intterrupt.
  * \param  None.
  * \return None.
  */
void ETH_DeInit(void);

/**
  * \brief  To send frame.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  FrameLength: The length of data to be sent.
  * \return ETHSendFrameStatus_Typedef: The result of the operation. \ref ETHSendFrameStatus_Typedef.
  */
ETHSendFrameStatus_Typedef ETH_SendFrame(ETH_InitTypeDef *ETH_InitStruct, uint32_t FrameLength);

/**
  * \brief  Copy data to ethernet tx buffer.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  send_data: The address of data to be sent, must be continuous.
  * \param  send_len: The len of data to be sent.
  * \return ETHSendFrameStatus_Typedef: The result of the operation. \ref ETHSendFrameStatus_Typedef.
  */
ETHSendFrameStatus_Typedef ETH_CopyDataToTxBuffer(ETH_InitTypeDef *ETH_InitStruct,
                                                  uint8_t *send_data, uint32_t send_len);

/**
  * \brief  Enable 1st Priority DMA-Ethernet Transmit.
  * \return none.
  */
void ETH_Enable1stPriorityTxDMAETH(void);

/**
  * \brief  Check tx desc is available.
  * \return true: tx desc is available.
  *         false: tx desc is not available.
  */
bool ETH_IsTxDescAvailable(uint32_t dw1);

/**
  * \brief  Check rx desc is available.
  * \return true: rx desc is available.
  *         false: rx desc is not available.
  */
bool ETH_IsRxDescAvailable(uint32_t dw1);

/**
  * \brief  Config ethernet interrupt.
  * \param  ETH_INT: The interrupt of ethernet. \ref ETHINT_TypeDef.
  * \param  newState: Disable or enable ethernet interrupt.
  * \return none.
  */
void ETH_INTConfig(ETHINT_TypeDef ETH_INT, FunctionalState newState);

/**
  * \brief  Check ethernet interrupt is enable.
  * \param  ETH_INT: The interrupt of ethernet. \ref ETHINT_TypeDef.
  * \return SET: The interrupt is enable.
  *         RESET: The interrupt is disable.
  */
ITStatus ETH_IsINTEnable(ETHINT_TypeDef ETH_INT);

/**
  * \brief  Get ethernet interrupt status.
  * \param  ETH_INT: The interrupt of ethernet. \ref ETHINT_TypeDef.
  * \return SET: The interrupt is pending.
  *         RESET: The interrupt is not pending.
  */
ITStatus ETH_GetINTStatus(ETHINT_TypeDef ETH_INT);

/**
  * \brief  Clear ethernet interrupt status.
  * \param  ETH_INT: The interrupt of ethernet. \ref ETHINT_TypeDef.
  * \return none
  */
void ETH_ClearINTStatus(ETHINT_TypeDef ETH_INT);

/**
  * \brief  Get ethernet link status.
  * \return ETHLinkStatus_Typedef: ethernet link status. \ref ETHLinkStatus_Typedef.
  */
ETHLinkStatus_Typedef ETH_GetLinkStatus(void);

/**
  * \brief  Set rx desc back to ethernet.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  rx_desc_index: Rx desc index.
  * \param  buffer_size: Rx desc buffer size.
  * \return none.
  */
void ETH_SetRxDescBackToETH(ETH_InitTypeDef *ETH_InitStruct, uint8_t rx_desc_index,
                            uint16_t buffer_size);

/**
  * \brief  To receive frame.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \return ETHReceiveFrameStatus_Typedef: receive frame result. \ref ETHReceiveFrameStatus_Typedef.
  */
ETHReceiveFrameStatus_Typedef ETH_ReceiveFrame(ETH_InitTypeDef *ETH_InitStruct);

/**
  * \brief  Copy ethernet rx buffer to data.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  ETH_InitStruct: The pointer to ETH_InitTypeDef.
  * \param  recv_data: The address of data to be received, must be continuous.
  * \param  recv_len: The len of data to be received.
  * \param  data_size: The len of recv_data, must be large enough to receive data.
  * \return ETHReceiveFrameStatus_Typedef: copy frame result. \ref ETHReceiveFrameStatus_Typedef.
  */
ETHReceiveFrameStatus_Typedef ETH_CopyRxBufferToData(ETH_InitTypeDef *ETH_InitStruct,
                                                     uint8_t *recv_data, uint32_t *recv_len, uint32_t data_size);
#if (ETH_SUPPORT_CLOCK_CONFIG == 1)
/**
  * \brief  Config ethernet clock.
  * \param  clk_src: Select ethernet clock src. \ref ETHClkSrc_TypeDef.
  * \param  clk: Select ethernet clock. \ref ETHClk_TypeDef.
  * \param  div: Select ethernet clock div. \ref ETHClkDiv_TypeDef.
  * \return none.
  */
void ETH_ClkConfig(ETHClkSrc_TypeDef clk_src, ETHClk_TypeDef clk, ETHClkDiv_TypeDef div);
#endif

/** End of ETH_Exported_Functions
  * \}
  */

/** End of ETH
  * \}
  */

#ifdef  __cplusplus
}
#endif

#endif /* RTL_ETHERNET_H */

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor *****END OF FILE****/

