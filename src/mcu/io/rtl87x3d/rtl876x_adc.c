/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_adc.c
* @brief    This file provides all the ADC firmware functions.
* @details
* @author   renee
* @date     2023-02-10
* @version  v0.1
*********************************************************************************************************
*/
#include <string.h>
#include "rtl876x.h"
#include "rtl876x_bitfields.h"
#include "rtl876x_rcc.h"
#include "rtl876x_adc.h"
#include "rtl876x_aon_reg.h"
#include "section.h"

/* PWRDLY[24] : ADC_DATA_AVG_SEL_EN. . */
#define ADC_DATA_AVG_EN_Pos                             (24UL)
#define ADC_DATA_AVG_EN_BIT                             ((uint32_t)(0x1UL<< ADC_DATA_AVG_EN_Pos))

/* PWRDLY[27:25] : ADC_DATA_AVG_SEL_Pos. . */
#define ADC_DATA_AVG_SEL_Pos                            (25UL)
#define ADC_DATA_AVG_SEL_BIT                            ((uint32_t)(0x7UL<<ADC_DATA_AVG_SEL_Pos))

/**
 * @brief ADC DLPS restore register.
 *
 */
typedef struct
{
    uint32_t adc_reg[14];
} ADCStoreReg_TypeDef;

void ADC_SISet(void)
{
    /*Added to stabilize the power supply!*/
    uint16_t reg_value = 0;
    reg_value = btaon_fast_read_safe(AON_FAST_RG2X_AUXADC);
    reg_value &= ~0x06;
    /*set auxadc_SEL_LDO33_REF as 1.8v*/
    btaon_fast_write_safe(AON_FAST_RG2X_AUXADC, reg_value | 0x04);

    /*Todo set auxadc_LDO08_REF as 0.7v*/
    AON_FAST_RG1X_AUXADC_TYPE reg_1512;
    reg_1512.d16 = btaon_fast_read_safe(AON_FAST_RG1X_AUXADC);
    reg_1512.AUXADC_SEL_LDO08_REF = 2;
    reg_1512.AUXADC_SEL_LDO_MODE = 1;
    reg_1512.AUXADC_EN_TG = 1; //Joseph's request
    btaon_fast_write_safe(AON_FAST_RG1X_AUXADC, reg_1512.d16);

    /*Todo set auxadc_SEL_VREF as 0.7v*/
    AON_FAST_RG0X_AUXADC_TYPE reg_1510;
    reg_1510.d16 = btaon_fast_read_safe(AON_FAST_RG0X_AUXADC);
    reg_1510.AUXADC_SEL_VREF = 2;
    reg_1510.AUXADC_VCM_SEL = 2;
    btaon_fast_write_safe(AON_FAST_RG0X_AUXADC, reg_1510.d16);

}
/**
  * @brief Initializes the ADC peripheral according to the specified
  *   parameters in the ADC_InitStruct
  * @param  ADCx: selected ADC peripheral.
  * @param  ADC_InitStruct: pointer to a ADCInitTypeDef structure that
  *   contains the configuration information for the specified ADC peripheral
  * @retval None
  */
void ADC_Init(ADC_TypeDef *ADCx, ADC_InitTypeDef *ADC_InitStruct)
{
    uint8_t index = 0;

    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_ADC_LATCH_MODE(ADC_InitStruct->dataLatchEdge));
    assert_param(IS_ADC_CLOCK(ADC_InitStruct->adcClock));
    assert_param(IS_ADC_POWER_MODE(ADC_InitStruct->pwrmode));
    assert_param(IS_ADC_RG2X_0_DELAY_TIME(ADC_InitStruct->adcRG2X0Dly));
    assert_param(IS_ADC_RG0X_1_DELAY_TIME(ADC_InitStruct->adcRG0X1Dly));
    assert_param(IS_ADC_RG0X_0_DELAY_TIME(ADC_InitStruct->adcRG0X0Dly));
    assert_param(IS_ADC_BURST_SIZE_CONFIG(ADC_InitStruct->adcBurstSize));
    assert_param(IS_ADC_FIFO_THRESHOLD(ADC_InitStruct->adcFifoThd));

    ADC_SISet();

    /*Disable all interrupt.*/
    ADCx->INTCR &= (~0x1f);
    /* Set power mode first */
    ADCx->PWRDLY = (ADC_InitStruct->pwrmode | (ADC_InitStruct->adc_fifo_stop_write) \
                    | (ADC_InitStruct->datalatchDly << 6)) | ADC_InitStruct->adcPowerAlwaysOnCmd;//| BIT15
    if (ADC_InitStruct->pwrmode == ADC_POWER_AUTO)
    {
        ADCx->PWRDLY |= (ADC_InitStruct->adcRG2X0Dly \
                         | ADC_InitStruct->adcRG0X1Dly \
                         | ADC_InitStruct->adcRG0X0Dly);
    }
    /* Disable schedule table */
    ADCx->SCHCR &= (~0xffff);
    /* Set schedule table */
    for (index = 0; index < 8; index++)
    {
        *(__IO uint32_t *)((uint32_t *)(&ADCx->SCHTAB0) + index) = (ADC_InitStruct->schIndex[index * 2] |
                                                                    (ADC_InitStruct->schIndex[index * 2 + 1] << 16));
    }
    ADCx->SCHCR = ADC_InitStruct->bitmap;
    /* Set ADC mode */
    ADCx->CR = ((ADC_InitStruct->dataLatchEdge)
                | (ADC_InitStruct->adcFifoThd << 20)
                | (ADC_InitStruct->adcBurstSize << 14)
                | (ADC_InitStruct->adcFifoOverWritEn)
                | (ADC_InitStruct->dataWriteToFifo << 27));
    /* adc data and clock config */
    if ((ADC_InitStruct->adcClock & 0x3FFF) < 5)
    {
        /* adc sample period should larger than 4(100ns)*/
        ADC_InitStruct->adcClock = 5;
    }
    ADCx->DATCLK = (ADC_InitStruct->dataMinusEn
                    | (ADC_InitStruct->dataAligned)
                    | (ADC_InitStruct->timerTriggerEn << 29)
                    | ((ADC_InitStruct->dataMinusOffset) << 16));
    ADCx->ANACTL |= (0x03 << 10);
    ADCx->REG_5C_CLK = ((ADC_InitStruct->adcClock & 0x3fff)
                        | (ADC_InitStruct->adcConvertTimePeriod << 30));
    /*clear adc fifo*/
    ADCx->CR |= BIT26;
    /*clear all interrupt*/
    ADCx->INTCR |= (0x1f << 8);
    return;
}

/**
  * @brief  Fills each ADC_InitStruct member with its default value.
  * @param  ADC_InitStruct: pointer to an ADC_InitTypeDef structure which will be initialized.
  * @retval None
  */
void ADC_StructInit(ADC_InitTypeDef *ADC_InitStruct)
{
    ADC_InitStruct->adcConvertTimePeriod = ADC_CONVERT_TIME_500NS;
    ADC_InitStruct->adcClock                = ADC_CLK_78_125K;      /* ( n+ 1) cycle of 10M Hz */
    ADC_InitStruct->dataLatchEdge     = ADC_Latch_Data_Positive;
    ADC_InitStruct->adcFifoOverWritEn = ADC_FIFO_OVER_WRITE_ENABLE;
    ADC_InitStruct->timerTriggerEn    = DISABLE;
    ADC_InitStruct->dataWriteToFifo   = DISABLE;
    ADC_InitStruct->dataAligned       = ADC_DATA_LSB;
    ADC_InitStruct->dataMinusEn       = ADC_DATA_MINUS_DIS;
    ADC_InitStruct->dataMinusOffset   = 0;

    for (uint8_t i = 0; i < 16; i++)
    {
        ADC_InitStruct->schIndex[i]         = 0;
    }
    ADC_InitStruct->bitmap              = 0x0;

    ADC_InitStruct->adcFifoThd          = 0x06;
    ADC_InitStruct->adcBurstSize        = 0x1;

    ADC_InitStruct->adc_fifo_stop_write = 0;

    /*Reserved parameter, please do not change values*/
    ADC_InitStruct->pwrmode           = ADC_POWER_AUTO;
    ADC_InitStruct->datalatchDly      = 0x1;
    ADC_InitStruct->adcRG2X0Dly       = ADC_RG2X_0_DELAY_80_US;
    ADC_InitStruct->adcRG0X1Dly       = ADC_RG0X_1_DELAY_320_US;
    ADC_InitStruct->adcRG0X0Dly       = ADC_RG0X_0_DELAY_240_US;
    ADC_InitStruct->adcPowerAlwaysOnCmd = ADC_POWER_ALWAYS_ON_DISABLE;

    return;
}

/**
  * @brief  Deinitializes the ADC peripheral registers to their default reset values(turn off ADC clock).
  * @param  ADCx: selected ADC peripheral.
  * @retval None
  */
void ADC_DeInit(ADC_TypeDef *ADCx)
{
    assert_param(IS_ADC_PERIPH(ADCx));

    RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, (FunctionalState)DISABLE);

    return;
}

/**
  * @brief  Enables or disables the specified ADC interrupts.
  * @param  ADCx: selected ADC peripheral.
  * @param  ADC_IT: specifies the ADC interrupts sources to be enabled or disabled.
  *   This parameter can be any combination of the following values:
  *     @arg ADC_INT_FIFO_RD_REQ :FIFO read request
  *     @arg ADC_INT_FIFO_RD_ERR :FIFO read error
  *     @arg ADC_INT_FIFO_TH :ADC FIFO size>thd
  *     @arg ADC_INT_FIFO_FULL :ADC FIFO overflow
  *     @arg ADC_INT_ONE_SHOT_DONE :ADC one shot mode done
  * @param  NewState: new state of the specified ADC interrupts.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
void ADC_INTConfig(ADC_TypeDef *ADCx, uint32_t ADC_IT, FunctionalState newState)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_FUNCTIONAL_STATE(newState));
    assert_param(IS_ADC_IT(ADC_IT));

    if (newState != DISABLE)
    {
        /* Enable the selected ADC interrupts */
        ADCx->INTCR |= ADC_IT;
    }
    else
    {
        /* Disable the selected ADC interrupts */
        ADCx->INTCR &= (uint32_t)~ADC_IT;
    }
}

/**
  * @brief  read ADC data according to specific channel.
  * @param  ADCx: selected ADC peripheral.
  * @param  ScheduleIndex: can be 0 to 15
  * @retval The 12-bit converted ADC data.
  */
uint16_t ADC_Read(ADC_TypeDef *ADCx, uint8_t ScheduleIndex)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(ScheduleIndex < 16);

    if (ScheduleIndex & BIT(0))
    {
        return ((*(uint32_t *)((uint32_t *)(&ADCx->SCHD0) + (ScheduleIndex >> 1))) >> 16);
    }
    else
    {
        return (*(uint32_t *)((uint32_t *)(&ADCx->SCHD0) + (ScheduleIndex >> 1)));
    }
}

extern uint16_t ADC_GetBypassStaus(void);
RAM_TEXT_SECTION void ADC_BypassDisable(void)
{
    uint16_t adc_bypass_status = ADC_GetBypassStaus();
    if (adc_bypass_status)
    {
        uint16_t value16 = btaon_fast_read_safe(AON_FAST_RG1X_AUXADC);
        value16 &= ~adc_bypass_status;
        btaon_fast_write_safe(AON_FAST_RG1X_AUXADC, value16);
    }
}

#define ADC_EXTERNAL_MODE_CHECK(mode)       (mode < 0x2)
#define ADC_MODE_DIFFERENTIAL_VALUE         (0x1)
RAM_TEXT_SECTION void ADC_BypassEnable(void)
{
    uint16_t adc_bypass_status = ADC_GetBypassStaus();
    if (adc_bypass_status)
    {
        uint16_t value16 = btaon_fast_read_safe(AON_FAST_RG1X_AUXADC);
        uint16_t adc_bitmap = ADC->SCHCR & 0xffff;
        uint8_t index = 0;
        uint8_t adc_sch_table = 0;
        uint8_t adc_mode = 0;
        uint8_t ch_num = 0;

        while (adc_bitmap)
        {
            index = __builtin_ffs(adc_bitmap) - 1;
            adc_bitmap &= ~BIT(index);
            adc_sch_table = (index & BIT(0)) ? ((*(uint32_t *)((uint32_t *)(&ADC->SCHTAB0) +
                                                               (index >> 1))) >> 16) : (*(uint32_t *)((uint32_t *)(&ADC->SCHTAB0) + (index >> 1))) ;
            adc_mode = (adc_sch_table >> 3) & 0x3;
            ch_num = adc_sch_table & 0x7;

            if (ADC_EXTERNAL_MODE_CHECK(adc_mode) && (BIT(ch_num) & adc_bypass_status))
            {
                value16 |= BIT(ch_num);
                if (adc_mode == ADC_MODE_DIFFERENTIAL_VALUE)
                {
                    ch_num = (ch_num & BIT(0)) ? (ch_num - 1) : (ch_num + 1);
                    if (BIT(ch_num) & adc_bypass_status)
                    {
                        value16 |= BIT(ch_num);
                    }
                }
            }
        }
        btaon_fast_write_safe(AON_FAST_RG1X_AUXADC, value16);
    }
}

/**
  * @brief  Enables or disables the ADC peripheral.
  * @param  ADCx: selected ADC peripheral.
  * @param  adcMode: adc mode select.
        This parameter can be one of the following values:
  *     @arg ADC_One_Shot_Mode: one shot mode.
  *     @arg ADC_Auto_Sensor_Mode: compare mode.
  * @param  NewState: new state of the ADC peripheral.
  *   This parameter can be: ENABLE or DISABLE.
  * @retval None
  */
RAM_TEXT_SECTION void ADC_Cmd(ADC_TypeDef *ADCx, uint8_t adcMode, FunctionalState NewState)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    assert_param(IS_ADC_MODE(adcMode));

    if (NewState == ENABLE)
    {
        ADC_BypassEnable();
        /* In case manual mode */
        if (ADCx->PWRDLY  & BIT19)
        {
            ADCx->PWRDLY  |= (BIT10 | BIT12);
            ADC_delayUS(120);
            ADCx->PWRDLY  |= (BIT11);
            ADC_delayUS(30);
            ADCx->PWRDLY  |= (BIT13);
            ADC_delayUS(30);
            ADCx->PWRDLY  |= (BIT14);
        }

        /* Reset ADC mode first */
        ADCx->CR &= ~0x03;
        /* Enable ADC */
        ADCx->CR |= adcMode;

    }
    else
    {
        ADCx->CR &= ~0x03;
        ADC_BypassDisable();
    }

    return;
}

/**
  * @brief  Config ADC schedule table.
  * @param  ADCx: selected ADC peripheral.
  * @param  Index: Schedule table index.
  * @param  adcMode: ADC mode.
  *      this parameter can be one of the following values:
  *     @arg EXT_SINGLE_ENDED(index)
  *     @arg EXT_DIFFERENTIAL(index)
  *     @arg VREF_AT_CHANNEL7(index)
  *     @arg INTERNAL_VBAT_MODE
  *     @arg INTERNAL_VADPIN_MODE
  *     @arg INTERNAL_VDDCORE_MODE
  *     @arg INTERNAL_VCORE_MODE
  *     @arg INTERNAL_ICHARGE_MODE
  *     @arg INTERNAL_IDISCHG_MODE
  * @return none.
  */
void ADC_SchTableConfig(ADC_TypeDef *ADCx, uint16_t Index,
                        uint8_t adcMode)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));
    assert_param(IS_ADC_SCHEDULE_INDEX_CONFIG(adcMode));

    if (Index & BIT0)
    {
        *(uint32_t *)((uint32_t *)(&ADCx->SCHTAB0) + (Index >> 1)) |= (adcMode << 16);
    }
    else
    {
        *(uint32_t *)((uint32_t *)(&ADCx->SCHTAB0) + (Index >> 1)) |= adcMode;
    }

    return;
}

/**
  * @brief  Data from ADC FIFO.
  * @param  ADCx: selected ADC peripheral.
  * @param[out]  outBuf: buffer to save data read from ADC FIFO.
  * @param  count: number of data to be read.
  * @retval None
  */
void ADC_GetFifoData(ADC_TypeDef *ADCx, uint16_t *outBuf, uint16_t count)
{
    /* Check the parameters */
    assert_param(IS_ADC_PERIPH(ADCx));

    while (count--)
    {
        *outBuf++ = (uint16_t)ADCx->FIFO;
    }

    return;
}

/**
  * @brief  Config ADC bypass resistor.Attention!!!Channels using bypass mode cannot over 0.9V!!!!
  * @param  channelNum: external channel number, can be 0~7.
  * @param  NewState: ENABLE or DISABLE.
  * @retval None
  */
#define CHIP_ADC_CHANNEL_NUM             (8)
#define IS_ADC_EXTERNAL_CHANNEL(ch)      ((ch) < CHIP_ADC_CHANNEL_NUM)
extern void ADC_SetBypassStatus(uint16_t status);
void ADC_HighBypassCmd(uint8_t channelNum, FunctionalState NewState)
{
    if (!IS_ADC_EXTERNAL_CHANNEL(channelNum))
    {
        return;
    }

    uint16_t adc_bypass_status = ADC_GetBypassStaus();
    if (NewState != DISABLE)
    {
        adc_bypass_status |= BIT(channelNum);
    }
    else
    {
        adc_bypass_status &= ~BIT(channelNum);
    }
    ADC_SetBypassStatus(adc_bypass_status);
}

void ADC_HwEvgEn(ADC_TypeDef *ADCx, FunctionalState Newstate)
{
    if (Newstate)
    {
        ADCx->PWRDLY |= ADC_DATA_AVG_EN_BIT;
    }
    else
    {
        ADCx->PWRDLY &= ~ADC_DATA_AVG_EN_BIT;
    }
}

void ADC_HwEvgSel(ADC_TypeDef *ADCx, T_ADC_DTAT_AVG_SEL data_sel_by)
{
    ADCx->PWRDLY &= ~ADC_DATA_AVG_SEL_BIT;
    ADCx->PWRDLY |= ((uint32_t)data_sel_by << ADC_DATA_AVG_SEL_Pos);
}

uint32_t ADC_GetDLPSBufferLen(void)
{
    return sizeof(ADCStoreReg_TypeDef);
}

/**
  * @brief  ADC enter dlps callback function(Save ADC register values when system enter DLPS)
  * @param  None
  * @retval None
  */
void ADC_DLPSEnter(void *peri_reg, void *p_store_buf)
{
    ADCStoreReg_TypeDef *store_buf = (ADCStoreReg_TypeDef *)p_store_buf;
    ADC_TypeDef *ADCx = (ADC_TypeDef *)peri_reg;

    store_buf->adc_reg[0] =  ADCx->CR;
    store_buf->adc_reg[1] =  ADCx->SCHCR;
    store_buf->adc_reg[2] =  ADCx->INTCR;
    store_buf->adc_reg[3] =  ADCx->SCHTAB0;
    store_buf->adc_reg[4] =  ADCx->SCHTAB1;
    store_buf->adc_reg[5] =  ADCx->SCHTAB2;
    store_buf->adc_reg[6] =  ADCx->SCHTAB3;
    store_buf->adc_reg[7] =  ADCx->SCHTAB4;
    store_buf->adc_reg[8] =  ADCx->SCHTAB5;
    store_buf->adc_reg[9] =  ADCx->SCHTAB6;
    store_buf->adc_reg[10] = ADCx->SCHTAB7;
    store_buf->adc_reg[11] = ADCx->PWRDLY;
    store_buf->adc_reg[12] = ADCx->DATCLK;
    store_buf->adc_reg[13] = ADCx->ANACTL;

    return;
}

/**
  * @brief  ADC exit dlps callback function(Resume ADC register values when system exit DLPS)
  * @param  None
  * @retval None
  */
void ADC_DLPSExit(void *peri_reg, void *p_store_buf)
{
    ADCStoreReg_TypeDef *store_buf = (ADCStoreReg_TypeDef *)p_store_buf;
    ADC_TypeDef *ADCx = (ADC_TypeDef *)peri_reg;

    /*Disable all interrupt.*/
    ADCx->INTCR &= (~0x1f);

    /* Set power mode first */
    ADCx->PWRDLY = store_buf->adc_reg[11];

    /* Disable schedule table */
    ADCx->SCHCR &= (~0xffff);

    ADCx->SCHTAB0 = store_buf->adc_reg[3];
    ADCx->SCHTAB1 = store_buf->adc_reg[4];
    ADCx->SCHTAB2 = store_buf->adc_reg[5];
    ADCx->SCHTAB3 = store_buf->adc_reg[6];
    ADCx->SCHTAB4 = store_buf->adc_reg[7];
    ADCx->SCHTAB5 = store_buf->adc_reg[8];
    ADCx->SCHTAB6 = store_buf->adc_reg[9];
    ADCx->SCHTAB7 = store_buf->adc_reg[10];
    ADCx->SCHCR   = store_buf->adc_reg[1];
    ADCx->CR = (store_buf->adc_reg[0] & (~((uint32_t)0x03)));
    ADCx->DATCLK = store_buf->adc_reg[12];
    ADCx->ANACTL = store_buf->adc_reg[13];

    /*Clear ADC FIFO */
    ADCx->CR |= BIT26;
    /* Clear all interrupt */
    ADCx->INTCR |= (0x1f << 8);

    /* Restore specify interrupt */
    ADCx->INTCR = store_buf->adc_reg[2];

    return;
}



