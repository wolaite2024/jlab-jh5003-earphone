/**
**********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_tim.c
* @brief    This file provides all the Timer firmware functions.
* @details
* @author
* @date     2023-02-10
* @version  v0.1
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include "rtl876x_tim.h"
#include "rtl876x_rcc.h"
#include "section.h"

/**
  * @brief  Deinitializes the TIMx peripheral registers to their default reset values.
  * @param  TIMx: where x can be 0 to 7 to select the TIM peripheral.
  * @retval None
  */
void TIM_DeInit(void)
{
    /*enable timer IP clock and function */

    RCC_PeriphClockCmd(APBPeriph_TIMERA, APBPeriph_TIMERA_CLOCK, DISABLE);
    RCC_PeriphClockCmd(APBPeriph_TIMERB, APBPeriph_TIMERB_CLOCK, DISABLE);
}

/**
  * @brief  Initializes the TIMx Time Base Unit peripheral according to
  *         the specified parameters in the TIM_TimeBaseInitStruct.
  * @param  TIMx: where x can be 0 to 7 to select the TIM peripheral.
  * @param  TIM_TimeBaseInitStruct: pointer to a TIM_TimeBaseInitTypeDef
  *         structure that contains the configuration information for the
  *         specified TIM peripheral.
  * @retval None
  */
void TIM_TimeBaseInit(TIM_TypeDef *TIMx, TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct)
{
    uint32_t timerid = 0;
    uint32_t tempreg = 0;
    volatile uint32_t *count2_address;

    /* Check the parameters */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    assert_param(IS_TIM_MODE(TIM_TimeBaseInitStruct->TIM_Mode));
    assert_param(IS_TIM_PWM_DeadZone_En(TIM_TimeBaseInitStruct->PWMDeadZone_En));

    /* Select clock source which can be system clock or 40 MHz  or pll*/
    tempreg = (uint32_t)TIMx;
    if (tempreg <= TIM7_REG_BASE)
    {
        timerid = (tempreg - TIM0_REG_BASE) / 20;
    }
    else
    {
        timerid = 8 + (tempreg - TIM8_REG_BASE) / 20;
    }
    /*div the clock source,actually it need enable TIM_SOURCE_CLOCK_DIV_EN*/
    SYSBLKCTRL->u_348.REG_PERION_REG_PERI_GTIMER_CLK_CTRL |= BIT4;
    /*timer Fix 1M need to enable r_clk_timer_f1m_en  r_CLK_1M_SRC_EN r_CLK_5M_SRC_EN*/
    if (TIM_TimeBaseInitStruct->TIM_SOURCE_DIV == TIM_CLOCK_DIVIDER_40)
    {
        SYSBLKCTRL->u_20C.REG_PERION_REG_SYS_CLK_SEL_2  |=  BIT(29) | BIT(30);
        SYSBLKCTRL->u_360.REG_PERION_REG_PERI_GTIMER_CLK_SRC0 |= BIT(12);
    }
    if (timerid < 5)
    {
        SYSBLKCTRL->u_360.REG_PERION_REG_PERI_GTIMER_CLK_SRC0 &= ~(0x7 << (16 + timerid * 3));
        SYSBLKCTRL->u_360.REG_PERION_REG_PERI_GTIMER_CLK_SRC0 |= ((TIM_TimeBaseInitStruct->TIM_SOURCE_DIV)
                                                                  << (16 + timerid * 3));
    }
    else if ((timerid >= 5) && (timerid < 8))
    {
        SYSBLKCTRL->u_35C.REG_PERION_REG_PERI_GTIMER_CLK_SRC1 &= ~(0x7 << ((timerid - 5) * 3));
        SYSBLKCTRL->u_35C.REG_PERION_REG_PERI_GTIMER_CLK_SRC1 |= ((TIM_TimeBaseInitStruct->TIM_SOURCE_DIV)
                                                                  << ((timerid - 5) * 3));
    }
    else if ((timerid >= 8) && (timerid < 16))
    {
        SYSBLKCTRL->u_348.REG_PERION_REG_PERI_GTIMER_CLK_CTRL &= ~((uint32_t)0x7 << (8 +
                                                                                     (timerid - 8) * 3));
        SYSBLKCTRL->u_348.REG_PERION_REG_PERI_GTIMER_CLK_CTRL |= ((TIM_TimeBaseInitStruct->TIM_SOURCE_DIV)
                                                                  << (8 + (timerid - 8) * 3));
    }

    if (timerid == 3 || timerid == 5 || timerid == 7 || \
        timerid == 9 || timerid == 11)//depend todo 9 11
    {
        if (TIM_TimeBaseInitStruct->ClockDepend == true)
        {
            SYSBLKCTRL->u_360.REG_PERION_REG_PERI_GTIMER_CLK_SRC0 |= BIT(((timerid - 1) >> 1) - 1);
        }
        else
        {
            SYSBLKCTRL->u_360.REG_PERION_REG_PERI_GTIMER_CLK_SRC0 &= ~BIT(((timerid - 1) >> 1) - 1);
        }
    }

    /* set timer mode and mask interrupt */
    if (TIM_TimeBaseInitStruct->TIM_PWM_En == PWM_DISABLE)
    {
        TIMx->ControlReg = (TIM_TimeBaseInitStruct->TIM_Mode << 1) | BIT(2);
        /* set timer period */
        TIMx->LoadCount = TIM_TimeBaseInitStruct->TIM_Period;
    }
    else
    {
        TIMx->ControlReg = (TIM_TimeBaseInitStruct->TIM_Mode << 1) | BIT(2) | BIT(3);
        if (timerid < 8)
        {
            count2_address = &TIMER0_LOAD_COUNT2;
            count2_address = count2_address + timerid;
        }
        else
        {
            count2_address = &TIMER8_LOAD_COUNT2;
            count2_address = count2_address + timerid - 8;
        }
        *count2_address = TIM_TimeBaseInitStruct->TIM_PWM_High_Count ;
        /* set timer period */
        TIMx->LoadCount = TIM_TimeBaseInitStruct->TIM_PWM_Low_Count;
    }

    /* set pwm deadzone mode, pwm0_pn based on timer2, and pwm1_pn based on timer3 */
    if (TIM_TimeBaseInitStruct->PWMDeadZone_En == ENABLE)
    {
        if (timerid  == 2)
        {
            /* set pwm deadzone time */
            TIMER_PWM2_CR = ((TIM_TimeBaseInitStruct->PWM_Deazone_Size) \
                             | (TIM_TimeBaseInitStruct->PWM_Stop_State_N << 9) \
                             | (TIM_TimeBaseInitStruct->PWM_Stop_State_P << 10) \
                             | BIT12 \
                             | ((TIM_TimeBaseInitStruct->PWM_Deazone_Size == 0) ? BIT13 : 0));
        }
        else if (timerid == 3)
        {
            /* set pwm deadzone time */
            TIMER_PWM3_CR = ((TIM_TimeBaseInitStruct->PWM_Deazone_Size) \
                             | (TIM_TimeBaseInitStruct->PWM_Stop_State_N << 9) \
                             | (TIM_TimeBaseInitStruct->PWM_Stop_State_P << 10) \
                             | BIT12 \
                             | ((TIM_TimeBaseInitStruct->PWM_Deazone_Size == 0) ? BIT13 : 0));
        }
    }
    else
    {
        if (timerid == 2)
        {
            /*disable pwm0 deadzone mode*/
            TIMER_PWM2_CR &= ~BIT(12);
        }
        else if (timerid == 3)
        {
            /*disable pwm1 deadzone mode*/
            TIMER_PWM3_CR &= ~BIT(12);
        }
    }

    /* Clear the IT status */
    TIMx->EOI;
}

/**
  * @brief  Fills each TIM_InitStruct member with its default value.
  * @param  TIM_InitStruct : pointer to a TIM_InitTypeDef structure which will be initialized.
  * @retval None
  */
void TIM_StructInit(TIM_TimeBaseInitTypeDef *TIM_TimeBaseInitStruct)
{
    TIM_TimeBaseInitStruct->TIM_Mode = TIM_Mode_UserDefine;
    TIM_TimeBaseInitStruct->TIM_Period = 0xfff;
    TIM_TimeBaseInitStruct->TIM_EventMode = false;
    TIM_TimeBaseInitStruct->TIM_EventIndex = 1;
    TIM_TimeBaseInitStruct->TIM_EventDuration = TIM_EventDuration_64us;
    TIM_TimeBaseInitStruct->TIM_SOURCE_DIV = TIM_CLOCK_DIVIDER_1;
    TIM_TimeBaseInitStruct->ClockDepend = false;
    TIM_TimeBaseInitStruct->PWM_Stop_State_N = PWM_STOP_AT_HIGH;
    TIM_TimeBaseInitStruct->PWM_Stop_State_P = PWM_STOP_AT_LOW;
}

/**
  * @brief  Enables or disables the specified TIM peripheral.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @param  NewState: new state of the TIMx peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
RAM_TEXT_SECTION void TIM_Cmd(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the TIM Counter */
        TIMx->ControlReg |= BIT(0);
    }
    else
    {
        /* Disable the TIM Counter */
        TIMx->ControlReg &= ~(BIT(0));
    }
}

/**
  * @brief  change TIM period value.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @retval The new state of success or not  (SET or RESET).
  */
RAM_TEXT_SECTION void TIM_ChangePeriod(TIM_TypeDef *TIMx, uint32_t period)
{
    /* Check the parameters */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));

    TIMx->LoadCount = period;

    return;
}

/**
  * @brief  Enables or disables the specified TIMx interrupt.
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @param  NewState: new state of the TIMx peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
RAM_TEXT_SECTION void TIM_INTConfig(TIM_TypeDef *TIMx, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_TIM_ALL_PERIPH(TIMx));

    if (NewState != DISABLE)
    {
        /* Enable the Interrupt sources */
        TIMx->ControlReg &= ~(BIT(2));
    }
    else
    {
        /* Disable the Interrupt sources */
        TIMx->ControlReg |= BIT(2);
    }
}

/**
  * @brief  Change PWM freq and duty according high_cnt and low_cnt
  * @param  TIMx: where x can be 0 to 7 to select the TIMx peripheral.
  * @param  high_count:
    *   This parameter can be:
    * @param  low_count:
  *   This parameter can be:
  * @retval None
  */
RAM_TEXT_SECTION void TIM_PWMChangeFreqAndDuty(TIM_TypeDef *TIMx, uint32_t high_count,
                                               uint32_t low_count)
{
    volatile uint32_t *count2_address;
    uint32_t timerid = 0;
    uint32_t tempreg = 0;
    tempreg = (uint32_t)TIMx;
    if (tempreg <= TIM7_REG_BASE)
    {
        timerid = (tempreg - TIM0_REG_BASE) / 20;
    }
    else
    {
        timerid = 8 + (tempreg - TIM8_REG_BASE) / 20;
    }
    TIMx->LoadCount = low_count;

    if (timerid < 8)
    {
        count2_address = &TIMER0_LOAD_COUNT2;
        count2_address = count2_address + timerid;
    }
    else
    {
        count2_address = &TIMER8_LOAD_COUNT2;
        count2_address = count2_address + timerid - 8;
    }
    *count2_address = high_count;

}

/******************* (C) COPYRIGHT 2023 Realtek Semiconductor Corporation *****END OF FILE****/

