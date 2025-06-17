/***************************************************************************
 Copyright (C) Realtek Semiconductor Corp.
 This module is a confidential and proprietary property of Realtek and
 a possession or use of this module requires written permission of Realtek.
 ***************************************************************************/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef _LC_PHY_INIT_H_
#define _LC_PHY_INIT_H_

/*============================================================================*
 *                               Header Files
 *============================================================================*/
#include "lc_phy_init_external_defines.h"
#include "platform.h"
#include "patch.h"

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                              Definitions
 *============================================================================*/
#define ABS_DOUBLE(x)                       (((x) > 0) ? (x) : (-1 * (x)))

#define RF_CTRL_RF_OFF_BIT                  BIT8
#define RF_CTRL_RF_OFF_READY_BIT            BIT9

#define MODEM_TX_EDGE_DISABLE_BIT           BIT5
#define MODEM_RX_EDGE_DISABLE_BIT           BIT7

/* The MACRO can translate from tx_table_idx in LUT to corresponding index in   *
 * tx_power_track_manager.txgain_XX. e.g. If we have 4 levels (0 ~ 3), the      *
 * largest level in LUT is 3, and the corresponding txgain index is stored      *
 * in tx_power_track_manager.txgain_XX[4]                                       */
#define TRANS_FROM_LEVEL_IN_LUT(num)        (STEP_NUM - 1 - (num))

// Vendor register offset for modem/rfc pi
#define REG_BTRFC_PI_IF                     0x348
#define REG_BTPHY_FPI_IF                    0x354
#define IS_RFC_PAGE0                        (!(rfc_reg_read(0x0E) & 0xE000))

/* =============================================== */
/* ========== Script Parsing Definition ========== */
/* =============================================== */
#define PHY_INIT_MODEM_REG_WRITE            0x0000
#define PHY_INIT_MODEM_REG_UPDATE           0x1000
#define PHY_INIT_RFC_REG_WRITE              0x2000
#define PHY_INIT_RFC_REG_UPDATE             0x3000
#define PHY_INIT_BLUEWIZ_REG_WRITE          0x4000
#define PHY_INIT_BLUEWIZ_REG_UPDATE         0x5000
#define PHY_INIT_LE_REG_WRITE               0x6000
#define PHY_INIT_LE_REG_UPDATE              0x7000
#define PHY_INIT_VENDOR_REG_WRITE           0x8000
#define PHY_INIT_VENDOR_REG_UPDATE          0x9000
#define PHY_INIT_DELAY_US                   0xA000
#define PHY_INIT_DELAY_MS                   0xB000
#if (SUPPORT_POLAR_TX == 1)
#define PHY_INIT_RSVD_C                     0xC000
#define PHY_INIT_RSVD_D                     0xD000
#define PHY_INIT_RSVD_E                     0xE000
#else
#define PHY_INIT_LCK_POLLING                0xC000
#define PHY_INIT_TPMK_POLLING               0xD000
#define PHY_INIT_IQKLOK_POLLING             0xE000
#endif /*SUPPORT_POLAR_TX*/
#define PHY_INIT_SET_BIT_MASK               0xF000

#define PHY_INIT_GET_REG_TYPE(x)            (x & 0xF000)
#define PHY_INIT_GET_REG_ADDR(x)            (x & 0x0FFF)
#define PHY_INIT_GET_MODEM_REG_PAGE(x)      ((x & 0x0F00) >> 8)
#define PHY_INIT_GET_MODEM_REG_ADDR(x)      (x & 0x00FF)

#if ((SUPPORT_IQM_MODE == 1) || (SUPPORT_TPM_MODE == 1))
/* for lck_efuse2rfc */
#define BASE_MASK               0x7F
#define BASE_NOB                7
#define RFC_DELTA_NOB           3
#define LCK_MASK                0x3
#endif

// for MODEM PSD SCAN IF Setting: pwreg5a.reg_btm_if_freq_psd //
#define MODEM_PSD_IF_1P4                  0x23D7
#define MODEM_PSD_IF_1P6                  0x28F5
#define MODEM_PSD_IF_2P4                  0x3D71
#define MODEM_PSD_IF_2P5                  0x4000
#define MODEM_PSD_IF_2P8                  0x47AE
#define MODEM_PSD_IF_3P0                  0x4CCC
#define MODEM_PSD_IF_3P2                  0x51EB
#define MODEM_PSD_MODE_HALF_SLOT          0
#define MODEM_PSD_MODE_FULL_SLOT          1

/*============================================================================*
 *                              Types
 *============================================================================*/

/* =================================================== */
/* ========== PHY Initialization Definition ========== */
/* =================================================== */
typedef enum
{
    MODEM_INIT,
    RFC_INIT,
    RCK_INIT,
    RX_ADC_K,
    TMETER_INIT,
    TXGAIN_FLATK,
    RXGAIN_K,
    CURRENT_K,
    GENERAL_PHY_INIT_STATE_MAX,
} GENERAL_PHY_INIT_STATE;

#if (SUPPORT_POLAR_TX == 1)
typedef enum
{
    TXDPK_INIT,
    RXIQK_INIT,
    POLAR_TX_RX_IIP2_K,
    POLAR_TX_PHY_INIT_STATE_MAX
} POLAR_TX_PHY_INIT_STATE;

typedef void(*PHY_INIT_SCRIPT_RSVD_TYPE_HDLR)(uint8_t, uint16_t, uint16_t);
#endif /* SUPPORT_POLAR_TX */

#if (SUPPORT_IQM_MODE == 1)
typedef enum
{
    IQM_LCK_INIT,
    IQK_LOK_INIT,
    IQM_PHY_INIT_STATE_MAX
} IQM_PHY_INIT_STATE;
#endif /* SUPPORT_IQM_MODE */

#if (SUPPORT_TPM_MODE == 1)
typedef enum
{
    MODEM_TPM_INIT,
    RFC_TPM_INIT,
    TPM_LCK_INIT,
    KVCO2K_INIT,
    TPMK_INIT,
    TPM_RX_IIP2_K,
    TPM_PHY_INIT_STATE_MAX
} TPM_PHY_INIT_STATE;
#endif /* SUPPORT_TPM_MODE */

typedef enum
{
    CH0_21,
    CH22_43,
    CH44_61,
    CH62_78,
    TOTAL_CH_GROUP
} CHANNEL_GROUP_TYPE;

typedef enum
{
    MODEM_FLATK_0P50_RES_NEG_2P5 = 0,
    MODEM_FLATK_0P50_RES_NEG_2P0 = 1,
    MODEM_FLATK_0P50_RES_NEG_1P5 = 2,
    MODEM_FLATK_0P50_RES_NEG_1P0 = 3,
    MODEM_FLATK_0P50_RES_NEG_0P5 = 4,
    MODEM_FLATK_0P50_RES_0 = 5,
    MODEM_FLATK_0P50_RES_POS_0P5 = 6,
    MODEM_FLATK_0P50_RES_POS_1P0 = 7,
} MODEM_FLATK_0P50;

typedef enum
{
    MODEM_FLATK_0P25_RES_0 = 0,
    MODEM_FLATK_0P25_RES_NEG_0P25 = 1,
} MODEM_FLATK_0P25;

typedef struct
{
    const uint16_t *script;
    uint16_t size;
} PHY_SCRIPT_TYPE;

typedef struct
{
    PHY_SCRIPT_TYPE modem_init_script;
    PHY_SCRIPT_TYPE modem_init_fw_ret_script_btmac;
    PHY_SCRIPT_TYPE modem_init_fw_ret_script_platform;
    PHY_SCRIPT_TYPE rfc_init_script;
    PHY_SCRIPT_TYPE rfc_init_fw_ret_script_btmac;
    PHY_SCRIPT_TYPE rfc_init_fw_ret_script_platform;
} PHY_INIT_SCRIPT_TYPE;

typedef union
{
    int8_t value;
    struct
    {
        uint8_t fw_part: 2;                       //[1:0] compensation by FW (TBD)
        uint8_t modem_part: 2;                    //[3:2] compensation in modem
        int8_t rfc_part: 4;                       //[7:4] 1dB compensation in rfc
    };
    struct
    {
        uint8_t fw_0p0625_comp: 1;                //[0] 0.0625dB compensation by FW (TBD)
        uint8_t fw_0p125_comp: 1;                 //[1] 0.125dB compensation by FW (TBD)
        uint8_t modem_neg_0p25_comp: 1;           //[2] -0.25dB compensation in modem
        uint8_t modem_neg_0p50_comp: 1;           //[3] -0.5dB compensation in modem
        int8_t rfc_1dB: 4;                        //[7:4] 1dB compensation in rfc
    };
    struct
    {
        int8_t rsvd_0p50_dB: 3;                   //[2:0] don't care
        int8_t res_0p50dB_part: 5;                //[7:3] flatk, unit 0.5dB
    };
    struct
    {
        int8_t rsvd_0p25_dB: 2;                   //[1:0] don't care
        int8_t res_0p25dB_part: 6;                //[7:2] flatk, unit 0.25dB
    };
} TXGAIN_FLATK_RAW; //resolution is 0.0625dB

/* ============================================== */
/* ========= Tx Power Track Definition ========== */
/* ============================================== */
typedef enum
{
    BEFORE_TRACK = 0,
    AFTER_TRACK = 1
} TX_PWR_TRACK_PHASE;

/* ============================================== */
/* ========== RFC Register Definition =========== */
/* ============================================== */
typedef union
{
    uint16_t u16;
    struct
    {
        uint16_t tc_tmeter_latch        : 6; //[5:0]
        uint16_t pow_tmeter_bt          : 1; //[6]
        uint16_t vld_tmeter             : 1; //[7]
        uint16_t tc_tmeter_latch_2      : 2; //[9:8]
        uint16_t rsvd                   : 4; //[13:10]
        uint16_t en_bt_tmeter_bbtrigger : 1; //[14]
        uint16_t rsvd1                  : 1; //[15]
    };
} RFC_P0_REG04;

/* ============================================== */
/* ========= Modem Register Definition ========== */
/* ============================================== */
typedef struct
{
    uint16_t reg_sram_debug_en: 1;
    uint16_t reg_sram_soft_rst: 1;
    uint16_t reg_spram_ds_en: 1;
    uint16_t reg_AGC_sat_num_le2m: 1;
    uint16_t r_AGC_pw_mon_th: 2;
    uint16_t reg_AGC_sat_num_bt4: 1;
    uint16_t reg_sram_debug_mode: 4;
    uint16_t r_AGC_sat_num: 1;
    uint16_t reg_sram_pktg_size: 3;
    uint16_t reg_sram_debug_filter_en: 1;
} MODEM_P0_REG08;

typedef struct
{
    uint16_t reg_settling_time_bt4_2:               6;
    uint16_t reg_settling_time_bt4_3:               6;
    uint16_t reg_psd_sram_report_addr_base_msb:     1;
    uint16_t reg_rf_ac_corner_bt4_sync:             3;
} MODEM_P2_REG50;

typedef struct
{
    uint16_t reg_sram_start_addr_lsb:               13;
    uint16_t reg_sram_mode0_sel:                    2;
    uint16_t reg_sram_phase_rate:                   1;
} MODEM_P2_REG52;

typedef struct
{
    uint16_t reg_sram_end_addr_lsb:                 13;
    uint16_t reg_sram_debug_pattern:                1;
    uint16_t reg_sram_adc_rate:                     1;
    uint16_t reg_sram_adc_msb:                      1;
} MODEM_P2_REG54;

typedef struct
{
    uint16_t reg_psd_sram_report_addr_base_lsb:     13;
    uint16_t reg_psd_agc_time:                      3;
} MODEM_P2_REG56;

typedef struct
{
    uint16_t reg_psd_cal_dly:                       3;
    uint16_t reg_psd_cal_opt:                       2;
    uint16_t reg_psd_iq2pwr_opt:                    1;
    uint16_t reg_psd_iq2pwr2_opt:                   1;
    uint16_t reg_psd_avg_opt:                       2;
    uint16_t reg_psd_avg2_opt:                      3;
    uint16_t reg_psd_avg_scale:                     3;
    uint16_t reg_psd_fix_gain:                      1;
} MODEM_P2_REG58;

typedef struct
{
    uint16_t reg_btm_if_freq_psd;
} MODEM_P2_REG5A;

typedef struct
{
    uint16_t reg_psd_deltaf:                        8;
    uint16_t reg_if_bw_psd:                         3;
    uint16_t reg_psd_en_bpf_psd:                    1;
    uint16_t reg_btm_dfir_bw_psd:                   3;
    uint16_t reg_psd_share_lna_en:                  1;
} MODEM_P2_REG5C;

typedef struct
{
    uint16_t reg_sram_start_addr_msb:               1;
    uint16_t reg_sram_end_addr_msb:                 1;
    uint16_t reg_sram_sym_cnt_th:                   4;
    uint16_t rsvd:                                  10;
} MODEM_P2_REG5E;

typedef struct
{
    uint16_t reg_psd_scale_margin:                  3;
    uint16_t reg_psd_scale_en:                      1;
    uint16_t reg_pre_dfir_scale_margin:             3;
    uint16_t reg_pre_dfir_scale_en:                 1;
    uint16_t reg_psd_ini_gain_mp:                   6;
    uint16_t reg_psd_rpt_db:                        1;
    uint16_t rsvd:                                  1;
} MODEM_P3_REG06;

typedef struct
{
    uint16_t reg_lbt_deg_sel:                       3;
    uint16_t reg_adc_fifo_force_on:                 1;
    uint16_t reg_dac_fifo_force_on:                 1;
    uint16_t reg_rfc_ind_en:                        1;
    uint16_t reg_rf_ac_corner_bt2_sync:             3;
    uint16_t reg_rfc_ind_gnt_opt:                   1;
    uint16_t reg_set_tx_gain_dly:                   3;
    uint16_t reg_DPSK_ini_phase:                    3;
} MODEM_P6_REG18;

typedef struct
{
    uint16_t reg_rfc_ind_addr:                      8;
    uint16_t reg_rfc_ind_web:                       1;
    uint16_t rsvd:                                  7;
} MODEM_P6_REG4E;

typedef struct
{
    uint16_t rfc_ind_wdata;
} MODEM_P6_REG50;

typedef struct
{
    uint16_t rfc_ind_req:                           1;
    uint16_t rfc_ind_wr_done:                       1;
    uint16_t rfc_ind_rd_done:                       1;
    uint16_t rfc_access_conflict:                   1;
    uint16_t rsvd:                                  12;
} MODEM_P6_REG7A;

typedef union
{
    uint16_t d16;
    MODEM_P0_REG08 p0reg08;
    MODEM_P2_REG50 p2reg50;
    MODEM_P2_REG52 p2reg52;
    MODEM_P2_REG54 p2reg54;
    MODEM_P2_REG56 p2reg56;
    MODEM_P2_REG58 p2reg58;
    MODEM_P2_REG5C p2reg5c;
    MODEM_P2_REG5E p2reg5e;
    MODEM_P3_REG06 p3reg06;
    MODEM_P6_REG18 p6reg18;
    MODEM_P6_REG4E p6reg4e;
    MODEM_P6_REG50 p6reg50;
    MODEM_P6_REG7A p6reg7a;
} MODEM_REG_S_TYPE;

/* ============================================== */
/* ========== Modem/RFC PI Definition =========== */
/* ============================================== */
typedef enum
{
    PI_RFC_ARBITER_MODEM,
    PI_RFC_ARBITER_CPU
} PI_RFC_ARBITER_TYPE;

typedef enum
{
    PI_READ,
    PI_WRITE,
} PI_ACCESS_TYPE;

typedef enum
{
    RFC_IND_WRITE,
    RFC_IND_READ,
} RFC_INDIRECT_ACCESS_TYPE;

typedef union
{
    uint32_t u32;
    struct
    {
        uint32_t bt_ioq_rfc:                        16;
        uint32_t bt_ioad_rfc:                       8;
        uint32_t rf_iow:                            1;
        uint32_t rfc_pi_sel:                        1;
        uint32_t rsvd:                              6;
    };
} RFC_PI_VENDOR_REG;

typedef union
{
    uint32_t u32;
    struct
    {
        uint32_t pi_wrdata:                         16;
        uint32_t pi_modem_addr:                     6;
        uint32_t pi_modem_page:                     6;
        uint32_t pi_wr:                             1;
        uint32_t bt_phy_pi_en:                      1;
        uint32_t pi_rising_edge_time:               2;
    };
} MODEM_PI_VENDOR_REG;

/*============================================================================*
 *                              Variables
 *============================================================================*/
extern PHY_INIT_SCRIPT_TYPE phy_init_script;

extern void (*phy_init_script_execute)(const uint16_t *, uint16_t);
extern void (*phy_auto_gated_on)(void);
extern void (*phy_auto_gated_off)(void);
extern void (*phy_reset)(void);
extern void (*phy_param_init)(void);
extern void (*phy_reg_backup)(void);
extern void (*phy_enable_expa_lna_setting)(void);
extern void (*phy_dlps_restore_btmac)(void);
extern void (*phy_dlps_restore_platform)(void);

extern void (*(general_phy_init_handle[]))(void);
#if (SUPPORT_POLAR_TX == 1)
extern PHY_INIT_SCRIPT_RSVD_TYPE_HDLR phy_script_rsvd_type_handler[3];
extern void (*(polar_tx_phy_init_handle[]))(void);
#endif
#if (SUPPORT_IQM_MODE == 1)
extern void (*(iqm_phy_init_handle[]))(void);
#endif
#if (SUPPORT_TPM_MODE == 1)
extern void (*(tpm_phy_init_handle[]))(void);
#endif

extern void (*get_valid_txgain_flatk)(int8_t *txgain_flatk);
extern void (*txgain_flatk_post_processing)(int8_t *, uint8_t);
extern void (*modem_rf_set_txgain_flatk)(int8_t *);
extern void (*set_tx_tankk)(void);

extern void (*lc_init_radio_phy_func)(uint8_t);

/* ============================================== */
/* ========== Thermal Meter Definition ========== */
/* ============================================== */
extern int32_t (*get_thermal_meter_delta)(uint8_t);
extern void (*thermal_meter_trigger)(void);
extern void (*thermal_meter_read)(void);
extern void (*thermal_meter_update)(void);
extern void (*thermal_timer_handler)(TimerHandle_t);
extern void (*thermal_timer_init)(void);
extern void (*thermal_tracking)(void);

/* ============================================== */
/* ========= Tx Power Track Definition ========== */
/* ============================================== */
extern void (*tx_power_track_param_init)(void);
extern void (*execute_post_processing)(uint8_t *, TXGAIN_TYPE, RF_MODE, bool);
extern void (*calculate_txgain_index)(RF_MODE);

extern int8_t (*get_txgaink_value)(RF_MODE);

extern void (*modem_psd_scan_set_3ch_mode)(void);
extern void (*modem_psd_set_agc)(uint8_t, uint8_t, uint8_t, uint8_t);
extern void (*modem_psd_init)(uint8_t);
extern void (*modem_psd_retention)(void);

extern void (*modem_mse_init)(void);

extern bool (*rfc_indirect_access)(uint8_t, uint16_t *, RFC_INDIRECT_ACCESS_TYPE);
extern void (*modem_pi_access)(uint8_t, uint8_t, uint16_t *, PI_ACCESS_TYPE);

/*============================================================================*
 *                              Functions
 *============================================================================*/

void modem_set_lbt_mode(LBT_MODE lbt_mode);
void modem_set_rxadck_value(uint16_t dc_comp);
void rf_set_flatk(int8_t *txgain_flatk);
void modem_set_flatk(int8_t *txgain_flatk);
uint32_t get_rxgaink_value(uint8_t *rxgaink_result);

#if (SUPPORT_IQM_MODE == 1)
void set_iqm_tx_tankk(uint8_t *tankk);

void iqm_set_rxgaink_to_modem(uint32_t raw_rxgaink);

uint16_t get_specified_bit_mask(uint8_t lsb, uint8_t msb);
void lck_efuse2rfc(uint8_t *efuse_lck, uint8_t ch_grp_num, uint8_t lck_addr_start);
void read_lok_txiqk_auto(uint16_t *lok_result, uint16_t *iqk_x_reault, uint16_t *iqk_y_result);
void set_lok_txiqk_manual(uint16_t lok_result, uint16_t txiqk_result_x, uint16_t txiqk_result_y);
#endif

#if (SUPPORT_TPM_MODE == 1)
void tpm_set_rxgaink_to_modem(uint32_t raw_rxgaink);
#endif

void pf_sram_modem_access_enable(void);
void pf_sram_modem_access_disable(void);

void phy_error_function(void);

void dump_tx_power_track_log(TX_PWR_TRACK_PHASE phase, RF_MODE rf_mode);

#ifdef __cplusplus
}
#endif

#endif /*_LC_PHY_INIT_H_*/
