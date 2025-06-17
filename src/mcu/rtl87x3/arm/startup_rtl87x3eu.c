#include <string.h>
#include "mem_config.h"
#include "trace.h"
#include "vector_table.h"

#define IMG_HEADER_SECTION      __attribute__((section(".image_entry")))

void Default_Handler(void);


//#if 0//defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)  || defined ( __GNUC__ )
//void Reset_Handler                (void) __attribute__ ((weak, alias("Default_Handler")));
void NMI_Handler(void) __attribute__((weak, alias("Default_Handler")));
void HardFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void MemManageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void BusFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void UsageFault_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SVC_Handler(void) __attribute__((weak, alias("Default_Handler")));
//void Debug_Monitor_Handler(void) __attribute__((weak, alias("Default_Handler")));
void PendSV_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SysTick_Handler(void) __attribute__((weak, alias("Default_Handler")));
void SYSTEM_ISR_ISR(void) __attribute__((weak, alias("Default_Handler")));
//void WDT_ISR(void) __attribute__((weak, alias("Default_Handler")));
void BTMAC_ISR(void) __attribute__((weak, alias("Default_Handler")));
void DSP_ISR(void) __attribute__((weak, alias("Default_Handler")));
//void RXI300_ISR_ISR(void) __attribute__((weak, alias("Default_Handler")));
void SPI0_ISR(void) __attribute__((weak, alias("Default_Handler")));
void I2C0_ISR(void) __attribute__((weak, alias("Default_Handler")));
void ADC_ISR(void) __attribute__((weak, alias("Default_Handler")));
void SPORT0_RX_ISR(void) __attribute__((weak, alias("Default_Handler")));
void SPORT0_TX_ISR(void) __attribute__((weak, alias("Default_Handler")));
void TIMER1_CH0_ISR(void) __attribute__((weak, alias("Default_Handler")));
void TIMER1_CH1_ISR(void) __attribute__((weak, alias("Default_Handler")));
void TIMER1_CH2_ISR(void) __attribute__((weak, alias("Default_Handler")));
void RTC_ISR(void) __attribute__((weak, alias("Default_Handler")));
void UART0_ISR(void) __attribute__((weak, alias("Default_Handler")));
void UART1_ISR(void) __attribute__((weak, alias("Default_Handler")));
void UART2_ISR(void) __attribute__((weak, alias("Default_Handler")));
void Peripheral_ISR_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GPIO_A0_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GPIO_A1_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GPIO_A2_7_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GPIO_A8_15_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GPIO_A16_23_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GPIO_A24_31_ISR(void) __attribute__((weak, alias("Default_Handler")));
void SPORT1_RX_ISR(void) __attribute__((weak, alias("Default_Handler")));
void SPORT1_TX_ISR(void) __attribute__((weak, alias("Default_Handler")));
void ADP_DET_ISR(void) __attribute__((weak, alias("Default_Handler")));
void VBAT_DET_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GDMA0_Channel0_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GDMA0_Channel1_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GDMA0_Channel2_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GDMA0_Channel3_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GDMA0_Channel4_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GDMA0_Channel5_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GDMA0_Channel6_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GDMA0_Channel7_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GDMA0_Channel8_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GPIO_B0_7_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GPIO_B8_15_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GPIO_B16_23_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GPIO_B24_31_ISR(void) __attribute__((weak, alias("Default_Handler")));
void SPI1_ISR(void) __attribute__((weak, alias("Default_Handler")));
void SPI2_ISR(void) __attribute__((weak, alias("Default_Handler")));
void I2C1_ISR(void) __attribute__((weak, alias("Default_Handler")));
void I2C2_ISR(void) __attribute__((weak, alias("Default_Handler")));
void KEYSCAN_ISR(void) __attribute__((weak, alias("Default_Handler")));
void QDECODE_ISR(void) __attribute__((weak, alias("Default_Handler")));
void UART3_ISR(void) __attribute__((weak, alias("Default_Handler")));
void UART4_ISR(void) __attribute__((weak, alias("Default_Handler")));
void UART5_ISR(void) __attribute__((weak, alias("Default_Handler")));
void SPIC0_ISR(void) __attribute__((weak, alias("Default_Handler")));
void SPIC1_ISR(void) __attribute__((weak, alias("Default_Handler")));
void SPIC2_ISR(void) __attribute__((weak, alias("Default_Handler")));
void TIMER1_CH3_ISR(void) __attribute__((weak, alias("Default_Handler")));
void TIMER1_CH4_ISR(void) __attribute__((weak, alias("Default_Handler")));
void TIMER1_CH5_ISR(void) __attribute__((weak, alias("Default_Handler")));
void ASRC0_ISR(void) __attribute__((weak, alias("Default_Handler")));
void ASRC1_ISR(void) __attribute__((weak, alias("Default_Handler")));
void ETHERNET_ISR(void) __attribute__((weak, alias("Default_Handler")));
void ISO7816_ISR(void) __attribute__((weak, alias("Default_Handler")));
void SDIO0_ISR(void) __attribute__((weak, alias("Default_Handler")));
void SPORT2_RX_ISR(void) __attribute__((weak, alias("Default_Handler")));
void Zigbee_ISR(void) __attribute__((weak, alias("Default_Handler")));
void MJPEG_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GDMA0_Channel9_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GDMA0_Channel10_ISR(void) __attribute__((weak, alias("Default_Handler")));
void GDMA0_Channel11_ISR(void) __attribute__((weak, alias("Default_Handler")));
void Display_ISR(void) __attribute__((weak, alias("Default_Handler")));
void PPE_ISR(void) __attribute__((weak, alias("Default_Handler")));
void RTZIP_ISR(void) __attribute__((weak, alias("Default_Handler")));
void SLAVE_PORT_MONITOR_ISR(void) __attribute__((weak, alias("Default_Handler")));
void TIMER1_CH6_ISR(void) __attribute__((weak, alias("Default_Handler")));
void TIMER1_CH7_ISR(void) __attribute__((weak, alias("Default_Handler")));
void TIMER1_CH8_ISR(void) __attribute__((weak, alias("Default_Handler")));
void TIMER1_CH9_ISR(void) __attribute__((weak, alias("Default_Handler")));
void CAN0_ISR(void) __attribute__((weak, alias("Default_Handler")));
void BTMAC_WRAP_AROUND_ISR(void) __attribute__((weak, alias("Default_Handler")));
void PUBLIC_KEY_ENGINE_ISR(void) __attribute__((weak, alias("Default_Handler")));
void MFB_DET_L_ISR(void) __attribute__((weak, alias("Default_Handler")));
void spi_phy1_intr_ISR(void) __attribute__((weak, alias("Default_Handler")));
void CAN1_ISR(void) __attribute__((weak, alias("Default_Handler")));
void USB_ISR(void) __attribute__((weak, alias("Default_Handler")));
void ENDP_MULTI_PROC_INT_ISR(void) __attribute__((weak, alias("Default_Handler")));
void SUSPEND_N_INT_ISR(void) __attribute__((weak, alias("Default_Handler")));
void USB_IN_EP_INT_0_ISR(void) __attribute__((weak, alias("Default_Handler")));
void USB_IN_EP_INT_1_ISR(void) __attribute__((weak, alias("Default_Handler")));
void USB_IN_EP_INT_2_ISR(void) __attribute__((weak, alias("Default_Handler")));
void USB_IN_EP_INT_3_ISR(void) __attribute__((weak, alias("Default_Handler")));
void USB_IN_EP_INT_4_ISR(void) __attribute__((weak, alias("Default_Handler")));
void USB_IN_EP_INT_5_ISR(void) __attribute__((weak, alias("Default_Handler")));
void USB_IN_EP_INT_6_ISR(void) __attribute__((weak, alias("Default_Handler")));
void USB_OUT_EP_INT_0_ISR(void) __attribute__((weak, alias("Default_Handler")));
void USB_OUT_EP_INT_1_ISR(void) __attribute__((weak, alias("Default_Handler")));
void USB_OUT_EP_INT_2_ISR(void) __attribute__((weak, alias("Default_Handler")));
void USB_OUT_EP_INT_3_ISR(void) __attribute__((weak, alias("Default_Handler")));
void USB_OUT_EP_INT_4_ISR(void) __attribute__((weak, alias("Default_Handler")));
void USB_OUT_EP_INT_5_ISR(void) __attribute__((weak, alias("Default_Handler")));
void USB_OUT_EP_INT_6_ISR(void) __attribute__((weak, alias("Default_Handler")));
void USB_SOF_INT_ISR(void) __attribute__((weak, alias("Default_Handler")));
void IR_ISR(void) __attribute__((weak, alias("Default_Handler")));
void TRNG_ISR(void) __attribute__((weak, alias("Default_Handler")));
void LPC_ISR(void) __attribute__((weak, alias("Default_Handler")));
void PF_RTC_ISR(void) __attribute__((weak, alias("Default_Handler")));
void SPI_SLAVE_ISR(void) __attribute__((weak, alias("Default_Handler")));
void FLASH_SEC0_ISR(void) __attribute__((weak, alias("Default_Handler")));
void PTA_ISR(void) __attribute__((weak, alias("Default_Handler")));
void DSP_WDT_ISR(void) __attribute__((weak, alias("Default_Handler")));
void VAD2MCU_INT_ISR(void) __attribute__((weak, alias("Default_Handler")));
void VB2MCU_INT_ISR(void) __attribute__((weak, alias("Default_Handler")));
void SDIO1_ISR(void) __attribute__((weak, alias("Default_Handler")));
void CAN2_ISR(void) __attribute__((weak, alias("Default_Handler")));
void ARM_AHB_CACHE_ISR(void) __attribute__((weak, alias("Default_Handler")));

const IRQ_Fun RamVectorTable_app[] __attribute__((section(".vectors_table"))) =
{
    (IRQ_Fun)APP_GLOBAL_ADDR,
    (IRQ_Fun)Reset_Handler,
    NMI_Handler,
    HardFault_Handler,
    MemManageFault_Handler,
    BusFault_Handler,
    UsageFault_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    Default_Handler,
    SVC_Handler,
    Debug_Monitor_Handler,
    Default_Handler,
    PendSV_Handler,
    SysTick_Handler,
    SYSTEM_ISR_ISR,
    WDT_ISR,
    BTMAC_ISR,
    DSP_ISR,
    RXI300_ISR_ISR,
    SPI0_ISR,
    I2C0_ISR,
    ADC_ISR,
    SPORT0_RX_ISR,
    SPORT0_TX_ISR,
    TIMER1_CH0_ISR,
    TIMER1_CH1_ISR,
    TIMER1_CH2_ISR,
    RTC_ISR,
    UART0_ISR,
    UART1_ISR,
    UART2_ISR,
    Peripheral_ISR_ISR,
    GPIO_A0_ISR,
    GPIO_A1_ISR,
    GPIO_A2_7_ISR,
    GPIO_A8_15_ISR,
    GPIO_A16_23_ISR,
    GPIO_A24_31_ISR,
    SPORT1_RX_ISR,
    SPORT1_TX_ISR,
    ADP_DET_ISR,
    VBAT_DET_ISR,
    GDMA0_Channel0_ISR,
    GDMA0_Channel1_ISR,
    GDMA0_Channel2_ISR,
    GDMA0_Channel3_ISR,
    GDMA0_Channel4_ISR,
    GDMA0_Channel5_ISR,
    GDMA0_Channel6_ISR,
    GDMA0_Channel7_ISR,
    GDMA0_Channel8_ISR,
    GPIO_B0_7_ISR,
    GPIO_B8_15_ISR,
    GPIO_B16_23_ISR,
    GPIO_B24_31_ISR,
    SPI1_ISR,
    SPI2_ISR,
    I2C1_ISR,
    I2C2_ISR,
    KEYSCAN_ISR,
    QDECODE_ISR,
    UART3_ISR,
    UART4_ISR,
    UART5_ISR,
    SPIC0_ISR,
    SPIC1_ISR,
    SPIC2_ISR,
    TIMER1_CH3_ISR,
    TIMER1_CH4_ISR,
    TIMER1_CH5_ISR,
    ASRC0_ISR,
    ASRC1_ISR,
    ETHERNET_ISR,
    ISO7816_ISR,
    SDIO0_ISR,
    SPORT2_RX_ISR,
    Zigbee_ISR,
    MJPEG_ISR,
    GDMA0_Channel9_ISR,
    GDMA0_Channel10_ISR,
    GDMA0_Channel11_ISR,
    Display_ISR,
    PPE_ISR,
    RTZIP_ISR,
    SLAVE_PORT_MONITOR_ISR,
    TIMER1_CH6_ISR,
    TIMER1_CH7_ISR,
    TIMER1_CH8_ISR,
    TIMER1_CH9_ISR,
    CAN0_ISR,
    BTMAC_WRAP_AROUND_ISR,
    PUBLIC_KEY_ENGINE_ISR,
    MFB_DET_L_ISR,
    spi_phy1_intr_ISR,
    CAN1_ISR,
    USB_ISR,
    ENDP_MULTI_PROC_INT_ISR,
    SUSPEND_N_INT_ISR,
    USB_IN_EP_INT_0_ISR,
    USB_IN_EP_INT_1_ISR,
    USB_IN_EP_INT_2_ISR,
    USB_IN_EP_INT_3_ISR,
    USB_IN_EP_INT_4_ISR,
    USB_IN_EP_INT_5_ISR,
    USB_IN_EP_INT_6_ISR,
    USB_OUT_EP_INT_0_ISR,
    USB_OUT_EP_INT_1_ISR,
    USB_OUT_EP_INT_2_ISR,
    USB_OUT_EP_INT_3_ISR,
    USB_OUT_EP_INT_4_ISR,
    USB_OUT_EP_INT_5_ISR,
    USB_OUT_EP_INT_6_ISR,
    USB_SOF_INT_ISR,
    IR_ISR,
    TRNG_ISR,
    LPC_ISR,
    PF_RTC_ISR,
    SPI_SLAVE_ISR,
    FLASH_SEC0_ISR,
    PTA_ISR,
    DSP_WDT_ISR,
    VAD2MCU_INT_ISR,
    VB2MCU_INT_ISR,
    SDIO1_ISR,
    CAN2_ISR,
    ARM_AHB_CACHE_ISR,
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
    DBG_DIRECT("APP reset handler");
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
