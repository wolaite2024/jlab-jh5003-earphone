/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* \file     rtl876x_keyscan.h
* \brief    The header file of the peripheral KEYSCAN driver.
* \details  This file provides all KEYSCAN firmware functions.
* \author
* \date
* \version  v1.0
* *********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef RTL876X_KEYSCAN_H
#define RTL876X_KEYSCAN_H

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include "rtl876x_keyscan_def.h"

/** \defgroup KEYSCAN     KEYSCAN
  * \brief
  * \{
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/
/** \defgroup KEYSCAN_Exported_Constants KEYSCAN Exported Constants
  * \brief
  * \{
  */

/**
 * \defgroup    KEYSCAN_FIFO_Depth KEYSCAN FIFO Depth
 * \{
 * \ingroup     KEYSCAN_Exported_Constants
 */
#define KEYSCAN_FIFO_DEPTH              108

/** End of KEYSCAN_FIFO_Depth
  * \}
  */

/**
 * \defgroup    Keyscan_Row_Number Keyscan Row Number
 * \{
 * \ingroup     KEYSCAN_Exported_Constants
 */
#define IS_KEYSCAN_ROW_NUM(ROW) ((ROW) <= 12)

/** End of Keyscan_Row_Number
  * \}
  */

/**
 * \defgroup    Keyscan_Column_Number Keyscan Column Number
 * \{
 * \ingroup     KEYSCAN_Exported_Constants
 */
#define IS_KEYSCAN_COL_NUM(COL) ((COL) <= 20)

/** End of Keyscan_Column_Number
  * \}
  */

/**
 * \defgroup    Keyscan_Debounce_Config Keyscan Debounce Config
 * \{
 * \ingroup     KEYSCAN_Exported_Constants
 */
#define IS_KEYSCAN_DEBOUNCE_EN(EN) (((EN) == ENABLE) || ((EN) == DISABLE))

/** End of Keyscan_Debounce_Config
  * \}
  */

/**
 * \defgroup    Keyscan_Scan_Interval_Enable Keyscan Scan Interval Enable
 * \{
 * \ingroup     KEYSCAN_Exported_Constants
 */
#define IS_KEYSCAN_SCANINTERVAL_EN(EN) (((EN) == ENABLE) || ((EN) == DISABLE))

/** End of Keyscan_Scan_Interval_Enable
  * \}
  */

/**
 * \defgroup    Keyscan_Release_Detect_Timer_Enable Keyscan Release Detect Timer Enable
 * \{
 * \ingroup     KEYSCAN_Exported_Constants
 */
#define IS_KEYSCAN_RELEASE_DETECT_EN(EN) (((EN) == ENABLE) || ((EN) == DISABLE))

/** End of Keyscan_Release_Detect_Timer_Enable
  * \}
  */

/**
 * \defgroup    Keyscan_scan_mode Keyscan scan mode
 * \{
 * \ingroup     KEYSCAN_Exported_Constants
 */
typedef enum
{
    KeyScan_Manual_Scan_Mode = 0x00,
    KeyScan_Auto_Scan_Mode = 0x01,
} KEYSCANScanMode_TypeDef;

#define IS_KEYSCAN_SCAN_MODE(MODE)  (((MODE) == KeyScan_Manual_Scan_Mode) || ((MODE) == KeyScan_Auto_Scan_Mode))

/** End of Keyscan_scan_mode
  * \}
  */

/**
 * \defgroup    Keyscan_Press_Detect_Mode Keyscan Press Detect Mode
 * \{
 * \ingroup     KEYSCAN_Exported_Constants
 */
typedef enum
{
    KeyScan_Detect_Mode_Edge = 0x00,
    KeyScan_Detect_Mode_Level = 0x01,
} KEYSCANPressDetectMode_TypeDef;

#define IS_KEYSCAN_DETECT_MODE(MODE)    (((MODE) == KeyScan_Detect_Mode_Edge) || ((MODE) == KeyScan_Detect_Mode_Level))

/** End of Keyscan_Press_Detect_Mode
  * \}
  */

/**
 * \defgroup    Keyscan_Fifo_Overflow_Control Keyscan Fifo Overflow Control
 * \{
 * \ingroup     KEYSCAN_Exported_Constants
 */
typedef enum
{
    KeyScan_FIFO_OVR_CTRL_DIS_ALL = 0x00,
    KeyScan_FIFO_OVR_CTRL_DIS_LAST = 0x01,
} KEYSCANFifoOverflowControl_TypeDef;

#define IS_KEYSCAN_FIFO_OVR_CTRL(CTRL)  (((CTRL) == KeyScan_FIFO_OVR_CTRL_DIS_ALL) || ((CTRL) == KeyScan_FIFO_OVR_CTRL_DIS_LAST))

/** End of Keyscan_Fifo_Overflow_Control
  * \}
  */

/**
 * \brief       Keyscan_Manual_Sel_Mode Keyscan Manual Sel Mode
 *
 * \ingroup     KEYSCAN_Exported_Constants
 */
typedef enum
{
    KeyScan_Manual_Sel_Bit = 0x00,
    KeyScan_Manual_Sel_Key = 0x01,
} KEYSCANManualMode_TypeDef;
/** End of Keyscan_Manual_Sel_Mode
  * \}
  */

/**
 * \defgroup    Keyscan_Key_Limit Keyscan Key Limit
 * \{
 * \ingroup     KEYSCAN_Exported_Constants
 */
#define IS_KEYSCAN_KEY_LIMIT(DATA_NUM) ((DATA_NUM) <= KEYSCAN_FIFO_DEPTH) //0 means no limit

/** End of Keyscan_Key_Limit
  * \}
  */

/**
 * \defgroup    Keyscan_Interrupt Keyscan Interrupt
 * \{
 * \ingroup     KEYSCAN_Exported_Constants
 */
#define KEYSCAN_INT_THRESHOLD                    BIT4
#define KEYSCAN_INT_OVER_READ                    BIT3
#define KEYSCAN_INT_SCAN_END                     BIT2
#define KEYSCAN_INT_FIFO_NOT_EMPTY               BIT1
#define KEYSCAN_INT_ALL_RELEASE                  BIT0
#define IS_KEYSCAN_CONFIG_IT(IT) ((((IT) & (uint32_t)0xFFF8) == 0x00) && ((IT) != 0x00))

/** End of Keyscan_Interrupt
  * \}
  */

/**
 * \defgroup    Keyscan_Flag Keyscan Flag
 * \{
 * \ingroup     KEYSCAN_Exported_Constants
 */
#define KEYSCAN_FLAG_FIFOLIMIT                       BIT20
#define KEYSCAN_INT_FLAG_THRESHOLD                   BIT19
#define KEYSCAN_INT_FLAG_OVER_READ                   BIT18
#define KEYSCAN_INT_FLAG_SCAN_END                    BIT17
#define KEYSCAN_INT_FLAG_FIFO_NOT_EMPTY              BIT16
#define KEYSCAN_INT_FLAG_ALL_RELEASE                 BIT15
#define KEYSCAN_FLAG_DATAFILTER                      BIT3
#define KEYSCAN_FLAG_OVR                             BIT2
#define KEYSCAN_FLAG_FULL                            BIT1
#define KEYSCAN_FLAG_EMPTY                           BIT0
#define IS_KEYSCAN_FLAG(FLAG)       ((((FLAG) & (uint32_t)0x01FF) == 0x00) && ((FLAG) != (uint32_t)0x00))
#define IS_KEYSCAN_CLEAR_FLAG(FLAG) ((((FLAG) & (uint32_t)0x00C0) == 0x00) && ((FLAG) != (uint32_t)0x00))

/** End of Keyscan_Flag
  * \}
  */

/** End of KEYSCAN_Exported_Constants
  * \}
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** \defgroup KEYSCAN_Exported_Types KEYSCAN Exported Types
  * \brief
  * \{
  */

/**
 * \brief       Keyscan initialize parameters
 *
 * \ingroup     KEYSCAN_Exported_Types
 */
typedef struct
{
    uint16_t rowSize;                               /*!< Specifies Keyscan Row Size.
                                                         This parameter can be a value <=12. */

    uint16_t colSize;                               /*!< Specifies Keyscan Column Size.
                                                         This parameter can be a value <=20. */

    uint16_t clockdiv;                              /*!< Specifies Keyscan clock divider.
                                                         scan clock = system clock/(SCAN_DIV+1). */

    uint8_t delayclk;                               /*!< Specifies Keyscan delay clock divider.
                                                         delay clock = scan clock/(DELAY_DIV+1). */

    FunctionalState debounceEn;                     /*!< Enable or disable debounce. */

    FunctionalState scantimerEn;                    /*!< Enable or disable scan timer. */

    FunctionalState detecttimerEn;                  /*!< Enable or disable detect timer. */

    uint16_t debouncecnt;                            /*!< Specifies Keyscan Debounce time. */

    uint16_t scanInterval;                          /*!< Specifies Keyscan release time. */

    uint16_t releasecnt;                             /*!< Specifies Keyscan release time. */

    KEYSCANScanMode_TypeDef scanmode;               /*!< Specifies Keyscan mode. */

    KEYSCANPressDetectMode_TypeDef detectMode;      /*!< Specifies Key Detect mode. */

    uint16_t fifotriggerlevel;                      /*!< Specifies Keyscan fifo threshold
                                                         to trigger interrupt KEYSCAN_INT_THRESHOLD. */

    KEYSCANFifoOverflowControl_TypeDef
    fifoOvrCtrl;                                    /*!< Specifies Keyscan fifo over flow control. */

    uint8_t keylimit;                               /*!< Specifies max scan data allowable in each scan. */

    KEYSCANManualMode_TypeDef
    manual_sel;                /*!< Specifies trigger mode in manual mode. */

#if KEYSCAN_SUPPORT_ROW_LEVEL_CONFIGURE
    FunctionalState rowpullhighEn;                  /*!< Configure Keyscan row pull.
                                                         This parameter can be a value of DISABLE or ENABLE. */
#endif
#if KEYSCAN_SUPPORT_COLUNM_LEVEL_CONFIGURE
    FunctionalState colunmoutputhighEn;             /*!< Configure Keyscan column output.
                                                         This parameter can be a value of DISABLE or ENABLE. */
#endif
} KEYSCAN_InitTypeDef;

/** End of KEYSCAN_Exported_Types
  * \}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** \defgroup KEYSCAN_Exported_Functions KEYSCAN Exported Functions
  * \brief
  * \{
  */

/**
 * \brief  Deinitializes the Keyscan peripheral registers to their default reset values(turn off keyscan clock).
 *
 * \param[in]  KeyScan: Selected KeyScan peripheral.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_keyscan_init(void)
 * {
 *     KeyScan_DeInit(KEYSCAN);
 * }
 * \endcode
 */
void KeyScan_DeInit(KEYSCAN_TypeDef *KeyScan);

/**
 * \brief   Initializes the KeyScan peripheral according to the specified
 *          parameters in the KeyScan_InitStruct
 *
 * \param[in]  KeyScan: Selected KeyScan peripheral.
 * \param[in]  KeyScan_InitStruct: Pointer to a KEYSCAN_InitTypeDef structure that
 *             contains the configuration information for the specified KeyScan peripheral
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_keyscan_init(void)
 * {
 *     RCC_PeriphClockCmd(APBPeriph_KEYSCAN, APBPeriph_KEYSCAN_CLOCK, ENABLE);

 *     KEYSCAN_InitTypeDef KEYSCAN_InitStruct;
 *     KeyScan_StructInit(&KEYSCAN_InitStruct);

 *     KEYSCAN_InitStruct.rowSize  = KEYBOARD_ROW_SIZE;
 *     KEYSCAN_InitStruct.colSize  = KEYBOARD_COLUMN_SIZE;
 *     KEYSCAN_InitStruct.scanmode     = KeyScan_Manual_Scan_Mode;
 *     KEYSCAN_InitStruct.debounceEn   = KeyScan_Debounce_Enable;

 *     KeyScan_Init(KEYSCAN, &KEYSCAN_InitStruct);

 *     KeyScan_INTConfig(KEYSCAN, KEYSCAN_INT_SCAN_END, ENABLE);
 *     KeyScan_ClearINTPendingBit(KEYSCAN, KEYSCAN_INT_SCAN_END);
 *     KeyScan_INTMask(KEYSCAN, KEYSCAN_INT_SCAN_END, DISABLE);  // Unmask keyscan interrupt
 *     KeyScan_Cmd(KEYSCAN, ENABLE);
 * }
 * \endcode
 */
void KeyScan_Init(KEYSCAN_TypeDef *KeyScan, KEYSCAN_InitTypeDef *KeyScan_InitStruct);

/**
 * \brief  Fills each Keyscan_InitStruct member with its default value.
 *
 * \param[in]  KeyScan_InitStruct: Pointer to a KEYSCAN_InitTypeDef structure which will be initialized.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_keyscan_init(void)
 * {
 *     RCC_PeriphClockCmd(APBPeriph_KEYSCAN, APBPeriph_KEYSCAN_CLOCK, ENABLE);

 *     KEYSCAN_InitTypeDef KEYSCAN_InitStruct;
 *     KeyScan_StructInit(&KEYSCAN_InitStruct);

 *     KEYSCAN_InitStruct.rowSize  = KEYBOARD_ROW_SIZE;
 *     KEYSCAN_InitStruct.colSize  = KEYBOARD_COLUMN_SIZE;
 *     KEYSCAN_InitStruct.scanmode     = KeyScan_Manual_Scan_Mode;
 *     KEYSCAN_InitStruct.debounceEn   = KeyScan_Debounce_Enable;

 *     KeyScan_Init(KEYSCAN, &KEYSCAN_InitStruct);

 *     KeyScan_INTConfig(KEYSCAN, KEYSCAN_INT_SCAN_END, ENABLE);
 *     KeyScan_ClearINTPendingBit(KEYSCAN, KEYSCAN_INT_SCAN_END);
 *     KeyScan_INTMask(KEYSCAN, KEYSCAN_INT_SCAN_END, DISABLE);  // Unmask keyscan interrupt
 *     KeyScan_Cmd(KEYSCAN, ENABLE);
 * }
 * \endcode
 */
void KeyScan_StructInit(KEYSCAN_InitTypeDef *KeyScan_InitStruct);

/**
 * \brief  Enables or disables the specified KeyScan interrupt.
 *
 * \param[in] KeyScan: Selected KeyScan peripheral.
 * \param[in] KeyScan_IT: Specifies the KeyScan interrupts sources to be enabled or disabled.
 *            This parameter can be any combination of the following values:
 *            \arg KEYSCAN_INT_THRESHOLD: Kescan FIFO data over threshold interrupt.
 *            \arg KEYSCAN_INT_OVER_READ: KeyScan over read interrupt.
 *            \arg KEYSCAN_INT_SCAN_END: KeyScan scan end interrupt.
 *            \arg KEYSCAN_INT_FIFO_NOT_EMPTY: KeyScan FIFO not empty interrupt.
 *            \arg KEYSCAN_INT_ALL_RELEASE: KeyScan all key release interrupt.
 * \param[in] NewState: New state of the specified KeyScan interrupts.
 *            This parameter can be: ENABLE or DISABLE.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void keyscan_demo(void)
 * {
 *     KeyScan_INTMask(KEYSCAN, KEYSCAN_INT_SCAN_END, ENABLE);
 *     KeyScan_INTConfig(KEYSCAN, KEYSCAN_INT_SCAN_END, ENABLE);
 *     KeyScan_INTMask(KEYSCAN, KEYSCAN_INT_SCAN_END, DISABLE);
 * }
 * \endcode
 */
void KeyScan_INTConfig(KEYSCAN_TypeDef *KeyScan, uint32_t KeyScan_IT,
                       FunctionalState NewState);

/**
 * \brief  Mask the specified KeyScan interrupt.
 *
 * \param[in] KeyScan: Selected KeyScan peripheral.
 * \param[in] KeyScan_IT: Specifies the KeyScan interrupts sources to be enabled or disabled.
 *            This parameter can be any combination of the following values:
 *            \arg KEYSCAN_INT_THRESHOLD: Kescan FIFO data over threshold interrupt.
 *            \arg KEYSCAN_INT_OVER_READ: KeyScan over read interrupt.
 *            \arg KEYSCAN_INT_SCAN_END: KeyScan scan end interrupt.
 *            \arg KEYSCAN_INT_FIFO_NOT_EMPTY: KeyScan FIFO not empty interrupt.
 *            \arg KEYSCAN_INT_ALL_RELEASE: KeyScan all key release interrupt.
 * \param[in] NewState: New state of the specified KeyScan interrupts mask.
 *            This parameter can be: ENABLE or DISABLE.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void keyscan_demo(void)
 * {
 *     KeyScan_INTMask(KEYSCAN, KEYSCAN_INT_SCAN_END, ENABLE);
 * }
 * \endcode
 */
void KeyScan_INTMask(KEYSCAN_TypeDef *KeyScan, uint32_t KeyScan_IT,
                     FunctionalState NewState);

/**
 * \brief  Read data from keyscan FIFO.
 *
 * \param[in]  KeyScan: Selected KeyScan peripheral.
 * \param[out] outBuf: Buffer to save data read from KeyScan FIFO.
 * \param[in]  count: Data length to be read.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void keyscan_demo(void)
 * {
 *     uint16_t data[3] = {0};
 *     KeyScan_Read(KEYSCAN, data, 3);
 * }
 * \endcode
 */
void KeyScan_Read(KEYSCAN_TypeDef *KeyScan, uint16_t *outBuf, uint16_t count);

/**
 * \brief   Enable or disable the KeyScan peripheral.
 *
 * \param[in] KeyScan: Selected KeyScan peripheral.
 * \param[in] NewState: New state of the KeyScan peripheral.
 *            This parameter can be: ENABLE or DISABLE.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void keyscan_demo(void)
 * {
 *     KeyScan_Cmd(KEYSCAN, ENABLE);
 * }
 * \endcode
 */
void KeyScan_Cmd(KEYSCAN_TypeDef *KeyScan, FunctionalState NewState);

/**
 * \brief   Set filter data.
 *
 * \param[in] KeyScan: Selected KeyScan peripheral.
 * \param[in] data: Config the data to be filtered.
 *            This parameter should not be more than 9 bits.
 * \param[in] NewState: New state of the KeyScan peripheral.
 *            This parameter can be: ENABLE or DISABLE.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void keyscan_demo(void)
 * {
 *     KeyScan_FilterDataConfig(KEYSCAN, 0x01, ENABLE);
 *
 * }
 * \endcode
 */
void KeyScan_FilterDataConfig(KEYSCAN_TypeDef *KeyScan, uint16_t data,
                              FunctionalState NewState);

/**
 * \brief   KeyScan debounce time config.
 *
 * \param[in] KeyScan: selected KeyScan peripheral.
 * \param[in] time: Keyscan hardware debounce time.
 * \param[in] NewState: New state of the KeyScan debounce function.
 *            This parameter can be: ENABLE or DISABLE.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void keyscan_demo(void)
 * {
 *     KeyScan_debounceConfig(KEYSCAN, 10, ENABLE);
 *
 * }
 * \endcode
 */
void KeyScan_debounceConfig(KEYSCAN_TypeDef *KeyScan, uint8_t time,
                            FunctionalState NewState);

/**
 * \brief   Get KeyScan FIFO data num.
 *
 * \param[in]  KeyScan: Selected KeyScan peripheral.
 *
 * \return  Data length in FIFO.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void keyscan_demo(void)
 * {
 *     uint16_t data_len = KeyScan_GetFifoDataNum(KEYSCAN);
 * }
 * \endcode
 */
uint16_t KeyScan_GetFifoDataNum(KEYSCAN_TypeDef *KeyScan);

/**
 * \brief  Clear the KeyScan interrupt pending bit.
 *
 * \param[in] KeyScan: selected KeyScan peripheral.
 * \param[in] KeyScan_IT: specifies the interrupt pending bit to clear.
 *            This parameter can be any combination of the following values:
 *            \arg KEYSCAN_INT_THRESHOLD: Kescan FIFO data over threshold interrupt.
 *            \arg KEYSCAN_INT_OVER_READ: KeyScan over read interrupt.
 *            \arg KEYSCAN_INT_SCAN_END: KeyScan scan end interrupt.
 *            \arg KEYSCAN_INT_FIFO_NOT_EMPTY: KeyScan FIFO not empty interrupt.
 *            \arg KEYSCAN_INT_ALL_RELEASE: KeyScan all key release interrupt.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void keyscan_demo(void)
 * {
 *     KeyScan_ClearINTPendingBit(KEYSCAN, KEYSCAN_INT_SCAN_END);
 * }
 * \endcode
 */
void KeyScan_ClearINTPendingBit(KEYSCAN_TypeDef *KeyScan, uint32_t KeyScan_IT);

/**
 * \brief   Clear the specified KeyScan flag.
 *
 * \param[in] KeyScan: Selected KeyScan peripheral.
 * \param[in] KeyScan_FLAG: Specifies the flag to clear.
 *            This parameter can be one of the following values:
 *            \arg KEYSCAN_FLAG_FIFOLIMIT
 *            \arg KEYSCAN_FLAG_DATAFILTER
 *            \arg KEYSCAN_FLAG_OVR
 *
 * \return  None.
 * \note    KEYSCAN_FLAG_FULL and KEYSCAN_FLAG_EMPTY can't be cleared manually.
 *          They are cleared by hardware automatically.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void keyscan_demo(void)
 * {
 *     KeyScan_ClearFlags(KEYSCAN, KEYSCAN_FLAG_FIFOLIMIT);
 * }
 * \endcode
 */
void KeyScan_ClearFlags(KEYSCAN_TypeDef *KeyScan, uint32_t KeyScan_FLAG);

/**
 * \brief   Check whether the specified KeyScan flag is set.
 *
 * \param[in] KeyScan: Selected KeyScan peripheral.
 * \param[in] KeyScan_FLAG: Specifies the flag to check.
 *            This parameter can be one of the following values:
 *            \arg KEYSCAN_FLAG_FIFOLIMIT:
 *            \arg KEYSCAN_FLAG_THRESHOLD:
 *            \arg KEYSCAN_FLAG_OVER_READ:
 *            \arg KEYSCAN_FLAG_SCAN_END:
 *            \arg KEYSCAN_FLAG_FIFO_NOT_EMPTY:
 *            \arg KEYSCAN_FLAG_ALL_RELEASE:
 *            \arg KEYSCAN_FLAG_DATAFILTER:
 *            \arg KEYSCAN_FLAG_OVR:
 *            \arg KEYSCAN_FLAG_FULL:
 *            \arg KEYSCAN_FLAG_EMPTY:
 *
 * \return  The new state of KeyScan_FLAG (SET or RESET).
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void keyscan_demo(void)
 * {
 *     FlagStatus flag_status = KeyScan_GetFlagState(KEYSCAN, KEYSCAN_FLAG_OVR);
 *
 * }
 * \endcode
 */
FlagStatus KeyScan_GetFlagState(KEYSCAN_TypeDef *KeyScan, uint32_t KeyScan_FLAG);

/**
 * \brief  Read FIFO data.
 *
 * \param[in] KeyScan: Selected KeyScan peripheral.
 *
 * \return Keyscan FIFO data.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void keyscan_demo(void)
 * {
 *     uint16_t data = KeyScan_ReadFifoData(KEYSCAN);
 * }
 * \endcode
 */
uint16_t KeyScan_ReadFifoData(KEYSCAN_TypeDef *KeyScan);

/** End of KEYSCAN_Exported_Functions
  * \}
  */

/** End of KEYSCAN
  * \}
  */

#ifdef __cplusplus
}
#endif

#endif /* RTL876X_KEYSCAN_H */


/******************* (C) COPYRIGHT 2024 Realtek Semiconductor *****END OF FILE****/
