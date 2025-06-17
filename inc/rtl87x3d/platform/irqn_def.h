/**
*********************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      irqn_def.h
* @brief
* @details
* @author
* @date
* @version   v1.0
* *********************************************************************************************************
*/

/** @addtogroup 87x3d_IRQN_DEF IRQn Define
  * @brief IRQn define module.
  * @{
  */

/*============================================================================*
 *                              Types
*============================================================================*/
/** @defgroup 87x3d_IRQN_DEF_Exported_Types IRQn Define Exported Types
  * @{
  */



/** brief Interrupt Number Definition */
typedef enum IRQn
{
    /* -------------------  Cortex-M4 Processor Exceptions Numbers  ------------------- */
    NonMaskableInt_IRQn       = -14,      /**< 2 Non Maskable Interrupt */
    HardFault_IRQn            = -13,      /**< 3 HardFault Interrupt */
    MemoryManagement_IRQn     = -12,      /**< 4 Memory Management Interrupt */
    BusFault_IRQn             = -11,      /**< 5 Bus Fault Interrupt */
    UsageFault_IRQn           = -10,      /**< 6 Usage Fault Interrupt */
    SVCall_IRQn               =  -5,      /**< 11 SV Call Interrupt */
    DebugMonitor_IRQn         =  -4,      /**< 12 Debug Monitor Interrupt */
    PendSV_IRQn               =  -2,      /**< 14 Pend SV Interrupt */
    SysTick_IRQn              =  -1,      /**< 15 System Tick Interrupt */
    /* -------------------  external interrupts --------------------------------------- */
    System_IRQn = 0,                      /**< 0  System interrupt */
    WDG_IRQn,                             /**< 1  Watch dog global insterrupt  */
    BTMAC_IRQn,                           /**< 2  See Below Table ( an Extension of interrupt ) */
    USB_IP_IRQn,                          /**< 3  USB IP interrupt */
    TIM2_IRQn,                            /**< 4  Timer2 global interrupt */
    Platform_IRQn,                        /**< 5  Platform error interrupt */
    I2S0_TX_IRQn,                         /**< 6  I2S0 TX interrupt */
    I2S0_RX_IRQn,                         /**< 7  I2S0 RX interrupt */
    UART2_IRQn,                           /**< 8  Data_Uart1 interrupt (used for DSP) */
    GPIO_A0_IRQn,                         /**< 9  GPIO 0 interrupt */
    GPIO_A1_IRQn,                         /**< 10 GPIO 1 interrupt */
    UART1_IRQn,                           /**< 11 Log_Uart interrupt (Log0) */
    UART0_IRQn,                           /**< 12 Data_Uart interrupt */
    RTC_IRQn,                             /**< 13 Realtime counter interrupt */
    SPI0_IRQn,                            /**< 14 SPI0 interrupt */
    SPI1_IRQn,                            /**< 15 SPI1 interrupt */
    I2C0_IRQn,                            /**< 16 I2C0 interrupt */
    I2C1_IRQn,                            /**< 17 I2C1 interrupt */
    ADC_IRQn,                             /**< 18 ADC global interrupt */
    Peripheral_IRQn,                      /**< 19 See Below Table ( an Extension of interrupt ) */
    GDMA0_Channel0_IRQn,                  /**< 20 RTK-DMA0 channel 0 global interrupt */
    GDMA0_Channel1_IRQn,                  /**< 21 RTK-DMA0 channel 1 global interrupt */
    GDMA0_Channel2_IRQn,                  /**< 22 RTK-DMA0 channel 2 global interrupt */
    GDMA0_Channel3_IRQn,                  /**< 23 RTK-DMA0 channel 3 global interrupt */
    GDMA0_Channel4_IRQn,                  /**< 24 RTK-DMA0 channel 4 global interrupt */
    GDMA0_Channel5_IRQn,                  /**< 25 RTK-DMA0 channel 5 global interrupt */
    KeyScan_IRQn,                         /**< 26 keyscan global interrupt */
    QDEC_IRQn,                            /**< 27 qdecode global interrupt */
    IR_IRQn,                              /**< 28 IR module global interrupt */
    DSP_IRQn,                             /**< 29 DSP interrupt */
    GDMA0_Channel6_IRQn,                  /**< 30 GDMA Channel 6 interrupt */
    GDMA0_Channel7_IRQn,                  /**< 31 GDMA Channel 7 interrupt */
    GDMA0_Channel8_IRQn,                  /**< 32 GDMA Channel 8 interrupt */
    GDMA0_Channel9_IRQn,                  /**< 33 GDMA Channel 9 interrupt */
    GDMA0_Channel10_IRQn,                 /**< 34 GDMA Channel 10 interrupt */
    GDMA0_Channel11_IRQn,                 /**< 35 GDMA Channel 11 interrupt */
    GDMA0_Channel12_IRQn,                 /**< 36 GDMA Channel 12 interrupt */
    GDMA0_Channel13_IRQn,                 /**< 37 GDMA Channel 13 interrupt */
    GDMA0_Channel14_IRQn,                 /**< 38 GDMA Channel 14 interrupt */
    GDMA0_Channel15_IRQn,                 /**< 39 GDMA Channel 15 interrupt */
    TIM3_IRQn,                            /**< 40 Timer3 global interrupt */
    TIM4_IRQn,                            /**< 41 Timer4 global interrupt */
    TIM5_IRQn,                            /**< 42 Timer5 global interrupt */
    TIM6_IRQn,                            /**< 43 Timer6 global interrupt */
    TIM7_IRQn,                            /**< 44 Timer7 global interrupt */
    TRNG_IRQn,                            /**< 45 TRNG global interrupt */
    GPIO_A_9_IRQn,                        /**< 46 GPIO A 9 interrupt */
    GPIO_A_16_IRQn,                       /**< 47 GPIO A 16 interrupt */
    GPIO_A_2_7_IRQn,                      /**< 48 GPIO A 2 to 7 interrupt */
    GPIO_A_8_15_IRQn,                     /**< 49 GPIO A 8 to 15 interrupt */
    GPIO_A_16_23_IRQn,                    /**< 50 GPIO A 16 to 23 interrupt */
    GPIO_A_24_31_IRQn,                    /**< 51 GPIO A 24 to 31 interrupt */
    GPIO_B_0to7_IRQn,                     /**< 52 GPIO B 0 to 7 interrupt */
    GPIO_B_8to15_IRQn,                    /**< 53 GPIO B 8 to 15 interrupt */
    GPIO_B_16to23_IRQn,                   /**< 54 GPIO B 16 to 23 interrupt */
    GPIO_B_24to31_IRQn,                   /**< 55 GPIO B 24 to 31 interrupt */
    UART3_IRQn,                           /**< 56 UART3 global interrupt */
    SPI2_IRQn,                            /**< 57 SPI2 global interrupt */
    I2C2_IRQn,                            /**< 58 I2C2 global interrupt */
    DSP_Event_1_IRQn,                     /**< 59 DSP Event interrupt */
    DSP_Event_2_IRQn,                     /**< 60 DSP Event 2 interrupt */
    SHA256_IRQn,                          /**< 61 SHA256 interrupt */
    USB_ISOC_IRQn,                        /**< 62 USB ISOC interrupt */
    BT_CLK_compare_IRQn,                  /**< 63 BT Clock Compare interrupt */
    HW_RSA_IRQn,                          /**< 64 HW RSA interrupt */
    PSRAMC_IRQn,                          /**< 65 PSRAM interrupt */
    Compare_clk_4_IRQn,                   /**< 66 Compare Clock 4 interrupt */
    Compare_clk_5_IRQn,                   /**< 67 Compare Clock 5 interrupt */
    dspdma_int_all_host_IRQn,             /**< 68 DSP DMA interrupt */
    NNA_IRQn,                             /**< 69 NNA interrupt */
    Compare_clk_9_IRQn,                   /**< 70 Compare Clock 9 interrupt */
    Compare_clk_10_IRQn,                  /**< 71 Compare Clock 10 interrupt */
    GPIO_C_0to7_IRQn,                     /**< 72 GPIO C 0 to 7 interrupt */
    GPIO_C_8to15_IRQn,                    /**< 73 GPIO C 8 to 15 interrupt */
    GPIO_C_16_IRQn,                       /**< 74 GPIO C 16 interrupt */
    RSVD5_IRQn,                           /**< 75 Reserved */
    TIM8_IRQn,                            /**< 76 Timer 8 interrupt */
    TIM9_IRQn,                            /**< 77 Timer 9 interrupt */
    TIM10_IRQn,                           /**< 78 Timer 10 interrupt */
    TIM11_IRQn,                           /**< 79 Timer 11 interrupt */
    TIM12_IRQn,                           /**< 80 Timer 12 interrupt */
    TIM13_IRQn,                           /**< 81 Timer 13 interrupt */
    TIM14_IRQn,                           /**< 82 Timer 14 interrupt */
    TIM15_IRQn,                           /**< 83 Timer 15 interrupt */
    SPIC0_IRQn,                           /**< 84 SPIC0 interrupt */
    SPIC1_IRQn,                           /**< 85 SPIC1 interrupt */
    SPIC2_IRQn,                           /**< 86 SPIC2 interrupt */
    SPIC3_IRQn,                           /**< 87 SPIC3 interrupt */
    SDIO_HOST_IRQn,                       /**< 88 SDIO Host interrupt */
    SPIC4_IRQn,                           /**< 89 SPIC4 interrupt */
    ERR_CORR_CODE_IRQn,                   /**< 90 Error Correct interrupt*/
    SLAVE_PORT_MONITOR_IRQn,              /**< 91 Slave Portr Monitor interrupt*/

    //  interrupts belong to Peripheral_IRQn, not directly connect to NVIC
    RESERVED0_IRQn,                       /**< 0, Reserved  */
    RESERVED1_IRQn,                       /**< 1, Reserved  */
    RESERVED2_IRQn,                       /**< 2, Reserved  */
    RESERVED3_IRQn,                       /**< 3, Reserved  */
    RESERVED4_IRQn,                       /**< 4, Reserved  */
    RESERVED5_IRQn,                       /**< 5, Reserved  */
    RESERVED6_IRQn,                       /**< 6, Reserved  */
    SPDIF_RX_IRQn,                        /**< 7, SPDIF RX interrupt */
    SPDIF_TX_IRQn,                        /**< 8, SPDIF TX interrupt */
    MFB_DET_L_IRQn,                       /**< 9, MFB Detection interrupt */
    SPI2W_IRQn,                           /**< 10, SPI2W interrupt */
    LPCOMP_IRQn,                          /**< 11, Low Power Comparator interrupt */
    MBIAS_VBAT_DET_IRQn,                  /**< 12, VBAT detection interrupt */
    MBIAS_ADP_DET_IRQn,                   /**< 13, ADP detection interrupt */
    HW_ASRC1_IRQn,                        /**< 14, HW ASRC1 interrupt */
    HW_ASRC2_IRQn,                        /**< 15, HW ASRC2 interrupt */
    VADBUF_IRQn,                          /**< 16, VAD BUF interrupt */
    PTA_Mailbox_IRQn,                     /**< 17, OTA Mailbox interrupt */
    DSP2MCU_IRQn,                         /**< 18, DSP2MCU interrupt */
    SPORT2_TX_IRQn,                       /**< 19, SPORT2 TX interrupt */
    SPORT2_RX_IRQn,                       /**< 20, SPORT2 RX interrupt */
    SPORT3_TX_IRQn,                       /**< 21, SPORT3 TX interrupt */
    SPORT3_RX_IRQn,                       /**< 22, SPORT3 RX interrupt */
    VAD_IRQn,                             /**< 23, VAD interrupt */
    ANC_IRQn,                             /**< 24, ANC interrupt */
    SPORT0_TX_IRQn,                       /**< 25, SPORT0 TX interrupt */
    SPORT0_RX_IRQn,                       /**< 26, SPORT0 RX interrupt */
    SPORT1_TX_IRQn,                       /**< 27, SPORT1 TX interrupt  */
    SPORT1_RX_IRQn,                       /**< 28, SPORT1 RX interrupt  */
    USB_UTMI_SUSPEND_N_IRQn,              /**< 29, USB UTMI Suspend interrupt  */
    USB_DLPS_RESUME_IRQn,                 /**< 30, USB DLPS Resume interrupt  */
    RESERVED7_IRQn,                       /**< 31, Reserved */
} IRQn_Type, *PIRQn_Type;

/** @} */ /* End of group 87x3d_IRQN_DEF_Exported_Types */
/** @} */ /* End of group 87x3d_IRQN_DEF */
