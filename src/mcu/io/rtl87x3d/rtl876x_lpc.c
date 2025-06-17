/**
**********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_lpc.c
* @brief    This file provides all the lpcomp firmware functions.
* @details
* @author   elliot chen
* @date     2023-02-10
* @version  v1.0
*********************************************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "rtl876x_lpc.h"
#include "rtl876x_aon_reg.h"

/* Internal defines ------------------------------------------------------------*/
#define REG_FAST_WRITE_BASE_ADDR    (0x40000100UL)
#define REG_LPC_FAST_WDATA          (0x400001f0UL)
#define REG_LPC_FAST_ADDR           (0x400001f4UL)
#define REG_LPC_WR_STROBE           (0x400001f8UL)

/**
  * @brief  Fast write LPC register
  * @param  address: the address of LPC register .
  * @param  data: dta which write to LPC register.
  * @retval None
  */
void LPC_WriteRegister(uint32_t offset, uint32_t data)
{
    *((volatile uint32_t *)REG_LPC_FAST_WDATA) = data;
    *((volatile uint32_t *)REG_LPC_FAST_ADDR) = offset;
    *((volatile uint32_t *)REG_LPC_WR_STROBE) = 1;
}

/**
  * @brief  Initializes LPC peripheral according to
  *    the specified parameters in the LPC_InitStruct.
  * @param  LPC_InitStruct: pointer to a LPC_InitTypeDef
  *    structure that contains the configuration information for the
  *    specified LPC peripheral.
  * @retval None
  */
void LPC_Init(LPC_InitTypeDef *LPC_InitStruct)
{
    uint32_t reg_value = 0;
    /* Check the parameters */
    assert_param(IS_LPC_CHANNEL(LPC_InitStruct->LPC_Channel));
    assert_param(IS_LPC_EDGE(LPC_InitStruct->LPC_Edge));
    assert_param(IS_LPC_THRESHOLD(LPC_InitStruct->LPC_Threshold));

    /* Configure parameters */
    reg_value = (LPC_InitStruct->LPC_Channel << LPC_CH_NUM_Pos) | \
                (LPC_InitStruct->LPC_Edge);
    LPC_WriteRegister((uint32_t) & (LPC->LPC_CR0) - REG_FAST_WRITE_BASE_ADDR, reg_value);

    LPC_WriteRegister(0, 0x40000000); /*Enable interrupt signal to CPU NVIC (0x40000100UL bit30) */
    /*Configure lpc threshold */
    btaon_fast_update_safe(AON_FAST_REG0X_REG_MASK, BIT15, BIT15); // MBIAS_REG_MASK = 1
    AON_FAST_REG1X_MBIAS_TYPE aon_0xc02 = {.d16 = btaon_fast_read_safe(AON_FAST_REG1X_MBIAS)};
    aon_0xc02.MBIAS_SEL_VR_AUXADC = LPC_InitStruct->LPC_Threshold;
    btaon_fast_write_safe(AON_FAST_REG1X_MBIAS, aon_0xc02.d16);
    btaon_fast_update_safe(AON_FAST_REG0X_REG_MASK, BIT15, 0);     // MBIAS_REG_MASK = 0
}


/**
  * @brief  Fills each LPC_InitStruct member with its default value.
  * @param  LPC_InitStruct : pointer to a LPC_InitTypeDef structure which will be initialized.
  * @retval None
  */
void LPC_StructInit(LPC_InitTypeDef *LPC_InitStruct)
{
    LPC_InitStruct->LPC_Channel   = ADC_0;
    LPC_InitStruct->LPC_Edge      = LPC_Vin_Below_Vth;
    LPC_InitStruct->LPC_Threshold = LPC_800_mV;
}

/**
  * @brief  Enables or disables LPC peripheral.
  * @param  NewState: new state of LPC peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void LPC_Cmd(FunctionalState NewState)
{
    uint32_t reg_value = 0;

    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the LPC power */
        reg_value = (LPC->LPC_CR0) | LPC_POWER_EN_Msk;
        reg_value &= ~(LPC_FLAG_CLEAR_Msk);
        LPC_WriteRegister((uint32_t) & (LPC->LPC_CR0) - REG_FAST_WRITE_BASE_ADDR, reg_value);
    }
    else
    {
        /* Disable the LPC power */
        reg_value = (LPC->LPC_CR0) & (~LPC_POWER_EN_Msk);
        reg_value &= ~(LPC_FLAG_CLEAR_Msk);
        LPC_WriteRegister((uint32_t) & (LPC->LPC_CR0) - REG_FAST_WRITE_BASE_ADDR, reg_value);
    }
}

/**
  * @brief Start or stop the LPC counter.
  * @param  NewState: new state of the LPC counter.
  *   This parameter can be one of the following values:
  *     @arg ENABLE: Start LPCOMP counter.
  *     @arg DISABLE: Stop LPCOMP counter.
  * @retval None
  */
void LPC_CounterCmd(FunctionalState NewState)
{
    uint32_t reg_value = 0;

    /* Check the parameters */
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Start the LPCOMP counter */
        reg_value = (LPC->LPC_CR0) | LPC_COUNTER_START_Msk;
        reg_value &= ~(LPC_FLAG_CLEAR_Msk);
        LPC_WriteRegister((uint32_t) & (LPC->LPC_CR0) - REG_FAST_WRITE_BASE_ADDR, reg_value);
    }
    else
    {
        /* Stop the LPCOMP counter */
        reg_value = (LPC->LPC_CR0) & (~LPC_COUNTER_START_Msk);
        reg_value &= ~(LPC_FLAG_CLEAR_Msk);
        LPC_WriteRegister((uint32_t) & (LPC->LPC_CR0) - REG_FAST_WRITE_BASE_ADDR, reg_value);
    }
}

/**
  * @brief Reset the LPC counter.
  * @retval None
  */
void LPC_CounterReset(void)
{
    uint32_t reg_value = 0;

    /* Reset the LPCOMP counter */
    reg_value = (LPC->LPC_CR0) | LPC_COUNTER_RESET_Msk;
    reg_value &= ~(LPC_FLAG_CLEAR_Msk);
    LPC_WriteRegister((uint32_t) & (LPC->LPC_CR0) - REG_FAST_WRITE_BASE_ADDR, reg_value);

    reg_value = (LPC->LPC_CR0) & (~LPC_COUNTER_RESET_Msk);
    reg_value &= ~(LPC_FLAG_CLEAR_Msk);
    LPC_WriteRegister((uint32_t) & (LPC->LPC_CR0) - REG_FAST_WRITE_BASE_ADDR, reg_value);
}

/**
  * @brief  Configure LPCOMP counter's comparator value.
  * @param  data: LPCOMP counter's comparator value which can be 0 to 0xfff.
  * @retval None.
  */
void LPC_WriteComparator(uint32_t data)
{
    LPC_WriteRegister((uint32_t) & (LPC->LPC_CMP_LOAD) - REG_FAST_WRITE_BASE_ADDR, data);
}

/**
  * @brief  read LPCOMP comparator value.
  * @param none.
  * @retval LPCOMP comparator value.
  */
uint16_t LPC_ReadComparator(void)
{
    return ((LPC->LPC_CMP_LOAD) & 0xfff);
}

/**
  * @brief  Read LPC counter value.
  * @retval LPCOMP counter value.
  */
uint16_t LPC_ReadCounter(void)
{
    return ((LPC->LPC_CMP_CNT) & 0xfff);
}

/**
  * @brief  Enables or disables the specified LPC interrupts.
  * @param  LPC_INT: specifies the LPC interrupt source to be enabled or disabled.
  *   This parameter can be one of the following values:
  *     @arg LPC_INT_VOLTAGE_COMP: voltage detection interrupt.If Vin<Vth, cause this interrupt.
  *     @arg LPC_INT_COUNT_COMP: couter comparator interrupt.
  * @param  NewState: new state of the specified LPC interrupt.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void LPC_INTConfig(uint32_t LPC_INT, FunctionalState NewState)
{
    uint32_t reg_value = 0;

    /* Check the parameters */
    assert_param(IS_LPC_CONFIG_INT(LPC_INT));
    assert_param(IS_FUNCTIONAL_STATE(NewState));

    if (NewState != DISABLE)
    {
        /* Enable the selected LPC interrupt */
        reg_value = (LPC->LPC_CR0) | LPC_INT;
        reg_value &= ~(LPC_FLAG_CLEAR_Msk);
        LPC_WriteRegister((uint32_t) & (LPC->LPC_CR0) - REG_FAST_WRITE_BASE_ADDR, reg_value);
    }
    else
    {
        /* Disable the selected LPC interrupt */
        reg_value = (LPC->LPC_CR0) & (~LPC_INT);
        reg_value &= ~(LPC_FLAG_CLEAR_Msk);
        LPC_WriteRegister((uint32_t) & (LPC->LPC_CR0) - REG_FAST_WRITE_BASE_ADDR, reg_value);
    }
}

/**
  * @brief  Clear the specified LPC interrupt.
  * @param  LPC_INT: specifies the LPC interrupt to clear.
  *   This parameter can be one of the following values:
  *     @arg LPC_INT_COUNT_COMP: couter comparator interrupt.
  * @retval None.
  */
void LPC_ClearINTPendingBit(uint32_t LPC_INT)
{
    uint32_t reg_value = 0;

    /* Check the parameters */
    assert_param(IS_LPC_CLEAR_INT(LPC_INT));

    /* Clear counter comparator interrupt */
    if (LPC_INT == LPC_INT_COUNT_COMP)
    {
        reg_value = (LPC->LPC_CR0) | LPC_FLAG_CLEAR_Msk;
        LPC_WriteRegister((uint32_t) & (LPC->LPC_CR0) - REG_FAST_WRITE_BASE_ADDR, reg_value);

        reg_value = (LPC->LPC_CR0) & (~LPC_FLAG_CLEAR_Msk);
        LPC_WriteRegister((uint32_t) & (LPC->LPC_CR0) - REG_FAST_WRITE_BASE_ADDR, reg_value);
        LPC->LPC_CR0;
    }
}

/**
  * @brief  Checks whether the specified LPC interrupt is set or not.
  * @param  LPC_INT: specifies the LPC interrupt to check.
  *   This parameter can be one of the following values:
  *     @arg LPC_INT_COUNT_COMP: couter comparator interrupt.
  * @retval The new state of SPI_IT (SET or RESET).
  */
ITStatus LPC_GetINTStatus(uint32_t LPC_INT)
{
    ITStatus bit_status = RESET;

    /* Check the parameters */
    assert_param(IS_LPC_CLEAR_INT(LPC_INT));

    if (LPC_INT == LPC_INT_COUNT_COMP)
    {
        if (((LPC->LPC_SR) & LPC_TFLAG_Msk) != (uint32_t)RESET)
        {
            bit_status = SET;
        }
    }

    /* Return the LPC_INT status */
    return  bit_status;
}

void LPC_SetTriggerEdge(uint32_t LPC_Edge)
{
    uint32_t reg_value = 0;

    reg_value = (LPC->LPC_CR0) & (~LPC_POSEDGE_Msk);
    reg_value |= LPC_Edge;
    LPC_WriteRegister((uint32_t) & (LPC->LPC_CR0) - REG_FAST_WRITE_BASE_ADDR, reg_value);
}

/******************* (C) COPYRIGHT 2023 Realtek Semiconductor Corporation *****END OF FILE****/

