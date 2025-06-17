#include "app_gui.h"
#if (TARGET_LCD_DEVICE == LCD_DEVICE_LCDC_A0500)
#include "lcd_a0500_800480_rgb.h"
#include "rtl_lcdc_edpi.h"
#include "rtl_lcdc.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_gpio.h"
#include "rtl876x_rcc.h"
//#include "utils.h"
#include "mem_config.h"
//#include "string.h"
#include "rtl876x_gdma.h"
#include "trace.h"
#include "platform_utils.h"

#define LCDC_DATA23         P9_6
#define LCDC_DATA22         P9_5
#define LCDC_DATA21         P9_4
#define LCDC_DATA20         P9_3
#define LCDC_DATA19         P9_2
#define LCDC_DATA18         P9_1
#define LCDC_DATA17         P8_7
#define LCDC_DATA16         P8_6

#define LCDC_DATA15         P8_5
#define LCDC_DATA14         P8_4
#define LCDC_DATA13         P8_3
#define LCDC_DATA12         P8_2
#define LCDC_DATA11         P8_1
#define LCDC_DATA10         P8_0
#define LCDC_DATA9          P4_7
#define LCDC_DATA8          P4_6

#define LCDC_DATA7          P4_5
#define LCDC_DATA6          P4_4
#define LCDC_DATA5          P4_3
#define LCDC_DATA4          P4_2
#define LCDC_DATA3          P4_1
#define LCDC_DATA2          P4_0
#define LCDC_DATA1          P2_7
#define LCDC_DATA0          P2_6

#define LCDC_RGB_WRCLK      P2_5
#define LCDC_HSYNC          P2_4
#define LCDC_CSN_DE         P2_3
#define LCDC_VSYNC          P2_2

#define RGB_BACKLIGHT       P6_0



//#define GPD0                P2_5
//#define GPD1                P2_7
//#define GPD2                P9_0
//#define GPD3                P2_6

#define LCDC_RESET          P9_0

#define LCDC_DMA_CHANNEL_NUM              0
#define LCDC_DMA_CHANNEL_INDEX            LCDC_DMA_Channel0

#define TRANSFER_DMA_CHANNEL_NUM              0
#define TRANSFER_DMA_CHANNEL_INDEX            GDMA_Channel0

//#define PSRAM_FRAME_BUF1_ADDR 0x4000000

static void a0500_reset_high(void)
{
//    GPIO_WriteBit(GPIO_GetPin(LCDC_RESET), GPIO_GetPin(LCDC_RESET), 1);
    Pad_Config(LCDC_RESET, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
}

static void a0500_reset_low(void)
{
//    GPIO_WriteBit(GPIO_GetPin(LCDC_RESET), GPIO_GetPin(LCDC_RESET), 0);
    Pad_Config(LCDC_RESET, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
}
void rtk_lcd_hal_set_reset(bool reset)
{
    if (reset)
    {
        a0500_reset_low();
    }
    else
    {
        a0500_reset_high();
    }
}
//--------------------initial code-----------------------------------------//
static void a0500_gpio_init(void)
{

    Pad_Config(LCDC_RESET, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pinmux_Config(LCDC_RESET, DWGPIO);
    RCC_PeriphClockCmd(APBPeriph_GPIOA, APBPeriph_GPIOA_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_GPIOB, APBPeriph_GPIOB_CLOCK, ENABLE);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_PinBit    = GPIO_GetPin(LCDC_RESET);
    GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_OUT;
    GPIO_InitStruct.GPIO_ITCmd  = DISABLE;
//    GPIOx_Init(GPIO_GetPin(LCDC_RESET), &GPIO_InitStruct);//error xuyaochayan
    a0500_reset_high();

}

static void a0500_dma_init(uint8_t *init_buffer)
{
    LCDC_DMA_InitTypeDef LCDC_DMA_InitStruct = {0};
    LCDC_DMA_StructInit(&LCDC_DMA_InitStruct);
    LCDC_DMA_InitStruct.LCDC_DMA_ChannelNum          = LCDC_DMA_CHANNEL_NUM;
    LCDC_DMA_InitStruct.LCDC_DMA_DIR                 = LCDC_DMA_DIR_PeripheralToMemory;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceInc           = LCDC_DMA_SourceInc_Inc;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationInc      = LCDC_DMA_DestinationInc_Fix;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceDataSize      = LCDC_DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationDataSize = LCDC_DMA_DataSize_Word;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceMsize         = LCDC_DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_DestinationMsize    = LCDC_DMA_Msize_8;
    LCDC_DMA_InitStruct.LCDC_DMA_SourceAddr          = 0;

    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_Mode   =
        LLI_TRANSFER;//LLI_TRANSFER or LLI_WITH_CONTIGUOUS_SAR
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_En     = 1;
    LCDC_DMA_InitStruct.LCDC_DMA_Multi_Block_Struct  = LCDC_DMA_LINKLIST_REG_BASE + 0x50;
    LCDC_DMA_Init(LCDC_DMA_CHANNEL_INDEX, &LCDC_DMA_InitStruct);

    LCDC_SET_GROUP1_BLOCKSIZE(A0500_800480_LCD_WIDTH * A0500_DRV_PIXEL_BITS / 8);
    LCDC_SET_GROUP2_BLOCKSIZE(A0500_800480_LCD_WIDTH * A0500_DRV_PIXEL_BITS / 8);

    LCDC_DMALLI_InitTypeDef LCDC_DMA_LLI_Init = {0};

    LCDC_DMA_LLI_Init.g1_source_addr = (uint32_t)init_buffer;
    LCDC_DMA_LLI_Init.g2_source_addr = (uint32_t)((uint32_t)init_buffer + A0500_800480_LCD_WIDTH *
                                                  A0500_DRV_PIXEL_BITS / 8);
    LCDC_DMA_LLI_Init.g1_sar_offset = A0500_800480_LCD_WIDTH * A0500_DRV_PIXEL_BITS / 8 * 2;
    LCDC_DMA_LLI_Init.g2_sar_offset = A0500_800480_LCD_WIDTH * A0500_DRV_PIXEL_BITS / 8 * 2;

    LCDC_DMA_Infinite_Buf_Update((uint8_t *)init_buffer,
                                 (uint8_t *)init_buffer + A0500_800480_LCD_WIDTH *
                                 A0500_DRV_PIXEL_BITS / 8);
    LCDC_DMA_LinkList_Init(&LCDC_DMA_LLI_Init,
                           &LCDC_DMA_InitStruct);//LLI_TRANSFER or LLI_WITH_CONTIGUOUS_SAR

    LCDC_ClearDmaFifo();
    LCDC_ClearTxPixelCnt();

    LCDC_SwitchMode(LCDC_AUTO_MODE);
    LCDC_SwitchDirect(LCDC_TX_MODE);

    LCDC_SetTxPixelLen(A0500_800480_LCD_WIDTH * A0500_800480_LCD_HEIGHT);
#if LV_USE_GPU_RTK_PPE
    LCDC_ForceBurst(ENABLE);
#endif
    LCDC_ForceBurst(ENABLE);
    LCDC_Cmd(ENABLE);

    LCDC_DMA_SetSourceAddress(LCDC_DMA_CHANNEL_INDEX, (uint32_t)init_buffer);

    LCDC_DMA_MultiBlockCmd(ENABLE);

    LCDC_DMAChannelCmd(LCDC_DMA_CHANNEL_NUM, ENABLE);

    LCDC_DmaCmd(ENABLE);
    LCDC_AutoWriteCmd(ENABLE);
}

static void lcd_pad_and_clk_init(void)
{
    LCDC_Clock_Cfg(ENABLE);
    RCC_PeriphClockCmd(APBPeriph_DISP, APBPeriph_DISP_CLOCK, ENABLE);
    //from XTAL SOURCE = 40M
//    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.disp_ck_en = 1;
//    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.disp_func_en = 1;
//    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.r_disp_mux_clk_cg_en = 1;

//    //From PLL1, SOURCE = 100M
//    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.r_disp_div_en = 1;
//    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.r_disp_clk_src_sel0 = 0; //pll1_peri(0) or pll2(1, pll2 = 160M)
//    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.r_disp_clk_src_sel1 = 1; //pll(1) or xtal(0)
//    PERIBLKCTRL_PERI_CLK->u_324.BITS_324.r_disp_div_sel = 0; //div

    Pad_Config(LCDC_DATA0, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA1, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA2, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA3, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA4, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA5, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA6, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA7, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA8, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA9, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA10, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA11, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA12, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA13, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA14, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA15, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA16, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA17, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA18, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA19, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA20, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA21, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA22, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_DATA23, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);

    Pad_Config(LCDC_RGB_WRCLK, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE,
               PAD_OUT_HIGH);
    Pad_Config(LCDC_HSYNC, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_VSYNC, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    Pad_Config(LCDC_CSN_DE, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);



    Pad_HighSpeedFuncSel(LCDC_DATA0, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA1, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA2, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA3, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA4, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA5, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA6, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA7, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA8, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA9, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA10, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA11, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA12, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA13, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA14, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA15, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA16, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA17, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA18, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA19, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA20, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA21, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA22, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_DATA23, HS_Func0);

    Pad_HighSpeedFuncSel(LCDC_RGB_WRCLK, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_HSYNC, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_VSYNC, HS_Func0);
    Pad_HighSpeedFuncSel(LCDC_CSN_DE, HS_Func0);

    Pad_HighSpeedMuxSel(LCDC_DATA0, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA1, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA2, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA3, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA4, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA5, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA6, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA7, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA8, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA9, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA10, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA11, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA12, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA13, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA14, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA15, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA16, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA17, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA18, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA19, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA20, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA21, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA22, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_DATA23, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_RGB_WRCLK, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_HSYNC, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_VSYNC, FROM_CORE_DOMAIN);
    Pad_HighSpeedMuxSel(LCDC_CSN_DE, FROM_CORE_DOMAIN);

    a0500_gpio_init();
    platform_delay_ms(1);
}

void rtk_lcd_hal_init(void)
{
    lcd_pad_and_clk_init();
    LCDC_InitTypeDef lcdc_init = {0};
    lcdc_init.LCDC_Interface = LCDC_IF_DPI;
    lcdc_init.LCDC_PixelInputFarmat = LCDC_INPUT_RGB565;
    lcdc_init.LCDC_PixelOutpuFarmat = LCDC_OUTPUT_RGB888;
    lcdc_init.LCDC_PixelBitSwap = LCDC_SWAP_BYPASS; //lcdc_handler_cfg->LCDC_TeEn = LCDC_TE_DISABLE;
    lcdc_init.LCDC_GroupSel = 0;

    lcdc_init.LCDC_DmaThreshold =
        112;  //only support threshold = 8 for DMA MSIZE = 8; the other threshold setting will be support later
    lcdc_init.LCDC_InfiniteModeEn = 1;
    LCDC_Init(&lcdc_init);
    RCC_DisplayClockConfig(DISPLAY_CLOCK_SOURCE_PLL1, DISPLAY_CLOCK_DIV_1);//PLL1:200M


    uint32_t HSA = 24, HFP = 112, HBP = 88, HACT = A0500_800480_LCD_WIDTH;
    uint32_t VSA = 2, VFP = 21, VBP = 39, VACT = A0500_800480_LCD_HEIGHT;

    LCDC_eDPICfgTypeDef eDPICfg = {0};//480*640  ---->   500 * 660
    eDPICfg.eDPI_ClockDiv = 6;


    eDPICfg.eDPI_HoriSyncWidth = HSA;
    eDPICfg.eDPI_VeriSyncHeight = VSA;
    eDPICfg.eDPI_AccumulatedHBP = HSA + HBP;
    eDPICfg.eDPI_AccumulatedVBP = VSA + VBP;
    eDPICfg.eDPI_AccumulatedActiveW = HSA + HBP + HACT;
    eDPICfg.eDPI_AccumulatedActiveH = VSA + VBP + VACT;
    eDPICfg.eDPI_TotalWidth = HSA + HBP + HACT + HFP;
    eDPICfg.eDPI_TotalHeight = VSA + VBP + VACT + VFP;
    eDPICfg.eDPI_HoriSyncPolarity = 0;
    eDPICfg.eDPI_VeriSyncPolarity = 0;
    eDPICfg.eDPI_DataEnPolarity = 1;
    eDPICfg.eDPI_LineIntMask = 1;
    eDPICfg.eDPI_ColorMap = EDPI_PIXELFORMAT_RGB888;//for RGB888
    eDPICfg.eDPI_OperateMode = 0;//video mode
    eDPICfg.eDPI_LcdArc = 0;
    eDPICfg.eDPI_ShutdnPolarity = 0;
    eDPICfg.eDPI_ColorModePolarity = 0;
    eDPICfg.eDPI_ShutdnEn = 0;
    eDPICfg.eDPI_ColorModeEn = 0;
    eDPICfg.eDPI_UpdateCfgEn = 0;
    eDPICfg.eDPI_TearReq = 0;
    eDPICfg.eDPI_Halt = 0;
    eDPICfg.eDPI_CmdMaxLatency = 0;//todo
    eDPICfg.eDPI_LineBufferPixelThreshold = eDPICfg.eDPI_TotalWidth / 2;

    EDPI_Init(&eDPICfg);
//    EDPI_RGB_COMPATIBLE_t edpi_reg_0x58 = {.d32 = EDPI->EDPI_RGB_COMPATIBLE};
//    edpi_reg_0x58.b.hsync_constant = EDPI_HSYNC_CONSTANT_HIGH;
//    edpi_reg_0x58.b.vsync_constant = EDPI_VSYNC_CONSTANT_HIGH;
//    edpi_reg_0x58.b.de_constant = EDPI_DE_CONSTANT_NONE;
//    EDPI->EDPI_RGB_COMPATIBLE = edpi_reg_0x58.d32;

//    EDPI_RGB_COMPATIBLE_t edpi_reg_0x58 = {.d32 = EDPI->EDPI_RGB_COMPATIBLE};
//    edpi_reg_0x58.b.hsync_constant = EDPI_HSYNC_CONSTANT_NONE;
//    edpi_reg_0x58.b.vsync_constant = EDPI_VSYNC_CONSTANT_NONE;
//    edpi_reg_0x58.b.de_constant = EDPI_DE_CONSTANT_LOW;
//    EDPI->EDPI_RGB_COMPATIBLE = edpi_reg_0x58.d32;

    a0500_reset_high();
    platform_delay_ms(200);
    a0500_reset_low();
    platform_delay_ms(200);
    a0500_reset_high();
    platform_delay_ms(200);
    Pad_Config(RGB_BACKLIGHT, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    Pad_Config(P2_1, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    //*******************************/
    uint8_t *pixel = (uint8_t *)PSRAM_FRAME_BUF1_ADDR;
#if A0500_DRV_PIXEL_BITS == 24
    for (uint32_t i = 0; i < A0500_800480_LCD_WIDTH * A0500_800480_LCD_HEIGHT * 3; i = i + 3)
    {
        pixel[i] = 0xFF;
        pixel[i + 1] = 0;
        pixel[i + 2] = 0;
    }
#endif
#if A0500_DRV_PIXEL_BITS == 16
    for (uint32_t i = 0; i < A0500_800480_LCD_WIDTH * A0500_800480_LCD_HEIGHT * 2; i = i + 2)
    {
        pixel[i] = 0x1f;
        pixel[i + 1] = 0x00;
    }
#endif
//    DBG_DIRECT("pixel[0]:0x%x,pixel[100]:0x%x,pixel[1000]:0x%x",pixel[0],pixel[100],pixel[1000]);
    rtk_lcd_hal_update_framebuffer((uint8_t *)PSRAM_FRAME_BUF1_ADDR,
                                   A0500_800480_LCD_WIDTH * A0500_800480_LCD_HEIGHT);
}
static bool flush_first = true;

void rtk_lcd_hal_update_framebuffer(uint8_t *buf, uint32_t len)
{
    if (flush_first == true)
    {
        a0500_dma_init(buf);
        flush_first = false;
    }
    else
    {
        LCDC_DMA_Infinite_Buf_Update(buf,
                                     (uint8_t *)buf + A0500_800480_LCD_WIDTH * A0500_DRV_PIXEL_BITS / 8);
    }
}

void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len)
{
    return;//todo by howie
}

void rtk_lcd_hal_transfer_done(void)
{
    return;//todo by howie
}

void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h)
{
    return;//todo by howie
}

uint32_t rtk_lcd_hal_get_width(void)
{
    return A0500_800480_LCD_WIDTH;
}

uint32_t rtk_lcd_hal_get_height(void)
{
    return A0500_800480_LCD_HEIGHT;
}

uint32_t rtk_lcd_hal_get_pixel_bits(void)
{
    return A0500_DRV_PIXEL_BITS;
}

bool rtk_lcd_hal_power_off(void)
{
//    flush_first = true;
    return 0;
}

bool rtk_lcd_hal_power_on(void)
{
//    rtk_lcd_hal_init();
    return 0;
}

bool rtk_lcd_hal_dlps_check(void)
{
    return true;
}

bool rtk_lcd_wake_up(void)
{
    return 0;
}

uint32_t rtk_lcd_hal_dlps_restore(void)
{
    return 0;
}

void rtk_lcd_dlps_init(void)
{

}
void rtk_lcd_hal_lcd_enter_dlps(void)
{
    /*enter dlps*/

    /*do nothing*/
}
#endif
