/***************************************************************************
 Copyright (C) Realtek Semiconductor Corp.
 This module is a confidential and proprietary property of Realtek and
 a possession or use of this module requires written permission of Realtek.
 ***************************************************************************/

/**
 * \file
 * Contains external definitions for PHY driver, this file exports enum which need
 * to be used in other files, and prevent circular dependency or redefinition
 */

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef _LC_PHY_INIT_EXT_DEFINES_H_
#define _LC_PHY_INIT_EXT_DEFINES_H_

/*============================================================================*
 *                               Header Files
 *============================================================================*/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "hci_phy_vendor_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Compile Flags
 *============================================================================*/
#define RL6497

// rf mode
#define SUPPORT_POLAR_TX                                0
#define SUPPORT_IQM_MODE                                1
#define SUPPORT_TPM_MODE                                0

#define SUPPORT_SWITCH_RF_MODE_BY_LINK                  0

#define MODEM_SRAM_DEBUG                                1

/*============================================================================*
 *                              Definitions
 *============================================================================*/
#define ROM_DEFAULT_RSSI                                (-108)
#define ROM_DEFAULT_THERMAL_SLOPE_A                     (0.1136)
#define ROM_DEFAULT_THERMAL_SLOPE_B                     (0.5936)

#define STEP_NUM                                        8

#define TRANS_MODEM_REG(x)                              (((x) >> 1) | 0x40)

#if (MODEM_SRAM_DEBUG == 1)
#define LOG_NUM_1TIME                   64
#define MODEM_SRAM_DBG_ENTRY_NUM        (256*2)
#define MODEM_SRAM_DEBUG_ENDING_PATTERN 0x00000000FFFFFFFF
#endif /*MODEM_SRAM_DEBUG*/
/*============================================================================*
 *                              Types
 *============================================================================*/
typedef enum
{
#if (SUPPORT_POLAR_TX == 1)
    POLAR_TX,
#endif
#if (SUPPORT_IQM_MODE == 1)
    IQM_MODE,
#endif
#if (SUPPORT_TPM_MODE == 1)
    TPM_MODE,
#endif
    NUM_RF_MODE
} RF_MODE;

typedef enum
{
    LEGACY_BR,
    LEGACY_EDR,
    LEGACY_TYPE_NUM,
} LEGACY_TYPE;

typedef enum
{
    LE_ADV,
    LE_LINK,
    LE_TYPE_NUM,
} LE_TYPE;

typedef enum
{
    DEFAULT_RF_MODE_BREDR,
    DEFAULT_RF_MODE_BLE,
    DEFAULT_RF_MODE_TYPE_NUM,
} DEFAULT_RF_MODE_TYPE;

typedef enum
{
    LBT_MODE_NO_TX,
    LBT_MODE_REDUCE_POWER_TX,
    LBT_MODE_RSVD1,
    LBT_MODE_RSVD2,
    LBT_MODE_NUM
} LBT_MODE;

typedef enum
{
    //When lc_baseband_partial_reset() call lc_check_lbt(), lmp not init yet,
    //cannot judge by lmp_connection_entity[ce_index].epc_data.epc_step[mod_scheme]
    LBT_INIT_COND,
    //Broadcast use LUT[0], no ce_index
    LBT_BC_COND,
    LBT_LINK_COND
} LBT_COND;

typedef union
{
    uint16_t u16;
    struct
    {
        uint16_t rck_log:           1;
        uint16_t rxadck_log:        1;
        uint16_t lbt_log:           1;
        uint16_t iqk_lok_log:       1;
        uint16_t flatk_log:         1;
        uint16_t funcptr_log:       1;
        uint16_t rf_ctrl_log:       1;
        uint16_t modem_ctrl_log:    1;
        uint16_t rssi_diff_log:     1;
        uint16_t rsvd:              7;
    };
} PHY_LOG_OPTION;

typedef union
{
    uint8_t u8;
    struct
    {
        uint8_t rf_pi_write_lock:   1; //When the flag is TRUE, the RF PI write cannot be used
        uint8_t rsvd:               7;
    };
} PHY_FEATURE_OPTION;

typedef union
{
    uint8_t u8;
    struct
    {
        uint8_t calibration_mode:   1;
        uint8_t scan_3ch_mode:      1;
        uint8_t scan_even_ch_mode:  1;
        uint8_t rsvd:               5;
    };
} MODEM_PSD_SETTING_TYPE;

typedef struct
{
    PHY_LOG_OPTION phy_log_option;
    PHY_FEATURE_OPTION phy_feature_option;

    uint8_t last_tmeter_lck[NUM_RF_MODE];
    uint8_t last_tmeter_tpmk;

    uint8_t rxadck_upperbound;
    uint8_t rxadck_lowerbound;
    uint16_t rxadck_sum;
    int8_t phy_rssi0_dBm[NUM_RF_MODE];

    uint8_t is_support_rf_adaptivity :  1;
    uint8_t modem_psd_enable:           1;
    uint8_t modem_mse_enable:           1;
    uint8_t rsvd:                       5;

    MODEM_PSD_SETTING_TYPE modem_psd_setting;

    RF_MODE rf_mode_for_pouter;
    uint32_t phy_version;
} PHY_INIT_MANAGER;

/* ============================================== */
/* ========== Thermal Meter Definition ========== */
/* ============================================== */
typedef struct
{
    void *thermal_timer;
    uint8_t thermal_now;
    uint8_t thermal_prev;
    double tmeter_slope;
} THERMAL_MANAGER;

/* ============================================== */
/* ========= Tx Power Track Definition ========== */
/* ============================================== */
typedef enum
{
    SET_MIN_TX_POWER = 0x7E,
    SET_MAX_TX_POWER = 0x7F,
} TX_POWER_REQUEST_TYPE;

typedef enum
{
    TXGAIN_BR_1M = 0,
    TXGAIN_EDR_2M,
    TXGAIN_EDR_3M,
    TXGAIN_LEGACY_MAX,
    TXGAIN_LE_1M = TXGAIN_LEGACY_MAX,
    TXGAIN_LE_2M,
    TXGAIN_LR,
    TXGAIN_LE_1M_ADV,
    TXGAIN_LE_2M_ADV,
    TXGAIN_LE_1M_2402,
    TXGAIN_LE_1M_2480,
    TXGAIN_LE_2M_2402,
    TXGAIN_LE_2M_2480,
    TXGAIN_LE_MAX,
    NUM_TXGAIN_TYPE = TXGAIN_LE_MAX,
} TXGAIN_TYPE;

typedef struct
{
    int8_t tx_power_table_offset;

    uint8_t txgain_br_1M[NUM_RF_MODE][STEP_NUM];
    uint8_t txgain_edr_2M[STEP_NUM];
    uint8_t txgain_edr_3M[STEP_NUM];

    uint8_t txgain_LE1M[NUM_RF_MODE];
    uint8_t txgain_LE2M[NUM_RF_MODE];
    uint8_t txgain_LR[NUM_RF_MODE];
    uint8_t txgain_LE1M_adv[NUM_RF_MODE];
    uint8_t txgain_LE2M_adv[NUM_RF_MODE];
    uint8_t txgain_LE1M_2402[NUM_RF_MODE];
    uint8_t txgain_LE1M_2480[NUM_RF_MODE];
    uint8_t txgain_LE2M_2402[NUM_RF_MODE];
    uint8_t txgain_LE2M_2480[NUM_RF_MODE];

    uint8_t max_tx_step_index[NUM_RF_MODE];
    uint8_t default_br_tx_step_index[NUM_RF_MODE];
    uint8_t default_edr_tx_step_index[NUM_RF_MODE];

    int8_t txgaink[NUM_RF_MODE];
    uint8_t raw_txgain_lbt[NUM_RF_MODE]; //Record the raw txgain index of 10 dBm, used for LBT
    uint8_t rxgain_curve_valid[NUM_RF_MODE];
    uint8_t edr_txgain_backoff; //unit: 0.5dB
    uint8_t tmeter_default_final;

    uint8_t enable_tx_power_track_log: 1;
    uint8_t enable_0p25_comp_for_thermal_tracking: 1;
    uint8_t need_0p25_comp_for_thermal_tracking: 1;
    uint8_t rsvd: 5;

    uint16_t rf_flatk_final;
    uint8_t modem_flatk_neg_0p25_final;
    uint16_t modem_flatk_neg_0p50_final;

    double slope_a;
    double slope_b;
    double compensation_threshold;

    uint8_t (*post_processing_table[NUM_TXGAIN_TYPE])[2];
    uint8_t post_processing_table_size[NUM_TXGAIN_TYPE];

#if (SUPPORT_TPM_MODE == 1)
    uint8_t *tpm_txgain_table;
    int8_t *tpm_txpower_table;
    uint8_t tpm_txgain_txpower_table_size;
#endif
} TX_POWER_TRACK_MANAGER;

/* ============================================== */
/* =========== PHY Config Definition ============ */
/* ============================================== */
typedef union
{
    uint8_t u8;
    struct
    {
#if (SUPPORT_POLAR_TX == 1)
        uint8_t polar_tx:   1;
#endif
#if (SUPPORT_IQM_MODE == 1)
        uint8_t iqm_mode:   1;
#endif
#if (SUPPORT_TPM_MODE == 1)
        uint8_t tpm_mode:   1;
#endif
uint8_t rsvd: (8 - (SUPPORT_POLAR_TX + SUPPORT_IQM_MODE + SUPPORT_TPM_MODE));
    };
} RF_MODE_VALID_TYPE;

typedef struct
{
    /* PHY Initilization Flow Control Parameters */
    uint8_t enable_phy:                 1;  // [0]      rw  1: enable   0: disable    1'b0
    uint8_t enable_thermal_tracking:    1;  // [1]      rw  1: enable   0: disable    1'b1
    uint8_t enable_tx_power_track:      1;  // [2]      rw  1: enable   0: disable    1'b0
    uint8_t enable_cfo_track:           1;  // [3]      rw  1: enable   0: disable    1'b0
    uint8_t enable_lbt:                 1;  // [4]      rw  1: enable   0: disable    1'b0
    uint8_t re_tpmk_at_switch_mode:     1;  // [5]      rw  1: enable   0: disable    1'b0
    uint8_t enable_tpmk_track:          1;  // [6]      rw  1: enable   0: disable    1'b0
    uint8_t enable_3wire_ext_rf:        1;  // [7]      rw  0: INTERNAL_RF 1: Connect RL6497 1'b0

    RF_MODE_VALID_TYPE rf_mode_valid;
    RF_MODE_VALID_TYPE txgain_flatk_module_valid;
    RF_MODE_VALID_TYPE txgaink_module_valid;
    RF_MODE_VALID_TYPE rxgaink_module_valid;

    LBT_MODE lbt_mode;

    // Before compare with txgain index using in rfc.0x02 (for LBT mechanism),
    // these var need to reduce lbt_ant_gain then experience post-processing
    uint8_t txgain_10dBm_raw_index[NUM_RF_MODE];
    int8_t lbt_ant_gain; //unit: 0.5dBi

    /* Thermal Meter Parameters */
    uint8_t thermal_default;
    uint8_t thermal_default_celsius;
    uint8_t thermal_default_txgaink;                // may not be 25 degree celsius
    uint8_t thermal_update_interval;                // unit: 1s
    uint8_t thermal_read_after_trigger_interval;    // unit: 1us

    /* TX Power Track Parameters */
    /* Legacy tx power */
    uint8_t max_txgain_br_1M[NUM_RF_MODE];
    uint8_t max_txgain_edr_2M;
    uint8_t max_txgain_edr_3M;

    /* LE tx power */
    uint8_t max_txgain_LE1M[NUM_RF_MODE];
    uint8_t max_txgain_LE2M[NUM_RF_MODE];
    uint8_t max_txgain_LR[NUM_RF_MODE];
    uint8_t max_txgain_LE1M_adv[NUM_RF_MODE];
    uint8_t max_txgain_LE2M_adv[NUM_RF_MODE];
    uint8_t max_txgain_LE1M_2402[NUM_RF_MODE];
    uint8_t max_txgain_LE1M_2480[NUM_RF_MODE];
    uint8_t max_txgain_LE2M_2402[NUM_RF_MODE];
    uint8_t max_txgain_LE2M_2480[NUM_RF_MODE];

    /* Setting */
    uint8_t txgain_upperbound[NUM_RF_MODE];
    uint8_t txgain_lowerbound[NUM_RF_MODE];
    uint8_t txgain_level_diff[NUM_RF_MODE];
    uint8_t txgain_level_num[NUM_RF_MODE];

    int8_t tx_power_lowerbound[NUM_RF_MODE];

    /* Parameter */
    uint8_t br_txgain_mapping_0dBm[NUM_RF_MODE];
    int8_t txgain_flatk_module[4];
    int8_t txgaink_module[NUM_RF_MODE];
    uint8_t rxgaink_module[NUM_RF_MODE][4];
    int8_t tpm_txpower_track_table[17];

    uint8_t default_br_pwr_level[NUM_RF_MODE];
    uint8_t default_edr_pwr_level[NUM_RF_MODE];

    int8_t path_loss_module;

    uint8_t rsvd1[16];
} PHY_CFG;

#if (SUPPORT_SWITCH_RF_MODE_BY_LINK == 1)
enum
{
    MODIFY_BREDR_BROADCAST_RF_MODE = 0,
    MODIFY_BLE_ADV_RF_MODE,
    MODIFY_LINK_RF_MODE,
    MODIFY_BREDR_RF_MODE,
    MODIFY_BLE_RF_MODE,
    MODIFY_BREDR_AND_BLE_RF_MODE,
    TOTAL_MODIFY_RANGE
};
#endif

typedef enum
{
    MODEM_TRX_DISABLE,
    MODEM_TX_ENABLE,
    MODEM_RX_ENABLE,
    MODEM_TRX_ENABLE,
} MODEM_TRX_MODE;

typedef enum
{
    MODEM_TX_CONTROL_DISABLE = 0x4,
    MODEM_TX_CONTROL_ENABLE = 0x5,
    MODEM_RX_CONTROL_DISABLE = 0x8,
    MODEM_RX_CONTROL_ENABLE = 0xA,
    MODEM_TRX_CONTROL_DISABLE = 0xC,
    MODEM_TRX_CONTROL_ENABLE = 0xF,
} MODEM_TRX_CONTROL_TYPE;

typedef enum
{
    MODEM_PI_PAGE_0,
    MODEM_PI_PAGE_1,
    MODEM_PI_PAGE_2,
    MODEM_PI_PAGE_3,
    MODEM_PI_PAGE_4,
    MODEM_PI_PAGE_5,
    MODEM_PI_PAGE_6,
    MODEM_PI_PAGE_7,
    MODEM_PI_PAGE_NEW_AGC_TAB0,
    MODEM_PI_PAGE_W_ADDR0_START = MODEM_PI_PAGE_NEW_AGC_TAB0,
    MODEM_PI_PAGE_NEW_AGC_TAB1,
    MODEM_PI_PAGE_NEW_AGC_TAB2,
    MODEM_PI_PAGE_NEW_AGC_TAB3,
    MODEM_PI_PAGE_OLD_AGC_TAB0,
    MODEM_PI_PAGE_W_ADDR0_END = MODEM_PI_PAGE_OLD_AGC_TAB0,
    MODEM_PI_PAGE_C = MODEM_PI_PAGE_OLD_AGC_TAB0,
    MODEM_PI_PAGE_D,
    MODEM_PI_PAGE_E,
} MODEM_PI_PAGE;

/* ============================================== */
/* ========= Modem PSD Scan Definition ========== */
/* ============================================== */

typedef union
{
    uint32_t d32[4];
    struct // RTL8703B, RTL8822B, RTL8821B-MP
    {
        uint32_t psd_avg_dc: 16;              //[15:0]
        uint32_t psd_avg_pos: 16;             ///[31:16]
        uint32_t psd_avg_neg: 16;             //[47:32]
        uint32_t mp_gain_idx: 6;              //[53:48]
        uint32_t gnt_bt_hold: 1;              //[54]
        uint32_t gnt_bt_final: 1;             //[55]

        uint32_t psd_u28_scale: 3;            //[58:56]
        uint32_t gnt_wl_hold: 1;              //[59]
        uint32_t gnt_wl_final: 1;             //[60]
        uint32_t wifi_txon_hold: 1;           //[61]
        uint32_t wifi_txon_final: 1;          //[62]
        uint32_t rsvd1: 1;                    //[63]

        uint32_t psd_avg_dfir_lsb: 16;        //[79:64]
        uint32_t psd_avg_dfir_msb: 4;         //[83:80]
        uint32_t psd_clip_ratio_start: 6;     //[89:84]
        uint32_t psd_clip_ratio_end: 6;       //[95:90]
        uint32_t agc_ste: 4;                  //[99:96]
        uint32_t lna_idx_use: 3;              //[102:100]
        uint32_t bpf_mode_use: 1;             //[103]
        uint32_t gain_idx_use: 5;             //[108:104]
        uint32_t rf_pd_en_use: 1;             //[109]
        uint32_t signature_bit: 2;            //[111:110]    //default 2'b0
        uint32_t rsvd3: 16;                   //[127:112]
    };
} MODEM_PSD_REPORT_ELEMENT_S_TYPE;

#if (MODEM_SRAM_DEBUG == 1)
/* ============================================== */
/* ======== Modem SRAM Debug Definition ========= */
/* ============================================== */
typedef enum
{
    MODEM_SRAM_DEBUG_MODE_ADC = 0x00,       /*0 *< ADC debug */
    MODEM_SRAM_DEBUG_MODE_AGC,              /*1 *< AGC debug */
    MODEM_SRAM_DEBUG_MODE_GFSK_A,           /*2 *< GFSK_A debug */
    MODEM_SRAM_DEBUG_MODE_GFSK_B,           /*3 *< GFSK_B debug */
    MODEM_SRAM_DEBUG_MODE_GFSK_C,           /*4 *< GFSK_C debug */
    MODEM_SRAM_DEBUG_MODE_EDR_A,            /*5 *< DPSK_A debug */
    MODEM_SRAM_DEBUG_MODE_EDR_B,            /*6 *< DPSK_B debug */
    MODEM_SRAM_DEBUG_MODE_AGC_10M,          /*7 */
    MODEM_SRAM_DEBUG_MODE_TPM_TPMK,         /*8 */
    MODEM_SRAM_DEBUG_MODE_GFSK_PHASE,       /*9 */
    MODEM_SRAM_DEBUG_MODE_LR_PAYLOAD,       /*10*/
    MODEM_SRAM_DEBUG_MODE_AOX,              /*11*/
    MODEM_SRAM_DEBUG_MODE_LBT,              /*12*/
    MODEM_SRAM_DEBUG_MODE_INVALID = 0xff    /**< Invalid Radio Type */
} MODEM_DBG_MODE_TYPE;

typedef enum
{
    MODEM_SRAM_DEBUG_ADC_RATE_20M,
    MODEM_SRAM_DEBUG_ADC_RATE_10M,
} MODEM_DBG_ADC_RATE_TYPE;

typedef enum
{
    MODEM_SRAM_DEBUG_PKT_SIZE_16 = 0x00,
    MODEM_SRAM_DEBUG_PKT_SIZE_32 = 0x01,
    MODEM_SRAM_DEBUG_PKT_SIZE_64 = 0x02,
    MODEM_SRAM_DEBUG_PKT_SIZE_128 = 0x03,
    MODEM_SRAM_DEBUG_PKT_SIZE_1PKT = 0x07,
} MODEM_DBG_PKT_SIZE;

typedef struct
{
    uint32_t adc_re7:                               8;
    uint32_t adc_re6:                               8;
    uint32_t adc_re5:                               8;
    uint32_t adc_re4:                               8;

    uint32_t adc_re3:                               8;
    uint32_t adc_re2:                               8;
    uint32_t adc_re1:                               8;
    uint32_t adc_re0:                               8;
} MODEM_SRAM_DEBUG_MODE_0_TYPE;

typedef struct
{
    uint32_t agc_ste:                               4;
    uint32_t lna_idx_use:                           3;
    uint32_t gain_idx_use:                          5;
    uint32_t mp_gain_idx:                           6;
    uint32_t pga_gain_offset:                       6;
    uint32_t lna_constraint:                        3;
    uint32_t share_lna:                             1;
    uint32_t lna_wifi:                              3;
    uint32_t bt_rfpd_0:                             1;

    uint32_t bt_rfpd_3_1:                           3;
    uint32_t bpf_mode_use:                          1;
    uint32_t rf_pd_en_use:                          1;
    uint32_t sync_done:                             1;
    uint32_t cfoe_post_fir_80M:                     15;
    uint32_t rsvd:                                  11;
} MODEM_SRAM_DEBUG_MODE_1_TYPE; // AGC signals @ (10MHz and agc_ste changes)

typedef struct
{
    uint32_t dc_acc:                                21;
    uint32_t eq_out_d_10_0:                         11;

    uint32_t eq_out_d_15_11:                        5;
    uint32_t cfoe_post_fir_14_3:                    12;
    uint32_t cfoe_pre_fir_14_3:                     12;
    uint32_t rsvd:                                  3;
} MODEM_SRAM_DEBUG_MODE_2_TYPE;

typedef struct
{
    uint32_t ffe_gfsk_coef2_20_9:                   12;
    uint32_t ffe_gfsk_coef3_20_9:                   12;
    uint32_t ffe_gfsk_coef4_16_9:                   8;

    uint32_t ffe_gfsk_coef4_20_17:                  4;
    uint32_t ffe_gfsk_coef5_20_9:                   12;
    uint32_t ffe_gfsk_coef6_20_9:                   12;
    uint32_t rsvd:                                  4;
} MODEM_SRAM_DEBUG_MODE_3_TYPE;

typedef struct
{
    uint32_t ffe_gfsk_coef7_20_9:                   12;
    uint32_t ffe_gfsk_coef8_20_9:                   12;
    uint32_t fbf_gfsk_coef1_16_9:                   8;

    uint32_t fbf_gfsk_coef1_20_17:                  4;
    uint32_t fbf_gfsk_coef2_20_9:                   12;
    uint32_t fbf_gfsk_coef3_20_9:                   12;
    uint32_t rsvd:                                  4;
} MODEM_SRAM_DEBUG_MODE_4_TYPE;

typedef struct
{
    uint32_t zk_re:                                 12;
    uint32_t zk_im:                                 12;
    uint32_t ki_acc_r_11_4:                         8;

    uint32_t ki_acc_r_19_12:                        8;
    uint32_t eq_out_re:                             12;
    uint32_t eq_out_im:                             12;
} MODEM_SRAM_DEBUG_MODE_5_TYPE;

typedef struct
{
    uint32_t eq_coef0_qz_re:                        12;
    uint32_t eq_coef0_qz_im:                        12;
    uint32_t eq_coef1_qz_re_7_0:                    8;

    uint32_t eq_coef1_qz_re_11_8:                   4;
    uint32_t eq_coef2_qz_im:                        12;
    uint32_t eq_coef2_qz_re:                        12;
    uint32_t rsvd:                                  4;
} MODEM_SRAM_DEBUG_MODE_6_TYPE;

typedef union
{
    uint32_t d32[2];
    MODEM_SRAM_DEBUG_MODE_0_TYPE mode0; // ADC @ 20MHz
    MODEM_SRAM_DEBUG_MODE_1_TYPE mode1; // AGC signals @ (10MHz and agc_ste changes)
    MODEM_SRAM_DEBUG_MODE_2_TYPE mode2; // GFSK signals @ 1MHz
    MODEM_SRAM_DEBUG_MODE_3_TYPE mode3; // GFSK signals @ 1MHz
    MODEM_SRAM_DEBUG_MODE_4_TYPE mode4; // GFSK signals @ 1MHz
    MODEM_SRAM_DEBUG_MODE_5_TYPE mode5; // DPSK signals @ 1MHz
    MODEM_SRAM_DEBUG_MODE_6_TYPE mode6; // DPSK signals @ 1MHz
} MODEM_SRAM_DEBUG_ELEMENT_S_TYPE;

typedef struct
{
    union
    {
        uint8_t status_byte;
        struct
        {
            uint8_t debug_enable: 1;
            uint8_t debug_log_enable: 1;
            uint8_t captured_flag: 1;
            uint8_t xdh5_trig_enable: 1;
            uint8_t xdh5_trig_crc_ok: 1;
            uint8_t le_trig_enable: 1;
            uint8_t le_trig_crc_ok: 1;
            uint8_t xdh5_xdh3_3dh1_error_log_enable: 1;
        };
    };

    union
    {
        uint8_t control_byte;
        struct
        {
            uint8_t sram_adc_rate: 1;
            uint8_t sram_pktg_size: 3;
            uint8_t rsvd: 4;
        };
    };

    MODEM_SRAM_DEBUG_ELEMENT_S_TYPE *debug_mem;
    MODEM_SRAM_DEBUG_ELEMENT_S_TYPE *debug_array;
    uint16_t entry_num;
    uint16_t debug_log_count;
} MODEM_SRAM_DEBUG_MANAGER;
#endif /*MODEM_SRAM_DEBUG*/
/*============================================================================*
 *                              Variables
 *============================================================================*/
extern PHY_CFG phy_cfg;
extern PHY_INIT_MANAGER phy_init_manager;
extern THERMAL_MANAGER thermal_manager;
extern TX_POWER_TRACK_MANAGER tx_power_track_manager;

extern void (*phy_init)(uint8_t);

extern uint8_t(*hci_phy_vendor_cmd)(HCI_CMD_PKT *);
extern void (*hci_phy_vendor_event)(HCI_EVENT_PKT *, uint8_t *, uint32_t);

extern uint8_t (*get_lbt_threshold)(TXGAIN_TYPE, RF_MODE);
extern bool (*lbt_check)(uint8_t, TXGAIN_TYPE, RF_MODE);

extern void (*lc_init_radio)(uint8_t);

extern int8_t (*legacy_get_phy_rssi0_dbm)(void);
extern int8_t (*le_get_phy_rssi0_dbm)(void);
extern int8_t(*legacy_get_rssi_offset)(int8_t, int8_t, int8_t);
extern int8_t (*lc_calculate_log_from_rssi)(uint16_t, uint8_t);

/* ============================================== */
/* ========= Tx Power Track Definition ========== */
/* ============================================== */
extern bool (*tx_power_table_offset_update)(void);
extern bool (*tx_power_track)(void);
extern bool (*force_tx_power_track)(void);
extern bool (*(txgain_to_power_conversion[]))(int8_t *, uint8_t, TXGAIN_TYPE);
extern bool (*(power_to_txgain_conversion[]))(uint8_t *, int8_t, TXGAIN_TYPE);

extern int8_t (*get_tx_power_upperbound)(TXGAIN_TYPE, RF_MODE);
extern int8_t (*get_tx_power_lowerbound)(TXGAIN_TYPE, RF_MODE);
extern uint8_t (*txgain_index_apply_compensation)(uint8_t, TXGAIN_TYPE, RF_MODE, bool);
extern bool (*get_valid_txgain_index_from_tx_power)(int8_t, TXGAIN_TYPE, RF_MODE, bool, uint8_t *,
                                                    int8_t *);

extern MODEM_PSD_REPORT_ELEMENT_S_TYPE *g_modem_psd_report_array;
extern uint8_t (*modem_psd_get_entry_num)(void);

#if (MODEM_SRAM_DEBUG == 1)
extern MODEM_SRAM_DEBUG_MANAGER modem_sram_debug_manager;
#endif

extern uint16_t (*rfc_ioq_read)(uint8_t);
extern uint16_t (*rfc_reg_read)(uint8_t);
extern void (*rfc_reg_write)(uint8_t, uint16_t);

extern uint8_t (*get_max_tx_step_index)(uint8_t);
extern uint8_t (*get_min_tx_step_index)(uint8_t);
extern uint8_t (*get_default_tx_step_index)(TXGAIN_TYPE, uint8_t);

/* ========== APIs registered from lowerstack ========== */
extern void (*pf_BB_write_baseband_register)(uint16_t, uint16_t);
extern uint16_t(*pf_BB_read_baseband_register)(uint16_t);
extern uint16_t(*pf_le_read_register)(uint16_t);
extern void (*pf_le_write_register)(uint16_t, uint16_t);
extern void (*pf_update_legacy_tx_gain)(RF_MODE, uint8_t *, uint8_t *, uint8_t *);
extern void (*pf_update_le_adv_tx_gain)(RF_MODE, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t,
                                        uint8_t);
extern void (*pf_update_le_link_tx_gain)(void);
extern void (*pf_update_le_tx_power_database)(void);

extern RF_MODE(*pf_ll_get_rf_mode_by_register)(uint8_t);
extern RF_MODE(*pf_lc_get_rf_mode_by_register)(uint8_t);

extern RF_MODE(*pf_get_default_rf_mode)(uint8_t);

extern uint8_t(*modify_rf_mode_in_mac)(uint8_t, uint8_t, uint16_t);

extern void (*pf_set_tx_agc_1m_register)(uint16_t, uint16_t, uint16_t, uint16_t);
extern void (*pf_set_tx_agc_2m_register)(uint16_t, uint16_t, uint16_t, uint16_t);
extern void (*pf_set_tx_agc_3m_register)(uint16_t, uint16_t, uint16_t, uint16_t);

extern void (*pf_radio_reg_write)(uint8_t, uint16_t, bool);
extern uint16_t(*pf_radio_reg_read)(uint8_t, bool);

extern void (*pf_lc_init_radio_select)(void);

extern uint32_t (*mac_phy_interface)(uint32_t, uint32_t);
/* ========== End of APIs registered from lowerstack ========== */

/*============================================================================*
 *                              Functions
 *============================================================================*/
void rf_control_init(bool dlps_flow);
void rf_control_set(bool enable);
bool rf_control_get(void);
bool rf_control_wait_ready(uint32_t wait_count);
uint16_t rf_control_get_control_bit(void);

void modem_control_init(bool dlps_flow);
void modem_control_set(MODEM_TRX_CONTROL_TYPE type);
MODEM_TRX_MODE modem_control_get(void);

void rfc_reg_update(uint8_t addr, uint16_t mask, uint16_t wdata);
bool rfc_indirect_read(uint8_t addr, uint16_t *p_rdata);
bool rfc_indirect_write(uint8_t addr, uint16_t wdata);
bool rfc_indirect_update(uint8_t addr, uint16_t mask, uint16_t wdata);
uint16_t modem_pi_read(uint8_t modem_page, uint8_t addr);
void modem_pi_write(uint8_t modem_page, uint8_t addr, uint16_t wdata);
void modem_pi_update(uint8_t modem_page, uint8_t addr, uint16_t mask, uint16_t wdata);

bool modem_psd_check_enable(void);
bool modem_psd_check_calibration_mode(void);
bool modem_psd_check_scan_3ch_mode(void);
bool modem_psd_check_scan_even_ch_mode(void);

bool modem_mse_check_enable(void);
uint32_t get_current_snr(void);
void read_modem_rx_report(void);

uint8_t get_thermal_meter(void);
int16_t get_thermal_meter_celsius(void);

uint8_t get_txgain_index(uint8_t step_index, TXGAIN_TYPE txgain_type, RF_MODE rf_mode);
int8_t get_tx_power(uint8_t step_index, TXGAIN_TYPE txgain_type, RF_MODE rf_mode);
uint8_t get_txgain_index_from_tx_power_track_table(uint8_t step_index, TXGAIN_TYPE txgain_type,
                                                   RF_MODE rf_mode);

RF_MODE legacy_get_tx_rf_mode(uint8_t ce_index);
RF_MODE legacy_get_rx_rf_mode(void);
RF_MODE le_get_tx_rf_mode(uint8_t conn_entry_ori);
RF_MODE le_get_rx_rf_mode(void);

#if (MODEM_SRAM_DEBUG == 1)
void btrf_modem_sram_debug_set_size(uint8_t value);
void btrf_modem_sram_debug_set_en(uint8_t value);
uint8_t btrf_modem_sram_debug_get_en(void);
void btrf_modem_sram_debug_set_rst(uint8_t value);
void btrf_modem_sram_debug_set_mode(uint8_t value);
void btrf_modem_sram_debug_init(MODEM_SRAM_DEBUG_ELEMENT_S_TYPE *modem_sram_debug_array,
                                uint8_t mode);
void btrf_modem_sram_debug_test_log(MODEM_SRAM_DEBUG_ELEMENT_S_TYPE *modem_sram_debug_array);
#endif

int8_t get_le_min_output_power(RF_MODE rf_mode);
int8_t get_le_max_output_power(RF_MODE rf_mode);

#ifdef __cplusplus
}
#endif

#endif /*_LC_PHY_INIT_EXT_DEFINES_H_*/

