#include <string.h>
#include "mem_config.h"
#include "rtl876x.h"
#include "trace.h"
#include "vector_table.h"

#define IMG_HEADER_SECTION      __attribute__((section(".image_entry")))

void Default_Handler(void);

const IRQ_Fun RamVectorTable_app[220] __attribute__((section(".vectors_table"))) =
{
    Default_Handler,           // Top of Stack
    Reset_Handler,             // Reset Handler
    NMI_Handler,               // NMI Handler
    HardFault_Handler,         // Hard Fault Handler
    MemManage_Handler,         // MPU Fault Handler
    BusFault_Handler,          // Bus Fault Handler
    UsageFault_Handler,        // Usage Fault Handler
    Default_Handler,           // Reserved
    Default_Handler,           // Reserved
    Default_Handler,           // Reserved
    Default_Handler,           // Reserved
    SVC_Handler,               // SVCall Handler
    DebugMon_Handler,          // Debug Monitor Handler
    Default_Handler,           // Reserved
    PendSV_Handler,            // PendSV Handler
    SysTick_Handler,           // SysTick Handler
    System_Handler,            //[0]  System On interrupt
    WDT_Handler,               //[1]  Watch dog global insterrupt
    BTMAC_Handler,             //[2]  See Below Table ( an Extension of interrupt )
    USB_IP_Handler,            //[3]  Master Peripheral IP interrupt (usb or sdio)
    TIM2_Handler,              //[4]  Timer2 global interrupt
    Platform_Handler,          //[5]  Platform interrupt (platfrom error interrupt)
    I2S0_TX_Handler,           //[6]  I2S0 interrupt
    I2S0_RX_Handler,           //[7]  I2S1 interrupt
    UART2_Handler,             //[8]  Data_Uart1 interrupt (used for DSP)
    GPIOA0_Handler,            //[9]  GPIOA0 interrupt
    GPIOA1_Handler,            //[10] GPIOA1 interrupt
    UART1_Handler,             //[11] Log_Uart interrupt (Log0)
    UART0_Handler,             //[12] Data_Uart interrupt
    RTC_Handler,               //[13] Realtime counter interrupt
    SPI0_Handler,              //[14] SPI0 interrupt
    SPI1_Handler,              //[15] SPI1 interrupt
    I2C0_Handler,              //[16] I2C0 interrupt
    I2C1_Handler,              //[17] I2C1 interrupt
    ADC_Handler,               //[18] ADC global interrupt
    Peripheral_Handler,        //[19] See Below Table ( an Extension of interrupt )
    GDMA0_Channel0_Handler,    //[20] RTK-DMA0 channel 0 global interrupt
    GDMA0_Channel1_Handler,    //[21] RTK-DMA0 channel 1 global interrupt
    GDMA0_Channel2_Handler,    //[22] RTK-DMA0 channel 2 global interrupt
    GDMA0_Channel3_Handler,    //[23] RTK-DMA0 channel 3 global interrupt
    GDMA0_Channel4_Handler,    //[24] RTK-DMA0 channel 4 global interrupt
    GDMA0_Channel5_Handler,    //[25] RTK-DMA0 channel 5 global interrupt
    KeyScan_Handler,           //[26] keyscan global interrupt
    QDEC_Handler,              //[27] qdecode global interrupt
    IR_Handler,                //[28] IR module global interrupt
    DSP_Handler,               //[29] DSP interrupt
    GDMA0_Channel6_Handler,    //[30] RTK-DMA0 channel 6 global interrupt
    GDMA0_Channel7_Handler,    //[31] RTK-DMA0 channel 7 global interrupt
    GDMA0_Channel8_Handler,    //[32] RTK-DMA0 channel 8 global interrupt
    GDMA0_Channel9_Handler,    //[33] RTK-DMA0 channel 9 global interrupt
    GDMA0_Channel10_Handler,   //[34] RTK-DMA0 channel 6 global interrupt
    GDMA0_Channel11_Handler,   //[35] RTK-DMA0 channel 7 global interrupt
    GDMA0_Channel12_Handler,   //[36] RTK-DMA0 channel 8 global interrupt
    GDMA0_Channel13_Handler,   //[37] RTK-DMA0 channel 9 global interrupt
    GDMA0_Channel14_Handler,   //[38] RTK-DMA0 channel 6 global interrupt
    GDMA0_Channel15_Handler,   //[39] RTK-DMA0 channel 7 global interrupt
    TIM3_Handler,              //[40] Timer3 global interrupt
    TIM4_Handler,              //[41] Timer4 global interrupt
    TIM5_Handler,              //[42] Timer5 global interrupt
    TIM6_Handler,              //[43] Timer6 global interrupt
    TIM7_Handler,              //[44] Timer7 global interrupt
    TIM8to11_Handler,          //[45] TRNG interrupt
    GPIO_A_9_Handler,          //[46] GPIO_A[9] interrupt
    GPIO_A_16_Handler,         //[47] GPIO_A[16] interrupt
    GPIO_A_2to7_Handler,       //[48] GPIO_A[2:7] interrupt
    GPIO_A_8_10to15_Handler,   //[49] GPIO_A[8,10:15] interrupt
    GPIO_A_17to23_Handler,     //[50] GPIO_A[17:23] interrupt
    GPIO_A_24to31_Handler,     //[51] GPIO_A[24:31] interrupt
    GPIO_B_0to7_Handler,       //[52] GPIO_B[0:7] interrupt
    GPIO_B_8to15_Handler,      //[53] GPIO_B[8:15] interrupt
    GPIO_B_16to23_Handler,     //[54] GPIO_B[16:23] interrupt
    GPIO_B_24to31_Handler,     //[55] GPIO_B[24:31] interrupt
    UART3_Handler,             //[56] Uart3 interrupt(used for DSP)
    SPI2_Handler,              //[57] SPI2 interrupt
    I2C2_Handler,              //[58] I2C2 interrupt
    DSP_Event_1_Handler,       //[59] DSP event interrupt 1
    DSP_Event_2_Handler,       //[60] DSP event interrupt 2
    SHA256_Handler,            //[61] SHA256 interrupt
    USB_ISOC_Handler,          //[62] USB ISO interrupt
    BT_CLK_compare_Handler,    //[63] BT clock compare interrupt
    HW_RSA_Handler,            //[64] HW RSA interrupt
    PSRAM_Handler,             //[65] PSRAM interrupt
    Compare_clk_4_Handler,     //[66] Compare Clock interrupt 4
    Compare_clk_5_Handler,     //[67] Compare Clock interrupt 5
    dspdma_int_all_host_Handler,    //[68] RTK-DMA0 channel 0 global interrupt
    NNA_Handler,               //[69] RTK-DMA0 channel 1 global interrupt
    Compare_clk_9_Handler,     //[70] RTK-DMA0 channel 2 global interrupt
    Compare_clk_10_Handler,    //[71] RTK-DMA0 channel 3 global interrupt
    GPIO_C_0to7_Handler,       //[72] RTK-DMA0 channel 4 global interrupt
    GPIO_C_8to15_Handler,      //[73] RTK-DMA0 channel 5 global interrupt
    Default_Handler,           //[74] RTK-DMA0 channel 6 global interrupt
    Default_Handler,           //[75] RTK-DMA0 channel 7 global interrupt
    TIM8_Handler,              //[76] Timer8 global interrupt
    TIM9_Handler,              //[77] Timer9 global interrupt
    TIM10_Handler,             //[78] Timer10 global interrupt
    TIM11_Handler,             //[79] Timer11 global interrupt
    TIM12_Handler,             //[80] Timer12 global interrupt
    TIM13_Handler,             //[81] Timer13 global interrupt
    TIM14_Handler,             //[82] Timer14 global interrupt
    TIM15_Handler,             //[83] Timer15 global interrupt
    SPIC0_Handler,             //[84] SPIC0 interrupt
    SPIC1_Handler,             //[85] SPIC1 interrupt
    SPIC2_Handler,             //[86] SPIC2 interrupt
    SPIC3_Handler,             //[87] SPIC3 interrupt
    SDIO_HOST_Handler,         //[88] SDIO host interrupt
    SPIC4_Handler,
    ERR_CORR_CODE_Handler,
    SLAVE_PORT_MONITOR_Handler,
    RESERVED0_Handler,         //19,      0,
    RESERVED1_Handler,         //19,      1,
    RESERVED2_Handler,         //19,      2,
    RESERVED3_Handler,         //19,      3,
    RESERVED4_Handler,         //19,      4,
    RESERVED5_Handler,         //19,      5,
    RESERVED6_Handler,         //19,      6,
    SPDIF_Rx_Handler,          //19,      7,
    SPDIF_Tx_Handler,          //19,      8,
    Default_Handler,           //19,      9,
    SPI2W_Handler,             //19,      10,
    LPCOMP_Handler,            //19,      11,
    MBIAS_VBAT_DET_Handler,    //19,      12,
    MBIAS_ADP_DET_Handler,     //19,      13,
    HW_ASRC1_Handler,          //19,      14,
    HW_ASRC2_Handler,          //19,      15,
    VADBUF_Handler,            //19,      16,
    PTA_Mailbox_Handler,       //19,      17,
    DSP2MCU_Handler,           //19,      18,
    SPORT2_TX_Handler,         //19,      19,
    SPORT2_RX_Handler,         //19,      20,
    SPORT3_TX_Handler,         //19,      21,
    SPORT3_RX_Handler,         //19,      22,
    VAD_Handler,               //19,      23,
    ANC_Handler,               //19,      24,
    SPORT0_TX_Handler,         //19,      25,
    SPORT0_RX_Handler,         //19,      26,
    SPORT1_TX_Handler,         //19,      27,
    SPORT1_RX_Handler,         //19,      28,
    USB_UTMI_SUSPEND_N_Handler,//19,      29,
    USB_DLPS_Resume_Handler,   //19,      30,
    RESERVED7_Handler,         //19,      31,
    ADP_IN_DET_Handler,
    ADP_OUT_DET_Handler,
    GPIOA2_Handler,
    GPIOA3_Handler,
    GPIOA4_Handler,
    GPIOA5_Handler,
    GPIOA6_Handler,
    GPIOA7_Handler,
    GPIOA8_Handler,
    GPIOA9_Handler,
    GPIOA10_Handler,
    GPIOA11_Handler,
    GPIOA12_Handler,
    GPIOA13_Handler,
    GPIOA14_Handler,
    GPIOA15_Handler,
    GPIOA16_Handler,
    GPIOA17_Handler,
    GPIOA18_Handler,
    GPIOA19_Handler,
    GPIOA20_Handler,
    GPIOA21_Handler,
    GPIOA22_Handler,
    GPIOA23_Handler,
    GPIOA24_Handler,
    GPIOA25_Handler,
    GPIOA26_Handler,
    GPIOA27_Handler,
    GPIOA28_Handler,
    GPIOA29_Handler,
    GPIOA30_Handler,
    GPIOA31_Handler,
    GPIOB0_Handler,
    GPIOB1_Handler,
    GPIOB2_Handler,
    GPIOB3_Handler,
    GPIOB4_Handler,
    GPIOB5_Handler,
    GPIOB6_Handler,
    GPIOB7_Handler,
    GPIOB8_Handler,
    GPIOB9_Handler,
    GPIOB10_Handler,
    GPIOB11_Handler,
    GPIOB12_Handler,
    GPIOB13_Handler,
    GPIOB14_Handler,
    GPIOB15_Handler,
    GPIOB16_Handler,
    GPIOB17_Handler,
    GPIOB18_Handler,
    GPIOB19_Handler,
    GPIOB20_Handler,
    GPIOB21_Handler,
    GPIOB22_Handler,
    GPIOB23_Handler,
    GPIOB24_Handler,
    GPIOB25_Handler,
    GPIOB26_Handler,
    GPIOB27_Handler,
    GPIOB28_Handler,
    GPIOB29_Handler,
    GPIOB30_Handler,
    GPIOB31_Handler,
    GPIOC0_Handler,
    GPIOC1_Handler,
    GPIOC2_Handler,
    GPIOC3_Handler,
    GPIOC4_Handler,
    GPIOC5_Handler,
    GPIOC6_Handler,
    GPIOC7_Handler,
    GPIOC8_Handler,
    GPIOC9_Handler,
    GPIOC10_Handler,
    GPIOC11_Handler,
    GPIOC12_Handler,
    GPIOC13_Handler,
    GPIOC14_Handler,
    GPIOC15_Handler,
};

__attribute__((naked)) void __user_setup_stackheap(void)
{
    __asm volatile(
        "BX      lr                        \n"
    );

}

extern void system_init(void);
extern int main(void);

IMG_HEADER_SECTION void Reset_Handler(void)
{
    /* copy data on rw*/
    extern uint32_t Load$$RAM_GLOBAL$$RW$$Base;
    extern uint32_t Image$$RAM_GLOBAL$$RW$$Base;
    extern uint32_t Image$$RAM_GLOBAL$$RW$$Length;
    memcpy((void *) &Image$$RAM_GLOBAL$$RW$$Base, (void *) &Load$$RAM_GLOBAL$$RW$$Base,
           (uint32_t)&Image$$RAM_GLOBAL$$RW$$Length);

    /* clear data on zi */
    extern uint32_t Image$$RAM_GLOBAL$$ZI$$Base;
    extern uint32_t Image$$RAM_GLOBAL$$ZI$$Length;
    memset((uint8_t *)&Image$$RAM_GLOBAL$$ZI$$Base, 0, (uint32_t)&Image$$RAM_GLOBAL$$ZI$$Length);

    /* copy buffer on rw */
    extern uint32_t Load$$RAM_TEXT$$RW$$Base;
    extern uint32_t Image$$RAM_TEXT$$RW$$Base;
    extern uint32_t Image$$RAM_TEXT$$RW$$Length;
    memcpy((void *) &Image$$RAM_TEXT$$RW$$Base, (void *) &Load$$RAM_TEXT$$RW$$Base,
           (uint32_t)&Image$$RAM_TEXT$$RW$$Length);

    /* copy buffer on ro (for ram function) */
    extern uint32_t Load$$RAM_TEXT$$RO$$Base;
    extern uint32_t Image$$RAM_TEXT$$RO$$Base;
    extern uint32_t Image$$RAM_TEXT$$RO$$Length;
    memcpy((void *) &Image$$RAM_TEXT$$RO$$Base, (void *) &Load$$RAM_TEXT$$RO$$Base,
           (uint32_t)&Image$$RAM_TEXT$$RO$$Length);

    /* clear buffer on zi */
    extern uint32_t Image$$RAM_TEXT$$ZI$$Base;
    extern uint32_t Image$$RAM_TEXT$$ZI$$Length;
    memset((uint8_t *)&Image$$RAM_TEXT$$ZI$$Base, 0, (uint32_t)&Image$$RAM_TEXT$$ZI$$Length);

    system_init();
    main();
}

/*----------------------------------------------------------------------------
  Default Handler for Exceptions / Interrupts
 *----------------------------------------------------------------------------*/
void Default_Handler(void)
{
    DBG_DIRECT("Error! Please implement your ISR Handler for IRQ number %d!", __get_IPSR());
    while (1);
}
