/**
*********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      pin_def.h
* @brief
* @details
* @author
* @date
* @version   v1.1
* *********************************************************************************************************
*/

/** @defgroup 87x3d_PIN_DEF Pin Define
  * @brief Pin define.
  * @{
  */
#ifndef _PIN_DEF_H_
#define _PIN_DEF_H_

/*============================================================================*
 *                              RTL876X Pin Number
*============================================================================*/
/** @defgroup 87x3d_PIN_DEF_Exported_Constants Pin Define Exported Constants
  * @{
  */

/** @defgroup 87x3d_RTL876X_Pin_Number Pin Number
  * @{
  */
#define P0_0        0       /**< GPIO0 */
#define P0_1        1       /**< GPIO1 */
#define P0_2        2       /**< GPIO2 */
#define P0_3        3       /**< GPIO3 */

#define P0_4        4       /**< GPIO4 */
#define P0_5        5       /**< GPIO5 */
#define P0_6        6       /**< GPIO6 */
#define P0_7        7       /**< GPIO7 */

#define P1_0        8       /**< GPIO8 */
#define P1_1        9       /**< GPIO9 */
#define P1_2        10      /**< GPIO10*/
#define P1_3        11      /**< GPIO11*/

#define P1_4        12      /**< GPIO12*/
#define P1_5        13      /**< GPIO13*/
#define P1_6        14      /**< GPIO14*/
#define P1_7        15      /**< GPIO15*/

#define P2_0        16      /**< GPIO16 */
#define P2_1        17      /**< GPIO17 */
#define P2_2        18      /**< GPIO18 */
#define P2_3        19      /**< GPIO19 */

#define P2_4        20      /**< GPIO20 */
#define P2_5        21      /**< GPIO21 */
#define P2_6        22      /**< GPIO22 */
#define P2_7        23      /**< GPIO23 */

#define P3_0        24      /**< GPIO24 */
#define P3_1        25      /**< GPIO25 */
#define P3_2        26      /**< GPIO26 */
#define P3_3        27      /**< GPIO27 */

#define P3_4        28      /**< GPIO28 */
#define P3_5        29      /**< GPIO29 */
#define P3_6        30      /**< GPIO30 */
#define P3_7        31      /**< GPIO31 */

#define P4_0        32      /**< GPIOB0 */
#define P4_1        33      /**< GPIOB1 */
#define P4_2        34      /**< GPIOB2 */
#define P4_3        35      /**< GPIOB3 */

#define P4_4        36      /**< GPIOB4 */
#define P4_5        37      /**< GPIOB5 */
#define P4_6        38      /**< GPIOB6 */
#define P4_7        39      /**< GPIOB7 */

#define P5_0        40      /**< GPIOB8 */
#define P5_1        41      /**< GPIOB9 */
#define P5_2        42      /**< GPIOB10*/
#define P5_3        43      /**< GPIOB11*/

#define P5_4        44      /**< GPIOB12*/
#define P5_5        45      /**< GPIOB13*/
#define P5_6        46      /**< GPIOB14*/
#define P5_7        47      /**< GPIOB15*/

#define P6_0        48      /**< GPIOB16*/
#define P6_1        49      /**< GPIOB17*/

#define LOUT_N      50      /**< GPIOC7 */
#define P_UART      51      /**< GPIOC6 */
#define ROUT_N      52      /**< GPIOC9 */
#define ROUT_P      53      /**< GPIOC8 */

#define MIC1_N      54      /**< GPIOC3 */
#define MIC1_P      55      /**< GPIOC2 */
#define MIC2_N      56      /**< GPIOC5 */
#define MIC2_P      57      /**< GPIOC4 */
#define MIC3_N      58      /**< GPIOC11*/
#define MIC3_P      59      /**< GPIOC10*/
#define MIC4_N      60      /**< GPIOC13*/
#define MIC4_P      61      /**< GPIOC12*/
#define MIC5_N      62
#define MIC5_P      63
#define MIC6_N      64
#define MIC6_P      65

#define AUX_R       66      /**< GPIOC0  */
#define AUX_L       67      /**< GPIOC1  */
#define MICBIAS     68      /**< GPIOC14 */

//#define RESEVED_PIN  69
#define XI_32K      70      /**< GPIOB30*/
#define XO_32K      71      /**< GPIOB31*/

/*The below pins  asle is called PAD_LPC pins*/
#define P6_2        72      /**< GPIOB18*/
#define P6_3        73      /**< GPIOB19*/
#define P6_4        74      /**< GPIOB20*/
#define P6_5        75      /**< GPIOB21*/
#define P6_6        76      /**< GPIOB22*/
#define P6_7        77      /**< GPIOB23*/

#define P7_0        78      /**< GPIOB24*/
#define P7_1        79      /**< GPIOB25*/
#define P7_2        80      /**< GPIOB26*/
#define P7_3        81      /**< GPIOB27*/
#define P7_4        82      /**< GPIOB28*/
#define P7_5        83      /**< GPIOB29*/
#define P7_6        84      /**< GPIOC15*/
#define P7_7        85      /**< GPIOC16*/

#define ADC_0       P0_0    /**< GPIO0  */
#define ADC_1       P0_1    /**< GPIO1  */
#define ADC_2       P0_2    /**< GPIO2  */
#define ADC_3       P0_3    /**< GPIO3  */
#define ADC_4       P0_4    /**< GPIO4  */
#define ADC_5       P0_5    /**< GPIO5  */
#define ADC_6       P0_6    /**< GPIO6  */
#define ADC_7       P0_7    /**< GPIO7 */

#define  TOTAL_PIN_NUM             86

#define PIN_DEF_IS_INVALID_PIN(pin)       ((pin) >= TOTAL_PIN_NUM)

#endif

/** @} */ /* End of group 87x3d_RTL876X_Pin_Number */

/** End of Group 87x3d_PIN_DEF_Exported_Constants
  * @}
  */

/** @} */ /* End of group 87x3d_PIN_DEF */
