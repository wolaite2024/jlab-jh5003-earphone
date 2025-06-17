#ifndef _CLOCK_MANAGER_H_
#define _CLOCK_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>

#ifndef MIN
#define MIN(a,b)            (((a) < (b)) ? (a) : (b))
#endif

#ifndef ABS
#define ABS(x)     (((x) > 0) ? (x) : ((~(x))+1))
#endif

#define CLOCK_0KHZ      0
#define CLOCK_625KHZ    625000
#define CLOCK_1P25MHZ   1250000
#define CLOCK_2P5MHZ    2500000
#define CLOCK_3P3MHZ    3333333
#define CLOCK_4MHZ      4000000
#define CLOCK_5MHZ      5000000
#define CLOCK_5P71MHZ   5714285
#define CLOCK_6P67MHZ   6666667
#define CLOCK_8MHZ      8000000
#define CLOCK_10MHZ     10000000
#define CLOCK_13P3MHZ   13333333
#define CLOCK_16MHZ     16000000
#define CLOCK_20MHZ     20000000
#define CLOCK_24MHZ     24000000
#define CLOCK_26P7MHZ   26666667
#define CLOCK_28MHZ     28000000
#define CLOCK_30MHZ     30000000
#define CLOCK_32MHZ     32000000
#define CLOCK_35MHZ     35000000
#define CLOCK_40MHZ     40000000
#define CLOCK_45MHZ     45000000
#define CLOCK_48MHZ     48000000
#define CLOCK_50MHZ     50000000
#define CLOCK_53P3MHZ   53333333
#define CLOCK_60MHZ     60000000
#define CLOCK_70MHZ     70000000
#define CLOCK_80MHZ     80000000
#define CLOCK_90MHZ     90000000
#define CLOCK_100MHZ    100000000
#define CLOCK_105MHZ    105000000
#define CLOCK_120MHZ    120000000
#define CLOCK_125MHZ    125000000
#define CLOCK_140MHZ    140000000
#define CLOCK_160MHZ    160000000
#define CLOCK_180MHZ    180000000
#define CLOCK_200MHZ    200000000
#define CLOCK_240MHZ    240000000
#define CLOCK_280MHZ    280000000
#define CLOCK_320MHZ    320000000
#define CLOCK_98P304MHZ 98304000
#define CLK_MHZ(x)      ((x) / 1000000)
#define CLK_KHZ(x)      ((x) / 1000)

#define SLAVE_NUM                                 5
#define SLAVE_OFFSET(slave)                       ((slave) - CLK_MODULE_MAX)
#define IS_MODULE_RETENTION_CLOCK(module)         (((module) == CLK_CPU) || ((module) == CLK_SPIC0) || ((module) == CLK_40M))

#define MAX_PLL1_FREQ   CLOCK_200MHZ
#define MAX_PLL2_FREQ   CLOCK_280MHZ
#define MAX_PLL3_FREQ   CLOCK_280MHZ

/* PLL DIV FAON REG Value */
#define FAON_REG_PLL1_120MHZ        4
#define FAON_REG_PLL1_140MHZ        5
#define FAON_REG_PLL1_160MHZ        6
#define FAON_REG_PLL1_180MHZ        7
#define FAON_REG_PLL1_200MHZ        8
#define FAON_REG_PLL2_160MHZ        0
#define FAON_REG_PLL2_80MHZ         1

/* PLL2 parameter */
#define PLL2_REF_CLK_MHZ            20
#define PLL2_DIVN_ADDEND            2

/* PLL3 parameter */
#define PLL3_REF_CLK_MHZ            20
#define PLL3_DIVN_ADDEND            2
#define PLL3_F0N_DIVISOR            6
#define PLL3_F0F_DIVISOR            (6*1<<13)

/* ADC/DAC FAON REG Value */
#define FAON_REG_ADC_10MHZ         0
#define FAON_REG_ADC_20MHZ         1
#define FAON_REG_DAC_20MHZ         0
#define FAON_REG_DAC_40MHZ         1

/* PLL1 DIV SEL REG Value */
#define FAON_PLL1_DIV4             3
#define FAON_PLL1_DIV2             1

#define XTAL32K_7P5PF_SEL_CUR_GM_INI    0x4
#define XTAL32K_7P5PF_SEL_CUR_GM        0x4
#define XTAL32K_12P5PF_SEL_CUR_GM_INI   0x9
#define XTAL32K_12P5PF_SEL_CUR_GM       0x5

/* 32k empty function */
#define internal_32k_bypass_sdm_ini        empty_function

#define external_32k_cal_en                empty_function
#define internal_32k_bypass_sdm_en         empty_function
#define lp_xtal_cal_en                     empty_function

#define lp_xtal_check_lock                 (uint8_t(*)(void))true_function
#define internal_32k_bypass_sdm_check_lock (uint8_t(*)(void))true_function


typedef enum
{
    CORE_NORMAL   = 0,
    CORE_SLOW     = 1,
    CORE_MODE_MAX = 2,
} CORE_MODE;

typedef enum
{
    USE_32K_EXT_XTAL = 0,
    USE_40M_LP_MODE,
    USE_32K_RCOSC,
    USE_32K_RCOSC_BYPASS_SDM,
    NUM_CLK_SRC,
} USE_32K_CLK_SRC_TYPE;

typedef enum
{
    USER_RTC_PF_RTC = 0,
    USER_RTC_EXT_XTAL,
    USE_RTC_GPIO_IN,
    MAX_USE_RTC_SRC,
} USER_RTC_CLK_SRC_TYPE;
/* ================== */

typedef enum
{
    RTC_MODULE = 0,
    BTMAC_MODULE = 1,
    AON_MODULE = 2,
    NUM_MODULE,
} LOW_POW_MODULE_TYPE;

typedef enum _RTC_IN_SEL_TYPE
{
    PAD_RTC_IN_DISABLE,
    PAD_P2_3_RTC_IN,
    PAD_P3_4_RTC_IN,
    PAD_P4_5_RTC_IN,
    PAD_P5_2_RTC_IN,
    PAD_P9_5_RTC_IN,
    PAD_ADC_3_RTC_IN,
    PAD_MIC2_N_RTC_IN,
    RTC_IN_SEL_DEFAULT = PAD_RTC_IN_DISABLE,
} RTC_IN_SEL_TYPE;

typedef enum _RTC_OUT_SEL_TYPE
{
    RTC_OUT_DISABLE = 0,
    RTC_OUT_P2_3    = 1,
    RTC_OUT_P3_5    = 2,
    RTC_OUT_P4_5    = 3,
    RTC_OUT_P5_2    = 4,
    RTC_OUT_P9_5    = 5,
    RTC_OUT_ADC_3   = 6,
    RTC_OUT_MIC2_N  = 7,
} RTC_OUT_SEL_TYPE;

typedef enum _EXT_XTAL_FREQ_TYPE
{
    EXT_CLK_32768 = 0,
    EXT_CLK_32K = 1,
} EXT_XTAL_FREQ_TYPE;

typedef enum _ACTIVE_CLK_SRC
{
    CLK_SRC_XTAL  = 0,
    CLK_SRC_PLL1,
    CLK_SRC_PLL2,
    CLK_SRC_PLL3,
    CLK_SRC_OSC,
    CLK_SRC_DEF_MAX,
} ACTIVE_CLK_SRC_TYPE;

typedef enum _CLK_FREQ_TYPE
{
    CLK_0KHZ,   //XTAL div64
    CLK_625KHZ,   //XTAL div64
    CLK_1P25MHZ, //XTAL div32
    CLK_2P5MHZ,  //XTAL div16
    CLK_3P3MHZ,  //XTAL div12
    CLK_4MHZ,  // XTAL div10
    CLK_5MHZ,  // XTAL div8
    CLK_8MHZ,  // XTAL div5
    CLK_10MHZ, // XTAL div4
    CLK_20MHZ, // XTAL div2
    CLK_30MHZ,  // PLL1 120MHz div4
    CLK_35MHZ,  // PLL1 140MHz div4
    CLK_40MHZ, // XTAL max freq, PLL1 160MHz div4
    CLK_45MHZ, // PLL1 180MHz div4
    CLK_50MHZ, // PLL1 200MHz div4
    CLK_60MHZ, // PLL1 120MHz div2
    CLK_70MHZ, // PLL1 140MHz div2
    CLK_80MHZ, // PLL1 160MHz div2
    CLK_90MHZ, // PLL1 180MHz div2
    CLK_100MHZ, // PLL1 200MHz div2
    CLK_120MHZ,
    CLK_125MHZ,
    CLK_140MHZ,
    CLK_160MHZ, // PLL2 max freq
    CLK_180MHZ,
    CLK_200MHZ, // PLL1 max freq
    CLK_280MHZ,
    CLK_MAX_FREQ,
} CLK_FREQ_TYPE;

typedef enum _XTAL_MODE
{
    XTAL_MODE_NM     = 0,
    XTAL_MODE_LP     = 1,
    XTAL_MODE_HP     = 4,
} XTAL_MODE;

typedef enum _ACTIVE_CLK_TYPE
{
    CLK_CPU            = 0,
    CLK_DSP            = 1,
    CLK_SPIC0          = 2,
    CLK_SPIC1          = 3,
    CLK_SPIC2          = 4,
    CLK_SPIC3          = 5,
    CLK_40M            = 6,
    CLK_SYS_MODULE_MAX = 7,
    CLK_SDIO0          = CLK_SYS_MODULE_MAX,
    CLK_SDIO1          = 8,
    CLK_SPI1_MASTER    = 9,
    CLK_DISPLAY        = 10,
    CLK_IR             = 11,
    CLK_TIMER_CH6_8    = 12,
    CLK_PPE            = 13,
    CLK_PKE            = 14,
    CLK_IDU            = 15,
    CLK_JPEG           = 16,
    CLK_USB            = 17,
    CLK_GMAC           = 18,
    CLK_MODULE_MAX     = 19,
    CLK_S4             = CLK_MODULE_MAX,
    CLK_S5             = 20,
    CLK_S6             = 21,
    CLK_S7             = 22,
    CLK_S9             = 23,
    DVFS_LIMIT_CLK_MAX = 24,
    CLK_MAX            = DVFS_LIMIT_CLK_MAX,
} ACTIVE_CLK_TYPE;

typedef enum _CLOCK_SRC_STATUS_TYPE
{
    POWER_OFF    = 0,
    POWER_ON     = 1,
} CLOCK_SRC_STATUS_TYPE;

typedef struct
{
    ACTIVE_CLK_SRC_TYPE     src_sel;
    uint32_t                src_output_Hz;
    bool                    src_enable;

} ActiveSrc_InitType;

typedef struct
{
    uint8_t cpu_slow_enable          : 1;
    uint8_t cpu_slow_opt_wfi         : 1;
    uint8_t cpu_slow_opt_dsp         : 1;
    uint8_t cpu_slow_opt_at_tx       : 1;
    uint8_t cpu_slow_opt_at_rx       : 1;
    uint8_t rsvd                     : 3;

} CPU_AUTO_SLOW_TypeDef;

//=============== low power clock sources => 32K clock sources (internal, external) ================
#define _ALPHA_TRACKER_FB_ALG               1
#define SDM_K_CLK_BIT_NUM           19
#define INTERNAL_32K_FW_CAL_TIMER_ID        0x00
#define IS_USE_ALPHA_TRACKER_FB_REG()       \
    (clk_cfg.osc_k_method_sel == SLEEP_K_ALPHA_TRACKER_FB_REG \
     || clk_cfg.osc_k_method_sel == ACTIVE_K_ALPHA_TRACKER_FB_REG)

#define ALTRAC_REFNUM_MASK() (altrac_fb_alg_setting.report0_ref_num - 1)
#define IS_RESTART_NECESSARY() is_alpha_tracker_fb_alg_restart_necessary()
#define IS_USE_ALPHA_TRACKER()  \
    (clk_cfg.osc_k_method_sel == SLEEP_K_ALPHA_TRACKER \
     || clk_cfg.osc_k_method_sel == ACTIVE_K_ALPHA_TRACKER)

typedef enum _OSC_32_CAL_STATUS_TYPE
{
    SW_CAL     = 0,
    HW_CAL     = 1,
} OSC_32_CAL_STATUS_TYPE;

typedef enum _CAL_MODE_TYPE
{
    LINEAR_CAL = 0,
    BINEARY_CAL,
    DSM_CAL,
    DSM_CAL_OUT_SEL,
} CAL_MODE_TYPE;

typedef struct INTERNAL32k_SDM_SETTING_S
{
    uint8_t num_32k_cyc;
    uint8_t min_32k_cyc;
    uint16_t rsvd_0;
    uint32_t en_sdmk_in_dlps: 1;
    uint32_t alpha_fb_reg_sel: 1;     //select alpha_fb_reg as the alpha tracker feedback value
    uint32_t alpha_ls_5: 5;           //alpha = alpha_ls_5 / 32,  alpha_ls_5 = 0~31
    uint32_t alpha_fb_reg: 22;
    uint32_t already_enabled_sdm_cal: 1;
    uint32_t rsvd_1: 2;
} INTERNAL32k_SDM_SETTING_S_TYPE;

typedef enum _OSC_K_METHOD_SEL_TYPE
{
#if (_ALPHA_TRACKER_FB_ALG == 1)
    SLEEP_K_ALPHA_TRACKER_FB_REG = 0,
#endif
    SLEEP_K_ALPHA_TRACKER,
#if (_ALPHA_TRACKER_FB_ALG == 1)
    ACTIVE_K_ALPHA_TRACKER_FB_REG,
#endif
    ACTIVE_K_ALPHA_TRACKER,
    MAX_K_METHOD_SEL_TYPE,
} OSC_K_METHOD_SEL_TYPE;

typedef struct ALPHA_TRACKER_FB_ALG_DATA_S
{
    int32_t sdm_k_tmeter;
    uint32_t sdm_k_clock;
    uint32_t sdm_k_clock_diff;
    uint32_t sdm_k_osc_f;
} ALPHA_TRACKER_FB_ALG_DATA;

typedef volatile struct ALPHA_TRACKER_FB_ALG_S
{
    uint8_t en_alg;
    uint8_t restart_threshold;
    uint8_t report0_ref_num;
    uint8_t cur_index;
    uint8_t last_used_num_32k_cyc;
    uint16_t normal_k_countdown;
    uint32_t last_report0;
    ALPHA_TRACKER_FB_ALG_DATA(*data)[];
} ALPHA_TRACKER_FB_ALG_S_TYPE;

typedef union PON_F08_REG_S  /* Vendor 0xF08 */
{
    uint32_t d32;
    struct
    {
        uint32_t cal_restart: 1;           //[0] R/W
        uint32_t cal_auto_resume: 1;       //[1] R/W
        uint32_t min_force_done: 1;        //[2] R/W
        uint32_t cal_mode: 2;              //[4:3]   R/W
        uint32_t min_32k_cyc: 3;           //[7:5]   R/W
        uint32_t num_32k_cyc: 8;           //[15:8]  R/W
        uint32_t rsvd: 16;                 //[31:17] R/W
    };
} PON_F08_REG_S_TYPE;

typedef union BTON_INDIR_32K_00_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t fw_stop_cal: 1;            //[0]   R/W
        uint16_t reg_sdm_reset_n: 1;        //[1]   R/W
        uint16_t bs_start_bit: 4;           //[5:2]   R/W
        uint16_t cal_mode: 2;               //[7:6]   R/W
        uint16_t cnt_read_mode: 1;          //[8]     R/W
        uint16_t inc_mode: 1;               //[9]     R/W
        uint16_t reg_hw_cal_en: 1;          //[10]    R/W
        uint16_t reg_reset_n: 1;            //[11]    R/W
        uint16_t dsm_fo_sel: 1;             //[12]    R/W
        uint16_t cnt_fref_st_opt: 1;        //[13]    R/W
        uint16_t rsvd_0: 1;                 //[14]    R/W
        uint16_t sw_cal_en: 1;              //[15]    R/W
    };
} BTON_INDIR_32K_00_REG_S_TYPE;

typedef union BTON_INDIR_32K_01_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t center_cnt_fref_15_0: 16;  //[15:0]  R/W
    };
} BTON_INDIR_32K_01_REG_S_TYPE;

typedef union BTON_INDIR_32K_02_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t kt_lim: 6;                 //[5:0]   R/W
        uint16_t rsvd: 10;                  //[15:6] R/W
    };
} BTON_INDIR_32K_02_REG_S_TYPE;

typedef union BTON_INDIR_32K_03_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t tm_RCAL: 15;               //[14:0]  R/W
        uint16_t test_mode: 1;              //[15]    R/W
    };
} BTON_INDIR_32K_03_REG_S_TYPE;

typedef union BTON_INDIR_32K_04_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t inc_step: 4;               //[3:0]   R/W
        uint16_t num_settle_3_0: 4;         //[7:4]   R/W
        uint16_t center_cnt_fref_21_16: 6;  //[13:8]  R/W
        uint16_t rsvd: 2;                   //[15:14] R/W
    };
} BTON_INDIR_32K_04_REG_S_TYPE;

typedef union BTON_INDIR_32K_05_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t tm_N_int: 10;              //[9:0]   R/W
        uint16_t rsvd_0: 2;                 //[11:10] R/W
        uint16_t num_settle_6_4: 3;         //[14:12] R/W
        uint16_t rsvd_1: 1;                 //[15]    R/W
    };
} BTON_INDIR_32K_05_REG_S_TYPE;

typedef union BTON_INDIR_32K_06_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t tm_N_frac: 15;             //[14:0]  R/W
        uint16_t rsvd: 1;                   //[15]    R/W
    };
} BTON_INDIR_32K_06_REG_S_TYPE;

typedef union BTON_INDIR_32K_07_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t N_frac_os: 15;             //[14:0]  R/W
        uint16_t rsvd: 1;                   //[15]    R/W
    };
} BTON_INDIR_32K_07_REG_S_TYPE;


typedef union BTON_INDIR_32K_08_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t criter0: 10;               //[9:0]   R/W
        uint16_t RCAL_h: 4;                 //[13:10] R/W
        uint16_t dbg_sel: 1;                //[14]    R/W
        uint16_t rsvd: 1;                   //[15]    R/W
    };
} BTON_INDIR_32K_08_REG_S_TYPE;

typedef union BTON_INDIR_32K_09_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t RCAL: 15;                  //[14:0]  R
        uint16_t rsvd: 1;                   //[15]    R
    };
} BTON_INDIR_32K_09_REG_S_TYPE;

typedef union BTON_INDIR_32K_0A_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t Cnt_fref_rep_0_15_0: 16;   //[15:0]  R
    };
} BTON_INDIR_32K_0A_REG_S_TYPE;


typedef union BTON_INDIR_32K_0B_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t Cnt_fref_rep_1_15_0: 16;   //[15:0]  R
    };
} BTON_INDIR_32K_0B_REG_S_TYPE;


typedef union BTON_INDIR_32K_0C_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t Cnt_fref_rep_2_15_0: 16;   //[15:0]  R
    };
} BTON_INDIR_32K_0C_REG_S_TYPE;


typedef union BTON_INDIR_32K_0D_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t Cnt_fref_rep_3_15_0: 16;   //[15:0]  R
    };
} BTON_INDIR_32K_0D_REG_S_TYPE;


typedef union BTON_INDIR_32K_0E_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t Cnt_fref_rep_0_21_16: 6;   //[5:0]   R
        uint16_t Cnt_fref_rep_3_21_16: 6;   //[11:6]  R
        uint16_t rsvd: 4;                   //[15:12] R
    };
} BTON_INDIR_32K_0E_REG_S_TYPE;


typedef union BTON_INDIR_32K_0F_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t N_int: 10;                 //[9:0]   R
        uint16_t Cnt_fref_rep_1_19_16: 4;   //[13:10] R
        uint16_t rsvd: 2;                   //[15:14] R
    };
} BTON_INDIR_32K_0F_REG_S_TYPE;


typedef union BTON_INDIR_32K_10_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t N_frac: 15;                //[14:0]  R
        uint16_t rsvd: 1;                   //[15]    R
    };
} BTON_INDIR_32K_10_REG_S_TYPE;


typedef union BTON_INDIR_32K_11_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t Cnt_fref_rep_2_21_16: 6;   //[5:0]   R
        uint16_t kt_cnt: 6;                 //[11:6]  R
        uint16_t LOCK: 2;                   //[13:12] R
        uint16_t rsvd: 2;                   //[15:14] R/W
    };
} BTON_INDIR_32K_11_REG_S_TYPE;

typedef union BTON_INDIR_32K_12_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t dbg_cnt_32k: 14;           //[13:0]  R
        uint16_t cal_stop_ready: 1;         //[14]    R
        uint16_t rsvd_15: 1;                //[15] R
    };
} BTON_INDIR_32K_12_REG_S_TYPE;

typedef union BTON_INDIR_32K_13_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t dsm_cnt_32k: 14;           //[13:0]  R
        uint16_t rsvd_14: 2;                //[15:14] R
    };
} BTON_INDIR_32K_13_REG_S_TYPE;

typedef union BTON_INDIR_32K_14_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t alpha_ls_5: 5;            //[4:0] R
        uint16_t lp_xtal_dbg_cnt_18_16: 3; //[7:5] R
        uint16_t alpha_fb_reg_21_16: 6;    //[13:8] R
        uint16_t alpha_fb_reg_sel: 1;      //[14] R
        uint16_t rsvd: 1;                  //[15] R
    };
} BTON_INDIR_32K_14_REG_S_TYPE;

typedef union BTON_INDIR_32K_15_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t lp_xtal_dbg_cnt_15_0;      //[15:0] R
    };
} BTON_INDIR_32K_15_REG_S_TYPE;

typedef union BTON_INDIR_32K_16_REG_S
{
    uint16_t d16;
    struct
    {
        uint16_t alpha_fb_reg;              //[15:0] R
    };
} BTON_INDIR_32K_16_REG_S_TYPE;

// =================================================================================================

#define get_spic0_clk_div()\
    count_clk_src_div(clk_cfg.clk_src_spic0, clk_freq_all_map[clk_cfg.clk_freq_spic0])
#define get_spic1_clk_div()\
    count_clk_src_div(clk_cfg.clk_src_spic1, clk_freq_all_map[clk_cfg.clk_freq_spic1])
#define get_spic2_clk_div()\
    count_clk_src_div(clk_cfg.clk_src_spic2, clk_freq_all_map[clk_cfg.clk_freq_spic2])
#define get_spic3_clk_div()\
    count_clk_src_div(clk_cfg.clk_src_spic3, clk_freq_all_map[clk_cfg.clk_freq_spic3])
#define get_psram_clk_div()\
    count_clk_src_div(clk_cfg.clk_src_psram, clk_freq_all_map[clk_cfg.psram_freq])


/* ===================================== */
extern uint32_t SystemCpuClock;      /**< System Clock Frequency (Core Clock)*/
extern uint32_t SystemDspClock;
extern uint32_t SysCpuClkIdx;
extern void (*deinit_clock_core_domain_module)(void);
extern void (*clock_set_unit_cfg_rate)(ACTIVE_CLK_TYPE module, CLK_FREQ_TYPE clk_rate,
                                       CLK_FREQ_TYPE clk_rate_slow);
extern void (*clock_get_unit_cfg_rate)(ACTIVE_CLK_TYPE module, CLK_FREQ_TYPE *clk_rate,
                                       CLK_FREQ_TYPE *clk_rate_slow);
extern uint32_t (*get_SystemCpuClock)(void);
extern void (*platform_pm_restore_handle_32k_clk)(void);
extern void (*btmac_pm_restore_handle_internal_32k_clk)(void);
extern uint8_t (*count_clk_40m_div)(uint8_t clk_src_40m);
extern uint8_t (*count_clk_src_div)(uint32_t clk_src_type,
                                    uint32_t clk_rate_wanted);
extern void (*(set_clk_mux_and_div[]))(uint8_t, uint8_t, uint8_t);
extern void (*(set_clock_divider[]))(uint8_t, uint8_t, uint32_t);
extern uint8_t (*get_cpu_clk_div)(CORE_MODE);
extern uint8_t (*get_dsp_clk_div)(CORE_MODE);
extern void (*update_slave_clk_div)(void);
extern void (*update_cpu_bus_clk_div)(uint8_t div, uint8_t div_slow);
extern void (*set_cpu_clock)(uint8_t clk_src, uint8_t div, uint8_t div_slow);
extern void (*set_dsp_clock)(uint8_t clk_src, uint8_t div, uint8_t div_slow);
extern void (*set_spic0_clock)(uint8_t clk_src, uint8_t div, uint8_t div_slow);
extern void (*set_spic1_clock)(uint8_t clk_src, uint8_t div, uint8_t div_slow);
extern void (*set_spic2_clock)(uint8_t clk_src, uint8_t div, uint8_t div_slow);
extern void (*set_spic3_clock)(uint8_t clk_src, uint8_t div, uint8_t div_slow);
extern void (*set_psram_clock)(uint8_t clk_src, uint8_t div, uint8_t div_slow);
extern void (*set_40m_clock)(uint8_t clk_src, uint8_t div, uint8_t div_slow);
extern void (*get_clock_gen_rate)(ACTIVE_CLK_TYPE clk_load, CLK_FREQ_TYPE *clk_rate,
                                  CLK_FREQ_TYPE *clk_rate_slow);
extern void (*set_osc40_clk_src)(bool is_enable);
extern void (*set_xtal40_oscillate)(bool is_enable);
extern void (*set_xtal40_capacity)(uint8_t xtal_sc_xi, uint8_t xtal_sc_xo);
extern void (*set_xtal40_low_power_pdck)(void);
extern void (*set_xtal40_aac_calibration)(void);
extern void (*set_pll_clk_src)(ACTIVE_CLK_SRC_TYPE src_type, bool is_enable);
extern void (*set_adc_dac_clock)(bool is_enable);
extern void (*set_pll_ldo)(bool enable);
extern void (*set_pll_pow)(ACTIVE_CLK_SRC_TYPE clk_src, bool is_enable);
extern void (*set_pll_div)(ACTIVE_CLK_SRC_TYPE, uint32_t);
extern void (*set_pll1_cko1_div)(uint32_t pll1_freq_hz);
extern void (*set_pll2_cko2_div)(uint32_t pll2_freq_hz);
extern void (*set_pll3_cko3_div)(uint32_t pll3_freq_hz);
extern void (*set_clock_output)(ACTIVE_CLK_SRC_TYPE type, bool is_enable);
extern void (*settle_required_cko)(void);
extern void (*set_active_mode_clk_src)(void);
extern void (*get_pllx_rate)(ACTIVE_CLK_SRC_TYPE pll_src, uint32_t *pll_rate);
extern void (*disable_unused_clock)(void);
extern bool (*check_clock_src_output)(ACTIVE_CLK_SRC_TYPE active_clk_src);
extern void (*set_clk_40m_and_log)(void);
extern void (*set_cpu_auto_slow)(CPU_AUTO_SLOW_TypeDef cpu_slow_type);
extern bool (*set_active_clock_src_output)(ActiveSrc_InitType src_config);

/* 32k */
extern bool(*(is_module_clk_32000Hz[NUM_MODULE]))(void);
extern void (*set_required_32k_rtc_in)(void);
extern void (*set_32k_clk_path)(void);
extern void (*init_32k_clk_src)(void);
extern void (*(bton_32k_cal_ini[]))(void);
extern void (*(bton_32k_cal_en[]))(void);
extern uint8_t(*(bton_32k_cal_check_lock[]))(void);
extern void (*internal_32k_linear_cal_ini)(void);
extern void (*internal_32k_linear_cal_en)(void);
extern uint8_t(*internal_32k_linear_check_lock)(void);
extern void (*set_32k_clock_setting)(void);
extern void (*internal_32k_cal_timer_cb)(void *p_handle);
extern void (*bton_32k_set_alpha_tracker)(void);
extern INTERNAL32k_SDM_SETTING_S_TYPE sdm_setting;
extern ALPHA_TRACKER_FB_ALG_S_TYPE altrac_fb_alg_setting;
extern bool(*is_alpha_tracker_fb_alg_restart_necessary)(void);
extern void (*alpha_tracker_fb_alg_ini)(void);
extern void (*alpha_tracker_fb_alg_restart)(uint16_t, bool, uint32_t);
extern void (*handle_alpha_tracker_fb_alg)(void);
extern void (*update_alpha_tracker_fb_para)(void);
extern void (*prepare_next_internal_32k_alpha_tracker_sdm_k)(void);
extern bool (*lpm_check_32k_clock)(void);
extern void (*init_sdm_setting)(void);
extern void (*lpm_handle_32k_clk_src)(bool is_enter_lpm);
extern void (*lpm_force_32k_to_osc32)(void);
extern uint16_t (*get_btmac_32k_drift_ppm)(void);
extern void (*lpm_handle_lp_xtal)(bool is_enter_lpm);
extern uint16_t (*get_lop_pof_cal_32k_cycle)(bool *is_need_hw_k);
extern void(*set_clock_gen)(ACTIVE_CLK_TYPE type, ACTIVE_CLK_SRC_TYPE clk_src);
extern void (*set_xtal40_path_state)(bool is_enter_lps);
/* ===================================== */
bool check_required_clk_gen(ACTIVE_CLK_SRC_TYPE clk_src);
bool check_required_32k_clk_src(USE_32K_CLK_SRC_TYPE clk_src);
void init_osc_sdm_timer(void);
void set_clock_gen_rom(ACTIVE_CLK_TYPE type, ACTIVE_CLK_SRC_TYPE clk_src);
uint32_t low_stack_get_cpu_clock(void);
uint8_t get_ahb_wait_cnt(void);
void clock_set_unit_rate(ACTIVE_CLK_TYPE module, uint32_t freq);
uint32_t clock_get_unit_rate(ACTIVE_CLK_TYPE module);
/* Set correspoinding function pointer during boot:  */

#ifndef _IS_ASIC_
void test_ahb_wait_cnt_config(void);
#endif

#endif
