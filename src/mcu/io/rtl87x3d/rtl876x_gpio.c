/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_gpio.c
* @brief    This file provides all the GPIO firmware functions.
* @details
* @author   elliot chen
* @date     2023-02-10
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "rtl876x.h"
#include "rtl876x_gpio.h"
#include "rtl876x_rcc.h"
#include "section.h"

#define GPIO_CLOCK_SOURCE         (40000000)
#define GPIO_CLOCK_SOURCE_KHZ     (40000)

/**
  * @brief  GPIO DLPS store register.
  */
typedef struct
{
    uint32_t gpio_reg[11];
} GPIOStoreReg_Typedef;

const uint8_t GPIO_mapping_tbl[TOTAL_PIN_NUM] =
{
    0,  1,  2,  3,/*P0_0~P0_3*/
    4,  5,  6,  7,/*P0_4~P0_7*/
    8,  9,  10, 11,/*P1_0~P1_3*/
    12, 13, 14, 15,/*P1_4~P1_7*/
    16, 17, 18, 19,/*P2_0~P2_3*/
    20, 21, 22, 23,/*P2_4~P2_7*/
    24, 25, 26, 27,/*P3_0~P3_3*/
    28, 29, 30, 31,/*P3_4~P3_7*/
    32, 33, 34, 35,/*P4_0~P4_3*/
    36, 37, 38, 39,/*P4_4~P4_7*/
    40, 41, 42, 43,/*P5_0~P5_3*/
    44, 45, 46, 47,/*P5_4~P5_7*/
    /*P6_0 P6_1*/
    48, 49,
    /*LOUT_N LOUT_P ROUT_N  ROUT_P */
    71, 70, 73, 72,
    /*MIC1_N MIC1_P MIC2_N  MIC2_P*/
    67, 66, 69, 68,
    /*MIC3_N MIC3_P MIC4_N  MIC4_P*/
    75, 74, 77, 76,
    /*MIC5_N MIC5_P MIC6_N  MIC6_P*/
    255, 255, 255, 255,
    /*AUX_R  AUX_L  MICBIAS NULL*/
    64, 65, 78, 255,
    /*XIN_32K XO_32K*/
    62, 63,
    /*P6_2 ~ P6_5 */
    50, 51, 52, 53,
    /*P6_6 P6_7 */
    54, 55,
    /*P7_0 ~ P7_3 */
    56, 57, 58, 59,
    /*P7_4 ~ P7_7 */
    60, 61, 79, 80
};

//GPIO_TypeDef *GPIO = (GPIO_TypeDef *) GPIOA_REG_BASE;
/**
  * @brief  Deinitializes the GPIO peripheral registers to their default reset values.
  * @param  None.
  * @retval None.
  */
void GPIO_DeInit()
{
    GPIOx_DeInit(GPIOA);
}
/**
  * @brief  Deinitializes the GPIO peripheral registers to their default reset values.
  * @param  None.
  * @retval None.
  */
void GPIOx_DeInit(GPIO_TypeDef *GPIOx)
{
    if (GPIOx == GPIOA)
    {
        RCC_PeriphClockCmd(APBPeriph_GPIOA, APBPeriph_GPIOA_CLOCK, DISABLE);
    }
    else if (GPIOx == GPIOB)
    {
        RCC_PeriphClockCmd(APBPeriph_GPIOB, APBPeriph_GPIOB_CLOCK, DISABLE);
    }
    else
    {
        RCC_PeriphClockCmd(APBPeriph_GPIOC, APBPeriph_GPIOC_CLOCK, DISABLE);
    }
}
/**
  * @brief  Initializes the GPIO peripheral according to the specified
  *         parameters in the GPIO_InitStruct.
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that
  *         contains the configuration information for the specified GPIO peripheral.
  * @retval None
  */
void GPIO_Init(GPIO_InitTypeDef *GPIO_InitStruct)
{
    GPIOx_Init(GPIOA, GPIO_InitStruct);
}

/**
  * @brief  Initializes the GPIO A and B peripheral according to the specified
  *         parameters in the GPIO_InitStruct.
  * @param  GPIOx choose GPIOA or GPIOB
  * @param  GPIO_InitStruct: pointer to a GPIO_InitTypeDef structure that
  *         contains the configuration information for the specified GPIO peripheral.
  * @retval None
  */
void GPIOx_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct)
{
    /* Check the parameters */
    assert_param(IS_GPIO_PIN(GPIO_InitStruct->GPIO_PinBit));
    assert_param(IS_GPIO_MODE(GPIO_InitStruct->GPIO_Mode));
    assert_param(IS_GPIOIT_LEVEL_TYPE(GPIO_InitStruct->GPIO_ITTrigger));
    assert_param(IS_GPIOIT_POLARITY_TYPE(GPIO_InitStruct->GPIO_ITPolarity));
    assert_param(IS_GPIOIT_DEBOUNCE_TYPE(GPIO_InitStruct->GPIO_ITDebounce));

    /*if used P6_4~P7_6 to be gpio, need to set the switch function,
     The pins' default value means psram pin*/
    uint32_t pin_bit = GPIO_InitStruct->GPIO_PinBit;
    uint32_t p_debounce_ms = GPIO_InitStruct->GPIO_DebounceTime ? GPIO_InitStruct->GPIO_DebounceTime :
                             1;

    if (((GPIOx == GPIOB) && (pin_bit >= GPIO_GetPin(P6_4)) && (pin_bit <= GPIO_GetPin(P7_5)))
        || ((GPIOx == GPIOC) && (pin_bit == GPIO_GetPin(P7_6))))
    {
        uint16_t reg_temp = btaon_fast_read_safe(0x3f4);
        reg_temp |= BIT5;
        btaon_fast_write_safe(0x3f4, reg_temp);
    }
    /* GPIOx configure */
    if (GPIO_InitStruct->GPIO_Mode == GPIO_Mode_OUT)
    {
        GPIOx->DATADIR |= GPIO_InitStruct->GPIO_PinBit;

        if (GPIO_InitStruct->GPIO_ControlMode == GPIO_SOFTWARE_MODE)
        {
            /* Config GPIOx control software mode */
            GPIOx->DATASRC &= (~GPIO_InitStruct->GPIO_PinBit);
        }
        else
        {
            /* Config GPIOx hardware control mode */
            GPIOx->DATASRC |= (GPIO_InitStruct->GPIO_PinBit);
        }

    }
    else
    {
        /*Configure GPIO input mode */
        GPIOx->DATADIR = GPIOx->DATADIR & (~GPIO_InitStruct->GPIO_PinBit);

        if (GPIO_InitStruct->GPIO_ITCmd == ENABLE)
        {

            GPIOx->INTMASK = ~GPIO_Pin_All;

            /* configure GPIO interrupt trigger type */
            if (GPIO_InitStruct->GPIO_ITTrigger == GPIO_INT_Trigger_LEVEL)
            {
                GPIOx->INTTYPE = GPIOx->INTTYPE & (~GPIO_InitStruct->GPIO_PinBit);

                /* Level-sensitive synchronization enable register */
                GPIOx->LSSYNC |= GPIO_InitStruct->GPIO_PinBit;
            }
            else if (GPIO_InitStruct->GPIO_ITTrigger == GPIO_INT_Trigger_EDGE)
            {
                GPIOx->INTTYPE = (GPIOx->INTTYPE & (~GPIO_InitStruct->GPIO_PinBit))
                                 | GPIO_InitStruct->GPIO_PinBit;
            }
            else
            {
                GPIOx->INTBOTHEDGE |= GPIO_InitStruct->GPIO_PinBit;
            }

            /* configure Interrupt polarity register */
            if (GPIO_InitStruct->GPIO_ITPolarity == GPIO_INT_POLARITY_ACTIVE_LOW)
            {
                GPIOx->INTPOLARITY = GPIOx->INTPOLARITY & (~GPIO_InitStruct->GPIO_PinBit);
            }
            else
            {
                GPIOx->INTPOLARITY = (GPIOx->INTPOLARITY & (~GPIO_InitStruct->GPIO_PinBit))
                                     | GPIO_InitStruct->GPIO_PinBit;
            }
            /* Configure Debounce enable register */
            if (GPIO_InitStruct->GPIO_ITDebounce == GPIO_INT_DEBOUNCE_DISABLE)
            {
                GPIOx->DEBOUNCE = GPIOx->DEBOUNCE & (~GPIO_InitStruct->GPIO_PinBit);
            }
            else
            {
                GPIOx->DEBOUNCE = (GPIOx->DEBOUNCE & (~GPIO_InitStruct->GPIO_PinBit))
                                  | GPIO_InitStruct->GPIO_PinBit;

                /* Config debounce time , default debounce DIV is 14*/
                if (GPIOx == GPIOA)
                {
                    GPIOAB_DBCLK_DIV = (GPIOAB_DBCLK_DIV & 0xFFFF0000) | (((0xd) << 8) | (1 << 12));
                    GPIOAB_DBCLK_DIV |= (((p_debounce_ms * (GPIO_CLOCK_SOURCE_KHZ) >>
                                           (14)) - 1) & 0xff);
                }
                else if (GPIOx == GPIOB)
                {
                    GPIOAB_DBCLK_DIV = (GPIOAB_DBCLK_DIV & 0xFFFF) | (((0xd) << 24) | (1 << 28));
                    GPIOAB_DBCLK_DIV |= (((((p_debounce_ms) * (GPIO_CLOCK_SOURCE_KHZ) >>
                                            (14)) - 1) << 16) & 0xff0000);
                }
                else
                {
                    GPIOC_DBCLK_DIV = (((0xd) << 8) | (1 << 12));
                    GPIOC_DBCLK_DIV |= ((((p_debounce_ms) * (GPIO_CLOCK_SOURCE_KHZ) >>
                                          (14)) - 1) & 0xff);
                }
            }

            /* Configure Interrupt enable register */
            //GPIOx->INTEN |= GPIO_InitStruct->GPIO_PinBit;
        }
    }
}

/**
  * @brief    Fills each GPIO_InitStruct member with its default value.
  * @param  GPIO_InitStruct : pointer to a GPIO_InitTypeDef structure which will
  *    be initialized.
  * @retval None
  */
void GPIO_StructInit(GPIO_InitTypeDef *GPIO_InitStruct)
{
    /* Reset GPIO init structure parameters values */
    GPIO_InitStruct->GPIO_PinBit  = GPIO_Pin_All;
    GPIO_InitStruct->GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStruct->GPIO_ITCmd = DISABLE;
    GPIO_InitStruct->GPIO_ITTrigger = GPIO_INT_Trigger_LEVEL;
    GPIO_InitStruct->GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;
    GPIO_InitStruct->GPIO_ITDebounce = GPIO_INT_DEBOUNCE_DISABLE;
    GPIO_InitStruct->GPIO_ControlMode = GPIO_SOFTWARE_MODE;
    GPIO_InitStruct->GPIO_DebounceTime = 20;                 /* ms , can be 1~64 ms */
}

/**
  * @brief enable the specified GPIO interrupt.
  * @param  GPIO_Pin_x: where x can be 0 or 31.
  * @retval None
  */
//void GPIO_INTConfig(uint32_t GPIO_Pin, FunctionalState NewState)
void GPIO_INTConfig(uint32_t GPIO_Pin, FunctionalState NewState)
{
    GPIOx_INTConfig(GPIOA, GPIO_Pin, NewState);
}
/**
  * @brief enable the specified GPIO interrupt.
  * @param GPIOx choose GPIOA or GPIOB
  * @param  GPIO_Pin_x: where x can be 0 or 31.
  * @retval None
  */
//void GPIO_INTConfig(uint32_t GPIO_Pin, FunctionalState NewState)
void GPIOx_INTConfig(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin, FunctionalState NewState)
{

    /* Check the parameters */
    assert_param(IS_GPIO_PIN(GPIO_Pin));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the selected GPIO pin interrupts */
        GPIOx->INTEN |= GPIO_Pin;
    }
    else
    {
        /* Disable the selected GPIO pin interrupts */
        GPIOx->INTEN &= ~GPIO_Pin;
    }
}
/**
  * @brief clear the specified GPIO interrupt.
  * @param  GPIO_Pin_x: where x can be 0 or 31.
  * @retval None
  */
void GPIO_ClearINTPendingBit(uint32_t GPIO_Pin)
{
    GPIOx_ClearINTPendingBit(GPIOA,  GPIO_Pin);
}

/**
  * @brief clear the specified GPIO interrupt.
  * @param GPIOx choose GPIOA or GPIOB
  * @param  GPIO_Pin_x: where x can be 0 or 31.
  * @retval None
  */
RAM_TEXT_SECTION void GPIOx_ClearINTPendingBit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin)
{
    /* Check the parameters */
    assert_param(IS_GPIO_PIN(GPIO_Pin));

    GPIOx->INTCLR = GPIO_Pin;
    GPIOx->INTCLR;
}

/**
  * @brief mask the specified GPIO interrupt.
  * @param  GPIO_Pin_x: where x can be 0 or 31.
  * @retval None
  */
void GPIO_MaskINTConfig(uint32_t GPIO_Pin, FunctionalState NewState)
{
    GPIOx_MaskINTConfig(GPIOA, GPIO_Pin, NewState);
}

/**
  * @brief mask the specified GPIO interrupt.
  * @param GPIOx choose GPIOA or GPIOB
  * @param  GPIO_Pin_x: where x can be 0 or 31.
  * @retval None
  */
RAM_TEXT_SECTION void GPIOx_MaskINTConfig(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin,
                                          FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_GPIO_PIN(GPIO_Pin));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        GPIOx->INTMASK |= GPIO_Pin;
    }
    else
    {
        GPIOx->INTMASK &= ~(GPIO_Pin);
    }
}

/**
  * @brief get the specified GPIO pin.
  * @param  Pin_num: This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @retval  GPIO pin for GPIO initialization.
  */
RAM_TEXT_SECTION uint32_t GPIO_GetPinBit(uint8_t Pin_num)
{
    /* Check the parameters */
    /* Check the parameters, The pin_num which not assigned to key in app would be 0xFF*/
    if (Pin_num >= TOTAL_PIN_NUM)
    {
        return 0;
    }
    uint8_t GPIO_num = GPIO_mapping_tbl[Pin_num];

    if (GPIO_num > 80)
    {
        return 0;
    }
    return BIT((GPIO_num) & 0x1F);
}

/**
  * @brief get the specified GPIO pin number.
  * @param  Pin_num: This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @retval  GPIO pin number.
  */
uint8_t GPIO_GetNum(uint8_t Pin_num)
{
    /* Check the parameters, The pin_num which not assigned to key in app would be 0xFF*/
    if (Pin_num >= TOTAL_PIN_NUM)
    {
        return 0xFF;
    }
    uint8_t GPIO_num = GPIO_mapping_tbl[Pin_num];
    if (GPIO_num <= 80)
    {
        return GPIO_num;
    }
    else
    {
        return 0xFF;
    }
}

/**
  * @brief Enable the debance clk of GPIOA.
  * @param  Pin_num: This parameter is from ADC_0 to H4_1, please refer to rtl876x.h "Pin_Number" part.
  * @retval  GPIO pin number.
  */
void GPIO_DBClkCmd(FunctionalState NewState)
{
    GPIOx_DBClkCmd(GPIOA, NewState);
}

/**
  * @brief Enable the debance clk of GPIOx.
  * @param GPIOx choose GPIOA or GPIOB
  * @param Pin_num: This parameter is from ADC_0 to H4_1, please refer to rtl876x.h "Pin_Number" part.
  * @retval  none.
  */
void GPIOx_DBClkCmd(GPIO_TypeDef *GPIOx, FunctionalState NewState)
{
    if (NewState != DISABLE)
    {
        if (GPIOx == GPIOA)
        {
            GPIOAB_DBCLK_DIV |= BIT12;
        }
        else if (GPIOx == GPIOB)
        {
            GPIOAB_DBCLK_DIV |= BIT28;
        }
        else if (GPIOx == GPIOC)
        {
            GPIOC_DBCLK_DIV |= BIT12;
        }
    }
    else
    {
        if (GPIOx == GPIOA)
        {
            GPIOAB_DBCLK_DIV &= ~BIT12;
        }
        else if (GPIOx == GPIOB)
        {
            GPIOAB_DBCLK_DIV &= ~BIT28;
        }
        else if (GPIOx == GPIOC)
        {
            GPIOAB_DBCLK_DIV &= ~BIT12;
        }
    }
}

RAM_TEXT_SECTION void GPIOx_ChangePolarity(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin,
                                           GPIOIT_PolarityType int_type)
{
    GPIOx->INTMASK |= GPIO_Pin;

    if (GPIO_INT_POLARITY_ACTIVE_HIGH == int_type)
    {
        GPIOx->INTPOLARITY |= GPIO_Pin;
    }
    else if (GPIO_INT_POLARITY_ACTIVE_LOW == int_type)
    {
        GPIOx->INTPOLARITY &= ~GPIO_Pin;
    }

    GPIOx->INTCLR = GPIO_Pin;
    GPIOx->INTMASK &= ~GPIO_Pin;
}

uint32_t GPIO_GetDLPSBufferLen(void)
{
    return sizeof(GPIOStoreReg_Typedef);
}

void GPIO_DLPSEnter(void *peri_reg, void *p_store_buf)
{
    GPIOStoreReg_Typedef *store_buf = (GPIOStoreReg_Typedef *)p_store_buf;
    GPIO_TypeDef *GPIOx = (GPIO_TypeDef *)peri_reg;

    store_buf->gpio_reg[0] = GPIOx->DATAOUT;
    store_buf->gpio_reg[1] = GPIOx->DATADIR;
    store_buf->gpio_reg[2] = GPIOx->DATASRC;
    store_buf->gpio_reg[3] = GPIOx->INTEN;
    store_buf->gpio_reg[4] = GPIOx->INTMASK;
    store_buf->gpio_reg[5] = GPIOx->INTTYPE;
    store_buf->gpio_reg[6] = GPIOx->INTPOLARITY;
    store_buf->gpio_reg[7] = GPIOx->DEBOUNCE;
    if (GPIOx == GPIOA || GPIOx == GPIOB)
    {
        store_buf->gpio_reg[8] = SYSBLKCTRL->u_344.REG_PERION_PON_GPIO_DBNC_CTRL;
    }
    else
    {
        store_buf->gpio_reg[8] = SYSBLKCTRL->u_34C.REG_PERION_PON_GPIOC_DBNC_CTRL;
    }
    store_buf->gpio_reg[9] = GPIOx->INTBOTHEDGE;
    store_buf->gpio_reg[10] = GPIOx->LSSYNC;

    return;
}

void GPIO_DLPSExitOutputMode(void *peri_reg, void *p_store_buf)
{
    GPIOStoreReg_Typedef *store_buf = (GPIOStoreReg_Typedef *)p_store_buf;
    GPIO_TypeDef *GPIOx = (GPIO_TypeDef *)peri_reg;

    if (GPIOx == GPIOA)
    {
        SYSBLKCTRL->u_230.BITS_230.BIT_SOC_ACTCK_GPIOA_EN = 1;
        SYSBLKCTRL->u_230.BITS_230.BIT_SOC_SLPCK_GPIOA_EN = 1;
        SYSBLKCTRL->u_21C.BITS_21C.BIT_PERI_GPIOA_EN = 1;
    }
    else if (GPIOx == GPIOB)
    {
        SYSBLKCTRL->u_230.BITS_230.BIT_SOC_ACTCK_GPIOB_EN = 1;
        SYSBLKCTRL->u_230.BITS_230.BIT_SOC_SLPCK_GPIOB_EN = 1;
        SYSBLKCTRL->u_21C.BITS_21C.BIT_PERI_GPIOB_EN = 1;
    }
    else
    {
        SYSBLKCTRL->u_240.BITS_240.BIT_SOC_ACTCK_GPIOC_EN = 1;
        SYSBLKCTRL->u_240.BITS_240.BIT_SOC_SLPCK_GPIOC_EN = 1;
        SYSBLKCTRL->u_21C.BITS_21C.BIT_PERI_GPIOC_EN = 1;
    }

    GPIOx->DATADIR       = store_buf->gpio_reg[1];
    GPIOx->DATAOUT       = store_buf->gpio_reg[0];
}

void GPIO_DLPSExitInputMode(void *peri_reg, void *p_store_buf)
{
    GPIOStoreReg_Typedef *store_buf = (GPIOStoreReg_Typedef *)p_store_buf;
    GPIO_TypeDef *GPIOx = (GPIO_TypeDef *)peri_reg;

    GPIOx->DATASRC       = store_buf->gpio_reg[2];
    GPIOx->DATAOUT       = store_buf->gpio_reg[0];
    GPIOx->INTMASK       = 0xFFFFFFFF;
    GPIOx->INTTYPE       = store_buf->gpio_reg[5];
    GPIOx->INTPOLARITY   = store_buf->gpio_reg[6];
    GPIOx->LSSYNC        = store_buf->gpio_reg[10];
    GPIOx->INTBOTHEDGE   = store_buf->gpio_reg[9];

    GPIOx->INTEN         = store_buf->gpio_reg[3];
    if (GPIOx == GPIOA)
    {
        SYSBLKCTRL->u_344.REG_PERION_PON_GPIO_DBNC_CTRL |= ((store_buf->gpio_reg[8]) & 0x0000FFFF);
    }
    else if (GPIOx == GPIOB)
    {
        SYSBLKCTRL->u_344.REG_PERION_PON_GPIO_DBNC_CTRL |= ((store_buf->gpio_reg[8]) & 0xFFFF0000);
    }
    else
    {
        SYSBLKCTRL->u_34C.REG_PERION_PON_GPIOC_DBNC_CTRL = store_buf->gpio_reg[8];
    }
    GPIOx->DEBOUNCE      = store_buf->gpio_reg[7];
    /*
        Only clear interrupt when debounce enable and input mode
        If debounce disable, clear interrupt will lose interrupt
        If debounce enable, not clear interrupt will trigger interrupt twice
    */
    GPIOx->INTCLR        = store_buf->gpio_reg[7] & (~(store_buf->gpio_reg[1]));
    GPIOx->INTMASK       = store_buf->gpio_reg[4];

    return;
}

/******************* (C) COPYRIGHT 2023 Realtek Semiconductor Corporation *****END OF FILE****/

