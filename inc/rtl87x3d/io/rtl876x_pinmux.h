/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      rtl876x_pinmux.h
* @brief
* @details
* @author    Chuanguo Xue
* @date      2024-07-18
* @version   v1.0
* *********************************************************************************************************
*/


#ifndef _RTL876X_PINMUX_H_
#define _RTL876X_PINMUX_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include "rtl876x.h"
#include "rtl876x_bitfields.h"


/** @addtogroup 87x3d_PINMUX PINMUX
  * @brief PINMUX driver module.
  * @{
  */

/*============================================================================*
 *                         Constants
 *============================================================================*/

/** @defgroup PINMUX_Exported_Constants PINMUX Exported Constants
  * @{
  */


/** @defgroup Pin_Function_Number Pin Function Number
  * @{
  */

#define IDLE_MODE                   0
#define HCI_UART_TX                 1
#define HCI_UART_RX                 2
#define HCI_UART_CTS                3
#define HCI_UART_RTS                4
#define I2C0_CLK                    5
#define I2C0_DAT                    6
#define I2C1_CLK                    7
#define I2C1_DAT                    8
#define PWM2_P                      9
#define PWM2_N                      10
#define PWM3_P                      11
#define PWM3_N                      12
#define timer_pwm0                  13
#define timer_pwm1                  14
#define timer_pwm2                  15
#define timer_pwm3                  16
#define timer_pwm4                  17
#define timer_pwm5                  18
#define timer_pwm6                  19
#define timer_pwm7                  20
#define QDEC_PHASE_A_X              21
#define QDEC_PHASE_B_X              22
#define QDEC_PHASE_A_Y              23
#define QDEC_PHASE_B_Y              24
#define QDEC_PHASE_A_Z              25
#define QDEC_PHASE_B_Z              26
#define UART1_TX                    27
#define UART1_RX                    28
#define UART2_TX                    29
#define UART2_RX                    30
#define UART2_CTS                   31
#define UART2_RTS                   32
#define IRDA_TX                     33
#define IRDA_RX                     34
#define UART0_TX                    35
#define UART0_RX                    36
#define UART0_CTS                   37
#define UART0_RTS                   38
#define SPI1_SS_N_0_MASTER          39
#define SPI1_SS_N_1_MASTER          40
#define SPI1_SS_N_2_MASTER          41
#define SPI1_CLK_MASTER             42
#define SPI1_MO_MASTER              43
#define SPI1_MI_MASTER              44
#define SPI0_SS_N_0_SLAVE           45
#define SPI0_CLK_SLAVE              46
#define SPI0_SO_SLAVE               47
#define SPI0_SI_SLAVE               48
#define SPI0_SS_N_0_MASTER          49
#define SPI0_CLK_MASTER             50
#define SPI0_MO_MASTER              51
#define SPI0_MI_MASTER              52
#define SPI2W_DATA                  53
#define SPI2W_CLK                   54
#define SPI2W_CS                    55
#define SWD_CLK                     56
#define SWD_DIO                     57
#define KEY_COL_0                   58
#define KEY_COL_1                   59
#define KEY_COL_2                   60
#define KEY_COL_3                   61
#define KEY_COL_4                   62
#define KEY_COL_5                   63
#define KEY_COL_6                   64
#define KEY_COL_7                   65
#define KEY_COL_8                   66
#define KEY_COL_9                   67
#define KEY_COL_10                  68
#define KEY_COL_11                  69
#define KEY_COL_12                  70
#define KEY_COL_13                  71
#define KEY_COL_14                  72
#define KEY_COL_15                  73
#define KEY_COL_16                  74
#define KEY_COL_17                  75
#define KEY_COL_18                  76
#define KEY_COL_19                  77
#define KEY_ROW_0                   78
#define KEY_ROW_1                   79
#define KEY_ROW_2                   80
#define KEY_ROW_3                   81
#define KEY_ROW_4                   82
#define KEY_ROW_5                   83
#define KEY_ROW_6                   84
#define KEY_ROW_7                   85
#define KEY_ROW_8                   86
#define KEY_ROW_9                   87
#define KEY_ROW_10                  88
#define KEY_ROW_11                  89
#define DWGPIO                      90
#define LRC_SPORT1                  91
#define BCLK_SPORT1                 92
#define ADCDAT_SPORT1               93
#define DACDAT_SPORT1               94
#define SPDIF_TX                    95
#define DMIC1_CLK                   96
#define DMIC1_DAT                   97
#define LRC_I_CODEC_SLAVE           98
#define BCLK_I_CODEC_SLAVE          99
#define SDI_CODEC_SLAVE             100
#define SDO_CODEC_SLAVE             101
#define LRC_I_PCM                   102
#define BCLK_I_PCM                  103
#define SDI_PCM                     104
#define SDO_PCM                     105
#define BT_COEX_I_0                 106
#define BT_COEX_I_1                 107
#define BT_COEX_I_2                 108
#define BT_COEX_I_3                 109
#define BT_COEX_O_0                 110
#define BT_COEX_O_1                 111
#define BT_COEX_O_2                 112
#define BT_COEX_O_3                 113
#define PTA_I2C_CLK_SLAVE           114
#define PTA_I2C_DAT_SLAVE           115
#define PTA_I2C_INT_OUT             116
#define DSP_GPIO_OUT                117
#define DSP_JTCK                    118
#define DSP_JTDI                    119
#define DSP_JTDO                    120
#define DSP_JTMS                    121
#define DSP_JTRST                   122
#define LRC_SPORT0                  123
#define BCLK_SPORT0                 124
#define ADCDAT_SPORT0               125
#define DACDAT_SPORT0               126
#define MCLK_M                      127
#define SPI0_SS_N_1_MASTER          128
#define SPI0_SS_N_2_MASTER          129
#define SPI2_SS_N_0_MASTER          130
#define SPI2_CLK_MASTER             131
#define SPI2_MO_MASTER              132
#define SPI2_MI_MASTER              133
#define I2C2_CLK                    134
#define I2C2_DAT                    135
#define UART3_TX                    136
#define UART3_RX                    137
#define UART3_CTS                   138
#define UART3_RTS                   139
#define PWM8                        140
#define PWM9                        141
#define PWM10                       142
#define PWM11                       143
#define DMIC2_CLK                   144
#define DMIC2_DAT                   145
#define DMIC3_CLK                   146
#define DMIC3_DAT                   147
#define LRC_SPORT2                  148
#define BCLK_SPORT2                 149
#define ADCDAT_SPORT2               150
#define DACDAT_SPORT2               151
#define LRC_SPORT3                  152
#define BCLK_SPORT3                 153
#define ADCDAT_SPORT3               154
#define DACDAT_SPORT3               155
#define MCLK_S                      156
#define SPIC1_SCK                   157
#define SPIC1_CSN                   158
#define SPIC1_SIO_0                 159
#define SPIC1_SIO_1                 160
#define SPIC1_SIO_2                 161
#define SPIC1_SIO_3                 162
#define EN_EXPA                     163
#define EN_EXLNA                    164
#define SEL_TPM_SW                  165
#define SEL_TPM_N_SW                166
#define UART1_CTS                   167
#define UART1_RTS                   168
#define SPIC0_SCK                   169
#define SPIC0_CSN                   170
#define SPIC0_SIO_0                 171
#define SPIC0_SIO_1                 172
#define SPIC0_SIO_2                 173
#define SPIC0_SIO_3                 174
#define LRC_RX_CODEC_SLAVE          175
#define LRC_RX_SPORT0               176
#define LRC_RX_SPORT1               177
#define LRC_RX_SPORT2               178
#define LRC_RX_SPORT3               179
#define CLK_CPU_DIVIDED_BY_2        180
#define CLK_DSP_DIVIDED_BY_2        181
#define SPDIF_RX                    182
#define DSP2_JTCK                   183
#define DSP2_JTDI                   184
#define DSP2_JTDO                   185
#define DSP2_JTMS                   186
#define DSP2_JTRST                  187
#define ANCDSP_JTCK                 188
#define ANCDSP_JTDI                 189
#define ANCDSP_JTDO                 190
#define ANCDSP_JTMS                 191
#define ANCDSP_JTRST                192
#define PDM_DATA_PDM_AMP            193
#define PDM_CLK_PDM_AMP             194
#define PWM12                       195
#define PWM13                       196
#define PWM14                       197
#define PWM15                       198
#define DSP2_GPIO_OUT               199
#define ANCDSP_GPIO_OUT             200
#define SPIC2_SCK                   201
#define SPIC2_CSN                   202
#define SPIC2_SIO_0                 203
#define SPIC2_SIO_1                 204
#define SPIC2_SIO_2                 205
#define SPIC2_SIO_3                 206
#define SPIC3_SCK                   207
#define SPIC3_CSN                   208
#define SPIC3_SIO_0                 209
#define SPIC3_SIO_1                 210
#define SPIC3_SIO_2                 211
#define SPIC3_SIO_3                 212
#define LPC_RSTb_PSRAM              213
#define LPC_D_0_PSRAM               214
#define LPC_D_1_PSRAM               215
#define LPC_D_2_PSRAM               216
#define LPC_D_3_PSRAM               217
#define LPC_CS_PSRAM                218
#define LPC_CK_CMP_PSRAM            219
#define LPC_CK_PSRAM                220
#define LPC_D_4_PSRAM               221
#define LPC_D_5_PSRAM               222
#define LPC_D_6_PSRAM               223
#define LPC_D_7_PSRAM               224
#define LPC_RWDS_PSRAM              225
#define CLK_DSP1_DIV                226
#define CLK_DSP2_DIV                227
#define CLK_DSPP_DIV                228
#define ANT_SW0_AoA_AoD             229
#define ANT_SW1_AoA_AoD             230
#define ANT_SW2_AoA_AoD             231
#define ANT_SW3_AoA_AoD             232
#define ANT_SW4_AoA_AoD             233
#define ANT_SW5_AoA_AoD             234

#define PHY_GPIO_1                  235
#define PHY_GPIO_2                  236
#define SLOW_DEBUG_MUX_1            237
#define SLOW_DEBUG_MUX_2            238
#define SWO                         239
#define DIGI_DEBUG                  240

// for BBPRO compatible
#define LOG0_UART_TX     UART1_TX
#define LOG0_UART_RX     UART1_RX
#define LOG0_UART_CTS    UART1_CTS
#define LOG0_UART_RTS    UART1_RTS

#define LOG1_UART_TX     UART3_TX
#define LOG1_UART_RX     UART3_RX
#define LOG1_UART_CTS    UART3_CTS
#define LOG1_UART_RTS    UART3_RTS

#define DATA_UART_TX     UART0_TX
#define DATA_UART_RX     UART0_RX
#define DATA_UART_CTS    UART0_CTS
#define DATA_UART_RTS    UART0_RTS

/** End of group Pin_Function_Number
  * @}
  */

#define PINMUX_BASE0_OFFSET         0x480
#define PINMUX_BASE1_OFFSET         0x4F8

/** @cond private
  * @defgroup Pad_Table_Index
  * @{
  */

#define  PIN_REG_NUM               36
/* Pad Functions */


//#define Pin_Debounce           (BIT2)

#define OUTENREG     Output_En
#define OUTVAL       Output_Val
#define PINMODE      0x11
#define PULLEN       Pull_En
#define PULLDIR      Pull_Direction
#define POWER        SHDN
#define PULLVAL      PULL_VALUE


#define WKEN        WakeUp_En
//#define WKPOL       1
//#define WKINTEN     2
//#define WKSTS       3

/*the bit of AON register mean when it used ad pin mode set*/
#define SHDN                         (BIT0)


#define Output_En                  (BIT1)
#define WKPOL                      (BIT2)
#define WakeUp_En                (BIT3)
#define WakeUp_INTEn           (BIT3)
#define Output_Val                  (BIT4)
#define PULL_VALUE               (BIT5)
#define Pull_Direction              (BIT6)
#define Pull_En                       (BIT7)

/*  Postion of each pad funcion */
#define SHDN_Pos (0UL) /*!< Position of SHDN. */
#define Output_En_Pos (1UL) /*!< Position of Output. */
#define Output_Pd_Pos (2UL) /*!< Position of pull direction. */
#define Output_PuEn_Pos (3UL) /*!< Position of pull enable. */
#define Output_Val_Pos (4UL) /*!< Position of out value. */
#define Pin_pull_type_Pos (5UL) /*!< Position of pull resistor configure. **/
#define Pull_Direction_Pos (6UL) /*!< Position of pull direction. */
#define Pull_En_Pos (7UL) /*!< Position of pull enable. */

/* Pad Wake up enable */

//define PULL_VALUE                 (BIT4)
//#define WakeUp_En                  (BIT5)

/**
  * @}
  * @endcond
  */

extern uint8_t hci_uart_rx_pin;
extern uint8_t hci_uart_tx_pin;

/** End of group PINMUX_Exported_Constants
  * @}
  */

/*============================================================================*
 *                         Types
 *============================================================================*/

/** @defgroup PINMUX_Exported_Types PINMUX Exported Types
  * @{
  */

/** @defgroup PAD_Pull_Mode PAD Pull Mode
  * @{
  */

typedef enum _PAD_Pull_Mode
{
    PAD_PULL_NONE,
    PAD_PULL_UP,
    PAD_PULL_DOWN
} PAD_Pull_Mode;

/** End of group PAD_Pull_Mode
  * @}
  */

/** @defgroup PAD_Pull_VALUE PAD Pull Value
  * @{
  */
typedef enum _PAD_Pull_Value
{
    PAD_PULL_HIGH,
    PAD_PULL_LOW,

} PAD_Pull_VALUE;

/** End of group PAD_Pull_VALUE
  * @}
  */

/** @defgroup PAD_Pull_EN PAD Pull Enable
  * @{
  */

typedef enum _PAD_Pull_EN
{
    PAD_PULL_DISABLE,
    PAD_PULL_ENABLE
} PAD_Pull_EN;

/** End of group PAD_Pull_EN
  * @}
  */

/** @defgroup PAD_Mode PAD Mode
  * @{
  */

typedef enum _PAD_Mode
{
    PAD_SW_MODE,
    PAD_PINMUX_MODE
} PAD_Mode;

/** End of group PAD_Mode
  * @}
  */

/** @defgroup PAD_Power_Mode PAD Power Mode
  * @{
  */

typedef enum _PAD_PWR_Mode
{
    PAD_SHUTDOWN,
    PAD_IS_PWRON = 1
} PAD_PWR_Mode;

/** End of group PAD_Power_Mode
  * @}
  */

/** @defgroup PAD_Output_Config PAD Output Config
  * @{
  */

typedef enum _PAD_OUTPUT_ENABLE_Mode
{
    PAD_OUT_DISABLE,
    PAD_OUT_ENABLE
} PAD_OUTPUT_ENABLE_Mode;

/** End of group PAD_Output_Config
  * @}
  */

/** @defgroup PAD_Output_Value PAD Output Value
  * @{
  */

typedef enum _PAD_OUTPUT_VAL
{
    PAD_OUT_LOW,
    PAD_OUT_HIGH
} PAD_OUTPUT_VAL;

/** End of group PAD_Output_Value
  * @}
  */
/** @defgroup PAD_WakeUp_EN PAD Wake Up Enable
  * @{
  */

typedef enum _PAD_WAKEUP_EN
{
    PAD_WAKEUP_DISABLE,
    PAD_WAKEUP_ENABLE
} PAD_WAKEUP_EN;

/** End of group PAD_WakeUp_EN
  * @}
  */

/** @defgroup PAD_SLEEP_LED_Pin PAD SLEEP LED Pin
  * @{
  */
typedef enum _SLEEP_LED_PIN
{
    SLEEP_LED_ADC_0,
    SLEEP_LED_ADC_1,
    SLEEP_LED_ADC_6,
    SLEEP_LED_ADC_7,
    SLEEP_LED_P1_0,
    SLEEP_LED_P1_1,
    SLEEP_LED_P1_4,
    SLEEP_LED_P2_0,
    SLEEP_LED_P2_1,
    SLEEP_LED_P2_2,
} SLEEP_LED_PIN;

/** End of group PAD_SLEEP_LED_Pin
  * @}
  */

/** @defgroup PAD_Function_Config PAD Function Config
  * @{
  */

typedef enum _PAD_FUNCTION_CONFIG_VALUE
{
    AON_GPIO,
    LED0,
    LED1,
    LED2,
    CLK_REQ,
    EXTRN_SWR_POW_SWR,
    EXTRN_LDO1_POW_LDO,
    EXTRN_LDO2_POW_LDO,
    PAD_FUNC_MAX,
} PAD_FUNCTION_CONFIG_VAL;

/** End of group PAD_Function_Config
  * @}
  */

/** @defgroup PAD_WakeUp_Polarity_Value PAD Wake Up Polarity Value
  * @{
  */

typedef enum _PAD_WAKEUP_POL_VAL
{
    PAD_WAKEUP_POL_HIGH,
    PAD_WAKEUP_POL_LOW,
    PAD_WAKEUP_NONE
} PAD_WAKEUP_POL_VAL;

/** End of group PAD_WakeUp_Polarity_Value
  * @}
  */
/** @defgroup PAD_WakeUp_Debounce_En PAD Wake Up Debounce Enable
  * @{
  */
typedef enum _PAD_WAKEUP_DEBOUNCE_EN
{
    PAD_WK_DEBOUNCE_DISABLE,
    PAD_WK_DEBOUNCE_ENABLE
} PAD_WAKEUP_DEBOUNCE_EN;

/** End of group PAD_WakeUp_Debounce_En
  * @}
  */

/** @defgroup PAD_Pull_Value PAD Pull Value
  * @{
  */

typedef enum _PAD_PULL_CONFIG_VAL
{
    PAD_WEAKLY_PULL,
    PAD_STRONG_PULL
} PAD_PULL_VAL;

/** End of group PAD_Pull_Value
  * @}
  */

/** @defgroup PAD_LDO_Type PAD LDO Type
  * @{
  */

typedef enum _PAD_LDO_TYPE
{
    PAD_LDOAUX1,
    PAD_LDOAUX2
} PAD_LDO_TYPE;

/** End of group PAD_LDO_Type
  * @}
  */

/** @defgroup PAD_AON_STATUS PAD AON Status
  * @{
  */
typedef enum _PAD_AON_Status
{
    PAD_AON_OUTPUT_LOW,
    PAD_AON_OUTPUT_HIGH,
    PAD_AON_OUTPUT_DISABLE,
    PAD_AON_PINMUX_ON,
    PAD_AON_PIN_ERR
} PAD_AON_Status;
/** End of group PAD_AON_STATUS
  * @}
  */

/** @defgroup WAKEUP_POLARITY Wake Up Polartity
  * @{
  */
typedef enum _WAKEUP_POL
{
    POL_HIGH,
    POL_LOW,
} WAKEUP_POL;
/** End of group WAKEUP_POLARITY
  * @}
  */

/** @defgroup WAKEUP_ENABLE_MODE PAD Wake Up Enable Mode
  * @{
  */
typedef enum _WAKEUP_EN_MODE
{
    ADP_MODE,
    BAT_MODE,
    MFB_MODE
} WAKEUP_EN_MODE;
/** End of group WAKEUP_ENABLE_MODE
  * @}
  */

/** @defgroup PAD_DRIVING_CURRENT PAD Driving Current Value
  * @{
  */
typedef enum _DRIVER_LEVEL
{
    LEVEL0,
    LEVEL2,
    LEVEL1,
    LEVEL3,
} T_DRIVER_LEVEL_MODE;
/** End of group PAD_DRIVING_CURRENT
  * @}
  */

/** @defgroup PAD_POWER_GROUP Pad Power Supply Volt
  * @{
  */
typedef enum _PIN_POWER_GROUP
{
    INVALID_PIN_GROUP  = 0,
    VDDIO1             = 1,
    VDDIO2             = 2,
    VDDIO3             = 3,
    VDDIO4             = 4,
    VDDIO5             = 5,
    VCODEC             = 6,
    GROUP_ADC          = 7,
} T_PIN_POWER_GROUP;

/** End of group PAD_POWER_GROUP
  * @}
  */

/** @defgroup ANA_MODE PAD Analog/Digital Mode
  * @brief Pad analog/digital mode for CODEC hybrid IO.
  * @{
  */
typedef enum _ANA_MODE
{
    PAD_ANALOG_MODE,
    PAD_DIGITAL_MODE,
} ANA_MODE;

/** End of group ANA_MODE
  * @}
  */

/** @defgroup WAKE_UP_MODE PAD Wake Up Mode
 * @{
 */

typedef enum
{
    WAKE_UP_POWER_OFF,     /*!< Config power off wake up. */
    WAKE_UP_GENERAL,       /*!< Config DLPS or power down wake up. */
} T_WAKE_UP_MODE;

/** End of group WAKE_UP_MODE
  * @}
  */

/** End of group PINMUX_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/


/** @defgroup PINMUX_Exported_Functions PINMUX Exported Functions
  * @{
  */

/**
 * rtl876x_pinmux.h
 *
 * \brief   Reset all pins to idle mode.
 *
 * \param   None.
 *
 * \note   Two SWD pins will also be reset. Please use this function carefully.
 *
 * \return   None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void board_xxx_init(void)//XXX represents the name of the peripheral to be configured.
 * {
 *     Pinmux_Reset();
 * }
 * \endcode
 */
extern void Pinmux_Reset(void);

/**
 * rtl876x_pinmux.h
 *
 * \brief     Configure the specified pin to idle mode.
 *
 * \param[in] Pin_Num: Pin number to be configured.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void board_xxx_init(void)
 * {
 *     Pinmux_Deinit(P2_2);
 * }
 * \endcode
 */
extern void Pinmux_Deinit(uint8_t Pin_Num);

/**
 * rtl876x_pinmux.h
 *
 * \brief     Config the selected pin to its corresponding IO function.
 *
 * \param[in] Pin_Num: Pin number to be configured.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] Pin_Func: mean one IO function, can be a value of \ref Pin_Function_Number.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void driver_uart_init(void)
 * {
 *     Pad_Config(P2_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
 *     Pad_Config(P2_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);

 *     Pinmux_Config(P2_0, UART0_TX);
 *     Pinmux_Config(P2_1, UART0_RX);
 * }
 * \endcode
 */
extern void Pinmux_Config(uint8_t Pin_Num, uint8_t Pin_Func);

/**
 * rtl876x_pinmux.h
 *
 * \brief     Configure the relevant operation mode,
 *            peripheral circuit and output level value in software mode of the specified pin.
 *
 * \param[in] Pin_Num: Pin number to be configured.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] AON_PAD_Mode: Use software mode or PINMUX mode.
 *            This parameter can be one of the following values:
 *            \arg PAD_SW_MODE: Use software mode.
 *            \arg PAD_PINMUX_MODE: Use PINMUX mode.
 * \param[in] AON_PAD_PwrOn: Config power of pad.
 *            This parameter can be one of the following values:
 *            \arg PAD_SHUTDOWN: Shutdown power of pad.
 *            \arg PAD_IS_PWRON: Enable power of pad.
 * \param[in] AON_PAD_Pull: Config pad pull mode.
 *            This parameter can be one of the following values:
 *            \arg PAD_PULL_NONE: No pull.
 *            \arg PAD_PULL_UP: Pull this pin up.
 *            \arg PAD_PULL_DOWN: Pull this pin down.
 * \param[in] AON_PAD_E: Config pad output function, which only valid when PAD_SW_MODE.
 *            This parameter can be one of the following values:
 *            \arg PAD_OUT_DISABLE: Disable pin output.
 *            \arg PAD_OUT_ENABLE: Enable pad output.
 * \param[in] AON_PAD_O: Config pin output level, which only valid when PAD_SW_MODE and output mode.
 *            This parameter can be one of the following values:
 *            \arg PAD_OUT_LOW: Pad output low.
 *            \arg PAD_OUT_HIGH: Pad output high.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void driver_adc_init(void)
 * {
 *     Pad_Config(P2_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
 *     Pad_Config(P2_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
 * }
 * \endcode
 */
extern void Pad_Config(uint8_t Pin_Num,
                       PAD_Mode AON_PAD_Mode,
                       PAD_PWR_Mode AON_PAD_PwrOn,
                       PAD_Pull_Mode AON_PAD_Pull,
                       PAD_OUTPUT_ENABLE_Mode AON_PAD_E,
                       PAD_OUTPUT_VAL AON_PAD_O);

/**
 * rtl876x_pinmux.h
 *
 * \brief     Configure the relevant operation mode, peripheral circuit, pull resistor value and
 *            output level value in software mode of the specified pin.
 *
 * \param[in] Pin_Num: Pin number to be configured.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] AON_PAD_Mode: Use software mode or PINMUX mode.
 *            This parameter can be one of the following values:
 *            \arg PAD_SW_MODE: Use software mode.
 *            \arg PAD_PINMUX_MODE: Use PINMUX mode.
 * \param[in] AON_PAD_PwrOn: Config power of pad.
 *            This parameter can be one of the following values:
 *            \arg PAD_SHUTDOWN: Shutdown power of pad.
 *            \arg PAD_IS_PWRON: Enable power of pad.
 * \param[in] AON_PAD_Pull: Config pad pull mode.
 *            This parameter can be one of the following values:
 *            \arg PAD_PULL_NONE: No pull.
 *            \arg PAD_PULL_UP: Pull this pin up.
 *            \arg PAD_PULL_DOWN: Pull this pin down.
 * \param[in] AON_PAD_E: Config pad output function, which only valid when PAD_SW_MODE.
 *            This parameter can be one of the following values:
 *            \arg PAD_OUT_DISABLE: Disable pin output.
 *            \arg PAD_OUT_ENABLE: Enable pad output.
 * \param[in] AON_PAD_O: Config pin output level, which only valid when PAD_SW_MODE and output mode.
 *            This parameter can be one of the following values:
 *            \arg PAD_OUT_LOW: Pad output low.
 *            \arg PAD_OUT_HIGH: Pad output high.
 * \param[in] AON_PAD_P: Config resistor value.
 *            This parameter can be one of the following values:
 *            \arg PAD_150K_PULL: pad pull 150k resistance.
 *            \arg PAD_15K_PULL: pad pull 15k resistance.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void driver_adc_init(void)
 * {
 *     Pad_ConfigExt(P2_0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW, PAD_150K_PULL);
 *     Pad_ConfigExt(P2_1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW, PAD_150K_PULL);
 * }
 * \endcode
 */
extern void Pad_ConfigExt(uint8_t Pin_Num,
                          PAD_Mode AON_PAD_Mode,
                          PAD_PWR_Mode AON_PAD_PwrOn,
                          PAD_Pull_Mode AON_PAD_Pull,
                          PAD_OUTPUT_ENABLE_Mode AON_PAD_E,
                          PAD_OUTPUT_VAL AON_PAD_O,
                          PAD_PULL_VAL AON_PAD_P);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Set all pins to the default state.
 *
 * \param[in] None.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void board_xxx_init(void)
 * {
 *     Pad_AllConfigDefault();
 * }
 * \endcode
 */
extern void Pad_AllConfigDefault(void);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Enable the function of the wake-up system of the specified pin.
 *
 * \param[in] Pin_Num: Pin number to be configured.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] Polarity: Polarity to wake up.
 *            This parameter can be the following:
 *            \arg PAD_WAKEUP_POL_HIGH: Use high level wakeup.
 *            \arg PAD_WAKEUP_POL_LOW: Use low level wakeup.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * //IO enter DLPS call back function.
 * void io_uart_dlps_enter(void)
 * {
 *     // Switch pad to software mode
 *     Pad_ControlSelectValue(P2_0, PAD_SW_MODE);//TX pin
 *     Pad_ControlSelectValue(P2_1, PAD_SW_MODE);//RX pin
 *
 *     System_WakeUpPinEnable(P2_1, PAD_WAKEUP_POL_LOW);
 * }
 * \endcode
 */
extern void System_WakeUpPinEnable(uint8_t Pin_Num, uint8_t Polarity);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Disable the function of the wake-up system of the specified pin.
 *
 * \param[in] Pin_Num: Pin number to be configured.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * #define UART_RX_PIN   P4_1
 *
 * //System interrupt handler function, for wakeup pin.
 * void System_Handler(void)
 * {
 *     if (System_WakeUpInterruptValue(UART_RX_PIN) == SET)
 *     {
 *         Pad_ClearWakeupINTPendingBit(UART_RX_PIN);
 *         System_WakeUpPinDisable(UART_RX_PIN);
 *         //Add user code here.
 *     }
 * }
 * \endcode
 */
extern void System_WakeUpPinDisable(uint8_t Pin_Num);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Enable the function of the wake-up system interrupt of the specified pin.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * //IO enter DLPS call back function.
 * void io_uart_dlps_enter(void)
 * {
 *     // Switch pad to software mode
 *     Pad_ControlSelectValue(P2_0, PAD_SW_MODE);//TX pin
 *     Pad_ControlSelectValue(P2_1, PAD_SW_MODE);//RX pin
 *
 *     System_WakeUpInterruptEnable(P2_1);
 * }
 * \endcode
 */
extern void System_WakeUpInterruptEnable(uint8_t Pin_Num);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Disable the function of the wake-up system interrupt of the specified pin.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * //System interrupt handler function.
 * void System_Handler(void)
 * {
 *     if (System_WakeUpInterruptValue(P2_5) == SET)
 *     {
 *         Pad_ClearWakeupINTPendingBit(P2_5);
 *         System_WakeUpInterruptDisable(P2_5);
 *         //Add user code here.
 *     }
 * }
 * \endcode
 */
extern void System_WakeUpInterruptDisable(uint8_t Pin_Num);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Check wake up pin interrupt status.
 *
 * \param[in] Pin_Num: Pin number to be configured.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 *
 * \return   Pin interrupt status.
 * \retval 1: Pin wake up system.
 * \retval 0: The pin does not wake up the system.
 *
 * <b>Example usage</b>
 * \code{.c}
 * #define UART_RX_PIN                P4_1
 *
 * //System interrupt handler function.
 * void System_Handler(void)
 * {
 *     if (System_WakeUpInterruptValue(UART_RX_PIN) == SET)
 *     {
 *         Pad_ClearWakeupINTPendingBit(UART_RX_PIN);
 *         System_WakeUpPinDisable(UART_RX_PIN);
 *         //Add user code here.
 *     }
 * }
 * \endcode
 */
extern uint8_t System_WakeUpInterruptValue(uint8_t Pin_Num);
extern uint8_t Pad_WakeupInterruptValue(uint8_t Pin_Num);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Config pad output function.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] value: This parameter sets whether the pin outputs the level in software mode.
 *            This parameter can be one of the following values:
 *            \arg PAD_OUT_DISABLE: Disable pin output.
 *            \arg PAD_OUT_ENABLE: Enable pin output.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void board_xxx_init(void)
 * {
 *     Pad_OutputEnableValue(P2_0, PAD_OUT_ENABLE);
 * }
 * \endcode
 */
extern void Pad_OutputEnableValue(uint8_t Pin_Num, uint8_t value);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Config pad pull enable or not.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] value: Enable or disable pad pull-up / pull-down resistance function.
 *            This parameter can be one of the following values:
 *            \arg DISABLE: Disable pad pull-up / pull-down function.
 *            \arg ENABLE: Enable  pad pull-up / pull-down function.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void board_xxx_init(void)
 * {
 *     Pad_PullEnableValue(P2_0, ENABLE);
 * }
 * \endcode
 */
extern  void Pad_PullEnableValue(uint8_t Pin_Num, uint8_t value);
extern  void Pad_PullEnableValue_Dir(uint8_t Pin_Num, uint8_t value, uint8_t Pull_Direction_value);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Config pad pull up or down.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] value : This parameter sets whether the pin pull-up or pull-down.
 *            This parameter can be one of the following values:
 *            \arg 0: Config pad pull-down function.
 *            \arg 1: Config pad pull-up function.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void board_xxx_init(void)
 * {
 *     Pad_PullUpOrDownValue(P2_0, 1);
 * }
 * \endcode
 */
extern void Pad_PullUpOrDownValue(uint8_t Pin_Num, uint8_t value);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Config the pad control selected value.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] value: use software mode or PINMUX mode.
 *            This parameter can be one of the following values:
 *            \arg PAD_SW_MODE: use software mode, aon control.
 *            \arg PAD_PINMUX_MODE: use PINMUX mode, core control.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * //IO enter DLPS call back function.
 * void io_uart_dlps_enter(void)
 * {
 *     // Switch pad to software mode
 *     Pad_ControlSelectValue(P2_0, PAD_SW_MODE);//TX pin
 *     Pad_ControlSelectValue(P2_1, PAD_SW_MODE);//RX pin
 *
 *     System_WakeUpPinEnable(P2_1, PAD_WAKEUP_POL_LOW);
 * }
 * \endcode
 */
extern void Pad_ControlSelectValue(uint8_t Pin_Num, uint8_t value);

/**
 * rtl876x_pinmux.h
 *
 * \brief     Configure the pad output level when pad set to SW mode.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] value: Config pin output level.
 *            This parameter can be one of the following values:
 *            \arg PAD_OUT_LOW: Pad output low.
 *            \arg PAD_OUT_HIGH: Pad output high.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void board_xxx_init(void)
 * {
 *     Pad_OutputControlValue(P2_0, PAD_OUT_HIGH);
 * }
 * \endcode
 */
extern void Pad_OutputControlValue(uint8_t Pin_Num, uint8_t value);

/**
 * rtl876x_pinmux.h
 *
 * \brief     Enable the function of the wake-up system of the specified pin.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] value:  Enable or disable wake-up system function.
 *            \arg ENABLE: Enable pad to wake up system from DLPS.
 *            \arg DISABLE: Disable pad to wake up system from DLPS.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void board_xxx_init(void)
 * {
 *     Pad_WakeupEnableValue(P2_0, ENABLE);
 * }
 * \endcode
 */
extern void Pad_WakeupEnableValue(uint8_t Pin_Num, uint8_t value);


/**
 * rtl876x_pinmux.h
 *
 * \brief     Config the pad wake up polarity.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] Polarity: Polarity of wake-up system.
 *            This parameter can be the following:
 *            \arg PAD_WAKEUP_POL_LOW: Use low level wakeup.
 *            \arg PAD_WAKEUP_POL_HIGH: Use high level wakeup.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void board_xxx_init(void)
 * {
 *     Pad_WakeupPolarityValue(P2_0, PAD_WAKEUP_POL_LOW);
 * }
 * \endcode
 */
extern void Pad_WakeupPolarityValue(uint8_t Pin_Num, uint8_t value);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Config pad wake up interrupt.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] value: Enable or disable pad wake up interrupt.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable pad wake up to trigger system interrupt.
 *            \arg DISABLE: Disable pad wake up to trigger system interrupt.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void pad_demo(void)
 * {
 *     Pad_WakeupInterruptEnable(P2_0, ENABLE);
 * }
 * \endcode
 */
extern void Pad_WakeupInterruptEnable(uint8_t Pin_Num, uint8_t value);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Check pad wake up pin interrupt status.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 *
 * \return   Pin interrupt status.
 * \retval 1: Pin wake up system.
 * \retval 0: The pin does not wake up the system.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void System_Handler(void)
 * {
 *     if (Pad_WakeupInterruptValue(P4_1) == SET)
 *     {
 *         Pad_ClearWakeupINTPendingBit(P4_1);
 *     }
 *     NVIC_DisableIRQ(System_IRQn);
 *     NVIC_ClearPendingIRQ(System_IRQn);
 * }
 * \endcode
 */
extern uint8_t Pad_WakeupInterruptValue(uint8_t Pin_Num);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Clear pad wake up pin interrupt pending bit.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void System_Handler(void)
 * {
 *     if (Pad_WakeupInterruptValue(P4_1) == SET)
 *     {
 *         Pad_ClearWakeupINTPendingBit(P4_1);
 *     }
 *     NVIC_DisableIRQ(System_IRQn);
 *     NVIC_ClearPendingIRQ(System_IRQn);
 * }
 * \endcode
 */
extern void Pad_ClearWakeupINTPendingBit(uint8_t Pin_Num);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Clear all wake up pin interrupt pending bit.
 *
 * \param[in] None.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void dlps_io_enter_cb(void)
 * {
 *     io_dlps_callback(&io_dlps_enter_q);
 *
 *     //clear aon fast 0x12E ~ 0x131, 0x133 (PAD wake up INT status), write one clear
 *     Pad_ClearAllWakeupINT();
 *
 *     if (power_mode_get() == POWER_DLPS_MODE)
 *     {
 *         dlps_io_store();
 *     }
 * }
 * \endcode
 */
extern void Pad_ClearAllWakeupINT(void);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Config pin power mode.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] value: This parameter sets the power supply mode of the pin,
 *                   and the value is enumeration PAD_PWR_Mode One of the values.
 *            This parameter can be one of the following values:
 *            \arg PAD_SHUTDOWN: Power off.
 *            \arg PAD_IS_PWRON: Power on.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void pad_demo(void)
 * {
 *     Pad_PowerOrShutDownValue(P2_0, PAD_IS_PWRON);
 * }
 * \endcode
 */
extern void Pad_PowerOrShutDownValue(uint8_t Pin_Num, uint8_t value);

/**
 * rtl876x_pinmux.h
 *
 * \brief     Configure the strength of pull-up/pull-down resistance.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg  P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] value: This parameter sets the strength of pull-up/pull-down resistance.
 *            This parameter can be one of the following values:
 *            \arg PAD_150K_PULL: pad pull 150k resistance.
 *            \arg PAD_15K_PULL: pad pull 15k resistance.
 *
 * \return    None.
 *
 * <b>Example usage</b>
 * \code{.c}
 *
 * void board_xxx_init(void)
 * {
 *     Pad_PullConfigValue(P2_0, PAD_150K_PULL);
 * }
 * \endcode
 */
extern void Pad_PullConfigValue(uint8_t Pin_Num, uint8_t value);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Config driving current value.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] e2_value: set driving current value.
 *            This parameter can be one of the following values:
 *            \arg 0: set driving current value to low level.
 *            \arg 1: set driving current value to high level.
 * \param[in] e3_value: set driving current value.
 *            This parameter can be one of the following values:
 *            \arg 0: set driving current value to low level.
 *            \arg 1: set driving current value to high level.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void pad_demo(void)
 * {
 *     Pad_DrivingCurrentControl(P2_0, 1, 1);
 * }
 * \endcode
 */
extern void Pad_DrivingCurrentControl(uint8_t Pin_Num, uint8_t e2_value, uint8_t e3_value);
extern void System_WakeUpDebounceTime(uint8_t time);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Config Pad Digital Function.
 *
 * \param[in] Pin_Num: pin number.
 *     \arg For function AON_GPIO,LED0,LED1,LED2,CLK_REQ. The pin could be one of the following table:
 *                 ADC_0, ADC_1, ADC_6, ADC_7, P1_0,  P1_1,  P1_4,  P2_0,
 *                 P2_1,  P2_2
 *     \arg For function EXTRN_SWR_POW_SWR, EXTRN_LDO1_POW_LDO, EXTRN_LDO2_POW_LDO.The pin could be one of the following table:
 *                 P2_2,   P2_1,   P1_7,   P1_6,   P1_1,   P1_0,  XO_32K, XI_32K,
 *                 P3_2,   P4_1,   P4_0,   P2_7,   P2_6,   P2_5,  P2_4,   P2_3,
 *                 P5_7,   P5_6,   P5_0,   P3_7,   P3_6,   P3_5,  P3_4,   P3_3,
 *                 AUX_R,  AUX_L,  MIC4_P, MIC4_N, P6_1,   P6_0,  P4_7,   P4_6,
 * \param[in] value: Config value \ref PAD_FUNCTION_CONFIG_VAL.
 *            This parameter can be one of the following values:
 *            \arg AON_GPIO: Default GPIO function.
 *            \arg LED0: SLEEP LED channel 0.
 *            \arg LED1: SLEEP LED channel 1.
 *            \arg LED2: SLEEP LED channel 2.
 *            \arg CLK_REQ: Clock request, internal debug function.
 *            \arg EXTRN_SWR_POW_SWR:  external power control for SWR.
 *            \arg EXTRN_LDO1_POW_LDO: external power control for LDO1.
 *            \arg EXTRN_LDO2_POW_LDO: external power control for LDO2.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void pad_demo(void)
 * {
 *     Pad_FunctionConfig(P2_0, AON_GPIO);
 * }
 * \endcode
 */
extern bool Pad_FunctionConfig(uint8_t Pin_Num, PAD_FUNCTION_CONFIG_VAL value);


/**
 * rtl876x_pinmux.h
 *
 * \brief   Get pad current output/input setting.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 *
 * \return The pad current output/input setting, which can refer to PAD_AON_Status.
 * \retval PAD_AON_OUTPUT_LOW: Pad AON output low level.
 * \retval PAD_AON_OUTPUT_HIGH: Pad AON output high level.
 * \retval PAD_AON_OUTPUT_DISABLE: Pad AON output disable.
 * \retval PAD_AON_PINMUX_ON: Pad AON PINMUX on.
 * \retval PAD_AON_PIN_ERR: Pad AON pin error.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void pad_demo(void)
 * {
 *     if (Pad_GetOutputCtrl(P2_1) == PAD_AON_OUTPUT_LOW)
 *     {
 *         //Add user code here.
 *     }
 * }
 * \endcode
 */
uint8_t Pad_GetOutputCtrl(uint8_t Pin_Num);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Config the system wakeup mode.
 *
 * \param[in] mode: mode of set, this parameter can reference WAKEUP_EN_MODE.
 *            This parameter can be one of the following values:
 *            \arg ADP_MODE: Wake up by adapter.
 *            \arg BAT_MODE: Wake up by battery.
 *            \arg MFB_MODE: Wake up by MFB.
 *            \arg USB_MODE: Wake up by USB.
 * \param[in] pol: polarity to wake up.
 *            This parameter can be the following:
 *            \arg POL_HIGH: Use high level wakeup.
 *            \arg POL_LOW: Use low level wakeup.
 * \param[in] NewState: Enable or disable wake up.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable the system wake up at specified polarity.
 *            \arg DISABLE: Disable the system wake up at specified polarity.
 *
 * \return     Config the system wakeup mode fail or success.
 * \retval 0   Config success.
 * \retval 1   Config fail due to wrong mode.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void adapter_wake_up_enable(void)
 * {
 *     //adapter mode is WAKE_UP_GENERAL
 *     Pad_WakeUpCmd(ADP_MODE, POL_HIGH, ENABLE);
 * }
 * \endcode
 */
uint8_t  Pad_WakeUpCmd(WAKEUP_EN_MODE mode, WAKEUP_POL pol, FunctionalState NewState);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Get the pin power group.
 *
 * \param[in] pin: pin of set only VDDIO pin.
 *
 * \return   The power group of the pin. Please refer to T_PIN_POWER_GROUP.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void pad_demo(void)
 * {
 *     Pad_GetPowerGroup(P2_1);
 * }
 * \endcode
 */
T_PIN_POWER_GROUP  Pad_GetPowerGroup(uint8_t pin);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Configure the driving current of the pin.
 *
 * \param[in] Pin_Num: pin number.
 *            This parameter can be one of the following values:
 *            \arg P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] driver_level: refer to the T_DRIVER_LEVEL_MODE.
 *            This parameter can be: LEVEL0, LEVEL1, LEVEL2, LEVEL3.
 *            when VDDIO is set to 1.8V, the pad driving current is as followed:
 *            \arg LEVEL0: 1.25 mA
 *            \arg LEVEL1: 2.50 mA
 *            \arg LEVEL2: 3.75 mA
 *            \arg LEVEL3: 5.00 mA
 *            When VDDIO is set to 3.3V, the pad driving current is as followed:
 *            \arg LEVEL0: 4 mA
 *            \arg LEVEL1: 8 mA
 *            \arg LEVEL2: 12 mA
 *            \arg LEVEL3: 16 mA
 *
 * \return   The driving current of the pin set succeeded or failed.
 * \retval true    Driving current is set successfully.
 * \retval false   Driving current set fail. The failure reasons could be one of the following one:
 *                 invalid pin number or driving current setting is not supported for the setting pin.
 * \note  All pin support MODE_4MA MODE_8MA.
 *        Only follow pin support MODE_12MA, MODE_16MA driver current:
    {
    P1_0,    P1_1,    P1_2,    P1_3,    P1_4,    P1_5,    P1_6,    P1_7,
    P3_0,    P3_1,    P3_2,    P3_3,    P3_4,    P3_5,
    P5_0,    P5_1,    P5_2,    P5_3,    P5_4,    P5_5,    P5_6,    P5_7,
    P6_0,    P6_1,    P6_2,    P6_3,    P6_4,    P6_5,    P6_6,
    P7_0,    P7_1,    P7_2,    P7_3,    P7_4,    P7_5,    P7_6,
    AUX_R,    AUX_L,     MIC1_P,     MIC1_N,    MIC2_P, MIC2_N,   MICBIAS,    LOUT_P,
    LOUT_N,  ROUT_P, ROUT_N,    MIC3_P,    MIC3_N
    }
 *
 * <b>Example usage</b>
 * \code{.c}
 * void pad_demo(void)
 * {
 *     Pad_SetPinDrivingCurrent(P2_1, LEVEL0);
 * }
 * \endcode
 */
bool Pad_SetPinDrivingCurrent(uint8_t pin, T_DRIVER_LEVEL_MODE driver_level);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Config Hybrid pad analog/digital functions.
 *
 * \param[in] pin: pin number.
 *            This parameter can be one of the following values:
 *            \arg AUX_R ~ MIC3_N, please refer to rtl876x.h 'Pin_Number' part.
 * \param[in] mode: Please refer to ANA_MODE.
 *            \arg This parameter can be: PAD_ANALOG_MODE/PAD_DIGITAL_MODE.
 *
 * \return   None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void pad_demo(void)
 * {
 *     Pad_AnalogMode(P2_1, PAD_ANALOG_MODE);
 * }
 * \endcode
 */
void Pad_AnalogMode(uint8_t pin, ANA_MODE mode);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Get the pin name in string.
 *
 * \param[in] pin_num: pin number.
 *            This parameter can be one of the following values:
 *            \arg P0_0 ~ P7_7, please refer to rtl876x.h 'Pin_Number' part.
 *
 * \return   The pin name or null.
 * \retval The pin name.
 * \retval null      Fail to return pin name due to invalid pin index.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void pad_demo(void)
 * {
 *     Pad_GetPinName(P2_1);
 * }
 * \endcode
 */
const char *Pad_GetPinName(uint8_t pin_num);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Configure the adpater wake-up system functions in power off(shipping) mode.
 *
 * \param[in] NewState: Enable or disable adpater wake up.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable adpater wake up system at specified polarity.
 *            \arg DISABLE: Disable adpater wake up system.
 * \param[in] pol: Polarity to wake up.
 *            This parameter can be the following:
 *            \arg POL_HIGH: Use high level wakeup.
 *            \arg POL_LOW: Use low level wakeup.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * void adapter_wake_up_enable(void)
 * {
 *     //adapter mode is wake_up_power_off
 *     System_SetAdpWakeUpFunction(ENABLE, POL_HIGH);
 * }
 * \endcode
 */
void System_SetAdpWakeUpFunction(FunctionalState NewState, WAKEUP_POL pol);

/**
 * rtl876x_pinmux.h
 *
 * \brief   Configure the MFB wake-up system functions in power off(shipping) mode.
 *
 * \param[in] NewState: Enable or disable MFB wake up.
 *            This parameter can be one of the following values:
 *            \arg ENABLE: Enable MFB wake up system.
 *            \arg DISABLE: Disable MFB wake up system.
 *
 * \return None.
 *
 * <b>Example usage</b>
 * \code{.c}
 * //io_test_set_mfb_mode is POWER_OFF_WAKEUP_TEST
 * void mfb_wake_up_enable(void)
 * {
 *     System_SetMFBWakeUpFunction(ENABLE);
 * }
 * \endcode
 */
void System_SetMFBWakeUpFunction(FunctionalState NewState);

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_PINMUX_H_ */

/** @} */ /* End of group PINMUX_Exported_Functions */
/** @} */ /* End of group 87x3d_PINMUX */


/******************* (C) COPYRIGHT 2024 Realtek Semiconductor *****END OF FILE****/

