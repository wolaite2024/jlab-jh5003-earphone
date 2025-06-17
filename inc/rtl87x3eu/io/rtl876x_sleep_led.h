/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_sleep_led.h
* @brief
* @details
* @author
* @date
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _RTL876X_SLEEP_LED_H_
#define _RTL876X_SLEEP_LED_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtl876x.h"

#define LED_TIME_TICK_SCALE     320 //Unit: 1/32k sec, Range: 1~1023, Fix in 320, i.e. 10ms. This reg also effect PWM freq: 640->pwm freq become 50Hz
#define LED_BREATH_PHASE_NUM    8


/** @addtogroup 87x3d_LED LED
  * @brief LED driver module
  * @{
  */

/** @defgroup LED_Exported_Types LED Exported Types
  * @{
  */

/**
 * @brief SleepLed initialize parameters
 *
 * SleepLed initialize parameters
 */
typedef struct
{
    uint8_t mode;               /*!< Specifies the LED working mode.
                                                    This parameter can be a value of @ref LED_mode */
    uint8_t polarity;           /*!< Specifies the LED polarity.
                                                    This parameter can be a value of @ref LED_Output_polarity */
    uint16_t prescale;          /*!< Specifies the LED prescale value.
                                                     This parameter can be a value of 0-1023 */
    uint8_t period_high[3];     /*!< Specifies the LED blink mode period high.
                                                     This parameter can be a value of 0-1023 */
    uint8_t period_low[3];      /*!< Specifies the  LED blink mode period low.
                                                       This parameter can be a value of 0-1023 */

    uint8_t phase_uptate_rate[8];  /*!< Specifies the update rate. duty_step interval = (uptate_rate + 1) * phase_tick.
                                                     This parameter can be a value of Range: 0~15. */
    // Range: 0~15. duty_step interval = (uptate_rate + 1) * phase_tick
    uint16_t phase_phase_tick[8];    /*!< Specifies Unit: 10ms, Range: 1~1023, 0: Bypass this phase */

    uint16_t phase_initial_duty[8]; /*!< Specifies LED_TIME_TICK_SCALE = 100%, i.e. 3.2-->1% 320-->100% */

    uint8_t phase_increase_duty[8]; /*!< 1: Increase duty, 0: Decrease duty. It must be 0 if duty step = 0 or HW become abnormal */

    uint8_t phase_duty_step[8];   /*!< Range: 0~LED_TIME_TICK_SCALE, i.e. 3.2-->1% 320-->100% */

} SLEEP_LED_InitTypeDef;

/** End of group LED_Exported_Types
  * @}
  */

typedef struct
{
    __IO uint32_t S_LED_CR;                 //!<0x00
    __IO uint32_t S_LED_CH0_CR0;            //!<0x04
    __IO uint32_t S_LED_CH0_CR1;            //!<0x08
    __IO uint32_t S_LED_CH0_P1_CR;          //!<0x0C
    __IO uint32_t S_LED_CH0_P2_CR;          //!<0x10
    __IO uint32_t S_LED_CH0_P3_CR;          //!<0x14
    __IO uint32_t S_LED_CH0_P4_CR;          //!<0x18
    __IO uint32_t S_LED_CH0_P5_CR;          //!<0x1C
    __IO uint32_t S_LED_CH0_P6_CR;          //!<0x20
    __IO uint32_t S_LED_CH0_P7_CR;          //!<0x24
    __IO uint32_t S_LED_CH0_P8_CR;          //!<0x28

    __IO uint32_t RSV0;                     //!<0x2C

    __IO uint32_t S_LED_CH1_CR0;            //!<0x30
    __IO uint32_t S_LED_CH1_CR1;            //!<0x34
    __IO uint32_t S_LED_CH1_P1_CR;          //!<0x38
    __IO uint32_t S_LED_CH1_P2_CR;          //!<0x3C
    __IO uint32_t S_LED_CH1_P3_CR;          //!<0x40
    __IO uint32_t S_LED_CH1_P4_CR;          //!<0x44
    __IO uint32_t S_LED_CH1_P5_CR;          //!<0x48
    __IO uint32_t S_LED_CH1_P6_CR;          //!<0x4C
    __IO uint32_t S_LED_CH1_P7_CR;          //!<0x50
    __IO uint32_t S_LED_CH1_P8_CR;          //!<0x54

    __IO uint32_t RSV1;                     //!<0x58
    __IO uint32_t RSV2;                     //!<0x5C

    __IO uint32_t S_LED_CH2_CR0;            //!<0x60
    __IO uint32_t S_LED_CH2_CR1;            //!<0x64
    __IO uint32_t S_LED_CH2_P1_CR;          //!<0x68
    __IO uint32_t S_LED_CH2_P2_CR;          //!<0x6C
    __IO uint32_t S_LED_CH2_P3_CR;          //!<0x70
    __IO uint32_t S_LED_CH2_P4_CR;          //!<0x74
    __IO uint32_t S_LED_CH2_P5_CR;          //!<0x78
    __IO uint32_t S_LED_CH2_P6_CR;          //!<0x7C
    __IO uint32_t S_LED_CH2_P7_CR;          //!<0x80
    __IO uint32_t S_LED_CH2_P8_CR;          //!<0x84
} RTC_LED_TypeDef;

#define RTC_LED_REG_BASE                (SYSON_BASE + 0x600)
#define RTC_LED                         ((RTC_LED_TypeDef          *) RTC_LED_REG_BASE)

/*============================================================================*
 *                         Constants
 *============================================================================*/


/** @defgroup LED_Exported_Constants LED Exported Constants
  * @{
  */

/**
  * @brief LED channel enumeration
  */


typedef enum
{
    LED_CHANNEL_0 = 0x01,
    LED_CHANNEL_1 = 0x02,
    LED_CHANNEL_2 = 0x04
} SLEEP_LED_CHANNEL;


/** @defgroup LED_mode LED working mode
  * @{
  */

#define LED_BLINK_MODE              ((uint32_t)0x00000000)
#define LED_BREATHE_MODE            ((uint32_t)0x00000001)
#define IS_LED_MODE(MODE)           (((MODE) == LED_BLINK_MODE) || \
                                     ((MODE) == LED_BREATHE_MODE))

/** End of group LED_mode
  * @}
  */

/** @defgroup LED_Output_polarity LED output polarity
  * @{
  */

#define LED_OUTPUT_NORMAL           ((uint32_t)0x00000000)
#define LED_OUTPUT_INVERT           ((uint32_t)0x00000001)
#define IS_LED_OUTPUT_MODE(MODE)    (((MODE) == LED_OUTPUT_NORMAL) || \
                                     ((MODE) == LED_OUTPUT_INVERT))
/** End of group LED_Output_polarity
  * @}
  */

/** @defgroup LED_Idle_State  LED Idle State
  * @{
  */

#define LED_IDLE_HIGH               ((uint8_t)0x01)
#define LED_IDLE_LOW                ((uint8_t)0x00)
#define IS_LED_IDLE_MODE(MODE)    (((MODE) == LED_IDLE_HIGH) || \
                                   ((MODE) == LED_IDLE_LOW))
/** End of group LED_Output_polarity
  * @}
  */

/** End of group LED_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup LED_Exported_Functions LED Exported Functions
  * @{
  */

/**
 * rtl876x_sleep_led.h
 *
 * \brief   Clear all the Sleep LED registers to their default reset values.
 *
 * \param   None.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_led_init(void)
 * {
 *     SleepLed_Reset();
 * }
 * \endcode
 */
void SleepLed_Reset(void);


/**
 * rtl876x_sleep_led.h
 *
 * \brief   Clear Sleep LED registers of specified channel.
 *
 * \param[in] channel: Select the Sleep LED Channel.
 *            \arg LED_CHANNEL_x, where x can be 0 to 2.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void led_close(void)
 * {
 *     SleepLed_Cmd(LED_CHANNEL_0 | LED_CHANNEL_1 | LED_CHANNEL_2, DISABLE);
 *
 *     SleepLed_DeInit(LED_CHANNEL_0);
 *     SleepLed_DeInit(LED_CHANNEL_1);
 *     SleepLed_DeInit(LED_CHANNEL_2);
 * }
 * \endcode
 */
void SleepLed_DeInit(SLEEP_LED_CHANNEL channel);


/**
 * rtl876x_sleep_led.h
 *
 * \brief  Initialize the Sleep LED Channelx according to the specified
 *         parameters in the Sleep LED_InitStruct.
 *
 * \param[in] channel: Select the Sleep LED Channel.
 *            \arg LED_CHANNEL_x, where x can be 0 to 2.
 * \param[in] LED_InitStruct: Pointer to a SLEEP_LED_InitTypeDef structure that
 *            contains the configuration information for the specified Sleep LED Channel.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_led_init_blink(void)
 * {
 *     SleepLed_Reset();
 *     SLEEP_LED_InitTypeDef Led_Initsturcture;
 *     SleepLed_StructInit(&Led_Initsturcture);
 *     Led_Initsturcture.mode                        = LED_BLINK_MODE;
 *     Led_Initsturcture.polarity                    = LED_OUTPUT_NORMAL;
 *     Led_Initsturcture.prescale                    = 32;
 *     Led_Initsturcture.period_high[0]                = 200;
 *     Led_Initsturcture.period_low[0]                  = 100;
 *     SleepLed_Init(LED_CHANNEL_0, &Led_Initsturcture);
 *     SleepLed_Cmd(LED_CHANNEL_0, ENABLE);
 * }
 * \endcode
 */
void SleepLed_Init(SLEEP_LED_CHANNEL channel, SLEEP_LED_InitTypeDef *LED_InitStruct);


/**
 * rtl876x_sleep_led.h
 *
 * \brief   Fill each LED_InitStruct member with its default value.
 *
 * \param[in] LED_InitStruct: Pointer to a LED_InitTypeDef structure which will be initialized.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_led_init_breathe(void)
 * {
 *     SleepLed_Reset();
 *     SLEEP_LED_InitTypeDef Led_Initsturcture;
 *     SleepLed_StructInit(&Led_Initsturcture);
 *     Led_Initsturcture.mode                        = LED_BREATHE_MODE;
 *     Led_Initsturcture.polarity                    = LED_OUTPUT_NORMAL;
 *     Led_Initsturcture.prescale                    = 320;
 *     Led_Initsturcture.phase_uptate_rate[0]        = 0;
 *     Led_Initsturcture.phase_phase_tick[0]         = 50;
 *     Led_Initsturcture.phase_initial_duty[0]       = 0;
 *     Led_Initsturcture.phase_increase_duty[0]      = 1;
 *     Led_Initsturcture.phase_duty_step[0]          = 1;
 *     SleepLed_Init(LED_CHANNEL_0, &Led_Initsturcture);
 * }
 * \endcode
 */
void SleepLed_StructInit(SLEEP_LED_InitTypeDef *LED_InitStruct);


/**
 * rtl876x_sleep_led.h
 *
 * \brief   Enable or disable the specified LED Channelx.
 *
 * \param[in] channel: Select the Sleep LED Channel.
 *            \arg LED_CHANNEL_x, where x can be 0 to 2.
 * \param[in] NewState: New state of the specified LED Channelx.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable the specified LED Channelx.
 *            \arg DISABLE: Disable the specified LED Channelx.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_led_init(void)
 * {
 *     SleepLed_Cmd(LED_CHANNEL_0, ENABLE);
 * }
 * \endcode
 */
void SleepLed_Cmd(uint8_t channel, FunctionalState NewState);


/**
 * rtl876x_sleep_led.h
 *
 * \brief   Set idle mode of the specified LED Channelx.
 *
 * \param[in] channel: Select the Sleep LED Channel.
 *            \arg LED_CHANNEL_x, where x can be 0 to 2.
 * \param[in] State: idle output value of Channelx.
 *            \arg LED_IDLE_HIGH: Idle output value is high level.
 *            \arg LED_IDLE_LOW: Idle output value is low level.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void driver_led_init(void)
 * {
 *     SleepLed_SetIdleMode(LED_CHANNEL_0, LED_IDLE_LOW);
 * }
 * \endcode
 */
void SleepLed_SetIdleMode(SLEEP_LED_CHANNEL channel, uint8_t State);


/**
 * rtl876x_sleep_led.h
 *
 * \brief   Fast write Sleep LED register.
 *
 * \param[in] offset: The offset of LED register.
 * \param[in] data: Data to be writed.
 *
 * \return  None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SleepLed_Reset(void)
 * {
 *     SleepLed_RegisterWrite(0x00, 0x0);
 * }
 * \endcode
 */
void SleepLed_RegisterWrite(uint32_t offset, uint32_t data);


/**
 * rtl876x_sleep_led.h
 *
 * \brief   Fast Read LED register, actually you don't need use this function.
 *
 * \param[in] addr: the address of LED register.
 *
 * \return  The value of reading register.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void SleepLed_Cmd(void)
 * {
 *     uint32_t ctrl_value;
 *     uint32_t led_control_register;
 *     led_control_register = (uint32_t)(&(RTC_LED->S_LED_CR));
 *     ctrl_value = SleepLed_RegisterRead(led_control_register);
 * }
 * \endcode
 */
uint32_t SleepLed_RegisterRead(uint32_t addr);


/** @} */ /* End of group LED_Exported_Functions */
/** @} */ /* End of group 87x3d_LED */

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_SLEEP_LED_H_ */

/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/

