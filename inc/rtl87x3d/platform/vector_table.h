#ifndef VECTOR_TABLE_H
#define VECTOR_TABLE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

#define IRQ_NUM_MAX 128

/** @defgroup 87x3d_VECTOR_TABLE RTL87x3d Vector Define
  * @brief    Define vector table.
  * @{
  */

/*============================================================================*
 *                              Types
*============================================================================*/
/** @defgroup 87x3d_Vector_Table_Exported_types Vector Table Exported Types
  * @{
  */

typedef void (*IRQ_Fun)(void);       /**< ISR Handler Prototype */

typedef enum
{
    InitialSP_VECTORn = 0,
    Reset_VECTORn,
    NMI_VECTORn,
    HardFault_VECTORn,
    MemMang_VECTORn,
    BusFault_VECTORn,
    UsageFault_VECTORn,
    RSVD0_VECTORn,
    RSVD1_VECTORn,
    RSVD2_VECTORn,
    RSVD3_VECTORn,
    SVC_VECTORn,
    DebugMonitor_VECTORn,
    RSVD4_VECTORn,
    PendSV_VECTORn,
    SysTick_VECTORn,
    System_VECTORn = 16,
    WDG_VECTORn,
    BTMAC_VECTORn,
    USB_IP_VECTORn,
    Timer2_VECTORn,
    Platform_VECTORn,
    I2S0_VECTORn,
    I2S1_VECTORn,
    UART2_VECTORn,
    GPIO_A0_VECTORn,
    GPIO_A1_VECTORn,
    UART1_VECTORn,
    UART0_VECTORn,
    RTC_VECTORn,
    SPI0_VECTORn,
    SPI1_VECTORn,
    I2C0_VECTORn,
    I2C1_VECTORn,
    ADC_VECTORn,
    Peripheral_VECTORn,
    GDMA0_Channel0_VECTORn,
    GDMA0_Channel1_VECTORn,
    GDMA0_Channel2_VECTORn,
    GDMA0_Channel3_VECTORn,
    GDMA0_Channel4_VECTORn,
    GDMA0_Channel5_VECTORn,
    Keyscan_VECTORn,
    Qdecode_VECTORn,
    IR_VECTORn,
    DSP_VECTORn,
    GDMA0_Channel6_VECTORn,
    GDMA0_Channel7_VECTORn,
    GDMA0_Channel8_VECTORn,         /**< ;[32] */
    GDMA0_Channel9_VECTORn,         /**< ;[33] */
    GDMA0_Channel10_VECTORn,        /**< ;[34] */
    GDMA0_Channel11_VECTORn,        /**< ;[35] */
    GDMA0_Channel12_VECTORn,        /**< ;[36] */
    GDMA0_Channel13_VECTORn,        /**< ;[37] */
    GDMA0_Channel14_VECTORn,        /**< ;[38] */
    GDMA0_Channel15_VECTORn,        /**< ;[39] */
    TIM3_VECTORn,                   /**< ;[40] */
    TIM4_VECTORn,                   /**< ;[41] */
    TIM5_VECTORn,                   /**< ;[42] */
    Timer6_VECTORn,                   /**< ;[43] */
    TIM7_VECTORn,                   /**< ;[44] */
    TRNG_VECTORn,                   /**< ;[45] */
    GPIO_A_9_VECTORn,               /**< ;[46] */
    GPIO_A_16_VECTORn,              /**< ;[47] */
    GPIO_A_2_7_VECTORn,            /**< ;[48] */
    GPIO_A_8_10to15_VECTORn,        /**< ;[49] */
    GPIO_A_17to23_VECTORn,          /**< ;[50] */
    GPIO_A_24to31_VECTORn,          /**< ;[51] */
    GPIO_B_0to7_VECTORn,            /**< ;[52] */
    GPIO_B_8to15_VECTORn,           /**< ;[53] */
    GPIO_B_16to23_VECTORn,          /**< ;[54] */
    GPIO_B_24to31_VECTORn,          /**< ;[55] */
    UART3_VECTORn,                  /**< ;[56] */
    SPI2_VECTORn,                   /**< ;[57] */
    I2C2_VECTORn,                   /**< ;[58] */
    DSP_Event_1_VECTORn,            /**< ;[59] */
    DSP_Event_2_VECTORn,            /**< ;[60] */
    SHA256_VECTORn,                 /**< ;[61] */
    USB_ISOC_VECTORn,               /**< ;[62] */
    BT_CLK_compare_VECTORn,         /**< ;[63] */
    HW_RSA_VECTORn,                 /**< ;[64] */
    PSRAM_VECTORn,                  /**< ;[65] */
    Compare_clk_4_VECTORn,          /**< ;[66] */
    Compare_clk_5_VECTORn,          /**< ;[67] */
    dspdma_int_all_host_VECTORn,    /**< ;[68] */
    NNA_VECTORn,                    /**< ;[69] */
    Compare_clk_9_VECTORn,          /**< ;[70] */
    Compare_clk_10_VECTORn,         /**< ;[71] */
    GPIO_C_0to7_VECTORn,            /**< ;[72] */
    GPIO_C_8to15_VECTORn,           /**< ;[73] */
    GPIO_C_16_VECTORn,              /**< ;[74] */
    RSVD6_VECTORn,                  /**< ;[75] */
    TIM8_VECTORn,                   /**< ;[76] */
    TIM9_VECTORn,                   /**< ;[77] */
    TIM10_VECTORn,                  /**< ;[78] */
    TIM11_VECTORn,                  /**< ;[79] */
    TIM12_VECTORn,                  /**< ;[80] */
    TIM13_VECTORn,                  /**< ;[81] */
    TIM14_VECTORn,                  /**< ;[82] */
    TIM15_VECTORn,                  /**< ;[83] */
    SPIC0_VECTORn,                  /**< ;[84] */
    SPIC1_VECTORn,                  /**< ;[85] */
    SPIC2_VECTORn,                  /**< ;[86] */
    SPIC3_VECTORn,                  /**< ;[87] */
    SDIO_HOST_VECTORn,              /**< ;[88] */
    SPIC4_VECTORn,                  /**< ;[89] */
    ERR_CORR_CODE_VECTORn,          /**< ;[90] */
    SLAVE_PORT_MONITOR_VECTORn,     /**< ;[91] */
    // sub isr belong to Peripheral_VECTORn
    RESERVED0_VECTORn,              /**< 0     */
    RESERVED1_VECTORn,              /**< 1     */
    RESERVED2_VECTORn,              /**< 2     */
    RESERVED3_VECTORn,              /**< 3     */
    RESERVED4_VECTORn,              /**< 4     */
    RESERVED5_VECTORn,              /**< 5     */
    RESERVED6_VECTORn,              /**< 6     */
    SPDIF_RX_VECTORn,               /**< 7     */
    SPDIF_TX_VECTORn,               /**< 8     */
    MFB_DET_L_VECTORn,        /**< 9     */
    SPI2W_VECTORn,                  /**< 10    */
    LPCOMP_VECTORn,                 /**< 11    */
    MBIAS_VBAT_DET_VECTORn,         /**< 12    */
    ADP_DET_VECTORn,          /**< 13    */
    HW_ASRC1_VECTORn,               /**< 14    */
    HW_ASRC2_VECTORn,               /**< 15    */
    VADBUF_VECTORn,                 /**< 16    */
    PTA_Mailbox_VECTORn,            /**< 17    */
    DSP2MCU_VECTORn,                /**< 18    */
    SPORT2_TX_VECTORn,              /**< 19    */
    SPORT2_RX_VECTORn,              /**< 20    */
    SPORT3_TX_VECTORn,              /**< 21    */
    SPORT3_RX_VECTORn,              /**< 22    */
    VAD_VECTORn,                    /**< 23    */
    ANC_VECTORn,                    /**< 24    */
    SPORT0_TX_VECTORn,              /**< 25    */
    SPORT0_RX_VECTORn,              /**< 26    */
    SPORT1_TX_VECTORn,              /**< 27    */
    SPORT1_RX_VECTORn,              /**< 28    */
    USB_UTMI_SUSPEND_N_VECTORn,     /**< 29    */
    USB_DLPS_RESUME_VECTORn,        /**< 30    */
    RESERVED7_VECTORn,              /**< 31    */

    ADP_IN_DET_VECTORn,
    ADP_OUT_DET_VECTORn,

    // sub isr belong to GPIO_A_2to7_VECTORn
    GPIOA2_VECTORn,
    GPIOA3_VECTORn,
    GPIOA4_VECTORn,
    GPIOA5_VECTORn,
    GPIOA6_VECTORn,
    GPIOA7_VECTORn,
    // sub isr belong to GPIO_A_8_10to15_VECTORn
    GPIOA8_VECTORn,
    GPIOA9_VECTORn,
    GPIOA10_VECTORn,
    GPIOA11_VECTORn,
    GPIOA12_VECTORn,
    GPIOA13_VECTORn,
    GPIOA14_VECTORn,
    GPIOA15_VECTORn,

    GPIOA16_VECTORn,

    // sub isr belong to GPIO_A_17to23_VECTORn
    GPIOA17_VECTORn,
    GPIOA18_VECTORn,
    GPIOA19_VECTORn,
    GPIOA20_VECTORn,
    GPIOA21_VECTORn,
    GPIOA22_VECTORn,
    GPIOA23_VECTORn,
    // sub isr belong to GPIO_A_24to31_VECTORn
    GPIOA24_VECTORn,
    GPIOA25_VECTORn,
    GPIOA26_VECTORn,
    GPIOA27_VECTORn,
    GPIOA28_VECTORn,
    GPIOA29_VECTORn,
    GPIOA30_VECTORn,
    GPIOA31_VECTORn,
    // sub isr belong to GPIO_B_0to7_VECTORn
    GPIOB0_VECTORn,
    GPIOB1_VECTORn,
    GPIOB2_VECTORn,
    GPIOB3_VECTORn,
    GPIOB4_VECTORn,
    GPIOB5_VECTORn,
    GPIOB6_VECTORn,
    GPIOB7_VECTORn,
    // sub isr belong to GPIO_B_8to15_VECTORn
    GPIOB8_VECTORn,
    GPIOB9_VECTORn,
    GPIOB10_VECTORn,
    GPIOB11_VECTORn,
    GPIOB12_VECTORn,
    GPIOB13_VECTORn,
    GPIOB14_VECTORn,
    GPIOB15_VECTORn,
    // sub isr belong to GPIO_B_16to23_VECTORn
    GPIOB16_VECTORn,
    GPIOB17_VECTORn,
    GPIOB18_VECTORn,
    GPIOB19_VECTORn,
    GPIOB20_VECTORn,
    GPIOB21_VECTORn,
    GPIOB22_VECTORn,
    GPIOB23_VECTORn,
    // sub isr belong to GPIO_B_24to31_VECTORn
    GPIOB24_VECTORn,
    GPIOB25_VECTORn,
    GPIOB26_VECTORn,
    GPIOB27_VECTORn,
    GPIOB28_VECTORn,
    GPIOB29_VECTORn,
    GPIOB30_VECTORn,
    GPIOB31_VECTORn,
    // sub isr belong to GPIO_C_0to7_VECTORn
    GPIOC0_VECTORn,
    GPIOC1_VECTORn,
    GPIOC2_VECTORn,
    GPIOC3_VECTORn,
    GPIOC4_VECTORn,
    GPIOC5_VECTORn,
    GPIOC6_VECTORn,
    GPIOC7_VECTORn,
    // sub isr belong to GPIO_C_8to15_VECTORn
    GPIOC8_VECTORn,
    GPIOC9_VECTORn,
    GPIOC10_VECTORn,
    GPIOC11_VECTORn,
    GPIOC12_VECTORn,
    GPIOC13_VECTORn,
    GPIOC14_VECTORn,
    GPIOC15_VECTORn,

    VECTORn_MAX
} VECTORn_Type;

/** @} */ /* End of group 87x3d_Vector_Table_Exported_types */
/** @} */ /* End of group 87x3d_VECTOR_TABLE */


extern void *rom_vector_table[];
extern void *RamVectorTable[];

/**
 * @brief  Initialize RAM vector table to a given RAM address.
 * @param  ram_vector_addr: RAM Vector Address.
 * @retval true: Success
 *         false: Fail
 * @note   When using vector table relocation, the base address of the new vector
 *         table must be aligned to the size of the vector table extended to the
 *         next larger power of 2. In RTL8763, the base address is aligned at 0x100.
 */
bool RamVectorTableInit(uint32_t ram_vector_addr);

/**
 * @brief  Update ISR Handler in RAM Vector Table.
 * @param  v_num: Vector number(index)
 * @param  isr_handler: User defined ISR Handler.
 * @retval true: Success
 *         false: Fail
 */
extern bool RamVectorTableUpdate(VECTORn_Type v_num, IRQ_Fun isr_handler);
#if defined (__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
// used in RamVectorTable
// CPU exceptions
__weak void Reset_Handler(void);
__weak void NMI_Handler(void);
__weak void HardFault_Handler(void);
__weak void MemManage_Handler(void);
__weak void BusFault_Handler(void);
__weak void UsageFault_Handler(void);
__weak void SVC_Handler(void);
__weak void DebugMon_Handler(void);
__weak void PendSV_Handler(void);
__weak void SysTick_Handler(void);
// external interrupts direct to NVIC
__weak void System_Handler(void);
__weak void WDT_Handler(void);
__weak void BTMAC_Handler(void);
__weak void USB_IP_Handler(void);
__weak void TIM2_Handler(void);
__weak void Platform_Handler(void);
__weak void I2S0_TX_Handler(void);
__weak void I2S0_RX_Handler(void);
__weak void UART2_Handler(void);
__weak void GPIOA0_Handler(void);
__weak void GPIOA1_Handler(void);
__weak void UART1_Handler(void);
__weak void UART0_Handler(void);
__weak void RTC_Handler(void);
__weak void SPI0_Handler(void);
__weak void SPI1_Handler(void);
__weak void I2C0_Handler(void);
__weak void I2C1_Handler(void);
__weak void ADC_Handler(void);
__weak void Peripheral_Handler(void);
__weak void GDMA0_Channel0_Handler(void);
__weak void GDMA0_Channel1_Handler(void);
__weak void GDMA0_Channel2_Handler(void);
__weak void GDMA0_Channel3_Handler(void);
__weak void GDMA0_Channel4_Handler(void);
__weak void GDMA0_Channel5_Handler(void);
__weak void KeyScan_Handler(void);
__weak void QDEC_Handler(void);
__weak void IR_Handler(void);
__weak void DSP_Handler(void);
__weak void GDMA0_Channel6_Handler(void);
__weak void GDMA0_Channel7_Handler(void);
__weak void GDMA0_Channel8_Handler(void);
__weak void GDMA0_Channel9_Handler(void);
__weak void GDMA0_Channel10_Handler(void);
__weak void GDMA0_Channel11_Handler(void);
__weak void GDMA0_Channel12_Handler(void);
__weak void GDMA0_Channel13_Handler(void);
__weak void GDMA0_Channel14_Handler(void);
__weak void GDMA0_Channel15_Handler(void);
__weak void TIM3_Handler(void);
__weak void TIM4_Handler(void);
__weak void TIM5_Handler(void);
__weak void TIM6_Handler(void);
__weak void TIM7_Handler(void);
__weak void TIM8to11_Handler(void);
__weak void GPIO_A_9_Handler(void);
__weak void GPIO_A_16_Handler(void);
__weak void GPIO_A_2to7_Handler(void);
__weak void GPIO_A_8_10to15_Handler(void);
__weak void GPIO_A_17to23_Handler(void);
__weak void GPIO_A_24to31_Handler(void);
__weak void GPIO_B_0to7_Handler(void);
__weak void GPIO_B_8to15_Handler(void);
__weak void GPIO_B_16to23_Handler(void);
__weak void GPIO_B_24to31_Handler(void);
__weak void UART3_Handler(void);
__weak void SPI2_Handler(void);
__weak void I2C2_Handler(void);
__weak void DSP_Event_1_Handler(void);
__weak void DSP_Event_2_Handler(void);
__weak void SHA256_Handler(void);
__weak void USB_ISOC_Handler(void);
__weak void BT_CLK_compare_Handler(void);
__weak void HW_RSA_Handler(void);
__weak void PSRAM_Handler(void);
__weak void Compare_clk_4_Handler(void);
__weak void Compare_clk_5_Handler(void);
__weak void dspdma_int_all_host_Handler(void);
__weak void NNA_Handler(void);
__weak void Compare_clk_9_Handler(void);
__weak void Compare_clk_10_Handler(void);
__weak void GPIO_C_0to7_Handler(void);
__weak void GPIO_C_8to15_Handler(void);
__weak void TIM8_Handler(void);
__weak void TIM9_Handler(void);
__weak void TIM10_Handler(void);
__weak void TIM11_Handler(void);
__weak void TIM12_Handler(void);
__weak void TIM13_Handler(void);
__weak void TIM14_Handler(void);
__weak void TIM15_Handler(void);
__weak void SPIC0_Handler(void);
__weak void SPIC1_Handler(void);
__weak void SPIC2_Handler(void);
__weak void SPIC3_Handler(void);
__weak void SDIO_HOST_Handler(void);
__weak void SPIC4_Handler(void);
__weak void ERR_CORR_CODE_Handler(void);
__weak void SLAVE_PORT_MONITOR_Handler(void);
// 2nd level interrupts of Peripheral_Handler
__weak void RESERVED0_Handler(void);
__weak void RESERVED1_Handler(void);
__weak void RESERVED2_Handler(void);
__weak void RESERVED3_Handler(void);
__weak void RESERVED4_Handler(void);
__weak void RESERVED5_Handler(void);
__weak void RESERVED6_Handler(void);
__weak void SPDIF_Rx_Handler(void);
__weak void SPDIF_Tx_Handler(void);
__weak void MFB_DET_L_Handler(void);
__weak void SPI2W_Handler(void);
__weak void LPCOMP_Handler(void);
__weak void MBIAS_VBAT_DET_Handler(void);
__weak void MBIAS_ADP_DET_Handler(void);
__weak void HW_ASRC1_Handler(void);
__weak void HW_ASRC2_Handler(void);
__weak void VADBUF_Handler(void);
__weak void PTA_Mailbox_Handler(void);
__weak void DSP2MCU_Handler(void);
__weak void SPORT2_TX_Handler(void);
__weak void SPORT2_RX_Handler(void);
__weak void SPORT3_TX_Handler(void);
__weak void SPORT3_RX_Handler(void);
__weak void VAD_Handler(void);
__weak void ANC_Handler(void);
__weak void SPORT0_TX_Handler(void);
__weak void SPORT0_RX_Handler(void);
__weak void SPORT1_TX_Handler(void);
__weak void SPORT1_RX_Handler(void);
__weak void USB_UTMI_SUSPEND_N_Handler(void);
__weak void USB_DLPS_Resume_Handler(void);
__weak void RESERVED7_Handler(void);
__weak void TRNG_Handler(void);

__weak void ADP_IN_DET_Handler(void);
__weak void ADP_OUT_DET_Handler(void);

__weak void GPIOA2_Handler(void);
__weak void GPIOA3_Handler(void);
__weak void GPIOA4_Handler(void);
__weak void GPIOA5_Handler(void);
__weak void GPIOA6_Handler(void);
__weak void GPIOA7_Handler(void);

__weak void GPIOA8_Handler(void);
__weak void GPIOA9_Handler(void);
__weak void GPIOA10_Handler(void);
__weak void GPIOA11_Handler(void);
__weak void GPIOA12_Handler(void);
__weak void GPIOA13_Handler(void);
__weak void GPIOA14_Handler(void);
__weak void GPIOA15_Handler(void);

__weak void GPIOA16_Handler(void);

__weak void GPIOA17_Handler(void);
__weak void GPIOA18_Handler(void);
__weak void GPIOA19_Handler(void);
__weak void GPIOA20_Handler(void);
__weak void GPIOA21_Handler(void);
__weak void GPIOA22_Handler(void);
__weak void GPIOA23_Handler(void);

__weak void GPIOA24_Handler(void);
__weak void GPIOA25_Handler(void);
__weak void GPIOA26_Handler(void);
__weak void GPIOA27_Handler(void);
__weak void GPIOA28_Handler(void);
__weak void GPIOA29_Handler(void);
__weak void GPIOA30_Handler(void);
__weak void GPIOA31_Handler(void);

__weak void GPIOB0_Handler(void);
__weak void GPIOB1_Handler(void);
__weak void GPIOB2_Handler(void);
__weak void GPIOB3_Handler(void);
__weak void GPIOB4_Handler(void);
__weak void GPIOB5_Handler(void);
__weak void GPIOB6_Handler(void);
__weak void GPIOB7_Handler(void);

__weak void GPIOB8_Handler(void);
__weak void GPIOB9_Handler(void);
__weak void GPIOB10_Handler(void);
__weak void GPIOB11_Handler(void);
__weak void GPIOB12_Handler(void);
__weak void GPIOB13_Handler(void);
__weak void GPIOB14_Handler(void);
__weak void GPIOB15_Handler(void);

__weak void GPIOB16_Handler(void);
__weak void GPIOB17_Handler(void);
__weak void GPIOB18_Handler(void);
__weak void GPIOB19_Handler(void);
__weak void GPIOB20_Handler(void);
__weak void GPIOB21_Handler(void);
__weak void GPIOB22_Handler(void);
__weak void GPIOB23_Handler(void);

__weak void GPIOB24_Handler(void);
__weak void GPIOB25_Handler(void);
__weak void GPIOB26_Handler(void);
__weak void GPIOB27_Handler(void);
__weak void GPIOB28_Handler(void);
__weak void GPIOB29_Handler(void);
__weak void GPIOB30_Handler(void);
__weak void GPIOB31_Handler(void);

__weak void GPIOC0_Handler(void);
__weak void GPIOC1_Handler(void);
__weak void GPIOC2_Handler(void);
__weak void GPIOC3_Handler(void);
__weak void GPIOC4_Handler(void);
__weak void GPIOC5_Handler(void);
__weak void GPIOC6_Handler(void);
__weak void GPIOC7_Handler(void);

__weak void GPIOC8_Handler(void);
__weak void GPIOC9_Handler(void);
__weak void GPIOC10_Handler(void);
__weak void GPIOC11_Handler(void);
__weak void GPIOC12_Handler(void);
__weak void GPIOC13_Handler(void);
__weak void GPIOC14_Handler(void);
__weak void GPIOC15_Handler(void);
#else
// used in RamVectorTable
// CPU exceptions
extern void Reset_Handler(void);
extern void NMI_Handler(void);
extern void HardFault_Handler(void);
extern void MemManage_Handler(void);
extern void BusFault_Handler(void);
extern void UsageFault_Handler(void);
extern void SVC_Handler(void);
extern void DebugMon_Handler(void);
extern void PendSV_Handler(void);
extern void SysTick_Handler(void);
// external interrupts direct to NVIC
extern void System_Handler(void);
extern void WDT_Handler(void);
extern void BTMAC_Handler(void);
extern void USB_IP_Handler(void);
extern void TIM2_Handler(void);
extern void Platform_Handler(void);
extern void I2S0_TX_Handler(void);
extern void I2S0_RX_Handler(void);
extern void UART2_Handler(void);
extern void GPIOA0_Handler(void);
extern void GPIOA1_Handler(void);
extern void UART1_Handler(void);
extern void UART0_Handler(void);
extern void RTC_Handler(void);
extern void SPI0_Handler(void);
extern void SPI1_Handler(void);
extern void I2C0_Handler(void);
extern void I2C1_Handler(void);
extern void ADC_Handler(void);
extern void Peripheral_Handler(void);
extern void GDMA0_Channel0_Handler(void);
extern void GDMA0_Channel1_Handler(void);
extern void GDMA0_Channel2_Handler(void);
extern void GDMA0_Channel3_Handler(void);
extern void GDMA0_Channel4_Handler(void);
extern void GDMA0_Channel5_Handler(void);
extern void KeyScan_Handler(void);
extern void QDEC_Handler(void);
extern void IR_Handler(void);
extern void DSP_Handler(void);
extern void GDMA0_Channel6_Handler(void);
extern void GDMA0_Channel7_Handler(void);
extern void GDMA0_Channel8_Handler(void);
extern void GDMA0_Channel9_Handler(void);
extern void GDMA0_Channel10_Handler(void);
extern void GDMA0_Channel11_Handler(void);
extern void GDMA0_Channel12_Handler(void);
extern void GDMA0_Channel13_Handler(void);
extern void GDMA0_Channel14_Handler(void);
extern void GDMA0_Channel15_Handler(void);
extern void TIM3_Handler(void);
extern void TIM4_Handler(void);
extern void TIM5_Handler(void);
extern void TIM6_Handler(void);
extern void TIM7_Handler(void);
extern void TIM8to11_Handler(void);
extern void GPIO_A_9_Handler(void);
extern void GPIO_A_16_Handler(void);
extern void GPIO_A_2to7_Handler(void);
extern void GPIO_A_8_10to15_Handler(void);
extern void GPIO_A_17to23_Handler(void);
extern void GPIO_A_24to31_Handler(void);
extern void GPIO_B_0to7_Handler(void);
extern void GPIO_B_8to15_Handler(void);
extern void GPIO_B_16to23_Handler(void);
extern void GPIO_B_24to31_Handler(void);
extern void UART3_Handler(void);
extern void SPI2_Handler(void);
extern void I2C2_Handler(void);
extern void DSP_Event_1_Handler(void);
extern void DSP_Event_2_Handler(void);
extern void SHA256_Handler(void);
extern void USB_ISOC_Handler(void);
extern void BT_CLK_compare_Handler(void);
extern void HW_RSA_Handler(void);
extern void PSRAM_Handler(void);
extern void Compare_clk_4_Handler(void);
extern void Compare_clk_5_Handler(void);
extern void dspdma_int_all_host_Handler(void);
extern void NNA_Handler(void);
extern void Compare_clk_9_Handler(void);
extern void Compare_clk_10_Handler(void);
extern void GPIO_C_0to7_Handler(void);
extern void GPIO_C_8to15_Handler(void);
extern void TIM8_Handler(void);
extern void TIM9_Handler(void);
extern void TIM10_Handler(void);
extern void TIM11_Handler(void);
extern void TIM12_Handler(void);
extern void TIM13_Handler(void);
extern void TIM14_Handler(void);
extern void TIM15_Handler(void);
extern void SPIC0_Handler(void);
extern void SPIC1_Handler(void);
extern void SPIC2_Handler(void);
extern void SPIC3_Handler(void);
extern void SDIO_HOST_Handler(void);
extern void SPIC4_Handler(void);
extern void ERR_CORR_CODE_Handler(void);
extern void SLAVE_PORT_MONITOR_Handler(void);
// 2nd level interrupts of Peripheral_Handler
extern void RESERVED0_Handler(void);
extern void RESERVED1_Handler(void);
extern void RESERVED2_Handler(void);
extern void RESERVED3_Handler(void);
extern void RESERVED4_Handler(void);
extern void RESERVED5_Handler(void);
extern void RESERVED6_Handler(void);
extern void SPDIF_Rx_Handler(void);
extern void SPDIF_Tx_Handler(void);
extern void MFB_DET_L_Handler(void);
extern void SPI2W_Handler(void);
extern void LPCOMP_Handler(void);
extern void MBIAS_VBAT_DET_Handler(void);
extern void MBIAS_ADP_DET_Handler(void);
extern void HW_ASRC1_Handler(void);
extern void HW_ASRC2_Handler(void);
extern void VADBUF_Handler(void);
extern void PTA_Mailbox_Handler(void);
extern void DSP2MCU_Handler(void);
extern void SPORT2_TX_Handler(void);
extern void SPORT2_RX_Handler(void);
extern void SPORT3_TX_Handler(void);
extern void SPORT3_RX_Handler(void);
extern void VAD_Handler(void);
extern void ANC_Handler(void);
extern void SPORT0_TX_Handler(void);
extern void SPORT0_RX_Handler(void);
extern void SPORT1_TX_Handler(void);
extern void SPORT1_RX_Handler(void);
extern void USB_UTMI_SUSPEND_N_Handler(void);
extern void USB_DLPS_Resume_Handler(void);
extern void RESERVED7_Handler(void);
extern void TRNG_Handler(void);

extern void ADP_IN_DET_Handler(void);
extern void ADP_OUT_DET_Handler(void);

extern void GPIOA2_Handler(void);
extern void GPIOA3_Handler(void);
extern void GPIOA4_Handler(void);
extern void GPIOA5_Handler(void);
extern void GPIOA6_Handler(void);
extern void GPIOA7_Handler(void);

extern void GPIOA8_Handler(void);
extern void GPIOA9_Handler(void);
extern void GPIOA10_Handler(void);
extern void GPIOA11_Handler(void);
extern void GPIOA12_Handler(void);
extern void GPIOA13_Handler(void);
extern void GPIOA14_Handler(void);
extern void GPIOA15_Handler(void);

extern void GPIOA16_Handler(void);

extern void GPIOA17_Handler(void);
extern void GPIOA18_Handler(void);
extern void GPIOA19_Handler(void);
extern void GPIOA20_Handler(void);
extern void GPIOA21_Handler(void);
extern void GPIOA22_Handler(void);
extern void GPIOA23_Handler(void);

extern void GPIOA24_Handler(void);
extern void GPIOA25_Handler(void);
extern void GPIOA26_Handler(void);
extern void GPIOA27_Handler(void);
extern void GPIOA28_Handler(void);
extern void GPIOA29_Handler(void);
extern void GPIOA30_Handler(void);
extern void GPIOA31_Handler(void);

extern void GPIOB0_Handler(void);
extern void GPIOB1_Handler(void);
extern void GPIOB2_Handler(void);
extern void GPIOB3_Handler(void);
extern void GPIOB4_Handler(void);
extern void GPIOB5_Handler(void);
extern void GPIOB6_Handler(void);
extern void GPIOB7_Handler(void);

extern void GPIOB8_Handler(void);
extern void GPIOB9_Handler(void);
extern void GPIOB10_Handler(void);
extern void GPIOB11_Handler(void);
extern void GPIOB12_Handler(void);
extern void GPIOB13_Handler(void);
extern void GPIOB14_Handler(void);
extern void GPIOB15_Handler(void);

extern void GPIOB16_Handler(void);
extern void GPIOB17_Handler(void);
extern void GPIOB18_Handler(void);
extern void GPIOB19_Handler(void);
extern void GPIOB20_Handler(void);
extern void GPIOB21_Handler(void);
extern void GPIOB22_Handler(void);
extern void GPIOB23_Handler(void);

extern void GPIOB24_Handler(void);
extern void GPIOB25_Handler(void);
extern void GPIOB26_Handler(void);
extern void GPIOB27_Handler(void);
extern void GPIOB28_Handler(void);
extern void GPIOB29_Handler(void);
extern void GPIOB30_Handler(void);
extern void GPIOB31_Handler(void);

extern void GPIOC0_Handler(void);
extern void GPIOC1_Handler(void);
extern void GPIOC2_Handler(void);
extern void GPIOC3_Handler(void);
extern void GPIOC4_Handler(void);
extern void GPIOC5_Handler(void);
extern void GPIOC6_Handler(void);
extern void GPIOC7_Handler(void);

extern void GPIOC8_Handler(void);
extern void GPIOC9_Handler(void);
extern void GPIOC10_Handler(void);
extern void GPIOC11_Handler(void);
extern void GPIOC12_Handler(void);
extern void GPIOC13_Handler(void);
extern void GPIOC14_Handler(void);
extern void GPIOC15_Handler(void);
#endif
// for BBPRO compatible
#define GPIO0_Handler GPIOA0_Handler
#define GPIO1_Handler GPIOA1_Handler
#define GPIO2_Handler GPIOA2_Handler
#define GPIO3_Handler GPIOA3_Handler
#define GPIO4_Handler GPIOA4_Handler
#define GPIO5_Handler GPIOA5_Handler
#define GPIO6_Handler GPIOA6_Handler
#define GPIO7_Handler GPIOA7_Handler
#define GPIO8_Handler GPIOA8_Handler
#define GPIO9_Handler GPIOA9_Handler
#define GPIO10_Handler GPIOA10_Handler
#define GPIO11_Handler GPIOA11_Handler
#define GPIO12_Handler GPIOA12_Handler
#define GPIO13_Handler GPIOA13_Handler
#define GPIO14_Handler GPIOA14_Handler
#define GPIO15_Handler GPIOA15_Handler
#define GPIO16_Handler GPIOA16_Handler
#define GPIO17_Handler GPIOA17_Handler
#define GPIO18_Handler GPIOA18_Handler
#define GPIO19_Handler GPIOA19_Handler
#define GPIO20_Handler GPIOA20_Handler
#define GPIO21_Handler GPIOA21_Handler
#define GPIO22_Handler GPIOA22_Handler
#define GPIO23_Handler GPIOA23_Handler
#define GPIO24_Handler GPIOA24_Handler
#define GPIO25_Handler GPIOA25_Handler
#define GPIO26_Handler GPIOA26_Handler
#define GPIO27_Handler GPIOA27_Handler
#define GPIO28_Handler GPIOA28_Handler
#define GPIO29_Handler GPIOA29_Handler
#define GPIO30_Handler GPIOA30_Handler
#define GPIO31_Handler GPIOA31_Handler

// for BBPRO compatible
#define GPIO0_VECTORn GPIOA0_VECTORn
#define GPIO1_VECTORn GPIOA1_VECTORn
#define GPIO2_VECTORn GPIOA2_VECTORn
#define GPIO3_VECTORn GPIOA3_VECTORn
#define GPIO4_VECTORn GPIOA4_VECTORn
#define GPIO5_VECTORn GPIOA5_VECTORn
#define GPIO6_VECTORn GPIOA6_VECTORn
#define GPIO7_VECTORn GPIOA7_VECTORn
#define GPIO8_VECTORn GPIOA8_VECTORn
#define GPIO9_VECTORn GPIOA9_VECTORn
#define GPIO10_VECTORn GPIOA10_VECTORn
#define GPIO11_VECTORn GPIOA11_VECTORn
#define GPIO12_VECTORn GPIOA12_VECTORn
#define GPIO13_VECTORn GPIOA13_VECTORn
#define GPIO14_VECTORn GPIOA14_VECTORn
#define GPIO15_VECTORn GPIOA15_VECTORn
#define GPIO16_VECTORn GPIOA16_VECTORn
#define GPIO17_VECTORn GPIOA17_VECTORn
#define GPIO18_VECTORn GPIOA18_VECTORn
#define GPIO19_VECTORn GPIOA19_VECTORn
#define GPIO20_VECTORn GPIOA20_VECTORn
#define GPIO21_VECTORn GPIOA21_VECTORn
#define GPIO22_VECTORn GPIOA22_VECTORn
#define GPIO23_VECTORn GPIOA23_VECTORn
#define GPIO24_VECTORn GPIOA24_VECTORn
#define GPIO25_VECTORn GPIOA25_VECTORn
#define GPIO26_VECTORn GPIOA26_VECTORn
#define GPIO27_VECTORn GPIOA27_VECTORn
#define GPIO28_VECTORn GPIOA28_VECTORn
#define GPIO29_VECTORn GPIOA29_VECTORn
#define GPIO30_VECTORn GPIOA30_VECTORn
#define GPIO31_VECTORn GPIOA31_VECTORn

#ifdef __cplusplus
}
#endif

#endif // VECTOR_TABLE_H

