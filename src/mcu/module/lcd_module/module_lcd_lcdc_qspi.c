/**
*****************************************************************************************
*     Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file    module_lcd_lcdc_qspi.c
* @brief   This file provides LCDC QSPI functions
* @author
* @date
* @version v1.0
* *************************************************************************************
*/

#include "app_gui.h"
#if (LCD_INTERFACE == LCD_INTERFACE_LCDC_QSPI)
#include "rtl876x_rcc.h"
#include "rtl876x_pinmux.h"
#include "hal_gpio.h"
#include "module_lcd_lcdc_qspi.h"
#include "platform_utils.h"
#include "rtk_hal_lcd.h"
#include "rtl_lcdc_dbic.h"
#include "os_sched.h"
#include "module_lcd_te.h"
#if (TARGET_LCD_DEVICE == LCD_DEVICE_SH8601Z)
#include "lcd_sh8601z_454_qspi.h"
#elif (TARGET_LCD_DEVICE == LCD_DEVICE_ST7801)
#include "lcd_st7801_368_448_qspi.h"
#endif

#define LCDC_DMA_CHANNEL_NUM              lcdc_qspi_dma_ch_num
#define LCDC_DMA_CHANNEL_INDEX            LCDC_DMA_Channel0

static uint8_t lcd_rst_pin;
static uint8_t lcdc_qspi_dma_ch_num = 0xa5;

void lcd_pin_config(uint8_t lcd_rst)
{
    lcd_rst_pin = lcd_rst;
}

static void lcd_reset_init(void)
{
    hal_gpio_init();
    hal_gpio_init_pin(lcd_rst_pin, GPIO_TYPE_AON, GPIO_DIR_OUTPUT, GPIO_PULL_UP);
    hal_gpio_set_level(lcd_rst_pin, GPIO_LEVEL_HIGH);
}

void lcd_set_dma_ch_num(uint8_t num)
{
    lcdc_qspi_dma_ch_num = num;
}

void lcd_set_reset(bool reset)
{
    if (reset)
    {
        hal_gpio_set_level(lcd_rst_pin, GPIO_LEVEL_LOW);
    }
    else
    {
        hal_gpio_set_level(lcd_rst_pin, GPIO_LEVEL_HIGH);
    }
}

void rtk_lcd_hal_set_reset(bool reset)
{
#if (TARGET_LCD_DEVICE == LCD_DEVICE_ST7801)
    lcd_set_vci_en(false);
#endif
    lcd_set_reset(reset);
#if (TARGET_LCD_DEVICE == LCD_DEVICE_SH8601Z)
    lcd_set_avdd_en(false);
#endif
}

void lcdc_driver_init(void)
{
    RCC_DisplayClockConfig(DISPLAY_CLOCK_SOURCE_PLL1, DISPLAY_CLOCK_DIV_1);
    RCC_PeriphClockCmd(APBPeriph_DISP, APBPeriph_DISP_CLOCK, ENABLE);

    LCDC_InitTypeDef lcdc_init = {0};
    lcdc_init.LCDC_Interface = LCDC_IF_DBIC;
#if INPUT_PIXEL_BYTES == 4
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_ARGB8888;
#elif INPUT_PIXEL_BYTES == 3
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_RGB888;
#elif INPUT_PIXEL_BYTES == 2
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_RGB565;
#endif
    lcdc_init.LCDC_PixelOutpuFarmat = LCDC_OUTPUT_RGB565;
    lcdc_init.LCDC_PixelBitSwap = LCDC_SWAP_8BIT; //lcdc_handler_cfg->LCDC_TeEn = LCDC_TE_DISABLE;
#if (ENABLE_TE_FOR_LCD == 1)
    lcdc_init.LCDC_TeEn = ENABLE;
    lcdc_init.LCDC_TePolarity = LCDC_TE_EDGE_RISING;
    lcdc_init.LCDC_TeInputMux = LCDC_TE_LCD_INPUT;
#endif
    lcdc_init.LCDC_GroupSel = 1;
    lcdc_init.LCDC_DmaThreshold =
        112;    //only support threshold = 8 for DMA MSIZE = 8; the other threshold setting will be support later
#if (TARGET_LCD_DEVICE == LCD_DEVICE_SH8601Z)
    lcdc_init.LCDC_PhaseShift = LCDC_SPI_PHASE_SHIFT_180_DEGREE;
#endif
    LCDC_Init(&lcdc_init);

    LCDC_DBICCfgTypeDef dbic_init = {0};
    dbic_init.DBIC_SPEED_SEL         = 2;

    dbic_init.DBIC_TxThr             = 0;//0 or 4
    dbic_init.DBIC_RxThr             = 0;
    dbic_init.SCPOL                  = DBIC_SCPOL_LOW;
    dbic_init.SCPH                   = DBIC_SCPH_1Edge;
    DBIC_Init(&dbic_init);

    LCDC_SwitchMode(LCDC_MANUAL_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);
    LCDC_Cmd(ENABLE);

    LCDC_AXIMUXMode(LCDC_FW_MODE);
    DBIC_SwitchMode(DBIC_USER_MODE);
    DBIC_SwitchDirect(DBIC_TMODE_TX);

    DBIC_IMR_TypeDef dbic_reg_0x2c = {.d32 = DBIC->IMR};
    dbic_reg_0x2c.b.dreim = 1;
    DBIC->IMR = dbic_reg_0x2c.d32;

#if (TARGET_LCD_DEVICE == LCD_DEVICE_SH8601Z)
    lcd_set_avdd_en(true);
#endif
    lcd_set_reset(true);
    platform_delay_ms(120);
    lcd_set_reset(false);
    platform_delay_ms(50);
#if (TARGET_LCD_DEVICE == LCD_DEVICE_ST7801)
    lcd_set_vci_en(true);
#endif
}

static void lcd_pad_init(void)
{
    /*QSPI Config*/
    /*QSPI Pad Config, P9_0 -> SIO2, P9_1 -> SIO1, P9_2 -> CS, P9_3 -> SIO0, P9_4 -> CLK, P9_5 -> SIO3*/
    Pad_Config(P9_0, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(P9_1, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(P9_2, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(P9_3, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(P9_4, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(P9_5, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);

    Pad_HighSpeedFuncSel(P9_0, HS_Func0);
    Pad_HighSpeedFuncSel(P9_1, HS_Func0);
    Pad_HighSpeedFuncSel(P9_2, HS_Func0);
    Pad_HighSpeedFuncSel(P9_3, HS_Func0);
    Pad_HighSpeedFuncSel(P9_4, HS_Func0);
    Pad_HighSpeedFuncSel(P9_5, HS_Func0);

    Pad_HighSpeedMuxSel(P9_0, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(P9_1, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(P9_2, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(P9_3, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(P9_4, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(P9_5, FROM_CORE_DOMAIN);

    lcd_reset_init();
#if (TARGET_LCD_DEVICE == LCD_DEVICE_ST7801)
    lcd_vci_en_init();
#elif (TARGET_LCD_DEVICE == LCD_DEVICE_SH8601Z)
    lcd_avdd_en_init();
#endif

#if (ENABLE_TE_FOR_LCD == 1)
    /*TE Config*/
    /*TE Type Config*/
    rtk_lcd_hal_set_TE_type(LCDC_TE_TYPE_HW_TE);
    /*TE Pad Config, P2_2 -> Hardware TE*/
    if (rtk_lcd_hal_get_TE_type() == LCDC_TE_TYPE_HW_TE)
    {
        Pad_Config(P2_2, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_HIGH);
        Pad_HighSpeedMuxSel(P2_2, FROM_CORE_DOMAIN);
    }
    /*TE Pad Config, P4_5 -> Software TE*/
    else if (rtk_lcd_hal_get_TE_type() == LCDC_TE_TYPE_SW_TE)
    {
        lcd_te_device_init();
    }
#endif
}

void rtk_lcd_hal_init(void)
{
    lcd_pad_init();
    lcdc_driver_init();
#if (TARGET_LCD_DEVICE == LCD_DEVICE_SH8601Z)
    sh8601a_init();
#elif (TARGET_LCD_DEVICE == LCD_DEVICE_ST7801)
    st7801_init();
#endif
}

void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h)
{
    //APP_PRINT_TRACE2("rtk_lcd_hal_set_window w:%d, h:%d", w, h);
#if (TARGET_LCD_DEVICE == LCD_DEVICE_SH8601Z)
    lcd_SH8601A_qspi_454_set_window(xStart, yStart, xStart + w - 1, yStart + h - 1);
#elif (TARGET_LCD_DEVICE == LCD_DEVICE_ST7801)
    lcd_ST7801_qspi_368_448_set_window(xStart, yStart, xStart + w - 1, yStart + h - 1);
#endif
}

void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len)
{
    LCDC_DMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.LCDC_DMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceInc           = LCDC_DMA_SourceInc_Inc;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationInc      = LCDC_DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceDataSize      = LCDC_DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationDataSize = LCDC_DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceMsize         = LCDC_DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationMsize    = LCDC_DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = (uint32_t)buf;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En     = 0;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);

    LCDC_SetTxPixelLen(len);

    LCDC_Cmd(ENABLE);
    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
    LCDC_DmaCmd(ENABLE);
#if (ENABLE_TE_FOR_LCD == 1)
    LCDC_HANDLER_TEAR_CTR_TypeDef handler_reg_0x10 = {.d32 = LCDC_HANDLER->TEAR_CTR};
    handler_reg_0x10.b.bypass_t2w_delay = 0;
    handler_reg_0x10.b.t2w_delay = 0xfff;
    LCDC_HANDLER->TEAR_CTR = handler_reg_0x10.d32;
    LCDC_TeCmd(ENABLE);
#else
    LCDC_AutoWriteCmd(ENABLE);
#endif
    while ((LCDC_HANDLER->DMA_FIFO_CTRL & LCDC_DMA_ENABLE) != RESET)//wait dma finish
    {
        os_delay(1);
    }
    while (((LCDC_HANDLER->DMA_FIFO_OFFSET & LCDC_DMA_TX_FIFO_OFFSET) != RESET) &&
           (LCDC_HANDLER->TX_CNT == LCDC_HANDLER->TX_LEN));//wait lcd tx cnt finish
#if (ENABLE_TE_FOR_LCD == 1)
    LCDC_TeCmd(DISABLE);                            // disable Tear trigger auto_write_start
#endif
    LCDC_Cmd(DISABLE);
    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();
    LCDC_AXIMUXMode(LCDC_FW_MODE);
}

void rtk_lcd_hal_rect_fill(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h,
                           uint32_t color)
{
    rtk_lcd_hal_set_window(xStart, yStart, w, h);
    uint8_t *RGB_transfer = (uint8_t *)&color;
    uint32_t clear_buf[64] = {0};
#if INPUT_PIXEL_BYTES == 4
    uint32_t rgba8888 = RGB_transfer[3] << 24 | RGB_transfer[0] << 16 | RGB_transfer[1] << 8 |
                        RGB_transfer[2];
    for (int i = 0; i < 64; i++)
    {
        clear_buf[i] = rgba8888;
    }
#elif INPUT_PIXEL_BYTES == 3
    uint8_t *rgb888_buf = (uint8_t *)clear_buf;
    for (int i = 0; i < 64; i++)
    {
        rgb888_buf[i * 3] = RGB_transfer[0];
        rgb888_buf[i * 3 + 1] = RGB_transfer[1];
        rgb888_buf[i * 3 + 2] = RGB_transfer[2];
    }
#elif INPUT_PIXEL_BYTES == 2
    uint16_t rgb_color = 0;
    uint16_t *rgb565_buf = (uint16_t *)clear_buf;
    rgb_color = RGB_transfer[0] << 8 | RGB_transfer[1];
    for (int i = 0; i < 64 * 2; i++)
    {
        rgb565_buf[i] = rgb_color;
    }
#endif
    LCDC_DMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.LCDC_DMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceInc           = LCDC_DMA_SourceInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationInc      = LCDC_DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceDataSize      = LCDC_DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationDataSize = LCDC_DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceMsize         = LCDC_DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationMsize    = LCDC_DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = (uint32_t)clear_buf;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En     = 0;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);

    LCDC_SetTxPixelLen(w * h);

    LCDC_Cmd(ENABLE);

    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
    LCDC_DmaCmd(ENABLE);

    LCDC_AutoWriteCmd(ENABLE);

    while ((LCDC_HANDLER->DMA_FIFO_CTRL & LCDC_DMA_ENABLE) != RESET);//wait dma finish


    LCDC_HANDLER_OPERATE_CTR_t handler_reg_0x14;
    do
    {
        handler_reg_0x14.d32 = LCDC_HANDLER->OPERATE_CTR;
    }
    while (handler_reg_0x14.b.auto_write_start != RESET);

    LCDC_Cmd(DISABLE);
    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();
    LCDC_AXIMUXMode(LCDC_FW_MODE);
}

void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len)
{
    LCDC_DMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.LCDC_DMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.LCDC_DMA_DIR                 = LCDC_DMA_DIR_PeripheralToMemory;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceInc           = LCDC_DMA_SourceInc_Inc;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationInc      = LCDC_DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceDataSize      = LCDC_DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationDataSize = LCDC_DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceMsize         = LCDC_DMA_Msize_16;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationMsize    = LCDC_DMA_Msize_16;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = (uint32_t)buf;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En     = 0;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);
    LCDC_ForceBurst(ENABLE);
    LCDC_SetTxPixelLen(len);

    LCDC_Cmd(ENABLE);
    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);
    LCDC_DmaCmd(ENABLE);
#if (ENABLE_TE_FOR_LCD == 1)
    if (rtk_lcd_hal_get_TE_type() == LCDC_TE_TYPE_HW_TE)
    {
        LCDC_TeCmd(ENABLE);
    }
    else
    {
        LCDC_AutoWriteCmd(ENABLE);
    }
#endif
#if (ENABLE_TE_FOR_LCD == 0)
    LCDC_AutoWriteCmd(ENABLE);
#endif
}

void rtk_lcd_hal_transfer_done(void)
{
    LCDC_HANDLER_DMA_FIFO_CTRL_t handler_reg_0x18;
    do
    {
        handler_reg_0x18.d32 = LCDC_HANDLER->DMA_FIFO_CTRL;
    }
    while (handler_reg_0x18.b.dma_enable != RESET);

    LCDC_HANDLER_OPERATE_CTR_t handler_reg_0x14;
    LCDC_HANDLER_TX_LEN_TypeDef handler_reg_0x28;
    LCDC_HANDLER_TX_CNT_TypeDef handler_reg_0x2c;

    do
    {
        handler_reg_0x14.d32 = LCDC_HANDLER->OPERATE_CTR;
        handler_reg_0x28.d32 = LCDC_HANDLER->TX_LEN;
        handler_reg_0x2c.d32 = LCDC_HANDLER->TX_CNT;
    }
    while (handler_reg_0x14.b.auto_write_start != RESET &&
           handler_reg_0x2c.b.tx_output_pixel_cnt < handler_reg_0x28.b.tx_output_pixel_num);

#if (ENABLE_TE_FOR_LCD == 1)
    if (rtk_lcd_hal_get_TE_type() == LCDC_TE_TYPE_HW_TE)
    {
        LCDC_TeCmd(DISABLE);
    }
#endif

    LCDC_Cmd(DISABLE);
    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();
    LCDC_AXIMUXMode(LCDC_FW_MODE);
    DBIC_Cmd(DISABLE);

    DBIC_FLUSH_FIFO_TypeDef dbic_reg_0x128 = {.d32 = DBIC->FLUSH_FIFO};
    dbic_reg_0x128.b.flush_dr_fifo = 1;
    DBIC->FLUSH_FIFO = dbic_reg_0x128.d32;
}

uint32_t rtk_lcd_hal_get_width(void)
{
    return LCD_WIDTH;
}

uint32_t rtk_lcd_hal_get_height(void)
{
    return LCD_HIGHT;
}

uint32_t rtk_lcd_hal_get_pixel_bits(void)
{
    return RGB16BIT_565;
}
#endif
