/**
**********************************************************************************************************
*               Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     rtl876x_tim_def.h
* @brief    TIM related definitions
* @details
* @author
* @date
* @version  v1.0.0
*********************************************************************************************************
*/
#ifndef RTL876X_TIM_DEF_H
#define RTL876X_TIM_DEF_H

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "rtl876x.h"

/*============================================================================*
 *                          TIM Defines
 *============================================================================*/
/** \defgroup TIM      TIM
  * \brief
  * \{
  */

/** \defgroup TIM_Exported_Constants TIM Exported Constants
  * \brief
  * \{
  */

/**
 * \defgroup TIM_Defines TIM Defines
 * \{
 * \ingroup  TIM_Exported_Constants
 */
/**********************************************************************************************************************
 **********************************************************************************************************************
 |             Function              | case1  case7  case6  case2  case3  case4  case5 |         TIM Defines          |
 |-----------------------------------|-------------------------------------------------|------------------------------|
 |  Timer Only                       |   Y      Y      Y      Y      Y      Y      Y   |  NO                          |
 |-----------------------------------|-------------------------------------------------|------------------------------|
 |  PWM Only                         |          Y      Y      Y      Y      Y      Y   |  TIM_SUPPORT_PWM_FUNCTION    |
 |-----------------------------------|-------------------------------------------------|------------------------------|
 |  PWM with shift_mode + dead_zone  |                 Y      Y      Y      Y      Y   |  TIM_SUPPORT_PWM_PHASE_SHIFT |
 |                                   |                                                 |  TIM_SUPPORT_PWM_DEADZONE    |
 |-----------------------------------|-------------------------------------------------|------------------------------|
 |  PWM with CCR FIFO                |                        Y      Y      Y      Y   |  TIM_SUPPORT_CCR_FIFO        |
 |-----------------------------------|-------------------------------------------------|------------------------------|
 |  PWM with CCR FIFO + DMA          |                               Y      Y      Y   |  TIM_SUPPORT_CCR_FIFO        |
 |                                   |                                                 |  TIM_SUPPORT_DMA             |
 |-----------------------------------|-------------------------------------------------|------------------------------|
 |  LATCH_COUNT_0                    |                                      Y      Y   |  TIM_SUPPORT_LATCH_CNT_0     |
 |-----------------------------------|-------------------------------------------------|------------------------------|
 |  LATCH_COUNT_1 + LATCH_COUNT_2    |                                             Y   |  TIM_SUPPORT_LATCH_CNT_1     |
 |                                   |                                                 |  TIM_SUPPORT_LATCH_CNT_2     |
 |-----------------------------------|-------------------------------------------------|------------------------------|
 ***********************************************************************************************************************/
#define TIM_SUPPORT_PWM_FUNCTION                    (1) /* Case7 */
#define TIM_SUPPORT_PWM_PHASE_SHIFT                 (1) /* Case6 */
#define TIM_SUPPORT_PWM_DEADZONE                    (1) /* Case6 */
#define TIM_SUPPORT_CCR_FIFO                        (1) /* Case2 Case3*/
#define TIM_SUPPORT_DMA_FUNCTION                    (1) /* Case3 */
#define TIM_SUPPORT_LATCH_CNT_0                     (1) /* Case4 */
#define TIM_SUPPORT_LATCH_CNT_1                     (1) /* Case5 */
#define TIM_SUPPORT_LATCH_CNT_2                     (1) /* Case5 */

#define TIM_SUPPORT_RAP_FUNCTION                    (0)
#define TIM_SUPPORT_AUTO_CLOCK                      (0)

/** End of TIM_Defines
  * \}
  */

/** End of TIM_Exported_Constants
  * \}
  */

/** End of TIM
  * \}
  */

/*============================================================================*
 *                         TIM Registers Memory Map
 *============================================================================*/
typedef struct
{
    __I  uint32_t TIMER_CUR_CNT;                /*!< 0x00 */
    __IO uint32_t TIMER_MODE_CFG;               /*!< 0x04 */
    __IO uint32_t TIMER_MAX_CNT;                /*!< 0x08 */
    __IO uint32_t TIMER_CCR;                    /*!< 0x0C */
    __IO uint32_t TIMER_CCR_FIFO;               /*!< 0x10 */
    __IO uint32_t TIMER_PWM_CFG;                /*!< 0x14 */
    __IO uint32_t TIMER_PWM_SHIFT_CNT;          /*!< 0x18 */
    __IO uint32_t TIMER_LAT_CNT_0_CFG;          /*!< 0x1C */
    __I  uint32_t TIMER_LAT_CNT_0;              /*!< 0x20 */
    __I  uint32_t TIMER_LAT_CNT_0_FIFO_DEPTH;   /*!< 0x24 */
    __IO uint32_t TIMER_LAT_CNT_1_CFG;          /*!< 0x28 */
    __I  uint32_t TIMER_LAT_CNT_1;              /*!< 0x2C */
    __I  uint32_t TIMER_LAT_CNT_1_FIFO_DEPTH;   /*!< 0x30 */
    __IO uint32_t TIMER_LAT_CNT_2_CFG;          /*!< 0x34 */
    __I  uint32_t TIMER_LAT_CNT_2;              /*!< 0x38 */
    __I  uint32_t TIMER_LAT_CNT_2_FIFO_DEPTH;   /*!< 0x3C */
    __IO uint32_t TIMER_DMA_CFG;                /*!< 0x40 */
    __I  uint32_t TIMER_DMY_0;                  /*!< 0x44 */
    __I  uint32_t TIMER_DMY_1;                  /*!< 0x48 */
    __I  uint32_t TIMER_DMY_2;                  /*!< 0x4C */
} TIM_TypeDef;

typedef struct
{
    __IO uint32_t TIMER_EN_CTRL;                                   /*!< 0xA00 */
    __IO uint32_t TIMER_ONESHOT_GO_CTRL;                           /*!< 0xA04 */
    __I  uint32_t TIMER_PWM_STATE;                                 /*!< 0xA08 */
    __IO uint32_t TIMER_INTR_EN_CTRL;                              /*!< 0xA0C */
    __IO uint32_t TIMER_INTR_STS;                                  /*!< 0xA10 */
    __I  uint32_t TIMER_INTR;                                      /*!< 0xA14 */
    __I  uint32_t TIMER_CCR_FIFO_EMPTY_STS;                        /*!< 0xA18 */
    __I  uint32_t TIMER_CCR_FIFO_FULL_STS;                         /*!< 0xA1C */
    __IO uint32_t TIMER_LAT_CNT_0_FIFO_FULL_INTR_EN_CTRL;          /*!< 0xA20 */
    __IO uint32_t TIMER_LAT_CNT_0_FIFO_MET_THRESHOLD_INTR_EN_CTRL; /*!< 0xA24 */
    __I  uint32_t TIMER_LAT_CNT_0_FIFO_EMPTY_STS;                  /*!< 0xA28 */
    __IO uint32_t TIMER_LAT_CNT_0_FIFO_FULL_INTR;                  /*!< 0xA2C */
    __IO uint32_t TIMER_LAT_CNT_0_FIFO_MET_THRESHOLD_INTR;         /*!< 0xA30 */
    __I  uint32_t TIMER_LAT_CNT_1_FIFO_FULL_INTR_EN_CTRL;          /*!< 0xA34 */
    __I  uint32_t TIMER_LAT_CNT_1_FIFO_MET_THRESHOLD_INTR_EN_CTRL; /*!< 0xA38 */
    __I  uint32_t TIMER_LAT_CNT_1_FIFO_EMPTY_STS;                  /*!< 0xA3C */
    __I  uint32_t TIMER_LAT_CNT_1_FIFO_FULL_INTR;                  /*!< 0xA40 */
    __I  uint32_t TIMER_LAT_CNT_1_FIFO_MET_THRESHOLD_INTR;         /*!< 0xA44 */
    __I  uint32_t TIMER_LAT_CNT_2_FIFO_FULL_INTR_EN_CTRL;          /*!< 0xA48 */
    __I  uint32_t TIMER_LAT_CNT_2_FIFO_MET_THRESHOLD_INTR_EN_CTRL; /*!< 0xA4C */
    __I  uint32_t TIMER_LAT_CNT_2_FIFO_EMPTY_STS;                  /*!< 0xA50 */
    __I  uint32_t TIMER_LAT_CNT_2_FIFO_FULL_INTR;                  /*!< 0xA54 */
    __I  uint32_t TIMER_LAT_CNT_2_FIFO_MET_THRESHOLD_INTR;         /*!< 0xA58 */
    __IO uint32_t TIMER_PAUSE_CTRL;                                /*!< 0xA5C */
    __IO uint32_t TIMER_PAUSE_INTR_EN_CTRL;                        /*!< 0xA60 */
    __IO uint32_t TIMER_PAUSE_INTR_STS;                            /*!< 0xA64 */
    __I  uint32_t TIMER_PAUSE_INTR;                                /*!< 0xA68 */
    __I  uint32_t TIMER_DMY;                                       /*!< 0xA6C */
    __IO uint32_t TIMER_Q_CH_FW_CTRL0;                             /*!< 0xA70 */
    __IO uint32_t TIMER_Q_CH_FW_CTRL1;                             /*!< 0xA74 */
    __I  uint32_t TIMER_Q_CH_DMY_0;                                /*!< 0xA78 */
    __I  uint32_t TIMER_Q_CH_DMY_1;                                /*!< 0xA7C */
    __IO uint32_t TIMER_RAP_CTRL0;                                 /*!< 0xA80 */
    __IO uint32_t TIMER_RAP_CTRL1;                                 /*!< 0xA84 */
    __IO uint32_t TIMER_RAP_FW_TSAK_START;                         /*!< 0xA88 */
    __IO uint32_t TIMER_RAP_FW_TASK_PAUSE;                         /*!< 0xA8C */
    __IO uint32_t TIMER_RAP_FW_TASK_DISABLE;                       /*!< 0xA90 */
    __I  uint32_t TIMER_RAP_STS_ACTIVE;                            /*!< 0xA94 */
    __IO uint32_t TIMER_RAP_SHORTCUT_SUB_TASK_PAUSE;               /*!< 0xA98 */
    __IO uint32_t TIMER_RAP_SHORTCUT_SUB_TASK_DISABLE;             /*!< 0xA9C */
    __IO uint32_t TIMER_RAP_SHORTCUT_PUB_EVENT_TIMEOUT;            /*!< 0xAA0 */
    __I  uint32_t TIMER_RAP_DMY_0;                                 /*!< 0xAA4 */
    __I  uint32_t TIMER_RAP_DMY_1;                                 /*!< 0xAA8 */
    __I  uint32_t TIMER_RAP_DMY_2;                                 /*!< 0xAAC */
    __I  uint32_t TIMER_IP_VER;                                    /*!< 0xAB0 */
} TIM_ShareTypeDef;

/*============================================================================*
 *                         TIM Declaration
 *============================================================================*/
/** \defgroup TIM      TIM
  * \brief
  * \{
  */

/** \defgroup TIM_Exported_Constants TIM Exported Constants
  * \brief
  * \{
  */

/** \defgroup TIM_Declaration TIM Declaration
  * \{
  * \ingroup  TIM_Exported_Constants
  */

#define TIMER0_SHARE                 ((TIM_ShareTypeDef *) (TIMER0_BASE + 0xA00))
#define TIMER1_SHARE                 ((TIM_ShareTypeDef *) (TIMER1_BASE + 0xA00))

/*============================================================================*
 *                         TIM0 Declaration
 *============================================================================*/
#define TIMER0_CH0_REG_BASE          (TIMER0_BASE + 0x0000)
#define TIMER0_CH1_REG_BASE          (TIMER0_BASE + 0x0050)

#define TIM0_CH0                     ((TIM_TypeDef *) TIMER0_CH0_REG_BASE)
#define TIM0_CH1                     ((TIM_TypeDef *) TIMER0_CH1_REG_BASE)

#define IS_TIM0_PERIPH(PERIPH)       (((PERIPH) == TIM0_CH0) || \
                                      ((PERIPH) == TIM0_CH1))

/*============================================================================*
 *                         TIM1 Declaration
 *============================================================================*/
#define TIMER1_CH0_REG_BASE          (TIMER1_BASE + 0x0000)
#define TIMER1_CH1_REG_BASE          (TIMER1_BASE + 0x0050)
#define TIMER1_CH2_REG_BASE          (TIMER1_BASE + 0x00A0)
#define TIMER1_CH3_REG_BASE          (TIMER1_BASE + 0x00F0)
#define TIMER1_CH4_REG_BASE          (TIMER1_BASE + 0x0140)
#define TIMER1_CH5_REG_BASE          (TIMER1_BASE + 0x0190)
#define TIMER1_CH6_REG_BASE          (TIMER1_BASE + 0x01E0)
#define TIMER1_CH7_REG_BASE          (TIMER1_BASE + 0x0230)
#define TIMER1_CH8_REG_BASE          (TIMER1_BASE + 0x0280)
#define TIMER1_CH9_REG_BASE          (TIMER1_BASE + 0x02D0)

#define TIM1_CH0                     ((TIM_TypeDef *) TIMER1_CH0_REG_BASE)
#define TIM1_CH1                     ((TIM_TypeDef *) TIMER1_CH1_REG_BASE)
#define TIM1_CH2                     ((TIM_TypeDef *) TIMER1_CH2_REG_BASE)
#define TIM1_CH3                     ((TIM_TypeDef *) TIMER1_CH3_REG_BASE)
#define TIM1_CH4                     ((TIM_TypeDef *) TIMER1_CH4_REG_BASE)
#define TIM1_CH5                     ((TIM_TypeDef *) TIMER1_CH5_REG_BASE)
#define TIM1_CH6                     ((TIM_TypeDef *) TIMER1_CH6_REG_BASE)
#define TIM1_CH7                     ((TIM_TypeDef *) TIMER1_CH7_REG_BASE)
#define TIM1_CH8                     ((TIM_TypeDef *) TIMER1_CH8_REG_BASE)
#define TIM1_CH9                     ((TIM_TypeDef *) TIMER1_CH9_REG_BASE)

#define IS_TIM1_PERIPH(PERIPH)       (((PERIPH) == TIM1_CH0) || \
                                      ((PERIPH) == TIM1_CH1) || \
                                      ((PERIPH) == TIM1_CH2) || \
                                      ((PERIPH) == TIM1_CH3) || \
                                      ((PERIPH) == TIM1_CH4) || \
                                      ((PERIPH) == TIM1_CH5) || \
                                      ((PERIPH) == TIM1_CH6) || \
                                      ((PERIPH) == TIM1_CH7) || \
                                      ((PERIPH) == TIM1_CH8) || \
                                      ((PERIPH) == TIM1_CH9))

#define IS_TIM_ALL_PERIPH(PERIPH)    (IS_TIM0_PERIPH(PERIPH) || \
                                      IS_TIM1_PERIPH(PERIPH))

/** End of TIM_Declaration
  * \}
  */

/** End of TIM_Exported_Constants
  * \}
  */

/** End of TIM
  * \}
  */

/*============================================================================*
 *                         TIM Registers and Field Descriptions
 *============================================================================*/
/* 0x00
   31:0    R      reg_timer_cur_cnt                         32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reg_timer_cur_cnt: 32;
    } b;
} TIMER_CUR_CNT_TypeDef;


/* 0x04
   1:0     R/W    reg_timer_mode                            2'h0
   2       R/WAC  reg_timer_ccr_fifo_clr                    1'h0
   7:3     R      RESERVED8                                 5'h0
   8       R/W    reg_timer_oneshot_mode_en                 1'h0
   31:9    R      RESERVED6                                 23'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_mode: 2;
        uint32_t reg_timer_ccr_fifo_clr: 1;
        const uint32_t reserved_1: 5;
        uint32_t reg_timer_oneshot_mode_en: 1;
        const uint32_t reserved_0: 23;
    } b;
} TIMER_MODE_CFG_TypeDef;


/* 0x08
   31:0    R/W    reg_timer_max_cnt                         32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_max_cnt: 32;
    } b;
} TIMER_MAX_CNT_TypeDef;


/* 0x0C
   31:0    R/W    reg_timer_ccr                             32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_ccr: 32;
    } b;
} TIMER_CCR_TypeDef;


/* 0x10
   31:0    R/W    reg_timer_ccr_fifo                        32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_ccr_fifo: 32;
    } b;
} TIMER_CCR_FIFO_TypeDef;


/* 0x14
   0       R/W    reg_timer_pwm_en                          1'h0
   1       R/W    reg_timer_pwm_polarity                    1'h0
   2       R/W    reg_timer_pwm_open_drain_en               1'h0
   3       R/W    reg_timer_pwm_dz_en                       1'h0
   11:4    R/W    reg_timer_pwm_dz_size                     8'h0
   12      R/W    reg_timer_pwm_dz_emg_stop                 1'h0
   13      R/W    reg_timer_pwm_dz_stop_state_n             1'h0
   14      R/W    reg_timer_pwm_dz_stop_state_p             1'h0
   16:15   R/W    reg_timer_pwm_dz_ref_sel                  2'h0
   17      R/W    reg_timer_pwm_dz_ref_n_inv_en             1'h0
   18      R/W    reg_timer_pwm_dz_ref_p_inv_en             1'h0
   19      R      RESERVED19                                1'h0
   31:20   R      RESERVED18                                12'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_pwm_en: 1;
        uint32_t reg_timer_pwm_polarity: 1;
        uint32_t reg_timer_pwm_open_drain_en: 1;
        uint32_t reg_timer_pwm_dz_en: 1;
        uint32_t reg_timer_pwm_dz_size: 8;
        uint32_t reg_timer_pwm_dz_emg_stop: 1;
        uint32_t reg_timer_pwm_dz_stop_state_n: 1;
        uint32_t reg_timer_pwm_dz_stop_state_p: 1;
        uint32_t reg_timer_pwm_dz_ref_sel: 2;
        uint32_t reg_timer_pwm_dz_ref_n_inv_en: 1;
        uint32_t reg_timer_pwm_dz_ref_p_inv_en: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 12;
    } b;
} TIMER_PWM_CFG_TypeDef;


/* 0x18
   31:0    R/W    reg_timer_pwm_phase_shift_cnt             32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_pwm_phase_shift_cnt: 32;
    } b;
} TIMER_PWM_SHIFT_CNT_TypeDef;


/* 0x1C
   0       R/W    reg_timer_latch_cnt_0_en                  1'h0
   2:1     R/W    reg_timer_latch_cnt_0_trig_mode           2'h0
   3       R/WAC  reg_timer_latch_cnt_0_fifo_clr            1'h0
   7:4     R      RESERVED36                                4'h0
   15:8    R/W    reg_timer_latch_cnt_0_fifo_th             8'h1
   31:16   R      RESERVED34                                16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_latch_cnt_0_en: 1;
        uint32_t reg_timer_latch_cnt_0_trig_mode: 2;
        uint32_t reg_timer_latch_cnt_0_fifo_clr: 1;
        const uint32_t reserved_1: 4;
        uint32_t reg_timer_latch_cnt_0_fifo_th: 8;
        const uint32_t reserved_0: 16;
    } b;
} TIMER_LAT_CNT_0_CFG_TypeDef;


/* 0x20
   31:0    R      reg_timer_latch_cnt_0                     32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reg_timer_latch_cnt_0: 32;
    } b;
} TIMER_LAT_CNT_0_TypeDef;


/* 0x24
   7:0     R      reg_timer_latch_cnt_0_fifo_depth          8'h0
   31:8    R      RESERVED43                                  24'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reg_timer_latch_cnt_0_fifo_depth: 8;
        const uint32_t reserved_0: 24;
    } b;
} TIMER_LAT_CNT_0_FIFO_DEPTH_TypeDef;


/* 0x28
   0       R/W    reg_timer_latch_cnt_1_en                  1'h0
   2:1     R/W    reg_timer_latch_cnt_1_trig_mode           2'h0
   3       R      RESERVED49                                1'h0
   7:4     R      RESERVED48                                4'h0
   15:8    R      RESERVED47                                8'h1
   31:16   R      RESERVED46                                16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_latch_cnt_1_en: 1;
        uint32_t reg_timer_latch_cnt_1_trig_mode: 2;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 4;
        const uint32_t reserved_1: 8;
        const uint32_t reserved_0: 16;
    } b;
} TIMER_LAT_CNT_1_CFG_TypeDef;


/* 0x2C
   31:0    R      reg_timer_latch_cnt_1                     32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reg_timer_latch_cnt_1: 32;
    } b;
} TIMER_LAT_CNT_1_TypeDef;


/* 0x30
   7:0     R      RESERVED56                                8'h0
   31:8    R      RESERVED55                                24'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_1: 8;
        const uint32_t reserved_0: 24;
    } b;
} TIMER_LAT_CNT_1_FIFO_DEPTH_TypeDef;


/* 0x34
   0       R/W    reg_timer_latch_cnt_2_en                  1'h0
   2:1     R/W    reg_timer_latch_cnt_2_trig_mode           2'h0
   3       R      RESERVED61                                1'h0
   7:4     R      RESERVED60                                4'h0
   15:8    R      RESERVED59                                8'h1
   31:16   R      RESERVED58                                16'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_latch_cnt_2_en: 1;
        uint32_t reg_timer_latch_cnt_2_trig_mode: 2;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 4;
        const uint32_t reserved_1: 8;
        const uint32_t reserved_0: 16;
    } b;
} TIMER_LAT_CNT_2_CFG_TypeDef;


/* 0x38
   31:0    R      reg_timer_latch_cnt_2                     32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reg_timer_latch_cnt_2: 32;
    } b;
} TIMER_LAT_CNT_2_TypeDef;


/* 0x3C
   7:0     R      RESERVED68                                8'h0
   31:8    R      RESERVED67                                24'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_1: 8;
        const uint32_t reserved_0: 24;
    } b;
} TIMER_LAT_CNT_2_FIFO_DEPTH_TypeDef;


/* 0x40
   0       R/W    reg_timer_dma_en                          1'h0
   1       R/W    reg_timer_dma_mode                        1'h0
   3:2     R/W    reg_timer_dma_target                      2'h0
   31:4    R      RESERVED70                                28'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_dma_en: 1;
        uint32_t reg_timer_dma_mode: 1;
        uint32_t reg_timer_dma_target: 2;
        const uint32_t reserved_0: 28;
    } b;
} TIMER_DMA_CFG_TypeDef;


/* 0x44
   31:0    R      RESERVED75                                32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_0: 32;
    } b;
} TIMER_DMY_0_TypeDef;


/* 0x48
   31:0    R      RESERVED77                                32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_0: 32;
    } b;
} TIMER_DMY_1_TypeDef;


/* 0x4C
   31:0    R      RESERVED79                                32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_0: 32;
    } b;
} TIMER_DMY_2_TypeDef;


/* 0xA00
   0       R/W    reg_timer_0_en                              1'h0
   1       R/W    reg_timer_1_en                              1'h0
   2       R/W    reg_timer_2_en                              1'h0
   3       R/W    reg_timer_3_en                              1'h0
   4       R/W    reg_timer_4_en                              1'h0
   5       R/W    reg_timer_5_en                              1'h0
   6       R/W    reg_timer_6_en                              1'h0
   7       R/W    reg_timer_7_en                              1'h0
   8       R/W    reg_timer_8_en                              1'h0
   9       R/W    reg_timer_9_en                              1'h0
   10      R      RESERVED2489                                1'h0
   11      R      RESERVED2488                                1'h0
   12      R      RESERVED2487                                1'h0
   13      R      RESERVED2486                                1'h0
   14      R      RESERVED2485                                1'h0
   15      R      RESERVED2484                                1'h0
   16      R      RESERVED2483                                1'h0
   17      R      RESERVED2482                                1'h0
   18      R      RESERVED2481                                1'h0
   19      R      RESERVED2480                                1'h0
   20      R      RESERVED2479                                1'h0
   21      R      RESERVED2478                                1'h0
   22      R      RESERVED2477                                1'h0
   23      R      RESERVED2476                                1'h0
   24      R      RESERVED2475                                1'h0
   25      R      RESERVED2474                                1'h0
   26      R      RESERVED2473                                1'h0
   27      R      RESERVED2472                                1'h0
   28      R      RESERVED2471                                1'h0
   29      R      RESERVED2470                                1'h0
   30      R      RESERVED2469                                1'h0
   31      R      RESERVED2468                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_0_en: 1;
        uint32_t reg_timer_1_en: 1;
        uint32_t reg_timer_2_en: 1;
        uint32_t reg_timer_3_en: 1;
        uint32_t reg_timer_4_en: 1;
        uint32_t reg_timer_5_en: 1;
        uint32_t reg_timer_6_en: 1;
        uint32_t reg_timer_7_en: 1;
        uint32_t reg_timer_8_en: 1;
        uint32_t reg_timer_9_en: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_EN_CTRL_TypeDef;


/* 0xA04
   0       R/WAC  reg_timer_0_oneshot_go                      1'h0
   1       R/WAC  reg_timer_1_oneshot_go                      1'h0
   2       R/WAC  reg_timer_2_oneshot_go                      1'h0
   3       R/WAC  reg_timer_3_oneshot_go                      1'h0
   4       R/WAC  reg_timer_4_oneshot_go                      1'h0
   5       R/WAC  reg_timer_5_oneshot_go                      1'h0
   6       R/WAC  reg_timer_6_oneshot_go                      1'h0
   7       R/WAC  reg_timer_7_oneshot_go                      1'h0
   8       R/WAC  reg_timer_8_oneshot_go                      1'h0
   9       R/WAC  reg_timer_9_oneshot_go                      1'h0
   10      R      RESERVED2522                                1'h0
   11      R      RESERVED2521                                1'h0
   12      R      RESERVED2520                                1'h0
   13      R      RESERVED2519                                1'h0
   14      R      RESERVED2518                                1'h0
   15      R      RESERVED2517                                1'h0
   16      R      RESERVED2516                                1'h0
   17      R      RESERVED2515                                1'h0
   18      R      RESERVED2514                                1'h0
   19      R      RESERVED2513                                1'h0
   20      R      RESERVED2512                                1'h0
   21      R      RESERVED2511                                1'h0
   22      R      RESERVED2510                                1'h0
   23      R      RESERVED2509                                1'h0
   24      R      RESERVED2508                                1'h0
   25      R      RESERVED2507                                1'h0
   26      R      RESERVED2506                                1'h0
   27      R      RESERVED2505                                1'h0
   28      R      RESERVED2504                                1'h0
   29      R      RESERVED2503                                1'h0
   30      R      RESERVED2502                                1'h0
   31      R      RESERVED2501                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_0_oneshot_go: 1;
        uint32_t reg_timer_1_oneshot_go: 1;
        uint32_t reg_timer_2_oneshot_go: 1;
        uint32_t reg_timer_3_oneshot_go: 1;
        uint32_t reg_timer_4_oneshot_go: 1;
        uint32_t reg_timer_5_oneshot_go: 1;
        uint32_t reg_timer_6_oneshot_go: 1;
        uint32_t reg_timer_7_oneshot_go: 1;
        uint32_t reg_timer_8_oneshot_go: 1;
        uint32_t reg_timer_9_oneshot_go: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_ONESHOT_GO_CTRL_TypeDef;


/* 0xA08
   0       R      RESERVED2565                                1'h0
   1       R      RESERVED2564                                1'h0
   2       R      RESERVED2563                                1'h0
   3       R      RESERVED2562                                1'h0
   4       R      reg_timer_4_toggle_state                    1'h0
   5       R      reg_timer_5_toggle_state                    1'h0
   6       R      reg_timer_6_toggle_state                    1'h0
   7       R      reg_timer_7_toggle_state                    1'h0
   8       R      reg_timer_8_toggle_state                    1'h0
   9       R      reg_timer_9_toggle_state                    1'h0
   10      R      RESERVED2555                                1'h0
   11      R      RESERVED2554                                1'h0
   12      R      RESERVED2553                                1'h0
   13      R      RESERVED2552                                1'h0
   14      R      RESERVED2551                                1'h0
   15      R      RESERVED2550                                1'h0
   16      R      RESERVED2549                                1'h0
   17      R      RESERVED2548                                1'h0
   18      R      RESERVED2547                                1'h0
   19      R      RESERVED2546                                1'h0
   20      R      RESERVED2545                                1'h0
   21      R      RESERVED2544                                1'h0
   22      R      RESERVED2543                                1'h0
   23      R      RESERVED2542                                1'h0
   24      R      RESERVED2541                                1'h0
   25      R      RESERVED2540                                1'h0
   26      R      RESERVED2539                                1'h0
   27      R      RESERVED2538                                1'h0
   28      R      RESERVED2537                                1'h0
   29      R      RESERVED2536                                1'h0
   30      R      RESERVED2535                                1'h0
   31      R      RESERVED2534                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reg_timer_4_toggle_state: 1;
        const uint32_t reg_timer_5_toggle_state: 1;
        const uint32_t reg_timer_6_toggle_state: 1;
        const uint32_t reg_timer_7_toggle_state: 1;
        const uint32_t reg_timer_8_toggle_state: 1;
        const uint32_t reg_timer_9_toggle_state: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_PWM_STATE_TypeDef;


/* 0xA0C
   0       R/W    reg_timer_0_intr_en                         1'h0
   1       R/W    reg_timer_1_intr_en                         1'h0
   2       R/W    reg_timer_2_intr_en                         1'h0
   3       R/W    reg_timer_3_intr_en                         1'h0
   4       R/W    reg_timer_4_intr_en                         1'h0
   5       R/W    reg_timer_5_intr_en                         1'h0
   6       R/W    reg_timer_6_intr_en                         1'h0
   7       R/W    reg_timer_7_intr_en                         1'h0
   8       R/W    reg_timer_8_intr_en                         1'h0
   9       R/W    reg_timer_9_intr_en                         1'h0
   10      R      RESERVED2588                                1'h0
   11      R      RESERVED2587                                1'h0
   12      R      RESERVED2586                                1'h0
   13      R      RESERVED2585                                1'h0
   14      R      RESERVED2584                                1'h0
   15      R      RESERVED2583                                1'h0
   16      R      RESERVED2582                                1'h0
   17      R      RESERVED2581                                1'h0
   18      R      RESERVED2580                                1'h0
   19      R      RESERVED2579                                1'h0
   20      R      RESERVED2578                                1'h0
   21      R      RESERVED2577                                1'h0
   22      R      RESERVED2576                                1'h0
   23      R      RESERVED2575                                1'h0
   24      R      RESERVED2574                                1'h0
   25      R      RESERVED2573                                1'h0
   26      R      RESERVED2572                                1'h0
   27      R      RESERVED2571                                1'h0
   28      R      RESERVED2570                                1'h0
   29      R      RESERVED2569                                1'h0
   30      R      RESERVED2568                                1'h0
   31      R      RESERVED2567                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_0_intr_en: 1;
        uint32_t reg_timer_1_intr_en: 1;
        uint32_t reg_timer_2_intr_en: 1;
        uint32_t reg_timer_3_intr_en: 1;
        uint32_t reg_timer_4_intr_en: 1;
        uint32_t reg_timer_5_intr_en: 1;
        uint32_t reg_timer_6_intr_en: 1;
        uint32_t reg_timer_7_intr_en: 1;
        uint32_t reg_timer_8_intr_en: 1;
        uint32_t reg_timer_9_intr_en: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_INTR_EN_CTRL_TypeDef;


/* 0xA10
   0       R/W1C  reg_timer_0_intr_sts                        1'h0
   1       R/W1C  reg_timer_1_intr_sts                        1'h0
   2       R/W1C  reg_timer_2_intr_sts                        1'h0
   3       R/W1C  reg_timer_3_intr_sts                        1'h0
   4       R/W1C  reg_timer_4_intr_sts                        1'h0
   5       R/W1C  reg_timer_5_intr_sts                        1'h0
   6       R/W1C  reg_timer_6_intr_sts                        1'h0
   7       R/W1C  reg_timer_7_intr_sts                        1'h0
   8       R/W1C  reg_timer_8_intr_sts                        1'h0
   9       R/W1C  reg_timer_9_intr_sts                        1'h0
   10      R      RESERVED2621                                1'h0
   11      R      RESERVED2620                                1'h0
   12      R      RESERVED2619                                1'h0
   13      R      RESERVED2618                                1'h0
   14      R      RESERVED2617                                1'h0
   15      R      RESERVED2616                                1'h0
   16      R      RESERVED2615                                1'h0
   17      R      RESERVED2614                                1'h0
   18      R      RESERVED2613                                1'h0
   19      R      RESERVED2612                                1'h0
   20      R      RESERVED2611                                1'h0
   21      R      RESERVED2610                                1'h0
   22      R      RESERVED2609                                1'h0
   23      R      RESERVED2608                                1'h0
   24      R      RESERVED2607                                1'h0
   25      R      RESERVED2606                                1'h0
   26      R      RESERVED2605                                1'h0
   27      R      RESERVED2604                                1'h0
   28      R      RESERVED2603                                1'h0
   29      R      RESERVED2602                                1'h0
   30      R      RESERVED2601                                1'h0
   31      R      RESERVED2600                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_0_intr_sts: 1;
        uint32_t reg_timer_1_intr_sts: 1;
        uint32_t reg_timer_2_intr_sts: 1;
        uint32_t reg_timer_3_intr_sts: 1;
        uint32_t reg_timer_4_intr_sts: 1;
        uint32_t reg_timer_5_intr_sts: 1;
        uint32_t reg_timer_6_intr_sts: 1;
        uint32_t reg_timer_7_intr_sts: 1;
        uint32_t reg_timer_8_intr_sts: 1;
        uint32_t reg_timer_9_intr_sts: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_INTR_STS_TypeDef;


/* 0xA14
   0       R      reg_timer_0_intr_sts_masked                 1'h0
   1       R      reg_timer_1_intr_sts_masked                 1'h0
   2       R      reg_timer_2_intr_sts_masked                 1'h0
   3       R      reg_timer_3_intr_sts_masked                 1'h0
   4       R      reg_timer_4_intr_sts_masked                 1'h0
   5       R      reg_timer_5_intr_sts_masked                 1'h0
   6       R      reg_timer_6_intr_sts_masked                 1'h0
   7       R      reg_timer_7_intr_sts_masked                 1'h0
   8       R      reg_timer_8_intr_sts_masked                 1'h0
   9       R      reg_timer_9_intr_sts_masked                 1'h0
   10      R      RESERVED2654                                1'h0
   11      R      RESERVED2653                                1'h0
   12      R      RESERVED2652                                1'h0
   13      R      RESERVED2651                                1'h0
   14      R      RESERVED2650                                1'h0
   15      R      RESERVED2649                                1'h0
   16      R      RESERVED2648                                1'h0
   17      R      RESERVED2647                                1'h0
   18      R      RESERVED2646                                1'h0
   19      R      RESERVED2645                                1'h0
   20      R      RESERVED2644                                1'h0
   21      R      RESERVED2643                                1'h0
   22      R      RESERVED2642                                1'h0
   23      R      RESERVED2641                                1'h0
   24      R      RESERVED2640                                1'h0
   25      R      RESERVED2639                                1'h0
   26      R      RESERVED2638                                1'h0
   27      R      RESERVED2637                                1'h0
   28      R      RESERVED2636                                1'h0
   29      R      RESERVED2635                                1'h0
   30      R      RESERVED2634                                1'h0
   31      R      RESERVED2633                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reg_timer_0_intr_sts_masked: 1;
        const uint32_t reg_timer_1_intr_sts_masked: 1;
        const uint32_t reg_timer_2_intr_sts_masked: 1;
        const uint32_t reg_timer_3_intr_sts_masked: 1;
        const uint32_t reg_timer_4_intr_sts_masked: 1;
        const uint32_t reg_timer_5_intr_sts_masked: 1;
        const uint32_t reg_timer_6_intr_sts_masked: 1;
        const uint32_t reg_timer_7_intr_sts_masked: 1;
        const uint32_t reg_timer_8_intr_sts_masked: 1;
        const uint32_t reg_timer_9_intr_sts_masked: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_INTR_TypeDef;


/* 0xA18
   0       R      RESERVED2697                                1'h0
   1       R      RESERVED2696                                1'h0
   2       R      RESERVED2695                                1'h0
   3       R      RESERVED2694                                1'h0
   4       R      RESERVED2693                                1'h0
   5       R      RESERVED2692                                1'h0
   6       R      reg_timer_6_ccr_fifo_empty                  1'h1
   7       R      reg_timer_7_ccr_fifo_empty                  1'h1
   8       R      reg_timer_8_ccr_fifo_empty                  1'h1
   9       R      reg_timer_9_ccr_fifo_empty                  1'h1
   10      R      RESERVED2687                                1'h0
   11      R      RESERVED2686                                1'h0
   12      R      RESERVED2685                                1'h0
   13      R      RESERVED2684                                1'h0
   14      R      RESERVED2683                                1'h0
   15      R      RESERVED2682                                1'h0
   16      R      RESERVED2681                                1'h0
   17      R      RESERVED2680                                1'h0
   18      R      RESERVED2679                                1'h0
   19      R      RESERVED2678                                1'h0
   20      R      RESERVED2677                                1'h0
   21      R      RESERVED2676                                1'h0
   22      R      RESERVED2675                                1'h0
   23      R      RESERVED2674                                1'h0
   24      R      RESERVED2673                                1'h0
   25      R      RESERVED2672                                1'h0
   26      R      RESERVED2671                                1'h0
   27      R      RESERVED2670                                1'h0
   28      R      RESERVED2669                                1'h0
   29      R      RESERVED2668                                1'h0
   30      R      RESERVED2667                                1'h0
   31      R      RESERVED2666                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reg_timer_6_ccr_fifo_empty: 1;
        const uint32_t reg_timer_7_ccr_fifo_empty: 1;
        const uint32_t reg_timer_8_ccr_fifo_empty: 1;
        const uint32_t reg_timer_9_ccr_fifo_empty: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_CCR_FIFO_EMPTY_STS_TypeDef;


/* 0xA1C
   0       R      RESERVED2730                                1'h0
   1       R      RESERVED2729                                1'h0
   2       R      RESERVED2728                                1'h0
   3       R      RESERVED2727                                1'h0
   4       R      RESERVED2726                                1'h0
   5       R      RESERVED2725                                1'h0
   6       R      reg_timer_6_ccr_fifo_full                   1'h0
   7       R      reg_timer_7_ccr_fifo_full                   1'h0
   8       R      reg_timer_8_ccr_fifo_full                   1'h0
   9       R      reg_timer_9_ccr_fifo_full                   1'h0
   10      R      RESERVED2720                                1'h0
   11      R      RESERVED2719                                1'h0
   12      R      RESERVED2718                                1'h0
   13      R      RESERVED2717                                1'h0
   14      R      RESERVED2716                                1'h0
   15      R      RESERVED2715                                1'h0
   16      R      RESERVED2714                                1'h0
   17      R      RESERVED2713                                1'h0
   18      R      RESERVED2712                                1'h0
   19      R      RESERVED2711                                1'h0
   20      R      RESERVED2710                                1'h0
   21      R      RESERVED2709                                1'h0
   22      R      RESERVED2708                                1'h0
   23      R      RESERVED2707                                1'h0
   24      R      RESERVED2706                                1'h0
   25      R      RESERVED2705                                1'h0
   26      R      RESERVED2704                                1'h0
   27      R      RESERVED2703                                1'h0
   28      R      RESERVED2702                                1'h0
   29      R      RESERVED2701                                1'h0
   30      R      RESERVED2700                                1'h0
   31      R      RESERVED2699                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reg_timer_6_ccr_fifo_full: 1;
        const uint32_t reg_timer_7_ccr_fifo_full: 1;
        const uint32_t reg_timer_8_ccr_fifo_full: 1;
        const uint32_t reg_timer_9_ccr_fifo_full: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_CCR_FIFO_FULL_STS_TypeDef;


/* 0xA20
   0       R      RESERVED2763                              1'h0
   1       R      RESERVED2762                              1'h0
   2       R      RESERVED2761                              1'h0
   3       R      RESERVED2760                              1'h0
   4       R      RESERVED2759                              1'h0
   5       R      RESERVED2758                              1'h0
   6       R      RESERVED2757                              1'h0
   7       R      RESERVED2756                              1'h0
   8       R      RESERVED2755                              1'h0
   9       R/W    reg_timer_9_latch_cnt_0_fifo_full_intr_en 1'h0
   10      R      RESERVED2753                              1'h0
   11      R      RESERVED2752                              1'h0
   12      R      RESERVED2751                              1'h0
   13      R      RESERVED2750                              1'h0
   14      R      RESERVED2749                              1'h0
   15      R      RESERVED2748                              1'h0
   16      R      RESERVED2747                              1'h0
   17      R      RESERVED2746                              1'h0
   18      R      RESERVED2745                              1'h0
   19      R      RESERVED2744                              1'h0
   20      R      RESERVED2743                              1'h0
   21      R      RESERVED2742                              1'h0
   22      R      RESERVED2741                              1'h0
   23      R      RESERVED2740                              1'h0
   24      R      RESERVED2739                              1'h0
   25      R      RESERVED2738                              1'h0
   26      R      RESERVED2737                              1'h0
   27      R      RESERVED2736                              1'h0
   28      R      RESERVED2735                              1'h0
   29      R      RESERVED2734                              1'h0
   30      R      RESERVED2733                              1'h0
   31      R      RESERVED2732                              1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        uint32_t reg_timer_9_latch_cnt_0_fifo_full_intr_en: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_LAT_CNT_0_FIFO_FULL_INTR_EN_CTRL_TypeDef;


/* 0xA24
   0       R      RESERVED2796                                1'h0
   1       R      RESERVED2795                                1'h0
   2       R      RESERVED2794                                1'h0
   3       R      RESERVED2793                                1'h0
   4       R      RESERVED2792                                1'h0
   5       R      RESERVED2791                                1'h0
   6       R      RESERVED2790                                1'h0
   7       R      RESERVED2789                                1'h0
   8       R      RESERVED2788                                1'h0
   9       R/W    reg_timer_9_latch_cnt_0_fifo_met_th_intr_en 1'h0
   10      R      RESERVED2786                                1'h0
   11      R      RESERVED2785                                1'h0
   12      R      RESERVED2784                                1'h0
   13      R      RESERVED2783                                1'h0
   14      R      RESERVED2782                                1'h0
   15      R      RESERVED2781                                1'h0
   16      R      RESERVED2780                                1'h0
   17      R      RESERVED2779                                1'h0
   18      R      RESERVED2778                                1'h0
   19      R      RESERVED2777                                1'h0
   20      R      RESERVED2776                                1'h0
   21      R      RESERVED2775                                1'h0
   22      R      RESERVED2774                                1'h0
   23      R      RESERVED2773                                1'h0
   24      R      RESERVED2772                                1'h0
   25      R      RESERVED2771                                1'h0
   26      R      RESERVED2770                                1'h0
   27      R      RESERVED2769                                1'h0
   28      R      RESERVED2768                                1'h0
   29      R      RESERVED2767                                1'h0
   30      R      RESERVED2766                                1'h0
   31      R      RESERVED2765                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        uint32_t reg_timer_9_latch_cnt_0_fifo_met_th_intr_en: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_LAT_CNT_0_FIFO_MET_THRESHOLD_INTR_EN_CTRL_TypeDef;


/* 0xA28
   0       R      RESERVED2829                              1'h1
   1       R      RESERVED2828                              1'h1
   2       R      RESERVED2827                              1'h1
   3       R      RESERVED2826                              1'h1
   4       R      RESERVED2825                              1'h1
   5       R      RESERVED2824                              1'h1
   6       R      RESERVED2823                              1'h1
   7       R      RESERVED2822                              1'h1
   8       R      RESERVED2821                              1'h1
   9       R      reg_timer_9_latch_cnt_0_fifo_empty        1'h1
   10      R      RESERVED2819                              1'h1
   11      R      RESERVED2818                              1'h1
   12      R      RESERVED2817                              1'h1
   13      R      RESERVED2816                              1'h1
   14      R      RESERVED2815                              1'h1
   15      R      RESERVED2814                              1'h1
   16      R      RESERVED2813                              1'h1
   17      R      RESERVED2812                              1'h1
   18      R      RESERVED2811                              1'h1
   19      R      RESERVED2810                              1'h1
   20      R      RESERVED2809                              1'h1
   21      R      RESERVED2808                              1'h1
   22      R      RESERVED2807                              1'h1
   23      R      RESERVED2806                              1'h1
   24      R      RESERVED2805                              1'h1
   25      R      RESERVED2804                              1'h1
   26      R      RESERVED2803                              1'h1
   27      R      RESERVED2802                              1'h1
   28      R      RESERVED2801                              1'h1
   29      R      RESERVED2800                              1'h1
   30      R      RESERVED2799                              1'h1
   31      R      RESERVED2798                              1'h1
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reg_timer_9_latch_cnt_0_fifo_empty: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_LAT_CNT_0_FIFO_EMPTY_STS_TypeDef;


/* 0xA2C
   0       R      RESERVED2862                              1'h0
   1       R      RESERVED2861                              1'h0
   2       R      RESERVED2860                              1'h0
   3       R      RESERVED2859                              1'h0
   4       R      RESERVED2858                              1'h0
   5       R      RESERVED2857                              1'h0
   6       R      RESERVED2856                              1'h0
   7       R      RESERVED2855                              1'h0
   8       R      RESERVED2854                              1'h0
   9       R/W1C  reg_timer_9_latch_cnt_0_fifo_full         1'h0
   10      R      RESERVED2852                              1'h0
   11      R      RESERVED2851                              1'h0
   12      R      RESERVED2850                              1'h0
   13      R      RESERVED2849                              1'h0
   14      R      RESERVED2848                              1'h0
   15      R      RESERVED2847                              1'h0
   16      R      RESERVED2846                              1'h0
   17      R      RESERVED2845                              1'h0
   18      R      RESERVED2844                              1'h0
   19      R      RESERVED2843                              1'h0
   20      R      RESERVED2842                              1'h0
   21      R      RESERVED2841                              1'h0
   22      R      RESERVED2840                              1'h0
   23      R      RESERVED2839                              1'h0
   24      R      RESERVED2838                              1'h0
   25      R      RESERVED2837                              1'h0
   26      R      RESERVED2836                              1'h0
   27      R      RESERVED2835                              1'h0
   28      R      RESERVED2834                              1'h0
   29      R      RESERVED2833                              1'h0
   30      R      RESERVED2832                              1'h0
   31      R      RESERVED2831                              1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        uint32_t reg_timer_9_latch_cnt_0_fifo_full: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_LAT_CNT_0_FIFO_FULL_INTR_TypeDef;


/* 0xA30
   0       R      RESERVED2895                              1'h0
   1       R      RESERVED2894                              1'h0
   2       R      RESERVED2893                              1'h0
   3       R      RESERVED2892                              1'h0
   4       R      RESERVED2891                              1'h0
   5       R      RESERVED2890                              1'h0
   6       R      RESERVED2889                              1'h0
   7       R      RESERVED2888                              1'h0
   8       R      RESERVED2887                              1'h0
   9       R/W1C  reg_timer_9_latch_cnt_0_fifo_met_th       1'h0
   10      R      RESERVED2885                              1'h0
   11      R      RESERVED2884                              1'h0
   12      R      RESERVED2883                              1'h0
   13      R      RESERVED2882                              1'h0
   14      R      RESERVED2881                              1'h0
   15      R      RESERVED2880                              1'h0
   16      R      RESERVED2879                              1'h0
   17      R      RESERVED2878                              1'h0
   18      R      RESERVED2877                              1'h0
   19      R      RESERVED2876                              1'h0
   20      R      RESERVED2875                              1'h0
   21      R      RESERVED2874                              1'h0
   22      R      RESERVED2873                              1'h0
   23      R      RESERVED2872                              1'h0
   24      R      RESERVED2871                              1'h0
   25      R      RESERVED2870                              1'h0
   26      R      RESERVED2869                              1'h0
   27      R      RESERVED2868                              1'h0
   28      R      RESERVED2867                              1'h0
   29      R      RESERVED2866                              1'h0
   30      R      RESERVED2865                              1'h0
   31      R      RESERVED2864                              1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        uint32_t reg_timer_9_latch_cnt_0_fifo_met_th: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_LAT_CNT_0_FIFO_MET_THRESHOLD_INTR_TypeDef;


/* 0xA34
   0       R      RESERVED2928                                1'h0
   1       R      RESERVED2927                                1'h0
   2       R      RESERVED2926                                1'h0
   3       R      RESERVED2925                                1'h0
   4       R      RESERVED2924                                1'h0
   5       R      RESERVED2923                                1'h0
   6       R      RESERVED2922                                1'h0
   7       R      RESERVED2921                                1'h0
   8       R      RESERVED2920                                1'h0
   9       R      RESERVED2919                                1'h0
   10      R      RESERVED2918                                1'h0
   11      R      RESERVED2917                                1'h0
   12      R      RESERVED2916                                1'h0
   13      R      RESERVED2915                                1'h0
   14      R      RESERVED2914                                1'h0
   15      R      RESERVED2913                                1'h0
   16      R      RESERVED2912                                1'h0
   17      R      RESERVED2911                                1'h0
   18      R      RESERVED2910                                1'h0
   19      R      RESERVED2909                                1'h0
   20      R      RESERVED2908                                1'h0
   21      R      RESERVED2907                                1'h0
   22      R      RESERVED2906                                1'h0
   23      R      RESERVED2905                                1'h0
   24      R      RESERVED2904                                1'h0
   25      R      RESERVED2903                                1'h0
   26      R      RESERVED2902                                1'h0
   27      R      RESERVED2901                                1'h0
   28      R      RESERVED2900                                1'h0
   29      R      RESERVED2899                                1'h0
   30      R      RESERVED2898                                1'h0
   31      R      RESERVED2897                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_LAT_CNT_1_FIFO_FULL_INTR_EN_CTRL_TypeDef;


/* 0xA38
   0       R      RESERVED2961                                1'h0
   1       R      RESERVED2960                                1'h0
   2       R      RESERVED2959                                1'h0
   3       R      RESERVED2958                                1'h0
   4       R      RESERVED2957                                1'h0
   5       R      RESERVED2956                                1'h0
   6       R      RESERVED2955                                1'h0
   7       R      RESERVED2954                                1'h0
   8       R      RESERVED2953                                1'h0
   9       R      RESERVED2952                                1'h0
   10      R      RESERVED2951                                1'h0
   11      R      RESERVED2950                                1'h0
   12      R      RESERVED2949                                1'h0
   13      R      RESERVED2948                                1'h0
   14      R      RESERVED2947                                1'h0
   15      R      RESERVED2946                                1'h0
   16      R      RESERVED2945                                1'h0
   17      R      RESERVED2944                                1'h0
   18      R      RESERVED2943                                1'h0
   19      R      RESERVED2942                                1'h0
   20      R      RESERVED2941                                1'h0
   21      R      RESERVED2940                                1'h0
   22      R      RESERVED2939                                1'h0
   23      R      RESERVED2938                                1'h0
   24      R      RESERVED2937                                1'h0
   25      R      RESERVED2936                                1'h0
   26      R      RESERVED2935                                1'h0
   27      R      RESERVED2934                                1'h0
   28      R      RESERVED2933                                1'h0
   29      R      RESERVED2932                                1'h0
   30      R      RESERVED2931                                1'h0
   31      R      RESERVED2930                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_LAT_CNT_1_FIFO_MET_THRESHOLD_INTR_EN_CTRL_TypeDef;


/* 0xA3C
   0       R      RESERVED2994                                1'h1
   1       R      RESERVED2993                                1'h1
   2       R      RESERVED2992                                1'h1
   3       R      RESERVED2991                                1'h1
   4       R      RESERVED2990                                1'h1
   5       R      RESERVED2989                                1'h1
   6       R      RESERVED2988                                1'h1
   7       R      RESERVED2987                                1'h1
   8       R      RESERVED2986                                1'h1
   9       R      RESERVED2985                                1'h1
   10      R      RESERVED2984                                1'h1
   11      R      RESERVED2983                                1'h1
   12      R      RESERVED2982                                1'h1
   13      R      RESERVED2981                                1'h1
   14      R      RESERVED2980                                1'h1
   15      R      RESERVED2979                                1'h1
   16      R      RESERVED2978                                1'h1
   17      R      RESERVED2977                                1'h1
   18      R      RESERVED2976                                1'h1
   19      R      RESERVED2975                                1'h1
   20      R      RESERVED2974                                1'h1
   21      R      RESERVED2973                                1'h1
   22      R      RESERVED2972                                1'h1
   23      R      RESERVED2971                                1'h1
   24      R      RESERVED2970                                1'h1
   25      R      RESERVED2969                                1'h1
   26      R      RESERVED2968                                1'h1
   27      R      RESERVED2967                                1'h1
   28      R      RESERVED2966                                1'h1
   29      R      RESERVED2965                                1'h1
   30      R      RESERVED2964                                1'h1
   31      R      RESERVED2963                                1'h1
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_LAT_CNT_1_FIFO_EMPTY_STS_TypeDef;


/* 0xA40
   0       R      RESERVED3027                                1'h0
   1       R      RESERVED3026                                1'h0
   2       R      RESERVED3025                                1'h0
   3       R      RESERVED3024                                1'h0
   4       R      RESERVED3023                                1'h0
   5       R      RESERVED3022                                1'h0
   6       R      RESERVED3021                                1'h0
   7       R      RESERVED3020                                1'h0
   8       R      RESERVED3019                                1'h0
   9       R      RESERVED3018                                1'h0
   10      R      RESERVED3017                                1'h0
   11      R      RESERVED3016                                1'h0
   12      R      RESERVED3015                                1'h0
   13      R      RESERVED3014                                1'h0
   14      R      RESERVED3013                                1'h0
   15      R      RESERVED3012                                1'h0
   16      R      RESERVED3011                                1'h0
   17      R      RESERVED3010                                1'h0
   18      R      RESERVED3009                                1'h0
   19      R      RESERVED3008                                1'h0
   20      R      RESERVED3007                                1'h0
   21      R      RESERVED3006                                1'h0
   22      R      RESERVED3005                                1'h0
   23      R      RESERVED3004                                1'h0
   24      R      RESERVED3003                                1'h0
   25      R      RESERVED3002                                1'h0
   26      R      RESERVED3001                                1'h0
   27      R      RESERVED3000                                1'h0
   28      R      RESERVED2999                                1'h0
   29      R      RESERVED2998                                1'h0
   30      R      RESERVED2997                                1'h0
   31      R      RESERVED2996                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_LAT_CNT_1_FIFO_FULL_INTR_TypeDef;


/* 0xA44
   0       R      RESERVED3060                                1'h0
   1       R      RESERVED3059                                1'h0
   2       R      RESERVED3058                                1'h0
   3       R      RESERVED3057                                1'h0
   4       R      RESERVED3056                                1'h0
   5       R      RESERVED3055                                1'h0
   6       R      RESERVED3054                                1'h0
   7       R      RESERVED3053                                1'h0
   8       R      RESERVED3052                                1'h0
   9       R      RESERVED3051                                1'h0
   10      R      RESERVED3050                                1'h0
   11      R      RESERVED3049                                1'h0
   12      R      RESERVED3048                                1'h0
   13      R      RESERVED3047                                1'h0
   14      R      RESERVED3046                                1'h0
   15      R      RESERVED3045                                1'h0
   16      R      RESERVED3044                                1'h0
   17      R      RESERVED3043                                1'h0
   18      R      RESERVED3042                                1'h0
   19      R      RESERVED3041                                1'h0
   20      R      RESERVED3040                                1'h0
   21      R      RESERVED3039                                1'h0
   22      R      RESERVED3038                                1'h0
   23      R      RESERVED3037                                1'h0
   24      R      RESERVED3036                                1'h0
   25      R      RESERVED3035                                1'h0
   26      R      RESERVED3034                                1'h0
   27      R      RESERVED3033                                1'h0
   28      R      RESERVED3032                                1'h0
   29      R      RESERVED3031                                1'h0
   30      R      RESERVED3030                                1'h0
   31      R      RESERVED3029                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_LAT_CNT_1_FIFO_MET_THRESHOLD_INTR_TypeDef;


/* 0xA48
   0       R      RESERVED3093                                1'h0
   1       R      RESERVED3092                                1'h0
   2       R      RESERVED3091                                1'h0
   3       R      RESERVED3090                                1'h0
   4       R      RESERVED3089                                1'h0
   5       R      RESERVED3088                                1'h0
   6       R      RESERVED3087                                1'h0
   7       R      RESERVED3086                                1'h0
   8       R      RESERVED3085                                1'h0
   9       R      RESERVED3084                                1'h0
   10      R      RESERVED3083                                1'h0
   11      R      RESERVED3082                                1'h0
   12      R      RESERVED3081                                1'h0
   13      R      RESERVED3080                                1'h0
   14      R      RESERVED3079                                1'h0
   15      R      RESERVED3078                                1'h0
   16      R      RESERVED3077                                1'h0
   17      R      RESERVED3076                                1'h0
   18      R      RESERVED3075                                1'h0
   19      R      RESERVED3074                                1'h0
   20      R      RESERVED3073                                1'h0
   21      R      RESERVED3072                                1'h0
   22      R      RESERVED3071                                1'h0
   23      R      RESERVED3070                                1'h0
   24      R      RESERVED3069                                1'h0
   25      R      RESERVED3068                                1'h0
   26      R      RESERVED3067                                1'h0
   27      R      RESERVED3066                                1'h0
   28      R      RESERVED3065                                1'h0
   29      R      RESERVED3064                                1'h0
   30      R      RESERVED3063                                1'h0
   31      R      RESERVED3062                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_LAT_CNT_2_FIFO_FULL_INTR_EN_CTRL_TypeDef;


/* 0xA4C
   0       R      RESERVED3126                                1'h0
   1       R      RESERVED3125                                1'h0
   2       R      RESERVED3124                                1'h0
   3       R      RESERVED3123                                1'h0
   4       R      RESERVED3122                                1'h0
   5       R      RESERVED3121                                1'h0
   6       R      RESERVED3120                                1'h0
   7       R      RESERVED3119                                1'h0
   8       R      RESERVED3118                                1'h0
   9       R      RESERVED3117                                1'h0
   10      R      RESERVED3116                                1'h0
   11      R      RESERVED3115                                1'h0
   12      R      RESERVED3114                                1'h0
   13      R      RESERVED3113                                1'h0
   14      R      RESERVED3112                                1'h0
   15      R      RESERVED3111                                1'h0
   16      R      RESERVED3110                                1'h0
   17      R      RESERVED3109                                1'h0
   18      R      RESERVED3108                                1'h0
   19      R      RESERVED3107                                1'h0
   20      R      RESERVED3106                                1'h0
   21      R      RESERVED3105                                1'h0
   22      R      RESERVED3104                                1'h0
   23      R      RESERVED3103                                1'h0
   24      R      RESERVED3102                                1'h0
   25      R      RESERVED3101                                1'h0
   26      R      RESERVED3100                                1'h0
   27      R      RESERVED3099                                1'h0
   28      R      RESERVED3098                                1'h0
   29      R      RESERVED3097                                1'h0
   30      R      RESERVED3096                                1'h0
   31      R      RESERVED3095                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_LAT_CNT_2_FIFO_MET_THRESHOLD_INTR_EN_CTRL_TypeDef;


/* 0xA50
   0       R      RESERVED3159                                1'h1
   1       R      RESERVED3158                                1'h1
   2       R      RESERVED3157                                1'h1
   3       R      RESERVED3156                                1'h1
   4       R      RESERVED3155                                1'h1
   5       R      RESERVED3154                                1'h1
   6       R      RESERVED3153                                1'h1
   7       R      RESERVED3152                                1'h1
   8       R      RESERVED3151                                1'h1
   9       R      RESERVED3150                                1'h1
   10      R      RESERVED3149                                1'h1
   11      R      RESERVED3148                                1'h1
   12      R      RESERVED3147                                1'h1
   13      R      RESERVED3146                                1'h1
   14      R      RESERVED3145                                1'h1
   15      R      RESERVED3144                                1'h1
   16      R      RESERVED3143                                1'h1
   17      R      RESERVED3142                                1'h1
   18      R      RESERVED3141                                1'h1
   19      R      RESERVED3140                                1'h1
   20      R      RESERVED3139                                1'h1
   21      R      RESERVED3138                                1'h1
   22      R      RESERVED3137                                1'h1
   23      R      RESERVED3136                                1'h1
   24      R      RESERVED3135                                1'h1
   25      R      RESERVED3134                                1'h1
   26      R      RESERVED3133                                1'h1
   27      R      RESERVED3132                                1'h1
   28      R      RESERVED3131                                1'h1
   29      R      RESERVED3130                                1'h1
   30      R      RESERVED3129                                1'h1
   31      R      RESERVED3128                                1'h1
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_LAT_CNT_2_FIFO_EMPTY_STS_TypeDef;


/* 0xA54
   0       R      RESERVED3192                                1'h0
   1       R      RESERVED3191                                1'h0
   2       R      RESERVED3190                                1'h0
   3       R      RESERVED3189                                1'h0
   4       R      RESERVED3188                                1'h0
   5       R      RESERVED3187                                1'h0
   6       R      RESERVED3186                                1'h0
   7       R      RESERVED3185                                1'h0
   8       R      RESERVED3184                                1'h0
   9       R      RESERVED3183                                1'h0
   10      R      RESERVED3182                                1'h0
   11      R      RESERVED3181                                1'h0
   12      R      RESERVED3180                                1'h0
   13      R      RESERVED3179                                1'h0
   14      R      RESERVED3178                                1'h0
   15      R      RESERVED3177                                1'h0
   16      R      RESERVED3176                                1'h0
   17      R      RESERVED3175                                1'h0
   18      R      RESERVED3174                                1'h0
   19      R      RESERVED3173                                1'h0
   20      R      RESERVED3172                                1'h0
   21      R      RESERVED3171                                1'h0
   22      R      RESERVED3170                                1'h0
   23      R      RESERVED3169                                1'h0
   24      R      RESERVED3168                                1'h0
   25      R      RESERVED3167                                1'h0
   26      R      RESERVED3166                                1'h0
   27      R      RESERVED3165                                1'h0
   28      R      RESERVED3164                                1'h0
   29      R      RESERVED3163                                1'h0
   30      R      RESERVED3162                                1'h0
   31      R      RESERVED3161                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_LAT_CNT_2_FIFO_FULL_INTR_TypeDef;


/* 0xA58
   0       R      RESERVED3225                                1'h0
   1       R      RESERVED3224                                1'h0
   2       R      RESERVED3223                                1'h0
   3       R      RESERVED3222                                1'h0
   4       R      RESERVED3221                                1'h0
   5       R      RESERVED3220                                1'h0
   6       R      RESERVED3219                                1'h0
   7       R      RESERVED3218                                1'h0
   8       R      RESERVED3217                                1'h0
   9       R      RESERVED3216                                1'h0
   10      R      RESERVED3215                                1'h0
   11      R      RESERVED3214                                1'h0
   12      R      RESERVED3213                                1'h0
   13      R      RESERVED3212                                1'h0
   14      R      RESERVED3211                                1'h0
   15      R      RESERVED3210                                1'h0
   16      R      RESERVED3209                                1'h0
   17      R      RESERVED3208                                1'h0
   18      R      RESERVED3207                                1'h0
   19      R      RESERVED3206                                1'h0
   20      R      RESERVED3205                                1'h0
   21      R      RESERVED3204                                1'h0
   22      R      RESERVED3203                                1'h0
   23      R      RESERVED3202                                1'h0
   24      R      RESERVED3201                                1'h0
   25      R      RESERVED3200                                1'h0
   26      R      RESERVED3199                                1'h0
   27      R      RESERVED3198                                1'h0
   28      R      RESERVED3197                                1'h0
   29      R      RESERVED3196                                1'h0
   30      R      RESERVED3195                                1'h0
   31      R      RESERVED3194                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_LAT_CNT_2_FIFO_MET_THRESHOLD_INTR_TypeDef;


/* 0xA5C
   0       R/WAC  reg_timer_0_pause                           1'h0
   1       R/WAC  reg_timer_1_pause                           1'h0
   2       R/WAC  reg_timer_2_pause                           1'h0
   3       R/WAC  reg_timer_3_pause                           1'h0
   4       R/WAC  reg_timer_4_pause                           1'h0
   5       R/WAC  reg_timer_5_pause                           1'h0
   6       R/WAC  reg_timer_6_pause                           1'h0
   7       R/WAC  reg_timer_7_pause                           1'h0
   8       R/WAC  reg_timer_8_pause                           1'h0
   9       R/WAC  reg_timer_9_pause                           1'h0
   10      R      RESERVED3248                                1'h0
   11      R      RESERVED3247                                1'h0
   12      R      RESERVED3246                                1'h0
   13      R      RESERVED3245                                1'h0
   14      R      RESERVED3244                                1'h0
   15      R      RESERVED3243                                1'h0
   16      R      RESERVED3242                                1'h0
   17      R      RESERVED3241                                1'h0
   18      R      RESERVED3240                                1'h0
   19      R      RESERVED3239                                1'h0
   20      R      RESERVED3238                                1'h0
   21      R      RESERVED3237                                1'h0
   22      R      RESERVED3236                                1'h0
   23      R      RESERVED3235                                1'h0
   24      R      RESERVED3234                                1'h0
   25      R      RESERVED3233                                1'h0
   26      R      RESERVED3232                                1'h0
   27      R      RESERVED3231                                1'h0
   28      R      RESERVED3230                                1'h0
   29      R      RESERVED3229                                1'h0
   30      R      RESERVED3228                                1'h0
   31      R      RESERVED3227                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_0_pause: 1;
        uint32_t reg_timer_1_pause: 1;
        uint32_t reg_timer_2_pause: 1;
        uint32_t reg_timer_3_pause: 1;
        uint32_t reg_timer_4_pause: 1;
        uint32_t reg_timer_5_pause: 1;
        uint32_t reg_timer_6_pause: 1;
        uint32_t reg_timer_7_pause: 1;
        uint32_t reg_timer_8_pause: 1;
        uint32_t reg_timer_9_pause: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_PAUSE_CTRL_TypeDef;


/* 0xA60
   0       R/W    reg_timer_0_pause_intr_en                   1'h0
   1       R/W    reg_timer_1_pause_intr_en                   1'h0
   2       R/W    reg_timer_2_pause_intr_en                   1'h0
   3       R/W    reg_timer_3_pause_intr_en                   1'h0
   4       R/W    reg_timer_4_pause_intr_en                   1'h0
   5       R/W    reg_timer_5_pause_intr_en                   1'h0
   6       R/W    reg_timer_6_pause_intr_en                   1'h0
   7       R/W    reg_timer_7_pause_intr_en                   1'h0
   8       R/W    reg_timer_8_pause_intr_en                   1'h0
   9       R/W    reg_timer_9_pause_intr_en                   1'h0
   10      R      RESERVED3281                                1'h0
   11      R      RESERVED3280                                1'h0
   12      R      RESERVED3279                                1'h0
   13      R      RESERVED3278                                1'h0
   14      R      RESERVED3277                                1'h0
   15      R      RESERVED3276                                1'h0
   16      R      RESERVED3275                                1'h0
   17      R      RESERVED3274                                1'h0
   18      R      RESERVED3273                                1'h0
   19      R      RESERVED3272                                1'h0
   20      R      RESERVED3271                                1'h0
   21      R      RESERVED3270                                1'h0
   22      R      RESERVED3269                                1'h0
   23      R      RESERVED3268                                1'h0
   24      R      RESERVED3267                                1'h0
   25      R      RESERVED3266                                1'h0
   26      R      RESERVED3265                                1'h0
   27      R      RESERVED3264                                1'h0
   28      R      RESERVED3263                                1'h0
   29      R      RESERVED3262                                1'h0
   30      R      RESERVED3261                                1'h0
   31      R      RESERVED3260                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_0_pause_intr_en: 1;
        uint32_t reg_timer_1_pause_intr_en: 1;
        uint32_t reg_timer_2_pause_intr_en: 1;
        uint32_t reg_timer_3_pause_intr_en: 1;
        uint32_t reg_timer_4_pause_intr_en: 1;
        uint32_t reg_timer_5_pause_intr_en: 1;
        uint32_t reg_timer_6_pause_intr_en: 1;
        uint32_t reg_timer_7_pause_intr_en: 1;
        uint32_t reg_timer_8_pause_intr_en: 1;
        uint32_t reg_timer_9_pause_intr_en: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_PAUSE_INTR_EN_CTRL_TypeDef;


/* 0xA64
   0       R/W1C  reg_timer_0_pause_intr_sts                  1'h0
   1       R/W1C  reg_timer_1_pause_intr_sts                  1'h0
   2       R/W1C  reg_timer_2_pause_intr_sts                  1'h0
   3       R/W1C  reg_timer_3_pause_intr_sts                  1'h0
   4       R/W1C  reg_timer_4_pause_intr_sts                  1'h0
   5       R/W1C  reg_timer_5_pause_intr_sts                  1'h0
   6       R/W1C  reg_timer_6_pause_intr_sts                  1'h0
   7       R/W1C  reg_timer_7_pause_intr_sts                  1'h0
   8       R/W1C  reg_timer_8_pause_intr_sts                  1'h0
   9       R/W1C  reg_timer_9_pause_intr_sts                  1'h0
   10      R      RESERVED3314                                1'h0
   11      R      RESERVED3313                                1'h0
   12      R      RESERVED3312                                1'h0
   13      R      RESERVED3311                                1'h0
   14      R      RESERVED3310                                1'h0
   15      R      RESERVED3309                                1'h0
   16      R      RESERVED3308                                1'h0
   17      R      RESERVED3307                                1'h0
   18      R      RESERVED3306                                1'h0
   19      R      RESERVED3305                                1'h0
   20      R      RESERVED3304                                1'h0
   21      R      RESERVED3303                                1'h0
   22      R      RESERVED3302                                1'h0
   23      R      RESERVED3301                                1'h0
   24      R      RESERVED3300                                1'h0
   25      R      RESERVED3299                                1'h0
   26      R      RESERVED3298                                1'h0
   27      R      RESERVED3297                                1'h0
   28      R      RESERVED3296                                1'h0
   29      R      RESERVED3295                                1'h0
   30      R      RESERVED3294                                1'h0
   31      R      RESERVED3293                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        uint32_t reg_timer_0_pause_intr_sts: 1;
        uint32_t reg_timer_1_pause_intr_sts: 1;
        uint32_t reg_timer_2_pause_intr_sts: 1;
        uint32_t reg_timer_3_pause_intr_sts: 1;
        uint32_t reg_timer_4_pause_intr_sts: 1;
        uint32_t reg_timer_5_pause_intr_sts: 1;
        uint32_t reg_timer_6_pause_intr_sts: 1;
        uint32_t reg_timer_7_pause_intr_sts: 1;
        uint32_t reg_timer_8_pause_intr_sts: 1;
        uint32_t reg_timer_9_pause_intr_sts: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_PAUSE_INTR_STS_TypeDef;


/* 0xA68
   0       R      reg_timer_0_pause_intr_sts_masked           1'h0
   1       R      reg_timer_1_pause_intr_sts_masked           1'h0
   2       R      reg_timer_2_pause_intr_sts_masked           1'h0
   3       R      reg_timer_3_pause_intr_sts_masked           1'h0
   4       R      reg_timer_4_pause_intr_sts_masked           1'h0
   5       R      reg_timer_5_pause_intr_sts_masked           1'h0
   6       R      reg_timer_6_pause_intr_sts_masked           1'h0
   7       R      reg_timer_7_pause_intr_sts_masked           1'h0
   8       R      reg_timer_8_pause_intr_sts_masked           1'h0
   9       R      reg_timer_9_pause_intr_sts_masked           1'h0
   10      R      RESERVED3347                                1'h0
   11      R      RESERVED3346                                1'h0
   12      R      RESERVED3345                                1'h0
   13      R      RESERVED3344                                1'h0
   14      R      RESERVED3343                                1'h0
   15      R      RESERVED3342                                1'h0
   16      R      RESERVED3341                                1'h0
   17      R      RESERVED3340                                1'h0
   18      R      RESERVED3339                                1'h0
   19      R      RESERVED3338                                1'h0
   20      R      RESERVED3337                                1'h0
   21      R      RESERVED3336                                1'h0
   22      R      RESERVED3335                                1'h0
   23      R      RESERVED3334                                1'h0
   24      R      RESERVED3333                                1'h0
   25      R      RESERVED3332                                1'h0
   26      R      RESERVED3331                                1'h0
   27      R      RESERVED3330                                1'h0
   28      R      RESERVED3329                                1'h0
   29      R      RESERVED3328                                1'h0
   30      R      RESERVED3327                                1'h0
   31      R      RESERVED3326                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reg_timer_0_pause_intr_sts_masked: 1;
        const uint32_t reg_timer_1_pause_intr_sts_masked: 1;
        const uint32_t reg_timer_2_pause_intr_sts_masked: 1;
        const uint32_t reg_timer_3_pause_intr_sts_masked: 1;
        const uint32_t reg_timer_4_pause_intr_sts_masked: 1;
        const uint32_t reg_timer_5_pause_intr_sts_masked: 1;
        const uint32_t reg_timer_6_pause_intr_sts_masked: 1;
        const uint32_t reg_timer_7_pause_intr_sts_masked: 1;
        const uint32_t reg_timer_8_pause_intr_sts_masked: 1;
        const uint32_t reg_timer_9_pause_intr_sts_masked: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_PAUSE_INTR_TypeDef;


/* 0xA6C
   31:0    R      RESERVED3359                                32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_0: 32;
    } b;
} TIMER_DMY_TypeDef;


/* 0xA70
   0       R      RESERVED3392                                1'h0
   1       R      RESERVED3391                                1'h0
   2       R      RESERVED3390                                1'h0
   3       R      RESERVED3389                                1'h0
   4       R      RESERVED3388                                1'h0
   5       R      RESERVED3387                                1'h0
   6       R      RESERVED3386                                1'h0
   7       R      RESERVED3385                                1'h0
   8       R      RESERVED3384                                1'h0
   9       R      RESERVED3383                                1'h0
   10      R      RESERVED3382                                1'h0
   11      R      RESERVED3381                                1'h0
   12      R      RESERVED3380                                1'h0
   13      R      RESERVED3379                                1'h0
   14      R      RESERVED3378                                1'h0
   15      R      RESERVED3377                                1'h0
   16      R      RESERVED3376                                1'h0
   17      R      RESERVED3375                                1'h0
   18      R      RESERVED3374                                1'h0
   19      R      RESERVED3373                                1'h0
   20      R      RESERVED3372                                1'h0
   21      R      RESERVED3371                                1'h0
   22      R      RESERVED3370                                1'h0
   23      R      RESERVED3369                                1'h0
   24      R      RESERVED3368                                1'h0
   25      R      RESERVED3367                                1'h0
   26      R      RESERVED3366                                1'h0
   27      R      RESERVED3365                                1'h0
   28      R      RESERVED3364                                1'h0
   29      R      RESERVED3363                                1'h0
   30      R      RESERVED3362                                1'h0
   31      R      RESERVED3361                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_Q_CH_FW_CTRL0_TypeDef;


/* 0xA74
   0       R      RESERVED3425                                1'h0
   1       R      RESERVED3424                                1'h0
   2       R      RESERVED3423                                1'h0
   3       R      RESERVED3422                                1'h0
   4       R      RESERVED3421                                1'h0
   5       R      RESERVED3420                                1'h0
   6       R      RESERVED3419                                1'h0
   7       R      RESERVED3418                                1'h0
   8       R      RESERVED3417                                1'h0
   9       R      RESERVED3416                                1'h0
   10      R      RESERVED3415                                1'h0
   11      R      RESERVED3414                                1'h0
   12      R      RESERVED3413                                1'h0
   13      R      RESERVED3412                                1'h0
   14      R      RESERVED3411                                1'h0
   15      R      RESERVED3410                                1'h0
   16      R      RESERVED3409                                1'h0
   17      R      RESERVED3408                                1'h0
   18      R      RESERVED3407                                1'h0
   19      R      RESERVED3406                                1'h0
   20      R      RESERVED3405                                1'h0
   21      R      RESERVED3404                                1'h0
   22      R      RESERVED3403                                1'h0
   23      R      RESERVED3402                                1'h0
   24      R      RESERVED3401                                1'h0
   25      R      RESERVED3400                                1'h0
   26      R      RESERVED3399                                1'h0
   27      R      RESERVED3398                                1'h0
   28      R      RESERVED3397                                1'h0
   29      R      RESERVED3396                                1'h0
   30      R      RESERVED3395                                1'h0
   31      R      RESERVED3394                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_Q_CH_FW_CTRL1_TypeDef;


/* 0xA78
   31:0    R      RESERVED3427                                32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_0: 32;
    } b;
} TIMER_Q_CH_DMY_0_TypeDef;


/* 0xA7C
   31:0    R      RESERVED3429                                32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_0: 32;
    } b;
} TIMER_Q_CH_DMY_1_TypeDef;


/* 0xA80
   0       R      RESERVED3462                                1'h0
   1       R      RESERVED3461                                1'h0
   2       R      RESERVED3460                                1'h0
   3       R      RESERVED3459                                1'h0
   4       R      RESERVED3458                                1'h0
   5       R      RESERVED3457                                1'h0
   6       R      RESERVED3456                                1'h0
   7       R      RESERVED3455                                1'h0
   8       R      RESERVED3454                                1'h0
   9       R      RESERVED3453                                1'h0
   10      R      RESERVED3452                                1'h0
   11      R      RESERVED3451                                1'h0
   12      R      RESERVED3450                                1'h0
   13      R      RESERVED3449                                1'h0
   14      R      RESERVED3448                                1'h0
   15      R      RESERVED3447                                1'h0
   16      R      RESERVED3446                                1'h0
   17      R      RESERVED3445                                1'h0
   18      R      RESERVED3444                                1'h0
   19      R      RESERVED3443                                1'h0
   20      R      RESERVED3442                                1'h0
   21      R      RESERVED3441                                1'h0
   22      R      RESERVED3440                                1'h0
   23      R      RESERVED3439                                1'h0
   24      R      RESERVED3438                                1'h0
   25      R      RESERVED3437                                1'h0
   26      R      RESERVED3436                                1'h0
   27      R      RESERVED3435                                1'h0
   28      R      RESERVED3434                                1'h0
   29      R      RESERVED3433                                1'h0
   30      R      RESERVED3432                                1'h0
   31      R      RESERVED3431                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_RAP_CTRL0_TypeDef;


/* 0xA84
   0       R      RESERVED3495                                1'h0
   1       R      RESERVED3494                                1'h0
   2       R      RESERVED3493                                1'h0
   3       R      RESERVED3492                                1'h0
   4       R      RESERVED3491                                1'h0
   5       R      RESERVED3490                                1'h0
   6       R      RESERVED3489                                1'h0
   7       R      RESERVED3488                                1'h0
   8       R      RESERVED3487                                1'h0
   9       R      RESERVED3486                                1'h0
   10      R      RESERVED3485                                1'h0
   11      R      RESERVED3484                                1'h0
   12      R      RESERVED3483                                1'h0
   13      R      RESERVED3482                                1'h0
   14      R      RESERVED3481                                1'h0
   15      R      RESERVED3480                                1'h0
   16      R      RESERVED3479                                1'h0
   17      R      RESERVED3478                                1'h0
   18      R      RESERVED3477                                1'h0
   19      R      RESERVED3476                                1'h0
   20      R      RESERVED3475                                1'h0
   21      R      RESERVED3474                                1'h0
   22      R      RESERVED3473                                1'h0
   23      R      RESERVED3472                                1'h0
   24      R      RESERVED3471                                1'h0
   25      R      RESERVED3470                                1'h0
   26      R      RESERVED3469                                1'h0
   27      R      RESERVED3468                                1'h0
   28      R      RESERVED3467                                1'h0
   29      R      RESERVED3466                                1'h0
   30      R      RESERVED3465                                1'h0
   31      R      RESERVED3464                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_RAP_CTRL1_TypeDef;


/* 0xA88
   0       R      RESERVED3528                                1'h0
   1       R      RESERVED3527                                1'h0
   2       R      RESERVED3526                                1'h0
   3       R      RESERVED3525                                1'h0
   4       R      RESERVED3524                                1'h0
   5       R      RESERVED3523                                1'h0
   6       R      RESERVED3522                                1'h0
   7       R      RESERVED3521                                1'h0
   8       R      RESERVED3520                                1'h0
   9       R      RESERVED3519                                1'h0
   10      R      RESERVED3518                                1'h0
   11      R      RESERVED3517                                1'h0
   12      R      RESERVED3516                                1'h0
   13      R      RESERVED3515                                1'h0
   14      R      RESERVED3514                                1'h0
   15      R      RESERVED3513                                1'h0
   16      R      RESERVED3512                                1'h0
   17      R      RESERVED3511                                1'h0
   18      R      RESERVED3510                                1'h0
   19      R      RESERVED3509                                1'h0
   20      R      RESERVED3508                                1'h0
   21      R      RESERVED3507                                1'h0
   22      R      RESERVED3506                                1'h0
   23      R      RESERVED3505                                1'h0
   24      R      RESERVED3504                                1'h0
   25      R      RESERVED3503                                1'h0
   26      R      RESERVED3502                                1'h0
   27      R      RESERVED3501                                1'h0
   28      R      RESERVED3500                                1'h0
   29      R      RESERVED3499                                1'h0
   30      R      RESERVED3498                                1'h0
   31      R      RESERVED3497                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_RAP_FW_TSAK_START_TypeDef;


/* 0xA8C
   0       R      RESERVED3561                                1'h0
   1       R      RESERVED3560                                1'h0
   2       R      RESERVED3559                                1'h0
   3       R      RESERVED3558                                1'h0
   4       R      RESERVED3557                                1'h0
   5       R      RESERVED3556                                1'h0
   6       R      RESERVED3555                                1'h0
   7       R      RESERVED3554                                1'h0
   8       R      RESERVED3553                                1'h0
   9       R      RESERVED3552                                1'h0
   10      R      RESERVED3551                                1'h0
   11      R      RESERVED3550                                1'h0
   12      R      RESERVED3549                                1'h0
   13      R      RESERVED3548                                1'h0
   14      R      RESERVED3547                                1'h0
   15      R      RESERVED3546                                1'h0
   16      R      RESERVED3545                                1'h0
   17      R      RESERVED3544                                1'h0
   18      R      RESERVED3543                                1'h0
   19      R      RESERVED3542                                1'h0
   20      R      RESERVED3541                                1'h0
   21      R      RESERVED3540                                1'h0
   22      R      RESERVED3539                                1'h0
   23      R      RESERVED3538                                1'h0
   24      R      RESERVED3537                                1'h0
   25      R      RESERVED3536                                1'h0
   26      R      RESERVED3535                                1'h0
   27      R      RESERVED3534                                1'h0
   28      R      RESERVED3533                                1'h0
   29      R      RESERVED3532                                1'h0
   30      R      RESERVED3531                                1'h0
   31      R      RESERVED3530                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_RAP_FW_TASK_PAUSE_TypeDef;


/* 0xA90
   0       R      RESERVED3594                                1'h0
   1       R      RESERVED3593                                1'h0
   2       R      RESERVED3592                                1'h0
   3       R      RESERVED3591                                1'h0
   4       R      RESERVED3590                                1'h0
   5       R      RESERVED3589                                1'h0
   6       R      RESERVED3588                                1'h0
   7       R      RESERVED3587                                1'h0
   8       R      RESERVED3586                                1'h0
   9       R      RESERVED3585                                1'h0
   10      R      RESERVED3584                                1'h0
   11      R      RESERVED3583                                1'h0
   12      R      RESERVED3582                                1'h0
   13      R      RESERVED3581                                1'h0
   14      R      RESERVED3580                                1'h0
   15      R      RESERVED3579                                1'h0
   16      R      RESERVED3578                                1'h0
   17      R      RESERVED3577                                1'h0
   18      R      RESERVED3576                                1'h0
   19      R      RESERVED3575                                1'h0
   20      R      RESERVED3574                                1'h0
   21      R      RESERVED3573                                1'h0
   22      R      RESERVED3572                                1'h0
   23      R      RESERVED3571                                1'h0
   24      R      RESERVED3570                                1'h0
   25      R      RESERVED3569                                1'h0
   26      R      RESERVED3568                                1'h0
   27      R      RESERVED3567                                1'h0
   28      R      RESERVED3566                                1'h0
   29      R      RESERVED3565                                1'h0
   30      R      RESERVED3564                                1'h0
   31      R      RESERVED3563                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_RAP_FW_TASK_DISABLE_TypeDef;


/* 0xA94
   0       R      RESERVED3627                                1'h0
   1       R      RESERVED3626                                1'h0
   2       R      RESERVED3625                                1'h0
   3       R      RESERVED3624                                1'h0
   4       R      RESERVED3623                                1'h0
   5       R      RESERVED3622                                1'h0
   6       R      RESERVED3621                                1'h0
   7       R      RESERVED3620                                1'h0
   8       R      RESERVED3619                                1'h0
   9       R      RESERVED3618                                1'h0
   10      R      RESERVED3617                                1'h0
   11      R      RESERVED3616                                1'h0
   12      R      RESERVED3615                                1'h0
   13      R      RESERVED3614                                1'h0
   14      R      RESERVED3613                                1'h0
   15      R      RESERVED3612                                1'h0
   16      R      RESERVED3611                                1'h0
   17      R      RESERVED3610                                1'h0
   18      R      RESERVED3609                                1'h0
   19      R      RESERVED3608                                1'h0
   20      R      RESERVED3607                                1'h0
   21      R      RESERVED3606                                1'h0
   22      R      RESERVED3605                                1'h0
   23      R      RESERVED3604                                1'h0
   24      R      RESERVED3603                                1'h0
   25      R      RESERVED3602                                1'h0
   26      R      RESERVED3601                                1'h0
   27      R      RESERVED3600                                1'h0
   28      R      RESERVED3599                                1'h0
   29      R      RESERVED3598                                1'h0
   30      R      RESERVED3597                                1'h0
   31      R      RESERVED3596                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_RAP_STS_ACTIVE_TypeDef;


/* 0xA98
   0       R      RESERVED3660                                1'h0
   1       R      RESERVED3659                                1'h0
   2       R      RESERVED3658                                1'h0
   3       R      RESERVED3657                                1'h0
   4       R      RESERVED3656                                1'h0
   5       R      RESERVED3655                                1'h0
   6       R      RESERVED3654                                1'h0
   7       R      RESERVED3653                                1'h0
   8       R      RESERVED3652                                1'h0
   9       R      RESERVED3651                                1'h0
   10      R      RESERVED3650                                1'h0
   11      R      RESERVED3649                                1'h0
   12      R      RESERVED3648                                1'h0
   13      R      RESERVED3647                                1'h0
   14      R      RESERVED3646                                1'h0
   15      R      RESERVED3645                                1'h0
   16      R      RESERVED3644                                1'h0
   17      R      RESERVED3643                                1'h0
   18      R      RESERVED3642                                1'h0
   19      R      RESERVED3641                                1'h0
   20      R      RESERVED3640                                1'h0
   21      R      RESERVED3639                                1'h0
   22      R      RESERVED3638                                1'h0
   23      R      RESERVED3637                                1'h0
   24      R      RESERVED3636                                1'h0
   25      R      RESERVED3635                                1'h0
   26      R      RESERVED3634                                1'h0
   27      R      RESERVED3633                                1'h0
   28      R      RESERVED3632                                1'h0
   29      R      RESERVED3631                                1'h0
   30      R      RESERVED3630                                1'h0
   31      R      RESERVED3629                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_RAP_SHORTCUT_SUB_TASK_PAUSE_TypeDef;


/* 0xA9C
   0       R      RESERVED3693                                1'h0
   1       R      RESERVED3692                                1'h0
   2       R      RESERVED3691                                1'h0
   3       R      RESERVED3690                                1'h0
   4       R      RESERVED3689                                1'h0
   5       R      RESERVED3688                                1'h0
   6       R      RESERVED3687                                1'h0
   7       R      RESERVED3686                                1'h0
   8       R      RESERVED3685                                1'h0
   9       R      RESERVED3684                                1'h0
   10      R      RESERVED3683                                1'h0
   11      R      RESERVED3682                                1'h0
   12      R      RESERVED3681                                1'h0
   13      R      RESERVED3680                                1'h0
   14      R      RESERVED3679                                1'h0
   15      R      RESERVED3678                                1'h0
   16      R      RESERVED3677                                1'h0
   17      R      RESERVED3676                                1'h0
   18      R      RESERVED3675                                1'h0
   19      R      RESERVED3674                                1'h0
   20      R      RESERVED3673                                1'h0
   21      R      RESERVED3672                                1'h0
   22      R      RESERVED3671                                1'h0
   23      R      RESERVED3670                                1'h0
   24      R      RESERVED3669                                1'h0
   25      R      RESERVED3668                                1'h0
   26      R      RESERVED3667                                1'h0
   27      R      RESERVED3666                                1'h0
   28      R      RESERVED3665                                1'h0
   29      R      RESERVED3664                                1'h0
   30      R      RESERVED3663                                1'h0
   31      R      RESERVED3662                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_RAP_SHORTCUT_SUB_TASK_DISABLE_TypeDef;


/* 0xAA0
   0       R      RESERVED3726                                1'h0
   1       R      RESERVED3725                                1'h0
   2       R      RESERVED3724                                1'h0
   3       R      RESERVED3723                                1'h0
   4       R      RESERVED3722                                1'h0
   5       R      RESERVED3721                                1'h0
   6       R      RESERVED3720                                1'h0
   7       R      RESERVED3719                                1'h0
   8       R      RESERVED3718                                1'h0
   9       R      RESERVED3717                                1'h0
   10      R      RESERVED3716                                1'h0
   11      R      RESERVED3715                                1'h0
   12      R      RESERVED3714                                1'h0
   13      R      RESERVED3713                                1'h0
   14      R      RESERVED3712                                1'h0
   15      R      RESERVED3711                                1'h0
   16      R      RESERVED3710                                1'h0
   17      R      RESERVED3709                                1'h0
   18      R      RESERVED3708                                1'h0
   19      R      RESERVED3707                                1'h0
   20      R      RESERVED3706                                1'h0
   21      R      RESERVED3705                                1'h0
   22      R      RESERVED3704                                1'h0
   23      R      RESERVED3703                                1'h0
   24      R      RESERVED3702                                1'h0
   25      R      RESERVED3701                                1'h0
   26      R      RESERVED3700                                1'h0
   27      R      RESERVED3699                                1'h0
   28      R      RESERVED3698                                1'h0
   29      R      RESERVED3697                                1'h0
   30      R      RESERVED3696                                1'h0
   31      R      RESERVED3695                                1'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_31: 1;
        const uint32_t reserved_30: 1;
        const uint32_t reserved_29: 1;
        const uint32_t reserved_28: 1;
        const uint32_t reserved_27: 1;
        const uint32_t reserved_26: 1;
        const uint32_t reserved_25: 1;
        const uint32_t reserved_24: 1;
        const uint32_t reserved_23: 1;
        const uint32_t reserved_22: 1;
        const uint32_t reserved_21: 1;
        const uint32_t reserved_20: 1;
        const uint32_t reserved_19: 1;
        const uint32_t reserved_18: 1;
        const uint32_t reserved_17: 1;
        const uint32_t reserved_16: 1;
        const uint32_t reserved_15: 1;
        const uint32_t reserved_14: 1;
        const uint32_t reserved_13: 1;
        const uint32_t reserved_12: 1;
        const uint32_t reserved_11: 1;
        const uint32_t reserved_10: 1;
        const uint32_t reserved_9: 1;
        const uint32_t reserved_8: 1;
        const uint32_t reserved_7: 1;
        const uint32_t reserved_6: 1;
        const uint32_t reserved_5: 1;
        const uint32_t reserved_4: 1;
        const uint32_t reserved_3: 1;
        const uint32_t reserved_2: 1;
        const uint32_t reserved_1: 1;
        const uint32_t reserved_0: 1;
    } b;
} TIMER_RAP_SHORTCUT_PUB_EVENT_TIMEOUT_TypeDef;


/* 0xAA4
   31:0    R      RESERVED3728                                32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_0: 32;
    } b;
} TIMER_RAP_DMY_0_TypeDef;


/* 0xAA8
   31:0    R      RESERVED3730                                32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_0: 32;
    } b;
} TIMER_RAP_DMY_1_TypeDef;


/* 0xAAC
   31:0    R      RESERVED3732                                32'h0
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reserved_0: 32;
    } b;
} TIMER_RAP_DMY_2_TypeDef;


/* 0xAB0
   31:0    R      reg_timer_ip_ver                            32'h2402206A
*/
typedef union
{
    uint32_t d32;
    struct
    {
        const uint32_t reg_timer_ip_ver: 32;
    } b;
} TIMER_IP_VER_TypeDef;

/*============================================================================*
 *                          TIM Constants
 *============================================================================*/
/** \defgroup TIM         TIM
  * \brief
  * \{
  */

/** \defgroup TIM_Exported_Constants TIM Exported Constants
  * \brief
  * \{
  */

/**
 * \defgroup    TIM_Clock_Source TIM Clock Source
 * \{
 * TIML_CH6, TIML_CH7, and TIML_CH8 supports CK_PLL1_TIMER and CK_PLL2_TIMER and CK_PLL3_TIMER and CK_40M_TIMER.
 * The other channels of timer only support CK_40M_TIMER.
 * \ingroup     TIM_Exported_Constants
 */

typedef enum
{
    CK_PLL2_TIMER = 0x0,
    CK_PLL3_TIMER = 0x1,
    CK_PLL1_TIMER = 0x2,
    CK_40M_TIMER  = 0x3,
} TIMClockSrc_TypeDef;

#define IS_TIM_CLK_SOURCE(PERIPH)     (((PERIPH) == CK_PLL2_TIMER) || \
                                       ((PERIPH) == CK_PLL3_TIMER) || \
                                       ((PERIPH) == CK_PLL1_TIMER) || \
                                       ((PERIPH) == CK_40M_TIMER))

/** End of TIM_Clock_Source
  * \}
  */

/**
 * \defgroup    TIM_Clock_Divider TIM Clock Divider
 * \{
 * \ingroup     TIM_Exported_Constants
 */

typedef enum
{
    TIM_CLOCK_DIVIDER_1 = 0x0,
    TIM_CLOCK_DIVIDER_2 = 0x1,
    TIM_CLOCK_DIVIDER_4 = 0x2,
    TIM_CLOCK_DIVIDER_8 = 0x3,
    TIM_CLOCK_DIVIDER_16 = 0x4,
    TIM_CLOCK_DIVIDER_32 = 0x5,
    TIM_CLOCK_DIVIDER_40 = 0x6,
    TIM_CLOCK_DIVIDER_64 = 0x7,
} TIMClockDiv_TypeDef;

#define IS_TIM_CLK_DIV(DIV) (((DIV) == TIM_CLOCK_DIVIDER_1) || \
                             ((DIV) == TIM_CLOCK_DIVIDER_2) || \
                             ((DIV) == TIM_CLOCK_DIVIDER_4) || \
                             ((DIV) == TIM_CLOCK_DIVIDER_8) || \
                             ((DIV) == TIM_CLOCK_DIVIDER_16) || \
                             ((DIV) == TIM_CLOCK_DIVIDER_32) || \
                             ((DIV) == TIM_CLOCK_DIVIDER_40) || \
                             ((DIV) == TIM_CLOCK_DIVIDER_64))

/** End of TIM_Clock_Divider
  * \}
  */

/** End of TIM_Exported_Constants
  * \}
  */

/** End of TIM
  * \}
  */

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* RTL876X_TIM_DEF_H */

/******************* (C) COPYRIGHT 2024 Realtek Semiconductor Corporation *****END OF FILE****/
