/* Auto gen based on D:\leo825x\Desktop\aon_register\RL6747_AON_auto_gen_table_r86.xlsx */

#ifndef _RTL876X_AON_REG_H
#define _RTL876X_AON_REG_H

#include <stdint.h>

#define AON_RG0X        0x0
#define AON_RG1X        0x4
#define AON_RG2X        0x8
#define AON_RG3X        0xC
#define AON_RG4X        0x10
#define AON_RG5X        0x14
#define AON_RG6X        0x18
#define AON_RG7X        0x1C
#define AON_RG8X        0x20
#define AON_RG9X        0x24
#define AON_RG10X       0x28
#define AON_RG11X       0x2C
#define AON_RG12X       0x30

/* 0x0      0x4000_0000
    0       R/W AON_REG0X_DUMMY1                    1'h0        BTAON_reg_wr
    1       R/W AON_XTAL_GATED_AFEN                 1'b0        BTAON_reg_wr
    2       R/W AON_XTAL_GATED_AFEN_LP              1'b0        BTAON_reg_wr
    3       R/W AON_XTAL_GATED_AFEP                 1'b0        BTAON_reg_wr
    4       R/W AON_XTAL_GATED_AFEP_LP              1'b0        BTAON_reg_wr
    7:5     R/W AON_MBIAS_TUNE_BIAS_50NA_IQ         3'b000      BTAON_reg_wr
    10:8    R/W AON_MBIAS_TUNE_BIAS_500NA_IQ        3'b000      BTAON_reg_wr
    13:11   R/W AON_MBIAS_TUNE_BIAS_3NA_IQ          3'b000      BTAON_reg_wr
    16:14   R/W AON_MBIAS_TUNE_BIAS_10UA_IQ         3'b000      BTAON_reg_wr
    17      R/W AON_MBIAS_POW_VD105_RF_DET          1'b1        BTAON_reg_wr
    18      R/W AON_MBIAS_POW_VAUX3_DET             1'b1        BTAON_reg_wr
    19      R/W AON_MBIAS_POW_VAUX2_DET             1'b1        BTAON_reg_wr
    20      R/W AON_MBIAS_POW_VAUX1_DET             1'b1        BTAON_reg_wr
    21      R/W AON_MBIAS_POW_VAUDIO_DET            1'b1        BTAON_reg_wr
    22      R/W AON_XTAL_EN_LPS_CLK                 1'b1        BTAON_reg_wr
    23      R/W AON_MBIAS_POW_HV33_HQ_DET           1'b1        BTAON_reg_wr
    24      R/W AON_BT_AON2_RSTB                    1'b0        BTAON_reg_wr
    25      R/W AON_MBIAS_POW_HV_DET                1'b1        BTAON_reg_wr
    26      R/W AON_ISO_BT_AON2                     1'b1        BTAON_reg_wr
    27      R/W AON_MBIAS_FSM_DUMMY1                1'b0        BTAON_reg_wr
    28      R/W AON_MBIAS_POW_BIAS_500NA            1'b0        BTAON_reg_wr
    29      R/W AON_MBIAS_POW_BIAS_10UA             1'b0        BTAON_reg_wr
    30      R/W AON_CHG_EN_HV_PD                    1'b1        BTAON_reg_wr
    31      R/W AON_MBIAS_POW_PCUT_DVDD_TO_DVDD1    1'b1        BTAON_reg_wr
 */
typedef union _AON_RG0X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AON_REG0X_DUMMY1: 1;
        uint32_t AON_XTAL_GATED_AFEN: 1;
        uint32_t AON_XTAL_GATED_AFEN_LP: 1;
        uint32_t AON_XTAL_GATED_AFEP: 1;
        uint32_t AON_XTAL_GATED_AFEP_LP: 1;
        uint32_t AON_MBIAS_TUNE_BIAS_50NA_IQ: 3;
        uint32_t AON_MBIAS_TUNE_BIAS_500NA_IQ: 3;
        uint32_t AON_MBIAS_TUNE_BIAS_3NA_IQ: 3;
        uint32_t AON_MBIAS_TUNE_BIAS_10UA_IQ: 3;
        uint32_t AON_MBIAS_POW_VD105_RF_DET: 1;
        uint32_t AON_MBIAS_POW_VAUX3_DET: 1;
        uint32_t AON_MBIAS_POW_VAUX2_DET: 1;
        uint32_t AON_MBIAS_POW_VAUX1_DET: 1;
        uint32_t AON_MBIAS_POW_VAUDIO_DET: 1;
        uint32_t AON_XTAL_EN_LPS_CLK: 1;
        uint32_t AON_MBIAS_POW_HV33_HQ_DET: 1;
        uint32_t AON_BT_AON2_RSTB: 1;
        uint32_t AON_MBIAS_POW_HV_DET: 1;
        uint32_t AON_ISO_BT_AON2: 1;
        uint32_t AON_MBIAS_FSM_DUMMY1: 1;
        uint32_t AON_MBIAS_POW_BIAS_500NA: 1;
        uint32_t AON_MBIAS_POW_BIAS_10UA: 1;
        uint32_t AON_CHG_EN_HV_PD: 1;
        uint32_t AON_MBIAS_POW_PCUT_DVDD_TO_DVDD1: 1;
    };
} AON_RG0X_TYPE;

/* 0x4      0x4000_0004
    5:0     R/W AON_REG1X_DUMMY1                    6'h0        BTAON_reg_wr
    13:6    R/W AON_LDOAUX2_TUNE_LDO_VOUT           8'b00100101 BTAON_reg_wr
    21:14   R/W AON_LDOAUX1_TUNE_LDO_VOUT           8'b00100101 BTAON_reg_wr
    22      R/W AON_POW32K_32KXTAL33                1'b1        BTAON_reg_wr
    23      R/W AON_POW32K_32KOSC33                 1'b1        BTAON_reg_wr
    25:24   R/W AON_MBIAS_TUNE_LPBG_IQ              2'b00       BTAON_reg_wr
    31:26   R/W AON_MBIAS_TUNE_LDO309_VOUT          6'b100001   BTAON_reg_wr
 */
typedef union _AON_RG1X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AON_REG1X_DUMMY1: 6;
        uint32_t AON_LDOAUX2_TUNE_LDO_VOUT: 8;
        uint32_t AON_LDOAUX1_TUNE_LDO_VOUT: 8;
        uint32_t AON_POW32K_32KXTAL33: 1;
        uint32_t AON_POW32K_32KOSC33: 1;
        uint32_t AON_MBIAS_TUNE_LPBG_IQ: 2;
        uint32_t AON_MBIAS_TUNE_LDO309_VOUT: 6;
    };
} AON_RG1X_TYPE;

/* 0x8      0x4000_0008
    0       R/W AON_LDOAUX1_EN_VO_PD                1'b1        BTAON_reg_wr
    2:1     R/W AON_LDOUSB_SEL_BIAS                 2'b00       BTAON_reg_wr
    3       R/W AON_LDOUSB_POW_BIAS                 1'b0        BTAON_reg_wr
    4       R/W AON_LDOUSB_EN_LO_IQ2                1'b0        BTAON_reg_wr
    5       R/W AON_LDOUSB_EN_HI_IQ                 1'b0        BTAON_reg_wr
    6       R/W AON_LDOSYS_EN_VREF_LDO533           1'b0        BTAON_reg_wr
    7       R/W AON_LDOSYS_EN_VO_PD                 1'b1        BTAON_reg_wr
    8       R/W AON_LDOSYS_POS_EXT_RST_B            1'b0        BTAON_reg_wr
    9       R/W AON_LDOAUX3_EN_VREF                 1'b0        BTAON_reg_wr
    10      R/W AON_LDOAUX3_EN_VO_PD                1'b1        BTAON_reg_wr
    11      R/W AON_LDOAUX3_POS_EXT_RST_B           1'b0        BTAON_reg_wr
    13:12   R/W AON_LDOAUX2_SEL_BIAS                2'b00       BTAON_reg_wr
    14      R/W AON_LDOAUX2_POW_BIAS                1'b0        BTAON_reg_wr
    15      R/W AON_LDOAUX2_EN_LO_IQ2               1'b0        BTAON_reg_wr
    16      R/W AON_LDOAUX2_EN_HI_IQ                1'b0        BTAON_reg_wr
    18:17   R/W AON_LDOAUX1_SEL_BIAS                2'b00       BTAON_reg_wr
    19      R/W AON_LDOAUX1_POW_BIAS                1'b0        BTAON_reg_wr
    20      R/W AON_LDOAUX1_EN_LO_IQ2               1'b0        BTAON_reg_wr
    21      R/W AON_LDOAUX1_EN_HI_IQ                1'b0        BTAON_reg_wr
    22      R/W AON_CHG_POW_M2                      1'b0        BTAON_reg_wr
    23      R/W AON_CHG_POW_M1                      1'b0        BTAON_reg_wr
    31:24   R/W AON_LDOUSB_TUNE_LDO_VOUT            8'b00100101 BTAON_reg_wr
 */
typedef union _AON_RG2X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AON_LDOAUX1_EN_VO_PD: 1;
        uint32_t AON_LDOUSB_SEL_BIAS: 2;
        uint32_t AON_LDOUSB_POW_BIAS: 1;
        uint32_t AON_LDOUSB_EN_LO_IQ2: 1;
        uint32_t AON_LDOUSB_EN_HI_IQ: 1;
        uint32_t AON_LDOSYS_EN_VREF_LDO533: 1;
        uint32_t AON_LDOSYS_EN_VO_PD: 1;
        uint32_t AON_LDOSYS_POS_EXT_RST_B: 1;
        uint32_t AON_LDOAUX3_EN_VREF: 1;
        uint32_t AON_LDOAUX3_EN_VO_PD: 1;
        uint32_t AON_LDOAUX3_POS_EXT_RST_B: 1;
        uint32_t AON_LDOAUX2_SEL_BIAS: 2;
        uint32_t AON_LDOAUX2_POW_BIAS: 1;
        uint32_t AON_LDOAUX2_EN_LO_IQ2: 1;
        uint32_t AON_LDOAUX2_EN_HI_IQ: 1;
        uint32_t AON_LDOAUX1_SEL_BIAS: 2;
        uint32_t AON_LDOAUX1_POW_BIAS: 1;
        uint32_t AON_LDOAUX1_EN_LO_IQ2: 1;
        uint32_t AON_LDOAUX1_EN_HI_IQ: 1;
        uint32_t AON_CHG_POW_M2: 1;
        uint32_t AON_CHG_POW_M1: 1;
        uint32_t AON_LDOUSB_TUNE_LDO_VOUT: 8;
    };
} AON_RG2X_TYPE;

/* 0xC      0x4000_000c
    7:0     R/W AON_LDOSYS_TUNE_LDO533_VOUT         8'b10111100 BTAON_reg_wr
    8       R/W AON_LDOSYS_POS_EN_POS               1'b0        BTAON_reg_wr
    15:9    R/W AON_LDOAUX3_LQ_TUNE_LDO_VOUT        7'b0011010  BTAON_reg_wr
    23:16   R/W AON_LDOAUX3_TUNE_LDOHQ1_VOUT        8'b00100101 BTAON_reg_wr
    24      R/W AON_LDOAUX3_POS_EN_POS              1'b0        BTAON_reg_wr
    25      R/W AON_LDOAUX2_POW_LDO                 1'b0        BTAON_reg_wr
    26      R/W AON_LDOAUX1_POW_LDO                 1'b0        BTAON_reg_wr
    27      R/W AON_CHG_EN_M2FON1K                  1'b0        BTAON_reg_wr
    28      R/W AON_LDOSYS_POW_LDO533               1'b0        BTAON_reg_wr
    29      R/W AON_LDOAUX3_POW_LDOHQ1              1'b0        BTAON_reg_wr
    30      R/W AON_LDOUSB_EN_VO_PD                 1'b1        BTAON_reg_wr
    31      R/W AON_LDOAUX2_EN_VO_PD                1'b1        BTAON_reg_wr
 */
typedef union _AON_RG3X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AON_LDOSYS_TUNE_LDO533_VOUT: 8;
        uint32_t AON_LDOSYS_POS_EN_POS: 1;
        uint32_t AON_LDOAUX3_LQ_TUNE_LDO_VOUT: 7;
        uint32_t AON_LDOAUX3_TUNE_LDOHQ1_VOUT: 8;
        uint32_t AON_LDOAUX3_POS_EN_POS: 1;
        uint32_t AON_LDOAUX2_POW_LDO: 1;
        uint32_t AON_LDOAUX1_POW_LDO: 1;
        uint32_t AON_CHG_EN_M2FON1K: 1;
        uint32_t AON_LDOSYS_POW_LDO533: 1;
        uint32_t AON_LDOAUX3_POW_LDOHQ1: 1;
        uint32_t AON_LDOUSB_EN_VO_PD: 1;
        uint32_t AON_LDOAUX2_EN_VO_PD: 1;
    };
} AON_RG3X_TYPE;

/* 0x10     0x4000_0010
    7:0     R/W AON_REG4X_DUMMY1                    8'h0        BTAON_reg_wr
    14:8    R/W AON_LDO_PA_TUNE_LDO_VOUT            7'b0110010  BTAON_reg_wr
    21:15   R/W AON_CODEC_LDO_TUNE_LDO_DRV_VOUT     7'b0110010  BTAON_reg_wr
    22      R/W AON_EXTRN_LDO2_POW_LDO              1'b0        BTAON_reg_wr
    23      R/W AON_EXTRN_LDO1_POW_LDO              1'b0        BTAON_reg_wr
    24      R/W AON_LDOUSB_POW_LDO                  1'b0        BTAON_reg_wr
    31:25   R/W AON_LDOSYS_TUNE_LDO733_VOUT         7'b1010110  BTAON_reg_wr
 */
typedef union _AON_RG4X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AON_REG4X_DUMMY1: 8;
        uint32_t AON_LDO_PA_TUNE_LDO_VOUT: 7;
        uint32_t AON_CODEC_LDO_TUNE_LDO_DRV_VOUT: 7;
        uint32_t AON_EXTRN_LDO2_POW_LDO: 1;
        uint32_t AON_EXTRN_LDO1_POW_LDO: 1;
        uint32_t AON_LDOUSB_POW_LDO: 1;
        uint32_t AON_LDOSYS_TUNE_LDO733_VOUT: 7;
    };
} AON_RG4X_TYPE;

/* 0x14     0x4000_0014
    0       R/W AON_SWR_CORE_FPWM1                  1'b0        BTAON_reg_wr
    1       R/W AON_SWR_CORE_EN_SAW_RAMP2           1'b0        BTAON_reg_wr
    2       R/W AON_SWR_CORE_EN_SAW_RAMP1           1'b0        BTAON_reg_wr
    3       R/W AON_SWR_CORE_EN_OCP                 1'b0        BTAON_reg_wr
    4       R/W AON_SWR_AUDIO_POW_ZCD               1'b0        BTAON_reg_wr
    5       R/W AON_SWR_AUDIO_SEL_CK_CTRL           1'b0        BTAON_reg_wr
    6       R/W AON_SWR_AUDIO_POW_VDIV              1'b0        BTAON_reg_wr
    7       R/W AON_SWR_AUDIO_SEL_POS_VREFLPPFM     1'b0        BTAON_reg_wr
    8       R/W AON_SWR_AUDIO_POW_DIVPFM            1'b0        BTAON_reg_wr
    9       R/W AON_SWR_AUDIO_POW_POS_REF           1'b0        BTAON_reg_wr
    10      R/W AON_SWR_AUDIO_POW_PFM               1'b0        BTAON_reg_wr
    11      R/W AON_SWR_AUDIO_POW_IMIR              1'b0        BTAON_reg_wr
    12      R/W AON_SWR_AUDIO_SEL_CK_CTRL_NEW       1'b0        BTAON_reg_wr
    13      R/W AON_SWR_AUDIO_SEL_PFM_COT           1'b0        BTAON_reg_wr
    14      R/W AON_SWR_AUDIO_EN_OCP                1'b0        BTAON_reg_wr
    15      R/W AON_LDOSYS_EN_733TO533_PC           1'b0        BTAON_reg_wr
    16      R/W AON_SWR_AUDIO_ZCDQ_RST_B            1'b0        BTAON_reg_wr
    17      R/W AON_SWR_CORE_ZCDQ_RST_B             1'b0        BTAON_reg_wr
    18      R/W AON_LDOAUX3_LQ_POW_LDO              1'b0        BTAON_reg_wr
    19      R/W AON_LDOEXT_EN_VREF                  1'b0        BTAON_reg_wr
    20      R/W AON_LDOEXT_EN_VO_PD                 1'b1        BTAON_reg_wr
    21      R/W AON_LDOEXT_POS_EXT_RST_B            1'b0        BTAON_reg_wr
    22      R/W AON_LDO_DIG_EN_VO_PD                1'b1        BTAON_reg_wr
    23      R/W AON_CODEC_LDO_EN_LDO_DRV_VO_PD      1'b1        BTAON_reg_wr
    24      R/W AON_LDO_PA_EN_VO_PD                 1'b1        BTAON_reg_wr
    31:25   R/W AON_LDO_DIG_TUNE_LDO_VOUT           7'b0110010  BTAON_reg_wr
 */
typedef union _AON_RG5X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AON_SWR_CORE_FPWM1: 1;
        uint32_t AON_SWR_CORE_EN_SAW_RAMP2: 1;
        uint32_t AON_SWR_CORE_EN_SAW_RAMP1: 1;
        uint32_t AON_SWR_CORE_EN_OCP: 1;
        uint32_t AON_SWR_AUDIO_POW_ZCD: 1;
        uint32_t AON_SWR_AUDIO_SEL_CK_CTRL: 1;
        uint32_t AON_SWR_AUDIO_POW_VDIV: 1;
        uint32_t AON_SWR_AUDIO_SEL_POS_VREFLPPFM: 1;
        uint32_t AON_SWR_AUDIO_POW_DIVPFM: 1;
        uint32_t AON_SWR_AUDIO_POW_POS_REF: 1;
        uint32_t AON_SWR_AUDIO_POW_PFM: 1;
        uint32_t AON_SWR_AUDIO_POW_IMIR: 1;
        uint32_t AON_SWR_AUDIO_SEL_CK_CTRL_NEW: 1;
        uint32_t AON_SWR_AUDIO_SEL_PFM_COT: 1;
        uint32_t AON_SWR_AUDIO_EN_OCP: 1;
        uint32_t AON_LDOSYS_EN_733TO533_PC: 1;
        uint32_t AON_SWR_AUDIO_ZCDQ_RST_B: 1;
        uint32_t AON_SWR_CORE_ZCDQ_RST_B: 1;
        uint32_t AON_LDOAUX3_LQ_POW_LDO: 1;
        uint32_t AON_LDOEXT_EN_VREF: 1;
        uint32_t AON_LDOEXT_EN_VO_PD: 1;
        uint32_t AON_LDOEXT_POS_EXT_RST_B: 1;
        uint32_t AON_LDO_DIG_EN_VO_PD: 1;
        uint32_t AON_CODEC_LDO_EN_LDO_DRV_VO_PD: 1;
        uint32_t AON_LDO_PA_EN_VO_PD: 1;
        uint32_t AON_LDO_DIG_TUNE_LDO_VOUT: 7;
    };
} AON_RG5X_TYPE;

/* 0x18     0x4000_0018
    0       R/W AON_REG6X_DUMMY1                    1'h0        BTAON_reg_wr
    1       R/W AON_SWR_AUDIO_PFMCCMPOS_EXT_RST_B   1'b0        BTAON_reg_wr
    2       R/W AON_SWR_AUDIO_EN_ZCD_LOW_IQ         1'b0        BTAON_reg_wr
    3       R/W AON_SWR_AUDIO_ZCDQ_FORCE_CODE1      1'b0        BTAON_reg_wr
    4       R/W AON_SWR_CORE_ZCDQ_FORCE_CODE2       1'b0        BTAON_reg_wr
    5       R/W AON_SWR_CORE_ZCDQ_FORCE_CODE1       1'b0        BTAON_reg_wr
    6       R/W AON_LDOEXT_POS_EN_POS               1'b0        BTAON_reg_wr
    7       R/W AON_LDOEXT_EN_PC                    1'b0        BTAON_reg_wr
    8       R/W AON_LDOEXT_POW_LDOHQ1               1'b0        BTAON_reg_wr
    9       R/W AON_SWR_CORE_PFM1POS_EXT_RST_B      1'b0        BTAON_reg_wr
    10      R/W AON_SWR_CORE_EN_VDDCORE_PD          1'b1        BTAON_reg_wr
    11      R/W AON_SWR_CORE_EN_VD105_RF_PD         1'b1        BTAON_reg_wr
    12      R/W AON_SWR_AUDIO_EN_VO_PD              1'b1        BTAON_reg_wr
    13      R/W AON_SWR_CORE_PWM1POS_EXT_RST_B      1'b0        BTAON_reg_wr
    14      R/W AON_SWR_AUDIO_PFMPOS_EXT_RST_B      1'b0        BTAON_reg_wr
    15      R/W AON_LDO_DIG_POW_VREF                1'b0        BTAON_reg_wr
    16      R/W AON_LDO_PA_POW_VREF                 1'b0        BTAON_reg_wr
    17      R/W AON_CODEC_LDO_POW_LDO_DRV_VREF      1'b0        BTAON_reg_wr
    18      R/W AON_SWR_CORE_EN_PFM_TDM             1'b0        BTAON_reg_wr
    19      R/W AON_SWR_CORE_POW_ZCD                1'b0        BTAON_reg_wr
    20      R/W AON_SWR_CORE_POW_ZCD_COMP_LOWIQ     1'b0        BTAON_reg_wr
    21      R/W AON_SWR_CORE_POW_VDIV2              1'b1        BTAON_reg_wr
    22      R/W AON_SWR_CORE_POW_VDIV1              1'b1        BTAON_reg_wr
    23      R/W AON_SWR_CORE_POW_SAW_IB             1'b0        BTAON_reg_wr
    24      R/W AON_SWR_CORE_POW_SAW                1'b0        BTAON_reg_wr
    25      R/W AON_SWR_CORE_POW_REF                1'b0        BTAON_reg_wr
    26      R/W AON_SWR_CORE_POW_PWM2               1'b0        BTAON_reg_wr
    27      R/W AON_SWR_CORE_POW_PWM1               1'b0        BTAON_reg_wr
    28      R/W AON_SWR_CORE_POW_PFM2               1'b0        BTAON_reg_wr
    29      R/W AON_SWR_CORE_POW_PFM1               1'b0        BTAON_reg_wr
    30      R/W AON_SWR_CORE_POW_IMIR               1'b0        BTAON_reg_wr
    31      R/W AON_SWR_CORE_FPWM2                  1'b0        BTAON_reg_wr
 */
typedef union _AON_RG6X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AON_REG6X_DUMMY1: 1;
        uint32_t AON_SWR_AUDIO_PFMCCMPOS_EXT_RST_B: 1;
        uint32_t AON_SWR_AUDIO_EN_ZCD_LOW_IQ: 1;
        uint32_t AON_SWR_AUDIO_ZCDQ_FORCE_CODE1: 1;
        uint32_t AON_SWR_CORE_ZCDQ_FORCE_CODE2: 1;
        uint32_t AON_SWR_CORE_ZCDQ_FORCE_CODE1: 1;
        uint32_t AON_LDOEXT_POS_EN_POS: 1;
        uint32_t AON_LDOEXT_EN_PC: 1;
        uint32_t AON_LDOEXT_POW_LDOHQ1: 1;
        uint32_t AON_SWR_CORE_PFM1POS_EXT_RST_B: 1;
        uint32_t AON_SWR_CORE_EN_VDDCORE_PD: 1;
        uint32_t AON_SWR_CORE_EN_VD105_RF_PD: 1;
        uint32_t AON_SWR_AUDIO_EN_VO_PD: 1;
        uint32_t AON_SWR_CORE_PWM1POS_EXT_RST_B: 1;
        uint32_t AON_SWR_AUDIO_PFMPOS_EXT_RST_B: 1;
        uint32_t AON_LDO_DIG_POW_VREF: 1;
        uint32_t AON_LDO_PA_POW_VREF: 1;
        uint32_t AON_CODEC_LDO_POW_LDO_DRV_VREF: 1;
        uint32_t AON_SWR_CORE_EN_PFM_TDM: 1;
        uint32_t AON_SWR_CORE_POW_ZCD: 1;
        uint32_t AON_SWR_CORE_POW_ZCD_COMP_LOWIQ: 1;
        uint32_t AON_SWR_CORE_POW_VDIV2: 1;
        uint32_t AON_SWR_CORE_POW_VDIV1: 1;
        uint32_t AON_SWR_CORE_POW_SAW_IB: 1;
        uint32_t AON_SWR_CORE_POW_SAW: 1;
        uint32_t AON_SWR_CORE_POW_REF: 1;
        uint32_t AON_SWR_CORE_POW_PWM2: 1;
        uint32_t AON_SWR_CORE_POW_PWM1: 1;
        uint32_t AON_SWR_CORE_POW_PFM2: 1;
        uint32_t AON_SWR_CORE_POW_PFM1: 1;
        uint32_t AON_SWR_CORE_POW_IMIR: 1;
        uint32_t AON_SWR_CORE_FPWM2: 1;
    };
} AON_RG6X_TYPE;

/* 0x1C     0x4000_001c
    3:0     R/W AON_REG7X_DUMMY1                    4'h0        BTAON_reg_wr
    4       R/W AON_SWR_AUDIO_POW_SWR               1'b0        BTAON_reg_wr
    5       R/W AON_SWR_AUDIO_POW_CCMPFM            1'b0        BTAON_reg_wr
    6       R/W AON_SWR_AUDIO_PFMPOS_EN_POS         1'b0        BTAON_reg_wr
    11:7    R/W AON_MBIAS_TUNE_LDOAUDIO_LQ_VOUT     5'b10000    BTAON_reg_wr
    12      R/W AON_SWR_AUDIO_ZCDQ_POW_UD_DIG       1'b0        BTAON_reg_wr
    13      R/W AON_LDO_PA_POW_LDO                  1'b0        BTAON_reg_wr
    14      R/W AON_LDO_PA_EN_PC                    1'b0        BTAON_reg_wr
    15      R/W AON_CODEC_LDO_POW_LDO_DRV           1'b0        BTAON_reg_wr
    16      R/W AON_CODEC_LDO_EN_LDO_DRV_PC         1'b0        BTAON_reg_wr
    23:17   R/W AON_FSM_DUMMY1                      7'b0011010  BTAON_reg_wr
    31:24   R/W AON_LDOEXT_TUNE_LDOHQ1_VOUT         8'b01000000 BTAON_reg_wr
 */
typedef union _AON_RG7X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AON_REG7X_DUMMY1: 4;
        uint32_t AON_SWR_AUDIO_POW_SWR: 1;
        uint32_t AON_SWR_AUDIO_POW_CCMPFM: 1;
        uint32_t AON_SWR_AUDIO_PFMPOS_EN_POS: 1;
        uint32_t AON_MBIAS_TUNE_LDOAUDIO_LQ_VOUT: 5;
        uint32_t AON_SWR_AUDIO_ZCDQ_POW_UD_DIG: 1;
        uint32_t AON_LDO_PA_POW_LDO: 1;
        uint32_t AON_LDO_PA_EN_PC: 1;
        uint32_t AON_CODEC_LDO_POW_LDO_DRV: 1;
        uint32_t AON_CODEC_LDO_EN_LDO_DRV_PC: 1;
        uint32_t AON_FSM_DUMMY1: 7;
        uint32_t AON_LDOEXT_TUNE_LDOHQ1_VOUT: 8;
    };
} AON_RG7X_TYPE;

/* 0x20     0x4000_0020
    1:0     R/W AON_REG8X_DUMMY1                    2'h0        BTAON_reg_wr
    2       R/W AON_SWR_CORE_PWM1POS_EN_POS         1'b0        BTAON_reg_wr
    3       R/W AON_SWR_CORE_EN_SWR_OUT2            1'b0        BTAON_reg_wr
    4       R/W AON_SWR_CORE_EN_SWR_OUT1            1'b0        BTAON_reg_wr
    5       R/W AON_XTAL_GATED_DIGIN                1'b0        BTAON_reg_wr
    6       R/W AON_XTAL_GATED_DIGIP                1'b0        BTAON_reg_wr
    7       R/W AON_XTAL_GATED_DIGI_LP              1'b0        BTAON_reg_wr
    8       R/W AON_XTAL_GATED_LPS                  1'b0        BTAON_reg_wr
    9       R/W AON_XTAL_GATED_RFN                  1'b0        BTAON_reg_wr
    10      R/W AON_XTAL_GATED_RFP                  1'b0        BTAON_reg_wr
    11      R/W AON_LDO_DIG_POW_LDO                 1'b0        BTAON_reg_wr
    12      R/W AON_SWR_CORE_ZCDQ_POW_UD_DIG        1'b0        BTAON_reg_wr
    13      R/W AON_LDO_DIG_EN_PC                   1'b0        BTAON_reg_wr
    17:14   R/W AON_MBIAS_SEL_VR_LPPFM2             4'b1000     BTAON_reg_wr
    21:18   R/W AON_MBIAS_SEL_VR_LPPFM1             4'b1000     BTAON_reg_wr
    22      R/W AON_SWR_CORE_SEL_POS_VREFLPPFM2     1'b0        BTAON_reg_wr
    23      R/W AON_SWR_CORE_SEL_POS_VREFLPPFM1     1'b0        BTAON_reg_wr
    31:24   R/W AON_SWR_AUDIO_TUNE_VDIV             8'b10011110 BTAON_reg_wr
 */
typedef union _AON_RG8X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AON_REG8X_DUMMY1: 2;
        uint32_t AON_SWR_CORE_PWM1POS_EN_POS: 1;
        uint32_t AON_SWR_CORE_EN_SWR_OUT2: 1;
        uint32_t AON_SWR_CORE_EN_SWR_OUT1: 1;
        uint32_t AON_XTAL_GATED_DIGIN: 1;
        uint32_t AON_XTAL_GATED_DIGIP: 1;
        uint32_t AON_XTAL_GATED_DIGI_LP: 1;
        uint32_t AON_XTAL_GATED_LPS: 1;
        uint32_t AON_XTAL_GATED_RFN: 1;
        uint32_t AON_XTAL_GATED_RFP: 1;
        uint32_t AON_LDO_DIG_POW_LDO: 1;
        uint32_t AON_SWR_CORE_ZCDQ_POW_UD_DIG: 1;
        uint32_t AON_LDO_DIG_EN_PC: 1;
        uint32_t AON_MBIAS_SEL_VR_LPPFM2: 4;
        uint32_t AON_MBIAS_SEL_VR_LPPFM1: 4;
        uint32_t AON_SWR_CORE_SEL_POS_VREFLPPFM2: 1;
        uint32_t AON_SWR_CORE_SEL_POS_VREFLPPFM1: 1;
        uint32_t AON_SWR_AUDIO_TUNE_VDIV: 8;
    };
} AON_RG8X_TYPE;

/* 0x24     0x4000_0024
    4:0     R/W AON_REG9X_DUMMY1                    5'h0        BTAON_reg_wr
    5       R/W AON_MBIAS_POW_PCUT_VPON_TO_DVDD     1'b0        BTAON_reg_wr
    6       R/W AON_MBIAS_POW_PCUT_VD105_RF_TO_DVDD 1'b0        BTAON_reg_wr
    7       R/W AON_SWR_CORE_EN_SWR_SHORT           1'b1        BTAON_reg_wr
    8       R/W AON_SWR_CORE_SEL_FOLLOWCTRL1        1'b1        BTAON_reg_wr
    9       R/W AON_XTAL_EN_SUPPLY_MODE             1'b1        BTAON_reg_wr
    10      R/W AON_MBIAS_POW_LDOAUDIO_LQ           1'b0        BTAON_reg_wr
    11      R/W AON_SWR_CORE_SEL_CK_CTRL_PFM        1'b1        BTAON_reg_wr
    12      R/W AON_EXTRN_SWR_POW_SWR               1'b0        BTAON_reg_wr
    20:13   R/W AON_SWR_CORE_TUNE_VDIV2             8'b01010100 BTAON_reg_wr
    28:21   R/W AON_SWR_CORE_TUNE_VDIV1             8'b10001010 BTAON_reg_wr
    29      R/W AON_SWR_CORE_POW_SWR                1'b0        BTAON_reg_wr
    30      R/W AON_SWR_CORE_POW_LDO                1'b0        BTAON_reg_wr
    31      R/W AON_SWR_CORE_PFM1POS_EN_POS         1'b0        BTAON_reg_wr
 */
typedef union _AON_RG9X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AON_REG9X_DUMMY1: 5;
        uint32_t AON_MBIAS_POW_PCUT_VPON_TO_DVDD: 1;
        uint32_t AON_MBIAS_POW_PCUT_VD105_RF_TO_DVDD: 1;
        uint32_t AON_SWR_CORE_EN_SWR_SHORT: 1;
        uint32_t AON_SWR_CORE_SEL_FOLLOWCTRL1: 1;
        uint32_t AON_XTAL_EN_SUPPLY_MODE: 1;
        uint32_t AON_MBIAS_POW_LDOAUDIO_LQ: 1;
        uint32_t AON_SWR_CORE_SEL_CK_CTRL_PFM: 1;
        uint32_t AON_EXTRN_SWR_POW_SWR: 1;
        uint32_t AON_SWR_CORE_TUNE_VDIV2: 8;
        uint32_t AON_SWR_CORE_TUNE_VDIV1: 8;
        uint32_t AON_SWR_CORE_POW_SWR: 1;
        uint32_t AON_SWR_CORE_POW_LDO: 1;
        uint32_t AON_SWR_CORE_PFM1POS_EN_POS: 1;
    };
} AON_RG9X_TYPE;

/* 0x28     0x4000_0028
    0       R/W AON_BLE_STORE                       1'b0        BTAON_reg_wr
    1       R/W AON_RFC_RESTORE                     1'b0        BTAON_reg_wr
    2       R/W AON_PF_RESTORE                      1'b0        BTAON_reg_wr
    3       R/W AON_MODEM_RESTORE                   1'b0        BTAON_reg_wr
    4       R/W AON_DP_MODEM_RESTORE                1'b0        BTAON_reg_wr
    5       R/W AON_BZ_RESTORE                      1'b0        BTAON_reg_wr
    6       R/W AON_BLE_RESTORE                     1'b0        BTAON_reg_wr
    9:7     R/W AON_XTAL_SEL_MODE                   3'b100      BTAON_reg_wr
    10      R/W AON_ISO_XTAL                        1'b1        BTAON_reg_wr
    11      R/W AON_ISO_PLL4                        1'b0        BTAON_reg_wr
    12      R/W AON_ISO_PLL3                        1'b0        BTAON_reg_wr
    13      R/W AON_ISO_PLL2                        1'b1        BTAON_reg_wr
    14      R/W AON_ISO_PLL1                        1'b1        BTAON_reg_wr
    15      R/W AON_OSC40_POW_OSC                   1'b0        BTAON_reg_wr
    16      R/W AON_XTAL_POW_XTAL                   1'b0        BTAON_reg_wr
    17      R/W AON_VCORE_PC_POW_VCORE6_PC_VG2      1'b1        BTAON_reg_wr
    18      R/W AON_VCORE_PC_POW_VCORE6_PC_VG1      1'b1        BTAON_reg_wr
    19      R/W AON_VCORE_PC_POW_VCORE5_PC_VG2      1'b1        BTAON_reg_wr
    20      R/W AON_VCORE_PC_POW_VCORE5_PC_VG1      1'b1        BTAON_reg_wr
    21      R/W AON_VCORE_PC_POW_VCORE4_PC_VG2      1'b1        BTAON_reg_wr
    22      R/W AON_VCORE_PC_POW_VCORE4_PC_VG1      1'b1        BTAON_reg_wr
    23      R/W AON_VCORE_PC_POW_VCORE3_PC_VG2      1'b1        BTAON_reg_wr
    24      R/W AON_VCORE_PC_POW_VCORE3_PC_VG1      1'b1        BTAON_reg_wr
    25      R/W AON_XTAL_EN_LPMODE_AUTO_GATED       1'b0        BTAON_reg_wr
    26      R/W AON_XTAL_EN_LPCNT_CLK_DIV           1'b0        BTAON_reg_wr
    27      R/W AON_VCORE_PC_POW_VCORE1_PC_VG2      1'b1        BTAON_reg_wr
    28      R/W AON_VCORE_PC_POW_VCORE1_PC_VG1      1'b1        BTAON_reg_wr
    29      R/W AON_SWR_CORE_POW_CCMPFM             1'b0        BTAON_reg_wr
    30      R/W AON_BTPLL_LDO2_POW_LDO              1'b1        BTAON_reg_wr
    31      R/W AON_BTPLL_LDO1_POW_LDO              1'b0        BTAON_reg_wr
 */
typedef union _AON_RG10X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AON_BLE_STORE: 1;
        uint32_t AON_RFC_RESTORE: 1;
        uint32_t AON_PF_RESTORE: 1;
        uint32_t AON_MODEM_RESTORE: 1;
        uint32_t AON_DP_MODEM_RESTORE: 1;
        uint32_t AON_BZ_RESTORE: 1;
        uint32_t AON_BLE_RESTORE: 1;
        uint32_t AON_XTAL_SEL_MODE: 3;
        uint32_t AON_ISO_XTAL: 1;
        uint32_t AON_ISO_PLL4: 1;
        uint32_t AON_ISO_PLL3: 1;
        uint32_t AON_ISO_PLL2: 1;
        uint32_t AON_ISO_PLL1: 1;
        uint32_t AON_OSC40_POW_OSC: 1;
        uint32_t AON_XTAL_POW_XTAL: 1;
        uint32_t AON_VCORE_PC_POW_VCORE6_PC_VG2: 1;
        uint32_t AON_VCORE_PC_POW_VCORE6_PC_VG1: 1;
        uint32_t AON_VCORE_PC_POW_VCORE5_PC_VG2: 1;
        uint32_t AON_VCORE_PC_POW_VCORE5_PC_VG1: 1;
        uint32_t AON_VCORE_PC_POW_VCORE4_PC_VG2: 1;
        uint32_t AON_VCORE_PC_POW_VCORE4_PC_VG1: 1;
        uint32_t AON_VCORE_PC_POW_VCORE3_PC_VG2: 1;
        uint32_t AON_VCORE_PC_POW_VCORE3_PC_VG1: 1;
        uint32_t AON_XTAL_EN_LPMODE_AUTO_GATED: 1;
        uint32_t AON_XTAL_EN_LPCNT_CLK_DIV: 1;
        uint32_t AON_VCORE_PC_POW_VCORE1_PC_VG2: 1;
        uint32_t AON_VCORE_PC_POW_VCORE1_PC_VG1: 1;
        uint32_t AON_SWR_CORE_POW_CCMPFM: 1;
        uint32_t AON_BTPLL_LDO2_POW_LDO: 1;
        uint32_t AON_BTPLL_LDO1_POW_LDO: 1;
    };
} AON_RG10X_TYPE;

/* 0x2C     0x4000_002c
    0       R/W AON_REG11X_DUMMY1                   1'h0        BTAON_reg_wr
    1       R/W AON_SWR_AUDIO_PFMCCMPOS_EN_POS      1'b0        BTAON_reg_wr
    2       R/W AON_SWR_CORE_PFMCCM1POS_EN_POS      1'b0        BTAON_reg_wr
    3       R/W AON_SWR_CORE_PFMCCM1POS_EXT_RST_B   1'b0        BTAON_reg_wr
    4       R/W AON_BT_CORE6_RSTB                   1'b0        BTAON_reg_wr
    5       R/W AON_BT_CORE5_RSTB                   1'b0        BTAON_reg_wr
    6       R/W AON_BT_CORE4_RSTB                   1'b0        BTAON_reg_wr
    7       R/W AON_BT_CORE3_RSTB                   1'b0        BTAON_reg_wr
    8       R/W AON_BT_CORE2_RSTB                   1'b0        BTAON_reg_wr
    9       R/W AON_BT_CORE1_RSTB                   1'b0        BTAON_reg_wr
    10      R/W AON_BT_PON_RSTB                     1'b0        BTAON_reg_wr
    11      R/W AON_ISO_BT_PON                      1'b1        BTAON_reg_wr
    12      R/W AON_ISO_BT_CORE6                    1'b1        BTAON_reg_wr
    13      R/W AON_ISO_BT_CORE5                    1'b1        BTAON_reg_wr
    14      R/W AON_ISO_BT_CORE4                    1'b1        BTAON_reg_wr
    15      R/W AON_ISO_BT_CORE3                    1'b1        BTAON_reg_wr
    16      R/W AON_ISO_BT_CORE2                    1'b1        BTAON_reg_wr
    17      R/W AON_ISO_BT_CORE1                    1'b1        BTAON_reg_wr
    18      R/W AON_BT_RET_RSTB                     1'b0        BTAON_reg_wr
    19      R/W AON_BTPLL1_POW_PLL                  1'b0        BTAON_reg_wr
    20      R/W AON_BTPLL1_POW_DIV_PLL_CLK          1'b0        BTAON_reg_wr
    21      R/W AON_BTPLL2_POW_PLL                  1'b0        BTAON_reg_wr
    22      R/W AON_BTPLL2_POW_DIV_PLL_CLK          1'b0        BTAON_reg_wr
    23      R/W AON_BTPLL3_POW_PLL                  1'b0        BTAON_reg_wr
    24      R/W AON_BTPLL3_POW_DIV_PLL_CLK          1'b0        BTAON_reg_wr
    25      R/W AON_BTPLL4_POW_PLL                  1'b0        BTAON_reg_wr
    26      R/W AON_BTPLL4_POW_DIV_PLL_CLK          1'b0        BTAON_reg_wr
    27      R/W AON_RFC_STORE                       1'b0        BTAON_reg_wr
    28      R/W AON_PF_STORE                        1'b0        BTAON_reg_wr
    29      R/W AON_MODEM_STORE                     1'b0        BTAON_reg_wr
    30      R/W AON_DP_MODEM_STORE                  1'b0        BTAON_reg_wr
    31      R/W AON_BZ_STORE                        1'b0        BTAON_reg_wr
 */
typedef union _AON_RG11X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t AON_REG11X_DUMMY1: 1;
        uint32_t AON_SWR_AUDIO_PFMCCMPOS_EN_POS: 1;
        uint32_t AON_SWR_CORE_PFMCCM1POS_EN_POS: 1;
        uint32_t AON_SWR_CORE_PFMCCM1POS_EXT_RST_B: 1;
        uint32_t AON_BT_CORE6_RSTB: 1;
        uint32_t AON_BT_CORE5_RSTB: 1;
        uint32_t AON_BT_CORE4_RSTB: 1;
        uint32_t AON_BT_CORE3_RSTB: 1;
        uint32_t AON_BT_CORE2_RSTB: 1;
        uint32_t AON_BT_CORE1_RSTB: 1;
        uint32_t AON_BT_PON_RSTB: 1;
        uint32_t AON_ISO_BT_PON: 1;
        uint32_t AON_ISO_BT_CORE6: 1;
        uint32_t AON_ISO_BT_CORE5: 1;
        uint32_t AON_ISO_BT_CORE4: 1;
        uint32_t AON_ISO_BT_CORE3: 1;
        uint32_t AON_ISO_BT_CORE2: 1;
        uint32_t AON_ISO_BT_CORE1: 1;
        uint32_t AON_BT_RET_RSTB: 1;
        uint32_t AON_BTPLL1_POW_PLL: 1;
        uint32_t AON_BTPLL1_POW_DIV_PLL_CLK: 1;
        uint32_t AON_BTPLL2_POW_PLL: 1;
        uint32_t AON_BTPLL2_POW_DIV_PLL_CLK: 1;
        uint32_t AON_BTPLL3_POW_PLL: 1;
        uint32_t AON_BTPLL3_POW_DIV_PLL_CLK: 1;
        uint32_t AON_BTPLL4_POW_PLL: 1;
        uint32_t AON_BTPLL4_POW_DIV_PLL_CLK: 1;
        uint32_t AON_RFC_STORE: 1;
        uint32_t AON_PF_STORE: 1;
        uint32_t AON_MODEM_STORE: 1;
        uint32_t AON_DP_MODEM_STORE: 1;
        uint32_t AON_BZ_STORE: 1;
    };
} AON_RG11X_TYPE;

/* 0x30     0x4000_0030
    0       R/W FW_enter_lps                        1'b0        BTAON_reg_wr
    1       R/W FW_PON_SEQ_RST_N                    1'b0        BTAON_reg_wr
    2       R/W AON_GATED_EN                        1'b1        BTAON_reg_wr
    3       R/W AON_MBIAS_SEL_DPD_RCK               1'b0        BTAON_reg_wr
    31:4    R/W AON_RG12X_DUMMY1                    28'h0       BTAON_reg_wr
 */
typedef union _AON_RG12X_TYPE
{
    uint32_t d32;
    struct
    {
        uint32_t FW_enter_lps: 1;
        uint32_t FW_PON_SEQ_RST_N: 1;
        uint32_t AON_GATED_EN: 1;
        uint32_t AON_MBIAS_SEL_DPD_RCK: 1;
        uint32_t AON_RG12X_DUMMY1: 28;
    };
} AON_RG12X_TYPE;


#define AON_FAST_0x0                                0x0
#define AON_FAST_0x1                                0x1
#define AON_FAST_0x2                                0x2
#define AON_FAST_0x3                                0x3     // record patch_end_index
#define AON_FAST_DEBUG_PASSWORD                     0x4     // 0x04 ~ 0x13 store the debug password
#define AON_FAST_0x6                                0x6
#define AON_FAST_0x8                                0x8
#define AON_FAST_0xA                                0xA
#define AON_FAST_0xC                                0xC
#define AON_FAST_0xE                                0xE
#define AON_FAST_0x10                               0x10
#define AON_FAST_0x12                               0x12
#define AON_FAST_0x14                               0x14
#define AON_FAST_0x15                               0x15
#define AON_FAST_REF_RESISTANCE                     0x16    // 0x16 ~ 0x17 store the ref resistance
#define AON_FAST_0x18                               0x18
#define AON_FAST_0x1A                               0x1A
#define AON_FAST_0x1C                               0x1C
#define AON_FAST_0x1E                               0x1E
#define AON_FAST_0x20                               0x20
#define AON_FAST_0x22                               0x22
#define AON_FAST_0x24                               0x24
#define AON_FAST_0x26                               0x26
#define AON_FAST_0x28                               0x28
#define AON_FAST_0x2A                               0x2A
#define AON_FAST_0x2C                               0x2C
#define AON_FAST_0x2E                               0x2E
#define AON_FAST_0x30                               0x30
#define AON_FAST_0x46                               0x46
#define AON_FAST_0xA8                               0xA8
#define AON_FAST_0xAA                               0xAA
#define AON_FAST_0xAE                               0xAE
#define AON_FAST_0xC6                               0xC6
#define AON_FAST_0xCC                               0xCC
#define AON_FAST_0xCE                               0xCE
#define AON_FAST_0xEA                               0xEA
#define AON_FAST_0xEC                               0xEC
#define AON_FAST_0xEE                               0xEE
#define AON_FAST_0xF0                               0xF0
#define AON_FAST_0xF4                               0xF4
#define AON_FAST_0xF6                               0xF6
#define AON_FAST_0x122                              0x122
#define AON_FAST_0x124                              0x124
#define AON_FAST_0x126                              0x126
#define AON_FAST_0x128                              0x128
#define AON_FAST_0x12A                              0x12A
#define AON_FAST_0x12C                              0x12C
#define AON_FAST_0x12E                              0x12E
#define AON_FAST_0x130                              0x130
#define AON_FAST_0x132                              0x132
#define AON_FAST_0x13A                              0x13A
#define AON_FAST_0x140                              0x140
#define AON_FAST_0x142                              0x142
#define AON_FAST_0x154                              0x154
#define AON_FAST_0x156                              0x156
#define AON_FAST_0x158                              0x158
#define AON_FAST_0x15A                              0x15A
#define AON_FAST_0x15E                              0x15E
#define AON_FAST_0x160                              0x160
#define AON_FAST_0x164                              0x164
#define AON_FAST_0x166                              0x166
#define AON_FAST_0x172                              0x172
#define AON_FAST_0x178                              0x178
#define AON_FAST_0x17E                              0x17E
#define AON_FAST_0x180                              0x180
#define AON_FAST_0x182                              0x182
#define AON_FAST_0x184                              0x184
#define AON_FAST_0x186                              0x186
#define AON_FAST_0x188                              0x188
#define AON_FAST_0x18A                              0x18A
#define AON_FAST_0x18C                              0x18C
#define AON_FAST_0x18E                              0x18E
#define AON_FAST_0x190                              0x190
#define AON_FAST_0x192                              0x192
#define AON_FAST_0x194                              0x194
#define AON_FAST_0x196                              0x196
#define AON_FAST_0x198                              0x198
#define AON_FAST_0x19A                              0x19A
#define AON_FAST_0x19C                              0x19C
#define AON_FAST_0x19E                              0x19E
#define AON_FAST_0x1A0                              0x1A0
#define AON_FAST_0x1A2                              0x1A2
#define AON_FAST_0x1A4                              0x1A4
#define AON_FAST_0x1A6                              0x1A6
#define AON_FAST_0x1A8                              0x1A8
#define AON_FAST_0x1AA                              0x1AA
#define AON_FAST_0x1AC                              0x1AC
#define AON_FAST_0x1AE                              0x1AE
#define AON_FAST_0x1B0                              0x1B0
#define AON_FAST_0x1B2                              0x1B2
#define AON_FAST_0x1B4                              0x1B4
#define AON_FAST_0x1B6                              0x1B6
#define AON_FAST_0x1B8                              0x1B8
#define AON_FAST_0x1BA                              0x1BA
#define AON_FAST_0x1BC                              0x1BC
#define AON_FAST_0x1BE                              0x1BE
#define AON_FAST_0x1C0                              0x1C0
#define AON_FAST_0x1C2                              0x1C2
#define AON_FAST_0x1C4                              0x1C4
#define AON_FAST_0x1C6                              0x1C6
#define AON_FAST_0x1CC                              0x1CC
#define AON_FAST_0x1CE                              0x1CE
#define AON_FAST_0x1D0                              0x1D0
#define AON_FAST_0x1D2                              0x1D2
#define AON_FAST_0x1D4                              0x1D4
#define AON_FAST_0x1D6                              0x1D6
#define AON_FAST_0x1D8                              0x1D8
#define AON_FAST_0x1DA                              0x1DA
#define AON_FAST_0x1DC                              0x1DC
#define AON_FAST_0x1DE                              0x1DE
#define AON_FAST_0x1E0                              0x1E0
#define AON_FAST_0x1E2                              0x1E2
#define AON_FAST_0x1E4                              0x1E4
#define AON_FAST_0x1E6                              0x1E6
#define AON_FAST_0x1E8                              0x1E8
#define AON_FAST_0x1EA                              0x1EA
#define AON_FAST_0x1EC                              0x1EC
#define AON_FAST_0x1EE                              0x1EE
#define AON_FAST_0x1F0                              0x1F0
#define AON_FAST_0x1F2                              0x1F2
#define AON_FAST_0x1FE                              0x1FE
#define AON_FAST_0x200                              0x200
#define AON_FAST_0x202                              0x202
#define AON_FAST_0x204                              0x204
#define AON_FAST_0x206                              0x206
#define AON_FAST_0x208                              0x208
#define AON_FAST_0x20A                              0x20A
#define AON_FAST_0x20C                              0x20C
#define AON_FAST_0x20E                              0x20E
#define AON_FAST_0x210                              0x210
#define AON_FAST_0x212                              0x212
#define AON_FAST_0x214                              0x214
#define AON_FAST_0x216                              0x216
#define AON_FAST_0x218                              0x218
#define AON_FAST_0x21A                              0x21A
#define AON_FAST_0x21C                              0x21C
#define AON_FAST_0x21E                              0x21E
#define AON_FAST_0x220                              0x220
#define AON_FAST_0x222                              0x222
#define AON_FAST_0x224                              0x224
#define AON_FAST_0x226                              0x226
#define AON_FAST_0x228                              0x228
#define AON_FAST_0x22A                              0x22A
#define AON_FAST_0x22C                              0x22C
#define AON_FAST_0x22E                              0x22E
#define AON_FAST_0x230                              0x230
#define AON_FAST_0x232                              0x232
#define AON_FAST_0x234                              0x234
#define AON_FAST_0x236                              0x236
#define AON_FAST_0x238                              0x238
#define AON_FAST_0x23A                              0x23A
#define AON_FAST_0x23C                              0x23C
#define AON_FAST_0x23E                              0x23E
#define AON_FAST_0x240                              0x240
#define AON_FAST_0x242                              0x242
#define AON_FAST_0x244                              0x244
#define AON_FAST_0x246                              0x246
#define AON_FAST_0x248                              0x248
#define AON_FAST_0x24A                              0x24A
#define AON_FAST_0x24C                              0x24C
#define AON_FAST_0x24E                              0x24E
#define AON_FAST_0x250                              0x250
#define AON_FAST_0x252                              0x252
#define AON_FAST_0x254                              0x254
#define AON_FAST_0x256                              0x256
#define AON_FAST_0x258                              0x258
#define AON_FAST_0x25A                              0x25A
#define AON_FAST_0x25C                              0x25C
#define AON_FAST_0x25E                              0x25E
#define AON_FAST_0x260                              0x260
#define AON_FAST_0x262                              0x262
#define AON_FAST_0x264                              0x264
#define AON_FAST_0x266                              0x266
#define AON_FAST_0x268                              0x268
#define AON_FAST_0x26A                              0x26A
#define AON_FAST_0x26C                              0x26C
#define AON_FAST_0x26E                              0x26E
#define AON_FAST_0x35C                              0x35C
#define AON_FAST_0x35E                              0x35E
#define AON_FAST_0x36A                              0x36A
#define AON_FAST_0x370                              0x370
#define AON_FAST_0x376                              0x376
#define AON_FAST_0x378                              0x378
#define AON_FAST_0x37A                              0x37A
#define AON_FAST_0x3A8                              0x3A8
#define AON_FAST_0x3AA                              0x3AA
#define AON_FAST_0x3AC                              0x3AC
#define AON_FAST_0x3B0                              0x3B0
#define AON_FAST_0x3B2                              0x3B2
#define AON_FAST_0x3B4                              0x3B4
#define AON_FAST_0x3B6                              0x3B6
#define AON_FAST_0x3B8                              0x3B8
#define AON_FAST_0x3BA                              0x3BA
#define AON_FAST_0x3E8                              0x3E8
#define AON_FAST_0x3F4                              0x3F4
#define AON_FAST_0x3F6                              0x3F6
#define AON_FAST_REG0X_FW_GENERAL                   0x400
#define AON_FAST_REG1X_FW_GENERAL                   0x402
#define AON_FAST_REG2X_FW_GENERAL                   0x404
#define AON_FAST_REG3X_FW_GENERAL                   0x406
#define AON_FAST_REG4X_FW_GENERAL                   0x408
#define AON_FAST_REG5X_FW_GENERAL                   0x40A
#define AON_FAST_REG6X_FW_GENERAL                   0x40C
#define AON_FAST_REG7X_FW_GENERAL                   0x40E
#define AON_FAST_REG8X_FW_GENERAL                   0x410
#define AON_FAST_REG9X_FW_GENERAL                   0x412
#define AON_FAST_REG10X_FW_GENERAL                  0x414
#define AON_FAST_REG11X_FW_GENERAL                  0x416
#define AON_FAST_REG12X_FW_GENERAL                  0x418
#define AON_FAST_REG13X_FW_GENERAL                  0x41A
#define AON_FAST_REG14X_FW_GENERAL                  0x41C
#define AON_FAST_REG15X_FW_GENERAL                  0x41E
#define AON_FAST_REG16X_FW_GENERAL                  0x420
#define AON_FAST_REG17X_FW_GENERAL                  0x422
#define AON_FAST_REG18X_FW_GENERAL                  0x424
#define AON_FAST_REG19X_FW_GENERAL                  0x426
#define AON_FAST_REG20X_FW_GENERAL                  0x428
#define AON_FAST_REG21X_FW_GENERAL                  0x42A
#define AON_FAST_REG22X_FW_GENERAL                  0x42C
#define AON_FAST_REG23X_FW_GENERAL                  0x42E
#define AON_FAST_REG_PAD_P0_0_P0_1                  0x480
#define AON_FAST_REG_PAD_P0_2_P0_3                  0x482
#define AON_FAST_REG_PAD_P0_4_P0_5                  0x484
#define AON_FAST_REG_PAD_P0_6_P0_7                  0x486
#define AON_FAST_REG_PAD_P1_0_P1_1                  0x488
#define AON_FAST_REG_PAD_P1_2_P1_3                  0x48A
#define AON_FAST_REG_PAD_P1_4_P1_5                  0x48C
#define AON_FAST_REG_PAD_P1_6_P1_7                  0x48E
#define AON_FAST_REG_PAD_P2_0_P2_1                  0x490
#define AON_FAST_REG_PAD_P2_2_P2_3                  0x492
#define AON_FAST_REG_PAD_P2_4_P2_5                  0x494
#define AON_FAST_REG_PAD_P2_6_P2_7                  0x496
#define AON_FAST_REG_PAD_P3_0_P3_1                  0x498
#define AON_FAST_REG_PAD_P3_2_P3_3                  0x49A
#define AON_FAST_REG_PAD_P3_4_P3_5                  0x49C
#define AON_FAST_REG_PAD_P3_6_P3_7                  0x49E
#define AON_FAST_REG_PAD_P4_0_P4_1                  0x4A0
#define AON_FAST_REG_PAD_P4_2_P4_3                  0x4A2
#define AON_FAST_REG_PAD_P4_4_P4_5                  0x4A4
#define AON_FAST_REG_PAD_P4_6_P4_7                  0x4A6
#define AON_FAST_REG_PAD_P5_0_P5_1                  0x4A8
#define AON_FAST_REG_PAD_P5_2_P5_3                  0x4AA
#define AON_FAST_REG_PAD_P5_4_P5_5                  0x4AC
#define AON_FAST_REG_PAD_P5_6_P5_7                  0x4AE
#define AON_FAST_REG_PAD_P6_0_P6_1                  0x4B0
#define AON_FAST_REG_PAD_LOUT                       0x4B2
#define AON_FAST_REG_PAD_ROUT                       0x4B4
#define AON_FAST_REG_PAD_MIC1                       0x4B6
#define AON_FAST_REG_PAD_MIC2                       0x4B8
#define AON_FAST_REG_PAD_MIC3                       0x4BA
#define AON_FAST_REG_PAD_MIC4                       0x4BC
#define AON_FAST_REG_PAD_MIC5                       0x4BE
#define AON_FAST_REG_PAD_MIC6                       0x4C0
#define AON_FAST_REG_PAD_AUX                        0x4C2
#define AON_FAST_REG_PAD_MICBIAS                    0x4C4
#define AON_FAST_REG_PAD_32K                        0x4C6
#define AON_FAST_REG_TEST_MODE                      0x4C8
#define AON_FAST_REG1X_TEST_MODE                    0x4CA
#define AON_FAST_REG0X_PMUX_DIGI_MODE_EN            0x4CC
#define AON_FAST_REG1X_PMUX_DIGI_MODE_EN            0x4CE
#define AON_FAST_REG0X_PAD_POWER_DOMAIN_ADC         0x4D0
#define AON_FAST_REG1X_PAD_POWER_DOMAIN_ADC         0x4D2
#define AON_FAST_REG0X_PAD_POWER_DOMAIN_P1          0x4D4
#define AON_FAST_REG1X_PAD_POWER_DOMAIN_P1          0x4D6
#define AON_FAST_REG0X_PAD_POWER_DOMAIN_P2          0x4D8
#define AON_FAST_REG1X_PAD_POWER_DOMAIN_P2          0x4DA
#define AON_FAST_REG0X_PAD_POWER_DOMAIN_P3          0x4DC
#define AON_FAST_REG1X_PAD_POWER_DOMAIN_P3          0x4DE
#define AON_FAST_REG0X_PAD_POWER_DOMAIN_P4          0x4E0
#define AON_FAST_REG1X_PAD_POWER_DOMAIN_P4          0x4E2
#define AON_FAST_REG0X_PAD_POWER_DOMAIN_P5          0x4E4
#define AON_FAST_REG1X_PAD_POWER_DOMAIN_P5          0x4E6
#define AON_FAST_REG0X_PAD_POWER_DOMAIN_P6          0x4E8
#define AON_FAST_REG_PAD_POWER_DOMAIN_LOUT_ROUT     0x4EA
#define AON_FAST_REG_PAD_POWER_DOMAIN_MIC1_MIC2     0x4EC
#define AON_FAST_REG_PAD_POWER_DOMAIN_MIC3_MIC4     0x4EE
#define AON_FAST_REG_PAD_POWER_DOMAIN_MIC5_MIC6     0x4F0
#define AON_FAST_REG_PAD_POWER_DOMAIN_AUX_MICBIAS   0x4F2
#define AON_FAST_REG_PAD_POWER_DOMAIN_32K           0x4F4
#define AON_FAST_REG_SPIC_PULL_SELECT               0x4F6
#define AON_FAST_REG_PAD_P6_2_P6_3                  0x4F8
#define AON_FAST_REG_PAD_P6_4_P6_5                  0x4FA
#define AON_FAST_REG_PAD_P6_6_P6_7                  0x4FC
#define AON_FAST_REG_PAD_P7_0_P7_1                  0x4FE
#define AON_FAST_REG_PAD_P7_2_P7_3                  0x500
#define AON_FAST_REG_PAD_P7_4_P7_5                  0x502
#define AON_FAST_REG_PAD_P7_6                       0x504
#define AON_FAST_REG1X_PAD_POWER_DOMAIN_P6          0x506
#define AON_FAST_REG0X_PAD_POWER_DOMAIN_P7          0x508
#define AON_FAST_REG1X_PAD_POWER_DOMAIN_P7          0x50A
#define AON_FAST_REG2X_TEST_MODE                    0x50C
#define AON_FAST_REG1X_SPIC_PULL_SELECT             0x50E
#define AON_FAST_REG_0x580                          0x580
#define AON_FAST_REG_0x582                          0x582
#define AON_FAST_REG_0x584                          0x584
#define AON_FAST_REG_0x586                          0x586
#define AON_FAST_REG_0x588                          0x588
#define AON_FAST_REG_RAMROM_VSEL                    0x58A
#define AON_FAST_REG_ROM_MCU                        0x58C
#define AON_FAST_REG_ROM_MDM                        0x58E
#define AON_FAST_REG_ROM_DSP1_HV_LV                 0x590
#define AON_FAST_REG_ROM_DSP2_HV_LV                 0x592
#define AON_FAST_REG_ROM_RFC                        0x594
#define AON_FAST_REG_RAM_DATA_HV                    0x596
#define AON_FAST_REG_RAM_DATA_LV                    0x598
#define AON_FAST_REG_RAM_COM01_HV                   0x59A
#define AON_FAST_REG_RAM_COM01_LV                   0x59C
#define AON_FAST_REG_RAM_COM2_HV                    0x59E
#define AON_FAST_REG_RAM_COM2_LV                    0x5A0
#define AON_FAST_REG_RAM_CACHE                      0x5A2
#define AON_FAST_REG_RAM_BUF_HV                     0x5A4
#define AON_FAST_REG_RAM_BUF_LV                     0x5A6
#define AON_FAST_REG_RAM_RSA                        0x5A8
#define AON_FAST_REG_RAM_SDIO                       0x5AA
#define AON_FAST_REG_RAM_USB                        0x5AC
#define AON_FAST_REG_RAM_MAC_HV                     0x5AE
#define AON_FAST_REG_RAM_MAC_LV                     0x5B0
#define AON_FAST_REG_RAM_MAC_RF_HV                  0x5B2
#define AON_FAST_REG_RAM_MAC_RF_LV                  0x5B4
#define AON_FAST_REG_RAM_MDM_HV                     0x5B6
#define AON_FAST_REG_RAM_MDM_LV                     0x5B8
#define AON_FAST_REG_RAM_MDM_RF_HV                  0x5BA
#define AON_FAST_REG_RAM_MDM_RF_LV                  0x5BC
#define AON_FAST_REG_RAM_DSP1_HV                    0x5BE
#define AON_FAST_REG_RAM_DSP1_LV                    0x5C0
#define AON_FAST_REG_RAM_DSP1_CACHE_HV              0x5C2
#define AON_FAST_REG_RAM_DSP1_CACHE_LV              0x5C4
#define AON_FAST_REG_RAM_DSP1_FFT_HV                0x5C6
#define AON_FAST_REG_RAM_DSP1_FFT_LV                0x5C8
#define AON_FAST_REG_RAM_DSP2_HV                    0x5CA
#define AON_FAST_REG_RAM_DSP2_LV                    0x5CC
#define AON_FAST_REG_RAM_DSP2_CACHE_HV              0x5CE
#define AON_FAST_REG_RAM_DSP2_CACHE_LV              0x5D0
#define AON_FAST_REG_RAM_DSP2_FFT_HV                0x5D2
#define AON_FAST_REG_RAM_DSP2_FFT_LV                0x5D4
#define AON_FAST_REG_RAM_DSP_SYS_HV                 0x5D6
#define AON_FAST_REG_RAM_DSP_SYS_LV                 0x5D8
#define AON_FAST_REG_RAM_VADBUF_HV                  0x5DA
#define AON_FAST_REG_RAM_VADBUF_LV                  0x5DC
#define AON_FAST_REG_RAM_ANCDSP_HV                  0x5DE
#define AON_FAST_REG_RAM_ANCDSP_LV                  0x5E0
#define AON_FAST_REG_RAM_RFC_HV                     0x5E2
#define AON_FAST_REG_RAM_RFC_LV                     0x5E4
#define AON_FAST_REG_RAM_RFC_RF_HV                  0x5E6
#define AON_FAST_REG_RAM_RFC_RF_LV                  0x5E8
#define AON_FAST_REG_0x5EA                          0x5EA
#define AON_FAST_SRAM_DS_ISO0_1                     0x5EC
#define AON_FAST_SRAM_DS_ISO0_2                     0x5EE
#define AON_FAST_SRAM_DS_ISO0_3                     0x5F0
#define AON_FAST_SRAM_DS_ISO0_4                     0x5F2
#define AON_FAST_SRAM_DS_ISO0_5                     0x5F4
#define AON_FAST_SRAM_DS_ISO0_6                     0x5F6
#define AON_FAST_SRAM_DS_ISO0_7                     0x5F8
#define AON_FAST_SRAM_DS_ISO0_8                     0x5FA
#define AON_FAST_SRAM_DS_ISO0_9                     0x5FC
#define AON_FAST_SRAM_DS_ISO1_1                     0x5FE
#define AON_FAST_SRAM_DS_ISO1_2                     0x600
#define AON_FAST_SRAM_DS_ISO1_3                     0x602
#define AON_FAST_SRAM_DS_ISO1_4                     0x604
#define AON_FAST_SRAM_DS_ISO1_5                     0x606
#define AON_FAST_SRAM_DS_ISO1_6                     0x608
#define AON_FAST_SRAM_DS_ISO1_7                     0x60A
#define AON_FAST_SRAM_DS_ISO1_8                     0x60C
#define AON_FAST_SRAM_DS_ISO1_9                     0x60E
#define AON_FAST_SRAM_SD_ISO0_1                     0x610
#define AON_FAST_SRAM_SD_ISO0_2                     0x612
#define AON_FAST_SRAM_SD_ISO0_3                     0x614
#define AON_FAST_SRAM_SD_ISO0_4                     0x616
#define AON_FAST_SRAM_SD_ISO0_5                     0x618
#define AON_FAST_SRAM_SD_ISO0_6                     0x61A
#define AON_FAST_SRAM_SD_ISO0_7                     0x61C
#define AON_FAST_SRAM_SD_ISO0_8                     0x61E
#define AON_FAST_SRAM_SD_ISO0_9                     0x620
#define AON_FAST_SRAM_SD_ISO1_1                     0x622
#define AON_FAST_SRAM_SD_ISO1_2                     0x624
#define AON_FAST_SRAM_SD_ISO1_3                     0x626
#define AON_FAST_SRAM_SD_ISO1_4                     0x628
#define AON_FAST_SRAM_SD_ISO1_5                     0x62A
#define AON_FAST_SRAM_SD_ISO1_6                     0x62C
#define AON_FAST_SRAM_SD_ISO1_7                     0x62E
#define AON_FAST_SRAM_SD_ISO1_8                     0x630
#define AON_FAST_SRAM_SD_ISO1_9                     0x632
#define AON_FAST_SRAM_DSSD_VCORE_1                  0x634
#define AON_FAST_SRAM_DSSD_VCORE_2                  0x636
#define AON_FAST_AON_LOADER_TIMER0                  0x638
#define AON_FAST_AON_LOADER_TIMER1                  0x63A
#define AON_FAST_AON_LOADER_TIMER2                  0x63C
#define AON_FAST_AON_LOADER_TIMER3                  0x63E
#define AON_FAST_AON_LOADER_TIMER4                  0x640
#define AON_FAST_AON_LOADER_TIMER5                  0x642
#define AON_FAST_MEM_VSEL_SYNC_EN_MISC              0x644
#define AON_FAST_REG_ROM_ANCDSP                     0x646
#define AON_FAST_REG_RAM_CACHE_RF                   0x648
#define AON_FAST_REG_RAM_DSP2_CACHE_RF_HV           0x64A
#define AON_FAST_REG_RAM_DSP2_CACHE_RF_LV           0x64C
#define AON_FAST_REG_RAM_NNA_HV                     0x64E
#define AON_FAST_REG_RAM_NNA_LV                     0x650
#define AON_FAST_REG_RAM_NNA_RF_HV                  0x652
#define AON_FAST_REG_RAM_NNA_RF_LV                  0x654
#define AON_FAST_SRAM_DS_ISO0_10                    0x656
#define AON_FAST_SRAM_DS_ISO1_10                    0x658
#define AON_FAST_SRAM_SD_ISO0_10                    0x65A
#define AON_FAST_SRAM_SD_ISO1_10                    0x65C
#define AON_FAST_SRAM_DS_ISO0_11                    0x65E
#define AON_FAST_SRAM_DS_ISO0_12                    0x660
#define AON_FAST_SRAM_DS_ISO0_13                    0x662
#define AON_FAST_SRAM_DS_ISO0_14                    0x664
#define AON_FAST_SRAM_DS_ISO0_15                    0x666
#define AON_FAST_SRAM_DS_ISO1_11                    0x668
#define AON_FAST_SRAM_DS_ISO1_12                    0x66A
#define AON_FAST_SRAM_DS_ISO1_13                    0x66C
#define AON_FAST_SRAM_DS_ISO1_14                    0x66E
#define AON_FAST_SRAM_DS_ISO1_15                    0x670
#define AON_FAST_SRAM_SD_ISO0_11                    0x672
#define AON_FAST_SRAM_SD_ISO0_12                    0x674
#define AON_FAST_SRAM_SD_ISO0_13                    0x676
#define AON_FAST_SRAM_SD_ISO0_14                    0x678
#define AON_FAST_SRAM_SD_ISO0_15                    0x67A
#define AON_FAST_SRAM_SD_ISO1_11                    0x67C
#define AON_FAST_SRAM_SD_ISO1_12                    0x67E
#define AON_FAST_SRAM_SD_ISO1_13                    0x680
#define AON_FAST_SRAM_SD_ISO1_14                    0x682
#define AON_FAST_SRAM_SD_ISO1_15                    0x684
#define AON_FAST_SRAM_RM3_ISO0_1                    0x686
#define AON_FAST_SRAM_RM3_ISO0_2                    0x688
#define AON_FAST_SRAM_RM3_ISO0_3                    0x68A
#define AON_FAST_SRAM_RM3_ISO0_4                    0x68C
#define AON_FAST_SRAM_RM3_ISO0_5                    0x68E
#define AON_FAST_SRAM_RM3_ISO0_6                    0x690
#define AON_FAST_SRAM_RM3_ISO0_7                    0x692
#define AON_FAST_SRAM_RM3_ISO0_8                    0x694
#define AON_FAST_SRAM_RM3_ISO0_9                    0x696
#define AON_FAST_SRAM_RM3_ISO1_1                    0x698
#define AON_FAST_SRAM_RM3_ISO1_2                    0x69A
#define AON_FAST_SRAM_RM3_ISO1_3                    0x69C
#define AON_FAST_SRAM_RM3_ISO1_4                    0x69E
#define AON_FAST_SRAM_RM3_ISO1_5                    0x6A0
#define AON_FAST_SRAM_RM3_ISO1_6                    0x6A2
#define AON_FAST_SRAM_RM3_ISO1_7                    0x6A4
#define AON_FAST_SRAM_RM3_ISO1_8                    0x6A6
#define AON_FAST_SRAM_RM3_ISO1_9                    0x6A8
#define AON_FAST_SRAM_RM3_ISO0_10                   0x6AA
#define AON_FAST_SRAM_RM3_ISO1_10                   0x6AC
#define AON_FAST_SRAM_RM3_ISO0_11                   0x6AE
#define AON_FAST_SRAM_RM3_ISO0_12                   0x6B0
#define AON_FAST_SRAM_RM3_ISO0_13                   0x6B2
#define AON_FAST_SRAM_RM3_ISO0_14                   0x6B4
#define AON_FAST_SRAM_RM3_ISO0_15                   0x6B6
#define AON_FAST_SRAM_RM3_ISO1_11                   0x6B8
#define AON_FAST_SRAM_RM3_ISO1_12                   0x6BA
#define AON_FAST_SRAM_RM3_ISO1_13                   0x6BC
#define AON_FAST_SRAM_RM3_ISO1_14                   0x6BE
#define AON_FAST_SRAM_RM3_ISO1_15                   0x6C0
#define AON_FAST_REG0X_BTPLL_SYS                    0x6F0
#define AON_FAST_REG1X_BTPLL_SYS                    0x6F2
#define AON_FAST_REG2X_BTPLL_SYS                    0x6F4
#define AON_FAST_REG3X_BTPLL_SYS                    0x6F6
#define AON_FAST_REG0X_XTAL_OSC_SYS                 0x6F8
#define AON_FAST_REG0X_RET_SYS                      0x6FA
#define AON_FAST_REG0X_CORE_SYS                     0x6FC
#define AON_FAST_REG1X_CORE_SYS                     0x6FE
#define AON_FAST_REG2X_CORE_SYS                     0x700
#define AON_FAST_REG3X_CORE_SYS                     0x702
#define AON_FAST_REG4X_CORE_SYS                     0x704
#define AON_FAST_REG0X_REG_MASK                     0x706
#define AON_FAST_REG1X_REG_MASK                     0x708
#define AON_FAST_REG0X_PMU_POS_CLK_MUX              0x70A
#define AON_FAST_REG0X_ANCDSP_CTRL                  0x70C
#define AON_FAST_REG1X_ANCDSP_CTRL                  0x70E
#define AON_FAST_REG5X_CORE_SYS                     0x710
#define AON_FAST_REG6X_CORE_SYS                     0x712
#define AON_FAST_REG7X_CORE_SYS                     0x714
#define AON_FAST_REG8X_CORE_SYS                     0x716
#define AON_FAST_REG0X_PAD_LPC                      0x742
#define AON_FAST_REG1X_PAD_LPC                      0x744
#define AON_FAST_REG2X_PAD_LPC                      0x746
#define AON_FAST_REG3X_PAD_LPC                      0x748
#define AON_FAST_REG4X_PAD_LPC                      0x74A
#define AON_FAST_REG5X_PAD_LPC                      0x74C
#define AON_FAST_REG6X_PAD_LPC                      0x74E
#define AON_FAST_REG7X_PAD_LPC                      0x750
#define AON_FAST_LOP_PON_RG0X                       0x800
#define AON_FAST_LOP_PON_RG1X                       0x802
#define AON_FAST_LOP_PON_RG2X                       0x804
#define AON_FAST_LOP_PON_RG3X                       0x806
#define AON_FAST_LOP_PON_RG4X                       0x808
#define AON_FAST_LOP_PON_RG5X                       0x80A
#define AON_FAST_LOP_PON_RG6X                       0x80C
#define AON_FAST_LOP_PON_RG7X                       0x80E
#define AON_FAST_LOP_PON_RG8X                       0x810
#define AON_FAST_LOP_PON_RG9X                       0x812
#define AON_FAST_LOP_PON_RG10X                      0x814
#define AON_FAST_LOP_PON_RG11X                      0x816
#define AON_FAST_LOP_PON_RG12X                      0x818
#define AON_FAST_LOP_PON_RG13X                      0x81A
#define AON_FAST_LOP_PON_RG14X                      0x81C
#define AON_FAST_LOP_PON_RG15X                      0x81E
#define AON_FAST_LOP_PON_RG16X                      0x820
#define AON_FAST_LOP_PON_RG17X                      0x822
#define AON_FAST_LOP_PON_RG18X                      0x824
#define AON_FAST_LOP_PON_RG19X                      0x826
#define AON_FAST_LOP_PON_RG20X                      0x828
#define AON_FAST_LOP_PON_RG21X                      0x82A
#define AON_FAST_LOP_PON_RG22X                      0x82C
#define AON_FAST_LOP_PON_RG23X                      0x82E
#define AON_FAST_LOP_PON_DELAY_RG0X                 0x830
#define AON_FAST_LOP_PON_DELAY_RG1X                 0x832
#define AON_FAST_LOP_PON_DELAY_RG2X                 0x834
#define AON_FAST_LOP_PON_DELAY_RG3X                 0x836
#define AON_FAST_LOP_PON_DELAY_RG4X                 0x838
#define AON_FAST_LOP_PON_DELAY_RG5X                 0x83A
#define AON_FAST_LOP_PON_DELAY_RG6X                 0x83C
#define AON_FAST_LOP_POF_RG0X                       0x850
#define AON_FAST_LOP_POF_RG1X                       0x852
#define AON_FAST_LOP_POF_RG2X                       0x854
#define AON_FAST_LOP_POF_RG3X                       0x856
#define AON_FAST_LOP_POF_RG4X                       0x858
#define AON_FAST_LOP_POF_RG5X                       0x85A
#define AON_FAST_LOP_POF_RG6X                       0x85C
#define AON_FAST_LOP_POF_RG7X                       0x85E
#define AON_FAST_LOP_POF_RG8X                       0x860
#define AON_FAST_LOP_POF_RG9X                       0x862
#define AON_FAST_LOP_POF_RG10X                      0x864
#define AON_FAST_LOP_POF_RG11X                      0x866
#define AON_FAST_LOP_POF_RG12X                      0x868
#define AON_FAST_LOP_POF_RG13X                      0x86A
#define AON_FAST_LOP_POF_RG14X                      0x86C
#define AON_FAST_LOP_POF_RG15X                      0x86E
#define AON_FAST_LOP_POF_RG16X                      0x870
#define AON_FAST_LOP_POF_RG17X                      0x872
#define AON_FAST_LOP_POF_RG18X                      0x874
#define AON_FAST_LOP_POF_RG19X                      0x876
#define AON_FAST_LOP_POF_RG20X                      0x878
#define AON_FAST_LOP_POF_RG21X                      0x87A
#define AON_FAST_LOP_POF_RG22X                      0x87C
#define AON_FAST_LOP_POF_RG23X                      0x87E
#define AON_FAST_LOP_POF_DELAY_RG0X                 0x880
#define AON_FAST_LOP_POF_DELAY_RG1X                 0x882
#define AON_FAST_LOP_POF_DELAY_RG2X                 0x884
#define AON_FAST_LOP_POF_DELAY_RG3X                 0x886
#define AON_FAST_LOP_POF_DELAY_RG4X                 0x888
#define AON_FAST_LOP_POF_DELAY_RG5X                 0x88A
#define AON_FAST_LOP_POF_MISC                       0x88C
#define AON_FAST_LOP_POF_DELAY_RG6X                 0x88E
#define AON_FAST_REG0X_WAIT_READY                   0x8A0
#define AON_FAST_REG1X_WAIT_READY                   0x8A2
#define AON_FAST_REG0X_WAIT_AON_CNT                 0x8A4
#define AON_FAST_REG1X_WAIT_AON_CNT                 0x8A6
#define AON_FAST_REG2X_WAIT_AON_CNT                 0x8A8
#define AON_FAST_REG3X_WAIT_AON_CNT                 0x8AA
#define AON_FAST_REG4X_WAIT_AON_CNT                 0x8AC
#define AON_FAST_REG5X_WAIT_AON_CNT                 0x8AE
#define AON_FAST_REG6X_WAIT_AON_CNT                 0x8B0
#define AON_FAST_REG7X_WAIT_AON_CNT                 0x8B2
#define AON_FAST_REG8X_WAIT_AON_CNT                 0x8B4
#define AON_FAST_REG9X_WAIT_AON_CNT                 0x8B6
#define AON_FAST_REG10X_WAIT_AON_CNT                0x8B8
#define AON_FAST_REG11X_WAIT_AON_CNT                0x8BA
#define AON_FAST_REG12X_WAIT_AON_CNT                0x8BC
#define AON_FAST_REG13X_WAIT_AON_CNT                0x8BE
#define AON_FAST_REG14X_WAIT_AON_CNT                0x8C0
#define AON_FAST_REG15X_WAIT_AON_CNT                0x8C2
#define AON_FAST_REG16X_WAIT_AON_CNT                0x8C4
#define AON_FAST_REG17X_WAIT_AON_CNT                0x8C6
#define AON_FAST_REG18X_WAIT_AON_CNT                0x8C8
#define AON_FAST_REG19X_WAIT_AON_CNT                0x8CA
#define AON_FAST_SET_WKEN_MISC                      0x8CC
#define AON_FAST_SET_WKEN_P1_P2                     0x8CE
#define AON_FAST_SET_WKEN_P3_P4                     0x8D0
#define AON_FAST_SET_WKEN_P5_P6                     0x8D2
#define AON_FAST_SET_WKEN_P7_32k                    0x8D4
#define AON_FAST_SET_WKEN_REG0X_HYBRID              0x8D6
#define AON_FAST_SET_WKEN_REG1X_HYBRID              0x8D8
#define AON_FAST_SET_WKPOL_MISC                     0x8DA
#define AON_FAST_SET_WKPOL_P1_P2                    0x8DC
#define AON_FAST_SET_WKPOL_P3_P4                    0x8DE
#define AON_FAST_SET_WKPOL_P5_P6                    0x8E0
#define AON_FAST_SET_WKPOL_P7_32k                   0x8E2
#define AON_FAST_SET_WKPOL_REG0X_HYBRID             0x8E4
#define AON_FAST_SET_WKPOL_REG1X_HYBRID             0x8E6
#define AON_FAST_RO_WK_REG0X                        0x8E8
#define AON_FAST_SET_SHIP_MODE                      0x8EA
#define AON_FAST_REG0X_LOAD_RELEASE_PROTECT         0x8EC
#define AON_FAST_REG1X_LOAD_RELEASE_PROTECT         0x8EE
#define AON_FAST_REG2X_LOAD_RELEASE_PROTECT         0x8F0
#define AON_FAST_REG3X_LOAD_RELEASE_PROTECT         0x8F2
#define AON_FAST_REG4X_LOAD_RELEASE_PROTECT         0x8F4
#define AON_FAST_REG5X_LOAD_RELEASE_PROTECT         0x8F6
#define AON_FAST_REG6X_LOAD_RELEASE_PROTECT         0x8F8
#define AON_FAST_REG7X_LOAD_RELEASE_PROTECT         0x8FA
#define AON_FAST_REG8X_LOAD_RELEASE_PROTECT         0x8FC
#define AON_FAST_REG9X_LOAD_RELEASE_PROTECT         0x8FE
#define AON_FAST_REG0X_PMUX_SEL_FSM_CTRL            0x900
#define AON_FAST_REG1X_PMUX_SEL_FSM_CTRL            0x902
#define AON_FAST_REG2X_PMUX_SEL_FSM_CTRL            0x904
#define AON_FAST_REG3X_PMUX_SEL_FSM_CTRL            0x906
#define AON_FAST_REG4X_PMUX_SEL_FSM_CTRL            0x908
#define AON_FAST_CORE_MODULE_REG0X_MUX_SEL          0x95A
#define AON_FAST_REG0X_MBIAS                        0xC00
#define AON_FAST_REG1X_MBIAS                        0xC02
#define AON_FAST_REG2X_MBIAS                        0xC04
#define AON_FAST_REG3X_MBIAS                        0xC06
#define AON_FAST_REG4X_MBIAS                        0xC08
#define AON_FAST_REG5X_MBIAS                        0xC0A
#define AON_FAST_REG6X_MBIAS                        0xC0C
#define AON_FAST_REG7X_MBIAS                        0xC0E
#define AON_FAST_REG8X_MBIAS                        0xC10
#define AON_FAST_REG9X_MBIAS                        0xC12
#define AON_FAST_REG10X_MBIAS                       0xC14
#define AON_FAST_REG11X_MBIAS                       0xC16
#define AON_FAST_REG12X_MBIAS                       0xC18
#define AON_FAST_REG13X_MBIAS                       0xC1A
#define AON_FAST_REG14X_MBIAS                       0xC1C
#define AON_FAST_FLAG0X_MBIAS                       0xC1E
#define AON_FAST_FLAG1X_MBIAS                       0xC20
#define AON_FAST_FLAG2X_MBIAS                       0xC22
#define AON_FAST_FLAG3X_MBIAS                       0xC24
#define AON_FAST_FLAG4X_MBIAS                       0xC26
#define AON_FAST_FLAG5X_MBIAS                       0xC28
#define AON_FAST_REG0X_MBIAS_OSCK                   0xC2A
#define AON_FAST_REG1X_MBIAS_OSCK                   0xC2C
#define AON_FAST_REG2X_MBIAS_OSCK                   0xC2E
#define AON_FAST_C_KOUT0X_MBIAS_OSCK                0xC30
#define AON_FAST_C_KOUT1X_MBIAS_OSCK                0xC32
#define AON_FAST_REG0X_LDOSYS                       0xC80
#define AON_FAST_REG1X_LDOSYS                       0xC82
#define AON_FAST_REG2X_LDOSYS                       0xC84
#define AON_FAST_REG3X_LDOSYS                       0xC86
#define AON_FAST_REG0X_LDOSYS_POS                   0xC88
#define AON_FAST_REG1X_LDOSYS_POS                   0xC8A
#define AON_FAST_REG2X_LDOSYS_POS                   0xC8C
#define AON_FAST_REG3X_LDOSYS_POS                   0xC8E
#define AON_FAST_REG4X_LDOSYS_POS                   0xC90
#define AON_FAST_REG5X_LDOSYS_POS                   0xC92
#define AON_FAST_REG6X_LDOSYS_POS                   0xC94
#define AON_FAST_REG7X_LDOSYS_POS                   0xC96
#define AON_FAST_C_KOUT0X_LDOSYS_POS                0xC98
#define AON_FAST_C_KOUT1X_LDOSYS_POS                0xC9A
#define AON_FAST_REG0X_LDOAUX3                      0xCE0
#define AON_FAST_REG1X_LDOAUX3                      0xCE2
#define AON_FAST_REG2X_LDOAUX3                      0xCE4
#define AON_FAST_REG0X_LDOAUX3_POS                  0xCE6
#define AON_FAST_REG1X_LDOAUX3_POS                  0xCE8
#define AON_FAST_REG2X_LDOAUX3_POS                  0xCEA
#define AON_FAST_REG3X_LDOAUX3_POS                  0xCEC
#define AON_FAST_REG4X_LDOAUX3_POS                  0xCEE
#define AON_FAST_REG5X_LDOAUX3_POS                  0xCF0
#define AON_FAST_REG6X_LDOAUX3_POS                  0xCF2
#define AON_FAST_REG7X_LDOAUX3_POS                  0xCF4
#define AON_FAST_C_KOUT0X_LDOAUX3_POS               0xCF6
#define AON_FAST_C_KOUT1X_LDOAUX3_POS               0xCF8
#define AON_FAST_REG3X_LDOAUX3_LQ                   0xD30
#define AON_FAST_REG0X_LDOAUX1                      0xD50
#define AON_FAST_REG1X_LDOAUX1                      0xD52
#define AON_FAST_FLAG0X_LDOAUX1                     0xD54
#define AON_FAST_REG0X_LDOAUX2                      0xD80
#define AON_FAST_REG1X_LDOAUX2                      0xD82
#define AON_FAST_FLAG0X_LDOAUX2                     0xD84
#define AON_FAST_REG0X_LDOUSB                       0xDB0
#define AON_FAST_REG1X_LDOUSB                       0xDB2
#define AON_FAST_FLAG0X_LDOUSB                      0xDB4
#define AON_FAST_REG0X_LDOEXT                       0xDE0
#define AON_FAST_REG1X_LDOEXT                       0xDE2
#define AON_FAST_REG2X_LDOEXT                       0xDE4
#define AON_FAST_REG0X_LDOEXT_POS                   0xDE6
#define AON_FAST_REG1X_LDOEXT_POS                   0xDE8
#define AON_FAST_REG2X_LDOEXT_POS                   0xDEA
#define AON_FAST_REG3X_LDOEXT_POS                   0xDEC
#define AON_FAST_REG4X_LDOEXT_POS                   0xDEE
#define AON_FAST_REG5X_LDOEXT_POS                   0xDF0
#define AON_FAST_REG6X_LDOEXT_POS                   0xDF2
#define AON_FAST_REG7X_LDOEXT_POS                   0xDF4
#define AON_FAST_C_KOUT0X_LDOEXT_POS                0xDF6
#define AON_FAST_C_KOUT1X_LDOEXT_POS                0xDF8
#define AON_FAST_REG0X_LDO_DIG                      0xE60
#define AON_FAST_REG0X_VCORE_PC                     0xEB0
#define AON_FAST_REG0X_CHG                          0x1000
#define AON_FAST_REG1X_CHG                          0x1002
#define AON_FAST_REG2X_CHG                          0x1004
#define AON_FAST_REG3X_CHG                          0x1006
#define AON_FAST_REG4X_CHG                          0x1008
#define AON_FAST_REG5X_CHG                          0x100A
#define AON_FAST_FLAG0X_CHG                         0x100C
#define AON_FAST_REG0X_SWR_CORE                     0x1040
#define AON_FAST_REG1X_SWR_CORE                     0x1042
#define AON_FAST_REG2X_SWR_CORE                     0x1044
#define AON_FAST_REG3X_SWR_CORE                     0x1046
#define AON_FAST_REG4X_SWR_CORE                     0x1048
#define AON_FAST_REG5X_SWR_CORE                     0x104A
#define AON_FAST_REG6X_SWR_CORE                     0x104C
#define AON_FAST_REG7X_SWR_CORE                     0x104E
#define AON_FAST_REG8X_SWR_CORE                     0x1050
#define AON_FAST_REG9X_SWR_CORE                     0x1052
#define AON_FAST_REG10X_SWR_CORE                    0x1054
#define AON_FAST_REG11X_SWR_CORE                    0x1056
#define AON_FAST_REG12X_SWR_CORE                    0x1058
#define AON_FAST_REG13X_SWR_CORE                    0x105A
#define AON_FAST_REG14X_SWR_CORE                    0x105C
#define AON_FAST_REG15X_SWR_CORE                    0x105E
#define AON_FAST_REG16X_SWR_CORE                    0x1060
#define AON_FAST_REG17X_SWR_CORE                    0x1062
#define AON_FAST_REG18X_SWR_CORE                    0x1064
#define AON_FAST_REG19X_SWR_CORE                    0x1066
#define AON_FAST_REG20X_SWR_CORE                    0x1068
#define AON_FAST_REG21X_SWR_CORE                    0x106A
#define AON_FAST_REG22X_SWR_CORE                    0x106C
#define AON_FAST_REG23X_SWR_CORE                    0x106E
#define AON_FAST_FLAG0X_SWR_CORE                    0x1070
#define AON_FAST_FLAG1X_SWR_CORE                    0x1072
#define AON_FAST_FLAG2X_SWR_CORE                    0x1074
#define AON_FAST_FLAG3X_SWR_CORE                    0x1076
#define AON_FAST_C_KOUT0X_SWR_CORE                  0x1078
#define AON_FAST_C_KOUT1X_SWR_CORE                  0x107A
#define AON_FAST_C_KOUT2X_SWR_CORE                  0x107C
#define AON_FAST_C_KOUT3X_SWR_CORE                  0x107E
#define AON_FAST_C_KOUT4X_SWR_CORE                  0x1080
#define AON_FAST_REG0X_SWR_CORE_PWM1POS             0x1082
#define AON_FAST_REG1X_SWR_CORE_PWM1POS             0x1084
#define AON_FAST_REG2X_SWR_CORE_PWM1POS             0x1086
#define AON_FAST_REG3X_SWR_CORE_PWM1POS             0x1088
#define AON_FAST_REG4X_SWR_CORE_PWM1POS             0x108A
#define AON_FAST_REG5X_SWR_CORE_PWM1POS             0x108C
#define AON_FAST_REG6X_SWR_CORE_PWM1POS             0x108E
#define AON_FAST_REG7X_SWR_CORE_PWM1POS             0x1090
#define AON_FAST_C_KOUT0X_SWR_CORE_PWM1POS          0x1092
#define AON_FAST_C_KOUT1X_SWR_CORE_PWM1POS          0x1094
#define AON_FAST_REG0X_SWR_CORE_PFMCCM1POS          0x1096
#define AON_FAST_REG1X_SWR_CORE_PFMCCM1POS          0x1098
#define AON_FAST_REG2X_SWR_CORE_PFMCCM1POS          0x109A
#define AON_FAST_REG3X_SWR_CORE_PFMCCM1POS          0x109C
#define AON_FAST_REG4X_SWR_CORE_PFMCCM1POS          0x109E
#define AON_FAST_REG5X_SWR_CORE_PFMCCM1POS          0x10A0
#define AON_FAST_REG6X_SWR_CORE_PFMCCM1POS          0x10A2
#define AON_FAST_REG7X_SWR_CORE_PFMCCM1POS          0x10A4
#define AON_FAST_C_KOUT0X_SWR_CORE_PFMCCM1POS       0x10A6
#define AON_FAST_C_KOUT1X_SWR_CORE_PFMCCM1POS       0x10A8
#define AON_FAST_REG0X_SWR_CORE_PFM1POS             0x10AA
#define AON_FAST_REG1X_SWR_CORE_PFM1POS             0x10AC
#define AON_FAST_REG2X_SWR_CORE_PFM1POS             0x10AE
#define AON_FAST_REG3X_SWR_CORE_PFM1POS             0x10B0
#define AON_FAST_REG4X_SWR_CORE_PFM1POS             0x10B2
#define AON_FAST_REG5X_SWR_CORE_PFM1POS             0x10B4
#define AON_FAST_REG6X_SWR_CORE_PFM1POS             0x10B6
#define AON_FAST_REG7X_SWR_CORE_PFM1POS             0x10B8
#define AON_FAST_C_KOUT0X_SWR_CORE_PFM1POS          0x10BA
#define AON_FAST_C_KOUT1X_SWR_CORE_PFM1POS          0x10BC
#define AON_FAST_REG0X_SWR_CORE_PFM2POS             0x10BE
#define AON_FAST_REG1X_SWR_CORE_PFM2POS             0x10C0
#define AON_FAST_REG2X_SWR_CORE_PFM2POS             0x10C2
#define AON_FAST_REG3X_SWR_CORE_PFM2POS             0x10C4
#define AON_FAST_REG4X_SWR_CORE_PFM2POS             0x10C6
#define AON_FAST_REG5X_SWR_CORE_PFM2POS             0x10C8
#define AON_FAST_REG6X_SWR_CORE_PFM2POS             0x10CA
#define AON_FAST_REG7X_SWR_CORE_PFM2POS             0x10CC
#define AON_FAST_C_KOUT0X_SWR_CORE_PFM2POS          0x10CE
#define AON_FAST_C_KOUT1X_SWR_CORE_PFM2POS          0x10D0
#define AON_FAST_REG0X_SWR_CORE_PFM300to600K        0x10D2
#define AON_FAST_REG1X_SWR_CORE_PFM300to600K        0x10D4
#define AON_FAST_REG2X_SWR_CORE_PFM300to600K        0x10D6
#define AON_FAST_REG3X_SWR_CORE_PFM300to600K        0x10D8
#define AON_FAST_REG4X_SWR_CORE_PFM300to600K        0x10DA
#define AON_FAST_REG5X_SWR_CORE_PFM300to600K        0x10DC
#define AON_FAST_C_KOUT0X_SWR_CORE_PFM300to600K     0x10DE
#define AON_FAST_C_KOUT1X_SWR_CORE_PFM300to600K     0x10E0
#define AON_FAST_REG0X_SWR_CORE_PFM300to600K2       0x10E2
#define AON_FAST_REG1X_SWR_CORE_PFM300to600K2       0x10E4
#define AON_FAST_REG2X_SWR_CORE_PFM300to600K2       0x10E6
#define AON_FAST_REG3X_SWR_CORE_PFM300to600K2       0x10E8
#define AON_FAST_REG4X_SWR_CORE_PFM300to600K2       0x10EA
#define AON_FAST_REG5X_SWR_CORE_PFM300to600K2       0x10EC
#define AON_FAST_C_KOUT0X_SWR_CORE_PFM300to600K2    0x10EE
#define AON_FAST_C_KOUT1X_SWR_CORE_PFM300to600K2    0x10F0
#define AON_FAST_REG0X_SWR_CORE_ZCDQ                0x10F2
#define AON_FAST_REG1X_SWR_CORE_ZCDQ                0x10F4
#define AON_FAST_C_KOUT0X_SWR_CORE_ZCDQ             0x10F6
#define AON_FAST_C_KOUT1X_SWR_CORE_ZCDQ             0x10F8
#define AON_FAST_REG0X_SWR_AUDIO                    0x11C0
#define AON_FAST_REG1X_SWR_AUDIO                    0x11C2
#define AON_FAST_REG2X_SWR_AUDIO                    0x11C4
#define AON_FAST_REG3X_SWR_AUDIO                    0x11C6
#define AON_FAST_REG4X_SWR_AUDIO                    0x11C8
#define AON_FAST_REG5X_SWR_AUDIO                    0x11CA
#define AON_FAST_REG6X_SWR_AUDIO                    0x11CC
#define AON_FAST_REG7X_SWR_AUDIO                    0x11CE
#define AON_FAST_REG8X_SWR_AUDIO                    0x11D0
#define AON_FAST_REG9X_SWR_AUDIO                    0x11D2
#define AON_FAST_REG10X_SWR_AUDIO                   0x11D4
#define AON_FAST_REG11X_SWR_AUDIO                   0x11D6
#define AON_FAST_REG12X_SWR_AUDIO                   0x11D8
#define AON_FAST_REG13X_SWR_AUDIO                   0x11DA
#define AON_FAST_REG14X_SWR_AUDIO                   0x11DC
#define AON_FAST_REG15X_SWR_AUDIO                   0x11DE
#define AON_FAST_REG16X_SWR_AUDIO                   0x11E0
#define AON_FAST_REG17X_SWR_AUDIO                   0x11E2
#define AON_FAST_REG18X_SWR_AUDIO                   0x11E4
#define AON_FAST_FLAG0X_SWR_AUDIO                   0x11E6
#define AON_FAST_FLAG1X_SWR_AUDIO                   0x11E8
#define AON_FAST_FLAG2X_SWR_AUDIO                   0x11EA
#define AON_FAST_FLAG3X_SWR_AUDIO                   0x11EC
#define AON_FAST_C_KOUT0X_SWR_AUDIO                 0x11EE
#define AON_FAST_C_KOUT1X_SWR_AUDIO                 0x11F0
#define AON_FAST_C_KOUT2X_SWR_AUDIO                 0x11F2
#define AON_FAST_C_KOUT3X_SWR_AUDIO                 0x11F4
#define AON_FAST_REG0X_SWR_AUDIO_PFMCCMPOS          0x11F6
#define AON_FAST_REG1X_SWR_AUDIO_PFMCCMPOS          0x11F8
#define AON_FAST_REG2X_SWR_AUDIO_PFMCCMPOS          0x11FA
#define AON_FAST_REG3X_SWR_AUDIO_PFMCCMPOS          0x11FC
#define AON_FAST_REG4X_SWR_AUDIO_PFMCCMPOS          0x11FE
#define AON_FAST_REG5X_SWR_AUDIO_PFMCCMPOS          0x1200
#define AON_FAST_REG6X_SWR_AUDIO_PFMCCMPOS          0x1202
#define AON_FAST_REG7X_SWR_AUDIO_PFMCCMPOS          0x1204
#define AON_FAST_C_KOUT0X_SWR_AUDIO_PFMCCMPOS       0x1206
#define AON_FAST_C_KOUT1X_SWR_AUDIO_PFMCCMPOS       0x1208
#define AON_FAST_REG0X_SWR_AUDIO_PFMPOS             0x120A
#define AON_FAST_REG1X_SWR_AUDIO_PFMPOS             0x120C
#define AON_FAST_REG2X_SWR_AUDIO_PFMPOS             0x120E
#define AON_FAST_REG3X_SWR_AUDIO_PFMPOS             0x1210
#define AON_FAST_REG4X_SWR_AUDIO_PFMPOS             0x1212
#define AON_FAST_REG5X_SWR_AUDIO_PFMPOS             0x1214
#define AON_FAST_REG6X_SWR_AUDIO_PFMPOS             0x1216
#define AON_FAST_REG7X_SWR_AUDIO_PFMPOS             0x1218
#define AON_FAST_C_KOUT0X_SWR_AUDIO_PFMPOS          0x121A
#define AON_FAST_C_KOUT1X_SWR_AUDIO_PFMPOS          0x121C
#define AON_FAST_REG0X_SWR_AUDIO_PFM300to600K       0x121E
#define AON_FAST_REG1X_SWR_AUDIO_PFM300to600K       0x1220
#define AON_FAST_REG2X_SWR_AUDIO_PFM300to600K       0x1222
#define AON_FAST_REG3X_SWR_AUDIO_PFM300to600K       0x1224
#define AON_FAST_REG4X_SWR_AUDIO_PFM300to600K       0x1226
#define AON_FAST_REG5X_SWR_AUDIO_PFM300to600K       0x1228
#define AON_FAST_C_KOUT0X_SWR_AUDIO_PFM300to600K    0x122A
#define AON_FAST_C_KOUT1X_SWR_AUDIO_PFM300to600K    0x122C
#define AON_FAST_REG0X_SWR_AUDIO_ZCDQ               0x122E
#define AON_FAST_REG1X_SWR_AUDIO_ZCDQ               0x1230
#define AON_FAST_C_KOUT0X_SWR_AUDIO_ZCDQ            0x1232
#define AON_FAST_C_KOUT1X_SWR_AUDIO_ZCDQ            0x1234
#define AON_FAST_BT_ANAPAR_PLL0                     0x1400
#define AON_FAST_BT_ANAPAR_PLL1                     0x1402
#define AON_FAST_BT_ANAPAR_PLL2                     0x1404
#define AON_FAST_BT_ANAPAR_PLL3                     0x1406
#define AON_FAST_BT_ANAPAR_PLL4                     0x1408
#define AON_FAST_BT_ANAPAR_PLL5                     0x140A
#define AON_FAST_BT_ANAPAR_PLL6                     0x140C
#define AON_FAST_BT_ANAPAR_PLL7                     0x140E
#define AON_FAST_BT_ANAPAR_PLL8                     0x1410
#define AON_FAST_BT_ANAPAR_PLL9                     0x1412
#define AON_FAST_BT_ANAPAR_PLL10                    0x1414
#define AON_FAST_BT_ANAPAR_PLL11                    0x1416
#define AON_FAST_BT_ANAPAR_PLL12                    0x1418
#define AON_FAST_BT_ANAPAR_PLL13                    0x141A
#define AON_FAST_BT_ANAPAR_PLL14                    0x141C
#define AON_FAST_BT_ANAPAR_PLL15                    0x141E
#define AON_FAST_BT_ANAPAR_PLL16                    0x1420
#define AON_FAST_BT_ANAPAR_PLL17                    0x1422
#define AON_FAST_BT_ANAPAR_PLL18                    0x1424
#define AON_FAST_BT_ANAPAR_PLL19                    0x1426
#define AON_FAST_BT_ANAPAR_PLL20                    0x1428
#define AON_FAST_BT_ANAPAR_PLL21                    0x142A
#define AON_FAST_BT_ANAPAR_PLL22                    0x142C
#define AON_FAST_BT_ANAPAR_PLL23                    0x142E
#define AON_FAST_BT_ANAPAR_PLL_LDO1                 0x1430
#define AON_FAST_BT_ANAPAR_PLL_LDO2                 0x1432
#define AON_FAST_RG0X_32KXTAL                       0x1470
#define AON_FAST_RG1X_32KXTAL                       0x1472
#define AON_FAST_RG2X_32KXTAL                       0x1474
#define AON_FAST_RG0X_32KOSC                        0x1476
#define AON_FAST_RG0X_POW_32K                       0x1478
#define AON_FAST_ANAPAR_XTAL_mode                   0x1490
#define AON_FAST_ANAPAR_XTAL0                       0x1492
#define AON_FAST_ANAPAR_XTAL1                       0x1494
#define AON_FAST_ANAPAR_XTAL2                       0x1496
#define AON_FAST_ANAPAR_XTAL3                       0x1498
#define AON_FAST_ANAPAR_XTAL4                       0x149A
#define AON_FAST_ANAPAR_XTAL5                       0x149C
#define AON_FAST_ANAPAR_XTAL6                       0x149E
#define AON_FAST_ANAPAR_XTAL7                       0x14A0
#define AON_FAST_ANAPAR_XTAL8                       0x14A2
#define AON_FAST_ANAPAR_XTAL9                       0x14A4
#define AON_FAST_ANAPAR_XTAL10                      0x14A6
#define AON_FAST_ANAPAR_XTAL11                      0x14A8
#define AON_FAST_ANAPAR_XTAL12                      0x14AA
#define AON_FAST_ANAPAR_XTAL13                      0x14AC
#define AON_FAST_ANAPAR_XTAL14                      0x14AE
#define AON_FAST_ANAPAR_XTAL15                      0x14B0
#define AON_FAST_RG0X_40MOSC                        0x14F0
#define AON_FAST_POW_40MOSC                         0x14F2
#define AON_FAST_RG0X_AUXADC                        0x1510
#define AON_FAST_RG1X_AUXADC                        0x1512
#define AON_FAST_RG2X_AUXADC                        0x1514
#define AON_FAST_RG0X_CODEC_LDO                     0x1530
#define AON_FAST_RG1X_CODEC_LDO                     0x1532
#define AON_FAST_RG2X_CODEC_LDO                     0x1534
#define AON_FAST_RG3X_CODEC_LDO                     0x1536
#define AON_FAST_REG0X_BTADDA_LDO                   0x1540
#define AON_FAST_LDOPA_REG3X_LDO                    0x1550
#define AON_FAST_LDOPA_DIG_VRSEL_LDODIG             0x1552

/* 0x0
    00      R/W bootup_before                                   1'b0
    01      R/W need_restore                                    1'b0
    02      R/W dvfs_normal_vdd_mode                            1'b0
    04:03   R/W dvfs_low_vdd_mode                               2'b0
    15:05   R/W reserved                                        11'b0
 */
typedef union _AON_FAST_0x0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t bootup_before:         1;  /* bit[0]: 0: first boot up                     */
        uint16_t need_restore:          1;  /* bit[1]: need restore flow                    */
        uint16_t dvfs_normal_vdd_mode:  1;  /* bit[2]: reference to DVFSNormalVDDMode       */
        uint16_t dvfs_low_vdd_mode:     2;  /* bit[4:3]: reference to DVFSLowVDDMode        */
        uint16_t rsvd:                  11;
    };
} AON_FAST_0x0_TYPE;

/* 0x1
    03:00   R/W efuse_invalid_cnt                               4'b0
    07:04   R/W flash_invalid_cnt                               4'b0
    15:08   R/W RSVD                                            8'b0
 */
typedef union _AON_FAST_0x1_TYPE
{
    uint8_t d8;
    struct
    {
        uint8_t rsvd:                      8;
    };
} AON_FAST_0x1_TYPE;

/* 0x2
    0       R/W is_monitor_mode                                 1'b0
    1       R/W is_rom_code_patch                               1'b0
    2       R/W is_fw_trig_wdg_to                               1'b0
    3       R/W is_h5_link_reset                                1'b0
    4       R/W is_send_patch_end_evt                           1'b0
    5       R/W ota_mode                                        1'b0
    6       R/W is_ft_mode                                      1'b0
    7       R/W is_airplane_mode                                1'b0
    15:08   R/W RSVD                                            8'b0
 */
typedef union _AON_FAST_0x2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t is_monitor_mode: 1;
        uint16_t is_rom_code_patch: 1;
        uint16_t is_fw_trig_wdg_to: 1;
        uint16_t is_h5_link_reset: 1;
        uint16_t is_send_patch_end_evt: 1;
        uint16_t ota_mode: 1;
        uint16_t is_ft_mode: 1;
        uint16_t is_airplane_mode: 1;
        uint16_t wdg_reset_reason: 8;
    };
} AON_FAST_0x2_TYPE;

/* 0x6
    15:00   R/W freg3                                           16'h0
 */
typedef union _AON_FAST_0x6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg3;
    };
} AON_FAST_0x6_TYPE;

/* 0x8
    15:00   R/W freg4                                           16'h0
 */
typedef union _AON_FAST_0x8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg4;
    };
} AON_FAST_0x8_TYPE;

/* 0xA
    15:00   R/W freg5                                           16'h0
 */
typedef union _AON_FAST_0xA_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg5;
    };
} AON_FAST_0xA_TYPE;

/* 0xC
    15:00   R/W freg6                                           16'h0
 */
typedef union _AON_FAST_0xC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg6;
    };
} AON_FAST_0xC_TYPE;

/* 0xE
    15:00   R/W freg7                                           16'h0
 */
typedef union _AON_FAST_0xE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg7;
    };
} AON_FAST_0xE_TYPE;

/* 0x10
    15:00   R/W freg8                                           16'h0
 */
typedef union _AON_FAST_0x10_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg8;
    };
} AON_FAST_0x10_TYPE;

/* 0x12
    15:00   R/W freg9                                           16'h0
 */
typedef union _AON_FAST_0x12_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg9;
    };
} AON_FAST_0x12_TYPE;

/* 0x15 */
typedef union
{
    uint8_t d8;
    struct
    {
        uint8_t is_hci_mode:              1;  /* bit[0]: switch to hci mode                 */
        uint8_t is_single_tone_mode:      1;  /* bit[1]: switch to single tone test mode    */
        uint8_t is_password_hci_cmd:      1;  /* bit[2]: 1: hci cmd for entering password     */
        uint8_t rsvd4:                    4;  /* bit[7:3]: reserved                         */
        uint8_t is_single_ota_bank_under_dual_layout: 1;  /* bit[7]: force ota copy flag   */
    };
} AON_FAST_0x15_TYPE;

/* 0x14
    15:00   R/W freg10                                          16'h0
 */
typedef union _AON_FAST_0x14_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t otp_invalid_cnt:           4;
        uint16_t flash_invalid_cnt:         4;
        uint16_t is_hci_mode:               1;
        uint16_t is_single_tone_mode:       1;
        uint16_t is_password_hci_cmd:       1;
        uint16_t anc_measure_mode:          2;
        uint16_t rsvd2:                     2;
        uint16_t is_single_ota_bank_under_dual_layout: 1;
    };
} AON_FAST_0x14_TYPE;

/* 0x16
    15:00   R/W freg11                                          16'h0
 */
typedef union _AON_FAST_0x16_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t freg11;
    };
} AON_FAST_0x16_TYPE;

/* 0x18
    15:00   R/W DUMMY                                           16'b0
 */
typedef union _AON_FAST_0x18_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DUMMY;
    };
} AON_FAST_0x18_TYPE;

/* 0x1A
    05:00   R/W RCAL[5:0]                                       6'b100000
    10:06   R/W XTAL32K_Reserved15[10:6]                        5'b00000
    11:11   W1O reg_aon_w1o_clk_flash_dis                       1'b0
    15:12   W1O XTAL32K_Reserved15[14:12]                       4'b0000
 */
typedef union _AON_FAST_0x1A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RCAL_5_0: 6;
        uint16_t XTAL32K_Reserved15: 5;
        uint16_t reg_aon_flash_clk_dis: 1;
        uint16_t is_otp_invalid: 1;
        uint16_t is_enable_efuse_read_protect: 1;
        uint16_t is_enable_efuse_write_protect: 1;
        uint16_t XTAL32K_Reserved15_1: 1;
    };
} AON_FAST_0x1A_TYPE;

/* 0x1C
    15:00   R/W DUMMY                                           16'b0
 */
typedef union _AON_FAST_0x1C_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DUMMY;
    };
} AON_FAST_0x1C_TYPE;

/* 0x1E
    15:00   R/W DUMMY                                           16'b0
 */
typedef union _AON_FAST_0x1E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DUMMY;
    };
} AON_FAST_0x1E_TYPE;

/* 0x20
    0       R   POW_32KXTAL                                     1'b1
    1       R   POW_32KOSC                                      1'b1
    2       W1O reg_dsp_flash_prot                              1'b0
    3       W1O reg_aon_hwspi_en_rp                             1'b0
    4       R/W reg_aon_hwspi_en                                1'b0
    5       W1O reg_aon_debug_port_wp                           1'b0
    6       R/W reg_aon_debug_port                              1'b0
    7       W1O reg_aon_dbg_boot_dis                            1'b0
    15:08   W1O XTAL32K_Reserved16[13:6]                        8'h0
 */
typedef union _AON_FAST_0x20_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t POW_32KXTAL: 1;
        uint16_t POW_32KOSC: 1;
        uint16_t reg_aon_dsp_flash_port_dis: 1;
        uint16_t reg_aon_hwspi_en_wp: 1;
        uint16_t reg_aon_hwspi_en: 1;
        uint16_t reg_aon_debug_port_wp: 1;
        uint16_t reg_aon_dbg_port: 1;
        uint16_t reg_aon_dbg_boot_dis: 1;
        uint16_t is_disable_hci_ram_patch: 1;
        uint16_t is_disable_hci_flash_access: 1;
        uint16_t is_disable_hci_system_access: 1;
        uint16_t is_disable_hci_efuse_access: 1;
        uint16_t is_disable_hci_mac_rf_access: 1;
        uint16_t is_disable_usb_function: 1;
        uint16_t is_disable_sdio_function: 1;
        uint16_t is_debug_password_invalid: 1;
    };
} AON_FAST_0x20_TYPE;

/* 0x22
    15:00   R/W DUMMY                                           16'b0
 */
typedef union _AON_FAST_0x22_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DUMMY;
    };
} AON_FAST_0x22_TYPE;

/* 0x24
    15:00   R/W DUMMY                                           16'b0
 */
typedef union _AON_FAST_0x24_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DUMMY;
    };
} AON_FAST_0x24_TYPE;

/* 0x26
    03:00   R/W DUMMY0                                          4'b0
    4       R   AUXADC_POW_SD                                   1'b0
    5       R   AUXADC_SD_POSEDGE                               1'b0
    08:06   R   AUXADC_ch_num_sd[2:0]                           3'b0
    15:09   R/W DUMMY1                                          7'b0
 */
typedef union _AON_FAST_0x26_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DUMMY0: 4;
        uint16_t AUXADC_POW_SD: 1;
        uint16_t AUXADC_SD_POSEDGE: 1;
        uint16_t AUXADC_ch_num_sd_2_0: 3;
        uint16_t DUMMY1: 7;
    };
} AON_FAST_0x26_TYPE;

/* 0x28
    07:00   R   AUXADC_E[7:0]                                   8'b00000000
    15:08   R   AUXADC_E2[7:0]                                  8'b11111111
 */
typedef union _AON_FAST_0x28_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AUXADC_E_7_0: 8;
        uint16_t AUXADC_E2_7_0: 8;
    };
} AON_FAST_0x28_TYPE;

/* 0x2A
    07:00   R   AUXADC_PD[7:0]                                  8'b00000000
    15:08   R   AUXADC_PU[7:0]                                  8'b00000000
 */
typedef union _AON_FAST_0x2A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AUXADC_PD_7_0: 8;
        uint16_t AUXADC_PU_7_0: 8;
    };
} AON_FAST_0x2A_TYPE;

/* 0x2C
    07:00   R   AUXADC_SHDN[7:0]                                8'b00000000
    15:08   R   AUXADC_SMT[7:0]                                 8'b00000000
 */
typedef union _AON_FAST_0x2C_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AUXADC_SHDN_7_0: 8;
        uint16_t AUXADC_SMT_7_0: 8;
    };
} AON_FAST_0x2C_TYPE;

/* 0x2E
    07:00   R   AUXADC_I[7:0]                                   8'b00000000
    15:08   R/W AUXADC_PUPDC[7:0]                               8'b00000000
 */
typedef union _AON_FAST_0x2E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AUXADC_I_7_0: 8;
        uint16_t AUXADC_PUPDC_7_0: 8;
    };
} AON_FAST_0x2E_TYPE;

/* 0x46
    15:00   R/W DUMMY                                           16'b0
 */
typedef union _AON_FAST_0x46_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DUMMY;
    };
} AON_FAST_0x46_TYPE;

/* 0xA8
    02:00   R/W XTAL_FREQ_SEL                                   3'h5
    3       R/W AAC_SEL                                         1'b0
    09:04   R/W AAC_GM1                                         6'h1f
    15:10   R/W AAC_GM                                          6'h1f
 */
typedef union _AON_FAST_0xA8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_FREQ_SEL: 3;
        uint16_t AAC_SEL: 1;
        uint16_t AAC_GM1: 6;
        uint16_t AAC_GM: 6;
    };
} AON_FAST_0xA8_TYPE;

/* 0xAA
    08:00   R/W DUMMY                                           9'b0
    9       R/W U11_POW_BC_V33                                  1'b0
    10      R/W U11_POW_USB_V33                                 1'b0
    11      R/W U11_en_pc_mode                                  1'b0
    12      R/W DUMMY1                                          1'b0
    13      R/W U11_POW_BC_L                                    1'b0
    14      R/W ISO_UA2USB                                      1'b1
    15      R/W RCAL_32K_SEL                                    1'b0
 */
typedef union _AON_FAST_0xAA_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DUMMY: 9;
        uint16_t U11_POW_BC_V33: 1;
        uint16_t U11_POW_USB_V33: 1;
        uint16_t U11_en_pc_mode: 1;
        uint16_t DUMMY1: 1;
        uint16_t U11_POW_BC_L: 1;
        uint16_t ISO_UA2USB: 1;
        uint16_t RCAL_32K_SEL: 1;
    };
} AON_FAST_0xAA_TYPE;

/* 0xAE
    15:12   R/W XTAL_BUF_GMN[3:0]                               4'b1000
    11      R/W XTAL_BK_BG                                      1'b0
    10:09   R/W XTAL_AAC_PK_SEL[1:0]                            2'b01
    08:05   R/W XTAL_AAC_PK_LP_SEL[3:0]                         4'b0101
    4       R/W XTAL_AAC_PK_SEL_MSB                             1'b1
    3       R/W GATED_XTAL_OK0                                  1'b0
    2       R/W EN_XTAL_LPS                                     1'b0
    1       R/W EN_XTAL_AAC_PKDET                               1'b0
    0       R/W EN_XTAL_AAC_GM                                  1'b0
 */
typedef union _AON_FAST_0xAE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_BUF_GMN_3_0: 4;
        uint16_t XTAL_BK_BG: 1;
        uint16_t XTAL_AAC_PK_SEL_1_0: 2;
        uint16_t XTAL_AAC_PK_LP_SEL_3_0: 4;
        uint16_t XTAL_AAC_PK_SEL_MSB: 1;
        uint16_t GATED_XTAL_OK0: 1;
        uint16_t EN_XTAL_LPS: 1;
        uint16_t EN_XTAL_AAC_PKDET: 1;
        uint16_t EN_XTAL_AAC_GM: 1;
    };
} AON_FAST_0xAE_TYPE;

/* 0xC6
    15:00   R/W DUMMY                                           16'h0000
 */
typedef union _AON_FAST_0xC6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DUMMY;
    };
} AON_FAST_0xC6_TYPE;

/* 0xCC
    15:00   R/W DUMMY                                           16'h0784
 */
typedef union _AON_FAST_0xCC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DUMMY;
    };
} AON_FAST_0xCC_TYPE;

/* 0xCE
    15:00   R/W DUMMY                                           16'h0100
 */
typedef union _AON_FAST_0xCE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DUMMY;
    };
} AON_FAST_0xCE_TYPE;

/* 0xEA
    00:00   R/W MCLK2_EN                                        1'b0
    01:01   R/W MCLK2_DIV_SEL                                   1'b0
    03:02   R   RSVD                                            2'h0
    07:04   R/W MCLK2_SRC_SEL                                   4'h0
    15:08   R/W MCLK2_DIV_SETTING                               8'h0
 */
typedef union _AON_FAST_0xEA_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MCLK2_EN: 1;
        uint16_t MCLK2_DIV_SEL: 1;
        uint16_t RSVD: 2;
        uint16_t MCLK2_SRC_SEL: 4;
        uint16_t MCLK2_DIV_SETTING: 8;
    };
} AON_FAST_0xEA_TYPE;

/* 0xEC
    00:00   R/W MCLK3_EN                                        1'b0
    02:01   R/W MCLK3_DIV_SEL                                   2'b0
    03:03   R   RSVD0                                           2'h0
    07:04   R/W MCLK3_SRC_SEL                                   3'h0
    15:08   R   RSVD1                                           9'h0
 */
typedef union _AON_FAST_0xEC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MCLK3_EN: 1;
        uint16_t MCLK3_DIV_SEL: 2;
        uint16_t RSVD0: 1;
        uint16_t MCLK3_SRC_SEL: 4;
        uint16_t RSVD1: 8;
    };
} AON_FAST_0xEC_TYPE;

/* 0xEE
    01:00   R   RSVD0                                           2'b0
    02:02   R   sel_swr_ss_top                                  1'b0
    03:03   R/W usb_wakeup_sel                                  1'b1
    09:04   R   swr_ss_lpf_out                                  12'h0
    11:10   R   RSVD1                                           2'b0
    12:12   R   reg_aon_wdt_reset                               1'b0
    13:13   R   RSVD2                                           1'b0
    14:14   R/W r_aon_cko2_mux_cg_en                            1'b1
    15:15   R/W r_aon_cko2_mux_src_sel                          1'b0
 */
typedef union _AON_FAST_0xEE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD0: 2;
        uint16_t sel_swr_ss_top: 1;
        uint16_t usb_wakeup_sel: 1;
        uint16_t swr_ss_lpf_out: 6;
        uint16_t RSVD1: 2;
        uint16_t reg_aon_wdt_reset: 1;
        uint16_t RSVD2: 1;
        uint16_t r_aon_cko2_mux_cg_en: 1;
        uint16_t r_aon_cko2_mux_src_sel: 1;
    };
} AON_FAST_0xEE_TYPE;

/* 0xF0
    15:00   R/W DUMMY                                           16'h0
 */
typedef union _AON_FAST_0xF0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DUMMY: 16;
    };
} AON_FAST_0xF0_TYPE;

/* 0xF4
    0       R/W SEL_32K_XTAL                                    1'b0
    1       R/W SEL_32K_LP                                      1'b0
    2       R/W BRWN_OUT_GATING                                 1'b0
    3       R/W BRWN_OUT_RESET                                  1'b0
    4       R/W WAIT_HV_DET                                     1'b0
    06:05   R/W DUMMY0                                          2'b0
    7       R   SWR_BY_CORE                                     1'b0
    09:08   R/W DUMMY1                                          2'b11
    10      R   RSVD
    15:11   R/W AON_DBG_SEL                                     5'h0
 */
typedef union _AON_FAST_0xF4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SEL_32K_XTAL: 1;
        uint16_t SEL_32K_LP: 1;
        uint16_t BRWN_OUT_GATING: 1;
        uint16_t BRWN_OUT_RESET: 1;
        uint16_t WAIT_HV_DET: 1;
        uint16_t DUMMY0: 2;
        uint16_t SWR_BY_CORE: 1;
        uint16_t DUMMY1: 2;
        uint16_t RSVD: 1;
        uint16_t AON_DBG_SEL: 5;
    };
} AON_FAST_0xF4_TYPE;

/* 0xF6
    03:00   R/W AON_DBG_SWR_CORE_SEL                            4'b0
    06:04   R/W AON_DBG_SWR_AUDIO_SEL                           3'b0
    09:07   R/W AON_DBG_MBIAS_MISC_SEL                          3'b0
    15:10   R/W AON_DBG_DUMMY                                   6'h0
 */
typedef union _AON_FAST_0xF6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_DBG_SWR_CORE_SEL: 4;
        uint16_t AON_DBG_SWR_AUDIO_SEL: 3;
        uint16_t AON_DBG_MBIAS_MISC_SEL: 3;
        uint16_t AON_DBG_DUMMY: 6;
    };
} AON_FAST_0xF6_TYPE;

/* 0x122
    01:00   R   PAD_P6_STS                                      6'h3F
    07:02   W1C PAD_P6_STS                                      2'b11
    08:09   R   PAD_P7_STS                                      6'h3F
    15:10   W1C PAD_P7_STS                                      2'b11
 */
typedef union _AON_FAST_0x122_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P6_STS: 8;
        uint16_t PAD_P7_STS: 8;
    };
} AON_FAST_0x122_TYPE;

/* 0x124
    02:00   R   RSVD0                                           3'b0
    3       R/W PAD_SPI_FLASH_SHDN                              1'b0
    4       R/W AON_PAD_SPI_FLASH_S                             1'b1
    07:05   R   RSVD1                                           3'b0
    08:08   W1C AONWDT_STS                                      1'b1
    09:09   W1C COREWDT_STS                                     1'b1
    10:10   W1C GPIO_STS                                         1'b1
    11:11   W1C RTC_STS                                         1'b1
    12:12   W1C RIF_STS                                         1'b1
    13:10   R   RSVD2                                           6'h0
 */
typedef union _AON_FAST_0x124_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD0: 3;
        uint16_t PAD_SPI_FLASH_SHDN: 1;
        uint16_t AON_PAD_SPI_FLASH_S: 1;
        uint16_t RSVD1: 3;
        uint16_t AONWDT_STS: 1;
        uint16_t COREWDT_STS: 1;
        uint16_t GPIO_STS: 1;
        uint16_t RTC_STS: 1;
        uint16_t RIF_STS: 1;
        uint16_t RSVD2: 3;
    };
} AON_FAST_0x124_TYPE;

/* 0x126
    0       W1C PAD_P7_STS[0]                                   1'b1
    1       W1C PAD_P7_STS[1]                                   1'b1
    2       W1C PAD_P6_STS[0]                                   1'b1
    3       W1C PAD_P6_STS[1]                                   1'b1
    4       W1C PAD_32KXI_STS                                   1'b1
    5       W1C PAD_32KXO_STS                                   1'b1
    6       W1C USB_STS                                         1'b1
    7       W1C MFB_STS                                         1'b1
    8       W1C BAT_STS                                         1'b1
    9       W1C ADP_STS                                         1'b1
    10      R/W PAD_BONDING_SHDN                                1'b1
    11      R/W PAD_SPI_FLASH_H3L1                              1'b1
    12      W1C VAD_STS                                         1'b1
    13      W1C VADBUF_STS                                      1'b1
    14      W1C ADSP_STS                                        1'b1
    15      R   RSVD                                            2'b0
 */
typedef union _AON_FAST_0x126_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P7_STS_0: 1;
        uint16_t PAD_P7_STS_1: 1;
        uint16_t PAD_P6_STS_0: 1;
        uint16_t PAD_P6_STS_1: 1;
        uint16_t PAD_32KXI_STS: 1;
        uint16_t PAD_32KXO_STS: 1;
        uint16_t USB_STS: 1;
        uint16_t MFB_STS: 1;
        uint16_t BAT_STS: 1;
        uint16_t ADP_STS: 1;
        uint16_t PAD_BONDING_SHDN: 1;
        uint16_t PAD_SPI_FLASH_H3L1: 1;
        uint16_t VAD_STS: 1;
        uint16_t VADBUF_STS: 1;
        uint16_t ADSP_STS: 1;
        uint16_t RSVD: 1;
    };
} AON_FAST_0x126_TYPE;

/* 0x128
    0       W1C PAD_P4_STS[0]                                   1'b1
    1       W1C PAD_P4_STS[1]                                   1'b1
    2       W1C PAD_P4_STS[2]                                   1'b1
    3       W1C PAD_P4_STS[3]                                   1'b1
    4       W1C PAD_P4_STS[4]                                   1'b1
    5       W1C PAD_P4_STS[5]                                   1'b1
    6       W1C PAD_P4_STS[6]                                   1'b1
    7       W1C PAD_P4_STS[7]                                   1'b1
    8       W1C PAD_P3_STS[0]                                   1'b1
    9       W1C PAD_P3_STS[1]                                   1'b1
    10      W1C PAD_P3_STS[2]                                   1'b1
    11      W1C PAD_P3_STS[3]                                   1'b1
    12      W1C PAD_P3_STS[4]                                   1'b1
    13      W1C PAD_P3_STS[5]                                   1'b1
    14      W1C PAD_P3_STS[6]                                   1'b1
    15      W1C PAD_P3_STS[7]                                   1'b1
 */
typedef union _AON_FAST_0x128_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P4_STS_0: 1;
        uint16_t PAD_P4_STS_1: 1;
        uint16_t PAD_P4_STS_2: 1;
        uint16_t PAD_P4_STS_3: 1;
        uint16_t PAD_P4_STS_4: 1;
        uint16_t PAD_P4_STS_5: 1;
        uint16_t PAD_P4_STS_6: 1;
        uint16_t PAD_P4_STS_7: 1;
        uint16_t PAD_P3_STS_0: 1;
        uint16_t PAD_P3_STS_1: 1;
        uint16_t PAD_P3_STS_2: 1;
        uint16_t PAD_P3_STS_3: 1;
        uint16_t PAD_P3_STS_4: 1;
        uint16_t PAD_P3_STS_5: 1;
        uint16_t PAD_P3_STS_6: 1;
        uint16_t PAD_P3_STS_7: 1;
    };
} AON_FAST_0x128_TYPE;

/* 0x12A
    08:00   R/W DUMMY                                           9'b000000011
    9       R   RSVD                                            1'b0
    10      R/W MFB_WKPOL                                       1'b0
    11      R/W MFB_WKEN                                        1'b0
    12      R/W BAT_WKPOL                                       1'b0
    13      R/W BAT_WKEN                                        1'b0
    14      R/W ADP_WKPOL                                       1'b0
    15      R/W ADP_WKEN                                        1'b0
 */
typedef union _AON_FAST_0x12A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DUMMY: 9;
        uint16_t RSVD: 1;
        uint16_t MFB_WKPOL: 1;
        uint16_t MFB_WKEN: 1;
        uint16_t BAT_WKPOL: 1;
        uint16_t BAT_WKEN: 1;
        uint16_t ADP_WKPOL: 1;
        uint16_t ADP_WKEN: 1;
    };
} AON_FAST_0x12A_TYPE;

/* 0x12C
    05:00   R   RSVD0                                           6'h0
    6       R/W brwn_int_en                                     1'b0
    7       R/W brwn_int_pol                                    1'b1
    8       W1C brwn_det_intr                                   1'b1
    9       R/W wakeup_blk_ind                                  1'b1
    10      R   PAD_P2_0_I                                      1'b0
    11      R   PAD_BOOT_FROM_FLASH_I                           1'b0
    15:12   R   RSVD1                                           4'h0
 */
typedef union _AON_FAST_0x12C_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD0: 6;
        uint16_t brwn_int_en: 1;
        uint16_t brwn_int_pol: 1;
        uint16_t brwn_det_intr: 1;
        uint16_t wakeup_blk_ind: 1;
        uint16_t PAD_P2_0_I: 1;
        uint16_t PAD_BOOT_FROM_FLASH_I: 1;
        uint16_t RSVD1: 4;
    };
} AON_FAST_0x12C_TYPE;

/* 0x12E
    0       R   RSVD                                            1'b0
    1       W1C PAD_MIC4_N_STS                                  1'b1
    2       W1C PAD_MIC4_P_STS                                  1'b1
    3       W1C PAD_MIC3_N_STS                                  1'b1
    4       W1C PAD_MIC3_P_STS                                  1'b1
    5       W1C PAD_MICBIAS_STS                                 1'b1
    6       W1C PAD_AUX_R_STS                                   1'b1
    7       W1C PAD_AUX_L_STS                                   1'b1
    8       W1C PAD_ROUT_N_STS                                  1'b1
    9       W1C PAD_ROUT_P_STS                                  1'b1
    10      W1C PAD_LOUT_N_STS                                  1'b1
    11      W1C PAD_LOUT_P_STS                                  1'b1
    12      W1C PAD_MIC2_N_STS                                  1'b1
    13      W1C PAD_MIC2_P_STS                                  1'b1
    14      W1C PAD_MIC1_N_STS                                  1'b1
    15      W1C PAD_MIC1_P_STS                                  1'b1
 */
typedef union _AON_FAST_0x12E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD: 1;
        uint16_t PAD_MIC4_N_STS: 1;
        uint16_t PAD_MIC4_P_STS: 1;
        uint16_t PAD_MIC3_N_STS: 1;
        uint16_t PAD_MIC3_P_STS: 1;
        uint16_t PAD_MICBIAS_STS: 1;
        uint16_t PAD_AUX_R_STS: 1;
        uint16_t PAD_AUX_L_STS: 1;
        uint16_t PAD_ROUT_N_STS: 1;
        uint16_t PAD_ROUT_P_STS: 1;
        uint16_t PAD_LOUT_N_STS: 1;
        uint16_t PAD_LOUT_P_STS: 1;
        uint16_t PAD_MIC2_N_STS: 1;
        uint16_t PAD_MIC2_P_STS: 1;
        uint16_t PAD_MIC1_N_STS: 1;
        uint16_t PAD_MIC1_P_STS: 1;
    };
} AON_FAST_0x12E_TYPE;

/* 0x130
    0       W1C PAD_P2_STS[0]                                   1'b1
    1       W1C PAD_P2_STS[1]                                   1'b1
    2       W1C PAD_P2_STS[2]                                   1'b1
    3       W1C PAD_P2_STS[3]                                   1'b1
    4       W1C PAD_P2_STS[4]                                   1'b1
    5       W1C PAD_P2_STS[5]                                   1'b1
    6       W1C PAD_P2_STS[6]                                   1'b1
    7       W1C PAD_P2_STS[7]                                   1'b1
    8       W1C PAD_P1_STS[0]                                   1'b1
    9       W1C PAD_P1_STS[1]                                   1'b1
    10      W1C PAD_P1_STS[2]                                   1'b1
    11      W1C PAD_P1_STS[3]                                   1'b1
    12      W1C PAD_P1_STS[4]                                   1'b1
    13      W1C PAD_P1_STS[5]                                   1'b1
    14      W1C PAD_P1_STS[6]                                   1'b1
    15      W1C PAD_P1_STS[7]                                   1'b1
 */
typedef union _AON_FAST_0x130_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P2_STS_0: 1;
        uint16_t PAD_P2_STS_1: 1;
        uint16_t PAD_P2_STS_2: 1;
        uint16_t PAD_P2_STS_3: 1;
        uint16_t PAD_P2_STS_4: 1;
        uint16_t PAD_P2_STS_5: 1;
        uint16_t PAD_P2_STS_6: 1;
        uint16_t PAD_P2_STS_7: 1;
        uint16_t PAD_P1_STS_0: 1;
        uint16_t PAD_P1_STS_1: 1;
        uint16_t PAD_P1_STS_2: 1;
        uint16_t PAD_P1_STS_3: 1;
        uint16_t PAD_P1_STS_4: 1;
        uint16_t PAD_P1_STS_5: 1;
        uint16_t PAD_P1_STS_6: 1;
        uint16_t PAD_P1_STS_7: 1;
    };
} AON_FAST_0x130_TYPE;

/* 0x132
    0       W1C PAD_P5_STS[0]                                   1'b1
    1       W1C PAD_P5_STS[1]                                   1'b1
    2       W1C PAD_P5_STS[2]                                   1'b1
    3       W1C PAD_P5_STS[3]                                   1'b1
    4       W1C PAD_P5_STS[4]                                   1'b1
    5       W1C PAD_P5_STS[5]                                   1'b1
    6       W1C PAD_P5_STS[6]                                   1'b1
    7       W1C PAD_P5_STS[7]                                   1'b1
    8       W1C PAD_ADC_STS[0]                                  1'b1
    9       W1C PAD_ADC_STS[1]                                  1'b1
    10      W1C PAD_ADC_STS[2]                                  1'b1
    11      W1C PAD_ADC_STS[3]                                  1'b1
    12      W1C PAD_ADC_STS[4]                                  1'b1
    13      W1C PAD_ADC_STS[5]                                  1'b1
    14      W1C PAD_ADC_STS[6]                                  1'b1
    15      W1C PAD_ADC_STS[7]                                  1'b1
 */
typedef union _AON_FAST_0x132_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P5_STS_0: 1;
        uint16_t PAD_P5_STS_1: 1;
        uint16_t PAD_P5_STS_2: 1;
        uint16_t PAD_P5_STS_3: 1;
        uint16_t PAD_P5_STS_4: 1;
        uint16_t PAD_P5_STS_5: 1;
        uint16_t PAD_P5_STS_6: 1;
        uint16_t PAD_P5_STS_7: 1;
        uint16_t PAD_ADC_STS_0: 1;
        uint16_t PAD_ADC_STS_1: 1;
        uint16_t PAD_ADC_STS_2: 1;
        uint16_t PAD_ADC_STS_3: 1;
        uint16_t PAD_ADC_STS_4: 1;
        uint16_t PAD_ADC_STS_5: 1;
        uint16_t PAD_ADC_STS_6: 1;
        uint16_t PAD_ADC_STS_7: 1;
    };
} AON_FAST_0x132_TYPE;

/* 0x13A
    06:00   R   RSVD                                            7'h0
    7       R/W PAD_SPI_FLASH_PDPUC                             1'b0
    8       R/W PAD_SPI_FLASH_E5N                               1'b0
    9       R/W PAD_SPI_FLASH_E4N                               1'b0
    10      R/W PAD_SPI_FLASH_E3N                               1'b0
    11      R/W PAD_SPI_FLASH_E2N                               1'b0
    12      R/W PAD_SPI_FLASH_E5P                               1'b0
    13      R/W PAD_SPI_FLASH_E4P                               1'b0
    14      R/W PAD_SPI_FLASH_E3P                               1'b0
    15      R/W PAD_SPI_FLASH_E2P                               1'b0
 */
typedef union _AON_FAST_0x13A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD: 7;
        uint16_t PAD_SPI_FLASH_PDPUC: 1;
        uint16_t PAD_SPI_FLASH_E5N: 1;
        uint16_t PAD_SPI_FLASH_E4N: 1;
        uint16_t PAD_SPI_FLASH_E3N: 1;
        uint16_t PAD_SPI_FLASH_E2N: 1;
        uint16_t PAD_SPI_FLASH_E5P: 1;
        uint16_t PAD_SPI_FLASH_E4P: 1;
        uint16_t PAD_SPI_FLASH_E3P: 1;
        uint16_t PAD_SPI_FLASH_E2P: 1;
    };
} AON_FAST_0x13A_TYPE;

/* 0x140
    0       R/W AON_PAD_SPI_SIO1_O                              1'b0
    1       R/W AON_PAD_SPI_SIO1_OE                             1'b0
    2       R/W AON_PAD_SPI_SIO1_PD                             1'b0
    3       R/W AON_PAD_SPI_SIO1_PU_EN                          1'b0
    4       R/W AON_PAD_SPI_SIO0_O                              1'b0
    5       R/W AON_PAD_SPI_SIO0_OE                             1'b0
    6       R/W AON_PAD_SPI_SIO0_PD                             1'b0
    7       R/W AON_PAD_SPI_SIO0_PU_EN                          1'b0
    8       R/W AON_PAD_SPI_SCK_O                               1'b0
    9       R/W AON_PAD_SPI_SCK_OE                              1'b0
    10      R/W AON_PAD_SPI_SCK_PD                              1'b0
    11      R/W AON_PAD_SPI_SCK_PU_EN                           1'b0
    12      R/W AON_PAD_SPI_CSN_O                               1'b0
    13      R/W AON_PAD_SPI_CSN_OE                              1'b0
    14      R/W AON_PAD_SPI_CSN_PD                              1'b0
    15      R/W AON_PAD_SPI_CSN_PU_EN                           1'b0
 */
typedef union _AON_FAST_0x140_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_PAD_SPI_SIO1_O: 1;
        uint16_t AON_PAD_SPI_SIO1_OE: 1;
        uint16_t AON_PAD_SPI_SIO1_PD: 1;
        uint16_t AON_PAD_SPI_SIO1_PU_EN: 1;
        uint16_t AON_PAD_SPI_SIO0_O: 1;
        uint16_t AON_PAD_SPI_SIO0_OE: 1;
        uint16_t AON_PAD_SPI_SIO0_PD: 1;
        uint16_t AON_PAD_SPI_SIO0_PU_EN: 1;
        uint16_t AON_PAD_SPI_SCK_O: 1;
        uint16_t AON_PAD_SPI_SCK_OE: 1;
        uint16_t AON_PAD_SPI_SCK_PD: 1;
        uint16_t AON_PAD_SPI_SCK_PU_EN: 1;
        uint16_t AON_PAD_SPI_CSN_O: 1;
        uint16_t AON_PAD_SPI_CSN_OE: 1;
        uint16_t AON_PAD_SPI_CSN_PD: 1;
        uint16_t AON_PAD_SPI_CSN_PU_EN: 1;
    };
} AON_FAST_0x140_TYPE;

/* 0x142
    07:00   R/W RSVD                                            8'h0
    8       R/W AON_PAD_SPI_SIO3_O                              1'b0
    9       R/W AON_PAD_SPI_SIO3_OE                             1'b0
    10      R/W AON_PAD_SPI_SIO3_PD                             1'b0
    11      R/W AON_PAD_SPI_SIO3_PU_EN                          1'b0
    12      R/W AON_PAD_SPI_SIO2_O                              1'b0
    13      R/W AON_PAD_SPI_SIO2_OE                             1'b0
    14      R/W AON_PAD_SPI_SIO2_PD                             1'b0
    15      R/W AON_PAD_SPI_SIO2_PU_EN                          1'b0
 */
typedef union _AON_FAST_0x142_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD: 8;
        uint16_t AON_PAD_SPI_SIO3_O: 1;
        uint16_t AON_PAD_SPI_SIO3_OE: 1;
        uint16_t AON_PAD_SPI_SIO3_PD: 1;
        uint16_t AON_PAD_SPI_SIO3_PU_EN: 1;
        uint16_t AON_PAD_SPI_SIO2_O: 1;
        uint16_t AON_PAD_SPI_SIO2_OE: 1;
        uint16_t AON_PAD_SPI_SIO2_PD: 1;
        uint16_t AON_PAD_SPI_SIO2_PU_EN: 1;
    };
} AON_FAST_0x142_TYPE;

/* 0x154
    07:00   R/W DUMMY                                           8'h64
    10:08   R/W lp_xtal_sel                                     3'h0
    13:11   R/W lp_xtal_sel_prediv                              3'h0
    15:14   R   RSVD                                            2'h0
 */
typedef union _AON_FAST_0x154_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DUMMY: 8;
        uint16_t lp_xtal_sel: 3;
        uint16_t lp_xtal_sel_prediv: 3;
        uint16_t RSVD: 2;
    };
} AON_FAST_0x154_TYPE;

/* 0x156
    05:00   R/W lp_xtal_ppm_err                                 6'h0
    6       R/W lp_xtal_ppm_plus                                1'h0
    07:07   R   RSVD                                            1'b0
    15:08   R/W DUMMY                                           8'h0
 */
typedef union _AON_FAST_0x156_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t lp_xtal_ppm_err: 6;
        uint16_t lp_xtal_ppm_plus: 1;
        uint16_t RSVD: 1;
        uint16_t DUMMY: 8;
    };
} AON_FAST_0x156_TYPE;

/* 0x158
    0       R/W SEL_32K_SDM_go                                  1'b0
    1       R/W SEL_32K_GPIO_go                                 1'b0
    2       R/W SEL_32K_LP_go                                   1'b0
    3       R/W SEL_32K_XTAL_go                                 1'b0
    4       R/W SEL_32K_SDM_tic                                 1'b0
    5       R/W SEL_32K_GPIO_tic                                1'b0
    6       R/W SEL_32K_LP_tic                                  1'b0
    7       R/W SEL_32K_XTAL_tic                                1'b0
    8       R/W SEL_32K_SDM_bt                                  1'b0
    9       R/W SEL_32K_GPIO_bt                                 1'b0
    10      R/W SEL_32K_LP_bt                                   1'b0
    11      R/W SEL_32K_XTAL_bt                                 1'b0
    12      R/W SEL_32K_SDM_rtc                                 1'b0
    13      R/W SEL_32K_GPIO_rtc                                1'b0
    14      R/W SEL_32K_LP_rtc                                  1'b0
    15      R/W SEL_32K_XTAL_rtc                                1'b0
 */
typedef union _AON_FAST_0x158_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SEL_32K_SDM_go: 1;
        uint16_t SEL_32K_GPIO_go: 1;
        uint16_t SEL_32K_LP_go: 1;
        uint16_t SEL_32K_XTAL_go: 1;
        uint16_t SEL_32K_SDM_tic: 1;
        uint16_t SEL_32K_GPIO_tic: 1;
        uint16_t SEL_32K_LP_tic: 1;
        uint16_t SEL_32K_XTAL_tic: 1;
        uint16_t SEL_32K_SDM_bt: 1;
        uint16_t SEL_32K_GPIO_bt: 1;
        uint16_t SEL_32K_LP_bt: 1;
        uint16_t SEL_32K_XTAL_bt: 1;
        uint16_t SEL_32K_SDM_rtc: 1;
        uint16_t SEL_32K_GPIO_rtc: 1;
        uint16_t SEL_32K_LP_rtc: 1;
        uint16_t SEL_32K_XTAL_rtc: 1;
    };
} AON_FAST_0x158_TYPE;

/* 0x15A
    2:0     R/W rtc_out_sel                                     3'b000
    15:03   R   RSVD                                            13'h0
 */
typedef union _AON_FAST_0x15A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rtc_out_sel: 3;
        uint16_t RSVD: 13;
    };
} AON_FAST_0x15A_TYPE;

/* 0x15E
    0       R/W USB_WKPOL                                       1'b0
    1       R/W USB_WKEN                                        1'b0
    2       R/W VAD_WKPOL                                       1'b0
    3       R/W VAD_WKEN                                        1'b0
    4       R/W VADBUF_WKPOL                                    1'b0
    5       R/W VADBUF_WKEN                                     1'b0
    6       R/W ADSP_WKPOL                                      1'b0
    7       R/W ADSP_WKEN                                       1'b0
    15:6    R   RSVD                                            10'h0
 */
typedef union _AON_FAST_0x15E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t USB_WKPOL: 1;
        uint16_t USB_WKEN: 1;
        uint16_t VAD_WKPOL: 1;
        uint16_t VAD_WKEN: 1;
        uint16_t VADBUF_WKPOL: 1;
        uint16_t VADBUF_WKEN: 1;
        uint16_t ADSP_WKPOL: 1;
        uint16_t ADSP_WKEN: 1;
        uint16_t RSVD: 8;
    };
} AON_FAST_0x15E_TYPE;

/* 0x160
    0       R/W PAD_UART_E2                                     1'b0
    1       R/W PAD_UART_SMT                                    1'b0
    2       R/W PAD_UART_SHDN                                   1'b1
    07:03   R/W DUMMY                                           5'b11111
    8       R/W SEL_32K_SDM                                     1'b0
    9       R/W SEL_32K_GPIO                                    1'b0
    10      R/W SEL_1M                                          1'b0
    11      R   RSVD0                                           1'b0
    12      R/W en_gpio_32k                                     1'b0
    15:13   R/W RSVD1                                           3'b000
 */
typedef union _AON_FAST_0x160_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_UART_E2: 1;
        uint16_t PAD_UART_SMT: 1;
        uint16_t PAD_UART_SHDN: 1;
        uint16_t DUMMY: 5;
        uint16_t SEL_32K_SDM: 1;
        uint16_t SEL_32K_GPIO: 1;
        uint16_t SEL_1M: 1;
        uint16_t RSVD0: 1;
        uint16_t en_gpio_32k: 1;
        uint16_t RSVD1: 3;
    };
} AON_FAST_0x160_TYPE;

/* 0x164
    01:00   R   RSVD0                                           2'b00
    2       R/W r_PMUX_SCAN_FT_2_EN                             1'b0
    3       R/W r_PMUX_SCAN_FT_EN                               1'b0
    4       R/W r_PMUX_SCAN_FT_2_EN_src                         1'b0
    5       R/W r_PMUX_SCAN_FT_EN_src                           1'b0
    6       R/W r_PMUX_SCAN_MODE_EN                             1'b0
    15:07   R   RSVD1                                           9'h0
 */
typedef union _AON_FAST_0x164_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD0: 2;
        uint16_t r_PMUX_SCAN_FT_2_EN: 1;
        uint16_t r_PMUX_SCAN_FT_EN: 1;
        uint16_t r_PMUX_SCAN_FT_2_EN_src: 1;
        uint16_t r_PMUX_SCAN_FT_EN_src: 1;
        uint16_t r_PMUX_SCAN_MODE_EN: 1;
        uint16_t RSVD1: 9;
    };
} AON_FAST_0x164_TYPE;

/* 0x166
    04:00   R   XPDCK_VREF_OUT                                  5'b00000
    05:05   R   XPDCK_READY                                     1'b0
    06:06   R   XPDCK_BUSY                                      1'b0
    07:07   R   RSVD                                            1'b0
    12:08   R   XTAL_PDCK_LP                                    5'b00000
    13:13   R   XTAL_PDCK_OK                                    1'b0
    14:14   R   XTAL_PDCK_BUSY                                  1'b0
    15:15   R/W xtal_pdck_rslt_latch                            1'b0
 */
typedef union _AON_FAST_0x166_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XPDCK_VREF_OUT: 5;
        uint16_t XPDCK_READY: 1;
        uint16_t XPDCK_BUSY: 1;
        uint16_t RSVD: 1;
        uint16_t XTAL_PDCK_LP: 5;
        uint16_t XTAL_PDCK_OK: 1;
        uint16_t XTAL_PDCK_BUSY: 1;
        uint16_t xtal_pdck_rslt_latch: 1;
    };
} AON_FAST_0x166_TYPE;

/* 0x172
    00:00   R   RSVD0                                           1'b0
    01:01   R/W r_clk_cpu_tick_sel                              1'b0
    02:02   R/W DUMMY0                                          1'b0
    06:03   R   RSVD1                                           1'b0
    15:07   R/W DUMMY1                                          9'b111111111
 */
typedef union _AON_FAST_0x172_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD0: 1;
        uint16_t r_clk_cpu_tick_sel: 1;
        uint16_t DUMMY0: 1;
        uint16_t RSVD1: 4;
        uint16_t DUMMY1: 9;
    };
} AON_FAST_0x172_TYPE;

/* 0x178
    07:00   R   RSVD0                                           8'ha0
    11:08   R/W DUMMY                                           4'b0001
    15:12   R   RSVD1                                           4'h0
 */
typedef union _AON_FAST_0x178_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD0: 8;
        uint16_t DUMMY: 4;
        uint16_t RSVD1: 4;
    };
} AON_FAST_0x178_TYPE;

/* 0x17E
    00:00   W1O reg_anc_inst_rp                                 1'b0
    01:01   W1O reg_anc_para_rp                                 1'b0
    02:02   R/W ISO_AFE_PLL1_MCLK(ISO_PLL1toPON)                1'b1
    03:03   R/W ISO_AFE_PLL2_MCLK(ISO_PLL2toPON)                1'b1
    09:04   R/W DUMMY0                                          6'h0
    10:10   R   RSVD0                                           1'b0
    11:11   R/W DUMMY1                                          1'b0
    13:12   W1O reg_aon_w1o_dbg_dis_range                       2'h0
    15:14   R   RSVD1                                           2'h0
 */
typedef union _AON_FAST_0x17E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_anc_inst_rp: 1;
        uint16_t reg_anc_para_rp: 1;
        uint16_t ISO_AFE_PLL1_MCLK_ISO_PLL1toPON: 1;
        uint16_t ISO_AFE_PLL2_MCLK_ISO_PLL2toPON: 1;
        uint16_t DUMMY0: 6;
        uint16_t RSVD0: 1;
        uint16_t DUMMY1: 1;
        uint16_t reg_aon_w1o_dbg_dis_range: 2;
        uint16_t RSVD1: 2;
    };
} AON_FAST_0x17E_TYPE;

/* 0x180
    06:00   R/W DUMMY0                                          7'b0100101
    07:07   R   RSVD0                                           1'b0
    14:08   R/W DUMMY1                                          7'b0100000
    15:15   R   RSVD1                                           1'b0
 */
typedef union _AON_FAST_0x180_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DUMMY0: 7;
        uint16_t RSVD0: 1;
        uint16_t DUMMY1: 7;
        uint16_t RSVD1: 1;
    };
} AON_FAST_0x180_TYPE;

/* 0x182
    06:00   R/W DUMMY0                                          7'b0011100
    07:07   R   RSVD0                                           1'b0
    14:08   R/W DUMMY1                                          7'b0011000
    15:15   R   RSVD1                                           1'b0
 */
typedef union _AON_FAST_0x182_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DUMMY0: 7;
        uint16_t RSVD0: 1;
        uint16_t DUMMY1: 7;
        uint16_t RSVD1: 1;
    };
} AON_FAST_0x182_TYPE;

/* 0x184
    15:00   R/W reg_aon_gpr_0                                   16'h0
 */
typedef union _AON_FAST_0x184_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_gpr_0;
    };
} AON_FAST_0x184_TYPE;

/* 0x186
    15:00   R/W reg_aon_gpr_1                                   16'h0
 */
typedef union _AON_FAST_0x186_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_gpr_1;
    };
} AON_FAST_0x186_TYPE;

/* 0x188
    15:00   R/W reg_aon_gpr_2                                   16'h0
 */
typedef union _AON_FAST_0x188_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_gpr_2;
    };
} AON_FAST_0x188_TYPE;

/* 0x18A
    15:00   R/W reg_aon_gpr_3                                   16'h0
 */
typedef union _AON_FAST_0x18A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_gpr_3;
    };
} AON_FAST_0x18A_TYPE;

/* 0x18C
    15:00   R/W reg_aon_gpr_4                                   16'h0
 */
typedef union _AON_FAST_0x18C_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_gpr_4;
    };
} AON_FAST_0x18C_TYPE;

/* 0x18E
    15:00   R/W reg_aon_gpr_5                                   16'h0
 */
typedef union _AON_FAST_0x18E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_gpr_5;
    };
} AON_FAST_0x18E_TYPE;

/* 0x190
    15:00   R/W reg_aon_gpr_6                                   16'h0
 */
typedef union _AON_FAST_0x190_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_gpr_6;
    };
} AON_FAST_0x190_TYPE;

/* 0x192
    15:00   R/W reg_aon_gpr_7                                   16'h0
 */
typedef union _AON_FAST_0x192_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_gpr_7;
    };
} AON_FAST_0x192_TYPE;

/* 0x194
    15:00   R/W reg_aon_gpr_8                                   16'h0
 */
typedef union _AON_FAST_0x194_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_gpr_8;
    };
} AON_FAST_0x194_TYPE;

/* 0x196
    15:00   R/W reg_aon_gpr_9                                   16'h0
 */
typedef union _AON_FAST_0x196_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_gpr_9;
    };
} AON_FAST_0x196_TYPE;

/* 0x198
    15:00   R/W reg_aon_gpr_10                                  16'h0
 */
typedef union _AON_FAST_0x198_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_gpr_10;
    };
} AON_FAST_0x198_TYPE;

/* 0x19A
    15:00   R/W reg_aon_gpr_11                                  16'h0
 */
typedef union _AON_FAST_0x19A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_gpr_11;
    };
} AON_FAST_0x19A_TYPE;

/* 0x19C
    15:00   R/W reg_aon_gpr_12                                  16'h0
 */
typedef union _AON_FAST_0x19C_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_gpr_12;
    };
} AON_FAST_0x19C_TYPE;

/* 0x19E
    15:00   R/W reg_aon_gpr_13                                  16'h0
 */
typedef union _AON_FAST_0x19E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_gpr_13;
    };
} AON_FAST_0x19E_TYPE;

/* 0x1A0
    15:00   R/W reg_aon_gpr_14                                  16'h0
 */
typedef union _AON_FAST_0x1A0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_gpr_14;
    };
} AON_FAST_0x1A0_TYPE;

/* 0x1A2
    07:00   R/W IMG_ID                                          8'b0
    15:08   R/W RSVD                                            8'b0
 */
typedef union _AON_FAST_0x1A2_TYPE
{
    uint16_t d16;
    uint16_t IMG_ID;
} AON_FAST_0x1A2_TYPE;

/* 0x1A4
    12:00   W1O reg_aon_w1o_gpr_0                              14'h0
    13:13   W1O reg_aon_w1o_adsp_ahb_r_dis                      1'b0
    14:14   W1O reg_aon_w1o_pon_rst2core_en                     1'b0
    15:15   W1O reg_aon_w1o_sniff_half_slot_dis                 1'b0
 */
typedef union _AON_FAST_0x1A4_TYPE
{
    uint16_t d16;
    uint8_t d8[2];
    struct
    {
        uint16_t is_disable_hci_wifi_coexist_func: 1;      // 00
        uint16_t is_disable_hci_set_uart_baudrate: 1;      // 01
        uint16_t is_disable_hci_rf_dbg_func: 1;            // 02
        uint16_t is_disable_hci_read_chip_info: 1;         // 03
        uint16_t is_disable_hci_bt_extension: 1;           // 04
        uint16_t is_disable_hci_bt_dbg_func: 1;            // 05
        uint16_t is_disable_hci_bt_test: 1;                // 06
        uint16_t is_disable_hci_rf_test: 1;                // 07
        uint16_t enable_image_auth_when_resume: 1;         // 08
        uint16_t disable_set_reg_by_efuse: 1;              // 09
        uint16_t w1o_rsvd: 3;                              // 12:10
        uint16_t reg_aon_w1o_adsp_ahb_r_dis: 1;            // 13
        uint16_t reg_aon_w1o_pon_rst2core_en: 1;           // 14
        uint16_t reg_aon_w1o_sniff_half_slot_dis: 1;       // 15
    };
} AON_FAST_0x1A4_TYPE;

/* 0x1A6
    15:00   W1O reg_aon_w1o_gpr_1                               16'h0
 */
typedef union _AON_FAST_0x1A6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_gpr_1;
    };
} AON_FAST_0x1A6_TYPE;

/* 0x1A8
    15:00   W1O reg_aon_w1o_gpr_2                               16'h0
 */
typedef union _AON_FAST_0x1A8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_gpr_2;
    };
} AON_FAST_0x1A8_TYPE;

/* 0x1AA
    15:00   W1O reg_aon_w1o_gpr_3                               16'h0
 */
typedef union _AON_FAST_0x1AA_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_gpr_3;
    };
} AON_FAST_0x1AA_TYPE;

/* 0x1AC
    15:00   W1O reg_aon_w1o_gpr_4                               16'h0
 */
typedef union _AON_FAST_0x1AC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_gpr_4;
    };
} AON_FAST_0x1AC_TYPE;

/* 0x1AE
    15:00   W1O reg_aon_w1o_gpr_5                               16'h0
 */
typedef union _AON_FAST_0x1AE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_gpr_5;
    };
} AON_FAST_0x1AE_TYPE;

/* 0x1B0
    15:00   W1O reg_aon_w1o_gpr_6                               16'h0
 */
typedef union _AON_FAST_0x1B0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_gpr_6;
    };
} AON_FAST_0x1B0_TYPE;

/* 0x1B2
    15:00   W1O reg_aon_w1o_gpr_7                               16'h0
 */
typedef union _AON_FAST_0x1B2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_gpr_7;
    };
} AON_FAST_0x1B2_TYPE;

/* 0x1B4
    15:00   W1O reg_aon_w1o_gpr_8                               16'h0
 */
typedef union _AON_FAST_0x1B4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_gpr_8;
    };
} AON_FAST_0x1B4_TYPE;

/* 0x1B6
    15:00   W1O reg_aon_w1o_gpr_9                               16'h0
 */
typedef union _AON_FAST_0x1B6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_gpr_9;
    };
} AON_FAST_0x1B6_TYPE;

/* 0x1B8
    15:00   W1O reg_aon_w1o_gpr_10                              16'h0
 */
typedef union _AON_FAST_0x1B8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_gpr_10;
    };
} AON_FAST_0x1B8_TYPE;

/* 0x1BA
    15:00   W1O reg_aon_w1o_gpr_11                              16'h0
 */
typedef union _AON_FAST_0x1BA_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_gpr_11;
    };
} AON_FAST_0x1BA_TYPE;

/* 0x1BC
    15:00   W1O reg_aon_w1o_gpr_12                              16'h0
 */
typedef union _AON_FAST_0x1BC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_gpr_12;
    };
} AON_FAST_0x1BC_TYPE;

/* 0x1BE
    15:00   W1O reg_aon_w1o_gpr_13                              16'h0
 */
typedef union _AON_FAST_0x1BE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_gpr_13;
    };
} AON_FAST_0x1BE_TYPE;

/* 0x1C0
    15:00   W1O reg_aon_w1o_gpr_14                              16'h0
 */
typedef union _AON_FAST_0x1C0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_gpr_14;
    };
} AON_FAST_0x1C0_TYPE;

/* 0x1C2
    15:00   W1O reg_aon_w1o_gpr_15                              16'h0
 */
typedef union _AON_FAST_0x1C2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_gpr_15;
    };
} AON_FAST_0x1C2_TYPE;

/* 0x1C4
    0       R/W reg_aon_w1o_wpgpr_lock0                         1'b0
    1       R/W reg_aon_w1o_wpgpr_lock1                         1'b0
    2       R/W reg_aon_w1o_wpgpr_lock2                         1'b0
    3       R/W reg_aon_w1o_wpgpr_lock3                         1'b0
    4       R/W reg_aon_w1o_wpgpr_lock4                         1'b0
    5       R/W reg_aon_w1o_wpgpr_lock5                         1'b0
    6       R/W reg_aon_w1o_wpgpr_lock6                         1'b0
    7       R/W reg_aon_w1o_wpgpr_lock7                         1'b0
    8       R/W reg_aon_w1o_xo_lock                             1'b0
    9       R/W reg_aon_w1o_aes_out_hidden_lock                 1'b0
    10      R/W reg_aon_pll1_cp_dis_by_xtal                     1'b0
    11      R/W reg_aon_pll2_cp_dis_by_xtal                     1'b0
    12      R/W reg_aon_pll1_cp_dis_fw                          1'b0
    13      R/W reg_aon_pll2_cp_dis_fw                          1'b0
    14      R/W reg_aon_pll3_cp_dis_by_xtal                     1'b0
    15      R/W reg_aon_pll3_cp_dis_fw                          1'b0
 */
typedef union _AON_FAST_0x1C4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_wpgpr_lock0: 1;
        uint16_t reg_aon_w1o_wpgpr_lock1: 1;
        uint16_t reg_aon_w1o_wpgpr_lock2: 1;
        uint16_t reg_aon_w1o_wpgpr_lock3: 1;
        uint16_t reg_aon_w1o_wpgpr_lock4: 1;
        uint16_t reg_aon_w1o_wpgpr_lock5: 1;
        uint16_t reg_aon_w1o_wpgpr_lock6: 1;
        uint16_t reg_aon_w1o_wpgpr_lock7: 1;
        uint16_t reg_aon_w1o_xo_lock: 1;
        uint16_t reg_aon_w1o_aes_out_hidden_lock: 1;
        uint16_t reg_aon_pll1_cp_dis_by_xtal: 1;
        uint16_t reg_aon_pll2_cp_dis_by_xtal: 1;
        uint16_t reg_aon_pll1_cp_dis_fw: 1;
        uint16_t reg_aon_pll2_cp_dis_fw: 1;
        uint16_t reg_aon_pll3_cp_dis_by_xtal: 1;
        uint16_t reg_aon_pll3_cp_dis_fw: 1;
    };
} AON_FAST_0x1C4_TYPE;

/* 0x1C6
    0       R/W reg_aon_pll4_cp_dis_by_xtal                         1'b0
    1       R/W reg_aon_pll4_cp_dis_fw                              1'b0
    15:02   R/W RSVD                                                14'h0
 */
typedef union _AON_FAST_0x1C6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_pll4_cp_dis_by_xtal: 1;
        uint16_t reg_aon_pll4_cp_dis_fw: 1;
        uint16_t RSVD: 14;
    };
} AON_FAST_0x1C6_TYPE;

/* 0x1CC
    15:0    R/W reg_aon_wpgpr_0                                 16'h0
 */
typedef union _AON_FAST_0x1CC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_wpgpr_0;
    };
} AON_FAST_0x1CC_TYPE;

/* 0x1CE
    31:16   R/W reg_aon_wpgpr_0                                 16'h0
 */
typedef union _AON_FAST_0x1CE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_wpgpr_0;
    };
} AON_FAST_0x1CE_TYPE;

/* 0x1D0
    15:0    R/W reg_aon_wpgpr_1                                 16'h0
 */
typedef union _AON_FAST_0x1D0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_wpgpr_1;
    };
} AON_FAST_0x1D0_TYPE;

/* 0x1D2
    31:16   R/W reg_aon_wpgpr_1                                 16'h0
 */
typedef union _AON_FAST_0x1D2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_wpgpr_1;
    };
} AON_FAST_0x1D2_TYPE;

/* 0x1D4
    15:0    R/W reg_aon_wpgpr_2                                 16'h0
 */
typedef union _AON_FAST_0x1D4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_wpgpr_2;
    };
} AON_FAST_0x1D4_TYPE;

/* 0x1D6
    31:16   R/W reg_aon_wpgpr_2                                 16'h0
 */
typedef union _AON_FAST_0x1D6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_wpgpr_2;
    };
} AON_FAST_0x1D6_TYPE;

/* 0x1D8
    15:0    R/W reg_aon_wpgpr_3                                 16'h0
 */
typedef union _AON_FAST_0x1D8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_wpgpr_3;
    };
} AON_FAST_0x1D8_TYPE;

/* 0x1DA
    31:16   R/W reg_aon_wpgpr_3                                 16'h0
 */
typedef union _AON_FAST_0x1DA_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_wpgpr_3;
    };
} AON_FAST_0x1DA_TYPE;

/* 0x1DC
    15:0    R/W reg_aon_wpgpr_4                                 16'h0
 */
typedef union _AON_FAST_0x1DC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_wpgpr_4;
    };
} AON_FAST_0x1DC_TYPE;

/* 0x1DE
    31:16   R/W reg_aon_wpgpr_4                                 16'h0
 */
typedef union _AON_FAST_0x1DE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_wpgpr_4;
    };
} AON_FAST_0x1DE_TYPE;

/* 0x1E0
    15:0    R/W reg_aon_wpgpr_5                                 16'h0
 */
typedef union _AON_FAST_0x1E0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_wpgpr_5;
    };
} AON_FAST_0x1E0_TYPE;

/* 0x1E2
    31:16   R/W reg_aon_wpgpr_5                                 16'h0
 */
typedef union _AON_FAST_0x1E2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_wpgpr_5;
    };
} AON_FAST_0x1E2_TYPE;

/* 0x1E4
    15:0    R/W reg_aon_wpgpr_6                                 16'h0
 */
typedef union _AON_FAST_0x1E4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_wpgpr_6;
    };
} AON_FAST_0x1E4_TYPE;

/* 0x1E6
    31:16   R/W reg_aon_wpgpr_6                                 16'h0
 */
typedef union _AON_FAST_0x1E6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_wpgpr_6;
    };
} AON_FAST_0x1E6_TYPE;

/* 0x1E8
    15:0    R/W reg_aon_wpgpr_7                                 16'h0
 */
typedef union _AON_FAST_0x1E8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_wpgpr_7;
    };
} AON_FAST_0x1E8_TYPE;

/* 0x1EA
    31:16   R/W reg_aon_wpgpr_7                                 16'h0
 */
typedef union _AON_FAST_0x1EA_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_wpgpr_7;
    };
} AON_FAST_0x1EA_TYPE;

/* 0x1EC
    0       WP  reg_aon_w1o_xo_disable                          1'b0
    1       W1O reg_aon_w1o_dsp_dis                             1'b0
    2       WP  reg_aon_w1o_dsp_jtag_dis                        1'b0
    3       W1O reg_aon_w1o_vad_dis                             1'b0
    4       W1O reg_aon_w1o_mic1_dis                            1'b0
    5       W1O reg_aon_w1o_mic2_dis                            1'b0
    6       W1O reg_aon_w1o_mic3_dis                            1'b0
    7       W1O reg_aon_w1o_mic4_dis                            1'b0
    8       W1O reg_aon_w1o_spk1_dis                            1'b0
    9       W1O reg_aon_w1o_spk2_dis                            1'b0
    15:10   W1O DUMMY                                           6'h0
 */
typedef union _AON_FAST_0x1EC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_xo_disable: 1;
        uint16_t reg_aon_w1o_dsp_dis: 1;
        uint16_t reg_aon_w1o_dsp_jtag_dis: 1;
        uint16_t reg_aon_w1o_vad_dis: 1;
        uint16_t reg_aon_w1o_mic1_dis: 1;
        uint16_t reg_aon_w1o_mic2_dis: 1;
        uint16_t reg_aon_w1o_mic3_dis: 1;
        uint16_t reg_aon_w1o_mic4_dis: 1;
        uint16_t reg_aon_w1o_spk1_dis: 1;
        uint16_t reg_aon_w1o_spk2_dis: 1;
        uint16_t DUMMY: 6;
    };
} AON_FAST_0x1EC_TYPE;

/* 0x1EE
    7:0     W1O reg_anc_io_en_wp                                8'h0
    8       W1O reg_aon_w1o_bt4_func_dis                        1'b0
    9       W1O reg_aon_w1o_bt2_func_dis                        1'b0
    10      W1O reg_aon_w1o_handover_func_dis                   1'b0
    11      W1O reg_aon_w1o_sniff2_func_dis                     1'b0
    12      W1O reg_aon_w1o_sniff1_func_dis                     1'b0
    13      W1O reg_aon_w1o_spic0_ft_dis                        1'b0
    14      W1O reg_aon_w1o_spic0_mp_dis                        1'b0
    15      W1O reg_aon_w1o_spic0_dbg_dis                       1'b0
 */
typedef union _AON_FAST_0x1EE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_anc_io_en_wp: 8;
        uint16_t reg_aon_w1o_bt4_func_dis: 1;
        uint16_t reg_aon_w1o_bt2_func_dis: 1;
        uint16_t reg_aon_w1o_handover_func_dis: 1;
        uint16_t reg_aon_w1o_sniff2_func_dis: 1;
        uint16_t reg_aon_w1o_sniff1_func_dis: 1;
        uint16_t reg_aon_w1o_spic0_ft_dis: 1;
        uint16_t reg_aon_w1o_spic0_mp_dis: 1;
        uint16_t reg_aon_w1o_spic0_dbg_dis: 1;
    };
} AON_FAST_0x1EE_TYPE;

/* 0x1F0
    0       W1O reg_aon_w1o_mic5_dis                            1'b0
    1       W1O reg_aon_w1o_mic6_dis                            1'b0
    2       W1O reg_aon_w1o_vadbuf_dis                          1'b0
    3       W1O reg_aon_w1o_ram_lock                            1'b0
    4       W1O reg_aon_w1o_required_mode_lock                  1'b0
    5       W1O reg_aon_w1o_test_row_en_lock                    1'b0
    6       W1O reg_aon_w1o_test_mode_sel_lock                  1'b0
    7       W1O reg_aon_w1o_spic4_dis                           1'b0
    8       W1O reg_aon_w1o_spic1_dis                           1'b0
    9       W1O reg_aon_w1o_spic2_dis                           1'b0
    10      W1O reg_aon_w1o_spic3_dis                           1'b0
    11      W1O reg_aon_w1o_dsp_peri_dis                        1'b0
    12      W1O reg_aon_w1o_guardtime_agc_dis                   1'b0
    13      W1O reg_aon_w1o_pn_cfo_comp_dis                     1'b0
    14      W1O reg_aon_w1o_detail_rxrpt_dis                    1'b0
    15      W1O reg_aon_w1o_detail_rxrpt_cfoe_dis               1'b0
 */
typedef union _AON_FAST_0x1F0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_mic5_dis: 1;
        uint16_t reg_aon_w1o_mic6_dis: 1;
        uint16_t reg_aon_w1o_vadbuf_dis: 1;
        uint16_t reg_aon_w1o_ram_lock: 1;
        uint16_t reg_aon_w1o_required_mode_lock: 1;
        uint16_t reg_aon_w1o_test_row_en_lock: 1;
        uint16_t reg_aon_w1o_test_mode_sel_lock: 1;
        uint16_t reg_aon_w1o_spic4_dis: 1;
        uint16_t reg_aon_w1o_spic1_dis: 1;
        uint16_t reg_aon_w1o_spic2_dis: 1;
        uint16_t reg_aon_w1o_spic3_dis: 1;
        uint16_t reg_aon_w1o_dsp_peri_dis: 1;
        uint16_t reg_aon_w1o_guardtime_agc_dis: 1;
        uint16_t reg_aon_w1o_pn_cfo_comp_dis: 1;
        uint16_t reg_aon_w1o_detail_rxrpt_dis: 1;
        uint16_t reg_aon_w1o_detail_rxrpt_cfoe_dis: 1;
    };
} AON_FAST_0x1F0_TYPE;

/* 0x1F2
    0       W1O reg_aon_w1o_sys_dma_dis                         1'b0
    1       W1O reg_aon_w1o_nna_dis                             1'b0
    2       R   RSVD                                            1'b0
    3       W1O reg_aon_w1o_dlps_iso_bt_core3                   1'b0
    4       W1O reg_aon_w1o_sys_dma_dis_spic0                   1'b0
    5       W1O reg_aon_w1o_sys_dma_dis_spic1                   1'b0
    6       W1O reg_aon_w1o_sys_dma_dis_spic2                   1'b0
    7       W1O reg_aon_w1o_sys_dma_dis_spic3                   1'b0
    8       W1O reg_aon_w1o_dsp1_dis                            1'b0
    9       W1O reg_aon_w1o_dsp2_dis                            1'b0
    10      W1O reg_aon_w1o_ancdsp_dis                          1'b0
    11      W1O reg_aon_w1o_ancfir_dis                          1'b0
    12      W1O reg_aon_w1o_dsp1_jtag_dis                       1'b0
    13      W1O reg_aon_w1o_dsp2_jtag_dis                       1'b0
    14      W1O reg_aon_w1o_ancdsp_jtag_dis                     1'b0
    15      W1O reg_aon_w1o_psramc_dis                          1'b0
 */
typedef union _AON_FAST_0x1F2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_aon_w1o_sys_dma_dis: 1;
        uint16_t reg_aon_w1o_nna_dis: 1;
        uint16_t RSVD: 1;
        uint16_t reg_aon_w1o_dlps_iso_bt_core3: 1;
        uint16_t reg_aon_w1o_sys_dma_dis_spic0: 1;
        uint16_t reg_aon_w1o_sys_dma_dis_spic1: 1;
        uint16_t reg_aon_w1o_sys_dma_dis_spic2: 1;
        uint16_t reg_aon_w1o_sys_dma_dis_spic3: 1;
        uint16_t reg_aon_w1o_dsp1_dis: 1;
        uint16_t reg_aon_w1o_dsp2_dis: 1;
        uint16_t reg_aon_w1o_ancdsp_dis: 1;
        uint16_t reg_aon_w1o_ancfir_dis: 1;
        uint16_t reg_aon_w1o_dsp1_jtag_dis: 1;
        uint16_t reg_aon_w1o_dsp2_jtag_dis: 1;
        uint16_t reg_aon_w1o_ancdsp_jtag_dis: 1;
        uint16_t reg_aon_w1o_psramc_dis: 1;
    };
} AON_FAST_0x1F2_TYPE;

/* 0x1FE
    06:00   R/W rtc_in_sel                                      7'h7F
    07:07   R/W dummy                                           5'h0
    12:08   R/W MCLK2_OUT_SEL                                   5'b00000
    15:13   R/W MCLK3_OUT_SEL                                   3'b000
 */
typedef union _AON_FAST_0x1FE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t rtc_in_sel: 7;
        uint16_t dummy: 1;
        uint16_t MCLK2_OUT_SEL: 5;
        uint16_t MCLK3_OUT_SEL: 3;
    };
} AON_FAST_0x1FE_TYPE;

/* 0x200
    0       R/W PAD_P1_SHDN[0]                                  1'b1
    1       R/W AON_PAD_P1_E[0]                                 1'b0
    2       R/W PAD_P1_WKPOL[0]                                 1'b0
    3       R/W PAD_P1_WKEN[0]                                  1'b0
    4       R/W AON_PAD_P1_O[0]                                 1'b0
    5       R/W PAD_P1_PDPUC[0]                                 1'b0
    6       R/W AON_PAD_P1_PU[0]                                1'b0
    7       R/W AON_PAD_P1_PU_EN[0]                             1'b1
    8       R/W PAD_P1_SHDN[1]                                  1'b1
    9       R/W AON_PAD_P1_E[1]                                 1'b0
    10      R/W PAD_P1_WKPOL[1]                                 1'b0
    11      R/W PAD_P1_WKEN[1]                                  1'b0
    12      R/W AON_PAD_P1_O[1]                                 1'b0
    13      R/W PAD_P1_PDPUC[1]                                 1'b0
    14      R/W AON_PAD_P1_PU[1]                                1'b0
    15      R/W AON_PAD_P1_PU_EN[1]                             1'b1
 */
typedef union _AON_FAST_0x200_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P1_SHDN_0: 1;
        uint16_t AON_PAD_P1_E_0: 1;
        uint16_t PAD_P1_WKPOL_0: 1;
        uint16_t PAD_P1_WKEN_0: 1;
        uint16_t AON_PAD_P1_O_0: 1;
        uint16_t PAD_P1_PDPUC_0: 1;
        uint16_t AON_PAD_P1_PU_0: 1;
        uint16_t AON_PAD_P1_PU_EN_0: 1;
        uint16_t PAD_P1_SHDN_1: 1;
        uint16_t AON_PAD_P1_E_1: 1;
        uint16_t PAD_P1_WKPOL_1: 1;
        uint16_t PAD_P1_WKEN_1: 1;
        uint16_t AON_PAD_P1_O_1: 1;
        uint16_t PAD_P1_PDPUC_1: 1;
        uint16_t AON_PAD_P1_PU_1: 1;
        uint16_t AON_PAD_P1_PU_EN_1: 1;
    };
} AON_FAST_0x200_TYPE;

/* 0x202
    0       R/W PAD_P1_SHDN[2]                                  1'b1
    1       R/W AON_PAD_P1_E[2]                                 1'b0
    2       R/W PAD_P1_WKPOL[2]                                 1'b0
    3       R/W PAD_P1_WKEN[2]                                  1'b0
    4       R/W AON_PAD_P1_O[2]                                 1'b0
    5       R/W PAD_P1_PDPUC[2]                                 1'b0
    6       R/W AON_PAD_P1_PU[2]                                1'b0
    7       R/W AON_PAD_P1_PU_EN[2]                             1'b1
    8       R/W PAD_P1_SHDN[3]                                  1'b1
    9       R/W AON_PAD_P1_E[3]                                 1'b0
    10      R/W PAD_P1_WKPOL[3]                                 1'b0
    11      R/W PAD_P1_WKEN[3]                                  1'b0
    12      R/W AON_PAD_P1_O[3]                                 1'b0
    13      R/W PAD_P1_PDPUC[3]                                 1'b0
    14      R/W AON_PAD_P1_PU[3]                                1'b0
    15      R/W AON_PAD_P1_PU_EN[3]                             1'b1
 */
typedef union _AON_FAST_0x202_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P1_SHDN_2: 1;
        uint16_t AON_PAD_P1_E_2: 1;
        uint16_t PAD_P1_WKPOL_2: 1;
        uint16_t PAD_P1_WKEN_2: 1;
        uint16_t AON_PAD_P1_O_2: 1;
        uint16_t PAD_P1_PDPUC_2: 1;
        uint16_t AON_PAD_P1_PU_2: 1;
        uint16_t AON_PAD_P1_PU_EN_2: 1;
        uint16_t PAD_P1_SHDN_3: 1;
        uint16_t AON_PAD_P1_E_3: 1;
        uint16_t PAD_P1_WKPOL_3: 1;
        uint16_t PAD_P1_WKEN_3: 1;
        uint16_t AON_PAD_P1_O_3: 1;
        uint16_t PAD_P1_PDPUC_3: 1;
        uint16_t AON_PAD_P1_PU_3: 1;
        uint16_t AON_PAD_P1_PU_EN_3: 1;
    };
} AON_FAST_0x202_TYPE;

/* 0x204
    0       R/W PAD_P1_SHDN[4]                                  1'b1
    1       R/W AON_PAD_P1_E[4]                                 1'b0
    2       R/W PAD_P1_WKPOL[4]                                 1'b0
    3       R/W PAD_P1_WKEN[4]                                  1'b0
    4       R/W AON_PAD_P1_O[4]                                 1'b0
    5       R/W PAD_P1_PDPUC[4]                                 1'b0
    6       R/W AON_PAD_P1_PU[4]                                1'b0
    7       R/W AON_PAD_P1_PU_EN[4]                             1'b1
    8       R/W PAD_P1_SHDN[5]                                  1'b1
    9       R/W AON_PAD_P1_E[5]                                 1'b0
    10      R/W PAD_P1_WKPOL[5]                                 1'b0
    11      R/W PAD_P1_WKEN[5]                                  1'b0
    12      R/W AON_PAD_P1_O[5]                                 1'b0
    13      R/W PAD_P1_PDPUC[5]                                 1'b0
    14      R/W AON_PAD_P1_PU[5]                                1'b0
    15      R/W AON_PAD_P1_PU_EN[5]                             1'b1
 */
typedef union _AON_FAST_0x204_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P1_SHDN_4: 1;
        uint16_t AON_PAD_P1_E_4: 1;
        uint16_t PAD_P1_WKPOL_4: 1;
        uint16_t PAD_P1_WKEN_4: 1;
        uint16_t AON_PAD_P1_O_4: 1;
        uint16_t PAD_P1_PDPUC_4: 1;
        uint16_t AON_PAD_P1_PU_4: 1;
        uint16_t AON_PAD_P1_PU_EN_4: 1;
        uint16_t PAD_P1_SHDN_5: 1;
        uint16_t AON_PAD_P1_E_5: 1;
        uint16_t PAD_P1_WKPOL_5: 1;
        uint16_t PAD_P1_WKEN_5: 1;
        uint16_t AON_PAD_P1_O_5: 1;
        uint16_t PAD_P1_PDPUC_5: 1;
        uint16_t AON_PAD_P1_PU_5: 1;
        uint16_t AON_PAD_P1_PU_EN_5: 1;
    };
} AON_FAST_0x204_TYPE;

/* 0x206
    0       R/W PAD_P1_SHDN[6]                                  1'b1
    1       R/W AON_PAD_P1_E[6]                                 1'b0
    2       R/W PAD_P1_WKPOL[6]                                 1'b0
    3       R/W PAD_P1_WKEN[6]                                  1'b0
    4       R/W AON_PAD_P1_O[6]                                 1'b0
    5       R/W PAD_P1_PDPUC[6]                                 1'b0
    6       R/W AON_PAD_P1_PU[6]                                1'b0
    7       R/W AON_PAD_P1_PU_EN[6]                             1'b1
    8       R/W PAD_P1_SHDN[7]                                  1'b1
    9       R/W AON_PAD_P1_E[7]                                 1'b0
    10      R/W PAD_P1_WKPOL[7]                                 1'b0
    11      R/W PAD_P1_WKEN[7]                                  1'b0
    12      R/W AON_PAD_P1_O[7]                                 1'b0
    13      R/W PAD_P1_PDPUC[7]                                 1'b0
    14      R/W AON_PAD_P1_PU[7]                                1'b0
    15      R/W AON_PAD_P1_PU_EN[7]                             1'b1
 */
typedef union _AON_FAST_0x206_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P1_SHDN_6: 1;
        uint16_t AON_PAD_P1_E_6: 1;
        uint16_t PAD_P1_WKPOL_6: 1;
        uint16_t PAD_P1_WKEN_6: 1;
        uint16_t AON_PAD_P1_O_6: 1;
        uint16_t PAD_P1_PDPUC_6: 1;
        uint16_t AON_PAD_P1_PU_6: 1;
        uint16_t AON_PAD_P1_PU_EN_6: 1;
        uint16_t PAD_P1_SHDN_7: 1;
        uint16_t AON_PAD_P1_E_7: 1;
        uint16_t PAD_P1_WKPOL_7: 1;
        uint16_t PAD_P1_WKEN_7: 1;
        uint16_t AON_PAD_P1_O_7: 1;
        uint16_t PAD_P1_PDPUC_7: 1;
        uint16_t AON_PAD_P1_PU_7: 1;
        uint16_t AON_PAD_P1_PU_EN_7: 1;
    };
} AON_FAST_0x206_TYPE;

/* 0x208
    07:00   R/W AON_PAD_P1_S[7:0]                               8'h0
    10:08   R/W PAD_P1_1_CFG                                    3'b0
    11:11   R/W AON_PAD_32KXI_S                                 1'b0
    14:12   R/W PAD_P1_0_CFG                                    3'b0
    15:15   R/W AON_PAD_32KXO_S                                 1'b0
 */
typedef union _AON_FAST_0x208_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_PAD_P1_S_7_0: 8;
        uint16_t PAD_P1_1_CFG: 3;
        uint16_t AON_PAD_32KXI_S: 1;
        uint16_t PAD_P1_0_CFG: 3;
        uint16_t AON_PAD_32KXO_S: 1;
    };
} AON_FAST_0x208_TYPE;

/* 0x20A
    02:00   R/W PAD_P1_4_CFG                                    3'b0
    15:03   R   RSVD                                            13'h0
 */
typedef union _AON_FAST_0x20A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P1_4_CFG: 3;
        uint16_t RSVD: 13;
    };
} AON_FAST_0x20A_TYPE;

/* 0x20C
    0       R/W PAD_P2_SHDN[0]                                  1'b1
    1       R/W AON_PAD_P2_E[0]                                 1'b0
    2       R/W PAD_P2_WKPOL[0]                                 1'b0
    3       R/W PAD_P2_WKEN[0]                                  1'b0
    4       R/W AON_PAD_P2_O[0]                                 1'b0
    5       R/W PAD_P2_PDPUC[0]                                 1'b0
    6       R/W AON_PAD_P2_PU[0]                                1'b1
    7       R/W AON_PAD_P2_PU_EN[0]                             1'b1
    8       R/W PAD_P2_SHDN[1]                                  1'b1
    9       R/W AON_PAD_P2_E[1]                                 1'b0
    10      R/W PAD_P2_WKPOL[1]                                 1'b0
    11      R/W PAD_P2_WKEN[1]                                  1'b0
    12      R/W AON_PAD_P2_O[1]                                 1'b0
    13      R/W PAD_P2_PDPUC[1]                                 1'b0
    14      R/W AON_PAD_P2_PU[1]                                1'b0
    15      R/W AON_PAD_P2_PU_EN[1]                             1'b1
 */
typedef union _AON_FAST_0x20C_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P2_SHDN_0: 1;
        uint16_t AON_PAD_P2_E_0: 1;
        uint16_t PAD_P2_WKPOL_0: 1;
        uint16_t PAD_P2_WKEN_0: 1;
        uint16_t AON_PAD_P2_O_0: 1;
        uint16_t PAD_P2_PDPUC_0: 1;
        uint16_t AON_PAD_P2_PU_0: 1;
        uint16_t AON_PAD_P2_PU_EN_0: 1;
        uint16_t PAD_P2_SHDN_1: 1;
        uint16_t AON_PAD_P2_E_1: 1;
        uint16_t PAD_P2_WKPOL_1: 1;
        uint16_t PAD_P2_WKEN_1: 1;
        uint16_t AON_PAD_P2_O_1: 1;
        uint16_t PAD_P2_PDPUC_1: 1;
        uint16_t AON_PAD_P2_PU_1: 1;
        uint16_t AON_PAD_P2_PU_EN_1: 1;
    };
} AON_FAST_0x20C_TYPE;

/* 0x20E
    0       R/W PAD_P2_SHDN[2]                                  1'b1
    1       R/W AON_PAD_P2_E[2]                                 1'b0
    2       R/W PAD_P2_WKPOL[2]                                 1'b0
    3       R/W PAD_P2_WKEN[2]                                  1'b0
    4       R/W AON_PAD_P2_O[2]                                 1'b0
    5       R/W PAD_P2_PDPUC[2]                                 1'b0
    6       R/W AON_PAD_P2_PU[2]                                1'b0
    7       R/W AON_PAD_P2_PU_EN[2]                             1'b1
    8       R/W PAD_P2_SHDN[3]                                  1'b1
    9       R/W AON_PAD_P2_E[3]                                 1'b0
    10      R/W PAD_P2_WKPOL[3]                                 1'b0
    11      R/W PAD_P2_WKEN[3]                                  1'b0
    12      R/W AON_PAD_P2_O[3]                                 1'b0
    13      R/W PAD_P2_PDPUC[3]                                 1'b0
    14      R/W AON_PAD_P2_PU[3]                                1'b0
    15      R/W AON_PAD_P2_PU_EN[3]                             1'b1
 */
typedef union _AON_FAST_0x20E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P2_SHDN_2: 1;
        uint16_t AON_PAD_P2_E_2: 1;
        uint16_t PAD_P2_WKPOL_2: 1;
        uint16_t PAD_P2_WKEN_2: 1;
        uint16_t AON_PAD_P2_O_2: 1;
        uint16_t PAD_P2_PDPUC_2: 1;
        uint16_t AON_PAD_P2_PU_2: 1;
        uint16_t AON_PAD_P2_PU_EN_2: 1;
        uint16_t PAD_P2_SHDN_3: 1;
        uint16_t AON_PAD_P2_E_3: 1;
        uint16_t PAD_P2_WKPOL_3: 1;
        uint16_t PAD_P2_WKEN_3: 1;
        uint16_t AON_PAD_P2_O_3: 1;
        uint16_t PAD_P2_PDPUC_3: 1;
        uint16_t AON_PAD_P2_PU_3: 1;
        uint16_t AON_PAD_P2_PU_EN_3: 1;
    };
} AON_FAST_0x20E_TYPE;

/* 0x210
    0       R/W PAD_P2_SHDN[4]                                  1'b1
    1       R/W AON_PAD_P2_E[4]                                 1'b0
    2       R/W PAD_P2_WKPOL[4]                                 1'b0
    3       R/W PAD_P2_WKEN[4]                                  1'b0
    4       R/W AON_PAD_P2_O[4]                                 1'b0
    5       R/W PAD_P2_PDPUC[4]                                 1'b0
    6       R/W AON_PAD_P2_PU[4]                                1'b0
    7       R/W AON_PAD_P2_PU_EN[4]                             1'b1
    8       R/W PAD_P2_SHDN[5]                                  1'b1
    9       R/W AON_PAD_P2_E[5]                                 1'b0
    10      R/W PAD_P2_WKPOL[5]                                 1'b0
    11      R/W PAD_P2_WKEN[5]                                  1'b0
    12      R/W AON_PAD_P2_O[5]                                 1'b0
    13      R/W PAD_P2_PDPUC[5]                                 1'b0
    14      R/W AON_PAD_P2_PU[5]                                1'b0
    15      R/W AON_PAD_P2_PU_EN[5]                             1'b1
 */
typedef union _AON_FAST_0x210_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P2_SHDN_4: 1;
        uint16_t AON_PAD_P2_E_4: 1;
        uint16_t PAD_P2_WKPOL_4: 1;
        uint16_t PAD_P2_WKEN_4: 1;
        uint16_t AON_PAD_P2_O_4: 1;
        uint16_t PAD_P2_PDPUC_4: 1;
        uint16_t AON_PAD_P2_PU_4: 1;
        uint16_t AON_PAD_P2_PU_EN_4: 1;
        uint16_t PAD_P2_SHDN_5: 1;
        uint16_t AON_PAD_P2_E_5: 1;
        uint16_t PAD_P2_WKPOL_5: 1;
        uint16_t PAD_P2_WKEN_5: 1;
        uint16_t AON_PAD_P2_O_5: 1;
        uint16_t PAD_P2_PDPUC_5: 1;
        uint16_t AON_PAD_P2_PU_5: 1;
        uint16_t AON_PAD_P2_PU_EN_5: 1;
    };
} AON_FAST_0x210_TYPE;

/* 0x212
    0       R/W PAD_P2_SHDN[6]                                  1'b1
    1       R/W AON_PAD_P2_E[6]                                 1'b0
    2       R/W PAD_P2_WKPOL[6]                                 1'b0
    3       R/W PAD_P2_WKEN[6]                                  1'b0
    4       R/W AON_PAD_P2_O[6]                                 1'b0
    5       R/W PAD_P2_PDPUC[6]                                 1'b0
    6       R/W AON_PAD_P2_PU[6]                                1'b0
    7       R/W AON_PAD_P2_PU_EN[6]                             1'b1
    8       R/W PAD_P2_SHDN[7]                                  1'b1
    9       R/W AON_PAD_P2_E[7]                                 1'b0
    10      R/W PAD_P2_WKPOL[7]                                 1'b0
    11      R/W PAD_P2_WKEN[7]                                  1'b0
    12      R/W AON_PAD_P2_O[7]                                 1'b0
    13      R/W PAD_P2_PDPUC[7]                                 1'b0
    14      R/W AON_PAD_P2_PU[7]                                1'b0
    15      R/W AON_PAD_P2_PU_EN[7]                             1'b1
 */
typedef union _AON_FAST_0x212_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P2_SHDN_6: 1;
        uint16_t AON_PAD_P2_E_6: 1;
        uint16_t PAD_P2_WKPOL_6: 1;
        uint16_t PAD_P2_WKEN_6: 1;
        uint16_t AON_PAD_P2_O_6: 1;
        uint16_t PAD_P2_PDPUC_6: 1;
        uint16_t AON_PAD_P2_PU_6: 1;
        uint16_t AON_PAD_P2_PU_EN_6: 1;
        uint16_t PAD_P2_SHDN_7: 1;
        uint16_t AON_PAD_P2_E_7: 1;
        uint16_t PAD_P2_WKPOL_7: 1;
        uint16_t PAD_P2_WKEN_7: 1;
        uint16_t AON_PAD_P2_O_7: 1;
        uint16_t PAD_P2_PDPUC_7: 1;
        uint16_t AON_PAD_P2_PU_7: 1;
        uint16_t AON_PAD_P2_PU_EN_7: 1;
    };
} AON_FAST_0x212_TYPE;

/* 0x214
    07:00   R/W AON_PAD_P2_S[7:0]                               8'h0
    10:08   R/W PAD_P2_1_CFG                                    3'b0
    11:11   R   RSVD0                                           1'b0
    14:12   R/W PAD_P2_0_CFG                                    3'b0
    15:15   R   RSVD1                                           1'b0
 */
typedef union _AON_FAST_0x214_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_PAD_P2_S_7_0: 8;
        uint16_t PAD_P2_1_CFG: 3;
        uint16_t RSVD0: 1;
        uint16_t PAD_P2_0_CFG: 3;
        uint16_t RSVD1: 1;
    };
} AON_FAST_0x214_TYPE;

/* 0x216
    02:00   R/W PAD_P2_2_CFG                                    3'b0
    15:03   R   RSVD                                            13'h0
 */
typedef union _AON_FAST_0x216_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P2_2_CFG: 3;
        uint16_t RSVD: 13;
    };
} AON_FAST_0x216_TYPE;

/* 0x218
    0       R/W PAD_P3_SHDN[0]                                  1'b1
    1       R/W AON_PAD_P3_E[0]                                 1'b0
    2       R/W PAD_P3_WKPOL[0]                                 1'b0
    3       R/W PAD_P3_WKEN[0]                                  1'b0
    4       R/W AON_PAD_P3_O[0]                                 1'b0
    5       R/W PAD_P3_PDPUC[0]                                 1'b0
    6       R/W AON_PAD_P3_PU[0]                                1'b0
    7       R/W AON_PAD_P3_PU_EN[0]                             1'b1
    8       R/W PAD_P3_SHDN[1]                                  1'b1
    9       R/W AON_PAD_P3_E[1]                                 1'b0
    10      R/W PAD_P3_WKPOL[1]                                 1'b0
    11      R/W PAD_P3_WKEN[1]                                  1'b0
    12      R/W AON_PAD_P3_O[1]                                 1'b0
    13      R/W PAD_P3_PDPUC[1]                                 1'b0
    14      R/W AON_PAD_P3_PU[1]                                1'b0
    15      R/W AON_PAD_P3_PU_EN[1]                             1'b1
 */
typedef union _AON_FAST_0x218_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P3_SHDN_0: 1;
        uint16_t AON_PAD_P3_E_0: 1;
        uint16_t PAD_P3_WKPOL_0: 1;
        uint16_t PAD_P3_WKEN_0: 1;
        uint16_t AON_PAD_P3_O_0: 1;
        uint16_t PAD_P3_PDPUC_0: 1;
        uint16_t AON_PAD_P3_PU_0: 1;
        uint16_t AON_PAD_P3_PU_EN_0: 1;
        uint16_t PAD_P3_SHDN_1: 1;
        uint16_t AON_PAD_P3_E_1: 1;
        uint16_t PAD_P3_WKPOL_1: 1;
        uint16_t PAD_P3_WKEN_1: 1;
        uint16_t AON_PAD_P3_O_1: 1;
        uint16_t PAD_P3_PDPUC_1: 1;
        uint16_t AON_PAD_P3_PU_1: 1;
        uint16_t AON_PAD_P3_PU_EN_1: 1;
    };
} AON_FAST_0x218_TYPE;

/* 0x21A
    0       R/W PAD_P3_SHDN[2]                                  1'b1
    1       R/W AON_PAD_P3_E[2]                                 1'b0
    2       R/W PAD_P3_WKPOL[2]                                 1'b0
    3       R/W PAD_P3_WKEN[2]                                  1'b0
    4       R/W AON_PAD_P3_O[2]                                 1'b0
    5       R/W PAD_P3_PDPUC[2]                                 1'b0
    6       R/W AON_PAD_P3_PU[2]                                1'b0
    7       R/W AON_PAD_P3_PU_EN[2]                             1'b1
    8       R/W PAD_P3_SHDN[3]                                  1'b1
    9       R/W AON_PAD_P3_E[3]                                 1'b0
    10      R/W PAD_P3_WKPOL[3]                                 1'b0
    11      R/W PAD_P3_WKEN[3]                                  1'b0
    12      R/W AON_PAD_P3_O[3]                                 1'b0
    13      R/W PAD_P3_PDPUC[3]                                 1'b0
    14      R/W AON_PAD_P3_PU[3]                                1'b0
    15      R/W AON_PAD_P3_PU_EN[3]                             1'b1
 */
typedef union _AON_FAST_0x21A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P3_SHDN_2: 1;
        uint16_t AON_PAD_P3_E_2: 1;
        uint16_t PAD_P3_WKPOL_2: 1;
        uint16_t PAD_P3_WKEN_2: 1;
        uint16_t AON_PAD_P3_O_2: 1;
        uint16_t PAD_P3_PDPUC_2: 1;
        uint16_t AON_PAD_P3_PU_2: 1;
        uint16_t AON_PAD_P3_PU_EN_2: 1;
        uint16_t PAD_P3_SHDN_3: 1;
        uint16_t AON_PAD_P3_E_3: 1;
        uint16_t PAD_P3_WKPOL_3: 1;
        uint16_t PAD_P3_WKEN_3: 1;
        uint16_t AON_PAD_P3_O_3: 1;
        uint16_t PAD_P3_PDPUC_3: 1;
        uint16_t AON_PAD_P3_PU_3: 1;
        uint16_t AON_PAD_P3_PU_EN_3: 1;
    };
} AON_FAST_0x21A_TYPE;

/* 0x21C
    0       R/W PAD_P3_SHDN[4]                                  1'b1
    1       R/W AON_PAD_P3_E[4]                                 1'b0
    2       R/W PAD_P3_WKPOL[4]                                 1'b0
    3       R/W PAD_P3_WKEN[4]                                  1'b0
    4       R/W AON_PAD_P3_O[4]                                 1'b0
    5       R/W PAD_P3_PDPUC[4]                                 1'b0
    6       R/W AON_PAD_P3_PU[4]                                1'b0
    7       R/W AON_PAD_P3_PU_EN[4]                             1'b1
    8       R/W PAD_P3_SHDN[5]                                  1'b1
    9       R/W AON_PAD_P3_E[5]                                 1'b0
    10      R/W PAD_P3_WKPOL[5]                                 1'b0
    11      R/W PAD_P3_WKEN[5]                                  1'b0
    12      R/W AON_PAD_P3_O[5]                                 1'b0
    13      R/W PAD_P3_PDPUC[5]                                 1'b0
    14      R/W AON_PAD_P3_PU[5]                                1'b0
    15      R/W AON_PAD_P3_PU_EN[5]                             1'b1
 */
typedef union _AON_FAST_0x21C_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P3_SHDN_4: 1;
        uint16_t AON_PAD_P3_E_4: 1;
        uint16_t PAD_P3_WKPOL_4: 1;
        uint16_t PAD_P3_WKEN_4: 1;
        uint16_t AON_PAD_P3_O_4: 1;
        uint16_t PAD_P3_PDPUC_4: 1;
        uint16_t AON_PAD_P3_PU_4: 1;
        uint16_t AON_PAD_P3_PU_EN_4: 1;
        uint16_t PAD_P3_SHDN_5: 1;
        uint16_t AON_PAD_P3_E_5: 1;
        uint16_t PAD_P3_WKPOL_5: 1;
        uint16_t PAD_P3_WKEN_5: 1;
        uint16_t AON_PAD_P3_O_5: 1;
        uint16_t PAD_P3_PDPUC_5: 1;
        uint16_t AON_PAD_P3_PU_5: 1;
        uint16_t AON_PAD_P3_PU_EN_5: 1;
    };
} AON_FAST_0x21C_TYPE;

/* 0x21E
    0       R/W PAD_P3_SHDN[6]                                  1'b1
    1       R/W AON_PAD_P3_E[6]                                 1'b0
    2       R/W PAD_P3_WKPOL[6]                                 1'b0
    3       R/W PAD_P3_WKEN[6]                                  1'b0
    4       R/W AON_PAD_P3_O[6]                                 1'b0
    5       R/W PAD_P3_PDPUC[6]                                 1'b0
    6       R/W AON_PAD_P3_PU[6]                                1'b0
    7       R/W AON_PAD_P3_PU_EN[6]                             1'b1
    8       R/W PAD_P3_SHDN[7]                                  1'b1
    9       R/W AON_PAD_P3_E[7]                                 1'b0
    10      R/W PAD_P3_WKPOL[7]                                 1'b0
    11      R/W PAD_P3_WKEN[7]                                  1'b0
    12      R/W AON_PAD_P3_O[7]                                 1'b0
    13      R/W PAD_P3_PDPUC[7]                                 1'b0
    14      R/W AON_PAD_P3_PU[7]                                1'b0
    15      R/W AON_PAD_P3_PU_EN[7]                             1'b1
 */
typedef union _AON_FAST_0x21E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P3_SHDN_6: 1;
        uint16_t AON_PAD_P3_E_6: 1;
        uint16_t PAD_P3_WKPOL_6: 1;
        uint16_t PAD_P3_WKEN_6: 1;
        uint16_t AON_PAD_P3_O_6: 1;
        uint16_t PAD_P3_PDPUC_6: 1;
        uint16_t AON_PAD_P3_PU_6: 1;
        uint16_t AON_PAD_P3_PU_EN_6: 1;
        uint16_t PAD_P3_SHDN_7: 1;
        uint16_t AON_PAD_P3_E_7: 1;
        uint16_t PAD_P3_WKPOL_7: 1;
        uint16_t PAD_P3_WKEN_7: 1;
        uint16_t AON_PAD_P3_O_7: 1;
        uint16_t PAD_P3_PDPUC_7: 1;
        uint16_t AON_PAD_P3_PU_7: 1;
        uint16_t AON_PAD_P3_PU_EN_7: 1;
    };
} AON_FAST_0x21E_TYPE;

/* 0x220
    07:00   R/W AON_PAD_P3_S[7:0]                               8'h0
    15:08   R   RSVD                                            8'h0
 */
typedef union _AON_FAST_0x220_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_PAD_P3_S_7_0: 8;
        uint16_t RSVD: 8;
    };
} AON_FAST_0x220_TYPE;

/* 0x222
    0       R/W PAD_P4_SHDN[0]                                  1'b1
    1       R/W AON_PAD_P4_E[0]                                 1'b0
    2       R/W PAD_P4_WKPOL[0]                                 1'b0
    3       R/W PAD_P4_WKEN[0]                                  1'b0
    4       R/W AON_PAD_P4_O[0]                                 1'b0
    5       R/W PAD_P4_PDPUC[0]                                 1'b0
    6       R/W AON_PAD_P4_PU[0]                                1'b0
    7       R/W AON_PAD_P4_PU_EN[0]                             1'b1
    8       R/W PAD_P4_SHDN[1]                                  1'b1
    1       R/W AON_PAD_P4_E[1]                                 1'b0
    10      R/W PAD_P4_WKPOL[1]                                 1'b0
    11      R/W PAD_P4_WKEN[1]                                  1'b0
    12      R/W AON_PAD_P4_O[1]                                 1'b0
    13      R/W PAD_P4_PDPUC[1]                                 1'b0
    14      R/W AON_PAD_P4_PU[1]                                1'b0
    15      R/W AON_PAD_P4_PU_EN[1]                             1'b1
 */
typedef union _AON_FAST_0x222_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P4_SHDN_0: 1;
        uint16_t AON_PAD_P4_E_0: 1;
        uint16_t PAD_P4_WKPOL_0: 1;
        uint16_t PAD_P4_WKEN_0: 1;
        uint16_t AON_PAD_P4_O_0: 1;
        uint16_t PAD_P4_PDPUC_0: 1;
        uint16_t AON_PAD_P4_PU_0: 1;
        uint16_t AON_PAD_P4_PU_EN_0: 1;
        uint16_t PAD_P4_SHDN_1: 1;
        uint16_t AON_PAD_P4_E_1: 1;
        uint16_t PAD_P4_WKPOL_1: 1;
        uint16_t PAD_P4_WKEN_1: 1;
        uint16_t AON_PAD_P4_O_1: 1;
        uint16_t PAD_P4_PDPUC_1: 1;
        uint16_t AON_PAD_P4_PU_1: 1;
        uint16_t AON_PAD_P4_PU_EN_1: 1;
    };
} AON_FAST_0x222_TYPE;

/* 0x224
    0       R/W PAD_P4_SHDN[2]                                  1'b1
    1       R/W AON_PAD_P4_E[2]                                 1'b0
    2       R/W PAD_P4_WKPOL[2]                                 1'b0
    3       R/W PAD_P4_WKEN[2]                                  1'b0
    4       R/W AON_PAD_P4_O[2]                                 1'b0
    5       R/W PAD_P4_PDPUC[2]                                 1'b0
    6       R/W AON_PAD_P4_PU[2]                                1'b0
    7       R/W AON_PAD_P4_PU_EN[2]                             1'b1
    8       R/W PAD_P4_SHDN[3]                                  1'b1
    9       R/W AON_PAD_P4_E[3]                                 1'b0
    10      R/W PAD_P4_WKPOL[3]                                 1'b0
    11      R/W PAD_P4_WKEN[3]                                  1'b0
    12      R/W AON_PAD_P4_O[3]                                 1'b0
    13      R/W PAD_P4_PDPUC[3]                                 1'b0
    14      R/W AON_PAD_P4_PU[3]                                1'b0
    15      R/W AON_PAD_P4_PU_EN[3]                             1'b1
 */
typedef union _AON_FAST_0x224_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P4_SHDN_2: 1;
        uint16_t AON_PAD_P4_E_2: 1;
        uint16_t PAD_P4_WKPOL_2: 1;
        uint16_t PAD_P4_WKEN_2: 1;
        uint16_t AON_PAD_P4_O_2: 1;
        uint16_t PAD_P4_PDPUC_2: 1;
        uint16_t AON_PAD_P4_PU_2: 1;
        uint16_t AON_PAD_P4_PU_EN_2: 1;
        uint16_t PAD_P4_SHDN_3: 1;
        uint16_t AON_PAD_P4_E_3: 1;
        uint16_t PAD_P4_WKPOL_3: 1;
        uint16_t PAD_P4_WKEN_3: 1;
        uint16_t AON_PAD_P4_O_3: 1;
        uint16_t PAD_P4_PDPUC_3: 1;
        uint16_t AON_PAD_P4_PU_3: 1;
        uint16_t AON_PAD_P4_PU_EN_3: 1;
    };
} AON_FAST_0x224_TYPE;

/* 0x226
    0       R/W PAD_P4_SHDN[4]                                  1'b1
    1       R/W AON_PAD_P4_E[4]                                 1'b0
    2       R/W PAD_P4_WKPOL[4]                                 1'b0
    3       R/W PAD_P4_WKEN[4]                                  1'b0
    4       R/W AON_PAD_P4_O[4]                                 1'b0
    5       R/W PAD_P4_PDPUC[4]                                 1'b0
    6       R/W AON_PAD_P4_PU[4]                                1'b0
    7       R/W AON_PAD_P4_PU_EN[4]                             1'b1
    8       R/W PAD_P4_SHDN[5]                                  1'b1
    9       R/W AON_PAD_P4_E[5]                                 1'b0
    10      R/W PAD_P4_WKPOL[5]                                 1'b0
    11      R/W PAD_P4_WKEN[5]                                  1'b0
    12      R/W AON_PAD_P4_O[5]                                 1'b0
    13      R/W PAD_P4_PDPUC[5]                                 1'b0
    14      R/W AON_PAD_P4_PU[5]                                1'b0
    15      R/W AON_PAD_P4_PU_EN[5]                             1'b1
 */
typedef union _AON_FAST_0x226_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P4_SHDN_4: 1;
        uint16_t AON_PAD_P4_E_4: 1;
        uint16_t PAD_P4_WKPOL_4: 1;
        uint16_t PAD_P4_WKEN_4: 1;
        uint16_t AON_PAD_P4_O_4: 1;
        uint16_t PAD_P4_PDPUC_4: 1;
        uint16_t AON_PAD_P4_PU_4: 1;
        uint16_t AON_PAD_P4_PU_EN_4: 1;
        uint16_t PAD_P4_SHDN_5: 1;
        uint16_t AON_PAD_P4_E_5: 1;
        uint16_t PAD_P4_WKPOL_5: 1;
        uint16_t PAD_P4_WKEN_5: 1;
        uint16_t AON_PAD_P4_O_5: 1;
        uint16_t PAD_P4_PDPUC_5: 1;
        uint16_t AON_PAD_P4_PU_5: 1;
        uint16_t AON_PAD_P4_PU_EN_5: 1;
    };
} AON_FAST_0x226_TYPE;

/* 0x228
    0       R/W PAD_P4_SHDN[6]                                  1'b1
    1       R/W AON_PAD_P4_E[6]                                 1'b0
    2       R/W PAD_P4_WKPOL[6]                                 1'b0
    3       R/W PAD_P4_WKEN[6]                                  1'b0
    4       R/W AON_PAD_P4_O[6]                                 1'b0
    5       R/W PAD_P4_PDPUC[6]                                 1'b0
    6       R/W AON_PAD_P4_PU[6]                                1'b0
    7       R/W AON_PAD_P4_PU_EN[6]                             1'b1
    8       R/W PAD_P4_SHDN[7]                                  1'b1
    9       R/W AON_PAD_P4_E[7]                                 1'b0
    10      R/W PAD_P4_WKPOL[7]                                 1'b0
    11      R/W PAD_P4_WKEN[7]                                  1'b0
    12      R/W AON_PAD_P4_O[7]                                 1'b0
    13      R/W PAD_P4_PDPUC[7]                                 1'b0
    14      R/W AON_PAD_P4_PU[7]                                1'b0
    15      R/W AON_PAD_P4_PU_EN[7]                             1'b1
 */
typedef union _AON_FAST_0x228_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P4_SHDN_6: 1;
        uint16_t AON_PAD_P4_E_6: 1;
        uint16_t PAD_P4_WKPOL_6: 1;
        uint16_t PAD_P4_WKEN_6: 1;
        uint16_t AON_PAD_P4_O_6: 1;
        uint16_t PAD_P4_PDPUC_6: 1;
        uint16_t AON_PAD_P4_PU_6: 1;
        uint16_t AON_PAD_P4_PU_EN_6: 1;
        uint16_t PAD_P4_SHDN_7: 1;
        uint16_t AON_PAD_P4_E_7: 1;
        uint16_t PAD_P4_WKPOL_7: 1;
        uint16_t PAD_P4_WKEN_7: 1;
        uint16_t AON_PAD_P4_O_7: 1;
        uint16_t PAD_P4_PDPUC_7: 1;
        uint16_t AON_PAD_P4_PU_7: 1;
        uint16_t AON_PAD_P4_PU_EN_7: 1;
    };
} AON_FAST_0x228_TYPE;

/* 0x22A
    07:00   R/W AON_PAD_P4_S[7:0]                               8'h0
    15:08   R   RSVD                                            8'h0
 */
typedef union _AON_FAST_0x22A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_PAD_P4_S_7_0: 8;
        uint16_t RSVD: 8;
    };
} AON_FAST_0x22A_TYPE;

/* 0x22C
    0       R/W PAD_P5_SHDN[0]                                  1'b1
    1       R/W AON_PAD_P5_E[0]                                 1'b0
    2       R/W PAD_P5_WKPOL[0]                                 1'b0
    3       R/W PAD_P5_WKEN[0]                                  1'b0
    4       R/W AON_PAD_P5_O[0]                                 1'b0
    5       R/W PAD_P5_PDPUC[0]                                 1'b0
    6       R/W AON_PAD_P5_PU[0]                                1'b0
    7       R/W AON_PAD_P5_PU_EN[0]                             1'b1
    8       R/W PAD_P5_SHDN[1]                                  1'b1
    9       R/W AON_PAD_P5_E[1]                                 1'b0
    10      R/W PAD_P5_WKPOL[1]                                 1'b0
    11      R/W PAD_P5_WKEN[1]                                  1'b0
    12      R/W AON_PAD_P5_O[1]                                 1'b0
    13      R/W PAD_P5_PDPUC[1]                                 1'b0
    14      R/W AON_PAD_P5_PU[1]                                1'b0
    15      R/W AON_PAD_P5_PU_EN[1]                             1'b1
 */
typedef union _AON_FAST_0x22C_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P5_SHDN_0: 1;
        uint16_t AON_PAD_P5_E_0: 1;
        uint16_t PAD_P5_WKPOL_0: 1;
        uint16_t PAD_P5_WKEN_0: 1;
        uint16_t AON_PAD_P5_O_0: 1;
        uint16_t PAD_P5_PDPUC_0: 1;
        uint16_t AON_PAD_P5_PU_0: 1;
        uint16_t AON_PAD_P5_PU_EN_0: 1;
        uint16_t PAD_P5_SHDN_1: 1;
        uint16_t AON_PAD_P5_E_1: 1;
        uint16_t PAD_P5_WKPOL_1: 1;
        uint16_t PAD_P5_WKEN_1: 1;
        uint16_t AON_PAD_P5_O_1: 1;
        uint16_t PAD_P5_PDPUC_1: 1;
        uint16_t AON_PAD_P5_PU_1: 1;
        uint16_t AON_PAD_P5_PU_EN_1: 1;
    };
} AON_FAST_0x22C_TYPE;

/* 0x22E
    0       R/W PAD_P5_SHDN[2]                                  1'b1
    1       R/W AON_PAD_P5_E[2]                                 1'b0
    2       R/W PAD_P5_WKPOL[2]                                 1'b0
    3       R/W PAD_P5_WKEN[2]                                  1'b0
    4       R/W AON_PAD_P5_O[2]                                 1'b0
    5       R/W PAD_P5_PDPUC[2]                                 1'b0
    6       R/W AON_PAD_P5_PU[2]                                1'b0
    7       R/W AON_PAD_P5_PU_EN[2]                             1'b1
    8       R/W PAD_P5_SHDN[3]                                  1'b1
    9       R/W AON_PAD_P5_E[3]                                 1'b0
    10      R/W PAD_P5_WKPOL[3]                                 1'b0
    11      R/W PAD_P5_WKEN[3]                                  1'b0
    12      R/W AON_PAD_P5_O[3]                                 1'b0
    13      R/W PAD_P5_PDPUC[3]                                 1'b0
    14      R/W AON_PAD_P5_PU[3]                                1'b0
    15      R/W AON_PAD_P5_PU_EN[3]                             1'b1
 */
typedef union _AON_FAST_0x22E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P5_SHDN_2: 1;
        uint16_t AON_PAD_P5_E_2: 1;
        uint16_t PAD_P5_WKPOL_2: 1;
        uint16_t PAD_P5_WKEN_2: 1;
        uint16_t AON_PAD_P5_O_2: 1;
        uint16_t PAD_P5_PDPUC_2: 1;
        uint16_t AON_PAD_P5_PU_2: 1;
        uint16_t AON_PAD_P5_PU_EN_2: 1;
        uint16_t PAD_P5_SHDN_3: 1;
        uint16_t AON_PAD_P5_E_3: 1;
        uint16_t PAD_P5_WKPOL_3: 1;
        uint16_t PAD_P5_WKEN_3: 1;
        uint16_t AON_PAD_P5_O_3: 1;
        uint16_t PAD_P5_PDPUC_3: 1;
        uint16_t AON_PAD_P5_PU_3: 1;
        uint16_t AON_PAD_P5_PU_EN_3: 1;
    };
} AON_FAST_0x22E_TYPE;

/* 0x230
    0       R/W PAD_P5_SHDN[4]                                  1'b1
    1       R/W AON_PAD_P5_E[4]                                 1'b0
    2       R/W PAD_P5_WKPOL[4]                                 1'b0
    3       R/W PAD_P5_WKEN[4]                                  1'b0
    4       R/W AON_PAD_P5_O[4]                                 1'b0
    5       R/W PAD_P5_PDPUC[4]                                 1'b0
    6       R/W AON_PAD_P5_PU[4]                                1'b0
    7       R/W AON_PAD_P5_PU_EN[4]                             1'b1
    8       R/W PAD_P5_SHDN[5]                                  1'b1
    9       R/W AON_PAD_P5_E[5]                                 1'b0
    10      R/W PAD_P5_WKPOL[5]                                 1'b0
    11      R/W PAD_P5_WKEN[5]                                  1'b0
    12      R/W AON_PAD_P5_O[5]                                 1'b0
    13      R/W PAD_P5_PDPUC[5]                                 1'b0
    14      R/W AON_PAD_P5_PU[5]                                1'b0
    15      R/W AON_PAD_P5_PU_EN[5]                             1'b1
 */
typedef union _AON_FAST_0x230_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P5_SHDN_4: 1;
        uint16_t AON_PAD_P5_E_4: 1;
        uint16_t PAD_P5_WKPOL_4: 1;
        uint16_t PAD_P5_WKEN_4: 1;
        uint16_t AON_PAD_P5_O_4: 1;
        uint16_t PAD_P5_PDPUC_4: 1;
        uint16_t AON_PAD_P5_PU_4: 1;
        uint16_t AON_PAD_P5_PU_EN_4: 1;
        uint16_t PAD_P5_SHDN_5: 1;
        uint16_t AON_PAD_P5_E_5: 1;
        uint16_t PAD_P5_WKPOL_5: 1;
        uint16_t PAD_P5_WKEN_5: 1;
        uint16_t AON_PAD_P5_O_5: 1;
        uint16_t PAD_P5_PDPUC_5: 1;
        uint16_t AON_PAD_P5_PU_5: 1;
        uint16_t AON_PAD_P5_PU_EN_5: 1;
    };
} AON_FAST_0x230_TYPE;

/* 0x232
    0       R/W PAD_P5_SHDN[6]                                  1'b1
    1       R/W AON_PAD_P5_E[6]                                 1'b0
    2       R/W PAD_P5_WKPOL[6]                                 1'b0
    3       R/W PAD_P5_WKEN[6]                                  1'b0
    4       R/W AON_PAD_P5_O[6]                                 1'b0
    5       R/W PAD_P5_PDPUC[6]                                 1'b0
    6       R/W AON_PAD_P5_PU[6]                                1'b0
    7       R/W AON_PAD_P5_PU_EN[6]                             1'b1
    8       R/W PAD_P5_SHDN[7]                                  1'b1
    9       R/W AON_PAD_P5_E[7]                                 1'b0
    10      R/W PAD_P5_WKPOL[7]                                 1'b0
    11      R/W PAD_P5_WKEN[7]                                  1'b0
    12      R/W AON_PAD_P5_O[7]                                 1'b0
    13      R/W PAD_P5_PDPUC[7]                                 1'b0
    14      R/W AON_PAD_P5_PU[7]                                1'b0
    15      R/W AON_PAD_P5_PU_EN[7]                             1'b1
 */
typedef union _AON_FAST_0x232_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P5_SHDN_6: 1;
        uint16_t AON_PAD_P5_E_6: 1;
        uint16_t PAD_P5_WKPOL_6: 1;
        uint16_t PAD_P5_WKEN_6: 1;
        uint16_t AON_PAD_P5_O_6: 1;
        uint16_t PAD_P5_PDPUC_6: 1;
        uint16_t AON_PAD_P5_PU_6: 1;
        uint16_t AON_PAD_P5_PU_EN_6: 1;
        uint16_t PAD_P5_SHDN_7: 1;
        uint16_t AON_PAD_P5_E_7: 1;
        uint16_t PAD_P5_WKPOL_7: 1;
        uint16_t PAD_P5_WKEN_7: 1;
        uint16_t AON_PAD_P5_O_7: 1;
        uint16_t PAD_P5_PDPUC_7: 1;
        uint16_t AON_PAD_P5_PU_7: 1;
        uint16_t AON_PAD_P5_PU_EN_7: 1;
    };
} AON_FAST_0x232_TYPE;

/* 0x234
    07:00   R/W AON_PAD_P5_S[7:0]                               8'h0
    15:08   R   RSVD                                            8'h0
 */
typedef union _AON_FAST_0x234_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_PAD_P5_S_7_0: 8;
        uint16_t RSVD: 8;
    };
} AON_FAST_0x234_TYPE;

/* 0x236
    0       R/W PAD_P6_SHDN[0]                                  1'b1
    1       R/W AON_PAD_P6_E[0]                                 1'b0
    2       R/W PAD_P6_WKPOL[0]                                 1'b0
    3       R/W PAD_P6_WKEN[0]                                  1'b0
    4       R/W AON_PAD_P6_O[0]                                 1'b0
    5       R/W PAD_P6_PDPUC[0]                                 1'b0
    6       R/W AON_PAD_P6_PU[0]                                1'b0
    7       R/W AON_PAD_P6_PU_EN[0]                             1'b1
    8       R/W PAD_P6_SHDN[1]                                  1'b1
    9       R/W AON_PAD_P6_E[1]                                 1'b0
    10      R/W PAD_P6_WKPOL[1]                                 1'b0
    11      R/W PAD_P6_WKEN[1]                                  1'b0
    12      R/W AON_PAD_P6_O[1]                                 1'b0
    13      R/W PAD_P6_PDPUC[1]                                 1'b0
    14      R/W AON_PAD_P6_PU[1]                                1'b0
    15      R/W AON_PAD_P6_PU_EN[1]                             1'b1
 */
typedef union _AON_FAST_0x236_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P6_SHDN_0: 1;
        uint16_t AON_PAD_P6_E_0: 1;
        uint16_t PAD_P6_WKPOL_0: 1;
        uint16_t PAD_P6_WKEN_0: 1;
        uint16_t AON_PAD_P6_O_0: 1;
        uint16_t PAD_P6_PDPUC_0: 1;
        uint16_t AON_PAD_P6_PU_0: 1;
        uint16_t AON_PAD_P6_PU_EN_0: 1;
        uint16_t PAD_P6_SHDN_1: 1;
        uint16_t AON_PAD_P6_E_1: 1;
        uint16_t PAD_P6_WKPOL_1: 1;
        uint16_t PAD_P6_WKEN_1: 1;
        uint16_t AON_PAD_P6_O_1: 1;
        uint16_t PAD_P6_PDPUC_1: 1;
        uint16_t AON_PAD_P6_PU_1: 1;
        uint16_t AON_PAD_P6_PU_EN_1: 1;
    };
} AON_FAST_0x236_TYPE;

/* 0x238
    07:00   R/W AON_PAD_P6_S[7:0]                               8'h0
    15:08   R   RSVD                                            8'h0
 */
typedef union _AON_FAST_0x238_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_PAD_P6_S_7_0: 8;
        uint16_t RSVD: 8;
    };
} AON_FAST_0x238_TYPE;

/* 0x23A
    0       R/W PAD_P7_SHDN[0]                                  1'b1
    1       R/W AON_PAD_P7_E[0]                                 1'b0
    2       R/W PAD_P7_WKPOL[0]                                 1'b0
    3       R/W PAD_P7_WKEN[0]                                  1'b0
    4       R/W AON_PAD_P7_O[0]                                 1'b0
    5       R/W PAD_P7_PDPUC[0]                                 1'b0
    6       R/W AON_PAD_P7_PU[0]                                1'b0
    7       R/W AON_PAD_P7_PU_EN[0]                             1'b1
    8       R/W PAD_P7_SHDN[1]                                  1'b1
    9       R/W AON_PAD_P7_E[1]                                 1'b0
    10      R/W PAD_P7_WKPOL[1]                                 1'b0
    11      R/W PAD_P7_WKEN[1]                                  1'b0
    12      R/W AON_PAD_P7_O[1]                                 1'b0
    13      R/W PAD_P7_PDPUC[1]                                 1'b0
    14      R/W AON_PAD_P7_PU[1]                                1'b0
    15      R/W AON_PAD_P7_PU_EN[1]                             1'b1
 */
typedef union _AON_FAST_0x23A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P7_SHDN_0: 1;
        uint16_t AON_PAD_P7_E_0: 1;
        uint16_t PAD_P7_WKPOL_0: 1;
        uint16_t PAD_P7_WKEN_0: 1;
        uint16_t AON_PAD_P7_O_0: 1;
        uint16_t PAD_P7_PDPUC_0: 1;
        uint16_t AON_PAD_P7_PU_0: 1;
        uint16_t AON_PAD_P7_PU_EN_0: 1;
        uint16_t PAD_P7_SHDN_1: 1;
        uint16_t AON_PAD_P7_E_1: 1;
        uint16_t PAD_P7_WKPOL_1: 1;
        uint16_t PAD_P7_WKEN_1: 1;
        uint16_t AON_PAD_P7_O_1: 1;
        uint16_t PAD_P7_PDPUC_1: 1;
        uint16_t AON_PAD_P7_PU_1: 1;
        uint16_t AON_PAD_P7_PU_EN_1: 1;
    };
} AON_FAST_0x23A_TYPE;

/* 0x23C
    07:00   R/W AON_PAD_P7_S[7:0]                               8'h0
    15:08   R   RSVD                                            8'h0
 */
typedef union _AON_FAST_0x23C_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_PAD_P7_S_7_0: 8;
        uint16_t RSVD: 8;
    };
} AON_FAST_0x23C_TYPE;

/* 0x23E
    0       R/W PAD_ADC_SHDN[0]                                 1'b1
    1       R/W AON_PAD_ADC_E[0]                                1'b0
    2       R/W PAD_ADC_WKPOL[0]                                1'b0
    3       R/W PAD_ADC_WKEN[0]                                 1'b0
    4       R/W AON_PAD_ADC_O[0]                                1'b0
    5       R/W AON_PAD_ADC_PDPUC[0]                            1'b0
    6       R/W AON_PAD_ADC_PU[0]                               1'b0
    7       R/W AON_PAD_ADC_PU_EN[0]                            1'b1
    8       R/W PAD_ADC_SHDN[1]                                 1'b1
    9       R/W AON_PAD_ADC_E[1]                                1'b0
    10      R/W PAD_ADC_WKPOL[1]                                1'b0
    11      R/W PAD_ADC_WKEN[1]                                 1'b0
    12      R/W AON_PAD_ADC_O[1]                                1'b0
    13      R/W AON_PAD_ADC_PDPUC[1]                            1'b0
    14      R/W AON_PAD_ADC_PU[1]                               1'b0
    15      R/W AON_PAD_ADC_PU_EN[1]                            1'b1
 */
typedef union _AON_FAST_0x23E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_ADC_SHDN_0: 1;
        uint16_t AON_PAD_ADC_E_0: 1;
        uint16_t PAD_ADC_WKPOL_0: 1;
        uint16_t PAD_ADC_WKEN_0: 1;
        uint16_t AON_PAD_ADC_O_0: 1;
        uint16_t AON_PAD_ADC_PDPUC_0: 1;
        uint16_t AON_PAD_ADC_PU_0: 1;
        uint16_t AON_PAD_ADC_PU_EN_0: 1;
        uint16_t PAD_ADC_SHDN_1: 1;
        uint16_t AON_PAD_ADC_E_1: 1;
        uint16_t PAD_ADC_WKPOL_1: 1;
        uint16_t PAD_ADC_WKEN_1: 1;
        uint16_t AON_PAD_ADC_O_1: 1;
        uint16_t AON_PAD_ADC_PDPUC_1: 1;
        uint16_t AON_PAD_ADC_PU_1: 1;
        uint16_t AON_PAD_ADC_PU_EN_1: 1;
    };
} AON_FAST_0x23E_TYPE;

/* 0x240
    0       R/W PAD_ADC_SHDN[2]                                 1'b1
    1       R/W AON_PAD_ADC_E[2]                                1'b0
    2       R/W PAD_ADC_WKPOL[2]                                1'b0
    3       R/W PAD_ADC_WKEN[2]                                 1'b0
    4       R/W AON_PAD_ADC_O[2]                                1'b0
    5       R/W AON_PAD_ADC_PDPUC[2]                            1'b0
    6       R/W AON_PAD_ADC_PU[2]                               1'b0
    7       R/W AON_PAD_ADC_PU_EN[2]                            1'b1
    8       R/W PAD_ADC_SHDN[3]                                 1'b1
    9       R/W AON_PAD_ADC_E[3]                                1'b0
    10      R/W PAD_ADC_WKPOL[3]                                1'b0
    11      R/W PAD_ADC_WKEN[3]                                 1'b0
    12      R/W AON_PAD_ADC_O[3]                                1'b0
    13      R/W AON_PAD_ADC_PDPUC[3]                            1'b0
    14      R/W AON_PAD_ADC_PU[3]                               1'b0
    15      R/W AON_PAD_ADC_PU_EN[3]                            1'b1
 */
typedef union _AON_FAST_0x240_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_ADC_SHDN_2: 1;
        uint16_t AON_PAD_ADC_E_2: 1;
        uint16_t PAD_ADC_WKPOL_2: 1;
        uint16_t PAD_ADC_WKEN_2: 1;
        uint16_t AON_PAD_ADC_O_2: 1;
        uint16_t AON_PAD_ADC_PDPUC_2: 1;
        uint16_t AON_PAD_ADC_PU_2: 1;
        uint16_t AON_PAD_ADC_PU_EN_2: 1;
        uint16_t PAD_ADC_SHDN_3: 1;
        uint16_t AON_PAD_ADC_E_3: 1;
        uint16_t PAD_ADC_WKPOL_3: 1;
        uint16_t PAD_ADC_WKEN_3: 1;
        uint16_t AON_PAD_ADC_O_3: 1;
        uint16_t AON_PAD_ADC_PDPUC_3: 1;
        uint16_t AON_PAD_ADC_PU_3: 1;
        uint16_t AON_PAD_ADC_PU_EN_3: 1;
    };
} AON_FAST_0x240_TYPE;

/* 0x242
    15:0    R   RSVD                                            16'h0
 */
typedef union _AON_FAST_0x242_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_0x242_TYPE;

/* 0x244
    0       R/W PAD_ADC_SHDN[4]                                 1'b1
    1       R/W AON_PAD_ADC_E[4]                                1'b0
    2       R/W PAD_ADC_WKPOL[4]                                1'b0
    3       R/W PAD_ADC_WKEN[4]                                 1'b0
    4       R/W AON_PAD_ADC_O[4]                                1'b0
    5       R/W AON_PAD_ADC_PDPUC[4]                            1'b0
    6       R/W AON_PAD_ADC_PU[4]                               1'b0
    7       R/W AON_PAD_ADC_PU_EN[4]                            1'b1
    8       R/W PAD_ADC_SHDN[5]                                 1'b1
    9       R/W AON_PAD_ADC_E[5]                                1'b0
    10      R/W PAD_ADC_WKPOL[5]                                1'b0
    11      R/W PAD_ADC_WKEN[5]                                 1'b0
    12      R/W AON_PAD_ADC_O[5]                                1'b0
    13      R/W AON_PAD_ADC_PDPUC[5]                            1'b0
    14      R/W AON_PAD_ADC_PU[5]                               1'b0
    15      R/W AON_PAD_ADC_PU_EN[5]                            1'b1
 */
typedef union _AON_FAST_0x244_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_ADC_SHDN_4: 1;
        uint16_t AON_PAD_ADC_E_4: 1;
        uint16_t PAD_ADC_WKPOL_4: 1;
        uint16_t PAD_ADC_WKEN_4: 1;
        uint16_t AON_PAD_ADC_O_4: 1;
        uint16_t AON_PAD_ADC_PDPUC_4: 1;
        uint16_t AON_PAD_ADC_PU_4: 1;
        uint16_t AON_PAD_ADC_PU_EN_4: 1;
        uint16_t PAD_ADC_SHDN_5: 1;
        uint16_t AON_PAD_ADC_E_5: 1;
        uint16_t PAD_ADC_WKPOL_5: 1;
        uint16_t PAD_ADC_WKEN_5: 1;
        uint16_t AON_PAD_ADC_O_5: 1;
        uint16_t AON_PAD_ADC_PDPUC_5: 1;
        uint16_t AON_PAD_ADC_PU_5: 1;
        uint16_t AON_PAD_ADC_PU_EN_5: 1;
    };
} AON_FAST_0x244_TYPE;

/* 0x246
    0       R/W PAD_ADC_SHDN[6]                                 1'b1
    1       R/W AON_PAD_ADC_E[6]                                1'b0
    2       R/W PAD_ADC_WKPOL[6]                                1'b0
    3       R/W PAD_ADC_WKEN[6]                                 1'b0
    4       R/W AON_PAD_ADC_O[6]                                1'b0
    5       R/W AON_PAD_ADC_PDPUC[6]                            1'b0
    6       R/W AON_PAD_ADC_PU[6]                               1'b1
    7       R/W AON_PAD_ADC_PU_EN[6]                            1'b1
    8       R/W PAD_ADC_SHDN[7]                                 1'b1
    9       R/W AON_PAD_ADC_E[7]                                1'b0
    10      R/W PAD_ADC_WKPOL[7]                                1'b0
    11      R/W PAD_ADC_WKEN[7]                                 1'b0
    12      R/W AON_PAD_ADC_O[7]                                1'b0
    13      R/W AON_PAD_ADC_PDPUC[7]                            1'b0
    14      R/W AON_PAD_ADC_PU[7]                               1'b1
    15      R/W AON_PAD_ADC_PU_EN[7]                            1'b1
 */
typedef union _AON_FAST_0x246_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_ADC_SHDN_6: 1;
        uint16_t AON_PAD_ADC_E_6: 1;
        uint16_t PAD_ADC_WKPOL_6: 1;
        uint16_t PAD_ADC_WKEN_6: 1;
        uint16_t AON_PAD_ADC_O_6: 1;
        uint16_t AON_PAD_ADC_PDPUC_6: 1;
        uint16_t AON_PAD_ADC_PU_6: 1;
        uint16_t AON_PAD_ADC_PU_EN_6: 1;
        uint16_t PAD_ADC_SHDN_7: 1;
        uint16_t AON_PAD_ADC_E_7: 1;
        uint16_t PAD_ADC_WKPOL_7: 1;
        uint16_t PAD_ADC_WKEN_7: 1;
        uint16_t AON_PAD_ADC_O_7: 1;
        uint16_t AON_PAD_ADC_PDPUC_7: 1;
        uint16_t AON_PAD_ADC_PU_7: 1;
        uint16_t AON_PAD_ADC_PU_EN_7: 1;
    };
} AON_FAST_0x246_TYPE;

/* 0x248
    07:00   R/W PAD_ADC_S[7:0]                                  8'h0
    10:08   R/W PAD_ADC_0_CFG                                   3'b0
    11      R   RSVD0                                           1'b0
    14:12   R/W PAD_ADC_1_CFG                                   3'b0
    15      R   RSVD1                                           1'b0
 */
typedef union _AON_FAST_0x248_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_ADC_S_7_0: 8;
        uint16_t PAD_ADC_0_CFG: 3;
        uint16_t RSVD0: 1;
        uint16_t PAD_ADC_1_CFG: 3;
        uint16_t RSVD1: 1;
    };
} AON_FAST_0x248_TYPE;

/* 0x24A
    02:00   R/W PAD_ADC_6_CFG                                   3'b0
    03:03   R   RSVD0                                           1'b0
    06:04   R/W PAD_ADC_7_CFG                                   3'b0
    15:07   R   RSVD1                                           9'h0
 */
typedef union _AON_FAST_0x24A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_ADC_6_CFG: 3;
        uint16_t RSVD0: 1;
        uint16_t PAD_ADC_7_CFG: 3;
        uint16_t RSVD1: 9;
    };
} AON_FAST_0x24A_TYPE;

/* 0x24C
    0       R/W PAD_MIC4_N_SHDN                                 1'b1
    1       R/W AON_PAD_MIC4_N_E                                1'b0
    2       R/W PAD_MIC4_N_WKPOL                                1'b0
    3       R/W PAD_MIC4_N_WKEN                                 1'b0
    4       R/W AON_PAD_MIC4_N_O                                1'b0
    5       R/W PAD_MIC4_N_PDPUC                                1'b0
    6       R/W AON_PAD_MIC4_N_PU                               1'b0
    7       R/W AON_PAD_MIC4_N_PU_EN                            1'b1
    8       R/W PAD_MIC4_P_SHDN                                 1'b1
    9       R/W AON_PAD_MIC4_P_E                                1'b0
    10      R/W PAD_MIC4_P_WKPOL                                1'b0
    11      R/W PAD_MIC4_P_WKEN                                 1'b0
    12      R/W AON_PAD_MIC4_P_O                                1'b0
    13      R/W PAD_MIC4_P_PDPUC                                1'b0
    14      R/W AON_PAD_MIC4_P_PU                               1'b0
    15      R/W AON_PAD_MIC4_P_PU_EN                            1'b1
 */
typedef union _AON_FAST_0x24C_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_MIC4_N_SHDN: 1;
        uint16_t AON_PAD_MIC4_N_E: 1;
        uint16_t PAD_MIC4_N_WKPOL: 1;
        uint16_t PAD_MIC4_N_WKEN: 1;
        uint16_t AON_PAD_MIC4_N_O: 1;
        uint16_t PAD_MIC4_N_PDPUC: 1;
        uint16_t AON_PAD_MIC4_N_PU: 1;
        uint16_t AON_PAD_MIC4_N_PU_EN: 1;
        uint16_t PAD_MIC4_P_SHDN: 1;
        uint16_t AON_PAD_MIC4_P_E: 1;
        uint16_t PAD_MIC4_P_WKPOL: 1;
        uint16_t PAD_MIC4_P_WKEN: 1;
        uint16_t AON_PAD_MIC4_P_O: 1;
        uint16_t PAD_MIC4_P_PDPUC: 1;
        uint16_t AON_PAD_MIC4_P_PU: 1;
        uint16_t AON_PAD_MIC4_P_PU_EN: 1;
    };
} AON_FAST_0x24C_TYPE;

/* 0x24E
    0       R/W PAD_MIC3_N_SHDN                                 1'b1
    1       R/W AON_PAD_MIC3_N_E                                1'b0
    2       R/W PAD_MIC3_N_WKPOL                                1'b0
    3       R/W PAD_MIC3_N_WKEN                                 1'b0
    4       R/W AON_PAD_MIC3_N_O                                1'b0
    5       R/W PAD_MIC3_N_PDPUC                                1'b0
    6       R/W AON_PAD_MIC3_N_PU                               1'b0
    7       R/W AON_PAD_MIC3_N_PU_EN                            1'b1
    8       R/W PAD_MIC3_P_SHDN                                 1'b1
    9       R/W AON_PAD_MIC3_P_E                                1'b0
    10      R/W PAD_MIC3_P_WKPOL                                1'b0
    11      R/W PAD_MIC3_P_WKEN                                 1'b0
    12      R/W AON_PAD_MIC3_P_O                                1'b0
    13      R/W PAD_MIC3_P_PDPUC                                1'b0
    14      R/W AON_PAD_MIC3_P_PU                               1'b0
    15      R/W AON_PAD_MIC3_P_PU_EN                            1'b1
 */
typedef union _AON_FAST_0x24E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_MIC3_N_SHDN: 1;
        uint16_t AON_PAD_MIC3_N_E: 1;
        uint16_t PAD_MIC3_N_WKPOL: 1;
        uint16_t PAD_MIC3_N_WKEN: 1;
        uint16_t AON_PAD_MIC3_N_O: 1;
        uint16_t PAD_MIC3_N_PDPUC: 1;
        uint16_t AON_PAD_MIC3_N_PU: 1;
        uint16_t AON_PAD_MIC3_N_PU_EN: 1;
        uint16_t PAD_MIC3_P_SHDN: 1;
        uint16_t AON_PAD_MIC3_P_E: 1;
        uint16_t PAD_MIC3_P_WKPOL: 1;
        uint16_t PAD_MIC3_P_WKEN: 1;
        uint16_t AON_PAD_MIC3_P_O: 1;
        uint16_t PAD_MIC3_P_PDPUC: 1;
        uint16_t AON_PAD_MIC3_P_PU: 1;
        uint16_t AON_PAD_MIC3_P_PU_EN: 1;
    };
} AON_FAST_0x24E_TYPE;

/* 0x250
    0       R/W PAD_MIC2_N_SHDN                                 1'b1
    1       R/W AON_PAD_MIC2_N_E                                1'b0
    2       R/W PAD_MIC2_N_WKPOL                                1'b0
    3       R/W PAD_MIC2_N_WKEN                                 1'b0
    4       R/W AON_PAD_MIC2_N_O                                1'b0
    5       R/W PAD_MIC2_N_PDPUC                                1'b0
    6       R/W AON_PAD_MIC2_N_PU                               1'b0
    7       R/W AON_PAD_MIC2_N_PU_EN                            1'b1
    8       R/W PAD_MIC2_P_SHDN                                 1'b1
    9       R/W AON_PAD_MIC2_P_E                                1'b0
    10      R/W PAD_MIC2_P_WKPOL                                1'b0
    11      R/W PAD_MIC2_P_WKEN                                 1'b0
    12      R/W AON_PAD_MIC2_P_O                                1'b0
    13      R/W PAD_MIC2_P_PDPUC                                1'b0
    14      R/W AON_PAD_MIC2_P_PU                               1'b0
    15      R/W AON_PAD_MIC2_P_PU_EN                            1'b1
 */
typedef union _AON_FAST_0x250_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_MIC2_N_SHDN: 1;
        uint16_t AON_PAD_MIC2_N_E: 1;
        uint16_t PAD_MIC2_N_WKPOL: 1;
        uint16_t PAD_MIC2_N_WKEN: 1;
        uint16_t AON_PAD_MIC2_N_O: 1;
        uint16_t PAD_MIC2_N_PDPUC: 1;
        uint16_t AON_PAD_MIC2_N_PU: 1;
        uint16_t AON_PAD_MIC2_N_PU_EN: 1;
        uint16_t PAD_MIC2_P_SHDN: 1;
        uint16_t AON_PAD_MIC2_P_E: 1;
        uint16_t PAD_MIC2_P_WKPOL: 1;
        uint16_t PAD_MIC2_P_WKEN: 1;
        uint16_t AON_PAD_MIC2_P_O: 1;
        uint16_t PAD_MIC2_P_PDPUC: 1;
        uint16_t AON_PAD_MIC2_P_PU: 1;
        uint16_t AON_PAD_MIC2_P_PU_EN: 1;
    };
} AON_FAST_0x250_TYPE;

/* 0x252
    0       R/W PAD_MIC1_N_SHDN                                 1'b1
    1       R/W AON_PAD_MIC1_N_E                                1'b0
    2       R/W PAD_MIC1_N_WKPOL                                1'b0
    3       R/W PAD_MIC1_N_WKEN                                 1'b0
    4       R/W AON_PAD_MIC1_N_O                                1'b0
    5       R/W PAD_MIC1_N_PDPUC                                1'b0
    6       R/W AON_PAD_MIC1_N_PU                               1'b0
    7       R/W AON_PAD_MIC1_N_PU_EN                            1'b1
    8       R/W PAD_MIC1_P_SHDN                                 1'b1
    9       R/W AON_PAD_MIC1_P_E                                1'b0
    10      R/W PAD_MIC1_P_WKPOL                                1'b0
    11      R/W PAD_MIC1_P_WKEN                                 1'b0
    12      R/W AON_PAD_MIC1_P_O                                1'b0
    13      R/W PAD_MIC1_P_PDPUC                                1'b0
    14      R/W AON_PAD_MIC1_P_PU                               1'b0
    15      R/W AON_PAD_MIC1_P_PU_EN                            1'b1
 */
typedef union _AON_FAST_0x252_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_MIC1_N_SHDN: 1;
        uint16_t AON_PAD_MIC1_N_E: 1;
        uint16_t PAD_MIC1_N_WKPOL: 1;
        uint16_t PAD_MIC1_N_WKEN: 1;
        uint16_t AON_PAD_MIC1_N_O: 1;
        uint16_t PAD_MIC1_N_PDPUC: 1;
        uint16_t AON_PAD_MIC1_N_PU: 1;
        uint16_t AON_PAD_MIC1_N_PU_EN: 1;
        uint16_t PAD_MIC1_P_SHDN: 1;
        uint16_t AON_PAD_MIC1_P_E: 1;
        uint16_t PAD_MIC1_P_WKPOL: 1;
        uint16_t PAD_MIC1_P_WKEN: 1;
        uint16_t AON_PAD_MIC1_P_O: 1;
        uint16_t PAD_MIC1_P_PDPUC: 1;
        uint16_t AON_PAD_MIC1_P_PU: 1;
        uint16_t AON_PAD_MIC1_P_PU_EN: 1;
    };
} AON_FAST_0x252_TYPE;

/* 0x254
    0       R/W PAD_LOUT_P_ANA_MODE                             1'b0
    1       R/W PAD_LOUT_N_ANA_MODE                             1'b0
    2       R/W PAD_ROUT_P_ANA_MODE                             1'b0
    3       R/W PAD_ROUT_N_ANA_MODE                             1'b0
    4       R/W PAD_AUX_L_ANA_MODE                              1'b0
    5       R/W PAD_AUX_R_ANA_MODE                              1'b0
    6       R/W PAD_MICBIAS_ANA_MODE                            1'b0
    7       R/W PAD_MIC1_P_ANA_MODE                             1'b0
    8       R/W PAD_MIC1_N_ANA_MODE                             1'b0
    9       R/W PAD_MIC2_P_ANA_MODE                             1'b0
    10      R/W PAD_MIC2_N_ANA_MODE                             1'b0
    11      R/W PAD_MIC3_P_ANA_MODE                             1'b0
    12      R/W PAD_MIC3_N_ANA_MODE                             1'b0
    13      R/W PAD_MIC4_P_ANA_MODE                             1'b0
    14      R/W PAD_MIC4_N_ANA_MODE                             1'b0
    15      R   RSVD                                            1'b0
 */
typedef union _AON_FAST_0x254_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_LOUT_P_ANA_MODE: 1;
        uint16_t PAD_LOUT_N_ANA_MODE: 1;
        uint16_t PAD_ROUT_P_ANA_MODE: 1;
        uint16_t PAD_ROUT_N_ANA_MODE: 1;
        uint16_t PAD_AUX_L_ANA_MODE: 1;
        uint16_t PAD_AUX_R_ANA_MODE: 1;
        uint16_t PAD_MICBIAS_ANA_MODE: 1;
        uint16_t PAD_MIC1_P_ANA_MODE: 1;
        uint16_t PAD_MIC1_N_ANA_MODE: 1;
        uint16_t PAD_MIC2_P_ANA_MODE: 1;
        uint16_t PAD_MIC2_N_ANA_MODE: 1;
        uint16_t PAD_MIC3_P_ANA_MODE: 1;
        uint16_t PAD_MIC3_N_ANA_MODE: 1;
        uint16_t PAD_MIC4_P_ANA_MODE: 1;
        uint16_t PAD_MIC4_N_ANA_MODE: 1;
        uint16_t RSVD: 1;
    };
} AON_FAST_0x254_TYPE;

/* 0x256
    0       R/W PAD_LOUT_N_SHDN                                 1'b1
    1       R/W AON_PAD_LOUT_N_E                                1'b0
    2       R/W PAD_LOUT_N_WKPOL                                1'b0
    3       R/W PAD_LOUT_N_WKEN                                 1'b0
    4       R/W AON_PAD_LOUT_N_O                                1'b0
    5       R/W PAD_LOUT_N_PDPUC                                1'b0
    6       R/W AON_PAD_LOUT_N_PU                               1'b0
    7       R/W AON_PAD_LOUT_N_PU_EN                            1'b1
    8       R/W PAD_LOUT_P_SHDN                                 1'b1
    9       R/W AON_PAD_LOUT_P_E                                1'b0
    10      R/W PAD_LOUT_P_WKPOL                                1'b0
    11      R/W PAD_LOUT_P_WKEN                                 1'b0
    12      R/W AON_PAD_LOUT_P_O                                1'b0
    13      R/W PAD_LOUT_P_PDPUC                                1'b0
    14      R/W AON_PAD_LOUT_P_PU                               1'b0
    15      R/W AON_PAD_LOUT_P_PU_EN                            1'b1
 */
typedef union _AON_FAST_0x256_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_LOUT_N_SHDN: 1;
        uint16_t AON_PAD_LOUT_N_E: 1;
        uint16_t PAD_LOUT_N_WKPOL: 1;
        uint16_t PAD_LOUT_N_WKEN: 1;
        uint16_t AON_PAD_LOUT_N_O: 1;
        uint16_t PAD_LOUT_N_PDPUC: 1;
        uint16_t AON_PAD_LOUT_N_PU: 1;
        uint16_t AON_PAD_LOUT_N_PU_EN: 1;
        uint16_t PAD_LOUT_P_SHDN: 1;
        uint16_t AON_PAD_LOUT_P_E: 1;
        uint16_t PAD_LOUT_P_WKPOL: 1;
        uint16_t PAD_LOUT_P_WKEN: 1;
        uint16_t AON_PAD_LOUT_P_O: 1;
        uint16_t PAD_LOUT_P_PDPUC: 1;
        uint16_t AON_PAD_LOUT_P_PU: 1;
        uint16_t AON_PAD_LOUT_P_PU_EN: 1;
    };
} AON_FAST_0x256_TYPE;

/* 0x258
    0       R/W PAD_ROUT_N_SHDN                                 1'b1
    1       R/W AON_PAD_ROUT_N_E                                1'b0
    2       R/W PAD_ROUT_N_WKPOL                                1'b0
    3       R/W PAD_ROUT_N_WKEN                                 1'b0
    4       R/W AON_PAD_ROUT_N_O                                1'b0
    5       R/W PAD_ROUT_N_PDPUC                                1'b0
    6       R/W AON_PAD_ROUT_N_PU                               1'b0
    7       R/W AON_PAD_ROUT_N_PU_EN                            1'b1
    8       R/W PAD_ROUT_P_SHDN                                 1'b1
    9       R/W AON_PAD_ROUT_P_E                                1'b0
    10      R/W PAD_ROUT_P_WKPOL                                1'b0
    11      R/W PAD_ROUT_P_WKEN                                 1'b0
    12      R/W AON_PAD_ROUT_P_O                                1'b0
    13      R/W PAD_ROUT_P_PDPUC                                1'b0
    14      R/W AON_PAD_ROUT_P_PU                               1'b0
    15      R/W AON_PAD_ROUT_P_PU_EN                            1'b1
 */
typedef union _AON_FAST_0x258_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_ROUT_N_SHDN: 1;
        uint16_t AON_PAD_ROUT_N_E: 1;
        uint16_t PAD_ROUT_N_WKPOL: 1;
        uint16_t PAD_ROUT_N_WKEN: 1;
        uint16_t AON_PAD_ROUT_N_O: 1;
        uint16_t PAD_ROUT_N_PDPUC: 1;
        uint16_t AON_PAD_ROUT_N_PU: 1;
        uint16_t AON_PAD_ROUT_N_PU_EN: 1;
        uint16_t PAD_ROUT_P_SHDN: 1;
        uint16_t AON_PAD_ROUT_P_E: 1;
        uint16_t PAD_ROUT_P_WKPOL: 1;
        uint16_t PAD_ROUT_P_WKEN: 1;
        uint16_t AON_PAD_ROUT_P_O: 1;
        uint16_t PAD_ROUT_P_PDPUC: 1;
        uint16_t AON_PAD_ROUT_P_PU: 1;
        uint16_t AON_PAD_ROUT_P_PU_EN: 1;
    };
} AON_FAST_0x258_TYPE;

/* 0x25A
    0       R/W PAD_AUX_L_SHDN                                  1'b1
    1       R/W AON_PAD_AUX_L_E                                 1'b0
    2       R/W PAD_AUX_L_WKPOL                                 1'b0
    3       R/W PAD_AUX_L_WKEN                                  1'b0
    4       R/W AON_PAD_AUX_L_O                                 1'b0
    5       R/W PAD_AUX_L_PDPUC                                 1'b0
    6       R/W AON_PAD_AUX_L_PU                                1'b0
    7       R/W AON_PAD_AUX_L_PU_EN                             1'b1
    8       R/W PAD_AUX_R_SHDN                                  1'b1
    9       R/W AON_PAD_AUX_R_E                                 1'b0
    10      R/W PAD_AUX_R_WKPOL                                 1'b0
    11      R/W PAD_AUX_R_WKEN                                  1'b0
    12      R/W AON_PAD_AUX_R_O                                 1'b0
    13      R/W PAD_AUX_R_PDPUC                                 1'b0
    14      R/W AON_PAD_AUX_R_PU                                1'b0
    15      R/W AON_PAD_AUX_R_PU_EN                             1'b1
 */
typedef union _AON_FAST_0x25A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_AUX_L_SHDN: 1;
        uint16_t AON_PAD_AUX_L_E: 1;
        uint16_t PAD_AUX_L_WKPOL: 1;
        uint16_t PAD_AUX_L_WKEN: 1;
        uint16_t AON_PAD_AUX_L_O: 1;
        uint16_t PAD_AUX_L_PDPUC: 1;
        uint16_t AON_PAD_AUX_L_PU: 1;
        uint16_t AON_PAD_AUX_L_PU_EN: 1;
        uint16_t PAD_AUX_R_SHDN: 1;
        uint16_t AON_PAD_AUX_R_E: 1;
        uint16_t PAD_AUX_R_WKPOL: 1;
        uint16_t PAD_AUX_R_WKEN: 1;
        uint16_t AON_PAD_AUX_R_O: 1;
        uint16_t PAD_AUX_R_PDPUC: 1;
        uint16_t AON_PAD_AUX_R_PU: 1;
        uint16_t AON_PAD_AUX_R_PU_EN: 1;
    };
} AON_FAST_0x25A_TYPE;

/* 0x25C
    0       R/W PAD_MICBIAS_SHDN                                1'b1
    1       R/W AON_PAD_MICBIAS_E                               1'b0
    2       R/W PAD_MICBIAS_WKPOL                               1'b0
    3       R/W PAD_MICBIAS_WKEN                                1'b0
    4       R/W AON_PAD_MICBIAS_O                               1'b0
    5       R/W PAD_MICBIAS_PDPUC                               1'b0
    6       R/W AON_PAD_MICBIAS_PU                              1'b0
    7       R/W AON_PAD_MICBIAS_PU_EN                           1'b1
    15:08   R   RSVD                                            8'h0
 */
typedef union _AON_FAST_0x25C_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_MICBIAS_SHDN: 1;
        uint16_t AON_PAD_MICBIAS_E: 1;
        uint16_t PAD_MICBIAS_WKPOL: 1;
        uint16_t PAD_MICBIAS_WKEN: 1;
        uint16_t AON_PAD_MICBIAS_O: 1;
        uint16_t PAD_MICBIAS_PDPUC: 1;
        uint16_t AON_PAD_MICBIAS_PU: 1;
        uint16_t AON_PAD_MICBIAS_PU_EN: 1;
        uint16_t RSVD: 8;
    };
} AON_FAST_0x25C_TYPE;

/* 0x25E
    0       R/W AON_PAD_LOUT_P_S                                1'b0
    1       R/W AON_PAD_LOUT_N_S                                1'b0
    2       R/W AON_PAD_ROUT_P_S                                1'b0
    3       R/W AON_PAD_ROUT_N_S                                1'b0
    4       R/W AON_PAD_AUX_L_S                                 1'b0
    5       R/W AON_PAD_AUX_R_S                                 1'b0
    6       R/W AON_PAD_MICBIAS_S                               1'b0
    7       R/W AON_PAD_MIC1_P_S                                1'b0
    8       R/W AON_PAD_MIC1_N_S                                1'b0
    9       R/W AON_PAD_MIC2_P_S                                1'b0
    10      R/W AON_PAD_MIC2_N_S                                1'b0
    11      R/W AON_PAD_MIC3_P_S                                1'b0
    12      R/W AON_PAD_MIC3_N_S                                1'b0
    13      R/W AON_PAD_MIC4_P_S                                1'b0
    14      R/W AON_PAD_MIC4_N_S                                1'b0
    15      R   RSVD                                            1'b0
 */
typedef union _AON_FAST_0x25E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_PAD_LOUT_P_S: 1;
        uint16_t AON_PAD_LOUT_N_S: 1;
        uint16_t AON_PAD_ROUT_P_S: 1;
        uint16_t AON_PAD_ROUT_N_S: 1;
        uint16_t AON_PAD_AUX_L_S: 1;
        uint16_t AON_PAD_AUX_R_S: 1;
        uint16_t AON_PAD_MICBIAS_S: 1;
        uint16_t AON_PAD_MIC1_P_S: 1;
        uint16_t AON_PAD_MIC1_N_S: 1;
        uint16_t AON_PAD_MIC2_P_S: 1;
        uint16_t AON_PAD_MIC2_N_S: 1;
        uint16_t AON_PAD_MIC3_P_S: 1;
        uint16_t AON_PAD_MIC3_N_S: 1;
        uint16_t AON_PAD_MIC4_P_S: 1;
        uint16_t AON_PAD_MIC4_N_S: 1;
        uint16_t RSVD: 1;
    };
} AON_FAST_0x25E_TYPE;

/* 0x260
    0       R/W PAD_G1_SMT                                      1'b1
    1       R/W PAD_G1_H3L1                                     1'b0
    2       R/W PAD_G1_E3                                       1'b0
    3       R/W PAD_G1_E2                                       1'b0
    4       R/W PAD_G2_SMT                                      1'b1
    5       R/W PAD_G2_H3L1                                     1'b1
    6       R/W PAD_G2_E3                                       1'b0
    7       R/W PAD_G2_E2                                       1'b0
    8       R/W PAD_G3_SMT                                      1'b1
    9       R/W PAD_G3_H3L1                                     1'b0
    10      R/W PAD_G3_E3                                       1'b0
    11      R/W PAD_G3_E2                                       1'b0
    12      R/W PAD_G4_SMT                                      1'b1
    13      R/W PAD_G4_H3L1                                     1'b0
    14      R/W PAD_G4_E3                                       1'b0
    15      R/W PAD_G4_E2                                       1'b0
 */
typedef union _AON_FAST_0x260_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_G1_SMT: 1;
        uint16_t PAD_G1_H3L1: 1;
        uint16_t PAD_G1_E3: 1;
        uint16_t PAD_G1_E2: 1;
        uint16_t PAD_G2_SMT: 1;
        uint16_t PAD_G2_H3L1: 1;
        uint16_t PAD_G2_E3: 1;
        uint16_t PAD_G2_E2: 1;
        uint16_t PAD_G3_SMT: 1;
        uint16_t PAD_G3_H3L1: 1;
        uint16_t PAD_G3_E3: 1;
        uint16_t PAD_G3_E2: 1;
        uint16_t PAD_G4_SMT: 1;
        uint16_t PAD_G4_H3L1: 1;
        uint16_t PAD_G4_E3: 1;
        uint16_t PAD_G4_E2: 1;
    };
} AON_FAST_0x260_TYPE;

/* 0x262
    0       R/W PAD_ADC_SMT                                     1'b1
    1       R/W PAD_ADC_H3L1                                    1'b0
    2       R/W PAD_ADC_E3                                      1'b0
    3       R/W PAD_ADC_E2                                      1'b0
    4       R/W PAD_HYBRID_SMT                                  1'b1
    5       R/W PAD_HYBRID_H3L1                                 1'b0
    6       R/W PAD_HYBRID_E3                                   1'b0
    7       R/W PAD_HYBRID_E2                                   1'b0
    15:08   R   RSVD                                            8'h0
 */
typedef union _AON_FAST_0x262_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_ADC_SMT: 1;
        uint16_t PAD_ADC_H3L1: 1;
        uint16_t PAD_ADC_E3: 1;
        uint16_t PAD_ADC_E2: 1;
        uint16_t PAD_HYBRID_SMT: 1;
        uint16_t PAD_HYBRID_H3L1: 1;
        uint16_t PAD_HYBRID_E3: 1;
        uint16_t PAD_HYBRID_E2: 1;
        uint16_t RSVD: 8;
    };
} AON_FAST_0x262_TYPE;

/* 0x264
    0       R/W PAD_32KXO_SHDN                                  1'b1
    1       R/W AON_PAD_32KXO                                   1'b0
    2       R/W PAD_32KXO_WKPOL                                 1'b0
    3       R/W PAD_32KXO_WKEN                                  1'b0
    4       R/W AON_PAD_32KXO_O                                 1'b0
    5       R/W PAD_32KXO_PDPUC                                 1'b0
    6       R/W AON_PAD_32KXO_PU                                1'b0
    7       R/W AON_PAD_32KXO_PU_EN                             1'b1
    8       R/W PAD_32KXI_SHDN                                  1'b1
    9       R/W AON_PAD_32KXI_E                                 1'b0
    10      R/W PAD_32KXI_WKPOL                                 1'b0
    11      R/W PAD_32KXI_WKEN                                  1'b0
    12      R/W AON_PAD_32KXI_O                                 1'b0
    13      R/W PAD_32KXI_PDPUC                                 1'b0
    14      R/W AON_PAD_32KXI_PU                                1'b0
    15      R/W AON_PAD_32KXI_PU_EN                             1'b1
 */
typedef union _AON_FAST_0x264_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_32KXO_SHDN: 1;
        uint16_t AON_PAD_32KXO: 1;
        uint16_t PAD_32KXO_WKPOL: 1;
        uint16_t PAD_32KXO_WKEN: 1;
        uint16_t AON_PAD_32KXO_O: 1;
        uint16_t PAD_32KXO_PDPUC: 1;
        uint16_t AON_PAD_32KXO_PU: 1;
        uint16_t AON_PAD_32KXO_PU_EN: 1;
        uint16_t PAD_32KXI_SHDN: 1;
        uint16_t AON_PAD_32KXI_E: 1;
        uint16_t PAD_32KXI_WKPOL: 1;
        uint16_t PAD_32KXI_WKEN: 1;
        uint16_t AON_PAD_32KXI_O: 1;
        uint16_t PAD_32KXI_PDPUC: 1;
        uint16_t AON_PAD_32KXI_PU: 1;
        uint16_t AON_PAD_32KXI_PU_EN: 1;
    };
} AON_FAST_0x264_TYPE;

/* 0x266
    0       R/W PAD_MIC6_N_SHDN                                 1'b1
    1       R/W AON_PAD_MIC6_N_E                                1'b0
    2       R/W PAD_MIC6_N_WKPOL                                1'b0
    3       R/W PAD_MIC6_N_WKEN                                 1'b0
    4       R/W AON_PAD_MIC6_N_O                                1'b0
    5       R/W PAD_MIC6_N_PDPUC                                1'b0
    6       R/W AON_PAD_MIC6_N_PU                               1'b0
    7       R/W AON_PAD_MIC6_N_PU_EN                            1'b1
    8       R/W PAD_MIC6_P_SHDN                                 1'b1
    9       R/W AON_PAD_MIC6_P_E                                1'b0
    10      R/W PAD_MIC6_P_WKPOL                                1'b0
    11      R/W PAD_MIC6_P_WKEN                                 1'b0
    12      R/W AON_PAD_MIC6_P_O                                1'b0
    13      R/W PAD_MIC6_P_PDPUC                                1'b0
    14      R/W AON_PAD_MIC6_P_PU                               1'b0
    15      R/W AON_PAD_MIC6_P_PU_EN                            1'b1
 */
typedef union _AON_FAST_0x266_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_MIC6_N_SHDN: 1;
        uint16_t AON_PAD_MIC6_N_E: 1;
        uint16_t PAD_MIC6_N_WKPOL: 1;
        uint16_t PAD_MIC6_N_WKEN: 1;
        uint16_t AON_PAD_MIC6_N_O: 1;
        uint16_t PAD_MIC6_N_PDPUC: 1;
        uint16_t AON_PAD_MIC6_N_PU: 1;
        uint16_t AON_PAD_MIC6_N_PU_EN: 1;
        uint16_t PAD_MIC6_P_SHDN: 1;
        uint16_t AON_PAD_MIC6_P_E: 1;
        uint16_t PAD_MIC6_P_WKPOL: 1;
        uint16_t PAD_MIC6_P_WKEN: 1;
        uint16_t AON_PAD_MIC6_P_O: 1;
        uint16_t PAD_MIC6_P_PDPUC: 1;
        uint16_t AON_PAD_MIC6_P_PU: 1;
        uint16_t AON_PAD_MIC6_P_PU_EN: 1;
    };
} AON_FAST_0x266_TYPE;

/* 0x268
    0       R/W PAD_MIC5_N_SHDN                                 1'b1
    1       R/W AON_PAD_MIC5_N_E                                1'b0
    2       R/W PAD_MIC5_N_WKPOL                                1'b0
    3       R/W PAD_MIC5_N_WKEN                                 1'b0
    4       R/W AON_PAD_MIC5_N_O                                1'b0
    5       R/W PAD_MIC5_N_PDPUC                                1'b0
    6       R/W AON_PAD_MIC5_N_PU                               1'b0
    7       R/W AON_PAD_MIC5_N_PU_EN                            1'b1
    8       R/W PAD_MIC5_P_SHDN                                 1'b1
    9       R/W AON_PAD_MIC5_P_E                                1'b0
    10      R/W PAD_MIC5_P_WKPOL                                1'b0
    11      R/W PAD_MIC5_P_WKEN                                 1'b0
    12      R/W AON_PAD_MIC5_P_O                                1'b0
    13      R/W PAD_MIC5_P_PDPUC                                1'b0
    14      R/W AON_PAD_MIC5_P_PU                               1'b0
    15      R/W AON_PAD_MIC5_P_PU_EN                            1'b1
 */
typedef union _AON_FAST_0x268_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_MIC5_N_SHDN: 1;
        uint16_t AON_PAD_MIC5_N_E: 1;
        uint16_t PAD_MIC5_N_WKPOL: 1;
        uint16_t PAD_MIC5_N_WKEN: 1;
        uint16_t AON_PAD_MIC5_N_O: 1;
        uint16_t PAD_MIC5_N_PDPUC: 1;
        uint16_t AON_PAD_MIC5_N_PU: 1;
        uint16_t AON_PAD_MIC5_N_PU_EN: 1;
        uint16_t PAD_MIC5_P_SHDN: 1;
        uint16_t AON_PAD_MIC5_P_E: 1;
        uint16_t PAD_MIC5_P_WKPOL: 1;
        uint16_t PAD_MIC5_P_WKEN: 1;
        uint16_t AON_PAD_MIC5_P_O: 1;
        uint16_t PAD_MIC5_P_PDPUC: 1;
        uint16_t AON_PAD_MIC5_P_PU: 1;
        uint16_t AON_PAD_MIC5_P_PU_EN: 1;
    };
} AON_FAST_0x268_TYPE;

/* 0x26A
    0       R/W PAD_MIC5_P_ANA_MODE                             1'b0
    1       R/W PAD_MIC5_N_ANA_MODE                             1'b0
    2       R/W PAD_MIC6_P_ANA_MODE                             1'b0
    3       R/W PAD_MIC6_N_ANA_MODE                             1'b0
    15:04   R   RSVD                                            12'h0
 */
typedef union _AON_FAST_0x26A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_MIC5_P_ANA_MODE: 1;
        uint16_t PAD_MIC5_N_ANA_MODE: 1;
        uint16_t PAD_MIC6_P_ANA_MODE: 1;
        uint16_t PAD_MIC6_N_ANA_MODE: 1;
        uint16_t RSVD: 12;
    };
} AON_FAST_0x26A_TYPE;

/* 0x26C
    0       W1C PAD_MIC5_P_STS                                  1'b1
    1       W1C PAD_MIC5_N_STS                                  1'b1
    2       W1C PAD_MIC6_P_STS                                  1'b1
    3       W1C PAD_MIC6_N_STS                                  1'b1
    15:04   R   RSVD                                            12'h0
 */
typedef union _AON_FAST_0x26C_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_MIC5_P_STS: 1;
        uint16_t PAD_MIC5_N_STS: 1;
        uint16_t PAD_MIC6_P_STS: 1;
        uint16_t PAD_MIC6_N_STS: 1;
        uint16_t RSVD: 12;
    };
} AON_FAST_0x26C_TYPE;

/* 0x26E
    0       R/W AON_PAD_MIC5_P_S                                1'b0
    1       R/W AON_PAD_MIC5_N_S                                1'b0
    2       R/W AON_PAD_MIC6_P_S                                1'b0
    3       R/W AON_PAD_MIC6_N_S                                1'b0
    15:04   R   RSVD                                            12'h0
 */
typedef union _AON_FAST_0x26E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AON_PAD_MIC5_P_S: 1;
        uint16_t AON_PAD_MIC5_N_S: 1;
        uint16_t AON_PAD_MIC6_P_S: 1;
        uint16_t AON_PAD_MIC6_N_S: 1;
        uint16_t RSVD: 12;
    };
} AON_FAST_0x26E_TYPE;

/* 0x35C
    15:00   R   aon_st                                          16'h1
 */
typedef union _AON_FAST_0x35C_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t aon_st;
    };
} AON_FAST_0x35C_TYPE;

/* 0x35E
    08:00   R   aon_st[24:16]                                   9'h0
    15:09   R   RSVD                                            7'h0
 */
typedef union _AON_FAST_0x35E_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t aon_st_24_16: 9;
        uint16_t RSVD: 7;
    };
} AON_FAST_0x35E_TYPE;

/* 0x36A
    0       R   ISO_BT_PON                                      1'b1
    1       R   ISO_BT_CORE                                     1'b1
    2       R   ISO_XTAL                                        1'b1
    3       R   BT_RET_RSTB                                     1'b0
    4       R   BT_PON_RSTB                                     1'b0
    5       R   BT_CORE_RSTB                                    1'b0
    6       R   POW_32KXTAL                                     1'b0
    7       R   POW_32KOSC                                      1'b1
    9:8     R   RSVD0                                           2'h0
    10      R   AON_GATED_EN                                    1'b1
    15:11   R   RSVD1                                           5'h0
 */
typedef union _AON_FAST_0x36A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t ISO_BT_PON: 1;
        uint16_t ISO_BT_CORE: 1;
        uint16_t ISO_XTAL: 1;
        uint16_t BT_RET_RSTB: 1;
        uint16_t BT_PON_RSTB: 1;
        uint16_t BT_CORE_RSTB: 1;
        uint16_t POW_32KXTAL: 1;
        uint16_t POW_32KOSC: 1;
        uint16_t RSVD0: 2;
        uint16_t AON_GATED_EN: 1;
        uint16_t RSVD1: 5;
    };
} AON_FAST_0x36A_TYPE;

/* 0x370
    0       R   FW_enter_lps                                    1'b0
    1       R   FW_PON_SEQ_RST_N                                1'b0
    2       R   ture_power_off                                  1'b0
    3       R   DP_MODEN_RESTORE                                1'b1
    4       R   DP_MODEN_STORE                                  1'b0
    5       R   PF_RESTORE                                      1'b1
    6       R   PF_STORE                                        1'b0
    7       R   BLE_RESTORE                                     1'b1
    8       R   BLE_STORE                                       1'b0
    9       R   BZ_RESTORE                                      1'b1
    10      R   BZ_STORE                                        1'b0
    11      R   MODEM_RESTORE                                   1'b1
    12      R   MODEM_STORE                                     1'b0
    13      R   RFC_RESTORE                                     1'b1
    14      R   RFC_STORE                                       1'b0
    15      R   RSVD                                            1'b0
 */
typedef union _AON_FAST_0x370_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_enter_lps: 1;
        uint16_t FW_PON_SEQ_RST_N: 1;
        uint16_t ture_power_off: 1;
        uint16_t DP_MODEN_RESTORE: 1;
        uint16_t DP_MODEN_STORE: 1;
        uint16_t PF_RESTORE: 1;
        uint16_t PF_STORE: 1;
        uint16_t BLE_RESTORE: 1;
        uint16_t BLE_STORE: 1;
        uint16_t BZ_RESTORE: 1;
        uint16_t BZ_STORE: 1;
        uint16_t MODEM_RESTORE: 1;
        uint16_t MODEM_STORE: 1;
        uint16_t RFC_RESTORE: 1;
        uint16_t RFC_STORE: 1;
        uint16_t RSVD: 1;
    };
} AON_FAST_0x370_TYPE;

/* 0x376
    08:00   R   DPD_R                                           9'h1FE
    9       R   DPD_RCK                                         1'b0
    15:10   R   RSVD                                            6'h0
 */
typedef union _AON_FAST_0x376_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t DPD_R: 9;
        uint16_t DPD_RCK: 1;
        uint16_t RSVD: 6;
    };
} AON_FAST_0x376_TYPE;

/* 0x378
    15:0    R   RSVD                                            16'hffff
 */
typedef union _AON_FAST_0x378_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD;
    };
} AON_FAST_0x378_TYPE;

/* 0x37A
    0       R   loader_over_512byte                             1'b0
    1       R   forbidden_addr                                  1'b0
    2       R   incorrext_key                                   1'b0
    3       R   aon_loader_done                                 1'b0
    4       R   aon_loader_bypass                               1'b0
    15:05   R   RSVD                                            11'h0
 */
typedef union _AON_FAST_0x37A_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t loader_over_512byte: 1;
        uint16_t forbidden_addr: 1;
        uint16_t incorrext_key: 1;
        uint16_t aon_loader_done: 1;
        uint16_t aon_loader_bypass: 1;
        uint16_t RSVD: 11;
    };
} AON_FAST_0x37A_TYPE;

/* 0x3A8
    0:0     R/W POW_OSC40_fast_reg                              1'b0
    1:1     R/W r_aon_cko2_mux_force_hp                         1'b0
    2:2     R/W CCOT_ECO_EN_32K                                 1'b0
    3:3     R/W SWRAUD_ENLDO_SEL                                1'b0
    7:4     R/W FAST_REG_468_dummy_B7toB0                       4'h0
    15:08   R/W FAST_REG_468_dummy_B15toB8                      8'hff
 */
typedef union _AON_FAST_0x3A8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t POW_OSC40_fast_reg: 1;
        uint16_t r_aon_cko2_mux_force_hp: 1;
        uint16_t CCOT_ECO_EN_32K: 1;
        uint16_t SWRAUD_ENLDO_SEL: 1;
        uint16_t FAST_REG_468_dummy_B7toB0: 4;
        uint16_t FAST_REG_468_dummy_B15toB8: 8;
    };
} AON_FAST_0x3A8_TYPE;

/* 0x3AA
    0       R/W aon_loader_efuse_ctrl_sel                       1'b1
    1       R/W aon_loader_clk_en                               1'b1
    2       R/W PENVDD2_VDD2_ctrl_sel                           1'b0
    3       R/W EN_LDOAUX2_TI                                   1'b0
    4       R/W EN_LDOPA_TI                                     1'b0
    5       R/W IQM_ON2OFF_EN                                   1'b0
    7:6     R/W IQM_TX_PA_EN_sel                                2'b00
    15:08   R/W FAST_REG_469_dummy_B15toB8                      8'b11111111
 */
typedef union _AON_FAST_0x3AA_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t aon_loader_efuse_ctrl_sel: 1;
        uint16_t aon_loader_clk_en: 1;
        uint16_t PENVDD2_VDD2_ctrl_sel: 1;
        uint16_t EN_LDOAUX2_TI: 1;
        uint16_t EN_LDOPA_TI: 1;
        uint16_t IQM_ON2OFF_EN: 1;
        uint16_t IQM_TX_PA_EN_sel: 2;
        uint16_t FAST_REG_469_dummy_B15toB8: 8;
    };
} AON_FAST_0x3AA_TYPE;

/* 0x3AC
    0       R/W EN_OFF_VR_LDO_USB                               1'b0
    1       R/W EN_OFF_VR_LDO_PA                                1'b0
    2       R/W EN_BOTH_OSC_XTAL                                1'b0
    06:03   R/W EN_SWRAUD_PFM20K_PRBS_ECO                       4'b0000
    7       R/W EN_SWRAUD_PFM20K_LPFO_ECO                       1'b0
    8       R/W CKO2_LP_enable                                  1'b1
    9       R/W CKO2_HP_enable                                  1'b1
    10      R/W disable_xtal_clk                                1'b1
    15:11   R/W FAST_REG_470_dummy_B15toB8                      5'b1111111
 */
typedef union _AON_FAST_0x3AC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t EN_OFF_VR_LDO_USB: 1;
        uint16_t EN_OFF_VR_LDO_PA: 1;
        uint16_t EN_BOTH_OSC_XTAL: 1;
        uint16_t EN_SWRAUD_PFM20K_PRBS_ECO: 4;
        uint16_t EN_SWRAUD_PFM20K_LPFO_ECO: 1;
        uint16_t CKO2_LP_enable: 1;
        uint16_t CKO2_HP_enable: 1;
        uint16_t disable_xtal_clk: 1;
        uint16_t FAST_REG_470_dummy_B15toB8: 5;
    };
} AON_FAST_0x3AC_TYPE;

/* 0x3B0
    0       R/W PAD_P6_SHDN[2]                                  1'b1
    1       R/W AON_PAD_P6_E[2]                                 1'b0
    2       R/W PAD_P6_WKPOL[2]                                 1'b0
    3       R/W PAD_P6_WKEN[2]                                  1'b0
    4       R/W AON_PAD_P6_O[2]                                 1'b0
    5       R/W PAD_P6_PDPUC[2]                                 1'b0
    6       R/W AON_PAD_P6_PU[2]                                1'b0
    7       R/W AON_PAD_P6_PU_EN[2]                             1'b1
    8       R/W PAD_P6_SHDN[3]                                  1'b1
    9       R/W AON_PAD_P6_E[3]                                 1'b0
    10      R/W PAD_P6_WKPOL[3]                                 1'b0
    11      R/W PAD_P6_WKEN[3]                                  1'b0
    12      R/W AON_PAD_P6_O[3]                                 1'b0
    13      R/W PAD_P6_PDPUC[3]                                 1'b0
    14      R/W AON_PAD_P6_PU[3]                                1'b0
    15      R/W AON_PAD_P6_PU_EN[3]                             1'b1
 */
typedef union _AON_FAST_0x3B0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P6_SHDN_2: 1;
        uint16_t AON_PAD_P6_E_2: 1;
        uint16_t PAD_P6_WKPOL_2: 1;
        uint16_t PAD_P6_WKEN_2: 1;
        uint16_t AON_PAD_P6_O_2: 1;
        uint16_t PAD_P6_PDPUC_2: 1;
        uint16_t AON_PAD_P6_PU_2: 1;
        uint16_t AON_PAD_P6_PU_EN_2: 1;
        uint16_t PAD_P6_SHDN_3: 1;
        uint16_t AON_PAD_P6_E_3: 1;
        uint16_t PAD_P6_WKPOL_3: 1;
        uint16_t PAD_P6_WKEN_3: 1;
        uint16_t AON_PAD_P6_O_3: 1;
        uint16_t PAD_P6_PDPUC_3: 1;
        uint16_t AON_PAD_P6_PU_3: 1;
        uint16_t AON_PAD_P6_PU_EN_3: 1;
    };
} AON_FAST_0x3B0_TYPE;

/* 0x3B2
    0       R/W PAD_P6_SHDN[4]                                  1'b1
    1       R/W AON_PAD_P6_E[4]                                 1'b0
    2       R/W PAD_P6_WKPOL[4]                                 1'b0
    3       R/W PAD_P6_WKEN[4]                                  1'b0
    4       R/W AON_PAD_P6_O[4]                                 1'b0
    5       R/W PAD_P6_PDPUC[4]                                 1'b0
    6       R/W AON_PAD_P6_PU[4]                                1'b0
    7       R/W AON_PAD_P6_PU_EN[4]                             1'b1
    8       R/W PAD_P6_SHDN[5]                                  1'b1
    9       R/W AON_PAD_P6_E[5]                                 1'b0
    10      R/W PAD_P6_WKPOL[5]                                 1'b0
    11      R/W PAD_P6_WKEN[5]                                  1'b0
    12      R/W AON_PAD_P6_O[5]                                 1'b0
    13      R/W PAD_P6_PDPUC[5]                                 1'b0
    14      R/W AON_PAD_P6_PU[5]                                1'b0
    15      R/W AON_PAD_P6_PU_EN[5]                             1'b1
 */
typedef union _AON_FAST_0x3B2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P6_SHDN_4: 1;
        uint16_t AON_PAD_P6_E_4: 1;
        uint16_t PAD_P6_WKPOL_4: 1;
        uint16_t PAD_P6_WKEN_4: 1;
        uint16_t AON_PAD_P6_O_4: 1;
        uint16_t PAD_P6_PDPUC_4: 1;
        uint16_t AON_PAD_P6_PU_4: 1;
        uint16_t AON_PAD_P6_PU_EN_4: 1;
        uint16_t PAD_P6_SHDN_5: 1;
        uint16_t AON_PAD_P6_E_5: 1;
        uint16_t PAD_P6_WKPOL_5: 1;
        uint16_t PAD_P6_WKEN_5: 1;
        uint16_t AON_PAD_P6_O_5: 1;
        uint16_t PAD_P6_PDPUC_5: 1;
        uint16_t AON_PAD_P6_PU_5: 1;
        uint16_t AON_PAD_P6_PU_EN_5: 1;
    };
} AON_FAST_0x3B2_TYPE;

/* 0x3B4
    0       R/W PAD_P6_SHDN[6]                                  1'b1
    1       R/W AON_PAD_P6_E[6]                                 1'b0
    2       R/W PAD_P6_WKPOL[6]                                 1'b0
    3       R/W PAD_P6_WKEN[6]                                  1'b0
    4       R/W AON_PAD_P6_O[6]                                 1'b0
    5       R/W PAD_P6_PDPUC[6]                                 1'b0
    6       R/W AON_PAD_P6_PU[6]                                1'b0
    7       R/W AON_PAD_P6_PU_EN[6]                             1'b1
    8       R/W PAD_P6_SHDN[7]                                  1'b1
    9       R/W AON_PAD_P6_E[7]                                 1'b0
    10      R/W PAD_P6_WKPOL[7]                                 1'b0
    11      R/W PAD_P6_WKEN[7]                                  1'b0
    12      R/W AON_PAD_P6_O[7]                                 1'b0
    13      R/W PAD_P6_PDPUC[7]                                 1'b0
    14      R/W AON_PAD_P6_PU[7]                                1'b0
    15      R/W AON_PAD_P6_PU_EN[7]                             1'b1
 */
typedef union _AON_FAST_0x3B4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P6_SHDN_6: 1;
        uint16_t AON_PAD_P6_E_6: 1;
        uint16_t PAD_P6_WKPOL_6: 1;
        uint16_t PAD_P6_WKEN_6: 1;
        uint16_t AON_PAD_P6_O_6: 1;
        uint16_t PAD_P6_PDPUC_6: 1;
        uint16_t AON_PAD_P6_PU_6: 1;
        uint16_t AON_PAD_P6_PU_EN_6: 1;
        uint16_t PAD_P6_SHDN_7: 1;
        uint16_t AON_PAD_P6_E_7: 1;
        uint16_t PAD_P6_WKPOL_7: 1;
        uint16_t PAD_P6_WKEN_7: 1;
        uint16_t AON_PAD_P6_O_7: 1;
        uint16_t PAD_P6_PDPUC_7: 1;
        uint16_t AON_PAD_P6_PU_7: 1;
        uint16_t AON_PAD_P6_PU_EN_7: 1;
    };
} AON_FAST_0x3B4_TYPE;

/* 0x3B6
    0       R/W PAD_P7_SHDN[2]                                  1'b1
    1       R/W AON_PAD_P7_E[2]                                 1'b0
    2       R/W PAD_P7_WKPOL[2]                                 1'b0
    3       R/W PAD_P7_WKEN[2]                                  1'b0
    4       R/W AON_PAD_P7_O[2]                                 1'b0
    5       R/W PAD_P7_PDPUC[2]                                 1'b0
    6       R/W AON_PAD_P7_PU[2]                                1'b0
    7       R/W AON_PAD_P7_PU_EN[2]                             1'b1
    8       R/W PAD_P7_SHDN[3]                                  1'b1
    9       R/W AON_PAD_P7_E[3]                                 1'b0
    10      R/W PAD_P7_WKPOL[3]                                 1'b0
    11      R/W PAD_P7_WKEN[3]                                  1'b0
    12      R/W AON_PAD_P7_O[3]                                 1'b0
    13      R/W PAD_P7_PDPUC[3]                                 1'b0
    14      R/W AON_PAD_P7_PU[3]                                1'b0
    15      R/W AON_PAD_P7_PU_EN[3]                             1'b1
 */
typedef union _AON_FAST_0x3B6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P7_SHDN_2: 1;
        uint16_t AON_PAD_P7_E_2: 1;
        uint16_t PAD_P7_WKPOL_2: 1;
        uint16_t PAD_P7_WKEN_2: 1;
        uint16_t AON_PAD_P7_O_2: 1;
        uint16_t PAD_P7_PDPUC_2: 1;
        uint16_t AON_PAD_P7_PU_2: 1;
        uint16_t AON_PAD_P7_PU_EN_2: 1;
        uint16_t PAD_P7_SHDN_3: 1;
        uint16_t AON_PAD_P7_E_3: 1;
        uint16_t PAD_P7_WKPOL_3: 1;
        uint16_t PAD_P7_WKEN_3: 1;
        uint16_t AON_PAD_P7_O_3: 1;
        uint16_t PAD_P7_PDPUC_3: 1;
        uint16_t AON_PAD_P7_PU_3: 1;
        uint16_t AON_PAD_P7_PU_EN_3: 1;
    };
} AON_FAST_0x3B6_TYPE;

/* 0x3B8
    0       R/W PAD_P7_SHDN[4]                                  1'b1
    1       R/W AON_PAD_P7_E[4]                                 1'b0
    2       R/W PAD_P7_WKPOL[4]                                 1'b0
    3       R/W PAD_P7_WKEN[4]                                  1'b0
    4       R/W AON_PAD_P7_O[4]                                 1'b0
    5       R/W PAD_P7_PDPUC[4]                                 1'b0
    6       R/W AON_PAD_P7_PU[4]                                1'b0
    7       R/W AON_PAD_P7_PU_EN[4]                             1'b1
    8       R/W PAD_P7_SHDN[5]                                  1'b1
    9       R/W AON_PAD_P7_E[5]                                 1'b0
    10      R/W PAD_P7_WKPOL[5]                                 1'b0
    11      R/W PAD_P7_WKEN[5]                                  1'b0
    12      R/W AON_PAD_P7_O[5]                                 1'b0
    13      R/W PAD_P7_PDPUC[5]                                 1'b0
    14      R/W AON_PAD_P7_PU[5]                                1'b0
    15      R/W AON_PAD_P7_PU_EN[5]                             1'b1
 */
typedef union _AON_FAST_0x3B8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P7_SHDN_4: 1;
        uint16_t AON_PAD_P7_E_4: 1;
        uint16_t PAD_P7_WKPOL_4: 1;
        uint16_t PAD_P7_WKEN_4: 1;
        uint16_t AON_PAD_P7_O_4: 1;
        uint16_t PAD_P7_PDPUC_4: 1;
        uint16_t AON_PAD_P7_PU_4: 1;
        uint16_t AON_PAD_P7_PU_EN_4: 1;
        uint16_t PAD_P7_SHDN_5: 1;
        uint16_t AON_PAD_P7_E_5: 1;
        uint16_t PAD_P7_WKPOL_5: 1;
        uint16_t PAD_P7_WKEN_5: 1;
        uint16_t AON_PAD_P7_O_5: 1;
        uint16_t PAD_P7_PDPUC_5: 1;
        uint16_t AON_PAD_P7_PU_5: 1;
        uint16_t AON_PAD_P7_PU_EN_5: 1;
    };
} AON_FAST_0x3B8_TYPE;

/* 0x3BA
    0       R/W PAD_P7_SHDN[6]                                  1'b1
    1       R/W AON_PAD_P7_E[6]                                 1'b0
    2       R/W PAD_P7_WKPOL[6]                                 1'b0
    3       R/W PAD_P7_WKEN[6]                                  1'b0
    4       R/W AON_PAD_P7_O[6]                                 1'b0
    5       R/W PAD_P7_PDPUC[6]                                 1'b0
    6       R/W AON_PAD_P7_PU[6]                                1'b0
    7       R/W AON_PAD_P7_PU_EN[6]                             1'b1
    8       R/W PAD_P7_SHDN[7]                                  1'b1
    9       R/W AON_PAD_P7_E[7]                                 1'b0
    10      R/W PAD_P7_WKPOL[7]                                 1'b0
    11      R/W PAD_P7_WKEN[7]                                  1'b0
    12      R/W AON_PAD_P7_O[7]                                 1'b0
    13      R/W PAD_P7_PDPUC[7]                                 1'b0
    14      R/W AON_PAD_P7_PU[7]                                1'b0
    15      R/W AON_PAD_P7_PU_EN[7]                             1'b1
 */
typedef union _AON_FAST_0x3BA_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P7_SHDN_6: 1;
        uint16_t AON_PAD_P7_E_6: 1;
        uint16_t PAD_P7_WKPOL_6: 1;
        uint16_t PAD_P7_WKEN_6: 1;
        uint16_t AON_PAD_P7_O_6: 1;
        uint16_t PAD_P7_PDPUC_6: 1;
        uint16_t AON_PAD_P7_PU_6: 1;
        uint16_t AON_PAD_P7_PU_EN_6: 1;
        uint16_t PAD_P7_SHDN_7: 1;
        uint16_t AON_PAD_P7_E_7: 1;
        uint16_t PAD_P7_WKPOL_7: 1;
        uint16_t PAD_P7_WKEN_7: 1;
        uint16_t AON_PAD_P7_O_7: 1;
        uint16_t PAD_P7_PDPUC_7: 1;
        uint16_t AON_PAD_P7_PU_7: 1;
        uint16_t AON_PAD_P7_PU_EN_7: 1;
    };
} AON_FAST_0x3BA_TYPE;


/* 0x3F4
    1:0     R/W BT_CKI_AD_DIG_ctl                               2'b00
    3:2     R/W BT_CKI_DA_DIG_ctl                               2'b00
    4       R/W BT_CKI_AD_DIG_sel                               1'b0
    5       R/W DUMMY                                           1'b0
    6       R/W BT_CKI_DA_DIG_sel                               1'b0
    7       R/W NEW_SHDN_CTL                                    1'b0
    15:08   R/W IO_MUX_SW[7:0]                                  8'h0
 */
typedef union _AON_FAST_0x3F4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BT_CKI_AD_DIG_ctl: 2;
        uint16_t BT_CKI_DA_DIG_ctl: 2;
        uint16_t BT_CKI_AD_DIG_sel: 1;
        uint16_t DUMMY: 1;
        uint16_t BT_CKI_DA_DIG_sel: 1;
        uint16_t NEW_SHDN_CTL: 1;
        uint16_t IO_MUX_SW_7_0: 8;
    };
} AON_FAST_0x3F4_TYPE;

/* 0x3F6
    07:00   R/W IO_MUX_P6_7_0                                   8'h0
    15:08   R/W IO_MUX_P7_7_0                                   8'h0
 */
typedef union _AON_FAST_0x3F6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t IO_MUX_P6_7_0: 8;
        uint16_t IO_MUX_P7_7_0: 8;
    };
} AON_FAST_0x3F6_TYPE;


/* 0x400
    15:0    R/W FW_GENERAL_REG0X                                16'h0
 */
typedef union _AON_FAST_REG0X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG0X;
    };
} AON_FAST_REG0X_FW_GENERAL_TYPE;

/* 0x402
    15:0    R/W FW_GENERAL_REG1X                                16'h0
 */
typedef union _AON_FAST_REG1X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG1X;
    };
} AON_FAST_REG1X_FW_GENERAL_TYPE;

/* 0x404
    15:0    R/W FW_GENERAL_REG2X                                16'h0
 */
typedef union _AON_FAST_REG2X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG2X;
    };
} AON_FAST_REG2X_FW_GENERAL_TYPE;

/* 0x406
    15:0    R/W FW_GENERAL_REG3X                                16'h0
 */
typedef union _AON_FAST_REG3X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG3X;
    };
} AON_FAST_REG3X_FW_GENERAL_TYPE;

/* 0x408
    15:0    R/W FW_GENERAL_REG4X                                16'h0
 */
typedef union _AON_FAST_REG4X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG4X;
    };
} AON_FAST_REG4X_FW_GENERAL_TYPE;

/* 0x40A
    15:0    R/W FW_GENERAL_REG5X                                16'h0
 */
typedef union _AON_FAST_REG5X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG5X;
    };
} AON_FAST_REG5X_FW_GENERAL_TYPE;

/* 0x40C
    15:0    R/W FW_GENERAL_REG6X                                16'h0
 */
typedef union _AON_FAST_REG6X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG6X;
    };
} AON_FAST_REG6X_FW_GENERAL_TYPE;

/* 0x40E
    15:0    R/W FW_GENERAL_REG7X                                16'h0
 */
typedef union _AON_FAST_REG7X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG7X;
    };
} AON_FAST_REG7X_FW_GENERAL_TYPE;

/* 0x410
    15:0    R/W FW_GENERAL_REG8X                                16'h0
 */
typedef union _AON_FAST_REG8X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG8X;
    };
} AON_FAST_REG8X_FW_GENERAL_TYPE;

/* 0x412
    15:0    R/W FW_GENERAL_REG9X                                16'h0
 */
typedef union _AON_FAST_REG9X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG9X;
    };
} AON_FAST_REG9X_FW_GENERAL_TYPE;

/* 0x414
    15:0    R/W FW_GENERAL_REG10X                               16'h0
 */
typedef union _AON_FAST_REG10X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG10X;
    };
} AON_FAST_REG10X_FW_GENERAL_TYPE;

/* 0x416
    15:0    R/W FW_GENERAL_REG11X                               16'h0
 */
typedef union _AON_FAST_REG11X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG11X;
    };
} AON_FAST_REG11X_FW_GENERAL_TYPE;

/* 0x418
    15:0    R/W FW_GENERAL_REG12X                               16'h0
 */
typedef union _AON_FAST_REG12X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG12X;
    };
} AON_FAST_REG12X_FW_GENERAL_TYPE;

/* 0x41A
    15:0    R/W FW_GENERAL_REG13X                               16'h0
 */
typedef union _AON_FAST_REG13X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG13X;
    };
} AON_FAST_REG13X_FW_GENERAL_TYPE;

/* 0x41C
    15:0    R/W FW_GENERAL_REG14X                               16'h0
 */
typedef union _AON_FAST_REG14X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG14X;
    };
} AON_FAST_REG14X_FW_GENERAL_TYPE;

/* 0x41E
    15:0    R/W FW_GENERAL_REG15X                               16'h0
 */
typedef union _AON_FAST_REG15X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG15X;
    };
} AON_FAST_REG15X_FW_GENERAL_TYPE;

/* 0x420
    15:0    R/W FW_GENERAL_REG16X                               16'h0
 */
typedef union _AON_FAST_REG16X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG16X;
    };
} AON_FAST_REG16X_FW_GENERAL_TYPE;

/* 0x422
    15:0    R/W FW_GENERAL_REG17X                               16'h0
 */
typedef union _AON_FAST_REG17X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG17X;
    };
} AON_FAST_REG17X_FW_GENERAL_TYPE;

/* 0x424
    15:0    R/W FW_GENERAL_REG18X                               16'h0
 */
typedef union _AON_FAST_REG18X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG18X;
    };
} AON_FAST_REG18X_FW_GENERAL_TYPE;

/* 0x426
    15:0    R/W FW_GENERAL_REG19X                               16'h0
 */
typedef union _AON_FAST_REG19X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG19X;
    };
} AON_FAST_REG19X_FW_GENERAL_TYPE;

/* 0x428
    15:0    R/W FW_GENERAL_REG20X                               16'h0
 */
typedef union _AON_FAST_REG20X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG20X;
    };
} AON_FAST_REG20X_FW_GENERAL_TYPE;

/* 0x42A
    15:0    R/W FW_GENERAL_REG21X                               16'h0
 */
typedef union _AON_FAST_REG21X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG21X;
    };
} AON_FAST_REG21X_FW_GENERAL_TYPE;

/* 0x42C
    15:0    R/W FW_GENERAL_REG22X                               16'h0
 */
typedef union _AON_FAST_REG22X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG22X;
    };
} AON_FAST_REG22X_FW_GENERAL_TYPE;

/* 0x42E
    15:0    R/W FW_GENERAL_REG23X                               16'h0
 */
typedef union _AON_FAST_REG23X_FW_GENERAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t FW_GENERAL_REG23X;
    };
} AON_FAST_REG23X_FW_GENERAL_TYPE;

/* 0x480
    7:0     R/W PMUX_GPIO_P0_0                                  8'h0
    15:8    R/W PMUX_GPIO_P0_1                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P0_0_P0_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P0_0: 8;
        uint16_t PMUX_GPIO_P0_1: 8;
    };
} AON_FAST_REG_PAD_P0_0_P0_1_TYPE;

/* 0x482
    7:0     R/W PMUX_GPIO_P0_2                                  8'h0
    15:8    R/W PMUX_GPIO_P0_3                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P0_2_P0_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P0_2: 8;
        uint16_t PMUX_GPIO_P0_3: 8;
    };
} AON_FAST_REG_PAD_P0_2_P0_3_TYPE;

/* 0x484
    7:0     R/W PMUX_GPIO_P0_4                                  8'h0
    15:8    R/W PMUX_GPIO_P0_5                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P0_4_P0_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P0_4: 8;
        uint16_t PMUX_GPIO_P0_5: 8;
    };
} AON_FAST_REG_PAD_P0_4_P0_5_TYPE;

/* 0x486
    7:0     R/W PMUX_GPIO_P0_6                                  8'h0
    15:8    R/W PMUX_GPIO_P0_7                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P0_6_P0_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P0_6: 8;
        uint16_t PMUX_GPIO_P0_7: 8;
    };
} AON_FAST_REG_PAD_P0_6_P0_7_TYPE;

/* 0x488
    7:0     R/W PMUX_GPIO_P1_0                                  8'h0
    15:8    R/W PMUX_GPIO_P1_1                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P1_0_P1_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P1_0: 8;
        uint16_t PMUX_GPIO_P1_1: 8;
    };
} AON_FAST_REG_PAD_P1_0_P1_1_TYPE;

/* 0x48A
    7:0     R/W PMUX_GPIO_P1_2                                  8'h0
    15:8    R/W PMUX_GPIO_P1_3                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P1_2_P1_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P1_2: 8;
        uint16_t PMUX_GPIO_P1_3: 8;
    };
} AON_FAST_REG_PAD_P1_2_P1_3_TYPE;

/* 0x48C
    7:0     R/W PMUX_GPIO_P1_4                                  8'h0
    15:8    R/W PMUX_GPIO_P1_5                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P1_4_P1_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P1_4: 8;
        uint16_t PMUX_GPIO_P1_5: 8;
    };
} AON_FAST_REG_PAD_P1_4_P1_5_TYPE;

/* 0x48E
    7:0     R/W PMUX_GPIO_P1_6                                  8'h0
    15:8    R/W PMUX_GPIO_P1_7                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P1_6_P1_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P1_6: 8;
        uint16_t PMUX_GPIO_P1_7: 8;
    };
} AON_FAST_REG_PAD_P1_6_P1_7_TYPE;

/* 0x490
    7:0     R/W PMUX_GPIO_P2_0                                  8'h0
    15:8    R/W PMUX_GPIO_P2_1                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P2_0_P2_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P2_0: 8;
        uint16_t PMUX_GPIO_P2_1: 8;
    };
} AON_FAST_REG_PAD_P2_0_P2_1_TYPE;

/* 0x492
    7:0     R/W PMUX_GPIO_P2_2                                  8'h0
    15:8    R/W PMUX_GPIO_P2_3                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P2_2_P2_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P2_2: 8;
        uint16_t PMUX_GPIO_P2_3: 8;
    };
} AON_FAST_REG_PAD_P2_2_P2_3_TYPE;

/* 0x494
    7:0     R/W PMUX_GPIO_P2_4                                  8'h0
    15:8    R/W PMUX_GPIO_P2_5                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P2_4_P2_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P2_4: 8;
        uint16_t PMUX_GPIO_P2_5: 8;
    };
} AON_FAST_REG_PAD_P2_4_P2_5_TYPE;

/* 0x496
    7:0     R/W PMUX_GPIO_P2_6                                  8'h0
    15:8    R/W PMUX_GPIO_P2_7                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P2_6_P2_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P2_6: 8;
        uint16_t PMUX_GPIO_P2_7: 8;
    };
} AON_FAST_REG_PAD_P2_6_P2_7_TYPE;

/* 0x498
    7:0     R/W PMUX_GPIO_P3_0                                  8'h0
    15:8    R/W PMUX_GPIO_P3_1                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P3_0_P3_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P3_0: 8;
        uint16_t PMUX_GPIO_P3_1: 8;
    };
} AON_FAST_REG_PAD_P3_0_P3_1_TYPE;

/* 0x49A
    7:0     R/W PMUX_GPIO_P3_2                                  8'h0
    15:8    R/W PMUX_GPIO_P3_3                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P3_2_P3_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P3_2: 8;
        uint16_t PMUX_GPIO_P3_3: 8;
    };
} AON_FAST_REG_PAD_P3_2_P3_3_TYPE;

/* 0x49C
    7:0     R/W PMUX_GPIO_P3_4                                  8'h0
    15:8    R/W PMUX_GPIO_P3_5                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P3_4_P3_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P3_4: 8;
        uint16_t PMUX_GPIO_P3_5: 8;
    };
} AON_FAST_REG_PAD_P3_4_P3_5_TYPE;

/* 0x49E
    7:0     R/W PMUX_GPIO_P3_6                                  8'h0
    15:8    R/W PMUX_GPIO_P3_7                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P3_6_P3_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P3_6: 8;
        uint16_t PMUX_GPIO_P3_7: 8;
    };
} AON_FAST_REG_PAD_P3_6_P3_7_TYPE;

/* 0x4A0
    7:0     R/W PMUX_GPIO_P4_0                                  8'h0
    15:8    R/W PMUX_GPIO_P4_1                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P4_0_P4_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P4_0: 8;
        uint16_t PMUX_GPIO_P4_1: 8;
    };
} AON_FAST_REG_PAD_P4_0_P4_1_TYPE;

/* 0x4A2
    7:0     R/W PMUX_GPIO_P4_2                                  8'h0
    15:8    R/W PMUX_GPIO_P4_3                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P4_2_P4_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P4_2: 8;
        uint16_t PMUX_GPIO_P4_3: 8;
    };
} AON_FAST_REG_PAD_P4_2_P4_3_TYPE;

/* 0x4A4
    7:0     R/W PMUX_GPIO_P4_4                                  8'h0
    15:8    R/W PMUX_GPIO_P4_5                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P4_4_P4_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P4_4: 8;
        uint16_t PMUX_GPIO_P4_5: 8;
    };
} AON_FAST_REG_PAD_P4_4_P4_5_TYPE;

/* 0x4A6
    7:0     R/W PMUX_GPIO_P4_6                                  8'h0
    15:8    R/W PMUX_GPIO_P4_7                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P4_6_P4_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P4_6: 8;
        uint16_t PMUX_GPIO_P4_7: 8;
    };
} AON_FAST_REG_PAD_P4_6_P4_7_TYPE;

/* 0x4A8
    7:0     R/W PMUX_GPIO_P5_0                                  8'h0
    15:8    R/W PMUX_GPIO_P5_1                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P5_0_P5_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P5_0: 8;
        uint16_t PMUX_GPIO_P5_1: 8;
    };
} AON_FAST_REG_PAD_P5_0_P5_1_TYPE;

/* 0x4AA
    7:0     R/W PMUX_GPIO_P5_2                                  8'h0
    15:8    R/W PMUX_GPIO_P5_3                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P5_2_P5_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P5_2: 8;
        uint16_t PMUX_GPIO_P5_3: 8;
    };
} AON_FAST_REG_PAD_P5_2_P5_3_TYPE;

/* 0x4AC
    7:0     R/W PMUX_GPIO_P5_4                                  8'h0
    15:8    R/W PMUX_GPIO_P5_5                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P5_4_P5_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P5_4: 8;
        uint16_t PMUX_GPIO_P5_5: 8;
    };
} AON_FAST_REG_PAD_P5_4_P5_5_TYPE;

/* 0x4AE
    7:0     R/W PMUX_GPIO_P5_6                                  8'h0
    15:8    R/W PMUX_GPIO_P5_7                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P5_6_P5_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P5_6: 8;
        uint16_t PMUX_GPIO_P5_7: 8;
    };
} AON_FAST_REG_PAD_P5_6_P5_7_TYPE;

/* 0x4B0
    7:0     R/W PMUX_GPIO_P6_0                                  8'h0
    15:8    R/W PMUX_GPIO_P6_1                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P6_0_P6_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P6_0: 8;
        uint16_t PMUX_GPIO_P6_1: 8;
    };
} AON_FAST_REG_PAD_P6_0_P6_1_TYPE;

/* 0x4B2
    7:0     R/W PMUX_GPIO_LOUT_N                                8'h0
    15:8    R/W PMUX_GPIO_LOUT_P                                8'h0
 */
typedef union _AON_FAST_REG_PAD_LOUT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_LOUT_N: 8;
        uint16_t PMUX_GPIO_LOUT_P: 8;
    };
} AON_FAST_REG_PAD_LOUT_TYPE;

/* 0x4B4
    7:0     R/W PMUX_GPIO_ROUT_N                                8'h0
    15:8    R/W PMUX_GPIO_ROUT_P                                8'h0
 */
typedef union _AON_FAST_REG_PAD_ROUT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_ROUT_N: 8;
        uint16_t PMUX_GPIO_ROUT_P: 8;
    };
} AON_FAST_REG_PAD_ROUT_TYPE;

/* 0x4B6
    7:0     R/W PMUX_GPIO_MIC1_N                                8'h0
    15:8    R/W PMUX_GPIO_MIC1_P                                8'h0
 */
typedef union _AON_FAST_REG_PAD_MIC1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_MIC1_N: 8;
        uint16_t PMUX_GPIO_MIC1_P: 8;
    };
} AON_FAST_REG_PAD_MIC1_TYPE;

/* 0x4B8
    7:0     R/W PMUX_GPIO_MIC2_N                                8'h0
    15:8    R/W PMUX_GPIO_MIC2_P                                8'h0
 */
typedef union _AON_FAST_REG_PAD_MIC2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_MIC2_N: 8;
        uint16_t PMUX_GPIO_MIC2_P: 8;
    };
} AON_FAST_REG_PAD_MIC2_TYPE;

/* 0x4BA
    7:0     R/W PMUX_GPIO_MIC3_N                                8'h0
    15:8    R/W PMUX_GPIO_MIC3_P                                8'h0
 */
typedef union _AON_FAST_REG_PAD_MIC3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_MIC3_N: 8;
        uint16_t PMUX_GPIO_MIC3_P: 8;
    };
} AON_FAST_REG_PAD_MIC3_TYPE;

/* 0x4BC
    7:0     R/W PMUX_GPIO_MIC4_N                                8'h0
    15:8    R/W PMUX_GPIO_MIC4_P                                8'h0
 */
typedef union _AON_FAST_REG_PAD_MIC4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_MIC4_N: 8;
        uint16_t PMUX_GPIO_MIC4_P: 8;
    };
} AON_FAST_REG_PAD_MIC4_TYPE;

/* 0x4BE
    7:0     R/W PMUX_GPIO_MIC5_N                                8'h0
    15:8    R/W PMUX_GPIO_MIC5_P                                8'h0
 */
typedef union _AON_FAST_REG_PAD_MIC5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_MIC5_N: 8;
        uint16_t PMUX_GPIO_MIC5_P: 8;
    };
} AON_FAST_REG_PAD_MIC5_TYPE;

/* 0x4C0
    7:0     R/W PMUX_GPIO_MIC6_N                                8'h0
    15:8    R/W PMUX_GPIO_MIC6_P                                8'h0
 */
typedef union _AON_FAST_REG_PAD_MIC6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_MIC6_N: 8;
        uint16_t PMUX_GPIO_MIC6_P: 8;
    };
} AON_FAST_REG_PAD_MIC6_TYPE;

/* 0x4C2
    7:0     R/W PMUX_GPIO_AUX_R                                 8'h0
    15:8    R/W PMUX_GPIO_AUX_L                                 8'h0
 */
typedef union _AON_FAST_REG_PAD_AUX_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_AUX_R: 8;
        uint16_t PMUX_GPIO_AUX_L: 8;
    };
} AON_FAST_REG_PAD_AUX_TYPE;

/* 0x4C4
    7:0     R/W PMUX_GPIO_MICBIAS                               8'h0
    15:8    R/W PMUX_GPIO_DUMMY1                                8'h0
 */
typedef union _AON_FAST_REG_PAD_MICBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_MICBIAS: 8;
        uint16_t PMUX_GPIO_DUMMY1: 8;
    };
} AON_FAST_REG_PAD_MICBIAS_TYPE;

/* 0x4C6
    7:0     R/W PMUX_GPIO_32KXI                                 8'h0
    15:8    R/W PMUX_GPIO_32KXO                                 8'h0
 */
typedef union _AON_FAST_REG_PAD_32K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_32KXI: 8;
        uint16_t PMUX_GPIO_32KXO: 8;
    };
} AON_FAST_REG_PAD_32K_TYPE;

/* 0x4C8
    3:0     R/W PMUX_DBG_MODE_SEL                               4'h2
    4       R/W REG_TEST_MODE_DUMMY1                            0
    5       R/W SPIC_MASTER_3_EN                                0
    6       R/W SPIC_MASTER_2_EN                                0
    7       R/W SPIC_MASTER_1_EN                                0
    8       R/W SPIC_MASTER_0_EN                                0
    9       R/W PMUX_SDIO_SEL                                   0
    10      R/W PMUX_SDIO_EN                                    0
    11      R/W PMUX_FLASH_EN                                   0
    12      R/W PMUX_HCI_UART_DISABLE                           0
    13      R/W PMUX_FLASH_EXTC_EN                              0
    14      R/W r_clk_cpu_dsp_dbg_en                            0
    15      R/W bypass_non_std_det                              0
 */
typedef union _AON_FAST_REG_TEST_MODE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_DBG_MODE_SEL: 4;
        uint16_t REG_TEST_MODE_DUMMY1: 1;
        uint16_t SPIC_MASTER_3_EN: 1;
        uint16_t SPIC_MASTER_2_EN: 1;
        uint16_t SPIC_MASTER_1_EN: 1;
        uint16_t SPIC_MASTER_0_EN: 1;
        uint16_t PMUX_SDIO_SEL: 1;
        uint16_t PMUX_SDIO_EN: 1;
        uint16_t PMUX_FLASH_EN: 1;
        uint16_t PMUX_HCI_UART_DISABLE: 1;
        uint16_t PMUX_FLASH_EXTC_EN: 1;
        uint16_t r_clk_cpu_dsp_dbg_en: 1;
        uint16_t bypass_non_std_det: 1;
    };
} AON_FAST_REG_TEST_MODE_TYPE;

/* 0x4CA
    3:0     R/W PMUX_TEST_MODE                                  4'h1
    4       R/W REG1X_TEST_MODE_DUMMY1                          0
    5       R/W spic_mon_en                                     0
    6       R/W bypass_dcd_dbnc                                 0
    7       R/W PMUX_TEST_MODE_EN                               0
    10:8    R/W SPI_DLY_MON_SEL                                 3'h0
    11      R/W PMUX_EXT_RFAFE_EN                               0
    12      R/W PMUX_SWO_EN                                     0
    15:13   R/W PMUX_SWO_MODE                                   3'h0
 */
typedef union _AON_FAST_REG1X_TEST_MODE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_TEST_MODE: 4;
        uint16_t REG1X_TEST_MODE_DUMMY1: 1;
        uint16_t spic_mon_en: 1;
        uint16_t bypass_dcd_dbnc: 1;
        uint16_t PMUX_TEST_MODE_EN: 1;
        uint16_t SPI_DLY_MON_SEL: 3;
        uint16_t PMUX_EXT_RFAFE_EN: 1;
        uint16_t PMUX_SWO_EN: 1;
        uint16_t PMUX_SWO_MODE: 3;
    };
} AON_FAST_REG1X_TEST_MODE_TYPE;

/* 0x4CC
    15:0    R/W PMUX_DIGI_MODE_EN                               16'h0
 */
typedef union _AON_FAST_REG0X_PMUX_DIGI_MODE_EN_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_DIGI_MODE_EN;
    };
} AON_FAST_REG0X_PMUX_DIGI_MODE_EN_TYPE;

/* 0x4CE
    15:0    R/W REG1X_PMUX_DIGI_MODE_EN_DUMMY1                  16'h0
 */
typedef union _AON_FAST_REG1X_PMUX_DIGI_MODE_EN_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG1X_PMUX_DIGI_MODE_EN_DUMMY1;
    };
} AON_FAST_REG1X_PMUX_DIGI_MODE_EN_TYPE;

/* 0x4D0
    3:0     R/W PMUX_SEL_ADC_0_PWR_DOMN                         4'h0
    7:4     R/W PMUX_SEL_ADC_1_PWR_DOMN                         4'h0
    11:8    R/W PMUX_SEL_ADC_2_PWR_DOMN                         4'h0
    15:12   R/W PMUX_SEL_ADC_3_PWR_DOMN                         4'h0
 */
typedef union _AON_FAST_REG0X_PAD_POWER_DOMAIN_ADC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_ADC_0_PWR_DOMN: 4;
        uint16_t PMUX_SEL_ADC_1_PWR_DOMN: 4;
        uint16_t PMUX_SEL_ADC_2_PWR_DOMN: 4;
        uint16_t PMUX_SEL_ADC_3_PWR_DOMN: 4;
    };
} AON_FAST_REG0X_PAD_POWER_DOMAIN_ADC_TYPE;

/* 0x4D2
    3:0     R/W PMUX_SEL_ADC_4_PWR_DOMN                         4'h0
    7:4     R/W PMUX_SEL_ADC_5_PWR_DOMN                         4'h0
    11:8    R/W PMUX_SEL_ADC_6_PWR_DOMN                         4'h0
    15:12   R/W PMUX_SEL_ADC_7_PWR_DOMN                         4'h0
 */
typedef union _AON_FAST_REG1X_PAD_POWER_DOMAIN_ADC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_ADC_4_PWR_DOMN: 4;
        uint16_t PMUX_SEL_ADC_5_PWR_DOMN: 4;
        uint16_t PMUX_SEL_ADC_6_PWR_DOMN: 4;
        uint16_t PMUX_SEL_ADC_7_PWR_DOMN: 4;
    };
} AON_FAST_REG1X_PAD_POWER_DOMAIN_ADC_TYPE;

/* 0x4D4
    3:0     R/W PMUX_SEL_P1_0_PWR_DOMN                          4'h0
    7:4     R/W PMUX_SEL_P1_1_PWR_DOMN                          4'h0
    11:8    R/W PMUX_SEL_P1_2_PWR_DOMN                          4'h0
    15:12   R/W PMUX_SEL_P1_3_PWR_DOMN                          4'h0
 */
typedef union _AON_FAST_REG0X_PAD_POWER_DOMAIN_P1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P1_0_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P1_1_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P1_2_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P1_3_PWR_DOMN: 4;
    };
} AON_FAST_REG0X_PAD_POWER_DOMAIN_P1_TYPE;

/* 0x4D6
    3:0     R/W PMUX_SEL_P1_4_PWR_DOMN                          4'h0
    7:4     R/W PMUX_SEL_P1_5_PWR_DOMN                          4'h0
    11:8    R/W PMUX_SEL_P1_6_PWR_DOMN                          4'h0
    15:12   R/W PMUX_SEL_P1_7_PWR_DOMN                          4'h0
 */
typedef union _AON_FAST_REG1X_PAD_POWER_DOMAIN_P1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P1_4_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P1_5_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P1_6_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P1_7_PWR_DOMN: 4;
    };
} AON_FAST_REG1X_PAD_POWER_DOMAIN_P1_TYPE;

/* 0x4D8
    3:0     R/W PMUX_SEL_P2_0_PWR_DOMN                          4'h0
    7:4     R/W PMUX_SEL_P2_1_PWR_DOMN                          4'h0
    11:8    R/W PMUX_SEL_P2_2_PWR_DOMN                          4'h0
    15:12   R/W PMUX_SEL_P2_3_PWR_DOMN                          4'h0
 */
typedef union _AON_FAST_REG0X_PAD_POWER_DOMAIN_P2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P2_0_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P2_1_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P2_2_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P2_3_PWR_DOMN: 4;
    };
} AON_FAST_REG0X_PAD_POWER_DOMAIN_P2_TYPE;

/* 0x4DA
    3:0     R/W PMUX_SEL_P2_4_PWR_DOMN                          4'h0
    7:4     R/W PMUX_SEL_P2_5_PWR_DOMN                          4'h0
    11:8    R/W PMUX_SEL_P2_6_PWR_DOMN                          4'h0
    15:12   R/W PMUX_SEL_P2_7_PWR_DOMN                          4'h0
 */
typedef union _AON_FAST_REG1X_PAD_POWER_DOMAIN_P2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P2_4_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P2_5_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P2_6_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P2_7_PWR_DOMN: 4;
    };
} AON_FAST_REG1X_PAD_POWER_DOMAIN_P2_TYPE;

/* 0x4DC
    3:0     R/W PMUX_SEL_P3_0_PWR_DOMN                          4'h0
    7:4     R/W PMUX_SEL_P3_1_PWR_DOMN                          4'h0
    11:8    R/W PMUX_SEL_P3_2_PWR_DOMN                          4'h0
    15:12   R/W PMUX_SEL_P3_3_PWR_DOMN                          4'h0
 */
typedef union _AON_FAST_REG0X_PAD_POWER_DOMAIN_P3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P3_0_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P3_1_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P3_2_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P3_3_PWR_DOMN: 4;
    };
} AON_FAST_REG0X_PAD_POWER_DOMAIN_P3_TYPE;

/* 0x4DE
    3:0     R/W PMUX_SEL_P3_4_PWR_DOMN                          4'h0
    7:4     R/W PMUX_SEL_P3_5_PWR_DOMN                          4'h0
    11:8    R/W PMUX_SEL_P3_6_PWR_DOMN                          4'h0
    15:12   R/W PMUX_SEL_P3_7_PWR_DOMN                          4'h0
 */
typedef union _AON_FAST_REG1X_PAD_POWER_DOMAIN_P3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P3_4_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P3_5_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P3_6_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P3_7_PWR_DOMN: 4;
    };
} AON_FAST_REG1X_PAD_POWER_DOMAIN_P3_TYPE;

/* 0x4E0
    3:0     R/W PMUX_SEL_P4_0_PWR_DOMN                          4'h0
    7:4     R/W PMUX_SEL_P4_1_PWR_DOMN                          4'h0
    11:8    R/W PMUX_SEL_P4_2_PWR_DOMN                          4'h0
    15:12   R/W PMUX_SEL_P4_3_PWR_DOMN                          4'h0
 */
typedef union _AON_FAST_REG0X_PAD_POWER_DOMAIN_P4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P4_0_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P4_1_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P4_2_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P4_3_PWR_DOMN: 4;
    };
} AON_FAST_REG0X_PAD_POWER_DOMAIN_P4_TYPE;

/* 0x4E2
    3:0     R/W PMUX_SEL_P4_4_PWR_DOMN                          4'h0
    7:4     R/W PMUX_SEL_P4_5_PWR_DOMN                          4'h0
    11:8    R/W PMUX_SEL_P4_6_PWR_DOMN                          4'h0
    15:12   R/W PMUX_SEL_P4_7_PWR_DOMN                          4'h0
 */
typedef union _AON_FAST_REG1X_PAD_POWER_DOMAIN_P4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P4_4_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P4_5_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P4_6_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P4_7_PWR_DOMN: 4;
    };
} AON_FAST_REG1X_PAD_POWER_DOMAIN_P4_TYPE;

/* 0x4E4
    3:0     R/W PMUX_SEL_P5_0_PWR_DOMN                          4'h0
    7:4     R/W PMUX_SEL_P5_1_PWR_DOMN                          4'h0
    11:8    R/W PMUX_SEL_P5_2_PWR_DOMN                          4'h0
    15:12   R/W PMUX_SEL_P5_3_PWR_DOMN                          4'h0
 */
typedef union _AON_FAST_REG0X_PAD_POWER_DOMAIN_P5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P5_0_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P5_1_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P5_2_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P5_3_PWR_DOMN: 4;
    };
} AON_FAST_REG0X_PAD_POWER_DOMAIN_P5_TYPE;

/* 0x4E6
    3:0     R/W PMUX_SEL_P5_4_PWR_DOMN                          4'h0
    7:4     R/W PMUX_SEL_P5_5_PWR_DOMN                          4'h0
    11:8    R/W PMUX_SEL_P5_6_PWR_DOMN                          4'h0
    15:12   R/W PMUX_SEL_P5_7_PWR_DOMN                          4'h0
 */
typedef union _AON_FAST_REG1X_PAD_POWER_DOMAIN_P5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P5_4_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P5_5_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P5_6_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P5_7_PWR_DOMN: 4;
    };
} AON_FAST_REG1X_PAD_POWER_DOMAIN_P5_TYPE;

/* 0x4E8
    3:0     R/W PMUX_SEL_P6_0_PWR_DOMN                          4'h0
    7:4     R/W PMUX_SEL_P6_1_PWR_DOMN                          4'h0
    11:8    R/W PMUX_SEL_P6_2_PWR_DOMN                          4'h0
    15:12   R/W PMUX_SEL_P6_3_PWR_DOMN                          4'h0
 */
typedef union _AON_FAST_REG0X_PAD_POWER_DOMAIN_P6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P6_0_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P6_1_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P6_2_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P6_3_PWR_DOMN: 4;
    };
} AON_FAST_REG0X_PAD_POWER_DOMAIN_P6_TYPE;

/* 0x4EA
    3:0     R/W PMUX_SEL_LOUT_N_PWR_DOMN                        4'h0
    7:4     R/W PMUX_SEL_LOUT_P_PWR_DOMN                        4'h0
    11:8    R/W PMUX_SEL_ROUT_N_PWR_DOMN                        4'h0
    15:12   R/W PMUX_SEL_ROUT_P_PWR_DOMN                        4'h0
 */
typedef union _AON_FAST_REG_PAD_POWER_DOMAIN_LOUT_ROUT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_LOUT_N_PWR_DOMN: 4;
        uint16_t PMUX_SEL_LOUT_P_PWR_DOMN: 4;
        uint16_t PMUX_SEL_ROUT_N_PWR_DOMN: 4;
        uint16_t PMUX_SEL_ROUT_P_PWR_DOMN: 4;
    };
} AON_FAST_REG_PAD_POWER_DOMAIN_LOUT_ROUT_TYPE;

/* 0x4EC
    3:0     R/W PMUX_SEL_MIC1_N_PWR_DOMN                        4'h0
    7:4     R/W PMUX_SEL_MIC1_P_PWR_DOMN                        4'h0
    11:8    R/W PMUX_SEL_MIC2_N_PWR_DOMN                        4'h0
    15:12   R/W PMUX_SEL_MIC2_P_PWR_DOMN                        4'h0
 */
typedef union _AON_FAST_REG_PAD_POWER_DOMAIN_MIC1_MIC2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_MIC1_N_PWR_DOMN: 4;
        uint16_t PMUX_SEL_MIC1_P_PWR_DOMN: 4;
        uint16_t PMUX_SEL_MIC2_N_PWR_DOMN: 4;
        uint16_t PMUX_SEL_MIC2_P_PWR_DOMN: 4;
    };
} AON_FAST_REG_PAD_POWER_DOMAIN_MIC1_MIC2_TYPE;

/* 0x4EE
    3:0     R/W PMUX_SEL_MIC3_N_PWR_DOMN                        4'h0
    7:4     R/W PMUX_SEL_MIC3_P_PWR_DOMN                        4'h0
    11:8    R/W PMUX_SEL_MIC4_N_PWR_DOMN                        4'h0
    15:12   R/W PMUX_SEL_MIC4_P_PWR_DOMN                        4'h0
 */
typedef union _AON_FAST_REG_PAD_POWER_DOMAIN_MIC3_MIC4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_MIC3_N_PWR_DOMN: 4;
        uint16_t PMUX_SEL_MIC3_P_PWR_DOMN: 4;
        uint16_t PMUX_SEL_MIC4_N_PWR_DOMN: 4;
        uint16_t PMUX_SEL_MIC4_P_PWR_DOMN: 4;
    };
} AON_FAST_REG_PAD_POWER_DOMAIN_MIC3_MIC4_TYPE;

/* 0x4F0
    3:0     R/W PMUX_SEL_MIC5_N_PWR_DOMN                        4'h0
    7:4     R/W PMUX_SEL_MIC5_P_PWR_DOMN                        4'h0
    11:8    R/W PMUX_SEL_MIC6_N_PWR_DOMN                        4'h0
    15:12   R/W PMUX_SEL_MIC6_P_PWR_DOMN                        4'h0
 */
typedef union _AON_FAST_REG_PAD_POWER_DOMAIN_MIC5_MIC6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_MIC5_N_PWR_DOMN: 4;
        uint16_t PMUX_SEL_MIC5_P_PWR_DOMN: 4;
        uint16_t PMUX_SEL_MIC6_N_PWR_DOMN: 4;
        uint16_t PMUX_SEL_MIC6_P_PWR_DOMN: 4;
    };
} AON_FAST_REG_PAD_POWER_DOMAIN_MIC5_MIC6_TYPE;

/* 0x4F2
    3:0     R/W PMUX_SEL_AUX_R_PWR_DOMN                         4'h0
    7:4     R/W PMUX_SEL_AUX_L_PWR_DOMN                         4'h0
    11:8    R/W PMUX_SEL_MICBIAS_PWR_DOMN                       4'h0
    15:12   R/W REG_PAD_POWER_DOMAIN_AUX_MICBIAS_DUMMY1         4'h0
 */
typedef union _AON_FAST_REG_PAD_POWER_DOMAIN_AUX_MICBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_AUX_R_PWR_DOMN: 4;
        uint16_t PMUX_SEL_AUX_L_PWR_DOMN: 4;
        uint16_t PMUX_SEL_MICBIAS_PWR_DOMN: 4;
        uint16_t REG_PAD_POWER_DOMAIN_AUX_MICBIAS_DUMMY1: 4;
    };
} AON_FAST_REG_PAD_POWER_DOMAIN_AUX_MICBIAS_TYPE;

/* 0x4F4
    3:0     R/W PMUX_SEL_32KXI_PWR_DOMN                         4'h0
    7:4     R/W PMUX_SEL_32KXO_PWR_DOMN                         4'h0
    11:8    R/W REG_PAD_POWER_DOMAIN_32K_DUMMY2                 4'h0
    15:12   R/W REG_PAD_POWER_DOMAIN_32K_DUMMY1                 4'h0
 */
typedef union _AON_FAST_REG_PAD_POWER_DOMAIN_32K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_32KXI_PWR_DOMN: 4;
        uint16_t PMUX_SEL_32KXO_PWR_DOMN: 4;
        uint16_t REG_PAD_POWER_DOMAIN_32K_DUMMY2: 4;
        uint16_t REG_PAD_POWER_DOMAIN_32K_DUMMY1: 4;
    };
} AON_FAST_REG_PAD_POWER_DOMAIN_32K_TYPE;

/* 0x4F6
    0       R/W SPIC_PULL_SEL_SIO0_PULL_CTRL                    1'b0
    1       R/W SPIC_PULL_SEL_SIO1_PULL_CTRL                    1'b0
    2       R/W SPIC_PULL_SEL_SIO2_PULL_CTRL                    1'b0
    3       R/W SPIC_PULL_SEL_SIO3_PULL_CTRL                    1'b0
    4       R/W SPIC1_PULL_SEL_SIO0_PULL_CTRL                   1'b0
    5       R/W SPIC1_PULL_SEL_SIO1_PULL_CTRL                   1'b0
    6       R/W SPIC1_PULL_SEL_SIO2_PULL_CTRL                   1'b0
    7       R/W SPIC1_PULL_SEL_SIO3_PULL_CTRL                   1'b0
    8       R/W SPIC2_PULL_SEL_SIO0_PULL_CTRL                   1'b0
    9       R/W SPIC2_PULL_SEL_SIO1_PULL_CTRL                   1'b0
    10      R/W SPIC2_PULL_SEL_SIO2_PULL_CTRL                   1'b0
    11      R/W SPIC2_PULL_SEL_SIO3_PULL_CTRL                   1'b0
    12      R/W SPIC3_PULL_SEL_SIO0_PULL_CTRL                   1'b0
    13      R/W SPIC3_PULL_SEL_SIO1_PULL_CTRL                   1'b0
    14      R/W SPIC3_PULL_SEL_SIO2_PULL_CTRL                   1'b0
    15      R/W SPIC3_PULL_SEL_SIO3_PULL_CTRL                   1'b0
 */
typedef union _AON_FAST_REG_SPIC_PULL_SELECT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SPIC_PULL_SEL_SIO0_PULL_CTRL: 1;
        uint16_t SPIC_PULL_SEL_SIO1_PULL_CTRL: 1;
        uint16_t SPIC_PULL_SEL_SIO2_PULL_CTRL: 1;
        uint16_t SPIC_PULL_SEL_SIO3_PULL_CTRL: 1;
        uint16_t SPIC1_PULL_SEL_SIO0_PULL_CTRL: 1;
        uint16_t SPIC1_PULL_SEL_SIO1_PULL_CTRL: 1;
        uint16_t SPIC1_PULL_SEL_SIO2_PULL_CTRL: 1;
        uint16_t SPIC1_PULL_SEL_SIO3_PULL_CTRL: 1;
        uint16_t SPIC2_PULL_SEL_SIO0_PULL_CTRL: 1;
        uint16_t SPIC2_PULL_SEL_SIO1_PULL_CTRL: 1;
        uint16_t SPIC2_PULL_SEL_SIO2_PULL_CTRL: 1;
        uint16_t SPIC2_PULL_SEL_SIO3_PULL_CTRL: 1;
        uint16_t SPIC3_PULL_SEL_SIO0_PULL_CTRL: 1;
        uint16_t SPIC3_PULL_SEL_SIO1_PULL_CTRL: 1;
        uint16_t SPIC3_PULL_SEL_SIO2_PULL_CTRL: 1;
        uint16_t SPIC3_PULL_SEL_SIO3_PULL_CTRL: 1;
    };
} AON_FAST_REG_SPIC_PULL_SELECT_TYPE;

/* 0x4F8
    7:0     R/W PMUX_GPIO_P6_2                                  8'h0
    15:8    R/W PMUX_GPIO_P6_3                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P6_2_P6_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P6_2: 8;
        uint16_t PMUX_GPIO_P6_3: 8;
    };
} AON_FAST_REG_PAD_P6_2_P6_3_TYPE;

/* 0x4FA
    7:0     R/W PMUX_GPIO_P6_4                                  8'h0
    15:8    R/W PMUX_GPIO_P6_5                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P6_4_P6_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P6_4: 8;
        uint16_t PMUX_GPIO_P6_5: 8;
    };
} AON_FAST_REG_PAD_P6_4_P6_5_TYPE;

/* 0x4FC
    7:0     R/W PMUX_GPIO_P6_6                                  8'h0
    15:8    R/W PMUX_GPIO_P6_7                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P6_6_P6_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P6_6: 8;
        uint16_t PMUX_GPIO_P6_7: 8;
    };
} AON_FAST_REG_PAD_P6_6_P6_7_TYPE;

/* 0x4FE
    7:0     R/W PMUX_GPIO_P7_0                                  8'h0
    15:8    R/W PMUX_GPIO_P7_1                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P7_0_P7_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P7_0: 8;
        uint16_t PMUX_GPIO_P7_1: 8;
    };
} AON_FAST_REG_PAD_P7_0_P7_1_TYPE;

/* 0x500
    7:0     R/W PMUX_GPIO_P7_2                                  8'h0
    15:8    R/W PMUX_GPIO_P7_3                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P7_2_P7_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P7_2: 8;
        uint16_t PMUX_GPIO_P7_3: 8;
    };
} AON_FAST_REG_PAD_P7_2_P7_3_TYPE;

/* 0x502
    7:0     R/W PMUX_GPIO_P7_4                                  8'h0
    15:8    R/W PMUX_GPIO_P7_5                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P7_4_P7_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P7_4: 8;
        uint16_t PMUX_GPIO_P7_5: 8;
    };
} AON_FAST_REG_PAD_P7_4_P7_5_TYPE;

/* 0x504
    7:0     R/W PMUX_GPIO_P7_6                                  8'h0
    15:8    R/W PMUX_GPIO_P7_7                                  8'h0
 */
typedef union _AON_FAST_REG_PAD_P7_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_GPIO_P7_6: 8;
        uint16_t PMUX_GPIO_P7_7: 8;
    };
} AON_FAST_REG_PAD_P7_6_TYPE;

/* 0x506
    3:0     R/W PMUX_SEL_P6_4_PWR_DOMN                          4'h0
    7:4     R/W PMUX_SEL_P6_5_PWR_DOMN                          4'h0
    11:8    R/W PMUX_SEL_P6_6_PWR_DOMN                          4'h0
    15:12   R/W PMUX_SEL_P6_7_PWR_DOMN                          4'h0
 */
typedef union _AON_FAST_REG1X_PAD_POWER_DOMAIN_P6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P6_4_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P6_5_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P6_6_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P6_7_PWR_DOMN: 4;
    };
} AON_FAST_REG1X_PAD_POWER_DOMAIN_P6_TYPE;

/* 0x508
    3:0     R/W PMUX_SEL_P7_0_PWR_DOMN                          4'h0
    7:4     R/W PMUX_SEL_P7_1_PWR_DOMN                          4'h0
    11:8    R/W PMUX_SEL_P7_2_PWR_DOMN                          4'h0
    15:12   R/W PMUX_SEL_P7_3_PWR_DOMN                          4'h0
 */
typedef union _AON_FAST_REG0X_PAD_POWER_DOMAIN_P7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P7_0_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P7_1_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P7_2_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P7_3_PWR_DOMN: 4;
    };
} AON_FAST_REG0X_PAD_POWER_DOMAIN_P7_TYPE;

/* 0x50A
    3:0     R/W PMUX_SEL_P7_4_PWR_DOMN                          4'h0
    7:4     R/W PMUX_SEL_P7_5_PWR_DOMN                          4'h0
    11:8    R/W PMUX_SEL_P7_6_PWR_DOMN                          4'h0
    15:12   R/W PMUX_SEL_P7_7_PWR_DOMN                          4'h0
 */
typedef union _AON_FAST_REG1X_PAD_POWER_DOMAIN_P7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P7_4_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P7_5_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P7_6_PWR_DOMN: 4;
        uint16_t PMUX_SEL_P7_7_PWR_DOMN: 4;
    };
} AON_FAST_REG1X_PAD_POWER_DOMAIN_P7_TYPE;

/* 0x50C
    3:0     R/W REG2X_TEST_MODE_DUMMY1                          4'h0
    15:4    R/W REG2X_TEST_MODE_DUMMY2                          12'h0
 */
typedef union _AON_FAST_REG2X_TEST_MODE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG2X_TEST_MODE_DUMMY1: 4;
        uint16_t REG2X_TEST_MODE_DUMMY2: 12;
    };
} AON_FAST_REG2X_TEST_MODE_TYPE;

/* 0x50E
    0       R/W SPIC4_PULL_SEL_SIO0_PULL_CTRL                   1'b0
    1       R/W SPIC4_PULL_SEL_SIO1_PULL_CTRL                   1'b0
    2       R/W SPIC4_PULL_SEL_SIO2_PULL_CTRL                   1'b0
    3       R/W SPIC4_PULL_SEL_SIO3_PULL_CTRL                   1'b0
    4       R/W SPIC4_PULL_SEL_SIO4_PULL_CTRL                   1'b0
    5       R/W SPIC4_PULL_SEL_SIO5_PULL_CTRL                   1'b0
    6       R/W SPIC4_PULL_SEL_SIO6_PULL_CTRL                   1'b0
    7       R/W SPIC4_PULL_SEL_SIO7_PULL_CTRL                   1'b0
    15:8    R/W REG1X_SPIC_PULL_SELECT_DUMMY1                   8'h0
 */
typedef union _AON_FAST_REG1X_SPIC_PULL_SELECT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SPIC4_PULL_SEL_SIO0_PULL_CTRL: 1;
        uint16_t SPIC4_PULL_SEL_SIO1_PULL_CTRL: 1;
        uint16_t SPIC4_PULL_SEL_SIO2_PULL_CTRL: 1;
        uint16_t SPIC4_PULL_SEL_SIO3_PULL_CTRL: 1;
        uint16_t SPIC4_PULL_SEL_SIO4_PULL_CTRL: 1;
        uint16_t SPIC4_PULL_SEL_SIO5_PULL_CTRL: 1;
        uint16_t SPIC4_PULL_SEL_SIO6_PULL_CTRL: 1;
        uint16_t SPIC4_PULL_SEL_SIO7_PULL_CTRL: 1;
        uint16_t REG1X_SPIC_PULL_SELECT_DUMMY1: 8;
    };
} AON_FAST_REG1X_SPIC_PULL_SELECT_TYPE;

/* 0x580
    15:0    R/W reg_com2_share_en                               16'hfff
 */
typedef union _AON_FAST_REG_0x580_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_com2_share_en;
    };
} AON_FAST_REG_0x580_TYPE;

/* 0x582
    15:0    R/W reg_dsp1_sys_ram[15:0]                          16'h0
 */
typedef union _AON_FAST_REG_0x582_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_dsp1_sys_ram_15_0;
    };
} AON_FAST_REG_0x582_TYPE;

/* 0x584
    3:0     R/W reg_dsp1_sys_ram[19:16]                         4'h0
    11:4    R/W reg_dsp1_cache_sys_ram                          8'h0
    15:12   R/W MEMORY_REG2X_DUMMY1                             4'h0
 */
typedef union _AON_FAST_REG_0x584_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_dsp1_sys_ram_19_16: 4;
        uint16_t reg_dsp1_cache_sys_ram: 8;
        uint16_t MEMORY_REG2X_DUMMY1: 4;
    };
} AON_FAST_REG_0x584_TYPE;

/* 0x586
    11:0    R/W reg_dsp2_sys_ram                                8'h0
    15:12   R/W MEMORY_REG3X_DUMMY1                             8'h0
 */
typedef union _AON_FAST_REG_0x586_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_dsp2_sys_ram: 12;
        uint16_t MEMORY_REG3X_DUMMY1: 4;
    };
} AON_FAST_REG_0x586_TYPE;

/* 0x588
    3:0     R/W reg_vadbuf_ram_en                               4'hf
    15:4    R   reserved                                        12'b0
 */
typedef union _AON_FAST_REG_0x588_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_vadbuf_ram_en: 4;
        uint16_t reserved: 12;
    };
} AON_FAST_REG_0x588_TYPE;

/* 0x58A
    0       R/W ram_data_vsel                                   1'b1
    1       R/W ram_com01_vsel                                  1'b1
    2       R/W ram_com2_vsel                                   1'b1
    3       R/W ram_buf_vsel                                    1'b1
    4       R/W ram_mac_vsel                                    1'b1
    5       R/W ram_mdm_vsel                                    1'b1
    6       R/W rom_dsp1_vsel                                   1'b1
    7       R/W rom_dsp2_vsel                                   1'b1
    8       R/W ram_dsp1_vsel                                   1'b1
    9       R/W ram_dsp2_vsel                                   1'b1
    10      R/W ram_dsp_sys_vsel                                1'b1
    11      R/W ram_vadbuf_vsel                                 1'b1
    12      R/W ram_ancdsp_vsel                                 1'b1
    13      R/W ram_nna_vsel                                    1'b1
    14      R   reserved                                        1'b0
    15      R/W rom_ancdsp_vsel                                 1'b1
 */
typedef union _AON_FAST_REG_RAMROM_VSEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t ram_data_vsel: 1;
        uint16_t ram_com01_vsel: 1;
        uint16_t ram_com2_vsel: 1;
        uint16_t ram_buf_vsel: 1;
        uint16_t ram_mac_vsel: 1;
        uint16_t ram_mdm_vsel: 1;
        uint16_t rom_dsp1_vsel: 1;
        uint16_t rom_dsp2_vsel: 1;
        uint16_t ram_dsp1_vsel: 1;
        uint16_t ram_dsp2_vsel: 1;
        uint16_t ram_dsp_sys_vsel: 1;
        uint16_t ram_vadbuf_vsel: 1;
        uint16_t ram_ancdsp_vsel: 1;
        uint16_t ram_nna_vsel: 1;
        uint16_t reserved: 1;
        uint16_t rom_ancdsp_vsel: 1;
    };
} AON_FAST_REG_RAMROM_VSEL_TYPE;

/* 0x58C
    5:0     R/W reg_rom_mcu                                     6'b010011
    15:6    R   reserved                                        10'b0
 */
typedef union _AON_FAST_REG_ROM_MCU_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_rom_mcu: 6;
        uint16_t reserved: 10;
    };
} AON_FAST_REG_ROM_MCU_TYPE;

/* 0x58E
    4:0     R/W reg_rom_mdm                                     5'b10011
    15:5    R   reserved                                        11'b0
 */
typedef union _AON_FAST_REG_ROM_MDM_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_rom_mdm: 5;
        uint16_t reserved: 11;
    };
} AON_FAST_REG_ROM_MDM_TYPE;

/* 0x590
    5:0     R/W reg_rom_dsp1_hv                                 6'b010011
    11:6    R/W reg_rom_dsp1_lv                                 6'b010010
    15:12   R   reserved                                        4'b0
 */
typedef union _AON_FAST_REG_ROM_DSP1_HV_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_rom_dsp1_hv: 6;
        uint16_t reg_rom_dsp1_lv: 6;
        uint16_t reserved: 4;
    };
} AON_FAST_REG_ROM_DSP1_HV_LV_TYPE;

/* 0x592
    5:0     R/W reg_rom_dsp2_hv                                 6'b001010
    11:6    R/W reg_rom_dsp2_lv                                 6'b001011
    15:12   R   reserved                                        4'b0
 */
typedef union _AON_FAST_REG_ROM_DSP2_HV_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_rom_dsp2_hv: 6;
        uint16_t reg_rom_dsp2_lv: 6;
        uint16_t reserved: 4;
    };
} AON_FAST_REG_ROM_DSP2_HV_LV_TYPE;

/* 0x594
    5:0     R/W reg_rom_rfc                                     6'b010011
    15:6    R   reserved                                        10'b0
 */
typedef union _AON_FAST_REG_ROM_RFC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_rom_rfc: 6;
        uint16_t reserved: 10;
    };
} AON_FAST_REG_ROM_RFC_TYPE;

/* 0x596
    13:0    R/W reg_ram_data_hv                                 14'b00001000010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DATA_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_data_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DATA_HV_TYPE;

/* 0x598
    13:0    R/W reg_ram_data_lv                                 14'b00001100110001
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DATA_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_data_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DATA_LV_TYPE;

/* 0x59A
    13:0    R/W reg_ram_com01_hv                                14'b00001000010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_COM01_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_com01_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_COM01_HV_TYPE;

/* 0x59C
    13:0    R/W reg_ram_com01_lv                                14'b00001100110001
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_COM01_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_com01_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_COM01_LV_TYPE;

/* 0x59E
    13:0    R/W reg_ram_com2_hv                                 14'b00001000010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_COM2_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_com2_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_COM2_HV_TYPE;

/* 0x5A0
    13:0    R/W reg_ram_com2_lv                                 14'b00001100110001
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_COM2_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_com2_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_COM2_LV_TYPE;

/* 0x5A2
    13:0    R/W reg_ram_cache                                   14'b00001000010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_CACHE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_cache: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_CACHE_TYPE;

/* 0x5A4
    13:0    R/W reg_ram_buf_hv                                  14'b00001000010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_BUF_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_buf_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_BUF_HV_TYPE;

/* 0x5A6
    13:0    R/W reg_ram_buf_lv                                  14'b00001100110001
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_BUF_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_buf_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_BUF_LV_TYPE;

/* 0x5A8
    13:0    R/W reg_ram_rsa                                     14'b00001100010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_RSA_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_rsa: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_RSA_TYPE;

/* 0x5AA
    13:0    R/W reg_ram_sdio                                    14'b00001100010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_SDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_sdio: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_SDIO_TYPE;

/* 0x5AC
    13:0    R/W reg_ram_usb                                     14'b00001000010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_USB_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_usb: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_USB_TYPE;

/* 0x5AE
    13:0    R/W reg_ram_mac_fifo_hv                             14'b00001000010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_MAC_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_mac_fifo_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_MAC_HV_TYPE;

/* 0x5B0
    13:0    R/W reg_ram_mac_fifo_lv                             14'b00001100110001
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_MAC_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_mac_fifo_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_MAC_LV_TYPE;

/* 0x5B2
    13:0    R/W reg_ram_mac_fifo_rf_hv                          14'b00001100010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_MAC_RF_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_mac_fifo_rf_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_MAC_RF_HV_TYPE;

/* 0x5B4
    13:0    R/W reg_ram_mac_fifo_rf_lv                          14'b00001110110001
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_MAC_RF_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_mac_fifo_rf_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_MAC_RF_LV_TYPE;

/* 0x5B6
    13:0    R/W reg_ram_mdm_hv                                  14'b00001000010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_MDM_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_mdm_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_MDM_HV_TYPE;

/* 0x5B8
    13:0    R/W reg_ram_mdm_lv                                  14'b00001100110001
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_MDM_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_mdm_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_MDM_LV_TYPE;

/* 0x5BA
    13:0    R/W reg_ram_mdm_rf_hv                               14'b00001100010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_MDM_RF_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_mdm_rf_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_MDM_RF_HV_TYPE;

/* 0x5BC
    13:0    R/W reg_ram_mdm_rf_lv                               14'b00001110110001
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_MDM_RF_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_mdm_rf_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_MDM_RF_LV_TYPE;

/* 0x5BE
    13:0    R/W reg_ram_dsp1_hv                                 14'b00001000010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DSP1_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp1_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DSP1_HV_TYPE;

/* 0x5C0
    13:0    R/W reg_ram_dsp1_lv                                 14'b00001100110001
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DSP1_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp1_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DSP1_LV_TYPE;

/* 0x5C2
    13:0    R/W reg_ram_dsp1_cache_hv                           14'b00001000010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DSP1_CACHE_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp1_cache_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DSP1_CACHE_HV_TYPE;

/* 0x5C4
    13:0    R/W reg_ram_dsp1_cache_lv                           14'b00001100110001
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DSP1_CACHE_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp1_cache_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DSP1_CACHE_LV_TYPE;

/* 0x5C6
    13:0    R/W reg_ram_dsp1_fft_hv                             14'b00001000010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DSP1_FFT_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp1_fft_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DSP1_FFT_HV_TYPE;

/* 0x5C8
    13:0    R/W reg_ram_dsp1_fft_lv                             14'b00001100110001
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DSP1_FFT_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp1_fft_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DSP1_FFT_LV_TYPE;

/* 0x5CA
    13:0    R/W reg_ram_dsp2_hv                                 14'b00000110101000
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DSP2_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp2_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DSP2_HV_TYPE;

/* 0x5CC
    13:0    R/W reg_ram_dsp2_lv                                 14'b00010100001100
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DSP2_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp2_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DSP2_LV_TYPE;

/* 0x5CE
    13:0    R/W reg_ram_dsp2_cache_hv                           14'b00000010001100
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DSP2_CACHE_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp2_cache_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DSP2_CACHE_HV_TYPE;

/* 0x5D0
    13:0    R/W reg_ram_dsp2_cache_lv                           14'b00010100001100
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DSP2_CACHE_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp2_cache_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DSP2_CACHE_LV_TYPE;

/* 0x5D2
    13:0    R/W reg_ram_dsp2_fft_hv                             14'b00000110101000
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DSP2_FFT_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp2_fft_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DSP2_FFT_HV_TYPE;

/* 0x5D4
    13:0    R/W reg_ram_dsp2_fft_lv                             14'b00011000001000
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DSP2_FFT_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp2_fft_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DSP2_FFT_LV_TYPE;

/* 0x5D6
    13:0    R/W reg_ram_dsp_sys_hv                              14'b00001000010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DSP_SYS_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp_sys_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DSP_SYS_HV_TYPE;

/* 0x5D8
    13:0    R/W reg_ram_dsp_sys_lv                              14'b00001100110001
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DSP_SYS_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp_sys_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DSP_SYS_LV_TYPE;

/* 0x5DA
    13:0    R/W reg_ram_vadbuf_hv                               14'b00001000010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_VADBUF_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_vadbuf_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_VADBUF_HV_TYPE;

/* 0x5DC
    13:0    R/W reg_ram_vadbuf_lv                               14'b00001100110001
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_VADBUF_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_vadbuf_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_VADBUF_LV_TYPE;

/* 0x5DE
    13:0    R/W reg_ram_ancdsp_hv                               14'b00001000010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_ANCDSP_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_ancdsp_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_ANCDSP_HV_TYPE;

/* 0x5E0
    13:0    R/W reg_ram_ancdsp_lv                               14'b00001100110001
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_ANCDSP_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_ancdsp_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_ANCDSP_LV_TYPE;

/* 0x5E2
    13:0    R/W reg_ram_rfc_hv                                  14'b00001000010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_RFC_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_rfc_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_RFC_HV_TYPE;

/* 0x5E4
    13:0    R/W reg_ram_rfc_lv                                  14'b00001100110001
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_RFC_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_rfc_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_RFC_LV_TYPE;

/* 0x5E6
    13:0    R/W reg_ram_rfc_rf_hv                               14'b00001100010011
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_RFC_RF_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_rfc_rf_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_RFC_RF_HV_TYPE;

/* 0x5E8
    13:0    R/W reg_ram_rfc_rf_lv                               14'b00001110110001
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_RFC_RF_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_rfc_rf_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_RFC_RF_LV_TYPE;

/* 0x5EA
    0       R/W reg_sport0_bus_sel                              1'b0
    1       R/W reg_sport1_bus_sel                              1'b0
    2       R/W reg_sport2_bus_sel                              1'b0
    3       R/W reg_sport3_bus_sel                              1'b0
    4       R/W reg_vadbuf_bus_sel                              1'b0
    5       R/W reg_spdif_bus_sel                               1'b0
    7:6     R/W reg_bus_sel_dummy                               2'b0
    15:8    R   reserved                                        8'b0
 */
typedef union _AON_FAST_REG_0x5EA_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_sport0_bus_sel: 1;
        uint16_t reg_sport1_bus_sel: 1;
        uint16_t reg_sport2_bus_sel: 1;
        uint16_t reg_sport3_bus_sel: 1;
        uint16_t reg_vadbuf_bus_sel: 1;
        uint16_t reg_spdif_bus_sel: 1;
        uint16_t reg_bus_sel_dummy: 2;
        uint16_t reserved: 8;
    };
} AON_FAST_REG_0x5EA_TYPE;

/* 0x5EC
    3:0     R/W r_DS_data_iso0                                  4'h0
    7:4     R/W r_DS_cache_d_iso0                               4'h0
    11:8    R/W r_DS_cache_tag_iso0                             4'h0
    12      R/W r_DS_cache_ws_iso0                              1'h0
    13      R/W r_DS_usb_iso0                                   1'h0
    14      R/W SRAM_DS_ISO0_1_DUMMY2                           1'h0
    15      R/W SRAM_DS_ISO0_1_DUMMY1                           1'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_data_iso0: 4;
        uint16_t r_DS_cache_d_iso0: 4;
        uint16_t r_DS_cache_tag_iso0: 4;
        uint16_t r_DS_cache_ws_iso0: 1;
        uint16_t r_DS_usb_iso0: 1;
        uint16_t SRAM_DS_ISO0_1_DUMMY2: 1;
        uint16_t SRAM_DS_ISO0_1_DUMMY1: 1;
    };
} AON_FAST_SRAM_DS_ISO0_1_TYPE;

/* 0x5EE
    7:0     R/W r_DS_com01_iso0                                 8'h0
    11:8    R/W r_DS_buf_iso0                                   4'h0
    12      R/W r_DS_master_cam_iso0                            1'h0
    13      R/W r_DS_lut_cam_iso0                               1'h0
    14      R/W r_DS_slave_cam_iso0                             1'h0
    15      R/W r_DS_pa_cam_iso0                                1'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_com01_iso0: 8;
        uint16_t r_DS_buf_iso0: 4;
        uint16_t r_DS_master_cam_iso0: 1;
        uint16_t r_DS_lut_cam_iso0: 1;
        uint16_t r_DS_slave_cam_iso0: 1;
        uint16_t r_DS_pa_cam_iso0: 1;
    };
} AON_FAST_SRAM_DS_ISO0_2_TYPE;

/* 0x5F0
    15:0    R/W r_DS_dsp1_ram_iso0[15:0]                        16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp1_ram_iso0_15_0;
    };
} AON_FAST_SRAM_DS_ISO0_3_TYPE;

/* 0x5F2
    8:0     R/W r_DS_dsp1_ram_iso0[24:16]                       9'h0
    11:9    R/W r_DS_dsp1_icache_iso0                           3'h0
    14:12   R/W r_DS_dsp1_dcache_iso0                           3'h0
    15      R/W SRAM_DS_ISO0_4_DUMMY1                           1'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp1_ram_iso0_24_16: 9;
        uint16_t r_DS_dsp1_icache_iso0: 3;
        uint16_t r_DS_dsp1_dcache_iso0: 3;
        uint16_t SRAM_DS_ISO0_4_DUMMY1: 1;
    };
} AON_FAST_SRAM_DS_ISO0_4_TYPE;

/* 0x5F4
    2:0     R/W r_DS_dsp1_itag_iso0                             3'h0
    5:3     R/W r_DS_dsp1_dtag_iso0                             3'h0
    6       R/W r_DS_dsp1_prefetch_iso0                         1'h0
    8:7     R/W r_DS_dsp2_icache_iso0                           2'h0
    9       R/W r_DS_dsp2_itag_iso0                             1'h0
    13:10   R/W r_DS_dsp2_dcache_iso0                           4'h0
    14      R/W r_DS_dsp2_prefetch_iso0                         1'h0
    15      R/W reserved                                        1'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp1_itag_iso0: 3;
        uint16_t r_DS_dsp1_dtag_iso0: 3;
        uint16_t r_DS_dsp1_prefetch_iso0: 1;
        uint16_t r_DS_dsp2_icache_iso0: 2;
        uint16_t r_DS_dsp2_itag_iso0: 1;
        uint16_t r_DS_dsp2_dcache_iso0: 4;
        uint16_t r_DS_dsp2_prefetch_iso0: 1;
        uint16_t reserved: 1;
    };
} AON_FAST_SRAM_DS_ISO0_5_TYPE;

/* 0x5F6
    11:0    R/W r_DS_dsp2_ram_iso0                              12'h0
    15:12   R/W r_DS_dsp2_dtag_iso0                             4'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp2_ram_iso0: 12;
        uint16_t r_DS_dsp2_dtag_iso0: 4;
    };
} AON_FAST_SRAM_DS_ISO0_6_TYPE;

/* 0x5F8
    15:0    R/W r_DS_dsp_sys_iso0[15:0]                         1'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp_sys_iso0_15_0;
    };
} AON_FAST_SRAM_DS_ISO0_7_TYPE;

/* 0x5FA
    15:0    R/W r_DS_dsp_sys_iso0[31:16]                        1'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp_sys_iso0_31_16;
    };
} AON_FAST_SRAM_DS_ISO0_8_TYPE;

/* 0x5FC
    3:0     R/W r_DS_dsp_sys_iso0[35:32]                        4'h0
    11:4    R/W r_DS_dsp_sys_16k_iso0                           8'h0
    12      R/W r_DS_dsp2_fft_iso0                              1'h0
    13      R/W r_DS_rfc_iso0                                   1'h0
    14      R/W r_DS_adpll_iso0                                 1'h0
    15      R/W SRAM_DS_ISO0_9_DUMMY1                           1'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_9_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp_sys_iso0_35_32: 4;
        uint16_t r_DS_dsp_sys_16k_iso0: 8;
        uint16_t r_DS_dsp2_fft_iso0: 1;
        uint16_t r_DS_rfc_iso0: 1;
        uint16_t r_DS_adpll_iso0: 1;
        uint16_t SRAM_DS_ISO0_9_DUMMY1: 1;
    };
} AON_FAST_SRAM_DS_ISO0_9_TYPE;

/* 0x5FE
    3:0     R/W r_DS_data_iso1                                  4'h0
    7:4     R/W r_DS_cache_d_iso1                               4'h0
    11:8    R/W r_DS_cache_tag_iso1                             4'h0
    12      R/W r_DS_cache_ws_iso1                              1'h0
    13      R/W r_DS_usb_iso1                                   1'h0
    14      R/W SRAM_DS_ISO1_1_DUMMY2                           1'h0
    15      R/W SRAM_DS_ISO1_1_DUMMY1                           1'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_data_iso1: 4;
        uint16_t r_DS_cache_d_iso1: 4;
        uint16_t r_DS_cache_tag_iso1: 4;
        uint16_t r_DS_cache_ws_iso1: 1;
        uint16_t r_DS_usb_iso1: 1;
        uint16_t SRAM_DS_ISO1_1_DUMMY2: 1;
        uint16_t SRAM_DS_ISO1_1_DUMMY1: 1;
    };
} AON_FAST_SRAM_DS_ISO1_1_TYPE;

/* 0x600
    7:0     R/W r_DS_com01_iso1                                 8'h0
    11:8    R/W r_DS_buf_iso1                                   4'h0
    12      R/W r_DS_master_cam_iso1                            1'h0
    13      R/W r_DS_lut_cam_iso1                               1'h0
    14      R/W r_DS_slave_cam_iso1                             1'h0
    15      R/W r_DS_pa_cam_iso1                                1'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_com01_iso1: 8;
        uint16_t r_DS_buf_iso1: 4;
        uint16_t r_DS_master_cam_iso1: 1;
        uint16_t r_DS_lut_cam_iso1: 1;
        uint16_t r_DS_slave_cam_iso1: 1;
        uint16_t r_DS_pa_cam_iso1: 1;
    };
} AON_FAST_SRAM_DS_ISO1_2_TYPE;

/* 0x602
    15:0    R/W r_DS_dsp1_ram_iso1[15:0]                        16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp1_ram_iso1_15_0;
    };
} AON_FAST_SRAM_DS_ISO1_3_TYPE;

/* 0x604
    8:0     R/W r_DS_dsp1_ram_iso1[24:16]                       9'h0
    11:9    R/W r_DS_dsp1_icache_iso1                           3'h0
    14:12   R/W r_DS_dsp1_dcache_iso1                           3'h0
    15      R/W SRAM_DS_ISO1_4_DUMMY1                           1'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp1_ram_iso1_24_16: 9;
        uint16_t r_DS_dsp1_icache_iso1: 3;
        uint16_t r_DS_dsp1_dcache_iso1: 3;
        uint16_t SRAM_DS_ISO1_4_DUMMY1: 1;
    };
} AON_FAST_SRAM_DS_ISO1_4_TYPE;

/* 0x606
    2:0     R/W r_DS_dsp1_itag_iso1                             3'h0
    5:3     R/W r_DS_dsp1_dtag_iso1                             3'h0
    6       R/W r_DS_dsp1_prefetch_iso1                         1'h0
    8:7     R/W r_DS_dsp2_icache_iso1                           2'h0
    9       R/W r_DS_dsp2_itag_iso1                             1'h0
    13:10   R/W r_DS_dsp2_dcache_iso1                           4'h0
    14      R/W r_DS_dsp2_prefetch_iso1                         1'h0
    15      R/W reserved                                        1'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp1_itag_iso1: 3;
        uint16_t r_DS_dsp1_dtag_iso1: 3;
        uint16_t r_DS_dsp1_prefetch_iso1: 1;
        uint16_t r_DS_dsp2_icache_iso1: 2;
        uint16_t r_DS_dsp2_itag_iso1: 1;
        uint16_t r_DS_dsp2_dcache_iso1: 4;
        uint16_t r_DS_dsp2_prefetch_iso1: 1;
        uint16_t reserved: 1;
    };
} AON_FAST_SRAM_DS_ISO1_5_TYPE;

/* 0x608
    11:0    R/W r_DS_dsp2_ram_iso1                              12'h0
    15:12   R/W r_DS_dsp2_dtag_iso1                             4'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp2_ram_iso1: 12;
        uint16_t r_DS_dsp2_dtag_iso1: 4;
    };
} AON_FAST_SRAM_DS_ISO1_6_TYPE;

/* 0x60A
    15:0    R/W r_DS_dsp_sys_iso1[15:0]                         1'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp_sys_iso1_15_0;
    };
} AON_FAST_SRAM_DS_ISO1_7_TYPE;

/* 0x60C
    15:0    R/W r_DS_dsp_sys_iso1[31:16]                        1'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp_sys_iso1_31_16;
    };
} AON_FAST_SRAM_DS_ISO1_8_TYPE;

/* 0x60E
    3:0     R/W r_DS_dsp_sys_iso1[35:32]                        4'h0
    11:4    R/W r_DS_dsp_sys_16k_iso1                           8'h0
    12      R/W r_DS_dsp2_fft_iso1                              1'h0
    13      R/W r_DS_rfc_iso1                                   1'h0
    14      R/W r_DS_adpll_iso1                                 1'h0
    15      R/W SRAM_DS_ISO1_9_DUMMY1                           1'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_9_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp_sys_iso1_35_32: 4;
        uint16_t r_DS_dsp_sys_16k_iso1: 8;
        uint16_t r_DS_dsp2_fft_iso1: 1;
        uint16_t r_DS_rfc_iso1: 1;
        uint16_t r_DS_adpll_iso1: 1;
        uint16_t SRAM_DS_ISO1_9_DUMMY1: 1;
    };
} AON_FAST_SRAM_DS_ISO1_9_TYPE;

/* 0x610
    3:0     R/W r_SD_data_iso0                                  4'h0
    7:4     R/W r_SD_cache_d_iso0                               4'h0
    11:8    R/W r_SD_cache_tag_iso0                             4'h0
    12      R/W r_SD_cache_ws_iso0                              1'h0
    13      R/W r_SD_usb_iso0                                   1'h0
    14      R/W SRAM_SD_ISO0_1_DUMMY2                           1'h0
    15      R/W SRAM_SD_ISO0_1_DUMMY1                           1'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_data_iso0: 4;
        uint16_t r_SD_cache_d_iso0: 4;
        uint16_t r_SD_cache_tag_iso0: 4;
        uint16_t r_SD_cache_ws_iso0: 1;
        uint16_t r_SD_usb_iso0: 1;
        uint16_t SRAM_SD_ISO0_1_DUMMY2: 1;
        uint16_t SRAM_SD_ISO0_1_DUMMY1: 1;
    };
} AON_FAST_SRAM_SD_ISO0_1_TYPE;

/* 0x612
    7:0     R/W r_SD_com01_iso0                                 8'h0
    11:8    R/W r_SD_buf_iso0                                   4'h0
    12      R/W r_SD_master_cam_iso0                            1'h0
    13      R/W r_SD_lut_cam_iso0                               1'h0
    14      R/W r_SD_slave_cam_iso0                             1'h0
    15      R/W r_SD_pa_cam_iso0                                1'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_com01_iso0: 8;
        uint16_t r_SD_buf_iso0: 4;
        uint16_t r_SD_master_cam_iso0: 1;
        uint16_t r_SD_lut_cam_iso0: 1;
        uint16_t r_SD_slave_cam_iso0: 1;
        uint16_t r_SD_pa_cam_iso0: 1;
    };
} AON_FAST_SRAM_SD_ISO0_2_TYPE;

/* 0x614
    15:0    R/W r_SD_dsp1_ram_iso0[15:0]                        16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp1_ram_iso0_15_0;
    };
} AON_FAST_SRAM_SD_ISO0_3_TYPE;

/* 0x616
    8:0     R/W r_SD_dsp1_ram_iso0[24:16]                       9'h0
    11:9    R/W r_SD_dsp1_icache_iso0                           3'h0
    14:12   R/W r_SD_dsp1_dcache_iso0                           3'h0
    15      R/W SRAM_SD_ISO0_4_DUMMY1                           1'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp1_ram_iso0_24_16: 9;
        uint16_t r_SD_dsp1_icache_iso0: 3;
        uint16_t r_SD_dsp1_dcache_iso0: 3;
        uint16_t SRAM_SD_ISO0_4_DUMMY1: 1;
    };
} AON_FAST_SRAM_SD_ISO0_4_TYPE;

/* 0x618
    2:0     R/W r_SD_dsp1_itag_iso0                             3'h0
    5:3     R/W r_SD_dsp1_dtag_iso0                             3'h0
    6       R/W r_SD_dsp1_prefetch_iso0                         1'h0
    8:7     R/W r_SD_dsp2_icache_iso0                           2'h0
    9       R/W r_SD_dsp2_itag_iso0                             1'h0
    13:10   R/W r_SD_dsp2_dcache_iso0                           4'h0
    14      R/W r_SD_dsp2_prefetch_iso0                         1'h0
    15      R/W reserved                                        1'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp1_itag_iso0: 3;
        uint16_t r_SD_dsp1_dtag_iso0: 3;
        uint16_t r_SD_dsp1_prefetch_iso0: 1;
        uint16_t r_SD_dsp2_icache_iso0: 2;
        uint16_t r_SD_dsp2_itag_iso0: 1;
        uint16_t r_SD_dsp2_dcache_iso0: 4;
        uint16_t r_SD_dsp2_prefetch_iso0: 1;
        uint16_t reserved: 1;
    };
} AON_FAST_SRAM_SD_ISO0_5_TYPE;

/* 0x61A
    11:0    R/W r_SD_dsp2_ram_iso0                              12'h0
    15:12   R/W r_SD_dsp2_dtag_iso0                             4'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp2_ram_iso0: 12;
        uint16_t r_SD_dsp2_dtag_iso0: 4;
    };
} AON_FAST_SRAM_SD_ISO0_6_TYPE;

/* 0x61C
    15:0    R/W r_SD_dsp_sys_iso0[15:0]                         1'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp_sys_iso0_15_0;
    };
} AON_FAST_SRAM_SD_ISO0_7_TYPE;

/* 0x61E
    15:0    R/W r_SD_dsp_sys_iso0[31:16]                        1'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp_sys_iso0_31_16;
    };
} AON_FAST_SRAM_SD_ISO0_8_TYPE;

/* 0x620
    3:0     R/W r_SD_dsp_sys_iso0[35:32]                        4'h0
    11:4    R/W r_SD_dsp_sys_16k_iso0                           8'h0
    12      R/W r_SD_dsp2_fft_iso0                              1'h0
    13      R/W r_SD_rfc_iso0                                   1'h0
    14      R/W r_SD_adpll_iso0                                 1'h0
    15      R/W SRAM_SD_ISO0_9_DUMMY1                           1'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_9_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp_sys_iso0_35_32: 4;
        uint16_t r_SD_dsp_sys_16k_iso0: 8;
        uint16_t r_SD_dsp2_fft_iso0: 1;
        uint16_t r_SD_rfc_iso0: 1;
        uint16_t r_SD_adpll_iso0: 1;
        uint16_t SRAM_SD_ISO0_9_DUMMY1: 1;
    };
} AON_FAST_SRAM_SD_ISO0_9_TYPE;

/* 0x622
    3:0     R/W r_SD_data_iso1                                  4'h0
    7:4     R/W r_SD_cache_d_iso1                               4'h0
    11:8    R/W r_SD_cache_tag_iso1                             4'h0
    12      R/W r_SD_cache_ws_iso1                              1'h0
    13      R/W r_SD_usb_iso1                                   1'h0
    14      R/W SRAM_SD_ISO1_1_DUMMY2                           1'h0
    15      R/W SRAM_SD_ISO1_1_DUMMY1                           1'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_data_iso1: 4;
        uint16_t r_SD_cache_d_iso1: 4;
        uint16_t r_SD_cache_tag_iso1: 4;
        uint16_t r_SD_cache_ws_iso1: 1;
        uint16_t r_SD_usb_iso1: 1;
        uint16_t SRAM_SD_ISO1_1_DUMMY2: 1;
        uint16_t SRAM_SD_ISO1_1_DUMMY1: 1;
    };
} AON_FAST_SRAM_SD_ISO1_1_TYPE;

/* 0x624
    7:0     R/W r_SD_com01_iso1                                 8'h0
    11:8    R/W r_SD_buf_iso1                                   4'h0
    12      R/W r_SD_master_cam_iso1                            1'h0
    13      R/W r_SD_lut_cam_iso1                               1'h0
    14      R/W r_SD_slave_cam_iso1                             1'h0
    15      R/W r_SD_pa_cam_iso1                                1'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_com01_iso1: 8;
        uint16_t r_SD_buf_iso1: 4;
        uint16_t r_SD_master_cam_iso1: 1;
        uint16_t r_SD_lut_cam_iso1: 1;
        uint16_t r_SD_slave_cam_iso1: 1;
        uint16_t r_SD_pa_cam_iso1: 1;
    };
} AON_FAST_SRAM_SD_ISO1_2_TYPE;

/* 0x626
    15:0    R/W r_SD_dsp1_ram_iso1[15:0]                        16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp1_ram_iso1_15_0;
    };
} AON_FAST_SRAM_SD_ISO1_3_TYPE;

/* 0x628
    8:0     R/W r_SD_dsp1_ram_iso1[24:16]                       9'h0
    11:9    R/W r_SD_dsp1_icache_iso1                           3'h0
    14:12   R/W r_SD_dsp1_dcache_iso1                           3'h0
    15      R/W SRAM_SD_ISO1_4_DUMMY1                           1'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp1_ram_iso1_24_16: 9;
        uint16_t r_SD_dsp1_icache_iso1: 3;
        uint16_t r_SD_dsp1_dcache_iso1: 3;
        uint16_t SRAM_SD_ISO1_4_DUMMY1: 1;
    };
} AON_FAST_SRAM_SD_ISO1_4_TYPE;

/* 0x62A
    2:0     R/W r_SD_dsp1_itag_iso1                             3'h0
    5:3     R/W r_SD_dsp1_dtag_iso1                             3'h0
    6       R/W r_SD_dsp1_prefetch_iso1                         1'h0
    8:7     R/W r_SD_dsp2_icache_iso1                           2'h0
    9       R/W r_SD_dsp2_itag_iso1                             1'h0
    13:10   R/W r_SD_dsp2_dcache_iso1                           4'h0
    14      R/W r_SD_dsp2_prefetch_iso1                         1'h0
    15      R/W reserved                                        1'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp1_itag_iso1: 3;
        uint16_t r_SD_dsp1_dtag_iso1: 3;
        uint16_t r_SD_dsp1_prefetch_iso1: 1;
        uint16_t r_SD_dsp2_icache_iso1: 2;
        uint16_t r_SD_dsp2_itag_iso1: 1;
        uint16_t r_SD_dsp2_dcache_iso1: 4;
        uint16_t r_SD_dsp2_prefetch_iso1: 1;
        uint16_t reserved: 1;
    };
} AON_FAST_SRAM_SD_ISO1_5_TYPE;

/* 0x62C
    11:0    R/W r_SD_dsp2_ram_iso1                              12'h0
    15:12   R/W r_SD_dsp2_dtag_iso1                             4'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp2_ram_iso1: 12;
        uint16_t r_SD_dsp2_dtag_iso1: 4;
    };
} AON_FAST_SRAM_SD_ISO1_6_TYPE;

/* 0x62E
    15:0    R/W r_SD_dsp_sys_iso1[15:0]                         1'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp_sys_iso1_15_0;
    };
} AON_FAST_SRAM_SD_ISO1_7_TYPE;

/* 0x630
    15:0    R/W r_SD_dsp_sys_iso1[31:16]                        1'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp_sys_iso1_31_16;
    };
} AON_FAST_SRAM_SD_ISO1_8_TYPE;

/* 0x632
    3:0     R/W r_SD_dsp_sys_iso1[35:32]                        4'h0
    11:4    R/W r_SD_dsp_sys_16k_iso1                           8'h0
    12      R/W r_SD_dsp2_fft_iso1                              1'h0
    13      R/W r_SD_rfc_iso1                                   1'h0
    14      R/W r_SD_adpll_iso1                                 1'h0
    15      R/W SRAM_SD_ISO1_9_DUMMY1                           1'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_9_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp_sys_iso1_35_32: 4;
        uint16_t r_SD_dsp_sys_16k_iso1: 8;
        uint16_t r_SD_dsp2_fft_iso1: 1;
        uint16_t r_SD_rfc_iso1: 1;
        uint16_t r_SD_adpll_iso1: 1;
        uint16_t SRAM_SD_ISO1_9_DUMMY1: 1;
    };
} AON_FAST_SRAM_SD_ISO1_9_TYPE;

/* 0x634
    0       R/W r_SD_rsa                                        1'h0
    1       R/W r_DS_rsa                                        1'h0
    2       R/W r_SD_mac_fifo                                   1'h0
    3       R/W r_DS_mac_fifo                                   1'h0
    4       R/W r_SD_cis_cam                                    1'h0
    5       R/W r_DS_cis_cam                                    1'h0
    6       R/W r_SD_phy_ram                                    1'h0
    7       R/W r_DS_phy_ram                                    1'h0
    8       R/W r_SD_dsp1_fft                                   1'h0
    9       R/W r_DS_dsp1_fft                                   1'h0
    11:10   R   reserved                                        2'h0
    12      R/W r_SD_sdh                                        1'h0
    13      R/W r_DS_sdh                                        1'h0
    14      R/W r_SD_bis_cam                                    1'h0
    15      R/W r_DS_bis_cam                                    1'h0
 */
typedef union _AON_FAST_SRAM_DSSD_VCORE_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_rsa: 1;
        uint16_t r_DS_rsa: 1;
        uint16_t r_SD_mac_fifo: 1;
        uint16_t r_DS_mac_fifo: 1;
        uint16_t r_SD_cis_cam: 1;
        uint16_t r_DS_cis_cam: 1;
        uint16_t r_SD_phy_ram: 1;
        uint16_t r_DS_phy_ram: 1;
        uint16_t r_SD_dsp1_fft: 1;
        uint16_t r_DS_dsp1_fft: 1;
        uint16_t reserved: 2;
        uint16_t r_SD_sdh: 1;
        uint16_t r_DS_sdh: 1;
        uint16_t r_SD_bis_cam: 1;
        uint16_t r_DS_bis_cam: 1;
    };
} AON_FAST_SRAM_DSSD_VCORE_1_TYPE;

/* 0x636
    0       R/W r_LS_mcu_rom                                    1'h0
    1       R/W r_LS_dsp1_rom                                   1'h0
    2       R/W r_LS_dsp2_rom                                   1'h0
    3       R/W r_LS_rfc_rom                                    1'h0
    4       R/W r_LS_ancdsp_rom                                 1'h0
    7:5     R/W SRAM_DSSD_VCORE_2_DUMMY2                        3'h0
    15:8    R   reserved                                        8'h0
 */
typedef union _AON_FAST_SRAM_DSSD_VCORE_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_LS_mcu_rom: 1;
        uint16_t r_LS_dsp1_rom: 1;
        uint16_t r_LS_dsp2_rom: 1;
        uint16_t r_LS_rfc_rom: 1;
        uint16_t r_LS_ancdsp_rom: 1;
        uint16_t SRAM_DSSD_VCORE_2_DUMMY2: 3;
        uint16_t reserved: 8;
    };
} AON_FAST_SRAM_DSSD_VCORE_2_TYPE;

/* 0x638
    15:0    R/W reg_otp_ctrl_w4_loader_0                        16'd1
 */
typedef union _AON_FAST_AON_LOADER_TIMER0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_otp_ctrl_w4_loader_0;
    };
} AON_FAST_AON_LOADER_TIMER0_TYPE;

/* 0x63A
    15:0    R/W reg_otp_ctrl_w4_loader_1                        16'd2
 */
typedef union _AON_FAST_AON_LOADER_TIMER1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_otp_ctrl_w4_loader_1;
    };
} AON_FAST_AON_LOADER_TIMER1_TYPE;

/* 0x63C
    15:0    R/W reg_otp_ctrl_w5_loader_0                        16'd1
 */
typedef union _AON_FAST_AON_LOADER_TIMER2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_otp_ctrl_w5_loader_0;
    };
} AON_FAST_AON_LOADER_TIMER2_TYPE;

/* 0x63E
    15:0    R/W reg_otp_ctrl_w5_loader_1                        16'd1
 */
typedef union _AON_FAST_AON_LOADER_TIMER3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_otp_ctrl_w5_loader_1;
    };
} AON_FAST_AON_LOADER_TIMER3_TYPE;

/* 0x640
    15:0    R/W reg_otp_ctrl_w9_loader_0                        16'd1
 */
typedef union _AON_FAST_AON_LOADER_TIMER4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_otp_ctrl_w9_loader_0;
    };
} AON_FAST_AON_LOADER_TIMER4_TYPE;

/* 0x642
    15:0    R/W reg_otp_ctrl_w9_loader_1                        16'd17
 */
typedef union _AON_FAST_AON_LOADER_TIMER5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_otp_ctrl_w9_loader_1;
    };
} AON_FAST_AON_LOADER_TIMER5_TYPE;

/* 0x644
    0       R/W r_ram_ret1n                                     1'h1
    1       R/W r_bist_mem_speed_mode                           1'h0
    10:2    R   reserved                                        9'h0
    11      R/W mem_dsp1_vsel_sync_en                           1'h1
    12      R/W mem_dsp2_vsel_sync_en                           1'h1
    13      R/W ram_nna_vsel_sync_en                            1'h1
    14      R/W mem_ancdsp_sys_vsel_sync_en                     1'h1
    15      R/W ram_dsp_sys_vsel_sync_en                        1'h1
 */
typedef union _AON_FAST_MEM_VSEL_SYNC_EN_MISC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_ram_ret1n: 1;
        uint16_t r_bist_mem_speed_mode: 1;
        uint16_t reserved: 9;
        uint16_t mem_dsp1_vsel_sync_en: 1;
        uint16_t mem_dsp2_vsel_sync_en: 1;
        uint16_t ram_nna_vsel_sync_en: 1;
        uint16_t mem_ancdsp_sys_vsel_sync_en: 1;
        uint16_t ram_dsp_sys_vsel_sync_en: 1;
    };
} AON_FAST_MEM_VSEL_SYNC_EN_MISC_TYPE;

/* 0x646
    5:0     R/W reg_rom_ancdsp_hv                               6'b010011
    11:6    R/W reg_rom_ancdsp_lv                               6'b010010
    15:12   R   reserved                                        0
 */
typedef union _AON_FAST_REG_ROM_ANCDSP_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_rom_ancdsp_hv: 6;
        uint16_t reg_rom_ancdsp_lv: 6;
        uint16_t reserved: 4;
    };
} AON_FAST_REG_ROM_ANCDSP_TYPE;

/* 0x648
    13:0    R/W reg_ram_cache_rf                                14'b00001100010011
    15:14   R   reserved                                        2'h0
 */
typedef union _AON_FAST_REG_RAM_CACHE_RF_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_cache_rf: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_CACHE_RF_TYPE;

/* 0x64A
    13:0    R/W reg_ram_dsp2_cache_rf_hv                        14'b00000110101000
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DSP2_CACHE_RF_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp2_cache_rf_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DSP2_CACHE_RF_HV_TYPE;

/* 0x64C
    13:0    R/W reg_ram_dsp2_cache_rf_lv                        14'b00011000001000
    15:14   R   reserved                                        2'b0
 */
typedef union _AON_FAST_REG_RAM_DSP2_CACHE_RF_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_dsp2_cache_rf_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_DSP2_CACHE_RF_LV_TYPE;

/* 0x64E
    13:0    R/W reg_ram_nna_hv                                  14'b00000110101000
    15:14   R   reserved                                        2'h0
 */
typedef union _AON_FAST_REG_RAM_NNA_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_nna_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_NNA_HV_TYPE;

/* 0x650
    13:0    R/W reg_ram_nna_lv                                  14'b00010100001100
    15:14   R   reserved                                        2'h0
 */
typedef union _AON_FAST_REG_RAM_NNA_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_nna_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_NNA_LV_TYPE;

/* 0x652
    13:0    R/W reg_ram_nna_rf_hv                               14'b00000110101000
    15:14   R   reserved                                        2'h0
 */
typedef union _AON_FAST_REG_RAM_NNA_RF_HV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_nna_rf_hv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_NNA_RF_HV_TYPE;

/* 0x654
    13:0    R/W reg_ram_nna_rf_lv                               14'b00011000001000
    15:14   R   reserved                                        2'h0
 */
typedef union _AON_FAST_REG_RAM_NNA_RF_LV_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t reg_ram_nna_rf_lv: 14;
        uint16_t reserved: 2;
    };
} AON_FAST_REG_RAM_NNA_RF_LV_TYPE;

/* 0x656
    2:0     R/W r_DS_dsp_sys_iso0[38:36]                        3'h0
    7:3     R/W SRAM_DS_ISO0_10_DUMMY1                          5'h0
    15:8    R/W r_DS_ancdsp_iso0                                8'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_10_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp_sys_iso0_38_36: 3;
        uint16_t SRAM_DS_ISO0_10_DUMMY1: 5;
        uint16_t r_DS_ancdsp_iso0: 8;
    };
} AON_FAST_SRAM_DS_ISO0_10_TYPE;

/* 0x658
    2:0     R/W r_DS_dsp_sys_iso1[38:36]                        3'h0
    7:3     R/W SRAM_DS_ISO1_10_DUMMY1                          5'h0
    15:8    R/W r_DS_ancdsp_iso1                                8'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_10_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_dsp_sys_iso1_38_36: 3;
        uint16_t SRAM_DS_ISO1_10_DUMMY1: 5;
        uint16_t r_DS_ancdsp_iso1: 8;
    };
} AON_FAST_SRAM_DS_ISO1_10_TYPE;

/* 0x65A
    2:0     R/W r_SD_dsp_sys_iso0[38:36]                        3'h0
    7:3     R/W SRAM_SD_ISO0_10_DUMMY1                          5'h0
    15:8    R/W r_SD_ancdsp_iso0                                8'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_10_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp_sys_iso0_38_36: 3;
        uint16_t SRAM_SD_ISO0_10_DUMMY1: 5;
        uint16_t r_SD_ancdsp_iso0: 8;
    };
} AON_FAST_SRAM_SD_ISO0_10_TYPE;

/* 0x65C
    2:0     R/W r_SD_dsp_sys_iso1[38:36]                        3'h0
    7:3     R/W SRAM_SD_ISO1_10_DUMMY1                          5'h0
    15:8    R/W r_SD_ancdsp_iso1                                8'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_10_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_dsp_sys_iso1_38_36: 3;
        uint16_t SRAM_SD_ISO1_10_DUMMY1: 5;
        uint16_t r_SD_ancdsp_iso1: 8;
    };
} AON_FAST_SRAM_SD_ISO1_10_TYPE;

/* 0x65E
    15:0    R/W r_DS_nna_iso0[15:0]                             16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_11_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_nna_iso0_15_0;
    };
} AON_FAST_SRAM_DS_ISO0_11_TYPE;

/* 0x660
    15:0    R/W r_DS_nna_iso0[31:16]                            16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_12_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_nna_iso0_31_16;
    };
} AON_FAST_SRAM_DS_ISO0_12_TYPE;

/* 0x662
    15:0    R/W r_DS_nna_iso0[47:32]                            16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_13_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_nna_iso0_47_32;
    };
} AON_FAST_SRAM_DS_ISO0_13_TYPE;

/* 0x664
    15:0    R/W r_DS_nna_iso0[63:48]                            16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_14_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_nna_iso0_63_48;
    };
} AON_FAST_SRAM_DS_ISO0_14_TYPE;

/* 0x666
    12:0    R/W r_DS_nna_iso0[76:64]                            13'h0
    15:13   R/W SRAM_DS_ISO0_15_DUMMY1                          3'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO0_15_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_nna_iso0_76_64: 13;
        uint16_t SRAM_DS_ISO0_15_DUMMY1: 3;
    };
} AON_FAST_SRAM_DS_ISO0_15_TYPE;

/* 0x668
    15:0    R/W r_DS_nna_iso1[15:0]                             16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_11_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_nna_iso1_15_0;
    };
} AON_FAST_SRAM_DS_ISO1_11_TYPE;

/* 0x66A
    15:0    R/W r_DS_nna_iso1[31:16]                            16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_12_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_nna_iso1_31_16;
    };
} AON_FAST_SRAM_DS_ISO1_12_TYPE;

/* 0x66C
    15:0    R/W r_DS_nna_iso1[47:32]                            16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_13_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_nna_iso1_47_32;
    };
} AON_FAST_SRAM_DS_ISO1_13_TYPE;

/* 0x66E
    15:0    R/W r_DS_nna_iso1[63:48]                            16'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_14_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_nna_iso1_63_48;
    };
} AON_FAST_SRAM_DS_ISO1_14_TYPE;

/* 0x670
    12:0    R/W r_DS_nna_iso1[76:64]                            13'h0
    15:13   R/W SRAM_DS_ISO1_15_DUMMY1                          3'h0
 */
typedef union _AON_FAST_SRAM_DS_ISO1_15_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_DS_nna_iso1_76_64: 13;
        uint16_t SRAM_DS_ISO1_15_DUMMY1: 3;
    };
} AON_FAST_SRAM_DS_ISO1_15_TYPE;

/* 0x672
    15:0    R/W r_SD_nna_iso0[15:0]                             16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_11_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_nna_iso0_15_0;
    };
} AON_FAST_SRAM_SD_ISO0_11_TYPE;

/* 0x674
    15:0    R/W r_SD_nna_iso0[31:16]                            16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_12_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_nna_iso0_31_16;
    };
} AON_FAST_SRAM_SD_ISO0_12_TYPE;

/* 0x676
    15:0    R/W r_SD_nna_iso0[47:32]                            16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_13_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_nna_iso0_47_32;
    };
} AON_FAST_SRAM_SD_ISO0_13_TYPE;

/* 0x678
    15:0    R/W r_SD_nna_iso0[63:48]                            16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_14_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_nna_iso0_63_48;
    };
} AON_FAST_SRAM_SD_ISO0_14_TYPE;

/* 0x67A
    12:0    R/W r_SD_nna_iso0[76:64]                            13'h0
    15:13   R/W SRAM_SD_ISO0_15_DUMMY1                          3'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO0_15_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_nna_iso0_76_64: 13;
        uint16_t SRAM_SD_ISO0_15_DUMMY1: 3;
    };
} AON_FAST_SRAM_SD_ISO0_15_TYPE;

/* 0x67C
    15:0    R/W r_SD_nna_iso1[15:0]                             16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_11_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_nna_iso1_15_0;
    };
} AON_FAST_SRAM_SD_ISO1_11_TYPE;

/* 0x67E
    15:0    R/W r_SD_nna_iso1[31:16]                            16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_12_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_nna_iso1_31_16;
    };
} AON_FAST_SRAM_SD_ISO1_12_TYPE;

/* 0x680
    15:0    R/W r_SD_nna_iso1[47:32]                            16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_13_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_nna_iso1_47_32;
    };
} AON_FAST_SRAM_SD_ISO1_13_TYPE;

/* 0x682
    15:0    R/W r_SD_nna_iso1[63:48]                            16'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_14_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_nna_iso1_63_48;
    };
} AON_FAST_SRAM_SD_ISO1_14_TYPE;

/* 0x684
    12:0    R/W r_SD_nna_iso1[76:64]                            13'h0
    15:13   R/W SRAM_SD_ISO1_15_DUMMY1                          3'h0
 */
typedef union _AON_FAST_SRAM_SD_ISO1_15_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_SD_nna_iso1_76_64: 13;
        uint16_t SRAM_SD_ISO1_15_DUMMY1: 3;
    };
} AON_FAST_SRAM_SD_ISO1_15_TYPE;

/* 0x686
    3:0     R/W r_RM3_data_iso0                                 4'h0
    7:4     R/W r_RM3_cache_d_iso0                              4'h0
    11:8    R/W r_RM3_cache_tag_iso0                            4'h0
    12      R/W r_RM3_cache_ws_iso0                             1'h0
    13      R/W r_RM3_usb_iso0                                  1'h0
    14      R/W SRAM_RM3_ISO0_1_DUMMY2                          1'h0
    15      R/W SRAM_RM3_ISO0_1_DUMMY1                          1'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_data_iso0: 4;
        uint16_t r_RM3_cache_d_iso0: 4;
        uint16_t r_RM3_cache_tag_iso0: 4;
        uint16_t r_RM3_cache_ws_iso0: 1;
        uint16_t r_RM3_usb_iso0: 1;
        uint16_t SRAM_RM3_ISO0_1_DUMMY2: 1;
        uint16_t SRAM_RM3_ISO0_1_DUMMY1: 1;
    };
} AON_FAST_SRAM_RM3_ISO0_1_TYPE;

/* 0x688
    7:0     R/W r_RM3_com01_iso0                                8'h0
    11:8    R/W r_RM3_buf_iso0                                  4'h0
    12      R/W r_RM3_master_cam_iso0                           1'h0
    13      R/W r_RM3_lut_cam_iso0                              1'h0
    14      R/W r_RM3_slave_cam_iso0                            1'h0
    15      R/W r_RM3_pa_cam_iso0                               1'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_com01_iso0: 8;
        uint16_t r_RM3_buf_iso0: 4;
        uint16_t r_RM3_master_cam_iso0: 1;
        uint16_t r_RM3_lut_cam_iso0: 1;
        uint16_t r_RM3_slave_cam_iso0: 1;
        uint16_t r_RM3_pa_cam_iso0: 1;
    };
} AON_FAST_SRAM_RM3_ISO0_2_TYPE;

/* 0x68A
    15:0    R/W r_RM3_dsp1_ram_iso0[15:0]                       16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp1_ram_iso0_15_0;
    };
} AON_FAST_SRAM_RM3_ISO0_3_TYPE;

/* 0x68C
    8:0     R/W r_RM3_dsp1_ram_iso0[24:16]                      9'h0
    11:9    R/W r_RM3_dsp1_icache_iso0                          3'h0
    14:12   R/W r_RM3_dsp1_dcache_iso0                          3'h0
    15      R/W SRAM_RM3_ISO0_4_DUMMY1                          1'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp1_ram_iso0_24_16: 9;
        uint16_t r_RM3_dsp1_icache_iso0: 3;
        uint16_t r_RM3_dsp1_dcache_iso0: 3;
        uint16_t SRAM_RM3_ISO0_4_DUMMY1: 1;
    };
} AON_FAST_SRAM_RM3_ISO0_4_TYPE;

/* 0x68E
    2:0     R/W r_RM3_dsp1_itag_iso0                            3'h0
    5:3     R/W r_RM3_dsp1_dtag_iso0                            3'h0
    6       R/W r_RM3_dsp1_prefetch_iso0                        1'h0
    8:7     R/W r_RM3_dsp2_icache_iso0                          2'h0
    9       R/W r_RM3_dsp2_itag_iso0                            1'h0
    13:10   R/W r_RM3_dsp2_dcache_iso0                          4'h0
    14      R/W r_RM3_dsp2_prefetch_iso0                        1'h0
    15      R/W reserved                                        1'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp1_itag_iso0: 3;
        uint16_t r_RM3_dsp1_dtag_iso0: 3;
        uint16_t r_RM3_dsp1_prefetch_iso0: 1;
        uint16_t r_RM3_dsp2_icache_iso0: 2;
        uint16_t r_RM3_dsp2_itag_iso0: 1;
        uint16_t r_RM3_dsp2_dcache_iso0: 4;
        uint16_t r_RM3_dsp2_prefetch_iso0: 1;
        uint16_t reserved: 1;
    };
} AON_FAST_SRAM_RM3_ISO0_5_TYPE;

/* 0x690
    11:0    R/W r_RM3_dsp2_ram_iso0                             12'h0
    15:12   R/W r_RM3_dsp2_dtag_iso0                            4'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp2_ram_iso0: 12;
        uint16_t r_RM3_dsp2_dtag_iso0: 4;
    };
} AON_FAST_SRAM_RM3_ISO0_6_TYPE;

/* 0x692
    15:0    R/W r_RM3_dsp_sys_iso0[15:0]                        1'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp_sys_iso0_15_0;
    };
} AON_FAST_SRAM_RM3_ISO0_7_TYPE;

/* 0x694
    15:0    R/W r_RM3_dsp_sys_iso0[31:16]                       1'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp_sys_iso0_31_16;
    };
} AON_FAST_SRAM_RM3_ISO0_8_TYPE;

/* 0x696
    3:0     R/W r_RM3_dsp_sys_iso0[35:32]                       4'h0
    11:4    R/W r_RM3_dsp_sys_16k_iso0                          8'h0
    12      R/W r_RM3_dsp2_fft_iso0                             1'h0
    13      R/W r_RM3_rfc_iso0                                  1'h0
    14      R/W r_RM3_adpll_iso0                                1'h0
    15      R/W SRAM_RM3_ISO0_9_DUMMY1                          1'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_9_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp_sys_iso0_35_32: 4;
        uint16_t r_RM3_dsp_sys_16k_iso0: 8;
        uint16_t r_RM3_dsp2_fft_iso0: 1;
        uint16_t r_RM3_rfc_iso0: 1;
        uint16_t r_RM3_adpll_iso0: 1;
        uint16_t SRAM_RM3_ISO0_9_DUMMY1: 1;
    };
} AON_FAST_SRAM_RM3_ISO0_9_TYPE;

/* 0x698
    3:0     R/W r_RM3_data_iso1                                 4'h0
    7:4     R/W r_RM3_cache_d_iso1                              4'h0
    11:8    R/W r_RM3_cache_tag_iso1                            4'h0
    12      R/W r_RM3_cache_ws_iso1                             1'h0
    13      R/W r_RM3_usb_iso1                                  1'h0
    14      R/W SRAM_RM3_ISO1_1_DUMMY2                          1'h0
    15      R/W SRAM_RM3_ISO1_1_DUMMY1                          1'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_data_iso1: 4;
        uint16_t r_RM3_cache_d_iso1: 4;
        uint16_t r_RM3_cache_tag_iso1: 4;
        uint16_t r_RM3_cache_ws_iso1: 1;
        uint16_t r_RM3_usb_iso1: 1;
        uint16_t SRAM_RM3_ISO1_1_DUMMY2: 1;
        uint16_t SRAM_RM3_ISO1_1_DUMMY1: 1;
    };
} AON_FAST_SRAM_RM3_ISO1_1_TYPE;

/* 0x69A
    7:0     R/W r_RM3_com01_iso1                                8'h0
    11:8    R/W r_RM3_buf_iso1                                  4'h0
    12      R/W r_RM3_master_cam_iso1                           1'h0
    13      R/W r_RM3_lut_cam_iso1                              1'h0
    14      R/W r_RM3_slave_cam_iso1                            1'h0
    15      R/W r_RM3_pa_cam_iso1                               1'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_com01_iso1: 8;
        uint16_t r_RM3_buf_iso1: 4;
        uint16_t r_RM3_master_cam_iso1: 1;
        uint16_t r_RM3_lut_cam_iso1: 1;
        uint16_t r_RM3_slave_cam_iso1: 1;
        uint16_t r_RM3_pa_cam_iso1: 1;
    };
} AON_FAST_SRAM_RM3_ISO1_2_TYPE;

/* 0x69C
    15:0    R/W r_RM3_dsp1_ram_iso1[15:0]                       16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp1_ram_iso1_15_0;
    };
} AON_FAST_SRAM_RM3_ISO1_3_TYPE;

/* 0x69E
    8:0     R/W r_RM3_dsp1_ram_iso1[24:16]                      9'h0
    11:9    R/W r_RM3_dsp1_icache_iso1                          3'h0
    14:12   R/W r_RM3_dsp1_dcache_iso1                          3'h0
    15      R/W SRAM_RM3_ISO1_4_DUMMY1                          1'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp1_ram_iso1_24_16: 9;
        uint16_t r_RM3_dsp1_icache_iso1: 3;
        uint16_t r_RM3_dsp1_dcache_iso1: 3;
        uint16_t SRAM_RM3_ISO1_4_DUMMY1: 1;
    };
} AON_FAST_SRAM_RM3_ISO1_4_TYPE;

/* 0x6A0
    2:0     R/W r_RM3_dsp1_itag_iso1                            3'h0
    5:3     R/W r_RM3_dsp1_dtag_iso1                            3'h0
    6       R/W r_RM3_dsp1_prefetch_iso1                        1'h0
    8:7     R/W r_RM3_dsp2_icache_iso1                          2'h0
    9       R/W r_RM3_dsp2_itag_iso1                            1'h0
    13:10   R/W r_RM3_dsp2_dcache_iso1                          4'h0
    14      R/W r_RM3_dsp2_prefetch_iso1                        1'h0
    15      R/W reserved                                        1'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp1_itag_iso1: 3;
        uint16_t r_RM3_dsp1_dtag_iso1: 3;
        uint16_t r_RM3_dsp1_prefetch_iso1: 1;
        uint16_t r_RM3_dsp2_icache_iso1: 2;
        uint16_t r_RM3_dsp2_itag_iso1: 1;
        uint16_t r_RM3_dsp2_dcache_iso1: 4;
        uint16_t r_RM3_dsp2_prefetch_iso1: 1;
        uint16_t reserved: 1;
    };
} AON_FAST_SRAM_RM3_ISO1_5_TYPE;

/* 0x6A2
    11:0    R/W r_RM3_dsp2_ram_iso1                             12'h0
    15:12   R/W r_RM3_dsp2_dtag_iso1                            4'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp2_ram_iso1: 12;
        uint16_t r_RM3_dsp2_dtag_iso1: 4;
    };
} AON_FAST_SRAM_RM3_ISO1_6_TYPE;

/* 0x6A4
    15:0    R/W r_RM3_dsp_sys_iso1[15:0]                        1'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp_sys_iso1_15_0;
    };
} AON_FAST_SRAM_RM3_ISO1_7_TYPE;

/* 0x6A6
    15:0    R/W r_RM3_dsp_sys_iso1[31:16]                       1'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp_sys_iso1_31_16;
    };
} AON_FAST_SRAM_RM3_ISO1_8_TYPE;

/* 0x6A8
    3:0     R/W r_RM3_dsp_sys_iso1[35:32]                       4'h0
    11:4    R/W r_RM3_dsp_sys_16k_iso1                          8'h0
    12      R/W r_RM3_dsp2_fft_iso1                             1'h0
    13      R/W r_RM3_rfc_iso1                                  1'h0
    14      R/W r_RM3_adpll_iso1                                1'h0
    15      R/W SRAM_RM3_ISO1_9_DUMMY1                          1'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_9_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp_sys_iso1_35_32: 4;
        uint16_t r_RM3_dsp_sys_16k_iso1: 8;
        uint16_t r_RM3_dsp2_fft_iso1: 1;
        uint16_t r_RM3_rfc_iso1: 1;
        uint16_t r_RM3_adpll_iso1: 1;
        uint16_t SRAM_RM3_ISO1_9_DUMMY1: 1;
    };
} AON_FAST_SRAM_RM3_ISO1_9_TYPE;

/* 0x6AA
    2:0     R/W r_RM3_dsp_sys_iso0[38:36]                       3'h0
    7:3     R/W SRAM_RM3_ISO0_10_DUMMY1                         5'h0
    15:8    R/W r_RM3_ancdsp_iso0                               8'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_10_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp_sys_iso0_38_36: 3;
        uint16_t SRAM_RM3_ISO0_10_DUMMY1: 5;
        uint16_t r_RM3_ancdsp_iso0: 8;
    };
} AON_FAST_SRAM_RM3_ISO0_10_TYPE;

/* 0x6AC
    2:0     R/W r_RM3_dsp_sys_iso1[38:36]                       3'h0
    7:3     R/W SRAM_RM3_ISO1_10_DUMMY1                         5'h0
    15:8    R/W r_RM3_ancdsp_iso1                               8'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_10_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_dsp_sys_iso1_38_36: 3;
        uint16_t SRAM_RM3_ISO1_10_DUMMY1: 5;
        uint16_t r_RM3_ancdsp_iso1: 8;
    };
} AON_FAST_SRAM_RM3_ISO1_10_TYPE;

/* 0x6AE
    15:0    R/W r_RM3_nna_iso0[15:0]                            16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_11_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_nna_iso0_15_0;
    };
} AON_FAST_SRAM_RM3_ISO0_11_TYPE;

/* 0x6B0
    15:0    R/W r_RM3_nna_iso0[31:16]                           16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_12_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_nna_iso0_31_16;
    };
} AON_FAST_SRAM_RM3_ISO0_12_TYPE;

/* 0x6B2
    15:0    R/W r_RM3_nna_iso0[47:32]                           16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_13_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_nna_iso0_47_32;
    };
} AON_FAST_SRAM_RM3_ISO0_13_TYPE;

/* 0x6B4
    15:0    R/W r_RM3_nna_iso0[63:48]                           16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_14_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_nna_iso0_63_48;
    };
} AON_FAST_SRAM_RM3_ISO0_14_TYPE;

/* 0x6B6
    12:0    R/W r_RM3_nna_iso0[76:64]                           13'h0
    15:13   R/W SRAM_RM3_ISO0_15_DUMMY1                         3'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO0_15_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_nna_iso0_76_64: 13;
        uint16_t SRAM_RM3_ISO0_15_DUMMY1: 3;
    };
} AON_FAST_SRAM_RM3_ISO0_15_TYPE;

/* 0x6B8
    15:0    R/W r_RM3_nna_iso1[15:0]                            16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_11_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_nna_iso1_15_0;
    };
} AON_FAST_SRAM_RM3_ISO1_11_TYPE;

/* 0x6BA
    15:0    R/W r_RM3_nna_iso1[31:16]                           16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_12_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_nna_iso1_31_16;
    };
} AON_FAST_SRAM_RM3_ISO1_12_TYPE;

/* 0x6BC
    15:0    R/W r_RM3_nna_iso1[47:32]                           16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_13_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_nna_iso1_47_32;
    };
} AON_FAST_SRAM_RM3_ISO1_13_TYPE;

/* 0x6BE
    15:0    R/W r_RM3_nna_iso1[63:48]                           16'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_14_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_nna_iso1_63_48;
    };
} AON_FAST_SRAM_RM3_ISO1_14_TYPE;

/* 0x6C0
    12:0    R/W r_RM3_nna_iso1[76:64]                           13'h0
    15:13   R/W SRAM_RM3_ISO1_15_DUMMY1                         3'h0
 */
typedef union _AON_FAST_SRAM_RM3_ISO1_15_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_RM3_nna_iso1_76_64: 13;
        uint16_t SRAM_RM3_ISO1_15_DUMMY1: 3;
    };
} AON_FAST_SRAM_RM3_ISO1_15_TYPE;

/* 0x6F0
    1:0     R/W BTPLL_SYS_REG0X_DUMMY12                         2'b00
    2       R   RO_AON_ISO_PLL4                                 1'b1
    3       R   RO_AON_ISO_PLL3                                 1'b1
    4       R   RO_AON_ISO_PLL2                                 1'b1
    5       R   RO_AON_ISO_PLL1                                 1'b1
    6       R/W BTPLL_SYS_REG0X_DUMMY10                         1'b0
    7       R/W BTPLL_SYS_REG0X_DUMMY9                          1'b0
    8       R/W BTPLL_SYS_REG0X_DUMMY8                          1'b0
    9       R/W BTPLL_SYS_REG0X_DUMMY7                          1'b0
    10      R/W BTPLL_SYS_REG0X_DUMMY6                          1'b0
    11      R/W BTPLL_SYS_REG0X_DUMMY5                          1'b0
    12      R/W BTPLL_SYS_REG0X_DUMMY4                          1'b0
    13      R/W BTPLL_SYS_REG0X_DUMMY3                          1'b0
    14      R/W BTPLL_SYS_REG0X_DUMMY2                          1'b0
    15      R/W BTPLL_SYS_REG0X_DUMMY1                          1'b0
 */
typedef union _AON_FAST_REG0X_BTPLL_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL_SYS_REG0X_DUMMY12: 2;
        uint16_t RO_AON_ISO_PLL4: 1;
        uint16_t RO_AON_ISO_PLL3: 1;
        uint16_t RO_AON_ISO_PLL2: 1;
        uint16_t RO_AON_ISO_PLL1: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY10: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY9: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY8: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY7: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY6: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY5: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY4: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY3: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY2: 1;
        uint16_t BTPLL_SYS_REG0X_DUMMY1: 1;
    };
} AON_FAST_REG0X_BTPLL_SYS_TYPE;

/* 0x6F2
    7:0     R/W r_PLL_DIV1_SETTING                              8'h0
    15:8    R/W r_PLL_DIV0_SETTING                              8'h0
 */
typedef union _AON_FAST_REG1X_BTPLL_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_PLL_DIV1_SETTING: 8;
        uint16_t r_PLL_DIV0_SETTING: 8;
    };
} AON_FAST_REG1X_BTPLL_SYS_TYPE;

/* 0x6F4
    7:0     R/W r_PLL_DIV3_SETTING                              8'h0
    15:8    R/W r_PLL_DIV2_SETTING                              8'h0
 */
typedef union _AON_FAST_REG2X_BTPLL_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_PLL_DIV3_SETTING: 8;
        uint16_t r_PLL_DIV2_SETTING: 8;
    };
} AON_FAST_REG2X_BTPLL_SYS_TYPE;

/* 0x6F6
    4:0     R/W BTPLL_SYS_REG3X_DUMMY1                          5'h0
    7:5     R/W r_PLL3_DIV_SETTING                              3'h0
    15:8    R/W r_PLL_DIV4_SETTING                              8'h0
 */
typedef union _AON_FAST_REG3X_BTPLL_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL_SYS_REG3X_DUMMY1: 5;
        uint16_t r_PLL3_DIV_SETTING: 3;
        uint16_t r_PLL_DIV4_SETTING: 8;
    };
} AON_FAST_REG3X_BTPLL_SYS_TYPE;

/* 0x6F8
    13:0    R/W XTAL_SYS_REG0X_DUMMY1                           14'h0
    14      R   RO_AON_ISO_XTAL                                 1'b1
    15      R   RO_AON_XTAL_POW_XTAL                            1'b0
 */
typedef union _AON_FAST_REG0X_XTAL_OSC_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_SYS_REG0X_DUMMY1: 14;
        uint16_t RO_AON_ISO_XTAL: 1;
        uint16_t RO_AON_XTAL_POW_XTAL: 1;
    };
} AON_FAST_REG0X_XTAL_OSC_SYS_TYPE;

/* 0x6FA
    2:0     R/W RET_SYS_REG0X_DUMMY1                            3'h0
    3       R   RO_AON_RFC_STORE                                1'b0
    4       R   RO_AON_PF_STORE                                 1'b0
    5       R   RO_AON_MODEM_STORE                              1'b0
    6       R   RO_AON_DP_MODEM_STORE                           1'b0
    7       R   RO_AON_BZ_STORE                                 1'b0
    8       R   RO_AON_BLE_STORE                                1'b0
    9       R   RO_AON_RFC_RESTORE                              1'b0
    10      R   RO_AON_PF_RESTORE                               1'b0
    11      R   RO_AON_MODEM_RESTORE                            1'b0
    12      R   RO_AON_DP_MODEM_RESTORE                         1'b0
    13      R   RO_AON_BZ_RESTORE                               1'b0
    14      R   RO_AON_BLE_RESTORE                              1'b0
    15      R   RO_AON_BT_RET_RSTB                              1'b0
 */
typedef union _AON_FAST_REG0X_RET_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RET_SYS_REG0X_DUMMY1: 3;
        uint16_t RO_AON_RFC_STORE: 1;
        uint16_t RO_AON_PF_STORE: 1;
        uint16_t RO_AON_MODEM_STORE: 1;
        uint16_t RO_AON_DP_MODEM_STORE: 1;
        uint16_t RO_AON_BZ_STORE: 1;
        uint16_t RO_AON_BLE_STORE: 1;
        uint16_t RO_AON_RFC_RESTORE: 1;
        uint16_t RO_AON_PF_RESTORE: 1;
        uint16_t RO_AON_MODEM_RESTORE: 1;
        uint16_t RO_AON_DP_MODEM_RESTORE: 1;
        uint16_t RO_AON_BZ_RESTORE: 1;
        uint16_t RO_AON_BLE_RESTORE: 1;
        uint16_t RO_AON_BT_RET_RSTB: 1;
    };
} AON_FAST_REG0X_RET_SYS_TYPE;

/* 0x6FC
    0       R   RO_AON_ISO_BT_AON2                              1'b1
    1       R   RO_AON_ISO_BT_PON                               1'b1
    2       R   RO_AON_ISO_BT_CORE6                             1'b1
    3       R   RO_AON_ISO_BT_CORE5                             1'b1
    4       R   RO_AON_ISO_BT_CORE4                             1'b1
    5       R   RO_AON_ISO_BT_CORE3                             1'b1
    6       R   RO_AON_ISO_BT_CORE2                             1'b1
    7       R   RO_AON_ISO_BT_CORE1                             1'b1
    8       R   RO_AON_BT_CORE6_RSTB                            1'b0
    9       R   RO_AON_BT_CORE5_RSTB                            1'b0
    10      R   RO_AON_BT_CORE4_RSTB                            1'b0
    11      R   RO_AON_BT_CORE3_RSTB                            1'b0
    12      R   RO_AON_BT_CORE2_RSTB                            1'b0
    13      R   RO_AON_BT_CORE1_RSTB                            1'b0
    14      R   RO_AON_BT_AON2_RSTB                             1'b0
    15      R   RO_AON_BT_PON_RSTB                              1'b0
 */
typedef union _AON_FAST_REG0X_CORE_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_ISO_BT_AON2: 1;
        uint16_t RO_AON_ISO_BT_PON: 1;
        uint16_t RO_AON_ISO_BT_CORE6: 1;
        uint16_t RO_AON_ISO_BT_CORE5: 1;
        uint16_t RO_AON_ISO_BT_CORE4: 1;
        uint16_t RO_AON_ISO_BT_CORE3: 1;
        uint16_t RO_AON_ISO_BT_CORE2: 1;
        uint16_t RO_AON_ISO_BT_CORE1: 1;
        uint16_t RO_AON_BT_CORE6_RSTB: 1;
        uint16_t RO_AON_BT_CORE5_RSTB: 1;
        uint16_t RO_AON_BT_CORE4_RSTB: 1;
        uint16_t RO_AON_BT_CORE3_RSTB: 1;
        uint16_t RO_AON_BT_CORE2_RSTB: 1;
        uint16_t RO_AON_BT_CORE1_RSTB: 1;
        uint16_t RO_AON_BT_AON2_RSTB: 1;
        uint16_t RO_AON_BT_PON_RSTB: 1;
    };
} AON_FAST_REG0X_CORE_SYS_TYPE;

/* 0x6FE
    3:0     R/W CORE_SYS_REG1X_DUMMY1                           4'h0
    4       R/W r_SPDIF_PLL_1_2_SEL                             1'b0
    5       R/W r_SPORT3_PLL_1_2_SEL                            1'b0
    6       R/W r_SPORT2_PLL_1_2_SEL                            1'b0
    7       R/W r_SPORT1_PLL_1_2_SEL                            1'b0
    8       R/W r_SPORT0_PLL_1_2_SEL                            1'b0
    9       R/W r_AUDIO_CLK_FROM_CKO2                           1'b0
    10      R/W r_AUDIO_CLK_FROM_PLL                            1'b0
    11      R/W r_AUDIO_XTAL_SEL                                1'b0
    12      R/W r_aon_cko2_mux_src_sel_1                        1'b0
    13      R/W r_aon_cko2_mux_src_sel_0                        1'b0
    14      R/W r_cpu_clk_src_sel_0                             1'b0
    15      R/W r_cpu_clk_src_sel_1                             1'b0
 */
typedef union _AON_FAST_REG1X_CORE_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CORE_SYS_REG1X_DUMMY1: 4;
        uint16_t r_SPDIF_PLL_1_2_SEL: 1;
        uint16_t r_SPORT3_PLL_1_2_SEL: 1;
        uint16_t r_SPORT2_PLL_1_2_SEL: 1;
        uint16_t r_SPORT1_PLL_1_2_SEL: 1;
        uint16_t r_SPORT0_PLL_1_2_SEL: 1;
        uint16_t r_AUDIO_CLK_FROM_CKO2: 1;
        uint16_t r_AUDIO_CLK_FROM_PLL: 1;
        uint16_t r_AUDIO_XTAL_SEL: 1;
        uint16_t r_aon_cko2_mux_src_sel_1: 1;
        uint16_t r_aon_cko2_mux_src_sel_0: 1;
        uint16_t r_cpu_clk_src_sel_0: 1;
        uint16_t r_cpu_clk_src_sel_1: 1;
    };
} AON_FAST_REG1X_CORE_SYS_TYPE;

/* 0x700
    0       R/W ISO_OTP_PDOUT                                   1'b0
    3:1     R/W spdif_dsp_clk_sel                               3'b000
    6:4     R/W sp3_dsp_clk_sel                                 3'b000
    9:7     R/W sp2_dsp_clk_sel                                 3'b000
    12:10   R/W sp1_dsp_clk_sel                                 3'b000
    15:13   R/W sp0_dsp_clk_sel                                 3'b000
 */
typedef union _AON_FAST_REG2X_CORE_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t ISO_OTP_PDOUT: 1;
        uint16_t spdif_dsp_clk_sel: 3;
        uint16_t sp3_dsp_clk_sel: 3;
        uint16_t sp2_dsp_clk_sel: 3;
        uint16_t sp1_dsp_clk_sel: 3;
        uint16_t sp0_dsp_clk_sel: 3;
    };
} AON_FAST_REG2X_CORE_SYS_TYPE;

/* 0x702
    0       R/W r_CLK_EN_AUDIO_REG                              1'b1
    1       R/W r_FEN_USB                                       1'b0
    2       R/W r_FEN_SPDIF                                     1'b0
    3       R/W r_FEN_SPORT3                                    1'b0
    4       R/W r_FEN_SPORT2                                    1'b0
    5       R/W r_FEN_SPORT1                                    1'b0
    6       R/W r_FEN_SPORT0                                    1'b0
    7       R/W r_FEN_AUDIO                                     1'b0
    8       R/W r_CLK_EN_USB                                    1'b0
    9       R/W r_CLK_EN_SPDIF                                  1'b0
    10      R/W r_CLK_EN_SPORT3                                 1'b0
    11      R/W r_CLK_EN_SPORT2                                 1'b0
    12      R/W r_CLK_EN_SPORT1                                 1'b0
    13      R/W r_CLK_EN_SPORT0                                 1'b0
    14      R/W r_CLK_EN_SPORT_40M                              1'b0
    15      R/W r_CLK_EN_AUDIO                                  1'b0
 */
typedef union _AON_FAST_REG3X_CORE_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_CLK_EN_AUDIO_REG: 1;
        uint16_t r_FEN_USB: 1;
        uint16_t r_FEN_SPDIF: 1;
        uint16_t r_FEN_SPORT3: 1;
        uint16_t r_FEN_SPORT2: 1;
        uint16_t r_FEN_SPORT1: 1;
        uint16_t r_FEN_SPORT0: 1;
        uint16_t r_FEN_AUDIO: 1;
        uint16_t r_CLK_EN_USB: 1;
        uint16_t r_CLK_EN_SPDIF: 1;
        uint16_t r_CLK_EN_SPORT3: 1;
        uint16_t r_CLK_EN_SPORT2: 1;
        uint16_t r_CLK_EN_SPORT1: 1;
        uint16_t r_CLK_EN_SPORT0: 1;
        uint16_t r_CLK_EN_SPORT_40M: 1;
        uint16_t r_CLK_EN_AUDIO: 1;
    };
} AON_FAST_REG3X_CORE_SYS_TYPE;

/* 0x704
    7:0     R/W CORE_SYS_REG4X_DUMMY1                           8'b0000000
    8       R/W r_SPORT2_EXT_CODEC                              1'b0
    9       R/W r_SPORT3_MCLK_OUT                               1'b0
    10      R/W r_SPORT2_MCLK_OUT                               1'b0
    11      R/W r_SPORT1_MCLK_OUT                               1'b0
    12      R/W r_SPORT0_MCLK_OUT                               1'b0
    13      R/W r_CODEC_STANDALONE                              1'b0
    14      R/W r_SPORT1_EXT_CODEC                              1'b0
    15      R/W r_SPORT0_EXT_CODEC                              1'b0
 */
typedef union _AON_FAST_REG4X_CORE_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CORE_SYS_REG4X_DUMMY1: 8;
        uint16_t r_SPORT2_EXT_CODEC: 1;
        uint16_t r_SPORT3_MCLK_OUT: 1;
        uint16_t r_SPORT2_MCLK_OUT: 1;
        uint16_t r_SPORT1_MCLK_OUT: 1;
        uint16_t r_SPORT0_MCLK_OUT: 1;
        uint16_t r_CODEC_STANDALONE: 1;
        uint16_t r_SPORT1_EXT_CODEC: 1;
        uint16_t r_SPORT0_EXT_CODEC: 1;
    };
} AON_FAST_REG4X_CORE_SYS_TYPE;

/* 0x706
    0       R/W XTAL_REG_MASK                                   1'b1
    1       R/W XTAL32K_REG_MASK                                1'b1
    2       R/W BTPLL_REG_MASK                                  1'b1
    3       R/W SWR_AUDIO_REG_MASK                              1'b1
    4       R/W SWR_CORE_REG_MASK                               1'b1
    5       R/W CHG_REG_MASK                                    1'b1
    6       R/W VCORE_PC_REG_MASK                               1'b1
    7       R/W LDO_DIG_REG_MASK                                1'b1
    8       R/W LDO_PA_REG_MASK                                 1'b1
    9       R/W LDOEXT_REG_MASK                                 1'b1
    10      R/W LDOUSB_REG_MASK                                 1'b1
    11      R/W LDOAUX2_REG_MASK                                1'b1
    12      R/W LDOAUX1_REG_MASK                                1'b1
    13      R/W LDOAUX3_REG_MASK                                1'b1
    14      R/W LDOSYS_REG_MASK                                 1'b1
    15      R/W MBIAS_REG_MASK                                  1'b1
 */
typedef union _AON_FAST_REG0X_REG_MASK_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_REG_MASK: 1;
        uint16_t XTAL32K_REG_MASK: 1;
        uint16_t BTPLL_REG_MASK: 1;
        uint16_t SWR_AUDIO_REG_MASK: 1;
        uint16_t SWR_CORE_REG_MASK: 1;
        uint16_t CHG_REG_MASK: 1;
        uint16_t VCORE_PC_REG_MASK: 1;
        uint16_t LDO_DIG_REG_MASK: 1;
        uint16_t LDO_PA_REG_MASK: 1;
        uint16_t LDOEXT_REG_MASK: 1;
        uint16_t LDOUSB_REG_MASK: 1;
        uint16_t LDOAUX2_REG_MASK: 1;
        uint16_t LDOAUX1_REG_MASK: 1;
        uint16_t LDOAUX3_REG_MASK: 1;
        uint16_t LDOSYS_REG_MASK: 1;
        uint16_t MBIAS_REG_MASK: 1;
    };
} AON_FAST_REG0X_REG_MASK_TYPE;

/* 0x708
    12:0    R/W REG_MASK_REG1X_DUMMY1                           13'h0
    13      R/W CODEC_REG_MASK                                  1'b1
    14      R/W AUXADC_REG_MASK                                 1'b1
    15      R/W OSC40_REG_MASK                                  1'b1
 */
typedef union _AON_FAST_REG1X_REG_MASK_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t REG_MASK_REG1X_DUMMY1: 13;
        uint16_t CODEC_REG_MASK: 1;
        uint16_t AUXADC_REG_MASK: 1;
        uint16_t OSC40_REG_MASK: 1;
    };
} AON_FAST_REG1X_REG_MASK_TYPE;

/* 0x70A
    7:0     R/W PMU_POS_CLK_REG0X_DUMMY2                        8'h0
    8       R/W REG_SWR_AUDIO_POS_CLK_SEL                       1'b1
    9       R/W REG_SWR_CORE_POS_CLK_SEL                        1'b1
    12:10   R/W PMU_POS_CLK_REG0X_DUMMY1                        3'b000
    13      R/W REG_LDOEXT_POS_CLK_SEL                          1'b1
    14      R/W REG_LDOAUX3_POS_CLK_SEL                         1'b1
    15      R/W REG_LDOSYS_POS_CLK_SEL                          1'b1
 */
typedef union _AON_FAST_REG0X_PMU_POS_CLK_MUX_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMU_POS_CLK_REG0X_DUMMY2: 8;
        uint16_t REG_SWR_AUDIO_POS_CLK_SEL: 1;
        uint16_t REG_SWR_CORE_POS_CLK_SEL: 1;
        uint16_t PMU_POS_CLK_REG0X_DUMMY1: 3;
        uint16_t REG_LDOEXT_POS_CLK_SEL: 1;
        uint16_t REG_LDOAUX3_POS_CLK_SEL: 1;
        uint16_t REG_LDOSYS_POS_CLK_SEL: 1;
    };
} AON_FAST_REG0X_PMU_POS_CLK_MUX_TYPE;

/* 0x70C
    7:0     R   RSVD                                            8'h0
    15:8    R/W r_ADSP_DIV_SETTING                              8'h0
 */
typedef union _AON_FAST_REG0X_ANCDSP_CTRL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD: 8;
        uint16_t r_ADSP_DIV_SETTING: 8;
    };
} AON_FAST_REG0X_ANCDSP_CTRL_TYPE;

/* 0x70E
    6:0     R   RSVD                                            7'h0
    7       R/W r_FEN_ANCDSP_BUS                                1'h0
    8       R/W r_CKE_ANCDSP_BUS                                1'h0
    9       R/W r_FEN_ANCDSP                                    1'h0
    10      R/W r_CKE_ANCDSP                                    1'h0
    11      R/W ancdsp_clk_sel_0                                1'h0
    12      R/W ancdsp_clk_sel_1                                1'h0
    13      R/W ancdsp_clk_sel_2                                1'h0
    14      R/W r_ANCDSP_PLL_1_3_SEL                            1'h0
    15      R/W r_ANCDSP_CLK_FROM_PLL                           1'h0
 */
typedef union _AON_FAST_REG1X_ANCDSP_CTRL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RSVD: 7;
        uint16_t r_FEN_ANCDSP_BUS: 1;
        uint16_t r_CKE_ANCDSP_BUS: 1;
        uint16_t r_FEN_ANCDSP: 1;
        uint16_t r_CKE_ANCDSP: 1;
        uint16_t ancdsp_clk_sel_0: 1;
        uint16_t ancdsp_clk_sel_1: 1;
        uint16_t ancdsp_clk_sel_2: 1;
        uint16_t r_ANCDSP_PLL_1_3_SEL: 1;
        uint16_t r_ANCDSP_CLK_FROM_PLL: 1;
    };
} AON_FAST_REG1X_ANCDSP_CTRL_TYPE;

/* 0x710
    0       R/W r_PLL_EN_SPORT0                                 1'b0
    1       R/W r_PLL_EN_SPORT1                                 1'b0
    2       R/W r_PLL_EN_SPORT2                                 1'b0
    3       R/W r_PLL_EN_SPORT3                                 1'b0
    4       R/W r_clk_pll1_sel_aon                              1'b0
    5       R/W r_clk_pll3_sel_aon                              1'b0
    6       R/W r_clk_pll3_2_sel_aon                            1'b0
    7       R/W r_clk_pll3_3_sel_aon                            1'b0
    8       R/W r_clk_pll3_4_sel_aon                            1'b0
    15:9    R/W CORE_SYS_REG5X_DUMMY1                           7'h0
 */
typedef union _AON_FAST_REG5X_CORE_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_PLL_EN_SPORT0: 1;
        uint16_t r_PLL_EN_SPORT1: 1;
        uint16_t r_PLL_EN_SPORT2: 1;
        uint16_t r_PLL_EN_SPORT3: 1;
        uint16_t r_clk_pll1_sel_aon: 1;
        uint16_t r_clk_pll3_sel_aon: 1;
        uint16_t r_clk_pll3_2_sel_aon: 1;
        uint16_t r_clk_pll3_3_sel_aon: 1;
        uint16_t r_clk_pll3_4_sel_aon: 1;
        uint16_t CORE_SYS_REG5X_DUMMY1: 7;
    };
} AON_FAST_REG5X_CORE_SYS_TYPE;

/* 0x712
    0       R/W r_aon_clk_sel0                                  1'b0
    1       R/W r_aon_clk_sel1                                  1'b0
    2       R/W r_aon_clk_sel2                                  1'b0
    3       R/W r_aon_clk_sel3                                  1'b0
    4       R/W r_aon_clk_sel4                                  1'b0
    5       R/W r_aon_clk_sel5                                  1'b0
    6       R/W r_aon_cko2_mux_vcore3_cg_en                     1'b0
    7       R/W r_aon_cko2_pll_src_sel                          1'b0
    8       R/W r_aon_clk_cg_en0                                1'b0
    9       R/W r_aon_clk_cg_en1                                1'b0
    10      R/W r_aon_clk_cg_en2                                1'b0
    11      R/W r_aon_clk_cg_en3                                1'b0
    12      R/W r_aon_clk_cg_en4                                1'b0
    13      R/W r_aon_clk_cg_en5                                1'b0
    14      R/W r_aon_cpu_pll_src_sel0                          1'b0
    15      R/W r_aon_cpu_pll_src_sel1                          1'b0
 */
typedef union _AON_FAST_REG6X_CORE_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_aon_clk_sel0: 1;
        uint16_t r_clk_pll3_5_sel_aon: 1;
        uint16_t r_aon_clk_sel2: 1;
        uint16_t r_aon_clk_sel3: 1;
        uint16_t r_aon_clk_sel4: 1;
        uint16_t r_aon_clk_sel5: 1;
        uint16_t r_aon_cko2_mux_vcore3_cg_en: 1;
        uint16_t r_aon_cko2_pll_src_sel: 1;
        uint16_t r_aon_clk_cg_en0: 1;
        uint16_t r_aon_clk_cg_en1: 1;
        uint16_t r_aon_clk_cg_en2: 1;
        uint16_t r_aon_clk_cg_en3: 1;
        uint16_t r_aon_clk_cg_en4: 1;
        uint16_t r_aon_clk_cg_en5: 1;
        uint16_t r_aon_cpu_pll_src_sel0: 1;
        uint16_t r_aon_cpu_pll_src_sel1: 1;
    };
} AON_FAST_REG6X_CORE_SYS_TYPE;

/* 0x714
    0       R/W r_aon_clk_sel6                                  1'b0
    1       R/W r_aon_clk_sel7                                  1'b0
    2       R/W r_aon_clk_sel8                                  1'b0
    3       R/W r_aon_clk_sel9                                  1'b0
    4       R/W r_aon_clk_sel10                                 1'b0
    5       R/W r_aon_clk_sel11                                 1'b0
    6       R/W r_aon_clk_sel12                                 1'b0
    7       R/W r_aon_clk_sel13                                 1'b0
    8       R/W r_aon_clk_cg_en6                                1'b0
    9       R/W r_aon_clk_cg_en7                                1'b0
    10      R/W r_aon_clk_cg_en8                                1'b0
    11      R/W r_aon_clk_cg_en9                                1'b0
    12      R/W r_aon_clk_cg_en10                               1'b0
    13      R/W r_aon_clk_cg_en11                               1'b0
    14      R/W r_aon_clk_cg_en12                               1'b0
    15      R/W r_aon_clk_cg_en13                               1'b0
 */
typedef union _AON_FAST_REG7X_CORE_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_aon_clk_sel6: 1;
        uint16_t r_aon_clk_sel7: 1;
        uint16_t r_aon_clk_sel8: 1;
        uint16_t r_aon_clk_sel9: 1;
        uint16_t r_aon_clk_sel10: 1;
        uint16_t r_aon_clk_sel11: 1;
        uint16_t r_aon_clk_sel12: 1;
        uint16_t r_aon_clk_sel13: 1;
        uint16_t r_aon_clk_cg_en6: 1;
        uint16_t r_aon_clk_cg_en7: 1;
        uint16_t r_aon_clk_cg_en8: 1;
        uint16_t r_aon_clk_cg_en9: 1;
        uint16_t r_aon_clk_cg_en10: 1;
        uint16_t r_aon_clk_cg_en11: 1;
        uint16_t r_aon_clk_cg_en12: 1;
        uint16_t r_aon_clk_cg_en13: 1;
    };
} AON_FAST_REG7X_CORE_SYS_TYPE;

/* 0x716
    0       R/W r_aon_clk_sel14                                 1'b0
    1       R/W r_aon_clk_sel15                                 1'b0
    2       R/W r_aon_clk_sel16                                 1'b0
    3       R/W r_aon_clk_sel17                                 1'b0
    4       R/W r_aon_clk_sel18                                 1'b0
    5       R/W r_aon_clk_sel19                                 1'b0
    6       R/W r_aon_clk_sel20                                 1'b0
    7       R/W r_aon_clk_sel21                                 1'b0
    8       R/W r_aon_clk_cg_en14                               1'b0
    9       R/W r_aon_clk_cg_en15                               1'b0
    10      R/W r_aon_clk_cg_en16                               1'b0
    11      R/W r_aon_clk_cg_en17                               1'b0
    12      R/W r_aon_clk_cg_en18                               1'b0
    13      R/W r_aon_clk_cg_en19                               1'b0
    14      R/W r_aon_clk_cg_en20                               1'b0
    15      R/W r_aon_clk_cg_en21                               1'b0
 */
typedef union _AON_FAST_REG8X_CORE_SYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t r_aon_clk_sel14: 1;
        uint16_t r_aon_clk_sel15: 1;
        uint16_t r_aon_clk_sel16: 1;
        uint16_t r_aon_clk_sel17: 1;
        uint16_t r_aon_clk_sel18: 1;
        uint16_t r_aon_clk_sel19: 1;
        uint16_t r_aon_clk_sel20: 1;
        uint16_t r_aon_clk_sel21: 1;
        uint16_t r_aon_clk_cg_en14: 1;
        uint16_t r_aon_clk_cg_en15: 1;
        uint16_t r_aon_clk_cg_en16: 1;
        uint16_t r_aon_clk_cg_en17: 1;
        uint16_t r_aon_clk_cg_en18: 1;
        uint16_t r_aon_clk_cg_en19: 1;
        uint16_t r_aon_clk_cg_en20: 1;
        uint16_t r_aon_clk_cg_en21: 1;
    };
} AON_FAST_REG8X_CORE_SYS_TYPE;

/* 0x742
    0       R/W PAD_LPC_CSN_SHDN                                1'b1
    1       R/W PAD_LPC_REG0X_DUMMY8                            1'b0
    2       R/W PAD_LPC_REG0X_DUMMY7                            1'b0
    3       R/W PAD_LPC_REG0X_DUMMY6                            1'b0
    4       R/W PAD_LPC_REG0X_DUMMY5                            1'b0
    5       R/W PAD_LPC_CSN_PDPUC                               1'b0
    6       R/W AON_PAD_LPC_CSN_PU                              1'b0
    7       R/W AON_PAD_LPC_CSN_PU_EN                           1'b1
    8       R/W PAD_LPC_RWSD_SHDN                               1'b1
    9       R/W PAD_LPC_REG0X_DUMMY4                            1'b0
    10      R/W PAD_LPC_REG0X_DUMMY3                            1'b0
    11      R/W PAD_LPC_REG0X_DUMMY2                            1'b0
    12      R/W PAD_LPC_REG0X_DUMMY1                            1'b0
    13      R/W PAD_LPC_RWSD_PDPUC                              1'b0
    14      R/W AON_PAD_LPC_RWSD_PU                             1'b0
    15      R/W AON_PAD_LPC_RWSD_PU_EN                          1'b1
 */
typedef union _AON_FAST_REG0X_PAD_LPC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_LPC_CSN_SHDN: 1;
        uint16_t PAD_LPC_REG0X_DUMMY8: 1;
        uint16_t PAD_LPC_REG0X_DUMMY7: 1;
        uint16_t PAD_LPC_REG0X_DUMMY6: 1;
        uint16_t PAD_LPC_REG0X_DUMMY5: 1;
        uint16_t PAD_LPC_CSN_PDPUC: 1;
        uint16_t AON_PAD_LPC_CSN_PU: 1;
        uint16_t AON_PAD_LPC_CSN_PU_EN: 1;
        uint16_t PAD_LPC_RWSD_SHDN: 1;
        uint16_t PAD_LPC_REG0X_DUMMY4: 1;
        uint16_t PAD_LPC_REG0X_DUMMY3: 1;
        uint16_t PAD_LPC_REG0X_DUMMY2: 1;
        uint16_t PAD_LPC_REG0X_DUMMY1: 1;
        uint16_t PAD_LPC_RWSD_PDPUC: 1;
        uint16_t AON_PAD_LPC_RWSD_PU: 1;
        uint16_t AON_PAD_LPC_RWSD_PU_EN: 1;
    };
} AON_FAST_REG0X_PAD_LPC_TYPE;

/* 0x744
    0       R/W PAD_LPC_D6_SHDN                                 1'b1
    1       R/W PAD_LPC_REG1X_DUMMY8                            1'b0
    2       R/W PAD_LPC_REG1X_DUMMY7                            1'b0
    3       R/W PAD_LPC_REG1X_DUMMY6                            1'b0
    4       R/W PAD_LPC_REG1X_DUMMY5                            1'b0
    5       R/W PAD_LPC_D6_PDPUC                                1'b0
    6       R/W AON_PAD_LPC_D6_PU                               1'b0
    7       R/W AON_PAD_LPC_D6_PU_EN                            1'b1
    8       R/W PAD_LPC_D7_SHDN                                 1'b1
    9       R/W PAD_LPC_REG1X_DUMMY4                            1'b0
    10      R/W PAD_LPC_REG1X_DUMMY3                            1'b0
    11      R/W PAD_LPC_REG1X_DUMMY2                            1'b0
    12      R/W PAD_LPC_REG1X_DUMMY1                            1'b0
    13      R/W PAD_LPC_D7_PDPUC                                1'b0
    14      R/W AON_PAD_LPC_D7_PU                               1'b0
    15      R/W AON_PAD_LPC_D7_PU_EN                            1'b1
 */
typedef union _AON_FAST_REG1X_PAD_LPC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_LPC_D6_SHDN: 1;
        uint16_t PAD_LPC_REG1X_DUMMY8: 1;
        uint16_t PAD_LPC_REG1X_DUMMY7: 1;
        uint16_t PAD_LPC_REG1X_DUMMY6: 1;
        uint16_t PAD_LPC_REG1X_DUMMY5: 1;
        uint16_t PAD_LPC_D6_PDPUC: 1;
        uint16_t AON_PAD_LPC_D6_PU: 1;
        uint16_t AON_PAD_LPC_D6_PU_EN: 1;
        uint16_t PAD_LPC_D7_SHDN: 1;
        uint16_t PAD_LPC_REG1X_DUMMY4: 1;
        uint16_t PAD_LPC_REG1X_DUMMY3: 1;
        uint16_t PAD_LPC_REG1X_DUMMY2: 1;
        uint16_t PAD_LPC_REG1X_DUMMY1: 1;
        uint16_t PAD_LPC_D7_PDPUC: 1;
        uint16_t AON_PAD_LPC_D7_PU: 1;
        uint16_t AON_PAD_LPC_D7_PU_EN: 1;
    };
} AON_FAST_REG1X_PAD_LPC_TYPE;

/* 0x746
    0       R/W PAD_LPC_D4_SHDN                                 1'b1
    1       R/W PAD_LPC_REG2X_DUMMY8                            1'b0
    2       R/W PAD_LPC_REG2X_DUMMY7                            1'b0
    3       R/W PAD_LPC_REG2X_DUMMY6                            1'b0
    4       R/W PAD_LPC_REG2X_DUMMY5                            1'b0
    5       R/W PAD_LPC_D4_PDPUC                                1'b0
    6       R/W AON_PAD_LPC_D4_PU                               1'b0
    7       R/W AON_PAD_LPC_D4_PU_EN                            1'b1
    8       R/W PAD_LPC_D5_SHDN                                 1'b1
    9       R/W PAD_LPC_REG2X_DUMMY4                            1'b0
    10      R/W PAD_LPC_REG2X_DUMMY3                            1'b0
    11      R/W PAD_LPC_REG2X_DUMMY2                            1'b0
    12      R/W PAD_LPC_REG2X_DUMMY1                            1'b0
    13      R/W PAD_LPC_D5_PDPUC                                1'b0
    14      R/W AON_PAD_LPC_D5_PU                               1'b0
    15      R/W AON_PAD_LPC_D5_PU_EN                            1'b1
 */
typedef union _AON_FAST_REG2X_PAD_LPC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_LPC_D4_SHDN: 1;
        uint16_t PAD_LPC_REG2X_DUMMY8: 1;
        uint16_t PAD_LPC_REG2X_DUMMY7: 1;
        uint16_t PAD_LPC_REG2X_DUMMY6: 1;
        uint16_t PAD_LPC_REG2X_DUMMY5: 1;
        uint16_t PAD_LPC_D4_PDPUC: 1;
        uint16_t AON_PAD_LPC_D4_PU: 1;
        uint16_t AON_PAD_LPC_D4_PU_EN: 1;
        uint16_t PAD_LPC_D5_SHDN: 1;
        uint16_t PAD_LPC_REG2X_DUMMY4: 1;
        uint16_t PAD_LPC_REG2X_DUMMY3: 1;
        uint16_t PAD_LPC_REG2X_DUMMY2: 1;
        uint16_t PAD_LPC_REG2X_DUMMY1: 1;
        uint16_t PAD_LPC_D5_PDPUC: 1;
        uint16_t AON_PAD_LPC_D5_PU: 1;
        uint16_t AON_PAD_LPC_D5_PU_EN: 1;
    };
} AON_FAST_REG2X_PAD_LPC_TYPE;

/* 0x748
    0       R/W PAD_LPC_D2_SHDN                                 1'b1
    1       R/W PAD_LPC_REG3X_DUMMY8                            1'b0
    2       R/W PAD_LPC_REG3X_DUMMY7                            1'b0
    3       R/W PAD_LPC_REG3X_DUMMY6                            1'b0
    4       R/W PAD_LPC_REG3X_DUMMY5                            1'b0
    5       R/W PAD_LPC_D2_PDPUC                                1'b0
    6       R/W AON_PAD_LPC_D2_PU                               1'b0
    7       R/W AON_PAD_LPC_D2_PU_EN                            1'b1
    8       R/W PAD_LPC_D3_SHDN                                 1'b1
    9       R/W PAD_LPC_REG3X_DUMMY4                            1'b0
    10      R/W PAD_LPC_REG3X_DUMMY3                            1'b0
    11      R/W PAD_LPC_REG3X_DUMMY2                            1'b0
    12      R/W PAD_LPC_REG3X_DUMMY1                            1'b0
    13      R/W PAD_LPC_D3_PDPUC                                1'b0
    14      R/W AON_PAD_LPC_D3_PU                               1'b0
    15      R/W AON_PAD_LPC_D3_PU_EN                            1'b1
 */
typedef union _AON_FAST_REG3X_PAD_LPC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_LPC_D2_SHDN: 1;
        uint16_t PAD_LPC_REG3X_DUMMY8: 1;
        uint16_t PAD_LPC_REG3X_DUMMY7: 1;
        uint16_t PAD_LPC_REG3X_DUMMY6: 1;
        uint16_t PAD_LPC_REG3X_DUMMY5: 1;
        uint16_t PAD_LPC_D2_PDPUC: 1;
        uint16_t AON_PAD_LPC_D2_PU: 1;
        uint16_t AON_PAD_LPC_D2_PU_EN: 1;
        uint16_t PAD_LPC_D3_SHDN: 1;
        uint16_t PAD_LPC_REG3X_DUMMY4: 1;
        uint16_t PAD_LPC_REG3X_DUMMY3: 1;
        uint16_t PAD_LPC_REG3X_DUMMY2: 1;
        uint16_t PAD_LPC_REG3X_DUMMY1: 1;
        uint16_t PAD_LPC_D3_PDPUC: 1;
        uint16_t AON_PAD_LPC_D3_PU: 1;
        uint16_t AON_PAD_LPC_D3_PU_EN: 1;
    };
} AON_FAST_REG3X_PAD_LPC_TYPE;

/* 0x74A
    0       R/W PAD_LPC_D0_SHDN                                 1'b1
    1       R/W PAD_LPC_REG4X_DUMMY8                            1'b0
    2       R/W PAD_LPC_REG4X_DUMMY7                            1'b0
    3       R/W PAD_LPC_REG4X_DUMMY6                            1'b0
    4       R/W PAD_LPC_REG4X_DUMMY5                            1'b0
    5       R/W PAD_LPC_D0_PDPUC                                1'b0
    6       R/W AON_PAD_LPC_D0_PU                               1'b0
    7       R/W AON_PAD_LPC_D0_PU_EN                            1'b1
    8       R/W PAD_LPC_D1_SHDN                                 1'b1
    9       R/W PAD_LPC_REG4X_DUMMY4                            1'b0
    10      R/W PAD_LPC_REG4X_DUMMY3                            1'b0
    11      R/W PAD_LPC_REG4X_DUMMY2                            1'b0
    12      R/W PAD_LPC_REG4X_DUMMY1                            1'b0
    13      R/W PAD_LPC_D1_PDPUC                                1'b0
    14      R/W AON_PAD_LPC_D1_PU                               1'b0
    15      R/W AON_PAD_LPC_D1_PU_EN                            1'b1
 */
typedef union _AON_FAST_REG4X_PAD_LPC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_LPC_D0_SHDN: 1;
        uint16_t PAD_LPC_REG4X_DUMMY8: 1;
        uint16_t PAD_LPC_REG4X_DUMMY7: 1;
        uint16_t PAD_LPC_REG4X_DUMMY6: 1;
        uint16_t PAD_LPC_REG4X_DUMMY5: 1;
        uint16_t PAD_LPC_D0_PDPUC: 1;
        uint16_t AON_PAD_LPC_D0_PU: 1;
        uint16_t AON_PAD_LPC_D0_PU_EN: 1;
        uint16_t PAD_LPC_D1_SHDN: 1;
        uint16_t PAD_LPC_REG4X_DUMMY4: 1;
        uint16_t PAD_LPC_REG4X_DUMMY3: 1;
        uint16_t PAD_LPC_REG4X_DUMMY2: 1;
        uint16_t PAD_LPC_REG4X_DUMMY1: 1;
        uint16_t PAD_LPC_D1_PDPUC: 1;
        uint16_t AON_PAD_LPC_D1_PU: 1;
        uint16_t AON_PAD_LPC_D1_PU_EN: 1;
    };
} AON_FAST_REG4X_PAD_LPC_TYPE;

/* 0x74C
    0       R/W PAD_LPC_CKN_SHDN                                1'b1
    1       R/W PAD_LPC_REG5X_DUMMY8                            1'b0
    2       R/W PAD_LPC_REG5X_DUMMY7                            1'b0
    3       R/W PAD_LPC_REG5X_DUMMY6                            1'b0
    4       R/W PAD_LPC_REG5X_DUMMY5                            1'b0
    5       R/W PAD_LPC_CKN_PDPUC                               1'b0
    6       R/W AON_PAD_LPC_CKN_PU                              1'b0
    7       R/W AON_PAD_LPC_CKN_PU_EN                           1'b1
    8       R/W PAD_LPC_CK_SHDN                                 1'b1
    9       R/W PAD_LPC_REG5X_DUMMY4                            1'b0
    10      R/W PAD_LPC_REG5X_DUMMY3                            1'b0
    11      R/W PAD_LPC_REG5X_DUMMY2                            1'b0
    12      R/W PAD_LPC_REG5X_DUMMY1                            1'b0
    13      R/W PAD_LPC_CK_PDPUC                                1'b0
    14      R/W AON_PAD_LPC_CK_PU                               1'b0
    15      R/W AON_PAD_LPC_CK_PU_EN                            1'b1
 */
typedef union _AON_FAST_REG5X_PAD_LPC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_LPC_CKN_SHDN: 1;
        uint16_t PAD_LPC_REG5X_DUMMY8: 1;
        uint16_t PAD_LPC_REG5X_DUMMY7: 1;
        uint16_t PAD_LPC_REG5X_DUMMY6: 1;
        uint16_t PAD_LPC_REG5X_DUMMY5: 1;
        uint16_t PAD_LPC_CKN_PDPUC: 1;
        uint16_t AON_PAD_LPC_CKN_PU: 1;
        uint16_t AON_PAD_LPC_CKN_PU_EN: 1;
        uint16_t PAD_LPC_CK_SHDN: 1;
        uint16_t PAD_LPC_REG5X_DUMMY4: 1;
        uint16_t PAD_LPC_REG5X_DUMMY3: 1;
        uint16_t PAD_LPC_REG5X_DUMMY2: 1;
        uint16_t PAD_LPC_REG5X_DUMMY1: 1;
        uint16_t PAD_LPC_CK_PDPUC: 1;
        uint16_t AON_PAD_LPC_CK_PU: 1;
        uint16_t AON_PAD_LPC_CK_PU_EN: 1;
    };
} AON_FAST_REG5X_PAD_LPC_TYPE;

/* 0x74E
    0       R/W PAD_LPC_RTSN_SHDN                               1'b1
    1       R/W PAD_LPC_REG6X_DUMMY12                           1'b0
    2       R/W PAD_LPC_REG6X_DUMMY11                           1'b0
    3       R/W PAD_LPC_REG6X_DUMMY10                           1'b0
    4       R/W PAD_LPC_REG6X_DUMMY9                            1'b0
    5       R/W PAD_LPC_RTSN_PDPUC                              1'b0
    6       R/W AON_PAD_LPC_RTSN_PU                             1'b0
    7       R/W AON_PAD_LPC_RTSN_PU_EN                          1'b1
    8       R/W PAD_LPC_REG6X_DUMMY8                            1'b0
    9       R/W PAD_LPC_REG6X_DUMMY7                            1'b0
    10      R/W PAD_LPC_REG6X_DUMMY6                            1'b0
    11      R/W PAD_LPC_REG6X_DUMMY5                            1'b0
    12      R/W PAD_LPC_REG6X_DUMMY4                            1'b0
    13      R/W PAD_LPC_REG6X_DUMMY3                            1'b0
    14      R/W PAD_LPC_REG6X_DUMMY2                            1'b0
    15      R/W PAD_LPC_REG6X_DUMMY1                            1'b0
 */
typedef union _AON_FAST_REG6X_PAD_LPC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_LPC_RTSN_SHDN: 1;
        uint16_t PAD_LPC_REG6X_DUMMY12: 1;
        uint16_t PAD_LPC_REG6X_DUMMY11: 1;
        uint16_t PAD_LPC_REG6X_DUMMY10: 1;
        uint16_t PAD_LPC_REG6X_DUMMY9: 1;
        uint16_t PAD_LPC_RTSN_PDPUC: 1;
        uint16_t AON_PAD_LPC_RTSN_PU: 1;
        uint16_t AON_PAD_LPC_RTSN_PU_EN: 1;
        uint16_t PAD_LPC_REG6X_DUMMY8: 1;
        uint16_t PAD_LPC_REG6X_DUMMY7: 1;
        uint16_t PAD_LPC_REG6X_DUMMY6: 1;
        uint16_t PAD_LPC_REG6X_DUMMY5: 1;
        uint16_t PAD_LPC_REG6X_DUMMY4: 1;
        uint16_t PAD_LPC_REG6X_DUMMY3: 1;
        uint16_t PAD_LPC_REG6X_DUMMY2: 1;
        uint16_t PAD_LPC_REG6X_DUMMY1: 1;
    };
} AON_FAST_REG6X_PAD_LPC_TYPE;

/* 0x750
    0       R/W PAD_LPC_REG7X_DUMMY16                           1'b0
    1       R/W PAD_LPC_REG7X_DUMMY15                           1'b0
    2       R/W PAD_LPC_REG7X_DUMMY14                           1'b0
    3       R/W PAD_LPC_REG7X_DUMMY13                           1'b0
    4       R/W PAD_LPC_REG7X_DUMMY12                           1'b0
    5       R/W PAD_LPC_REG7X_DUMMY11                           1'b0
    6       R/W PAD_LPC_REG7X_DUMMY10                           1'b0
    7       R/W PAD_LPC_REG7X_DUMMY9                            1'b0
    8       R/W PAD_LPC_REG7X_DUMMY8                            1'b0
    9       R/W PAD_LPC_REG7X_DUMMY7                            1'b0
    10      R/W PAD_LPC_REG7X_DUMMY6                            1'b0
    11      R/W PAD_LPC_REG7X_DUMMY5                            1'b0
    12      R/W PAD_LPC_REG7X_DUMMY4                            1'b0
    13      R/W PAD_LPC_REG7X_DUMMY3                            1'b0
    14      R/W PAD_LPC_E3                                      1'b0
    15      R/W PAD_LPC_E2                                      1'b0
 */
typedef union _AON_FAST_REG7X_PAD_LPC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_LPC_REG7X_DUMMY16: 1;
        uint16_t PAD_LPC_REG7X_DUMMY15: 1;
        uint16_t PAD_LPC_REG7X_DUMMY14: 1;
        uint16_t PAD_LPC_REG7X_DUMMY13: 1;
        uint16_t PAD_LPC_REG7X_DUMMY12: 1;
        uint16_t PAD_LPC_REG7X_DUMMY11: 1;
        uint16_t PAD_LPC_REG7X_DUMMY10: 1;
        uint16_t PAD_LPC_REG7X_DUMMY9: 1;
        uint16_t PAD_LPC_REG7X_DUMMY8: 1;
        uint16_t PAD_LPC_REG7X_DUMMY7: 1;
        uint16_t PAD_LPC_REG7X_DUMMY6: 1;
        uint16_t PAD_LPC_REG7X_DUMMY5: 1;
        uint16_t PAD_LPC_REG7X_DUMMY4: 1;
        uint16_t PAD_LPC_REG7X_DUMMY3: 1;
        uint16_t PAD_LPC_E3: 1;
        uint16_t PAD_LPC_E2: 1;
    };
} AON_FAST_REG7X_PAD_LPC_TYPE;

/* 0x800
    0       R/W LOP_PON_RG0X_DUMMY1                             1'h0
    1       R/W LOP_PON_MBIAS_POW_VD105_RF_DET                  1'b1
    2       R/W LOP_PON_MBIAS_POW_VAUX3_DET                     1'b1
    3       R/W LOP_PON_MBIAS_POW_VAUX2_DET                     1'b1
    4       R/W LOP_PON_MBIAS_POW_VAUX1_DET                     1'b1
    5       R/W LOP_PON_MBIAS_POW_VAUDIO_DET                    1'b1
    6       R/W LOP_PON_XTAL_EN_LPS_CLK                         1'b1
    7       R/W LOP_PON_MBIAS_POW_HV33_HQ_DET                   1'b1
    8       R/W LOP_PON_BT_AON2_RSTB                            1'b1
    9       R/W LOP_PON_MBIAS_POW_HV_DET                        1'b1
    10      R/W LOP_PON_ISO_BT_AON2                             1'b0
    11      R/W LOP_PON_MBIAS_FSM_DUMMY1                        1'b1
    12      R/W LOP_PON_MBIAS_POW_BIAS_500NA                    1'b1
    13      R/W LOP_PON_MBIAS_POW_BIAS_10UA                     1'b1
    14      R/W LOP_PON_CHG_EN_HV_PD                            1'b0
    15      R/W LOP_PON_MBIAS_POW_PCUT_DVDD_TO_DVDD1            1'b1
 */
typedef union _AON_FAST_LOP_PON_RG0X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_RG0X_DUMMY1: 1;
        uint16_t LOP_PON_MBIAS_POW_VD105_RF_DET: 1;
        uint16_t LOP_PON_MBIAS_POW_VAUX3_DET: 1;
        uint16_t LOP_PON_MBIAS_POW_VAUX2_DET: 1;
        uint16_t LOP_PON_MBIAS_POW_VAUX1_DET: 1;
        uint16_t LOP_PON_MBIAS_POW_VAUDIO_DET: 1;
        uint16_t LOP_PON_XTAL_EN_LPS_CLK: 1;
        uint16_t LOP_PON_MBIAS_POW_HV33_HQ_DET: 1;
        uint16_t LOP_PON_BT_AON2_RSTB: 1;
        uint16_t LOP_PON_MBIAS_POW_HV_DET: 1;
        uint16_t LOP_PON_ISO_BT_AON2: 1;
        uint16_t LOP_PON_MBIAS_FSM_DUMMY1: 1;
        uint16_t LOP_PON_MBIAS_POW_BIAS_500NA: 1;
        uint16_t LOP_PON_MBIAS_POW_BIAS_10UA: 1;
        uint16_t LOP_PON_CHG_EN_HV_PD: 1;
        uint16_t LOP_PON_MBIAS_POW_PCUT_DVDD_TO_DVDD1: 1;
    };
} AON_FAST_LOP_PON_RG0X_TYPE;

/* 0x802
    0       R/W LOP_PON_XTAL_GATED_AFEN                         1'b0
    1       R/W LOP_PON_XTAL_GATED_AFEN_LP                      1'b0
    2       R/W LOP_PON_XTAL_GATED_AFEP                         1'b0
    3       R/W LOP_PON_XTAL_GATED_AFEP_LP                      1'b0
    6:4     R/W LOP_PON_MBIAS_TUNE_BIAS_50NA_IQ                 3'b000
    9:7     R/W LOP_PON_MBIAS_TUNE_BIAS_500NA_IQ                3'b000
    12:10   R/W LOP_PON_MBIAS_TUNE_BIAS_3NA_IQ                  3'b000
    15:13   R/W LOP_PON_MBIAS_TUNE_BIAS_10UA_IQ                 3'b000
 */
typedef union _AON_FAST_LOP_PON_RG1X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_XTAL_GATED_AFEN: 1;
        uint16_t LOP_PON_XTAL_GATED_AFEN_LP: 1;
        uint16_t LOP_PON_XTAL_GATED_AFEP: 1;
        uint16_t LOP_PON_XTAL_GATED_AFEP_LP: 1;
        uint16_t LOP_PON_MBIAS_TUNE_BIAS_50NA_IQ: 3;
        uint16_t LOP_PON_MBIAS_TUNE_BIAS_500NA_IQ: 3;
        uint16_t LOP_PON_MBIAS_TUNE_BIAS_3NA_IQ: 3;
        uint16_t LOP_PON_MBIAS_TUNE_BIAS_10UA_IQ: 3;
    };
} AON_FAST_LOP_PON_RG1X_TYPE;

/* 0x804
    5:0     R/W LOP_PON_RG2X_DUMMY1                             6'h0
    6       R/W LOP_PON_POW32K_32KXTAL33                        1'b1
    7       R/W LOP_PON_POW32K_32KOSC33                         1'b1
    9:8     R/W LOP_PON_MBIAS_TUNE_LPBG_IQ                      2'b10
    15:10   R/W LOP_PON_MBIAS_TUNE_LDO309_VOUT                  6'b100001
 */
typedef union _AON_FAST_LOP_PON_RG2X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_RG2X_DUMMY1: 6;
        uint16_t LOP_PON_POW32K_32KXTAL33: 1;
        uint16_t LOP_PON_POW32K_32KOSC33: 1;
        uint16_t LOP_PON_MBIAS_TUNE_LPBG_IQ: 2;
        uint16_t LOP_PON_MBIAS_TUNE_LDO309_VOUT: 6;
    };
} AON_FAST_LOP_PON_RG2X_TYPE;

/* 0x806
    7:0     R/W LOP_PON_LDOAUX2_TUNE_LDO_VOUT                   8'b00100101
    15:8    R/W LOP_PON_LDOAUX1_TUNE_LDO_VOUT                   8'b00100101
 */
typedef union _AON_FAST_LOP_PON_RG3X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_LDOAUX2_TUNE_LDO_VOUT: 8;
        uint16_t LOP_PON_LDOAUX1_TUNE_LDO_VOUT: 8;
    };
} AON_FAST_LOP_PON_RG3X_TYPE;

/* 0x808
    0       R/W LOP_PON_LDOAUX2_EN_HI_IQ                        1'b0
    2:1     R/W LOP_PON_LDOAUX1_SEL_BIAS                        2'b00
    3       R/W LOP_PON_LDOAUX1_POW_BIAS                        1'b1
    4       R/W LOP_PON_LDOAUX1_EN_LO_IQ2                       1'b0
    5       R/W LOP_PON_LDOAUX1_EN_HI_IQ                        1'b0
    6       R/W LOP_PON_CHG_POW_M2                              1'b1
    7       R/W LOP_PON_CHG_POW_M1                              1'b1
    15:8    R/W LOP_PON_LDOUSB_TUNE_LDO_VOUT                    8'b00100101
 */
typedef union _AON_FAST_LOP_PON_RG4X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_LDOAUX2_EN_HI_IQ: 1;
        uint16_t LOP_PON_LDOAUX1_SEL_BIAS: 2;
        uint16_t LOP_PON_LDOAUX1_POW_BIAS: 1;
        uint16_t LOP_PON_LDOAUX1_EN_LO_IQ2: 1;
        uint16_t LOP_PON_LDOAUX1_EN_HI_IQ: 1;
        uint16_t LOP_PON_CHG_POW_M2: 1;
        uint16_t LOP_PON_CHG_POW_M1: 1;
        uint16_t LOP_PON_LDOUSB_TUNE_LDO_VOUT: 8;
    };
} AON_FAST_LOP_PON_RG4X_TYPE;

/* 0x80A
    0       R/W LOP_PON_LDOAUX1_EN_VO_PD                        1'b0
    2:1     R/W LOP_PON_LDOUSB_SEL_BIAS                         2'b00
    3       R/W LOP_PON_LDOUSB_POW_BIAS                         1'b1
    4       R/W LOP_PON_LDOUSB_EN_LO_IQ2                        1'b0
    5       R/W LOP_PON_LDOUSB_EN_HI_IQ                         1'b0
    6       R/W LOP_PON_LDOSYS_EN_VREF_LDO533                   1'b1
    7       R/W LOP_PON_LDOSYS_EN_VO_PD                         1'b0
    8       R/W LOP_PON_LDOSYS_POS_EXT_RST_B                    1'b1
    9       R/W LOP_PON_LDOAUX3_EN_VREF                         1'b1
    10      R/W LOP_PON_LDOAUX3_EN_VO_PD                        1'b0
    11      R/W LOP_PON_LDOAUX3_POS_EXT_RST_B                   1'b1
    13:12   R/W LOP_PON_LDOAUX2_SEL_BIAS                        2'b00
    14      R/W LOP_PON_LDOAUX2_POW_BIAS                        1'b1
    15      R/W LOP_PON_LDOAUX2_EN_LO_IQ2                       1'b0
 */
typedef union _AON_FAST_LOP_PON_RG5X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_LDOAUX1_EN_VO_PD: 1;
        uint16_t LOP_PON_LDOUSB_SEL_BIAS: 2;
        uint16_t LOP_PON_LDOUSB_POW_BIAS: 1;
        uint16_t LOP_PON_LDOUSB_EN_LO_IQ2: 1;
        uint16_t LOP_PON_LDOUSB_EN_HI_IQ: 1;
        uint16_t LOP_PON_LDOSYS_EN_VREF_LDO533: 1;
        uint16_t LOP_PON_LDOSYS_EN_VO_PD: 1;
        uint16_t LOP_PON_LDOSYS_POS_EXT_RST_B: 1;
        uint16_t LOP_PON_LDOAUX3_EN_VREF: 1;
        uint16_t LOP_PON_LDOAUX3_EN_VO_PD: 1;
        uint16_t LOP_PON_LDOAUX3_POS_EXT_RST_B: 1;
        uint16_t LOP_PON_LDOAUX2_SEL_BIAS: 2;
        uint16_t LOP_PON_LDOAUX2_POW_BIAS: 1;
        uint16_t LOP_PON_LDOAUX2_EN_LO_IQ2: 1;
    };
} AON_FAST_LOP_PON_RG5X_TYPE;

/* 0x80C
    7:0     R/W LOP_PON_LDOAUX3_TUNE_LDOHQ1_VOUT                8'b00100101
    8       R/W LOP_PON_LDOAUX3_POS_EN_POS                      1'b1
    9       R/W LOP_PON_LDOAUX2_POW_LDO                         1'b1
    10      R/W LOP_PON_LDOAUX1_POW_LDO                         1'b1
    11      R/W LOP_PON_CHG_EN_M2FON1K                          1'b1
    12      R/W LOP_PON_LDOSYS_POW_LDO533                       1'b1
    13      R/W LOP_PON_LDOAUX3_POW_LDOHQ1                      1'b1
    14      R/W LOP_PON_LDOUSB_EN_VO_PD                         1'b0
    15      R/W LOP_PON_LDOAUX2_EN_VO_PD                        1'b0
 */
typedef union _AON_FAST_LOP_PON_RG6X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_LDOAUX3_TUNE_LDOHQ1_VOUT: 8;
        uint16_t LOP_PON_LDOAUX3_POS_EN_POS: 1;
        uint16_t LOP_PON_LDOAUX2_POW_LDO: 1;
        uint16_t LOP_PON_LDOAUX1_POW_LDO: 1;
        uint16_t LOP_PON_CHG_EN_M2FON1K: 1;
        uint16_t LOP_PON_LDOSYS_POW_LDO533: 1;
        uint16_t LOP_PON_LDOAUX3_POW_LDOHQ1: 1;
        uint16_t LOP_PON_LDOUSB_EN_VO_PD: 1;
        uint16_t LOP_PON_LDOAUX2_EN_VO_PD: 1;
    };
} AON_FAST_LOP_PON_RG6X_TYPE;

/* 0x80E
    7:0     R/W LOP_PON_LDOSYS_TUNE_LDO533_VOUT                 8'b10111100
    8       R/W LOP_PON_LDOSYS_POS_EN_POS                       1'b1
    15:9    R/W LOP_PON_LDOAUX3_LQ_TUNE_LDO_VOUT                7'b0011010
 */
typedef union _AON_FAST_LOP_PON_RG7X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_LDOSYS_TUNE_LDO533_VOUT: 8;
        uint16_t LOP_PON_LDOSYS_POS_EN_POS: 1;
        uint16_t LOP_PON_LDOAUX3_LQ_TUNE_LDO_VOUT: 7;
    };
} AON_FAST_LOP_PON_RG7X_TYPE;

/* 0x810
    5:0     R/W LOP_PON_RG8X_DUMMY1                             6'h0
    6       R/W LOP_PON_EXTRN_LDO2_POW_LDO                      1'b1
    7       R/W LOP_PON_EXTRN_LDO1_POW_LDO                      1'b1
    8       R/W LOP_PON_LDOUSB_POW_LDO                          1'b1
    15:9    R/W LOP_PON_LDOSYS_TUNE_LDO733_VOUT                 7'b1010110
 */
typedef union _AON_FAST_LOP_PON_RG8X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_RG8X_DUMMY1: 6;
        uint16_t LOP_PON_EXTRN_LDO2_POW_LDO: 1;
        uint16_t LOP_PON_EXTRN_LDO1_POW_LDO: 1;
        uint16_t LOP_PON_LDOUSB_POW_LDO: 1;
        uint16_t LOP_PON_LDOSYS_TUNE_LDO733_VOUT: 7;
    };
} AON_FAST_LOP_PON_RG8X_TYPE;

/* 0x812
    1:0     R/W LOP_PON_RG9X_DUMMY1                             2'h0
    8:2     R/W LOP_PON_LDO_PA_TUNE_LDO_VOUT                    7'b1010110
    15:9    R/W LOP_PON_CODEC_LDO_TUNE_LDO_DRV_VOUT             7'b1010110
 */
typedef union _AON_FAST_LOP_PON_RG9X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_RG9X_DUMMY1: 2;
        uint16_t LOP_PON_LDO_PA_TUNE_LDO_VOUT: 7;
        uint16_t LOP_PON_CODEC_LDO_TUNE_LDO_DRV_VOUT: 7;
    };
} AON_FAST_LOP_PON_RG9X_TYPE;

/* 0x814
    0       R/W LOP_PON_SWR_AUDIO_ZCDQ_RST_B                    1'b1
    1       R/W LOP_PON_SWR_CORE_ZCDQ_RST_B                     1'b1
    2       R/W LOP_PON_LDOAUX3_LQ_POW_LDO                      1'b0
    3       R/W LOP_PON_LDOEXT_EN_VREF                          1'b1
    4       R/W LOP_PON_LDOEXT_EN_VO_PD                         1'b0
    5       R/W LOP_PON_LDOEXT_POS_EXT_RST_B                    1'b1
    6       R/W LOP_PON_LDO_DIG_EN_VO_PD                        1'b0
    7       R/W LOP_PON_CODEC_LDO_EN_LDO_DRV_VO_PD              1'b0
    8       R/W LOP_PON_LDO_PA_EN_VO_PD                         1'b0
    15:9    R/W LOP_PON_LDO_DIG_TUNE_LDO_VOUT                   7'b0110010
 */
typedef union _AON_FAST_LOP_PON_RG10X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_SWR_AUDIO_ZCDQ_RST_B: 1;
        uint16_t LOP_PON_SWR_CORE_ZCDQ_RST_B: 1;
        uint16_t LOP_PON_LDOAUX3_LQ_POW_LDO: 1;
        uint16_t LOP_PON_LDOEXT_EN_VREF: 1;
        uint16_t LOP_PON_LDOEXT_EN_VO_PD: 1;
        uint16_t LOP_PON_LDOEXT_POS_EXT_RST_B: 1;
        uint16_t LOP_PON_LDO_DIG_EN_VO_PD: 1;
        uint16_t LOP_PON_CODEC_LDO_EN_LDO_DRV_VO_PD: 1;
        uint16_t LOP_PON_LDO_PA_EN_VO_PD: 1;
        uint16_t LOP_PON_LDO_DIG_TUNE_LDO_VOUT: 7;
    };
} AON_FAST_LOP_PON_RG10X_TYPE;

/* 0x816
    0       R/W LOP_PON_SWR_CORE_FPWM1                          1'b0
    1       R/W LOP_PON_SWR_CORE_EN_SAW_RAMP2                   1'b1
    2       R/W LOP_PON_SWR_CORE_EN_SAW_RAMP1                   1'b1
    3       R/W LOP_PON_SWR_CORE_EN_OCP                         1'b1
    4       R/W LOP_PON_SWR_AUDIO_POW_ZCD                       1'b1
    5       R/W LOP_PON_SWR_AUDIO_SEL_CK_CTRL                   1'b0
    6       R/W LOP_PON_SWR_AUDIO_POW_VDIV                      1'b1
    7       R/W LOP_PON_SWR_AUDIO_SEL_POS_VREFLPPFM             1'b0
    8       R/W LOP_PON_SWR_AUDIO_POW_DIVPFM                    1'b0
    9       R/W LOP_PON_SWR_AUDIO_POW_POS_REF                   1'b1
    10      R/W LOP_PON_SWR_AUDIO_POW_PFM                       1'b1
    11      R/W LOP_PON_SWR_AUDIO_POW_IMIR                      1'b1
    12      R/W LOP_PON_SWR_AUDIO_SEL_CK_CTRL_NEW               1'b0
    13      R/W LOP_PON_SWR_AUDIO_SEL_PFM_COT                   1'b0
    14      R/W LOP_PON_SWR_AUDIO_EN_OCP                        1'b1
    15      R/W LOP_PON_LDOSYS_EN_733TO533_PC                   1'b1
 */
typedef union _AON_FAST_LOP_PON_RG11X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_SWR_CORE_FPWM1: 1;
        uint16_t LOP_PON_SWR_CORE_EN_SAW_RAMP2: 1;
        uint16_t LOP_PON_SWR_CORE_EN_SAW_RAMP1: 1;
        uint16_t LOP_PON_SWR_CORE_EN_OCP: 1;
        uint16_t LOP_PON_SWR_AUDIO_POW_ZCD: 1;
        uint16_t LOP_PON_SWR_AUDIO_SEL_CK_CTRL: 1;
        uint16_t LOP_PON_SWR_AUDIO_POW_VDIV: 1;
        uint16_t LOP_PON_SWR_AUDIO_SEL_POS_VREFLPPFM: 1;
        uint16_t LOP_PON_SWR_AUDIO_POW_DIVPFM: 1;
        uint16_t LOP_PON_SWR_AUDIO_POW_POS_REF: 1;
        uint16_t LOP_PON_SWR_AUDIO_POW_PFM: 1;
        uint16_t LOP_PON_SWR_AUDIO_POW_IMIR: 1;
        uint16_t LOP_PON_SWR_AUDIO_SEL_CK_CTRL_NEW: 1;
        uint16_t LOP_PON_SWR_AUDIO_SEL_PFM_COT: 1;
        uint16_t LOP_PON_SWR_AUDIO_EN_OCP: 1;
        uint16_t LOP_PON_LDOSYS_EN_733TO533_PC: 1;
    };
} AON_FAST_LOP_PON_RG11X_TYPE;

/* 0x818
    0       R/W LOP_PON_LDO_PA_POW_VREF                         1'b1
    1       R/W LOP_PON_CODEC_LDO_POW_LDO_DRV_VREF              1'b1
    2       R/W LOP_PON_SWR_CORE_EN_PFM_TDM                     1'b0
    3       R/W LOP_PON_SWR_CORE_POW_ZCD                        1'b1
    4       R/W LOP_PON_SWR_CORE_POW_ZCD_COMP_LOWIQ             1'b0
    5       R/W LOP_PON_SWR_CORE_POW_VDIV2                      1'b1
    6       R/W LOP_PON_SWR_CORE_POW_VDIV1                      1'b1
    7       R/W LOP_PON_SWR_CORE_POW_SAW_IB                     1'b1
    8       R/W LOP_PON_SWR_CORE_POW_SAW                        1'b1
    9       R/W LOP_PON_SWR_CORE_POW_REF                        1'b1
    10      R/W LOP_PON_SWR_CORE_POW_PWM2                       1'b0
    11      R/W LOP_PON_SWR_CORE_POW_PWM1                       1'b0
    12      R/W LOP_PON_SWR_CORE_POW_PFM2                       1'b1
    13      R/W LOP_PON_SWR_CORE_POW_PFM1                       1'b1
    14      R/W LOP_PON_SWR_CORE_POW_IMIR                       1'b1
    15      R/W LOP_PON_SWR_CORE_FPWM2                          1'b0
 */
typedef union _AON_FAST_LOP_PON_RG12X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_LDO_PA_POW_VREF: 1;
        uint16_t LOP_PON_CODEC_LDO_POW_LDO_DRV_VREF: 1;
        uint16_t LOP_PON_SWR_CORE_EN_PFM_TDM: 1;
        uint16_t LOP_PON_SWR_CORE_POW_ZCD: 1;
        uint16_t LOP_PON_SWR_CORE_POW_ZCD_COMP_LOWIQ: 1;
        uint16_t LOP_PON_SWR_CORE_POW_VDIV2: 1;
        uint16_t LOP_PON_SWR_CORE_POW_VDIV1: 1;
        uint16_t LOP_PON_SWR_CORE_POW_SAW_IB: 1;
        uint16_t LOP_PON_SWR_CORE_POW_SAW: 1;
        uint16_t LOP_PON_SWR_CORE_POW_REF: 1;
        uint16_t LOP_PON_SWR_CORE_POW_PWM2: 1;
        uint16_t LOP_PON_SWR_CORE_POW_PWM1: 1;
        uint16_t LOP_PON_SWR_CORE_POW_PFM2: 1;
        uint16_t LOP_PON_SWR_CORE_POW_PFM1: 1;
        uint16_t LOP_PON_SWR_CORE_POW_IMIR: 1;
        uint16_t LOP_PON_SWR_CORE_FPWM2: 1;
    };
} AON_FAST_LOP_PON_RG12X_TYPE;

/* 0x81A
    0       R/W LOP_PON_RG13X_DUMMY1                            1'h0
    1       R/W LOP_PON_SWR_AUDIO_PFMCCMPOS_EXT_RST_B           1'b1
    2       R/W LOP_PON_SWR_AUDIO_EN_ZCD_LOW_IQ                 1'b0
    3       R/W LOP_PON_SWR_AUDIO_ZCDQ_FORCE_CODE1              1'b0
    4       R/W LOP_PON_SWR_CORE_ZCDQ_FORCE_CODE2               1'b0
    5       R/W LOP_PON_SWR_CORE_ZCDQ_FORCE_CODE1               1'b0
    6       R/W LOP_PON_LDOEXT_POS_EN_POS                       1'b1
    7       R/W LOP_PON_LDOEXT_EN_PC                            1'b0
    8       R/W LOP_PON_LDOEXT_POW_LDOHQ1                       1'b1
    9       R/W LOP_PON_SWR_CORE_PFM1POS_EXT_RST_B              1'b1
    10      R/W LOP_PON_SWR_CORE_EN_VDDCORE_PD                  1'b0
    11      R/W LOP_PON_SWR_CORE_EN_VD105_RF_PD                 1'b0
    12      R/W LOP_PON_SWR_AUDIO_EN_VO_PD                      1'b0
    13      R/W LOP_PON_SWR_CORE_PWM1POS_EXT_RST_B              1'b1
    14      R/W LOP_PON_SWR_AUDIO_PFMPOS_EXT_RST_B              1'b1
    15      R/W LOP_PON_LDO_DIG_POW_VREF                        1'b1
 */
typedef union _AON_FAST_LOP_PON_RG13X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_RG13X_DUMMY1: 1;
        uint16_t LOP_PON_SWR_AUDIO_PFMCCMPOS_EXT_RST_B: 1;
        uint16_t LOP_PON_SWR_AUDIO_EN_ZCD_LOW_IQ: 1;
        uint16_t LOP_PON_SWR_AUDIO_ZCDQ_FORCE_CODE1: 1;
        uint16_t LOP_PON_SWR_CORE_ZCDQ_FORCE_CODE2: 1;
        uint16_t LOP_PON_SWR_CORE_ZCDQ_FORCE_CODE1: 1;
        uint16_t LOP_PON_LDOEXT_POS_EN_POS: 1;
        uint16_t LOP_PON_LDOEXT_EN_PC: 1;
        uint16_t LOP_PON_LDOEXT_POW_LDOHQ1: 1;
        uint16_t LOP_PON_SWR_CORE_PFM1POS_EXT_RST_B: 1;
        uint16_t LOP_PON_SWR_CORE_EN_VDDCORE_PD: 1;
        uint16_t LOP_PON_SWR_CORE_EN_VD105_RF_PD: 1;
        uint16_t LOP_PON_SWR_AUDIO_EN_VO_PD: 1;
        uint16_t LOP_PON_SWR_CORE_PWM1POS_EXT_RST_B: 1;
        uint16_t LOP_PON_SWR_AUDIO_PFMPOS_EXT_RST_B: 1;
        uint16_t LOP_PON_LDO_DIG_POW_VREF: 1;
    };
} AON_FAST_LOP_PON_RG13X_TYPE;

/* 0x81C
    0       R/W LOP_PON_CODEC_LDO_EN_LDO_DRV_PC                 1'b0
    7:1     R/W LOP_PON_FSM_DUMMY1                              7'b0011010
    15:8    R/W LOP_PON_LDOEXT_TUNE_LDOHQ1_VOUT                 8'b01000000
 */
typedef union _AON_FAST_LOP_PON_RG14X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_CODEC_LDO_EN_LDO_DRV_PC: 1;
        uint16_t LOP_PON_FSM_DUMMY1: 7;
        uint16_t LOP_PON_LDOEXT_TUNE_LDOHQ1_VOUT: 8;
    };
} AON_FAST_LOP_PON_RG14X_TYPE;

/* 0x81E
    3:0     R/W LOP_PON_RG15X_DUMMY1                            4'h0
    4       R/W LOP_PON_SWR_AUDIO_POW_SWR                       1'b1
    5       R/W LOP_PON_SWR_AUDIO_POW_CCMPFM                    1'b0
    6       R/W LOP_PON_SWR_AUDIO_PFMPOS_EN_POS                 1'b1
    11:7    R/W LOP_PON_MBIAS_TUNE_LDOAUDIO_LQ_VOUT             5'b10000
    12      R/W LOP_PON_SWR_AUDIO_ZCDQ_POW_UD_DIG               1'b1
    13      R/W LOP_PON_LDO_PA_POW_LDO                          1'b1
    14      R/W LOP_PON_LDO_PA_EN_PC                            1'b0
    15      R/W LOP_PON_CODEC_LDO_POW_LDO_DRV                   1'b1
 */
typedef union _AON_FAST_LOP_PON_RG15X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_RG15X_DUMMY1: 4;
        uint16_t LOP_PON_SWR_AUDIO_POW_SWR: 1;
        uint16_t LOP_PON_SWR_AUDIO_POW_CCMPFM: 1;
        uint16_t LOP_PON_SWR_AUDIO_PFMPOS_EN_POS: 1;
        uint16_t LOP_PON_MBIAS_TUNE_LDOAUDIO_LQ_VOUT: 5;
        uint16_t LOP_PON_SWR_AUDIO_ZCDQ_POW_UD_DIG: 1;
        uint16_t LOP_PON_LDO_PA_POW_LDO: 1;
        uint16_t LOP_PON_LDO_PA_EN_PC: 1;
        uint16_t LOP_PON_CODEC_LDO_POW_LDO_DRV: 1;
    };
} AON_FAST_LOP_PON_RG15X_TYPE;

/* 0x820
    1:0     R/W LOP_PON_RG16X_DUMMY1                            2'h0
    5:2     R/W LOP_PON_MBIAS_SEL_VR_LPPFM1                     4'b1000
    6       R/W LOP_PON_SWR_CORE_SEL_POS_VREFLPPFM2             1'b0
    7       R/W LOP_PON_SWR_CORE_SEL_POS_VREFLPPFM1             1'b0
    15:8    R/W LOP_PON_SWR_AUDIO_TUNE_VDIV                     8'b10011110
 */
typedef union _AON_FAST_LOP_PON_RG16X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_RG16X_DUMMY1: 2;
        uint16_t LOP_PON_MBIAS_SEL_VR_LPPFM1: 4;
        uint16_t LOP_PON_SWR_CORE_SEL_POS_VREFLPPFM2: 1;
        uint16_t LOP_PON_SWR_CORE_SEL_POS_VREFLPPFM1: 1;
        uint16_t LOP_PON_SWR_AUDIO_TUNE_VDIV: 8;
    };
} AON_FAST_LOP_PON_RG16X_TYPE;

/* 0x822
    0       R/W LOP_PON_SWR_CORE_PWM1POS_EN_POS                 1'b1
    1       R/W LOP_PON_SWR_CORE_EN_SWR_OUT2                    1'b1
    2       R/W LOP_PON_SWR_CORE_EN_SWR_OUT1                    1'b1
    3       R/W LOP_PON_XTAL_GATED_DIGIN                        1'b0
    4       R/W LOP_PON_XTAL_GATED_DIGIP                        1'b0
    5       R/W LOP_PON_XTAL_GATED_DIGI_LP                      1'b0
    6       R/W LOP_PON_XTAL_GATED_LPS                          1'b0
    7       R/W LOP_PON_XTAL_GATED_RFN                          1'b0
    8       R/W LOP_PON_XTAL_GATED_RFP                          1'b0
    9       R/W LOP_PON_LDO_DIG_POW_LDO                         1'b1
    10      R/W LOP_PON_SWR_CORE_ZCDQ_POW_UD_DIG                1'b1
    11      R/W LOP_PON_LDO_DIG_EN_PC                           1'b0
    15:12   R/W LOP_PON_MBIAS_SEL_VR_LPPFM2                     4'b1000
 */
typedef union _AON_FAST_LOP_PON_RG17X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_SWR_CORE_PWM1POS_EN_POS: 1;
        uint16_t LOP_PON_SWR_CORE_EN_SWR_OUT2: 1;
        uint16_t LOP_PON_SWR_CORE_EN_SWR_OUT1: 1;
        uint16_t LOP_PON_XTAL_GATED_DIGIN: 1;
        uint16_t LOP_PON_XTAL_GATED_DIGIP: 1;
        uint16_t LOP_PON_XTAL_GATED_DIGI_LP: 1;
        uint16_t LOP_PON_XTAL_GATED_LPS: 1;
        uint16_t LOP_PON_XTAL_GATED_RFN: 1;
        uint16_t LOP_PON_XTAL_GATED_RFP: 1;
        uint16_t LOP_PON_LDO_DIG_POW_LDO: 1;
        uint16_t LOP_PON_SWR_CORE_ZCDQ_POW_UD_DIG: 1;
        uint16_t LOP_PON_LDO_DIG_EN_PC: 1;
        uint16_t LOP_PON_MBIAS_SEL_VR_LPPFM2: 4;
    };
} AON_FAST_LOP_PON_RG17X_TYPE;

/* 0x824
    4:0     R/W LOP_PON_RG18X_DUMMY1                            5'h0
    12:5    R/W LOP_PON_SWR_CORE_TUNE_VDIV1                     8'b10001010
    13      R/W LOP_PON_SWR_CORE_POW_SWR                        1'b1
    14      R/W LOP_PON_SWR_CORE_POW_LDO                        1'b0
    15      R/W LOP_PON_SWR_CORE_PFM1POS_EN_POS                 1'b1
 */
typedef union _AON_FAST_LOP_PON_RG18X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_RG18X_DUMMY1: 5;
        uint16_t LOP_PON_SWR_CORE_TUNE_VDIV1: 8;
        uint16_t LOP_PON_SWR_CORE_POW_SWR: 1;
        uint16_t LOP_PON_SWR_CORE_POW_LDO: 1;
        uint16_t LOP_PON_SWR_CORE_PFM1POS_EN_POS: 1;
    };
} AON_FAST_LOP_PON_RG18X_TYPE;

/* 0x826
    0       R/W LOP_PON_MBIAS_POW_PCUT_VPON_TO_DVDD             1'b0
    1       R/W LOP_PON_MBIAS_POW_PCUT_VD105_RF_TO_DVDD         1'b0
    2       R/W LOP_PON_SWR_CORE_EN_SWR_SHORT                   1'b0
    3       R/W LOP_PON_SWR_CORE_SEL_FOLLOWCTRL1                1'b1
    4       R/W LOP_PON_XTAL_EN_SUPPLY_MODE                     1'b1
    5       R/W LOP_PON_MBIAS_POW_LDOAUDIO_LQ                   1'b0
    6       R/W LOP_PON_SWR_CORE_SEL_CK_CTRL_PFM                1'b1
    7       R/W LOP_PON_EXTRN_SWR_POW_SWR                       1'b1
    15:8    R/W LOP_PON_SWR_CORE_TUNE_VDIV2                     8'b01010100
 */
typedef union _AON_FAST_LOP_PON_RG19X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_MBIAS_POW_PCUT_VPON_TO_DVDD: 1;
        uint16_t LOP_PON_MBIAS_POW_PCUT_VD105_RF_TO_DVDD: 1;
        uint16_t LOP_PON_SWR_CORE_EN_SWR_SHORT: 1;
        uint16_t LOP_PON_SWR_CORE_SEL_FOLLOWCTRL1: 1;
        uint16_t LOP_PON_XTAL_EN_SUPPLY_MODE: 1;
        uint16_t LOP_PON_MBIAS_POW_LDOAUDIO_LQ: 1;
        uint16_t LOP_PON_SWR_CORE_SEL_CK_CTRL_PFM: 1;
        uint16_t LOP_PON_EXTRN_SWR_POW_SWR: 1;
        uint16_t LOP_PON_SWR_CORE_TUNE_VDIV2: 8;
    };
} AON_FAST_LOP_PON_RG19X_TYPE;

/* 0x828
    0       R/W LOP_PON_XTAL_POW_XTAL                           1'b1
    1       R/W LOP_PON_VCORE_PC_POW_VCORE6_PC_VG2              1'b0
    2       R/W LOP_PON_VCORE_PC_POW_VCORE6_PC_VG1              1'b0
    3       R/W LOP_PON_VCORE_PC_POW_VCORE5_PC_VG2              1'b0
    4       R/W LOP_PON_VCORE_PC_POW_VCORE5_PC_VG1              1'b0
    5       R/W LOP_PON_VCORE_PC_POW_VCORE4_PC_VG2              1'b0
    6       R/W LOP_PON_VCORE_PC_POW_VCORE4_PC_VG1              1'b0
    7       R/W LOP_PON_VCORE_PC_POW_VCORE3_PC_VG2              1'b0
    8       R/W LOP_PON_VCORE_PC_POW_VCORE3_PC_VG1              1'b0
    9       R/W LOP_PON_XTAL_EN_LPMODE_AUTO_GATED               1'b0
    10      R/W LOP_PON_XTAL_EN_LPCNT_CLK_DIV                   1'b0
    11      R/W LOP_PON_VCORE_PC_POW_VCORE1_PC_VG2              1'b0
    12      R/W LOP_PON_VCORE_PC_POW_VCORE1_PC_VG1              1'b0
    13      R/W LOP_PON_SWR_CORE_POW_CCMPFM                     1'b0
    14      R/W LOP_PON_BTPLL_LDO2_POW_LDO                      1'b1
    15      R/W LOP_PON_BTPLL_LDO1_POW_LDO                      1'b1
 */
typedef union _AON_FAST_LOP_PON_RG20X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_XTAL_POW_XTAL: 1;
        uint16_t LOP_PON_VCORE_PC_POW_VCORE6_PC_VG2: 1;
        uint16_t LOP_PON_VCORE_PC_POW_VCORE6_PC_VG1: 1;
        uint16_t LOP_PON_VCORE_PC_POW_VCORE5_PC_VG2: 1;
        uint16_t LOP_PON_VCORE_PC_POW_VCORE5_PC_VG1: 1;
        uint16_t LOP_PON_VCORE_PC_POW_VCORE4_PC_VG2: 1;
        uint16_t LOP_PON_VCORE_PC_POW_VCORE4_PC_VG1: 1;
        uint16_t LOP_PON_VCORE_PC_POW_VCORE3_PC_VG2: 1;
        uint16_t LOP_PON_VCORE_PC_POW_VCORE3_PC_VG1: 1;
        uint16_t LOP_PON_XTAL_EN_LPMODE_AUTO_GATED: 1;
        uint16_t LOP_PON_XTAL_EN_LPCNT_CLK_DIV: 1;
        uint16_t LOP_PON_VCORE_PC_POW_VCORE1_PC_VG2: 1;
        uint16_t LOP_PON_VCORE_PC_POW_VCORE1_PC_VG1: 1;
        uint16_t LOP_PON_SWR_CORE_POW_CCMPFM: 1;
        uint16_t LOP_PON_BTPLL_LDO2_POW_LDO: 1;
        uint16_t LOP_PON_BTPLL_LDO1_POW_LDO: 1;
    };
} AON_FAST_LOP_PON_RG20X_TYPE;

/* 0x82A
    0       R/W LOP_PON_BLE_STORE                               1'b0
    1       R/W LOP_PON_RFC_RESTORE                             1'b0
    2       R/W LOP_PON_PF_RESTORE                              1'b0
    3       R/W LOP_PON_MODEM_RESTORE                           1'b0
    4       R/W LOP_PON_DP_MODEM_RESTORE                        1'b0
    5       R/W LOP_PON_BZ_RESTORE                              1'b0
    6       R/W LOP_PON_BLE_RESTORE                             1'b0
    9:7     R/W LOP_PON_XTAL_SEL_MODE                           3'b100
    10      R/W LOP_PON_ISO_XTAL                                1'b0
    11      R/W LOP_PON_ISO_PLL4                                1'b0
    12      R/W LOP_PON_ISO_PLL3                                1'b0
    13      R/W LOP_PON_ISO_PLL2                                1'b0
    14      R/W LOP_PON_ISO_PLL1                                1'b0
    15      R/W LOP_PON_OSC40_POW_OSC                           1'b1
 */
typedef union _AON_FAST_LOP_PON_RG21X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_BLE_STORE: 1;
        uint16_t LOP_PON_RFC_RESTORE: 1;
        uint16_t LOP_PON_PF_RESTORE: 1;
        uint16_t LOP_PON_MODEM_RESTORE: 1;
        uint16_t LOP_PON_DP_MODEM_RESTORE: 1;
        uint16_t LOP_PON_BZ_RESTORE: 1;
        uint16_t LOP_PON_BLE_RESTORE: 1;
        uint16_t LOP_PON_XTAL_SEL_MODE: 3;
        uint16_t LOP_PON_ISO_XTAL: 1;
        uint16_t LOP_PON_ISO_PLL4: 1;
        uint16_t LOP_PON_ISO_PLL3: 1;
        uint16_t LOP_PON_ISO_PLL2: 1;
        uint16_t LOP_PON_ISO_PLL1: 1;
        uint16_t LOP_PON_OSC40_POW_OSC: 1;
    };
} AON_FAST_LOP_PON_RG21X_TYPE;

/* 0x82C
    0       R/W LOP_PON_ISO_BT_CORE2                            1'b0
    1       R/W LOP_PON_ISO_BT_CORE1                            1'b0
    2       R/W LOP_PON_BT_RET_RSTB                             1'b1
    3       R/W LOP_PON_BTPLL1_POW_PLL                          1'b1
    4       R/W LOP_PON_BTPLL1_POW_DIV_PLL_CLK                  1'b1
    5       R/W LOP_PON_BTPLL2_POW_PLL                          1'b1
    6       R/W LOP_PON_BTPLL2_POW_DIV_PLL_CLK                  1'b1
    7       R/W LOP_PON_BTPLL3_POW_PLL                          1'b1
    8       R/W LOP_PON_BTPLL3_POW_DIV_PLL_CLK                  1'b1
    9       R/W LOP_PON_BTPLL4_POW_PLL                          1'b1
    10      R/W LOP_PON_BTPLL4_POW_DIV_PLL_CLK                  1'b1
    11      R/W LOP_PON_RFC_STORE                               1'b0
    12      R/W LOP_PON_PF_STORE                                1'b0
    13      R/W LOP_PON_MODEM_STORE                             1'b0
    14      R/W LOP_PON_DP_MODEM_STORE                          1'b0
    15      R/W LOP_PON_BZ_STORE                                1'b0
 */
typedef union _AON_FAST_LOP_PON_RG22X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_ISO_BT_CORE2: 1;
        uint16_t LOP_PON_ISO_BT_CORE1: 1;
        uint16_t LOP_PON_BT_RET_RSTB: 1;
        uint16_t LOP_PON_BTPLL1_POW_PLL: 1;
        uint16_t LOP_PON_BTPLL1_POW_DIV_PLL_CLK: 1;
        uint16_t LOP_PON_BTPLL2_POW_PLL: 1;
        uint16_t LOP_PON_BTPLL2_POW_DIV_PLL_CLK: 1;
        uint16_t LOP_PON_BTPLL3_POW_PLL: 1;
        uint16_t LOP_PON_BTPLL3_POW_DIV_PLL_CLK: 1;
        uint16_t LOP_PON_BTPLL4_POW_PLL: 1;
        uint16_t LOP_PON_BTPLL4_POW_DIV_PLL_CLK: 1;
        uint16_t LOP_PON_RFC_STORE: 1;
        uint16_t LOP_PON_PF_STORE: 1;
        uint16_t LOP_PON_MODEM_STORE: 1;
        uint16_t LOP_PON_DP_MODEM_STORE: 1;
        uint16_t LOP_PON_BZ_STORE: 1;
    };
} AON_FAST_LOP_PON_RG22X_TYPE;

/* 0x82E
    0       R/W LOP_PON_RG23X_DUMMY1                            1'h0
    1       R/W LOP_PON_SWR_AUDIO_PFMCCMPOS_EN_POS              1'b1
    2       R/W LOP_PON_SWR_CORE_PFMCCM1POS_EN_POS              1'b1
    3       R/W LOP_PON_SWR_CORE_PFMCCM1POS_EXT_RST_B           1'b1
    4       R/W LOP_PON_BT_CORE6_RSTB                           1'b1
    5       R/W LOP_PON_BT_CORE5_RSTB                           1'b1
    6       R/W LOP_PON_BT_CORE4_RSTB                           1'b1
    7       R/W LOP_PON_BT_CORE3_RSTB                           1'b1
    8       R/W LOP_PON_BT_CORE2_RSTB                           1'b1
    9       R/W LOP_PON_BT_CORE1_RSTB                           1'b1
    10      R/W LOP_PON_BT_PON_RSTB                             1'b1
    11      R/W LOP_PON_ISO_BT_PON                              1'b0
    12      R/W LOP_PON_ISO_BT_CORE6                            1'b0
    13      R/W LOP_PON_ISO_BT_CORE5                            1'b0
    14      R/W LOP_PON_ISO_BT_CORE4                            1'b0
    15      R/W LOP_PON_ISO_BT_CORE3                            1'b0
 */
typedef union _AON_FAST_LOP_PON_RG23X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_RG23X_DUMMY1: 1;
        uint16_t LOP_PON_SWR_AUDIO_PFMCCMPOS_EN_POS: 1;
        uint16_t LOP_PON_SWR_CORE_PFMCCM1POS_EN_POS: 1;
        uint16_t LOP_PON_SWR_CORE_PFMCCM1POS_EXT_RST_B: 1;
        uint16_t LOP_PON_BT_CORE6_RSTB: 1;
        uint16_t LOP_PON_BT_CORE5_RSTB: 1;
        uint16_t LOP_PON_BT_CORE4_RSTB: 1;
        uint16_t LOP_PON_BT_CORE3_RSTB: 1;
        uint16_t LOP_PON_BT_CORE2_RSTB: 1;
        uint16_t LOP_PON_BT_CORE1_RSTB: 1;
        uint16_t LOP_PON_BT_PON_RSTB: 1;
        uint16_t LOP_PON_ISO_BT_PON: 1;
        uint16_t LOP_PON_ISO_BT_CORE6: 1;
        uint16_t LOP_PON_ISO_BT_CORE5: 1;
        uint16_t LOP_PON_ISO_BT_CORE4: 1;
        uint16_t LOP_PON_ISO_BT_CORE3: 1;
    };
} AON_FAST_LOP_PON_RG23X_TYPE;

/* 0x830
    7:0     R/W LOP_PON_M1M2_DELAY                              8'ha0
    15:8    R/W LOP_PON_BIAS_DELAY                              8'ha0
 */
typedef union _AON_FAST_LOP_PON_DELAY_RG0X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_M1M2_DELAY: 8;
        uint16_t LOP_PON_BIAS_DELAY: 8;
    };
} AON_FAST_LOP_PON_DELAY_RG0X_TYPE;

/* 0x832
    7:0     R/W LOP_PON_SWR_BIAS_DELAY                          8'ha0
    15:8    R/W LOP_PON_LDOHQ_DELAY                             8'ha0
 */
typedef union _AON_FAST_LOP_PON_DELAY_RG1X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_SWR_BIAS_DELAY: 8;
        uint16_t LOP_PON_LDOHQ_DELAY: 8;
    };
} AON_FAST_LOP_PON_DELAY_RG1X_TYPE;

/* 0x834
    7:0     R/W LOP_PON_SWR_CORE_DELAY                          8'ha0
    15:8    R/W LOP_PON_SWR_AUDIO_DELAY                         8'ha0
 */
typedef union _AON_FAST_LOP_PON_DELAY_RG2X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_SWR_CORE_DELAY: 8;
        uint16_t LOP_PON_SWR_AUDIO_DELAY: 8;
    };
} AON_FAST_LOP_PON_DELAY_RG2X_TYPE;

/* 0x836
    7:0     R/W LOP_PON_XTAL_MODE_CHNG_DELAY                    8'ha0
    15:8    R/W LOP_PON_XTAL_DELAY                              8'ha0
 */
typedef union _AON_FAST_LOP_PON_DELAY_RG3X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_XTAL_MODE_CHNG_DELAY: 8;
        uint16_t LOP_PON_XTAL_DELAY: 8;
    };
} AON_FAST_LOP_PON_DELAY_RG3X_TYPE;

/* 0x838
    7:0     R/W LOP_PON_RST_DELAY                               8'ha0
    15:8    R/W LOP_PON_PLL_DELAY                               8'ha0
 */
typedef union _AON_FAST_LOP_PON_DELAY_RG4X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_RST_DELAY: 8;
        uint16_t LOP_PON_PLL_DELAY: 8;
    };
} AON_FAST_LOP_PON_DELAY_RG4X_TYPE;

/* 0x83A
    7:0     R/W LOP_PON_LDOHQ2_DELAY                            8'ha0
    15:8    R/W LOP_PON_ISO_DELAY                               8'ha0
 */
typedef union _AON_FAST_LOP_PON_DELAY_RG5X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_LDOHQ2_DELAY: 8;
        uint16_t LOP_PON_ISO_DELAY: 8;
    };
} AON_FAST_LOP_PON_DELAY_RG5X_TYPE;

/* 0x83C
    7:0     R/W LOP_PON_RG6X_DUMMY1                             8'ha0
    15:8    R/W LOP_PON_RET_DELAY                               8'ha0
 */
typedef union _AON_FAST_LOP_PON_DELAY_RG6X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_PON_RG6X_DUMMY1: 8;
        uint16_t LOP_PON_RET_DELAY: 8;
    };
} AON_FAST_LOP_PON_DELAY_RG6X_TYPE;

/* 0x850
    0       R/W LOP_POF_RG0X_DUMMY1                             1'h0
    1       R/W LOP_POF_MBIAS_POW_VD105_RF_DET                  1'b0
    2       R/W LOP_POF_MBIAS_POW_VAUX3_DET                     1'b0
    3       R/W LOP_POF_MBIAS_POW_VAUX2_DET                     1'b0
    4       R/W LOP_POF_MBIAS_POW_VAUX1_DET                     1'b0
    5       R/W LOP_POF_MBIAS_POW_VAUDIO_DET                    1'b0
    6       R/W LOP_POF_XTAL_EN_LPS_CLK                         1'b0
    7       R/W LOP_POF_MBIAS_POW_HV33_HQ_DET                   1'b0
    8       R/W LOP_POF_BT_AON2_RSTB                            1'b0
    9       R/W LOP_POF_MBIAS_POW_HV_DET                        1'b0
    10      R/W LOP_POF_ISO_BT_AON2                             1'b1
    11      R/W LOP_POF_MBIAS_FSM_DUMMY1                        1'b0
    12      R/W LOP_POF_MBIAS_POW_BIAS_500NA                    1'b0
    13      R/W LOP_POF_MBIAS_POW_BIAS_10UA                     1'b0
    14      R/W LOP_POF_CHG_EN_HV_PD                            1'b0
    15      R/W LOP_POF_MBIAS_POW_PCUT_DVDD_TO_DVDD1            1'b1
 */
typedef union _AON_FAST_LOP_POF_RG0X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_RG0X_DUMMY1: 1;
        uint16_t LOP_POF_MBIAS_POW_VD105_RF_DET: 1;
        uint16_t LOP_POF_MBIAS_POW_VAUX3_DET: 1;
        uint16_t LOP_POF_MBIAS_POW_VAUX2_DET: 1;
        uint16_t LOP_POF_MBIAS_POW_VAUX1_DET: 1;
        uint16_t LOP_POF_MBIAS_POW_VAUDIO_DET: 1;
        uint16_t LOP_POF_XTAL_EN_LPS_CLK: 1;
        uint16_t LOP_POF_MBIAS_POW_HV33_HQ_DET: 1;
        uint16_t LOP_POF_BT_AON2_RSTB: 1;
        uint16_t LOP_POF_MBIAS_POW_HV_DET: 1;
        uint16_t LOP_POF_ISO_BT_AON2: 1;
        uint16_t LOP_POF_MBIAS_FSM_DUMMY1: 1;
        uint16_t LOP_POF_MBIAS_POW_BIAS_500NA: 1;
        uint16_t LOP_POF_MBIAS_POW_BIAS_10UA: 1;
        uint16_t LOP_POF_CHG_EN_HV_PD: 1;
        uint16_t LOP_POF_MBIAS_POW_PCUT_DVDD_TO_DVDD1: 1;
    };
} AON_FAST_LOP_POF_RG0X_TYPE;

/* 0x852
    0       R/W LOP_POF_XTAL_GATED_AFEN                         1'b1
    1       R/W LOP_POF_XTAL_GATED_AFEN_LP                      1'b1
    2       R/W LOP_POF_XTAL_GATED_AFEP                         1'b1
    3       R/W LOP_POF_XTAL_GATED_AFEP_LP                      1'b1
    6:4     R/W LOP_POF_MBIAS_TUNE_BIAS_50NA_IQ                 3'b000
    9:7     R/W LOP_POF_MBIAS_TUNE_BIAS_500NA_IQ                3'b000
    12:10   R/W LOP_POF_MBIAS_TUNE_BIAS_3NA_IQ                  3'b000
    15:13   R/W LOP_POF_MBIAS_TUNE_BIAS_10UA_IQ                 3'b000
 */
typedef union _AON_FAST_LOP_POF_RG1X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_XTAL_GATED_AFEN: 1;
        uint16_t LOP_POF_XTAL_GATED_AFEN_LP: 1;
        uint16_t LOP_POF_XTAL_GATED_AFEP: 1;
        uint16_t LOP_POF_XTAL_GATED_AFEP_LP: 1;
        uint16_t LOP_POF_MBIAS_TUNE_BIAS_50NA_IQ: 3;
        uint16_t LOP_POF_MBIAS_TUNE_BIAS_500NA_IQ: 3;
        uint16_t LOP_POF_MBIAS_TUNE_BIAS_3NA_IQ: 3;
        uint16_t LOP_POF_MBIAS_TUNE_BIAS_10UA_IQ: 3;
    };
} AON_FAST_LOP_POF_RG1X_TYPE;

/* 0x854
    5:0     R/W LOP_POF_RG2X_DUMMY1                             6'h0
    6       R/W LOP_POF_POW32K_32KXTAL33                        1'b0
    7       R/W LOP_POF_POW32K_32KOSC33                         1'b1
    9:8     R/W LOP_POF_MBIAS_TUNE_LPBG_IQ                      2'b00
    15:10   R/W LOP_POF_MBIAS_TUNE_LDO309_VOUT                  6'b010111
 */
typedef union _AON_FAST_LOP_POF_RG2X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_RG2X_DUMMY1: 6;
        uint16_t LOP_POF_POW32K_32KXTAL33: 1;
        uint16_t LOP_POF_POW32K_32KOSC33: 1;
        uint16_t LOP_POF_MBIAS_TUNE_LPBG_IQ: 2;
        uint16_t LOP_POF_MBIAS_TUNE_LDO309_VOUT: 6;
    };
} AON_FAST_LOP_POF_RG2X_TYPE;

/* 0x856
    7:0     R/W LOP_POF_LDOAUX2_TUNE_LDO_VOUT                   8'b00100101
    15:8    R/W LOP_POF_LDOAUX1_TUNE_LDO_VOUT                   8'b00100101
 */
typedef union _AON_FAST_LOP_POF_RG3X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_LDOAUX2_TUNE_LDO_VOUT: 8;
        uint16_t LOP_POF_LDOAUX1_TUNE_LDO_VOUT: 8;
    };
} AON_FAST_LOP_POF_RG3X_TYPE;

/* 0x858
    0       R/W LOP_POF_LDOAUX2_EN_HI_IQ                        1'b0
    2:1     R/W LOP_POF_LDOAUX1_SEL_BIAS                        2'b00
    3       R/W LOP_POF_LDOAUX1_POW_BIAS                        1'b1
    4       R/W LOP_POF_LDOAUX1_EN_LO_IQ2                       1'b0
    5       R/W LOP_POF_LDOAUX1_EN_HI_IQ                        1'b0
    6       R/W LOP_POF_CHG_POW_M2                              1'b0
    7       R/W LOP_POF_CHG_POW_M1                              1'b0
    15:8    R/W LOP_POF_LDOUSB_TUNE_LDO_VOUT                    8'b00100101
 */
typedef union _AON_FAST_LOP_POF_RG4X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_LDOAUX2_EN_HI_IQ: 1;
        uint16_t LOP_POF_LDOAUX1_SEL_BIAS: 2;
        uint16_t LOP_POF_LDOAUX1_POW_BIAS: 1;
        uint16_t LOP_POF_LDOAUX1_EN_LO_IQ2: 1;
        uint16_t LOP_POF_LDOAUX1_EN_HI_IQ: 1;
        uint16_t LOP_POF_CHG_POW_M2: 1;
        uint16_t LOP_POF_CHG_POW_M1: 1;
        uint16_t LOP_POF_LDOUSB_TUNE_LDO_VOUT: 8;
    };
} AON_FAST_LOP_POF_RG4X_TYPE;

/* 0x85A
    0       R/W LOP_POF_LDOAUX1_EN_VO_PD                        1'b0
    2:1     R/W LOP_POF_LDOUSB_SEL_BIAS                         2'b00
    3       R/W LOP_POF_LDOUSB_POW_BIAS                         1'b1
    4       R/W LOP_POF_LDOUSB_EN_LO_IQ2                        1'b0
    5       R/W LOP_POF_LDOUSB_EN_HI_IQ                         1'b0
    6       R/W LOP_POF_LDOSYS_EN_VREF_LDO533                   1'b0
    7       R/W LOP_POF_LDOSYS_EN_VO_PD                         1'b0
    8       R/W LOP_POF_LDOSYS_POS_EXT_RST_B                    1'b0
    9       R/W LOP_POF_LDOAUX3_EN_VREF                         1'b0
    10      R/W LOP_POF_LDOAUX3_EN_VO_PD                        1'b0
    11      R/W LOP_POF_LDOAUX3_POS_EXT_RST_B                   1'b1
    13:12   R/W LOP_POF_LDOAUX2_SEL_BIAS                        2'b00
    14      R/W LOP_POF_LDOAUX2_POW_BIAS                        1'b1
    15      R/W LOP_POF_LDOAUX2_EN_LO_IQ2                       1'b0
 */
typedef union _AON_FAST_LOP_POF_RG5X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_LDOAUX1_EN_VO_PD: 1;
        uint16_t LOP_POF_LDOUSB_SEL_BIAS: 2;
        uint16_t LOP_POF_LDOUSB_POW_BIAS: 1;
        uint16_t LOP_POF_LDOUSB_EN_LO_IQ2: 1;
        uint16_t LOP_POF_LDOUSB_EN_HI_IQ: 1;
        uint16_t LOP_POF_LDOSYS_EN_VREF_LDO533: 1;
        uint16_t LOP_POF_LDOSYS_EN_VO_PD: 1;
        uint16_t LOP_POF_LDOSYS_POS_EXT_RST_B: 1;
        uint16_t LOP_POF_LDOAUX3_EN_VREF: 1;
        uint16_t LOP_POF_LDOAUX3_EN_VO_PD: 1;
        uint16_t LOP_POF_LDOAUX3_POS_EXT_RST_B: 1;
        uint16_t LOP_POF_LDOAUX2_SEL_BIAS: 2;
        uint16_t LOP_POF_LDOAUX2_POW_BIAS: 1;
        uint16_t LOP_POF_LDOAUX2_EN_LO_IQ2: 1;
    };
} AON_FAST_LOP_POF_RG5X_TYPE;

/* 0x85C
    7:0     R/W LOP_POF_LDOAUX3_TUNE_LDOHQ1_VOUT                8'b00100101
    8       R/W LOP_POF_LDOAUX3_POS_EN_POS                      1'b0
    9       R/W LOP_POF_LDOAUX2_POW_LDO                         1'b1
    10      R/W LOP_POF_LDOAUX1_POW_LDO                         1'b1
    11      R/W LOP_POF_CHG_EN_M2FON1K                          1'b1
    12      R/W LOP_POF_LDOSYS_POW_LDO533                       1'b0
    13      R/W LOP_POF_LDOAUX3_POW_LDOHQ1                      1'b0
    14      R/W LOP_POF_LDOUSB_EN_VO_PD                         1'b0
    15      R/W LOP_POF_LDOAUX2_EN_VO_PD                        1'b0
 */
typedef union _AON_FAST_LOP_POF_RG6X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_LDOAUX3_TUNE_LDOHQ1_VOUT: 8;
        uint16_t LOP_POF_LDOAUX3_POS_EN_POS: 1;
        uint16_t LOP_POF_LDOAUX2_POW_LDO: 1;
        uint16_t LOP_POF_LDOAUX1_POW_LDO: 1;
        uint16_t LOP_POF_CHG_EN_M2FON1K: 1;
        uint16_t LOP_POF_LDOSYS_POW_LDO533: 1;
        uint16_t LOP_POF_LDOAUX3_POW_LDOHQ1: 1;
        uint16_t LOP_POF_LDOUSB_EN_VO_PD: 1;
        uint16_t LOP_POF_LDOAUX2_EN_VO_PD: 1;
    };
} AON_FAST_LOP_POF_RG6X_TYPE;

/* 0x85E
    7:0     R/W LOP_POF_LDOSYS_TUNE_LDO533_VOUT                 8'b10010011
    8       R/W LOP_POF_LDOSYS_POS_EN_POS                       1'b0
    15:9    R/W LOP_POF_LDOAUX3_LQ_TUNE_LDO_VOUT                7'b0011010
 */
typedef union _AON_FAST_LOP_POF_RG7X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_LDOSYS_TUNE_LDO533_VOUT: 8;
        uint16_t LOP_POF_LDOSYS_POS_EN_POS: 1;
        uint16_t LOP_POF_LDOAUX3_LQ_TUNE_LDO_VOUT: 7;
    };
} AON_FAST_LOP_POF_RG7X_TYPE;

/* 0x860
    5:0     R/W LOP_POF_RG8X_DUMMY1                             6'h0
    6       R/W LOP_POF_EXTRN_LDO2_POW_LDO                      1'b1
    7       R/W LOP_POF_EXTRN_LDO1_POW_LDO                      1'b1
    8       R/W LOP_POF_LDOUSB_POW_LDO                          1'b1
    15:9    R/W LOP_POF_LDOSYS_TUNE_LDO733_VOUT                 7'b1010110
 */
typedef union _AON_FAST_LOP_POF_RG8X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_RG8X_DUMMY1: 6;
        uint16_t LOP_POF_EXTRN_LDO2_POW_LDO: 1;
        uint16_t LOP_POF_EXTRN_LDO1_POW_LDO: 1;
        uint16_t LOP_POF_LDOUSB_POW_LDO: 1;
        uint16_t LOP_POF_LDOSYS_TUNE_LDO733_VOUT: 7;
    };
} AON_FAST_LOP_POF_RG8X_TYPE;

/* 0x862
    1:0     R/W LOP_POF_RG9X_DUMMY1                             2'h0
    8:2     R/W LOP_POF_LDO_PA_TUNE_LDO_VOUT                    7'b0110010
    15:9    R/W LOP_POF_CODEC_LDO_TUNE_LDO_DRV_VOUT             7'b0110010
 */
typedef union _AON_FAST_LOP_POF_RG9X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_RG9X_DUMMY1: 2;
        uint16_t LOP_POF_LDO_PA_TUNE_LDO_VOUT: 7;
        uint16_t LOP_POF_CODEC_LDO_TUNE_LDO_DRV_VOUT: 7;
    };
} AON_FAST_LOP_POF_RG9X_TYPE;

/* 0x864
    0       R/W LOP_POF_SWR_AUDIO_ZCDQ_RST_B                    1'b0
    1       R/W LOP_POF_SWR_CORE_ZCDQ_RST_B                     1'b0
    2       R/W LOP_POF_LDOAUX3_LQ_POW_LDO                      1'b0
    3       R/W LOP_POF_LDOEXT_EN_VREF                          1'b0
    4       R/W LOP_POF_LDOEXT_EN_VO_PD                         1'b0
    5       R/W LOP_POF_LDOEXT_POS_EXT_RST_B                    1'b1
    6       R/W LOP_POF_LDO_DIG_EN_VO_PD                        1'b0
    7       R/W LOP_POF_CODEC_LDO_EN_LDO_DRV_VO_PD              1'b0
    8       R/W LOP_POF_LDO_PA_EN_VO_PD                         1'b0
    15:9    R/W LOP_POF_LDO_DIG_TUNE_LDO_VOUT                   7'b0011101
 */
typedef union _AON_FAST_LOP_POF_RG10X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_SWR_AUDIO_ZCDQ_RST_B: 1;
        uint16_t LOP_POF_SWR_CORE_ZCDQ_RST_B: 1;
        uint16_t LOP_POF_LDOAUX3_LQ_POW_LDO: 1;
        uint16_t LOP_POF_LDOEXT_EN_VREF: 1;
        uint16_t LOP_POF_LDOEXT_EN_VO_PD: 1;
        uint16_t LOP_POF_LDOEXT_POS_EXT_RST_B: 1;
        uint16_t LOP_POF_LDO_DIG_EN_VO_PD: 1;
        uint16_t LOP_POF_CODEC_LDO_EN_LDO_DRV_VO_PD: 1;
        uint16_t LOP_POF_LDO_PA_EN_VO_PD: 1;
        uint16_t LOP_POF_LDO_DIG_TUNE_LDO_VOUT: 7;
    };
} AON_FAST_LOP_POF_RG10X_TYPE;

/* 0x866
    0       R/W LOP_POF_SWR_CORE_FPWM1                          1'b0
    1       R/W LOP_POF_SWR_CORE_EN_SAW_RAMP2                   1'b0
    2       R/W LOP_POF_SWR_CORE_EN_SAW_RAMP1                   1'b0
    3       R/W LOP_POF_SWR_CORE_EN_OCP                         1'b0
    4       R/W LOP_POF_SWR_AUDIO_POW_ZCD                       1'b0
    5       R/W LOP_POF_SWR_AUDIO_SEL_CK_CTRL                   1'b0
    6       R/W LOP_POF_SWR_AUDIO_POW_VDIV                      1'b0
    7       R/W LOP_POF_SWR_AUDIO_SEL_POS_VREFLPPFM             1'b0
    8       R/W LOP_POF_SWR_AUDIO_POW_DIVPFM                    1'b0
    9       R/W LOP_POF_SWR_AUDIO_POW_POS_REF                   1'b0
    10      R/W LOP_POF_SWR_AUDIO_POW_PFM                       1'b0
    11      R/W LOP_POF_SWR_AUDIO_POW_IMIR                      1'b0
    12      R/W LOP_POF_SWR_AUDIO_SEL_CK_CTRL_NEW               1'b0
    13      R/W LOP_POF_SWR_AUDIO_SEL_PFM_COT                   1'b0
    14      R/W LOP_POF_SWR_AUDIO_EN_OCP                        1'b0
    15      R/W LOP_POF_LDOSYS_EN_733TO533_PC                   1'b1
 */
typedef union _AON_FAST_LOP_POF_RG11X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_SWR_CORE_FPWM1: 1;
        uint16_t LOP_POF_SWR_CORE_EN_SAW_RAMP2: 1;
        uint16_t LOP_POF_SWR_CORE_EN_SAW_RAMP1: 1;
        uint16_t LOP_POF_SWR_CORE_EN_OCP: 1;
        uint16_t LOP_POF_SWR_AUDIO_POW_ZCD: 1;
        uint16_t LOP_POF_SWR_AUDIO_SEL_CK_CTRL: 1;
        uint16_t LOP_POF_SWR_AUDIO_POW_VDIV: 1;
        uint16_t LOP_POF_SWR_AUDIO_SEL_POS_VREFLPPFM: 1;
        uint16_t LOP_POF_SWR_AUDIO_POW_DIVPFM: 1;
        uint16_t LOP_POF_SWR_AUDIO_POW_POS_REF: 1;
        uint16_t LOP_POF_SWR_AUDIO_POW_PFM: 1;
        uint16_t LOP_POF_SWR_AUDIO_POW_IMIR: 1;
        uint16_t LOP_POF_SWR_AUDIO_SEL_CK_CTRL_NEW: 1;
        uint16_t LOP_POF_SWR_AUDIO_SEL_PFM_COT: 1;
        uint16_t LOP_POF_SWR_AUDIO_EN_OCP: 1;
        uint16_t LOP_POF_LDOSYS_EN_733TO533_PC: 1;
    };
} AON_FAST_LOP_POF_RG11X_TYPE;

/* 0x868
    0       R/W LOP_POF_LDO_PA_POW_VREF                         1'b0
    1       R/W LOP_POF_CODEC_LDO_POW_LDO_DRV_VREF              1'b0
    2       R/W LOP_POF_SWR_CORE_EN_PFM_TDM                     1'b0
    3       R/W LOP_POF_SWR_CORE_POW_ZCD                        1'b0
    4       R/W LOP_POF_SWR_CORE_POW_ZCD_COMP_LOWIQ             1'b0
    5       R/W LOP_POF_SWR_CORE_POW_VDIV2                      1'b0
    6       R/W LOP_POF_SWR_CORE_POW_VDIV1                      1'b0
    7       R/W LOP_POF_SWR_CORE_POW_SAW_IB                     1'b0
    8       R/W LOP_POF_SWR_CORE_POW_SAW                        1'b0
    9       R/W LOP_POF_SWR_CORE_POW_REF                        1'b0
    10      R/W LOP_POF_SWR_CORE_POW_PWM2                       1'b0
    11      R/W LOP_POF_SWR_CORE_POW_PWM1                       1'b0
    12      R/W LOP_POF_SWR_CORE_POW_PFM2                       1'b0
    13      R/W LOP_POF_SWR_CORE_POW_PFM1                       1'b0
    14      R/W LOP_POF_SWR_CORE_POW_IMIR                       1'b0
    15      R/W LOP_POF_SWR_CORE_FPWM2                          1'b0
 */
typedef union _AON_FAST_LOP_POF_RG12X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_LDO_PA_POW_VREF: 1;
        uint16_t LOP_POF_CODEC_LDO_POW_LDO_DRV_VREF: 1;
        uint16_t LOP_POF_SWR_CORE_EN_PFM_TDM: 1;
        uint16_t LOP_POF_SWR_CORE_POW_ZCD: 1;
        uint16_t LOP_POF_SWR_CORE_POW_ZCD_COMP_LOWIQ: 1;
        uint16_t LOP_POF_SWR_CORE_POW_VDIV2: 1;
        uint16_t LOP_POF_SWR_CORE_POW_VDIV1: 1;
        uint16_t LOP_POF_SWR_CORE_POW_SAW_IB: 1;
        uint16_t LOP_POF_SWR_CORE_POW_SAW: 1;
        uint16_t LOP_POF_SWR_CORE_POW_REF: 1;
        uint16_t LOP_POF_SWR_CORE_POW_PWM2: 1;
        uint16_t LOP_POF_SWR_CORE_POW_PWM1: 1;
        uint16_t LOP_POF_SWR_CORE_POW_PFM2: 1;
        uint16_t LOP_POF_SWR_CORE_POW_PFM1: 1;
        uint16_t LOP_POF_SWR_CORE_POW_IMIR: 1;
        uint16_t LOP_POF_SWR_CORE_FPWM2: 1;
    };
} AON_FAST_LOP_POF_RG12X_TYPE;

/* 0x86A
    0       R/W LOP_POF_RG13X_DUMMY1                            1'h0
    1       R/W LOP_POF_SWR_AUDIO_PFMCCMPOS_EXT_RST_B           1'b0
    2       R/W LOP_POF_SWR_AUDIO_EN_ZCD_LOW_IQ                 1'b0
    3       R/W LOP_POF_SWR_AUDIO_ZCDQ_FORCE_CODE1              1'b0
    4       R/W LOP_POF_SWR_CORE_ZCDQ_FORCE_CODE2               1'b0
    5       R/W LOP_POF_SWR_CORE_ZCDQ_FORCE_CODE1               1'b0
    6       R/W LOP_POF_LDOEXT_POS_EN_POS                       1'b0
    7       R/W LOP_POF_LDOEXT_EN_PC                            1'b1
    8       R/W LOP_POF_LDOEXT_POW_LDOHQ1                       1'b1
    9       R/W LOP_POF_SWR_CORE_PFM1POS_EXT_RST_B              1'b0
    10      R/W LOP_POF_SWR_CORE_EN_VDDCORE_PD                  1'b1
    11      R/W LOP_POF_SWR_CORE_EN_VD105_RF_PD                 1'b1
    12      R/W LOP_POF_SWR_AUDIO_EN_VO_PD                      1'b0
    13      R/W LOP_POF_SWR_CORE_PWM1POS_EXT_RST_B              1'b0
    14      R/W LOP_POF_SWR_AUDIO_PFMPOS_EXT_RST_B              1'b0
    15      R/W LOP_POF_LDO_DIG_POW_VREF                        1'b0
 */
typedef union _AON_FAST_LOP_POF_RG13X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_RG13X_DUMMY1: 1;
        uint16_t LOP_POF_SWR_AUDIO_PFMCCMPOS_EXT_RST_B: 1;
        uint16_t LOP_POF_SWR_AUDIO_EN_ZCD_LOW_IQ: 1;
        uint16_t LOP_POF_SWR_AUDIO_ZCDQ_FORCE_CODE1: 1;
        uint16_t LOP_POF_SWR_CORE_ZCDQ_FORCE_CODE2: 1;
        uint16_t LOP_POF_SWR_CORE_ZCDQ_FORCE_CODE1: 1;
        uint16_t LOP_POF_LDOEXT_POS_EN_POS: 1;
        uint16_t LOP_POF_LDOEXT_EN_PC: 1;
        uint16_t LOP_POF_LDOEXT_POW_LDOHQ1: 1;
        uint16_t LOP_POF_SWR_CORE_PFM1POS_EXT_RST_B: 1;
        uint16_t LOP_POF_SWR_CORE_EN_VDDCORE_PD: 1;
        uint16_t LOP_POF_SWR_CORE_EN_VD105_RF_PD: 1;
        uint16_t LOP_POF_SWR_AUDIO_EN_VO_PD: 1;
        uint16_t LOP_POF_SWR_CORE_PWM1POS_EXT_RST_B: 1;
        uint16_t LOP_POF_SWR_AUDIO_PFMPOS_EXT_RST_B: 1;
        uint16_t LOP_POF_LDO_DIG_POW_VREF: 1;
    };
} AON_FAST_LOP_POF_RG13X_TYPE;

/* 0x86C
    0       R/W LOP_POF_CODEC_LDO_EN_LDO_DRV_PC                 1'b1
    7:1     R/W LOP_POF_FSM_DUMMY1                              7'b0011010
    15:8    R/W LOP_POF_LDOEXT_TUNE_LDOHQ1_VOUT                 8'b01000000
 */
typedef union _AON_FAST_LOP_POF_RG14X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_CODEC_LDO_EN_LDO_DRV_PC: 1;
        uint16_t LOP_POF_FSM_DUMMY1: 7;
        uint16_t LOP_POF_LDOEXT_TUNE_LDOHQ1_VOUT: 8;
    };
} AON_FAST_LOP_POF_RG14X_TYPE;

/* 0x86E
    3:0     R/W LOP_POF_RG15X_DUMMY1                            4'h0
    4       R/W LOP_POF_SWR_AUDIO_POW_SWR                       1'b0
    5       R/W LOP_POF_SWR_AUDIO_POW_CCMPFM                    1'b0
    6       R/W LOP_POF_SWR_AUDIO_PFMPOS_EN_POS                 1'b0
    11:7    R/W LOP_POF_MBIAS_TUNE_LDOAUDIO_LQ_VOUT             5'b10000
    12      R/W LOP_POF_SWR_AUDIO_ZCDQ_POW_UD_DIG               1'b0
    13      R/W LOP_POF_LDO_PA_POW_LDO                          1'b1
    14      R/W LOP_POF_LDO_PA_EN_PC                            1'b1
    15      R/W LOP_POF_CODEC_LDO_POW_LDO_DRV                   1'b1
 */
typedef union _AON_FAST_LOP_POF_RG15X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_RG15X_DUMMY1: 4;
        uint16_t LOP_POF_SWR_AUDIO_POW_SWR: 1;
        uint16_t LOP_POF_SWR_AUDIO_POW_CCMPFM: 1;
        uint16_t LOP_POF_SWR_AUDIO_PFMPOS_EN_POS: 1;
        uint16_t LOP_POF_MBIAS_TUNE_LDOAUDIO_LQ_VOUT: 5;
        uint16_t LOP_POF_SWR_AUDIO_ZCDQ_POW_UD_DIG: 1;
        uint16_t LOP_POF_LDO_PA_POW_LDO: 1;
        uint16_t LOP_POF_LDO_PA_EN_PC: 1;
        uint16_t LOP_POF_CODEC_LDO_POW_LDO_DRV: 1;
    };
} AON_FAST_LOP_POF_RG15X_TYPE;

/* 0x870
    1:0     R/W LOP_POF_RG16X_DUMMY1                            2'h0
    5:2     R/W LOP_POF_MBIAS_SEL_VR_LPPFM1                     4'b1000
    6       R/W LOP_POF_SWR_CORE_SEL_POS_VREFLPPFM2             1'b0
    7       R/W LOP_POF_SWR_CORE_SEL_POS_VREFLPPFM1             1'b0
    15:8    R/W LOP_POF_SWR_AUDIO_TUNE_VDIV                     8'b10011110
 */
typedef union _AON_FAST_LOP_POF_RG16X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_RG16X_DUMMY1: 2;
        uint16_t LOP_POF_MBIAS_SEL_VR_LPPFM1: 4;
        uint16_t LOP_POF_SWR_CORE_SEL_POS_VREFLPPFM2: 1;
        uint16_t LOP_POF_SWR_CORE_SEL_POS_VREFLPPFM1: 1;
        uint16_t LOP_POF_SWR_AUDIO_TUNE_VDIV: 8;
    };
} AON_FAST_LOP_POF_RG16X_TYPE;

/* 0x872
    0       R/W LOP_POF_SWR_CORE_PWM1POS_EN_POS                 1'b0
    1       R/W LOP_POF_SWR_CORE_EN_SWR_OUT2                    1'b0
    2       R/W LOP_POF_SWR_CORE_EN_SWR_OUT1                    1'b0
    3       R/W LOP_POF_XTAL_GATED_DIGIN                        1'b1
    4       R/W LOP_POF_XTAL_GATED_DIGIP                        1'b1
    5       R/W LOP_POF_XTAL_GATED_DIGI_LP                      1'b1
    6       R/W LOP_POF_XTAL_GATED_LPS                          1'b1
    7       R/W LOP_POF_XTAL_GATED_RFN                          1'b1
    8       R/W LOP_POF_XTAL_GATED_RFP                          1'b1
    9       R/W LOP_POF_LDO_DIG_POW_LDO                         1'b1
    10      R/W LOP_POF_SWR_CORE_ZCDQ_POW_UD_DIG                1'b0
    11      R/W LOP_POF_LDO_DIG_EN_PC                           1'b1
    15:12   R/W LOP_POF_MBIAS_SEL_VR_LPPFM2                     4'b1000
 */
typedef union _AON_FAST_LOP_POF_RG17X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_SWR_CORE_PWM1POS_EN_POS: 1;
        uint16_t LOP_POF_SWR_CORE_EN_SWR_OUT2: 1;
        uint16_t LOP_POF_SWR_CORE_EN_SWR_OUT1: 1;
        uint16_t LOP_POF_XTAL_GATED_DIGIN: 1;
        uint16_t LOP_POF_XTAL_GATED_DIGIP: 1;
        uint16_t LOP_POF_XTAL_GATED_DIGI_LP: 1;
        uint16_t LOP_POF_XTAL_GATED_LPS: 1;
        uint16_t LOP_POF_XTAL_GATED_RFN: 1;
        uint16_t LOP_POF_XTAL_GATED_RFP: 1;
        uint16_t LOP_POF_LDO_DIG_POW_LDO: 1;
        uint16_t LOP_POF_SWR_CORE_ZCDQ_POW_UD_DIG: 1;
        uint16_t LOP_POF_LDO_DIG_EN_PC: 1;
        uint16_t LOP_POF_MBIAS_SEL_VR_LPPFM2: 4;
    };
} AON_FAST_LOP_POF_RG17X_TYPE;

/* 0x874
    4:0     R/W LOP_POF_RG18X_DUMMY1                            5'h0
    12:5    R/W LOP_POF_SWR_CORE_TUNE_VDIV1                     8'b10001010
    13      R/W LOP_POF_SWR_CORE_POW_SWR                        1'b0
    14      R/W LOP_POF_SWR_CORE_POW_LDO                        1'b0
    15      R/W LOP_POF_SWR_CORE_PFM1POS_EN_POS                 1'b0
 */
typedef union _AON_FAST_LOP_POF_RG18X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_RG18X_DUMMY1: 5;
        uint16_t LOP_POF_SWR_CORE_TUNE_VDIV1: 8;
        uint16_t LOP_POF_SWR_CORE_POW_SWR: 1;
        uint16_t LOP_POF_SWR_CORE_POW_LDO: 1;
        uint16_t LOP_POF_SWR_CORE_PFM1POS_EN_POS: 1;
    };
} AON_FAST_LOP_POF_RG18X_TYPE;

/* 0x876
    0       R/W LOP_POF_MBIAS_POW_PCUT_VPON_TO_DVDD             1'b0
    1       R/W LOP_POF_MBIAS_POW_PCUT_VD105_RF_TO_DVDD         1'b0
    2       R/W LOP_POF_SWR_CORE_EN_SWR_SHORT                   1'b0
    3       R/W LOP_POF_SWR_CORE_SEL_FOLLOWCTRL1                1'b1
    4       R/W LOP_POF_XTAL_EN_SUPPLY_MODE                     1'b1
    5       R/W LOP_POF_MBIAS_POW_LDOAUDIO_LQ                   1'b1
    6       R/W LOP_POF_SWR_CORE_SEL_CK_CTRL_PFM                1'b1
    7       R/W LOP_POF_EXTRN_SWR_POW_SWR                       1'b0
    15:8    R/W LOP_POF_SWR_CORE_TUNE_VDIV2                     8'b01010100
 */
typedef union _AON_FAST_LOP_POF_RG19X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_MBIAS_POW_PCUT_VPON_TO_DVDD: 1;
        uint16_t LOP_POF_MBIAS_POW_PCUT_VD105_RF_TO_DVDD: 1;
        uint16_t LOP_POF_SWR_CORE_EN_SWR_SHORT: 1;
        uint16_t LOP_POF_SWR_CORE_SEL_FOLLOWCTRL1: 1;
        uint16_t LOP_POF_XTAL_EN_SUPPLY_MODE: 1;
        uint16_t LOP_POF_MBIAS_POW_LDOAUDIO_LQ: 1;
        uint16_t LOP_POF_SWR_CORE_SEL_CK_CTRL_PFM: 1;
        uint16_t LOP_POF_EXTRN_SWR_POW_SWR: 1;
        uint16_t LOP_POF_SWR_CORE_TUNE_VDIV2: 8;
    };
} AON_FAST_LOP_POF_RG19X_TYPE;

/* 0x878
    0       R/W LOP_POF_XTAL_POW_XTAL                           1'b0
    1       R/W LOP_POF_VCORE_PC_POW_VCORE6_PC_VG2              1'b1
    2       R/W LOP_POF_VCORE_PC_POW_VCORE6_PC_VG1              1'b1
    3       R/W LOP_POF_VCORE_PC_POW_VCORE5_PC_VG2              1'b1
    4       R/W LOP_POF_VCORE_PC_POW_VCORE5_PC_VG1              1'b1
    5       R/W LOP_POF_VCORE_PC_POW_VCORE4_PC_VG2              1'b1
    6       R/W LOP_POF_VCORE_PC_POW_VCORE4_PC_VG1              1'b1
    7       R/W LOP_POF_VCORE_PC_POW_VCORE3_PC_VG2              1'b1
    8       R/W LOP_POF_VCORE_PC_POW_VCORE3_PC_VG1              1'b1
    9       R/W LOP_POF_XTAL_EN_LPMODE_AUTO_GATED               1'b0
    10      R/W LOP_POF_XTAL_EN_LPCNT_CLK_DIV                   1'b0
    11      R/W LOP_POF_VCORE_PC_POW_VCORE1_PC_VG2              1'b1
    12      R/W LOP_POF_VCORE_PC_POW_VCORE1_PC_VG1              1'b1
    13      R/W LOP_POF_SWR_CORE_POW_CCMPFM                     1'b0
    14      R/W LOP_POF_BTPLL_LDO2_POW_LDO                      1'b0
    15      R/W LOP_POF_BTPLL_LDO1_POW_LDO                      1'b0
 */
typedef union _AON_FAST_LOP_POF_RG20X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_XTAL_POW_XTAL: 1;
        uint16_t LOP_POF_VCORE_PC_POW_VCORE6_PC_VG2: 1;
        uint16_t LOP_POF_VCORE_PC_POW_VCORE6_PC_VG1: 1;
        uint16_t LOP_POF_VCORE_PC_POW_VCORE5_PC_VG2: 1;
        uint16_t LOP_POF_VCORE_PC_POW_VCORE5_PC_VG1: 1;
        uint16_t LOP_POF_VCORE_PC_POW_VCORE4_PC_VG2: 1;
        uint16_t LOP_POF_VCORE_PC_POW_VCORE4_PC_VG1: 1;
        uint16_t LOP_POF_VCORE_PC_POW_VCORE3_PC_VG2: 1;
        uint16_t LOP_POF_VCORE_PC_POW_VCORE3_PC_VG1: 1;
        uint16_t LOP_POF_XTAL_EN_LPMODE_AUTO_GATED: 1;
        uint16_t LOP_POF_XTAL_EN_LPCNT_CLK_DIV: 1;
        uint16_t LOP_POF_VCORE_PC_POW_VCORE1_PC_VG2: 1;
        uint16_t LOP_POF_VCORE_PC_POW_VCORE1_PC_VG1: 1;
        uint16_t LOP_POF_SWR_CORE_POW_CCMPFM: 1;
        uint16_t LOP_POF_BTPLL_LDO2_POW_LDO: 1;
        uint16_t LOP_POF_BTPLL_LDO1_POW_LDO: 1;
    };
} AON_FAST_LOP_POF_RG20X_TYPE;

/* 0x87A
    0       R/W LOP_POF_BLE_STORE                               1'b0
    1       R/W LOP_POF_RFC_RESTORE                             1'b0
    2       R/W LOP_POF_PF_RESTORE                              1'b0
    3       R/W LOP_POF_MODEM_RESTORE                           1'b0
    4       R/W LOP_POF_DP_MODEM_RESTORE                        1'b0
    5       R/W LOP_POF_BZ_RESTORE                              1'b0
    6       R/W LOP_POF_BLE_RESTORE                             1'b0
    9:7     R/W LOP_POF_XTAL_SEL_MODE                           3'b100
    10      R/W LOP_POF_ISO_XTAL                                1'b1
    11      R/W LOP_POF_ISO_PLL4                                1'b1
    12      R/W LOP_POF_ISO_PLL3                                1'b1
    13      R/W LOP_POF_ISO_PLL2                                1'b1
    14      R/W LOP_POF_ISO_PLL1                                1'b1
    15      R/W LOP_POF_OSC40_POW_OSC                           1'b0
 */
typedef union _AON_FAST_LOP_POF_RG21X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_BLE_STORE: 1;
        uint16_t LOP_POF_RFC_RESTORE: 1;
        uint16_t LOP_POF_PF_RESTORE: 1;
        uint16_t LOP_POF_MODEM_RESTORE: 1;
        uint16_t LOP_POF_DP_MODEM_RESTORE: 1;
        uint16_t LOP_POF_BZ_RESTORE: 1;
        uint16_t LOP_POF_BLE_RESTORE: 1;
        uint16_t LOP_POF_XTAL_SEL_MODE: 3;
        uint16_t LOP_POF_ISO_XTAL: 1;
        uint16_t LOP_POF_ISO_PLL4: 1;
        uint16_t LOP_POF_ISO_PLL3: 1;
        uint16_t LOP_POF_ISO_PLL2: 1;
        uint16_t LOP_POF_ISO_PLL1: 1;
        uint16_t LOP_POF_OSC40_POW_OSC: 1;
    };
} AON_FAST_LOP_POF_RG21X_TYPE;

/* 0x87C
    0       R/W LOP_POF_ISO_BT_CORE2                            1'b1
    1       R/W LOP_POF_ISO_BT_CORE1                            1'b1
    2       R/W LOP_POF_BT_RET_RSTB                             1'b1
    3       R/W LOP_POF_BTPLL1_POW_PLL                          1'b0
    4       R/W LOP_POF_BTPLL1_POW_DIV_PLL_CLK                  1'b0
    5       R/W LOP_POF_BTPLL2_POW_PLL                          1'b0
    6       R/W LOP_POF_BTPLL2_POW_DIV_PLL_CLK                  1'b0
    7       R/W LOP_POF_BTPLL3_POW_PLL                          1'b0
    8       R/W LOP_POF_BTPLL3_POW_DIV_PLL_CLK                  1'b0
    9       R/W LOP_POF_BTPLL4_POW_PLL                          1'b0
    10      R/W LOP_POF_BTPLL4_POW_DIV_PLL_CLK                  1'b0
    11      R/W LOP_POF_RFC_STORE                               1'b0
    12      R/W LOP_POF_PF_STORE                                1'b0
    13      R/W LOP_POF_MODEM_STORE                             1'b0
    14      R/W LOP_POF_DP_MODEM_STORE                          1'b0
    15      R/W LOP_POF_BZ_STORE                                1'b0
 */
typedef union _AON_FAST_LOP_POF_RG22X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_ISO_BT_CORE2: 1;
        uint16_t LOP_POF_ISO_BT_CORE1: 1;
        uint16_t LOP_POF_BT_RET_RSTB: 1;
        uint16_t LOP_POF_BTPLL1_POW_PLL: 1;
        uint16_t LOP_POF_BTPLL1_POW_DIV_PLL_CLK: 1;
        uint16_t LOP_POF_BTPLL2_POW_PLL: 1;
        uint16_t LOP_POF_BTPLL2_POW_DIV_PLL_CLK: 1;
        uint16_t LOP_POF_BTPLL3_POW_PLL: 1;
        uint16_t LOP_POF_BTPLL3_POW_DIV_PLL_CLK: 1;
        uint16_t LOP_POF_BTPLL4_POW_PLL: 1;
        uint16_t LOP_POF_BTPLL4_POW_DIV_PLL_CLK: 1;
        uint16_t LOP_POF_RFC_STORE: 1;
        uint16_t LOP_POF_PF_STORE: 1;
        uint16_t LOP_POF_MODEM_STORE: 1;
        uint16_t LOP_POF_DP_MODEM_STORE: 1;
        uint16_t LOP_POF_BZ_STORE: 1;
    };
} AON_FAST_LOP_POF_RG22X_TYPE;

/* 0x87E
    0       R/W LOP_POF_RG23X_DUMMY1                            1'h0
    1       R/W LOP_POF_SWR_AUDIO_PFMCCMPOS_EN_POS              1'b0
    2       R/W LOP_POF_SWR_CORE_PFMCCM1POS_EN_POS              1'b0
    3       R/W LOP_POF_SWR_CORE_PFMCCM1POS_EXT_RST_B           1'b0
    4       R/W LOP_POF_BT_CORE6_RSTB                           1'b0
    5       R/W LOP_POF_BT_CORE5_RSTB                           1'b0
    6       R/W LOP_POF_BT_CORE4_RSTB                           1'b0
    7       R/W LOP_POF_BT_CORE3_RSTB                           1'b0
    8       R/W LOP_POF_BT_CORE2_RSTB                           1'b0
    9       R/W LOP_POF_BT_CORE1_RSTB                           1'b0
    10      R/W LOP_POF_BT_PON_RSTB                             1'b0
    11      R/W LOP_POF_ISO_BT_PON                              1'b1
    12      R/W LOP_POF_ISO_BT_CORE6                            1'b1
    13      R/W LOP_POF_ISO_BT_CORE5                            1'b1
    14      R/W LOP_POF_ISO_BT_CORE4                            1'b1
    15      R/W LOP_POF_ISO_BT_CORE3                            1'b1
 */
typedef union _AON_FAST_LOP_POF_RG23X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_RG23X_DUMMY1: 1;
        uint16_t LOP_POF_SWR_AUDIO_PFMCCMPOS_EN_POS: 1;
        uint16_t LOP_POF_SWR_CORE_PFMCCM1POS_EN_POS: 1;
        uint16_t LOP_POF_SWR_CORE_PFMCCM1POS_EXT_RST_B: 1;
        uint16_t LOP_POF_BT_CORE6_RSTB: 1;
        uint16_t LOP_POF_BT_CORE5_RSTB: 1;
        uint16_t LOP_POF_BT_CORE4_RSTB: 1;
        uint16_t LOP_POF_BT_CORE3_RSTB: 1;
        uint16_t LOP_POF_BT_CORE2_RSTB: 1;
        uint16_t LOP_POF_BT_CORE1_RSTB: 1;
        uint16_t LOP_POF_BT_PON_RSTB: 1;
        uint16_t LOP_POF_ISO_BT_PON: 1;
        uint16_t LOP_POF_ISO_BT_CORE6: 1;
        uint16_t LOP_POF_ISO_BT_CORE5: 1;
        uint16_t LOP_POF_ISO_BT_CORE4: 1;
        uint16_t LOP_POF_ISO_BT_CORE3: 1;
    };
} AON_FAST_LOP_POF_RG23X_TYPE;

/* 0x880
    7:0     R/W LOP_POF_M1M2_DELAY                              8'ha0
    15:8    R/W LOP_POF_BIAS_DELAY                              8'ha0
 */
typedef union _AON_FAST_LOP_POF_DELAY_RG0X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_M1M2_DELAY: 8;
        uint16_t LOP_POF_BIAS_DELAY: 8;
    };
} AON_FAST_LOP_POF_DELAY_RG0X_TYPE;

/* 0x882
    7:0     R/W LOP_POF_SWR_BIAS_DELAY                          8'ha0
    15:8    R/W LOP_POF_LDOHQ_DELAY                             8'ha0
 */
typedef union _AON_FAST_LOP_POF_DELAY_RG1X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_SWR_BIAS_DELAY: 8;
        uint16_t LOP_POF_LDOHQ_DELAY: 8;
    };
} AON_FAST_LOP_POF_DELAY_RG1X_TYPE;

/* 0x884
    7:0     R/W LOP_POF_SWR_CORE_DELAY                          8'ha0
    15:8    R/W LOP_POF_SWR_AUDIO_DELAY                         8'ha0
 */
typedef union _AON_FAST_LOP_POF_DELAY_RG2X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_SWR_CORE_DELAY: 8;
        uint16_t LOP_POF_SWR_AUDIO_DELAY: 8;
    };
} AON_FAST_LOP_POF_DELAY_RG2X_TYPE;

/* 0x886
    7:0     R/W LOP_POF_XTAL_MODE_CHNG_DELAY                    8'ha0
    15:8    R/W LOP_POF_XTAL_DELAY                              8'ha0
 */
typedef union _AON_FAST_LOP_POF_DELAY_RG3X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_XTAL_MODE_CHNG_DELAY: 8;
        uint16_t LOP_POF_XTAL_DELAY: 8;
    };
} AON_FAST_LOP_POF_DELAY_RG3X_TYPE;

/* 0x888
    7:0     R/W LOP_POF_RET_DELAY                               8'ha0
    15:8    R/W LOP_POF_PLL_DELAY                               8'ha0
 */
typedef union _AON_FAST_LOP_POF_DELAY_RG4X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_RET_DELAY: 8;
        uint16_t LOP_POF_PLL_DELAY: 8;
    };
} AON_FAST_LOP_POF_DELAY_RG4X_TYPE;

/* 0x88A
    7:0     R/W LOP_POF_LDOHQ2_DELAY                            8'ha0
    15:8    R/W LOP_POF_ISO_DELAY                               8'ha0
 */
typedef union _AON_FAST_LOP_POF_DELAY_RG5X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_LDOHQ2_DELAY: 8;
        uint16_t LOP_POF_ISO_DELAY: 8;
    };
} AON_FAST_LOP_POF_DELAY_RG5X_TYPE;

/* 0x88C
    13:0    R/W LOP_POF_MISC_DUMMY1                             14'h0
    14      R/W LOP_POF_CAL                                     1'b0
    15      R/W LOP_POF_AON_GATED_EN                            1'b0
 */
typedef union _AON_FAST_LOP_POF_MISC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_MISC_DUMMY1: 14;
        uint16_t LOP_POF_CAL: 1;
        uint16_t LOP_POF_AON_GATED_EN: 1;
    };
} AON_FAST_LOP_POF_MISC_TYPE;

/* 0x88E
    7:0     R/W LOP_POF_RG6X_DUMMY1                             8'ha0
    15:8    R/W LOP_POF_RST_DELAY                               8'ha0
 */
typedef union _AON_FAST_LOP_POF_DELAY_RG6X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LOP_POF_RG6X_DUMMY1: 8;
        uint16_t LOP_POF_RST_DELAY: 8;
    };
} AON_FAST_LOP_POF_DELAY_RG6X_TYPE;

/* 0x8A0
    0       R/W WAIT_READY_REG0X_DUMMY1                         1'b0
    1       R/W WAIT_VAUX3_DET                                  1'b0
    2       R/W WAIT_VAUX2_DET                                  1'b0
    3       R/W WAIT_VAUX1_DET                                  1'b0
    4       R/W WAIT_PLL4_READY                                 1'b0
    5       R/W WAIT_CP_DIS_OUT                                 1'b0
    6       R/W WAIT_PLL3_READY                                 1'b0
    7       R/W WAIT_PLL2_READY                                 1'b0
    8       R/W WAIT_PLL1_READY                                 1'b0
    9       R/W WAIT_XTAL_CLK_OK                                1'b0
    10      R/W WAIT_SWR_CORE_POR_DET                           1'b0
    11      R/W WAIT_SWR_AUDIO_POR_DET                          1'b0
    12      R/W WAIT_SWR_CORE_HVD17_POR_DET                     1'b0
    13      R/W WAIT_SWR_AUDIO_HVD17_POR_DET                    1'b0
    14      R/W WAIT_HV_DET                                     1'b0
    15      R/W WAIT_HV33_HQ_DET                                1'b0
 */
typedef union _AON_FAST_REG0X_WAIT_READY_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t WAIT_READY_REG0X_DUMMY1: 1;
        uint16_t WAIT_VAUX3_DET: 1;
        uint16_t WAIT_VAUX2_DET: 1;
        uint16_t WAIT_VAUX1_DET: 1;
        uint16_t WAIT_PLL4_READY: 1;
        uint16_t WAIT_CP_DIS_OUT: 1;
        uint16_t WAIT_PLL3_READY: 1;
        uint16_t WAIT_PLL2_READY: 1;
        uint16_t WAIT_PLL1_READY: 1;
        uint16_t WAIT_XTAL_CLK_OK: 1;
        uint16_t WAIT_SWR_CORE_POR_DET: 1;
        uint16_t WAIT_SWR_AUDIO_POR_DET: 1;
        uint16_t WAIT_SWR_CORE_HVD17_POR_DET: 1;
        uint16_t WAIT_SWR_AUDIO_HVD17_POR_DET: 1;
        uint16_t WAIT_HV_DET: 1;
        uint16_t WAIT_HV33_HQ_DET: 1;
    };
} AON_FAST_REG0X_WAIT_READY_TYPE;

/* 0x8A2
    15:0    R/W WAIT_READY_REG1X_DUMMY1                         16'h0
 */
typedef union _AON_FAST_REG1X_WAIT_READY_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t WAIT_READY_REG1X_DUMMY1;
    };
} AON_FAST_REG1X_WAIT_READY_TYPE;

/* 0x8A4
    15:0    R   RO_AON_CNT_EXTRA_DELAY_PON_BIAS                 16'h0
 */
typedef union _AON_FAST_REG0X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_DELAY_PON_BIAS;
    };
} AON_FAST_REG0X_WAIT_AON_CNT_TYPE;

/* 0x8A6
    15:0    R   RO_AON_CNT_EXTRA_DELAY_PON_M1M2                 16'h0
 */
typedef union _AON_FAST_REG1X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_DELAY_PON_M1M2;
    };
} AON_FAST_REG1X_WAIT_AON_CNT_TYPE;

/* 0x8A8
    15:0    R   RO_AON_CNT_EXTRA_DELAY_PON_SWR_BIAS             16'h0
 */
typedef union _AON_FAST_REG2X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_DELAY_PON_SWR_BIAS;
    };
} AON_FAST_REG2X_WAIT_AON_CNT_TYPE;

/* 0x8AA
    15:0    R   RO_AON_CNT_EXTRA_DELAY_PON_SWR_AUDIO            16'h0
 */
typedef union _AON_FAST_REG3X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_DELAY_PON_SWR_AUDIO;
    };
} AON_FAST_REG3X_WAIT_AON_CNT_TYPE;

/* 0x8AC
    15:0    R   RO_AON_CNT_EXTRA_DELAY_PON_SWR_CORE             16'h0
 */
typedef union _AON_FAST_REG4X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_DELAY_PON_SWR_CORE;
    };
} AON_FAST_REG4X_WAIT_AON_CNT_TYPE;

/* 0x8AE
    15:0    R   RO_AON_CNT_EXTRA_DELAY_PON_XTAL                 16'h0
 */
typedef union _AON_FAST_REG5X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_DELAY_PON_XTAL;
    };
} AON_FAST_REG5X_WAIT_AON_CNT_TYPE;

/* 0x8B0
    15:0    R   RO_AON_CNT_EXTRA_DELAY_PON_XTAL_MODE_CHNG       16'h0
 */
typedef union _AON_FAST_REG6X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_DELAY_PON_XTAL_MODE_CHNG;
    };
} AON_FAST_REG6X_WAIT_AON_CNT_TYPE;

/* 0x8B2
    15:0    R   RO_AON_CNT_EXTRA_DELAY_PON_PLL                  16'h0
 */
typedef union _AON_FAST_REG7X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_DELAY_PON_PLL;
    };
} AON_FAST_REG7X_WAIT_AON_CNT_TYPE;

/* 0x8B4
    15:0    R   RO_AON_CNT_EXTRA_DELAY_POF_XTAL_MODE_CHNG       16'h0
 */
typedef union _AON_FAST_REG8X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_DELAY_POF_XTAL_MODE_CHNG;
    };
} AON_FAST_REG8X_WAIT_AON_CNT_TYPE;

/* 0x8B6
    15:0    R   RO_AON_CNT_EXTRA_REG9X_DUMMY1                   16'h0
 */
typedef union _AON_FAST_REG9X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_REG9X_DUMMY1;
    };
} AON_FAST_REG9X_WAIT_AON_CNT_TYPE;

/* 0x8B8
    15:0    R   RO_AON_CNT_EXTRA_REG10X_DUMMY1                  16'h0
 */
typedef union _AON_FAST_REG10X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_REG10X_DUMMY1;
    };
} AON_FAST_REG10X_WAIT_AON_CNT_TYPE;

/* 0x8BA
    15:0    R   RO_AON_CNT_EXTRA_REG11X_DUMMY1                  16'h0
 */
typedef union _AON_FAST_REG11X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_REG11X_DUMMY1;
    };
} AON_FAST_REG11X_WAIT_AON_CNT_TYPE;

/* 0x8BC
    15:0    R   RO_AON_CNT_EXTRA_REG12X_DUMMY1                  16'h0
 */
typedef union _AON_FAST_REG12X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_REG12X_DUMMY1;
    };
} AON_FAST_REG12X_WAIT_AON_CNT_TYPE;

/* 0x8BE
    15:0    R   RO_AON_CNT_EXTRA_REG13X_DUMMY1                  16'h0
 */
typedef union _AON_FAST_REG13X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_REG13X_DUMMY1;
    };
} AON_FAST_REG13X_WAIT_AON_CNT_TYPE;

/* 0x8C0
    15:0    R   RO_AON_CNT_EXTRA_REG14X_DUMMY1                  16'h0
 */
typedef union _AON_FAST_REG14X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_REG14X_DUMMY1;
    };
} AON_FAST_REG14X_WAIT_AON_CNT_TYPE;

/* 0x8C2
    15:0    R   RO_AON_CNT_EXTRA_REG15X_DUMMY1                  16'h0
 */
typedef union _AON_FAST_REG15X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_REG15X_DUMMY1;
    };
} AON_FAST_REG15X_WAIT_AON_CNT_TYPE;

/* 0x8C4
    15:0    R   RO_AON_CNT_EXTRA_REG16X_DUMMY1                  16'h0
 */
typedef union _AON_FAST_REG16X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_REG16X_DUMMY1;
    };
} AON_FAST_REG16X_WAIT_AON_CNT_TYPE;

/* 0x8C6
    15:0    R   RO_AON_CNT_EXTRA_REG17X_DUMMY1                  16'h0
 */
typedef union _AON_FAST_REG17X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_REG17X_DUMMY1;
    };
} AON_FAST_REG17X_WAIT_AON_CNT_TYPE;

/* 0x8C8
    15:0    R   RO_AON_CNT_EXTRA_REG18X_DUMMY1                  16'h0
 */
typedef union _AON_FAST_REG18X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_REG18X_DUMMY1;
    };
} AON_FAST_REG18X_WAIT_AON_CNT_TYPE;

/* 0x8CA
    15:0    R   RO_AON_CNT_EXTRA_REG19X_DUMMY1                  16'h0
 */
typedef union _AON_FAST_REG19X_WAIT_AON_CNT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CNT_EXTRA_REG19X_DUMMY1;
    };
} AON_FAST_REG19X_WAIT_AON_CNT_TYPE;

/* 0x8CC
    11:0    R/W SET_WKEN_REG0X_DUMMY1                           12'h0
    12      R/W USB_WKEN                                        1'b0
    13      R/W MFB_WKEN                                        1'b0
    14      R/W BAT_WKEN                                        1'b0
    15      R/W ADP_WKEN                                        1'b0
 */
typedef union _AON_FAST_SET_WKEN_MISC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SET_WKEN_REG0X_DUMMY1: 12;
        uint16_t USB_WKEN: 1;
        uint16_t MFB_WKEN: 1;
        uint16_t BAT_WKEN: 1;
        uint16_t ADP_WKEN: 1;
    };
} AON_FAST_SET_WKEN_MISC_TYPE;

/* 0x8CE
    0       R/W PAD_P2_WKEN[0]                                  1'b0
    1       R/W PAD_P2_WKEN[1]                                  1'b0
    2       R/W PAD_P2_WKEN[2]                                  1'b0
    3       R/W PAD_P2_WKEN[3]                                  1'b0
    4       R/W PAD_P2_WKEN[4]                                  1'b0
    5       R/W PAD_P2_WKEN[5]                                  1'b0
    6       R/W PAD_P2_WKEN[6]                                  1'b0
    7       R/W PAD_P2_WKEN[7]                                  1'b0
    8       R/W PAD_P1_WKEN[0]                                  1'b0
    9       R/W PAD_P1_WKEN[1]                                  1'b0
    10      R/W PAD_P1_WKEN[2]                                  1'b0
    11      R/W PAD_P1_WKEN[3]                                  1'b0
    12      R/W PAD_P1_WKEN[4]                                  1'b0
    13      R/W PAD_P1_WKEN[5]                                  1'b0
    14      R/W PAD_P1_WKEN[6]                                  1'b0
    15      R/W PAD_P1_WKEN[7]                                  1'b0
 */
typedef union _AON_FAST_SET_WKEN_P1_P2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P2_WKEN_0: 1;
        uint16_t PAD_P2_WKEN_1: 1;
        uint16_t PAD_P2_WKEN_2: 1;
        uint16_t PAD_P2_WKEN_3: 1;
        uint16_t PAD_P2_WKEN_4: 1;
        uint16_t PAD_P2_WKEN_5: 1;
        uint16_t PAD_P2_WKEN_6: 1;
        uint16_t PAD_P2_WKEN_7: 1;
        uint16_t PAD_P1_WKEN_0: 1;
        uint16_t PAD_P1_WKEN_1: 1;
        uint16_t PAD_P1_WKEN_2: 1;
        uint16_t PAD_P1_WKEN_3: 1;
        uint16_t PAD_P1_WKEN_4: 1;
        uint16_t PAD_P1_WKEN_5: 1;
        uint16_t PAD_P1_WKEN_6: 1;
        uint16_t PAD_P1_WKEN_7: 1;
    };
} AON_FAST_SET_WKEN_P1_P2_TYPE;

/* 0x8D0
    0       R/W PAD_P4_WKEN[0]                                  1'b0
    1       R/W PAD_P4_WKEN[1]                                  1'b0
    2       R/W PAD_P4_WKEN[2]                                  1'b0
    3       R/W PAD_P4_WKEN[3]                                  1'b0
    4       R/W PAD_P4_WKEN[4]                                  1'b0
    5       R/W PAD_P4_WKEN[5]                                  1'b0
    6       R/W PAD_P4_WKEN[6]                                  1'b0
    7       R/W PAD_P4_WKEN[7]                                  1'b0
    8       R/W PAD_P3_WKEN[0]                                  1'b0
    9       R/W PAD_P3_WKEN[1]                                  1'b0
    10      R/W PAD_P3_WKEN[2]                                  1'b0
    11      R/W PAD_P3_WKEN[3]                                  1'b0
    12      R/W PAD_P3_WKEN[4]                                  1'b0
    13      R/W PAD_P3_WKEN[5]                                  1'b0
    14      R/W PAD_P3_WKEN[6]                                  1'b0
    15      R/W PAD_P3_WKEN[7]                                  1'b0
 */
typedef union _AON_FAST_SET_WKEN_P3_P4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P4_WKEN_0: 1;
        uint16_t PAD_P4_WKEN_1: 1;
        uint16_t PAD_P4_WKEN_2: 1;
        uint16_t PAD_P4_WKEN_3: 1;
        uint16_t PAD_P4_WKEN_4: 1;
        uint16_t PAD_P4_WKEN_5: 1;
        uint16_t PAD_P4_WKEN_6: 1;
        uint16_t PAD_P4_WKEN_7: 1;
        uint16_t PAD_P3_WKEN_0: 1;
        uint16_t PAD_P3_WKEN_1: 1;
        uint16_t PAD_P3_WKEN_2: 1;
        uint16_t PAD_P3_WKEN_3: 1;
        uint16_t PAD_P3_WKEN_4: 1;
        uint16_t PAD_P3_WKEN_5: 1;
        uint16_t PAD_P3_WKEN_6: 1;
        uint16_t PAD_P3_WKEN_7: 1;
    };
} AON_FAST_SET_WKEN_P3_P4_TYPE;

/* 0x8D2
    0       R/W PAD_P6_WKEN[0]                                  1'b0
    1       R/W PAD_P6_WKEN[1]                                  1'b0
    2       R/W PAD_P6_WKEN[2]                                  1'b0
    3       R/W PAD_P6_WKEN[3]                                  1'b0
    4       R/W PAD_P6_WKEN[4]                                  1'b0
    5       R/W PAD_P6_WKEN[5]                                  1'b0
    6       R/W PAD_P6_WKEN[6]                                  1'b0
    7       R/W PAD_P6_WKEN[7]                                  1'b0
    8       R/W PAD_P5_WKEN[0]                                  1'b0
    9       R/W PAD_P5_WKEN[1]                                  1'b0
    10      R/W PAD_P5_WKEN[2]                                  1'b0
    11      R/W PAD_P5_WKEN[3]                                  1'b0
    12      R/W PAD_P5_WKEN[4]                                  1'b0
    13      R/W PAD_P5_WKEN[5]                                  1'b0
    14      R/W PAD_P5_WKEN[6]                                  1'b0
    15      R/W PAD_P5_WKEN[7]                                  1'b0
 */
typedef union _AON_FAST_SET_WKEN_P5_P6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P6_WKEN_0: 1;
        uint16_t PAD_P6_WKEN_1: 1;
        uint16_t PAD_P6_WKEN_2: 1;
        uint16_t PAD_P6_WKEN_3: 1;
        uint16_t PAD_P6_WKEN_4: 1;
        uint16_t PAD_P6_WKEN_5: 1;
        uint16_t PAD_P6_WKEN_6: 1;
        uint16_t PAD_P6_WKEN_7: 1;
        uint16_t PAD_P5_WKEN_0: 1;
        uint16_t PAD_P5_WKEN_1: 1;
        uint16_t PAD_P5_WKEN_2: 1;
        uint16_t PAD_P5_WKEN_3: 1;
        uint16_t PAD_P5_WKEN_4: 1;
        uint16_t PAD_P5_WKEN_5: 1;
        uint16_t PAD_P5_WKEN_6: 1;
        uint16_t PAD_P5_WKEN_7: 1;
    };
} AON_FAST_SET_WKEN_P5_P6_TYPE;

/* 0x8D4
    0       R/W PAD_P7_WKEN[0]                                  1'b0
    1       R/W PAD_P7_WKEN[1]                                  1'b0
    2       R/W PAD_P7_WKEN[2]                                  1'b0
    3       R/W PAD_P7_WKEN[3]                                  1'b0
    4       R/W PAD_P7_WKEN[4]                                  1'b0
    5       R/W PAD_P7_WKEN[5]                                  1'b0
    6       R/W PAD_P7_WKEN[6]                                  1'b0
    7       R/W PAD_P7_WKEN[7]                                  1'b0
    8       R/W SET_WKEN_P7_32k_DUMMY6                          1'b0
    9       R/W SET_WKEN_P7_32k_DUMMY5                          1'b0
    10      R/W SET_WKEN_P7_32k_DUMMY4                          1'b0
    11      R/W SET_WKEN_P7_32k_DUMMY3                          1'b0
    12      R/W SET_WKEN_P7_32k_DUMMY2                          1'b0
    13      R/W SET_WKEN_P7_32k_DUMMY1                          1'b0
    14      R/W PAD_32KXI_WKEN                                  1'b0
    15      R/W PAD_32KXO_WKEN                                  1'b0
 */
typedef union _AON_FAST_SET_WKEN_P7_32k_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P7_WKEN_0: 1;
        uint16_t PAD_P7_WKEN_1: 1;
        uint16_t PAD_P7_WKEN_2: 1;
        uint16_t PAD_P7_WKEN_3: 1;
        uint16_t PAD_P7_WKEN_4: 1;
        uint16_t PAD_P7_WKEN_5: 1;
        uint16_t PAD_P7_WKEN_6: 1;
        uint16_t PAD_P7_WKEN_7: 1;
        uint16_t SET_WKEN_P7_32k_DUMMY6: 1;
        uint16_t SET_WKEN_P7_32k_DUMMY5: 1;
        uint16_t SET_WKEN_P7_32k_DUMMY4: 1;
        uint16_t SET_WKEN_P7_32k_DUMMY3: 1;
        uint16_t SET_WKEN_P7_32k_DUMMY2: 1;
        uint16_t SET_WKEN_P7_32k_DUMMY1: 1;
        uint16_t PAD_32KXI_WKEN: 1;
        uint16_t PAD_32KXO_WKEN: 1;
    };
} AON_FAST_SET_WKEN_P7_32k_TYPE;

/* 0x8D6
    0       R/W WKEN_REG0X_HYBRID_DUMMY1                        1'b0
    1       R/W PAD_MICBIAS_WKEN                                1'b0
    2       R/W PAD_AUX_R_WKEN                                  1'b0
    3       R/W PAD_AUX_L_WKEN                                  1'b0
    4       R/W PAD_ROUT_N_WKEN                                 1'b0
    5       R/W PAD_ROUT_P_WKEN                                 1'b0
    6       R/W PAD_LOUT_N_WKEN                                 1'b0
    7       R/W PAD_LOUT_P_WKEN                                 1'b0
    8       R/W PAD_ADC_WKEN[0]                                 1'b0
    9       R/W PAD_ADC_WKEN[1]                                 1'b0
    10      R/W PAD_ADC_WKEN[2]                                 1'b0
    11      R/W PAD_ADC_WKEN[3]                                 1'b0
    12      R/W PAD_ADC_WKEN[4]                                 1'b0
    13      R/W PAD_ADC_WKEN[5]                                 1'b0
    14      R/W PAD_ADC_WKEN[6]                                 1'b0
    15      R/W PAD_ADC_WKEN[7]                                 1'b0
 */
typedef union _AON_FAST_SET_WKEN_REG0X_HYBRID_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t WKEN_REG0X_HYBRID_DUMMY1: 1;
        uint16_t PAD_MICBIAS_WKEN: 1;
        uint16_t PAD_AUX_R_WKEN: 1;
        uint16_t PAD_AUX_L_WKEN: 1;
        uint16_t PAD_ROUT_N_WKEN: 1;
        uint16_t PAD_ROUT_P_WKEN: 1;
        uint16_t PAD_LOUT_N_WKEN: 1;
        uint16_t PAD_LOUT_P_WKEN: 1;
        uint16_t PAD_ADC_WKEN_0: 1;
        uint16_t PAD_ADC_WKEN_1: 1;
        uint16_t PAD_ADC_WKEN_2: 1;
        uint16_t PAD_ADC_WKEN_3: 1;
        uint16_t PAD_ADC_WKEN_4: 1;
        uint16_t PAD_ADC_WKEN_5: 1;
        uint16_t PAD_ADC_WKEN_6: 1;
        uint16_t PAD_ADC_WKEN_7: 1;
    };
} AON_FAST_SET_WKEN_REG0X_HYBRID_TYPE;

/* 0x8D8
    2:0     R/W WKEN_REG1X_HYBRID_DUMMY6                        3'h0
    3       R/W WKEN_REG1X_HYBRID_DUMMY5                        1'b0
    4       R/W WKEN_REG1X_HYBRID_DUMMY4                        1'b0
    5       R/W WKEN_REG1X_HYBRID_DUMMY3                        1'b0
    6       R/W WKEN_REG1X_HYBRID_DUMMY2                        1'b0
    7       R/W WKEN_REG1X_HYBRID_DUMMY1                        1'b0
    8       R/W PAD_MIC1_N_WKEN                                 1'b0
    9       R/W PAD_MIC1_P_WKEN                                 1'b0
    10      R/W PAD_MIC2_N_WKEN                                 1'b0
    11      R/W PAD_MIC2_P_WKEN                                 1'b0
    12      R/W PAD_MIC3_N_WKEN                                 1'b0
    13      R/W PAD_MIC3_P_WKEN                                 1'b0
    14      R/W PAD_MIC4_N_WKEN                                 1'b0
    15      R/W PAD_MIC4_P_WKEN                                 1'b0
 */
typedef union _AON_FAST_SET_WKEN_REG1X_HYBRID_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t WKEN_REG1X_HYBRID_DUMMY6: 3;
        uint16_t WKEN_REG1X_HYBRID_DUMMY5: 1;
        uint16_t WKEN_REG1X_HYBRID_DUMMY4: 1;
        uint16_t WKEN_REG1X_HYBRID_DUMMY3: 1;
        uint16_t WKEN_REG1X_HYBRID_DUMMY2: 1;
        uint16_t WKEN_REG1X_HYBRID_DUMMY1: 1;
        uint16_t PAD_MIC1_N_WKEN: 1;
        uint16_t PAD_MIC1_P_WKEN: 1;
        uint16_t PAD_MIC2_N_WKEN: 1;
        uint16_t PAD_MIC2_P_WKEN: 1;
        uint16_t PAD_MIC3_N_WKEN: 1;
        uint16_t PAD_MIC3_P_WKEN: 1;
        uint16_t PAD_MIC4_N_WKEN: 1;
        uint16_t PAD_MIC4_P_WKEN: 1;
    };
} AON_FAST_SET_WKEN_REG1X_HYBRID_TYPE;

/* 0x8DA
    11:0    R/W SET_WKPOL_REG0X_DUMMY1                          12'h0
    12      R/W USB_WKPOL                                       1'b0
    13      R/W MFB_WKPOL                                       1'b0
    14      R/W BAT_WKPOL                                       1'b0
    15      R/W ADP_WKPOL                                       1'b0
 */
typedef union _AON_FAST_SET_WKPOL_MISC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SET_WKPOL_REG0X_DUMMY1: 12;
        uint16_t USB_WKPOL: 1;
        uint16_t MFB_WKPOL: 1;
        uint16_t BAT_WKPOL: 1;
        uint16_t ADP_WKPOL: 1;
    };
} AON_FAST_SET_WKPOL_MISC_TYPE;

/* 0x8DC
    0       R/W PAD_P2_WKPOL[0]                                 1'b0
    1       R/W PAD_P2_WKPOL[1]                                 1'b0
    2       R/W PAD_P2_WKPOL[2]                                 1'b0
    3       R/W PAD_P2_WKPOL[3]                                 1'b0
    4       R/W PAD_P2_WKPOL[4]                                 1'b0
    5       R/W PAD_P2_WKPOL[5]                                 1'b0
    6       R/W PAD_P2_WKPOL[6]                                 1'b0
    7       R/W PAD_P2_WKPOL[7]                                 1'b0
    8       R/W PAD_P1_WKPOL[0]                                 1'b0
    9       R/W PAD_P1_WKPOL[1]                                 1'b0
    10      R/W PAD_P1_WKPOL[2]                                 1'b0
    11      R/W PAD_P1_WKPOL[3]                                 1'b0
    12      R/W PAD_P1_WKPOL[4]                                 1'b0
    13      R/W PAD_P1_WKPOL[5]                                 1'b0
    14      R/W PAD_P1_WKPOL[6]                                 1'b0
    15      R/W PAD_P1_WKPOL[7]                                 1'b0
 */
typedef union _AON_FAST_SET_WKPOL_P1_P2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P2_WKPOL_0: 1;
        uint16_t PAD_P2_WKPOL_1: 1;
        uint16_t PAD_P2_WKPOL_2: 1;
        uint16_t PAD_P2_WKPOL_3: 1;
        uint16_t PAD_P2_WKPOL_4: 1;
        uint16_t PAD_P2_WKPOL_5: 1;
        uint16_t PAD_P2_WKPOL_6: 1;
        uint16_t PAD_P2_WKPOL_7: 1;
        uint16_t PAD_P1_WKPOL_0: 1;
        uint16_t PAD_P1_WKPOL_1: 1;
        uint16_t PAD_P1_WKPOL_2: 1;
        uint16_t PAD_P1_WKPOL_3: 1;
        uint16_t PAD_P1_WKPOL_4: 1;
        uint16_t PAD_P1_WKPOL_5: 1;
        uint16_t PAD_P1_WKPOL_6: 1;
        uint16_t PAD_P1_WKPOL_7: 1;
    };
} AON_FAST_SET_WKPOL_P1_P2_TYPE;

/* 0x8DE
    0       R/W PAD_P4_WKPOL[0]                                 1'b0
    1       R/W PAD_P4_WKPOL[1]                                 1'b0
    2       R/W PAD_P4_WKPOL[2]                                 1'b0
    3       R/W PAD_P4_WKPOL[3]                                 1'b0
    4       R/W PAD_P4_WKPOL[4]                                 1'b0
    5       R/W PAD_P4_WKPOL[5]                                 1'b0
    6       R/W PAD_P4_WKPOL[6]                                 1'b0
    7       R/W PAD_P4_WKPOL[7]                                 1'b0
    8       R/W PAD_P3_WKPOL[0]                                 1'b0
    9       R/W PAD_P3_WKPOL[1]                                 1'b0
    10      R/W PAD_P3_WKPOL[2]                                 1'b0
    11      R/W PAD_P3_WKPOL[3]                                 1'b0
    12      R/W PAD_P3_WKPOL[4]                                 1'b0
    13      R/W PAD_P3_WKPOL[5]                                 1'b0
    14      R/W PAD_P3_WKPOL[6]                                 1'b0
    15      R/W PAD_P3_WKPOL[7]                                 1'b0
 */
typedef union _AON_FAST_SET_WKPOL_P3_P4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P4_WKPOL_0: 1;
        uint16_t PAD_P4_WKPOL_1: 1;
        uint16_t PAD_P4_WKPOL_2: 1;
        uint16_t PAD_P4_WKPOL_3: 1;
        uint16_t PAD_P4_WKPOL_4: 1;
        uint16_t PAD_P4_WKPOL_5: 1;
        uint16_t PAD_P4_WKPOL_6: 1;
        uint16_t PAD_P4_WKPOL_7: 1;
        uint16_t PAD_P3_WKPOL_0: 1;
        uint16_t PAD_P3_WKPOL_1: 1;
        uint16_t PAD_P3_WKPOL_2: 1;
        uint16_t PAD_P3_WKPOL_3: 1;
        uint16_t PAD_P3_WKPOL_4: 1;
        uint16_t PAD_P3_WKPOL_5: 1;
        uint16_t PAD_P3_WKPOL_6: 1;
        uint16_t PAD_P3_WKPOL_7: 1;
    };
} AON_FAST_SET_WKPOL_P3_P4_TYPE;

/* 0x8E0
    0       R/W PAD_P6_WKPOL[0]                                 1'b0
    1       R/W PAD_P6_WKPOL[1]                                 1'b0
    2       R/W PAD_P6_WKPOL[2]                                 1'b0
    3       R/W PAD_P6_WKPOL[3]                                 1'b0
    4       R/W PAD_P6_WKPOL[4]                                 1'b0
    5       R/W PAD_P6_WKPOL[5]                                 1'b0
    6       R/W PAD_P6_WKPOL[6]                                 1'b0
    7       R/W PAD_P6_WKPOL[7]                                 1'b0
    8       R/W PAD_P5_WKPOL[0]                                 1'b0
    9       R/W PAD_P5_WKPOL[1]                                 1'b0
    10      R/W PAD_P5_WKPOL[2]                                 1'b0
    11      R/W PAD_P5_WKPOL[3]                                 1'b0
    12      R/W PAD_P5_WKPOL[4]                                 1'b0
    13      R/W PAD_P5_WKPOL[5]                                 1'b0
    14      R/W PAD_P5_WKPOL[6]                                 1'b0
    15      R/W PAD_P5_WKPOL[7]                                 1'b0
 */
typedef union _AON_FAST_SET_WKPOL_P5_P6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P6_WKPOL_0: 1;
        uint16_t PAD_P6_WKPOL_1: 1;
        uint16_t PAD_P6_WKPOL_2: 1;
        uint16_t PAD_P6_WKPOL_3: 1;
        uint16_t PAD_P6_WKPOL_4: 1;
        uint16_t PAD_P6_WKPOL_5: 1;
        uint16_t PAD_P6_WKPOL_6: 1;
        uint16_t PAD_P6_WKPOL_7: 1;
        uint16_t PAD_P5_WKPOL_0: 1;
        uint16_t PAD_P5_WKPOL_1: 1;
        uint16_t PAD_P5_WKPOL_2: 1;
        uint16_t PAD_P5_WKPOL_3: 1;
        uint16_t PAD_P5_WKPOL_4: 1;
        uint16_t PAD_P5_WKPOL_5: 1;
        uint16_t PAD_P5_WKPOL_6: 1;
        uint16_t PAD_P5_WKPOL_7: 1;
    };
} AON_FAST_SET_WKPOL_P5_P6_TYPE;

/* 0x8E2
    0       R/W PAD_P7_WKPOL[0]                                 1'b0
    1       R/W PAD_P7_WKPOL[1]                                 1'b0
    2       R/W PAD_P7_WKPOL[2]                                 1'b0
    3       R/W PAD_P7_WKPOL[3]                                 1'b0
    4       R/W PAD_P7_WKPOL[4]                                 1'b0
    5       R/W PAD_P7_WKPOL[5]                                 1'b0
    6       R/W PAD_P7_WKPOL[6]                                 1'b0
    7       R/W PAD_P7_WKPOL[7]                                 1'b0
    8       R/W SET_WKPOL_P7_32k_DUMMY6                         1'b0
    9       R/W SET_WKPOL_P7_32k_DUMMY5                         1'b0
    10      R/W SET_WKPOL_P7_32k_DUMMY4                         1'b0
    11      R/W SET_WKPOL_P7_32k_DUMMY3                         1'b0
    12      R/W SET_WKPOL_P7_32k_DUMMY2                         1'b0
    13      R/W SET_WKPOL_P7_32k_DUMMY1                         1'b0
    14      R/W PAD_32KXI_WKPOL                                 1'b0
    15      R/W PAD_32KXO_WKPOL                                 1'b0
 */
typedef union _AON_FAST_SET_WKPOL_P7_32k_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PAD_P7_WKPOL_0: 1;
        uint16_t PAD_P7_WKPOL_1: 1;
        uint16_t PAD_P7_WKPOL_2: 1;
        uint16_t PAD_P7_WKPOL_3: 1;
        uint16_t PAD_P7_WKPOL_4: 1;
        uint16_t PAD_P7_WKPOL_5: 1;
        uint16_t PAD_P7_WKPOL_6: 1;
        uint16_t PAD_P7_WKPOL_7: 1;
        uint16_t SET_WKPOL_P7_32k_DUMMY6: 1;
        uint16_t SET_WKPOL_P7_32k_DUMMY5: 1;
        uint16_t SET_WKPOL_P7_32k_DUMMY4: 1;
        uint16_t SET_WKPOL_P7_32k_DUMMY3: 1;
        uint16_t SET_WKPOL_P7_32k_DUMMY2: 1;
        uint16_t SET_WKPOL_P7_32k_DUMMY1: 1;
        uint16_t PAD_32KXI_WKPOL: 1;
        uint16_t PAD_32KXO_WKPOL: 1;
    };
} AON_FAST_SET_WKPOL_P7_32k_TYPE;

/* 0x8E4
    0       R/W WKPOL_REG0X_HYBRID_DUMMY1                       1'b0
    1       R/W PAD_MICBIAS_WKPOL                               1'b0
    2       R/W PAD_AUX_R_WKPOL                                 1'b0
    3       R/W PAD_AUX_L_WKPOL                                 1'b0
    4       R/W PAD_ROUT_N_WKPOL                                1'b0
    5       R/W PAD_ROUT_P_WKPOL                                1'b0
    6       R/W PAD_LOUT_N_WKPOL                                1'b0
    7       R/W PAD_LOUT_P_WKPOL                                1'b0
    8       R/W PAD_ADC_WKPOL[0]                                1'b0
    9       R/W PAD_ADC_WKPOL[1]                                1'b0
    10      R/W PAD_ADC_WKPOL[2]                                1'b0
    11      R/W PAD_ADC_WKPOL[3]                                1'b0
    12      R/W PAD_ADC_WKPOL[4]                                1'b0
    13      R/W PAD_ADC_WKPOL[5]                                1'b0
    14      R/W PAD_ADC_WKPOL[6]                                1'b0
    15      R/W PAD_ADC_WKPOL[7]                                1'b0
 */
typedef union _AON_FAST_SET_WKPOL_REG0X_HYBRID_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t WKPOL_REG0X_HYBRID_DUMMY1: 1;
        uint16_t PAD_MICBIAS_WKPOL: 1;
        uint16_t PAD_AUX_R_WKPOL: 1;
        uint16_t PAD_AUX_L_WKPOL: 1;
        uint16_t PAD_ROUT_N_WKPOL: 1;
        uint16_t PAD_ROUT_P_WKPOL: 1;
        uint16_t PAD_LOUT_N_WKPOL: 1;
        uint16_t PAD_LOUT_P_WKPOL: 1;
        uint16_t PAD_ADC_WKPOL_0: 1;
        uint16_t PAD_ADC_WKPOL_1: 1;
        uint16_t PAD_ADC_WKPOL_2: 1;
        uint16_t PAD_ADC_WKPOL_3: 1;
        uint16_t PAD_ADC_WKPOL_4: 1;
        uint16_t PAD_ADC_WKPOL_5: 1;
        uint16_t PAD_ADC_WKPOL_6: 1;
        uint16_t PAD_ADC_WKPOL_7: 1;
    };
} AON_FAST_SET_WKPOL_REG0X_HYBRID_TYPE;

/* 0x8E6
    2:0     R/W WKPOL_REG1X_HYBRID_DUMMY6                       3'h0
    3       R/W WKPOL_REG1X_HYBRID_DUMMY5                       1'b0
    4       R/W WKPOL_REG1X_HYBRID_DUMMY4                       1'b0
    5       R/W WKPOL_REG1X_HYBRID_DUMMY3                       1'b0
    6       R/W WKPOL_REG1X_HYBRID_DUMMY2                       1'b0
    7       R/W WKPOL_REG1X_HYBRID_DUMMY1                       1'b0
    8       R/W PAD_MIC1_N_WKPOL                                1'b0
    9       R/W PAD_MIC1_P_WKPOL                                1'b0
    10      R/W PAD_MIC2_N_WKPOL                                1'b0
    11      R/W PAD_MIC2_P_WKPOL                                1'b0
    12      R/W PAD_MIC3_N_WKPOL                                1'b0
    13      R/W PAD_MIC3_P_WKPOL                                1'b0
    14      R/W PAD_MIC4_N_WKPOL                                1'b0
    15      R/W PAD_MIC4_P_WKPOL                                1'b0
 */
typedef union _AON_FAST_SET_WKPOL_REG1X_HYBRID_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t WKPOL_REG1X_HYBRID_DUMMY6: 3;
        uint16_t WKPOL_REG1X_HYBRID_DUMMY5: 1;
        uint16_t WKPOL_REG1X_HYBRID_DUMMY4: 1;
        uint16_t WKPOL_REG1X_HYBRID_DUMMY3: 1;
        uint16_t WKPOL_REG1X_HYBRID_DUMMY2: 1;
        uint16_t WKPOL_REG1X_HYBRID_DUMMY1: 1;
        uint16_t PAD_MIC1_N_WKPOL: 1;
        uint16_t PAD_MIC1_P_WKPOL: 1;
        uint16_t PAD_MIC2_N_WKPOL: 1;
        uint16_t PAD_MIC2_P_WKPOL: 1;
        uint16_t PAD_MIC3_N_WKPOL: 1;
        uint16_t PAD_MIC3_P_WKPOL: 1;
        uint16_t PAD_MIC4_N_WKPOL: 1;
        uint16_t PAD_MIC4_P_WKPOL: 1;
    };
} AON_FAST_SET_WKPOL_REG1X_HYBRID_TYPE;

/* 0x8E8
    5:0     R   RO_WK_REG0X_DUMMY1                              1'b0
    15:6    R   RO_WK_REASON                                    1'b0
 */
typedef union _AON_FAST_RO_WK_REG0X_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_WK_REG0X_DUMMY1: 6;
        uint16_t RO_WK_REASON: 10;
    };
} AON_FAST_RO_WK_REG0X_TYPE;

/* 0x8EA
    0       R/W true_power_off                                  1'b0
    15:1    R/W SET_SHIP_MODE_DUMMY1                            15'h0
 */
typedef union _AON_FAST_SET_SHIP_MODE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t true_power_off: 1;
        uint16_t SET_SHIP_MODE_DUMMY1: 15;
    };
} AON_FAST_SET_SHIP_MODE_TYPE;

/* 0x8EC
    0       R/W LD_RLS_REG0X_DUMMY10                            1'b0
    1       R/W LD_RLS_REG0X_DUMMY9                             1'b0
    2       R/W LD_RLS_REG0X_DUMMY8                             1'b0
    3       R/W LD_RLS_REG0X_DUMMY7                             1'b0
    4       R/W LD_RLS_REG0X_DUMMY6                             1'b0
    5       R/W LD_RLS_REG0X_DUMMY5                             1'b0
    6       R/W LD_RLS_REG0X_DUMMY4                             1'b0
    7       R/W LD_RLS_REG0X_DUMMY3                             1'b0
    8       R/W LD_RLS_REG0X_DUMMY2                             1'b0
    9       R/W LD_RLS_REG0X_DUMMY1                             1'b0
    10      R/W LD_RLS_EN_LDOPA_VTUNE                           1'b0
    11      R/W LD_RLS_EN_LDOEXT_VTUNE                          1'b0
    12      R/W LD_RLS_EN_LDOUSB_VTUNE                          1'b0
    13      R/W LD_RLS_EN_LDOAUX1_VTUNE                         1'b0
    14      R/W LD_RLS_EN_LDOAUX2_VTUNE                         1'b0
    15      R/W LD_RLS_EN_LDOAUX3_VTUNE                         1'b0
 */
typedef union _AON_FAST_REG0X_LOAD_RELEASE_PROTECT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LD_RLS_REG0X_DUMMY10: 1;
        uint16_t LD_RLS_REG0X_DUMMY9: 1;
        uint16_t LD_RLS_REG0X_DUMMY8: 1;
        uint16_t LD_RLS_REG0X_DUMMY7: 1;
        uint16_t LD_RLS_REG0X_DUMMY6: 1;
        uint16_t LD_RLS_REG0X_DUMMY5: 1;
        uint16_t LD_RLS_REG0X_DUMMY4: 1;
        uint16_t LD_RLS_REG0X_DUMMY3: 1;
        uint16_t LD_RLS_REG0X_DUMMY2: 1;
        uint16_t LD_RLS_REG0X_DUMMY1: 1;
        uint16_t LD_RLS_EN_LDOPA_VTUNE: 1;
        uint16_t LD_RLS_EN_LDOEXT_VTUNE: 1;
        uint16_t LD_RLS_EN_LDOUSB_VTUNE: 1;
        uint16_t LD_RLS_EN_LDOAUX1_VTUNE: 1;
        uint16_t LD_RLS_EN_LDOAUX2_VTUNE: 1;
        uint16_t LD_RLS_EN_LDOAUX3_VTUNE: 1;
    };
} AON_FAST_REG0X_LOAD_RELEASE_PROTECT_TYPE;

/* 0x8EE
    11:0    R/W LD_RLS_SEL_LDOAUX3_AND_SRC                      12'b000000000000
    15:12   R/W LD_RLS_SEL_LDOAUX3_SRC                          4'b0000
 */
typedef union _AON_FAST_REG1X_LOAD_RELEASE_PROTECT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LD_RLS_SEL_LDOAUX3_AND_SRC: 12;
        uint16_t LD_RLS_SEL_LDOAUX3_SRC: 4;
    };
} AON_FAST_REG1X_LOAD_RELEASE_PROTECT_TYPE;

/* 0x8F0
    11:0    R/W LD_RLS_SEL_LDOAUX2_AND_SRC                      12'b000000000000
    15:12   R/W LD_RLS_SEL_LDOAUX2_SRC                          4'b0000
 */
typedef union _AON_FAST_REG2X_LOAD_RELEASE_PROTECT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LD_RLS_SEL_LDOAUX2_AND_SRC: 12;
        uint16_t LD_RLS_SEL_LDOAUX2_SRC: 4;
    };
} AON_FAST_REG2X_LOAD_RELEASE_PROTECT_TYPE;

/* 0x8F2
    11:0    R/W LD_RLS_SEL_LDOAUX1_AND_SRC                      12'b000000000000
    15:12   R/W LD_RLS_SEL_LDOAUX1_SRC                          4'b0000
 */
typedef union _AON_FAST_REG3X_LOAD_RELEASE_PROTECT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LD_RLS_SEL_LDOAUX1_AND_SRC: 12;
        uint16_t LD_RLS_SEL_LDOAUX1_SRC: 4;
    };
} AON_FAST_REG3X_LOAD_RELEASE_PROTECT_TYPE;

/* 0x8F4
    11:0    R/W LD_RLS_SEL_LDOUSB_AND_SRC                       12'b000000000000
    15:12   R/W LD_RLS_SEL_LDOUSB_SRC                           4'b0000
 */
typedef union _AON_FAST_REG4X_LOAD_RELEASE_PROTECT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LD_RLS_SEL_LDOUSB_AND_SRC: 12;
        uint16_t LD_RLS_SEL_LDOUSB_SRC: 4;
    };
} AON_FAST_REG4X_LOAD_RELEASE_PROTECT_TYPE;

/* 0x8F6
    11:0    R/W LD_RLS_SEL_LDOEXT_AND_SRC                       12'b000000000000
    15:12   R/W LD_RLS_SEL_LDOEXT_SRC                           4'b0000
 */
typedef union _AON_FAST_REG5X_LOAD_RELEASE_PROTECT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LD_RLS_SEL_LDOEXT_AND_SRC: 12;
        uint16_t LD_RLS_SEL_LDOEXT_SRC: 4;
    };
} AON_FAST_REG5X_LOAD_RELEASE_PROTECT_TYPE;

/* 0x8F8
    11:0    R/W LD_RLS_SEL_LDOPA_AND_SRC                        12'b000000000000
    15:12   R/W LD_RLS_SEL_LDOPA_SRC                            4'b0000
 */
typedef union _AON_FAST_REG6X_LOAD_RELEASE_PROTECT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LD_RLS_SEL_LDOPA_AND_SRC: 12;
        uint16_t LD_RLS_SEL_LDOPA_SRC: 4;
    };
} AON_FAST_REG6X_LOAD_RELEASE_PROTECT_TYPE;

/* 0x8FA
    7:0     R/W LD_RLS_TUNE_LDOAUX2_VOUT                        8'b00101011
    15:8    R/W LD_RLS_TUNE_LDOAUX3_VOUT                        8'b00101011
 */
typedef union _AON_FAST_REG7X_LOAD_RELEASE_PROTECT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LD_RLS_TUNE_LDOAUX2_VOUT: 8;
        uint16_t LD_RLS_TUNE_LDOAUX3_VOUT: 8;
    };
} AON_FAST_REG7X_LOAD_RELEASE_PROTECT_TYPE;

/* 0x8FC
    7:0     R/W LD_RLS_TUNE_LDOUSB_VOUT                         8'b00101011
    15:8    R/W LD_RLS_TUNE_LDOAUX1_VOUT                        8'b00101011
 */
typedef union _AON_FAST_REG8X_LOAD_RELEASE_PROTECT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LD_RLS_TUNE_LDOUSB_VOUT: 8;
        uint16_t LD_RLS_TUNE_LDOAUX1_VOUT: 8;
    };
} AON_FAST_REG8X_LOAD_RELEASE_PROTECT_TYPE;

/* 0x8FE
    7:0     R/W LD_RLS_TUNE_LDOPA_VOUT                          8'b00101011
    15:8    R/W LD_RLS_TUNE_LDOEXT_VOUT                         8'b00101011
 */
typedef union _AON_FAST_REG9X_LOAD_RELEASE_PROTECT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LD_RLS_TUNE_LDOPA_VOUT: 8;
        uint16_t LD_RLS_TUNE_LDOEXT_VOUT: 8;
    };
} AON_FAST_REG9X_LOAD_RELEASE_PROTECT_TYPE;

/* 0x900
    1:0     R/W PMUX_SEL_ADC_7_FSM_CTRL                         2'b11
    3:2     R/W PMUX_SEL_ADC_6_FSM_CTRL                         2'b11
    5:4     R/W PMUX_SEL_ADC_5_FSM_CTRL                         2'b11
    7:6     R/W PMUX_SEL_ADC_4_FSM_CTRL                         2'b11
    9:8     R/W PMUX_SEL_ADC_3_FSM_CTRL                         2'b11
    11:10   R/W PMUX_SEL_ADC_2_FSM_CTRL                         2'b11
    13:12   R/W PMUX_SEL_ADC_1_FSM_CTRL                         2'b11
    15:14   R/W PMUX_SEL_ADC_0_FSM_CTRL                         2'b11
 */
typedef union _AON_FAST_REG0X_PMUX_SEL_FSM_CTRL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_ADC_7_FSM_CTRL: 2;
        uint16_t PMUX_SEL_ADC_6_FSM_CTRL: 2;
        uint16_t PMUX_SEL_ADC_5_FSM_CTRL: 2;
        uint16_t PMUX_SEL_ADC_4_FSM_CTRL: 2;
        uint16_t PMUX_SEL_ADC_3_FSM_CTRL: 2;
        uint16_t PMUX_SEL_ADC_2_FSM_CTRL: 2;
        uint16_t PMUX_SEL_ADC_1_FSM_CTRL: 2;
        uint16_t PMUX_SEL_ADC_0_FSM_CTRL: 2;
    };
} AON_FAST_REG0X_PMUX_SEL_FSM_CTRL_TYPE;

/* 0x902
    1:0     R/W PMUX_SEL_P2_2_FSM_CTRL                          2'b11
    3:2     R/W PMUX_SEL_P2_1_FSM_CTRL                          2'b11
    5:4     R/W PMUX_SEL_P1_7_FSM_CTRL                          2'b11
    7:6     R/W PMUX_SEL_P1_6_FSM_CTRL                          2'b11
    9:8     R/W PMUX_SEL_P1_1_FSM_CTRL                          2'b11
    11:10   R/W PMUX_SEL_P1_0_FSM_CTRL                          2'b11
    13:12   R/W PMUX_SEL_32k_XO_FSM_CTRL                        2'b11
    15:14   R/W PMUX_SEL_32k_XI_FSM_CTRL                        2'b11
 */
typedef union _AON_FAST_REG1X_PMUX_SEL_FSM_CTRL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P2_2_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P2_1_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P1_7_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P1_6_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P1_1_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P1_0_FSM_CTRL: 2;
        uint16_t PMUX_SEL_32k_XO_FSM_CTRL: 2;
        uint16_t PMUX_SEL_32k_XI_FSM_CTRL: 2;
    };
} AON_FAST_REG1X_PMUX_SEL_FSM_CTRL_TYPE;

/* 0x904
    1:0     R/W PMUX_SEL_P3_2_FSM_CTRL                          2'b11
    3:2     R/W PMUX_SEL_P4_1_FSM_CTRL                          2'b11
    5:4     R/W PMUX_SEL_P4_0_FSM_CTRL                          2'b11
    7:6     R/W PMUX_SEL_P2_7_FSM_CTRL                          2'b11
    9:8     R/W PMUX_SEL_P2_6_FSM_CTRL                          2'b11
    11:10   R/W PMUX_SEL_P2_5_FSM_CTRL                          2'b11
    13:12   R/W PMUX_SEL_P2_4_FSM_CTRL                          2'b11
    15:14   R/W PMUX_SEL_P2_3_FSM_CTRL                          2'b11
 */
typedef union _AON_FAST_REG2X_PMUX_SEL_FSM_CTRL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P3_2_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P4_1_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P4_0_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P2_7_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P2_6_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P2_5_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P2_4_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P2_3_FSM_CTRL: 2;
    };
} AON_FAST_REG2X_PMUX_SEL_FSM_CTRL_TYPE;

/* 0x906
    1:0     R/W PMUX_SEL_P5_7_FSM_CTRL                          2'b11
    3:2     R/W PMUX_SEL_P5_6_FSM_CTRL                          2'b11
    5:4     R/W PMUX_SEL_P5_0_FSM_CTRL                          2'b11
    7:6     R/W PMUX_SEL_P3_7_FSM_CTRL                          2'b11
    9:8     R/W PMUX_SEL_P3_6_FSM_CTRL                          2'b11
    11:10   R/W PMUX_SEL_P3_5_FSM_CTRL                          2'b11
    13:12   R/W PMUX_SEL_P3_4_FSM_CTRL                          2'b11
    15:14   R/W PMUX_SEL_P3_3_FSM_CTRL                          2'b11
 */
typedef union _AON_FAST_REG3X_PMUX_SEL_FSM_CTRL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_P5_7_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P5_6_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P5_0_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P3_7_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P3_6_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P3_5_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P3_4_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P3_3_FSM_CTRL: 2;
    };
} AON_FAST_REG3X_PMUX_SEL_FSM_CTRL_TYPE;

/* 0x908
    1:0     R/W PMUX_SEL_AUX_R_FSM_CTRL                         2'b11
    3:2     R/W PMUX_SEL_AUX_L_FSM_CTRL                         2'b11
    5:4     R/W PMUX_SEL_MIC4_P_FSM_CTRL                        2'b11
    7:6     R/W PMUX_SEL_MIC4_N_FSM_CTRL                        2'b11
    9:8     R/W PMUX_SEL_P6_1_FSM_CTRL                          2'b11
    11:10   R/W PMUX_SEL_P6_0_FSM_CTRL                          2'b11
    13:12   R/W PMUX_SEL_P4_7_FSM_CTRL                          2'b11
    15:14   R/W PMUX_SEL_P4_6_FSM_CTRL                          2'b11
 */
typedef union _AON_FAST_REG4X_PMUX_SEL_FSM_CTRL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t PMUX_SEL_AUX_R_FSM_CTRL: 2;
        uint16_t PMUX_SEL_AUX_L_FSM_CTRL: 2;
        uint16_t PMUX_SEL_MIC4_P_FSM_CTRL: 2;
        uint16_t PMUX_SEL_MIC4_N_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P6_1_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P6_0_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P4_7_FSM_CTRL: 2;
        uint16_t PMUX_SEL_P4_6_FSM_CTRL: 2;
    };
} AON_FAST_REG4X_PMUX_SEL_FSM_CTRL_TYPE;

/* 0x95A
    13:0    R/W CORE_MODULE_REG0X_DUMMY1                        14'h0
    14      R/W CORE_MODULE_REG0X_DUMMY2                        1'b0
    15      R/W SWR_BY_CORE                                     1'b0
 */
typedef union _AON_FAST_CORE_MODULE_REG0X_MUX_SEL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CORE_MODULE_REG0X_DUMMY1: 14;
        uint16_t CORE_MODULE_REG0X_DUMMY2: 1;
        uint16_t SWR_BY_CORE: 1;
    };
} AON_FAST_CORE_MODULE_REG0X_MUX_SEL_TYPE;

/* 0xC00
    0       R/W MBIAS_SEL_BIAS_PSRR_RES                         1'b0                    MBIAS_REG_MASK
    1       R   RO_AON_MBIAS_POW_BIAS_10UA                      1'b0
    2       R   RO_AON_MBIAS_POW_BIAS_500NA                     1'b0
    3       R   RO_AON_MBIAS_FSM_DUMMY1                         1'b0
    6:4     R   RO_AON_MBIAS_TUNE_BIAS_10UA_IQ                  3'b000
    9:7     R   RO_AON_MBIAS_TUNE_BIAS_500NA_IQ                 3'b000
    12:10   R   RO_AON_MBIAS_TUNE_BIAS_50NA_IQ                  3'b000
    15:13   R   RO_AON_MBIAS_TUNE_BIAS_3NA_IQ                   3'b000
 */
typedef union _AON_FAST_REG0X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_SEL_BIAS_PSRR_RES: 1;
        uint16_t RO_AON_MBIAS_POW_BIAS_10UA: 1;
        uint16_t RO_AON_MBIAS_POW_BIAS_500NA: 1;
        uint16_t RO_AON_MBIAS_FSM_DUMMY1: 1;
        uint16_t RO_AON_MBIAS_TUNE_BIAS_10UA_IQ: 3;
        uint16_t RO_AON_MBIAS_TUNE_BIAS_500NA_IQ: 3;
        uint16_t RO_AON_MBIAS_TUNE_BIAS_50NA_IQ: 3;
        uint16_t RO_AON_MBIAS_TUNE_BIAS_3NA_IQ: 3;
    };
} AON_FAST_REG0X_MBIAS_TYPE;

/* 0xC02
    1:0     R   RO_AON_MBIAS_TUNE_LPBG_IQ                       2'b00
    4:2     R/W MBIAS_TUNE_LPBG_TC                              3'b100                  MBIAS_REG_MASK
    7:5     R/W MBIAS_TUNE_LPBG_VREF                            3'b100                  MBIAS_REG_MASK
    11:8    R/W MBIAS_SEL_VR_AUXADC                             4'b1000                 MBIAS_REG_MASK
    14:12   R/W MBIAS_REG1X_DUMMY2                              3'b100                  MBIAS_REG_MASK
    15      R/W MBIAS_EN_BG_EXT_CAP                             1'b0                    MBIAS_REG_MASK
 */
typedef union _AON_FAST_REG1X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_MBIAS_TUNE_LPBG_IQ: 2;
        uint16_t MBIAS_TUNE_LPBG_TC: 3;
        uint16_t MBIAS_TUNE_LPBG_VREF: 3;
        uint16_t MBIAS_SEL_VR_AUXADC: 4;
        uint16_t MBIAS_REG1X_DUMMY2: 3;
        uint16_t MBIAS_EN_BG_EXT_CAP: 1;
    };
} AON_FAST_REG1X_MBIAS_TYPE;

/* 0xC04
    2:0     R/W MBIAS_TUNE_LPBG_VREF500NA                       3'b100                  MBIAS_REG_MASK
    6:3     R   RO_AON_MBIAS_SEL_VR_LPPFM2                      4'b1000
    10:7    R   RO_AON_MBIAS_SEL_VR_LPPFM1                      4'b1000
    12:11   R/W MBIAS_SEL_VR_LDOHQ2                             2'b00                   MBIAS_REG_MASK
    13      R/W MBIAS_EN_IB10U_AUDIO_B                          1'b0                    MBIAS_REG_MASK
    14      R/W MBIAS_REG2X_DUMMY2                              1'b1                    MBIAS_REG_MASK
    15      R/W MBIAS_EN_DL_LDOHVDPD33_B                        1'b1                    MBIAS_REG_MASK
 */
typedef union _AON_FAST_REG2X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_TUNE_LPBG_VREF500NA: 3;
        uint16_t RO_AON_MBIAS_SEL_VR_LPPFM2: 4;
        uint16_t RO_AON_MBIAS_SEL_VR_LPPFM1: 4;
        uint16_t MBIAS_SEL_VR_LDOHQ2: 2;
        uint16_t MBIAS_EN_IB10U_AUDIO_B: 1;
        uint16_t MBIAS_REG2X_DUMMY2: 1;
        uint16_t MBIAS_EN_DL_LDOHVDPD33_B: 1;
    };
} AON_FAST_REG2X_MBIAS_TYPE;

/* 0xC06
    3:0     R/W MBIAS_SEL_ADPIN_DPD_VR_H                        4'b0010                 MBIAS_REG_MASK
    7:4     R/W MBIAS_SEL_ADPIN_DPD_VR_L                        4'b0010                 MBIAS_REG_MASK
    10:8    R/W MBIAS_SEL_VBAT_DPD_VR_H                         3'b010                  MBIAS_REG_MASK
    13:11   R/W MBIAS_SEL_VBAT_DPD_VR_L                         3'b010                  MBIAS_REG_MASK
    15:14   R/W MBIAS_REG3X_DUMMY1                              2'b10                   MBIAS_REG_MASK
 */
typedef union _AON_FAST_REG3X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_SEL_ADPIN_DPD_VR_H: 4;
        uint16_t MBIAS_SEL_ADPIN_DPD_VR_L: 4;
        uint16_t MBIAS_SEL_VBAT_DPD_VR_H: 3;
        uint16_t MBIAS_SEL_VBAT_DPD_VR_L: 3;
        uint16_t MBIAS_REG3X_DUMMY1: 2;
    };
} AON_FAST_REG3X_MBIAS_TYPE;

/* 0xC08
    0       R/W MBIAS_EN_DPD_COMP_HYS                           1'b0                    MBIAS_REG_MASK
    1       R   RO_AON_MBIAS_SEL_DPD_RCK                        1'b0
    3:2     R/W MBIAS_SEL_DPD_VBAT_DR                           2'b00                   MBIAS_REG_MASK
    5:4     R/W MBIAS_SEL_DPD_ADPIN_DR                          2'b00                   MBIAS_REG_MASK
    7:6     R/W MBIAS_SEL_MFB2                                  2'b11                   MBIAS_REG_MASK
    9:8     R/W MBIAS_SEL_MFB1                                  2'b11                   MBIAS_REG_MASK
    12:10   R/W MBIAS_TUNE_LDO733DPD_VOUT                       3'b001                  MBIAS_REG_MASK
    15:13   R/W MBIAS_REG4X_DUMMY1                              3'b100                  MBIAS_REG_MASK
 */
typedef union _AON_FAST_REG4X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_EN_DPD_COMP_HYS: 1;
        uint16_t RO_AON_MBIAS_SEL_DPD_RCK: 1;
        uint16_t MBIAS_SEL_DPD_VBAT_DR: 2;
        uint16_t MBIAS_SEL_DPD_ADPIN_DR: 2;
        uint16_t MBIAS_SEL_MFB2: 2;
        uint16_t MBIAS_SEL_MFB1: 2;
        uint16_t MBIAS_TUNE_LDO733DPD_VOUT: 3;
        uint16_t MBIAS_REG4X_DUMMY1: 3;
    };
} AON_FAST_REG4X_MBIAS_TYPE;

/* 0xC0A
    0       R/W MBIAS_POW_OSC_1KHZ                              1'b1                    MBIAS_REG_MASK
    6:1     R/W MBIAS_TUNE_OSC_1KHZ_FREQ                        6'b100000               MBIAS_REG_MASK
    7       R/W MBIAS_SEL_DPD_R0                                1'b0                    MBIAS_REG_MASK
    8       R/W MBIAS_SEL_DPD_R1                                1'b1                    MBIAS_REG_MASK
    9       R/W MBIAS_SEL_DPD_R2                                1'b1                    MBIAS_REG_MASK
    10      R/W MBIAS_SEL_DPD_R3                                1'b1                    MBIAS_REG_MASK
    11      R/W MBIAS_SEL_DPD_R4                                1'b1                    MBIAS_REG_MASK
    12      R/W MBIAS_SEL_DPD_R5                                1'b1                    MBIAS_REG_MASK
    13      R/W MBIAS_SEL_DPD_R6                                1'b1                    MBIAS_REG_MASK
    14      R/W MBIAS_SEL_DPD_R7                                1'b1                    MBIAS_REG_MASK
    15      R/W MBIAS_SEL_DPD_R8                                1'b1                    MBIAS_REG_MASK
 */
typedef union _AON_FAST_REG5X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_POW_OSC_1KHZ: 1;
        uint16_t MBIAS_TUNE_OSC_1KHZ_FREQ: 6;
        uint16_t MBIAS_SEL_DPD_R0: 1;
        uint16_t MBIAS_SEL_DPD_R1: 1;
        uint16_t MBIAS_SEL_DPD_R2: 1;
        uint16_t MBIAS_SEL_DPD_R3: 1;
        uint16_t MBIAS_SEL_DPD_R4: 1;
        uint16_t MBIAS_SEL_DPD_R5: 1;
        uint16_t MBIAS_SEL_DPD_R6: 1;
        uint16_t MBIAS_SEL_DPD_R7: 1;
        uint16_t MBIAS_SEL_DPD_R8: 1;
    };
} AON_FAST_REG5X_MBIAS_TYPE;

/* 0xC0C
    0       R/W MBIAS_POW_PCUT_VCORE1                           1'b0                    MBIAS_REG_MASK
    1       R/W MBIAS_POW_PCUT_DVDD                             1'b1                    MBIAS_REG_MASK
    2       R/W MBIAS_POW_LDORET                                1'b0                    MBIAS_REG_MASK
    3       R/W MBIAS_EN_LDO309_FT                              1'b0                    MBIAS_REG_MASK
    4       R/W MBIAS_TRIM_LDO309_FT                            1'b0                    MBIAS_REG_MASK
    9:5     R/W MBIAS_TUNE_LDO318_VOUT                          5'b10000                MBIAS_REG_MASK
    15:10   R   RO_AON_MBIAS_TUNE_LDO309_VOUT                   6'b100000
 */
typedef union _AON_FAST_REG6X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_POW_PCUT_VCORE1: 1;
        uint16_t MBIAS_POW_PCUT_DVDD: 1;
        uint16_t MBIAS_POW_LDORET: 1;
        uint16_t MBIAS_EN_LDO309_FT: 1;
        uint16_t MBIAS_TRIM_LDO309_FT: 1;
        uint16_t MBIAS_TUNE_LDO318_VOUT: 5;
        uint16_t RO_AON_MBIAS_TUNE_LDO309_VOUT: 6;
    };
} AON_FAST_REG6X_MBIAS_TYPE;

/* 0xC0E
    5:0     R/W MBIAS_FSM_DUMMY2                                6'b100000               MBIAS_REG_MASK
    6       R/W MBIAS_EN_DL_LDO309_B                            1'b0                    MBIAS_REG_MASK
    7       R/W MBIAS_EN_DL_LDO318_B                            1'b1                    MBIAS_REG_MASK
    8       R/W MBIAS_EN_DL_LDORET_B                            1'b1                    MBIAS_REG_MASK
    10:9    R/W MBIAS_EN_SW_OFF_B                               2'b00                   MBIAS_REG_MASK
    12:11   R/W MBIAS_SEL_ADP_SST                               2'b00                   MBIAS_REG_MASK
    15:13   R/W MBIAS_SEL_LDO309_DL                             3'b111                  MBIAS_REG_MASK
 */
typedef union _AON_FAST_REG7X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_FSM_DUMMY2: 6;
        uint16_t MBIAS_EN_DL_LDO309_B: 1;
        uint16_t MBIAS_EN_DL_LDO318_B: 1;
        uint16_t MBIAS_EN_DL_LDORET_B: 1;
        uint16_t MBIAS_EN_SW_OFF_B: 2;
        uint16_t MBIAS_SEL_ADP_SST: 2;
        uint16_t MBIAS_SEL_LDO309_DL: 3;
    };
} AON_FAST_REG7X_MBIAS_TYPE;

/* 0xC10
    0       R   RO_AON_MBIAS_POW_VAUX1_DET                      1'b1
    1       R   RO_AON_MBIAS_POW_VAUX2_DET                      1'b1
    2       R   RO_AON_MBIAS_POW_VAUX3_DET                      1'b1
    3       R   RO_AON_MBIAS_POW_HV_DET                         1'b1
    4       R/W MBIAS_POW_HV33_DET                              1'b1                    MBIAS_REG_MASK
    5       R   RO_AON_MBIAS_POW_HV33_HQ_DET                    1'b1
    6       R   RO_AON_MBIAS_POW_VAUDIO_DET                     1'b1
    7       R   RO_AON_MBIAS_POW_VD105_RF_DET                   1'b1
    8       R/W MBIAS_POW_DVDD_DET                              1'b1                    MBIAS_REG_MASK
    9       R/W MBIAS_POW_HV18_DET                              1'b1                    MBIAS_REG_MASK
    10      R   RO_AON_MBIAS_POW_PCUT_VPON_TO_DVDD              1'b0
    11      R   RO_AON_MBIAS_POW_PCUT_DVDD_TO_DVDD1             1'b1
    12      R   RO_AON_MBIAS_POW_PCUT_VD105_RF_TO_DVDD          1'b0
    15:13   R/W MBIAS_REG8X_DUMMY1                              3'b111                  MBIAS_REG_MASK
 */
typedef union _AON_FAST_REG8X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_MBIAS_POW_VAUX1_DET: 1;
        uint16_t RO_AON_MBIAS_POW_VAUX2_DET: 1;
        uint16_t RO_AON_MBIAS_POW_VAUX3_DET: 1;
        uint16_t RO_AON_MBIAS_POW_HV_DET: 1;
        uint16_t MBIAS_POW_HV33_DET: 1;
        uint16_t RO_AON_MBIAS_POW_HV33_HQ_DET: 1;
        uint16_t RO_AON_MBIAS_POW_VAUDIO_DET: 1;
        uint16_t RO_AON_MBIAS_POW_VD105_RF_DET: 1;
        uint16_t MBIAS_POW_DVDD_DET: 1;
        uint16_t MBIAS_POW_HV18_DET: 1;
        uint16_t RO_AON_MBIAS_POW_PCUT_VPON_TO_DVDD: 1;
        uint16_t RO_AON_MBIAS_POW_PCUT_DVDD_TO_DVDD1: 1;
        uint16_t RO_AON_MBIAS_POW_PCUT_VD105_RF_TO_DVDD: 1;
        uint16_t MBIAS_REG8X_DUMMY1: 3;
    };
} AON_FAST_REG8X_MBIAS_TYPE;

/* 0xC12
    2:0     R/W MBIAS_SEL_VAUX2_VR_H                            3'b100                  MBIAS_REG_MASK
    5:3     R/W MBIAS_SEL_VAUX2_VR_L                            3'b100                  MBIAS_REG_MASK
    8:6     R/W MBIAS_SEL_VAUX3_VR_H                            3'b100                  MBIAS_REG_MASK
    11:9    R/W MBIAS_SEL_VAUX3_VR_L                            3'b100                  MBIAS_REG_MASK
    14:12   R/W MBIAS_SEL_HV_VR_H                               3'b100                  MBIAS_REG_MASK
    15      R/W MBIAS_REG9X_DUMMY1                              1'b0                    MBIAS_REG_MASK
 */
typedef union _AON_FAST_REG9X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_SEL_VAUX2_VR_H: 3;
        uint16_t MBIAS_SEL_VAUX2_VR_L: 3;
        uint16_t MBIAS_SEL_VAUX3_VR_H: 3;
        uint16_t MBIAS_SEL_VAUX3_VR_L: 3;
        uint16_t MBIAS_SEL_HV_VR_H: 3;
        uint16_t MBIAS_REG9X_DUMMY1: 1;
    };
} AON_FAST_REG9X_MBIAS_TYPE;

/* 0xC14
    2:0     R/W MBIAS_SEL_HV_VR_L                               3'b100                  MBIAS_REG_MASK
    5:3     R/W MBIAS_SEL_HV33_VR_H                             3'b100                  MBIAS_REG_MASK
    8:6     R/W MBIAS_SEL_HV33_VR_L                             3'b100                  MBIAS_REG_MASK
    11:9    R/W MBIAS_SEL_HV33_HQ_VR_H                          3'b100                  MBIAS_REG_MASK
    14:12   R/W MBIAS_SEL_HV33_HQ_VR_L                          3'b100                  MBIAS_REG_MASK
    15      R/W MBIAS_REG10X_DUMMY1                             1'b0                    MBIAS_REG_MASK
 */
typedef union _AON_FAST_REG10X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_SEL_HV_VR_L: 3;
        uint16_t MBIAS_SEL_HV33_VR_H: 3;
        uint16_t MBIAS_SEL_HV33_VR_L: 3;
        uint16_t MBIAS_SEL_HV33_HQ_VR_H: 3;
        uint16_t MBIAS_SEL_HV33_HQ_VR_L: 3;
        uint16_t MBIAS_REG10X_DUMMY1: 1;
    };
} AON_FAST_REG10X_MBIAS_TYPE;

/* 0xC16
    2:0     R/W MBIAS_SEL_VAUDIO_VR_H                           3'b100                  MBIAS_REG_MASK
    5:3     R/W MBIAS_SEL_VAUDIO_VR_L                           3'b100                  MBIAS_REG_MASK
    8:6     R/W MBIAS_SEL_VD105_RF_VR_H                         3'b100                  MBIAS_REG_MASK
    11:9    R/W MBIAS_SEL_VD105_RF_VR_L                         3'b100                  MBIAS_REG_MASK
    14:12   R/W MBIAS_SEL_VAUX1_VR_L                            3'b100                  MBIAS_REG_MASK
    15      R/W MBIAS_REG11X_DUMMY1                             1'b0                    MBIAS_REG_MASK
 */
typedef union _AON_FAST_REG11X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_SEL_VAUDIO_VR_H: 3;
        uint16_t MBIAS_SEL_VAUDIO_VR_L: 3;
        uint16_t MBIAS_SEL_VD105_RF_VR_H: 3;
        uint16_t MBIAS_SEL_VD105_RF_VR_L: 3;
        uint16_t MBIAS_SEL_VAUX1_VR_L: 3;
        uint16_t MBIAS_REG11X_DUMMY1: 1;
    };
} AON_FAST_REG11X_MBIAS_TYPE;

/* 0xC18
    2:0     R/W MBIAS_SEL_DVDD_VR_H                             3'b100                  MBIAS_REG_MASK
    5:3     R/W MBIAS_SEL_DVDD_VR_L                             3'b100                  MBIAS_REG_MASK
    8:6     R/W MBIAS_SEL_HV18_VR_H                             3'b100                  MBIAS_REG_MASK
    11:9    R/W MBIAS_SEL_HV18_VR_L                             3'b100                  MBIAS_REG_MASK
    14:12   R/W MBIAS_SEL_VAUX1_VR_H                            3'b100                  MBIAS_REG_MASK
    15      R/W MBIAS_REG12X_DUMMY1                             1'b0                    MBIAS_REG_MASK
 */
typedef union _AON_FAST_REG12X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_SEL_DVDD_VR_H: 3;
        uint16_t MBIAS_SEL_DVDD_VR_L: 3;
        uint16_t MBIAS_SEL_HV18_VR_H: 3;
        uint16_t MBIAS_SEL_HV18_VR_L: 3;
        uint16_t MBIAS_SEL_VAUX1_VR_H: 3;
        uint16_t MBIAS_REG12X_DUMMY1: 1;
    };
} AON_FAST_REG12X_MBIAS_TYPE;

/* 0xC1A
    3:0     R/W MBIAS_REG13X_DUMMY1                             4'b1000                 MBIAS_REG_MASK
    7:4     R/W MBIAS_REG13X_DUMMY2                             4'b1000                 MBIAS_REG_MASK
    8       R/W MBIAS_REG13X_DUMMY3                             1'b0                    MBIAS_REG_MASK
    9       R/W MBIAS_EN_DL_LDOAUDIO_B                          1'b1                    MBIAS_REG_MASK
    10      R   RO_AON_MBIAS_POW_LDOAUDIO_LQ                    1'b0
    15:11   R   RO_AON_MBIAS_TUNE_LDOAUDIO_LQ_VOUT              5'b10000
 */
typedef union _AON_FAST_REG13X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_REG13X_DUMMY1: 4;
        uint16_t MBIAS_REG13X_DUMMY2: 4;
        uint16_t MBIAS_REG13X_DUMMY3: 1;
        uint16_t MBIAS_EN_DL_LDOAUDIO_B: 1;
        uint16_t RO_AON_MBIAS_POW_LDOAUDIO_LQ: 1;
        uint16_t RO_AON_MBIAS_TUNE_LDOAUDIO_LQ_VOUT: 5;
    };
} AON_FAST_REG13X_MBIAS_TYPE;

/* 0xC1C
    1:0     R/W MBIAS_REG14X_DUMMY10                            2'b00                   MBIAS_REG_MASK
    2       R/W MBIAS_REG14X_DUMMY9                             1'b0                    MBIAS_REG_MASK
    3       R/W MBIAS_REG14X_DUMMY8                             1'b0                    MBIAS_REG_MASK
    4       R/W MBIAS_REG14X_DUMMY7                             1'b0                    MBIAS_REG_MASK
    5       R/W MBIAS_REG14X_DUMMY6                             1'b0                    MBIAS_REG_MASK
    6       R/W MBIAS_REG14X_DUMMY5                             1'b0                    MBIAS_REG_MASK
    7       R/W MBIAS_REG14X_DUMMY4                             1'b0                    MBIAS_REG_MASK
    8       R/W MBIAS_REG14X_DUMMY3                             1'b0                    MBIAS_REG_MASK
    9       R/W FAON_PENVDD2_VDD2                               1'b0                    MBIAS_REG_MASK
    15:10   R/W MBIAS_REG14X_DUMMY1                             6'b000000               MBIAS_REG_MASK
 */
typedef union _AON_FAST_REG14X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_REG14X_DUMMY10: 2;
        uint16_t MBIAS_REG14X_DUMMY9: 1;
        uint16_t MBIAS_REG14X_DUMMY8: 1;
        uint16_t MBIAS_REG14X_DUMMY7: 1;
        uint16_t MBIAS_REG14X_DUMMY6: 1;
        uint16_t MBIAS_REG14X_DUMMY5: 1;
        uint16_t MBIAS_REG14X_DUMMY4: 1;
        uint16_t MBIAS_REG14X_DUMMY3: 1;
        uint16_t FAON_PENVDD2_VDD2: 1;
        uint16_t MBIAS_REG14X_DUMMY1: 6;
    };
} AON_FAST_REG14X_MBIAS_TYPE;

/* 0xC1E
    0       R   MBIAS_FLAG_HVBGOK                               1'b0
    1       R   MBIAS_FLAG_EN_LDO_ON                            1'b0
    2       R   MBIAS_FLAG_EN_LDO_ON_DELAY2                     1'b0
    3       R   MBIAS_FLAG_HW_RST_H                             1'b0
    4       R   MBIAS_FLAG_FPOR                                 1'b0
    10:5    R   MBIAS_FLAG_DUMMY1                               1'b0
    11      R   MBIAS_FLAG_MFB2_DET_L                           1'b0
    12      R   MBIAS_FLAG_MFB1_DET_L                           1'b0
    13      R   MBIAS_FLAG_LDO733DPD_DET_L                      1'b0
    14      R   MBIAS_FLAG_VBAT_DET_L                           1'b0
    15      R   MBIAS_FLAG_ADP_DET_L                            1'b0
 */
typedef union _AON_FAST_FLAG0X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_FLAG_HVBGOK: 1;
        uint16_t MBIAS_FLAG_EN_LDO_ON: 1;
        uint16_t MBIAS_FLAG_EN_LDO_ON_DELAY2: 1;
        uint16_t MBIAS_FLAG_HW_RST_H: 1;
        uint16_t MBIAS_FLAG_FPOR: 1;
        uint16_t MBIAS_FLAG_DUMMY1: 6;
        uint16_t MBIAS_FLAG_MFB2_DET_L: 1;
        uint16_t MBIAS_FLAG_MFB1_DET_L: 1;
        uint16_t MBIAS_FLAG_LDO733DPD_DET_L: 1;
        uint16_t MBIAS_FLAG_VBAT_DET_L: 1;
        uint16_t MBIAS_FLAG_ADP_DET_L: 1;
    };
} AON_FAST_FLAG0X_MBIAS_TYPE;

/* 0xC20
    3:0     R   MBIAS_FLAG_ADPIN_DPD_VR_H                       4'b0010
    7:4     R   MBIAS_FLAG_ADPIN_DPD_VR_L                       4'b0010
    10:8    R   MBIAS_FLAG_VBAT_DPD_VR_H                        3'b010
    13:11   R   MBIAS_FLAG_VBAT_DPD_VR_L                        3'b010
    15:14   R   MBIAS_FLAG1_DUMMY1                              2'b00
 */
typedef union _AON_FAST_FLAG1X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_FLAG_ADPIN_DPD_VR_H: 4;
        uint16_t MBIAS_FLAG_ADPIN_DPD_VR_L: 4;
        uint16_t MBIAS_FLAG_VBAT_DPD_VR_H: 3;
        uint16_t MBIAS_FLAG_VBAT_DPD_VR_L: 3;
        uint16_t MBIAS_FLAG1_DUMMY1: 2;
    };
} AON_FAST_FLAG1X_MBIAS_TYPE;

/* 0xC22
    0       R   MBIAS_FLAG_DPD_R0                               1'b0
    1       R   MBIAS_FLAG_DPD_R1                               1'b1
    2       R   MBIAS_FLAG_DPD_R2                               1'b1
    3       R   MBIAS_FLAG_DPD_R3                               1'b1
    4       R   MBIAS_FLAG_DPD_R4                               1'b1
    5       R   MBIAS_FLAG_DPD_R5                               1'b1
    6       R   MBIAS_FLAG_DPD_R6                               1'b1
    7       R   MBIAS_FLAG_DPD_R7                               1'b1
    8       R   MBIAS_FLAG_DPD_R8                               1'b0
    9       R   MBIAS_FLAG_DPD_COMP_HYS                         1'b0
    11:10   R   MBIAS_FLAG_DPD_ADPIN_DR                         2'b00
    13:12   R   MBIAS_FLAG_DPD_VBAT_DR                          2'b00
    15:14   R   MBIAS_FLAG3_DUMMY1                              2'b00
 */
typedef union _AON_FAST_FLAG2X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_FLAG_DPD_R0: 1;
        uint16_t MBIAS_FLAG_DPD_R1: 1;
        uint16_t MBIAS_FLAG_DPD_R2: 1;
        uint16_t MBIAS_FLAG_DPD_R3: 1;
        uint16_t MBIAS_FLAG_DPD_R4: 1;
        uint16_t MBIAS_FLAG_DPD_R5: 1;
        uint16_t MBIAS_FLAG_DPD_R6: 1;
        uint16_t MBIAS_FLAG_DPD_R7: 1;
        uint16_t MBIAS_FLAG_DPD_R8: 1;
        uint16_t MBIAS_FLAG_DPD_COMP_HYS: 1;
        uint16_t MBIAS_FLAG_DPD_ADPIN_DR: 2;
        uint16_t MBIAS_FLAG_DPD_VBAT_DR: 2;
        uint16_t MBIAS_FLAG3_DUMMY1: 2;
    };
} AON_FAST_FLAG2X_MBIAS_TYPE;

/* 0xC24
    0       R   MBIAS_FLAG_DFF_RST_T                            1'b0
    5:1     R   MBIAS_FLAG_DPD_EN                               4'b0000
    7:6     R   MBIAS_FLAG_MFB2_SEL                             2'b00
    9:8     R   MBIAS_FLAG_MFB1_SEL                             2'b00
    12:10   R   MBIAS_FLAG_LDO733DPD_VOUT_TUNE                  3'b000
    15:13   R   MBIAS_FLAG2_DUMMY1                              3'b000
 */
typedef union _AON_FAST_FLAG3X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_FLAG_DFF_RST_T: 1;
        uint16_t MBIAS_FLAG_DPD_EN: 5;
        uint16_t MBIAS_FLAG_MFB2_SEL: 2;
        uint16_t MBIAS_FLAG_MFB1_SEL: 2;
        uint16_t MBIAS_FLAG_LDO733DPD_VOUT_TUNE: 3;
        uint16_t MBIAS_FLAG2_DUMMY1: 3;
    };
} AON_FAST_FLAG3X_MBIAS_TYPE;

/* 0xC26
    0       R   MBIAS_FLAG4_DUMMY16                             1'b0
    1       R   MBIAS_FLAG4_DUMMY15                             1'b0
    2       R   MBIAS_FLAG4_DUMMY14                             1'b0
    3       R   MBIAS_FLAG4_DUMMY13                             1'b0
    4       R   MBIAS_FLAG4_DUMMY12                             1'b0
    5       R   MBIAS_FLAG4_DUMMY11                             1'b0
    6       R   MBIAS_FLAG4_DUMMY10                             1'b0
    7       R   MBIAS_FLAG4_DUMMY9                              1'b0
    8       R   MBIAS_FLAG4_DUMMY8                              1'b0
    9       R   MBIAS_FLAG4_DUMMY7                              1'b0
    10      R   MBIAS_FLAG4_DUMMY6                              1'b0
    11      R   MBIAS_FLAG4_DUMMY5                              1'b0
    12      R   MBIAS_FLAG4_DUMMY4                              1'b0
    13      R   MBIAS_FLAG4_DUMMY3                              1'b0
    14      R   MBIAS_FLAG4_DUMMY2                              1'b0
    15      R   MBIAS_FLAG4_DUMMY1                              1'b0
 */
typedef union _AON_FAST_FLAG4X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_FLAG4_DUMMY16: 1;
        uint16_t MBIAS_FLAG4_DUMMY15: 1;
        uint16_t MBIAS_FLAG4_DUMMY14: 1;
        uint16_t MBIAS_FLAG4_DUMMY13: 1;
        uint16_t MBIAS_FLAG4_DUMMY12: 1;
        uint16_t MBIAS_FLAG4_DUMMY11: 1;
        uint16_t MBIAS_FLAG4_DUMMY10: 1;
        uint16_t MBIAS_FLAG4_DUMMY9: 1;
        uint16_t MBIAS_FLAG4_DUMMY8: 1;
        uint16_t MBIAS_FLAG4_DUMMY7: 1;
        uint16_t MBIAS_FLAG4_DUMMY6: 1;
        uint16_t MBIAS_FLAG4_DUMMY5: 1;
        uint16_t MBIAS_FLAG4_DUMMY4: 1;
        uint16_t MBIAS_FLAG4_DUMMY3: 1;
        uint16_t MBIAS_FLAG4_DUMMY2: 1;
        uint16_t MBIAS_FLAG4_DUMMY1: 1;
    };
} AON_FAST_FLAG4X_MBIAS_TYPE;

/* 0xC28
    0       R   MBIAS_FLAG_BGOK_L                               1'b0
    1       R   MBIAS_FLAG_DVDD_DET_L                           1'b0
    2       R   MBIAS_FLAG_VD105_RF_DET_L                       1'b0
    3       R   MBIAS_FLAG_VAUDIO_DET_L                         1'b0
    4       R   MBIAS_FLAG_VAUX3_DET_L                          1'b0
    5       R   MBIAS_FLAG_VAUX2_DET_L                          1'b0
    6       R   MBIAS_FLAG_VAUX1_DET_L                          1'b0
    7       R   MBIAS_FLAG_HV33_HQ_DET_L                        1'b0
    8       R   MBIAS_FLAG_HV33_AON_DET_L                       1'b0
    9       R   MBIAS_FLAG_HV_DET_L                             1'b0
    10      R   MBIAS_FLAG_HV18_DET_L                           1'b0
    15:11   R   MBIAS_FLAG5_DUMMY1                              5'b00000
 */
typedef union _AON_FAST_FLAG5X_MBIAS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_FLAG_BGOK_L: 1;
        uint16_t MBIAS_FLAG_DVDD_DET_L: 1;
        uint16_t MBIAS_FLAG_VD105_RF_DET_L: 1;
        uint16_t MBIAS_FLAG_VAUDIO_DET_L: 1;
        uint16_t MBIAS_FLAG_VAUX3_DET_L: 1;
        uint16_t MBIAS_FLAG_VAUX2_DET_L: 1;
        uint16_t MBIAS_FLAG_VAUX1_DET_L: 1;
        uint16_t MBIAS_FLAG_HV33_HQ_DET_L: 1;
        uint16_t MBIAS_FLAG_HV33_AON_DET_L: 1;
        uint16_t MBIAS_FLAG_HV_DET_L: 1;
        uint16_t MBIAS_FLAG_HV18_DET_L: 1;
        uint16_t MBIAS_FLAG5_DUMMY1: 5;
    };
} AON_FAST_FLAG5X_MBIAS_TYPE;

/* 0xC2A
    5:0     R/W MBIAS_OSCK_TUNE_LOWER_BND                       6'b000000               MBIAS_REG_MASK
    11:6    R/W MBIAS_OSCK_TUNE_UPPER_BND                       6'b111111               MBIAS_REG_MASK
    12      R/W MBIAS_OSCK_TUNE_POLAR                           1'b0                    MBIAS_REG_MASK
    13      R/W MBIAS_OSCK_REG0X_DUMMY1                         1'b1                    MBIAS_REG_MASK
    14      R/W MBIAS_OSCK_EN_OSCK                              1'b0                    MBIAS_REG_MASK
    15      R/W MBIAS_OSCK_RST_B                                1'b0                    MBIAS_REG_MASK
 */
typedef union _AON_FAST_REG0X_MBIAS_OSCK_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_OSCK_TUNE_LOWER_BND: 6;
        uint16_t MBIAS_OSCK_TUNE_UPPER_BND: 6;
        uint16_t MBIAS_OSCK_TUNE_POLAR: 1;
        uint16_t MBIAS_OSCK_REG0X_DUMMY1: 1;
        uint16_t MBIAS_OSCK_EN_OSCK: 1;
        uint16_t MBIAS_OSCK_RST_B: 1;
    };
} AON_FAST_REG0X_MBIAS_OSCK_TYPE;

/* 0xC2C
    9:0     R/W MBIAS_OSCK_COUNT_TARGET                         10'b0000100000          MBIAS_REG_MASK
    15:10   R/W MBIAS_OSCK_TUNE_INIT_CODE                       6'b100000               MBIAS_REG_MASK
 */
typedef union _AON_FAST_REG1X_MBIAS_OSCK_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_OSCK_COUNT_TARGET: 10;
        uint16_t MBIAS_OSCK_TUNE_INIT_CODE: 6;
    };
} AON_FAST_REG1X_MBIAS_OSCK_TYPE;

/* 0xC2E
    5:0     R/W MBIAS_OSCK_TUNE_MANU_CODE                       6'b100000               MBIAS_REG_MASK
    6       R/W MBIAS_OSCK_SEL_OSC_MANU_MODE                    1'b1                    MBIAS_REG_MASK
    7       R/W MBIAS_OSCK_REG2X_DUMMY3                         1'b1                    MBIAS_REG_MASK
    8       R/W MBIAS_OSCK_REG2X_DUMMY2                         1'b0                    MBIAS_REG_MASK
    15:9    R/W MBIAS_OSCK_REG2X_DUMMY1                         7'b1010101              MBIAS_REG_MASK
 */
typedef union _AON_FAST_REG2X_MBIAS_OSCK_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_OSCK_TUNE_MANU_CODE: 6;
        uint16_t MBIAS_OSCK_SEL_OSC_MANU_MODE: 1;
        uint16_t MBIAS_OSCK_REG2X_DUMMY3: 1;
        uint16_t MBIAS_OSCK_REG2X_DUMMY2: 1;
        uint16_t MBIAS_OSCK_REG2X_DUMMY1: 7;
    };
} AON_FAST_REG2X_MBIAS_OSCK_TYPE;

/* 0xC30
    5:0     R   MBIAS_OSCK_TUNE_OSC_1KHZ_FREQ                   6'b100000
    6       R   MBIAS_OSCK_FLAG_BND_DET                         1'h0
    7       R   MBIAS_OSCK_FLAG_1KOK                            1'h0
    8       R   MBIAS_OSCK_C_KOUT0X_DUMMY8                      1'h0
    9       R   MBIAS_OSCK_C_KOUT0X_DUMMY9                      1'h0
    10      R   MBIAS_OSCK_C_KOUT0X_DUMMY10                     1'h0
    11      R   MBIAS_OSCK_C_KOUT0X_DUMMY11                     1'h0
    12      R   MBIAS_OSCK_C_KOUT0X_DUMMY12                     1'h0
    13      R   MBIAS_OSCK_C_KOUT0X_DUMMY13                     1'h0
    14      R   MBIAS_OSCK_C_KOUT0X_DUMMY14                     1'h0
    15      R   MBIAS_OSCK_C_KOUT0X_DUMMY15                     1'h0
 */
typedef union _AON_FAST_C_KOUT0X_MBIAS_OSCK_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_OSCK_TUNE_OSC_1KHZ_FREQ: 6;
        uint16_t MBIAS_OSCK_FLAG_BND_DET: 1;
        uint16_t MBIAS_OSCK_FLAG_1KOK: 1;
        uint16_t MBIAS_OSCK_C_KOUT0X_DUMMY8: 1;
        uint16_t MBIAS_OSCK_C_KOUT0X_DUMMY9: 1;
        uint16_t MBIAS_OSCK_C_KOUT0X_DUMMY10: 1;
        uint16_t MBIAS_OSCK_C_KOUT0X_DUMMY11: 1;
        uint16_t MBIAS_OSCK_C_KOUT0X_DUMMY12: 1;
        uint16_t MBIAS_OSCK_C_KOUT0X_DUMMY13: 1;
        uint16_t MBIAS_OSCK_C_KOUT0X_DUMMY14: 1;
        uint16_t MBIAS_OSCK_C_KOUT0X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT0X_MBIAS_OSCK_TYPE;

/* 0xC32
    9:0     R   MBIAS_OSCK_COUNT_LATCH                          10'h0
    13:10   R   MBIAS_OSCK_DIGITAL_STATE                        4'h0
    14      R   MBIAS_OSCK_C_KOUT1X_DUMMY14                     1'h0
    15      R   MBIAS_OSCK_C_KOUT1X_DUMMY15                     1'h0
 */
typedef union _AON_FAST_C_KOUT1X_MBIAS_OSCK_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t MBIAS_OSCK_COUNT_LATCH: 10;
        uint16_t MBIAS_OSCK_DIGITAL_STATE: 4;
        uint16_t MBIAS_OSCK_C_KOUT1X_DUMMY14: 1;
        uint16_t MBIAS_OSCK_C_KOUT1X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT1X_MBIAS_OSCK_TYPE;

/* 0xC80
    7:0     R   RO_AON_LDOSYS_TUNE_LDO533_VOUT                  8'b10111100
    10:8    R/W LDOSYS_TUNE_VREF_LDO533                         3'b000                  LDOSYS_REG_MASK
    11      R   RO_AON_LDOSYS_EN_VO_PD                          1'b0
    12      R/W LDOSYS_EN_TR_BST                                1'b1                    LDOSYS_REG_MASK
    13      R/W LDOSYS_EN_DL_3P5U_B                             1'b0                    LDOSYS_REG_MASK
    14      R   RO_AON_LDOSYS_EN_VREF_LDO533                    1'b0
    15      R   RO_AON_LDOSYS_POW_LDO533                        1'b0
 */
typedef union _AON_FAST_REG0X_LDOSYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_LDOSYS_TUNE_LDO533_VOUT: 8;
        uint16_t LDOSYS_TUNE_VREF_LDO533: 3;
        uint16_t RO_AON_LDOSYS_EN_VO_PD: 1;
        uint16_t LDOSYS_EN_TR_BST: 1;
        uint16_t LDOSYS_EN_DL_3P5U_B: 1;
        uint16_t RO_AON_LDOSYS_EN_VREF_LDO533: 1;
        uint16_t RO_AON_LDOSYS_POW_LDO533: 1;
    };
} AON_FAST_REG0X_LDOSYS_TYPE;

/* 0xC82
    2:0     R/W LDOSYS_SEL_ISET                                 3'b100                  LDOSYS_REG_MASK
    5:3     R/W LDOSYS_SEL_DL                                   3'b111                  LDOSYS_REG_MASK
    10:6    R/W LDOSYS_REG1X_DUMMY2                             5'b00000                LDOSYS_REG_MASK
    11      R/W LDOSYS_EN_FB_LOCAL_B                            1'b1                    LDOSYS_REG_MASK
    12      R/W LDOSYS_REG1X_DUMMY1                             1'b0                    LDOSYS_REG_MASK
    13      R/W LDOSYS_SEL_VREF_LDO533                          1'b0                    LDOSYS_REG_MASK
    14      R/W LDOSYS_EN_FAST_STARTUP                          1'b1                    LDOSYS_REG_MASK
    15      R/W LDOSYS_EN_LOWIQ                                 1'b0                    LDOSYS_REG_MASK
 */
typedef union _AON_FAST_REG1X_LDOSYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_SEL_ISET: 3;
        uint16_t LDOSYS_SEL_DL: 3;
        uint16_t LDOSYS_REG1X_DUMMY2: 5;
        uint16_t LDOSYS_EN_FB_LOCAL_B: 1;
        uint16_t LDOSYS_REG1X_DUMMY1: 1;
        uint16_t LDOSYS_SEL_VREF_LDO533: 1;
        uint16_t LDOSYS_EN_FAST_STARTUP: 1;
        uint16_t LDOSYS_EN_LOWIQ: 1;
    };
} AON_FAST_REG1X_LDOSYS_TYPE;

/* 0xC84
    2:0     R/W LDOSYS_SEL_VREF_MANUAL                          3'b000                  LDOSYS_REG_MASK
    9:3     R/W LDOSYS_REG2X_DUMMY1                             7'b0000000              LDOSYS_REG_MASK
    14:10   R/W LDOSYS_SEL_DUMMYLOAD                            5'b00000                LDOSYS_REG_MASK
    15      R/W LDOSYS_EN_DUMMYLOAD                             1'b0                    LDOSYS_REG_MASK
 */
typedef union _AON_FAST_REG2X_LDOSYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_SEL_VREF_MANUAL: 3;
        uint16_t LDOSYS_REG2X_DUMMY1: 7;
        uint16_t LDOSYS_SEL_DUMMYLOAD: 5;
        uint16_t LDOSYS_EN_DUMMYLOAD: 1;
    };
} AON_FAST_REG2X_LDOSYS_TYPE;

/* 0xC86
    2:0     R/W LDOSYS_REG3X_DUMMY1                             3'b000                  LDOSYS_REG_MASK
    3       R/W LDOSYS_EN_733TOOTP_PC                           1'b1                    LDOSYS_REG_MASK
    4       R/W LDOSYS_EN_VREF_LDO733                           1'b0                    LDOSYS_REG_MASK
    5       R   RO_AON_LDOSYS_EN_733TO533_PC                    1'b0
    12:6    R   RO_AON_LDOSYS_TUNE_LDO733_VOUT                  7'b1011000
    13      R/W LDOSYS_EN_DL_B                                  1'b1                    LDOSYS_REG_MASK
    14      R/W LDOSYS_REG3X_DUMMY2                             1'b0                    LDOSYS_REG_MASK
    15      R/W LDOSYS_EN_SSOC_B                                1'b1                    LDOSYS_REG_MASK
 */
typedef union _AON_FAST_REG3X_LDOSYS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_REG3X_DUMMY1: 3;
        uint16_t LDOSYS_EN_733TOOTP_PC: 1;
        uint16_t LDOSYS_EN_VREF_LDO733: 1;
        uint16_t RO_AON_LDOSYS_EN_733TO533_PC: 1;
        uint16_t RO_AON_LDOSYS_TUNE_LDO733_VOUT: 7;
        uint16_t LDOSYS_EN_DL_B: 1;
        uint16_t LDOSYS_REG3X_DUMMY2: 1;
        uint16_t LDOSYS_EN_SSOC_B: 1;
    };
} AON_FAST_REG3X_LDOSYS_TYPE;

/* 0xC88
    0       R/W LDOSYS_POS_INT_RST_B                            1'b0                    LDOSYS_REG_MASK
    1       R   RO_AON_LDOSYS_POS_EXT_RST_B                     1'b0
    2       R/W LDOSYS_POS_SEL_EXT_RST_B                        1'b0                    LDOSYS_REG_MASK
    3       R   RO_AON_LDOSYS_POS_EN_POS                        1'b0
    6:4     R/W LDOSYS_POS_DIV_CLK                              3'b010                  LDOSYS_REG_MASK
    7       R/W LDOSYS_POS_REG0X_DUMMY7                         1'b0                    LDOSYS_REG_MASK
    8       R/W LDOSYS_POS_REG0X_DUMMY8                         1'b0                    LDOSYS_REG_MASK
    9       R/W LDOSYS_POS_REG0X_DUMMY9                         1'b0                    LDOSYS_REG_MASK
    10      R/W LDOSYS_POS_REG0X_DUMMY10                        1'b0                    LDOSYS_REG_MASK
    11      R/W LDOSYS_POS_REG0X_DUMMY11                        1'b0                    LDOSYS_REG_MASK
    12      R/W LDOSYS_POS_REG0X_DUMMY12                        1'b0                    LDOSYS_REG_MASK
    13      R/W LDOSYS_POS_REG0X_DUMMY13                        1'b0                    LDOSYS_REG_MASK
    14      R/W LDOSYS_POS_REG0X_DUMMY14                        1'b0                    LDOSYS_REG_MASK
    15      R/W LDOSYS_POS_REG0X_DUMMY15                        1'b0                    LDOSYS_REG_MASK
 */
typedef union _AON_FAST_REG0X_LDOSYS_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_POS_INT_RST_B: 1;
        uint16_t RO_AON_LDOSYS_POS_EXT_RST_B: 1;
        uint16_t LDOSYS_POS_SEL_EXT_RST_B: 1;
        uint16_t RO_AON_LDOSYS_POS_EN_POS: 1;
        uint16_t LDOSYS_POS_DIV_CLK: 3;
        uint16_t LDOSYS_POS_REG0X_DUMMY7: 1;
        uint16_t LDOSYS_POS_REG0X_DUMMY8: 1;
        uint16_t LDOSYS_POS_REG0X_DUMMY9: 1;
        uint16_t LDOSYS_POS_REG0X_DUMMY10: 1;
        uint16_t LDOSYS_POS_REG0X_DUMMY11: 1;
        uint16_t LDOSYS_POS_REG0X_DUMMY12: 1;
        uint16_t LDOSYS_POS_REG0X_DUMMY13: 1;
        uint16_t LDOSYS_POS_REG0X_DUMMY14: 1;
        uint16_t LDOSYS_POS_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_LDOSYS_POS_TYPE;

/* 0xC8A
    3:0     R/W LDOSYS_POS_POF_STEP                             4'b0001                 LDOSYS_REG_MASK
    7:4     R/W LDOSYS_POS_PON_STEP                             4'b0001                 LDOSYS_REG_MASK
    10:8    R/W LDOSYS_POS_POF_WAIT                             3'b000                  LDOSYS_REG_MASK
    13:11   R/W LDOSYS_POS_PON_WAIT                             3'b000                  LDOSYS_REG_MASK
    14      R/W LDOSYS_POS_REG1X_DUMMY14                        1'b0                    LDOSYS_REG_MASK
    15      R/W LDOSYS_POS_REG1X_DUMMY15                        1'b0                    LDOSYS_REG_MASK
 */
typedef union _AON_FAST_REG1X_LDOSYS_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_POS_POF_STEP: 4;
        uint16_t LDOSYS_POS_PON_STEP: 4;
        uint16_t LDOSYS_POS_POF_WAIT: 3;
        uint16_t LDOSYS_POS_PON_WAIT: 3;
        uint16_t LDOSYS_POS_REG1X_DUMMY14: 1;
        uint16_t LDOSYS_POS_REG1X_DUMMY15: 1;
    };
} AON_FAST_REG1X_LDOSYS_POS_TYPE;

/* 0xC8C
    7:0     R/W LDOSYS_POS_PON_OVER                             8'b00000111             LDOSYS_REG_MASK
    15:8    R/W LDOSYS_POS_PON_START                            8'b00000000             LDOSYS_REG_MASK
 */
typedef union _AON_FAST_REG2X_LDOSYS_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_POS_PON_OVER: 8;
        uint16_t LDOSYS_POS_PON_START: 8;
    };
} AON_FAST_REG2X_LDOSYS_POS_TYPE;

/* 0xC8E
    7:0     R/W LDOSYS_POS_POF_OVER                             8'b00000000             LDOSYS_REG_MASK
    15:8    R/W LDOSYS_POS_POF_START                            8'b00000111             LDOSYS_REG_MASK
 */
typedef union _AON_FAST_REG3X_LDOSYS_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_POS_POF_OVER: 8;
        uint16_t LDOSYS_POS_POF_START: 8;
    };
} AON_FAST_REG3X_LDOSYS_POS_TYPE;

/* 0xC90
    7:0     R/W LDOSYS_POS_SET_PON_FLAG2                        8'b00000001             LDOSYS_REG_MASK
    15:8    R/W LDOSYS_POS_SET_PON_FLAG1                        8'b00000000             LDOSYS_REG_MASK
 */
typedef union _AON_FAST_REG4X_LDOSYS_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_POS_SET_PON_FLAG2: 8;
        uint16_t LDOSYS_POS_SET_PON_FLAG1: 8;
    };
} AON_FAST_REG4X_LDOSYS_POS_TYPE;

/* 0xC92
    7:0     R/W LDOSYS_POS_SET_PON_FLAG4                        8'b00000111             LDOSYS_REG_MASK
    15:8    R/W LDOSYS_POS_SET_PON_FLAG3                        8'b00000110             LDOSYS_REG_MASK
 */
typedef union _AON_FAST_REG5X_LDOSYS_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_POS_SET_PON_FLAG4: 8;
        uint16_t LDOSYS_POS_SET_PON_FLAG3: 8;
    };
} AON_FAST_REG5X_LDOSYS_POS_TYPE;

/* 0xC94
    7:0     R/W LDOSYS_POS_SET_POF_FLAG2                        8'b00000110             LDOSYS_REG_MASK
    15:8    R/W LDOSYS_POS_SET_POF_FLAG1                        8'b00000111             LDOSYS_REG_MASK
 */
typedef union _AON_FAST_REG6X_LDOSYS_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_POS_SET_POF_FLAG2: 8;
        uint16_t LDOSYS_POS_SET_POF_FLAG1: 8;
    };
} AON_FAST_REG6X_LDOSYS_POS_TYPE;

/* 0xC96
    7:0     R/W LDOSYS_POS_SET_POF_FLAG4                        8'b00000000             LDOSYS_REG_MASK
    15:8    R/W LDOSYS_POS_SET_POF_FLAG3                        8'b00000001             LDOSYS_REG_MASK
 */
typedef union _AON_FAST_REG7X_LDOSYS_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_POS_SET_POF_FLAG4: 8;
        uint16_t LDOSYS_POS_SET_POF_FLAG3: 8;
    };
} AON_FAST_REG7X_LDOSYS_POS_TYPE;

/* 0xC98
    0       R   LDOSYS_POS_POF_FLAG4                            1'b0
    1       R   LDOSYS_POS_POF_FLAG3                            1'b0
    2       R   LDOSYS_POS_POF_FLAG2                            1'b0
    3       R   LDOSYS_POS_POF_FLAG1                            1'b0
    4       R   LDOSYS_POS_PON_FLAG4                            1'b0
    5       R   LDOSYS_POS_PON_FLAG3                            1'b0
    6       R   LDOSYS_POS_PON_FLAG2                            1'b0
    7       R   LDOSYS_POS_PON_FLAG1                            1'b0
    10:8    R   LDOSYS_POS_FSM_CS                               3'h0
    11      R   LDOSYS_POS_C_KOUT0X_DUMMY11                     1'b0
    12      R   LDOSYS_POS_C_KOUT0X_DUMMY12                     1'b0
    13      R   LDOSYS_POS_C_KOUT0X_DUMMY13                     1'b0
    14      R   LDOSYS_POS_C_KOUT0X_DUMMY14                     1'b0
    15      R   LDOSYS_POS_C_KOUT0X_DUMMY15                     1'b0
 */
typedef union _AON_FAST_C_KOUT0X_LDOSYS_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_POS_POF_FLAG4: 1;
        uint16_t LDOSYS_POS_POF_FLAG3: 1;
        uint16_t LDOSYS_POS_POF_FLAG2: 1;
        uint16_t LDOSYS_POS_POF_FLAG1: 1;
        uint16_t LDOSYS_POS_PON_FLAG4: 1;
        uint16_t LDOSYS_POS_PON_FLAG3: 1;
        uint16_t LDOSYS_POS_PON_FLAG2: 1;
        uint16_t LDOSYS_POS_PON_FLAG1: 1;
        uint16_t LDOSYS_POS_FSM_CS: 3;
        uint16_t LDOSYS_POS_C_KOUT0X_DUMMY11: 1;
        uint16_t LDOSYS_POS_C_KOUT0X_DUMMY12: 1;
        uint16_t LDOSYS_POS_C_KOUT0X_DUMMY13: 1;
        uint16_t LDOSYS_POS_C_KOUT0X_DUMMY14: 1;
        uint16_t LDOSYS_POS_C_KOUT0X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT0X_LDOSYS_POS_TYPE;

/* 0xC9A
    7:0     R   LDOSYS_POS_DR                                   8'h0
    8       R   LDOSYS_POS_C_KOUT1X_DUMMY8                      1'h0
    9       R   LDOSYS_POS_C_KOUT1X_DUMMY9                      1'h0
    10      R   LDOSYS_POS_C_KOUT1X_DUMMY10                     1'h0
    11      R   LDOSYS_POS_C_KOUT1X_DUMMY11                     1'h0
    12      R   LDOSYS_POS_C_KOUT1X_DUMMY12                     1'h0
    13      R   LDOSYS_POS_C_KOUT1X_DUMMY13                     1'h0
    14      R   LDOSYS_POS_C_KOUT1X_DUMMY14                     1'h0
    15      R   LDOSYS_POS_C_KOUT1X_DUMMY15                     1'h0
 */
typedef union _AON_FAST_C_KOUT1X_LDOSYS_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOSYS_POS_DR: 8;
        uint16_t LDOSYS_POS_C_KOUT1X_DUMMY8: 1;
        uint16_t LDOSYS_POS_C_KOUT1X_DUMMY9: 1;
        uint16_t LDOSYS_POS_C_KOUT1X_DUMMY10: 1;
        uint16_t LDOSYS_POS_C_KOUT1X_DUMMY11: 1;
        uint16_t LDOSYS_POS_C_KOUT1X_DUMMY12: 1;
        uint16_t LDOSYS_POS_C_KOUT1X_DUMMY13: 1;
        uint16_t LDOSYS_POS_C_KOUT1X_DUMMY14: 1;
        uint16_t LDOSYS_POS_C_KOUT1X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT1X_LDOSYS_POS_TYPE;

/* 0xCE0
    7:0     R   RO_AON_LDOAUX3_TUNE_LDOHQ1_VOUT                 8'b10111100
    10:8    R/W LDOAUX3_TUNE_VREF                               3'b000                  LDOAUX3_REG_MASK
    11      R   RO_AON_LDOAUX3_EN_VO_PD                         1'b0
    12      R/W LDOAUX3_EN_TR_BST                               1'b1                    LDOAUX3_REG_MASK
    13      R/W LDOAUX3_EN_DL_3P5U_B                            1'b0                    LDOAUX3_REG_MASK
    14      R   RO_AON_LDOAUX3_EN_VREF                          1'b0
    15      R   RO_AON_LDOAUX3_POW_LDOHQ1                       1'b0
 */
typedef union _AON_FAST_REG0X_LDOAUX3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_LDOAUX3_TUNE_LDOHQ1_VOUT: 8;
        uint16_t LDOAUX3_TUNE_VREF: 3;
        uint16_t RO_AON_LDOAUX3_EN_VO_PD: 1;
        uint16_t LDOAUX3_EN_TR_BST: 1;
        uint16_t LDOAUX3_EN_DL_3P5U_B: 1;
        uint16_t RO_AON_LDOAUX3_EN_VREF: 1;
        uint16_t RO_AON_LDOAUX3_POW_LDOHQ1: 1;
    };
} AON_FAST_REG0X_LDOAUX3_TYPE;

/* 0xCE2
    2:0     R/W LDOAUX3_SEL_ISET                                3'b100                  LDOAUX3_REG_MASK
    5:3     R/W LDOAUX3_SEL_DL                                  3'b111                  LDOAUX3_REG_MASK
    10:6    R/W LDOAUX3_REG1X_DUMMY2                            5'b00000                LDOAUX3_REG_MASK
    11      R/W LDOAUX3_EN_FB_LOCAL_B                           1'b0                    LDOAUX3_REG_MASK
    12      R/W LDOAUX3_REG1X_DUMMY1                            1'b0                    LDOAUX3_REG_MASK
    13      R/W LDOAUX3_SEL_VREF                                1'b0                    LDOAUX3_REG_MASK
    14      R/W LDOAUX3_EN_FAST_STARTUP                         1'b1                    LDOAUX3_REG_MASK
    15      R/W LDOAUX3_EN_LOWIQ                                1'b0                    LDOAUX3_REG_MASK
 */
typedef union _AON_FAST_REG1X_LDOAUX3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX3_SEL_ISET: 3;
        uint16_t LDOAUX3_SEL_DL: 3;
        uint16_t LDOAUX3_REG1X_DUMMY2: 5;
        uint16_t LDOAUX3_EN_FB_LOCAL_B: 1;
        uint16_t LDOAUX3_REG1X_DUMMY1: 1;
        uint16_t LDOAUX3_SEL_VREF: 1;
        uint16_t LDOAUX3_EN_FAST_STARTUP: 1;
        uint16_t LDOAUX3_EN_LOWIQ: 1;
    };
} AON_FAST_REG1X_LDOAUX3_TYPE;

/* 0xCE4
    2:0     R/W LDOAUX3_SEL_VREF_MANUAL                         3'b000                  LDOAUX3_REG_MASK
    9:3     R/W LDOAUX3_REG2X_DUMMY1                            7'b0000000              LDOAUX3_REG_MASK
    14:10   R/W LDOAUX3_SEL_DUMMYLOAD                           5'b00000                LDOAUX3_REG_MASK
    15      R/W LDOAUX3_EN_DUMMYLOAD                            1'b0                    LDOAUX3_REG_MASK
 */
typedef union _AON_FAST_REG2X_LDOAUX3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX3_SEL_VREF_MANUAL: 3;
        uint16_t LDOAUX3_REG2X_DUMMY1: 7;
        uint16_t LDOAUX3_SEL_DUMMYLOAD: 5;
        uint16_t LDOAUX3_EN_DUMMYLOAD: 1;
    };
} AON_FAST_REG2X_LDOAUX3_TYPE;

/* 0xCE6
    0       R/W LDOAUX3_POS_INT_RST_B                           1'b0                    LDOAUX3_REG_MASK
    1       R   RO_AON_LDOAUX3_POS_EXT_RST_B                    1'b0
    2       R/W LDOAUX3_POS_SEL_EXT_RST_B                       1'b0                    LDOAUX3_REG_MASK
    3       R   RO_AON_LDOAUX3_POS_EN_POS                       1'b0
    6:4     R/W LDOAUX3_POS_DIV_CLK                             3'b010                  LDOAUX3_REG_MASK
    7       R/W LDOAUX3_POS_REG0X_DUMMY7                        1'b0                    LDOAUX3_REG_MASK
    8       R/W LDOAUX3_POS_REG0X_DUMMY8                        1'b0                    LDOAUX3_REG_MASK
    9       R/W LDOAUX3_POS_REG0X_DUMMY9                        1'b0                    LDOAUX3_REG_MASK
    10      R/W LDOAUX3_POS_REG0X_DUMMY10                       1'b0                    LDOAUX3_REG_MASK
    11      R/W LDOAUX3_POS_REG0X_DUMMY11                       1'b0                    LDOAUX3_REG_MASK
    12      R/W LDOAUX3_POS_REG0X_DUMMY12                       1'b0                    LDOAUX3_REG_MASK
    13      R/W LDOAUX3_POS_REG0X_DUMMY13                       1'b0                    LDOAUX3_REG_MASK
    14      R/W LDOAUX3_POS_REG0X_DUMMY14                       1'b0                    LDOAUX3_REG_MASK
    15      R/W LDOAUX3_POS_REG0X_DUMMY15                       1'b0                    LDOAUX3_REG_MASK
 */
typedef union _AON_FAST_REG0X_LDOAUX3_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX3_POS_INT_RST_B: 1;
        uint16_t RO_AON_LDOAUX3_POS_EXT_RST_B: 1;
        uint16_t LDOAUX3_POS_SEL_EXT_RST_B: 1;
        uint16_t RO_AON_LDOAUX3_POS_EN_POS: 1;
        uint16_t LDOAUX3_POS_DIV_CLK: 3;
        uint16_t LDOAUX3_POS_REG0X_DUMMY7: 1;
        uint16_t LDOAUX3_POS_REG0X_DUMMY8: 1;
        uint16_t LDOAUX3_POS_REG0X_DUMMY9: 1;
        uint16_t LDOAUX3_POS_REG0X_DUMMY10: 1;
        uint16_t LDOAUX3_POS_REG0X_DUMMY11: 1;
        uint16_t LDOAUX3_POS_REG0X_DUMMY12: 1;
        uint16_t LDOAUX3_POS_REG0X_DUMMY13: 1;
        uint16_t LDOAUX3_POS_REG0X_DUMMY14: 1;
        uint16_t LDOAUX3_POS_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_LDOAUX3_POS_TYPE;

/* 0xCE8
    3:0     R/W LDOAUX3_POS_POF_STEP                            4'b0001                 LDOAUX3_REG_MASK
    7:4     R/W LDOAUX3_POS_PON_STEP                            4'b0001                 LDOAUX3_REG_MASK
    10:8    R/W LDOAUX3_POS_POF_WAIT                            3'b000                  LDOAUX3_REG_MASK
    13:11   R/W LDOAUX3_POS_PON_WAIT                            3'b000                  LDOAUX3_REG_MASK
    14      R/W LDOAUX3_POS_REG1X_DUMMY14                       1'b0                    LDOAUX3_REG_MASK
    15      R/W LDOAUX3_POS_REG1X_DUMMY15                       1'b0                    LDOAUX3_REG_MASK
 */
typedef union _AON_FAST_REG1X_LDOAUX3_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX3_POS_POF_STEP: 4;
        uint16_t LDOAUX3_POS_PON_STEP: 4;
        uint16_t LDOAUX3_POS_POF_WAIT: 3;
        uint16_t LDOAUX3_POS_PON_WAIT: 3;
        uint16_t LDOAUX3_POS_REG1X_DUMMY14: 1;
        uint16_t LDOAUX3_POS_REG1X_DUMMY15: 1;
    };
} AON_FAST_REG1X_LDOAUX3_POS_TYPE;

/* 0xCEA
    7:0     R/W LDOAUX3_POS_PON_OVER                            8'b00000111             LDOAUX3_REG_MASK
    15:8    R/W LDOAUX3_POS_PON_START                           8'b00000000             LDOAUX3_REG_MASK
 */
typedef union _AON_FAST_REG2X_LDOAUX3_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX3_POS_PON_OVER: 8;
        uint16_t LDOAUX3_POS_PON_START: 8;
    };
} AON_FAST_REG2X_LDOAUX3_POS_TYPE;

/* 0xCEC
    7:0     R/W LDOAUX3_POS_POF_OVER                            8'b00000000             LDOAUX3_REG_MASK
    15:8    R/W LDOAUX3_POS_POF_START                           8'b00000111             LDOAUX3_REG_MASK
 */
typedef union _AON_FAST_REG3X_LDOAUX3_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX3_POS_POF_OVER: 8;
        uint16_t LDOAUX3_POS_POF_START: 8;
    };
} AON_FAST_REG3X_LDOAUX3_POS_TYPE;

/* 0xCEE
    7:0     R/W LDOAUX3_POS_SET_PON_FLAG2                       8'b00000001             LDOAUX3_REG_MASK
    15:8    R/W LDOAUX3_POS_SET_PON_FLAG1                       8'b00000000             LDOAUX3_REG_MASK
 */
typedef union _AON_FAST_REG4X_LDOAUX3_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX3_POS_SET_PON_FLAG2: 8;
        uint16_t LDOAUX3_POS_SET_PON_FLAG1: 8;
    };
} AON_FAST_REG4X_LDOAUX3_POS_TYPE;

/* 0xCF0
    7:0     R/W LDOAUX3_POS_SET_PON_FLAG4                       8'b00000111             LDOAUX3_REG_MASK
    15:8    R/W LDOAUX3_POS_SET_PON_FLAG3                       8'b00000110             LDOAUX3_REG_MASK
 */
typedef union _AON_FAST_REG5X_LDOAUX3_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX3_POS_SET_PON_FLAG4: 8;
        uint16_t LDOAUX3_POS_SET_PON_FLAG3: 8;
    };
} AON_FAST_REG5X_LDOAUX3_POS_TYPE;

/* 0xCF2
    7:0     R/W LDOAUX3_POS_SET_POF_FLAG2                       8'b00000110             LDOAUX3_REG_MASK
    15:8    R/W LDOAUX3_POS_SET_POF_FLAG1                       8'b00000111             LDOAUX3_REG_MASK
 */
typedef union _AON_FAST_REG6X_LDOAUX3_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX3_POS_SET_POF_FLAG2: 8;
        uint16_t LDOAUX3_POS_SET_POF_FLAG1: 8;
    };
} AON_FAST_REG6X_LDOAUX3_POS_TYPE;

/* 0xCF4
    7:0     R/W LDOAUX3_POS_SET_POF_FLAG4                       8'b00000000             LDOAUX3_REG_MASK
    15:8    R/W LDOAUX3_POS_SET_POF_FLAG3                       8'b00000001             LDOAUX3_REG_MASK
 */
typedef union _AON_FAST_REG7X_LDOAUX3_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX3_POS_SET_POF_FLAG4: 8;
        uint16_t LDOAUX3_POS_SET_POF_FLAG3: 8;
    };
} AON_FAST_REG7X_LDOAUX3_POS_TYPE;

/* 0xCF6
    0       R   LDOAUX3_POS_POF_FLAG4                           1'b0
    1       R   LDOAUX3_POS_POF_FLAG3                           1'b0
    2       R   LDOAUX3_POS_POF_FLAG2                           1'b0
    3       R   LDOAUX3_POS_POF_FLAG1                           1'b0
    4       R   LDOAUX3_POS_PON_FLAG4                           1'b0
    5       R   LDOAUX3_POS_PON_FLAG3                           1'b0
    6       R   LDOAUX3_POS_PON_FLAG2                           1'b0
    7       R   LDOAUX3_POS_PON_FLAG1                           1'b0
    10:8    R   LDOAUX3_POS_FSM_CS                              3'h0
    11      R   LDOAUX3_POS_C_KOUT0X_DUMMY11                    1'b0
    12      R   LDOAUX3_POS_C_KOUT0X_DUMMY12                    1'b0
    13      R   LDOAUX3_POS_C_KOUT0X_DUMMY13                    1'b0
    14      R   LDOAUX3_POS_C_KOUT0X_DUMMY14                    1'b0
    15      R   LDOAUX3_POS_C_KOUT0X_DUMMY15                    1'b0
 */
typedef union _AON_FAST_C_KOUT0X_LDOAUX3_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX3_POS_POF_FLAG4: 1;
        uint16_t LDOAUX3_POS_POF_FLAG3: 1;
        uint16_t LDOAUX3_POS_POF_FLAG2: 1;
        uint16_t LDOAUX3_POS_POF_FLAG1: 1;
        uint16_t LDOAUX3_POS_PON_FLAG4: 1;
        uint16_t LDOAUX3_POS_PON_FLAG3: 1;
        uint16_t LDOAUX3_POS_PON_FLAG2: 1;
        uint16_t LDOAUX3_POS_PON_FLAG1: 1;
        uint16_t LDOAUX3_POS_FSM_CS: 3;
        uint16_t LDOAUX3_POS_C_KOUT0X_DUMMY11: 1;
        uint16_t LDOAUX3_POS_C_KOUT0X_DUMMY12: 1;
        uint16_t LDOAUX3_POS_C_KOUT0X_DUMMY13: 1;
        uint16_t LDOAUX3_POS_C_KOUT0X_DUMMY14: 1;
        uint16_t LDOAUX3_POS_C_KOUT0X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT0X_LDOAUX3_POS_TYPE;

/* 0xCF8
    7:0     R   LDOAUX3_POS_DR                                  8'h0
    8       R   LDOAUX3_POS_C_KOUT1X_DUMMY8                     1'h0
    9       R   LDOAUX3_POS_C_KOUT1X_DUMMY9                     1'h0
    10      R   LDOAUX3_POS_C_KOUT1X_DUMMY10                    1'h0
    11      R   LDOAUX3_POS_C_KOUT1X_DUMMY11                    1'h0
    12      R   LDOAUX3_POS_C_KOUT1X_DUMMY12                    1'h0
    13      R   LDOAUX3_POS_C_KOUT1X_DUMMY13                    1'h0
    14      R   LDOAUX3_POS_C_KOUT1X_DUMMY14                    1'h0
    15      R   LDOAUX3_POS_C_KOUT1X_DUMMY15                    1'h0
 */
typedef union _AON_FAST_C_KOUT1X_LDOAUX3_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX3_POS_DR: 8;
        uint16_t LDOAUX3_POS_C_KOUT1X_DUMMY8: 1;
        uint16_t LDOAUX3_POS_C_KOUT1X_DUMMY9: 1;
        uint16_t LDOAUX3_POS_C_KOUT1X_DUMMY10: 1;
        uint16_t LDOAUX3_POS_C_KOUT1X_DUMMY11: 1;
        uint16_t LDOAUX3_POS_C_KOUT1X_DUMMY12: 1;
        uint16_t LDOAUX3_POS_C_KOUT1X_DUMMY13: 1;
        uint16_t LDOAUX3_POS_C_KOUT1X_DUMMY14: 1;
        uint16_t LDOAUX3_POS_C_KOUT1X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT1X_LDOAUX3_POS_TYPE;

/* 0xD30
    3:0     R/W LDOAUX3_LQ_REG2X_DUMMY2                         4'b0000                 LDOAUX3_REG_MASK
    4       R/W LDOAUX3_LQ_EN_VREF_LDO733                       1'b0                    LDOAUX3_REG_MASK
    5       R/W LDOAUX3_LQ_REG2X_DUMMY1                         1'b0                    LDOAUX3_REG_MASK
    12:6    R   RO_AON_LDOAUX3_LQ_TUNE_LDO_VOUT                 7'b0111000
    13      R/W LDOAUX3_LQ_EN_DL_B                              1'b1                    LDOAUX3_REG_MASK
    14      R   RO_AON_LDOAUX3_LQ_POW_LDO                       1'b0
    15      R/W LDOAUX3_LQ_EN_SSOC_B                            1'b1                    LDOAUX3_REG_MASK
 */
typedef union _AON_FAST_REG3X_LDOAUX3_LQ_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX3_LQ_REG2X_DUMMY2: 4;
        uint16_t LDOAUX3_LQ_EN_VREF_LDO733: 1;
        uint16_t LDOAUX3_LQ_REG2X_DUMMY1: 1;
        uint16_t RO_AON_LDOAUX3_LQ_TUNE_LDO_VOUT: 7;
        uint16_t LDOAUX3_LQ_EN_DL_B: 1;
        uint16_t RO_AON_LDOAUX3_LQ_POW_LDO: 1;
        uint16_t LDOAUX3_LQ_EN_SSOC_B: 1;
    };
} AON_FAST_REG3X_LDOAUX3_LQ_TYPE;

/* 0xD50
    7:0     R   RO_AON_LDOAUX1_TUNE_LDO_VOUT                    8'b10111100
    8       R/W LDOAUX1_REG0X_DUMMY2                            1'b0                    LDOAUX1_REG_MASK
    9       R/W LDOAUX1_REG0X_DUMMY1                            1'b0                    LDOAUX1_REG_MASK
    10      R   RO_AON_LDOAUX1_EN_LO_IQ2                        1'b0
    12:11   R   RO_AON_LDOAUX1_SEL_BIAS                         2'b00
    13      R   RO_AON_LDOAUX1_POW_BIAS                         1'b0
    14      R   RO_AON_LDOAUX1_EN_HI_IQ                         1'b0
    15      R   RO_AON_LDOAUX1_POW_LDO                          1'b0
 */
typedef union _AON_FAST_REG0X_LDOAUX1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_LDOAUX1_TUNE_LDO_VOUT: 8;
        uint16_t LDOAUX1_REG0X_DUMMY2: 1;
        uint16_t LDOAUX1_REG0X_DUMMY1: 1;
        uint16_t RO_AON_LDOAUX1_EN_LO_IQ2: 1;
        uint16_t RO_AON_LDOAUX1_SEL_BIAS: 2;
        uint16_t RO_AON_LDOAUX1_POW_BIAS: 1;
        uint16_t RO_AON_LDOAUX1_EN_HI_IQ: 1;
        uint16_t RO_AON_LDOAUX1_POW_LDO: 1;
    };
} AON_FAST_REG0X_LDOAUX1_TYPE;

/* 0xD52
    1:0     R/W LDOAUX1_SEL_DL                                  2'b00                   LDOAUX1_REG_MASK
    2       R   RO_AON_LDOAUX1_EN_VO_PD                         1'b0
    3       R/W LDOAUX1_REG1X_DUMMY3                            1'b1                    LDOAUX1_REG_MASK
    9:4     R/W LDOAUX1_REG1X_DUMMY2                            6'b100000               LDOAUX1_REG_MASK
    15:10   R/W LDOAUX1_REG1X_DUMMY1                            6'b100000               LDOAUX1_REG_MASK
 */
typedef union _AON_FAST_REG1X_LDOAUX1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX1_SEL_DL: 2;
        uint16_t RO_AON_LDOAUX1_EN_VO_PD: 1;
        uint16_t LDOAUX1_REG1X_DUMMY3: 1;
        uint16_t LDOAUX1_REG1X_DUMMY2: 6;
        uint16_t LDOAUX1_REG1X_DUMMY1: 6;
    };
} AON_FAST_REG1X_LDOAUX1_TYPE;

/* 0xD54
    0       R   LDOAUX1_FLAG0X_DUMMY0                           1'b0
    1       R   LDOAUX1_FLAG0X_DUMMY1                           1'b0
    2       R   LDOAUX1_FLAG0X_DUMMY2                           1'b0
    3       R   LDOAUX1_FLAG0X_DUMMY3                           1'b0
    4       R   LDOAUX1_FLAG0X_DUMMY4                           1'b0
    5       R   LDOAUX1_FLAG0X_DUMMY5                           1'b0
    6       R   LDOAUX1_FLAG0X_DUMMY6                           1'b0
    7       R   LDOAUX1_FLAG0X_DUMMY7                           1'b0
    8       R   LDOAUX1_FLAG0X_DUMMY8                           1'b0
    9       R   LDOAUX1_FLAG0X_DUMMY9                           1'b0
    10      R   LDOAUX1_FLAG0X_DUMMY10                          1'b0
    11      R   LDOAUX1_FLAG0X_DUMMY11                          1'b0
    12      R   LDOAUX1_FLAG0X_DUMMY12                          1'b0
    13      R   LDOAUX1_FLAG0X_DUMMY13                          1'b0
    14      R   LDOAUX1_FLAG0X_DUMMY14                          1'b0
    15      R   LDOAUX1_FLAG0X_DUMMY15                          1'b0
 */
typedef union _AON_FAST_FLAG0X_LDOAUX1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX1_FLAG0X_DUMMY0: 1;
        uint16_t LDOAUX1_FLAG0X_DUMMY1: 1;
        uint16_t LDOAUX1_FLAG0X_DUMMY2: 1;
        uint16_t LDOAUX1_FLAG0X_DUMMY3: 1;
        uint16_t LDOAUX1_FLAG0X_DUMMY4: 1;
        uint16_t LDOAUX1_FLAG0X_DUMMY5: 1;
        uint16_t LDOAUX1_FLAG0X_DUMMY6: 1;
        uint16_t LDOAUX1_FLAG0X_DUMMY7: 1;
        uint16_t LDOAUX1_FLAG0X_DUMMY8: 1;
        uint16_t LDOAUX1_FLAG0X_DUMMY9: 1;
        uint16_t LDOAUX1_FLAG0X_DUMMY10: 1;
        uint16_t LDOAUX1_FLAG0X_DUMMY11: 1;
        uint16_t LDOAUX1_FLAG0X_DUMMY12: 1;
        uint16_t LDOAUX1_FLAG0X_DUMMY13: 1;
        uint16_t LDOAUX1_FLAG0X_DUMMY14: 1;
        uint16_t LDOAUX1_FLAG0X_DUMMY15: 1;
    };
} AON_FAST_FLAG0X_LDOAUX1_TYPE;

/* 0xD80
    7:0     R   RO_AON_LDOAUX2_TUNE_LDO_VOUT                    8'b10111100
    8       R/W LDOAUX2_REG0X_DUMMY2                            1'b0                    LDOAUX2_REG_MASK
    9       R/W LDOAUX2_REG0X_DUMMY1                            1'b0                    LDOAUX2_REG_MASK
    10      R   RO_AON_LDOAUX2_EN_LO_IQ2                        1'b0
    12:11   R   RO_AON_LDOAUX2_SEL_BIAS                         2'b00
    13      R   RO_AON_LDOAUX2_POW_BIAS                         1'b0
    14      R   RO_AON_LDOAUX2_EN_HI_IQ                         1'b0
    15      R   RO_AON_LDOAUX2_POW_LDO                          1'b0
 */
typedef union _AON_FAST_REG0X_LDOAUX2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_LDOAUX2_TUNE_LDO_VOUT: 8;
        uint16_t LDOAUX2_REG0X_DUMMY2: 1;
        uint16_t LDOAUX2_REG0X_DUMMY1: 1;
        uint16_t RO_AON_LDOAUX2_EN_LO_IQ2: 1;
        uint16_t RO_AON_LDOAUX2_SEL_BIAS: 2;
        uint16_t RO_AON_LDOAUX2_POW_BIAS: 1;
        uint16_t RO_AON_LDOAUX2_EN_HI_IQ: 1;
        uint16_t RO_AON_LDOAUX2_POW_LDO: 1;
    };
} AON_FAST_REG0X_LDOAUX2_TYPE;

/* 0xD82
    1:0     R/W LDOAUX2_SEL_DL                                  2'b00                   LDOAUX2_REG_MASK
    2       R   RO_AON_LDOAUX2_EN_VO_PD                         1'b0
    3       R/W LDOAUX2_REG1X_DUMMY3                            1'b1                    LDOAUX2_REG_MASK
    9:4     R/W LDOAUX2_REG1X_DUMMY2                            6'b100000               LDOAUX2_REG_MASK
    15:10   R/W LDOAUX2_REG1X_DUMMY1                            6'b100000               LDOAUX2_REG_MASK
 */
typedef union _AON_FAST_REG1X_LDOAUX2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX2_SEL_DL: 2;
        uint16_t RO_AON_LDOAUX2_EN_VO_PD: 1;
        uint16_t LDOAUX2_REG1X_DUMMY3: 1;
        uint16_t LDOAUX2_REG1X_DUMMY2: 6;
        uint16_t LDOAUX2_REG1X_DUMMY1: 6;
    };
} AON_FAST_REG1X_LDOAUX2_TYPE;

/* 0xD84
    0       R   LDOAUX2_FLAG0X_DUMMY0                           1'b0
    1       R   LDOAUX2_FLAG0X_DUMMY1                           1'b0
    2       R   LDOAUX2_FLAG0X_DUMMY2                           1'b0
    3       R   LDOAUX2_FLAG0X_DUMMY3                           1'b0
    4       R   LDOAUX2_FLAG0X_DUMMY4                           1'b0
    5       R   LDOAUX2_FLAG0X_DUMMY5                           1'b0
    6       R   LDOAUX2_FLAG0X_DUMMY6                           1'b0
    7       R   LDOAUX2_FLAG0X_DUMMY7                           1'b0
    8       R   LDOAUX2_FLAG0X_DUMMY8                           1'b0
    9       R   LDOAUX2_FLAG0X_DUMMY9                           1'b0
    10      R   LDOAUX2_FLAG0X_DUMMY10                          1'b0
    11      R   LDOAUX2_FLAG0X_DUMMY11                          1'b0
    12      R   LDOAUX2_FLAG0X_DUMMY12                          1'b0
    13      R   LDOAUX2_FLAG0X_DUMMY13                          1'b0
    14      R   LDOAUX2_FLAG0X_DUMMY14                          1'b0
    15      R   LDOAUX2_FLAG0X_DUMMY15                          1'b0
 */
typedef union _AON_FAST_FLAG0X_LDOAUX2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOAUX2_FLAG0X_DUMMY0: 1;
        uint16_t LDOAUX2_FLAG0X_DUMMY1: 1;
        uint16_t LDOAUX2_FLAG0X_DUMMY2: 1;
        uint16_t LDOAUX2_FLAG0X_DUMMY3: 1;
        uint16_t LDOAUX2_FLAG0X_DUMMY4: 1;
        uint16_t LDOAUX2_FLAG0X_DUMMY5: 1;
        uint16_t LDOAUX2_FLAG0X_DUMMY6: 1;
        uint16_t LDOAUX2_FLAG0X_DUMMY7: 1;
        uint16_t LDOAUX2_FLAG0X_DUMMY8: 1;
        uint16_t LDOAUX2_FLAG0X_DUMMY9: 1;
        uint16_t LDOAUX2_FLAG0X_DUMMY10: 1;
        uint16_t LDOAUX2_FLAG0X_DUMMY11: 1;
        uint16_t LDOAUX2_FLAG0X_DUMMY12: 1;
        uint16_t LDOAUX2_FLAG0X_DUMMY13: 1;
        uint16_t LDOAUX2_FLAG0X_DUMMY14: 1;
        uint16_t LDOAUX2_FLAG0X_DUMMY15: 1;
    };
} AON_FAST_FLAG0X_LDOAUX2_TYPE;

/* 0xDB0
    7:0     R   RO_AON_LDOUSB_TUNE_LDO_VOUT                     8'b10111100
    8       R/W LDOUSB_REG0X_DUMMY2                             1'b0                    LDOUSB_REG_MASK
    9       R/W LDOUSB_REG0X_DUMMY1                             1'b0                    LDOUSB_REG_MASK
    10      R   RO_AON_LDOUSB_EN_LO_IQ2                         1'b0
    12:11   R   RO_AON_LDOUSB_SEL_BIAS                          2'b00
    13      R   RO_AON_LDOUSB_POW_BIAS                          1'b0
    14      R   RO_AON_LDOUSB_EN_HI_IQ                          1'b0
    15      R   RO_AON_LDOUSB_POW_LDO                           1'b0
 */
typedef union _AON_FAST_REG0X_LDOUSB_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_LDOUSB_TUNE_LDO_VOUT: 8;
        uint16_t LDOUSB_REG0X_DUMMY2: 1;
        uint16_t LDOUSB_REG0X_DUMMY1: 1;
        uint16_t RO_AON_LDOUSB_EN_LO_IQ2: 1;
        uint16_t RO_AON_LDOUSB_SEL_BIAS: 2;
        uint16_t RO_AON_LDOUSB_POW_BIAS: 1;
        uint16_t RO_AON_LDOUSB_EN_HI_IQ: 1;
        uint16_t RO_AON_LDOUSB_POW_LDO: 1;
    };
} AON_FAST_REG0X_LDOUSB_TYPE;

/* 0xDB2
    1:0     R/W LDOUSB_SEL_DL                                   2'b00                   LDOUSB_REG_MASK
    2       R   RO_AON_LDOUSB_EN_VO_PD                          1'b0
    3       R/W LDOUSB_REG1X_DUMMY3                             1'b1                    LDOUSB_REG_MASK
    9:4     R/W LDOUSB_REG1X_DUMMY2                             6'b100000               LDOUSB_REG_MASK
    15:10   R/W LDOUSB_REG1X_DUMMY1                             6'b100000               LDOUSB_REG_MASK
 */
typedef union _AON_FAST_REG1X_LDOUSB_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOUSB_SEL_DL: 2;
        uint16_t RO_AON_LDOUSB_EN_VO_PD: 1;
        uint16_t LDOUSB_REG1X_DUMMY3: 1;
        uint16_t LDOUSB_REG1X_DUMMY2: 6;
        uint16_t LDOUSB_REG1X_DUMMY1: 6;
    };
} AON_FAST_REG1X_LDOUSB_TYPE;

/* 0xDB4
    0       R   LDOUSB_FLAG0X_DUMMY0                            1'b0
    1       R   LDOUSB_FLAG0X_DUMMY1                            1'b0
    2       R   LDOUSB_FLAG0X_DUMMY2                            1'b0
    3       R   LDOUSB_FLAG0X_DUMMY3                            1'b0
    4       R   LDOUSB_FLAG0X_DUMMY4                            1'b0
    5       R   LDOUSB_FLAG0X_DUMMY5                            1'b0
    6       R   LDOUSB_FLAG0X_DUMMY6                            1'b0
    7       R   LDOUSB_FLAG0X_DUMMY7                            1'b0
    8       R   LDOUSB_FLAG0X_DUMMY8                            1'b0
    9       R   LDOUSB_FLAG0X_DUMMY9                            1'b0
    10      R   LDOUSB_FLAG0X_DUMMY10                           1'b0
    11      R   LDOUSB_FLAG0X_DUMMY11                           1'b0
    12      R   LDOUSB_FLAG0X_DUMMY12                           1'b0
    13      R   LDOUSB_FLAG0X_DUMMY13                           1'b0
    14      R   LDOUSB_FLAG0X_DUMMY14                           1'b0
    15      R   LDOUSB_FLAG0X_DUMMY15                           1'b0
 */
typedef union _AON_FAST_FLAG0X_LDOUSB_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOUSB_FLAG0X_DUMMY0: 1;
        uint16_t LDOUSB_FLAG0X_DUMMY1: 1;
        uint16_t LDOUSB_FLAG0X_DUMMY2: 1;
        uint16_t LDOUSB_FLAG0X_DUMMY3: 1;
        uint16_t LDOUSB_FLAG0X_DUMMY4: 1;
        uint16_t LDOUSB_FLAG0X_DUMMY5: 1;
        uint16_t LDOUSB_FLAG0X_DUMMY6: 1;
        uint16_t LDOUSB_FLAG0X_DUMMY7: 1;
        uint16_t LDOUSB_FLAG0X_DUMMY8: 1;
        uint16_t LDOUSB_FLAG0X_DUMMY9: 1;
        uint16_t LDOUSB_FLAG0X_DUMMY10: 1;
        uint16_t LDOUSB_FLAG0X_DUMMY11: 1;
        uint16_t LDOUSB_FLAG0X_DUMMY12: 1;
        uint16_t LDOUSB_FLAG0X_DUMMY13: 1;
        uint16_t LDOUSB_FLAG0X_DUMMY14: 1;
        uint16_t LDOUSB_FLAG0X_DUMMY15: 1;
    };
} AON_FAST_FLAG0X_LDOUSB_TYPE;

/* 0xDE0
    7:0     R   RO_AON_LDOEXT_TUNE_LDOHQ1_VOUT                  8'b10111100
    10:8    R/W LDOEXT_TUNE_VREF                                3'b000                  LDOEXT_REG_MASK
    11      R   RO_AON_LDOEXT_EN_VO_PD                          1'b0
    12      R/W LDOEXT_EN_TR_BST                                1'b1                    LDOEXT_REG_MASK
    13      R/W LDOEXT_EN_DL_3P5U_B                             1'b0                    LDOEXT_REG_MASK
    14      R   RO_AON_LDOEXT_EN_VREF                           1'b0
    15      R   RO_AON_LDOEXT_POW_LDOHQ1                        1'b0
 */
typedef union _AON_FAST_REG0X_LDOEXT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_LDOEXT_TUNE_LDOHQ1_VOUT: 8;
        uint16_t LDOEXT_TUNE_VREF: 3;
        uint16_t RO_AON_LDOEXT_EN_VO_PD: 1;
        uint16_t LDOEXT_EN_TR_BST: 1;
        uint16_t LDOEXT_EN_DL_3P5U_B: 1;
        uint16_t RO_AON_LDOEXT_EN_VREF: 1;
        uint16_t RO_AON_LDOEXT_POW_LDOHQ1: 1;
    };
} AON_FAST_REG0X_LDOEXT_TYPE;

/* 0xDE2
    2:0     R/W LDOEXT_SEL_ISET                                 3'b100                  LDOEXT_REG_MASK
    5:3     R/W LDOEXT_SEL_DL                                   3'b111                  LDOEXT_REG_MASK
    10:6    R/W LDOEXT_REG1X_DUMMY2                             5'b00000                LDOEXT_REG_MASK
    11      R/W LDOEXT_EN_FB_LOCAL_B                            1'b0                    LDOEXT_REG_MASK
    12      R   RO_AON_LDOEXT_EN_PC                             1'b0
    13      R/W LDOEXT_SEL_VREF                                 1'b0                    LDOEXT_REG_MASK
    14      R/W LDOEXT_EN_FAST_STARTUP                          1'b1                    LDOEXT_REG_MASK
    15      R/W LDOEXT_EN_LOWIQ                                 1'b0                    LDOEXT_REG_MASK
 */
typedef union _AON_FAST_REG1X_LDOEXT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOEXT_SEL_ISET: 3;
        uint16_t LDOEXT_SEL_DL: 3;
        uint16_t LDOEXT_REG1X_DUMMY2: 5;
        uint16_t LDOEXT_EN_FB_LOCAL_B: 1;
        uint16_t RO_AON_LDOEXT_EN_PC: 1;
        uint16_t LDOEXT_SEL_VREF: 1;
        uint16_t LDOEXT_EN_FAST_STARTUP: 1;
        uint16_t LDOEXT_EN_LOWIQ: 1;
    };
} AON_FAST_REG1X_LDOEXT_TYPE;

/* 0xDE4
    2:0     R/W LDOEXT_SEL_VREF_MANUAL                          3'b000                  LDOEXT_REG_MASK
    9:3     R/W LDOEXT_REG2X_DUMMY1                             7'b0000000              LDOEXT_REG_MASK
    14:10   R/W LDOEXT_SEL_DUMMYLOAD                            5'b00000                LDOEXT_REG_MASK
    15      R/W LDOEXT_EN_DUMMYLOAD                             1'b0                    LDOEXT_REG_MASK
 */
typedef union _AON_FAST_REG2X_LDOEXT_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOEXT_SEL_VREF_MANUAL: 3;
        uint16_t LDOEXT_REG2X_DUMMY1: 7;
        uint16_t LDOEXT_SEL_DUMMYLOAD: 5;
        uint16_t LDOEXT_EN_DUMMYLOAD: 1;
    };
} AON_FAST_REG2X_LDOEXT_TYPE;

/* 0xDE6
    0       R/W LDOEXT_POS_INT_RST_B                            1'b0                    LDOEXT_REG_MASK
    1       R   RO_AON_LDOEXT_POS_EXT_RST_B                     1'b0
    2       R/W LDOEXT_POS_SEL_EXT_RST_B                        1'b0                    LDOEXT_REG_MASK
    3       R   RO_AON_LDOEXT_POS_EN_POS                        1'b0
    6:4     R/W LDOEXT_POS_DIV_CLK                              3'b010                  LDOEXT_REG_MASK
    7       R/W LDOEXT_POS_REG0X_DUMMY7                         1'b0                    LDOEXT_REG_MASK
    8       R/W LDOEXT_POS_REG0X_DUMMY8                         1'b0                    LDOEXT_REG_MASK
    9       R/W LDOEXT_POS_REG0X_DUMMY9                         1'b0                    LDOEXT_REG_MASK
    10      R/W LDOEXT_POS_REG0X_DUMMY10                        1'b0                    LDOEXT_REG_MASK
    11      R/W LDOEXT_POS_REG0X_DUMMY11                        1'b0                    LDOEXT_REG_MASK
    12      R/W LDOEXT_POS_REG0X_DUMMY12                        1'b0                    LDOEXT_REG_MASK
    13      R/W LDOEXT_POS_REG0X_DUMMY13                        1'b0                    LDOEXT_REG_MASK
    14      R/W LDOEXT_POS_REG0X_DUMMY14                        1'b0                    LDOEXT_REG_MASK
    15      R/W LDOEXT_POS_REG0X_DUMMY15                        1'b0                    LDOEXT_REG_MASK
 */
typedef union _AON_FAST_REG0X_LDOEXT_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOEXT_POS_INT_RST_B: 1;
        uint16_t RO_AON_LDOEXT_POS_EXT_RST_B: 1;
        uint16_t LDOEXT_POS_SEL_EXT_RST_B: 1;
        uint16_t RO_AON_LDOEXT_POS_EN_POS: 1;
        uint16_t LDOEXT_POS_DIV_CLK: 3;
        uint16_t LDOEXT_POS_REG0X_DUMMY7: 1;
        uint16_t LDOEXT_POS_REG0X_DUMMY8: 1;
        uint16_t LDOEXT_POS_REG0X_DUMMY9: 1;
        uint16_t LDOEXT_POS_REG0X_DUMMY10: 1;
        uint16_t LDOEXT_POS_REG0X_DUMMY11: 1;
        uint16_t LDOEXT_POS_REG0X_DUMMY12: 1;
        uint16_t LDOEXT_POS_REG0X_DUMMY13: 1;
        uint16_t LDOEXT_POS_REG0X_DUMMY14: 1;
        uint16_t LDOEXT_POS_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_LDOEXT_POS_TYPE;

/* 0xDE8
    3:0     R/W LDOEXT_POS_POF_STEP                             4'b0001                 LDOEXT_REG_MASK
    7:4     R/W LDOEXT_POS_PON_STEP                             4'b0001                 LDOEXT_REG_MASK
    10:8    R/W LDOEXT_POS_POF_WAIT                             3'b000                  LDOEXT_REG_MASK
    13:11   R/W LDOEXT_POS_PON_WAIT                             3'b000                  LDOEXT_REG_MASK
    14      R/W LDOEXT_POS_REG1X_DUMMY14                        1'b0                    LDOEXT_REG_MASK
    15      R/W LDOEXT_POS_REG1X_DUMMY15                        1'b0                    LDOEXT_REG_MASK
 */
typedef union _AON_FAST_REG1X_LDOEXT_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOEXT_POS_POF_STEP: 4;
        uint16_t LDOEXT_POS_PON_STEP: 4;
        uint16_t LDOEXT_POS_POF_WAIT: 3;
        uint16_t LDOEXT_POS_PON_WAIT: 3;
        uint16_t LDOEXT_POS_REG1X_DUMMY14: 1;
        uint16_t LDOEXT_POS_REG1X_DUMMY15: 1;
    };
} AON_FAST_REG1X_LDOEXT_POS_TYPE;

/* 0xDEA
    7:0     R/W LDOEXT_POS_PON_OVER                             8'b00000111             LDOEXT_REG_MASK
    15:8    R/W LDOEXT_POS_PON_START                            8'b00000000             LDOEXT_REG_MASK
 */
typedef union _AON_FAST_REG2X_LDOEXT_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOEXT_POS_PON_OVER: 8;
        uint16_t LDOEXT_POS_PON_START: 8;
    };
} AON_FAST_REG2X_LDOEXT_POS_TYPE;

/* 0xDEC
    7:0     R/W LDOEXT_POS_POF_OVER                             8'b00000000             LDOEXT_REG_MASK
    15:8    R/W LDOEXT_POS_POF_START                            8'b00000111             LDOEXT_REG_MASK
 */
typedef union _AON_FAST_REG3X_LDOEXT_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOEXT_POS_POF_OVER: 8;
        uint16_t LDOEXT_POS_POF_START: 8;
    };
} AON_FAST_REG3X_LDOEXT_POS_TYPE;

/* 0xDEE
    7:0     R/W LDOEXT_POS_SET_PON_FLAG2                        8'b00000001             LDOEXT_REG_MASK
    15:8    R/W LDOEXT_POS_SET_PON_FLAG1                        8'b00000000             LDOEXT_REG_MASK
 */
typedef union _AON_FAST_REG4X_LDOEXT_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOEXT_POS_SET_PON_FLAG2: 8;
        uint16_t LDOEXT_POS_SET_PON_FLAG1: 8;
    };
} AON_FAST_REG4X_LDOEXT_POS_TYPE;

/* 0xDF0
    7:0     R/W LDOEXT_POS_SET_PON_FLAG4                        8'b00000111             LDOEXT_REG_MASK
    15:8    R/W LDOEXT_POS_SET_PON_FLAG3                        8'b00000110             LDOEXT_REG_MASK
 */
typedef union _AON_FAST_REG5X_LDOEXT_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOEXT_POS_SET_PON_FLAG4: 8;
        uint16_t LDOEXT_POS_SET_PON_FLAG3: 8;
    };
} AON_FAST_REG5X_LDOEXT_POS_TYPE;

/* 0xDF2
    7:0     R/W LDOEXT_POS_SET_POF_FLAG2                        8'b00000110             LDOEXT_REG_MASK
    15:8    R/W LDOEXT_POS_SET_POF_FLAG1                        8'b00000111             LDOEXT_REG_MASK
 */
typedef union _AON_FAST_REG6X_LDOEXT_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOEXT_POS_SET_POF_FLAG2: 8;
        uint16_t LDOEXT_POS_SET_POF_FLAG1: 8;
    };
} AON_FAST_REG6X_LDOEXT_POS_TYPE;

/* 0xDF4
    7:0     R/W LDOEXT_POS_SET_POF_FLAG4                        8'b00000000             LDOEXT_REG_MASK
    15:8    R/W LDOEXT_POS_SET_POF_FLAG3                        8'b00000001             LDOEXT_REG_MASK
 */
typedef union _AON_FAST_REG7X_LDOEXT_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOEXT_POS_SET_POF_FLAG4: 8;
        uint16_t LDOEXT_POS_SET_POF_FLAG3: 8;
    };
} AON_FAST_REG7X_LDOEXT_POS_TYPE;

/* 0xDF6
    0       R   LDOEXT_POS_POF_FLAG4                            1'b0
    1       R   LDOEXT_POS_POF_FLAG3                            1'b0
    2       R   LDOEXT_POS_POF_FLAG2                            1'b0
    3       R   LDOEXT_POS_POF_FLAG1                            1'b0
    4       R   LDOEXT_POS_PON_FLAG4                            1'b0
    5       R   LDOEXT_POS_PON_FLAG3                            1'b0
    6       R   LDOEXT_POS_PON_FLAG2                            1'b0
    7       R   LDOEXT_POS_PON_FLAG1                            1'b0
    10:8    R   LDOEXT_POS_FSM_CS                               3'h0
    11      R   LDOEXT_POS_C_KOUT0X_DUMMY11                     1'b0
    12      R   LDOEXT_POS_C_KOUT0X_DUMMY12                     1'b0
    13      R   LDOEXT_POS_C_KOUT0X_DUMMY13                     1'b0
    14      R   LDOEXT_POS_C_KOUT0X_DUMMY14                     1'b0
    15      R   LDOEXT_POS_C_KOUT0X_DUMMY15                     1'b0
 */
typedef union _AON_FAST_C_KOUT0X_LDOEXT_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOEXT_POS_POF_FLAG4: 1;
        uint16_t LDOEXT_POS_POF_FLAG3: 1;
        uint16_t LDOEXT_POS_POF_FLAG2: 1;
        uint16_t LDOEXT_POS_POF_FLAG1: 1;
        uint16_t LDOEXT_POS_PON_FLAG4: 1;
        uint16_t LDOEXT_POS_PON_FLAG3: 1;
        uint16_t LDOEXT_POS_PON_FLAG2: 1;
        uint16_t LDOEXT_POS_PON_FLAG1: 1;
        uint16_t LDOEXT_POS_FSM_CS: 3;
        uint16_t LDOEXT_POS_C_KOUT0X_DUMMY11: 1;
        uint16_t LDOEXT_POS_C_KOUT0X_DUMMY12: 1;
        uint16_t LDOEXT_POS_C_KOUT0X_DUMMY13: 1;
        uint16_t LDOEXT_POS_C_KOUT0X_DUMMY14: 1;
        uint16_t LDOEXT_POS_C_KOUT0X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT0X_LDOEXT_POS_TYPE;

/* 0xDF8
    7:0     R   LDOEXT_POS_DR                                   8'h0
    8       R   LDOEXT_POS_C_KOUT1X_DUMMY8                      1'h0
    9       R   LDOEXT_POS_C_KOUT1X_DUMMY9                      1'h0
    10      R   LDOEXT_POS_C_KOUT1X_DUMMY10                     1'h0
    11      R   LDOEXT_POS_C_KOUT1X_DUMMY11                     1'h0
    12      R   LDOEXT_POS_C_KOUT1X_DUMMY12                     1'h0
    13      R   LDOEXT_POS_C_KOUT1X_DUMMY13                     1'h0
    14      R   LDOEXT_POS_C_KOUT1X_DUMMY14                     1'h0
    15      R   LDOEXT_POS_C_KOUT1X_DUMMY15                     1'h0
 */
typedef union _AON_FAST_C_KOUT1X_LDOEXT_POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDOEXT_POS_DR: 8;
        uint16_t LDOEXT_POS_C_KOUT1X_DUMMY8: 1;
        uint16_t LDOEXT_POS_C_KOUT1X_DUMMY9: 1;
        uint16_t LDOEXT_POS_C_KOUT1X_DUMMY10: 1;
        uint16_t LDOEXT_POS_C_KOUT1X_DUMMY11: 1;
        uint16_t LDOEXT_POS_C_KOUT1X_DUMMY12: 1;
        uint16_t LDOEXT_POS_C_KOUT1X_DUMMY13: 1;
        uint16_t LDOEXT_POS_C_KOUT1X_DUMMY14: 1;
        uint16_t LDOEXT_POS_C_KOUT1X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT1X_LDOEXT_POS_TYPE;

/* 0xE60
    0       R   RO_AON_LDO_DIG_POW_LDO                          1'b0
    1       R   RO_AON_LDO_DIG_EN_PC                            1'b0
    2       R/W LDO_DIG_EN_FB_LOCAL_B                           1'b1                    LDO_DIG_REG_MASK
    9:3     R   RO_AON_LDO_DIG_TUNE_LDO_VOUT                    7'b1011010
    10      R/W LDO_DIG_REG0X_DUMMY2                            1'b1                    LDO_DIG_REG_MASK
    13:11   R/W LDO_DIG_TUNE_VREF                               3'b111                  LDO_DIG_REG_MASK
    14      R   RO_AON_LDO_DIG_EN_VO_PD                         1'b0
    15      R   RO_AON_LDO_DIG_POW_VREF                         1'b0
 */
typedef union _AON_FAST_REG0X_LDO_DIG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_LDO_DIG_POW_LDO: 1;
        uint16_t RO_AON_LDO_DIG_EN_PC: 1;
        uint16_t LDO_DIG_EN_FB_LOCAL_B: 1;
        uint16_t RO_AON_LDO_DIG_TUNE_LDO_VOUT: 7;
        uint16_t LDO_DIG_REG0X_DUMMY2: 1;
        uint16_t LDO_DIG_TUNE_VREF: 3;
        uint16_t RO_AON_LDO_DIG_EN_VO_PD: 1;
        uint16_t RO_AON_LDO_DIG_POW_VREF: 1;
    };
} AON_FAST_REG0X_LDO_DIG_TYPE;

/* 0xEB0
    1:0     R/W VCORE_PC_REG0X_DUMMY1                           2'b00                   VCORE_PC_REG_MASK
    2       R   RO_AON_VCORE_PC_POW_VCORE6_PC_VG2               1'b1
    3       R   RO_AON_VCORE_PC_POW_VCORE6_PC_VG1               1'b1
    4       R   RO_AON_VCORE_PC_POW_VCORE5_PC_VG2               1'b1
    5       R   RO_AON_VCORE_PC_POW_VCORE5_PC_VG1               1'b1
    6       R   RO_AON_VCORE_PC_POW_VCORE4_PC_VG2               1'b1
    7       R   RO_AON_VCORE_PC_POW_VCORE4_PC_VG1               1'b1
    8       R   RO_AON_VCORE_PC_POW_VCORE3_PC_VG2               1'b1
    9       R   RO_AON_VCORE_PC_POW_VCORE3_PC_VG1               1'b1
    10      R/W VCORE_PC_FSM_DUMMY2                             1'b1                    VCORE_PC_REG_MASK
    11      R/W VCORE_PC_FSM_DUMMY1                             1'b1                    VCORE_PC_REG_MASK
    12      R   RO_AON_VCORE_PC_POW_VCORE1_PC_VG2               1'b1
    13      R   RO_AON_VCORE_PC_POW_VCORE1_PC_VG1               1'b1
    15:14   R/W VCORE_PC_REG0X_DUMMY2                           2'b11                   VCORE_PC_REG_MASK
 */
typedef union _AON_FAST_REG0X_VCORE_PC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t VCORE_PC_REG0X_DUMMY1: 2;
        uint16_t RO_AON_VCORE_PC_POW_VCORE6_PC_VG2: 1;
        uint16_t RO_AON_VCORE_PC_POW_VCORE6_PC_VG1: 1;
        uint16_t RO_AON_VCORE_PC_POW_VCORE5_PC_VG2: 1;
        uint16_t RO_AON_VCORE_PC_POW_VCORE5_PC_VG1: 1;
        uint16_t RO_AON_VCORE_PC_POW_VCORE4_PC_VG2: 1;
        uint16_t RO_AON_VCORE_PC_POW_VCORE4_PC_VG1: 1;
        uint16_t RO_AON_VCORE_PC_POW_VCORE3_PC_VG2: 1;
        uint16_t RO_AON_VCORE_PC_POW_VCORE3_PC_VG1: 1;
        uint16_t VCORE_PC_FSM_DUMMY2: 1;
        uint16_t VCORE_PC_FSM_DUMMY1: 1;
        uint16_t RO_AON_VCORE_PC_POW_VCORE1_PC_VG2: 1;
        uint16_t RO_AON_VCORE_PC_POW_VCORE1_PC_VG1: 1;
        uint16_t VCORE_PC_REG0X_DUMMY2: 2;
    };
} AON_FAST_REG0X_VCORE_PC_TYPE;

/* 0x1000
    0       R   RO_AON_CHG_POW_M2                               1'b0
    1       R   RO_AON_CHG_POW_M1                               1'b0
    3:2     R/W CHG_SEL_M2CCD                                   2'b11                   CHG_REG_MASK
    4       R/W CHG_EN_OP_OFS_KMOD                              1'b0                    CHG_REG_MASK
    9:5     R/W CHG_TUNE_M2_CS_OFSK                             5'b10000                CHG_REG_MASK
    10      R/W CHG_POW_M2_DVDET                                1'b1                    CHG_REG_MASK
    11      R/W CHG_POW_M1_DVDET                                1'b1                    CHG_REG_MASK
    12      R/W CHG_EN_CS_KMOD                                  1'b0                    CHG_REG_MASK
    13      R/W CHG_EN_M2_DISBIAS                               1'b0                    CHG_REG_MASK
    14      R/W CHG_EN_M1_DISBIAS                               1'b0                    CHG_REG_MASK
    15      R/W CHG_SEL_CV_REF                                  1'b0                    CHG_REG_MASK
 */
typedef union _AON_FAST_REG0X_CHG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CHG_POW_M2: 1;
        uint16_t RO_AON_CHG_POW_M1: 1;
        uint16_t CHG_SEL_M2CCD: 2;
        uint16_t CHG_EN_OP_OFS_KMOD: 1;
        uint16_t CHG_TUNE_M2_CS_OFSK: 5;
        uint16_t CHG_POW_M2_DVDET: 1;
        uint16_t CHG_POW_M1_DVDET: 1;
        uint16_t CHG_EN_CS_KMOD: 1;
        uint16_t CHG_EN_M2_DISBIAS: 1;
        uint16_t CHG_EN_M1_DISBIAS: 1;
        uint16_t CHG_SEL_CV_REF: 1;
    };
} AON_FAST_REG0X_CHG_TYPE;

/* 0x1002
    6:0     R/W CHG_TUNE_M2_CVK                                 7'b1000000              CHG_REG_MASK
    11:7    R/W CHG_SEL_M2CV                                    5'b10100                CHG_REG_MASK
    15:12   R/W CHG_SEL_M1CV                                    4'b1000                 CHG_REG_MASK
 */
typedef union _AON_FAST_REG1X_CHG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CHG_TUNE_M2_CVK: 7;
        uint16_t CHG_SEL_M2CV: 5;
        uint16_t CHG_SEL_M1CV: 4;
    };
} AON_FAST_REG1X_CHG_TYPE;

/* 0x1004
    0       R/W CHG_EN_BATSHORTDET                              1'b0                    CHG_REG_MASK
    1       R   RO_AON_CHG_EN_M2FON1K                           1'b0
    3:2     R/W CHG_SEL_M2CCDFB                                 2'b11                   CHG_REG_MASK
    7:4     R/W CHG_SEL_M2CC                                    4'b0000                 CHG_REG_MASK
    9:8     R/W CHG_SEL_M2CCFB                                  2'b00                   CHG_REG_MASK
    13:10   R/W CHG_SEL_M1CC                                    4'b0010                 CHG_REG_MASK
    15:14   R/W CHG_SEL_M1CCFB                                  2'b11                   CHG_REG_MASK
 */
typedef union _AON_FAST_REG2X_CHG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CHG_EN_BATSHORTDET: 1;
        uint16_t RO_AON_CHG_EN_M2FON1K: 1;
        uint16_t CHG_SEL_M2CCDFB: 2;
        uint16_t CHG_SEL_M2CC: 4;
        uint16_t CHG_SEL_M2CCFB: 2;
        uint16_t CHG_SEL_M1CC: 4;
        uint16_t CHG_SEL_M1CCFB: 2;
    };
} AON_FAST_REG2X_CHG_TYPE;

/* 0x1006
    4:0     R/W CHG_TUNE_M2_CC_OFSK                             5'b10000                CHG_REG_MASK
    9:5     R/W CHG_TUNE_M2_CV_OFSK                             5'b10000                CHG_REG_MASK
    15:10   R/W CHG_TUNE_M2_CCK                                 6'b100000               CHG_REG_MASK
 */
typedef union _AON_FAST_REG3X_CHG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CHG_TUNE_M2_CC_OFSK: 5;
        uint16_t CHG_TUNE_M2_CV_OFSK: 5;
        uint16_t CHG_TUNE_M2_CCK: 6;
    };
} AON_FAST_REG3X_CHG_TYPE;

/* 0x1008
    0       R/W CHG_EN_TEMP_LOOP                                1'b0                    CHG_REG_MASK
    1       R/W CHG_EN_OTP                                      1'b0                    CHG_REG_MASK
    2       R/W CHG_EN_BJTCHG                                   1'b0                    CHG_REG_MASK
    3       R/W CHG_EN_BJTCCLG                                  1'b1                    CHG_REG_MASK
    7:4     R/W CHG_SEL_OTP                                     4'b0001                 CHG_REG_MASK
    9:8     R/W CHG_REG4X_CHG_DUMMY2                            2'b10                   CHG_REG_MASK
    13:10   R/W CHG_REG4X_CHG_DUMMY1                            4'b0000                 CHG_REG_MASK
    14      R/W CHG_EN_CS_OFS                                   1'b1                    CHG_REG_MASK
    15      R/W CHG_EN_ADPHIFAST_B                              1'b1                    CHG_REG_MASK
 */
typedef union _AON_FAST_REG4X_CHG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CHG_EN_TEMP_LOOP: 1;
        uint16_t CHG_EN_OTP: 1;
        uint16_t CHG_EN_BJTCHG: 1;
        uint16_t CHG_EN_BJTCCLG: 1;
        uint16_t CHG_SEL_OTP: 4;
        uint16_t CHG_REG4X_CHG_DUMMY2: 2;
        uint16_t CHG_REG4X_CHG_DUMMY1: 4;
        uint16_t CHG_EN_CS_OFS: 1;
        uint16_t CHG_EN_ADPHIFAST_B: 1;
    };
} AON_FAST_REG4X_CHG_TYPE;

/* 0x100A
    0       R   RO_AON_CHG_EN_HV_PD                             1'b0
    3:1     R/W CHG_REG5X_CHG_DUMMY2                            3'b100                  CHG_REG_MASK
    9:4     R/W CHG_REG5X_CHG_DUMMY1                            6'b100000               CHG_REG_MASK
    15:10   R/W CHG_EN_CS_KMOD_BJT                              6'b100000               CHG_REG_MASK
 */
typedef union _AON_FAST_REG5X_CHG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CHG_EN_HV_PD: 1;
        uint16_t CHG_REG5X_CHG_DUMMY2: 3;
        uint16_t CHG_REG5X_CHG_DUMMY1: 6;
        uint16_t CHG_EN_CS_KMOD_BJT: 6;
    };
} AON_FAST_REG5X_CHG_TYPE;

/* 0x100C
    0       R   CHG_FLAG0X_CHG_DUMMY9                           1'b0
    1       R   CHG_FLAG0X_CHG_DUMMY8                           1'b0
    2       R   CHG_FLAG0X_CHG_DUMMY7                           1'b0
    3       R   CHG_FLAG0X_CHG_DUMMY6                           1'b0
    4       R   CHG_FLAG0X_CHG_DUMMY5                           1'b0
    5       R   CHG_FLAG0X_CHG_DUMMY4                           1'b0
    6       R   CHG_FLAG0X_CHG_DUMMY3                           1'b0
    7       R   CHG_FLAG0X_CHG_DUMMY2                           1'b0
    8       R   CHG_FLAG0X_CHG_DUMMY1                           1'b0
    9       R   CHG_OTP                                         1'b0
    10      R   CHG_BJTCS_FLG                                   1'b0
    11      R   CHG_M2CSOPK_FLG                                 1'b0
    12      R   CHG_M2CCOPK_FLG                                 1'b0
    13      R   CHG_M2CVOPK_FLG                                 1'b0
    14      R   CHG_BATSUP                                      1'b0
    15      R   CHG_ADPHIBAT                                    1'b0
 */
typedef union _AON_FAST_FLAG0X_CHG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CHG_FLAG0X_CHG_DUMMY9: 1;
        uint16_t CHG_FLAG0X_CHG_DUMMY8: 1;
        uint16_t CHG_FLAG0X_CHG_DUMMY7: 1;
        uint16_t CHG_FLAG0X_CHG_DUMMY6: 1;
        uint16_t CHG_FLAG0X_CHG_DUMMY5: 1;
        uint16_t CHG_FLAG0X_CHG_DUMMY4: 1;
        uint16_t CHG_FLAG0X_CHG_DUMMY3: 1;
        uint16_t CHG_FLAG0X_CHG_DUMMY2: 1;
        uint16_t CHG_FLAG0X_CHG_DUMMY1: 1;
        uint16_t CHG_OTP: 1;
        uint16_t CHG_BJTCS_FLG: 1;
        uint16_t CHG_M2CSOPK_FLG: 1;
        uint16_t CHG_M2CCOPK_FLG: 1;
        uint16_t CHG_M2CVOPK_FLG: 1;
        uint16_t CHG_BATSUP: 1;
        uint16_t CHG_ADPHIBAT: 1;
    };
} AON_FAST_FLAG0X_CHG_TYPE;

/* 0x1040
    0       R   RO_AON_SWR_CORE_EN_SAW_RAMP1                    1'b1
    1       R   RO_AON_SWR_CORE_EN_SAW_RAMP2                    1'b1
    7:2     R/W SWR_CORE_TUNE_SAW_ICLK                          6'b100110               SWR_CORE_REG_MASK
    9:8     R/W SWR_CORE_TUNE_SAW_CCLK                          2'b10                   SWR_CORE_REG_MASK
    10      R/W SWR_CORE_SEL_POS_VREFPFM2                       1'b0                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_SEL_POS_VREFPFM1                       1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_BYPASS_SAW_RAMPONDELAY                 1'b1                    SWR_CORE_REG_MASK
    15:13   R/W SWR_CORE_TUNE_POS_VREFOCPPFM1                   3'b100                  SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG0X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_SWR_CORE_EN_SAW_RAMP1: 1;
        uint16_t RO_AON_SWR_CORE_EN_SAW_RAMP2: 1;
        uint16_t SWR_CORE_TUNE_SAW_ICLK: 6;
        uint16_t SWR_CORE_TUNE_SAW_CCLK: 2;
        uint16_t SWR_CORE_SEL_POS_VREFPFM2: 1;
        uint16_t SWR_CORE_SEL_POS_VREFPFM1: 1;
        uint16_t SWR_CORE_BYPASS_SAW_RAMPONDELAY: 1;
        uint16_t SWR_CORE_TUNE_POS_VREFOCPPFM1: 3;
    };
} AON_FAST_REG0X_SWR_CORE_TYPE;

/* 0x1042
    0       R/W SWR_CORE_POW_PWM_CLP1                           1'b0                    SWR_CORE_REG_MASK
    1       R/W SWR_CORE_POW_PWM_CLP2                           1'b0                    SWR_CORE_REG_MASK
    4:2     R/W SWR_CORE_TUNE_PWM_VCH_OUT1                      3'b110                  SWR_CORE_REG_MASK
    7:5     R/W SWR_CORE_TUNE_PWM_VCL_OUT1                      3'b001                  SWR_CORE_REG_MASK
    10:8    R/W SWR_CORE_TUNE_PWM_VCH_OUT2                      3'b110                  SWR_CORE_REG_MASK
    13:11   R/W SWR_CORE_TUNE_PWM_VCL_OUT2                      3'b001                  SWR_CORE_REG_MASK
    14      R/W SWR_CORE_SEL_PWM_BP_ROUGH_SS1                   1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_SEL_PWM_BP_ROUGH_SS2                   1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG1X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_POW_PWM_CLP1: 1;
        uint16_t SWR_CORE_POW_PWM_CLP2: 1;
        uint16_t SWR_CORE_TUNE_PWM_VCH_OUT1: 3;
        uint16_t SWR_CORE_TUNE_PWM_VCL_OUT1: 3;
        uint16_t SWR_CORE_TUNE_PWM_VCH_OUT2: 3;
        uint16_t SWR_CORE_TUNE_PWM_VCL_OUT2: 3;
        uint16_t SWR_CORE_SEL_PWM_BP_ROUGH_SS1: 1;
        uint16_t SWR_CORE_SEL_PWM_BP_ROUGH_SS2: 1;
    };
} AON_FAST_REG1X_SWR_CORE_TYPE;

/* 0x1044
    0       R/W SWR_CORE_SEL_POS_VREFSS1                        1'b1                    SWR_CORE_REG_MASK
    3:1     R/W SWR_CORE_TUNE_POS_VREFPWM1                      3'b011                  SWR_CORE_REG_MASK
    11:4    R/W SWR_CORE_TUNE_POS_VREFPFM1                      8'b01101110             SWR_CORE_REG_MASK
    14:12   R/W SWR_CORE_TUNE_POS_VREFOCP1                      3'b111                  SWR_CORE_REG_MASK
    15      R/W SWR_CORE_SEL_POS_VREFOCP1                       1'b1                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG2X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_SEL_POS_VREFSS1: 1;
        uint16_t SWR_CORE_TUNE_POS_VREFPWM1: 3;
        uint16_t SWR_CORE_TUNE_POS_VREFPFM1: 8;
        uint16_t SWR_CORE_TUNE_POS_VREFOCP1: 3;
        uint16_t SWR_CORE_SEL_POS_VREFOCP1: 1;
    };
} AON_FAST_REG2X_SWR_CORE_TYPE;

/* 0x1046
    0       R/W SWR_CORE_SEL_POS_VREFSS2                        1'b1                    SWR_CORE_REG_MASK
    3:1     R/W SWR_CORE_TUNE_POS_VREFPWM2                      3'b011                  SWR_CORE_REG_MASK
    11:4    R/W SWR_CORE_TUNE_POS_VREFPFM2                      8'b01011010             SWR_CORE_REG_MASK
    14:12   R/W SWR_CORE_TUNE_POS_VREFOCP2                      3'b111                  SWR_CORE_REG_MASK
    15      R/W SWR_CORE_SEL_POS_VREFOCP2                       1'b1                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG3X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_SEL_POS_VREFSS2: 1;
        uint16_t SWR_CORE_TUNE_POS_VREFPWM2: 3;
        uint16_t SWR_CORE_TUNE_POS_VREFPFM2: 8;
        uint16_t SWR_CORE_TUNE_POS_VREFOCP2: 3;
        uint16_t SWR_CORE_SEL_POS_VREFOCP2: 1;
    };
} AON_FAST_REG3X_SWR_CORE_TYPE;

/* 0x1048
    0       R   RO_AON_SWR_CORE_FPWM1                           1'b1
    1       R   RO_AON_SWR_CORE_FPWM2                           1'b1
    2       R   RO_AON_SWR_CORE_POW_PWM1                        1'b0
    3       R   RO_AON_SWR_CORE_POW_PWM2                        1'b0
    4       R   RO_AON_SWR_CORE_POW_PFM1                        1'b0
    5       R   RO_AON_SWR_CORE_POW_PFM2                        1'b0
    6       R/W SWR_CORE_BYPASS_PWM_SSR1                        1'b1                    SWR_CORE_REG_MASK
    7       R/W SWR_CORE_BYPASS_PWM_SSR2                        1'b1                    SWR_CORE_REG_MASK
    8       R/W SWR_CORE_X4_PWM_COMP_IB_OUT1                    1'b0                    SWR_CORE_REG_MASK
    9       R/W SWR_CORE_X4_PWM_COMP_IB_OUT2                    1'b0                    SWR_CORE_REG_MASK
    12:10   R/W SWR_CORE_TUNE_PWM_C1_OUT1                       3'b101                  SWR_CORE_REG_MASK
    15:13   R/W SWR_CORE_TUNE_PWM_C2_OUT1                       3'b000                  SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG4X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_SWR_CORE_FPWM1: 1;
        uint16_t RO_AON_SWR_CORE_FPWM2: 1;
        uint16_t RO_AON_SWR_CORE_POW_PWM1: 1;
        uint16_t RO_AON_SWR_CORE_POW_PWM2: 1;
        uint16_t RO_AON_SWR_CORE_POW_PFM1: 1;
        uint16_t RO_AON_SWR_CORE_POW_PFM2: 1;
        uint16_t SWR_CORE_BYPASS_PWM_SSR1: 1;
        uint16_t SWR_CORE_BYPASS_PWM_SSR2: 1;
        uint16_t SWR_CORE_X4_PWM_COMP_IB_OUT1: 1;
        uint16_t SWR_CORE_X4_PWM_COMP_IB_OUT2: 1;
        uint16_t SWR_CORE_TUNE_PWM_C1_OUT1: 3;
        uint16_t SWR_CORE_TUNE_PWM_C2_OUT1: 3;
    };
} AON_FAST_REG4X_SWR_CORE_TYPE;

/* 0x104A
    2:0     R/W SWR_CORE_TUNE_PWM_C3_OUT1                       3'b000                  SWR_CORE_REG_MASK
    5:3     R/W SWR_CORE_TUNE_PWM_R1_OUT1                       3'b000                  SWR_CORE_REG_MASK
    8:6     R/W SWR_CORE_TUNE_PWM_R2_OUT1                       3'b111                  SWR_CORE_REG_MASK
    11:9    R/W SWR_CORE_TUNE_PWM_R3_OUT1                       3'b000                  SWR_CORE_REG_MASK
    12      R/W SWR_CORE_REG5X_DUMMY1                           1'b0                    SWR_CORE_REG_MASK
    15:13   R/W SWR_CORE_TUNE_POS_VREFOCPPFM2                   3'b100                  SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG5X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_TUNE_PWM_C3_OUT1: 3;
        uint16_t SWR_CORE_TUNE_PWM_R1_OUT1: 3;
        uint16_t SWR_CORE_TUNE_PWM_R2_OUT1: 3;
        uint16_t SWR_CORE_TUNE_PWM_R3_OUT1: 3;
        uint16_t SWR_CORE_REG5X_DUMMY1: 1;
        uint16_t SWR_CORE_TUNE_POS_VREFOCPPFM2: 3;
    };
} AON_FAST_REG5X_SWR_CORE_TYPE;

/* 0x104C
    2:0     R/W SWR_CORE_TUNE_PWM_C_OUT2                        3'b100                  SWR_CORE_REG_MASK
    5:3     R/W SWR_CORE_TUNE_PWM_R_OUT2                        3'b011                  SWR_CORE_REG_MASK
    6       R/W SWR_CORE_SEL_PFM_COT1                           1'b1                    SWR_CORE_REG_MASK
    7       R/W SWR_CORE_SEL_PFM_COT2                           1'b1                    SWR_CORE_REG_MASK
    12:8    R/W SWR_CORE_TUNE_PFM_CCOT1                         5'b00011                SWR_CORE_REG_MASK
    14:13   R/W SWR_CORE_TUNE_PFM_ICOT1                         2'b01                   SWR_CORE_REG_MASK
    15      R/W SWR_CORE_SEL_CCMPFM_COT                         1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG6X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_TUNE_PWM_C_OUT2: 3;
        uint16_t SWR_CORE_TUNE_PWM_R_OUT2: 3;
        uint16_t SWR_CORE_SEL_PFM_COT1: 1;
        uint16_t SWR_CORE_SEL_PFM_COT2: 1;
        uint16_t SWR_CORE_TUNE_PFM_CCOT1: 5;
        uint16_t SWR_CORE_TUNE_PFM_ICOT1: 2;
        uint16_t SWR_CORE_SEL_CCMPFM_COT: 1;
    };
} AON_FAST_REG6X_SWR_CORE_TYPE;

/* 0x104E
    4:0     R/W SWR_CORE_TUNE_PFM_CCOT2                         5'b00011                SWR_CORE_REG_MASK
    6:5     R/W SWR_CORE_TUNE_PFM_ICOT2                         2'b01                   SWR_CORE_REG_MASK
    7       R/W SWR_CORE_EN_PFM_FORCE_OFF_TO_ZCD1               1'b1                    SWR_CORE_REG_MASK
    8       R/W SWR_CORE_EN_PFM_FORCE_OFF_TO_ZCD2               1'b1                    SWR_CORE_REG_MASK
    9       R   RO_AON_SWR_CORE_EN_PFM_TDM                      1'b1
    10      R/W SWR_CORE_POW_PWM_MINON1                         1'b1                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_POW_PWM_MINON2                         1'b1                    SWR_CORE_REG_MASK
    13:12   R/W SWR_CORE_TUNE_PWM_IMINON1                       2'b10                   SWR_CORE_REG_MASK
    15:14   R/W SWR_CORE_TUNE_PWM_IMINON2                       2'b10                   SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG7X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_TUNE_PFM_CCOT2: 5;
        uint16_t SWR_CORE_TUNE_PFM_ICOT2: 2;
        uint16_t SWR_CORE_EN_PFM_FORCE_OFF_TO_ZCD1: 1;
        uint16_t SWR_CORE_EN_PFM_FORCE_OFF_TO_ZCD2: 1;
        uint16_t RO_AON_SWR_CORE_EN_PFM_TDM: 1;
        uint16_t SWR_CORE_POW_PWM_MINON1: 1;
        uint16_t SWR_CORE_POW_PWM_MINON2: 1;
        uint16_t SWR_CORE_TUNE_PWM_IMINON1: 2;
        uint16_t SWR_CORE_TUNE_PWM_IMINON2: 2;
    };
} AON_FAST_REG7X_SWR_CORE_TYPE;

/* 0x1050
    0       R/W SWR_CORE_EN_NONOVERLAP_SR                       1'b0                    SWR_CORE_REG_MASK
    1       R/W SWR_CORE_EN_NONOVERLAP_OCPMUX                   1'b1                    SWR_CORE_REG_MASK
    2       R/W SWR_CORE_EN_NONOVERLAP_DLY_PFB                  1'b0                    SWR_CORE_REG_MASK
    3       R/W SWR_CORE_EN_NONOVERLAP_DLY_NFB                  1'b0                    SWR_CORE_REG_MASK
    6:4     R/W SWR_CORE_TUNE_NONOVERLAP_SDZP                   3'b000                  SWR_CORE_REG_MASK
    9:7     R/W SWR_CORE_TUNE_NONOVERLAP_SDZN                   3'b000                  SWR_CORE_REG_MASK
    12:10   R/W SWR_CORE_SEL_PREDRV_P                           3'b111                  SWR_CORE_REG_MASK
    15:13   R/W SWR_CORE_SEL_PREDRV_N                           3'b111                  SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG8X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_EN_NONOVERLAP_SR: 1;
        uint16_t SWR_CORE_EN_NONOVERLAP_OCPMUX: 1;
        uint16_t SWR_CORE_EN_NONOVERLAP_DLY_PFB: 1;
        uint16_t SWR_CORE_EN_NONOVERLAP_DLY_NFB: 1;
        uint16_t SWR_CORE_TUNE_NONOVERLAP_SDZP: 3;
        uint16_t SWR_CORE_TUNE_NONOVERLAP_SDZN: 3;
        uint16_t SWR_CORE_SEL_PREDRV_P: 3;
        uint16_t SWR_CORE_SEL_PREDRV_N: 3;
    };
} AON_FAST_REG8X_SWR_CORE_TYPE;

/* 0x1052
    3:0     R/W SWR_CORE_TUNE_PREDRV_P_SP                       4'b1111                 SWR_CORE_REG_MASK
    7:4     R/W SWR_CORE_TUNE_PREDRV_P_SN                       4'b1111                 SWR_CORE_REG_MASK
    11:8    R/W SWR_CORE_TUNE_PREDRV_N_SP                       4'b1111                 SWR_CORE_REG_MASK
    15:12   R/W SWR_CORE_TUNE_PREDRV_N_SN                       4'b1111                 SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG9X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_TUNE_PREDRV_P_SP: 4;
        uint16_t SWR_CORE_TUNE_PREDRV_P_SN: 4;
        uint16_t SWR_CORE_TUNE_PREDRV_N_SP: 4;
        uint16_t SWR_CORE_TUNE_PREDRV_N_SN: 4;
    };
} AON_FAST_REG9X_SWR_CORE_TYPE;

/* 0x1054
    7:0     R   RO_AON_SWR_CORE_TUNE_VDIV1                      8'b01110110
    15:8    R   RO_AON_SWR_CORE_TUNE_VDIV2                      8'b01001110
 */
typedef union _AON_FAST_REG10X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_SWR_CORE_TUNE_VDIV1: 8;
        uint16_t RO_AON_SWR_CORE_TUNE_VDIV2: 8;
    };
} AON_FAST_REG10X_SWR_CORE_TYPE;

/* 0x1056
    7:0     R/W SWR_CORE_TUNE_ZCD_FORCECODE1                    8'b01100000             SWR_CORE_REG_MASK
    8       R/W SWR_CORE_STICKY_ZCD_CODE2                       1'b0                    SWR_CORE_REG_MASK
    9       R/W SWR_CORE_FORCE_ZCD_CODE2                        1'b0                    SWR_CORE_REG_MASK
    10      R/W SWR_CORE_SEL_ZCD_ZCDQ                           1'b0                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_POW_ZCD_COMP_CLAMPLX                   1'b1                    SWR_CORE_REG_MASK
    13:12   R/W SWR_CORE_TUNE_ZCD_SDZ1                          2'b00                   SWR_CORE_REG_MASK
    14      R/W SWR_CORE_STICKY_ZCD_CODE1                       1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_FORCE_ZCD_CODE1                        1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG11X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_TUNE_ZCD_FORCECODE1: 8;
        uint16_t SWR_CORE_STICKY_ZCD_CODE2: 1;
        uint16_t SWR_CORE_FORCE_ZCD_CODE2: 1;
        uint16_t SWR_CORE_SEL_ZCD_ZCDQ: 1;
        uint16_t SWR_CORE_POW_ZCD_COMP_CLAMPLX: 1;
        uint16_t SWR_CORE_TUNE_ZCD_SDZ1: 2;
        uint16_t SWR_CORE_STICKY_ZCD_CODE1: 1;
        uint16_t SWR_CORE_FORCE_ZCD_CODE1: 1;
    };
} AON_FAST_REG11X_SWR_CORE_TYPE;

/* 0x1058
    7:0     R/W SWR_CORE_TUNE_ZCD_FORCECODE2                    8'b01100000             SWR_CORE_REG_MASK
    8       R/W SWR_CORE_X4_PFM_COMP_IB_OUT1                    1'b0                    SWR_CORE_REG_MASK
    9       R/W SWR_CORE_X4_PFM_COMP_IB_OUT2                    1'b0                    SWR_CORE_REG_MASK
    10      R   RO_AON_SWR_CORE_POW_CCMPFM                      1'b0
    12:11   R/W SWR_CORE_TUNE_ZCD_SDZ2D                         2'b00                   SWR_CORE_REG_MASK
    15:13   R/W SWR_CORE_TUNE_ZCD_SDZ2                          3'b001                  SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG12X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_TUNE_ZCD_FORCECODE2: 8;
        uint16_t SWR_CORE_X4_PFM_COMP_IB_OUT1: 1;
        uint16_t SWR_CORE_X4_PFM_COMP_IB_OUT2: 1;
        uint16_t RO_AON_SWR_CORE_POW_CCMPFM: 1;
        uint16_t SWR_CORE_TUNE_ZCD_SDZ2D: 2;
        uint16_t SWR_CORE_TUNE_ZCD_SDZ2: 3;
    };
} AON_FAST_REG12X_SWR_CORE_TYPE;

/* 0x105A
    0       R/W SWR_CORE_EN_NONOVERLAP_DLY_N1FB                 1'b0                    SWR_CORE_REG_MASK
    1       R/W SWR_CORE_EN_NONOVERLAP_DLY_N2FB                 1'b0                    SWR_CORE_REG_MASK
    4:2     R/W SWR_CORE_TUNE_NONOVERLAP_SDZN1                  3'b000                  SWR_CORE_REG_MASK
    7:5     R/W SWR_CORE_TUNE_NONOVERLAP_SDZN2                  3'b000                  SWR_CORE_REG_MASK
    11:8    R/W SWR_CORE_TUNE_PREDRV_N_SP_OUT1                  4'b1111                 SWR_CORE_REG_MASK
    15:12   R/W SWR_CORE_TUNE_PREDRV_N_SN_OUT1                  4'b1111                 SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG13X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_EN_NONOVERLAP_DLY_N1FB: 1;
        uint16_t SWR_CORE_EN_NONOVERLAP_DLY_N2FB: 1;
        uint16_t SWR_CORE_TUNE_NONOVERLAP_SDZN1: 3;
        uint16_t SWR_CORE_TUNE_NONOVERLAP_SDZN2: 3;
        uint16_t SWR_CORE_TUNE_PREDRV_N_SP_OUT1: 4;
        uint16_t SWR_CORE_TUNE_PREDRV_N_SN_OUT1: 4;
    };
} AON_FAST_REG13X_SWR_CORE_TYPE;

/* 0x105C
    3:0     R/W SWR_CORE_TUNE_PREDRV_N_SP_OUT2                  4'b1111                 SWR_CORE_REG_MASK
    7:4     R/W SWR_CORE_TUNE_PREDRV_N_SN_OUT2                  4'b1111                 SWR_CORE_REG_MASK
    11:8    R/W SWR_CORE_REG14X_DUMMY2                          4'b0000                 SWR_CORE_REG_MASK
    15:12   R/W SWR_CORE_REG14X_DUMMY1                          4'b0000                 SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG14X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_TUNE_PREDRV_N_SP_OUT2: 4;
        uint16_t SWR_CORE_TUNE_PREDRV_N_SN_OUT2: 4;
        uint16_t SWR_CORE_REG14X_DUMMY2: 4;
        uint16_t SWR_CORE_REG14X_DUMMY1: 4;
    };
} AON_FAST_REG14X_SWR_CORE_TYPE;

/* 0x105E
    0       R   RO_AON_SWR_CORE_POW_VDIV1                       1'b1
    1       R   RO_AON_SWR_CORE_POW_VDIV2                       1'b1
    2       R/W SWR_CORE_EN_HVD17_LOWIQ                         1'b1                    SWR_CORE_REG_MASK
    3       R   RO_AON_SWR_CORE_SEL_POS_VREFLPPFM2              1'b1
    4       R   RO_AON_SWR_CORE_SEL_POS_VREFLPPFM1              1'b1
    7:5     R/W SWR_CORE_REG15X_DUMMY2                          3'b111                  SWR_CORE_REG_MASK
    9:8     R/W SWR_CORE_REG15X_DUMMY1                          2'b00                   SWR_CORE_REG_MASK
    10      R/W SWR_CORE_EN_DMYLOAD2                            1'b0                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_EN_DMYLOAD1                            1'b0                    SWR_CORE_REG_MASK
    13:12   R/W SWR_CORE_TUNE_PWM_ROUGH_SS1                     2'b11                   SWR_CORE_REG_MASK
    15:14   R/W SWR_CORE_TUNE_PWM_ROUGH_SS2                     2'b11                   SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG15X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_SWR_CORE_POW_VDIV1: 1;
        uint16_t RO_AON_SWR_CORE_POW_VDIV2: 1;
        uint16_t SWR_CORE_EN_HVD17_LOWIQ: 1;
        uint16_t RO_AON_SWR_CORE_SEL_POS_VREFLPPFM2: 1;
        uint16_t RO_AON_SWR_CORE_SEL_POS_VREFLPPFM1: 1;
        uint16_t SWR_CORE_REG15X_DUMMY2: 3;
        uint16_t SWR_CORE_REG15X_DUMMY1: 2;
        uint16_t SWR_CORE_EN_DMYLOAD2: 1;
        uint16_t SWR_CORE_EN_DMYLOAD1: 1;
        uint16_t SWR_CORE_TUNE_PWM_ROUGH_SS1: 2;
        uint16_t SWR_CORE_TUNE_PWM_ROUGH_SS2: 2;
    };
} AON_FAST_REG15X_SWR_CORE_TYPE;

/* 0x1060
    3:0     R/W SWR_CORE_REG16X_DUMMY4                          4'b1111                 SWR_CORE_REG_MASK
    7:4     R/W SWR_CORE_REG16X_DUMMY3                          4'b1111                 SWR_CORE_REG_MASK
    11:8    R/W SWR_CORE_REG16X_DUMMY2                          4'b0000                 SWR_CORE_REG_MASK
    15:12   R/W SWR_CORE_REG16X_DUMMY1                          4'b0000                 SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG16X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_REG16X_DUMMY4: 4;
        uint16_t SWR_CORE_REG16X_DUMMY3: 4;
        uint16_t SWR_CORE_REG16X_DUMMY2: 4;
        uint16_t SWR_CORE_REG16X_DUMMY1: 4;
    };
} AON_FAST_REG16X_SWR_CORE_TYPE;

/* 0x1062
    0       R   RO_AON_SWR_CORE_EN_SWR_SHORT                    1'b0
    1       R   RO_AON_SWR_CORE_SEL_FOLLOWCTRL1                 1'b1
    3:2     R/W SWR_CORE_REG17X_DUMMY2                          2'b11                   SWR_CORE_REG_MASK
    4       R/W SWR_CORE_POW_HVD17_SHORT                        1'b0                    SWR_CORE_REG_MASK
    5       R   RO_AON_SWR_CORE_POW_ZCD                         1'b0
    6       R/W SWR_CORE_POW_BNYCNT                             1'b0                    SWR_CORE_REG_MASK
    7       R   RO_AON_SWR_CORE_POW_SAW                         1'b0
    8       R   RO_AON_SWR_CORE_POW_SAW_IB                      1'b0
    9       R   RO_AON_SWR_CORE_POW_REF                         1'b0
    10      R   RO_AON_SWR_CORE_POW_IMIR                        1'b0
    11      R   RO_AON_SWR_CORE_POW_ZCD_COMP_LOWIQ              1'b0
    12      R   RO_AON_SWR_CORE_EN_SWR_OUT2                     1'b0
    13      R   RO_AON_SWR_CORE_EN_SWR_OUT1                     1'b0
    14      R   RO_AON_SWR_CORE_POW_SWR                         1'b0
    15      R   RO_AON_SWR_CORE_POW_LDO                         1'b0
 */
typedef union _AON_FAST_REG17X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_SWR_CORE_EN_SWR_SHORT: 1;
        uint16_t RO_AON_SWR_CORE_SEL_FOLLOWCTRL1: 1;
        uint16_t SWR_CORE_REG17X_DUMMY2: 2;
        uint16_t SWR_CORE_POW_HVD17_SHORT: 1;
        uint16_t RO_AON_SWR_CORE_POW_ZCD: 1;
        uint16_t SWR_CORE_POW_BNYCNT: 1;
        uint16_t RO_AON_SWR_CORE_POW_SAW: 1;
        uint16_t RO_AON_SWR_CORE_POW_SAW_IB: 1;
        uint16_t RO_AON_SWR_CORE_POW_REF: 1;
        uint16_t RO_AON_SWR_CORE_POW_IMIR: 1;
        uint16_t RO_AON_SWR_CORE_POW_ZCD_COMP_LOWIQ: 1;
        uint16_t RO_AON_SWR_CORE_EN_SWR_OUT2: 1;
        uint16_t RO_AON_SWR_CORE_EN_SWR_OUT1: 1;
        uint16_t RO_AON_SWR_CORE_POW_SWR: 1;
        uint16_t RO_AON_SWR_CORE_POW_LDO: 1;
    };
} AON_FAST_REG17X_SWR_CORE_TYPE;

/* 0x1064
    2:0     R/W SWR_CORE_REG18X_DUMMY7                          3'b111                  SWR_CORE_REG_MASK
    3       R/W SWR_CORE_REG18X_DUMMY6                          1'b1                    SWR_CORE_REG_MASK
    4       R/W SWR_CORE_SEL_POWSWR                             1'b1                    SWR_CORE_REG_MASK
    5       R/W SWR_CORE_SEL_VOUT2FB                            1'b1                    SWR_CORE_REG_MASK
    6       R/W SWR_CORE_SEL_VOUT1FB                            1'b1                    SWR_CORE_REG_MASK
    7       R   RO_AON_SWR_CORE_SEL_CK_CTRL_PFM                 1'b1
    12:8    R/W SWR_CORE_REG18X_DUMMY2                          5'b00000                SWR_CORE_REG_MASK
    15:13   R/W SWR_CORE_REG18X_DUMMY1                          3'b111                  SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG18X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_REG18X_DUMMY7: 3;
        uint16_t SWR_CORE_REG18X_DUMMY6: 1;
        uint16_t SWR_CORE_SEL_POWSWR: 1;
        uint16_t SWR_CORE_SEL_VOUT2FB: 1;
        uint16_t SWR_CORE_SEL_VOUT1FB: 1;
        uint16_t RO_AON_SWR_CORE_SEL_CK_CTRL_PFM: 1;
        uint16_t SWR_CORE_REG18X_DUMMY2: 5;
        uint16_t SWR_CORE_REG18X_DUMMY1: 3;
    };
} AON_FAST_REG18X_SWR_CORE_TYPE;

/* 0x1066
    3:0     R/W SWR_CORE_REG19X_DUMMY4                          4'b1111                 SWR_CORE_REG_MASK
    7:4     R/W SWR_CORE_REG19X_DUMMY3                          4'b1111                 SWR_CORE_REG_MASK
    11:8    R/W SWR_CORE_REG19X_DUMMY2                          4'b0000                 SWR_CORE_REG_MASK
    15:12   R/W SWR_CORE_REG19X_DUMMY1                          4'b0000                 SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG19X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_REG19X_DUMMY4: 4;
        uint16_t SWR_CORE_REG19X_DUMMY3: 4;
        uint16_t SWR_CORE_REG19X_DUMMY2: 4;
        uint16_t SWR_CORE_REG19X_DUMMY1: 4;
    };
} AON_FAST_REG19X_SWR_CORE_TYPE;

/* 0x1068
    9:0     R/W SWR_CORE_REG20X_DUMMY3                          10'b0100000000          SWR_CORE_REG_MASK
    11:10   R/W SWR_CORE_REG20X_DUMMY2                          2'b00                   SWR_CORE_REG_MASK
    15:12   R/W SWR_CORE_REG20X_DUMMY1                          4'b0000                 SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG20X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_REG20X_DUMMY3: 10;
        uint16_t SWR_CORE_REG20X_DUMMY2: 2;
        uint16_t SWR_CORE_REG20X_DUMMY1: 4;
    };
} AON_FAST_REG20X_SWR_CORE_TYPE;

/* 0x106A
    9:0     R/W SWR_CORE_REG21X_DUMMY1                          10'b0100000000          SWR_CORE_REG_MASK
    10      R   RO_AON_SWR_CORE_EN_OCP                          1'b0
    12:11   R/W SWR_CORE_TUNE_OCP_RES                           2'b10                   SWR_CORE_REG_MASK
    13      R/W SWR_CORE_SEL_OCP_RST                            1'b0                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_SEL_OCP_SET                            1'b1                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_SEL_OCP_TABLE                          1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG21X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_REG21X_DUMMY1: 10;
        uint16_t RO_AON_SWR_CORE_EN_OCP: 1;
        uint16_t SWR_CORE_TUNE_OCP_RES: 2;
        uint16_t SWR_CORE_SEL_OCP_RST: 1;
        uint16_t SWR_CORE_SEL_OCP_SET: 1;
        uint16_t SWR_CORE_SEL_OCP_TABLE: 1;
    };
} AON_FAST_REG21X_SWR_CORE_TYPE;

/* 0x106C
    2:0     R/W SWR_CORE_REG22X_DUMMY10                         3'b111                  SWR_CORE_REG_MASK
    6:3     R/W SWR_CORE_REG22X_DUMMY9                          4'b0111                 SWR_CORE_REG_MASK
    7       R/W SWR_CORE_REG22X_DUMMY8                          1'b0                    SWR_CORE_REG_MASK
    8       R/W SWR_CORE_REG22X_DUMMY7                          1'b0                    SWR_CORE_REG_MASK
    10:9    R/W SWR_CORE_REG22X_DUMMY6                          2'b10                   SWR_CORE_REG_MASK
    11      R/W SWR_CORE_REG22X_DUMMY5                          1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_REG22X_DUMMY4                          1'b0                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_REG22X_DUMMY3                          1'b0                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_REG22X_DUMMY2                          1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_REG22X_DUMMY1                          1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG22X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_REG22X_DUMMY10: 3;
        uint16_t SWR_CORE_REG22X_DUMMY9: 4;
        uint16_t SWR_CORE_REG22X_DUMMY8: 1;
        uint16_t SWR_CORE_REG22X_DUMMY7: 1;
        uint16_t SWR_CORE_REG22X_DUMMY6: 2;
        uint16_t SWR_CORE_REG22X_DUMMY5: 1;
        uint16_t SWR_CORE_REG22X_DUMMY4: 1;
        uint16_t SWR_CORE_REG22X_DUMMY3: 1;
        uint16_t SWR_CORE_REG22X_DUMMY2: 1;
        uint16_t SWR_CORE_REG22X_DUMMY1: 1;
    };
} AON_FAST_REG22X_SWR_CORE_TYPE;

/* 0x106E
    0       R/W SWR_CORE_EN_HVD17_POR17A                        1'b0                    SWR_CORE_REG_MASK
    1       R/W SWR_CORE_EN_HVD17_POR17B                        1'b0                    SWR_CORE_REG_MASK
    2       R/W SWR_CORE_FORCE_HVD17_POR                        1'b0                    SWR_CORE_REG_MASK
    6:3     R/W SWR_CORE_TUNE_HVD17_IB                          4'b1000                 SWR_CORE_REG_MASK
    7       R/W SWR_CORE_SEL_HVD17_POR17                        1'b1                    SWR_CORE_REG_MASK
    10:8    R/W SWR_CORE_TUNE_HVD17_POR17_VREFH                 3'b011                  SWR_CORE_REG_MASK
    13:11   R/W SWR_CORE_TUNE_HVD17_POR17_VREFL                 3'b011                  SWR_CORE_REG_MASK
    14      R/W SWR_CORE_EN_LXDET_BYPASS_LX                     1'b1                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_EN_HVD17_POWDN_CURRENT                 1'b1                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG23X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_EN_HVD17_POR17A: 1;
        uint16_t SWR_CORE_EN_HVD17_POR17B: 1;
        uint16_t SWR_CORE_FORCE_HVD17_POR: 1;
        uint16_t SWR_CORE_TUNE_HVD17_IB: 4;
        uint16_t SWR_CORE_SEL_HVD17_POR17: 1;
        uint16_t SWR_CORE_TUNE_HVD17_POR17_VREFH: 3;
        uint16_t SWR_CORE_TUNE_HVD17_POR17_VREFL: 3;
        uint16_t SWR_CORE_EN_LXDET_BYPASS_LX: 1;
        uint16_t SWR_CORE_EN_HVD17_POWDN_CURRENT: 1;
    };
} AON_FAST_REG23X_SWR_CORE_TYPE;

/* 0x1070
    7:0     R   SWR_CORE_ZCDQ                                   8'h0
    8       R   SWR_CORE_FLAG0X_DUMMY4                          1'b0
    9       R   SWR_CORE_FLAG0X_DUMMY3                          1'b0
    10      R   SWR_CORE_FLAG0X_DUMMY2                          1'b0
    15:11   R   SWR_CORE_FLAG0X_DUMMY1                          5'h0
 */
typedef union _AON_FAST_FLAG0X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_ZCDQ: 8;
        uint16_t SWR_CORE_FLAG0X_DUMMY4: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY3: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY2: 1;
        uint16_t SWR_CORE_FLAG0X_DUMMY1: 5;
    };
} AON_FAST_FLAG0X_SWR_CORE_TYPE;

/* 0x1072
    9:0     R   SWR_CORE_FLAG1X_DUMMY1                          10'h0
    10      R   SWR_CORE_LDO17_PORB                             1'b0
    11      R   SWR_CORE_LDO17_PORA                             1'b0
    12      R   SWR_CORE_ENLVS_O                                1'b0
    13      R   SWR_CORE_HVD17POR                               1'b0
    14      R   SWR_CORE_ENHV17_O                               1'b0
    15      R   SWR_CORE_OCP                                    1'b0
 */
typedef union _AON_FAST_FLAG1X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_FLAG1X_DUMMY1: 10;
        uint16_t SWR_CORE_LDO17_PORB: 1;
        uint16_t SWR_CORE_LDO17_PORA: 1;
        uint16_t SWR_CORE_ENLVS_O: 1;
        uint16_t SWR_CORE_HVD17POR: 1;
        uint16_t SWR_CORE_ENHV17_O: 1;
        uint16_t SWR_CORE_OCP: 1;
    };
} AON_FAST_FLAG1X_SWR_CORE_TYPE;

/* 0x1074
    15:0    R   SWR_CORE_FLAG2X_DUMMY1                          16'h0
 */
typedef union _AON_FAST_FLAG2X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_FLAG2X_DUMMY1;
    };
} AON_FAST_FLAG2X_SWR_CORE_TYPE;

/* 0x1076
    15:0    R   SWR_CORE_FLAG3X_DUMMY1                          16'h0
 */
typedef union _AON_FAST_FLAG3X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_FLAG3X_DUMMY1;
    };
} AON_FAST_FLAG3X_SWR_CORE_TYPE;

/* 0x1078
    15:0    R   SWR_CORE_C_KOUT0X_DUMMY1                        16'h0
 */
typedef union _AON_FAST_C_KOUT0X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_C_KOUT0X_DUMMY1;
    };
} AON_FAST_C_KOUT0X_SWR_CORE_TYPE;

/* 0x107A
    9:0     R   SWR_CORE_C_KOUT1X_DUMMY5                        10'h0
    10      R   SWR_CORE_C_KOUT1X_DUMMY4                        1'b0
    11      R   SWR_CORE_C_KOUT1X_DUMMY3                        1'b0
    12      R   SWR_CORE_C_KOUT1X_DUMMY2                        1'b0
    15:13   R   SWR_CORE_C_KOUT1X_DUMMY1                        3'h0
 */
typedef union _AON_FAST_C_KOUT1X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_C_KOUT1X_DUMMY5: 10;
        uint16_t SWR_CORE_C_KOUT1X_DUMMY4: 1;
        uint16_t SWR_CORE_C_KOUT1X_DUMMY3: 1;
        uint16_t SWR_CORE_C_KOUT1X_DUMMY2: 1;
        uint16_t SWR_CORE_C_KOUT1X_DUMMY1: 3;
    };
} AON_FAST_C_KOUT1X_SWR_CORE_TYPE;

/* 0x107C
    0       R   SWR_CORE_X_RAMP2ON                              1'b0
    1       R   SWR_CORE_X_RAMP1ON                              1'b0
    2       R   SWR_CORE_X_CLK2                                 1'b0
    3       R   SWR_CORE_X_CLK1                                 1'b0
    4       R   SWR_CORE_X_CLK1or2                              1'b0
    5       R   SWR_CORE_SEL_ZCDQ                               1'b0
    6       R   SWR_CORE_UPDATE_CK2                             1'b0
    7       R   SWR_CORE_UPDATE_CK1                             1'b0
    15:8    R   SWR_CORE_C_KOUT2X_DUMMY1                        8'h0
 */
typedef union _AON_FAST_C_KOUT2X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_X_RAMP2ON: 1;
        uint16_t SWR_CORE_X_RAMP1ON: 1;
        uint16_t SWR_CORE_X_CLK2: 1;
        uint16_t SWR_CORE_X_CLK1: 1;
        uint16_t SWR_CORE_X_CLK1or2: 1;
        uint16_t SWR_CORE_SEL_ZCDQ: 1;
        uint16_t SWR_CORE_UPDATE_CK2: 1;
        uint16_t SWR_CORE_UPDATE_CK1: 1;
        uint16_t SWR_CORE_C_KOUT2X_DUMMY1: 8;
    };
} AON_FAST_C_KOUT2X_SWR_CORE_TYPE;

/* 0x107E
    0       R   SWR_CORE_EN_UPDATE                              1'b0
    1       R   SWR_CORE_UD                                     1'b0
    2       R   SWR_CORE_ZCD                                    1'b0
    3       R   SWR_CORE_ZCD_SET                                1'b0
    4       R   SWR_CORE_OUT2_OCCUPY                            1'b0
    5       R   SWR_CORE_OUT1_OCCUPY                            1'b0
    6       R   SWR_CORE_OUT2_CTRL                              1'b0
    7       R   SWR_CORE_OUT1_CTRL                              1'b0
    8       R   SWR_CORE_ONTIME_OVER2                           1'b0
    9       R   SWR_CORE_VCOMPPFM2                              1'b0
    10      R   SWR_CORE_ONTIME_OVER1                           1'b0
    11      R   SWR_CORE_VCOMPPFM1                              1'b0
    12      R   SWR_CORE_VCOMP2                                 1'b0
    13      R   SWR_CORE_VCOMP1                                 1'b0
    15:14   R   SWR_CORE_S                                      2'h0
 */
typedef union _AON_FAST_C_KOUT3X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_EN_UPDATE: 1;
        uint16_t SWR_CORE_UD: 1;
        uint16_t SWR_CORE_ZCD: 1;
        uint16_t SWR_CORE_ZCD_SET: 1;
        uint16_t SWR_CORE_OUT2_OCCUPY: 1;
        uint16_t SWR_CORE_OUT1_OCCUPY: 1;
        uint16_t SWR_CORE_OUT2_CTRL: 1;
        uint16_t SWR_CORE_OUT1_CTRL: 1;
        uint16_t SWR_CORE_ONTIME_OVER2: 1;
        uint16_t SWR_CORE_VCOMPPFM2: 1;
        uint16_t SWR_CORE_ONTIME_OVER1: 1;
        uint16_t SWR_CORE_VCOMPPFM1: 1;
        uint16_t SWR_CORE_VCOMP2: 1;
        uint16_t SWR_CORE_VCOMP1: 1;
        uint16_t SWR_CORE_S: 2;
    };
} AON_FAST_C_KOUT3X_SWR_CORE_TYPE;

/* 0x1080
    0       R   SWR_CORE_PFM_CTRL2                              1'b0
    1       R   SWR_CORE_PFM_CTRL1                              1'b0
    2       R   SWR_CORE_PWM_CTRL2                              1'b0
    3       R   SWR_CORE_PWM_CTRL1                              1'b0
    4       R   SWR_CORE_CK_CTRL2                               1'b0
    5       R   SWR_CORE_CK_CTRL1                               1'b0
    6       R   SWR_CORE_CK_CTRL                                1'b0
    7       R   SWR_CORE_NI_OUT2                                1'b0
    8       R   SWR_CORE_NI_OUT1                                1'b0
    9       R   SWR_CORE_NI                                     1'b0
    10      R   SWR_CORE_PI                                     1'b0
    11      R   SWR_CORE_LX_FALL_DET                            1'b0
    12      R   SWR_CORE_OUT2GATE_HV                            1'b0
    13      R   SWR_CORE_OUT1GATE_HV                            1'b0
    14      R   SWR_CORE_NGATE_HV                               1'b0
    15      R   SWR_CORE_PGATE_HV                               1'b0
 */
typedef union _AON_FAST_C_KOUT4X_SWR_CORE_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM_CTRL2: 1;
        uint16_t SWR_CORE_PFM_CTRL1: 1;
        uint16_t SWR_CORE_PWM_CTRL2: 1;
        uint16_t SWR_CORE_PWM_CTRL1: 1;
        uint16_t SWR_CORE_CK_CTRL2: 1;
        uint16_t SWR_CORE_CK_CTRL1: 1;
        uint16_t SWR_CORE_CK_CTRL: 1;
        uint16_t SWR_CORE_NI_OUT2: 1;
        uint16_t SWR_CORE_NI_OUT1: 1;
        uint16_t SWR_CORE_NI: 1;
        uint16_t SWR_CORE_PI: 1;
        uint16_t SWR_CORE_LX_FALL_DET: 1;
        uint16_t SWR_CORE_OUT2GATE_HV: 1;
        uint16_t SWR_CORE_OUT1GATE_HV: 1;
        uint16_t SWR_CORE_NGATE_HV: 1;
        uint16_t SWR_CORE_PGATE_HV: 1;
    };
} AON_FAST_C_KOUT4X_SWR_CORE_TYPE;

/* 0x1082
    0       R/W SWR_CORE_PWM1POS_INT_RST_B                      1'b0                    SWR_CORE_REG_MASK
    1       R   RO_AON_SWR_CORE_PWM1POS_EXT_RST_B               1'b0
    2       R/W SWR_CORE_PWM1POS_SEL_EXT_RST_B                  1'b0                    SWR_CORE_REG_MASK
    3       R   RO_AON_SWR_CORE_PWM1POS_EN_POS                  1'b0
    6:4     R/W SWR_CORE_PWM1POS_DIV_CLK                        3'b011                  SWR_CORE_REG_MASK
    7       R/W SWR_CORE_PWM1POS_REG0X_DUMMY7                   1'b0                    SWR_CORE_REG_MASK
    8       R/W SWR_CORE_PWM1POS_REG0X_DUMMY8                   1'b0                    SWR_CORE_REG_MASK
    9       R/W SWR_CORE_PWM1POS_REG0X_DUMMY9                   1'b0                    SWR_CORE_REG_MASK
    10      R/W SWR_CORE_PWM1POS_REG0X_DUMMY10                  1'b0                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_PWM1POS_REG0X_DUMMY11                  1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_PWM1POS_REG0X_DUMMY12                  1'b0                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_PWM1POS_REG0X_DUMMY13                  1'b0                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PWM1POS_REG0X_DUMMY14                  1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PWM1POS_REG0X_DUMMY15                  1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG0X_SWR_CORE_PWM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PWM1POS_INT_RST_B: 1;
        uint16_t RO_AON_SWR_CORE_PWM1POS_EXT_RST_B: 1;
        uint16_t SWR_CORE_PWM1POS_SEL_EXT_RST_B: 1;
        uint16_t RO_AON_SWR_CORE_PWM1POS_EN_POS: 1;
        uint16_t SWR_CORE_PWM1POS_DIV_CLK: 3;
        uint16_t SWR_CORE_PWM1POS_REG0X_DUMMY7: 1;
        uint16_t SWR_CORE_PWM1POS_REG0X_DUMMY8: 1;
        uint16_t SWR_CORE_PWM1POS_REG0X_DUMMY9: 1;
        uint16_t SWR_CORE_PWM1POS_REG0X_DUMMY10: 1;
        uint16_t SWR_CORE_PWM1POS_REG0X_DUMMY11: 1;
        uint16_t SWR_CORE_PWM1POS_REG0X_DUMMY12: 1;
        uint16_t SWR_CORE_PWM1POS_REG0X_DUMMY13: 1;
        uint16_t SWR_CORE_PWM1POS_REG0X_DUMMY14: 1;
        uint16_t SWR_CORE_PWM1POS_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_SWR_CORE_PWM1POS_TYPE;

/* 0x1084
    3:0     R/W SWR_CORE_PWM1POS_POF_STEP                       4'b0001                 SWR_CORE_REG_MASK
    7:4     R/W SWR_CORE_PWM1POS_PON_STEP                       4'b0001                 SWR_CORE_REG_MASK
    10:8    R/W SWR_CORE_PWM1POS_POF_WAIT                       3'b000                  SWR_CORE_REG_MASK
    13:11   R/W SWR_CORE_PWM1POS_PON_WAIT                       3'b000                  SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PWM1POS_REG1X_DUMMY14                  1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PWM1POS_REG1X_DUMMY15                  1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG1X_SWR_CORE_PWM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PWM1POS_POF_STEP: 4;
        uint16_t SWR_CORE_PWM1POS_PON_STEP: 4;
        uint16_t SWR_CORE_PWM1POS_POF_WAIT: 3;
        uint16_t SWR_CORE_PWM1POS_PON_WAIT: 3;
        uint16_t SWR_CORE_PWM1POS_REG1X_DUMMY14: 1;
        uint16_t SWR_CORE_PWM1POS_REG1X_DUMMY15: 1;
    };
} AON_FAST_REG1X_SWR_CORE_PWM1POS_TYPE;

/* 0x1086
    7:0     R/W SWR_CORE_PWM1POS_PON_OVER                       8'b00011111             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PWM1POS_PON_START                      8'b00000000             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG2X_SWR_CORE_PWM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PWM1POS_PON_OVER: 8;
        uint16_t SWR_CORE_PWM1POS_PON_START: 8;
    };
} AON_FAST_REG2X_SWR_CORE_PWM1POS_TYPE;

/* 0x1088
    7:0     R/W SWR_CORE_PWM1POS_POF_OVER                       8'b00000000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PWM1POS_POF_START                      8'b00011111             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG3X_SWR_CORE_PWM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PWM1POS_POF_OVER: 8;
        uint16_t SWR_CORE_PWM1POS_POF_START: 8;
    };
} AON_FAST_REG3X_SWR_CORE_PWM1POS_TYPE;

/* 0x108A
    7:0     R/W SWR_CORE_PWM1POS_SET_PON_FLAG2                  8'b00011000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PWM1POS_SET_PON_FLAG1                  8'b00000000             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG4X_SWR_CORE_PWM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PWM1POS_SET_PON_FLAG2: 8;
        uint16_t SWR_CORE_PWM1POS_SET_PON_FLAG1: 8;
    };
} AON_FAST_REG4X_SWR_CORE_PWM1POS_TYPE;

/* 0x108C
    7:0     R/W SWR_CORE_PWM1POS_SET_PON_FLAG4                  8'b00011111             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PWM1POS_SET_PON_FLAG3                  8'b00011111             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG5X_SWR_CORE_PWM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PWM1POS_SET_PON_FLAG4: 8;
        uint16_t SWR_CORE_PWM1POS_SET_PON_FLAG3: 8;
    };
} AON_FAST_REG5X_SWR_CORE_PWM1POS_TYPE;

/* 0x108E
    7:0     R/W SWR_CORE_PWM1POS_SET_POF_FLAG2                  8'b00011000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PWM1POS_SET_POF_FLAG1                  8'b00011111             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG6X_SWR_CORE_PWM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PWM1POS_SET_POF_FLAG2: 8;
        uint16_t SWR_CORE_PWM1POS_SET_POF_FLAG1: 8;
    };
} AON_FAST_REG6X_SWR_CORE_PWM1POS_TYPE;

/* 0x1090
    7:0     R/W SWR_CORE_PWM1POS_SET_POF_FLAG4                  8'b00000000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PWM1POS_SET_POF_FLAG3                  8'b00000000             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG7X_SWR_CORE_PWM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PWM1POS_SET_POF_FLAG4: 8;
        uint16_t SWR_CORE_PWM1POS_SET_POF_FLAG3: 8;
    };
} AON_FAST_REG7X_SWR_CORE_PWM1POS_TYPE;

/* 0x1092
    0       R   SWR_CORE_PWM1POS_POF_FLAG4                      1'b0
    1       R   SWR_CORE_PWM1POS_POF_FLAG3                      1'b0
    2       R   SWR_CORE_PWM1POS_POF_FLAG2                      1'b0
    3       R   SWR_CORE_PWM1POS_POF_FLAG1                      1'b0
    4       R   SWR_CORE_PWM1POS_PON_FLAG4                      1'b0
    5       R   SWR_CORE_PWM1POS_PON_FLAG3                      1'b0
    6       R   SWR_CORE_PWM1POS_PON_FLAG2                      1'b0
    7       R   SWR_CORE_PWM1POS_PON_FLAG1                      1'b0
    10:8    R   SWR_CORE_PWM1POS_FSM_CS                         3'h0
    11      R   SWR_CORE_PWM1POS_C_KOUT0X_DUMMY11               1'b0
    12      R   SWR_CORE_PWM1POS_C_KOUT0X_DUMMY12               1'b0
    13      R   SWR_CORE_PWM1POS_C_KOUT0X_DUMMY13               1'b0
    14      R   SWR_CORE_PWM1POS_C_KOUT0X_DUMMY14               1'b0
    15      R   SWR_CORE_PWM1POS_C_KOUT0X_DUMMY15               1'b0
 */
typedef union _AON_FAST_C_KOUT0X_SWR_CORE_PWM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PWM1POS_POF_FLAG4: 1;
        uint16_t SWR_CORE_PWM1POS_POF_FLAG3: 1;
        uint16_t SWR_CORE_PWM1POS_POF_FLAG2: 1;
        uint16_t SWR_CORE_PWM1POS_POF_FLAG1: 1;
        uint16_t SWR_CORE_PWM1POS_PON_FLAG4: 1;
        uint16_t SWR_CORE_PWM1POS_PON_FLAG3: 1;
        uint16_t SWR_CORE_PWM1POS_PON_FLAG2: 1;
        uint16_t SWR_CORE_PWM1POS_PON_FLAG1: 1;
        uint16_t SWR_CORE_PWM1POS_FSM_CS: 3;
        uint16_t SWR_CORE_PWM1POS_C_KOUT0X_DUMMY11: 1;
        uint16_t SWR_CORE_PWM1POS_C_KOUT0X_DUMMY12: 1;
        uint16_t SWR_CORE_PWM1POS_C_KOUT0X_DUMMY13: 1;
        uint16_t SWR_CORE_PWM1POS_C_KOUT0X_DUMMY14: 1;
        uint16_t SWR_CORE_PWM1POS_C_KOUT0X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT0X_SWR_CORE_PWM1POS_TYPE;

/* 0x1094
    7:0     R   SWR_CORE_PWM1POS_DR                             8'h0
    8       R   SWR_CORE_PWM1POS_C_KOUT1X_DUMMY8                1'h0
    9       R   SWR_CORE_PWM1POS_C_KOUT1X_DUMMY9                1'h0
    10      R   SWR_CORE_PWM1POS_C_KOUT1X_DUMMY10               1'h0
    11      R   SWR_CORE_PWM1POS_C_KOUT1X_DUMMY11               1'h0
    12      R   SWR_CORE_PWM1POS_C_KOUT1X_DUMMY12               1'h0
    13      R   SWR_CORE_PWM1POS_C_KOUT1X_DUMMY13               1'h0
    14      R   SWR_CORE_PWM1POS_C_KOUT1X_DUMMY14               1'h0
    15      R   SWR_CORE_PWM1POS_C_KOUT1X_DUMMY15               1'h0
 */
typedef union _AON_FAST_C_KOUT1X_SWR_CORE_PWM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PWM1POS_DR: 8;
        uint16_t SWR_CORE_PWM1POS_C_KOUT1X_DUMMY8: 1;
        uint16_t SWR_CORE_PWM1POS_C_KOUT1X_DUMMY9: 1;
        uint16_t SWR_CORE_PWM1POS_C_KOUT1X_DUMMY10: 1;
        uint16_t SWR_CORE_PWM1POS_C_KOUT1X_DUMMY11: 1;
        uint16_t SWR_CORE_PWM1POS_C_KOUT1X_DUMMY12: 1;
        uint16_t SWR_CORE_PWM1POS_C_KOUT1X_DUMMY13: 1;
        uint16_t SWR_CORE_PWM1POS_C_KOUT1X_DUMMY14: 1;
        uint16_t SWR_CORE_PWM1POS_C_KOUT1X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT1X_SWR_CORE_PWM1POS_TYPE;

/* 0x1096
    0       R/W SWR_CORE_PFMCCM1POS_INT_RST_B                   1'b0                    SWR_CORE_REG_MASK
    1       R   RO_AON_SWR_CORE_PFMCCM1POS_EXT_RST_B            1'b0
    2       R/W SWR_CORE_PFMCCM1POS_SEL_EXT_RST_B               1'b0                    SWR_CORE_REG_MASK
    3       R   RO_AON_SWR_CORE_PFMCCM1POS_EN_POS               1'b0
    6:4     R/W SWR_CORE_PFMCCM1POS_DIV_CLK                     3'b011                  SWR_CORE_REG_MASK
    7       R/W SWR_CORE_PFMCCM1POS_REG0X_DUMMY7                1'b0                    SWR_CORE_REG_MASK
    8       R/W SWR_CORE_PFMCCM1POS_REG0X_DUMMY8                1'b0                    SWR_CORE_REG_MASK
    9       R/W SWR_CORE_PFMCCM1POS_REG0X_DUMMY9                1'b0                    SWR_CORE_REG_MASK
    10      R/W SWR_CORE_PFMCCM1POS_REG0X_DUMMY10               1'b0                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_PFMCCM1POS_REG0X_DUMMY11               1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_PFMCCM1POS_REG0X_DUMMY12               1'b0                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_PFMCCM1POS_REG0X_DUMMY13               1'b0                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFMCCM1POS_REG0X_DUMMY14               1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFMCCM1POS_REG0X_DUMMY15               1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG0X_SWR_CORE_PFMCCM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFMCCM1POS_INT_RST_B: 1;
        uint16_t RO_AON_SWR_CORE_PFMCCM1POS_EXT_RST_B: 1;
        uint16_t SWR_CORE_PFMCCM1POS_SEL_EXT_RST_B: 1;
        uint16_t RO_AON_SWR_CORE_PFMCCM1POS_EN_POS: 1;
        uint16_t SWR_CORE_PFMCCM1POS_DIV_CLK: 3;
        uint16_t SWR_CORE_PFMCCM1POS_REG0X_DUMMY7: 1;
        uint16_t SWR_CORE_PFMCCM1POS_REG0X_DUMMY8: 1;
        uint16_t SWR_CORE_PFMCCM1POS_REG0X_DUMMY9: 1;
        uint16_t SWR_CORE_PFMCCM1POS_REG0X_DUMMY10: 1;
        uint16_t SWR_CORE_PFMCCM1POS_REG0X_DUMMY11: 1;
        uint16_t SWR_CORE_PFMCCM1POS_REG0X_DUMMY12: 1;
        uint16_t SWR_CORE_PFMCCM1POS_REG0X_DUMMY13: 1;
        uint16_t SWR_CORE_PFMCCM1POS_REG0X_DUMMY14: 1;
        uint16_t SWR_CORE_PFMCCM1POS_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_SWR_CORE_PFMCCM1POS_TYPE;

/* 0x1098
    3:0     R/W SWR_CORE_PFMCCM1POS_POF_STEP                    4'b0101                 SWR_CORE_REG_MASK
    7:4     R/W SWR_CORE_PFMCCM1POS_PON_STEP                    4'b0101                 SWR_CORE_REG_MASK
    10:8    R/W SWR_CORE_PFMCCM1POS_POF_WAIT                    3'b000                  SWR_CORE_REG_MASK
    13:11   R/W SWR_CORE_PFMCCM1POS_PON_WAIT                    3'b000                  SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFMCCM1POS_REG1X_DUMMY14               1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFMCCM1POS_REG1X_DUMMY15               1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG1X_SWR_CORE_PFMCCM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFMCCM1POS_POF_STEP: 4;
        uint16_t SWR_CORE_PFMCCM1POS_PON_STEP: 4;
        uint16_t SWR_CORE_PFMCCM1POS_POF_WAIT: 3;
        uint16_t SWR_CORE_PFMCCM1POS_PON_WAIT: 3;
        uint16_t SWR_CORE_PFMCCM1POS_REG1X_DUMMY14: 1;
        uint16_t SWR_CORE_PFMCCM1POS_REG1X_DUMMY15: 1;
    };
} AON_FAST_REG1X_SWR_CORE_PFMCCM1POS_TYPE;

/* 0x109A
    7:0     R/W SWR_CORE_PFMCCM1POS_PON_OVER                    8'b10001100             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFMCCM1POS_PON_START                   8'b00000000             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG2X_SWR_CORE_PFMCCM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFMCCM1POS_PON_OVER: 8;
        uint16_t SWR_CORE_PFMCCM1POS_PON_START: 8;
    };
} AON_FAST_REG2X_SWR_CORE_PFMCCM1POS_TYPE;

/* 0x109C
    7:0     R/W SWR_CORE_PFMCCM1POS_POF_OVER                    8'b00000000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFMCCM1POS_POF_START                   8'b10001100             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG3X_SWR_CORE_PFMCCM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFMCCM1POS_POF_OVER: 8;
        uint16_t SWR_CORE_PFMCCM1POS_POF_START: 8;
    };
} AON_FAST_REG3X_SWR_CORE_PFMCCM1POS_TYPE;

/* 0x109E
    7:0     R/W SWR_CORE_PFMCCM1POS_SET_PON_FLAG2               8'b01111000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFMCCM1POS_SET_PON_FLAG1               8'b00000000             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG4X_SWR_CORE_PFMCCM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFMCCM1POS_SET_PON_FLAG2: 8;
        uint16_t SWR_CORE_PFMCCM1POS_SET_PON_FLAG1: 8;
    };
} AON_FAST_REG4X_SWR_CORE_PFMCCM1POS_TYPE;

/* 0x10A0
    7:0     R/W SWR_CORE_PFMCCM1POS_SET_PON_FLAG4               8'b10001100             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFMCCM1POS_SET_PON_FLAG3               8'b10001100             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG5X_SWR_CORE_PFMCCM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFMCCM1POS_SET_PON_FLAG4: 8;
        uint16_t SWR_CORE_PFMCCM1POS_SET_PON_FLAG3: 8;
    };
} AON_FAST_REG5X_SWR_CORE_PFMCCM1POS_TYPE;

/* 0x10A2
    7:0     R/W SWR_CORE_PFMCCM1POS_SET_POF_FLAG2               8'b01111000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFMCCM1POS_SET_POF_FLAG1               8'b10001100             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG6X_SWR_CORE_PFMCCM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFMCCM1POS_SET_POF_FLAG2: 8;
        uint16_t SWR_CORE_PFMCCM1POS_SET_POF_FLAG1: 8;
    };
} AON_FAST_REG6X_SWR_CORE_PFMCCM1POS_TYPE;

/* 0x10A4
    7:0     R/W SWR_CORE_PFMCCM1POS_SET_POF_FLAG4               8'b00000000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFMCCM1POS_SET_POF_FLAG3               8'b00000000             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG7X_SWR_CORE_PFMCCM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFMCCM1POS_SET_POF_FLAG4: 8;
        uint16_t SWR_CORE_PFMCCM1POS_SET_POF_FLAG3: 8;
    };
} AON_FAST_REG7X_SWR_CORE_PFMCCM1POS_TYPE;

/* 0x10A6
    0       R   SWR_CORE_PFMCCM1POS_POF_FLAG4                   1'b0
    1       R   SWR_CORE_PFMCCM1POS_POF_FLAG3                   1'b0
    2       R   SWR_CORE_PFMCCM1POS_POF_FLAG2                   1'b0
    3       R   SWR_CORE_PFMCCM1POS_POF_FLAG1                   1'b0
    4       R   SWR_CORE_PFMCCM1POS_PON_FLAG4                   1'b0
    5       R   SWR_CORE_PFMCCM1POS_PON_FLAG3                   1'b0
    6       R   SWR_CORE_PFMCCM1POS_PON_FLAG2                   1'b0
    7       R   SWR_CORE_PFMCCM1POS_PON_FLAG1                   1'b0
    10:8    R   SWR_CORE_PFMCCM1POS_FSM_CS                      3'h0
    11      R   SWR_CORE_PFMCCM1POS_C_KOUT0X_DUMMY11            1'b0
    12      R   SWR_CORE_PFMCCM1POS_C_KOUT0X_DUMMY12            1'b0
    13      R   SWR_CORE_PFMCCM1POS_C_KOUT0X_DUMMY13            1'b0
    14      R   SWR_CORE_PFMCCM1POS_C_KOUT0X_DUMMY14            1'b0
    15      R   SWR_CORE_PFMCCM1POS_C_KOUT0X_DUMMY15            1'b0
 */
typedef union _AON_FAST_C_KOUT0X_SWR_CORE_PFMCCM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFMCCM1POS_POF_FLAG4: 1;
        uint16_t SWR_CORE_PFMCCM1POS_POF_FLAG3: 1;
        uint16_t SWR_CORE_PFMCCM1POS_POF_FLAG2: 1;
        uint16_t SWR_CORE_PFMCCM1POS_POF_FLAG1: 1;
        uint16_t SWR_CORE_PFMCCM1POS_PON_FLAG4: 1;
        uint16_t SWR_CORE_PFMCCM1POS_PON_FLAG3: 1;
        uint16_t SWR_CORE_PFMCCM1POS_PON_FLAG2: 1;
        uint16_t SWR_CORE_PFMCCM1POS_PON_FLAG1: 1;
        uint16_t SWR_CORE_PFMCCM1POS_FSM_CS: 3;
        uint16_t SWR_CORE_PFMCCM1POS_C_KOUT0X_DUMMY11: 1;
        uint16_t SWR_CORE_PFMCCM1POS_C_KOUT0X_DUMMY12: 1;
        uint16_t SWR_CORE_PFMCCM1POS_C_KOUT0X_DUMMY13: 1;
        uint16_t SWR_CORE_PFMCCM1POS_C_KOUT0X_DUMMY14: 1;
        uint16_t SWR_CORE_PFMCCM1POS_C_KOUT0X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT0X_SWR_CORE_PFMCCM1POS_TYPE;

/* 0x10A8
    7:0     R   SWR_CORE_PFMCCM1POS_DR                          8'h0
    8       R   SWR_CORE_PFMCCM1POS_C_KOUT1X_DUMMY8             1'h0
    9       R   SWR_CORE_PFMCCM1POS_C_KOUT1X_DUMMY9             1'h0
    10      R   SWR_CORE_PFMCCM1POS_C_KOUT1X_DUMMY10            1'h0
    11      R   SWR_CORE_PFMCCM1POS_C_KOUT1X_DUMMY11            1'h0
    12      R   SWR_CORE_PFMCCM1POS_C_KOUT1X_DUMMY12            1'h0
    13      R   SWR_CORE_PFMCCM1POS_C_KOUT1X_DUMMY13            1'h0
    14      R   SWR_CORE_PFMCCM1POS_C_KOUT1X_DUMMY14            1'h0
    15      R   SWR_CORE_PFMCCM1POS_C_KOUT1X_DUMMY15            1'h0
 */
typedef union _AON_FAST_C_KOUT1X_SWR_CORE_PFMCCM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFMCCM1POS_DR: 8;
        uint16_t SWR_CORE_PFMCCM1POS_C_KOUT1X_DUMMY8: 1;
        uint16_t SWR_CORE_PFMCCM1POS_C_KOUT1X_DUMMY9: 1;
        uint16_t SWR_CORE_PFMCCM1POS_C_KOUT1X_DUMMY10: 1;
        uint16_t SWR_CORE_PFMCCM1POS_C_KOUT1X_DUMMY11: 1;
        uint16_t SWR_CORE_PFMCCM1POS_C_KOUT1X_DUMMY12: 1;
        uint16_t SWR_CORE_PFMCCM1POS_C_KOUT1X_DUMMY13: 1;
        uint16_t SWR_CORE_PFMCCM1POS_C_KOUT1X_DUMMY14: 1;
        uint16_t SWR_CORE_PFMCCM1POS_C_KOUT1X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT1X_SWR_CORE_PFMCCM1POS_TYPE;

/* 0x10AA
    0       R/W SWR_CORE_PFM1POS_INT_RST_B                      1'b0                    SWR_CORE_REG_MASK
    1       R   RO_AON_SWR_CORE_PFM1POS_EXT_RST_B               1'b0
    2       R/W SWR_CORE_PFM1POS_SEL_EXT_RST_B                  1'b0                    SWR_CORE_REG_MASK
    3       R   RO_AON_SWR_CORE_PFM1POS_EN_POS                  1'b0
    6:4     R/W SWR_CORE_PFM1POS_DIV_CLK                        3'b011                  SWR_CORE_REG_MASK
    7       R/W SWR_CORE_PFM1POS_REG0X_DUMMY7                   1'b0                    SWR_CORE_REG_MASK
    8       R/W SWR_CORE_PFM1POS_REG0X_DUMMY8                   1'b0                    SWR_CORE_REG_MASK
    9       R/W SWR_CORE_PFM1POS_REG0X_DUMMY9                   1'b0                    SWR_CORE_REG_MASK
    10      R/W SWR_CORE_PFM1POS_REG0X_DUMMY10                  1'b0                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_PFM1POS_REG0X_DUMMY11                  1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_PFM1POS_REG0X_DUMMY12                  1'b0                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_PFM1POS_REG0X_DUMMY13                  1'b0                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFM1POS_REG0X_DUMMY14                  1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFM1POS_REG0X_DUMMY15                  1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG0X_SWR_CORE_PFM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM1POS_INT_RST_B: 1;
        uint16_t RO_AON_SWR_CORE_PFM1POS_EXT_RST_B: 1;
        uint16_t SWR_CORE_PFM1POS_SEL_EXT_RST_B: 1;
        uint16_t RO_AON_SWR_CORE_PFM1POS_EN_POS: 1;
        uint16_t SWR_CORE_PFM1POS_DIV_CLK: 3;
        uint16_t SWR_CORE_PFM1POS_REG0X_DUMMY7: 1;
        uint16_t SWR_CORE_PFM1POS_REG0X_DUMMY8: 1;
        uint16_t SWR_CORE_PFM1POS_REG0X_DUMMY9: 1;
        uint16_t SWR_CORE_PFM1POS_REG0X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM1POS_REG0X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM1POS_REG0X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM1POS_REG0X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM1POS_REG0X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM1POS_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_SWR_CORE_PFM1POS_TYPE;

/* 0x10AC
    3:0     R/W SWR_CORE_PFM1POS_POF_STEP                       4'b0101                 SWR_CORE_REG_MASK
    7:4     R/W SWR_CORE_PFM1POS_PON_STEP                       4'b0101                 SWR_CORE_REG_MASK
    10:8    R/W SWR_CORE_PFM1POS_POF_WAIT                       3'b000                  SWR_CORE_REG_MASK
    13:11   R/W SWR_CORE_PFM1POS_PON_WAIT                       3'b000                  SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFM1POS_REG1X_DUMMY14                  1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFM1POS_REG1X_DUMMY15                  1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG1X_SWR_CORE_PFM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM1POS_POF_STEP: 4;
        uint16_t SWR_CORE_PFM1POS_PON_STEP: 4;
        uint16_t SWR_CORE_PFM1POS_POF_WAIT: 3;
        uint16_t SWR_CORE_PFM1POS_PON_WAIT: 3;
        uint16_t SWR_CORE_PFM1POS_REG1X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM1POS_REG1X_DUMMY15: 1;
    };
} AON_FAST_REG1X_SWR_CORE_PFM1POS_TYPE;

/* 0x10AE
    7:0     R/W SWR_CORE_PFM1POS_PON_OVER                       8'b10001100             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFM1POS_PON_START                      8'b00000000             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG2X_SWR_CORE_PFM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM1POS_PON_OVER: 8;
        uint16_t SWR_CORE_PFM1POS_PON_START: 8;
    };
} AON_FAST_REG2X_SWR_CORE_PFM1POS_TYPE;

/* 0x10B0
    7:0     R/W SWR_CORE_PFM1POS_POF_OVER                       8'b00000000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFM1POS_POF_START                      8'b10001100             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG3X_SWR_CORE_PFM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM1POS_POF_OVER: 8;
        uint16_t SWR_CORE_PFM1POS_POF_START: 8;
    };
} AON_FAST_REG3X_SWR_CORE_PFM1POS_TYPE;

/* 0x10B2
    7:0     R/W SWR_CORE_PFM1POS_SET_PON_FLAG2                  8'b01111000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFM1POS_SET_PON_FLAG1                  8'b00000000             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG4X_SWR_CORE_PFM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM1POS_SET_PON_FLAG2: 8;
        uint16_t SWR_CORE_PFM1POS_SET_PON_FLAG1: 8;
    };
} AON_FAST_REG4X_SWR_CORE_PFM1POS_TYPE;

/* 0x10B4
    7:0     R/W SWR_CORE_PFM1POS_SET_PON_FLAG4                  8'b10001100             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFM1POS_SET_PON_FLAG3                  8'b10001100             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG5X_SWR_CORE_PFM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM1POS_SET_PON_FLAG4: 8;
        uint16_t SWR_CORE_PFM1POS_SET_PON_FLAG3: 8;
    };
} AON_FAST_REG5X_SWR_CORE_PFM1POS_TYPE;

/* 0x10B6
    7:0     R/W SWR_CORE_PFM1POS_SET_POF_FLAG2                  8'b01111000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFM1POS_SET_POF_FLAG1                  8'b10001100             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG6X_SWR_CORE_PFM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM1POS_SET_POF_FLAG2: 8;
        uint16_t SWR_CORE_PFM1POS_SET_POF_FLAG1: 8;
    };
} AON_FAST_REG6X_SWR_CORE_PFM1POS_TYPE;

/* 0x10B8
    7:0     R/W SWR_CORE_PFM1POS_SET_POF_FLAG4                  8'b00000000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFM1POS_SET_POF_FLAG3                  8'b00000000             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG7X_SWR_CORE_PFM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM1POS_SET_POF_FLAG4: 8;
        uint16_t SWR_CORE_PFM1POS_SET_POF_FLAG3: 8;
    };
} AON_FAST_REG7X_SWR_CORE_PFM1POS_TYPE;

/* 0x10BA
    0       R   SWR_CORE_PFM1POS_POF_FLAG4                      1'b0
    1       R   SWR_CORE_PFM1POS_POF_FLAG3                      1'b0
    2       R   SWR_CORE_PFM1POS_POF_FLAG2                      1'b0
    3       R   SWR_CORE_PFM1POS_POF_FLAG1                      1'b0
    4       R   SWR_CORE_PFM1POS_PON_FLAG4                      1'b0
    5       R   SWR_CORE_PFM1POS_PON_FLAG3                      1'b0
    6       R   SWR_CORE_PFM1POS_PON_FLAG2                      1'b0
    7       R   SWR_CORE_PFM1POS_PON_FLAG1                      1'b0
    10:8    R   SWR_CORE_PFM1POS_FSM_CS                         3'h0
    11      R   SWR_CORE_PFM1POS_C_KOUT0X_DUMMY11               1'b0
    12      R   SWR_CORE_PFM1POS_C_KOUT0X_DUMMY12               1'b0
    13      R   SWR_CORE_PFM1POS_C_KOUT0X_DUMMY13               1'b0
    14      R   SWR_CORE_PFM1POS_C_KOUT0X_DUMMY14               1'b0
    15      R   SWR_CORE_PFM1POS_C_KOUT0X_DUMMY15               1'b0
 */
typedef union _AON_FAST_C_KOUT0X_SWR_CORE_PFM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM1POS_POF_FLAG4: 1;
        uint16_t SWR_CORE_PFM1POS_POF_FLAG3: 1;
        uint16_t SWR_CORE_PFM1POS_POF_FLAG2: 1;
        uint16_t SWR_CORE_PFM1POS_POF_FLAG1: 1;
        uint16_t SWR_CORE_PFM1POS_PON_FLAG4: 1;
        uint16_t SWR_CORE_PFM1POS_PON_FLAG3: 1;
        uint16_t SWR_CORE_PFM1POS_PON_FLAG2: 1;
        uint16_t SWR_CORE_PFM1POS_PON_FLAG1: 1;
        uint16_t SWR_CORE_PFM1POS_FSM_CS: 3;
        uint16_t SWR_CORE_PFM1POS_C_KOUT0X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM1POS_C_KOUT0X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM1POS_C_KOUT0X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM1POS_C_KOUT0X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM1POS_C_KOUT0X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT0X_SWR_CORE_PFM1POS_TYPE;

/* 0x10BC
    7:0     R   SWR_CORE_PFM1POS_DR                             8'h0
    8       R   SWR_CORE_PFM1POS_C_KOUT1X_DUMMY8                1'h0
    9       R   SWR_CORE_PFM1POS_C_KOUT1X_DUMMY9                1'h0
    10      R   SWR_CORE_PFM1POS_C_KOUT1X_DUMMY10               1'h0
    11      R   SWR_CORE_PFM1POS_C_KOUT1X_DUMMY11               1'h0
    12      R   SWR_CORE_PFM1POS_C_KOUT1X_DUMMY12               1'h0
    13      R   SWR_CORE_PFM1POS_C_KOUT1X_DUMMY13               1'h0
    14      R   SWR_CORE_PFM1POS_C_KOUT1X_DUMMY14               1'h0
    15      R   SWR_CORE_PFM1POS_C_KOUT1X_DUMMY15               1'h0
 */
typedef union _AON_FAST_C_KOUT1X_SWR_CORE_PFM1POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM1POS_DR: 8;
        uint16_t SWR_CORE_PFM1POS_C_KOUT1X_DUMMY8: 1;
        uint16_t SWR_CORE_PFM1POS_C_KOUT1X_DUMMY9: 1;
        uint16_t SWR_CORE_PFM1POS_C_KOUT1X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM1POS_C_KOUT1X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM1POS_C_KOUT1X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM1POS_C_KOUT1X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM1POS_C_KOUT1X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM1POS_C_KOUT1X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT1X_SWR_CORE_PFM1POS_TYPE;

/* 0x10BE
    0       R/W SWR_CORE_PFM2POS_INT_RST_B                      1'b0                    SWR_CORE_REG_MASK
    1       R/W SWR_CORE_PFM2POS_EXT_RST_B                      1'b0                    SWR_CORE_REG_MASK
    2       R/W SWR_CORE_PFM2POS_SEL_EXT_RST_B                  1'b0                    SWR_CORE_REG_MASK
    3       R/W SWR_CORE_PFM2POS_EN_POS                         1'b0                    SWR_CORE_REG_MASK
    6:4     R/W SWR_CORE_PFM2POS_DIV_CLK                        3'b011                  SWR_CORE_REG_MASK
    7       R/W SWR_CORE_PFM2POS_REG0X_DUMMY7                   1'b0                    SWR_CORE_REG_MASK
    8       R/W SWR_CORE_PFM2POS_REG0X_DUMMY8                   1'b0                    SWR_CORE_REG_MASK
    9       R/W SWR_CORE_PFM2POS_REG0X_DUMMY9                   1'b0                    SWR_CORE_REG_MASK
    10      R/W SWR_CORE_PFM2POS_REG0X_DUMMY10                  1'b0                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_PFM2POS_REG0X_DUMMY11                  1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_PFM2POS_REG0X_DUMMY12                  1'b0                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_PFM2POS_REG0X_DUMMY13                  1'b0                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFM2POS_REG0X_DUMMY14                  1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFM2POS_REG0X_DUMMY15                  1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG0X_SWR_CORE_PFM2POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM2POS_INT_RST_B: 1;
        uint16_t SWR_CORE_PFM2POS_EXT_RST_B: 1;
        uint16_t SWR_CORE_PFM2POS_SEL_EXT_RST_B: 1;
        uint16_t SWR_CORE_PFM2POS_EN_POS: 1;
        uint16_t SWR_CORE_PFM2POS_DIV_CLK: 3;
        uint16_t SWR_CORE_PFM2POS_REG0X_DUMMY7: 1;
        uint16_t SWR_CORE_PFM2POS_REG0X_DUMMY8: 1;
        uint16_t SWR_CORE_PFM2POS_REG0X_DUMMY9: 1;
        uint16_t SWR_CORE_PFM2POS_REG0X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM2POS_REG0X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM2POS_REG0X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM2POS_REG0X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM2POS_REG0X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM2POS_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_SWR_CORE_PFM2POS_TYPE;

/* 0x10C0
    3:0     R/W SWR_CORE_PFM2POS_POF_STEP                       4'b0101                 SWR_CORE_REG_MASK
    7:4     R/W SWR_CORE_PFM2POS_PON_STEP                       4'b0101                 SWR_CORE_REG_MASK
    10:8    R/W SWR_CORE_PFM2POS_POF_WAIT                       3'b000                  SWR_CORE_REG_MASK
    13:11   R/W SWR_CORE_PFM2POS_PON_WAIT                       3'b000                  SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFM2POS_REG1X_DUMMY14                  1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFM2POS_REG1X_DUMMY15                  1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG1X_SWR_CORE_PFM2POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM2POS_POF_STEP: 4;
        uint16_t SWR_CORE_PFM2POS_PON_STEP: 4;
        uint16_t SWR_CORE_PFM2POS_POF_WAIT: 3;
        uint16_t SWR_CORE_PFM2POS_PON_WAIT: 3;
        uint16_t SWR_CORE_PFM2POS_REG1X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM2POS_REG1X_DUMMY15: 1;
    };
} AON_FAST_REG1X_SWR_CORE_PFM2POS_TYPE;

/* 0x10C2
    7:0     R/W SWR_CORE_PFM2POS_PON_OVER                       8'b10001100             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFM2POS_PON_START                      8'b00000000             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG2X_SWR_CORE_PFM2POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM2POS_PON_OVER: 8;
        uint16_t SWR_CORE_PFM2POS_PON_START: 8;
    };
} AON_FAST_REG2X_SWR_CORE_PFM2POS_TYPE;

/* 0x10C4
    7:0     R/W SWR_CORE_PFM2POS_POF_OVER                       8'b00000000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFM2POS_POF_START                      8'b10001100             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG3X_SWR_CORE_PFM2POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM2POS_POF_OVER: 8;
        uint16_t SWR_CORE_PFM2POS_POF_START: 8;
    };
} AON_FAST_REG3X_SWR_CORE_PFM2POS_TYPE;

/* 0x10C6
    7:0     R/W SWR_CORE_PFM2POS_SET_PON_FLAG2                  8'b01111000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFM2POS_SET_PON_FLAG1                  8'b00000000             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG4X_SWR_CORE_PFM2POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM2POS_SET_PON_FLAG2: 8;
        uint16_t SWR_CORE_PFM2POS_SET_PON_FLAG1: 8;
    };
} AON_FAST_REG4X_SWR_CORE_PFM2POS_TYPE;

/* 0x10C8
    7:0     R/W SWR_CORE_PFM2POS_SET_PON_FLAG4                  8'b10001100             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFM2POS_SET_PON_FLAG3                  8'b10001100             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG5X_SWR_CORE_PFM2POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM2POS_SET_PON_FLAG4: 8;
        uint16_t SWR_CORE_PFM2POS_SET_PON_FLAG3: 8;
    };
} AON_FAST_REG5X_SWR_CORE_PFM2POS_TYPE;

/* 0x10CA
    7:0     R/W SWR_CORE_PFM2POS_SET_POF_FLAG2                  8'b01111000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFM2POS_SET_POF_FLAG1                  8'b10001100             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG6X_SWR_CORE_PFM2POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM2POS_SET_POF_FLAG2: 8;
        uint16_t SWR_CORE_PFM2POS_SET_POF_FLAG1: 8;
    };
} AON_FAST_REG6X_SWR_CORE_PFM2POS_TYPE;

/* 0x10CC
    7:0     R/W SWR_CORE_PFM2POS_SET_POF_FLAG4                  8'b00000000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_PFM2POS_SET_POF_FLAG3                  8'b00000000             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG7X_SWR_CORE_PFM2POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM2POS_SET_POF_FLAG4: 8;
        uint16_t SWR_CORE_PFM2POS_SET_POF_FLAG3: 8;
    };
} AON_FAST_REG7X_SWR_CORE_PFM2POS_TYPE;

/* 0x10CE
    0       R   SWR_CORE_PFM2POS_POF_FLAG4                      1'b0
    1       R   SWR_CORE_PFM2POS_POF_FLAG3                      1'b0
    2       R   SWR_CORE_PFM2POS_POF_FLAG2                      1'b0
    3       R   SWR_CORE_PFM2POS_POF_FLAG1                      1'b0
    4       R   SWR_CORE_PFM2POS_PON_FLAG4                      1'b0
    5       R   SWR_CORE_PFM2POS_PON_FLAG3                      1'b0
    6       R   SWR_CORE_PFM2POS_PON_FLAG2                      1'b0
    7       R   SWR_CORE_PFM2POS_PON_FLAG1                      1'b0
    10:8    R   SWR_CORE_PFM2POS_FSM_CS                         3'h0
    11      R   SWR_CORE_PFM2POS_C_KOUT0X_DUMMY11               1'b0
    12      R   SWR_CORE_PFM2POS_C_KOUT0X_DUMMY12               1'b0
    13      R   SWR_CORE_PFM2POS_C_KOUT0X_DUMMY13               1'b0
    14      R   SWR_CORE_PFM2POS_C_KOUT0X_DUMMY14               1'b0
    15      R   SWR_CORE_PFM2POS_C_KOUT0X_DUMMY15               1'b0
 */
typedef union _AON_FAST_C_KOUT0X_SWR_CORE_PFM2POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM2POS_POF_FLAG4: 1;
        uint16_t SWR_CORE_PFM2POS_POF_FLAG3: 1;
        uint16_t SWR_CORE_PFM2POS_POF_FLAG2: 1;
        uint16_t SWR_CORE_PFM2POS_POF_FLAG1: 1;
        uint16_t SWR_CORE_PFM2POS_PON_FLAG4: 1;
        uint16_t SWR_CORE_PFM2POS_PON_FLAG3: 1;
        uint16_t SWR_CORE_PFM2POS_PON_FLAG2: 1;
        uint16_t SWR_CORE_PFM2POS_PON_FLAG1: 1;
        uint16_t SWR_CORE_PFM2POS_FSM_CS: 3;
        uint16_t SWR_CORE_PFM2POS_C_KOUT0X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM2POS_C_KOUT0X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM2POS_C_KOUT0X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM2POS_C_KOUT0X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM2POS_C_KOUT0X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT0X_SWR_CORE_PFM2POS_TYPE;

/* 0x10D0
    7:0     R   SWR_CORE_PFM2POS_DR                             8'h0
    8       R   SWR_CORE_PFM2POS_C_KOUT1X_DUMMY8                1'h0
    9       R   SWR_CORE_PFM2POS_C_KOUT1X_DUMMY9                1'h0
    10      R   SWR_CORE_PFM2POS_C_KOUT1X_DUMMY10               1'h0
    11      R   SWR_CORE_PFM2POS_C_KOUT1X_DUMMY11               1'h0
    12      R   SWR_CORE_PFM2POS_C_KOUT1X_DUMMY12               1'h0
    13      R   SWR_CORE_PFM2POS_C_KOUT1X_DUMMY13               1'h0
    14      R   SWR_CORE_PFM2POS_C_KOUT1X_DUMMY14               1'h0
    15      R   SWR_CORE_PFM2POS_C_KOUT1X_DUMMY15               1'h0
 */
typedef union _AON_FAST_C_KOUT1X_SWR_CORE_PFM2POS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM2POS_DR: 8;
        uint16_t SWR_CORE_PFM2POS_C_KOUT1X_DUMMY8: 1;
        uint16_t SWR_CORE_PFM2POS_C_KOUT1X_DUMMY9: 1;
        uint16_t SWR_CORE_PFM2POS_C_KOUT1X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM2POS_C_KOUT1X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM2POS_C_KOUT1X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM2POS_C_KOUT1X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM2POS_C_KOUT1X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM2POS_C_KOUT1X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT1X_SWR_CORE_PFM2POS_TYPE;

/* 0x10D2
    0       R/W SWR_CORE_PFM300to600K_RST_B                     1'b0                    SWR_CORE_REG_MASK
    1       R/W SWR_CORE_PFM300to600K_EN_32K_AUDIO              1'b0                    SWR_CORE_REG_MASK
    3:2     R/W SWR_CORE_PFM300to600K_TUNE_32K_SAMPLE_CYC       2'b00                   SWR_CORE_REG_MASK
    8:4     R/W SWR_CORE_PFM300to600K_TUNE_CCOT_INIT            5'b00011                SWR_CORE_REG_MASK
    9       R/W SWR_CORE_PFM300to600K_REG0X_DUMMY9              1'b0                    SWR_CORE_REG_MASK
    10      R/W SWR_CORE_PFM300to600K_REG0X_DUMMY10             1'b1                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_PFM300to600K_REG0X_DUMMY11             1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_PFM300to600K_REG0X_DUMMY12             1'b1                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_PFM300to600K_REG0X_DUMMY13             1'b1                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFM300to600K_REG0X_DUMMY14             1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFM300to600K_REG0X_DUMMY15             1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG0X_SWR_CORE_PFM300to600K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM300to600K_RST_B: 1;
        uint16_t SWR_CORE_PFM300to600K_EN_32K_AUDIO: 1;
        uint16_t SWR_CORE_PFM300to600K_TUNE_32K_SAMPLE_CYC: 2;
        uint16_t SWR_CORE_PFM300to600K_TUNE_CCOT_INIT: 5;
        uint16_t SWR_CORE_PFM300to600K_REG0X_DUMMY9: 1;
        uint16_t SWR_CORE_PFM300to600K_REG0X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM300to600K_REG0X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM300to600K_REG0X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM300to600K_REG0X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM300to600K_REG0X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM300to600K_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_SWR_CORE_PFM300to600K_TYPE;

/* 0x10D4
    9:0     R/W SWR_CORE_PFM300to600K_TUNE_PFM_LOWER_BND        10'b0000001010          SWR_CORE_REG_MASK
    10      R/W SWR_CORE_PFM300to600K_REG1X_DUMMY10             1'b0                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_PFM300to600K_REG1X_DUMMY11             1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_PFM300to600K_REG1X_DUMMY12             1'b1                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_PFM300to600K_REG1X_DUMMY13             1'b1                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFM300to600K_REG1X_DUMMY14             1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFM300to600K_REG1X_DUMMY15             1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG1X_SWR_CORE_PFM300to600K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM300to600K_TUNE_PFM_LOWER_BND: 10;
        uint16_t SWR_CORE_PFM300to600K_REG1X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM300to600K_REG1X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM300to600K_REG1X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM300to600K_REG1X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM300to600K_REG1X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM300to600K_REG1X_DUMMY15: 1;
    };
} AON_FAST_REG1X_SWR_CORE_PFM300to600K_TYPE;

/* 0x10D6
    9:0     R/W SWR_CORE_PFM300to600K_TUNE_PFM_UPPER_BND        10'b0000010011          SWR_CORE_REG_MASK
    10      R/W SWR_CORE_PFM300to600K_REG2X_DUMMY10             1'b0                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_PFM300to600K_REG2X_DUMMY11             1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_PFM300to600K_REG2X_DUMMY12             1'b1                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_PFM300to600K_REG2X_DUMMY13             1'b1                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFM300to600K_REG2X_DUMMY14             1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFM300to600K_REG2X_DUMMY15             1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG2X_SWR_CORE_PFM300to600K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM300to600K_TUNE_PFM_UPPER_BND: 10;
        uint16_t SWR_CORE_PFM300to600K_REG2X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM300to600K_REG2X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM300to600K_REG2X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM300to600K_REG2X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM300to600K_REG2X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM300to600K_REG2X_DUMMY15: 1;
    };
} AON_FAST_REG2X_SWR_CORE_PFM300to600K_TYPE;

/* 0x10D8
    0       R/W SWR_CORE_PFM300to600K_SEL_NI_ON                 1'b0                    SWR_CORE_REG_MASK
    5:1     R/W SWR_CORE_PFM300to600K_TUNE_CCOT_FORCE           5'b01100                SWR_CORE_REG_MASK
    7:6     R/W SWR_CORE_PFM300to600K_TUNE_PFM_COMP_SAMPLE_CYC  2'b00                   SWR_CORE_REG_MASK
    8       R/W SWR_CORE_PFM300to600K_EN_VDROP_DET              1'b0                    SWR_CORE_REG_MASK
    9       R/W SWR_CORE_PFM300to600K_REG3X_DUMMY9              1'b0                    SWR_CORE_REG_MASK
    10      R/W SWR_CORE_PFM300to600K_REG3X_DUMMY10             1'b1                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_PFM300to600K_REG3X_DUMMY11             1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_PFM300to600K_REG3X_DUMMY12             1'b1                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_PFM300to600K_REG3X_DUMMY13             1'b1                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFM300to600K_REG3X_DUMMY14             1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFM300to600K_REG3X_DUMMY15             1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG3X_SWR_CORE_PFM300to600K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM300to600K_SEL_NI_ON: 1;
        uint16_t SWR_CORE_PFM300to600K_TUNE_CCOT_FORCE: 5;
        uint16_t SWR_CORE_PFM300to600K_TUNE_PFM_COMP_SAMPLE_CYC: 2;
        uint16_t SWR_CORE_PFM300to600K_EN_VDROP_DET: 1;
        uint16_t SWR_CORE_PFM300to600K_REG3X_DUMMY9: 1;
        uint16_t SWR_CORE_PFM300to600K_REG3X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM300to600K_REG3X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM300to600K_REG3X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM300to600K_REG3X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM300to600K_REG3X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM300to600K_REG3X_DUMMY15: 1;
    };
} AON_FAST_REG3X_SWR_CORE_PFM300to600K_TYPE;

/* 0x10DA
    4:0     R/W SWR_CORE_PFM300to600K_TUNE_CCOT_MANU_CODE       5'b00011                SWR_CORE_REG_MASK
    5       R/W SWR_CORE_PFM300to600K_SEL_CCOT_MANU_MODE        1'b1                    SWR_CORE_REG_MASK
    6       R/W SWR_CORE_PFM300to600K_REG4X_DUMMY6              1'b1                    SWR_CORE_REG_MASK
    7       R/W SWR_CORE_PFM300to600K_REG4X_DUMMY7              1'b1                    SWR_CORE_REG_MASK
    8       R/W SWR_CORE_PFM300to600K_REG4X_DUMMY8              1'b1                    SWR_CORE_REG_MASK
    9       R/W SWR_CORE_PFM300to600K_REG4X_DUMMY9              1'b1                    SWR_CORE_REG_MASK
    10      R/W SWR_CORE_PFM300to600K_REG4X_DUMMY10             1'b1                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_PFM300to600K_REG4X_DUMMY11             1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_PFM300to600K_REG4X_DUMMY12             1'b0                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_PFM300to600K_REG4X_DUMMY13             1'b0                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFM300to600K_REG4X_DUMMY14             1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFM300to600K_REG4X_DUMMY15             1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG4X_SWR_CORE_PFM300to600K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM300to600K_TUNE_CCOT_MANU_CODE: 5;
        uint16_t SWR_CORE_PFM300to600K_SEL_CCOT_MANU_MODE: 1;
        uint16_t SWR_CORE_PFM300to600K_REG4X_DUMMY6: 1;
        uint16_t SWR_CORE_PFM300to600K_REG4X_DUMMY7: 1;
        uint16_t SWR_CORE_PFM300to600K_REG4X_DUMMY8: 1;
        uint16_t SWR_CORE_PFM300to600K_REG4X_DUMMY9: 1;
        uint16_t SWR_CORE_PFM300to600K_REG4X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM300to600K_REG4X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM300to600K_REG4X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM300to600K_REG4X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM300to600K_REG4X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM300to600K_REG4X_DUMMY15: 1;
    };
} AON_FAST_REG4X_SWR_CORE_PFM300to600K_TYPE;

/* 0x10DC
    0       R/W SWR_CORE_PFM300to600K_REG5X_DUMMY0              1'b1                    SWR_CORE_REG_MASK
    1       R/W SWR_CORE_PFM300to600K_REG5X_DUMMY1              1'b1                    SWR_CORE_REG_MASK
    2       R/W SWR_CORE_PFM300to600K_REG5X_DUMMY2              1'b1                    SWR_CORE_REG_MASK
    3       R/W SWR_CORE_PFM300to600K_REG5X_DUMMY3              1'b1                    SWR_CORE_REG_MASK
    4       R/W SWR_CORE_PFM300to600K_REG5X_DUMMY4              1'b1                    SWR_CORE_REG_MASK
    5       R/W SWR_CORE_PFM300to600K_REG5X_DUMMY5              1'b1                    SWR_CORE_REG_MASK
    6       R/W SWR_CORE_PFM300to600K_REG5X_DUMMY6              1'b1                    SWR_CORE_REG_MASK
    7       R/W SWR_CORE_PFM300to600K_REG5X_DUMMY7              1'b1                    SWR_CORE_REG_MASK
    8       R/W SWR_CORE_PFM300to600K_REG5X_DUMMY8              1'b0                    SWR_CORE_REG_MASK
    9       R/W SWR_CORE_PFM300to600K_REG5X_DUMMY9              1'b0                    SWR_CORE_REG_MASK
    10      R/W SWR_CORE_PFM300to600K_REG5X_DUMMY10             1'b0                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_PFM300to600K_REG5X_DUMMY11             1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_PFM300to600K_REG5X_DUMMY12             1'b0                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_PFM300to600K_REG5X_DUMMY13             1'b0                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFM300to600K_REG5X_DUMMY14             1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFM300to600K_REG5X_DUMMY15             1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG5X_SWR_CORE_PFM300to600K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM300to600K_REG5X_DUMMY0: 1;
        uint16_t SWR_CORE_PFM300to600K_REG5X_DUMMY1: 1;
        uint16_t SWR_CORE_PFM300to600K_REG5X_DUMMY2: 1;
        uint16_t SWR_CORE_PFM300to600K_REG5X_DUMMY3: 1;
        uint16_t SWR_CORE_PFM300to600K_REG5X_DUMMY4: 1;
        uint16_t SWR_CORE_PFM300to600K_REG5X_DUMMY5: 1;
        uint16_t SWR_CORE_PFM300to600K_REG5X_DUMMY6: 1;
        uint16_t SWR_CORE_PFM300to600K_REG5X_DUMMY7: 1;
        uint16_t SWR_CORE_PFM300to600K_REG5X_DUMMY8: 1;
        uint16_t SWR_CORE_PFM300to600K_REG5X_DUMMY9: 1;
        uint16_t SWR_CORE_PFM300to600K_REG5X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM300to600K_REG5X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM300to600K_REG5X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM300to600K_REG5X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM300to600K_REG5X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM300to600K_REG5X_DUMMY15: 1;
    };
} AON_FAST_REG5X_SWR_CORE_PFM300to600K_TYPE;

/* 0x10DE
    9:0     R   SWR_CORE_PFM300to600K_PFM_COUNT                 10'b0000000000
    14:10   R   SWR_CORE_PFM300to600K_CCOT                      5'b00011
    15      R   SWR_CORE_PFM300to600K_EN_AUDK                   1'b0
 */
typedef union _AON_FAST_C_KOUT0X_SWR_CORE_PFM300to600K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM300to600K_PFM_COUNT: 10;
        uint16_t SWR_CORE_PFM300to600K_CCOT: 5;
        uint16_t SWR_CORE_PFM300to600K_EN_AUDK: 1;
    };
} AON_FAST_C_KOUT0X_SWR_CORE_PFM300to600K_TYPE;

/* 0x10E0
    0       R   SWR_CORE_PFM300to600K_PFM_DOWN_TT               1'h0
    1       R   SWR_CORE_PFM300to600K_PFM_UP_TT                 1'h0
    2       R   SWR_CORE_PFM300to600K_SEL_FORCE                 1'h0
    3       R   SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY3           1'h0
    4       R   SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY4           1'h0
    5       R   SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY5           1'h0
    6       R   SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY6           1'h0
    7       R   SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY7           1'h0
    8       R   SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY8           1'h0
    9       R   SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY9           1'h0
    10      R   SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY10          1'h0
    11      R   SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY11          1'h0
    12      R   SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY12          1'h0
    13      R   SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY13          1'h0
    14      R   SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY14          1'h0
    15      R   SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY15          1'h0
 */
typedef union _AON_FAST_C_KOUT1X_SWR_CORE_PFM300to600K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM300to600K_PFM_DOWN_TT: 1;
        uint16_t SWR_CORE_PFM300to600K_PFM_UP_TT: 1;
        uint16_t SWR_CORE_PFM300to600K_SEL_FORCE: 1;
        uint16_t SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY3: 1;
        uint16_t SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY4: 1;
        uint16_t SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY5: 1;
        uint16_t SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY6: 1;
        uint16_t SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY7: 1;
        uint16_t SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY8: 1;
        uint16_t SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY9: 1;
        uint16_t SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM300to600K_C_KOUT1X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT1X_SWR_CORE_PFM300to600K_TYPE;

/* 0x10E2
    0       R/W SWR_CORE_PFM300to600K2_RST_B                    1'b0                    SWR_CORE_REG_MASK
    1       R/W SWR_CORE_PFM300to600K2_EN_32K_AUDIO             1'b0                    SWR_CORE_REG_MASK
    3:2     R/W SWR_CORE_PFM300to600K2_TUNE_32K_SAMPLE_CYC      2'b00                   SWR_CORE_REG_MASK
    8:4     R/W SWR_CORE_PFM300to600K2_TUNE_CCOT_INIT           5'b00011                SWR_CORE_REG_MASK
    9       R/W SWR_CORE_PFM300to600K2_REG0X_DUMMY9             1'b0                    SWR_CORE_REG_MASK
    10      R/W SWR_CORE_PFM300to600K2_REG0X_DUMMY10            1'b1                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_PFM300to600K2_REG0X_DUMMY11            1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_PFM300to600K2_REG0X_DUMMY12            1'b1                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_PFM300to600K2_REG0X_DUMMY13            1'b1                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFM300to600K2_REG0X_DUMMY14            1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFM300to600K2_REG0X_DUMMY15            1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG0X_SWR_CORE_PFM300to600K2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM300to600K2_RST_B: 1;
        uint16_t SWR_CORE_PFM300to600K2_EN_32K_AUDIO: 1;
        uint16_t SWR_CORE_PFM300to600K2_TUNE_32K_SAMPLE_CYC: 2;
        uint16_t SWR_CORE_PFM300to600K2_TUNE_CCOT_INIT: 5;
        uint16_t SWR_CORE_PFM300to600K2_REG0X_DUMMY9: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG0X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG0X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG0X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG0X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG0X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_SWR_CORE_PFM300to600K2_TYPE;

/* 0x10E4
    9:0     R/W SWR_CORE_PFM300to600K2_TUNE_PFM_LOWER_BND       10'b0000001010          SWR_CORE_REG_MASK
    10      R/W SWR_CORE_PFM300to600K2_REG1X_DUMMY10            1'b0                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_PFM300to600K2_REG1X_DUMMY11            1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_PFM300to600K2_REG1X_DUMMY12            1'b1                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_PFM300to600K2_REG1X_DUMMY13            1'b1                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFM300to600K2_REG1X_DUMMY14            1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFM300to600K2_REG1X_DUMMY15            1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG1X_SWR_CORE_PFM300to600K2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM300to600K2_TUNE_PFM_LOWER_BND: 10;
        uint16_t SWR_CORE_PFM300to600K2_REG1X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG1X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG1X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG1X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG1X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG1X_DUMMY15: 1;
    };
} AON_FAST_REG1X_SWR_CORE_PFM300to600K2_TYPE;

/* 0x10E6
    9:0     R/W SWR_CORE_PFM300to600K2_TUNE_PFM_UPPER_BND       10'b0000010011          SWR_CORE_REG_MASK
    10      R/W SWR_CORE_PFM300to600K2_REG2X_DUMMY10            1'b0                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_PFM300to600K2_REG2X_DUMMY11            1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_PFM300to600K2_REG2X_DUMMY12            1'b1                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_PFM300to600K2_REG2X_DUMMY13            1'b1                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFM300to600K2_REG2X_DUMMY14            1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFM300to600K2_REG2X_DUMMY15            1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG2X_SWR_CORE_PFM300to600K2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM300to600K2_TUNE_PFM_UPPER_BND: 10;
        uint16_t SWR_CORE_PFM300to600K2_REG2X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG2X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG2X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG2X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG2X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG2X_DUMMY15: 1;
    };
} AON_FAST_REG2X_SWR_CORE_PFM300to600K2_TYPE;

/* 0x10E8
    0       R/W SWR_CORE_PFM300to600K2_SEL_NI_ON                1'b0                    SWR_CORE_REG_MASK
    5:1     R/W SWR_CORE_PFM300to600K2_TUNE_CCOT_FORCE          5'b01100                SWR_CORE_REG_MASK
    7:6     R/W SWR_CORE_PFM300to600K2_TUNE_PFM_COMP_SAMPLE_CYC 2'b00                   SWR_CORE_REG_MASK
    8       R/W SWR_CORE_PFM300to600K2_EN_VDROP_DET             1'b0                    SWR_CORE_REG_MASK
    9       R/W SWR_CORE_PFM300to600K2_REG3X_DUMMY9             1'b0                    SWR_CORE_REG_MASK
    10      R/W SWR_CORE_PFM300to600K2_REG3X_DUMMY10            1'b1                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_PFM300to600K2_REG3X_DUMMY11            1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_PFM300to600K2_REG3X_DUMMY12            1'b1                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_PFM300to600K2_REG3X_DUMMY13            1'b1                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFM300to600K2_REG3X_DUMMY14            1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFM300to600K2_REG3X_DUMMY15            1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG3X_SWR_CORE_PFM300to600K2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM300to600K2_SEL_NI_ON: 1;
        uint16_t SWR_CORE_PFM300to600K2_TUNE_CCOT_FORCE: 5;
        uint16_t SWR_CORE_PFM300to600K2_TUNE_PFM_COMP_SAMPLE_CYC: 2;
        uint16_t SWR_CORE_PFM300to600K2_EN_VDROP_DET: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG3X_DUMMY9: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG3X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG3X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG3X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG3X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG3X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG3X_DUMMY15: 1;
    };
} AON_FAST_REG3X_SWR_CORE_PFM300to600K2_TYPE;

/* 0x10EA
    4:0     R/W SWR_CORE_PFM300to600K2_TUNE_CCOT_MANU_CODE      5'b00011                SWR_CORE_REG_MASK
    5       R/W SWR_CORE_PFM300to600K2_SEL_CCOT_MANU_MODE       1'b1                    SWR_CORE_REG_MASK
    6       R/W SWR_CORE_PFM300to600K2_REG4X_DUMMY6             1'b1                    SWR_CORE_REG_MASK
    7       R/W SWR_CORE_PFM300to600K2_REG4X_DUMMY7             1'b1                    SWR_CORE_REG_MASK
    8       R/W SWR_CORE_PFM300to600K2_REG4X_DUMMY8             1'b1                    SWR_CORE_REG_MASK
    9       R/W SWR_CORE_PFM300to600K2_REG4X_DUMMY9             1'b1                    SWR_CORE_REG_MASK
    10      R/W SWR_CORE_PFM300to600K2_REG4X_DUMMY10            1'b1                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_PFM300to600K2_REG4X_DUMMY11            1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_PFM300to600K2_REG4X_DUMMY12            1'b0                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_PFM300to600K2_REG4X_DUMMY13            1'b0                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFM300to600K2_REG4X_DUMMY14            1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFM300to600K2_REG4X_DUMMY15            1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG4X_SWR_CORE_PFM300to600K2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM300to600K2_TUNE_CCOT_MANU_CODE: 5;
        uint16_t SWR_CORE_PFM300to600K2_SEL_CCOT_MANU_MODE: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG4X_DUMMY6: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG4X_DUMMY7: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG4X_DUMMY8: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG4X_DUMMY9: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG4X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG4X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG4X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG4X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG4X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG4X_DUMMY15: 1;
    };
} AON_FAST_REG4X_SWR_CORE_PFM300to600K2_TYPE;

/* 0x10EC
    0       R/W SWR_CORE_PFM300to600K2_REG5X_DUMMY0             1'b1                    SWR_CORE_REG_MASK
    1       R/W SWR_CORE_PFM300to600K2_REG5X_DUMMY1             1'b1                    SWR_CORE_REG_MASK
    2       R/W SWR_CORE_PFM300to600K2_REG5X_DUMMY2             1'b1                    SWR_CORE_REG_MASK
    3       R/W SWR_CORE_PFM300to600K2_REG5X_DUMMY3             1'b1                    SWR_CORE_REG_MASK
    4       R/W SWR_CORE_PFM300to600K2_REG5X_DUMMY4             1'b1                    SWR_CORE_REG_MASK
    5       R/W SWR_CORE_PFM300to600K2_REG5X_DUMMY5             1'b1                    SWR_CORE_REG_MASK
    6       R/W SWR_CORE_PFM300to600K2_REG5X_DUMMY6             1'b1                    SWR_CORE_REG_MASK
    7       R/W SWR_CORE_PFM300to600K2_REG5X_DUMMY7             1'b1                    SWR_CORE_REG_MASK
    8       R/W SWR_CORE_PFM300to600K2_REG5X_DUMMY8             1'b0                    SWR_CORE_REG_MASK
    9       R/W SWR_CORE_PFM300to600K2_REG5X_DUMMY9             1'b0                    SWR_CORE_REG_MASK
    10      R/W SWR_CORE_PFM300to600K2_REG5X_DUMMY10            1'b0                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_PFM300to600K2_REG5X_DUMMY11            1'b0                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_PFM300to600K2_REG5X_DUMMY12            1'b0                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_PFM300to600K2_REG5X_DUMMY13            1'b0                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_PFM300to600K2_REG5X_DUMMY14            1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_PFM300to600K2_REG5X_DUMMY15            1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG5X_SWR_CORE_PFM300to600K2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM300to600K2_REG5X_DUMMY0: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG5X_DUMMY1: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG5X_DUMMY2: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG5X_DUMMY3: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG5X_DUMMY4: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG5X_DUMMY5: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG5X_DUMMY6: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG5X_DUMMY7: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG5X_DUMMY8: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG5X_DUMMY9: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG5X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG5X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG5X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG5X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG5X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM300to600K2_REG5X_DUMMY15: 1;
    };
} AON_FAST_REG5X_SWR_CORE_PFM300to600K2_TYPE;

/* 0x10EE
    9:0     R   SWR_CORE_PFM300to600K2_PFM_COUNT                10'b0000000000
    14:10   R   SWR_CORE_PFM300to600K2_CCOT                     5'b00011
    15      R   SWR_CORE_PFM300to600K2_EN_AUDK                  1'b0
 */
typedef union _AON_FAST_C_KOUT0X_SWR_CORE_PFM300to600K2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM300to600K2_PFM_COUNT: 10;
        uint16_t SWR_CORE_PFM300to600K2_CCOT: 5;
        uint16_t SWR_CORE_PFM300to600K2_EN_AUDK: 1;
    };
} AON_FAST_C_KOUT0X_SWR_CORE_PFM300to600K2_TYPE;

/* 0x10F0
    0       R   SWR_CORE_PFM300to600K2_PFM_DOWN_TT              1'h0
    1       R   SWR_CORE_PFM300to600K2_PFM_UP_TT                1'h0
    2       R   SWR_CORE_PFM300to600K2_SEL_FORCE                1'h0
    3       R   SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY3          1'h0
    4       R   SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY4          1'h0
    5       R   SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY5          1'h0
    6       R   SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY6          1'h0
    7       R   SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY7          1'h0
    8       R   SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY8          1'h0
    9       R   SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY9          1'h0
    10      R   SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY10         1'h0
    11      R   SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY11         1'h0
    12      R   SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY12         1'h0
    13      R   SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY13         1'h0
    14      R   SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY14         1'h0
    15      R   SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY15         1'h0
 */
typedef union _AON_FAST_C_KOUT1X_SWR_CORE_PFM300to600K2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_PFM300to600K2_PFM_DOWN_TT: 1;
        uint16_t SWR_CORE_PFM300to600K2_PFM_UP_TT: 1;
        uint16_t SWR_CORE_PFM300to600K2_SEL_FORCE: 1;
        uint16_t SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY3: 1;
        uint16_t SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY4: 1;
        uint16_t SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY5: 1;
        uint16_t SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY6: 1;
        uint16_t SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY7: 1;
        uint16_t SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY8: 1;
        uint16_t SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY9: 1;
        uint16_t SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY10: 1;
        uint16_t SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY11: 1;
        uint16_t SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY12: 1;
        uint16_t SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY13: 1;
        uint16_t SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY14: 1;
        uint16_t SWR_CORE_PFM300to600K2_C_KOUT1X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT1X_SWR_CORE_PFM300to600K2_TYPE;

/* 0x10F2
    0       R   RO_AON_SWR_CORE_ZCDQ_RST_B                      1'b0
    1       R   RO_AON_SWR_CORE_ZCDQ_POW_UD_DIG                 1'b0
    2       R   RO_AON_SWR_CORE_ZCDQ_FORCE_CODE2                1'b0
    3       R/W SWR_CORE_ZCDQ_STICKY_CODE2                      1'b0                    SWR_CORE_REG_MASK
    4       R   RO_AON_SWR_CORE_ZCDQ_FORCE_CODE1                1'b0
    5       R/W SWR_CORE_ZCDQ_STICKY_CODE1                      1'b0                    SWR_CORE_REG_MASK
    6       R/W SWR_CORE_ZCDQ_EXT_UD                            1'b1                    SWR_CORE_REG_MASK
    7       R/W SWR_CORE_ZCDQ_SEL_EXT_UD                        1'b0                    SWR_CORE_REG_MASK
    8       R/W SWR_CORE_ZCDQ_REG0X_DUMMY8                      1'b1                    SWR_CORE_REG_MASK
    9       R/W SWR_CORE_ZCDQ_REG0X_DUMMY9                      1'b1                    SWR_CORE_REG_MASK
    10      R/W SWR_CORE_ZCDQ_REG0X_DUMMY10                     1'b1                    SWR_CORE_REG_MASK
    11      R/W SWR_CORE_ZCDQ_REG0X_DUMMY11                     1'b1                    SWR_CORE_REG_MASK
    12      R/W SWR_CORE_ZCDQ_REG0X_DUMMY12                     1'b0                    SWR_CORE_REG_MASK
    13      R/W SWR_CORE_ZCDQ_REG0X_DUMMY13                     1'b0                    SWR_CORE_REG_MASK
    14      R/W SWR_CORE_ZCDQ_REG0X_DUMMY14                     1'b0                    SWR_CORE_REG_MASK
    15      R/W SWR_CORE_ZCDQ_REG0X_DUMMY15                     1'b0                    SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG0X_SWR_CORE_ZCDQ_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_SWR_CORE_ZCDQ_RST_B: 1;
        uint16_t RO_AON_SWR_CORE_ZCDQ_POW_UD_DIG: 1;
        uint16_t RO_AON_SWR_CORE_ZCDQ_FORCE_CODE2: 1;
        uint16_t SWR_CORE_ZCDQ_STICKY_CODE2: 1;
        uint16_t RO_AON_SWR_CORE_ZCDQ_FORCE_CODE1: 1;
        uint16_t SWR_CORE_ZCDQ_STICKY_CODE1: 1;
        uint16_t SWR_CORE_ZCDQ_EXT_UD: 1;
        uint16_t SWR_CORE_ZCDQ_SEL_EXT_UD: 1;
        uint16_t SWR_CORE_ZCDQ_REG0X_DUMMY8: 1;
        uint16_t SWR_CORE_ZCDQ_REG0X_DUMMY9: 1;
        uint16_t SWR_CORE_ZCDQ_REG0X_DUMMY10: 1;
        uint16_t SWR_CORE_ZCDQ_REG0X_DUMMY11: 1;
        uint16_t SWR_CORE_ZCDQ_REG0X_DUMMY12: 1;
        uint16_t SWR_CORE_ZCDQ_REG0X_DUMMY13: 1;
        uint16_t SWR_CORE_ZCDQ_REG0X_DUMMY14: 1;
        uint16_t SWR_CORE_ZCDQ_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_SWR_CORE_ZCDQ_TYPE;

/* 0x10F4
    7:0     R/W SWR_CORE_ZCDQ_TUNE_FORCECODE2                   8'b01100000             SWR_CORE_REG_MASK
    15:8    R/W SWR_CORE_ZCDQ_TUNE_FORCECODE1                   8'b01100000             SWR_CORE_REG_MASK
 */
typedef union _AON_FAST_REG1X_SWR_CORE_ZCDQ_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_ZCDQ_TUNE_FORCECODE2: 8;
        uint16_t SWR_CORE_ZCDQ_TUNE_FORCECODE1: 8;
    };
} AON_FAST_REG1X_SWR_CORE_ZCDQ_TYPE;

/* 0x10F6
    7:0     R   SWR_CORE_ZCDQ_DIG_ZCDQ                          8'b01100000
    8       R   SWR_CORE_ZCDQ_C_KOUT0X_DUMMY8                   1'b0
    9       R   SWR_CORE_ZCDQ_C_KOUT0X_DUMMY9                   1'b0
    10      R   SWR_CORE_ZCDQ_C_KOUT0X_DUMMY10                  1'b0
    11      R   SWR_CORE_ZCDQ_C_KOUT0X_DUMMY11                  1'b0
    12      R   SWR_CORE_ZCDQ_C_KOUT0X_DUMMY12                  1'b0
    13      R   SWR_CORE_ZCDQ_C_KOUT0X_DUMMY13                  1'b0
    14      R   SWR_CORE_ZCDQ_C_KOUT0X_DUMMY14                  1'b0
    15      R   SWR_CORE_ZCDQ_C_KOUT0X_DUMMY15                  1'b0
 */
typedef union _AON_FAST_C_KOUT0X_SWR_CORE_ZCDQ_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_ZCDQ_DIG_ZCDQ: 8;
        uint16_t SWR_CORE_ZCDQ_C_KOUT0X_DUMMY8: 1;
        uint16_t SWR_CORE_ZCDQ_C_KOUT0X_DUMMY9: 1;
        uint16_t SWR_CORE_ZCDQ_C_KOUT0X_DUMMY10: 1;
        uint16_t SWR_CORE_ZCDQ_C_KOUT0X_DUMMY11: 1;
        uint16_t SWR_CORE_ZCDQ_C_KOUT0X_DUMMY12: 1;
        uint16_t SWR_CORE_ZCDQ_C_KOUT0X_DUMMY13: 1;
        uint16_t SWR_CORE_ZCDQ_C_KOUT0X_DUMMY14: 1;
        uint16_t SWR_CORE_ZCDQ_C_KOUT0X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT0X_SWR_CORE_ZCDQ_TYPE;

/* 0x10F8
    7:0     R   SWR_CORE_ZCDQ_DIG_ZCDQ2                         8'b01100000
    15:8    R   SWR_CORE_ZCDQ_DIG_ZCDQ1                         8'b01100000
 */
typedef union _AON_FAST_C_KOUT1X_SWR_CORE_ZCDQ_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_CORE_ZCDQ_DIG_ZCDQ2: 8;
        uint16_t SWR_CORE_ZCDQ_DIG_ZCDQ1: 8;
    };
} AON_FAST_C_KOUT1X_SWR_CORE_ZCDQ_TYPE;

/* 0x11C0
    0       R/W SWR_AUDIO_REG0X_DUMMY2                          1'b0                    SWR_AUDIO_REG_MASK
    1       R   RO_AON_SWR_AUDIO_POW_SWR                        1'b0
    2       R   RO_AON_SWR_AUDIO_POW_CCMPFM                     1'b0
    3       R/W SWR_AUDIO_REG0X_DUMMY1                          1'b0                    SWR_AUDIO_REG_MASK
    4       R   RO_AON_SWR_AUDIO_SEL_PFM_COT                    1'b0
    5       R/W SWR_AUDIO_EN_PFM_AOT                            1'b0                    SWR_AUDIO_REG_MASK
    6       R   RO_AON_SWR_AUDIO_EN_OCP                         1'b0
    7       R/W SWR_AUDIO_SEL_VREF_SSOVER                       1'b0                    SWR_AUDIO_REG_MASK
    8       R/W SWR_AUDIO_SEL_SOFTSTART                         1'b0                    SWR_AUDIO_REG_MASK
    9       R/W SWR_AUDIO_X4_PWM_COMP_IB                        1'b0                    SWR_AUDIO_REG_MASK
    10      R/W SWR_AUDIO_EN_PFM_FORCE_OFF_TO_ZCD               1'b1                    SWR_AUDIO_REG_MASK
    11      R/W SWR_AUDIO_EN_VREFSS1                            1'b1                    SWR_AUDIO_REG_MASK
    12      R/W SWR_AUDIO_EN_SR                                 1'b0                    SWR_AUDIO_REG_MASK
    13      R/W SWR_AUDIO_EN_OCPMUX                             1'b1                    SWR_AUDIO_REG_MASK
    14      R/W SWR_AUDIO_BP_ROUGH_SS1                          1'b0                    SWR_AUDIO_REG_MASK
    15      R/W SWR_AUDIO_SEL_STOP                              1'b1                    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG0X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_REG0X_DUMMY2: 1;
        uint16_t RO_AON_SWR_AUDIO_POW_SWR: 1;
        uint16_t RO_AON_SWR_AUDIO_POW_CCMPFM: 1;
        uint16_t SWR_AUDIO_REG0X_DUMMY1: 1;
        uint16_t RO_AON_SWR_AUDIO_SEL_PFM_COT: 1;
        uint16_t SWR_AUDIO_EN_PFM_AOT: 1;
        uint16_t RO_AON_SWR_AUDIO_EN_OCP: 1;
        uint16_t SWR_AUDIO_SEL_VREF_SSOVER: 1;
        uint16_t SWR_AUDIO_SEL_SOFTSTART: 1;
        uint16_t SWR_AUDIO_X4_PWM_COMP_IB: 1;
        uint16_t SWR_AUDIO_EN_PFM_FORCE_OFF_TO_ZCD: 1;
        uint16_t SWR_AUDIO_EN_VREFSS1: 1;
        uint16_t SWR_AUDIO_EN_SR: 1;
        uint16_t SWR_AUDIO_EN_OCPMUX: 1;
        uint16_t SWR_AUDIO_BP_ROUGH_SS1: 1;
        uint16_t SWR_AUDIO_SEL_STOP: 1;
    };
} AON_FAST_REG0X_SWR_AUDIO_TYPE;

/* 0x11C2
    0       R   RO_AON_SWR_AUDIO_POW_VDIV                       1'b0
    1       R   RO_AON_SWR_AUDIO_SEL_POS_VREFLPPFM              1'b0
    2       R   RO_AON_SWR_AUDIO_POW_POS_REF                    1'b0
    3       R   RO_AON_SWR_AUDIO_POW_DIVPFM                     1'b0
    4       R/W SWR_AUDIO_POW_HVD17_POR17B                      1'b1                    SWR_AUDIO_REG_MASK
    5       R/W SWR_AUDIO_REG1X_DUMMY1                          1'b1                    SWR_AUDIO_REG_MASK
    6       R   RO_AON_SWR_AUDIO_POW_PFM                        1'b0
    7       R/W SWR_AUDIO_EN_PWM_MINON                          1'b1                    SWR_AUDIO_REG_MASK
    8       R   RO_AON_SWR_AUDIO_EN_ZCD_LOW_IQ                  1'b0
    9       R/W SWR_AUDIO_SEL_OCP_LATCH                         1'b1                    SWR_AUDIO_REG_MASK
    10      R   RO_AON_SWR_AUDIO_POW_IMIR                       1'b0
    11      R   RO_AON_SWR_AUDIO_SEL_CK_CTRL                    1'b0
    12      R/W SWR_AUDIO_POW_HVD17_SHORT                       1'b0                    SWR_AUDIO_REG_MASK
    13      R/W SWR_AUDIO_SEL_OCP_RST                           1'b0                    SWR_AUDIO_REG_MASK
    14      R/W SWR_AUDIO_POW_PWM_CLP                           1'b1                    SWR_AUDIO_REG_MASK
    15      R/W SWR_AUDIO_POW_ZCD_CLAMP_LX                      1'b1                    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG1X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_SWR_AUDIO_POW_VDIV: 1;
        uint16_t RO_AON_SWR_AUDIO_SEL_POS_VREFLPPFM: 1;
        uint16_t RO_AON_SWR_AUDIO_POW_POS_REF: 1;
        uint16_t RO_AON_SWR_AUDIO_POW_DIVPFM: 1;
        uint16_t SWR_AUDIO_POW_HVD17_POR17B: 1;
        uint16_t SWR_AUDIO_REG1X_DUMMY1: 1;
        uint16_t RO_AON_SWR_AUDIO_POW_PFM: 1;
        uint16_t SWR_AUDIO_EN_PWM_MINON: 1;
        uint16_t RO_AON_SWR_AUDIO_EN_ZCD_LOW_IQ: 1;
        uint16_t SWR_AUDIO_SEL_OCP_LATCH: 1;
        uint16_t RO_AON_SWR_AUDIO_POW_IMIR: 1;
        uint16_t RO_AON_SWR_AUDIO_SEL_CK_CTRL: 1;
        uint16_t SWR_AUDIO_POW_HVD17_SHORT: 1;
        uint16_t SWR_AUDIO_SEL_OCP_RST: 1;
        uint16_t SWR_AUDIO_POW_PWM_CLP: 1;
        uint16_t SWR_AUDIO_POW_ZCD_CLAMP_LX: 1;
    };
} AON_FAST_REG1X_SWR_AUDIO_TYPE;

/* 0x11C4
    0       R/W SWR_AUDIO_BYPASS_PWM_SSR1                       1'b1                    SWR_AUDIO_REG_MASK
    3:1     R/W SWR_AUDIO_TUNE_POS_VREF_OCPPFM                  3'b000                  SWR_AUDIO_REG_MASK
    6:4     R/W SWR_AUDIO_TUNE_POS_VREF_OCP                     3'b111                  SWR_AUDIO_REG_MASK
    12:7    R/W SWR_AUDIO_TUNE_SAW_ICLK                         6'b100000               SWR_AUDIO_REG_MASK
    14:13   R/W SWR_AUDIO_TUNE_SAW_CCLK                         2'b10                   SWR_AUDIO_REG_MASK
    15      R   RO_AON_SWR_AUDIO_POW_ZCD                        1'b0
 */
typedef union _AON_FAST_REG2X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_BYPASS_PWM_SSR1: 1;
        uint16_t SWR_AUDIO_TUNE_POS_VREF_OCPPFM: 3;
        uint16_t SWR_AUDIO_TUNE_POS_VREF_OCP: 3;
        uint16_t SWR_AUDIO_TUNE_SAW_ICLK: 6;
        uint16_t SWR_AUDIO_TUNE_SAW_CCLK: 2;
        uint16_t RO_AON_SWR_AUDIO_POW_ZCD: 1;
    };
} AON_FAST_REG2X_SWR_AUDIO_TYPE;

/* 0x11C6
    1:0     R/W SWR_AUDIO_TUNE_PWM_IMINON1                      2'b11                   SWR_AUDIO_REG_MASK
    3:2     R/W SWR_AUDIO_TUNE_PFM_ICOT1                        2'b01                   SWR_AUDIO_REG_MASK
    4       R   RO_AON_SWR_AUDIO_SEL_CK_CTRL_NEW                1'b0
    7:5     R/W SWR_AUDIO_TUNE_POS_VREFPWM1                     3'b011                  SWR_AUDIO_REG_MASK
    15:8    R/W SWR_AUDIO_TUNE_POS_VREFPFM1                     8'b10110000             SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG3X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_TUNE_PWM_IMINON1: 2;
        uint16_t SWR_AUDIO_TUNE_PFM_ICOT1: 2;
        uint16_t RO_AON_SWR_AUDIO_SEL_CK_CTRL_NEW: 1;
        uint16_t SWR_AUDIO_TUNE_POS_VREFPWM1: 3;
        uint16_t SWR_AUDIO_TUNE_POS_VREFPFM1: 8;
    };
} AON_FAST_REG3X_SWR_AUDIO_TYPE;

/* 0x11C8
    2:0     R/W SWR_AUDIO_TUNE_PWM_R1_OUT1                      3'b111                  SWR_AUDIO_REG_MASK
    3       R/W SWR_AUDIO_STICKY_CODE_ZCD2                      1'b0                    SWR_AUDIO_REG_MASK
    4       R/W SWR_AUDIO_FORCE_CODE_ZCD2                       1'b0                    SWR_AUDIO_REG_MASK
    5       R/W SWR_AUDIO_EN_ZCDPWM_ZCD2                        1'b0                    SWR_AUDIO_REG_MASK
    6       R/W SWR_AUDIO_EN_ZCDPWMPS_ZCD2                      1'b0                    SWR_AUDIO_REG_MASK
    7       R/W SWR_AUDIO_EN_ZCDPFM_ZCD2                        1'b0                    SWR_AUDIO_REG_MASK
    8       R/W SWR_AUDIO_EN_ZCDPFMPS_ZCD2                      1'b0                    SWR_AUDIO_REG_MASK
    9       R/W SWR_AUDIO_SEL_PGATE_FB                          1'b0                    SWR_AUDIO_REG_MASK
    12:10   R/W SWR_AUDIO_SEL_VCL_OUT1                          3'b111                  SWR_AUDIO_REG_MASK
    15:13   R/W SWR_AUDIO_SEL_VCH_OUT1                          3'b111                  SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG4X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_TUNE_PWM_R1_OUT1: 3;
        uint16_t SWR_AUDIO_STICKY_CODE_ZCD2: 1;
        uint16_t SWR_AUDIO_FORCE_CODE_ZCD2: 1;
        uint16_t SWR_AUDIO_EN_ZCDPWM_ZCD2: 1;
        uint16_t SWR_AUDIO_EN_ZCDPWMPS_ZCD2: 1;
        uint16_t SWR_AUDIO_EN_ZCDPFM_ZCD2: 1;
        uint16_t SWR_AUDIO_EN_ZCDPFMPS_ZCD2: 1;
        uint16_t SWR_AUDIO_SEL_PGATE_FB: 1;
        uint16_t SWR_AUDIO_SEL_VCL_OUT1: 3;
        uint16_t SWR_AUDIO_SEL_VCH_OUT1: 3;
    };
} AON_FAST_REG4X_SWR_AUDIO_TYPE;

/* 0x11CA
    2:0     R/W SWR_AUDIO_TUNE_PWM_C3_OUT1                      3'b101                  SWR_AUDIO_REG_MASK
    5:3     R/W SWR_AUDIO_TUNE_PWM_C2_OUT1                      3'b000                  SWR_AUDIO_REG_MASK
    8:6     R/W SWR_AUDIO_TUNE_PWM_C1_OUT1                      3'b101                  SWR_AUDIO_REG_MASK
    11:9    R/W SWR_AUDIO_TUNE_PWM_R3_OUT1                      3'b000                  SWR_AUDIO_REG_MASK
    14:12   R/W SWR_AUDIO_TUNE_PWM_R2_OUT1                      3'b001                  SWR_AUDIO_REG_MASK
    15      R/W SWR_AUDIO_SEL_ZCD                               1'b1                    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG5X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_TUNE_PWM_C3_OUT1: 3;
        uint16_t SWR_AUDIO_TUNE_PWM_C2_OUT1: 3;
        uint16_t SWR_AUDIO_TUNE_PWM_C1_OUT1: 3;
        uint16_t SWR_AUDIO_TUNE_PWM_R3_OUT1: 3;
        uint16_t SWR_AUDIO_TUNE_PWM_R2_OUT1: 3;
        uint16_t SWR_AUDIO_SEL_ZCD: 1;
    };
} AON_FAST_REG5X_SWR_AUDIO_TYPE;

/* 0x11CC
    3:0     R/W SWR_AUDIO_TUNE_PREDRV_P_SP                      4'b1111                 SWR_AUDIO_REG_MASK
    7:4     R/W SWR_AUDIO_TUNE_PREDRV_P_SN                      4'b1111                 SWR_AUDIO_REG_MASK
    9:8     R/W SWR_AUDIO_TUNE_SS_DIVSEL                        2'b00                   SWR_AUDIO_REG_MASK
    10      R/W SWR_AUDIO_SEL_OCP2                              1'b0                    SWR_AUDIO_REG_MASK
    15:11   R/W SWR_AUDIO_TUNE_PFM_CCOT1                        5'b01000                SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG6X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_TUNE_PREDRV_P_SP: 4;
        uint16_t SWR_AUDIO_TUNE_PREDRV_P_SN: 4;
        uint16_t SWR_AUDIO_TUNE_SS_DIVSEL: 2;
        uint16_t SWR_AUDIO_SEL_OCP2: 1;
        uint16_t SWR_AUDIO_TUNE_PFM_CCOT1: 5;
    };
} AON_FAST_REG6X_SWR_AUDIO_TYPE;

/* 0x11CE
    2:0     R/W SWR_AUDIO_TUNE_NONOVERLAP_SDZP                  3'b000                  SWR_AUDIO_REG_MASK
    5:3     R/W SWR_AUDIO_TUNE_NONOVERLAP_SDZN                  3'b000                  SWR_AUDIO_REG_MASK
    9:6     R/W SWR_AUDIO_TUNE_PREDRV_N_SP                      4'b1111                 SWR_AUDIO_REG_MASK
    13:10   R/W SWR_AUDIO_TUNE_PREDRV_N_SN                      4'b1111                 SWR_AUDIO_REG_MASK
    15:14   R/W SWR_AUDIO_TUNE_OCP_RES                          2'b10                   SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG7X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_TUNE_NONOVERLAP_SDZP: 3;
        uint16_t SWR_AUDIO_TUNE_NONOVERLAP_SDZN: 3;
        uint16_t SWR_AUDIO_TUNE_PREDRV_N_SP: 4;
        uint16_t SWR_AUDIO_TUNE_PREDRV_N_SN: 4;
        uint16_t SWR_AUDIO_TUNE_OCP_RES: 2;
    };
} AON_FAST_REG7X_SWR_AUDIO_TYPE;

/* 0x11D0
    1:0     R/W SWR_AUDIO_TUNE_SDZ1                             2'b00                   SWR_AUDIO_REG_MASK
    4:2     R/W SWR_AUDIO_SEL_POR17_VREFL                       3'b011                  SWR_AUDIO_REG_MASK
    7:5     R/W SWR_AUDIO_SEL_POR17_VREFH                       3'b011                  SWR_AUDIO_REG_MASK
    8       R/W SWR_AUDIO_SEL_POR17                             1'b0                    SWR_AUDIO_REG_MASK
    9       R/W SWR_AUDIO_FORCE_HV17POR                         1'b0                    SWR_AUDIO_REG_MASK
    13:10   R/W SWR_AUDIO_TUNE_HVD17_IB                         4'b1001                 SWR_AUDIO_REG_MASK
    14      R/W SWR_AUDIO_SEL_OCP_VREF                          1'b1                    SWR_AUDIO_REG_MASK
    15      R/W SWR_AUDIO_EN_NONOVERLAP_DLY_N2FB                1'b0                    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG8X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_TUNE_SDZ1: 2;
        uint16_t SWR_AUDIO_SEL_POR17_VREFL: 3;
        uint16_t SWR_AUDIO_SEL_POR17_VREFH: 3;
        uint16_t SWR_AUDIO_SEL_POR17: 1;
        uint16_t SWR_AUDIO_FORCE_HV17POR: 1;
        uint16_t SWR_AUDIO_TUNE_HVD17_IB: 4;
        uint16_t SWR_AUDIO_SEL_OCP_VREF: 1;
        uint16_t SWR_AUDIO_EN_NONOVERLAP_DLY_N2FB: 1;
    };
} AON_FAST_REG8X_SWR_AUDIO_TYPE;

/* 0x11D2
    1:0     R/W SWR_AUDIO_TUNE_PWM_ROUGH_SS1                    2'b00                   SWR_AUDIO_REG_MASK
    9:2     R/W SWR_AUDIO_TUNE_ZCD_FORCECODE1                   8'b01100000             SWR_AUDIO_REG_MASK
    11:10   R/W SWR_AUDIO_TUNE_SDZ2D                            2'b00                   SWR_AUDIO_REG_MASK
    14:12   R/W SWR_AUDIO_TUNE_SDZ2                             3'b001                  SWR_AUDIO_REG_MASK
    15      R/W SWR_AUDIO_EN_NONOVERLAP_DLY_P2FB                1'b0                    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG9X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_TUNE_PWM_ROUGH_SS1: 2;
        uint16_t SWR_AUDIO_TUNE_ZCD_FORCECODE1: 8;
        uint16_t SWR_AUDIO_TUNE_SDZ2D: 2;
        uint16_t SWR_AUDIO_TUNE_SDZ2: 3;
        uint16_t SWR_AUDIO_EN_NONOVERLAP_DLY_P2FB: 1;
    };
} AON_FAST_REG9X_SWR_AUDIO_TYPE;

/* 0x11D4
    7:0     R   RO_AON_SWR_AUDIO_TUNE_VDIV                      8'b10000111
    15:8    R/W SWR_AUDIO_TUNE_ZCD_FORCECODE2                   8'b01100000             SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG10X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_SWR_AUDIO_TUNE_VDIV: 8;
        uint16_t SWR_AUDIO_TUNE_ZCD_FORCECODE2: 8;
    };
} AON_FAST_REG10X_SWR_AUDIO_TYPE;

/* 0x11D6
    1:0     R/W SWR_AUDIO_TUNE_ZCD2_SDZ                         2'b10                   SWR_AUDIO_REG_MASK
    11:2    R/W SWR_AUDIO_TUNE_ZCD2_PFMFORCECODE                10'b0100000000          SWR_AUDIO_REG_MASK
    15:12   R/W SWR_AUDIO_TUNE_ZCD2_IB                          4'b0111                 SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG11X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_TUNE_ZCD2_SDZ: 2;
        uint16_t SWR_AUDIO_TUNE_ZCD2_PFMFORCECODE: 10;
        uint16_t SWR_AUDIO_TUNE_ZCD2_IB: 4;
    };
} AON_FAST_REG11X_SWR_AUDIO_TYPE;

/* 0x11D8
    0       R/W SWR_AUDIO_SEL_POS_VREFPFM1                      1'b1                    SWR_AUDIO_REG_MASK
    1       R/W SWR_AUDIO_EN_HV17_LOIQ                          1'b1                    SWR_AUDIO_REG_MASK
    2       R/W SWR_AUDIO_EN_HVD17_POWDN_CURRENT                1'b1                    SWR_AUDIO_REG_MASK
    3       R/W SWR_AUDIO_STICKY_CODE_ZCD1                      1'b0                    SWR_AUDIO_REG_MASK
    4       R/W SWR_AUDIO_FORCE_CODE_ZCD1                       1'b0                    SWR_AUDIO_REG_MASK
    5       R/W SWR_AUDIO_SEL_ZCD2_RSTNEW_B                     1'b0                    SWR_AUDIO_REG_MASK
    15:6    R/W SWR_AUDIO_TUNE_ZCD2_PWMFORCECODE                10'b0100000000          SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG12X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_SEL_POS_VREFPFM1: 1;
        uint16_t SWR_AUDIO_EN_HV17_LOIQ: 1;
        uint16_t SWR_AUDIO_EN_HVD17_POWDN_CURRENT: 1;
        uint16_t SWR_AUDIO_STICKY_CODE_ZCD1: 1;
        uint16_t SWR_AUDIO_FORCE_CODE_ZCD1: 1;
        uint16_t SWR_AUDIO_SEL_ZCD2_RSTNEW_B: 1;
        uint16_t SWR_AUDIO_TUNE_ZCD2_PWMFORCECODE: 10;
    };
} AON_FAST_REG12X_SWR_AUDIO_TYPE;

/* 0x11DA
    3:0     R/W SWR_AUDIO_REG13X_DUMMY1                         4'b0000                 SWR_AUDIO_REG_MASK
    4       R/W SWR_AUDIO_EN_PLUS_CODE_B                        1'b0                    SWR_AUDIO_REG_MASK
    5       R/W SWR_AUDIO_EN_PORT_SS                            1'b0                    SWR_AUDIO_REG_MASK
    6       R/W SWR_AUDIO_EN_SCP_DIG                            1'b0                    SWR_AUDIO_REG_MASK
    7       R/W SWR_AUDIO_EN_OCP_DIG                            1'b0                    SWR_AUDIO_REG_MASK
    8       R/W SWR_AUDIO_SEL_DR                                1'b0                    SWR_AUDIO_REG_MASK
    9       R   RO_AON_SWR_AUDIO_EN_VO_PD                       1'b0
    12:10   R/W SWR_AUDIO_SEL_PDRV                              3'b111                  SWR_AUDIO_REG_MASK
    15:13   R/W SWR_AUDIO_SEL_NDRV                              3'b111                  SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG13X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_REG13X_DUMMY1: 4;
        uint16_t SWR_AUDIO_EN_PLUS_CODE_B: 1;
        uint16_t SWR_AUDIO_EN_PORT_SS: 1;
        uint16_t SWR_AUDIO_EN_SCP_DIG: 1;
        uint16_t SWR_AUDIO_EN_OCP_DIG: 1;
        uint16_t SWR_AUDIO_SEL_DR: 1;
        uint16_t RO_AON_SWR_AUDIO_EN_VO_PD: 1;
        uint16_t SWR_AUDIO_SEL_PDRV: 3;
        uint16_t SWR_AUDIO_SEL_NDRV: 3;
    };
} AON_FAST_REG13X_SWR_AUDIO_TYPE;

/* 0x11DC
    15:0    R/W SWR_AUDIO_REG14X_DUMMY1                         16'b0000000000000000    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG14X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_REG14X_DUMMY1;
    };
} AON_FAST_REG14X_SWR_AUDIO_TYPE;

/* 0x11DE
    15:0    R/W SWR_AUDIO_REG15X_DUMMY1                         16'b0000000000000000    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG15X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_REG15X_DUMMY1;
    };
} AON_FAST_REG15X_SWR_AUDIO_TYPE;

/* 0x11E0
    15:0    R/W SWR_AUDIO_REG16X_DUMMY1                         16'b0000000000000000    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG16X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_REG16X_DUMMY1;
    };
} AON_FAST_REG16X_SWR_AUDIO_TYPE;

/* 0x11E2
    15:0    R/W SWR_AUDIO_REG17X_DUMMY1                         16'b0000000000000000    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG17X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_REG17X_DUMMY1;
    };
} AON_FAST_REG17X_SWR_AUDIO_TYPE;

/* 0x11E4
    15:0    R/W SWR_AUDIO_REG18X_DUMMY1                         16'b0000000000000000    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG18X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_REG18X_DUMMY1;
    };
} AON_FAST_REG18X_SWR_AUDIO_TYPE;

/* 0x11E6
    0       R   SWR_AUDIO_FLAG0X_DUMMY2                         1'b0
    1       R   SWR_AUDIO_FLAG0X_DUMMY1                         1'b0
    2       R   SWR_AUDIO_ENLVS_O                               1'b0
    3       R   SWR_AUDIO_ENHV17_O                              1'b0
    4       R   SWR_AUDIO_OCP_FLAG                              1'b0
    5       R   SWR_AUDIO_VREFSS_START                          1'b0
    6       R   SWR_AUDIO_VREFSS_OVER                           1'b0
    7       R   SWR_AUDIO_SOFTSTART_OVER                        1'b0
    8       R   SWR_AUDIO_HVD17POR                              1'b0
    9       R   SWR_AUDIO_LDO17_PORB                            1'b0
    10      R   SWR_AUDIO_LDO17_PORA                            1'b0
    15:11   R   SWR_AUDIO_D                                     5'h0
 */
typedef union _AON_FAST_FLAG0X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_FLAG0X_DUMMY2: 1;
        uint16_t SWR_AUDIO_FLAG0X_DUMMY1: 1;
        uint16_t SWR_AUDIO_ENLVS_O: 1;
        uint16_t SWR_AUDIO_ENHV17_O: 1;
        uint16_t SWR_AUDIO_OCP_FLAG: 1;
        uint16_t SWR_AUDIO_VREFSS_START: 1;
        uint16_t SWR_AUDIO_VREFSS_OVER: 1;
        uint16_t SWR_AUDIO_SOFTSTART_OVER: 1;
        uint16_t SWR_AUDIO_HVD17POR: 1;
        uint16_t SWR_AUDIO_LDO17_PORB: 1;
        uint16_t SWR_AUDIO_LDO17_PORA: 1;
        uint16_t SWR_AUDIO_D: 5;
    };
} AON_FAST_FLAG0X_SWR_AUDIO_TYPE;

/* 0x11E8
    15:0    R   SWR_AUDIO_FLAG1X_DUMMY1                         16'h0
 */
typedef union _AON_FAST_FLAG1X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_FLAG1X_DUMMY1;
    };
} AON_FAST_FLAG1X_SWR_AUDIO_TYPE;

/* 0x11EA
    15:0    R   SWR_AUDIO_FLAG2X_DUMMY1                         16'h0
 */
typedef union _AON_FAST_FLAG2X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_FLAG2X_DUMMY1;
    };
} AON_FAST_FLAG2X_SWR_AUDIO_TYPE;

/* 0x11EC
    15:0    R   SWR_AUDIO_FLAG3X_DUMMY1                         16'h0
 */
typedef union _AON_FAST_FLAG3X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_FLAG3X_DUMMY1;
    };
} AON_FAST_FLAG3X_SWR_AUDIO_TYPE;

/* 0x11EE
    15:0    R   SWR_AUDIO_C_KOUT0X_DUMMY1                       16'h0
 */
typedef union _AON_FAST_C_KOUT0X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_C_KOUT0X_DUMMY1;
    };
} AON_FAST_C_KOUT0X_SWR_AUDIO_TYPE;

/* 0x11F0
    0       R   SWR_AUDIO_C_KOUT1X_DUMMY2                       1'b0
    2:1     R   SWR_AUDIO_S                                     2'h0
    15:3    R   SWR_AUDIO_C_KOUT1X_DUMMY1                       12'h0
 */
typedef union _AON_FAST_C_KOUT1X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_C_KOUT1X_DUMMY2: 1;
        uint16_t SWR_AUDIO_S: 2;
        uint16_t SWR_AUDIO_C_KOUT1X_DUMMY1: 13;
    };
} AON_FAST_C_KOUT1X_SWR_AUDIO_TYPE;

/* 0x11F2
    0       R   SWR_AUDIO_X_CLK1                                1'b0
    1       R   SWR_AUDIO_X_RAMP1ON                             1'b0
    2       R   SWR_AUDIO_PWM_CTRL1                             1'b0
    3       R   SWR_AUDIO_PFM_CTRL1                             1'b0
    4       R   SWR_AUDIO_NI                                    1'b0
    5       R   SWR_AUDIO_PI                                    1'b0
    6       R   SWR_AUDIO_UD_2                                  1'b0
    7       R   SWR_AUDIO_POWZCD_2                              1'b0
    8       R   SWR_AUDIO_ENZCD_2                               1'b0
    9       R   SWR_AUDIO_OCP                                   1'b0
    10      R   SWR_AUDIO_LX_DET                                1'b0
    11      R   SWR_AUDIO_ZCD_SET                               1'b0
    12      R   SWR_AUDIO_ZCD                                   1'b0
    13      R   SWR_AUDIO_SAMPLE_OVER                           1'b0
    14      R   SWR_AUDIO_UPDATE_CK2                            1'b0
    15      R   SWR_AUDIO_UPDATE_CK                             1'b0
 */
typedef union _AON_FAST_C_KOUT2X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_X_CLK1: 1;
        uint16_t SWR_AUDIO_X_RAMP1ON: 1;
        uint16_t SWR_AUDIO_PWM_CTRL1: 1;
        uint16_t SWR_AUDIO_PFM_CTRL1: 1;
        uint16_t SWR_AUDIO_NI: 1;
        uint16_t SWR_AUDIO_PI: 1;
        uint16_t SWR_AUDIO_UD_2: 1;
        uint16_t SWR_AUDIO_POWZCD_2: 1;
        uint16_t SWR_AUDIO_ENZCD_2: 1;
        uint16_t SWR_AUDIO_OCP: 1;
        uint16_t SWR_AUDIO_LX_DET: 1;
        uint16_t SWR_AUDIO_ZCD_SET: 1;
        uint16_t SWR_AUDIO_ZCD: 1;
        uint16_t SWR_AUDIO_SAMPLE_OVER: 1;
        uint16_t SWR_AUDIO_UPDATE_CK2: 1;
        uint16_t SWR_AUDIO_UPDATE_CK: 1;
    };
} AON_FAST_C_KOUT2X_SWR_AUDIO_TYPE;

/* 0x11F4
    0       R   SWR_AUDIO_UPDATE_CK1                            1'b0
    1       R   SWR_AUDIO_UD_H                                  1'b0
    9:2     R   SWR_AUDIO_ZCDQ1                                 8'h0
    10      R   SWR_AUDIO_EN_UPDATE                             1'b0
    11      R   SWR_AUDIO_OUT1_OCCUPY                           1'b0
    12      R   SWR_AUDIO_ONTIME_OVER1                          1'b0
    13      R   SWR_AUDIO_VCOMPPFM1                             1'b0
    14      R   SWR_AUDIO_VCOMP1                                1'b0
    15      R   SWR_AUDIO_CK_CTRL                               1'b0
 */
typedef union _AON_FAST_C_KOUT3X_SWR_AUDIO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_UPDATE_CK1: 1;
        uint16_t SWR_AUDIO_UD_H: 1;
        uint16_t SWR_AUDIO_ZCDQ1: 8;
        uint16_t SWR_AUDIO_EN_UPDATE: 1;
        uint16_t SWR_AUDIO_OUT1_OCCUPY: 1;
        uint16_t SWR_AUDIO_ONTIME_OVER1: 1;
        uint16_t SWR_AUDIO_VCOMPPFM1: 1;
        uint16_t SWR_AUDIO_VCOMP1: 1;
        uint16_t SWR_AUDIO_CK_CTRL: 1;
    };
} AON_FAST_C_KOUT3X_SWR_AUDIO_TYPE;

/* 0x11F6
    0       R/W SWR_AUDIO_PFMCCMPOS_INT_RST_B                   1'b0                    SWR_AUDIO_REG_MASK
    1       R   RO_AON_SWR_AUDIO_PFMCCMPOS_EXT_RST_B            1'b0
    2       R/W SWR_AUDIO_PFMCCMPOS_SEL_EXT_RST_B               1'b0                    SWR_AUDIO_REG_MASK
    3       R   RO_AON_SWR_AUDIO_PFMCCMPOS_EN_POS               1'b0
    6:4     R/W SWR_AUDIO_PFMCCMPOS_DIV_CLK                     3'b011                  SWR_AUDIO_REG_MASK
    7       R/W SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY7                1'b0                    SWR_AUDIO_REG_MASK
    8       R/W SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY8                1'b0                    SWR_AUDIO_REG_MASK
    9       R/W SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY9                1'b0                    SWR_AUDIO_REG_MASK
    10      R/W SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY10               1'b0                    SWR_AUDIO_REG_MASK
    11      R/W SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY11               1'b0                    SWR_AUDIO_REG_MASK
    12      R/W SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY12               1'b0                    SWR_AUDIO_REG_MASK
    13      R/W SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY13               1'b0                    SWR_AUDIO_REG_MASK
    14      R/W SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY14               1'b0                    SWR_AUDIO_REG_MASK
    15      R/W SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY15               1'b0                    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG0X_SWR_AUDIO_PFMCCMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMCCMPOS_INT_RST_B: 1;
        uint16_t RO_AON_SWR_AUDIO_PFMCCMPOS_EXT_RST_B: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_SEL_EXT_RST_B: 1;
        uint16_t RO_AON_SWR_AUDIO_PFMCCMPOS_EN_POS: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_DIV_CLK: 3;
        uint16_t SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY7: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY8: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY9: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY10: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY11: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY12: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY13: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY14: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_SWR_AUDIO_PFMCCMPOS_TYPE;

/* 0x11F8
    3:0     R/W SWR_AUDIO_PFMCCMPOS_POF_STEP                    4'b0110                 SWR_AUDIO_REG_MASK
    7:4     R/W SWR_AUDIO_PFMCCMPOS_PON_STEP                    4'b0110                 SWR_AUDIO_REG_MASK
    10:8    R/W SWR_AUDIO_PFMCCMPOS_POF_WAIT                    3'b000                  SWR_AUDIO_REG_MASK
    13:11   R/W SWR_AUDIO_PFMCCMPOS_PON_WAIT                    3'b000                  SWR_AUDIO_REG_MASK
    14      R/W SWR_AUDIO_PFMCCMPOS_REG1X_DUMMY14               1'b0                    SWR_AUDIO_REG_MASK
    15      R/W SWR_AUDIO_PFMCCMPOS_REG1X_DUMMY15               1'b0                    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG1X_SWR_AUDIO_PFMCCMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMCCMPOS_POF_STEP: 4;
        uint16_t SWR_AUDIO_PFMCCMPOS_PON_STEP: 4;
        uint16_t SWR_AUDIO_PFMCCMPOS_POF_WAIT: 3;
        uint16_t SWR_AUDIO_PFMCCMPOS_PON_WAIT: 3;
        uint16_t SWR_AUDIO_PFMCCMPOS_REG1X_DUMMY14: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_REG1X_DUMMY15: 1;
    };
} AON_FAST_REG1X_SWR_AUDIO_PFMCCMPOS_TYPE;

/* 0x11FA
    7:0     R/W SWR_AUDIO_PFMCCMPOS_PON_OVER                    8'b10110000             SWR_AUDIO_REG_MASK
    15:8    R/W SWR_AUDIO_PFMCCMPOS_PON_START                   8'b00000000             SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG2X_SWR_AUDIO_PFMCCMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMCCMPOS_PON_OVER: 8;
        uint16_t SWR_AUDIO_PFMCCMPOS_PON_START: 8;
    };
} AON_FAST_REG2X_SWR_AUDIO_PFMCCMPOS_TYPE;

/* 0x11FC
    7:0     R/W SWR_AUDIO_PFMCCMPOS_POF_OVER                    8'b00000000             SWR_AUDIO_REG_MASK
    15:8    R/W SWR_AUDIO_PFMCCMPOS_POF_START                   8'b10110000             SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG3X_SWR_AUDIO_PFMCCMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMCCMPOS_POF_OVER: 8;
        uint16_t SWR_AUDIO_PFMCCMPOS_POF_START: 8;
    };
} AON_FAST_REG3X_SWR_AUDIO_PFMCCMPOS_TYPE;

/* 0x11FE
    7:0     R/W SWR_AUDIO_PFMCCMPOS_SET_PON_FLAG2               8'b10010000             SWR_AUDIO_REG_MASK
    15:8    R/W SWR_AUDIO_PFMCCMPOS_SET_PON_FLAG1               8'b00000000             SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG4X_SWR_AUDIO_PFMCCMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMCCMPOS_SET_PON_FLAG2: 8;
        uint16_t SWR_AUDIO_PFMCCMPOS_SET_PON_FLAG1: 8;
    };
} AON_FAST_REG4X_SWR_AUDIO_PFMCCMPOS_TYPE;

/* 0x1200
    7:0     R/W SWR_AUDIO_PFMCCMPOS_SET_PON_FLAG4               8'b10110000             SWR_AUDIO_REG_MASK
    15:8    R/W SWR_AUDIO_PFMCCMPOS_SET_PON_FLAG3               8'b10110000             SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG5X_SWR_AUDIO_PFMCCMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMCCMPOS_SET_PON_FLAG4: 8;
        uint16_t SWR_AUDIO_PFMCCMPOS_SET_PON_FLAG3: 8;
    };
} AON_FAST_REG5X_SWR_AUDIO_PFMCCMPOS_TYPE;

/* 0x1202
    7:0     R/W SWR_AUDIO_PFMCCMPOS_SET_POF_FLAG2               8'b10010000             SWR_AUDIO_REG_MASK
    15:8    R/W SWR_AUDIO_PFMCCMPOS_SET_POF_FLAG1               8'b10110000             SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG6X_SWR_AUDIO_PFMCCMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMCCMPOS_SET_POF_FLAG2: 8;
        uint16_t SWR_AUDIO_PFMCCMPOS_SET_POF_FLAG1: 8;
    };
} AON_FAST_REG6X_SWR_AUDIO_PFMCCMPOS_TYPE;

/* 0x1204
    7:0     R/W SWR_AUDIO_PFMCCMPOS_SET_POF_FLAG4               8'b00000000             SWR_AUDIO_REG_MASK
    15:8    R/W SWR_AUDIO_PFMCCMPOS_SET_POF_FLAG3               8'b00000000             SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG7X_SWR_AUDIO_PFMCCMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMCCMPOS_SET_POF_FLAG4: 8;
        uint16_t SWR_AUDIO_PFMCCMPOS_SET_POF_FLAG3: 8;
    };
} AON_FAST_REG7X_SWR_AUDIO_PFMCCMPOS_TYPE;

/* 0x1206
    0       R   SWR_AUDIO_PFMCCMPOS_POF_FLAG4                   1'b0
    1       R   SWR_AUDIO_PFMCCMPOS_POF_FLAG3                   1'b0
    2       R   SWR_AUDIO_PFMCCMPOS_POF_FLAG2                   1'b0
    3       R   SWR_AUDIO_PFMCCMPOS_POF_FLAG1                   1'b0
    4       R   SWR_AUDIO_PFMCCMPOS_PON_FLAG4                   1'b0
    5       R   SWR_AUDIO_PFMCCMPOS_PON_FLAG3                   1'b0
    6       R   SWR_AUDIO_PFMCCMPOS_PON_FLAG2                   1'b0
    7       R   SWR_AUDIO_PFMCCMPOS_PON_FLAG1                   1'b0
    10:8    R   SWR_AUDIO_PFMCCMPOS_FSM_CS                      3'h0
    11      R   SWR_AUDIO_PFMCCMPOS_C_KOUT0X_DUMMY11            1'b0
    12      R   SWR_AUDIO_PFMCCMPOS_C_KOUT0X_DUMMY12            1'b0
    13      R   SWR_AUDIO_PFMCCMPOS_C_KOUT0X_DUMMY13            1'b0
    14      R   SWR_AUDIO_PFMCCMPOS_C_KOUT0X_DUMMY14            1'b0
    15      R   SWR_AUDIO_PFMCCMPOS_C_KOUT0X_DUMMY15            1'b0
 */
typedef union _AON_FAST_C_KOUT0X_SWR_AUDIO_PFMCCMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMCCMPOS_POF_FLAG4: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_POF_FLAG3: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_POF_FLAG2: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_POF_FLAG1: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_PON_FLAG4: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_PON_FLAG3: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_PON_FLAG2: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_PON_FLAG1: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_FSM_CS: 3;
        uint16_t SWR_AUDIO_PFMCCMPOS_C_KOUT0X_DUMMY11: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_C_KOUT0X_DUMMY12: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_C_KOUT0X_DUMMY13: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_C_KOUT0X_DUMMY14: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_C_KOUT0X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT0X_SWR_AUDIO_PFMCCMPOS_TYPE;

/* 0x1208
    7:0     R   SWR_AUDIO_PFMCCMPOS_DR                          8'h0
    8       R   SWR_AUDIO_PFMCCMPOS_C_KOUT1X_DUMMY8             1'h0
    9       R   SWR_AUDIO_PFMCCMPOS_C_KOUT1X_DUMMY9             1'h0
    10      R   SWR_AUDIO_PFMCCMPOS_C_KOUT1X_DUMMY10            1'h0
    11      R   SWR_AUDIO_PFMCCMPOS_C_KOUT1X_DUMMY11            1'h0
    12      R   SWR_AUDIO_PFMCCMPOS_C_KOUT1X_DUMMY12            1'h0
    13      R   SWR_AUDIO_PFMCCMPOS_C_KOUT1X_DUMMY13            1'h0
    14      R   SWR_AUDIO_PFMCCMPOS_C_KOUT1X_DUMMY14            1'h0
    15      R   SWR_AUDIO_PFMCCMPOS_C_KOUT1X_DUMMY15            1'h0
 */
typedef union _AON_FAST_C_KOUT1X_SWR_AUDIO_PFMCCMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMCCMPOS_DR: 8;
        uint16_t SWR_AUDIO_PFMCCMPOS_C_KOUT1X_DUMMY8: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_C_KOUT1X_DUMMY9: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_C_KOUT1X_DUMMY10: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_C_KOUT1X_DUMMY11: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_C_KOUT1X_DUMMY12: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_C_KOUT1X_DUMMY13: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_C_KOUT1X_DUMMY14: 1;
        uint16_t SWR_AUDIO_PFMCCMPOS_C_KOUT1X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT1X_SWR_AUDIO_PFMCCMPOS_TYPE;

/* 0x120A
    0       R/W SWR_AUDIO_PFMPOS_INT_RST_B                      1'b0                    SWR_AUDIO_REG_MASK
    1       R   RO_AON_SWR_AUDIO_PFMPOS_EXT_RST_B               1'b0
    2       R/W SWR_AUDIO_PFMPOS_SEL_EXT_RST_B                  1'b0                    SWR_AUDIO_REG_MASK
    3       R   RO_AON_SWR_AUDIO_PFMPOS_EN_POS                  1'b0
    6:4     R/W SWR_AUDIO_PFMPOS_DIV_CLK                        3'b011                  SWR_AUDIO_REG_MASK
    7       R/W SWR_AUDIO_PFMPOS_REG0X_DUMMY7                   1'b0                    SWR_AUDIO_REG_MASK
    8       R/W SWR_AUDIO_PFMPOS_REG0X_DUMMY8                   1'b0                    SWR_AUDIO_REG_MASK
    9       R/W SWR_AUDIO_PFMPOS_REG0X_DUMMY9                   1'b0                    SWR_AUDIO_REG_MASK
    10      R/W SWR_AUDIO_PFMPOS_REG0X_DUMMY10                  1'b0                    SWR_AUDIO_REG_MASK
    11      R/W SWR_AUDIO_PFMPOS_REG0X_DUMMY11                  1'b0                    SWR_AUDIO_REG_MASK
    12      R/W SWR_AUDIO_PFMPOS_REG0X_DUMMY12                  1'b0                    SWR_AUDIO_REG_MASK
    13      R/W SWR_AUDIO_PFMPOS_REG0X_DUMMY13                  1'b0                    SWR_AUDIO_REG_MASK
    14      R/W SWR_AUDIO_PFMPOS_REG0X_DUMMY14                  1'b0                    SWR_AUDIO_REG_MASK
    15      R/W SWR_AUDIO_PFMPOS_REG0X_DUMMY15                  1'b0                    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG0X_SWR_AUDIO_PFMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMPOS_INT_RST_B: 1;
        uint16_t RO_AON_SWR_AUDIO_PFMPOS_EXT_RST_B: 1;
        uint16_t SWR_AUDIO_PFMPOS_SEL_EXT_RST_B: 1;
        uint16_t RO_AON_SWR_AUDIO_PFMPOS_EN_POS: 1;
        uint16_t SWR_AUDIO_PFMPOS_DIV_CLK: 3;
        uint16_t SWR_AUDIO_PFMPOS_REG0X_DUMMY7: 1;
        uint16_t SWR_AUDIO_PFMPOS_REG0X_DUMMY8: 1;
        uint16_t SWR_AUDIO_PFMPOS_REG0X_DUMMY9: 1;
        uint16_t SWR_AUDIO_PFMPOS_REG0X_DUMMY10: 1;
        uint16_t SWR_AUDIO_PFMPOS_REG0X_DUMMY11: 1;
        uint16_t SWR_AUDIO_PFMPOS_REG0X_DUMMY12: 1;
        uint16_t SWR_AUDIO_PFMPOS_REG0X_DUMMY13: 1;
        uint16_t SWR_AUDIO_PFMPOS_REG0X_DUMMY14: 1;
        uint16_t SWR_AUDIO_PFMPOS_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_SWR_AUDIO_PFMPOS_TYPE;

/* 0x120C
    3:0     R/W SWR_AUDIO_PFMPOS_POF_STEP                       4'b0110                 SWR_AUDIO_REG_MASK
    7:4     R/W SWR_AUDIO_PFMPOS_PON_STEP                       4'b0110                 SWR_AUDIO_REG_MASK
    10:8    R/W SWR_AUDIO_PFMPOS_POF_WAIT                       3'b000                  SWR_AUDIO_REG_MASK
    13:11   R/W SWR_AUDIO_PFMPOS_PON_WAIT                       3'b000                  SWR_AUDIO_REG_MASK
    14      R/W SWR_AUDIO_PFMPOS_REG1X_DUMMY14                  1'b0                    SWR_AUDIO_REG_MASK
    15      R/W SWR_AUDIO_PFMPOS_REG1X_DUMMY15                  1'b0                    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG1X_SWR_AUDIO_PFMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMPOS_POF_STEP: 4;
        uint16_t SWR_AUDIO_PFMPOS_PON_STEP: 4;
        uint16_t SWR_AUDIO_PFMPOS_POF_WAIT: 3;
        uint16_t SWR_AUDIO_PFMPOS_PON_WAIT: 3;
        uint16_t SWR_AUDIO_PFMPOS_REG1X_DUMMY14: 1;
        uint16_t SWR_AUDIO_PFMPOS_REG1X_DUMMY15: 1;
    };
} AON_FAST_REG1X_SWR_AUDIO_PFMPOS_TYPE;

/* 0x120E
    7:0     R/W SWR_AUDIO_PFMPOS_PON_OVER                       8'b10110000             SWR_AUDIO_REG_MASK
    15:8    R/W SWR_AUDIO_PFMPOS_PON_START                      8'b00000000             SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG2X_SWR_AUDIO_PFMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMPOS_PON_OVER: 8;
        uint16_t SWR_AUDIO_PFMPOS_PON_START: 8;
    };
} AON_FAST_REG2X_SWR_AUDIO_PFMPOS_TYPE;

/* 0x1210
    7:0     R/W SWR_AUDIO_PFMPOS_POF_OVER                       8'b00000000             SWR_AUDIO_REG_MASK
    15:8    R/W SWR_AUDIO_PFMPOS_POF_START                      8'b10110000             SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG3X_SWR_AUDIO_PFMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMPOS_POF_OVER: 8;
        uint16_t SWR_AUDIO_PFMPOS_POF_START: 8;
    };
} AON_FAST_REG3X_SWR_AUDIO_PFMPOS_TYPE;

/* 0x1212
    7:0     R/W SWR_AUDIO_PFMPOS_SET_PON_FLAG2                  8'b10010000             SWR_AUDIO_REG_MASK
    15:8    R/W SWR_AUDIO_PFMPOS_SET_PON_FLAG1                  8'b00000000             SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG4X_SWR_AUDIO_PFMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMPOS_SET_PON_FLAG2: 8;
        uint16_t SWR_AUDIO_PFMPOS_SET_PON_FLAG1: 8;
    };
} AON_FAST_REG4X_SWR_AUDIO_PFMPOS_TYPE;

/* 0x1214
    7:0     R/W SWR_AUDIO_PFMPOS_SET_PON_FLAG4                  8'b10110000             SWR_AUDIO_REG_MASK
    15:8    R/W SWR_AUDIO_PFMPOS_SET_PON_FLAG3                  8'b10110000             SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG5X_SWR_AUDIO_PFMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMPOS_SET_PON_FLAG4: 8;
        uint16_t SWR_AUDIO_PFMPOS_SET_PON_FLAG3: 8;
    };
} AON_FAST_REG5X_SWR_AUDIO_PFMPOS_TYPE;

/* 0x1216
    7:0     R/W SWR_AUDIO_PFMPOS_SET_POF_FLAG2                  8'b10010000             SWR_AUDIO_REG_MASK
    15:8    R/W SWR_AUDIO_PFMPOS_SET_POF_FLAG1                  8'b10110000             SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG6X_SWR_AUDIO_PFMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMPOS_SET_POF_FLAG2: 8;
        uint16_t SWR_AUDIO_PFMPOS_SET_POF_FLAG1: 8;
    };
} AON_FAST_REG6X_SWR_AUDIO_PFMPOS_TYPE;

/* 0x1218
    7:0     R/W SWR_AUDIO_PFMPOS_SET_POF_FLAG4                  8'b00000000             SWR_AUDIO_REG_MASK
    15:8    R/W SWR_AUDIO_PFMPOS_SET_POF_FLAG3                  8'b00000000             SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG7X_SWR_AUDIO_PFMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMPOS_SET_POF_FLAG4: 8;
        uint16_t SWR_AUDIO_PFMPOS_SET_POF_FLAG3: 8;
    };
} AON_FAST_REG7X_SWR_AUDIO_PFMPOS_TYPE;

/* 0x121A
    0       R   SWR_AUDIO_PFMPOS_POF_FLAG4                      1'b0
    1       R   SWR_AUDIO_PFMPOS_POF_FLAG3                      1'b0
    2       R   SWR_AUDIO_PFMPOS_POF_FLAG2                      1'b0
    3       R   SWR_AUDIO_PFMPOS_POF_FLAG1                      1'b0
    4       R   SWR_AUDIO_PFMPOS_PON_FLAG4                      1'b0
    5       R   SWR_AUDIO_PFMPOS_PON_FLAG3                      1'b0
    6       R   SWR_AUDIO_PFMPOS_PON_FLAG2                      1'b0
    7       R   SWR_AUDIO_PFMPOS_PON_FLAG1                      1'b0
    10:8    R   SWR_AUDIO_PFMPOS_FSM_CS                         3'h0
    11      R   SWR_AUDIO_PFMPOS_C_KOUT0X_DUMMY11               1'b0
    12      R   SWR_AUDIO_PFMPOS_C_KOUT0X_DUMMY12               1'b0
    13      R   SWR_AUDIO_PFMPOS_C_KOUT0X_DUMMY13               1'b0
    14      R   SWR_AUDIO_PFMPOS_C_KOUT0X_DUMMY14               1'b0
    15      R   SWR_AUDIO_PFMPOS_C_KOUT0X_DUMMY15               1'b0
 */
typedef union _AON_FAST_C_KOUT0X_SWR_AUDIO_PFMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMPOS_POF_FLAG4: 1;
        uint16_t SWR_AUDIO_PFMPOS_POF_FLAG3: 1;
        uint16_t SWR_AUDIO_PFMPOS_POF_FLAG2: 1;
        uint16_t SWR_AUDIO_PFMPOS_POF_FLAG1: 1;
        uint16_t SWR_AUDIO_PFMPOS_PON_FLAG4: 1;
        uint16_t SWR_AUDIO_PFMPOS_PON_FLAG3: 1;
        uint16_t SWR_AUDIO_PFMPOS_PON_FLAG2: 1;
        uint16_t SWR_AUDIO_PFMPOS_PON_FLAG1: 1;
        uint16_t SWR_AUDIO_PFMPOS_FSM_CS: 3;
        uint16_t SWR_AUDIO_PFMPOS_C_KOUT0X_DUMMY11: 1;
        uint16_t SWR_AUDIO_PFMPOS_C_KOUT0X_DUMMY12: 1;
        uint16_t SWR_AUDIO_PFMPOS_C_KOUT0X_DUMMY13: 1;
        uint16_t SWR_AUDIO_PFMPOS_C_KOUT0X_DUMMY14: 1;
        uint16_t SWR_AUDIO_PFMPOS_C_KOUT0X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT0X_SWR_AUDIO_PFMPOS_TYPE;

/* 0x121C
    7:0     R   SWR_AUDIO_PFMPOS_DR                             8'h0
    8       R   SWR_AUDIO_PFMPOS_C_KOUT1X_DUMMY8                1'h0
    9       R   SWR_AUDIO_PFMPOS_C_KOUT1X_DUMMY9                1'h0
    10      R   SWR_AUDIO_PFMPOS_C_KOUT1X_DUMMY10               1'h0
    11      R   SWR_AUDIO_PFMPOS_C_KOUT1X_DUMMY11               1'h0
    12      R   SWR_AUDIO_PFMPOS_C_KOUT1X_DUMMY12               1'h0
    13      R   SWR_AUDIO_PFMPOS_C_KOUT1X_DUMMY13               1'h0
    14      R   SWR_AUDIO_PFMPOS_C_KOUT1X_DUMMY14               1'h0
    15      R   SWR_AUDIO_PFMPOS_C_KOUT1X_DUMMY15               1'h0
 */
typedef union _AON_FAST_C_KOUT1X_SWR_AUDIO_PFMPOS_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFMPOS_DR: 8;
        uint16_t SWR_AUDIO_PFMPOS_C_KOUT1X_DUMMY8: 1;
        uint16_t SWR_AUDIO_PFMPOS_C_KOUT1X_DUMMY9: 1;
        uint16_t SWR_AUDIO_PFMPOS_C_KOUT1X_DUMMY10: 1;
        uint16_t SWR_AUDIO_PFMPOS_C_KOUT1X_DUMMY11: 1;
        uint16_t SWR_AUDIO_PFMPOS_C_KOUT1X_DUMMY12: 1;
        uint16_t SWR_AUDIO_PFMPOS_C_KOUT1X_DUMMY13: 1;
        uint16_t SWR_AUDIO_PFMPOS_C_KOUT1X_DUMMY14: 1;
        uint16_t SWR_AUDIO_PFMPOS_C_KOUT1X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT1X_SWR_AUDIO_PFMPOS_TYPE;

/* 0x121E
    0       R/W SWR_AUDIO_PFM300to600K_RST_B                    1'b0                    SWR_AUDIO_REG_MASK
    1       R/W SWR_AUDIO_PFM300to600K_EN_32K_AUDIO             1'b0                    SWR_AUDIO_REG_MASK
    3:2     R/W SWR_AUDIO_PFM300to600K_TUNE_32K_SAMPLE_CYC      2'b00                   SWR_AUDIO_REG_MASK
    8:4     R/W SWR_AUDIO_PFM300to600K_TUNE_CCOT_INIT           5'b00011                SWR_AUDIO_REG_MASK
    9       R/W SWR_AUDIO_PFM300to600K_REG0X_DUMMY9             1'b0                    SWR_AUDIO_REG_MASK
    10      R/W SWR_AUDIO_PFM300to600K_REG0X_DUMMY10            1'b1                    SWR_AUDIO_REG_MASK
    11      R/W SWR_AUDIO_PFM300to600K_REG0X_DUMMY11            1'b0                    SWR_AUDIO_REG_MASK
    12      R/W SWR_AUDIO_PFM300to600K_REG0X_DUMMY12            1'b1                    SWR_AUDIO_REG_MASK
    13      R/W SWR_AUDIO_PFM300to600K_REG0X_DUMMY13            1'b1                    SWR_AUDIO_REG_MASK
    14      R/W SWR_AUDIO_PFM300to600K_REG0X_DUMMY14            1'b0                    SWR_AUDIO_REG_MASK
    15      R/W SWR_AUDIO_PFM300to600K_REG0X_DUMMY15            1'b0                    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG0X_SWR_AUDIO_PFM300to600K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFM300to600K_RST_B: 1;
        uint16_t SWR_AUDIO_PFM300to600K_EN_32K_AUDIO: 1;
        uint16_t SWR_AUDIO_PFM300to600K_TUNE_32K_SAMPLE_CYC: 2;
        uint16_t SWR_AUDIO_PFM300to600K_TUNE_CCOT_INIT: 5;
        uint16_t SWR_AUDIO_PFM300to600K_REG0X_DUMMY9: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG0X_DUMMY10: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG0X_DUMMY11: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG0X_DUMMY12: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG0X_DUMMY13: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG0X_DUMMY14: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_SWR_AUDIO_PFM300to600K_TYPE;

/* 0x1220
    9:0     R/W SWR_AUDIO_PFM300to600K_TUNE_PFM_LOWER_BND       10'b0000001010          SWR_AUDIO_REG_MASK
    10      R/W SWR_AUDIO_PFM300to600K_REG1X_DUMMY10            1'b0                    SWR_AUDIO_REG_MASK
    11      R/W SWR_AUDIO_PFM300to600K_REG1X_DUMMY11            1'b0                    SWR_AUDIO_REG_MASK
    12      R/W SWR_AUDIO_PFM300to600K_REG1X_DUMMY12            1'b1                    SWR_AUDIO_REG_MASK
    13      R/W SWR_AUDIO_PFM300to600K_REG1X_DUMMY13            1'b1                    SWR_AUDIO_REG_MASK
    14      R/W SWR_AUDIO_PFM300to600K_REG1X_DUMMY14            1'b0                    SWR_AUDIO_REG_MASK
    15      R/W SWR_AUDIO_PFM300to600K_REG1X_DUMMY15            1'b0                    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG1X_SWR_AUDIO_PFM300to600K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFM300to600K_TUNE_PFM_LOWER_BND: 10;
        uint16_t SWR_AUDIO_PFM300to600K_REG1X_DUMMY10: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG1X_DUMMY11: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG1X_DUMMY12: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG1X_DUMMY13: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG1X_DUMMY14: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG1X_DUMMY15: 1;
    };
} AON_FAST_REG1X_SWR_AUDIO_PFM300to600K_TYPE;

/* 0x1222
    9:0     R/W SWR_AUDIO_PFM300to600K_TUNE_PFM_UPPER_BND       10'b0000010011          SWR_AUDIO_REG_MASK
    10      R/W SWR_AUDIO_PFM300to600K_REG2X_DUMMY10            1'b0                    SWR_AUDIO_REG_MASK
    11      R/W SWR_AUDIO_PFM300to600K_REG2X_DUMMY11            1'b0                    SWR_AUDIO_REG_MASK
    12      R/W SWR_AUDIO_PFM300to600K_REG2X_DUMMY12            1'b1                    SWR_AUDIO_REG_MASK
    13      R/W SWR_AUDIO_PFM300to600K_REG2X_DUMMY13            1'b1                    SWR_AUDIO_REG_MASK
    14      R/W SWR_AUDIO_PFM300to600K_REG2X_DUMMY14            1'b0                    SWR_AUDIO_REG_MASK
    15      R/W SWR_AUDIO_PFM300to600K_REG2X_DUMMY15            1'b0                    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG2X_SWR_AUDIO_PFM300to600K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFM300to600K_TUNE_PFM_UPPER_BND: 10;
        uint16_t SWR_AUDIO_PFM300to600K_REG2X_DUMMY10: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG2X_DUMMY11: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG2X_DUMMY12: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG2X_DUMMY13: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG2X_DUMMY14: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG2X_DUMMY15: 1;
    };
} AON_FAST_REG2X_SWR_AUDIO_PFM300to600K_TYPE;

/* 0x1224
    0       R/W SWR_AUDIO_PFM300to600K_SEL_NI_ON                1'b0                    SWR_AUDIO_REG_MASK
    5:1     R/W SWR_AUDIO_PFM300to600K_TUNE_CCOT_FORCE          5'b01100                SWR_AUDIO_REG_MASK
    7:6     R/W SWR_AUDIO_PFM300to600K_TUNE_PFM_COMP_SAMPLE_CYC 2'b00                   SWR_AUDIO_REG_MASK
    8       R/W SWR_AUDIO_PFM300to600K_EN_VDROP_DET             1'b0                    SWR_AUDIO_REG_MASK
    9       R/W SWR_AUDIO_PFM300to600K_REG3X_DUMMY9             1'b0                    SWR_AUDIO_REG_MASK
    10      R/W SWR_AUDIO_PFM300to600K_REG3X_DUMMY10            1'b1                    SWR_AUDIO_REG_MASK
    11      R/W SWR_AUDIO_PFM300to600K_REG3X_DUMMY11            1'b0                    SWR_AUDIO_REG_MASK
    12      R/W SWR_AUDIO_PFM300to600K_REG3X_DUMMY12            1'b1                    SWR_AUDIO_REG_MASK
    13      R/W SWR_AUDIO_PFM300to600K_REG3X_DUMMY13            1'b1                    SWR_AUDIO_REG_MASK
    14      R/W SWR_AUDIO_PFM300to600K_REG3X_DUMMY14            1'b0                    SWR_AUDIO_REG_MASK
    15      R/W SWR_AUDIO_PFM300to600K_REG3X_DUMMY15            1'b0                    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG3X_SWR_AUDIO_PFM300to600K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFM300to600K_SEL_NI_ON: 1;
        uint16_t SWR_AUDIO_PFM300to600K_TUNE_CCOT_FORCE: 5;
        uint16_t SWR_AUDIO_PFM300to600K_TUNE_PFM_COMP_SAMPLE_CYC: 2;
        uint16_t SWR_AUDIO_PFM300to600K_EN_VDROP_DET: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG3X_DUMMY9: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG3X_DUMMY10: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG3X_DUMMY11: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG3X_DUMMY12: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG3X_DUMMY13: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG3X_DUMMY14: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG3X_DUMMY15: 1;
    };
} AON_FAST_REG3X_SWR_AUDIO_PFM300to600K_TYPE;

/* 0x1226
    4:0     R/W SWR_AUDIO_PFM300to600K_TUNE_CCOT_MANU_CODE      5'b00011                SWR_AUDIO_REG_MASK
    5       R/W SWR_AUDIO_PFM300to600K_SEL_CCOT_MANU_MODE       1'b1                    SWR_AUDIO_REG_MASK
    6       R/W SWR_AUDIO_PFM300to600K_REG4X_DUMMY6             1'b1                    SWR_AUDIO_REG_MASK
    7       R/W SWR_AUDIO_PFM300to600K_REG4X_DUMMY7             1'b1                    SWR_AUDIO_REG_MASK
    8       R/W SWR_AUDIO_PFM300to600K_REG4X_DUMMY8             1'b1                    SWR_AUDIO_REG_MASK
    9       R/W SWR_AUDIO_PFM300to600K_REG4X_DUMMY9             1'b1                    SWR_AUDIO_REG_MASK
    10      R/W SWR_AUDIO_PFM300to600K_REG4X_DUMMY10            1'b1                    SWR_AUDIO_REG_MASK
    11      R/W SWR_AUDIO_PFM300to600K_REG4X_DUMMY11            1'b0                    SWR_AUDIO_REG_MASK
    12      R/W SWR_AUDIO_PFM300to600K_REG4X_DUMMY12            1'b0                    SWR_AUDIO_REG_MASK
    13      R/W SWR_AUDIO_PFM300to600K_REG4X_DUMMY13            1'b0                    SWR_AUDIO_REG_MASK
    14      R/W SWR_AUDIO_PFM300to600K_REG4X_DUMMY14            1'b0                    SWR_AUDIO_REG_MASK
    15      R/W SWR_AUDIO_PFM300to600K_REG4X_DUMMY15            1'b0                    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG4X_SWR_AUDIO_PFM300to600K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFM300to600K_TUNE_CCOT_MANU_CODE: 5;
        uint16_t SWR_AUDIO_PFM300to600K_SEL_CCOT_MANU_MODE: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG4X_DUMMY6: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG4X_DUMMY7: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG4X_DUMMY8: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG4X_DUMMY9: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG4X_DUMMY10: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG4X_DUMMY11: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG4X_DUMMY12: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG4X_DUMMY13: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG4X_DUMMY14: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG4X_DUMMY15: 1;
    };
} AON_FAST_REG4X_SWR_AUDIO_PFM300to600K_TYPE;

/* 0x1228
    0       R/W SWR_AUDIO_PFM300to600K_REG5X_DUMMY0             1'b1                    SWR_AUDIO_REG_MASK
    1       R/W SWR_AUDIO_PFM300to600K_REG5X_DUMMY1             1'b1                    SWR_AUDIO_REG_MASK
    2       R/W SWR_AUDIO_PFM300to600K_REG5X_DUMMY2             1'b1                    SWR_AUDIO_REG_MASK
    3       R/W SWR_AUDIO_PFM300to600K_REG5X_DUMMY3             1'b1                    SWR_AUDIO_REG_MASK
    4       R/W SWR_AUDIO_PFM300to600K_REG5X_DUMMY4             1'b1                    SWR_AUDIO_REG_MASK
    5       R/W SWR_AUDIO_PFM300to600K_REG5X_DUMMY5             1'b1                    SWR_AUDIO_REG_MASK
    6       R/W SWR_AUDIO_PFM300to600K_REG5X_DUMMY6             1'b1                    SWR_AUDIO_REG_MASK
    7       R/W SWR_AUDIO_PFM300to600K_REG5X_DUMMY7             1'b1                    SWR_AUDIO_REG_MASK
    8       R/W SWR_AUDIO_PFM300to600K_REG5X_DUMMY8             1'b0                    SWR_AUDIO_REG_MASK
    9       R/W SWR_AUDIO_PFM300to600K_REG5X_DUMMY9             1'b0                    SWR_AUDIO_REG_MASK
    10      R/W SWR_AUDIO_PFM300to600K_REG5X_DUMMY10            1'b0                    SWR_AUDIO_REG_MASK
    11      R/W SWR_AUDIO_PFM300to600K_REG5X_DUMMY11            1'b0                    SWR_AUDIO_REG_MASK
    12      R/W SWR_AUDIO_PFM300to600K_REG5X_DUMMY12            1'b0                    SWR_AUDIO_REG_MASK
    13      R/W SWR_AUDIO_PFM300to600K_REG5X_DUMMY13            1'b0                    SWR_AUDIO_REG_MASK
    14      R/W SWR_AUDIO_PFM300to600K_REG5X_DUMMY14            1'b0                    SWR_AUDIO_REG_MASK
    15      R/W SWR_AUDIO_PFM300to600K_REG5X_DUMMY15            1'b0                    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG5X_SWR_AUDIO_PFM300to600K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFM300to600K_REG5X_DUMMY0: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG5X_DUMMY1: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG5X_DUMMY2: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG5X_DUMMY3: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG5X_DUMMY4: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG5X_DUMMY5: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG5X_DUMMY6: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG5X_DUMMY7: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG5X_DUMMY8: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG5X_DUMMY9: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG5X_DUMMY10: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG5X_DUMMY11: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG5X_DUMMY12: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG5X_DUMMY13: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG5X_DUMMY14: 1;
        uint16_t SWR_AUDIO_PFM300to600K_REG5X_DUMMY15: 1;
    };
} AON_FAST_REG5X_SWR_AUDIO_PFM300to600K_TYPE;

/* 0x122A
    9:0     R   SWR_AUDIO_PFM300to600K_PFM_COUNT                10'b0000000000
    14:10   R   SWR_AUDIO_PFM300to600K_CCOT                     5'b00011
    15      R   SWR_AUDIO_PFM300to600K_EN_AUDK                  1'b0
 */
typedef union _AON_FAST_C_KOUT0X_SWR_AUDIO_PFM300to600K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFM300to600K_PFM_COUNT: 10;
        uint16_t SWR_AUDIO_PFM300to600K_CCOT: 5;
        uint16_t SWR_AUDIO_PFM300to600K_EN_AUDK: 1;
    };
} AON_FAST_C_KOUT0X_SWR_AUDIO_PFM300to600K_TYPE;

/* 0x122C
    0       R   SWR_AUDIO_PFM300to600K_PFM_DOWN_TT              1'h0
    1       R   SWR_AUDIO_PFM300to600K_PFM_UP_TT                1'h0
    2       R   SWR_AUDIO_PFM300to600K_SEL_FORCE                1'h0
    3       R   SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY3          1'h0
    4       R   SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY4          1'h0
    5       R   SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY5          1'h0
    6       R   SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY6          1'h0
    7       R   SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY7          1'h0
    8       R   SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY8          1'h0
    9       R   SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY9          1'h0
    10      R   SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY10         1'h0
    11      R   SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY11         1'h0
    12      R   SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY12         1'h0
    13      R   SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY13         1'h0
    14      R   SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY14         1'h0
    15      R   SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY15         1'h0
 */
typedef union _AON_FAST_C_KOUT1X_SWR_AUDIO_PFM300to600K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_PFM300to600K_PFM_DOWN_TT: 1;
        uint16_t SWR_AUDIO_PFM300to600K_PFM_UP_TT: 1;
        uint16_t SWR_AUDIO_PFM300to600K_SEL_FORCE: 1;
        uint16_t SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY3: 1;
        uint16_t SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY4: 1;
        uint16_t SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY5: 1;
        uint16_t SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY6: 1;
        uint16_t SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY7: 1;
        uint16_t SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY8: 1;
        uint16_t SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY9: 1;
        uint16_t SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY10: 1;
        uint16_t SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY11: 1;
        uint16_t SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY12: 1;
        uint16_t SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY13: 1;
        uint16_t SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY14: 1;
        uint16_t SWR_AUDIO_PFM300to600K_C_KOUT1X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT1X_SWR_AUDIO_PFM300to600K_TYPE;

/* 0x122E
    0       R   RO_AON_SWR_AUDIO_ZCDQ_RST_B                     1'b0
    1       R   RO_AON_SWR_AUDIO_ZCDQ_POW_UD_DIG                1'b0
    2       R/W SWR_AUDIO_ZCDQ_FORCE_CODE2                      1'b0                    SWR_AUDIO_REG_MASK
    3       R/W SWR_AUDIO_ZCDQ_STICKY_CODE2                     1'b0                    SWR_AUDIO_REG_MASK
    4       R   RO_AON_SWR_AUDIO_ZCDQ_FORCE_CODE1               1'b0
    5       R/W SWR_AUDIO_ZCDQ_STICKY_CODE1                     1'b0                    SWR_AUDIO_REG_MASK
    6       R/W SWR_AUDIO_ZCDQ_EXT_UD                           1'b1                    SWR_AUDIO_REG_MASK
    7       R/W SWR_AUDIO_ZCDQ_SEL_EXT_UD                       1'b0                    SWR_AUDIO_REG_MASK
    8       R/W SWR_AUDIO_ZCDQ_REG0X_DUMMY8                     1'b1                    SWR_AUDIO_REG_MASK
    9       R/W SWR_AUDIO_ZCDQ_REG0X_DUMMY9                     1'b1                    SWR_AUDIO_REG_MASK
    10      R/W SWR_AUDIO_ZCDQ_REG0X_DUMMY10                    1'b1                    SWR_AUDIO_REG_MASK
    11      R/W SWR_AUDIO_ZCDQ_REG0X_DUMMY11                    1'b1                    SWR_AUDIO_REG_MASK
    12      R/W SWR_AUDIO_ZCDQ_REG0X_DUMMY12                    1'b0                    SWR_AUDIO_REG_MASK
    13      R/W SWR_AUDIO_ZCDQ_REG0X_DUMMY13                    1'b0                    SWR_AUDIO_REG_MASK
    14      R/W SWR_AUDIO_ZCDQ_REG0X_DUMMY14                    1'b0                    SWR_AUDIO_REG_MASK
    15      R/W SWR_AUDIO_ZCDQ_REG0X_DUMMY15                    1'b0                    SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG0X_SWR_AUDIO_ZCDQ_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_SWR_AUDIO_ZCDQ_RST_B: 1;
        uint16_t RO_AON_SWR_AUDIO_ZCDQ_POW_UD_DIG: 1;
        uint16_t SWR_AUDIO_ZCDQ_FORCE_CODE2: 1;
        uint16_t SWR_AUDIO_ZCDQ_STICKY_CODE2: 1;
        uint16_t RO_AON_SWR_AUDIO_ZCDQ_FORCE_CODE1: 1;
        uint16_t SWR_AUDIO_ZCDQ_STICKY_CODE1: 1;
        uint16_t SWR_AUDIO_ZCDQ_EXT_UD: 1;
        uint16_t SWR_AUDIO_ZCDQ_SEL_EXT_UD: 1;
        uint16_t SWR_AUDIO_ZCDQ_REG0X_DUMMY8: 1;
        uint16_t SWR_AUDIO_ZCDQ_REG0X_DUMMY9: 1;
        uint16_t SWR_AUDIO_ZCDQ_REG0X_DUMMY10: 1;
        uint16_t SWR_AUDIO_ZCDQ_REG0X_DUMMY11: 1;
        uint16_t SWR_AUDIO_ZCDQ_REG0X_DUMMY12: 1;
        uint16_t SWR_AUDIO_ZCDQ_REG0X_DUMMY13: 1;
        uint16_t SWR_AUDIO_ZCDQ_REG0X_DUMMY14: 1;
        uint16_t SWR_AUDIO_ZCDQ_REG0X_DUMMY15: 1;
    };
} AON_FAST_REG0X_SWR_AUDIO_ZCDQ_TYPE;

/* 0x1230
    7:0     R/W SWR_AUDIO_ZCDQ_TUNE_FORCECODE2                  8'b01100000             SWR_AUDIO_REG_MASK
    15:8    R/W SWR_AUDIO_ZCDQ_TUNE_FORCECODE1                  8'b01100000             SWR_AUDIO_REG_MASK
 */
typedef union _AON_FAST_REG1X_SWR_AUDIO_ZCDQ_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_ZCDQ_TUNE_FORCECODE2: 8;
        uint16_t SWR_AUDIO_ZCDQ_TUNE_FORCECODE1: 8;
    };
} AON_FAST_REG1X_SWR_AUDIO_ZCDQ_TYPE;

/* 0x1232
    7:0     R   SWR_AUDIO_ZCDQ_DIG_ZCDQ                         8'b01100000
    8       R   SWR_AUDIO_ZCDQ_C_KOUT0X_DUMMY8                  1'b0
    9       R   SWR_AUDIO_ZCDQ_C_KOUT0X_DUMMY9                  1'b0
    10      R   SWR_AUDIO_ZCDQ_C_KOUT0X_DUMMY10                 1'b0
    11      R   SWR_AUDIO_ZCDQ_C_KOUT0X_DUMMY11                 1'b0
    12      R   SWR_AUDIO_ZCDQ_C_KOUT0X_DUMMY12                 1'b0
    13      R   SWR_AUDIO_ZCDQ_C_KOUT0X_DUMMY13                 1'b0
    14      R   SWR_AUDIO_ZCDQ_C_KOUT0X_DUMMY14                 1'b0
    15      R   SWR_AUDIO_ZCDQ_C_KOUT0X_DUMMY15                 1'b0
 */
typedef union _AON_FAST_C_KOUT0X_SWR_AUDIO_ZCDQ_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_ZCDQ_DIG_ZCDQ: 8;
        uint16_t SWR_AUDIO_ZCDQ_C_KOUT0X_DUMMY8: 1;
        uint16_t SWR_AUDIO_ZCDQ_C_KOUT0X_DUMMY9: 1;
        uint16_t SWR_AUDIO_ZCDQ_C_KOUT0X_DUMMY10: 1;
        uint16_t SWR_AUDIO_ZCDQ_C_KOUT0X_DUMMY11: 1;
        uint16_t SWR_AUDIO_ZCDQ_C_KOUT0X_DUMMY12: 1;
        uint16_t SWR_AUDIO_ZCDQ_C_KOUT0X_DUMMY13: 1;
        uint16_t SWR_AUDIO_ZCDQ_C_KOUT0X_DUMMY14: 1;
        uint16_t SWR_AUDIO_ZCDQ_C_KOUT0X_DUMMY15: 1;
    };
} AON_FAST_C_KOUT0X_SWR_AUDIO_ZCDQ_TYPE;

/* 0x1234
    7:0     R   SWR_AUDIO_ZCDQ_DIG_ZCDQ2                        8'b01100000
    15:8    R   SWR_AUDIO_ZCDQ_DIG_ZCDQ1                        8'b01100000
 */
typedef union _AON_FAST_C_KOUT1X_SWR_AUDIO_ZCDQ_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t SWR_AUDIO_ZCDQ_DIG_ZCDQ2: 8;
        uint16_t SWR_AUDIO_ZCDQ_DIG_ZCDQ1: 8;
    };
} AON_FAST_C_KOUT1X_SWR_AUDIO_ZCDQ_TYPE;

/* 0x1400
    0       R/W BTPLL1_ANAPAR_PLL0_DUMMY3                       1'b0                    BTPLL_REG_MASK
    1       R   RO_AON_BTPLL1_POW_PLL                           1'b0
    4:2     R/W BTPLL1_SEL_CP_BIA                               3'b000                  BTPLL_REG_MASK
    5       R/W BTPLL1_ANAPAR_PLL0_DUMMY2                       1'b0                    BTPLL_REG_MASK
    7:6     R/W BTPLL1_SEL_LDO1_VOUT                            2'b01                   BTPLL_REG_MASK
    9:8     R/W BTPLL1_SEL_LPF_CP                               2'b10                   BTPLL_REG_MASK
    11:10   R/W BTPLL1_SEL_LPF_CS                               2'b10                   BTPLL_REG_MASK
    14:12   R/W BTPLL1_SEL_LPF_R3                               3'b010                  BTPLL_REG_MASK
    15      R/W BTPLL1_ANAPAR_PLL0_DUMMY1                       1'b0                    BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL1_ANAPAR_PLL0_DUMMY3: 1;
        uint16_t RO_AON_BTPLL1_POW_PLL: 1;
        uint16_t BTPLL1_SEL_CP_BIA: 3;
        uint16_t BTPLL1_ANAPAR_PLL0_DUMMY2: 1;
        uint16_t BTPLL1_SEL_LDO1_VOUT: 2;
        uint16_t BTPLL1_SEL_LPF_CP: 2;
        uint16_t BTPLL1_SEL_LPF_CS: 2;
        uint16_t BTPLL1_SEL_LPF_R3: 3;
        uint16_t BTPLL1_ANAPAR_PLL0_DUMMY1: 1;
    };
} AON_FAST_BT_ANAPAR_PLL0_TYPE;

/* 0x1402
    2:0     R/W BTPLL1_SEL_LPF_RS                               3'b100                  BTPLL_REG_MASK
    3       R/W BTPLL1_EN_DOG_B                                 1'b1                    BTPLL_REG_MASK
    5:4     R/W BTPLL1_VC_THL                                   2'b00                   BTPLL_REG_MASK
    6       R/W BTPLL1_SEL_FREF_EDGE                            1'b0                    BTPLL_REG_MASK
    13:7    R/W BTPLL1_ANAPAR_PLL1_DUMMY1                       7'b0000000              BTPLL_REG_MASK
    14      R/W BTPLL1_GATED_DAC_CLK                            1'b1                    BTPLL_REG_MASK
    15      R/W BTPLL1_GATED_ADC_CLK                            1'b1                    BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL1_SEL_LPF_RS: 3;
        uint16_t BTPLL1_EN_DOG_B: 1;
        uint16_t BTPLL1_VC_THL: 2;
        uint16_t BTPLL1_SEL_FREF_EDGE: 1;
        uint16_t BTPLL1_ANAPAR_PLL1_DUMMY1: 7;
        uint16_t BTPLL1_GATED_DAC_CLK: 1;
        uint16_t BTPLL1_GATED_ADC_CLK: 1;
    };
} AON_FAST_BT_ANAPAR_PLL1_TYPE;

/* 0x1404
    0       R/W BTPLL1_EN_PLL_CKO1                              1'b1                    BTPLL_REG_MASK
    1       R/W BTPLL1_SEL_DIV2_PLL_CKO1                        1'b1                    BTPLL_REG_MASK
    2       R/W BTPLL1_SEL_PLL_CKO1                             1'b0                    BTPLL_REG_MASK
    3       R/W BTPLL1_EN_PLL_CKO2                              1'b1                    BTPLL_REG_MASK
    4       R/W BTPLL1_EN_PLL_ADC_CLK                           1'b0                    BTPLL_REG_MASK
    5       R/W BTPLL1_SEL_PLL_ADC_CLK                          1'b1                    BTPLL_REG_MASK
    6       R/W BTPLL1_EN_PLL_DAC_CLK                           1'b0                    BTPLL_REG_MASK
    7       R/W BTPLL1_SEL_PLL_DAC_CLK                          1'b1                    BTPLL_REG_MASK
    8       R/W BTPLL1_EN_PLL_TPMKADC_CLK                       1'b1                    BTPLL_REG_MASK
    10:9    R/W BTPLL1_SEL_DIVDN_PLL_CLK                        2'b01                   BTPLL_REG_MASK
    11      R/W BTPLL1_SEL_IF40M                                1'b0                    BTPLL_REG_MASK
    12      R/W BTPLL1_ANAPAR_PLL2_DUMMY2                       1'b0                    BTPLL_REG_MASK
    14:13   R/W BTPLL1_SEL_LPF_C3                               2'b10                   BTPLL_REG_MASK
    15      R/W BTPLL1_EN_BTADDA                                1'b1                    BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL1_EN_PLL_CKO1: 1;
        uint16_t BTPLL1_SEL_DIV2_PLL_CKO1: 1;
        uint16_t BTPLL1_SEL_PLL_CKO1: 1;
        uint16_t BTPLL1_EN_PLL_CKO2: 1;
        uint16_t BTPLL1_EN_PLL_ADC_CLK: 1;
        uint16_t BTPLL1_SEL_PLL_ADC_CLK: 1;
        uint16_t BTPLL1_EN_PLL_DAC_CLK: 1;
        uint16_t BTPLL1_SEL_PLL_DAC_CLK: 1;
        uint16_t BTPLL1_EN_PLL_TPMKADC_CLK: 1;
        uint16_t BTPLL1_SEL_DIVDN_PLL_CLK: 2;
        uint16_t BTPLL1_SEL_IF40M: 1;
        uint16_t BTPLL1_ANAPAR_PLL2_DUMMY2: 1;
        uint16_t BTPLL1_SEL_LPF_C3: 2;
        uint16_t BTPLL1_EN_BTADDA: 1;
    };
} AON_FAST_BT_ANAPAR_PLL2_TYPE;

/* 0x1406
    0       R   RO_AON_BTPLL1_POW_DIV_PLL_CLK                   1'b0
    1       R/W BTPLL1_EN_MBIAS_LPF                             1'b1                    BTPLL_REG_MASK
    2       R/W BTPLL1_SEL_CKFREF                               1'b0                    BTPLL_REG_MASK
    4:3     R/W BTPLL1_SEL_DIVDN_XTAL                           2'b11                   BTPLL_REG_MASK
    8:5     R/W BTPLL1_SEL_DBG_SDM                              4'b0000                 BTPLL_REG_MASK
    15:9    R/W BTPLL1_SEL_DIVN_SDM                             7'b0111010              BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_BTPLL1_POW_DIV_PLL_CLK: 1;
        uint16_t BTPLL1_EN_MBIAS_LPF: 1;
        uint16_t BTPLL1_SEL_CKFREF: 1;
        uint16_t BTPLL1_SEL_DIVDN_XTAL: 2;
        uint16_t BTPLL1_SEL_DBG_SDM: 4;
        uint16_t BTPLL1_SEL_DIVN_SDM: 7;
    };
} AON_FAST_BT_ANAPAR_PLL3_TYPE;

/* 0x1408
    15:0    R/W BTPLL1_SEL_SDM_F0F_LSB                          16'h0000                BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL1_SEL_SDM_F0F_LSB;
    };
} AON_FAST_BT_ANAPAR_PLL4_TYPE;

/* 0x140A
    4:0     R/W BTPLL1_SEL_SDM_F0F_MSB                          5'b00000                BTPLL_REG_MASK
    7:5     R/W BTPLL1_SEL_SDM_F0N                              3'b000                  BTPLL_REG_MASK
    8       R/W BTPLL1_SEL_SDM_ORDER                            1'b0                    BTPLL_REG_MASK
    9       R/W BTPLL1_EN_AGPIO                                 1'b0                    BTPLL_REG_MASK
    10      R/W BTPLL1_SEL_AGPIO                                1'b0                    BTPLL_REG_MASK
    12:11   R/W BTPLL1_SEL_AGPIO_BUFFER                         2'b00                   BTPLL_REG_MASK
    13      R/W BTPLL1_ANAPAR_PLL5_DUMMY1                       1'b0                    BTPLL_REG_MASK
    14      R/W BTPLL1_EN_SDM_NF_CODE_SYNC                      1'b0                    BTPLL_REG_MASK
    15      R/W BTPLL1_EN_SDM_NF_CODE_LOAD                      1'b0                    BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL1_SEL_SDM_F0F_MSB: 5;
        uint16_t BTPLL1_SEL_SDM_F0N: 3;
        uint16_t BTPLL1_SEL_SDM_ORDER: 1;
        uint16_t BTPLL1_EN_AGPIO: 1;
        uint16_t BTPLL1_SEL_AGPIO: 1;
        uint16_t BTPLL1_SEL_AGPIO_BUFFER: 2;
        uint16_t BTPLL1_ANAPAR_PLL5_DUMMY1: 1;
        uint16_t BTPLL1_EN_SDM_NF_CODE_SYNC: 1;
        uint16_t BTPLL1_EN_SDM_NF_CODE_LOAD: 1;
    };
} AON_FAST_BT_ANAPAR_PLL5_TYPE;

/* 0x140C
    0       R/W BTPLL2_ANAPAR_PLL6_DUMMY3                       1'b0                    BTPLL_REG_MASK
    1       R   RO_AON_BTPLL2_POW_PLL                           1'b0
    4:2     R/W BTPLL2_SEL_CP_BIA                               3'b000                  BTPLL_REG_MASK
    5       R/W BTPLL2_ANAPAR_PLL6_DUMMY2                       1'b0                    BTPLL_REG_MASK
    7:6     R/W BTPLL2_SEL_LDO1_VOUT                            2'b01                   BTPLL_REG_MASK
    9:8     R/W BTPLL2_SEL_LPF_CP                               2'b10                   BTPLL_REG_MASK
    11:10   R/W BTPLL2_SEL_LPF_CS                               2'b10                   BTPLL_REG_MASK
    14:12   R/W BTPLL2_SEL_LPF_R3                               3'b010                  BTPLL_REG_MASK
    15      R/W BTPLL2_ANAPAR_PLL6_DUMMY1                       1'b0                    BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL2_ANAPAR_PLL6_DUMMY3: 1;
        uint16_t RO_AON_BTPLL2_POW_PLL: 1;
        uint16_t BTPLL2_SEL_CP_BIA: 3;
        uint16_t BTPLL2_ANAPAR_PLL6_DUMMY2: 1;
        uint16_t BTPLL2_SEL_LDO1_VOUT: 2;
        uint16_t BTPLL2_SEL_LPF_CP: 2;
        uint16_t BTPLL2_SEL_LPF_CS: 2;
        uint16_t BTPLL2_SEL_LPF_R3: 3;
        uint16_t BTPLL2_ANAPAR_PLL6_DUMMY1: 1;
    };
} AON_FAST_BT_ANAPAR_PLL6_TYPE;

/* 0x140E
    2:0     R/W BTPLL2_SEL_LPF_RS                               3'b100                  BTPLL_REG_MASK
    3       R/W BTPLL2_EN_DOG_B                                 1'b1                    BTPLL_REG_MASK
    5:4     R/W BTPLL2_VC_THL                                   2'b00                   BTPLL_REG_MASK
    6       R/W BTPLL2_SEL_FREF_EDGE                            1'b0                    BTPLL_REG_MASK
    15:7    R/W BTPLL2_ANAPAR_PLL7_DUMMY1                       9'b0000000              BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL2_SEL_LPF_RS: 3;
        uint16_t BTPLL2_EN_DOG_B: 1;
        uint16_t BTPLL2_VC_THL: 2;
        uint16_t BTPLL2_SEL_FREF_EDGE: 1;
        uint16_t BTPLL2_ANAPAR_PLL7_DUMMY1: 9;
    };
} AON_FAST_BT_ANAPAR_PLL7_TYPE;

/* 0x1410
    0       R/W BTPLL2_EN_PLL_CKO1                              1'b1                    BTPLL_REG_MASK
    1       R/W BTPLL2_SEL_DIV2_PLL_CKO1                        1'b1                    BTPLL_REG_MASK
    2       R/W BTPLL2_SEL_PLL_CKO1                             1'b0                    BTPLL_REG_MASK
    3       R/W BTPLL2_ANAPAR_PLL8_DUMMY3                       1'b0                    BTPLL_REG_MASK
    5:4     R/W BTPLL2_SEL_AGPIO_OUTPUT                         2'b01                   BTPLL_REG_MASK
    12:6    R/W BTPLL2_ANAPAR_PLL8_DUMMY2                       7'b0000000              BTPLL_REG_MASK
    14:13   R/W BTPLL2_SEL_LPF_C3                               2'b10                   BTPLL_REG_MASK
    15      R/W BTPLL2_ANAPAR_PLL8_DUMMY1                       1'b0                    BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL2_EN_PLL_CKO1: 1;
        uint16_t BTPLL2_SEL_DIV2_PLL_CKO1: 1;
        uint16_t BTPLL2_SEL_PLL_CKO1: 1;
        uint16_t BTPLL2_ANAPAR_PLL8_DUMMY3: 1;
        uint16_t BTPLL2_SEL_AGPIO_OUTPUT: 2;
        uint16_t BTPLL2_ANAPAR_PLL8_DUMMY2: 7;
        uint16_t BTPLL2_SEL_LPF_C3: 2;
        uint16_t BTPLL2_ANAPAR_PLL8_DUMMY1: 1;
    };
} AON_FAST_BT_ANAPAR_PLL8_TYPE;

/* 0x1412
    0       R   RO_AON_BTPLL2_POW_DIV_PLL_CLK                   1'b0
    1       R/W BTPLL2_EN_MBIAS_LPF                             1'b1                    BTPLL_REG_MASK
    2       R/W BTPLL2_SEL_CKFREF                               1'b0                    BTPLL_REG_MASK
    4:3     R/W BTPLL2_SEL_DIVDN_XTAL                           2'b11                   BTPLL_REG_MASK
    8:5     R/W BTPLL2_SEL_DBG_SDM                              4'b0000                 BTPLL_REG_MASK
    15:9    R/W BTPLL2_SEL_DIVN_SDM                             7'b0011100              BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL9_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_BTPLL2_POW_DIV_PLL_CLK: 1;
        uint16_t BTPLL2_EN_MBIAS_LPF: 1;
        uint16_t BTPLL2_SEL_CKFREF: 1;
        uint16_t BTPLL2_SEL_DIVDN_XTAL: 2;
        uint16_t BTPLL2_SEL_DBG_SDM: 4;
        uint16_t BTPLL2_SEL_DIVN_SDM: 7;
    };
} AON_FAST_BT_ANAPAR_PLL9_TYPE;

/* 0x1414
    15:0    R/W BTPLL2_SEL_SDM_F0F_LSB                          16'h0000                BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL10_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL2_SEL_SDM_F0F_LSB;
    };
} AON_FAST_BT_ANAPAR_PLL10_TYPE;

/* 0x1416
    4:0     R/W BTPLL2_SEL_SDM_F0F_MSB                          5'b00000                BTPLL_REG_MASK
    7:5     R/W BTPLL2_SEL_SDM_F0N                              3'b000                  BTPLL_REG_MASK
    8       R/W BTPLL2_SEL_SDM_ORDER                            1'b0                    BTPLL_REG_MASK
    9       R/W BTPLL2_EN_AGPIO                                 1'b0                    BTPLL_REG_MASK
    10      R/W BTPLL2_SEL_AGPIO                                1'b0                    BTPLL_REG_MASK
    12:11   R/W BTPLL2_SEL_AGPIO_BUFFER                         2'b00                   BTPLL_REG_MASK
    13      R/W BTPLL2_ANAPAR_PLL11_DUMMY1                      1'b0                    BTPLL_REG_MASK
    14      R/W BTPLL2_EN_SDM_NF_CODE_SYNC                      1'b0                    BTPLL_REG_MASK
    15      R/W BTPLL2_EN_SDM_NF_CODE_LOAD                      1'b0                    BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL11_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL2_SEL_SDM_F0F_MSB: 5;
        uint16_t BTPLL2_SEL_SDM_F0N: 3;
        uint16_t BTPLL2_SEL_SDM_ORDER: 1;
        uint16_t BTPLL2_EN_AGPIO: 1;
        uint16_t BTPLL2_SEL_AGPIO: 1;
        uint16_t BTPLL2_SEL_AGPIO_BUFFER: 2;
        uint16_t BTPLL2_ANAPAR_PLL11_DUMMY1: 1;
        uint16_t BTPLL2_EN_SDM_NF_CODE_SYNC: 1;
        uint16_t BTPLL2_EN_SDM_NF_CODE_LOAD: 1;
    };
} AON_FAST_BT_ANAPAR_PLL11_TYPE;

/* 0x1418
    0       R/W BTPLL3_DUMMY3                                   1'b0                    BTPLL_REG_MASK
    1       R   RO_AON_BTPLL3_POW_PLL                           1'b0
    4:2     R/W BTPLL3_SEL_CP_BIA                               3'b000                  BTPLL_REG_MASK
    7:5     R/W BTPLL3_ANAPAR_PLL12_DUMMY2                      3'b0                    BTPLL_REG_MASK
    9:8     R/W BTPLL3_SEL_LPF_CP                               2'b10                   BTPLL_REG_MASK
    11:10   R/W BTPLL3_SEL_LPF_CS                               2'b10                   BTPLL_REG_MASK
    14:12   R/W BTPLL3_SEL_LPF_R3                               3'b010                  BTPLL_REG_MASK
    15      R/W BTPLL3_ANAPAR_PLL12_DUMMY1                      1'b0                    BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL12_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL3_DUMMY3: 1;
        uint16_t RO_AON_BTPLL3_POW_PLL: 1;
        uint16_t BTPLL3_SEL_CP_BIA: 3;
        uint16_t BTPLL3_ANAPAR_PLL12_DUMMY2: 3;
        uint16_t BTPLL3_SEL_LPF_CP: 2;
        uint16_t BTPLL3_SEL_LPF_CS: 2;
        uint16_t BTPLL3_SEL_LPF_R3: 3;
        uint16_t BTPLL3_ANAPAR_PLL12_DUMMY1: 1;
    };
} AON_FAST_BT_ANAPAR_PLL12_TYPE;

/* 0x141A
    2:0     R/W BTPLL3_SEL_LPF_RS                               3'b100                  BTPLL_REG_MASK
    3       R/W BTPLL3_EN_DOG_B                                 1'b1                    BTPLL_REG_MASK
    5:4     R/W BTPLL3_VC_THL                                   2'b00                   BTPLL_REG_MASK
    6       R/W BTPLL3_SEL_FREF_EDGE                            1'b0                    BTPLL_REG_MASK
    15:7    R/W BTPLL3_ANAPAR_PLL13_DUMMY1                      9'b0000000              BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL13_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL3_SEL_LPF_RS: 3;
        uint16_t BTPLL3_EN_DOG_B: 1;
        uint16_t BTPLL3_VC_THL: 2;
        uint16_t BTPLL3_SEL_FREF_EDGE: 1;
        uint16_t BTPLL3_ANAPAR_PLL13_DUMMY1: 9;
    };
} AON_FAST_BT_ANAPAR_PLL13_TYPE;

/* 0x141C
    0       R/W BTPLL3_EN_PLL_CKO1                              1'b1                    BTPLL_REG_MASK
    1       R/W BTPLL3_SEL_DIV2_PLL_CKO1                        1'b1                    BTPLL_REG_MASK
    2       R/W BTPLL3_SEL_PLL_CKO1                             1'b0                    BTPLL_REG_MASK
    12:3    R/W BTPLL3_DUMMY2                                   10'b0000000000          BTPLL_REG_MASK
    14:13   R/W BTPLL3_SEL_LPF_C3                               2'b10                   BTPLL_REG_MASK
    15      R/W BTPLL3_ANAPAR_PLL14_DUMMY1                      1'b0                    BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL14_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL3_EN_PLL_CKO1: 1;
        uint16_t BTPLL3_SEL_DIV2_PLL_CKO1: 1;
        uint16_t BTPLL3_SEL_PLL_CKO1: 1;
        uint16_t BTPLL3_DUMMY2: 10;
        uint16_t BTPLL3_SEL_LPF_C3: 2;
        uint16_t BTPLL3_ANAPAR_PLL14_DUMMY1: 1;
    };
} AON_FAST_BT_ANAPAR_PLL14_TYPE;

/* 0x141E
    0       R   RO_AON_BTPLL3_POW_DIV_PLL_CLK                   1'b0
    1       R/W BTPLL3_EN_MBIAS_LPF                             1'b1                    BTPLL_REG_MASK
    2       R/W BTPLL3_SEL_CKFREF                               1'b0                    BTPLL_REG_MASK
    4:3     R/W BTPLL3_SEL_DIVDN_XTAL                           2'b00                   BTPLL_REG_MASK
    8:5     R/W BTPLL3_ANAPAR_PLL15_DUMMY1                      4'b0000                 BTPLL_REG_MASK
    15:9    R/W BTPLL3_SEL_DIVN_SDM                             7'b0000010              BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL15_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_BTPLL3_POW_DIV_PLL_CLK: 1;
        uint16_t BTPLL3_EN_MBIAS_LPF: 1;
        uint16_t BTPLL3_SEL_CKFREF: 1;
        uint16_t BTPLL3_SEL_DIVDN_XTAL: 2;
        uint16_t BTPLL3_ANAPAR_PLL15_DUMMY1: 4;
        uint16_t BTPLL3_SEL_DIVN_SDM: 7;
    };
} AON_FAST_BT_ANAPAR_PLL15_TYPE;

/* 0x1420
    15:0    R/W BTPLL3_ANAPAR_PLL16_DUMMY1                      16'h0000                BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL16_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL3_ANAPAR_PLL16_DUMMY1;
    };
} AON_FAST_BT_ANAPAR_PLL16_TYPE;

/* 0x1422
    15:0    R/W BTPLL3_ANAPAR_PLL17_DUMMY1                      16'h0000                BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL17_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL3_ANAPAR_PLL17_DUMMY1;
    };
} AON_FAST_BT_ANAPAR_PLL17_TYPE;

/* 0x1424
    0       R/W BTPLL4_DUMMY3                                   1'b0                    BTPLL_REG_MASK
    1       R   RO_AON_BTPLL4_POW_PLL                           1'b0
    4:2     R/W BTPLL4_SEL_CP_BIA                               3'b000                  BTPLL_REG_MASK
    7:5     R/W BTPLL4_ANAPAR_PLL12_DUMMY2                      3'b0                    BTPLL_REG_MASK
    9:8     R/W BTPLL4_SEL_LPF_CP                               2'b10                   BTPLL_REG_MASK
    11:10   R/W BTPLL4_SEL_LPF_CS                               2'b10                   BTPLL_REG_MASK
    14:12   R/W BTPLL4_SEL_LPF_R3                               3'b000                  BTPLL_REG_MASK
    15      R/W BTPLL4_ANAPAR_PLL18_DUMMY1                      1'b0                    BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL18_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL4_DUMMY3: 1;
        uint16_t RO_AON_BTPLL4_POW_PLL: 1;
        uint16_t BTPLL4_SEL_CP_BIA: 3;
        uint16_t BTPLL4_ANAPAR_PLL12_DUMMY2: 3;
        uint16_t BTPLL4_SEL_LPF_CP: 2;
        uint16_t BTPLL4_SEL_LPF_CS: 2;
        uint16_t BTPLL4_SEL_LPF_R3: 3;
        uint16_t BTPLL4_ANAPAR_PLL18_DUMMY1: 1;
    };
} AON_FAST_BT_ANAPAR_PLL18_TYPE;

/* 0x1426
    2:0     R/W BTPLL4_SEL_LPF_RS                               3'b100                  BTPLL_REG_MASK
    3       R/W BTPLL4_EN_DOG_B                                 1'b1                    BTPLL_REG_MASK
    5:4     R/W BTPLL4_VC_THL                                   2'b00                   BTPLL_REG_MASK
    6       R/W BTPLL4_SEL_FREF_EDGE                            1'b0                    BTPLL_REG_MASK
    15:7    R/W BTPLL4_ANAPAR_PLL19_DUMMY1                      9'b0000000              BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL19_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL4_SEL_LPF_RS: 3;
        uint16_t BTPLL4_EN_DOG_B: 1;
        uint16_t BTPLL4_VC_THL: 2;
        uint16_t BTPLL4_SEL_FREF_EDGE: 1;
        uint16_t BTPLL4_ANAPAR_PLL19_DUMMY1: 9;
    };
} AON_FAST_BT_ANAPAR_PLL19_TYPE;

/* 0x1428
    0       R/W BTPLL4_EN_PLL_CKO1                              1'b1                    BTPLL_REG_MASK
    1       R/W BTPLL4_SEL_DIV2_PLL_CKO1                        1'b1                    BTPLL_REG_MASK
    2       R/W BTPLL4_SEL_PLL_CKO1                             1'b0                    BTPLL_REG_MASK
    12:3    R/W BTPLL4_DUMMY2                                   10'b0000000000          BTPLL_REG_MASK
    14:13   R/W BTPLL4_SEL_LPF_C3                               2'b10                   BTPLL_REG_MASK
    15      R/W BTPLL4_ANAPAR_PLL20_DUMMY1                      1'b0                    BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL20_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL4_EN_PLL_CKO1: 1;
        uint16_t BTPLL4_SEL_DIV2_PLL_CKO1: 1;
        uint16_t BTPLL4_SEL_PLL_CKO1: 1;
        uint16_t BTPLL4_DUMMY2: 10;
        uint16_t BTPLL4_SEL_LPF_C3: 2;
        uint16_t BTPLL4_ANAPAR_PLL20_DUMMY1: 1;
    };
} AON_FAST_BT_ANAPAR_PLL20_TYPE;

/* 0x142A
    0       R   RO_AON_BTPLL4_POW_DIV_PLL_CLK                   1'b0
    1       R/W BTPLL4_EN_MBIAS_LPF                             1'b1                    BTPLL_REG_MASK
    2       R/W BTPLL4_SEL_CKFREF                               1'b0                    BTPLL_REG_MASK
    4:3     R/W BTPLL4_SEL_DIVDN_XTAL                           2'b00                   BTPLL_REG_MASK
    8:5     R/W BTPLL4_ANAPAR_PLL21_DUMMY1                      4'b0000                 BTPLL_REG_MASK
    15:9    R/W BTPLL4_SEL_DIVN_SDM                             7'b0000010              BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL21_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_BTPLL4_POW_DIV_PLL_CLK: 1;
        uint16_t BTPLL4_EN_MBIAS_LPF: 1;
        uint16_t BTPLL4_SEL_CKFREF: 1;
        uint16_t BTPLL4_SEL_DIVDN_XTAL: 2;
        uint16_t BTPLL4_ANAPAR_PLL21_DUMMY1: 4;
        uint16_t BTPLL4_SEL_DIVN_SDM: 7;
    };
} AON_FAST_BT_ANAPAR_PLL21_TYPE;

/* 0x142C
    15:0    R/W BTPLL4_ANAPAR_PLL22_DUMMY1                      16'h0000                BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL22_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL4_ANAPAR_PLL22_DUMMY1;
    };
} AON_FAST_BT_ANAPAR_PLL22_TYPE;

/* 0x142E
    15:0    R/W BTPLL3_ANAPAR_PLL23_DUMMY1                      16'h0000                BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL23_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL3_ANAPAR_PLL23_DUMMY1;
    };
} AON_FAST_BT_ANAPAR_PLL23_TYPE;

/* 0x1430
    0       R/W BTPLL_LDO1_EN_LDO_PC                            1'b0                    BTPLL_REG_MASK
    1       R/W BTPLL_LDO1_DUMMY3                               1'b1                    BTPLL_REG_MASK
    2       R   RO_AON_BTPLL_LDO1_POW_LDO                       1'b0
    3       R/W BTPLL_LDO1_EN_LDO_VPULSE                        1'b0                    BTPLL_REG_MASK
    5:4     R/W BTPLL_LDO1_TUNE_LDO_VOUT                        2'b10                   BTPLL_REG_MASK
    6       R/W BTPLL_LDO1_DUMMY2                               1'b0                    BTPLL_REG_MASK
    7       R/W BTPLL_LDO1_DOUBLE_LDO_OP_I                      1'b0                    BTPLL_REG_MASK
    15:8    R/W BTPLL_LDO1_DUMMY1                               8'b00000000             BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL_LDO1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL_LDO1_EN_LDO_PC: 1;
        uint16_t BTPLL_LDO1_DUMMY3: 1;
        uint16_t RO_AON_BTPLL_LDO1_POW_LDO: 1;
        uint16_t BTPLL_LDO1_EN_LDO_VPULSE: 1;
        uint16_t BTPLL_LDO1_TUNE_LDO_VOUT: 2;
        uint16_t BTPLL_LDO1_DUMMY2: 1;
        uint16_t BTPLL_LDO1_DOUBLE_LDO_OP_I: 1;
        uint16_t BTPLL_LDO1_DUMMY1: 8;
    };
} AON_FAST_BT_ANAPAR_PLL_LDO1_TYPE;

/* 0x1432
    0       R/W BTPLL_LDO2_EN_LDO_PC                            1'b0                    BTPLL_REG_MASK
    1       R/W BTPLL_LDO2_DUMMY3                               1'b1                    BTPLL_REG_MASK
    2       R   RO_AON_BTPLL_LDO2_POW_LDO                       1'b0
    3       R/W BTPLL_LDO2_EN_LDO_VPULSE                        1'b0                    BTPLL_REG_MASK
    5:4     R/W BTPLL_LDO2_TUNE_LDO_VOUT                        2'b10                   BTPLL_REG_MASK
    6       R/W BTPLL_LDO2_DUMMY2                               1'b0                    BTPLL_REG_MASK
    7       R/W BTPLL_LDO2_DOUBLE_LDO_OP_I                      1'b0                    BTPLL_REG_MASK
    15:8    R/W BTPLL_LDO2_DUMMY1                               8'b00000000             BTPLL_REG_MASK
 */
typedef union _AON_FAST_BT_ANAPAR_PLL_LDO2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTPLL_LDO2_EN_LDO_PC: 1;
        uint16_t BTPLL_LDO2_DUMMY3: 1;
        uint16_t RO_AON_BTPLL_LDO2_POW_LDO: 1;
        uint16_t BTPLL_LDO2_EN_LDO_VPULSE: 1;
        uint16_t BTPLL_LDO2_TUNE_LDO_VOUT: 2;
        uint16_t BTPLL_LDO2_DUMMY2: 1;
        uint16_t BTPLL_LDO2_DOUBLE_LDO_OP_I: 1;
        uint16_t BTPLL_LDO2_DUMMY1: 8;
    };
} AON_FAST_BT_ANAPAR_PLL_LDO2_TYPE;

/* 0x1470
    1:0     R/W XTAL32K_SEL_CUR_MAIN                            2'b01                   XTAL32K_REG_MASK
    5:2     R/W XTAL32K_SEL_CUR_GM_INI                          4'b1001                 XTAL32K_REG_MASK
    9:6     R/W XTAL32K_SEL_CUR_GM                              4'b0101                 XTAL32K_REG_MASK
    11:10   R/W XTAL32K_SEL_CUR_REP                             2'b01                   XTAL32K_REG_MASK
    15:12   R/W XTAL32K_SEL_GM                                  4'b1111                 XTAL32K_REG_MASK
 */
typedef union _AON_FAST_RG0X_32KXTAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL32K_SEL_CUR_MAIN: 2;
        uint16_t XTAL32K_SEL_CUR_GM_INI: 4;
        uint16_t XTAL32K_SEL_CUR_GM: 4;
        uint16_t XTAL32K_SEL_CUR_REP: 2;
        uint16_t XTAL32K_SEL_GM: 4;
    };
} AON_FAST_RG0X_32KXTAL_TYPE;

/* 0x1472
    0       R/W XTAL32K_EN_CAP_INITIAL                          1'b1                    XTAL32K_REG_MASK
    1       R/W XTAL32K_EN_CAP_AWAKE                            1'b1                    XTAL32K_REG_MASK
    7:2     R/W XTAL32K_TUNE_SC_XI_FREQ                         6'b100000               XTAL32K_REG_MASK
    13:8    R/W XTAL32K_TUNE_SC_XO_FREQ                         6'b100000               XTAL32K_REG_MASK
    15:14   R/W XTAL32K_SEL_TOK                                 2'b11                   XTAL32K_REG_MASK
 */
typedef union _AON_FAST_RG1X_32KXTAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL32K_EN_CAP_INITIAL: 1;
        uint16_t XTAL32K_EN_CAP_AWAKE: 1;
        uint16_t XTAL32K_TUNE_SC_XI_FREQ: 6;
        uint16_t XTAL32K_TUNE_SC_XO_FREQ: 6;
        uint16_t XTAL32K_SEL_TOK: 2;
    };
} AON_FAST_RG1X_32KXTAL_TYPE;

/* 0x1474
    0       R/W XTAL32K_EN_FBRES_B                              1'b0                    XTAL32K_REG_MASK
    3:1     R/W XTAL32K_SEL_GM_REP                              3'b111                  XTAL32K_REG_MASK
    4       R/W XTAL32K_TUNE_SC_XI_EXTRA_B                      1'b0                    XTAL32K_REG_MASK
    5       R/W XTAL32K_TUNE_SC_XO_EXTRA_B                      1'b0                    XTAL32K_REG_MASK
    6       R/W XTAL32K_EN_GPIO_MODE                            1'b0                    XTAL32K_REG_MASK
    7       R/W XTAL32K_RG2X_DUMMY2                             1'b0                    XTAL32K_REG_MASK
    15:8    R/W XTAL32K_RG2X_DUMMY1                             8'b00000000             XTAL32K_REG_MASK
 */
typedef union _AON_FAST_RG2X_32KXTAL_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL32K_EN_FBRES_B: 1;
        uint16_t XTAL32K_SEL_GM_REP: 3;
        uint16_t XTAL32K_TUNE_SC_XI_EXTRA_B: 1;
        uint16_t XTAL32K_TUNE_SC_XO_EXTRA_B: 1;
        uint16_t XTAL32K_EN_GPIO_MODE: 1;
        uint16_t XTAL32K_RG2X_DUMMY2: 1;
        uint16_t XTAL32K_RG2X_DUMMY1: 8;
    };
} AON_FAST_RG2X_32KXTAL_TYPE;

/* 0x1476
    5:0     R/W OSC32K_TUNE_RCAL_FREQ                           6'b100000               XTAL32K_REG_MASK
    12:6    R/W OSC32K_REG0X_DUMMY2                             7'b0000000              XTAL32K_REG_MASK
    13      R/W OSC32K_REG0X_DUMMY1                             1'b0                    XTAL32K_REG_MASK
    14      R/W OSC32K_GATED_STUP_OK                            1'b0                    XTAL32K_REG_MASK
    15      R/W OSC32K_SEL_LDO_VREF                             1'b0                    XTAL32K_REG_MASK
 */
typedef union _AON_FAST_RG0X_32KOSC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t OSC32K_TUNE_RCAL_FREQ: 6;
        uint16_t OSC32K_REG0X_DUMMY2: 7;
        uint16_t OSC32K_REG0X_DUMMY1: 1;
        uint16_t OSC32K_GATED_STUP_OK: 1;
        uint16_t OSC32K_SEL_LDO_VREF: 1;
    };
} AON_FAST_RG0X_32KOSC_TYPE;

/* 0x1478
    0       R   RO_AON_POW32K_32KOSC33                          1'b1
    1       R   RO_AON_POW32K_32KXTAL33                         1'b0
    7:2     R/W POW32K_DUMMY3                                   6'b100000               XTAL32K_REG_MASK
    14:8    R/W POW32K_DUMMY2                                   7'b0000000              XTAL32K_REG_MASK
    15      R/W POW32K_DUMMY1                                   1'b0                    XTAL32K_REG_MASK
 */
typedef union _AON_FAST_RG0X_POW_32K_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_POW32K_32KOSC33: 1;
        uint16_t RO_AON_POW32K_32KXTAL33: 1;
        uint16_t POW32K_DUMMY3: 6;
        uint16_t POW32K_DUMMY2: 7;
        uint16_t POW32K_DUMMY1: 1;
    };
} AON_FAST_RG0X_POW_32K_TYPE;

/* 0x1490
    2:0     R   RO_AON_XTAL_SEL_MODE                            3'b100
    5:3     R/W XTAL_EN_MODE_DEBUG                              3'h0                    XTAL_REG_MASK
    6       R/W XTAL_EN_MODE_OVERLAP                            1'b0                    XTAL_REG_MASK
    7       R/W XTAL_EN_MODE_OVERLAPX2                          1'b0                    XTAL_REG_MASK
    15:8    R/W XTAL_REG0X_MODE_DUMMY1                          8'h0                    XTAL_REG_MASK
 */
typedef union _AON_FAST_ANAPAR_XTAL_mode_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_XTAL_SEL_MODE: 3;
        uint16_t XTAL_EN_MODE_DEBUG: 3;
        uint16_t XTAL_EN_MODE_OVERLAP: 1;
        uint16_t XTAL_EN_MODE_OVERLAPX2: 1;
        uint16_t XTAL_REG0X_MODE_DUMMY1: 8;
    };
} AON_FAST_ANAPAR_XTAL_mode_TYPE;

/* 0x1492
    7:0     R/W XTAL_TUNE_SC_XI_FREQ                            8'b00111111             XTAL_REG_MASK
    15:8    R/W XTAL_TUNE_SC_XO_FREQ                            8'b00111111             XTAL_REG_MASK
 */
typedef union _AON_FAST_ANAPAR_XTAL0_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_TUNE_SC_XI_FREQ: 8;
        uint16_t XTAL_TUNE_SC_XO_FREQ: 8;
    };
} AON_FAST_ANAPAR_XTAL0_TYPE;

/* 0x1494
    7:0     R/W XTAL_TUNE_SC_INI                                8'b01110000             XTAL_REG_MASK
    15:8    R/W XTAL_TUNE_SC_LP                                 8'b01111111             XTAL_REG_MASK
 */
typedef union _AON_FAST_ANAPAR_XTAL1_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_TUNE_SC_INI: 8;
        uint16_t XTAL_TUNE_SC_LP: 8;
    };
} AON_FAST_ANAPAR_XTAL1_TYPE;

/* 0x1496
    0       R/W XTAL_EN_BK_BG                                   1'b0                    XTAL_REG_MASK
    5:1     R/W XTAL_SEL_BUF_GM_N                               5'b01000                XTAL_REG_MASK
    10:6    R/W XTAL_SEL_BUF_GM_N_LP                            5'b01000                XTAL_REG_MASK
    15:11   R/W XTAL_SEL_BUF_GM_P                               5'b01000                XTAL_REG_MASK
 */
typedef union _AON_FAST_ANAPAR_XTAL2_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_EN_BK_BG: 1;
        uint16_t XTAL_SEL_BUF_GM_N: 5;
        uint16_t XTAL_SEL_BUF_GM_N_LP: 5;
        uint16_t XTAL_SEL_BUF_GM_P: 5;
    };
} AON_FAST_ANAPAR_XTAL2_TYPE;

/* 0x1498
    4:0     R/W XTAL_SEL_BUF_GM_P_LP                            5'b01000                XTAL_REG_MASK
    5       R   RO_AON_XTAL_EN_LPCNT_CLK_DIV                    1'b0
    6       R/W XTAL_EN_LPMODE_CLK_SEL                          1'b0                    XTAL_REG_MASK
    7       R   RO_AON_XTAL_EN_LPS_CLK                          1'b1
    8       R/W XTAL_EN_PDCK_VREF                               1'b1                    XTAL_REG_MASK
    9       R/W XTAL_EN_XO_CLK_SW                               1'b0                    XTAL_REG_MASK
    10      R/W XTAL_EN_DAAC_GM                                 1'b0                    XTAL_REG_MASK
    11      R/W XTAL_EN_SEL_TOK01                               1'b0                    XTAL_REG_MASK
    13:12   R/W XTAL_SEL_AAAC_CAP                               2'b11                   XTAL_REG_MASK
    15:14   R/W XTAL_SEL_AAAC_IOFFSET                           2'b00                   XTAL_REG_MASK
 */
typedef union _AON_FAST_ANAPAR_XTAL3_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_SEL_BUF_GM_P_LP: 5;
        uint16_t RO_AON_XTAL_EN_LPCNT_CLK_DIV: 1;
        uint16_t XTAL_EN_LPMODE_CLK_SEL: 1;
        uint16_t RO_AON_XTAL_EN_LPS_CLK: 1;
        uint16_t XTAL_EN_PDCK_VREF: 1;
        uint16_t XTAL_EN_XO_CLK_SW: 1;
        uint16_t XTAL_EN_DAAC_GM: 1;
        uint16_t XTAL_EN_SEL_TOK01: 1;
        uint16_t XTAL_SEL_AAAC_CAP: 2;
        uint16_t XTAL_SEL_AAAC_IOFFSET: 2;
    };
} AON_FAST_ANAPAR_XTAL3_TYPE;

/* 0x149A
    1:0     R/W XTAL_SEL_AAAC_OP_CUR                            2'b00                   XTAL_REG_MASK
    3:2     R/W XTAL_SEL_AAAC_PKDET_SW                          2'b01                   XTAL_REG_MASK
    4       R/W XTAL_EN_AAAC_TIE_MID                            1'b0                    XTAL_REG_MASK
    9:5     R/W XTAL_SEL_AAAC_VREF                              5'b10000                XTAL_REG_MASK
    10      R/W XTAL_SEL_PMU_BGCUR                              1'b0                    XTAL_REG_MASK
    11      R/W XTAL_SEL_BUF_LPS                                1'b0                    XTAL_REG_MASK
    14:12   R/W XTAL_SEL_CKO_LPCNT                              3'b110                  XTAL_REG_MASK
    15      R/W XTAL_REG4X_DUMMY1                               1'b0                    XTAL_REG_MASK
 */
typedef union _AON_FAST_ANAPAR_XTAL4_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_SEL_AAAC_OP_CUR: 2;
        uint16_t XTAL_SEL_AAAC_PKDET_SW: 2;
        uint16_t XTAL_EN_AAAC_TIE_MID: 1;
        uint16_t XTAL_SEL_AAAC_VREF: 5;
        uint16_t XTAL_SEL_PMU_BGCUR: 1;
        uint16_t XTAL_SEL_BUF_LPS: 1;
        uint16_t XTAL_SEL_CKO_LPCNT: 3;
        uint16_t XTAL_REG4X_DUMMY1: 1;
    };
} AON_FAST_ANAPAR_XTAL4_TYPE;

/* 0x149C
    0       R/W XTAL_REG5X_DUMMY1                               1'b0                    XTAL_REG_MASK
    1       R/W XTAL_EN_MANU_DAAC_PKDET                         1'b0                    XTAL_REG_MASK
    4:2     R/W XTAL_SEL_DAAC_PK                                3'b100                  XTAL_REG_MASK
    7:5     R/W XTAL_EN_MANU_DAAC_PK_SEL                        3'b100                  XTAL_REG_MASK
    8       R/W XTAL_EN_DEBUG                                   1'b0                    XTAL_REG_MASK
    9       R/W XTAL_SEL_AFE_DELAY                              1'b0                    XTAL_REG_MASK
    10      R/W XTAL_SEL_DIGI_DELAY                             1'b0                    XTAL_REG_MASK
    12:11   R/W XTAL_SEL_AFE_DRV                                2'b11                   XTAL_REG_MASK
    13      R/W XTAL_EN_AFE_DRV_LATCH                           1'b0                    XTAL_REG_MASK
    15:14   R/W XTAL_SEL_AFE_DRV_LP                             2'b11                   XTAL_REG_MASK
 */
typedef union _AON_FAST_ANAPAR_XTAL5_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_REG5X_DUMMY1: 1;
        uint16_t XTAL_EN_MANU_DAAC_PKDET: 1;
        uint16_t XTAL_SEL_DAAC_PK: 3;
        uint16_t XTAL_EN_MANU_DAAC_PK_SEL: 3;
        uint16_t XTAL_EN_DEBUG: 1;
        uint16_t XTAL_SEL_AFE_DELAY: 1;
        uint16_t XTAL_SEL_DIGI_DELAY: 1;
        uint16_t XTAL_SEL_AFE_DRV: 2;
        uint16_t XTAL_EN_AFE_DRV_LATCH: 1;
        uint16_t XTAL_SEL_AFE_DRV_LP: 2;
    };
} AON_FAST_ANAPAR_XTAL5_TYPE;

/* 0x149E
    0       R/W XTAL_EN_AFE_DRV_LP_LATCH                        1'b0                    XTAL_REG_MASK
    2:1     R/W XTAL_SEL_DIGI_DRV                               2'b11                   XTAL_REG_MASK
    3       R/W XTAL_EN_DIGI_DRV_LATCH                          1'b0                    XTAL_REG_MASK
    5:4     R/W XTAL_SEL_DIGI_DRV_LP                            2'b11                   XTAL_REG_MASK
    7:6     R/W XTAL_SEL_LPS_DRV                                2'b00                   XTAL_REG_MASK
    9:8     R/W XTAL_SEL_RF_DRV                                 2'b11                   XTAL_REG_MASK
    10      R/W XTAL_EN_RF_DRV_LATCH                            1'b0                    XTAL_REG_MASK
    11      R/W XTAL_EN_FW_CTRL_GATED_B                         1'b1                    XTAL_REG_MASK
    12      R   RO_AON_XTAL_GATED_AFEN                          1'b0
    13      R   RO_AON_XTAL_GATED_AFEN_LP                       1'b0
    14      R   RO_AON_XTAL_GATED_AFEP                          1'b0
    15      R   RO_AON_XTAL_GATED_AFEP_LP                       1'b0
 */
typedef union _AON_FAST_ANAPAR_XTAL6_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_EN_AFE_DRV_LP_LATCH: 1;
        uint16_t XTAL_SEL_DIGI_DRV: 2;
        uint16_t XTAL_EN_DIGI_DRV_LATCH: 1;
        uint16_t XTAL_SEL_DIGI_DRV_LP: 2;
        uint16_t XTAL_SEL_LPS_DRV: 2;
        uint16_t XTAL_SEL_RF_DRV: 2;
        uint16_t XTAL_EN_RF_DRV_LATCH: 1;
        uint16_t XTAL_EN_FW_CTRL_GATED_B: 1;
        uint16_t RO_AON_XTAL_GATED_AFEN: 1;
        uint16_t RO_AON_XTAL_GATED_AFEN_LP: 1;
        uint16_t RO_AON_XTAL_GATED_AFEP: 1;
        uint16_t RO_AON_XTAL_GATED_AFEP_LP: 1;
    };
} AON_FAST_ANAPAR_XTAL6_TYPE;

/* 0x14A0
    0       R   RO_AON_XTAL_GATED_DIGIN                         1'b0
    1       R   RO_AON_XTAL_GATED_DIGIP                         1'b0
    2       R   RO_AON_XTAL_GATED_DIGI_LP                       1'b0
    3       R   RO_AON_XTAL_GATED_LPS                           1'b0
    4       R   RO_AON_XTAL_GATED_RFN                           1'b0
    5       R   RO_AON_XTAL_GATED_RFP                           1'b0
    11:6    R/W XTAL_SEL_GM_N                                   6'b111111               XTAL_REG_MASK
    15:12   R/W XTAL_REG7X_DUMMY1                               4'b0000                 XTAL_REG_MASK
 */
typedef union _AON_FAST_ANAPAR_XTAL7_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_XTAL_GATED_DIGIN: 1;
        uint16_t RO_AON_XTAL_GATED_DIGIP: 1;
        uint16_t RO_AON_XTAL_GATED_DIGI_LP: 1;
        uint16_t RO_AON_XTAL_GATED_LPS: 1;
        uint16_t RO_AON_XTAL_GATED_RFN: 1;
        uint16_t RO_AON_XTAL_GATED_RFP: 1;
        uint16_t XTAL_SEL_GM_N: 6;
        uint16_t XTAL_REG7X_DUMMY1: 4;
    };
} AON_FAST_ANAPAR_XTAL7_TYPE;

/* 0x14A2
    1:0     R/W XTAL_REG8X_DUMMY2                               2'b00                   XTAL_REG_MASK
    7:2     R/W XTAL_SEL_GM_P                                   6'b111111               XTAL_REG_MASK
    13:8    R/W XTAL_SEL_GM_P_LP                                6'b111111               XTAL_REG_MASK
    14      R/W XTAL_EN_GM_SEPERATE                             1'b0                    XTAL_REG_MASK
    15      R/W XTAL_REG8X_DUMMY1                               1'b0                    XTAL_REG_MASK
 */
typedef union _AON_FAST_ANAPAR_XTAL8_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_REG8X_DUMMY2: 2;
        uint16_t XTAL_SEL_GM_P: 6;
        uint16_t XTAL_SEL_GM_P_LP: 6;
        uint16_t XTAL_EN_GM_SEPERATE: 1;
        uint16_t XTAL_REG8X_DUMMY1: 1;
    };
} AON_FAST_ANAPAR_XTAL8_TYPE;

/* 0x14A4
    2:0     R/W XTAL_REG9X_DUMMY1                               3'b000                  XTAL_REG_MASK
    4:3     R/W XTAL_SEL_LDOFASTSET_CYCLE                       2'b01                   XTAL_REG_MASK
    5       R/W XTAL_EN_LDOFASTSET_MANU                         1'b0                    XTAL_REG_MASK
    6       R/W XTAL_SEL_LDO_PC                                 1'b0                    XTAL_REG_MASK
    10:7    R/W XTAL_TUNE_LDO_OK_VOUT                           4'b0101                 XTAL_REG_MASK
    11      R   RO_AON_XTAL_EN_LPMODE_AUTO_GATED                1'b0
    12      R/W XTAL_EN_LPMODE_CLK_AON                          1'b0                    XTAL_REG_MASK
    15:13   R/W XTAL_SEL_LPS_DIV                                3'b110                  XTAL_REG_MASK
 */
typedef union _AON_FAST_ANAPAR_XTAL9_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_REG9X_DUMMY1: 3;
        uint16_t XTAL_SEL_LDOFASTSET_CYCLE: 2;
        uint16_t XTAL_EN_LDOFASTSET_MANU: 1;
        uint16_t XTAL_SEL_LDO_PC: 1;
        uint16_t XTAL_TUNE_LDO_OK_VOUT: 4;
        uint16_t RO_AON_XTAL_EN_LPMODE_AUTO_GATED: 1;
        uint16_t XTAL_EN_LPMODE_CLK_AON: 1;
        uint16_t XTAL_SEL_LPS_DIV: 3;
    };
} AON_FAST_ANAPAR_XTAL9_TYPE;

/* 0x14A6
    0       R/W XTAL_EN_LPS_STATE                               1'b0                    XTAL_REG_MASK
    1       R/W XTAL_LP_GATE_FW_CTRL_B                          1'b0                    XTAL_REG_MASK
    2       R/W XTAL_EN_NMDAAC                                  1'b0                    XTAL_REG_MASK
    3       R/W XTAL_EN_OVERLAPX2_SYS                           1'b0                    XTAL_REG_MASK
    4       R/W XTAL_EN_OVERLAP_SYS                             1'b0                    XTAL_REG_MASK
    5       R/W XTAL_EN_PDCK_OK_MANU                            1'b1                    XTAL_REG_MASK
    6       R/W XTAL_EN_PKDET_CMP_SWAP                          1'b0                    XTAL_REG_MASK
    7       R/W XTAL_EN_PKDET_LOAD_SWAP                         1'b0                    XTAL_REG_MASK
    10:8    R/W XTAL_SEL_TOK01                                  3'b100                  XTAL_REG_MASK
    13:11   R/W XTAL_SEL_TOK                                    3'b101                  XTAL_REG_MASK
    14      R   RO_AON_XTAL_EN_SUPPLY_MODE                      1'b1
    15      R/W XTAL_EN_SUPPLY_MODE_CTRLBYFASTAON_B             1'b1                    XTAL_REG_MASK
 */
typedef union _AON_FAST_ANAPAR_XTAL10_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_EN_LPS_STATE: 1;
        uint16_t XTAL_LP_GATE_FW_CTRL_B: 1;
        uint16_t XTAL_EN_NMDAAC: 1;
        uint16_t XTAL_EN_OVERLAPX2_SYS: 1;
        uint16_t XTAL_EN_OVERLAP_SYS: 1;
        uint16_t XTAL_EN_PDCK_OK_MANU: 1;
        uint16_t XTAL_EN_PKDET_CMP_SWAP: 1;
        uint16_t XTAL_EN_PKDET_LOAD_SWAP: 1;
        uint16_t XTAL_SEL_TOK01: 3;
        uint16_t XTAL_SEL_TOK: 3;
        uint16_t RO_AON_XTAL_EN_SUPPLY_MODE: 1;
        uint16_t XTAL_EN_SUPPLY_MODE_CTRLBYFASTAON_B: 1;
    };
} AON_FAST_ANAPAR_XTAL10_TYPE;

/* 0x14A8
    0       R/W XTAL_EN_SUPPLY_MODE_FASTAON                     1'b1                    XTAL_REG_MASK
    1       R/W XTAL_EN_LDO2PWRCUT                              1'b0                    XTAL_REG_MASK
    4:2     R/W XTAL_SEL_XORES                                  3'b000                  XTAL_REG_MASK
    5       R/W XTAL_EN_PDCK_MANU                               1'b0                    XTAL_REG_MASK
    6       R/W XTAL_EN_PLL_CP_ALWAYS_ON                        1'b0                    XTAL_REG_MASK
    8:7     R/W XTAL_SEL_CP_DISABLE_CYCLE                       2'b11                   XTAL_REG_MASK
    14:9    R/W XTAL_SEL_GM_N_LP                                6'b111111               XTAL_REG_MASK
    15      R/W EN_XTAL_AAC_PKDET                               1'b0                    XTAL_REG_MASK
 */
typedef union _AON_FAST_ANAPAR_XTAL11_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_EN_SUPPLY_MODE_FASTAON: 1;
        uint16_t XTAL_EN_LDO2PWRCUT: 1;
        uint16_t XTAL_SEL_XORES: 3;
        uint16_t XTAL_EN_PDCK_MANU: 1;
        uint16_t XTAL_EN_PLL_CP_ALWAYS_ON: 1;
        uint16_t XTAL_SEL_CP_DISABLE_CYCLE: 2;
        uint16_t XTAL_SEL_GM_N_LP: 6;
        uint16_t EN_XTAL_AAC_PKDET: 1;
    };
} AON_FAST_ANAPAR_XTAL11_TYPE;

/* 0x14AA
    3:0     R/W XTAL_TUNE_LDO_VOUT                              4'b0101                 XTAL_REG_MASK
    4       R/W XTAL_EN_HP_AAAC                                 1'b0                    XTAL_REG_MASK
    5       R/W XTAL_EN_SUPPLY_MODE_CTRL                        1'b1                    XTAL_REG_MASK
    6       R/W XTAL_EN_IBG_FROMPMU                             1'b1                    XTAL_REG_MASK
    15:7    R/W XTAL_REG12X_DUMMY1                              9'b000000000            XTAL_REG_MASK
 */
typedef union _AON_FAST_ANAPAR_XTAL12_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_TUNE_LDO_VOUT: 4;
        uint16_t XTAL_EN_HP_AAAC: 1;
        uint16_t XTAL_EN_SUPPLY_MODE_CTRL: 1;
        uint16_t XTAL_EN_IBG_FROMPMU: 1;
        uint16_t XTAL_REG12X_DUMMY1: 9;
    };
} AON_FAST_ANAPAR_XTAL12_TYPE;

/* 0x14AC
    15:0    R/W XTAL_REG13X_DUMMY1                              16'b0000000000000000    XTAL_REG_MASK
 */
typedef union _AON_FAST_ANAPAR_XTAL13_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_REG13X_DUMMY1;
    };
} AON_FAST_ANAPAR_XTAL13_TYPE;

/* 0x14AE
    15:0    R/W XTAL_REG14X_DUMMY1                              16'b0000000000000000    XTAL_REG_MASK
 */
typedef union _AON_FAST_ANAPAR_XTAL14_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_REG14X_DUMMY1;
    };
} AON_FAST_ANAPAR_XTAL14_TYPE;

/* 0x14B0
    15:0    R/W XTAL_REG15X_DUMMY1                              16'b0000000000000000    XTAL_REG_MASK
 */
typedef union _AON_FAST_ANAPAR_XTAL15_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t XTAL_REG15X_DUMMY1;
    };
} AON_FAST_ANAPAR_XTAL15_TYPE;

/* 0x14F0
    3:0     R/W OSC40_OSC_FSET                                  4'b1000                 OSC40_REG_MASK
    15:4    R/W OSC40_RG0X_DUMMY1                               12'h0                   OSC40_REG_MASK
 */
typedef union _AON_FAST_RG0X_40MOSC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t OSC40_OSC_FSET: 4;
        uint16_t OSC40_RG0X_DUMMY1: 12;
    };
} AON_FAST_RG0X_40MOSC_TYPE;

/* 0x14F2
    0       R   RO_AON_OSC40_POW_OSC                            1'b0
    15:1    R/W OSC40_POW_40MOSC_DUMMY1                         15'h0                   OSC40_REG_MASK
 */
typedef union _AON_FAST_POW_40MOSC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_OSC40_POW_OSC: 1;
        uint16_t OSC40_POW_40MOSC_DUMMY1: 15;
    };
} AON_FAST_POW_40MOSC_TYPE;

/* 0x1510
    0       R/W AUXADC_POW_ADC                                  1'b0                    AUXADC_REG_MASK
    1       R/W AUXADC_POW_REF                                  1'b0                    AUXADC_REG_MASK
    2       R/W AUXADC_SEL_CLK                                  1'b0                    AUXADC_REG_MASK
    3       R/W AUXADC_EN_CLK_DELAY                             1'b0                    AUXADC_REG_MASK
    5:4     R/W AUXADC_SEL_VREF                                 2'b01                   AUXADC_REG_MASK
    6       R/W AUXADC_EN_CLK_REVERSE                           1'b0                    AUXADC_REG_MASK
    8:7     R/W AUXADC_SEL_CMPDEC                               2'b00                   AUXADC_REG_MASK
    9       R/W AUXADC_EN_META                                  1'b0                    AUXADC_REG_MASK
    10      R/W AUXADC_EN_LN                                    1'b0                    AUXADC_REG_MASK
    11      R/W AUXADC_EN_LNA                                   1'b0                    AUXADC_REG_MASK
    13:12   R/W AUXADC_VCM_SEL                                  2'b11                   AUXADC_REG_MASK
    15:14   R/W AUXADC_RG0X_DUMMY                               2'b00                   AUXADC_REG_MASK
 */
typedef union _AON_FAST_RG0X_AUXADC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AUXADC_POW_ADC: 1;
        uint16_t AUXADC_POW_REF: 1;
        uint16_t AUXADC_SEL_CLK: 1;
        uint16_t AUXADC_EN_CLK_DELAY: 1;
        uint16_t AUXADC_SEL_VREF: 2;
        uint16_t AUXADC_EN_CLK_REVERSE: 1;
        uint16_t AUXADC_SEL_CMPDEC: 2;
        uint16_t AUXADC_EN_META: 1;
        uint16_t AUXADC_EN_LN: 1;
        uint16_t AUXADC_EN_LNA: 1;
        uint16_t AUXADC_VCM_SEL: 2;
        uint16_t AUXADC_RG0X_DUMMY: 2;
    };
} AON_FAST_RG0X_AUXADC_TYPE;

/* 0x1512
    7:0     R/W AUXADC_EN_BYPASS_MODE                           8'b00000000             AUXADC_REG_MASK
    8       R/W AUXADC_EN_TG                                    1'b0                    AUXADC_REG_MASK
    10:9    R/W AUXADC_SEL_LDO08_REF                            2'b01                   AUXADC_REG_MASK
    11      R/W AUXADC_EN_LDO_VPULSE                            1'b0                    AUXADC_REG_MASK
    12      R/W AUXADC_SEL_LDO_MODE                             1'b1                    AUXADC_REG_MASK
    15:13   R/W AUXADC_RG1X_DUMMY                               3'b000                  AUXADC_REG_MASK
 */
typedef union _AON_FAST_RG1X_AUXADC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AUXADC_EN_BYPASS_MODE: 8;
        uint16_t AUXADC_EN_TG: 1;
        uint16_t AUXADC_SEL_LDO08_REF: 2;
        uint16_t AUXADC_EN_LDO_VPULSE: 1;
        uint16_t AUXADC_SEL_LDO_MODE: 1;
        uint16_t AUXADC_RG1X_DUMMY: 3;
    };
} AON_FAST_RG1X_AUXADC_TYPE;

/* 0x1514
    0       R/W AUXADC_EN_LDO33_PC                              1'b0                    AUXADC_REG_MASK
    2:1     R/W AUXADC_SEL_LDO33_REF                            2'b10                   AUXADC_REG_MASK
    3       R/W AUXADC_EN_LDO08_PC                              1'b0                    AUXADC_REG_MASK
    4       R/W POW_SD                                          1'b1                    AUXADC_REG_MASK
    5       R/W SD_POSEDGE_H                                    1'b0                    AUXADC_REG_MASK
    8:6     R/W ch_num_sd<2:0>                                  3'b000                  AUXADC_REG_MASK
    15:9    R/W AUXADC_RG2X_DUMMY1                              7'b0000000              AUXADC_REG_MASK
 */
typedef union _AON_FAST_RG2X_AUXADC_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t AUXADC_EN_LDO33_PC: 1;
        uint16_t AUXADC_SEL_LDO33_REF: 2;
        uint16_t AUXADC_EN_LDO08_PC: 1;
        uint16_t POW_SD: 1;
        uint16_t SD_POSEDGE_H: 1;
        uint16_t ch_num_sd_2_0: 3;
        uint16_t AUXADC_RG2X_DUMMY1: 7;
    };
} AON_FAST_RG2X_AUXADC_TYPE;

/* 0x1530
    3:0     R/W CODEC_LDO_SEL_LDO_LV_DL                         4'b0000                 CODEC_REG_MASK
    4       R/W CODEC_LDO_RG0X_DUMMY2                           1'b0                    CODEC_REG_MASK
    8:5     R/W CODEC_LDO_VCM_0V9_TUNE                          4'b1000                 CODEC_REG_MASK
    9       R/W CODEC_LDO_EN_PRE_CHARGE                         1'b0                    CODEC_REG_MASK
    10      R/W CODEC_LDO_SEL_LDO_AVCC_PRECHG                   1'b0                    CODEC_REG_MASK
    11      R/W CODEC_LDO_POW_LDO_AVCC                          1'b0                    CODEC_REG_MASK
    12      R/W CODEC_LDO_EN_LDO_AVCC_DISCHARGE                 1'b0                    CODEC_REG_MASK
    13      R/W CODEC_LDO_RG0X_DUMMY1                           1'b0                    CODEC_REG_MASK
    14      R/W CODEC_LDO_EN_LDO_AVCC_INT_COMP                  1'b0                    CODEC_REG_MASK
    15      R/W CODEC_LDO_POW_LDO_LV                            1'b0                    CODEC_REG_MASK
 */
typedef union _AON_FAST_RG0X_CODEC_LDO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CODEC_LDO_SEL_LDO_LV_DL: 4;
        uint16_t CODEC_LDO_RG0X_DUMMY2: 1;
        uint16_t CODEC_LDO_VCM_0V9_TUNE: 4;
        uint16_t CODEC_LDO_EN_PRE_CHARGE: 1;
        uint16_t CODEC_LDO_SEL_LDO_AVCC_PRECHG: 1;
        uint16_t CODEC_LDO_POW_LDO_AVCC: 1;
        uint16_t CODEC_LDO_EN_LDO_AVCC_DISCHARGE: 1;
        uint16_t CODEC_LDO_RG0X_DUMMY1: 1;
        uint16_t CODEC_LDO_EN_LDO_AVCC_INT_COMP: 1;
        uint16_t CODEC_LDO_POW_LDO_LV: 1;
    };
} AON_FAST_RG0X_CODEC_LDO_TYPE;

/* 0x1532
    14:0    R/W CODEC_LDO_RG1X_DUMMY1                           15'b000000000000000     CODEC_REG_MASK
    15      R/W CODEC_LDO_ISO_LDO_LV_B                          1'b0                    CODEC_REG_MASK
 */
typedef union _AON_FAST_RG1X_CODEC_LDO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CODEC_LDO_RG1X_DUMMY1: 15;
        uint16_t CODEC_LDO_ISO_LDO_LV_B: 1;
    };
} AON_FAST_RG1X_CODEC_LDO_TYPE;

/* 0x1534
    0       R   RO_AON_CODEC_LDO_POW_LDO_DRV                    1'b0
    1       R   RO_AON_CODEC_LDO_EN_LDO_DRV_PC                  1'b0
    2       R/W CODEC_LDO_EN_LDO_DRV_FB_LOCAL_B                 1'b0                    CODEC_REG_MASK
    9:3     R   RO_AON_CODEC_LDO_TUNE_LDO_DRV_VOUT              7'b1011010
    10      R/W CODEC_LDO_EN_LDO_DRV_PGTIELOW_R                 1'b1                    CODEC_REG_MASK
    13:11   R/W CODEC_LDO_TUNE_LDO_DRV_VREF                     3'b111                  CODEC_REG_MASK
    14      R   RO_AON_CODEC_LDO_EN_LDO_DRV_VO_PD               1'b0
    15      R   RO_AON_CODEC_LDO_POW_LDO_DRV_VREF               1'b0
 */
typedef union _AON_FAST_RG2X_CODEC_LDO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_CODEC_LDO_POW_LDO_DRV: 1;
        uint16_t RO_AON_CODEC_LDO_EN_LDO_DRV_PC: 1;
        uint16_t CODEC_LDO_EN_LDO_DRV_FB_LOCAL_B: 1;
        uint16_t RO_AON_CODEC_LDO_TUNE_LDO_DRV_VOUT: 7;
        uint16_t CODEC_LDO_EN_LDO_DRV_PGTIELOW_R: 1;
        uint16_t CODEC_LDO_TUNE_LDO_DRV_VREF: 3;
        uint16_t RO_AON_CODEC_LDO_EN_LDO_DRV_VO_PD: 1;
        uint16_t RO_AON_CODEC_LDO_POW_LDO_DRV_VREF: 1;
    };
} AON_FAST_RG2X_CODEC_LDO_TYPE;

/* 0x1536
    2:0     R/W CODEC_LDO_SEL_LDO_DRV_DIG_VREF                  3'b000                  CODEC_REG_MASK
    15:3    R/W CODEC_LDO_RG3X_DUMMY1                           13'b0000000000000       CODEC_REG_MASK
 */
typedef union _AON_FAST_RG3X_CODEC_LDO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t CODEC_LDO_SEL_LDO_DRV_DIG_VREF: 3;
        uint16_t CODEC_LDO_RG3X_DUMMY1: 13;
    };
} AON_FAST_RG3X_CODEC_LDO_TYPE;

/* 0x1540
    7:0     R/W BTADDA_LDO_REG0X_DUMMY1                         8'h0
    8       R/W BTADDA_LDO_EN_LDO_PC                            1'b0
    9       R/W BTADDA_LDO_POW_LDO_OP                           1'b1
    10      R/W BTADDA_LDO_EN_LDO_VPULSE                        1'b0
    14:11   R/W BTADDA_LDO_TUNE_LDO_VOUT                        4'b0100
    15      R/W BTADDA_LDO_DOUBLE_OP_I                          1'b0
 */
typedef union _AON_FAST_REG0X_BTADDA_LDO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t BTADDA_LDO_REG0X_DUMMY1: 8;
        uint16_t BTADDA_LDO_EN_LDO_PC: 1;
        uint16_t BTADDA_LDO_POW_LDO_OP: 1;
        uint16_t BTADDA_LDO_EN_LDO_VPULSE: 1;
        uint16_t BTADDA_LDO_TUNE_LDO_VOUT: 4;
        uint16_t BTADDA_LDO_DOUBLE_OP_I: 1;
    };
} AON_FAST_REG0X_BTADDA_LDO_TYPE;

/* 0x1550
    0       R   RO_AON_LDO_PA_POW_LDO                           1'b0
    1       R   RO_AON_LDO_PA_EN_PC                             1'b0
    2       R/W LDO_PA_EN_FB_LOCAL_B                            1'b0                    LDO_PA_REG_MASK
    9:3     R   RO_AON_LDO_PA_TUNE_LDO_VOUT                     7'b1011010
    10      R/W LDO_PA_DUMMY1                                   1'b1                    LDO_PA_REG_MASK
    13:11   R/W LDO_PA_TUNE_VREF                                3'b111                  LDO_PA_REG_MASK
    14      R   RO_AON_LDO_PA_EN_VO_PD                          1'b0
    15      R   RO_AON_LDO_PA_POW_VREF                          1'b0
 */
typedef union _AON_FAST_LDOPA_REG3X_LDO_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t RO_AON_LDO_PA_POW_LDO: 1;
        uint16_t RO_AON_LDO_PA_EN_PC: 1;
        uint16_t LDO_PA_EN_FB_LOCAL_B: 1;
        uint16_t RO_AON_LDO_PA_TUNE_LDO_VOUT: 7;
        uint16_t LDO_PA_DUMMY1: 1;
        uint16_t LDO_PA_TUNE_VREF: 3;
        uint16_t RO_AON_LDO_PA_EN_VO_PD: 1;
        uint16_t RO_AON_LDO_PA_POW_VREF: 1;
    };
} AON_FAST_LDOPA_REG3X_LDO_TYPE;

/* 0x1552
    15:0    R/W LDO_PA_DIG_REG_DUMMY                            16'b0000000000000000    LDO_PA_REG_MASK
 */
typedef union _AON_FAST_LDOPA_DIG_VRSEL_LDODIG_TYPE
{
    uint16_t d16;
    struct
    {
        uint16_t LDO_PA_DIG_REG_DUMMY;
    };
} AON_FAST_LDOPA_DIG_VRSEL_LDODIG_TYPE;

#endif

