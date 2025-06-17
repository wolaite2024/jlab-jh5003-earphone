/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl876x_iso7816.h
* \brief    The header file of the peripheral ISO7816 driver.
* \details
* \author   qinyuan_hu
* @date     2024-06-18
* \version  v1.0
* *******************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef RTL876X_ISO7816_H
#define RTL876X_ISO7816_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "rtl876x_iso7816_def.h"

/** \defgroup 87X2G_ISO7816     ISO7816
  * \brief
  * \{
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** \defgroup ISO7816_Exported_Constants ISO7816 Exported Constants
  * \brief
  * \{
  */

/**
 * \defgroup    ISO7816_FIFO_SIZE ISO7816 FIFO SIZE
 * \{
 * \ingroup     ISO7816_Exported_Constants
 */
#define ISO7816_TX_FIFO_SIZE             16
#define ISO7816_RX_FIFO_SIZE             32

/** End of ISO7816_FIFO_SIZE
  * \}
  */

/**
 * \defgroup    ISO7816_Clock_Cmd ISO7816 Clock Cmd
 * \{
 * \ingroup     ISO7816_Exported_Constants
 */
#define ISO7816_CLK_CONTACT_Reset                    ISO7816_CLOCK_ENABLE_CLR
#define ISO7816_CLK_CONTACT_Set                      ISO7816_CLOCK_ENABLE_MSK

/** End of ISO7816_Clock_Cmd
  * \}
  */

/**
 * \defgroup    ISO7816_Clock_Stop_Polar ISO7816 Clock Stop Polar
 * \{
 * \ingroup     ISO7816_Exported_Constants
 */
#define ISO7816_CLOCK_STOP_POLAR_LOW                 ISO7816_CLOCK_STOP_POLAR_CLR
#define ISO7816_CLOCK_STOP_POLAR_HIGH                ISO7816_CLOCK_STOP_POLAR_MSK

/** End of ISO7816_Clock_Stop_Polar
  * \}
  */

/**
 * \defgroup    ISO7816_Power_Cmd ISO7816 Power Cmd
 * \{
 * \ingroup     ISO7816_Exported_Constants
 */
#define ISO7816_VCC_CONTACT_Reset                    ISO7816_VCC_ENABLE_CLR
#define ISO7816_VCC_CONTACT_Set                      ISO7816_VCC_ENABLE_MSK

/** End of ISO7816_Power_Cmd
  * \}
  */

/**
 * \defgroup    ISO7816_Reset_Cmd ISO7816 Reset Cmd
 * \{
 * \ingroup     ISO7816_Exported_Constants
 */
#define ISO7816_RST_CONTACT_Reset                    ISO7816_RESET_ENABLE_CLR
#define ISO7816_RST_CONTACT_Set                      ISO7816_RESET_ENABLE_MSK

/** End of ISO7816_Reset_Cmd
  * \}
  */

/**
 * \defgroup    ISO7816_Generral_Purpose_Counter_Cmd ISO7816 GP Counter Cmd
 * \{
 * \ingroup     ISO7816_Exported_Constants
 */
#define ISO7816_GP_CNT_DISABLE                       ISO7816_GP_COUNTER_ENABLE_CLR
#define ISO7816_GP_CNT_ENABLE                        ISO7816_GP_COUNTER_ENABLE_MSK

/** End of ISO7816_Generral_Purpose_Counter_Cmd
  * \}
  */

/**
 * \defgroup    ISO7816_Controller_Cmd ISO7816 Controller Cmd
 * \{
 * \ingroup     ISO7816_Exported_Constants
 */
#define ISO7816_CR_DISABLE                           ISO7816_CONTROLLER_ENBALE_CLR
#define ISO7816_CR_ENABLE                            ISO7816_CONTROLLER_ENBALE_MSK

/** End of ISO7816_Controller_Cmd
  * \}
  */

/**
 * \defgroup    ISO7816_IO_Mode ISO7816 IO Mode
 * \{
 * \ingroup     ISO7816_Exported_Constants
 */
#define ISO7816_IO_MODE_RX                           ISO7816_IO_MODE_CLR
#define ISO7816_IO_MODE_TX                           ISO7816_IO_MODE_MSK

/** End of ISO7816_IO_Mode
  * \}
  */

/**
 * \defgroup    ISO7816_Coding_Convention_Cmd ISO7816 Coding Convention Cmd
 * \{
 * \ingroup     ISO7816_Exported_Constants
 */
#define ISO7816_CODING_CONVENTION_DIRECT             ISO7816_CODING_CONVENTION_CLR
#define ISO7816_CODING_CONVENTION_INVERSE            ISO7816_CODING_CONVENTION_MSK

/** End of ISO7816_Coding_Convention_Cmd
  * \}
  */

/**
 * \defgroup    ISO7816_Coding_Convention_Cmd ISO7816 Coding Convention Cmd
 * \{
 * \ingroup     ISO7816_Exported_Constants
 */
#define ISO7816_IO_STATE_SAMPLE_DISABLE              ISO7816_IO_STATE_SAMPLE_CLR
#define ISO7816_IO_STATE_SAMPLE_ENABLE               ISO7816_IO_STATE_SAMPLE_MSK

/** End of ISO7816_Coding_Convention_Cmd
  * \}
  */

/**
 * \defgroup    ISO7816_Interrupts_Definition  ISO7816 Interrupts Definition
 * \{
 * \ingroup     ISO7816_Exported_Constants
 */
#define ISO7816_INT_RESET_TIMING_VIOLATION           ((uint16_t)(1 << 0))
#define ISO7816_INT_TX_NAK_THD                       ((uint16_t)(1 << 1))
#define ISO7816_INT_CHAR_WAIT_TIMEOUT                ((uint16_t)(1 << 2))
#define ISO7816_INT_GP_COUNTER                       ((uint16_t)(1 << 3))
#define ISO7816_INT_TX_FIFO_EMPTY                    ((uint16_t)(1 << 4))
#define ISO7816_INT_TX_FIFO_NOT_FULL                 ((uint16_t)(1 << 5))
#define ISO7816_INT_TX_FIFO_OVERFLOW                 ((uint16_t)(1 << 6))
#define ISO7816_INT_RX_FIFO_NOT_EMPTY                ((uint16_t)(1 << 7))
#define ISO7816_INT_RX_FIFO_ALMOST_FULL              ((uint16_t)(1 << 8))
#define ISO7816_INT_RX_FIFO_FULL                     ((uint16_t)(1 << 9))
#define ISO7816_INT_RX_FIFO_OVERFLOW                 ((uint16_t)(1 << 10))
#define ISO7816_INT_RX_FIFO_UNDERFLOW                ((uint16_t)(1 << 11))
#define ISO7816_INT_TX_DONE                          ((uint16_t)(1 << 12))

/** End of ISO7816_Interrupts_Definition
  * \}
  */

#define IS_ISO7816_INT(INT) (((INT) == ISO7816_INT_RESET_TIMING_VIOLATION) || ((INT) == ISO7816_INT_TX_NAK_THD)\
                             || ((INT) == ISO7816_INT_CHAR_WAIT_TIMEOUT) || ((INT) == ISO7816_INT_GP_COUNTER)\
                             || ((INT) == ISO7816_INT_TX_FIFO_EMPTY) || ((INT) == ISO7816_INT_TX_FIFO_NOT_FULL)\
                             || ((INT) == ISO7816_INT_TX_FIFO_OVERFLOW) || ((INT) == ISO7816_INT_RX_FIFO_NOT_EMPTY)\
                             || ((INT) == ISO7816_INT_RX_FIFO_ALMOST_FULL) || ((INT) == ISO7816_INT_RX_FIFO_FULL)\
                             || ((INT) == ISO7816_INT_RX_FIFO_OVERFLOW) || ((INT) == ISO7816_INT_RX_FIFO_UNDERFLOW)\
                             || ((INT) == ISO7816_INT_TX_DONE))

/**
 * \defgroup    ISO7816_Flags_Definition ISO7816 Flags Definition
 * \{
 * \ingroup     ISO7816_Exported_Constants
 */
#define ISO7816_FLAG_TX_FIFO_EMPTY                   ((uint16_t)(1 << 0))
#define ISO7816_FLAG_TX_FIFO_NOT_FULL                ((uint16_t)(1 << 1))
#define ISO7816_FLAG_TX_IDLE                         ((uint16_t)(1 << 2))
#define ISO7816_FLAG_RX_FIFO_NOT_EMPTY               ((uint16_t)(1 << 4))
#define ISO7816_FLAG_RX_FIFO_ALMOST_FULL             ((uint16_t)(1 << 5))
#define ISO7816_FLAG_RX_FIFO_FULL                    ((uint16_t)(1 << 6))
#define ISO7816_FLAG_RX_IO_HIGH_TIMING_VIOLATION     ((uint16_t)(1 << 8))
#define ISO7816_FLAG_RX_ATR_TIMING_VIOLATION         ((uint16_t)(1 << 9))
#define ISO7816_FLAG_TX_NAK_CNT_THD                  ((uint16_t)(1 << 10))
#define ISO7816_FLAG_RX_NAK_CNT_THD                  ((uint16_t)(1 << 11))
#define ISO7816_FLAG_TX_FIFO_OVERFLOW                ((uint16_t)(1 << 12))
#define ISO7816_FLAG_RX_FIFO_OVERFLOW                ((uint16_t)(1 << 13))
#define ISO7816_FLAG_RX_FIFO_UNDERFLOW               ((uint16_t)(1 << 14))
#define ISO7816_FLAG_RX_PARITY_ERR                   ((uint16_t)(1 << 15))

/** End of ISO7816_Flags_Definition
  * \}
  */

#define IS_ISO7816_FLAG(FLAG) (((FLAG) == ISO7816_FLAG_TX_FIFO_EMPTY) || ((FLAG) == ISO7816_FLAG_TX_FIFO_NOT_FULL)\
                               || ((FLAG) == ISO7816_FLAG_TX_IDLE) || ((FLAG) == ISO7816_FLAG_RX_FIFO_NOT_EMPTY)\
                               || ((FLAG) == ISO7816_FLAG_RX_FIFO_ALMOST_FULL) || ((FLAG) == ISO7816_FLAG_RX_FIFO_FULL)\
                               || ((FLAG) == ISO7816_FLAG_RX_AD_TIMING_VIOLATION) || ((FLAG) == ISO7816_FLAG_RX_CF_TIMING_VIOLATION)\
                               || ((FLAG) == ISO7816_FLAG_TX_NAK_CNT_THD) || ((FLAG) == ISO7816_FLAG_RX_NAK_CNT_THD)\
                               || ((FLAG) == ISO7816_FLAG_TX_FIFO_OVERFLOW) || ((FLAG) == ISO7816_FLAG_RX_FIFO_OVERFLOW)\
                               || ((FLAG) == ISO7816_FLAG_RX_FIFO_UNDERFLOW) || ((FLAG) == ISO7816_FLAG_RX_PARITY_ERR))

/**
 * \defgroup    ISO7816_CMD ISO7816 CMD
 * \{
 * \ingroup     ISO7816_Exported_Constants
 */
#define ISO7816_DISABLE                              ((uint8_t)(0 << 0))
#define ISO7816_ENABLE                               ((uint8_t)(1 << 0))

/** End of ISO7816_CMD
  * \}
  */

#define IS_ISO7816_CMD(CMD) (((CMD) == ISO7816_DISABLE) || ((CMD) == ISO7816_ENABLE))

/** End of ISO7816_Exported_Constants
  * \}
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** \defgroup ISO7816_Exported_Types ISO7816 Exported Types
  * \brief
  * \{
  */

/**
 * \brief       ISO7816 initialize parameters
 *
 * \ingroup     ISO7816_Exported_Types
 */
typedef struct
{
    uint32_t ISO7816_NACKCmd;            /*!< Specifies the clock rate conversion integer.*/
    uint32_t ISO7816_ProtocolType;       /*!< Specifies the protocol type.*/
    uint32_t ISO7816_ClockStopPolar;     /*!< Specifies clock stop polarity. */
    uint32_t ISO7816_IOMode;             /*!< Specifies IO mode. 0: reception mode. 1: transmission mode. */
    uint32_t ISO7816_CodingConvention;   /*!< Specifies the coding convention. */
    uint16_t ISO7816_IntegerF;           /*!< Specifies the clock rate conversion integer.*/
    uint16_t ISO7816_IntegerD;           /*!< Specifies the baud rate adjustment integer.*/
    uint8_t  ISO7816_ExtraGuardTime;     /*!< Specifies the Extra Guard Time N.
                                         This parameter must range from 0 to 255, N=255: minimum etu (12 in T=0, 11 in T=1). */
    uint16_t ISO7816_WaitingTime;        /*!< Specifies the Waiting Time.
                                         This parameter must range from 0 to 0xFFFE. */
    uint32_t ISO7816_IOStateSample;      /*!< Sample I/O line in middle of start bit to confirm if a falling edge does lead to a character or not.
                                         0: Not to sample. 1: To sample. */
    uint16_t ISO7816_RxAFWaterLevel;     /*!< Used to calculate watermark of RX FIFO almost full signal generation.
                                         RXFIFO almost full means the number of unread bytes in RXFIFO exceeds 2*SIMRFAFWM. */
    uint8_t  ISO7816_RxRetryCountLevel;  /*!< Specifies the Rx retry count level.
                                         This parameter must range from 0 to 0xF.In T=0 application only. */
    uint8_t  ISO7816_TxRetryCountLevel;  /*!< Specifies the Tx retry count level.
                                         This parameter must range from 0 to 0xF.In T=0 application only. */
} ISO7816_InitTypeDef;

/** End of ISO7816_Exported_Types
  * \}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** \defgroup ISO7816_Exported_Functions ISO7816 Exported Functions
  * \brief
  * \{
  */

/**
 * \brief   Deinitialize the ISO7816 peripheral registers to their default reset values(turn off ISO7816 clock).
 * \param   None.
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_smartcard_init(void)
 * {
 *    ISO7816_DeInit();
 * }
 * \endcode
 */
void ISO7816_DeInit(void);

/**
 * \brief       Initialize the ISO7816 peripheral according to the specified parameters in ISO7816_InitStruct.
 * \param[in]   ISO7816_InitStruct: Pointer to a ISO7816_InitTypeDef structure that contains
 *                               the configuration information for the ISO7816 peripheral.
 * \return      None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_smartcard_init(void)
 * {
 *     RCC_PeriphClockCmd(APBPeriph_ISO7816, APBPeriph_ISO7816_CLOCK, ENABLE);
 *
 *     ISO7816_InitTypeDef ISO7816_InitStruct;
 *     ISO7816_StructInit(&ISO7816_InitStruct);
 *     ISO7816_InitStruct.ISO7816_IntegerF       = 372;
 *     ISO7816_InitStruct.ISO7816_IntegerD       = 1;
 *     ISO7816_InitStruct.ISO7816_RxAFWaterLevel = 0x5;
 *     //Add other initialization parameters that need to be configured here.
 *     ISO7816_Init(&ISO7816_InitStruct);
 * }
 * \endcode
 */
void ISO7816_Init(ISO7816_InitTypeDef *ISO7816_InitStruct);

/**
 * \brief   Fills each ISO7816_InitStruct member with its default value.
 * \param[in]   ISO7816_InitStruct: pointer to an ISO7816_InitTypeDef structure which will be initialized.
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_smartcard_init(void)
 * {
 *     ISO7816_DeInit();
 *
 *     RCC_PeriphClockCmd(APBPeriph_ISO7816, APBPeriph_ISO7816_CLOCK, ENABLE);
 *
 *     ISO7816_InitTypeDef ISO7816_InitStruct;
 *     ISO7816_StructInit(&ISO7816_InitStruct);
 *     ISO7816_InitStruct.ISO7816_IntegerF       = 372;
 *     ISO7816_InitStruct.ISO7816_IntegerD       = 1;
 *     ISO7816_InitStruct.ISO7816_RxAFWaterLevel = 0x5;
 *     //Add other initialization parameters that need to be configured here.
 *     ISO7816_Init(&ISO7816_InitStruct);
 * }
 * \endcode
 */
void ISO7816_StructInit(ISO7816_InitTypeDef *ISO7816_InitStruct);

/**
 * rtl876x_sc.h
 * \brief   Enables or disables the specified ISO7816 interrupts.
 * \param[in] ISO7816_IT: Specified the ISO7816 interrupt source that to be enabled or disabled.
 *      This parameter can be any combination of the following values:
 *      \arg ISO7816_INT_RD_AVA: Rx data avaliable interrupt source.
 *      \arg ISO7816_INT_TX_FIFO_EMPTY: Tx FIFO empty interrupt source.
 *      \arg ISO7816_INT_RX_LINE_STS: Rx line status interrupt source.
 *      \arg ISO7816_INT_MODEM_STS: Modem status interrupt source.
 *      \arg ISO7816_INT_TX_DONE: Tx done interrupt source.
 *      \arg ISO7816_INT_TX_THD: Tx threshold(FIFO data length <= thredhold) interrupt source.
 *      \arg ISO7816_INT_RX_BREAK: Rx break signal interrupt source.
 *      \arg ISO7816_INT_RX_IDLE: Rx bus ilde interrupt source.
 * \param[in] newState: New state of the specified ISO7816 interrupt source.
 *      This parameter can be: ENABLE or DISABLE.
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_smartcard_init(void)
 * {
 *     ISO7816_DeInit();
 *
 *     RCC_PeriphClockCmd(APBPeriph_ISO7816, APBPeriph_ISO7816_CLOCK, ENABLE);
 *
 *     ISO7816_InitTypeDef ISO7816_InitStruct;
 *     ISO7816_StructInit(&ISO7816_InitStruct);
 *     ISO7816_InitStruct.ISO7816_IntegerF       = 372;
 *     ISO7816_InitStruct.ISO7816_IntegerD       = 1;
 *     ISO7816_InitStruct.ISO7816_RxAFWaterLevel = 0x5;
 *     //Add other initialization parameters that need to be configured here.
 *     ISO7816_Init(&ISO7816_InitStruct);
 *
 *     ISO7816_INTConfig(ISO7816_INT_RX_FIFO_NOT_EMPTY, ENABLE);
 * }
 * \endcode
 */
void ISO7816_INTConfig(uint32_t ISO7816_INT, FunctionalState NewState);

/**
 * \brief   Send the specified data to TX FIFO.
 * \param[in] inBuf: Buffer of data to be sent.
 * \param[in] len: Length of data to be sent.
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void smartcard_demo(void)
 * {
 *     uint8_t data[] = "ISO7816 demo";
 *     ISO7816_SendData(data, sizeof(data));
 * }
 * \endcode
 */
void ISO7816_SendData(const uint8_t *inBuf, uint16_t len);

/**
 * \brief   Receive data from RX FIFO.
 * \param[out] outBuf: Buffer to store data which read from RX FIFO.
 * \param[in]  len: Length of data to read.
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void smartcard_demo(void)
 * {
 *     uint8_t data[32] = {10};
 *     ISO7816_ReceiveData(data, 10);
 * }
 * \endcode
 */
void ISO7816_ReceiveData(uint8_t *outBuf, uint16_t len);

/**
 * \brief   Cold reset the smartcard.
 * \param   None.
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void smartcard_demo(void)
 * {
 *     ISO7816_CardColdReset();
 * }
 * \endcode
 */
void ISO7816_CardColdReset(void);

/**
 * \brief   Warm reset the smartcard.
 * \param   None.
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void smartcard_demo(void)
 * {
 *     ISO7816_CardWarmReset();
 * }
 * \endcode
 */
void ISO7816_CardWarmReset(void);

/**
 * \brief   Stop the clock output of the smartcard.
 * \param   None.
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void smartcard_demo(void)
 * {
 *     ISO7816_CardClockStop();
 * }
 */
void ISO7816_CardClockStop(void);

/**
 * \brief   Restart the clock output of the smartcard.
 * \param   None.
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void smartcard_demo(void)
 * {
 *     ISO7816_CardClockRestart();
 * }
 * \endcode
 */
void ISO7816_CardClockRestart(void);

/**
 * \brief   Deactive a physically connected smartcard.
 * \param   None.
 * \return  None.
 */
void ISO7816_CardDeactive(void);

/**
 * \brief  Config delay based on cycle count Which should be count/Freq (s), given unit of Freq is Hz.
 * \param[in]  cycle: count of cycle to delay.
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void smartcard_demo(void)
 * {
 *     ISO7816_DelayCycle(400);
 * }
 * \endcode
 */
void ISO7816_DelayCycle(uint32_t cycle);

/**
 * \brief       Check whether the specified SmartCard flag is set.
 * \param[in]   ISO7816_FLAG: Specified ISO7816 flag to check.
 *              This parameter can be one of the following values:
 *              \arg ISO7816_FLAG_RX_DATA_AVA: Rx data is avaliable.
 *              \arg ISO7816_FLAG_RX_OVERRUN: Rx FIFO overrun.
 *              \arg ISO7816_FLAG_RX_PARITY_ERR: Rx parity error.
 *              \arg ISO7816_FLAG_RX_FRAME_ERR: Rx frame error.
 *              \arg ISO7816_FLAG_RX_BREAK_ERR: Rx break error.
 *              \arg ISO7816_FLAG_TX_FIFO_EMPTY: Tx Holding Register or Tx FIFO empty
 *              \arg ISO7816_FLAG_TX_EMPTY: Tx FIFO and Tx shift register are both empty.
 *              \arg ISO7816_FLAG_RX_FIFO_ERR: Rx FIFO error.
 *              \arg ISO7816_FLAG_RX_BREAK: Rx break.
 *              \arg ISO7816_FLAG_RX_IDLE: Rx idle.
 *              \arg ISO7816_FLAG_TX_DONE: Tx waveform done & TX_FIFO_EMPTY = 1.
 *              \arg ISO7816_FLAG_TX_THD: TX_FIFO_LEVEL<=txfifo_trigger_level.
 * \return      New status of ISO7816 flag.
 * \retval      SET: The specified ISO7816 flag bit is set.
 * \retval      RESET: The specified flag is not set.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void ISO7816_senddata_continuous(ISO7816_TypeDef *ISO7816x, const uint8_t *pSend_Buf, uint16_t vCount)
 * {
 *     uint8_t count;
 *
 *     while (vCount / ISO7816_TX_FIFO_SIZE > 0)
 *     {
 *         while (ISO7816_GetFlagStatus(ISO7816_FLAG_TX_FIFO_EMPTY) == 0);
 *         for (count = ISO7816_TX_FIFO_SIZE; count > 0; count--)
 *         {
 *             ISO7816x->RB_THR = *pSend_Buf++;
 *         }
 *         vCount -= ISO7816_TX_FIFO_SIZE;
 *     }
 *
 *     while (ISO7816_GetFlagStatus(ISO7816_FLAG_TX_FIFO_EMPTY) == 0);
 *     while (vCount--)
 *     {
 *         ISO7816x->RB_THR = *pSend_Buf++;
 *     }
 * }
 * \endcode
 */
FlagStatus ISO7816_GetFlagStatus(uint32_t ISO7816_FLAG);

/**
 * \brief   Active a physically connected smartcard.
 * \param   None.
 * \return  None.
 */
void ISO7816_CardActivtion(void);

/**
 * \brief       Check whether the specified ISO7816 interrupt source is set or not.
                Software reads to clear all interrupts.
 * \param[in]   ISO7816_INT: specifies the interrupt source to check.
 *              This parameter can be one of the following values:
 *              \arg ISO7816_INT_RESET_TIMING_VIOLATION: Reset timing violation.
 *              \arg ISO7816_INT_TX_NAK_THD:             TNAKTH exceeded.
 *              \arg ISO7816_INT_RX_WAIT_TIMEOUT:        Character wait timeout.
 *              \arg ISO7816_INT_GP_COUNTER:             General purpose counter hit.
 *              \arg ISO7816_INT_TX_FIFO_EMPTY:          TX FIFO empty.
 *              \arg ISO7816_INT_TX_FIFO_NOT_FULL:       TX FIFO not full, triggered when TX FIFO changes from full to non-full.
 *              \arg ISO7816_INT_TX_FIFO_OVERFLOW:       TX FIFO overflow.
 *              \arg ISO7816_INT_RX_FIFO_NOT_EMPTY:      RX FIFO not empty, triggered when RX FIFO changes from empty to non-empty.
 *              \arg ISO7816_INT_RX_FIFO_ALMOST_FULL:    RX FIFO almost full
 *              \arg ISO7816_INT_RX_FIFO_FULL:           RX FIFO full.
 *              \arg ISO7816_INT_RX_FIFO_OVERFLOW:       RX FIFO overflow.
 *              \arg ISO7816_INT_RX_FIFO_UNDERFLOW:      RX FIFO underflow.
 *              \arg ISO7816_INT_TX_DONE:                Transmission completed, triggered when TX FIFO is empty and the last bit has been sent out.
 * \return      The new state of ISO7816_INT (SET or RESET).
 */
uint16_t ISO7816_GetINTStatus(void);

/**
 * \brief       Check whether the specified ISO7816 error status is set or not.
 * \param[in]   None.
 * \return      None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void smartcard_demo(void)
 * {
 *     ISO7816_GPCounterRestart(400);
 * }
 * \endcode
  */
uint8_t ISO7816_GetErrStatus(void);

/**
 * \brief       Clear the specified ISO7816 error status.
 * \param[in]   status: 0~0xFFFF.
 * \return      None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void smartcard_demo(void)
 * {
 *     ISO7816_GPCounterRestart(400);
 * }
 * \endcode
  */
void ISO7816_ClearErrStatus(uint8_t status);

/**
 * \brief       Send one byte of data.
 * \param[in]   data: Byte data to send.
 * \return      None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void smartcard_demo(void)
 * {
 *     uint8_t data = 0x55;
 *     ISO7816_SendByte(data);
 * }
 * \endcode
 */
void ISO7816_SendByte(uint8_t data);

/**
 * \brief   Read one byte of data from SmartCard RX FIFO.
 * \param   None.
 * \return  Which byte data has been read.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void smartcard_demo(void)
 * {
 *     uint8_t data = ISO7816_ReceiveByte();
 *
 * }
 * \endcode
 */
uint8_t ISO7816_ReceiveByte(void);

/**
 * \brief   Clear Tx FIFO of SmartCard peripheral.
 * \param   None.
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void smartcard_demo(void)
 * {
 *     ISO7816_ClearTxFIFO();
 * }
 * \endcode
 */
void ISO7816_ClearTxFIFO(void);

/**
 * \brief   Clear Rx FIFO of SmartCard peripheral.
 * \param   None.
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void smartcard_demo(void)
 * {
 *     ISO7816_ClearRxFIFO();
 * }
 * \endcode
 */
void ISO7816_ClearRxFIFO(void);

/**
 * \brief       Restart the general purpose counter.
 * \param[in]   value: counter limitation, 0~0xFFFF.
 * \return      None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void smartcard_demo(void)
 * {
 *     ISO7816_GPCounterRestart(400);
 * }
 * \endcode
  */
void ISO7816_GPCounterRestart(uint16_t value);

/**
 * \brief       Set the general purpose counter value.
 * \param[in]   value: counter limitation, 0~0xFFFF.
 * \return      None.
 */
void ISO7816_GPCounterSet(uint16_t value);

/**
 * \brief       Enable the general purpose counter.
 * \param[in]   None.
 * \return      None.
 */
void ISO7816_GPCounterEnable(void);

/**
 * \brief       Disable the general purpose counter.
 * \param[in]   None.
 * \return      None.
 */
void ISO7816_GPCounterDisable(void);

/**
 * \brief       Set RST output high level.
 * \param[in]   None.
 * \return      None.
 */
void ISO7816_SetRST(void);

/**
 * \brief       Config the specified ISO7816 protocol type.
 * \param[in]   protocol:  0: T=0, mandatory error signal and character repetition; 1: T=1.
 * \return      None.
 */
void ISO7816_ProtocolTypeConfig(uint8_t protocol);

/**
 * \brief       Config the clk output polar when clock stop.
 * \param[in]   polar: 0: low level, 1: high level.
 * \return      None.
 */
void ISO7816_ClockPolarConfig(uint8_t polar);

/**
 * \brief       Config the character convention of TS.
 * \param[in]   convention: 0: direct convention, 1: inverse convention.
 * \return      None.
 */
void ISO7816_ConventionConfig(uint8_t convention);

/**
 * \brief       Set speed of etu time (equal to F/D).
 * \param[in]   value: 0~0xFFF.
 * \return      None.
 */
void ISO7816_SetSpeed(uint16_t speed);

/**
 * \brief       Set extra guard time N (in unit of etu).
 * \param[in]   time: 0~0xFF.
 * \return      None.
 */
void ISO7816_SetGT(uint8_t time);

/**
 * \brief       Set waiting time (WWT in T=0 or CWT in T=1).
 * \param[in]   value: 0~0xFFFE.
 * \return      None.
 */
void ISO7816_SetWT(uint16_t time);

/** End of ISO7816_Exported_Functions
  * \}
  */

/** End of ISO7816
  * \}
  */

#ifdef __cplusplus
}
#endif

#endif /* RTL876X_ISO7816_H */


/******************* (C) COPYRIGHT 2024 Realtek Semiconductor *****END OF FILE****/

