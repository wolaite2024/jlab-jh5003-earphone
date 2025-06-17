/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_pinmux.c
* @brief    This file provides all the PINMUX firmware functions.
* @details
* @author   Chuanguo Xue
* @date     2023-02-10
* @version  v0.1
*********************************************************************************************************
*/
#include <string.h>
#include "rtl8763_syson_reg.h"
#include "rtl876x.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "section.h"

#define TOTAL_LED_PAD_NUM             (10)
#define TOTAL_EXT_PWR_PIN_NUM         (40)

#define PAD_BIT_TABLE_OFFSET                   (12)
#define PAD_ITEM(reg_addr, bit_num)            ((bit_num << PAD_BIT_TABLE_OFFSET) | (reg_addr))
#define PAD_ITEM_ADDR(item)                    (item & 0xFFF)
#define PAD_ITEM_BIT_OFFSET(item)              ((item & 0xF000) >> PAD_BIT_TABLE_OFFSET)
#define PAD_ITEM_EMPTY                         (0)

#define PIN_TABLE_SIZE                         (sizeof(PINADDR_TABLE)/sizeof(uint16_t))

#define PAD_CFG_REG_MASK                       (0xF3)
#define PAD_PSRAM_CFG_REG_MASK                 (0xE1)

typedef struct
{
    uint8_t pad_buf[11];
    uint32_t uart_1wire_reg;
} PADStoreReg_Typedef;

typedef union _PAD_REG_TYPE
{
    uint8_t d8;
    struct
    {
        uint16_t PAD_SHDN:      1;
        uint16_t AON_PAD_E:     1;
        uint16_t PAD_WKPOL:     1;
        uint16_t PAD_WKEN:      1;
        uint16_t AON_PAD_O:     1;
        uint16_t AON_PAD_PDPUC: 1;
        uint16_t AON_PAD_PU:    1;
        uint16_t AON_PAD_PU_EN: 1;
    };
} PAD_REG_TYPE;

const uint16_t PINADDR_TABLE[] =
{
    0x23E, 0x23F, 0x240, 0x241, 0x244, 0x245, 0x246, 0x247,/*P0 0  ~ 7*/
    0x200, 0x201, 0x202, 0x203, 0x204, 0x205, 0x206, 0x207,/*P1 8  ~ 15*/
    0x20C, 0x20D, 0x20E, 0x20F, 0x210, 0x211, 0x212, 0x213,/*P2 16 ~ 23*/
    0x218, 0x219, 0x21A, 0x21B, 0x21C, 0x21D, 0x21E, 0x21F,/*P3 24 ~ 31*/
    0x222, 0x223, 0x224, 0x225, 0x226, 0x227, 0x228, 0x229,/*P4 32 ~ 39*/
    0x22C, 0x22D, 0x22E, 0x22F, 0x230, 0x231, 0x232, 0x233,/*P5 40 ~ 47*/
    /*P6_0 P6_1   LOUT_N LOUT_P ROUT_N ROUT_P MIC1_N MIC1_P     48 ~ 55*/
    0x236, 0x237, 0x256, 0x257, 0x258, 0x259, 0x252, 0x253,
    /*MIC2_N MIC2_P MIC3_N MIC3_P MIC4_N MIC4_P MIC5_N MIC5_P   56 ~ 63*/
    0x250, 0x251, 0x24E, 0x24F, 0x24C, 0x24D, 0x268, 0x269,
    /*MIC6_N MIC6_P AUX_R  AUX_L  MICBIAS NULL XI_32K XO_32K    64 ~ 71*/
    0x266, 0x267, 0x25B, 0x25A, 0x25C, 0x0,   0x265, 0x264,
    /*P6_2  P6_3   P6_4   P6_5   P6_6  P6_7   P7_0    P7_1      72 ~ 79*/
    0x3B0, 0x3B1, 0x3B2, 0x3B3, 0x3B4, 0x3B5, 0x23A, 0x23B,
    /*P7_2 P7_3   P7_4   P7_5   P7_6   P7_7                     80 ~ 87*/
    0x3B6, 0x3B7, 0x3B8, 0x3B9, 0x3BA, 0x3BB,
};

const uint16_t psram_pin_table[] =
{
    AON_FAST_REG0X_PAD_LPC + 1, AON_FAST_REG1X_PAD_LPC + 1,     //P6_2 ~ P6_3
    AON_FAST_REG1X_PAD_LPC,     AON_FAST_REG2X_PAD_LPC + 1,     //P6_4 ~ P6_5
    AON_FAST_REG2X_PAD_LPC,     AON_FAST_REG5X_PAD_LPC + 1,     //P6_6 ~ P6_7
    AON_FAST_REG5X_PAD_LPC,     AON_FAST_REG0X_PAD_LPC,         //P7_0 ~ P7_1
    AON_FAST_REG3X_PAD_LPC + 1, AON_FAST_REG3X_PAD_LPC,         //P7_2 ~ P7_3
    AON_FAST_REG4X_PAD_LPC + 1, AON_FAST_REG4X_PAD_LPC,         //P7_4 ~ P7_5
    AON_FAST_REG6X_PAD_LPC,     AON_FAST_REG6X_PAD_LPC,         //P7_6 ~ P7_7
};

const uint16_t PIN_ADDR_SW_TABLE[] =
{
    PAD_ITEM(0x248, 0),  PAD_ITEM(0x248, 1), PAD_ITEM(0x248, 2),  PAD_ITEM(0x248, 3),
    PAD_ITEM(0x248, 4),  PAD_ITEM(0x248, 5), PAD_ITEM(0x248, 6),  PAD_ITEM(0x248, 7), /*P0*/
    PAD_ITEM(0x208, 0),  PAD_ITEM(0x208, 1), PAD_ITEM(0x208, 2),  PAD_ITEM(0x208, 3),
    PAD_ITEM(0x208, 4),  PAD_ITEM(0x208, 5), PAD_ITEM(0x208, 6),  PAD_ITEM(0x208, 7), /*P1*/
    PAD_ITEM(0x214, 0),  PAD_ITEM(0x214, 1), PAD_ITEM(0x214, 2),  PAD_ITEM(0x214, 3),
    PAD_ITEM(0x214, 4),  PAD_ITEM(0x214, 5), PAD_ITEM(0x214, 6),  PAD_ITEM(0x214, 7), /*P2*/
    PAD_ITEM(0x220, 0),  PAD_ITEM(0x220, 1), PAD_ITEM(0x220, 2),  PAD_ITEM(0x220, 3),
    PAD_ITEM(0x220, 4),  PAD_ITEM(0x220, 5), PAD_ITEM(0x220, 6),  PAD_ITEM(0x220, 7), /*P3*/
    PAD_ITEM(0x22A, 0),  PAD_ITEM(0x22A, 1), PAD_ITEM(0x22A, 2),  PAD_ITEM(0x22A, 3),
    PAD_ITEM(0x22A, 4),  PAD_ITEM(0x22A, 5), PAD_ITEM(0x22A, 6),  PAD_ITEM(0x22A, 7), /*P4*/
    PAD_ITEM(0x234, 0),  PAD_ITEM(0x234, 1), PAD_ITEM(0x234, 2),  PAD_ITEM(0x234, 3),
    PAD_ITEM(0x234, 4),  PAD_ITEM(0x234, 5), PAD_ITEM(0x234, 6),  PAD_ITEM(0x234, 7), /*P5*/
    PAD_ITEM(0x238, 0),  PAD_ITEM(0x238, 1),                                          /*P6_0/P6_1*/
    PAD_ITEM(0x25E, 1),  PAD_ITEM(0x25E, 0), PAD_ITEM(0x25E, 3),  PAD_ITEM(0x25E, 2), /*LOUT_N/LOUT_P/ROUT_N/ROUT_P*/
    PAD_ITEM(0x25E, 8),  PAD_ITEM(0x25E, 7), PAD_ITEM(0x25E, 10), PAD_ITEM(0x25E, 9), /*MIC1_N/MIC1_P/MIC2_N/MIC2_P*/
    PAD_ITEM(0x25E, 12), PAD_ITEM(0x25E, 11), PAD_ITEM(0x25E, 14), PAD_ITEM(0x25E, 13), /*MIC3_N/MIC3_P/MIC4_N/MIC4_P*/
    PAD_ITEM(0x26E, 1),  PAD_ITEM(0x26E, 0), PAD_ITEM(0x26E, 3),  PAD_ITEM(0x26E, 2), /*MIC5_N/MIC5_P/MIC6_N/MIC6_P*/
    PAD_ITEM(0x25E, 5),  PAD_ITEM(0x25E, 4), PAD_ITEM(0x25E, 6),   PAD_ITEM_EMPTY,    /*AUX_R/AUX_L/MICBIAS/NULL*/
    PAD_ITEM(0x208, 11), PAD_ITEM(0x208, 15),                                         /*XI_32K/XO_32K*/
    PAD_ITEM(0x238, 2),  PAD_ITEM(0x238, 3), PAD_ITEM(0x238, 4),   PAD_ITEM(0x238, 5),
    PAD_ITEM(0x238, 6),  PAD_ITEM(0x238, 7),                                          /*P6_2~P6_7*/
    PAD_ITEM(0x23C, 0),  PAD_ITEM(0x23C, 1), PAD_ITEM(0x23C, 2),  PAD_ITEM(0x23C, 3),
    PAD_ITEM(0x23C, 4),  PAD_ITEM(0x23C, 5), PAD_ITEM(0x23C, 6),  PAD_ITEM(0x23C, 7), /*P7*/
};

const uint16_t WKSTATUS_TABLE[] =
{
    PAD_ITEM(0x132, 8),   PAD_ITEM(0x132, 9),  PAD_ITEM(0x132, 10),  PAD_ITEM(0x132, 11),
    PAD_ITEM(0x132, 12),  PAD_ITEM(0x132, 13), PAD_ITEM(0x132, 14),  PAD_ITEM(0x132, 15), /*P0*/
    PAD_ITEM(0x130, 8),   PAD_ITEM(0x130, 9),  PAD_ITEM(0x130, 10),  PAD_ITEM(0x130, 11),
    PAD_ITEM(0x130, 12),  PAD_ITEM(0x130, 13), PAD_ITEM(0x130, 14),  PAD_ITEM(0x130, 15), /*P1*/
    PAD_ITEM(0x130, 0),   PAD_ITEM(0x130, 1),  PAD_ITEM(0x130, 2),   PAD_ITEM(0x130, 3),
    PAD_ITEM(0x130, 4),   PAD_ITEM(0x130, 5),  PAD_ITEM(0x130, 6),   PAD_ITEM(0x130, 7),  /*P2*/
    PAD_ITEM(0x128, 8),   PAD_ITEM(0x128, 9),  PAD_ITEM(0x128, 10),  PAD_ITEM(0x128, 11),
    PAD_ITEM(0x128, 12),  PAD_ITEM(0x128, 13), PAD_ITEM(0x128, 14),  PAD_ITEM(0x128, 15), /*P3*/
    PAD_ITEM(0x128, 0),   PAD_ITEM(0x128, 1),  PAD_ITEM(0x128, 2),   PAD_ITEM(0x128, 3),
    PAD_ITEM(0x128, 4),   PAD_ITEM(0x128, 5),  PAD_ITEM(0x128, 6),   PAD_ITEM(0x128, 7),  /*P4*/
    PAD_ITEM(0x132, 0),   PAD_ITEM(0x132, 1),  PAD_ITEM(0x132, 2),   PAD_ITEM(0x132, 3),
    PAD_ITEM(0x132, 4),   PAD_ITEM(0x132, 5),  PAD_ITEM(0x132, 6),   PAD_ITEM(0x132, 7),  /*P5*/
    PAD_ITEM(0x126, 2),   PAD_ITEM(0x126, 3),                                             /*P6_0/P6_1*/
    PAD_ITEM(0x12e, 10),  PAD_ITEM(0x12e, 11), PAD_ITEM(0x12e, 8),   PAD_ITEM(0x12e, 9),  /*LOUT_N/LOUT_P/ROUT_N/ROUT_P*/
    PAD_ITEM(0x12e, 14),  PAD_ITEM(0x12e, 15), PAD_ITEM(0x12e, 12),  PAD_ITEM(0x12e, 13), /*MIC1_N/MIC1_P/MIC2_N/MIC2_P*/
    PAD_ITEM(0x12e, 3),   PAD_ITEM(0x12e, 4),  PAD_ITEM(0x12e, 1),   PAD_ITEM(0x12e, 2),  /*MIC3_N/MIC3_P/MIC4_N/MIC4_P*/
    PAD_ITEM(0x26c, 1),   PAD_ITEM(0x26c, 0),  PAD_ITEM(0x26c, 3),   PAD_ITEM(0x26c, 2),  /*MIC5_N/MIC5_P/MIC6_N/MIC6_P*/
    PAD_ITEM(0x12e, 6),   PAD_ITEM(0x12e, 7),  PAD_ITEM(0x12e, 5),   PAD_ITEM_EMPTY,      /*AUX_R/AUX_L/MICBIAS/NULL*/
    PAD_ITEM(0x126, 4),   PAD_ITEM(0x126, 5),                                             /*XI_32K/XO_32K*/
    PAD_ITEM(0x122, 2),   PAD_ITEM(0x122, 3),  PAD_ITEM(0x122, 4),   PAD_ITEM(0x122, 5),
    PAD_ITEM(0x122, 6),   PAD_ITEM(0x122, 7),                                             /*P6_2~P6_7*/
    PAD_ITEM(0x126, 0),   PAD_ITEM(0x126, 1),  PAD_ITEM(0x122, 10),  PAD_ITEM(0x122, 11),
    PAD_ITEM(0x122, 12),  PAD_ITEM(0x122, 13), PAD_ITEM(0x122, 14),  PAD_ITEM(0x122, 15), /*P7*/
};

const uint16_t SLEEP_LED_PIN_REG[TOTAL_LED_PAD_NUM] =
{
    /*ADC_0                                 ADC_1                       ADC_6                       ADC_7                       P1_0            */
    ((8 << 12) | 0x248), ((12 << 12) | 0x248), ((0 << 12) | 0x24A), ((4 << 12) | 0x24A), ((12 << 12) | 0x208),
    /*P1_1                                P1_4                            P2_0                                P2_1                                    P2_2 */
    ((8 << 12) | 0x208), ((0 << 12) | 0x20A), ((12 << 12) | 0x214), ((8 << 12) | 0x214), ((0 << 12) | 0x216)
};

const uint8_t pin_led[TOTAL_LED_PAD_NUM] =
{
    ADC_0, ADC_1, ADC_6, ADC_7,
    P1_0,  P1_1,  P1_4,  P2_0,
    P2_1,  P2_2
};

const uint8_t ext_pwr_pin_table[TOTAL_EXT_PWR_PIN_NUM] =
{
    ADC_7,  ADC_6,  ADC_5,  ADC_4,
    ADC_3,  ADC_2,  ADC_1,  ADC_0,
    P2_2,   P2_1,   P1_7,   P1_6,
    P1_1,   P1_0,   XO_32K, XI_32K,
    P3_2,   P4_1,   P4_0,   P2_7,
    P2_6,   P2_5,   P2_4,   P2_3,
    P5_7,   P5_6,   P5_0,   P3_7,
    P3_6,   P3_5,   P3_4,   P3_3,
    AUX_R,  AUX_L,  MIC4_P, MIC4_N,
    P6_1,   P6_0,   P4_7,   P4_6,
};

uint8_t hci_uart_rx_pin = P3_0;
uint8_t hci_uart_tx_pin = P3_1;

// TOTAL_PIN_NUM : 72
static const char *const pin_name[] =
{
    "P0_0",     "P0_1",      "P0_2",      "P0_3",      "P0_4",    "P0_5",      "P0_6",      "P0_7",      //0  ~ 7
    "P1_0",     "P1_1",      "P1_2",      "P1_3",      "P1_4",    "P1_5",      "P1_6",      "P1_7",      //8  ~ 15
    "P2_0",     "P2_1",      "P2_2",      "P2_3",      "P2_4",    "P2_5",      "P2_6",      "P2_7",      //16 ~ 23
    "P3_0",     "P3_1",      "P3_2",      "P3_3",      "P3_4",    "P3_5",      "P3_6",      "P3_7",      //24 ~ 31
    "P4_0",     "P4_1",      "P4_2",      "P4_3",      "P4_4",    "P4_5",      "P4_6",      "P4_7",      //32 ~ 39
    "P5_0",     "P5_1",      "P5_2",      "P5_3",      "P5_4",    "P5_5",      "P5_6",      "P5_7",      //40 ~ 47
    "P6_0",     "P6_1",      "LOUT_N",    "LOUT_P",    "ROUT_N",  "ROUT_P",    "MIC1_N",    "MIC1_P",    //48 ~ 55
    "MIC2_N",   "MIC2_P",    "MIC3_N",    "MIC3_P",    "MIC4_N",  "MIC4_P",    "NULL",      "NULL",      //56 ~ 63
    "NULL",     "NULL",      "AUX_R",     "AUX_L",     "MICBIAS", "NULL",      "XI_32K",    "XO_32K",    //64 ~ 71
    "P6_2",     "P6_3",      "P6_4",      "P6_5",      "P6_6",    "P6_7",      "P7_0",      "P7_1",      //72 ~ 79
    "P7_2",     "P7_3",      "P7_4",      "P7_5",      "P7_6",    "P7_7"                                 //80 ~ 87
};

uint8_t pad_dlps_store_vcore3_record[11] = {0};

extern RAM_TEXT_SECTION void io_dlps_pinmux_tag_vcore3(uint8_t pin_num, bool flag);

RAM_TEXT_SECTION void pad_update_aon_reg_8b(uint16_t reg_offset, uint8_t mask, uint8_t data)
{
    uint16_t read_data = btaon_fast_read_safe(reg_offset & 0xFFFE);
    uint8_t read_data_8b = 0;
    uint8_t data_offset = 0;

    if (reg_offset & BIT0)
    {
        read_data_8b = ((uint8_t)((read_data >> 8) & 0xFF));
        read_data &= 0xFF;
        reg_offset -= 1;
        data_offset = 8;
    }
    else
    {
        read_data_8b = (uint8_t)(read_data & 0xFF);
        read_data &= 0xFF00;
    }

    uint16_t write_data = (read_data | (((read_data_8b & (~mask)) | (data & mask)) << data_offset));
    btaon_fast_write_safe(reg_offset, write_data);
}

/**
  * @brief  According to the mode set to the pin , write the regster of AON which the pin coresponding .
  * @param  mode: mean one IO function, please refer to rtl876x_pinmux.h "Pin_Function_Number" part.
  *     @arg SHDN: use software mode.
  *     @arg Output_En: use pinmux mode.
        ......
        reference of bit of AON register mean in pinmux.h
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @param  value: value of the register bit ,0 or 1.
  * @retval None
  */

void Pad_TableConfig(uint8_t mode, uint8_t Pin_Num, uint8_t value)
{
    uint16_t tmpVal = 0;
    uint8_t tmpOffset = 0;
    uint16_t reg_temp;
    uint16_t    bit_temp;

    if (Pin_Num >= TOTAL_PIN_NUM)
    {
        return;
    }

    if (mode == PINMODE)
    {
        reg_temp = (PIN_ADDR_SW_TABLE[Pin_Num] & (0xfff));
        bit_temp = BIT(((PIN_ADDR_SW_TABLE[Pin_Num] & (0xf000)) >> 12));
        tmpVal = btaon_fast_read_safe(reg_temp);
        if (value)
        {
            tmpVal |= bit_temp;
        }
        else
        {
            tmpVal &= ~bit_temp;
        }
        btaon_fast_write_safe(reg_temp, tmpVal);

    }
    else
    {
        reg_temp = PINADDR_TABLE[Pin_Num];

        if ((reg_temp & 0x01) == 1)
        {
            reg_temp--;
            tmpOffset = 8;
        }
        tmpVal = btaon_fast_read_safe(reg_temp);
        if (value == 1)
        {
            tmpVal |= (mode << tmpOffset);
        }
        else
        {
            tmpVal &= ~(mode << tmpOffset);
        }
        btaon_fast_write_safe(reg_temp, tmpVal);
    }
}

/**
  * @brief  Deinit the IO function of one pin.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @retval None
  */
RAM_TEXT_SECTION void Pinmux_Deinit(uint8_t Pin_Num)
{
    if (Pin_Num >= TOTAL_PIN_NUM)
    {
        return;
    }
    uint16_t pinmux_offset_base;
    uint8_t reg_offset_bit;
    if (Pin_Num <= XO_32K)
    {
        pinmux_offset_base = PINMUX_BASE0_OFFSET;
    }
    else
    {
        pinmux_offset_base = PINMUX_BASE1_OFFSET;
        Pin_Num -= P6_2;
    }
    pinmux_offset_base += Pin_Num & (~BIT0);
    reg_offset_bit = (Pin_Num & BIT0) ? 8 : 0;

    uint16_t tmpVal = btaon_fast_read_safe(pinmux_offset_base);
    tmpVal = tmpVal & ~(0xFF << reg_offset_bit);
    btaon_fast_write_safe(pinmux_offset_base, tmpVal);
    return;
}

/**
  * @brief  Reset all pin to default value.
  * @param  None.
  * @note: two SWD pins will also be reset. Please use this function carefully.
  * @retval None
  */
void Pinmux_Reset(void)
{
    uint8_t i;
    uint16_t pinmux_offset_base = PINMUX_BASE0_OFFSET;
    for (i = 0; i <= XO_32K; i++)
    {
        btaon_fast_write_safe(pinmux_offset_base + i, 0x00);
    }
    pinmux_offset_base = PINMUX_BASE1_OFFSET;
    for (i = P6_2; i <= P7_6; i++)
    {
        btaon_fast_write_safe(pinmux_offset_base + i - P6_2, 0x00);
    }

    return;
}

/**
  * @brief  config the corresponding pad.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @param  AON_PAD_MODE: use software mode or pinmux mode.
  *     This parameter can be one of the following values:
  *     @arg PAD_SW_MODE: use software mode.
  *     @arg PAD_PINMUX_MODE: use pinmux mode.
  * @param  AON_PAD_PwrOn: config power of pad.
  *     This parameter can be one of the following values:
  *     @arg PAD_NOT_PWRON: shutdown power of pad.
  *     @arg PAD_IS_PWRON: enable power of pad.
  * @param  AON_PAD_Pull: config pad pull mode.
  *     This parameter can be one of the following values:
  *     @arg PAD_PULL_NONE: no pull.
  *     @arg PAD_PULL_UP: pull this pin up.
  *     @arg PAD_PULL_DOWN: pull thi pin down.
  * @param  AON_PAD_E: config pad out put function.
  *     This parameter can be one of the following values:
  *     @arg PAD_OUT_DISABLE: disable pin output.
  *     @arg PAD_OUT_ENABLE: enable pad output.
  * @param  AON_PAD_O: config pin output level.
  *     This parameter can be one of the following values:
  *     @arg PAD_OUT_LOW: pad output low.
  *     @arg PAD_OUT_HIGH: pad output high.
  * @retval None
  */
RAM_TEXT_SECTION
void Pad_Config(uint8_t Pin_Num,
                PAD_Mode AON_PAD_Mode,
                PAD_PWR_Mode AON_PAD_PwrOn,
                PAD_Pull_Mode AON_PAD_Pull,
                PAD_OUTPUT_ENABLE_Mode AON_PAD_E,
                PAD_OUTPUT_VAL AON_PAD_O)
{
    if (Pin_Num >= sizeof(PINADDR_TABLE) / sizeof(uint16_t))
    {
        return;
    }

    PAD_REG_TYPE  reg_value_8b = {.d8 = 0};

    /* Pad control mode */
    uint16_t pad_sw_item = PIN_ADDR_SW_TABLE[Pin_Num];
    uint16_t bit_temp = BIT(PAD_ITEM_BIT_OFFSET(pad_sw_item));
    btaon_fast_update_safe(PAD_ITEM_ADDR(pad_sw_item), bit_temp, (AON_PAD_Mode ? bit_temp : 0));

    /* Pad normal function */
    /* Pull Config */
    if (AON_PAD_Pull == PAD_PULL_UP)
    {
        reg_value_8b.AON_PAD_PU_EN = 1;
        reg_value_8b.AON_PAD_PU = 1;
    }
    else if (AON_PAD_Pull == PAD_PULL_DOWN)
    {
        reg_value_8b.AON_PAD_PU_EN = 1;
        reg_value_8b.AON_PAD_PU = 0;
    }
    else
    {
        reg_value_8b.AON_PAD_PU_EN = 0;
    }

    /* Output Config */
    reg_value_8b.AON_PAD_O = AON_PAD_O ? 1 : 0;
    reg_value_8b.AON_PAD_E = AON_PAD_E ? 1 : 0;
    reg_value_8b.PAD_SHDN = AON_PAD_PwrOn ? 1 : 0;

    pad_update_aon_reg_8b(PINADDR_TABLE[Pin_Num], PAD_CFG_REG_MASK, reg_value_8b.d8);

    if (Pin_Num >= P6_2 && Pin_Num <= P7_7)
    {
        pad_update_aon_reg_8b(psram_pin_table[Pin_Num - P6_2], PAD_PSRAM_CFG_REG_MASK, reg_value_8b.d8);
    }
}

/**
  * @brief  config the corresponding pad.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @param  AON_PAD_MODE: use software mode or pinmux mode.
  *     This parameter can be one of the following values:
  *     @arg PAD_SW_MODE: use software mode.
  *     @arg PAD_PINMUX_MODE: use pinmux mode.
  * @param  AON_PAD_PwrOn: config power of pad.
  *     This parameter can be one of the following values:
  *     @arg PAD_SHUTDOWN: shutdown power of pad.
  *     @arg PAD_IS_PWRON: enable power of pad.
  * @param  AON_PAD_Pull: config pad pull mode.
  *     This parameter can be one of the following values:
  *     @arg PAD_PULL_NONE: no pull.
  *     @arg PAD_PULL_UP: pull this pin up.
  *     @arg PAD_PULL_DOWN: pull thi pin down.
  * @param  AON_PAD_E: config pad out put function.
  *     This parameter can be one of the following values:
  *     @arg PAD_OUT_DISABLE: disable pin output.
  *     @arg PAD_OUT_ENABLE: enable pad output.
  * @param  AON_PAD_O: config pin output level.
  *     This parameter can be one of the following values:
  *     @arg PAD_OUT_LOW: pad output low.
  *     @arg PAD_OUT_HIGH: pad output high.
  * @retval None
  */
void Pad_ConfigExt(uint8_t Pin_Num,
                   PAD_Mode AON_PAD_Mode,
                   PAD_PWR_Mode AON_PAD_PwrOn,
                   PAD_Pull_Mode AON_PAD_Pull,
                   PAD_OUTPUT_ENABLE_Mode AON_PAD_E,
                   PAD_OUTPUT_VAL AON_PAD_O,
                   PAD_PULL_VAL AON_PAD_P)
{
    if (Pin_Num >= PIN_TABLE_SIZE)
    {
        return;
    }
    Pad_TableConfig(POWER, Pin_Num, AON_PAD_PwrOn);
    Pad_TableConfig(OUTVAL, Pin_Num, AON_PAD_O);
    Pad_TableConfig(OUTENREG, Pin_Num, AON_PAD_E);
    Pad_TableConfig(PULLVAL, Pin_Num, AON_PAD_P);

    if (AON_PAD_Pull == PAD_PULL_NONE)
    {
        Pad_TableConfig(PULLEN, Pin_Num, 0);
    }
    else if (AON_PAD_Pull == PAD_PULL_UP)
    {
        Pad_TableConfig(PULLDIR, Pin_Num, 1);
        Pad_TableConfig(PULLEN, Pin_Num, 1);
    }
    else
    {
        Pad_TableConfig(PULLDIR, Pin_Num, 0);
        Pad_TableConfig(PULLEN, Pin_Num, 1);
    }

    Pad_TableConfig(PINMODE, Pin_Num, AON_PAD_Mode);
}

/**
  * @brief  Set all pins to the default state.
  * @param  void.
  * @retval void.
  */

void Pad_AllConfigDefault(void)
{
    uint16_t i = 0;
    /* Set Output disable, pull-none, pull down, Software mode, Output_low, wakeup_disable, Wake up polarity high */
    for (i = 0; i < TOTAL_PIN_NUM; i++)
    {
        if (PINADDR_TABLE[i] == 0x0)
        {
            continue;
        }
        Pad_TableConfig(OUTENREG, i, PAD_OUT_DISABLE);
        Pad_TableConfig(PULLEN, i, PAD_PULL_DISABLE);
        Pad_TableConfig(PULLDIR, i, PAD_PULL_LOW);
        Pad_TableConfig(PINMODE, i, PAD_SW_MODE);
        Pad_TableConfig(OUTVAL, i, PAD_OUT_LOW);
        Pad_TableConfig(WakeUp_En, i, PAD_WAKEUP_DISABLE);
        Pad_TableConfig(WKPOL, i, PAD_WAKEUP_POL_HIGH);
    }

}

/**
  * @brief  Enable pin wakeup function.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @param  Polarity: PAD_WAKEUP_POL_HIGH--use high level wakeup, PAD_WAKEUP_POL_LOW-- use low level wakeup.
  * @retval None
  */
void System_WakeUpPinEnable(uint8_t Pin_Num, uint8_t Polarity)
{
    Pad_WakeupPolarityValue(Pin_Num, Polarity);
    Pad_WakeupEnableValue(Pin_Num, 1);
}

/**
  * @brief  Disable pin wakeup function.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @retval None
  */

void System_WakeUpPinDisable(uint8_t Pin_Num)
{
    Pad_WakeupEnableValue(Pin_Num, 0);
}

/**
  * @brief  Check wake up pin interrupt status.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @retval Pin interrupt status
  */
uint8_t System_WakeUpInterruptValue(uint8_t Pin_Num)
{
    return Pad_WakeupInterruptValue(Pin_Num);
}


/**
  * @brief  Clear all wake up pin interrupt pending bit.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part. PAD_Px_STS[x]
  * @retval None
  */
void Pad_ClearAllWakeupINT(void)
{
    uint16_t tmpVal;
    btaon_fast_write_safe(0x128, 0xFFFF);
    btaon_fast_write_safe(0x12e, 0xFFFF);
    btaon_fast_write_safe(0x130, 0xFFFF);
    btaon_fast_write_safe(0x132, 0xFFFF);

    /*P6_0,1 P7_0,1*/
    tmpVal = btaon_fast_read_safe(0x126);
    tmpVal |= 0x0F;
    btaon_fast_write_safe(0x126, tmpVal);

    /*P6_2~7 P7_2~7*/
    btaon_fast_write_safe(0x122, 0xFFFF);
}

void Pad_OutputEnableValue(uint8_t Pin_Num, uint8_t value)//0xf6
{
    Pad_TableConfig(Output_En, Pin_Num, value);
}

void Pad_OutputControlValue(uint8_t Pin_Num, uint8_t value)
{
    Pad_TableConfig(Output_Val, Pin_Num, value);
}

void Pad_PullEnableValue(uint8_t Pin_Num, uint8_t value)
{
    Pad_TableConfig(Pull_En, Pin_Num, value);
}

void Pad_PullEnableValue_Dir(uint8_t Pin_Num, uint8_t value, uint8_t Pull_Direction_value)
{
    uint8_t tmpOffset = 0;
    uint16_t tmpVal;
    uint16_t addr = PINADDR_TABLE[Pin_Num];
    if ((addr & 0x01) == 1)
    {
        addr--;
        tmpOffset = 8;
    }

    tmpVal = btaon_fast_read_safe(addr);
    if (value)
    {
        tmpVal |= Pull_En << tmpOffset;
    }
    else
    {
        tmpVal &= ~(Pull_En << tmpOffset);
    }
    if (Pull_Direction_value)
    {
        tmpVal |= Pull_Direction << tmpOffset;
    }
    else
    {
        tmpVal &= ~(Pull_Direction << tmpOffset);
    }
    btaon_fast_write_safe((addr), tmpVal);
}

void Pad_PullUpOrDownValue(uint8_t Pin_Num, uint8_t value)
{
    Pad_TableConfig(Pull_Direction, Pin_Num, value);
}

void Pad_ControlSelectValue(uint8_t Pin_Num, uint8_t value)
{
    /* Pad control mode */
    Pad_TableConfig(PINMODE, Pin_Num, value);
}

uint8_t Pad_WakeupInterruptValue(uint8_t Pin_Num)
{
    uint16_t reg_temp;
    uint16_t bit_temp;
    uint16_t value16 = 0;
    if (Pin_Num >= TOTAL_PIN_NUM)
    {
        return RESET;
    }
    uint8_t int_value = RESET;
    reg_temp = (WKSTATUS_TABLE[Pin_Num] & ~(0xf000));
    bit_temp = BIT((WKSTATUS_TABLE[Pin_Num]  & (0xf000)) >> 12);
    value16 = btaon_fast_read_safe(reg_temp);
    if (value16 & bit_temp)
    {
        int_value = SET;
    }
    return int_value;
}

void Pad_ClearWakeupINTPendingBit(uint8_t Pin_Num)
{
    uint16_t reg_temp;
    uint16_t bit_temp;
    if (Pin_Num >= TOTAL_PIN_NUM)
    {
        return;
    }
    reg_temp = (WKSTATUS_TABLE[Pin_Num] & ~(0xf000));
    bit_temp = BIT((WKSTATUS_TABLE[Pin_Num]  & (0xf000)) >> 12);
    btaon_fast_write_safe(reg_temp, bit_temp);
}

void Pad_PowerOrShutDownValue(uint8_t Pin_Num, uint8_t value)
{
    Pad_TableConfig(SHDN, Pin_Num, value);
}

void Pad_WakeupEnableValue(uint8_t Pin_Num, uint8_t value)
{
    Pad_TableConfig(WakeUp_En, Pin_Num, value);
}

void Pad_WakeupPolarityValue(uint8_t Pin_Num, uint8_t value)
{
    Pad_TableConfig(WKPOL, Pin_Num, value);
}

/*PAD_X_PDPUC[7:0] 0: 100K weakly pull. 1: 10K pull */
void Pad_PullConfigValue(uint8_t Pin_Num, uint8_t value)
{
    Pad_TableConfig(PULL_VALUE, Pin_Num, value);
}

/**
  * @brief  Config Pad Function.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P7_1, please refer to rtl876x.h "Pin_Number" part.
  * @param  PAD_FUNCTION_CONFIG_VAL: value.
  *   This parameter can be: AON_GPIO, LED0, LED1, LED2, CLK_REQ.
  * @retval None
  */
bool Pad_FunctionConfig(uint8_t Pin_Num, PAD_FUNCTION_CONFIG_VAL value)
{
    uint8_t i = 0;
    uint16_t tmpVal = 0;
    uint16_t reg_temp;
    uint16_t pos_temp;
    if (value <= CLK_REQ)
    {
        for (i = 0; i < TOTAL_LED_PAD_NUM ; i++)
        {
            if (Pin_Num == pin_led[i])
            {
                break;
            }
        }

        if (i >= TOTAL_LED_PAD_NUM)
        {
            return false;
        }

        reg_temp = (SLEEP_LED_PIN_REG[i] & (0xfff));
        pos_temp = (SLEEP_LED_PIN_REG[i] & (0xf000)) >> 12;

        tmpVal = btaon_fast_read_safe(reg_temp);
        tmpVal &= ~((0x7) << pos_temp);
        tmpVal |= ((value) << pos_temp);
        btaon_fast_write_safe(reg_temp, tmpVal);

        return true;
    }
    else if (value < PAD_FUNC_MAX)
    {
        uint8_t max_len = sizeof(ext_pwr_pin_table) / sizeof(uint8_t);
        for (i = 0; i < max_len ; i++)
        {
            if (Pin_Num == ext_pwr_pin_table[i])
            {
                break;
            }
        }

        if (i >= max_len)
        {
            return false;
        }

        value = (PAD_FUNCTION_CONFIG_VAL)(value - EXTRN_SWR_POW_SWR);

        reg_temp = AON_FAST_REG0X_PMUX_SEL_FSM_CTRL + (i >> 2);
        pos_temp = (i & 0x07) << 1;

        tmpVal = btaon_fast_read_safe(reg_temp);
        tmpVal &= ~((0x3) << pos_temp);
        tmpVal |= ((value) << pos_temp);
        btaon_fast_write_safe(reg_temp, tmpVal);

        return true;
    }
    else
    {
        return false;
    }
}

/**
  * @brief  Get the Pad AON fast register value ,the register addr get through mode and pin.
  * @param  mode: mode of set .
  *            This parameter reference the define from DRIE2 to PULLVAL in rtl876x_pinmux.h .
  *            Pin_Num:  pin number
  *            This parameter is from ADC_0 to P7_1, please refer to rtl876x.h "Pin_Number" part.
  * @retval reference PAD_Mode_Status.
  */
uint8_t  Pad_GetModeConfig(uint8_t mode, uint8_t Pin_Num)
{
    uint16_t reg_temp;
    uint16_t bit_temp;
    uint16_t tmpVal = 0;
    uint8_t tmpOffset = 0;
    if (Pin_Num < TOTAL_PIN_NUM)
    {
        if (mode == PINMODE)
        {
            reg_temp = (PIN_ADDR_SW_TABLE[Pin_Num] & (0xfff));
            bit_temp = BIT(((PIN_ADDR_SW_TABLE[Pin_Num] & (0xf000)) >> 12));
            tmpVal = btaon_fast_read_safe(reg_temp);
            if (tmpVal & bit_temp)
            {
                return SET;
            }
            else
            {
                return RESET;
            }
        }
        else
        {
            reg_temp = PINADDR_TABLE[Pin_Num];
            if ((reg_temp & 0x01) == 1)
            {
                reg_temp--;
                tmpOffset = 8;

            }
            tmpVal = btaon_fast_read_safe(reg_temp);

            if (tmpVal & (mode << tmpOffset))
            {
                return SET;
            }
            else
            {
                return RESET;
            }

        }
    }
    else
    {
        return PAD_AON_PIN_ERR;
    }

}

/**
  * @brief  Get the Pad AON output value .
  * @param  Pin_Num:  pin number
  *            This parameter is from ADC_0 to H_12, please refer to rtl876x.h "Pin_Number" part.
  * @retval reference PAD_AON_Status.
  */
uint8_t Pad_GetOutputCtrl(uint8_t Pin_Num)
{
    if (Pin_Num >= TOTAL_PIN_NUM)
    {
        return PAD_AON_PIN_ERR;
    }
    if (Pad_GetModeConfig(OUTENREG, Pin_Num) == RESET)
    {
        return PAD_AON_OUTPUT_DISABLE;
    }
    else if (Pad_GetModeConfig(PINMODE, Pin_Num) == SET)
    {
        return PAD_AON_PINMUX_ON;
    }
    if (Pad_GetModeConfig(OUTVAL, Pin_Num) == RESET)
    {
        return PAD_AON_OUTPUT_LOW;
    }
    else if (Pad_GetModeConfig(OUTVAL, Pin_Num) == SET)
    {
        return PAD_AON_OUTPUT_HIGH;
    }
    else
    {
        return PAD_AON_PIN_ERR;
    }
}

/**
  * @brief  set the system wakeup mode  .
  * @param  mode: mode of set .
  *            This parameter reference WAKEUP_EN_MODE .
  *        pol:  polarity to wake up
  *            This parameter WAKEUP_POL POL_HIGH means high level POL_LOW means low level to wakeup.
  *        NewState:  Enable or disable to wake up
  *            This parameter value is ENABLE or DISABLE.
  * @retval  1 means wrong mode.
  */
uint8_t  Pad_WakeUpCmd(WAKEUP_EN_MODE mode, WAKEUP_POL pol, FunctionalState NewState)
{
    AON_FAST_0x12A_TYPE wkup_pwdpad = {.d16 = btaon_fast_read_safe(AON_FAST_0x12A)};
    switch (mode)
    {
    case ADP_MODE:
        wkup_pwdpad.ADP_WKEN = NewState;
        wkup_pwdpad.ADP_WKPOL = pol;
        break;
    case BAT_MODE:
        wkup_pwdpad.BAT_WKEN = NewState;
        wkup_pwdpad.BAT_WKPOL = pol;
        break;
    case MFB_MODE:
        wkup_pwdpad.MFB_WKEN = NewState;
        wkup_pwdpad.MFB_WKPOL = (!pol);
        break;
    default:
        return 1;
    }
    btaon_fast_write_safe(AON_FAST_0x12A, wkup_pwdpad.d16);
    return 0;
}

const char *Pad_GetPinName(uint8_t pin_num)
{
    return pin_name[pin_num];
}

void Pad_SPICPinPullCtrl(SPIC_TypeDef *SPIC, bool Is_Aon_Control)
{
    AON_FAST_REG_SPIC_PULL_SELECT_TYPE aon_reg_spic_pull_sel = {.d16 = btaon_fast_read_safe(AON_FAST_REG_SPIC_PULL_SELECT)};

    if (SPIC == SPIC0)
    {
        aon_reg_spic_pull_sel.SPIC_PULL_SEL_SIO0_PULL_CTRL = Is_Aon_Control;
        aon_reg_spic_pull_sel.SPIC_PULL_SEL_SIO1_PULL_CTRL = Is_Aon_Control;
        aon_reg_spic_pull_sel.SPIC_PULL_SEL_SIO2_PULL_CTRL = Is_Aon_Control;
        aon_reg_spic_pull_sel.SPIC_PULL_SEL_SIO3_PULL_CTRL = Is_Aon_Control;
        btaon_fast_write_safe(AON_FAST_REG_SPIC_PULL_SELECT, aon_reg_spic_pull_sel.d16);
    }
    else if (SPIC == SPIC1)
    {
        aon_reg_spic_pull_sel.SPIC1_PULL_SEL_SIO0_PULL_CTRL = Is_Aon_Control;
        aon_reg_spic_pull_sel.SPIC1_PULL_SEL_SIO1_PULL_CTRL = Is_Aon_Control;
        aon_reg_spic_pull_sel.SPIC1_PULL_SEL_SIO2_PULL_CTRL = Is_Aon_Control;
        aon_reg_spic_pull_sel.SPIC1_PULL_SEL_SIO3_PULL_CTRL = Is_Aon_Control;
        btaon_fast_write_safe(AON_FAST_REG_SPIC_PULL_SELECT, aon_reg_spic_pull_sel.d16);
    }
    else if (SPIC == SPIC2)
    {
        aon_reg_spic_pull_sel.SPIC2_PULL_SEL_SIO0_PULL_CTRL = Is_Aon_Control;
        aon_reg_spic_pull_sel.SPIC2_PULL_SEL_SIO1_PULL_CTRL = Is_Aon_Control;
        aon_reg_spic_pull_sel.SPIC2_PULL_SEL_SIO2_PULL_CTRL = Is_Aon_Control;
        aon_reg_spic_pull_sel.SPIC2_PULL_SEL_SIO3_PULL_CTRL = Is_Aon_Control;
        btaon_fast_write_safe(AON_FAST_REG_SPIC_PULL_SELECT, aon_reg_spic_pull_sel.d16);
    }
    else if (SPIC == SPIC3)
    {
        aon_reg_spic_pull_sel.SPIC3_PULL_SEL_SIO0_PULL_CTRL = Is_Aon_Control;
        aon_reg_spic_pull_sel.SPIC3_PULL_SEL_SIO1_PULL_CTRL = Is_Aon_Control;
        aon_reg_spic_pull_sel.SPIC3_PULL_SEL_SIO2_PULL_CTRL = Is_Aon_Control;
        aon_reg_spic_pull_sel.SPIC3_PULL_SEL_SIO3_PULL_CTRL = Is_Aon_Control;
        btaon_fast_write_safe(AON_FAST_REG_SPIC_PULL_SELECT, aon_reg_spic_pull_sel.d16);
    }
    else if (SPIC == SPIC4)
    {
        AON_FAST_REG1X_SPIC_PULL_SELECT_TYPE aon_reg1x_spic_pull_sel = {.d16 = btaon_fast_read_safe(AON_FAST_REG1X_SPIC_PULL_SELECT)};
        aon_reg1x_spic_pull_sel.SPIC4_PULL_SEL_SIO0_PULL_CTRL = Is_Aon_Control;
        aon_reg1x_spic_pull_sel.SPIC4_PULL_SEL_SIO1_PULL_CTRL = Is_Aon_Control;
        aon_reg1x_spic_pull_sel.SPIC4_PULL_SEL_SIO2_PULL_CTRL = Is_Aon_Control;
        aon_reg1x_spic_pull_sel.SPIC4_PULL_SEL_SIO3_PULL_CTRL = Is_Aon_Control;
        aon_reg1x_spic_pull_sel.SPIC4_PULL_SEL_SIO4_PULL_CTRL = Is_Aon_Control;
        aon_reg1x_spic_pull_sel.SPIC4_PULL_SEL_SIO5_PULL_CTRL = Is_Aon_Control;
        aon_reg1x_spic_pull_sel.SPIC4_PULL_SEL_SIO6_PULL_CTRL = Is_Aon_Control;
        aon_reg1x_spic_pull_sel.SPIC4_PULL_SEL_SIO7_PULL_CTRL = Is_Aon_Control;
        btaon_fast_write_safe(AON_FAST_REG1X_SPIC_PULL_SELECT, aon_reg1x_spic_pull_sel.d16);
    }
}

#define PAD_POWER_DOMAIN_MASK                  (0xf)

typedef struct T_PAD_ANALOG_ITEM
{
    uint16_t pin_index;
    uint16_t item;
} T_PAD_ANALOG_ITEM;

typedef enum
{
    PAD_POWER_VCORE  = 0,
    PAD_POWER_VCORE2 = 1,
    PAD_POWER_VCORE3 = 2,

    PAD_POWER_MAX    = 3,
} T_PAD_POWER_DOMAIN;

const uint16_t pad_pwr_domain_table[TOTAL_PIN_NUM] =
{
    PAD_ITEM(0x4D0, 0),   PAD_ITEM(0x4D0, 4),  PAD_ITEM(0x4D0, 8),   PAD_ITEM(0x4D0, 12),
    PAD_ITEM(0x4D2, 0),   PAD_ITEM(0x4D2, 4),  PAD_ITEM(0x4D2, 8),   PAD_ITEM(0x4D2, 12), /*P0*/
    PAD_ITEM(0x4D4, 0),   PAD_ITEM(0x4D4, 4),  PAD_ITEM(0x4D4, 8),   PAD_ITEM(0x4D4, 12),
    PAD_ITEM(0x4D6, 0),   PAD_ITEM(0x4D6, 4),  PAD_ITEM(0x4D6, 8),   PAD_ITEM(0x4D6, 12), /*P1*/
    PAD_ITEM(0x4D8, 0),   PAD_ITEM(0x4D8, 4),  PAD_ITEM(0x4D8, 8),   PAD_ITEM(0x4D8, 12),
    PAD_ITEM(0x4DA, 0),   PAD_ITEM(0x4DA, 4),  PAD_ITEM(0x4DA, 8),   PAD_ITEM(0x4DA, 12), /*P2*/
    PAD_ITEM(0x4DC, 0),   PAD_ITEM(0x4DC, 4),  PAD_ITEM(0x4DC, 8),   PAD_ITEM(0x4DC, 12),
    PAD_ITEM(0x4DE, 0),   PAD_ITEM(0x4DE, 4),  PAD_ITEM(0x4DE, 8),   PAD_ITEM(0x4DE, 12), /*P3*/
    PAD_ITEM(0x4E0, 0),   PAD_ITEM(0x4E0, 4),  PAD_ITEM(0x4E0, 8),   PAD_ITEM(0x4E0, 12),
    PAD_ITEM(0x4E2, 0),   PAD_ITEM(0x4E2, 4),  PAD_ITEM(0x4E2, 8),   PAD_ITEM(0x4E2, 12), /*P4*/
    PAD_ITEM(0x4E4, 0),   PAD_ITEM(0x4E4, 4),  PAD_ITEM(0x4E4, 8),   PAD_ITEM(0x4E4, 12),
    PAD_ITEM(0x4E6, 0),   PAD_ITEM(0x4E6, 4),  PAD_ITEM(0x4E6, 8),   PAD_ITEM(0x4E6, 12), /*P5*/
    PAD_ITEM(0x4E8, 0),   PAD_ITEM(0x4E8, 4),                                             /*P6_0/P6_1*/
    PAD_ITEM(0x4EA, 0),   PAD_ITEM(0x4EA, 4),  PAD_ITEM(0x4EA, 8),   PAD_ITEM(0x4EA, 12), /*LOUT_N/P_UART/ROUT_N/ROUT_P*/
    PAD_ITEM(0x4EC, 0),   PAD_ITEM(0x4EC, 4),  PAD_ITEM(0x4EC, 8),   PAD_ITEM(0x4EC, 12), /*MIC1_N/MIC1_P/MIC2_N/MIC2_P*/
    PAD_ITEM(0x4EE, 0),   PAD_ITEM(0x4EE, 4),  PAD_ITEM(0x4EE, 8),   PAD_ITEM(0x4EE, 12), /*MIC3_N/MIC3_P/MIC4_N/MIC4_P*/
    PAD_ITEM(0x4F0, 0),   PAD_ITEM(0x4F0, 4),  PAD_ITEM(0x4F0, 8),   PAD_ITEM(0x4F0, 12), /*MIC5_N/MIC5_P/MIC6_N/MIC6_P*/
    PAD_ITEM(0x4F2, 0),   PAD_ITEM(0x4F2, 4),  PAD_ITEM(0x4F2, 8),   PAD_ITEM_EMPTY,      /*AUX_R/AUX_L/MICBIAS/NULL*/
    PAD_ITEM(0x4F4, 0),   PAD_ITEM(0x4F4, 4),                                             /*XI_32K/XO_32K*/
    PAD_ITEM(0x4E8, 8),   PAD_ITEM(0x4E8, 12),  PAD_ITEM(0x506, 0),   PAD_ITEM(0x506, 4),
    PAD_ITEM(0x506, 8),   PAD_ITEM(0x506, 12),                                            /*P6_2~P6_7*/
    PAD_ITEM(0x508, 0),   PAD_ITEM(0x508, 4),  PAD_ITEM(0x508, 8),   PAD_ITEM(0x508, 12),
    PAD_ITEM(0x50A, 0),   PAD_ITEM(0x50A, 4),  PAD_ITEM(0x50A, 8),   PAD_ITEM(0x50A, 12), /*P7*/
};

const T_PAD_ANALOG_ITEM analog_table[] =
{
    {P_UART,   PAD_ITEM(AON_FAST_0x254, 0)},
    {LOUT_N,   PAD_ITEM(AON_FAST_0x254, 1)},
    {ROUT_P,   PAD_ITEM(AON_FAST_0x254, 2)},
    {ROUT_N,   PAD_ITEM(AON_FAST_0x254, 3)},
    {AUX_L,    PAD_ITEM(AON_FAST_0x254, 4)},
    {AUX_R,    PAD_ITEM(AON_FAST_0x254, 5)},
    {MICBIAS,  PAD_ITEM(AON_FAST_0x254, 6)},
    {MIC1_P,   PAD_ITEM(AON_FAST_0x254, 7)},
    {MIC1_N,   PAD_ITEM(AON_FAST_0x254, 8)},
    {MIC2_P,   PAD_ITEM(AON_FAST_0x254, 9)},
    {MIC2_N,   PAD_ITEM(AON_FAST_0x254, 10)},
    {MIC3_P,   PAD_ITEM(AON_FAST_0x254, 11)},
    {MIC3_N,   PAD_ITEM(AON_FAST_0x254, 12)},
    {MIC4_P,   PAD_ITEM(AON_FAST_0x254, 13)},
    {MIC4_N,   PAD_ITEM(AON_FAST_0x254, 14)},
    {MIC5_P,   PAD_ITEM(AON_FAST_0x26A, 0)},
    {MIC5_N,   PAD_ITEM(AON_FAST_0x26A, 1)},
    {MIC6_P,   PAD_ITEM(AON_FAST_0x26A, 2)},
    {MIC6_N,   PAD_ITEM(AON_FAST_0x26A, 3)},
};

void System_SetAdpWakeUpFunction(FunctionalState NewState, WAKEUP_POL pol)
{
    AON_RG12X_TYPE aon_30 = {.d32 = HAL_READ32(SYSTEM_REG_BASE, AON_FAST_0x30)};
    AON_FAST_REG5X_MBIAS_TYPE aon_fast_reg5x_mbias = {.d16 = btaon_fast_read_safe(AON_FAST_REG5X_MBIAS)};
    aon_fast_reg5x_mbias. MBIAS_SEL_DPD_R5 = !pol;
    aon_fast_reg5x_mbias. MBIAS_SEL_DPD_R6 = NewState;
    btaon_fast_write_safe(AON_FAST_REG5X_MBIAS, aon_fast_reg5x_mbias.d16);
    aon_30.AON_MBIAS_SEL_DPD_RCK  = 1;
    HAL_WRITE32(SYSTEM_REG_BASE, AON_FAST_0x30, aon_30.d32);
    aon_30.AON_MBIAS_SEL_DPD_RCK  = 0;
    HAL_WRITE32(SYSTEM_REG_BASE, AON_FAST_0x30, aon_30.d32);
}

void System_DisableAdpWakeUpFunction(void)
{
    AON_FAST_REG5X_MBIAS_TYPE aon_fast_reg5x_mbias = {.d16 = btaon_fast_read_safe(AON_FAST_REG5X_MBIAS)};
    aon_fast_reg5x_mbias. MBIAS_SEL_DPD_R6 = DISABLE;
    btaon_fast_write_safe(AON_FAST_REG5X_MBIAS, aon_fast_reg5x_mbias.d16);
}

void System_SetMFBWakeUpFunction(FunctionalState NewState)
{
    AON_RG12X_TYPE aon_30 = {.d32 = HAL_READ32(SYSTEM_REG_BASE, AON_FAST_0x30)};
    AON_FAST_REG5X_MBIAS_TYPE aon_fast_reg5x_mbias = {.d16 = btaon_fast_read_safe(AON_FAST_REG5X_MBIAS)};
    aon_fast_reg5x_mbias. MBIAS_SEL_DPD_R7 = NewState;
    btaon_fast_write_safe(AON_FAST_REG5X_MBIAS, aon_fast_reg5x_mbias.d16);
    aon_30.AON_MBIAS_SEL_DPD_RCK  = 1;
    HAL_WRITE32(SYSTEM_REG_BASE, AON_FAST_0x30, aon_30.d32);
    aon_30.AON_MBIAS_SEL_DPD_RCK  = 0;
    HAL_WRITE32(SYSTEM_REG_BASE, AON_FAST_0x30, aon_30.d32);
}

static const uint8_t PIN_POWER_GROUP_mapping_tbl[TOTAL_PIN_NUM] =
{
    VDDIO1,  VDDIO1,  VDDIO1,  VDDIO1,
    VDDIO1,  VDDIO1,  VDDIO1,  VDDIO1, //adc
    VDDIO1,  VDDIO1,  VDDIO1,  VDDIO1,
    VDDIO1,  VDDIO1,  VDDIO1,  VDDIO1, //P1_x
    VDDIO2,  VDDIO2,  VDDIO2,  VDDIO2,
    VDDIO2,  VDDIO2,  VDDIO2,  VDDIO2, //P2_x
    VDDIO1,  VDDIO1,  VDDIO3,  VDDIO3,
    VDDIO3,  VDDIO3,  VDDIO3,  VDDIO3, //P3_x
    VDDIO2,  VDDIO2,  VDDIO4,  VDDIO4,
    VDDIO4,  VDDIO4,  VDDIO4,  VDDIO4, //P4_x
    VDDIO3,  VDDIO3,  VDDIO3,  VDDIO3,
    VDDIO3,  VDDIO3,  VDDIO3,  VDDIO3, //P5_x
    VDDIO4,  VDDIO4,  //P6_0  48  ,P6_1  49
    VCODEC,  VCODEC,  //50 51 LOUT_N P_UART
    VCODEC,  VCODEC, VCODEC, VCODEC,  //52~55 ROUT_N
    VCODEC,  VCODEC, VCODEC, VCODEC,  //56 ~59 MIC2_N
    VCODEC,  VCODEC, VCODEC, VCODEC,  //60 ~63 MIC4_N
    VCODEC,  VCODEC, VCODEC, VCODEC,  //64~67 MIC6_N
    VCODEC,  INVALID_PIN_GROUP, VDDIO1, VDDIO1, //68~71 XI_32K
    VDDIO5,  VDDIO5, VDDIO5, VDDIO5,  //72~75 P6_2~P6_5
    VDDIO5,  VDDIO5, VDDIO5, VDDIO5,  //76 ~ 79 P6_6 P7_1
    VDDIO5,  VDDIO5, VDDIO5, VDDIO5,  //80 ~ 83 P7_2~P7_5
    VDDIO5,  VDDIO5,                  //84 ~ 85 P7_6 P7_7
};

static const uint8_t Driver_register[4] =
{
    2,//VDDIO1 E2 E3 PAD_G1_E2 E3
    6,//VDDIO2 E2 E3 PAD_G2_E2 E3
    10,//VDDIO3 E2 E3 PAD_G3_E2 E3
    14,//VDDIO4 E2 E3 PAD_G4_E2 E3
};

T_PIN_POWER_GROUP Pad_GetPowerGroup(uint8_t pin)
{
    T_PIN_POWER_GROUP pin_power_group = INVALID_PIN_GROUP ;
    if (pin >= TOTAL_PIN_NUM)
    {
        return pin_power_group;
    }
    pin_power_group = (T_PIN_POWER_GROUP)PIN_POWER_GROUP_mapping_tbl[pin];
    return pin_power_group;
}

void Pad_SetDrivingCurrent(T_PIN_POWER_GROUP pin_power_group, T_DRIVER_LEVEL_MODE driver_level)
{
    if (pin_power_group == INVALID_PIN_GROUP)
    {
        return;
    }
    if (pin_power_group < VDDIO5)
    {
        btaon_fast_update_safe(AON_FAST_0x260, (0x03 << Driver_register[pin_power_group - 1]),
                               (driver_level << Driver_register[pin_power_group - 1]));
    }
    else if (pin_power_group == VDDIO5)
    {
        btaon_fast_update_safe(AON_FAST_0x262, (BIT10 | BIT11), (driver_level << 10));
    }
    else if (pin_power_group == VCODEC)
    {
        btaon_fast_update_safe(AON_FAST_0x262, (BIT6 | BIT7), (driver_level << 6));
    }
    else if (pin_power_group == GROUP_ADC)
    {
        btaon_fast_update_safe(AON_FAST_0x262, (BIT2 | BIT3), (driver_level << 2));
    }
}

bool Pad_SetPinDrivingCurrent(uint8_t pin, T_DRIVER_LEVEL_MODE driver_level)
{
    if (pin <= ADC_7)
    {
        Pad_SetDrivingCurrent(GROUP_ADC, driver_level);
    }
    else
    {
        Pad_SetDrivingCurrent(Pad_GetPowerGroup(pin), driver_level);
    }
    return true;
}

void Pad_AnalogMode(uint8_t pin, ANA_MODE mode)
{
    if ((pin < LOUT_N) | (pin > MICBIAS))
    {
        return;
    }

    for (uint16_t i = 0; i < sizeof(analog_table) / sizeof(T_PAD_ANALOG_ITEM); i++)
    {
        if (analog_table[i].pin_index == pin)
        {
            uint16_t pad_item = analog_table[i].item;

            if (pad_item != 0)
            {
                uint16_t addr = PAD_ITEM_ADDR(pad_item);
                uint16_t bit = BIT(PAD_ITEM_BIT_OFFSET(pad_item));

                if (mode == PAD_ANALOG_MODE)
                {
                    btaon_fast_update_safe(addr, bit, bit);
                }
                else
                {
                    btaon_fast_update_safe(addr, bit, 0);
                }
            }

            break;
        }
    }
}

void System_WakeUpInterruptEnable(uint8_t Pin_Num)
{
}

void System_WakeUpInterruptDisable(uint8_t Pin_Num)
{
}

RAM_TEXT_SECTION static void pad_power_domain(uint8_t pin_num, T_PAD_POWER_DOMAIN p_power_domain)
{
    if ((pin_num >= TOTAL_PIN_NUM) || (p_power_domain >= PAD_POWER_MAX))
    {
        return;
    }

    uint16_t reg_addr;
    uint16_t bit_offset;
    uint16_t pad_item = pad_pwr_domain_table[pin_num];
    reg_addr = PAD_ITEM_ADDR(pad_item);
    bit_offset = PAD_ITEM_BIT_OFFSET(pad_item);
    btaon_fast_update_safe(reg_addr, p_power_domain << bit_offset, PAD_POWER_DOMAIN_MASK << bit_offset);
    if (p_power_domain == PAD_POWER_VCORE3)
    {
        io_dlps_pinmux_tag_vcore3(pin_num, true);
    }
    else
    {
        io_dlps_pinmux_tag_vcore3(pin_num, false);
    }
}

#define IS_PINMUX_FUNC_VCORE2(func)           ((func== DSP2_GPIO_OUT) \
                                               || ((func>=DSP2_JTCK) && (func<=DSP2_JTRST)))
#define IS_PINMUX_FUNC_VCORE3(func)           (((func >= LRC_SPORT1) && (func) <=DACDAT_SPORT1) || \
                                               (func >= DMIC1_CLK) && (func <= SDO_CODEC_SLAVE)  || \
                                               ((func >= LRC_SPORT0) && (func <= MCLK_M)) || \
                                               ((func >= DMIC2_CLK) && (func <= MCLK_S)) ||\
                                               ((func >= ANCDSP_JTCK) && (func <= PDM_CLK_PDM_AMP)) ||\
                                               (func == ANCDSP_GPIO_OUT))

RAM_TEXT_SECTION static void Pinmux_ConfigApp(uint8_t Pin_Num, uint8_t Pin_Func)
{
    uint8_t reg_offset_bit;

    if (Pin_Num >= TOTAL_PIN_NUM)
    {
        return;
    }
    uint16_t pinmux_offset_base;
    if (Pin_Num <= XO_32K)
    {
        pinmux_offset_base = PINMUX_BASE0_OFFSET;
    }
    else
    {
        pinmux_offset_base = PINMUX_BASE1_OFFSET;
        Pin_Num -= P6_2;
    }
    pinmux_offset_base += Pin_Num & (~BIT0);
    reg_offset_bit = (Pin_Num & BIT0) ? 8 : 0;

    uint16_t tmpVal = btaon_fast_read_safe(pinmux_offset_base);
    tmpVal = (tmpVal & ~(0xFF << reg_offset_bit)) | Pin_Func << reg_offset_bit;
    btaon_fast_write_safe(pinmux_offset_base, tmpVal);
}

/**
  * @brief  Config pin to its corresponding IO function.
  * @param  Pin_Num: pin number.
  *     This parameter is from ADC_0 to P4_1, please refer to rtl876x.h "Pin_Number" part.
  * @param  Pin_Func: mean one IO function, please refer to rtl876x_pinmux.h "Pin_Function_Number" part.
  * @retval None
  */
RAM_TEXT_SECTION void Pinmux_Config(uint8_t Pin_Num, uint8_t Pin_Func)
{
    Pinmux_ConfigApp(Pin_Num, Pin_Func);

    if (IS_PINMUX_FUNC_VCORE2(Pin_Func))
    {
        pad_power_domain(Pin_Num, PAD_POWER_VCORE2);
    }
    else if (IS_PINMUX_FUNC_VCORE3(Pin_Func))
    {
        pad_power_domain(Pin_Num, PAD_POWER_VCORE3);
    }
    else
    {
        pad_power_domain(Pin_Num, PAD_POWER_VCORE);
    }
}

bool Pad_WakeUpDisable(WAKEUP_EN_MODE mode)
{
    AON_FAST_0x12A_TYPE wkup_pwdpad = {.d16 = btaon_fast_read_safe(AON_FAST_0x12A)};
    switch (mode)
    {
    case ADP_MODE:
        wkup_pwdpad.ADP_WKEN = DISABLE;
        break;
    case BAT_MODE:
        wkup_pwdpad.BAT_WKEN = DISABLE;
        break;
    case MFB_MODE:
        wkup_pwdpad.MFB_WKEN = DISABLE;
        break;
    default:
        return false;
    }
    btaon_fast_write_safe(AON_FAST_0x12A, wkup_pwdpad.d16);
    return true;
}

/**
  * @brief  Tag pinmux set to vcore3
  * @param  None
  * @retval None
  */
RAM_TEXT_SECTION void io_dlps_pinmux_tag_vcore3(uint8_t pin_num, bool flag)
{
    if (flag)
    {
        pad_dlps_store_vcore3_record[(pin_num >> 3)] |= BIT(pin_num & 0x7);
    }
    else
    {
        pad_dlps_store_vcore3_record[(pin_num >> 3)] &= ~(BIT(pin_num & 0x7));
    }
}

uint32_t Pad_GetDLPSBufferLen(void)
{
    return (sizeof(PADStoreReg_Typedef));
}

/**
  * @brief  Pinmux enter dlps callback function, Store the current pinmux setting
  * @param  None
  * @retval None
  */

void Pad_DLPSEnter(uint8_t skip_log_pin, void *p_store_buf)
{
    PADStoreReg_Typedef *store_buf = (PADStoreReg_Typedef *)p_store_buf;
    uint16_t reg_temp;
    uint16_t bit_temp = 0;
    uint16_t tmpVal = 0;

    for (uint8_t pin_index = 0; pin_index < TOTAL_PIN_NUM; pin_index++)
    {
        reg_temp = (PIN_ADDR_SW_TABLE[pin_index] & (0xfff));
        if ((reg_temp == 0) || (pin_index == skip_log_pin))
        {
            continue;
        }

        bit_temp = BIT(((PIN_ADDR_SW_TABLE[pin_index] & (0xf000)) >> 12));
        tmpVal = btaon_fast_read_safe(reg_temp);
        if ((tmpVal & bit_temp) &&
            (!(pad_dlps_store_vcore3_record[(pin_index >> 3)] & BIT(pin_index & 0x7))))
        {
            store_buf->pad_buf[(pin_index >> 3)] |= BIT(pin_index & 0x7);
            btaon_fast_update_safe(reg_temp, bit_temp, 0);
        }
    }
    store_buf->uart_1wire_reg = SYSBLKCTRL->u_2A8.REG_PERION_REG_TEST_MODE;

    return;
}

/**
  * @brief  Pad exit dlps callback function, Restore the pinmux setting from back up value.
  * @param  None
  * @retval None
  */
void Pad_DLPSExit(void *p_store_buf)
{
    PADStoreReg_Typedef *store_buf = (PADStoreReg_Typedef *)p_store_buf;
    uint16_t reg_temp;
    uint16_t bit_temp = 0;

    /* Restore uart 1wire before pinmux to prevent pulse in 1wire pin */
    SYSBLKCTRL->u_2A8.REG_PERION_REG_TEST_MODE = store_buf->uart_1wire_reg;

    for (uint8_t pin_index = 0; pin_index < TOTAL_PIN_NUM; pin_index++)
    {
        reg_temp = (PIN_ADDR_SW_TABLE[pin_index] & (0xfff));
        bit_temp = BIT(((PIN_ADDR_SW_TABLE[pin_index] & (0xf000)) >> 12));
        if (reg_temp && (store_buf->pad_buf[(pin_index >> 3)] & BIT(pin_index & 0x7)))
        {
            btaon_fast_update_safe(reg_temp, bit_temp, bit_temp);
        }
    }
    return;
}
