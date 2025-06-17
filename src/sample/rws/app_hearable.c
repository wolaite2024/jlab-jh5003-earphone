#if F_APP_HEARABLE_SUPPORT
#include <stdbool.h>
#include <string.h>
#include "stdlib.h"
#include "math.h"
#include "ftl.h"
#include "fmc_api.h"
#include "bt_hfp.h"
#include "audio.h"
#include "audio_probe.h"
#include "audio_passthrough.h"
#include "audio_vse.h"
#include "app_anc.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_timer.h"
#include "app_cmd.h"
#include "app_sensor.h"
#include "app_hearable.h"
#include "app_a2dp.h"
#include "app_hfp.h"
#include "ha_ext_ftl.h"
#include "app_audio_passthrough.h"
#include "app_audio_policy.h"
#include "app_audio_hearing.h"
#include "app_listening_mode.h"
#include "app_dsp_cfg.h"
#include "pm.h"

#define HA_BLOCK                            256
#define HA_PROG_OPTION                      4
#define HA_PROG_MAX_NUM                     16
#define HA_PROG_LIST_HEADER_LEN             36
#define HA_PROG_MAX_TONE_NUM                10
#define HA_VERSION                          9

#define HA_EFFECT_TO_PROG_OBJ(effect)       (effect + HA_PROG_OBJ_WDRC)
#define HA_PROG_OBJ_TO_EFFECT(obj)          (T_APP_HA_EFFECT)(obj - HA_PROG_OBJ_WDRC)

#define HA_GLOBAL_FEATURE_NUM               14
#define HA_PARAMS_FEATURE_NUM               21

#define HA_AUDIO_PARAMS_FEATURE_NUM         3
#define HA_VOICE_PARAMS_FEATURE_NUM         3
#define HA_AUDIO_PARAMS_TOTAL_NUM           3
#define HA_VOICE_PARAMS_TOTAL_NUM           3

#define HA_PARAMS_RNS_SIZE                              260
#define HA_PARAMS_SPEECH_ENHANCE_MAX_SIZE               1024
#define HA_PARAMS_MIC_SETTING_MAX_SIZE                  1024
#define HA_PARAMS_SPK_COMPENSATION_MAX_SIZE             1024
#define HA_PARAMS_MIC_COMPENSATION_MAX_SIZE             1024
#define HA_PARAMS_MMI_VOLUME_STEP_SIZE                  20
#define HA_PARAMS_WDRC_SIZE                             872
#define HA_PARAMS_WDRC_ENABLED_SIZE                     4
#define HA_PARAMS_HA_TEST_SIZE                          4
#define HA_PARAMS_HA_ENV_CLASSIFICATION_MAX_SIZE        16
#define HA_PARAMS_HA_BAND_WIDTH_SIZE                    4
#define HA_PARAMS_HA_MAX_GAIN_SETTING_MAX_SIZE          1024
#define HA_PARAMS_HA_FBC_SETTING_MAX_SIZE               1024
#define HA_PARAMS_HA_DC_REMOVER_MAX_SIZE                1024
#define HA_PARAMS_HA_CUSTOMER_EQ_MAX_SIZE               1024
#define HA_PARAMS_HA_RNS_COMPENSATION_MAX_SIZE          1024
#define HA_PARAMS_HA_DBFS_SIZE                          24
#define HA_PARAMS_HA_PARAMS_BYPASS_HA_PROCESSING_SIZE   8
#define HA_PARAMS_HA_PARAMS_DEHOWLING_PARA_SIZE         8
#define HA_PARAMS_HA_PARAMS_MIC_SWAP_SIZE               8
#define HA_PARAMS_HA_PARAMS_APT_MIC_GAIN_SIZE           24

#define HA_GLOBAL_OBJ_HA_MODE_SIZE              4
#define HA_GLOBAL_OBJ_RHA_WDRC_MAX_SIZE         1024
#define HA_GLOBAL_OBJ_ULLRHA_WDRC_MAX_SIZE      1024
#define HA_GLOBAL_OBJ_AUDIO_WDRC_MAX_SIZE       1024
#define HA_GLOBAL_OBJ_VOICE_WDRC_MAX_SIZE       1024
#define HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS_SIZE   16
#define HA_GLOBAL_OBJ_OVP_TRAINING_DATA_SIZE    32
#define HA_GLOBAL_OBJ_RHA_WDRC_ENABLED_SIZE     12
#define HA_GLOBAL_OBJ_ULLRHA_WDRC_ENABLED_SIZE  12
#define HA_GLOBAL_OBJ_PRESET_IDX_SIZE           4
#define HA_GLOBAL_OBJ_AUDIO_WDRC_ENABLE_SIZE    12
#define HA_GLOBAL_OBJ_VOICE_WDRC_ENABLE_SIZE    12
#define HA_GLOBAL_OBJ_AUDIO_MIC_SETTINGS_SIZE   16
#define HA_GLOBAL_OBJ_VOICE_MIC_SETTINGS_SIZE   16

#define HA_DSP_EXT_PARA_OFFSET                  0x24
#define HA_DSP_EXT_PARA2_OFFSET                 0x28
#define HA_DSP_FEATURE_BITS_OFFSET              0x2C
#define HA_DSP_EXT_PARA_SYNC_WORD               0xAA55
#define HA_DSP_FEATURE_HA_PROG                  0x01
#define HA_DSP_FEATURE_HA_SPK_RESPONSE          0x02
#define HA_DSP_FEATURE_HA_DSP_CFG_GAIN          0x03
#define HA_DSP_FEATURE_HA_RNS_PARAMS            0x04
#define HA_DSP_FEATURE_HA_SPK_COMPENSATION      0x05
#define HA_DSP_FEATURE_HA_MIC_COMPENSATION      0x06
#define HA_DSP_FEATURE_HA_RESERVED              0x07
#define HA_DSP_FEATURE_HA_SPEECH_ENHANCE        0x08
#define HA_DSP_FEATURE_HA_MMI_VOLUME_STEP       0x09
#define HA_DSP_FEATURE_HA_WDRC_PARAMS           0x0A
#define HA_DSP_FEATURE_HA_WDRC_ENABLED          0x0B
#define HA_DSP_FEATURE_HA_TEST                  0x0C
#define HA_DSP_FEATURE_HA_MIC_SETTING           0x0D
#define HA_DSP_FEATURE_HA_AUDIO_PARA            0x0E
#define HA_DSP_FEATURE_HA_VOICE_PARA            0x0F

#define HA_DSP_FEATURE_HA_BAND_WIDTH            0x10
#define HA_DSP_FEATURE_HA_MAX_GAIN_SETTING      0x11
#define HA_DSP_FEATURE_HA_FBC_SETTING           0x12
#define HA_DSP_FEATURE_HA_DC_REMOVER            0x13
#define HA_DSP_FEATURE_HA_CUSTOMER_EQ           0x14
#define HA_DSP_FEATURE_HA_RNS_COMPENSATION      0x15
#define HA_DSP_FEATURE_HA_DBFS                  0x16
#define HA_DSP_FEATURE_HA_DEHOWLING_PARA        0x17
#define HA_DSP_FEATURE_HA_MIC_SWAP              0x18
#define HA_DSP_FEATURE_HA_APT_MIC_GAIN          0x19
#define HA_DSP_FEATURE_HA_BYPASS_HA_PROCESSING  0x1A

#define HA_DSP_FEATURE_HA_ENV_CLASSIFICATION    0x20
#define HA_DSP_FEATURE_DEFAULT                  0xFF

#define HA_DSP_FEATURE_HA_AUDIO_PARA_WDRC           0x18 //type 24
#define HA_DSP_FEATURE_HA_AUDIO_PARA_WDRC_ENABLED   0x19 //type 25
#define HA_DSP_FEATURE_HA_AUDIO_PARA_MIC_SETTINGS   0x1B //type 27

#define HA_DSP_FEATURE_HA_VOICE_PARA_WDRC           0x18 //type 24
#define HA_DSP_FEATURE_HA_VOICE_PARA_WDRC_ENABLED   0x19 //type 25
#define HA_DSP_FEATURE_HA_VOICE_PARA_MIC_SETTINGS   0x1B //type 27

#define HA_LISTENING_DELAY_TIME             2000
#define HA_AUDIO_VOLUME_REFRESH_INTERVAL    100

#define HA_INFO_PRIMARY                     0
#define HA_INFO_SECONDARY                   1

#define HA_SCALE_UP_GAIN_DB                 64

#define HA_HEARING_TEST_SAMPLE_RATE             48000
#define HA_HEARING_TEST_SAMPLE_DEPTH            24
#define HA_HEARING_TEST_SAMPLE_DEPTH_IN_BYTES   3
#define HA_HEARING_TEST_SAMPLE_DEPTH_MAX_VAL    8388607 //pow(2,24-1)-1
#define HA_HEARING_TEST_FRAME_LEN               960
#define HA_HEARING_TEST_FRAME_SEND_DELAY_TIME   15
#define HA_HEARING_TEST_TONE_TABLE_MAX_LEN      384 //125Hz, 48*8
#define HA_HEARING_TEST_TONE_PHASE              4 //fade in, duration, fade_out, interval
#define HA_HEARING_TEST_TONE_FADE_IN_OUT_MS     40
#define HA_HEARING_TEST_TONE_DURATION_MS        216
#define HA_HEARING_TEST_TONE_INTERVAL_MS        204
#define HA_HEARING_TEST_TONE_DEFAULT_FREQ       1000
#define HA_HEARING_TEST_TONE_DEFAULT_VOLUME     15
#define HA_HEARING_TEST_TONE_DEFAULT_GAIN       -70
#define HA_HEARING_TEST_TONE_MAX_GAIN           -40

#define HA_CMD_TYPE_WDRC_0                  0
#define HA_CMD_TYPE_NR                      4
#define HA_CMD_TYPE_SPEECH_ENHANCE_ENABLED  5
#define HA_CMD_TYPE_BF                      6
#define HA_CMD_TYPE_GRAPHIC_EQ              7
#define HA_CMD_TYPE_HA_TEST                 8
#define HA_CMD_TYPE_DEHOWLING               9
#define HA_CMD_TYPE_HA_OUTPUT_LIMITER       10
#define HA_CMD_TYPE_WNR                     11
#define HA_CMD_TYPE_INR                     12
#define HA_CMD_TYPE_FBC                     13
#define HA_CMD_TYPE_RNS_ENABLED             14
#define HA_CMD_TYPE_RNS                     15
#define HA_CMD_TYPE_HA_VOLUME               16
#define HA_CMD_TYPE_MIC_COMPENSATION        17
#define HA_CMD_TYPE_SPK_COMPENSATION        18
#define HA_CMD_TYPE_SPEECH_ENHANCE          19
#define HA_CMD_TYPE_WDRC                    24
#define HA_CMD_TYPE_WDRC_ENABLED            25
#define HA_CMD_TYPE_BAND_WIDTH              26
#define HA_CMD_TYPE_MIC_SETTING             27
#define HA_CMD_TYPE_DC_REMOVER              34
#define HA_CMD_TYPE_BYPASS_HA_PROCESSING    35
#define HA_CMD_TYPE_MIC_SWAP                36
#define HA_CMD_TYPE_DSP_GAIN                37
#define HA_CMD_TYPE_ADAPTIVE_BF             64
#define HA_CMD_TYPE_OVP_ENABLED             65
#define HA_CMD_TYPE_OVP_TRAINING_CTRL       66
#define HA_CMD_TYPE_OVP_PARAMS              67
#define HA_CMD_TYPE_ENV_CLASSIFICATION      68

#define HA_LTV_DATA_HEADER_SIZE                 4 //len, data_type, version, ctrl
#define HA_LTV_DATA_MAX_SIZE                    252 //256 - length(1) - data_type(1) - version(1) - ctrl(1)
#define HA_LTV_DATA_START_TYPE_HEADER_SIZE      6 //len, data_type, version, ctrl, total_payload_len (2byts)
#define HA_LTV_DATA_START_TYPE_SIZE             250 //256 - length(1) - data_type(1) - version(1) - ctrl(1) - total_payload_len(2)

#define HA_MODE_PARAMS_BUF_SIZE                 1024

#define HA_INVALID_OFFSET                       0xFFFFFFFF

#define HA_PRESET_NOT_CONFIGURED                0xFD
#define HA_PRESET_HEARING_COMPENSATION          0xFE
#define HA_PRESET_INVALID                       0xFF

#define HA_PRESET_SET_FAILED                    0
#define HA_PRESET_SET_SUCCESS                   1

#define DEVICE_BUD_SIDE_BOTH                    2

#define GROUP_LTV_DATA_MAX_SIZE                 2048

typedef struct t_app_ha_dsp_ext_para
{
    uint32_t spk_channel                    : 1;
    uint32_t mic_channel                    : 1;
    uint32_t i2s_ext                        : 1;
    uint32_t spdif_enb                      : 1;
    uint32_t apt_enb                        : 1;
    uint32_t sound_effect                   : 1;
    uint32_t dac_output_mode                : 1;
    uint32_t sdk                            : 1;
    uint32_t opus_celt_encoder3             : 1;
    uint32_t record_mode_voice_parameter    : 1;
    uint32_t ha_enable                      : 1;
    uint32_t ha_scenario                    : 1;
    uint32_t ullrha_enable                  : 1;
    uint32_t resvbit                        : 19;
} T_APP_HA_DSP_EXT_PARA;

typedef enum
{
    APP_TIMER_HA_LISTENING_DELAY,
    APP_TIMER_HA_APT_VOL_LEVEL,
    APP_TIMER_HA_HEARING_TEST_TONE,
} T_HA_TIMER;

typedef enum
{
    APP_HA_CMD_MSG_SET_EFFECT_CMD       = 0x00,
    APP_HA_DATA_MSG_SET_PARAMS          = 0x01,
    APP_HA_HEARING_TEST_MSG_SET         = 0x02,
    APP_HA_GLOBAL_OBJ_MSG_SET           = 0X03,
    APP_HA_BANDWIDTH_EQ_PARAMS_GET      = 0X04,
    APP_HA_CUSTOMER_EQ_PARAMS_GET       = 0X05,
    APP_HA_RNS_COMPENSATION_PARAMS_GET  = 0X06,
    APP_HA_TOOL_DATA_PARAMS_GET         = 0x07,
    APP_HA_MIC0_DBFS_DBSPL_PARAMS_GET   = 0x08,
    APP_HA_CMD_MSG_SET_ALL_PARAMS       = 0x09,
    APP_HA_DATA_MSG_SET_ALL_PARAMS      = 0x0A,
    APP_HA_SET_PROG_ALL_OBJS            = 0x0B,
    APP_HA_CMD_MSG_TOTAL,
} T_APP_HA_CMD_MSG;

typedef enum
{
    HA_EFFECT_WDRC                          = 0x00,
    HA_EFFECT_NR                            = 0x01,
    HA_EFFECT_GRAPHIC_EQ                    = 0x02,
    HA_EFFECT_FBC                           = 0x03,
    HA_EFFECT_SPEECH_ENHANCE                = 0x04,
    HA_EFFECT_BF                            = 0x05,
    HA_EFFECT_WNR                           = 0x06,
    HA_EFFECT_INR                           = 0x07,
    HA_EFFECT_RNS                           = 0x08,
    HA_EFFECT_OUTPUT_DRC                    = 0x09,
    HA_EFFECT_ADAPTIVE_BF                   = 0x0A,
    HA_EFFECT_OVP                           = 0X0B,
    HA_EFFECT_INTEGRATED_EQ                 = 0X0C,
    HA_EFFECT_WDRC_ENABLED                  = 0x0D,
    HA_EFFECT_MAX_TOTAL_POWER_OUTPUT_CTRL   = 0x0E,
    HA_EFFECT_TOTAL                         = 0x0F,
} T_APP_HA_EFFECT;

typedef enum
{
    HA_EFFECT_WDRC_SIZE                         = 432,
    HA_EFFECT_NR_SIZE                           = 12,
    HA_EFFECT_GRAPHIC_EQ_SIZE                   = 140,
    HA_EFFECT_FBC_SIZE                          = 20,
    HA_EFFECT_SPEECH_ENHANCE_SIZE               = 12,
    HA_EFFECT_BF_SIZE                           = 12,
    HA_EFFECT_WNR_SIZE                          = 12,
    HA_EFFECT_INR_SIZE                          = 12,
    HA_EFFECT_RNS_SIZE                          = 12,
    HA_EFFECT_OUTPUT_DRC_SIZE                   = 24,
    HA_EFFECT_ADAPTIVE_BF_SIZE                  = 16,
    HA_EFFECT_OVP_SIZE                          = 12,
    HA_EFFECT_INTEGRATED_EQ_SIZE                = 1024,
    HA_EFFECT_WDRC_ENABLED_SIZE                 = 12,
    HA_EFFECT_MAX_TOTAL_POWER_OUTPUT_CTRL_SIZE  = 20,
} T_APP_HA_EFFECT_SIZE;

typedef enum
{
    HA_PARAMS_RNS                   = 0x00,
    HA_PARAMS_SPEECH_ENHANCE        = 0x01,
    HA_PARAMS_MMI_VOLUME_STEP       = 0x02,
    HA_PARAMS_WDRC                  = 0x03,
    HA_PARAMS_WDRC_ENABLED          = 0x04,
    HA_PARAMS_HA_TEST               = 0x05,
    HA_PARAMS_MIC_SETTING           = 0X06,
    HA_PARAMS_SPK_COMPENSATION      = 0X07,
    HA_PARAMS_MIC_COMPENSATION      = 0X08,
    HA_PARAMS_ENV_CLASSIFICATION    = 0x09,
    HA_PARAMS_BAND_WIDTH            = 0X0A,
    HA_PARAMS_MAX_GAIN_SETTING      = 0X0B,
    HA_PARAMS_FBC_SETTING           = 0X0C,
    HA_PARAMS_DC_REMOVER            = 0X0D,
    HA_PARAMS_CUSTOMER_EQ           = 0X0E,
    HA_PARAMS_RNS_COMPENSATION      = 0X0F,
    HA_PARAMS_HA_DBFS               = 0x10,
    HA_PARAMS_DEHOWLING_PARA        = 0X11,
    HA_PARAMS_MIC_SWAP              = 0X12,
    HA_PARAMS_APT_MIC_GAIN          = 0X13,
    HA_PARAMS_BYPASS_HA_PROCESSING  = 0X14,
    HA_PARAMS_TOTAL                 = 0x15,
} T_APP_HA_PARAMS;

typedef enum
{
    HA_AUDIO_PARAMS_WDRC            = 0X00,
    HA_AUDIO_PARAMS_WDRC_ENABLED    = 0x01,
    HA_AUDIO_PARAMS_MIC_SETTINGS    = 0X02,
} T_APP_HA_AUDIO_PARAMS;

typedef enum
{
    HA_VOICE_PARAMS_WDRC            = 0X00,
    HA_VOICE_PARAMS_WDRC_ENABLED    = 0x01,
    HA_VOICE_PARAMS_MIC_SETTINGS    = 0X02,
} T_APP_HA_VOICE_PARAMS;

typedef struct t_app_ha_effect_item
{
    struct t_app_ha_effect_item     *p_next;
    T_APP_HA_EFFECT                 effect;
    void                            *info_buf;
    uint16_t                        info_len;
    bool                            enabled;
} T_APP_HA_EFFECT_ITEM;

typedef enum
{
    APP_REMOTE_MSG_HA_PROGRAM_SET_ID                                = 0x00,
    APP_REMOTE_MSG_HA_PROGRAM_SET_OBJ                               = 0x01,
    APP_REMOTE_MSG_HA_PROGRAM_SET_ALL_OBJ                           = 0x02,
    APP_REMOTE_MSG_HA_PROGRAM_GET_SPK_RESPONSE                      = 0x03,
    APP_REMOTE_MSG_HA_PROGRAM_RELAY_SPK_RESPONSE                    = 0x04,
    APP_REMOTE_MSG_HA_PROGRAM_GET_AUDIO_VOLUME                      = 0x05,
    APP_REMOTE_MSG_HA_PROGRAM_RELAY_AUDIO_VOLUME                    = 0x06,
    APP_REMOTE_MSG_HA_PROGRAM_RESET_OBJ                             = 0x07,
    APP_REMOTE_MSG_HA_PROGRAM_GET_ALL_DIRECTION_OBJ                 = 0x08,
    APP_REMOTE_MSG_HA_PROGRAM_SET_HA_VOLUME                         = 0x09,
    APP_REMOTE_MSG_HA_PROGRAM_SET_HA_VOLUME_SYNC_STATUS             = 0x0A,
    APP_REMOTE_MSG_HA_PROGRAM_SET_HA_NR                             = 0x0B,
    APP_REMOTE_MSG_HA_PROGRAM_SET_HA_SPEECH_ENHANCE                 = 0x0C,
    APP_REMOTE_MSG_HA_PROGRAM_GET_HA_VOL                            = 0x0D,
    APP_REMOTE_MSG_HA_PROGRAM_SET_HA_VOL                            = 0x0E,
    APP_REMOTE_MSG_HA_PROGRAM_GET_HA_BF                             = 0x0F,
    APP_REMOTE_MSG_HA_PROGRAM_SET_HA_BF                             = 0x10,
    APP_REMOTE_MSG_HA_PROGRAM_STOP_A2DP_AUDIO_TRACK                 = 0x11,
    APP_REMOTE_MSG_HA_PROGRAM_DISABLE_AUDIO_EQ_START_HEARING_TEST   = 0X12,
    APP_REMOTE_MSG_HA_PROGRAM_RELAY_HEARING_TEST_STATUS             = 0X13,
    APP_REMOTE_MSG_HA_PROGRAM_RELAY_STOP_HEARING_TEST               = 0X14,
    APP_REMOTE_MSG_HA_PROGRAM_RELAY_HA_VOLUME_STATUS                = 0X15,
    APP_REMOTE_MSG_HA_PROGRAM_SET_HA_MODE                           = 0x16,
    APP_REMOTE_MSG_HA_CTRL_OVP_TRAINING                             = 0X17,
    APP_REMOTE_MSG_HA_PROGRAM_RELAY_OVP_TRAINING_STATUS             = 0X18,
    APP_REMOTE_MSG_HA_CTRL_OVP_TRAINING_RESET_PARAMS                = 0X19,
    APP_REMOTE_MSG_HA_SET_CLASSIFY                                  = 0X1A,
    APP_REMOTE_MSG_HA_GLOBAL_GET_ALL_DIRECTION_OBJ                  = 0X1B,
    APP_REMOTE_MSG_HA_SET_PRESET_IDX                                = 0X1C,

    APP_REMOTE_MSG_HA_TOTAL                                         = 0x1D,
} T_HA_REMOTE_MSG;

typedef enum
{
    HA_PROG_OBJ_SUSTAIN                     = 0x00,
    HA_PROG_OBJ_SELECTABLE                  = 0x01,
    HA_PROG_OBJ_NAME                        = 0x02,
    HA_PROG_OBJ_TOOL_DATA                   = 0x03,
    HA_PROG_OBJ_VOLUME_LEVEL                = 0x04,
    HA_PROG_OBJ_WDRC_PRESET_IDX             = 0x05,
    HA_PROG_OBJ_CTRL_TOTAL                  = 0x06,

    HA_PROG_OBJ_WDRC                        = 0x20,
    HA_PROG_OBJ_NR                          = 0x21,
    HA_PROG_OBJ_GRAPHIC_EQ                  = 0x22,
    HA_PROG_OBJ_FBC                         = 0x23,
    HA_PROG_OBJ_SPEECH_ENHANCE              = 0x24,
    HA_PROG_OBJ_BF                          = 0X25,
    HA_PROG_OBJ_WNR                         = 0X26,
    HA_PROG_OBJ_INR                         = 0X27,
    HA_PROG_OBJ_RNS                         = 0X28,
    HA_PROG_OBJ_OUTPUT_DRC                  = 0X29,
    HA_PROG_OBJ_ADAPTIVE_BF                 = 0X2A,
    HA_PROG_OBJ_OVP                         = 0X2B,
    HA_PROG_OBJ_INTEGRATED_EQ               = 0X2C,
    HA_PROG_OBJ_WDRC_ENABLED                = 0x2D,
    HA_PROG_OBJ_MAX_TOTAL_POWER_OUTPUT_CTRL = 0x2E,
    HA_PROG_OBJ_EFFECT_TOTAL                = 0x2F,
} T_APP_HA_PROG_OBJ;

typedef enum
{
    HA_PROG_OBJ_SUSTAIN_SIZE                        = 0,
    HA_PROG_OBJ_SELECTABLE_SIZE                     = 0,
    HA_PROG_OBJ_NAME_SIZE                           = 12,
    HA_PROG_OBJ_TOOL_DATA_SIZE                      = 192,
    HA_PROG_OBJ_VOLUME_LEVEL_SIZE                   = 4,
    HA_PROG_OBJ_WDRC_PRESET_IDX_SIZE                = 4,

    HA_PROG_OBJ_WDRC_SIZE                           = HA_EFFECT_WDRC_SIZE,
    HA_PROG_OBJ_NR_SIZE                             = HA_EFFECT_NR_SIZE,
    HA_PROG_OBJ_GRAPHIC_EQ_SIZE                     = HA_EFFECT_GRAPHIC_EQ_SIZE,
    HA_PROG_OBJ_FBC_SIZE                            = HA_EFFECT_FBC_SIZE,
    HA_PROG_OBJ_SPEECH_ENHANCE_SIZE                 = HA_EFFECT_SPEECH_ENHANCE_SIZE,
    HA_PROG_OBJ_BF_SIZE                             = HA_EFFECT_BF_SIZE,
    HA_PROG_OBJ_WNR_SIZE                            = HA_EFFECT_WNR_SIZE,
    HA_PROG_OBJ_INR_SIZE                            = HA_EFFECT_INR_SIZE,
    HA_PROG_OBJ_RNS_SIZE                            = HA_EFFECT_RNS_SIZE,
    HA_PROG_OBJ_OUTPUT_DRC_SIZE                     = HA_EFFECT_OUTPUT_DRC_SIZE,
    HA_PROG_OBJ_ADAPTIVE_BF_SIZE                    = HA_EFFECT_ADAPTIVE_BF_SIZE,
    HA_PROG_OBJ_OVP_SIZE                            = HA_EFFECT_OVP_SIZE,
    HA_PROG_OBJ_INTEGRATED_EQ_SIZE                  = HA_EFFECT_INTEGRATED_EQ_SIZE,
    HA_PROG_OBJ_WDRC_ENABLED_SIZE                   = HA_EFFECT_WDRC_ENABLED_SIZE,
    HA_PROG_OBJ_MAX_TOTAL_POWER_OUTPUT_CTRL_SIZE    = HA_EFFECT_MAX_TOTAL_POWER_OUTPUT_CTRL_SIZE,
} T_APP_HA_PROG_OBJ_SIZE;

typedef enum
{
    HA_PROG_OPCODE_GET_NUM                  = 0x00,
    HA_PROG_OPCODE_GET_ID                   = 0x01,
    HA_PROG_OPCODE_SET_ID                   = 0x02,
    HA_PROG_OPCODE_GET_OBJ                  = 0x03,
    HA_PROG_OPCODE_SET_OBJ                  = 0x04,
    HA_PROG_OPCODE_GET_ALL_OBJ              = 0x05,
    HA_PROG_OPCODE_SET_ALL_OBJ              = 0x06,
    HA_PROG_OPCODE_RESET_OBJ                = 0x07,
    HA_PROG_OPCODE_GET_ALL_DIRECTION_OBJ    = 0x08,

    HA_PROG_OPCODE_TOTAL                    = 0x09,
} T_APP_HA_PROG_OPCODE;

typedef enum
{
    HA_GLOBAL_OBJ_RHA_WDRC              = 0x00,
    HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS    = 0x01,
    HA_GLOBAL_OBJ_OVP_TRAINING_DATA     = 0x02,
    HA_GLOBAL_OBJ_RHA_WDRC_ENABLED      = 0x03,
    HA_GLOBAL_OBJ_PRESET_IDX            = 0x04,
    HA_GLOBAL_OBJ_ULLRHA_WDRC           = 0X05,
    HA_GLOBAL_OBJ_AUDIO_WDRC            = 0X06,
    HA_GLOBAL_OBJ_VOICE_WDRC            = 0X07,
    HA_GLOBAL_OBJ_ULLRHA_WDRC_ENABLED   = 0x08,
    HA_GLOBAL_OBJ_HA_MODE               = 0x09,
    HA_GLOBAL_OBJ_AUDIO_WDRC_ENABLE     = 0x0A,
    HA_GLOBAL_OBJ_VOICE_WDRC_ENABLE     = 0x0B,
    HA_GLOBAL_OBJ_AUDIO_MIC_SETTINGS    = 0x0C,
    HA_GLOBAL_OBJ_VOICE_MIC_SETTINGS    = 0x0D,
    HA_GLOBAL_OBJ_TOTAL                 = 0x0E,
} T_APP_HA_GLOBAL_OBJ;

typedef enum
{
    HA_HEARING_TEST_OPCODE_CREATE   = 0X00,
    HA_HEARING_TEST_OPCODE_SET      = 0X01,
    HA_HEARING_TEST_OPCODE_START    = 0X02,
    HA_HEARING_TEST_OPCODE_STOP     = 0X03,
    HA_HEARING_TEST_OPCODE_DESTROY  = 0X04,

    HA_HEARING_TEST_OPCODE_TOTAL    = 0X05,
} T_APP_HA_HEARING_TEST_OPCODE;

typedef enum
{
    HA_OVP_TRAINING_STATUS_FAILED   = 0x00,
    HA_OVP_TRAINING_STATUS_SUCCESS  = 0x01,
} T_APP_HA_OVP_TRAINING_STATUS;

typedef enum
{
    APP_HA_PRESET_MODE_AUDIO        = 0x00,
    APP_HA_PRESET_MODE_VOICE        = 0x01,
    APP_HA_PRESET_MODE_RHA          = 0x02,
    APP_HA_PRESET_MODE_ULLRHA       = 0X03,

    APP_HA_PRESET_MODE_TOTAL,
} T_APP_HA_PRESET_MODE;

typedef void (*APP_HA_PROG_APPLY_CBACK)(void);
typedef void (*APP_HA_EFFECT_APPLY_CBACK)(void);

typedef struct t_ha_effect_db
{
    T_OS_QUEUE *queue;
    APP_HA_EFFECT_APPLY_CBACK apply_cback;

} T_HA_EFFECT_DB;

typedef struct t_app_ha_prog_obj_item
{
    struct t_app_ha_prog_obj_item     *p_next;
    T_APP_HA_PROG_OBJ                 type;
    uint16_t                          len;
} T_APP_HA_PROG_OBJ_ITEM;

typedef struct t_ha_prog_db
{
    T_OS_QUEUE *obj_queue;
    uint8_t prog_num;
    uint16_t prog_size_arr[HA_PROG_MAX_NUM];
    uint16_t prog_offset_arr[HA_PROG_MAX_NUM];
    uint8_t permanent_prog_idx;
    uint8_t selectable_prog_idx;
    uint8_t current_prog_is_sync_status;
    APP_HA_PROG_APPLY_CBACK apply_cback;
} T_HA_PROG_DB;

typedef struct t_ha_mode_db
{
    T_APP_HA_MODE mode;
    T_HA_PROG_DB *prog_db;
    T_HA_EFFECT_DB *effect_db;
    struct t_ha_mode_db *next;
} T_HA_MODE_DB;

typedef struct t_app_ha_params_item
{
    struct t_app_ha_params_item *p_next;
    T_APP_HA_PARAMS             type;
    uint16_t                    len;
    uint8_t                     *buf;
} T_APP_HA_PARAMS_ITEM;

typedef struct t_app_ha_audio_params_item
{
    struct t_app_ha_audio_params_item   *p_next;
    uint16_t                            type;
    uint16_t                            cmd_params_len;
    uint16_t                            group_ltv_data_len;
    uint32_t                            offset;
} T_APP_HA_AUDIO_PARAMS_ITEM;

typedef struct t_app_ha_voice_params_item
{
    struct t_app_ha_voice_params_item   *p_next;
    uint16_t                            type;
    uint16_t                            cmd_params_len;
    uint16_t                            group_ltv_data_len;
    uint32_t                            offset;
} T_APP_HA_VOICE_PARAMS_ITEM;

typedef struct t_ha_params_db
{
    T_OS_QUEUE obj_queue;
} T_HA_PARAMS_DB;

typedef struct t_ha_audio_params_db
{
    T_OS_QUEUE obj_queue;
} T_HA_AUDIO_PARAMS_DB;

typedef struct t_ha_voice_params_db
{
    T_OS_QUEUE obj_queue;
} T_HA_VOICE_PARAMS_DB;

typedef struct t_ha_ltv_data_db
{
    T_OS_QUEUE obj_queue;
    uint16_t seq_num;
} T_HA_LTV_DATA_DB;

typedef struct t_ha_hearing_test_db
{
    uint32_t sample_rate;
    uint32_t freq;
    int32_t gain;
    uint32_t fade_in_out_ms;
    uint32_t duration_ms;
    uint32_t interval_ms;
    uint32_t offset_list[1 + HA_HEARING_TEST_TONE_PHASE];
    uint32_t filter_samples;
    double amplitude;
    double samples_per_sine_wave;
    int32_t mapping_table[HA_HEARING_TEST_TONE_TABLE_MAX_LEN];
    uint32_t test_tone_sample_num;
    void (*test_tone_func[HA_HEARING_TEST_TONE_PHASE])(uint32_t seq, uint32_t len, uint32_t buf_idx);
    uint8_t *buf;
    uint32_t buf_len;
    uint32_t buf_sample_num;
    uint32_t frame_len;
} T_HA_HEARING_TEST_DB;

typedef struct t_ha_ovp_training_db
{
    bool is_ready;
    bool bud_is_ready;
    bool ovp_training_status;
    bool bud_ovp_training_status;
} T_HA_OVP_TRAINING_DB;

typedef struct t_ha_adsp_mgr
{
    bool is_send;
} T_HA_ADSP_MGR;

typedef enum
{
    HA_ADSP_CMD_EMPTY               = 0x0000,
    HA_ADSP_CMD_PARA                = 0x0001,
    HA_ADSP_CMD_INIT_FINISH         = 0x0002,
    HA_ADSP_CMD_BOOT_ACK            = 0x0003,
    HA_ADSP_CMD_PARA_ULLRHA         = 0x0070,
    HA_ADSP_CMD_PARA_DEBUG          = 0x0071,
} T_HA_ADSP_CMD;

typedef enum
{
    HA_EFFECT_TYPE_PROG_OBJ             = 0X00,
    HA_EFFECT_TYPE_GLOBAL_OBJ           = 0X01,
    HA_EFFECT_TYPE_DSP_PARAMS_OBJ       = 0X02,
    HA_EFFECT_TYPE_CTRL_OBJ             = 0x03,
    HA_EFFECT_TYPE_DSP_AUDIO_PARAMS_OBJ = 0X04,
    HA_EFFECT_TYPE_DSP_VOICE_PARAMS_OBJ = 0X05,
} T_HA_EFFECT_TYPE;

typedef struct t_app_ha_ull_type_item
{
    struct t_app_ha_ull_type_item   *p_next;
    T_HA_EFFECT_TYPE                effect_type;
    uint16_t                        effect;
} T_APP_HA_ULL_EFFECT_ITEM;

typedef struct t_app_ha_ltv_data_item
{
    struct t_app_ha_ltv_data_item   *p_next;
    uint8_t                         *buf;
    uint16_t                        len;
} T_APP_HA_LTV_DATA_ITEM;


typedef struct t_ha_ull_effect_db
{
    T_OS_QUEUE *queue;
    uint16_t count;
    uint16_t sel_idx;
} T_HA_ULL_EFFECT_DB;

typedef struct t_app_ha_cmd_item
{
    struct t_app_ha_cmd_item        *p_next;
    T_HA_EFFECT_TYPE                effect_type;
    uint16_t                        effect;
} T_APP_HA_CMD_ITEM;

typedef struct t_ha_cmd_db
{
    T_OS_QUEUE *queue;
} T_HA_CMD_DB;

static uint8_t ha_timer_id = 0;
static uint8_t timer_idx_ha_listening_delay = 0;
static uint8_t timer_idx_ha_apt_vol_level = 0;
static uint8_t timer_idx_ha_hearing_test_tone = 0;

static uint8_t current_apt_vol_level_cnt = 0;
static uint8_t low_to_high_gain_level[5] = {7, 9, 11, 13, 15};

static T_HA_MODE_DB *ha_mode_db = NULL;
static T_APP_HA_MODE current_ha_mode = HA_MODE_RHA;
static T_HA_MODE_DB *current_ha_mode_db = NULL;

static bool ullrha_is_enable = false;

static uint8_t *params_buf = NULL;
static uint8_t *group_ltv_data = NULL;

static T_HA_EFFECT_DB *ha_effect_db;

static T_HA_PARAMS_DB ha_params_db;
static T_HA_LTV_DATA_DB ha_ltv_data_db;
static T_HA_LTV_DATA_DB audio_ltv_data_db;
static T_HA_LTV_DATA_DB voice_ltv_data_db;
T_HA_VSE_MGR_DB ha_vse_mgr_db;
static uint8_t ha_hearing_test_freq_handle = 0;
static bool is_hearing_test = false;
static bool bud_is_hearing_test = false;
static uint32_t org_actual_mhz = 40;
static T_AUDIO_TRACK_HANDLE last_playback_handle = NULL;

static uint8_t prev_selectable_prog_idx[HA_MODE_TOTAL] = {0};

static T_HA_CMD_DB *ha_cmd_db;
static T_HA_CMD_DB *audio_cmd_db;
static T_HA_CMD_DB *voice_cmd_db;

static uint16_t audio_seq_num = 0;
static uint16_t voice_seq_num = 0;

static T_HA_ULL_EFFECT_DB *ha_ull_effect_db;
T_HA_ADSP_MGR *ha_adsp_mgr;

T_AUDIO_EFFECT_INSTANCE ha_instance;

static uint16_t ha_params_size_table[HA_PARAMS_TOTAL] =
{
    [HA_PARAMS_RNS]                     = HA_PARAMS_RNS_SIZE,
    [HA_PARAMS_SPEECH_ENHANCE]          = HA_PARAMS_SPEECH_ENHANCE_MAX_SIZE,
    [HA_PARAMS_MMI_VOLUME_STEP]         = HA_PARAMS_MMI_VOLUME_STEP_SIZE,
    [HA_PARAMS_WDRC]                    = HA_PARAMS_WDRC_SIZE,
    [HA_PARAMS_WDRC_ENABLED]            = HA_PARAMS_WDRC_ENABLED_SIZE,
    [HA_PARAMS_HA_TEST]                 = HA_PARAMS_HA_TEST_SIZE,
    [HA_PARAMS_MIC_SETTING]             = HA_PARAMS_MIC_SETTING_MAX_SIZE,
    [HA_PARAMS_MIC_COMPENSATION]        = HA_PARAMS_MIC_COMPENSATION_MAX_SIZE,
    [HA_PARAMS_SPK_COMPENSATION]        = HA_PARAMS_SPK_COMPENSATION_MAX_SIZE,
    [HA_PARAMS_ENV_CLASSIFICATION]      = HA_PARAMS_HA_ENV_CLASSIFICATION_MAX_SIZE,
    [HA_PARAMS_BAND_WIDTH]              = HA_PARAMS_HA_BAND_WIDTH_SIZE,
    [HA_PARAMS_MAX_GAIN_SETTING]        = HA_PARAMS_HA_MAX_GAIN_SETTING_MAX_SIZE,
    [HA_PARAMS_DC_REMOVER]              = HA_PARAMS_HA_DC_REMOVER_MAX_SIZE,
    [HA_PARAMS_CUSTOMER_EQ]             = HA_PARAMS_HA_CUSTOMER_EQ_MAX_SIZE,
    [HA_PARAMS_RNS_COMPENSATION]        = HA_PARAMS_HA_RNS_COMPENSATION_MAX_SIZE,
    [HA_PARAMS_HA_DBFS]                 = HA_PARAMS_HA_DBFS_SIZE,
    [HA_PARAMS_DEHOWLING_PARA]          = HA_PARAMS_HA_PARAMS_DEHOWLING_PARA_SIZE,
    [HA_PARAMS_MIC_SWAP]                = HA_PARAMS_HA_PARAMS_MIC_SWAP_SIZE,
    [HA_PARAMS_APT_MIC_GAIN]            = HA_PARAMS_HA_PARAMS_APT_MIC_GAIN_SIZE,
    [HA_PARAMS_BYPASS_HA_PROCESSING]    = HA_PARAMS_HA_PARAMS_BYPASS_HA_PROCESSING_SIZE,
};

static const uint16_t ha_effect_size_table[HA_EFFECT_TOTAL] =
{
    [HA_EFFECT_WDRC]                        = HA_EFFECT_WDRC_SIZE,
    [HA_EFFECT_NR]                          = HA_EFFECT_NR_SIZE,
    [HA_EFFECT_GRAPHIC_EQ]                  = HA_EFFECT_GRAPHIC_EQ_SIZE,
    [HA_EFFECT_FBC]                         = HA_EFFECT_FBC_SIZE,
    [HA_EFFECT_SPEECH_ENHANCE]              = HA_EFFECT_SPEECH_ENHANCE_SIZE,
    [HA_EFFECT_BF]                          = HA_EFFECT_BF_SIZE,
    [HA_EFFECT_WNR]                         = HA_EFFECT_WNR_SIZE,
    [HA_EFFECT_INR]                         = HA_EFFECT_INR_SIZE,
    [HA_EFFECT_RNS]                         = HA_EFFECT_RNS_SIZE,
    [HA_EFFECT_OUTPUT_DRC]                  = HA_EFFECT_OUTPUT_DRC_SIZE,
    [HA_EFFECT_ADAPTIVE_BF]                 = HA_EFFECT_ADAPTIVE_BF_SIZE,
    [HA_EFFECT_OVP]                         = HA_EFFECT_OVP_SIZE,
    [HA_EFFECT_INTEGRATED_EQ]               = HA_EFFECT_INTEGRATED_EQ_SIZE,
    [HA_EFFECT_WDRC_ENABLED]                = HA_EFFECT_WDRC_ENABLED_SIZE,
    [HA_EFFECT_MAX_TOTAL_POWER_OUTPUT_CTRL] = HA_EFFECT_MAX_TOTAL_POWER_OUTPUT_CTRL_SIZE,
};

static const uint16_t ha_prog_obj_size_table[HA_PROG_OBJ_EFFECT_TOTAL] =
{
    [HA_PROG_OBJ_SUSTAIN]                       = HA_PROG_OBJ_SUSTAIN_SIZE,
    [HA_PROG_OBJ_SELECTABLE]                    = HA_PROG_OBJ_SELECTABLE_SIZE,
    [HA_PROG_OBJ_NAME]                          = HA_PROG_OBJ_NAME_SIZE,
    [HA_PROG_OBJ_TOOL_DATA]                     = HA_PROG_OBJ_TOOL_DATA_SIZE,
    [HA_PROG_OBJ_VOLUME_LEVEL]                  = HA_PROG_OBJ_VOLUME_LEVEL_SIZE,
    [HA_PROG_OBJ_WDRC_PRESET_IDX]               = HA_PROG_OBJ_WDRC_PRESET_IDX_SIZE,
    [HA_PROG_OBJ_WDRC]                          = HA_PROG_OBJ_WDRC_SIZE,
    [HA_PROG_OBJ_NR]                            = HA_PROG_OBJ_NR_SIZE,
    [HA_PROG_OBJ_GRAPHIC_EQ]                    = HA_PROG_OBJ_GRAPHIC_EQ_SIZE,
    [HA_PROG_OBJ_FBC]                           = HA_PROG_OBJ_FBC_SIZE,
    [HA_PROG_OBJ_SPEECH_ENHANCE]                = HA_PROG_OBJ_SPEECH_ENHANCE_SIZE,
    [HA_PROG_OBJ_BF]                            = HA_PROG_OBJ_BF_SIZE,
    [HA_PROG_OBJ_WNR]                           = HA_PROG_OBJ_WNR_SIZE,
    [HA_PROG_OBJ_INR]                           = HA_PROG_OBJ_INR_SIZE,
    [HA_PROG_OBJ_RNS]                           = HA_PROG_OBJ_RNS_SIZE,
    [HA_PROG_OBJ_VOLUME_LEVEL]                  = HA_PROG_OBJ_VOLUME_LEVEL_SIZE,
    [HA_PROG_OBJ_OUTPUT_DRC]                    = HA_PROG_OBJ_OUTPUT_DRC_SIZE,
    [HA_PROG_OBJ_ADAPTIVE_BF]                   = HA_PROG_OBJ_ADAPTIVE_BF_SIZE,
    [HA_PROG_OBJ_OVP]                           = HA_PROG_OBJ_OVP_SIZE,
    [HA_PROG_OBJ_INTEGRATED_EQ]                 = HA_PROG_OBJ_INTEGRATED_EQ_SIZE,
    [HA_PROG_OBJ_WDRC_ENABLED]                  = HA_PROG_OBJ_WDRC_ENABLED_SIZE,
    [HA_PROG_OBJ_MAX_TOTAL_POWER_OUTPUT_CTRL]   = HA_PROG_OBJ_MAX_TOTAL_POWER_OUTPUT_CTRL_SIZE,
};

static const uint16_t ha_global_obj_size_table[HA_GLOBAL_OBJ_TOTAL] =
{
    [HA_GLOBAL_OBJ_HA_MODE]             = HA_GLOBAL_OBJ_HA_MODE_SIZE,
    [HA_GLOBAL_OBJ_RHA_WDRC]            = HA_GLOBAL_OBJ_RHA_WDRC_MAX_SIZE,
    [HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS]  = HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS_SIZE,
    [HA_GLOBAL_OBJ_OVP_TRAINING_DATA]   = HA_GLOBAL_OBJ_OVP_TRAINING_DATA_SIZE,
    [HA_GLOBAL_OBJ_RHA_WDRC_ENABLED]    = HA_GLOBAL_OBJ_RHA_WDRC_ENABLED_SIZE,
    [HA_GLOBAL_OBJ_PRESET_IDX]          = HA_GLOBAL_OBJ_PRESET_IDX_SIZE,
    [HA_GLOBAL_OBJ_ULLRHA_WDRC]         = HA_GLOBAL_OBJ_ULLRHA_WDRC_MAX_SIZE,
    [HA_GLOBAL_OBJ_AUDIO_WDRC]          = HA_GLOBAL_OBJ_AUDIO_WDRC_MAX_SIZE,
    [HA_GLOBAL_OBJ_VOICE_WDRC]          = HA_GLOBAL_OBJ_VOICE_WDRC_MAX_SIZE,
    [HA_GLOBAL_OBJ_ULLRHA_WDRC_ENABLED] = HA_GLOBAL_OBJ_ULLRHA_WDRC_ENABLED_SIZE,
    [HA_GLOBAL_OBJ_AUDIO_WDRC_ENABLE]   = HA_GLOBAL_OBJ_AUDIO_WDRC_ENABLE_SIZE,
    [HA_GLOBAL_OBJ_VOICE_WDRC_ENABLE]   = HA_GLOBAL_OBJ_VOICE_WDRC_ENABLE_SIZE,
    [HA_GLOBAL_OBJ_AUDIO_MIC_SETTINGS]  = HA_GLOBAL_OBJ_AUDIO_MIC_SETTINGS_SIZE,
    [HA_GLOBAL_OBJ_VOICE_MIC_SETTINGS]  = HA_GLOBAL_OBJ_VOICE_MIC_SETTINGS_SIZE,
};

static const T_APP_HA_GLOBAL_OBJ ha_global_feature_table[HA_GLOBAL_FEATURE_NUM] =
{
    HA_GLOBAL_OBJ_HA_MODE,
    HA_GLOBAL_OBJ_RHA_WDRC,
    HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS,
    HA_GLOBAL_OBJ_OVP_TRAINING_DATA,
    HA_GLOBAL_OBJ_RHA_WDRC_ENABLED,
    HA_GLOBAL_OBJ_PRESET_IDX,
    HA_GLOBAL_OBJ_ULLRHA_WDRC,
    HA_GLOBAL_OBJ_ULLRHA_WDRC_ENABLED,
    HA_GLOBAL_OBJ_AUDIO_WDRC,
    HA_GLOBAL_OBJ_VOICE_WDRC,
    HA_GLOBAL_OBJ_AUDIO_WDRC_ENABLE,
    HA_GLOBAL_OBJ_VOICE_WDRC_ENABLE,
    HA_GLOBAL_OBJ_AUDIO_MIC_SETTINGS,
    HA_GLOBAL_OBJ_VOICE_MIC_SETTINGS,
};

static const T_APP_HA_PARAMS ha_params_feature_table[HA_PARAMS_FEATURE_NUM] =
{
    HA_PARAMS_RNS,
    HA_PARAMS_SPEECH_ENHANCE,
    HA_PARAMS_MMI_VOLUME_STEP,
    HA_PARAMS_WDRC,
    HA_PARAMS_WDRC_ENABLED,
    HA_PARAMS_HA_TEST,
    HA_PARAMS_MIC_SETTING,
    HA_PARAMS_SPK_COMPENSATION,
    HA_PARAMS_MIC_COMPENSATION,
    HA_PARAMS_ENV_CLASSIFICATION,
    HA_PARAMS_BAND_WIDTH,
    HA_PARAMS_MAX_GAIN_SETTING,
    HA_PARAMS_FBC_SETTING,
    HA_PARAMS_DC_REMOVER,
    HA_PARAMS_CUSTOMER_EQ,
    HA_PARAMS_RNS_COMPENSATION,
    HA_PARAMS_HA_DBFS,
    HA_PARAMS_BYPASS_HA_PROCESSING,
    HA_PARAMS_DEHOWLING_PARA,
    HA_PARAMS_MIC_SWAP,
    HA_PARAMS_APT_MIC_GAIN,
};

static const uint16_t ha_audio_param_mapping_table[HA_AUDIO_PARAMS_TOTAL_NUM] =
{
    [HA_AUDIO_PARAMS_WDRC]          = HA_DSP_FEATURE_HA_AUDIO_PARA_WDRC,
    [HA_AUDIO_PARAMS_WDRC_ENABLED]  = HA_DSP_FEATURE_HA_AUDIO_PARA_WDRC_ENABLED,
    [HA_AUDIO_PARAMS_MIC_SETTINGS]  = HA_DSP_FEATURE_HA_AUDIO_PARA_MIC_SETTINGS,
};

static const uint16_t ha_voice_param_mapping_table[HA_VOICE_PARAMS_TOTAL_NUM] =
{
    [HA_VOICE_PARAMS_WDRC]          = HA_DSP_FEATURE_HA_VOICE_PARA_WDRC,
    [HA_VOICE_PARAMS_WDRC_ENABLED]  = HA_DSP_FEATURE_HA_VOICE_PARA_WDRC_ENABLED,
    [HA_VOICE_PARAMS_MIC_SETTINGS]  = HA_DSP_FEATURE_HA_VOICE_PARA_MIC_SETTINGS,
};

static const T_APP_AUDIO_TONE_TYPE ha_prog_to_tone[HA_PROG_MAX_TONE_NUM] =
{
    TONE_HA_PROG_0,
    TONE_HA_PROG_1,
    TONE_HA_PROG_2,
    TONE_HA_PROG_3,
    TONE_HA_PROG_4,
    TONE_HA_PROG_5,
    TONE_HA_PROG_6,
    TONE_HA_PROG_7,
    TONE_HA_PROG_8,
    TONE_HA_PROG_9,
};

static T_HA_HEARING_TEST_DB ha_hearing_test_db;

static T_HA_OVP_TRAINING_DB ha_ovp_training_db;

uint8_t *app_ha_get_group_ltv_data()
{
    return group_ltv_data;
}

//wrap from rha cmd to ullrha cmd
void app_ha_wrap_ullrha_cmd(uint8_t *buf, uint16_t len)
{
    uint16_t buf_len = buf[2] + (buf[3] << 8);

    if (len < 4 || buf_len == 0)
    {
        return;
    }

    //is aleady in ullrha cmd format
    if (buf_len == len - 4)
    {
        return;
    }

    len -= 4; //exclude header len

    buf[2] = len & 0xFF;
    buf[3] = len >> 8;

    return;
}

static void app_ha_set_ovp_training_status(T_REMOTE_SESSION_ROLE role, bool is_success)
{

    if ((role == REMOTE_SESSION_ROLE_SINGLE) ||
        (role == REMOTE_SESSION_ROLE_PRIMARY))
    {
        ha_ovp_training_db.is_ready = true;
        ha_ovp_training_db.ovp_training_status = is_success;
    }
    else
    {
        ha_ovp_training_db.bud_is_ready = true;
        ha_ovp_training_db.bud_ovp_training_status = is_success;
    }
}

static bool app_ha_get_ovp_training_status()
{
    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
        (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED))
    {
        return ha_ovp_training_db.ovp_training_status;
    }

    return (ha_ovp_training_db.ovp_training_status & ha_ovp_training_db.bud_ovp_training_status);
}

static bool app_ha_get_ovp_training_is_ready_status()
{
    if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
        (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED))
    {
        return ha_ovp_training_db.is_ready;
    }

    return (ha_ovp_training_db.is_ready & ha_ovp_training_db.bud_is_ready);
}

static void app_ha_effect_apply_cback_register(APP_HA_EFFECT_APPLY_CBACK ha_effect_apply_cb)
{
    T_HA_MODE_DB *cur_ptr = ha_mode_db;

    while (cur_ptr)
    {
        cur_ptr->effect_db->apply_cback = ha_effect_apply_cb;
        cur_ptr = cur_ptr->next;
    }
}

static void app_ha_effect_create(T_HA_EFFECT_DB *effect_db, T_APP_HA_EFFECT effect)
{
    T_APP_HA_EFFECT_ITEM *p_item = NULL;

    if (effect < HA_EFFECT_TOTAL)
    {
        p_item = (T_APP_HA_EFFECT_ITEM *)malloc(sizeof(T_APP_HA_EFFECT_ITEM));

        p_item->p_next = NULL;
        p_item->effect = effect;
        p_item->info_buf = NULL;
        p_item->info_len = ha_effect_size_table[effect];
        p_item->enabled = false;

        os_queue_in(effect_db->queue, p_item);
    }
}

static void app_ha_effect_set(T_APP_HA_EFFECT effect, bool is_enabled, void *info_buf,
                              uint16_t info_len)
{
    T_APP_HA_EFFECT_ITEM *p_item = NULL;

    if (info_buf == NULL || info_len == 0)
    {
        return;
    }

    p_item = (T_APP_HA_EFFECT_ITEM *)current_ha_mode_db->effect_db->queue->p_first;

    while (p_item != NULL)
    {
        if (p_item->effect == effect)
        {
            p_item->enabled = is_enabled;

            if (p_item->info_len != info_len || info_buf == NULL || is_enabled == false)
            {
                return;
            }

            if (p_item->info_buf == NULL)
            {
                p_item->info_buf = malloc(sizeof(uint8_t) * info_len);
            }

            memcpy(p_item->info_buf, info_buf, info_len);

            return;
        }

        p_item = p_item->p_next;
    }
}

static T_APP_HA_EFFECT_ITEM *app_ha_effect_get(T_APP_HA_EFFECT effect)
{
    T_APP_HA_EFFECT_ITEM *p_item = NULL;

    p_item = (T_APP_HA_EFFECT_ITEM *)current_ha_mode_db->effect_db->queue->p_first;

    while (p_item != NULL)
    {
        if (p_item->effect == effect)
        {
            return p_item;
        }

        p_item = p_item->p_next;
    }

    return NULL;
}

#if 0
static void app_ha_effect_enable(T_APP_HA_EFFECT effect)
{
    T_APP_HA_EFFECT_ITEM *p_item = app_ha_effect_get(effect);

    if (p_item)
    {
        p_item->enabled = true;
    }
}
#endif

#if 0
static void app_ha_effect_disable(T_APP_HA_EFFECT effect)
{
    T_APP_HA_EFFECT_ITEM *p_item = app_ha_effect_get(effect);

    if (p_item)
    {
        p_item->enabled = false;
    }
}
#endif

static void app_ha_set_anc_apt_on_off(bool set_anc_apt_on)
{
    T_ANC_APT_STATE state = ANC_OFF_NORMAL_APT_ON;
    T_ANC_APT_EVENT event = EVENT_NORMAL_APT_ON;

    if (app_listening_get_is_special_event_ongoing())
    {
        //MMI triggered, change final to current
        *app_db.final_listening_state = app_db.current_listening_state;
    }

    if (set_anc_apt_on)
    {
        state = app_db.last_anc_apt_on_state;
        event = ANC_APT_STATE_TO_EVENT(state);
    }

    app_listening_state_machine(event, true, true);
}

#if 0
static void app_ha_effect_release(T_APP_HA_EFFECT effect)
{
    T_APP_HA_EFFECT_ITEM *p_item = app_ha_effect_get(effect);

    if (p_item->info_buf)
    {
        free(p_item->info_buf);
    }

    os_queue_delete(current_ha_mode_db->effect_db->queue, p_item);
}
#endif

static void app_ha_effect_set_scheme()
{
    uint8_t i = 0;

    for (i = 0; i < HA_EFFECT_TOTAL; i++)
    {
        app_ha_effect_create(current_ha_mode_db->effect_db, (T_APP_HA_EFFECT)i);
    }
}

static bool app_ha_tone_flush_and_play(T_APP_AUDIO_TONE_TYPE tone_type, bool relay)
{
    bool ret = false;

    app_audio_tone_type_cancel(ha_prog_to_tone[prev_selectable_prog_idx[current_ha_mode_db->mode]],
                               relay);

    ret = app_audio_tone_type_play(tone_type, false, relay);

    return ret;
}

static uint32_t app_ha_dsp_feature_offset_get(uint16_t *payload_len, uint32_t ext_para_offset,
                                              uint16_t type)
{
    uint32_t offset = 0;
    uint32_t type_offset = 0;
    uint32_t end_offset = 0;
    uint8_t buf[4];
    uint16_t sync_word;
    uint16_t ext_para_len;
    uint16_t feature_type = 0;
    uint16_t feature_para_len = 0;

    //get ext_para_offset
    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                       ext_para_offset,
                       (uint8_t *)&offset, sizeof(uint32_t));

    //get ext_para sync word, ext_para len
    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                       offset,
                       (uint8_t *)&buf, sizeof(uint32_t));
    sync_word = buf[0] + (buf[1] << 8);
    ext_para_len = buf[2] + (buf[3] << 8);
    end_offset = offset + 4 + ext_para_len;

    if (sync_word == HA_DSP_EXT_PARA_SYNC_WORD)
    {
        offset += 4;

        while (offset < end_offset)
        {
            fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                               offset,
                               (uint8_t *)&buf, sizeof(uint32_t));
            feature_type = buf[0] + (buf[1] << 8);
            feature_para_len = buf[2] + (buf[3] << 8);

            if (feature_type == type)
            {
                type_offset = offset + 4;
                *payload_len = feature_para_len;
                break;
            }

            offset += (4 + feature_para_len);
        }
    }

    return type_offset;
}

static uint32_t app_ha_get_prog_base(T_APP_HA_MODE mode)
{
    static uint32_t ha_mode_base[HA_MODE_TOTAL] = {0};
    static bool flag = false;

    if (flag == false)
    {
        T_HA_MODE_DB *cur_ptr = ha_mode_db;

        while (cur_ptr)
        {
            if (cur_ptr->mode == HA_MODE_RHA)
            {
                ha_mode_base[cur_ptr->mode] = 0;
            }
            else if (cur_ptr->mode == HA_MODE_ULLRHA)
            {
                uint16_t payload_len = 0;

                app_ha_dsp_feature_offset_get(&payload_len, HA_DSP_EXT_PARA_OFFSET,
                                              HA_DSP_FEATURE_HA_PROG);
                ha_mode_base[cur_ptr->mode] = payload_len;
            }

            cur_ptr = cur_ptr->next;
        }

        flag = true;
    }

    if (flag)
    {
        return ha_mode_base[mode];
    }

    return 0;
}

static uint16_t app_ha_get_effect_ui_from_tool_data(uint8_t effect)
{
    uint16_t len = HA_PROG_OBJ_TOOL_DATA_SIZE;
    uint16_t offset = HA_PROG_OBJ_TOOL_DATA_SIZE;

    switch (effect)
    {
    case HA_EFFECT_OUTPUT_DRC:
        {
            offset = len - 52;
        }
        break;

    default:
        {

        }
        break;

    }

    return offset;
}

static void app_ha_prog_set_selectable_idx(uint8_t prog_id)
{
    uint8_t buf[HA_PROG_OPTION];
    uint32_t base = app_ha_get_prog_base(current_ha_mode_db->mode);

    prev_selectable_prog_idx[current_ha_mode_db->mode] =
        current_ha_mode_db->prog_db->selectable_prog_idx;
    current_ha_mode_db->prog_db->selectable_prog_idx = prog_id;

    if (ha_ext_ftl_load_data(buf, base, HA_PROG_OPTION) == 0)
    {
        buf[2] = prog_id;
        ha_ext_ftl_save_data(buf, base, HA_PROG_OPTION);
    }

    if ((current_ha_mode_db->prog_db->selectable_prog_idx < HA_PROG_MAX_TONE_NUM) &&
        (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
         app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED))
    {
        app_ha_tone_flush_and_play((T_APP_AUDIO_TONE_TYPE)(TONE_HA_PROG_0 +
                                                           current_ha_mode_db->prog_db->selectable_prog_idx), true);
    }
}

static void app_ha_prog_add_object(T_APP_HA_PROG_OBJ type)
{
    T_APP_HA_PROG_OBJ_ITEM *p_item = NULL;

    if (type < HA_PROG_OBJ_CTRL_TOTAL ||
        (type >= HA_PROG_OBJ_WDRC && type < HA_PROG_OBJ_EFFECT_TOTAL))
    {

        p_item = (T_APP_HA_PROG_OBJ_ITEM *)malloc(sizeof(T_APP_HA_PROG_OBJ_ITEM));

        p_item->p_next = NULL;
        p_item->type = type;
        p_item->len = ha_prog_obj_size_table[type]; // include tool data
        os_queue_in(current_ha_mode_db->prog_db->obj_queue, p_item);
    }
}

static uint16_t app_ha_prog_get_object_size(uint8_t type)
{
    uint16_t size = 0;

    if (type < HA_PROG_OBJ_EFFECT_TOTAL)
    {
        size = ha_prog_obj_size_table[type];
    }

    return size;
}

static void app_ha_prog_set_object(uint8_t prog_id, uint8_t type, uint8_t *data)
{
    uint8_t *buf = calloc(current_ha_mode_db->prog_db->prog_size_arr[prog_id], sizeof(uint8_t));
    uint16_t cur_object_addr = 0;
    uint16_t cur_object_data_size = 0;
    uint16_t cur_object_size = 0;

    if (ha_ext_ftl_load_data(buf, current_ha_mode_db->prog_db->prog_offset_arr[prog_id],
                             current_ha_mode_db->prog_db->prog_size_arr[prog_id]))
    {
        free(buf);
        return;
    }

    cur_object_addr = 0;

    while (cur_object_addr < current_ha_mode_db->prog_db->prog_size_arr[prog_id])
    {
        cur_object_data_size = buf[cur_object_addr + 2] + (buf[cur_object_addr + 3] << 8);
        cur_object_size = 4 + cur_object_data_size;

        if (buf[cur_object_addr] == type)
        {
            buf[cur_object_addr + 1] = 1; //is set
            memcpy(buf + cur_object_addr + 4, data, cur_object_data_size);

            ha_ext_ftl_save_data(buf, current_ha_mode_db->prog_db->prog_offset_arr[prog_id],
                                 current_ha_mode_db->prog_db->prog_size_arr[prog_id]);
            break;
        }

        cur_object_addr += cur_object_size;
    }

    free(buf);
}

static void app_ha_prog_get_object(uint8_t prog_id, uint8_t type, uint8_t *buf, uint16_t *size)
{
    uint8_t *tp_buf = calloc(current_ha_mode_db->prog_db->prog_size_arr[prog_id], sizeof(uint8_t));
    uint16_t cur_effect_addr = 0;
    uint16_t cur_effect_size = 0;

    *size = 0;

    if (ha_ext_ftl_load_data(tp_buf, current_ha_mode_db->prog_db->prog_offset_arr[prog_id],
                             current_ha_mode_db->prog_db->prog_size_arr[prog_id]))
    {
        free(tp_buf);
        return;
    }

    cur_effect_addr = 0;

    while (cur_effect_addr < current_ha_mode_db->prog_db->prog_size_arr[prog_id])
    {
        cur_effect_size = 4 + tp_buf[cur_effect_addr + 2] + (tp_buf[cur_effect_addr + 3] << 8);

        if (tp_buf[cur_effect_addr] == type)
        {
            *size = cur_effect_size - 4;
            memcpy(buf, tp_buf + cur_effect_addr + 4, *size);

            break;
        }

        cur_effect_addr += cur_effect_size;
    }

    free(tp_buf);
}

static uint32_t app_ha_dsp_ext_para_offset_get(T_APP_HA_MODE mode, uint16_t *payload_len,
                                               uint16_t type)
{
    uint32_t offset = 0;
    uint32_t type_offset = 0;
    uint32_t end_offset = 0;
    uint8_t buf[4];
    uint16_t sync_word;
    uint16_t ext_para_len;
    uint16_t feature_type = 0;
    uint16_t feature_para_len = 0;
    uint32_t ext_para_offset = HA_DSP_EXT_PARA_OFFSET;

    if (mode == HA_MODE_ULLRHA)
    {
        ext_para_offset = HA_DSP_EXT_PARA2_OFFSET;
    }

    //get ext_para_offset
    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                       ext_para_offset,
                       (uint8_t *)&offset, sizeof(uint32_t));

    //get ext_para sync word, ext_para len
    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                       offset,
                       (uint8_t *)&buf, sizeof(uint32_t));
    sync_word = buf[0] + (buf[1] << 8);
    ext_para_len = buf[2] + (buf[3] << 8);
    end_offset = offset + 4 + ext_para_len;

    if (sync_word == HA_DSP_EXT_PARA_SYNC_WORD)
    {
        offset += 4;

        while (offset < end_offset)
        {
            fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                               offset,
                               (uint8_t *)&buf, sizeof(uint32_t));
            feature_type = buf[0] + (buf[1] << 8);
            feature_para_len = buf[2] + (buf[3] << 8);

            if (feature_type == type)
            {
                type_offset = offset + 4;
                *payload_len = feature_para_len;
                break;
            }

            offset += (4 + feature_para_len);
        }
    }

    return type_offset;
}

static uint32_t app_ha_dsp_ext_para_subtype_offset_get(uint16_t *payload_len, uint16_t type,
                                                       uint16_t subtype)
{
    uint32_t offset = 0;
    uint32_t type_offset = 0;
    uint32_t end_offset = 0;
    uint8_t buf[4];
    uint16_t sync_word;
    uint16_t ext_para_len;
    uint16_t feature_type = 0;
    uint16_t feature_para_len = 0;

    //get ext_para_offset
    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                       HA_DSP_EXT_PARA_OFFSET,
                       (uint8_t *)&offset, sizeof(uint32_t));

    //get ext_para sync word, ext_para len
    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                       offset,
                       (uint8_t *)&buf, sizeof(uint32_t));
    sync_word = buf[0] + (buf[1] << 8);
    ext_para_len = buf[2] + (buf[3] << 8);
    end_offset = offset + 4 + ext_para_len;

    if (sync_word == HA_DSP_EXT_PARA_SYNC_WORD)
    {
        offset += 4;

        while (offset < end_offset)
        {
            fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                               offset,
                               (uint8_t *)&buf, sizeof(uint32_t));
            feature_type = buf[0] + (buf[1] << 8);
            feature_para_len = buf[2] + (buf[3] << 8);

            if (feature_type == type)
            {
                end_offset = offset + 4 + feature_para_len;
                offset += 4; //points to params

                while (offset < end_offset)
                {
                    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                                       offset,
                                       (uint8_t *)&buf, sizeof(uint32_t));
                    feature_type = buf[0] + (buf[1] << 8);
                    feature_para_len = buf[2] + (buf[3] << 8);

                    if (feature_type == subtype)
                    {
                        type_offset = offset + 4;
                        *payload_len = feature_para_len;
                        break;
                    }

                    offset += (4 + feature_para_len);
                }
                break;
            }

            offset += (4 + feature_para_len);
        }
    }

    return type_offset;
}

static uint16_t app_ha_get_rns_params(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_RNS_PARAMS);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                           offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_speech_enhance(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_SPEECH_ENHANCE);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_wdrc_params(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_WDRC_PARAMS);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_wdrc_enabled_params(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_WDRC_ENABLED);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_ha_test_params(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_TEST);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

uint16_t app_ha_get_group_ltv_data_len(uint16_t params_len)
{
    uint16_t left_len = 0;
    uint16_t group_ltv_data_len = 0;

    if (params_len <= HA_LTV_DATA_MAX_SIZE) //single
    {
        group_ltv_data_len = HA_LTV_DATA_HEADER_SIZE + params_len;
    }
    else //params_lens > HA_LTV_DATA_MAX_SIZE
    {
        left_len = params_len;

        //start type
        group_ltv_data_len += (HA_LTV_DATA_START_TYPE_HEADER_SIZE + HA_LTV_DATA_START_TYPE_SIZE);
        left_len -= HA_LTV_DATA_START_TYPE_SIZE;

        while (left_len >= HA_LTV_DATA_MAX_SIZE)
        {
            group_ltv_data_len += (HA_LTV_DATA_HEADER_SIZE + HA_LTV_DATA_MAX_SIZE);
            left_len -= HA_LTV_DATA_MAX_SIZE;
        }

        if (left_len)
        {
            group_ltv_data_len += (HA_LTV_DATA_HEADER_SIZE + left_len);
        }
    }

    return group_ltv_data_len;
}

void app_ha_generate_ltv_data_payload(T_APP_HA_STEREO_TYPE stereo_type, uint8_t *buf_l,
                                      uint8_t *buf_r, uint16_t len)
{
    if (stereo_type == HA_STEREO_TYPE_L)
    {
        memcpy(group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - len, buf_l, len);
        group_ltv_data[ GROUP_LTV_DATA_MAX_SIZE - len - 1] = stereo_type;
    }
    else if (stereo_type == HA_STEREO_TYPE_R)
    {
        memcpy(group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - len, buf_r, len);
        group_ltv_data[ GROUP_LTV_DATA_MAX_SIZE - len - 1] = stereo_type;
    }
    else //both
    {
        memcpy(group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - 2 * len, buf_l, len);
        memcpy(group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - len, buf_r, len);
        group_ltv_data[ GROUP_LTV_DATA_MAX_SIZE - 2 * len - 1] = stereo_type;
    }
}

void app_ha_fill_prepend_payload(T_APP_HA_STEREO_TYPE type, uint16_t ha_type,
                                 uint8_t *prepend_payload, uint8_t *prepend_len)
{
    uint8_t channel_mask[4] = {0};
    uint8_t entity_id = 0; //dsp1
    uint8_t enable = 1;
    uint8_t direction = 0; //unspecify
    uint8_t position = 1; //algo gain
    uint8_t filter_type = 1; //high pass filter

    *prepend_len = 0;

    if (type == HA_STEREO_TYPE_BOTH)
    {
        channel_mask[0] = 6; //bit1, 2
    }
    else if (type == HA_STEREO_TYPE_L)
    {
        channel_mask[0] = 2;
    }
    else // HA_STEREO_TYPE_R
    {
        channel_mask[0] = 4;
    }

    switch (ha_type)
    {
    case HA_CMD_TYPE_NR:
        {
            *prepend_len = 6;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = enable;
        }
        break;

    case HA_CMD_TYPE_GRAPHIC_EQ:
        {
            *prepend_len = 7;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = direction;
            prepend_payload[6] = enable;
        }
        break;

    case HA_CMD_TYPE_SPEECH_ENHANCE_ENABLED:
    case HA_CMD_TYPE_SPEECH_ENHANCE:
        {
            *prepend_len = 6;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = enable;
        }
        break;

    case HA_CMD_TYPE_WNR:
        {
            *prepend_len = 6;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = enable;
        }
        break;

    case HA_CMD_TYPE_INR:
        {
            *prepend_len = 6;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = enable;
        }
        break;

    case HA_CMD_TYPE_HA_VOLUME:
        {
            *prepend_len = 6;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = position;
        }
        break;

    case HA_CMD_TYPE_HA_OUTPUT_LIMITER:
        {
            *prepend_len = 6;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = enable;
        }
        break;

    case HA_CMD_TYPE_ADAPTIVE_BF:
        {
            *prepend_len = 6;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = enable;
        }
        break;

    case HA_CMD_TYPE_OVP_ENABLED:
        {
            *prepend_len = 6;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = enable;
        }
        break;

    case HA_CMD_TYPE_WDRC_0:
    case HA_CMD_TYPE_WDRC:
    case HA_CMD_TYPE_WDRC_ENABLED:
    case HA_CMD_TYPE_MIC_SETTING:
        {
            *prepend_len = 6;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = enable;
        }
        break;

    case HA_CMD_TYPE_HA_TEST:
        {
            *prepend_len = 6;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = enable;
        }
        break;

    case HA_CMD_TYPE_BAND_WIDTH:
        {
            *prepend_len = 7;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = direction;
            prepend_payload[6] = enable;
        }
        break;

    case HA_CMD_TYPE_FBC:
        {
            *prepend_len = 6;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = enable;
        }
        break;

    case HA_CMD_TYPE_OVP_PARAMS:
        {
            *prepend_len = 6;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = enable;
        }
        break;

    case HA_CMD_TYPE_ENV_CLASSIFICATION:
        {
            *prepend_len = 6;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = enable;
        }
        break;

    case HA_CMD_TYPE_DC_REMOVER:
        {
            *prepend_len = 9;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = direction;
            prepend_payload[6] = enable;
            prepend_payload[7] = filter_type;
            prepend_payload[8] = position;
        }
        break;

    case HA_CMD_TYPE_DEHOWLING:
        {
            *prepend_len = 6;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
            prepend_payload[5] = enable;
        }
        break;

    case HA_CMD_TYPE_BYPASS_HA_PROCESSING:
    case HA_CMD_TYPE_MIC_SWAP:
        {
            *prepend_len = 5;
            prepend_payload[0] = entity_id;
            memcpy(prepend_payload + 1, channel_mask, 4);
        }
        break;

    default:
        {

        }
        break;
    }
}

void app_ha_init_group_ltv_data(T_APP_HA_STEREO_TYPE type, uint8_t *group_ltv_data, uint8_t *data_l,
                                uint8_t *data_r, uint16_t params_len, uint8_t *prepend_payload, uint8_t prepend_len)
{
    if (type == HA_STEREO_TYPE_BOTH)
    {
        memcpy(group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - 2 * params_len - prepend_len, prepend_payload,
               prepend_len);

        if (data_l && params_len)
        {
            memcpy(group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - 2 * params_len, data_l, params_len);
        }

        if (data_r && params_len)
        {
            memcpy(group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - params_len, data_r, params_len);
        }

    }
    else if (type == HA_STEREO_TYPE_L)
    {
        memcpy(group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - params_len - prepend_len, prepend_payload,
               prepend_len);

        if (data_l && params_len)
        {
            memcpy(group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - params_len, data_l, params_len);
        }
    }
    else // HA_STEREO_TYPE_R
    {
        memcpy(group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - params_len - prepend_len, prepend_payload,
               prepend_len);

        if (data_r && params_len)
        {
            memcpy(group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - params_len, data_r, params_len);
        }
    }
}

uint8_t app_ha_type_to_data_type(uint16_t ha_type)
{
    uint8_t data_type = 0xFF;

    switch (ha_type)
    {
    case HA_CMD_TYPE_NR:
        {
            data_type = 0x0E;
        }
        break;

    case HA_CMD_TYPE_GRAPHIC_EQ:
        {
            data_type = 0x14;
        }
        break;

    case HA_CMD_TYPE_SPEECH_ENHANCE_ENABLED:
    case HA_CMD_TYPE_SPEECH_ENHANCE:
        {
            data_type = 0x15;
        }
        break;

    case HA_CMD_TYPE_WNR:
        {
            data_type = 0x1A;
        }
        break;
    case HA_CMD_TYPE_INR:
        {
            data_type = 0x1B;
        }
        break;
    case HA_CMD_TYPE_HA_VOLUME:
        {
            data_type = 0x0C;
        }
        break;

    case HA_CMD_TYPE_HA_OUTPUT_LIMITER:
        {
            data_type = 0x19;
        }
        break;

    case HA_CMD_TYPE_ADAPTIVE_BF:
        {
            data_type = 0x16;
        }
        break;

    case HA_CMD_TYPE_OVP_ENABLED:
        {
            data_type = 0x1F;
        }
        break;

    case HA_CMD_TYPE_WDRC_0:
    case HA_CMD_TYPE_WDRC:
    case HA_CMD_TYPE_WDRC_ENABLED:
    case HA_CMD_TYPE_MIC_SETTING:
        {
            data_type = 0x13;
        }
        break;

    case HA_CMD_TYPE_HA_TEST:
        {
            data_type = 0x17;
        }
        break;

    case HA_CMD_TYPE_BAND_WIDTH:
        {
            data_type = 0x12;
        }
        break;

    case HA_CMD_TYPE_FBC:
        {
            data_type = 0x1C;
        }
        break;

    case HA_CMD_TYPE_OVP_PARAMS:
        {
            data_type = 0x1F;
        }
        break;

    case HA_CMD_TYPE_ENV_CLASSIFICATION:
        {
            data_type = 0x20;
        }
        break;

    case HA_CMD_TYPE_DC_REMOVER:
        {
            data_type = 0x04;
        }
        break;

    case HA_CMD_TYPE_DEHOWLING:
        {
            data_type = 0x18;
        }
        break;

    case HA_CMD_TYPE_BYPASS_HA_PROCESSING:
    case HA_CMD_TYPE_MIC_SWAP:
        {
            data_type = 0x21;
        }
        break;

    default:
        {

        }
        break;
    }

    return data_type;
}

void app_ha_generate_group_ltv_data(T_APP_HA_STEREO_TYPE type, uint8_t *group_ltv_data,
                                    uint8_t prepend_len, uint16_t params_len, uint16_t ha_type)
{
    uint16_t total_payload_len = 0;
    uint16_t total_payload_offset = 0;
    uint16_t ltv_data_offset = 0;
    uint8_t ctrl = 0;
    uint8_t data_type = app_ha_type_to_data_type(ha_type);

    if (type == HA_STEREO_TYPE_BOTH)
    {
        total_payload_len = prepend_len + 2 *
                            params_len; //entity_id (1byte) + channel_mask (4bytes) + L, R data
    }
    else if (type == HA_STEREO_TYPE_L)
    {
        total_payload_len = prepend_len + params_len; //entity_id (1byte) + channel_mask (4bytes) + L data
    }
    else // HA_STEREO_TYPE_R
    {
        total_payload_len = prepend_len + params_len; //entity_id (1byte) + channel_mask (4bytes) + R data
    }

    if (total_payload_len <= HA_LTV_DATA_MAX_SIZE) //single
    {
        uint8_t ltv_data_header[HA_LTV_DATA_HEADER_SIZE] = {0};

        ltv_data_offset = 0; // dst_offset
        total_payload_offset = GROUP_LTV_DATA_MAX_SIZE - total_payload_len; //src_offset

        ltv_data_header[0] = HA_LTV_DATA_HEADER_SIZE - 1 + total_payload_len; //data_len
        ltv_data_header[1] = data_type;
        ltv_data_header[2] = 0; //version
        ltv_data_header[3] = 0; //ctrl (single)

        //copy data header
        memcpy(group_ltv_data + ltv_data_offset, ltv_data_header, HA_LTV_DATA_HEADER_SIZE);
        ltv_data_offset += HA_LTV_DATA_HEADER_SIZE;

        //copy total_payload
        memcpy(group_ltv_data + ltv_data_offset, group_ltv_data + total_payload_offset, total_payload_len);
        total_payload_offset += total_payload_len;

        return;
    }

    ctrl = 1; //start
    ltv_data_offset = 0; //dst_offset
    total_payload_offset = GROUP_LTV_DATA_MAX_SIZE - total_payload_len; //src_offset

    while (total_payload_offset < GROUP_LTV_DATA_MAX_SIZE)
    {
        if (ctrl == 1) // start
        {
            uint8_t ltv_start_type_header[HA_LTV_DATA_START_TYPE_HEADER_SIZE] = {0};

            ltv_start_type_header[0] = HA_LTV_DATA_START_TYPE_HEADER_SIZE - 1 +
                                       HA_LTV_DATA_START_TYPE_SIZE; //data_len
            ltv_start_type_header[1] = data_type;
            ltv_start_type_header[2] = 0; //version
            ltv_start_type_header[3] = ctrl;
            ltv_start_type_header[4] = total_payload_len & 0xFF;
            ltv_start_type_header[5] = (total_payload_len >> 8) & 0xFF;

            //copy start_type data_header
            memcpy(group_ltv_data + 0, ltv_start_type_header, HA_LTV_DATA_START_TYPE_HEADER_SIZE);
            ltv_data_offset += HA_LTV_DATA_START_TYPE_HEADER_SIZE;

            //copy segment of total_payload
            memcpy(group_ltv_data + ltv_data_offset, group_ltv_data + total_payload_offset,
                   HA_LTV_DATA_START_TYPE_SIZE);

            ltv_data_offset += HA_LTV_DATA_START_TYPE_SIZE;
            total_payload_offset += HA_LTV_DATA_START_TYPE_SIZE;
        }
        else if (ctrl == 2) //continue
        {
            uint8_t ltv_data_header[HA_LTV_DATA_HEADER_SIZE] = {0};

            ltv_data_header[0] = 255; //data_len
            ltv_data_header[1] = data_type;
            ltv_data_header[2] = 0; //version
            ltv_data_header[3] = ctrl;

            //copy data_header
            memcpy(group_ltv_data + ltv_data_offset, ltv_data_header, HA_LTV_DATA_HEADER_SIZE);
            ltv_data_offset += HA_LTV_DATA_HEADER_SIZE;

            //copy segment of total_payload
            memcpy(group_ltv_data + ltv_data_offset, group_ltv_data + total_payload_offset,
                   HA_LTV_DATA_MAX_SIZE);
            ltv_data_offset += HA_LTV_DATA_MAX_SIZE;
            total_payload_offset += HA_LTV_DATA_MAX_SIZE;
        }
        else //end
        {
            uint8_t ltv_data_header[HA_LTV_DATA_HEADER_SIZE] = {0};

            //4 - 1 + 338 - 250
            //total_payload_len - ltv_data_offset
            ltv_data_header[0] = HA_LTV_DATA_HEADER_SIZE - 1 + (GROUP_LTV_DATA_MAX_SIZE -
                                                                total_payload_offset); //data_len
            ltv_data_header[1] = data_type;
            ltv_data_header[2] = 0; //version
            ltv_data_header[3] = ctrl;

            memcpy(group_ltv_data + ltv_data_offset, ltv_data_header, HA_LTV_DATA_HEADER_SIZE);

            ltv_data_offset += HA_LTV_DATA_HEADER_SIZE;
            memcpy(group_ltv_data + ltv_data_offset, group_ltv_data + total_payload_offset,
                   GROUP_LTV_DATA_MAX_SIZE - total_payload_offset);

            ltv_data_offset += (GROUP_LTV_DATA_MAX_SIZE - total_payload_offset);
            total_payload_offset += (GROUP_LTV_DATA_MAX_SIZE - total_payload_offset);
        }

        if (GROUP_LTV_DATA_MAX_SIZE - total_payload_offset <= HA_LTV_DATA_MAX_SIZE)
        {
            ctrl = 3; //end
        }
        else
        {
            ctrl = 2; //continue
        }
    }

    return;
}

static uint16_t app_ha_get_spk_compensation(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_SPK_COMPENSATION);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_mic_compensation(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_MIC_COMPENSATION);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_mic_setting(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_MIC_SETTING);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_env_classification_params(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len,
                                            HA_DSP_FEATURE_HA_ENV_CLASSIFICATION);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                           offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_band_width_params(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_BAND_WIDTH);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_max_gain_setting(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_MAX_GAIN_SETTING);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_fbc_setting(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_FBC_SETTING);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_dc_remover(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_DC_REMOVER);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_ha_bypass_params(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_BYPASS_HA_PROCESSING);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_ha_dehowling_params(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_DEHOWLING_PARA);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_ha_mic_swap_params(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_MIC_SWAP);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_ha_apt_mic_gain_params(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_APT_MIC_GAIN);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_customer_eq(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_CUSTOMER_EQ);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_rns_compensation(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_RNS_COMPENSATION);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_dbfs(T_APP_HA_MODE ha_mode, uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_offset_get(ha_mode, &len, HA_DSP_FEATURE_HA_DBFS);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_audio_wdrc_params(uint8_t *payload)
{
    uint32_t offset = 0;
    uint16_t params_len = 0;

    offset = app_ha_dsp_ext_para_subtype_offset_get(&params_len, HA_DSP_FEATURE_HA_AUDIO_PARA,
                                                    (uint16_t)ha_audio_param_mapping_table[HA_AUDIO_PARAMS_WDRC]);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset - 4, payload,
                           params_len + 4);
    }

    return params_len + 4;
}

static uint16_t app_ha_get_audio_wdrc_enabled_params(uint8_t *payload)
{
    uint32_t offset = 0;
    uint16_t params_len = 0;

    offset = app_ha_dsp_ext_para_subtype_offset_get(&params_len, HA_DSP_FEATURE_HA_AUDIO_PARA,
                                                    (uint16_t)ha_audio_param_mapping_table[HA_AUDIO_PARAMS_WDRC_ENABLED]);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset - 4, payload,
                           params_len + 4);
    }

    return params_len + 4;
}

static uint16_t app_ha_get_audio_mic_settings_params(uint8_t *payload)
{
    uint32_t offset = 0;
    uint16_t params_len = 0;

    offset = app_ha_dsp_ext_para_subtype_offset_get(&params_len, HA_DSP_FEATURE_HA_AUDIO_PARA,
                                                    (uint16_t)ha_audio_param_mapping_table[HA_AUDIO_PARAMS_MIC_SETTINGS]);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset - 4, payload,
                           params_len + 4);
    }

    return params_len + 4;
}

static uint16_t app_ha_get_voice_wdrc_params(uint8_t *payload)
{
    uint32_t offset = 0;
    uint16_t params_len = 0;

    offset = app_ha_dsp_ext_para_subtype_offset_get(&params_len, HA_DSP_FEATURE_HA_VOICE_PARA,
                                                    (uint16_t)ha_voice_param_mapping_table[HA_VOICE_PARAMS_WDRC]);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset - 4, payload,
                           params_len + 4);
    }

    return params_len + 4;
}

static uint16_t app_ha_get_voice_wdrc_enabled_params(uint8_t *payload)
{
    uint32_t offset = 0;
    uint16_t params_len = 0;

    offset = app_ha_dsp_ext_para_subtype_offset_get(&params_len, HA_DSP_FEATURE_HA_VOICE_PARA,
                                                    (uint16_t)ha_voice_param_mapping_table[HA_VOICE_PARAMS_WDRC_ENABLED]);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset - 4, payload,
                           params_len + 4);
    }

    return params_len + 4;
}

static uint16_t app_ha_get_voice_mic_settings_params(uint8_t *payload)
{
    uint32_t offset = 0;
    uint16_t params_len = 0;

    offset = app_ha_dsp_ext_para_subtype_offset_get(&params_len, HA_DSP_FEATURE_HA_VOICE_PARA,
                                                    (uint16_t)ha_voice_param_mapping_table[HA_VOICE_PARAMS_MIC_SETTINGS]);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset - 4, payload,
                           params_len + 4);
    }

    return params_len + 4;
}

static uint16_t app_ha_get_audio_wdrc_enabled(uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_subtype_offset_get(&len, HA_DSP_FEATURE_HA_AUDIO_PARA,
                                                    ha_audio_param_mapping_table[HA_AUDIO_PARAMS_WDRC_ENABLED]);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_audio_mic_settings(uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_subtype_offset_get(&len, HA_DSP_FEATURE_HA_AUDIO_PARA,
                                                    ha_audio_param_mapping_table[HA_AUDIO_PARAMS_MIC_SETTINGS]);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_voice_wdrc(uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_subtype_offset_get(&len, HA_DSP_FEATURE_HA_VOICE_PARA,
                                                    ha_voice_param_mapping_table[HA_VOICE_PARAMS_WDRC]);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_voice_wdrc_enabled(uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_subtype_offset_get(&len, HA_DSP_FEATURE_HA_VOICE_PARA,
                                                    ha_voice_param_mapping_table[HA_VOICE_PARAMS_WDRC_ENABLED]);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static uint16_t app_ha_get_voice_mic_settings(uint8_t *payload)
{
    uint16_t len = 0;
    uint32_t offset = 0;

    offset = app_ha_dsp_ext_para_subtype_offset_get(&len, HA_DSP_FEATURE_HA_VOICE_PARA,
                                                    ha_voice_param_mapping_table[HA_VOICE_PARAMS_MIC_SETTINGS]);

    if (offset)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, payload, len);
    }

    return len;
}

static void app_ha_params_add(T_APP_HA_PARAMS type)
{
    T_APP_HA_PARAMS_ITEM *p_item = NULL;
    uint16_t len = 0;

    if (type >= HA_PARAMS_TOTAL)
    {
        return;
    }

    p_item = (T_APP_HA_PARAMS_ITEM *)malloc(sizeof(T_APP_HA_PARAMS_ITEM));

    if (p_item == NULL)
    {
        return;
    }

    p_item->p_next = NULL;
    p_item->type = type;
    p_item->len = ha_params_size_table[type];
    p_item->buf = NULL;

    switch (type)
    {
    case HA_PARAMS_RNS:
        {
            p_item->len = 12 + HA_PARAMS_RNS_SIZE;
            p_item->buf = calloc(p_item->len, 1);

            if (p_item->buf == NULL)
            {
                free(p_item);
                break;
            }

            len = app_ha_get_rns_params(HA_MODE_RHA, p_item->buf + 12);

            if (len)
            {
                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len) / 4;
                p_item->buf[3] = ((p_item->len) / 4) >> 8;
                p_item->buf[4] = HA_CMD_TYPE_RNS;
                p_item->buf[8] = HA_PARAMS_RNS_SIZE & 0xFF;
                p_item->buf[9] = (HA_PARAMS_RNS_SIZE & 0xFF00) >> 8;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item->buf);
                p_item->buf = NULL;
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    case HA_PARAMS_SPEECH_ENHANCE:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &len, HA_DSP_FEATURE_HA_SPEECH_ENHANCE);

            if (len > 0)
            {
                p_item->len = 12 + len;
                p_item->buf = calloc(p_item->len, 1);

                if (p_item->buf == NULL)
                {
                    free(p_item);
                    break;
                }

                len = app_ha_get_speech_enhance(HA_MODE_RHA, p_item->buf + 12);

                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len) / 4;
                p_item->buf[3] = ((p_item->len) / 4) >> 8;
                p_item->buf[4] = HA_CMD_TYPE_SPEECH_ENHANCE;
                p_item->buf[8] = len & 0xFF;
                p_item->buf[9] = (len & 0xFF00) >> 8;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    case HA_PARAMS_WDRC:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &len, HA_DSP_FEATURE_HA_WDRC_PARAMS);
            p_item->len = 8 + len;
            p_item->buf = calloc(p_item->len, 1);

            if (p_item->buf == NULL)
            {
                free(p_item);
                break;
            }

            len = app_ha_get_wdrc_params(HA_MODE_RHA, p_item->buf + 8);

            if (len)
            {
                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len - 4) / 4;
                p_item->buf[3] = ((p_item->len - 4) / 4) >> 8;
                p_item->buf[4] = HA_CMD_TYPE_WDRC;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item->buf);
                p_item->buf = NULL;
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    case HA_PARAMS_WDRC_ENABLED:
        {
            p_item->len = 8 + HA_PARAMS_WDRC_ENABLED_SIZE;
            p_item->buf = calloc(p_item->len, 1);

            if (p_item->buf == NULL)
            {
                free(p_item);
                break;
            }

            len = app_ha_get_wdrc_enabled_params(HA_MODE_RHA, p_item->buf + 8);

            if (len)
            {
                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len - 4) / 4;
                p_item->buf[3] = ((p_item->len - 4) / 4) >> 8;
                p_item->buf[4] = HA_CMD_TYPE_WDRC_ENABLED;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item->buf);
                p_item->buf = NULL;
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    case HA_PARAMS_HA_TEST:
        {
            p_item->len = 8 + HA_PARAMS_HA_TEST_SIZE;
            p_item->buf = calloc(p_item->len, 1);

            if (p_item->buf == NULL)
            {
                free(p_item);
                break;
            }

            len = app_ha_get_ha_test_params(HA_MODE_RHA, p_item->buf + 8);

            if (len)
            {
                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len - 4) / 4;
                p_item->buf[3] = ((p_item->len - 4) / 4) >> 8;
                p_item->buf[4] = HA_CMD_TYPE_HA_TEST;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item->buf);
                p_item->buf = NULL;
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    case HA_PARAMS_SPK_COMPENSATION:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &len, HA_DSP_FEATURE_HA_SPK_COMPENSATION);

            p_item->len = 4 + len;
            p_item->buf = calloc(p_item->len, 1);

            if (p_item->buf == NULL)
            {
                free(p_item);
                break;
            }

            len = app_ha_get_spk_compensation(HA_MODE_RHA, p_item->buf + 4);

            if (len > 0)
            {
                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len - 4) / 4;
                p_item->buf[3] = ((p_item->len - 4) / 4) >> 8;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item->buf);
                p_item->buf = NULL;
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    case HA_PARAMS_MIC_COMPENSATION:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &len, HA_DSP_FEATURE_HA_MIC_COMPENSATION);

            p_item->len = 4 + len;
            p_item->buf = calloc(p_item->len, 1);

            if (p_item->buf == NULL)
            {
                free(p_item);
                break;
            }

            len = app_ha_get_mic_compensation(HA_MODE_RHA, p_item->buf + 4);

            if (len > 0)
            {
                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len - 4) / 4;
                p_item->buf[3] = ((p_item->len - 4) / 4) >> 8;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item->buf);
                p_item->buf = NULL;
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    case HA_PARAMS_MIC_SETTING:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &len, HA_DSP_FEATURE_HA_MIC_SETTING);

            p_item->len = 12 + len;
            p_item->buf = calloc(p_item->len, 1);

            if (p_item->buf == NULL)
            {
                free(p_item);
                break;
            }

            len = app_ha_get_mic_setting(HA_MODE_RHA, p_item->buf + 12);

            if (len > 0)
            {
                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len) / 4;
                p_item->buf[3] = ((p_item->len) / 4) >> 8;
                p_item->buf[4] = HA_CMD_TYPE_MIC_SETTING;
                p_item->buf[8] = (len / 4) & 0xFF;
                p_item->buf[9] = ((len / 4) & 0xFF00) >> 8;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item->buf);
                p_item->buf = NULL;
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    case HA_PARAMS_ENV_CLASSIFICATION:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &len, HA_DSP_FEATURE_HA_ENV_CLASSIFICATION);

            p_item->len = 8 + len;
            p_item->buf = calloc(p_item->len, 1);

            if (p_item->buf == NULL)
            {
                free(p_item);
                break;
            }

            len = app_ha_get_env_classification_params(HA_MODE_RHA, p_item->buf + 8);

            if (len > 0)
            {
                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len - 4) / 4;
                p_item->buf[3] = ((p_item->len - 4) / 4) >> 8;
                p_item->buf[4] = HA_CMD_TYPE_ENV_CLASSIFICATION;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item->buf);
                p_item->buf = NULL;
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    case HA_PARAMS_BAND_WIDTH:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &len, HA_DSP_FEATURE_HA_BAND_WIDTH);

            p_item->len = 8 + len;
            p_item->buf = calloc(p_item->len, 1);

            if (p_item->buf == NULL)
            {
                free(p_item);
                break;
            }

            len = app_ha_get_band_width_params(HA_MODE_RHA, p_item->buf + 8);

            if (len > 0)
            {
                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len - 4) / 4;
                p_item->buf[3] = ((p_item->len - 4) / 4) >> 8;
                p_item->buf[4] = HA_CMD_TYPE_BAND_WIDTH;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item->buf);
                p_item->buf = NULL;
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    case HA_PARAMS_MAX_GAIN_SETTING:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &len, HA_DSP_FEATURE_HA_MAX_GAIN_SETTING);

            p_item->len = 4 + len;
            p_item->buf = calloc(p_item->len, 1);

            if (p_item->buf == NULL)
            {
                free(p_item);
                break;
            }

            len = app_ha_get_max_gain_setting(HA_MODE_RHA, p_item->buf + 4);

            if (len > 0)
            {
                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len - 4) / 4;
                p_item->buf[3] = ((p_item->len - 4) / 4) >> 8;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item->buf);
                p_item->buf = NULL;
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    case HA_PARAMS_FBC_SETTING:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &len, HA_DSP_FEATURE_HA_FBC_SETTING);

            p_item->len = 4 + len;
            p_item->buf = calloc(p_item->len, 1);

            if (p_item->buf == NULL)
            {
                free(p_item);
                break;
            }

            len = app_ha_get_fbc_setting(HA_MODE_RHA, p_item->buf + 4);

            if (len > 0)
            {
                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len - 4) / 4;
                p_item->buf[3] = ((p_item->len - 4) / 4) >> 8;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item->buf);
                p_item->buf = NULL;
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    case HA_PARAMS_HA_DBFS:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &len, HA_DSP_FEATURE_HA_DBFS);

            p_item->len = 4 + len;
            p_item->buf = calloc(p_item->len, 1);

            if (p_item->buf == NULL)
            {
                free(p_item);
                break;
            }

            len = app_ha_get_dbfs(HA_MODE_RHA, p_item->buf + 4);

            if (len)
            {
                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len - 4) / 4;
                p_item->buf[3] = ((p_item->len - 4) / 4) >> 8;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item->buf);
                p_item->buf = NULL;
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    case HA_PARAMS_DC_REMOVER:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &len, HA_DSP_FEATURE_HA_DC_REMOVER);

            p_item->len = 4 + len;
            p_item->buf = calloc(p_item->len, 1);

            if (p_item->buf == NULL)
            {
                free(p_item);
                break;
            }

            len = app_ha_get_dc_remover(HA_MODE_RHA, p_item->buf + 4);

            if (len)
            {
                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len - 4) / 4;
                p_item->buf[3] = ((p_item->len - 4) / 4) >> 8;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item->buf);
                p_item->buf = NULL;
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    case HA_PARAMS_DEHOWLING_PARA:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &len, HA_DSP_FEATURE_HA_DEHOWLING_PARA);

            p_item->len = 4 + len;
            p_item->buf = calloc(p_item->len, 1);

            if (p_item->buf == NULL)
            {
                free(p_item);
                break;
            }

            len = app_ha_get_ha_dehowling_params(HA_MODE_RHA, p_item->buf + 4);

            if (len)
            {
                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len - 4) / 4;
                p_item->buf[3] = ((p_item->len - 4) / 4) >> 8;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item->buf);
                p_item->buf = NULL;
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    case HA_PARAMS_MIC_SWAP:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &len, HA_DSP_FEATURE_HA_MIC_SWAP);

            p_item->len = 4 + len;
            p_item->buf = calloc(p_item->len, 1);

            if (p_item->buf == NULL)
            {
                free(p_item);
                break;
            }

            len = app_ha_get_ha_mic_swap_params(HA_MODE_RHA, p_item->buf + 4);

            if (len)
            {
                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len - 4) / 4;
                p_item->buf[3] = ((p_item->len - 4) / 4) >> 8;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item->buf);
                p_item->buf = NULL;
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    case HA_PARAMS_BYPASS_HA_PROCESSING:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &len, HA_DSP_FEATURE_HA_BYPASS_HA_PROCESSING);

            p_item->len = 4 + len;
            p_item->buf = calloc(p_item->len, 1);

            if (p_item->buf == NULL)
            {
                free(p_item);
                break;
            }

            len = app_ha_get_ha_bypass_params(HA_MODE_RHA, p_item->buf + 4);

            if (len)
            {
                p_item->buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                p_item->buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                p_item->buf[2] = (p_item->len - 4) / 4;
                p_item->buf[3] = ((p_item->len - 4) / 4) >> 8;

                os_queue_in(&ha_params_db.obj_queue, p_item);
            }
            else
            {
                free(p_item->buf);
                p_item->buf = NULL;
                free(p_item);
                p_item = NULL;
            }
        }
        break;

    default:
        {
            free(p_item);
            p_item = NULL;
        }
        break;
    }

    return;
}

static void app_ha_params_init()
{
    uint8_t i = 0;

    for (i = 0; i < HA_PARAMS_FEATURE_NUM; i++)
    {
        app_ha_params_add((T_APP_HA_PARAMS)ha_params_feature_table[i]);
    }

    return;
}

void app_ha_generate_gain_db(uint8_t volume_level, uint8_t balance_level, uint8_t *buf)
{
    uint8_t payload[4] = {0};
    uint16_t payload_len = 0;
    uint32_t offset = 0;
    int8_t volume_max_gain = 0;
    int8_t volume_min_gain = 0;
    int8_t balance_max_gain = 0;
    int16_t volume_gain = 0;
    int16_t balance_gain = 0;
    int16_t gain_db = 0;

    offset = app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &payload_len, HA_DSP_FEATURE_HA_DSP_CFG_GAIN);

    //get first 4 bytes
    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                       offset, payload, 4);

    volume_max_gain = (int8_t)payload[0];
    volume_min_gain = (int8_t)payload[1];
    balance_max_gain = (int8_t)payload[2];

    volume_gain = volume_min_gain * HA_SCALE_UP_GAIN_DB + ((volume_max_gain - volume_min_gain) *
                                                           volume_level * HA_SCALE_UP_GAIN_DB / 100);

    if ((app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT && balance_level < 50) ||
        (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT && balance_level > 50))
    {
        if (balance_level < 50)
        {
            balance_level = 50 - balance_level;
        }
        else
        {
            balance_level = balance_level - 50;
        }

        balance_gain = balance_max_gain * balance_level * HA_SCALE_UP_GAIN_DB / 50;
    }

    gain_db = volume_gain + balance_gain;

    memset(buf, 0, 12);

    buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    buf[2] = 2;
    buf[4] = 16; //volume type
    buf[8] = volume_level;
    buf[10] = gain_db & 0x00FF;
    buf[11] = (gain_db & 0xFF00) >> 8;
}

static uint32_t app_ha_dsp_prog_offset_get(uint16_t *payload_len, T_APP_HA_MODE mode)
{
    static bool is_init = false;
    static uint32_t prog_offset[HA_MODE_TOTAL] = {0};
    static uint16_t len[HA_MODE_TOTAL] = {0};

    if (is_init == false)
    {
        uint32_t offset = 0;
        T_HA_MODE_DB *cur_ptr = ha_mode_db;

        while (cur_ptr)
        {
            if (cur_ptr->mode == HA_MODE_RHA)
            {
                offset = HA_DSP_EXT_PARA_OFFSET;
            }
            else if (cur_ptr->mode == HA_MODE_ULLRHA)
            {
                offset = HA_DSP_EXT_PARA2_OFFSET;
            }

            prog_offset[cur_ptr->mode] = app_ha_dsp_feature_offset_get(&(len[cur_ptr->mode]), offset,
                                                                       HA_DSP_FEATURE_HA_PROG);
            cur_ptr = cur_ptr->next;
        }

        is_init = true;
    }

    if (is_init)
    {
        *payload_len = len[mode];

        return prog_offset[mode];
    }

    return 0;
}

static uint32_t app_ha_get_global_obj_base()
{
    uint16_t payload_len = 0;
    uint32_t offset = 0;
    uint8_t i = 0;

    offset = 0;

    for (i = 0; i < HA_GLOBAL_OBJ_TOTAL; i++)
    {
        offset += (4 + ha_global_obj_size_table[i]);
    }

    offset = HA_SIZE - offset;

    return offset;
}

static uint32_t app_ha_get_global_obj_offset(uint8_t target_obj_type)
{
    uint32_t offset = 0;
    uint8_t obj_type = 0;
    uint16_t obj_size = 0;
    uint8_t i = 0;

    for (i = 0; i < HA_GLOBAL_FEATURE_NUM; i++)
    {
        obj_type = ha_global_feature_table[i];
        obj_size = ha_global_obj_size_table[obj_type];

        if (obj_type == target_obj_type)
        {
            return offset;
        }

        offset += (4 + obj_size); // header(4bytes) | payload
    }

    return HA_INVALID_OFFSET;
}

static uint16_t app_ha_get_global_obj_size(uint8_t target_obj_type)
{
    uint32_t base = app_ha_get_global_obj_base();
    uint32_t offset = 0;
    uint16_t obj_size = 0;
    uint8_t buf[4] = {0};

    offset = app_ha_get_global_obj_offset(target_obj_type);

    if (offset == HA_INVALID_OFFSET)
    {
        return 0;
    }

    ha_ext_ftl_load_data(buf, base + offset, 4);

    obj_size = buf[2] + (buf[3] << 8);

    return obj_size;
}

void app_ha_get_global_object(T_APP_HA_GLOBAL_OBJ target_obj_type, uint8_t *buf, uint16_t *size)
{
    uint32_t base = app_ha_get_global_obj_base();
    uint8_t header_buf[4];
    uint8_t obj_set = 0;
    uint16_t obj_size = 0;
    uint32_t offset = 0;

    *size = 0;
    offset = 0;

    offset = app_ha_get_global_obj_offset(target_obj_type);

    if (offset == HA_INVALID_OFFSET)
    {
        return;
    }

    ha_ext_ftl_load_data(header_buf, base + offset, 4);
    obj_set = header_buf[1];
    obj_size = header_buf[2] + (header_buf[3] << 8);

    if (obj_size > ha_global_obj_size_table[target_obj_type] ||
        obj_size == 0 ||
        obj_set == 0)
    {
        return;
    }

    *size = obj_size;
    ha_ext_ftl_load_data(buf, base + offset + 4, obj_size);
}

bool app_ha_check_switch_listening_mode(uint8_t prog_id)
{
    return false;
}

void app_ha_generate_ovp_training_data_cmd(uint8_t *buf, uint8_t *data_buf)
{
    buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    buf[2] = 9;
    buf[4] = 0x43; //ovp training data type
    memcpy(buf + 8, data_buf, HA_GLOBAL_OBJ_OVP_TRAINING_DATA_SIZE);
}

void app_ha_push_rha_effect(T_HA_EFFECT_TYPE effect_type, uint16_t effect)
{
    T_APP_HA_CMD_ITEM *p_item = NULL;

    p_item = calloc(sizeof(T_APP_HA_CMD_ITEM), 1);
    p_item->effect_type = effect_type;
    p_item->effect = effect;
    p_item->p_next = NULL;
    os_queue_in(ha_cmd_db->queue, p_item);
}

//pop effect and push to ltv_data queue
bool app_ha_pop_rha_effect(T_HA_EFFECT_TYPE *effect_type, uint16_t *effect)
{
    bool ret = false;
    T_APP_HA_CMD_ITEM *p_item = NULL;

    if (ha_cmd_db->queue->count == 0)
    {
        return ret;
    }

    p_item = os_queue_out(ha_cmd_db->queue);

    if (p_item)
    {
        *effect_type = p_item->effect_type;
        *effect = p_item->effect;
        free(p_item);

        ret = true;
    }

    return ret;
}

void app_ha_push_audio_effect(T_HA_EFFECT_TYPE effect_type, uint16_t effect)
{
    T_APP_HA_CMD_ITEM *p_item = NULL;

    p_item = calloc(sizeof(T_APP_HA_CMD_ITEM), 1);
    p_item->effect_type = effect_type;
    p_item->effect = effect;
    p_item->p_next = NULL;
    os_queue_in(audio_cmd_db->queue, p_item);
}

//pop effect and push to ltv_data queue
bool app_ha_pop_audio_effect(T_HA_EFFECT_TYPE *effect_type, uint16_t *effect)
{
    bool ret = false;
    T_APP_HA_CMD_ITEM *p_item = NULL;

    if (audio_cmd_db->queue->count == 0)
    {
        return ret;
    }

    p_item = os_queue_out(audio_cmd_db->queue);

    if (p_item)
    {
        *effect_type = p_item->effect_type;
        *effect = p_item->effect;
        free(p_item);

        ret = true;
    }

    return ret;
}

void app_ha_push_voice_effect(T_HA_EFFECT_TYPE effect_type, uint16_t effect)
{
    T_APP_HA_CMD_ITEM *p_item = NULL;

    p_item = calloc(sizeof(T_APP_HA_CMD_ITEM), 1);
    p_item->effect_type = effect_type;
    p_item->effect = effect;
    p_item->p_next = NULL;
    os_queue_in(voice_cmd_db->queue, p_item);
}

//pop effect and push to ltv_data queue
bool app_ha_pop_voice_effect(T_HA_EFFECT_TYPE *effect_type, uint16_t *effect)
{
    bool ret = false;
    T_APP_HA_CMD_ITEM *p_item = NULL;

    if (voice_cmd_db->queue->count == 0)
    {
        return ret;
    }

    p_item = os_queue_out(voice_cmd_db->queue);

    if (p_item)
    {
        *effect_type = p_item->effect_type;
        *effect = p_item->effect;
        free(p_item);

        ret = true;
    }

    return ret;
}

void app_ha_push_ullrha_effect(T_HA_EFFECT_TYPE effect_type, uint16_t effect)
{
    T_APP_HA_ULL_EFFECT_ITEM *p_item = NULL;

    p_item = os_queue_peek(ha_ull_effect_db->queue, ha_ull_effect_db->count);

    if (p_item == NULL)
    {
        p_item = calloc(sizeof(T_APP_HA_ULL_EFFECT_ITEM), 1);
        p_item->effect_type = effect_type;
        p_item->effect = effect;
        p_item->p_next = NULL;

        os_queue_in(ha_ull_effect_db->queue, p_item);
    }
    else
    {
        p_item->effect_type = effect_type;
        p_item->effect = effect;
    }

    ha_ull_effect_db->count += 1;
}

bool app_ha_pop_ullrha_effect(T_HA_EFFECT_TYPE *effect_type, uint16_t *effect)
{
    if (ha_ull_effect_db->sel_idx < ha_ull_effect_db->count)
    {
        T_APP_HA_ULL_EFFECT_ITEM *p_item = NULL;

        p_item = os_queue_peek(ha_ull_effect_db->queue, ha_ull_effect_db->sel_idx);
        *effect_type = p_item->effect_type;
        *effect = p_item->effect;

        ha_ull_effect_db->sel_idx += 1;

        return true;
    }
    else
    {
        ha_ull_effect_db->sel_idx = 0;
        ha_ull_effect_db->count = 0;
    }

    return false;
}

void app_ha_generate_wdrc_enabled_cmd(uint8_t *buf, uint16_t version, uint16_t is_enabled)
{
    memset(buf, 0, 12);

    buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    buf[2] = 3;
    buf[4] = HA_CMD_TYPE_WDRC_ENABLED;
    buf[8] = version & 0xFF;
    buf[9] = version >> 8;
    buf[10] = is_enabled & 0xFF;
    buf[11] = is_enabled >> 8;
}

uint16_t app_ha_get_params_size(uint16_t type)
{
    T_APP_HA_PARAMS_ITEM *p_item = NULL;
    uint16_t size = 0;

    p_item = os_queue_peek(&ha_params_db.obj_queue, 0);

    while (p_item)
    {
        if (p_item->len == 0)
        {
            break;
        }

        if (p_item->type == type)
        {
            size = p_item->len;
        }

        p_item = p_item->p_next;
    }

    return size;
}

void app_ha_get_params(uint16_t type, uint8_t *data_buf, uint16_t *size)
{
    T_APP_HA_PARAMS_ITEM *p_item = NULL;

    p_item = os_queue_peek(&ha_params_db.obj_queue, 0);

    while (p_item)
    {
        if (p_item->len == 0)
        {
            break;
        }

        if (p_item->type == type)
        {
            *size = p_item->len;
            memcpy(data_buf, p_item->buf, p_item->len);
        }

        p_item = p_item->p_next;
    }
}

void app_ha_global_obj_get(T_APP_HA_GLOBAL_OBJ type, uint8_t *data_buf)
{
    uint32_t base = app_ha_get_global_obj_base();
    uint32_t offset = 0;
    uint16_t obj_size = 0;
    uint8_t buf[4] = {0};
    uint8_t i = 0;

    if (type >= HA_GLOBAL_OBJ_TOTAL)
    {
        return;
    }

    offset = app_ha_get_global_obj_offset(type);

    if (offset == HA_INVALID_OFFSET)
    {
        return;
    }

    ha_ext_ftl_load_data(buf, base + offset, 4);

    if (buf[1]) //is_set
    {
        obj_size = buf[2] + (buf[3] << 8);
        ha_ext_ftl_load_data(data_buf, base + offset + 4, obj_size);
    }
}

void app_ha_global_obj_set(T_APP_HA_GLOBAL_OBJ type, uint8_t *data_buf, uint16_t len)
{
    uint32_t base = app_ha_get_global_obj_base();
    uint8_t buf[4] = {0};
    uint32_t offset = 0;
    uint8_t i = 0;

    if (type >= HA_GLOBAL_OBJ_TOTAL)
    {
        return;
    }

    offset = app_ha_get_global_obj_offset(type);

    if (offset == HA_INVALID_OFFSET || len == 0)
    {
        return;
    }

    if (len == 0 || len > ha_global_obj_size_table[type])
    {
        return;
    }

    //size not set
    ha_ext_ftl_load_data(buf, base + offset, 4);
    buf[1] = 1;
    buf[2] = len & 0xFF;
    buf[3] = (len >> 8) & 0xFF;
    ha_ext_ftl_save_data(buf, base + offset, 4);

    ha_ext_ftl_save_data(data_buf, base + offset + 4, ha_global_obj_size_table[type]);
}

void app_ha_load_effect_data(T_HA_EFFECT_TYPE effect_type, uint16_t type, uint8_t *data_buf)
{
    uint16_t size = 0;

    switch (effect_type)
    {
    case HA_EFFECT_TYPE_PROG_OBJ:
        {
            if (app_ha_prog_get_object_size(type) > HA_MODE_PARAMS_BUF_SIZE)
            {
                break;
            }

            switch (type)
            {
            case HA_PROG_OBJ_VOLUME_LEVEL:
                {
                    if (current_ha_mode_db->mode == HA_MODE_RHA)
                    {
                        if (app_apt_is_apt_on_state(app_db.current_listening_state))
                        {
                            app_ha_push_rha_effect(HA_EFFECT_TYPE_PROG_OBJ, type);
                        }
                    }
                    else //HA_MODE_ULLRHA
                    {
                        app_ha_push_ullrha_effect(HA_EFFECT_TYPE_PROG_OBJ, type);
                    }
                }
                break;

            case HA_PROG_OBJ_NR:
            case HA_PROG_OBJ_WDRC:
            case HA_PROG_OBJ_GRAPHIC_EQ:
            case HA_PROG_OBJ_FBC:
            case HA_PROG_OBJ_SPEECH_ENHANCE:
            case HA_PROG_OBJ_BF:
            case HA_PROG_OBJ_WNR:
            case HA_PROG_OBJ_INR:
            case HA_PROG_OBJ_OUTPUT_DRC:
            case HA_PROG_OBJ_ADAPTIVE_BF:
            case HA_PROG_OBJ_OVP:
                {
                    if (current_ha_mode_db-> mode == HA_MODE_RHA)
                    {
                        app_ha_push_rha_effect(HA_EFFECT_TYPE_PROG_OBJ, type);
                    }
                    else //HA_MODE_ULLRHA
                    {
                        app_ha_push_ullrha_effect(HA_EFFECT_TYPE_PROG_OBJ, type);
                    }
                }
                break;

            default:
                {

                }
                break;
            }
        }
        break;

    case HA_EFFECT_TYPE_GLOBAL_OBJ:
        {
            if (type == HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS)
            {
                uint8_t *data_buf = calloc(HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS_SIZE, 1);

                app_ha_global_obj_get(HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS, data_buf);
                current_ha_mode_db->prog_db->current_prog_is_sync_status =
                    data_buf[current_ha_mode_db->prog_db->selectable_prog_idx];
                app_ha_global_obj_set(HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS, data_buf,
                                      HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS_SIZE);

                free(data_buf);
                data_buf = NULL;

                break;
            }

            if (current_ha_mode_db-> mode == HA_MODE_RHA)
            {
                switch (type)
                {
                case HA_GLOBAL_OBJ_RHA_WDRC:
                case HA_GLOBAL_OBJ_RHA_WDRC_ENABLED:
                case HA_GLOBAL_OBJ_OVP_TRAINING_DATA:
                    {
                        app_ha_push_rha_effect(HA_EFFECT_TYPE_GLOBAL_OBJ, type);
                    }
                    break;
                }
            }
            else //HA_MODE_ULLRHA
            {
                app_ha_push_ullrha_effect(effect_type, type);
            }
        }
        break;

    case HA_EFFECT_TYPE_DSP_PARAMS_OBJ:
        {
            if (app_ha_get_params_size(type) > HA_MODE_PARAMS_BUF_SIZE)
            {
                break;
            }

            app_ha_get_params(type, data_buf, &size);

            if (size == 0)
            {
                break;
            }
            if (current_ha_mode_db-> mode == HA_MODE_RHA)
            {
                app_ha_push_rha_effect(effect_type, type);
            }
            else //HA_MODE_ULLRHA
            {
                app_ha_push_ullrha_effect(effect_type, type);
            }
        }
        break;

    default:
        {

        }
        break;
    }
}

void app_ha_prog_load(uint8_t prog_id)
{
    uint8_t *data_buf = calloc(HA_MODE_PARAMS_BUF_SIZE, 1);

    if (current_ha_mode_db->mode == HA_MODE_RHA &&
        !((audio_passthrough_state_get() == AUDIO_PASSTHROUGH_STATE_STARTED) &&
          app_apt_is_apt_on_state(app_db.current_listening_state)))
    {
        free(data_buf);
        data_buf = NULL;

        return;
    }

    if (current_ha_mode_db-> mode == HA_MODE_RHA)
    {
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_MIC_SETTING, data_buf);

        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_VOLUME_LEVEL, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_NR, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_WDRC, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_GRAPHIC_EQ, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_FBC, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_SPEECH_ENHANCE, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_BF, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_WNR, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_INR, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_OUTPUT_DRC, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_ADAPTIVE_BF, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_OVP, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_MAX_TOTAL_POWER_OUTPUT_CTRL, data_buf);

        app_ha_load_effect_data(HA_EFFECT_TYPE_GLOBAL_OBJ, HA_GLOBAL_OBJ_RHA_WDRC, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_GLOBAL_OBJ, HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_GLOBAL_OBJ, HA_GLOBAL_OBJ_OVP_TRAINING_DATA, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_GLOBAL_OBJ, HA_GLOBAL_OBJ_RHA_WDRC_ENABLED, data_buf);

        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_RNS, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_SPEECH_ENHANCE, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_HA_TEST, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_SPK_COMPENSATION, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_MIC_COMPENSATION, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_ENV_CLASSIFICATION, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_BAND_WIDTH, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_MAX_GAIN_SETTING, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_FBC_SETTING, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_DC_REMOVER, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_DEHOWLING_PARA, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_MIC_SWAP, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_BYPASS_HA_PROCESSING, data_buf);
        app_ha_send_rha_effect();
    }
    else //HA_MODE_ULLRHA
    {
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_MIC_SETTING, data_buf);

        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_VOLUME_LEVEL, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_NR, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_WDRC, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_GRAPHIC_EQ, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_FBC, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_SPEECH_ENHANCE, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_BF, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_WNR, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_INR, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_OUTPUT_DRC, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_ADAPTIVE_BF, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_OVP, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_MAX_TOTAL_POWER_OUTPUT_CTRL, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_INTEGRATED_EQ, data_buf);

        app_ha_load_effect_data(HA_EFFECT_TYPE_GLOBAL_OBJ, HA_GLOBAL_OBJ_ULLRHA_WDRC, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_GLOBAL_OBJ, HA_GLOBAL_OBJ_ULLRHA_WDRC_ENABLED, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_GLOBAL_OBJ, HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS, data_buf);

        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_RNS, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_SPEECH_ENHANCE, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_HA_TEST, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_SPK_COMPENSATION, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_MIC_COMPENSATION, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_ENV_CLASSIFICATION, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_BAND_WIDTH, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_MAX_GAIN_SETTING, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_FBC_SETTING, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_DEHOWLING_PARA, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_MIC_SWAP, data_buf);
        app_ha_load_effect_data(HA_EFFECT_TYPE_DSP_PARAMS_OBJ, HA_PARAMS_BYPASS_HA_PROCESSING, data_buf);
    }

    free(data_buf);
    data_buf = NULL;
}

bool app_ha_dsp_prog_is_set()
{
    T_APP_HA_DSP_EXT_PARA ext_para;

    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                       HA_DSP_FEATURE_BITS_OFFSET,
                       (uint8_t *)&ext_para, sizeof(T_APP_HA_DSP_EXT_PARA));

    return (bool)ext_para.ha_scenario;
}

bool app_ha_rha_is_enable()
{
    T_APP_HA_DSP_EXT_PARA ext_para;

    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                       HA_DSP_FEATURE_BITS_OFFSET,
                       (uint8_t *)&ext_para, sizeof(T_APP_HA_DSP_EXT_PARA));

    return (bool)ext_para.ha_enable;
}

bool app_ha_ullrha_is_enable()
{
    T_APP_HA_DSP_EXT_PARA ext_para;

    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                       HA_DSP_FEATURE_BITS_OFFSET,
                       (uint8_t *)&ext_para, sizeof(T_APP_HA_DSP_EXT_PARA));

    return (bool)ext_para.ullrha_enable;
}

static void app_ha_dsp_prog_header_get(uint8_t *buf, T_APP_HA_MODE mode)
{
    uint16_t payload_len;

    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                       app_ha_dsp_prog_offset_get(&payload_len, mode),
                       buf, HA_PROG_LIST_HEADER_LEN);
}

void app_ha_global_obj_reset(T_APP_HA_GLOBAL_OBJ type)
{
    uint32_t base = app_ha_get_global_obj_base();
    uint8_t buf[4] = {0};
    uint32_t offset = 0;
    uint8_t i = 0;

    if (type >= HA_GLOBAL_OBJ_TOTAL)
    {
        return;
    }

    offset = 0;

    for (i = 0; i < type; i++)
    {
        offset += (4 + ha_global_obj_size_table[i]);
    }

    ha_ext_ftl_load_data(buf, base + offset, 4);
    buf[1] = 0;
    ha_ext_ftl_save_data(buf, base + offset, 4);
}

static bool app_ha_dsp_prog_load()
{
    uint16_t payload_len;
    uint16_t base = 0;
    uint32_t dsp_prog_base = 0;
    uint8_t buf[HA_BLOCK] = {0};
    uint16_t offset = 0;
    uint8_t i = 0;

    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                       app_ha_dsp_prog_offset_get(&payload_len, current_ha_mode_db->mode),
                       buf, HA_PROG_LIST_HEADER_LEN);

    if (buf[0] == 0) //check prog num
    {
        return false;
    }

    base = app_ha_get_prog_base(current_ha_mode_db->mode);
    offset = 0;
    memset(buf, 0, HA_BLOCK);
    dsp_prog_base = app_ha_dsp_prog_offset_get(&payload_len, current_ha_mode_db->mode);

    for (i = 0; i < payload_len / HA_BLOCK; i++)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                           dsp_prog_base + offset,
                           buf, HA_BLOCK);

        ha_ext_ftl_save_data(buf, base + offset, HA_BLOCK);
        offset += HA_BLOCK;
    }

    if (payload_len % HA_BLOCK)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                           dsp_prog_base + offset,
                           buf, payload_len % HA_BLOCK);
        ha_ext_ftl_save_data(buf, base + offset, payload_len % HA_BLOCK);
    }

    return true;
}

static uint32_t app_ha_dsp_spk_response_offset_get(uint16_t *payload_len)
{
    static bool is_init = false;
    static uint32_t offset = 0;
    static uint16_t len = 0;

    if (is_init)
    {
        *payload_len = len;

        return offset;
    }

    offset = app_ha_dsp_feature_offset_get(payload_len, HA_DSP_EXT_PARA_OFFSET,
                                           HA_DSP_FEATURE_HA_SPK_RESPONSE);
    len = *payload_len;
    is_init = true;

    return offset;
}

static bool app_ha_dsp_spk_response_load(uint8_t *payload, uint16_t *payload_len)
{
    uint32_t offset = app_ha_dsp_spk_response_offset_get(payload_len);

    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                       offset, payload, *payload_len);

    return true;
}

static uint8_t app_ha_version_get()
{
    uint8_t buf[HA_PROG_LIST_HEADER_LEN] = {0};

    if (app_ha_dsp_prog_is_set())
    {
        app_ha_dsp_prog_header_get(buf, current_ha_mode_db->mode);

        return buf[3];
    }

    return HA_VERSION;
}

static uint8_t app_ha_prog_num_is_match()
{
    uint8_t dsp_buf[HA_PROG_LIST_HEADER_LEN] = {0};
    uint8_t buf[HA_PROG_LIST_HEADER_LEN] = {0};

    if (app_ha_rha_is_enable())
    {
        app_ha_dsp_prog_header_get(dsp_buf, HA_MODE_RHA);
        ha_ext_ftl_load_data(buf, app_ha_get_prog_base(HA_MODE_RHA), HA_PROG_LIST_HEADER_LEN);

        if (dsp_buf[0] != buf[0])
        {
            return false;
        }
    }

    if (app_ha_ullrha_is_enable())
    {
        app_ha_dsp_prog_header_get(dsp_buf, HA_MODE_ULLRHA);

        if (ha_ext_ftl_load_data(buf, app_ha_get_prog_base(HA_MODE_ULLRHA), HA_PROG_LIST_HEADER_LEN))
        {
            return false;
        }

        if (dsp_buf[0] != buf[0])
        {
            return false;
        }
    }

    return true;
}


static bool app_ha_prog_db_load()
{
    uint8_t buf[HA_BLOCK] = {0};
    uint8_t i = 0;
    uint16_t prog_start_offset = 0;

    ha_ext_ftl_load_data(buf, app_ha_get_prog_base(current_ha_mode_db->mode), HA_BLOCK);

    // check prog num
    if (buf[0] > 0)
    {
        current_ha_mode_db->prog_db->prog_num = buf[0];

        if (buf[1] != 0xFF)
        {
            current_ha_mode_db->prog_db->permanent_prog_idx = buf[1];
        }

        if (buf[2] != 0xFF)
        {
            prev_selectable_prog_idx[current_ha_mode_db->mode] = buf[2];
            current_ha_mode_db->prog_db->selectable_prog_idx = buf[2];
        }

        prog_start_offset = app_ha_get_prog_base(current_ha_mode_db->mode) + HA_PROG_LIST_HEADER_LEN;

        for (i = 0; i < current_ha_mode_db->prog_db->prog_num; i++)
        {
            uint16_t prog_size = buf[4 + i * 2] + (buf[4 + i * 2 + 1] << 8);

            current_ha_mode_db->prog_db->prog_offset_arr[i] = prog_start_offset;
            current_ha_mode_db->prog_db->prog_size_arr[i] = prog_size;

            //set raw data offset to next prog
            prog_start_offset += prog_size;
        }

        return true;
    }

    return false;
}

void app_ha_clear_param()
{
    uint8_t buf[HA_BLOCK] = {0};
    uint8_t i = 0;
    uint16_t base = 0;

    memset(buf, 0, HA_BLOCK);

    for (i = 0; i < HA_SIZE / HA_BLOCK; i++)
    {
        ha_ext_ftl_save_data(buf, base, HA_BLOCK);
        base += HA_BLOCK;
    }

    if (HA_SIZE % HA_BLOCK > 0)
    {
        ha_ext_ftl_save_data(buf, base, HA_SIZE % HA_BLOCK);
    }
}

void app_ha_check_init()
{
    uint8_t buf[HA_BLOCK] = {0};
    uint32_t status = 0;

    status = ha_ext_ftl_load_data(buf, 0, HA_BLOCK);

    if (status)
    {
        app_ha_clear_param();
    }
}

bool app_ha_prog_init()
{
    uint8_t buf[HA_PROG_OPTION] = {0};
    uint32_t status = 0;
    uint8_t i = 0;
    bool is_init = false;

    status = ha_ext_ftl_load_data(buf, app_ha_get_prog_base(current_ha_mode_db->mode),
                                  HA_PROG_OPTION);

    if (status)
    {
        return false;
    }

    // check HA version, HA program num
    if (buf[3] != app_ha_version_get() ||
        (app_ha_prog_num_is_match() == false))
    {
        //dsp_config is expected to enable HA scenario flag
        if (app_ha_dsp_prog_is_set())
        {
            app_ha_dsp_prog_load();
        }
        else
        {
            return false;
        }
    }

    is_init = app_ha_prog_db_load();

    return is_init;
}

void app_ha_prog_get_object_status(uint8_t prog_id, uint8_t type, uint8_t *data)
{
    uint8_t obj_data[HA_PROG_OBJ_TOOL_DATA_SIZE] = {0};
    uint16_t obj_size = 0;

    app_ha_prog_get_object(prog_id, HA_PROG_OBJ_TOOL_DATA, obj_data, &obj_size);

    switch (type)
    {
    case HA_PROG_OBJ_NR:
        {
            memcpy(data, obj_data + 112, 4);
        }
        break;

    case HA_PROG_OBJ_SPEECH_ENHANCE:
        {
            memcpy(data, obj_data + 108, 4);
        }
        break;

    case HA_PROG_OBJ_BF:
        {
            memcpy(data, obj_data + 104, 4);
        }
        break;

    default:
        {

        }
        break;
    }
}

void app_ha_prog_set_object_with_tool_data(uint8_t prog_id, uint8_t type, uint8_t *data)
{
    uint8_t obj_data[HA_PROG_OBJ_TOOL_DATA_SIZE] = {0};
    uint16_t obj_size = 0;

    app_ha_prog_get_object(prog_id, HA_PROG_OBJ_TOOL_DATA, obj_data, &obj_size);

    switch (type)
    {
    case HA_PROG_OBJ_NR:
        {
            memcpy(obj_data + HA_PROG_OBJ_TOOL_DATA_SIZE - 16, data + 8, 4);
            app_ha_prog_set_object(prog_id, HA_PROG_OBJ_TOOL_DATA, obj_data);
            app_ha_prog_set_object(prog_id, HA_PROG_OBJ_NR, data);
        }
        break;

    case HA_PROG_OBJ_SPEECH_ENHANCE:
        {
            memcpy(obj_data + HA_PROG_OBJ_TOOL_DATA_SIZE - 20, data + 8, 4);
            app_ha_prog_set_object(prog_id, HA_PROG_OBJ_TOOL_DATA, obj_data);
            app_ha_prog_set_object(prog_id, HA_PROG_OBJ_SPEECH_ENHANCE, data);
        }
        break;

    case HA_PROG_OBJ_BF:
        {
            memcpy(obj_data + HA_PROG_OBJ_TOOL_DATA_SIZE - 24, data + 8, 4);
            app_ha_prog_set_object(prog_id, HA_PROG_OBJ_TOOL_DATA, obj_data);
            app_ha_prog_set_object(prog_id, HA_PROG_OBJ_BF, data);
        }
        break;

    default:
        {

        }
        break;
    }


}

void app_ha_generate_nr_cmd(uint8_t is_enabled, uint8_t level, uint8_t *buf)
{
    buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    buf[2] = 2;
    buf[4] = 4; //nr type
    buf[8] = is_enabled;
    buf[9] = level;
}

void app_ha_generate_speech_enhance_cmd(uint8_t is_enabled, uint8_t level, uint8_t *buf)
{
    buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    buf[2] = 2;
    buf[4] = 5; //speech enhance type
    buf[8] = is_enabled;
    buf[9] = level;
}

void app_ha_generate_bf_cmd(uint8_t is_enabled, uint8_t *buf)
{
    buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    buf[2] = 2;
    buf[4] = 6; //bf type
    buf[8] = is_enabled;
}

void app_ha_generate_ovp_training_cmd(uint8_t is_start, uint8_t *buf)
{
    buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    buf[2] = 2;
    buf[4] = 0x42; //ovp training ctrl type
    buf[8] = is_start;
}

void app_ha_generate_classify_cmd(uint8_t is_enable, uint8_t *buf, uint16_t len)
{
    if (app_ha_get_env_classification_params(HA_MODE_RHA, buf + 8))
    {
        buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
        buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
        buf[2] = (len - 4) / 4;
        buf[3] = ((len - 4) / 4) >> 8;
        buf[4] = HA_CMD_TYPE_ENV_CLASSIFICATION;
        buf[len - 4] &= (is_enable | 0xFE);
    }
}

bool app_ha_generate_rha_rns_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_rns_params(HA_MODE_RHA, params_buf + 12);

    if (params_len == 0)
    {
        return false;
    }

    *len = 12 + HA_PARAMS_RNS_SIZE;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len) / 4;
    params_buf[3] = ((*len) / 4) >> 8;
    params_buf[4] = HA_CMD_TYPE_RNS;
    params_buf[8] = HA_PARAMS_RNS_SIZE & 0xFF;
    params_buf[9] = (HA_PARAMS_RNS_SIZE & 0xFF00) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_rha_speech_enhance_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_speech_enhance(HA_MODE_RHA, params_buf + 12);

    if (params_len == 0)
    {
        return false;
    }

    *len = 12 + params_len;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len) / 4;
    params_buf[3] = ((*len) / 4) >> 8;
    params_buf[4] = HA_CMD_TYPE_SPEECH_ENHANCE;
    params_buf[8] = params_len & 0xFF;
    params_buf[9] = (params_len & 0xFF00) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_rha_wdrc_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_wdrc_params(HA_MODE_RHA, params_buf + 8);

    if (params_len == 0)
    {
        return false;
    }

    *len = 8 + params_len;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) / 4;
    params_buf[3] = ((*len - 4) / 4) >> 8;
    params_buf[4] = HA_CMD_TYPE_WDRC;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_rha_wdrc_enabled_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_wdrc_enabled_params(HA_MODE_RHA, params_buf + 8);

    if (params_len == 0)
    {
        return false;
    }

    *len = 8 + params_len;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) / 4;
    params_buf[3] = ((*len - 4) / 4) >> 8;
    params_buf[4] = HA_CMD_TYPE_WDRC_ENABLED;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_rha_test_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_ha_test_params(HA_MODE_RHA, params_buf + 8);

    if (params_len == 0)
    {
        return false;
    }

    *len = 8 + params_len;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) / 4;
    params_buf[3] = ((*len - 4) / 4) >> 8;
    params_buf[4] = HA_CMD_TYPE_HA_TEST;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_rha_spk_compensation_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_spk_compensation(HA_MODE_RHA, params_buf + 4);

    if (params_len == 0)
    {
        return false;
    }

    *len = 4 + params_len;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) / 4;
    params_buf[3] = ((*len - 4) / 4) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_rha_mic_compensation_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_mic_compensation(HA_MODE_RHA, params_buf + 4);

    if (params_len == 0)
    {
        return false;
    }

    *len = 4 + params_len;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) / 4;
    params_buf[3] = ((*len - 4) / 4) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_rha_mic_setting_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_mic_setting(HA_MODE_RHA, params_buf + 12);

    if (params_len == 0)
    {
        return false;
    }

    *len = 12 + params_len;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) / 4;
    params_buf[3] = ((*len - 4) / 4) >> 8;
    params_buf[4] = HA_CMD_TYPE_MIC_SETTING;
    params_buf[8] = (params_len / 4) & 0xFF;
    params_buf[9] = ((params_len / 4) & 0xFF00) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_rha_env_classification_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_env_classification_params(HA_MODE_RHA, params_buf + 8);

    if (params_len == 0)
    {
        return false;
    }

    *len = 8 + params_len;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) / 4;
    params_buf[3] = ((*len - 4) / 4) >> 8;
    params_buf[4] = HA_CMD_TYPE_ENV_CLASSIFICATION;
    *buf = params_buf;

    return true;
}

bool app_ha_generate_rha_band_width_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_band_width_params(HA_MODE_RHA, params_buf + 8);

    if (params_len == 0)
    {
        return false;
    }

    *len = 8 + params_len;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) / 4;
    params_buf[3] = ((*len - 4) / 4) >> 8;
    params_buf[4] = HA_CMD_TYPE_BAND_WIDTH;
    *buf = params_buf;

    return true;
}

bool app_ha_generate_rha_max_gain_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_max_gain_setting(HA_MODE_RHA, params_buf + 4);

    if (params_len == 0)
    {
        return false;
    }

    *len = 4 + params_len;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) / 4;
    params_buf[3] = ((*len - 4) / 4) >> 8;
    *buf = params_buf;

    return true;
}

bool app_ha_generate_rha_fbc_setting_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_fbc_setting(HA_MODE_RHA, params_buf + 4);

    if (params_len == 0)
    {
        return false;
    }

    *len = 4 + params_len;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) / 4;
    params_buf[3] = ((*len - 4) / 4) >> 8;
    *buf = params_buf;

    return true;
}

bool app_ha_generate_rha_dc_remover_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_dc_remover(HA_MODE_RHA, params_buf + 4);

    if (params_len == 0)
    {
        return false;
    }

    *len = 4 + params_len;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) / 4;
    params_buf[3] = ((*len - 4) / 4) >> 8;
    *buf = params_buf;

    return true;
}

bool app_ha_generate_rha_dbfs_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_dbfs(HA_MODE_RHA,
                                 params_buf + 4); //only store dbfs params in Normal RHA section

    if (params_len == 0)
    {
        return false;
    }

    *len = params_len + 4;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_rha_bypass_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_ha_bypass_params(HA_MODE_RHA, params_buf + 4);

    if (params_len == 0)
    {
        return false;
    }

    *len = 4 + params_len;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) / 4;
    params_buf[3] = ((*len - 4) / 4) >> 8;
    *buf = params_buf;

    return true;
}

bool app_ha_generate_rha_dehowling_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_ha_dehowling_params(HA_MODE_RHA, params_buf + 4);

    if (params_len == 0)
    {
        return false;
    }

    *len = 4 + params_len;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) / 4;
    params_buf[3] = ((*len - 4) / 4) >> 8;
    *buf = params_buf;

    return true;
}

bool app_ha_generate_rha_mic_swap_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_ha_mic_swap_params(HA_MODE_RHA, params_buf + 4);

    if (params_len == 0)
    {
        return false;
    }

    *len = 4 + params_len;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) / 4;
    params_buf[3] = ((*len - 4) / 4) >> 8;
    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_rns_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_rns_params(HA_MODE_ULLRHA, params_buf + 12);

    if (params_len == 0)
    {
        return false;
    }

    *len = params_len + 12;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = AUDIO_PROBE_HA_PARA >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;
    params_buf[4] = 15; //RNS params type
    params_buf[8] = HA_PARAMS_RNS_SIZE & 0xFF;
    params_buf[9] = (HA_PARAMS_RNS_SIZE & 0xFF00) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_sppech_enhancement_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_speech_enhance(HA_MODE_ULLRHA, params_buf + 12);

    if (params_len == 0)
    {
        return false;
    }

    *len = params_len + 12;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = AUDIO_PROBE_HA_PARA >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;
    params_buf[4] = 19; //SPEECH ENHANCE type
    params_buf[8] = params_len & 0xFF;
    params_buf[9] = params_len >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_wdrc_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_wdrc_params(HA_MODE_ULLRHA, params_buf + 8);

    if (params_len == 0)
    {
        return false;
    }

    *len = params_len + 8;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;
    params_buf[4] = HA_CMD_TYPE_WDRC;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_wdrc_enabled_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_wdrc_enabled_params(HA_MODE_ULLRHA, params_buf + 8);

    if (params_len == 0)
    {
        return false;
    }

    *len = params_len + 8;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;
    params_buf[4] = HA_CMD_TYPE_WDRC_ENABLED;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_test_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_ha_test_params(HA_MODE_ULLRHA, params_buf + 8);

    if (params_len == 0)
    {
        return false;
    }

    *len = params_len + 8;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;
    params_buf[4] = 8; //HA TEST params type

    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_spk_compensation_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_spk_compensation(HA_MODE_ULLRHA, params_buf + 4);

    if (params_len == 0)
    {
        return false;
    }

    *len = params_len + 4;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_mic_compensation_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_mic_compensation(HA_MODE_ULLRHA, params_buf + 4);

    if (params_len == 0)
    {
        return false;
    }

    *len = params_len + 4;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_mic_setting_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_mic_setting(HA_MODE_ULLRHA, params_buf + 12);

    if (params_len == 0)
    {
        return false;
    }

    *len = params_len + 12;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;
    params_buf[4] = 27; //MIC SETTING type
    params_buf[8] = (params_len / 4) & 0xFF;
    params_buf[9] = (params_len / 4) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_max_gain_setting_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_max_gain_setting(HA_MODE_ULLRHA, params_buf + 4);

    if (params_len == 0)
    {
        return false;
    }

    *len = params_len + 4;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_fbc_setting_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_fbc_setting(HA_MODE_ULLRHA, params_buf + 4);

    if (params_len == 0)
    {
        return false;
    }

    *len = params_len + 4;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_dc_remover_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_dc_remover(HA_MODE_ULLRHA, params_buf + 4);

    if (params_len == 0)
    {
        return false;
    }

    *len = params_len + 4;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_dbfs_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_dbfs(HA_MODE_RHA,
                                 params_buf + 4); //only store dbfs params in Normal RHA section

    if (params_len == 0)
    {
        return false;
    }

    *len = params_len + 4;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_bypass_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_ha_bypass_params(HA_MODE_ULLRHA, params_buf + 4);

    if (params_len == 0)
    {
        return false;
    }

    *len = params_len + 4;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_dehowling_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_ha_dehowling_params(HA_MODE_ULLRHA, params_buf + 4);

    if (params_len == 0)
    {
        return false;
    }

    *len = params_len + 4;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_mic_swap_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    params_len = app_ha_get_ha_mic_swap_params(HA_MODE_ULLRHA, params_buf + 4);

    if (params_len == 0)
    {
        return false;
    }

    *len = params_len + 4;

    params_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
    params_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_init_finish_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);

    *len = 4;

    params_buf[0] = HA_ADSP_CMD_INIT_FINISH & 0xFF;
    params_buf[1] = HA_ADSP_CMD_INIT_FINISH >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_ullrha_boot_ack_cmd(uint8_t **buf, uint16_t *len)
{
    uint16_t params_len = 0;

    if (params_buf == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);

    *len = 4;

    params_buf[0] = HA_ADSP_CMD_BOOT_ACK & 0xFF;
    params_buf[1] = HA_ADSP_CMD_BOOT_ACK >> 8;
    params_buf[2] = (*len - 4) & 0xFF;
    params_buf[3] = (*len - 4) >> 8;

    *buf = params_buf;

    return true;
}

bool app_ha_generate_audio_wdrc_cmd(uint8_t **buf, uint16_t *params_len)
{
    if (params_buf == NULL || params_len == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    *params_len = app_ha_get_audio_wdrc_params(params_buf);

    if (*params_len == 0)
    {
        return false;
    }

    *buf = params_buf;

    return true;
}

bool app_ha_generate_audio_wdrc_enabled_cmd(uint8_t **buf, uint16_t *params_len)
{
    if (params_buf == NULL || params_len == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    *params_len = app_ha_get_audio_wdrc_enabled_params(params_buf);

    if (*params_len == 0)
    {
        return false;
    }

    *buf = params_buf;

    return true;
}

bool app_ha_generate_audio_mic_settings_cmd(uint8_t **buf, uint16_t *params_len)
{
    if (params_buf == NULL || params_len == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    *params_len = app_ha_get_audio_mic_settings_params(params_buf);

    if (*params_len == 0)
    {
        return false;
    }

    *buf = params_buf;

    return true;
}

bool app_ha_generate_voice_wdrc_cmd(uint8_t **buf, uint16_t *params_len)
{
    if (params_buf == NULL || params_len == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    *params_len = app_ha_get_voice_wdrc_params(params_buf);

    if (*params_len == 0)
    {
        return false;
    }

    *buf = params_buf;

    return true;
}

bool app_ha_generate_voice_wdrc_enabled_cmd(uint8_t **buf, uint16_t *params_len)
{
    if (params_buf == NULL || params_len == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    *params_len = app_ha_get_voice_wdrc_enabled_params(params_buf);

    if (*params_len == 0)
    {
        return false;
    }

    *buf = params_buf;

    return true;
}

bool app_ha_generate_voice_mic_settings_cmd(uint8_t **buf, uint16_t *params_len)
{
    if (params_buf == NULL || params_len == NULL)
    {
        return false;
    }

    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    *params_len = app_ha_get_voice_mic_settings_params(params_buf);

    if (*params_len == 0)
    {
        return false;
    }

    *buf = params_buf;

    return true;
}

bool app_ha_params_to_global_obj(T_APP_HA_MODE mode, T_APP_HA_PARAMS params_type,
                                 T_APP_HA_GLOBAL_OBJ global_obj)
{
    uint32_t base = app_ha_get_global_obj_base();
    uint32_t params_offset = 0;
    uint32_t global_obj_offset = 0;
    uint8_t buf[4] = {0};
    bool ret = false;
    uint16_t len = 0;
    uint8_t *params_buf = NULL;

    global_obj_offset = app_ha_get_global_obj_offset(global_obj);

    //Need to separate normal HA / ULLRHA ext_params to laod speficy params_type into correspond FTL
    //output buffer is expected to contain global object format payload
    switch (params_type)
    {
    case HA_PARAMS_WDRC:
        {
            ret = mode == HA_MODE_RHA ? app_ha_generate_rha_wdrc_cmd(&params_buf, &len) :
                  mode == HA_MODE_ULLRHA ? app_ha_generate_ullrha_wdrc_cmd(&params_buf, &len) : false;
        }
        break;
    case HA_PARAMS_WDRC_ENABLED:
        {
            ret = mode == HA_MODE_RHA ? app_ha_generate_rha_wdrc_enabled_cmd(&params_buf, &len) :
                  mode == HA_MODE_ULLRHA ? app_ha_generate_ullrha_wdrc_enabled_cmd(&params_buf, &len) : false;
        }
        break;

    default:
        {

        }
        break;
    }

    if (params_buf == NULL || len == 0)
    {
        return false;
    }

    buf[0] = global_obj;
    buf[1] = 1;
    buf[2] = len & 0xFF;
    buf[3] = len >> 8;
    ha_ext_ftl_save_data(buf, base + global_obj_offset, 4);
    ha_ext_ftl_save_data(params_buf, base + global_obj_offset + 4, len);

    return ret;
}

bool app_ha_audio_params_to_global_obj(T_APP_HA_AUDIO_PARAMS params_type,
                                       T_APP_HA_GLOBAL_OBJ global_obj)
{
    uint32_t params_offset = 0;
    bool ret = false;
    uint16_t len = 0;
    uint8_t *params_buf = NULL;

    switch (params_type)
    {
    case HA_AUDIO_PARAMS_WDRC:
        {
            ret = app_ha_generate_audio_wdrc_cmd(&params_buf, &len);
        }
        break;

    case HA_AUDIO_PARAMS_WDRC_ENABLED:
        {
            ret = app_ha_generate_audio_wdrc_enabled_cmd(&params_buf, &len);
        }
        break;

    case HA_AUDIO_PARAMS_MIC_SETTINGS:
        {
            ret = app_ha_generate_audio_mic_settings_cmd(&params_buf, &len);
        }
        break;

    default:
        {

        }
        break;
    }

    if (params_buf == NULL || len == 0)
    {
        return false;
    }

    app_ha_global_obj_set(global_obj, params_buf, len);
    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    app_ha_global_obj_get(global_obj, params_buf);

    return ret;
}

bool app_ha_voice_params_to_global_obj(T_APP_HA_VOICE_PARAMS params_type,
                                       T_APP_HA_GLOBAL_OBJ global_obj)
{
    uint32_t params_offset = 0;
    bool ret = false;
    uint16_t len = 0;
    uint8_t *params_buf = NULL;

    switch (params_type)
    {
    case HA_VOICE_PARAMS_WDRC:
        {
            ret = app_ha_generate_voice_wdrc_cmd(&params_buf, &len);
        }
        break;

    case HA_VOICE_PARAMS_WDRC_ENABLED:
        {
            ret = app_ha_generate_voice_wdrc_enabled_cmd(&params_buf, &len);
        }
        break;

    case HA_VOICE_PARAMS_MIC_SETTINGS:
        {
            ret = app_ha_generate_voice_mic_settings_cmd(&params_buf, &len);
        }
        break;

    default:
        {

        }
        break;
    }

    if (params_buf == NULL || len == 0)
    {
        return false;
    }

    app_ha_global_obj_set(global_obj, params_buf, len);
    memset(params_buf, 0, HA_MODE_PARAMS_BUF_SIZE);
    app_ha_global_obj_get(global_obj, params_buf);

    return ret;
}

uint32_t app_ha_check_global_obj_colume_is_match()
{
    uint32_t base = 0;
    uint32_t offset = 0;
    uint8_t obj_type = 0;
    uint8_t obj_set = 0;
    uint16_t cur_obj_size = 0;
    uint16_t obj_size = 0;
    uint32_t ret = 0;
    uint8_t i = 0;
    uint8_t cur_buf[4] = {0};
    uint8_t buf[4] = {0};

    base = app_ha_get_global_obj_base();

    if (HA_GLOBAL_FEATURE_NUM == 0)
    {
        return 0xFFFFFFFF;
    }

    offset = 0;
    obj_set = 0;

    for (i = 0; i < HA_GLOBAL_FEATURE_NUM; i++)
    {
        obj_type = ha_global_feature_table[i];
        obj_size = ha_global_obj_size_table[obj_type];

        buf[0] = obj_type;
        buf[1] = obj_set;
        buf[2] = obj_size;
        buf[3] = obj_size >> 8;

        ret = ha_ext_ftl_load_data(cur_buf, base + offset, 4);

        if (ret)
        {
            return ret;
        }

        // check header is matched or not
        if (buf[0] != cur_buf[0])
        {
            return 1;
        }

        cur_obj_size = buf[2] + (buf[3] >> 8);

        if (cur_obj_size > obj_size)
        {
            return 1;
        }

        offset += (4 + obj_size);
    }

    return ret;
}

bool app_ha_set_ha_mode(T_APP_HA_MODE mode)
{
    uint8_t idx = (uint8_t)mode;
    uint8_t data_buf[4] = {0};

    if (ullrha_is_enable == false &&
        mode == HA_MODE_ULLRHA)
    {
        return false;
    }

    if (idx < HA_MODE_TOTAL)
    {
        current_ha_mode = mode;
        current_ha_mode_db = ha_mode_db + idx;
        current_ha_mode_db->mode = mode;

        data_buf[0] = idx;
        data_buf[1] = 0;
        data_buf[2] = 0;
        data_buf[3] = 0;

        app_ha_global_obj_set(HA_GLOBAL_OBJ_HA_MODE, data_buf, HA_GLOBAL_OBJ_HA_MODE_SIZE);

        return true;
    }

    return false;
}

T_APP_HA_MODE app_ha_get_ha_mode()
{
    return current_ha_mode;
}

void app_ha_global_obj_init()
{
    uint32_t base = 0;
    uint32_t offset = 0;
    uint8_t obj_type = 0;
    uint8_t obj_set = 0;
    uint16_t obj_size = 0;
    uint8_t i = 0;
    uint8_t buf[4] = {0};
    uint8_t data_buf[HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS_SIZE] = {0};
    T_APP_HA_PARAMS_ITEM *p_item = NULL;

    base = app_ha_get_global_obj_base();

    if (HA_GLOBAL_FEATURE_NUM == 0)
    {
        return;
    }

    if (app_ha_check_global_obj_colume_is_match() != 0) //need to initilize global obj columns
    {
        offset = 0;
        obj_set = 0;

        for (i = 0; i < HA_GLOBAL_FEATURE_NUM; i++)
        {
            obj_type = ha_global_feature_table[i];
            obj_size = ha_global_obj_size_table[obj_type];

            buf[0] = obj_type;
            buf[1] = obj_set;
            buf[2] = obj_size;
            buf[3] = obj_size >> 8;

            ha_ext_ftl_save_data(buf, base + offset, 4);
            offset += (4 + obj_size);
        }

        data_buf[0] = 0; //0: normal HA, 1: ULL HA mode
        data_buf[1] = 0;
        data_buf[2] = 0;
        data_buf[3] = 0;

        app_ha_global_obj_set(HA_GLOBAL_OBJ_HA_MODE, data_buf, HA_GLOBAL_OBJ_HA_MODE_SIZE);
        app_ha_global_obj_get(HA_GLOBAL_OBJ_HA_MODE, data_buf);

        //initialize global object from dsp config ext params
        app_ha_params_to_global_obj(HA_MODE_RHA, HA_PARAMS_WDRC, HA_GLOBAL_OBJ_RHA_WDRC);
        app_ha_params_to_global_obj(HA_MODE_RHA, HA_PARAMS_WDRC_ENABLED, HA_GLOBAL_OBJ_RHA_WDRC_ENABLED);

        if (ullrha_is_enable)
        {
            //initialize global object from dsp config ext params
            app_ha_params_to_global_obj(HA_MODE_ULLRHA, HA_PARAMS_WDRC, HA_GLOBAL_OBJ_ULLRHA_WDRC);
            app_ha_params_to_global_obj(HA_MODE_ULLRHA, HA_PARAMS_WDRC_ENABLED,
                                        HA_GLOBAL_OBJ_ULLRHA_WDRC_ENABLED);
        }

        app_ha_audio_params_to_global_obj(HA_AUDIO_PARAMS_WDRC, HA_GLOBAL_OBJ_AUDIO_WDRC);
        app_ha_audio_params_to_global_obj(HA_AUDIO_PARAMS_WDRC_ENABLED, HA_GLOBAL_OBJ_AUDIO_WDRC_ENABLE);
        app_ha_audio_params_to_global_obj(HA_AUDIO_PARAMS_MIC_SETTINGS, HA_GLOBAL_OBJ_AUDIO_MIC_SETTINGS);

        app_ha_voice_params_to_global_obj(HA_VOICE_PARAMS_WDRC, HA_GLOBAL_OBJ_VOICE_WDRC);
        app_ha_voice_params_to_global_obj(HA_VOICE_PARAMS_WDRC_ENABLED, HA_GLOBAL_OBJ_VOICE_WDRC_ENABLE);
        app_ha_voice_params_to_global_obj(HA_VOICE_PARAMS_MIC_SETTINGS, HA_GLOBAL_OBJ_VOICE_MIC_SETTINGS);


        memset(data_buf, !app_cfg_const.rws_disallow_sync_apt_volume,
               HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS_SIZE);
        app_ha_global_obj_set(HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS, data_buf,
                              HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS_SIZE);

        memset(buf, HA_PRESET_NOT_CONFIGURED, HA_GLOBAL_OBJ_PRESET_IDX_SIZE);
        app_ha_global_obj_set(HA_GLOBAL_OBJ_PRESET_IDX, buf, HA_GLOBAL_OBJ_PRESET_IDX_SIZE);
    }

    app_ha_global_obj_get(HA_GLOBAL_OBJ_HA_MODE, data_buf);

    if (app_ha_set_ha_mode((T_APP_HA_MODE)(data_buf[0])) == false)
    {
        app_ha_set_ha_mode(HA_MODE_RHA);
    }
}

static void app_ha_get_dsp_cfg_gain(void)
{
    uint8_t payload[132] = {0};
    uint16_t payload_len = 0;
    uint32_t offset = 0;
    uint32_t ext_para_offset = HA_DSP_EXT_PARA_OFFSET;

    if (current_ha_mode_db->mode == HA_MODE_ULLRHA)
    {
        ext_para_offset = HA_DSP_EXT_PARA2_OFFSET;
    }

    offset = app_ha_dsp_feature_offset_get(&payload_len, ext_para_offset,
                                           HA_DSP_FEATURE_HA_DSP_CFG_GAIN);
    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                       offset, payload, payload_len);
    app_report_event_broadcast(EVENT_HA_DSP_CFG_GAIN_INFO, payload, payload_len);
}

static void app_ha_get_spk_response(bool is_remote)
{
    uint8_t payload[100] = {0};
    uint16_t payload_len = 0;

    app_ha_dsp_spk_response_load(payload, &payload_len);

    if (!is_remote)
    {
        app_report_event_broadcast(EVENT_HA_SPK_RESPONSE_INFO, payload, payload_len);
    }
    else
    {
        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                            APP_REMOTE_MSG_HA_PROGRAM_RELAY_SPK_RESPONSE,
                                            payload,
                                            payload_len);
    }

    return;
}

static uint8_t app_ha_get_volume_level(bool is_vol_up, uint8_t current_level)
{
    static uint8_t data[20] = {0};
    static bool is_init = false;
    static uint16_t data_len = 0;
    uint32_t offset = 0;
    uint8_t level = 0xFF; // 0xFF: no need to change level
    uint8_t i = 0;

    if (is_init == false)
    {
        offset = app_ha_dsp_feature_offset_get(&data_len, HA_DSP_EXT_PARA_OFFSET,
                                               HA_DSP_FEATURE_HA_MMI_VOLUME_STEP);
        is_init = true;

        if (data_len)
        {
            fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) + offset, data,
                               data_len);
        }
    }

    if (data_len)
    {
        if (is_vol_up)
        {
            for (i = 4; i < 4 + data[0]; i++)
            {
                if (data[i] > current_level)
                {
                    level = data[i];
                    break;
                }
            }
        }
        else
        {
            for (i = 4 + data[0] - 1; i >= 4; i--)
            {
                if (data[i] < current_level)
                {
                    level = data[i];
                    break;
                }
            }
        }
    }

    return level;
}

uint8_t app_ha_get_volume_sync()
{
    return current_ha_mode_db->prog_db->current_prog_is_sync_status;
}

static void app_ha_sync_volume_sync()
{
    uint8_t buf[HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS_SIZE] = {0};
    uint16_t size = 0;
    bool is_sync = false;

    if ((app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_PRIMARY) ||
        (app_db.remote_session_state != REMOTE_SESSION_STATE_CONNECTED))
    {
        return;
    }

    app_ha_global_obj_get(HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS, buf);

    // 0 is unsync, 1 is sync
    is_sync = (bool)buf[current_ha_mode_db->prog_db->selectable_prog_idx];

    if (is_sync == false)
    {
        return;
    }

    uint8_t data[HA_PROG_OBJ_VOLUME_LEVEL_SIZE] = {0};

    app_ha_prog_get_object(current_ha_mode_db->prog_db->selectable_prog_idx,
                           HA_PROG_OBJ_VOLUME_LEVEL, data, &size);

    //send current earbud's volume
    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                        APP_REMOTE_MSG_HA_PROGRAM_RELAY_HA_VOLUME_STATUS,
                                        data, 1);
}

void app_ha_get_all_direction_global_obj(uint8_t *ctrl_data)
{
    uint16_t ctrl_data_len = ctrl_data[0] + (ctrl_data[1] >> 8);

    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                        APP_REMOTE_MSG_HA_GLOBAL_GET_ALL_DIRECTION_OBJ,
                                        ctrl_data, ctrl_data_len);
}

void app_ha_get_all_direction_obj(uint8_t prog_id, uint8_t type)
{
    uint16_t obj_size = app_ha_prog_get_object_size(type);
    uint16_t event_len = 5 + 2 * obj_size;
    uint16_t offset = 5;
    uint8_t *event_data = calloc(event_len, sizeof(uint8_t));

    event_data[0] = prog_id;
    event_data[1] = HA_PROG_OPCODE_GET_ALL_DIRECTION_OBJ;
    event_data[2] = type;
    event_data[3] = obj_size;
    event_data[4] = obj_size >> 8;

    if (type == HA_PROG_OBJ_VOLUME_LEVEL) //set default volume value to 0xFF
    {
        uint8_t default_volume_buf[HA_PROG_OBJ_VOLUME_LEVEL_SIZE] = {0xFF, 0xFF, 0x00, 0x00};
        memcpy(event_data + offset, default_volume_buf, HA_PROG_OBJ_VOLUME_LEVEL_SIZE);
        memcpy(event_data + offset + obj_size, default_volume_buf, HA_PROG_OBJ_VOLUME_LEVEL_SIZE);
    }

    if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT)
    {
        offset += obj_size;
    }

    if (((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY) &&
         (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)) ||
        (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED))
    {
        if (offset < event_len)
        {
            app_ha_prog_get_object(prog_id, type, event_data + offset, &obj_size);
        }
    }

    if ((app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SINGLE) &&
        (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
    {
        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                            APP_REMOTE_MSG_HA_PROGRAM_GET_ALL_DIRECTION_OBJ,
                                            event_data, event_len);
    }
    else
    {
        app_report_event_broadcast(EVENT_HA_PROGRAM_INFO, event_data + 1, event_len - 1);
    }

    free(event_data);
}

void app_ha_get_all_direction_obj_status(uint8_t prog_id, uint8_t type)
{
    switch (type)
    {
    case HA_PROG_OBJ_VOLUME_LEVEL:
        {
            uint16_t event_len = 5; //prog_id, buds' status, L level, R level, balance
            uint16_t offset = 2;
            uint8_t *event_data = calloc(event_len, sizeof(uint8_t));
            uint8_t data[4] = {0};
            uint16_t size = 0;

            event_data[0] = prog_id;

            app_ha_prog_get_object(prog_id, HA_PROG_OBJ_VOLUME_LEVEL, data, &size);

            event_data[offset + app_cfg_const.bud_side] = data[0]; //set curr bud's volume
            event_data[offset + 2] = data[1]; //set curr bud's balance

            if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
            {
                event_data[1] = 2;
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                    APP_REMOTE_MSG_HA_PROGRAM_GET_HA_VOL,
                                                    event_data, event_len);
            }
            else
            {
                event_data[1] = app_cfg_const.bud_side;
                app_report_event_broadcast(EVENT_HA_PROGRAM_VOL_STATUS_INFO, event_data + 1, event_len - 1);
            }

            free(event_data);
            event_data = NULL;
        }
        break;

    default:
        {

        }
        break;
    }
}

static void *app_ha_get_audio_track()
{
    if (app_a2dp_is_streaming())
    {
        T_APP_BR_LINK *p_link = NULL;

        p_link = &app_db.br_link[app_a2dp_get_active_idx()];
        return p_link->a2dp_track_handle;
    }

    return NULL;
}

static void app_ha_get_audio_volume()
{
    void *a2dp_track_handle = app_ha_get_audio_track();
    T_AUDIO_TRACK_STATE state = AUDIO_TRACK_STATE_RELEASED;

    app_audio_hearing_state_get(&state);

    if (a2dp_track_handle)
    {
        audio_track_signal_out_monitoring_start(a2dp_track_handle,
                                                HA_AUDIO_VOLUME_REFRESH_INTERVAL,
                                                0,
                                                NULL);
    }
    else if (state == AUDIO_TRACK_STATE_STARTED)
    {
        audio_track_signal_out_monitoring_start(app_audio_hearing_track_handle_get(),
                                                HA_AUDIO_VOLUME_REFRESH_INTERVAL,
                                                0,
                                                NULL);
    }

    return;
}

bool app_ha_get_bud_is_hearing_test()
{
    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        return bud_is_hearing_test;
    }

    return false;
}

bool app_ha_get_is_hearing_test()
{
    return is_hearing_test;
}

void app_ha_set_is_hearing_test(bool status)
{
    is_hearing_test = status;

    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
    {
        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                            APP_REMOTE_MSG_HA_PROGRAM_RELAY_HEARING_TEST_STATUS,
                                            (uint8_t *)&is_hearing_test,
                                            1);
    }
}

void app_ha_clear_rha_effect()
{
    T_APP_HA_LTV_DATA_ITEM *p_item = NULL;

    while (ha_ltv_data_db.obj_queue.count)
    {
        p_item = os_queue_out(&ha_ltv_data_db.obj_queue);
        free(p_item);
    }

    ha_ltv_data_db.seq_num = 0;
}

void app_ha_clear_audio_effect()
{
    T_APP_HA_LTV_DATA_ITEM *p_item = NULL;

    while (audio_ltv_data_db.obj_queue.count)
    {
        p_item = os_queue_out(&audio_ltv_data_db.obj_queue);
        free(p_item);
    }

    audio_ltv_data_db.seq_num = 0;
}

void app_ha_clear_voice_effect()
{
    T_APP_HA_LTV_DATA_ITEM *p_item = NULL;

    while (voice_ltv_data_db.obj_queue.count)
    {
        p_item = os_queue_out(&voice_ltv_data_db.obj_queue);
        free(p_item);
    }

    voice_ltv_data_db.seq_num = 0;
}

void app_ha_split_ltv_data(T_OS_QUEUE *p_queue)
{
    uint16_t offset = 0;

    while (offset < GROUP_LTV_DATA_MAX_SIZE)
    {
        if (group_ltv_data[offset] > 0) //ltv_data start addr
        {
            T_APP_HA_LTV_DATA_ITEM *p_item = (T_APP_HA_LTV_DATA_ITEM *)malloc(sizeof(T_APP_HA_LTV_DATA_ITEM));

            p_item->p_next = NULL;
            p_item->buf = group_ltv_data + offset;
            p_item->len = group_ltv_data[offset] + 1;
            os_queue_in(p_queue, p_item);
        }
        else
        {
            break;
        }

        offset += 256;
    }
}

void app_ha_generate_ltv_data_from_h2d_cmd(T_APP_HA_STEREO_TYPE stereo_type, uint8_t *buf,
                                           uint16_t buf_len, uint8_t *prepend_len, uint16_t *ha_type)
{
    uint8_t *ptr_l = NULL;
    uint8_t *ptr_r = NULL;
    uint8_t prepend_payload[12] = {0};

    if (stereo_type == HA_STEREO_TYPE_BOTH)
    {
        ptr_l = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - 2 * buf_len;
        ptr_r = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - buf_len;
    }
    else if (stereo_type == HA_STEREO_TYPE_L)
    {
        ptr_l  = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - buf_len;
    }
    else // HA_STEREO_TYPE_R
    {
        ptr_r = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - buf_len;
    }
    if (stereo_type == HA_STEREO_TYPE_BOTH)
    {
        memcpy(ptr_l, buf, buf_len);
        memcpy(ptr_r, buf, buf_len);
    }
    else if (stereo_type == HA_STEREO_TYPE_L)
    {
        memcpy(ptr_l, buf, buf_len);
    }
    else // HA_STEREO_TYPE_R
    {
        memcpy(ptr_r, buf, buf_len);
    }

    if (ptr_l)
    {
        *ha_type = ptr_l[4] + (ptr_l[5] << 8);
        app_ha_fill_prepend_payload(stereo_type, *ha_type, prepend_payload, prepend_len);
    }
    else
    {
        *ha_type = ptr_r[4] + (ptr_r[5] << 8);
        app_ha_fill_prepend_payload(stereo_type, *ha_type, prepend_payload, prepend_len);
    }

    app_ha_init_group_ltv_data(stereo_type, group_ltv_data, ptr_l, ptr_r, buf_len, prepend_payload,
                               *prepend_len);
}

void app_ha_push_rha_runtime_effect(T_APP_HA_STEREO_TYPE stereo_type, uint8_t *buf,
                                    uint16_t buf_len)
{
    uint8_t prepend_len = 0;
    uint16_t ha_type = 0;
    uint16_t params_offset = 0;
    uint8_t idx = 0;

    if (buf == NULL ||
        buf_len < 8)
    {
        return;
    }

    app_ha_generate_ltv_data_from_h2d_cmd(stereo_type, buf, buf_len, &prepend_len, &ha_type);

    //initialize group_ltv_data from effect, push ltv_data
    app_ha_generate_group_ltv_data(HA_STEREO_TYPE_L, group_ltv_data, prepend_len, buf_len, ha_type);

    while (params_offset < buf_len)
    {
        params_offset += 100;
        idx += 1;
    }

    app_ha_split_ltv_data(&ha_ltv_data_db.obj_queue);
}

void app_ha_push_audio_runtime_effect(T_APP_HA_STEREO_TYPE stereo_type, uint8_t *buf,
                                      uint16_t buf_len)
{
    uint8_t prepend_len = 0;
    uint16_t ha_type = 0;
    uint16_t params_offset = 0;
    uint8_t idx = 0;

    if (buf == NULL ||
        buf_len < 8)
    {
        return;
    }

    app_ha_generate_ltv_data_from_h2d_cmd(stereo_type, buf, buf_len, &prepend_len, &ha_type);

    //initialize group_ltv_data from effect, push ltv_data
    app_ha_generate_group_ltv_data(HA_STEREO_TYPE_L, group_ltv_data, prepend_len, buf_len, ha_type);

    while (params_offset < buf_len)
    {
        params_offset += 100;
        idx += 1;
    }

    app_ha_split_ltv_data(&audio_ltv_data_db.obj_queue);
}

void app_ha_push_voice_runtime_effect(T_APP_HA_STEREO_TYPE stereo_type, uint8_t *buf,
                                      uint16_t buf_len)
{
    uint8_t prepend_len = 0;
    uint16_t ha_type = 0;
    uint16_t params_offset = 0;
    uint8_t idx = 0;

    if (buf == NULL ||
        buf_len < 8)
    {
        return;
    }

    app_ha_generate_ltv_data_from_h2d_cmd(stereo_type, buf, buf_len, &prepend_len, &ha_type);

    //initialize group_ltv_data from effect, push ltv_data
    app_ha_generate_group_ltv_data(HA_STEREO_TYPE_L, group_ltv_data, prepend_len, buf_len, ha_type);

    while (params_offset < buf_len)
    {
        params_offset += 100;
        idx += 1;
    }

    app_ha_split_ltv_data(&voice_ltv_data_db.obj_queue);
}

void app_ha_wrap_ltv_data(uint8_t *data, uint16_t len)
{
    if (len < 4)
    {
        return;
    }

    //type(2bytes), len(2bytes), params
    //memmove(data, data + 4, len);
    data[6] = (len - 4) & 0xFF;
    data[7] = (((len - 4) >> 8) & 0xFF);
}

bool app_ha_generate_rha_ltv_data(T_APP_HA_STEREO_TYPE stereo_type, T_HA_EFFECT_TYPE effect_type,
                                  uint16_t effect, uint8_t *prepend_len, uint16_t *params_len, uint16_t *ha_type)
{
    uint8_t *buf = NULL;
    uint8_t *wrap_buf = NULL;
    uint8_t *ptr_l = NULL;
    uint8_t *ptr_r = NULL;
    uint8_t *wrap_ptr_l = NULL;
    uint8_t *wrap_ptr_r = NULL;
    uint16_t len = 0;
    uint8_t prepend_payload[12] = {0};
    bool ret = false;

    *params_len = 0;
    memset(group_ltv_data, 0, GROUP_LTV_DATA_MAX_SIZE);

    switch (effect_type)
    {
    case HA_EFFECT_TYPE_PROG_OBJ:
        {
            *params_len = app_ha_prog_get_object_size(effect);

            if (*params_len == 0)
            {
                break;
            }

            if (effect == HA_PROG_OBJ_VOLUME_LEVEL)
            {
                *params_len = 12;
            }

            if (stereo_type == HA_STEREO_TYPE_BOTH)
            {
                ptr_l = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - 2 *
                        (*params_len); //minus 4 is for store payload
                ptr_r = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - (*params_len);
            }
            else if (stereo_type == HA_STEREO_TYPE_L)
            {
                ptr_l  = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - (*params_len);
            }
            else // HA_STEREO_TYPE_R
            {
                ptr_r = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - (*params_len);
            }

            switch (effect)
            {
            case HA_PROG_OBJ_VOLUME_LEVEL:
                {
                    uint16_t prog_obj_size = app_ha_prog_get_object_size(effect);

                    buf = calloc(prog_obj_size, 1);
                    app_ha_prog_get_object(current_ha_mode_db->prog_db->selectable_prog_idx, effect, buf,
                                           &prog_obj_size);
                    // modify from data payload, to type | payload
                    uint8_t volume_level = buf[0];
                    uint8_t balance_level = buf[1];
                    uint8_t data_buf[12] = {0};
                    uint8_t *ptr = NULL;

                    app_ha_generate_gain_db(volume_level, balance_level, data_buf);
                    *params_len -= 4;
                    app_ha_wrap_ltv_data(data_buf, *params_len);
                    ptr = data_buf + 4;

                    *ha_type = ((ptr[1] << 8) + ptr[0]);
                    app_ha_fill_prepend_payload(stereo_type, *ha_type, prepend_payload, prepend_len);
                    app_ha_init_group_ltv_data(stereo_type, group_ltv_data, ptr, NULL, *params_len,
                                               prepend_payload, *prepend_len);

                    free(buf);
                    buf = NULL;
                }
                break;

            default:
                {
                    //the payload's first 4 bytes header would be cutoff when wrap_ltv_data, the ptr needs to offset 4 bytes

                    if (stereo_type == HA_STEREO_TYPE_BOTH)
                    {
                        app_ha_prog_get_object(current_ha_mode_db->prog_db->selectable_prog_idx, effect, ptr_l,
                                               params_len);

                        if (*params_len == 0)
                        {
                            break;
                        }

                        app_ha_prog_get_object(current_ha_mode_db->prog_db->selectable_prog_idx, effect, ptr_r,
                                               params_len);

                        if (*params_len == 0)
                        {
                            break;
                        }
                        *params_len -= 4;
                        app_ha_wrap_ltv_data(ptr_l, *params_len);
                        app_ha_wrap_ltv_data(ptr_r, *params_len);
                        ptr_l += 4;
                        ptr_r += 4;
                    }
                    else if (stereo_type == HA_STEREO_TYPE_L)
                    {
                        app_ha_prog_get_object(current_ha_mode_db->prog_db->selectable_prog_idx, effect, ptr_l,
                                               params_len);
                        if (*params_len == 0)
                        {
                            break;
                        }

                        *params_len -= 4;
                        app_ha_wrap_ltv_data(ptr_l, *params_len);
                        ptr_l += 4;
                    }
                    else // HA_STEREO_TYPE_R
                    {
                        app_ha_prog_get_object(current_ha_mode_db->prog_db->selectable_prog_idx, effect, ptr_r,
                                               params_len);

                        if (*params_len == 0)
                        {
                            break;
                        }

                        *params_len -= 4;
                        app_ha_wrap_ltv_data(ptr_r, *params_len);
                        ptr_r += 4;
                    }

                    if (ptr_l)
                    {
                        *ha_type = ((ptr_l[1] << 8) + ptr_l[0]);
                    }
                    else
                    {
                        *ha_type = ((ptr_r[1] << 8) + ptr_r[0]);
                    }

                    app_ha_fill_prepend_payload(stereo_type, *ha_type, prepend_payload, prepend_len);

                    app_ha_init_group_ltv_data(stereo_type, group_ltv_data, ptr_l, ptr_r, *params_len, prepend_payload,
                                               *prepend_len);
                }
                break;
            }
        }
        break;

    case HA_EFFECT_TYPE_GLOBAL_OBJ:
        {
            *params_len = app_ha_get_global_obj_size(effect);

            if (*params_len == 0)
            {
                break;
            }

            if (effect == HA_GLOBAL_OBJ_OVP_TRAINING_DATA)
            {
                *params_len = 40;
            }

            if (stereo_type == HA_STEREO_TYPE_BOTH)
            {
                ptr_l = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - 2 * (*params_len);
                ptr_r = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - (*params_len);
            }
            else if (stereo_type == HA_STEREO_TYPE_L)
            {
                ptr_l  = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - (*params_len);
            }
            else // HA_STEREO_TYPE_R
            {
                ptr_r = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - (*params_len);
            }

            switch (effect)
            {
            case HA_GLOBAL_OBJ_RHA_WDRC:
            case HA_GLOBAL_OBJ_RHA_WDRC_ENABLED:
                {
                    if (stereo_type == HA_STEREO_TYPE_BOTH)
                    {
                        app_ha_get_global_object((T_APP_HA_GLOBAL_OBJ)effect, ptr_l, params_len);
                        app_ha_get_global_object((T_APP_HA_GLOBAL_OBJ)effect, ptr_r, params_len);
                        *params_len -= 4;
                        app_ha_wrap_ltv_data(ptr_l, *params_len);
                        app_ha_wrap_ltv_data(ptr_r, *params_len);
                        ptr_l += 4;
                        ptr_r += 4;
                    }
                    else if (stereo_type == HA_STEREO_TYPE_L)
                    {
                        app_ha_get_global_object((T_APP_HA_GLOBAL_OBJ)effect, ptr_l, params_len);

                        *params_len -= 4;
                        app_ha_wrap_ltv_data(ptr_l, *params_len);
                        ptr_l += 4;
                    }
                    else // HA_STEREO_TYPE_R
                    {
                        app_ha_get_global_object((T_APP_HA_GLOBAL_OBJ)effect, ptr_r, params_len);
                        *params_len -= 4;
                        app_ha_wrap_ltv_data(ptr_r, *params_len);
                        ptr_r += 4;
                    }

                    if (ptr_l)
                    {
                        *ha_type = ((ptr_l[1] << 8) + ptr_l[0]);
                        app_ha_fill_prepend_payload(stereo_type, *ha_type, prepend_payload, prepend_len);
                    }
                    else
                    {
                        *ha_type = ((ptr_r[1] << 8) + ptr_r[0]);
                        app_ha_fill_prepend_payload(stereo_type, *ha_type, prepend_payload, prepend_len);
                    }

                    app_ha_init_group_ltv_data(stereo_type, group_ltv_data, ptr_l, ptr_r, *params_len, prepend_payload,
                                               *prepend_len);

                    //app_ha_init_group_ltv_data(type, group_ltv_data, data_l, data_r, params_len, prepend_payload, prepend_len);
                }
                break;

            case HA_GLOBAL_OBJ_OVP_TRAINING_DATA:
                {
                    uint16_t obj_size = app_ha_get_global_obj_size(effect);
                    uint8_t *buf = calloc(obj_size, 1);
                    uint8_t data_buf[40] = {0};

                    app_ha_get_global_object((T_APP_HA_GLOBAL_OBJ)effect, buf, &obj_size);
                    app_ha_generate_ovp_training_data_cmd(data_buf, buf);
                    *params_len -= 4;
                    app_ha_wrap_ltv_data(data_buf, *params_len);
                    ptr_l = data_buf + 4;

                    if (ptr_l)
                    {
                        *ha_type = ((ptr_l[1] << 8) + ptr_l[0]);
                        app_ha_fill_prepend_payload(stereo_type, *ha_type, prepend_payload, prepend_len);
                    }
                    else
                    {
                        *ha_type = ((ptr_r[1] << 8) + ptr_r[0]);
                        app_ha_fill_prepend_payload(stereo_type, *ha_type, prepend_payload, prepend_len);
                    }

                    app_ha_init_group_ltv_data(stereo_type, group_ltv_data, ptr_l, NULL, *params_len,
                                               prepend_payload, *prepend_len);

                    free(buf);
                    buf = NULL;
                }
                break;

            default:
                {

                }
                break;
            }
        }
        break;

    case HA_EFFECT_TYPE_DSP_PARAMS_OBJ:
        {
            T_APP_HA_PARAMS_ITEM *p_item = NULL;

            p_item = os_queue_peek(&ha_params_db.obj_queue, 0);

            while (p_item)
            {
                if (p_item->type == effect)
                {
                    break;
                }

                p_item = p_item->p_next;
            }

            if (p_item == NULL)
            {
                break;
            }

            *params_len = p_item->len;

            if (*params_len == 0)
            {
                break;
            }

            if (stereo_type == HA_STEREO_TYPE_BOTH)
            {
                ptr_l = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - 2 * (*params_len);
                ptr_r = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - (*params_len);
            }
            else if (stereo_type == HA_STEREO_TYPE_L)
            {
                ptr_l  = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - (*params_len);
            }
            else // HA_STEREO_TYPE_R
            {
                ptr_r = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - (*params_len);
            }

            switch (effect)
            {
            case HA_PARAMS_RNS:
            case HA_PARAMS_SPEECH_ENHANCE:
            case HA_PARAMS_HA_TEST:
            case HA_PARAMS_SPK_COMPENSATION:
            case HA_PARAMS_MIC_COMPENSATION:
            case HA_PARAMS_MIC_SETTING:
            case HA_PARAMS_ENV_CLASSIFICATION:
            case HA_PARAMS_BAND_WIDTH:
            case HA_PARAMS_MAX_GAIN_SETTING:
            case HA_PARAMS_FBC_SETTING:
            case HA_PARAMS_DC_REMOVER:
            case HA_PARAMS_DEHOWLING_PARA:
            case HA_PARAMS_MIC_SWAP:
            case HA_PARAMS_BYPASS_HA_PROCESSING:
                {
                    if (stereo_type == HA_STEREO_TYPE_BOTH)
                    {
                        memcpy(ptr_l, p_item->buf, *params_len);
                        memcpy(ptr_r, p_item->buf, *params_len);
                        *params_len -= 4;
                        app_ha_wrap_ltv_data(ptr_l, *params_len);
                        app_ha_wrap_ltv_data(ptr_r, *params_len);
                        ptr_l += 4;
                        ptr_r += 4;
                    }
                    else if (stereo_type == HA_STEREO_TYPE_L)
                    {
                        memcpy(ptr_l, p_item->buf, *params_len);
                        *params_len -= 4;
                        app_ha_wrap_ltv_data(ptr_l, *params_len);
                        ptr_l += 4;
                    }
                    else // HA_STEREO_TYPE_R
                    {
                        memcpy(ptr_r, p_item->buf, *params_len);
                        *params_len -= 4;
                        app_ha_wrap_ltv_data(ptr_r, *params_len);
                        ptr_r += 4;
                    }

                    if (ptr_l)
                    {
                        *ha_type = ((ptr_l[1] << 8) + ptr_l[0]);
                        app_ha_fill_prepend_payload(stereo_type, *ha_type, prepend_payload, prepend_len);
                    }
                    else
                    {
                        *ha_type = ((ptr_r[1] << 8) + ptr_r[0]);
                        app_ha_fill_prepend_payload(stereo_type, *ha_type, prepend_payload, prepend_len);
                    }

                    app_ha_init_group_ltv_data(stereo_type, group_ltv_data, ptr_l, ptr_r, *params_len, prepend_payload,
                                               *prepend_len);
                }
                break;

            default:
                {

                }
                break;
            }
        }
        break;

    default:
        {

        }
        break;
    }

    return true;
}

bool app_ha_generate_audio_ltv_data(T_APP_HA_STEREO_TYPE stereo_type, T_HA_EFFECT_TYPE effect_type,
                                    uint16_t effect, uint8_t *prepend_len, uint16_t *params_len, uint16_t *ha_type)
{
    uint8_t *buf = NULL;
    uint8_t *ptr_l = NULL;
    uint8_t *ptr_r = NULL;
    uint16_t len = 0;
    uint8_t prepend_payload[12] = {0};
    bool ret = false;

    *params_len = 0;
    memset(group_ltv_data, 0, GROUP_LTV_DATA_MAX_SIZE);

    switch (effect_type)
    {
    case HA_EFFECT_TYPE_DSP_AUDIO_PARAMS_OBJ:
        {
            switch (effect)
            {
            case HA_AUDIO_PARAMS_WDRC:
                {
                    app_ha_generate_audio_wdrc_cmd((uint8_t **)&buf, params_len);
                }
                break;

            case HA_AUDIO_PARAMS_WDRC_ENABLED:
                {
                    app_ha_generate_audio_wdrc_enabled_cmd((uint8_t **)&buf, params_len);
                }
                break;

            case HA_AUDIO_PARAMS_MIC_SETTINGS:
                {
                    app_ha_generate_audio_mic_settings_cmd((uint8_t **)&buf, params_len);
                }
                break;

            default:
                {

                }
                break;
            }

            if (*params_len == 0)
            {
                break;
            }

            if (stereo_type == HA_STEREO_TYPE_BOTH)
            {
                ptr_l = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - 2 * (*params_len);
                ptr_r = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - (*params_len);
            }
            else if (stereo_type == HA_STEREO_TYPE_L)
            {
                ptr_l  = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - (*params_len);
            }
            else // HA_STEREO_TYPE_R
            {
                ptr_r = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - (*params_len);
            }

            switch (effect)
            {
            case HA_AUDIO_PARAMS_WDRC:
            case HA_AUDIO_PARAMS_WDRC_ENABLED:
            case HA_AUDIO_PARAMS_MIC_SETTINGS:
                {
                    if (stereo_type == HA_STEREO_TYPE_BOTH)
                    {
                        memcpy(ptr_l, buf, *params_len);
                        memcpy(ptr_r, buf, *params_len);
                    }
                    else if (stereo_type == HA_STEREO_TYPE_L)
                    {
                        memcpy(ptr_l, buf, *params_len);
                    }
                    else // HA_STEREO_TYPE_R
                    {
                        memcpy(ptr_r, buf, *params_len);
                    }

                    if (ptr_l)
                    {
                        *ha_type = ((ptr_l[5] << 8) + ptr_l[4]);
                        app_ha_fill_prepend_payload(stereo_type, *ha_type, prepend_payload, prepend_len);
                    }
                    else
                    {
                        *ha_type = ((ptr_r[5] << 8) + ptr_r[4]);
                        app_ha_fill_prepend_payload(stereo_type, *ha_type, prepend_payload, prepend_len);
                    }

                    app_ha_init_group_ltv_data(stereo_type, group_ltv_data, ptr_l, ptr_r, *params_len, prepend_payload,
                                               *prepend_len);
                }
                break;

            default:
                {

                }
                break;
            }
        }
        break;

    default:
        {

        }
        break;
    }

    return true;
}

bool app_ha_wrap_voice_cmd(T_APP_HA_STEREO_TYPE stereo_type, T_HA_EFFECT_TYPE effect_type,
                           uint16_t effect, uint8_t *prepend_len, uint16_t *params_len, uint16_t *ha_type)
{
    uint8_t *buf = NULL;
    uint8_t *ptr_l = NULL;
    uint8_t *ptr_r = NULL;
    uint16_t len = 0;
    uint8_t prepend_payload[12] = {0};
    bool ret = false;

    *params_len = 0;
    memset(group_ltv_data, 0, GROUP_LTV_DATA_MAX_SIZE);

    switch (effect_type)
    {
    case HA_EFFECT_TYPE_DSP_VOICE_PARAMS_OBJ:
        {
            switch (effect)
            {
            case HA_VOICE_PARAMS_WDRC:
                {
                    app_ha_generate_voice_wdrc_cmd((uint8_t **)&buf, params_len);
                }
                break;

            case HA_VOICE_PARAMS_WDRC_ENABLED:
                {
                    app_ha_generate_voice_wdrc_enabled_cmd((uint8_t **)&buf, params_len);
                }
                break;

            case HA_VOICE_PARAMS_MIC_SETTINGS:
                {
                    app_ha_generate_voice_mic_settings_cmd((uint8_t **)&buf, params_len);
                }
                break;

            default:
                {

                }
                break;
            }

            if (*params_len == 0)
            {
                break;
            }

            if (stereo_type == HA_STEREO_TYPE_BOTH)
            {
                ptr_l = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - 2 * (*params_len);
                ptr_r = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - (*params_len);
            }
            else if (stereo_type == HA_STEREO_TYPE_L)
            {
                ptr_l  = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - (*params_len);
            }
            else // HA_STEREO_TYPE_R
            {
                ptr_r = group_ltv_data + GROUP_LTV_DATA_MAX_SIZE - (*params_len);
            }

            switch (effect)
            {
            case HA_VOICE_PARAMS_WDRC:
            case HA_VOICE_PARAMS_WDRC_ENABLED:
            case HA_VOICE_PARAMS_MIC_SETTINGS:
                {
                    if (stereo_type == HA_STEREO_TYPE_BOTH)
                    {
                        memcpy(ptr_l, buf, *params_len);
                        memcpy(ptr_r, buf, *params_len);
                    }
                    else if (stereo_type == HA_STEREO_TYPE_L)
                    {
                        memcpy(ptr_l, buf, *params_len);
                    }
                    else // HA_STEREO_TYPE_R
                    {
                        memcpy(ptr_r, buf, *params_len);
                    }

                    if (ptr_l)
                    {
                        *ha_type = ((ptr_l[5] << 8) + ptr_l[4]);
                        app_ha_fill_prepend_payload(stereo_type, *ha_type, prepend_payload, prepend_len);
                    }
                    else
                    {
                        *ha_type = ((ptr_r[5] << 8) + ptr_r[4]);
                        app_ha_fill_prepend_payload(stereo_type, *ha_type, prepend_payload, prepend_len);
                    }

                    app_ha_init_group_ltv_data(stereo_type, group_ltv_data, ptr_l, ptr_r, *params_len, prepend_payload,
                                               *prepend_len);
                }
                break;

            default:
                {

                }
                break;
            }
        }
        break;

    default:
        {

        }
        break;
    }

    return true;
}

bool app_ha_send_rha_effect()
{
    T_APP_HA_LTV_DATA_ITEM *p_item = NULL;

    while (true)
    {
        p_item = os_queue_out(&ha_ltv_data_db.obj_queue);

        if (p_item)
        {
            //apply p_item buf
            audio_vse_apply(ha_instance, ha_ltv_data_db.seq_num++,
                            p_item->buf,
                            p_item->len);
            free(p_item);

            break;
        }
        else
        {
            T_HA_EFFECT_TYPE effect_type;
            uint16_t effect;
            uint8_t prepend_len = 0;
            uint16_t params_len = 0;
            uint16_t group_ltv_data_len = 0;
            uint16_t ha_type = 0;

            if (app_ha_pop_rha_effect(&effect_type, &effect) == false)
            {
                //ltv_data queue is empty & ha_effect queue is empty
                break;
            }

            app_ha_generate_rha_ltv_data(HA_STEREO_TYPE_L, effect_type, effect, &prepend_len, &params_len,
                                         &ha_type);

            if (params_len)
            {
                //initialize group_ltv_data from effect, push ltv_data
                app_ha_generate_group_ltv_data(HA_STEREO_TYPE_L, group_ltv_data, prepend_len, params_len, ha_type);
                app_ha_split_ltv_data(&ha_ltv_data_db.obj_queue);
            }
        }
    }

    return true;
}

bool app_ha_send_audio_effect()
{
    T_APP_HA_LTV_DATA_ITEM *p_item = NULL;
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();

    while (true)
    {
        p_item = os_queue_out(&audio_ltv_data_db.obj_queue);

        if (p_item)
        {
            audio_vse_apply(app_db.br_link[active_a2dp_idx].ha_audio_instance, audio_ltv_data_db.seq_num++,
                            p_item->buf,
                            p_item->len);
            free(p_item);

            break;
        }
        else
        {
            T_HA_EFFECT_TYPE effect_type;
            uint16_t effect;
            uint8_t prepend_len = 0;
            uint16_t params_len = 0;
            uint16_t group_ltv_data_len = 0;
            uint16_t ha_type = 0;

            if (app_ha_pop_audio_effect(&effect_type, &effect) == false)
            {
                //ltv_data queue is empty & ha_effect queue is empty
                break;
            }

            app_ha_generate_audio_ltv_data(HA_STEREO_TYPE_L, effect_type, effect, &prepend_len, &params_len,
                                           &ha_type);

            if (params_len)
            {
                //initialize group_ltv_data from effect, push ltv_data
                app_ha_generate_group_ltv_data(HA_STEREO_TYPE_L, group_ltv_data, prepend_len, params_len, ha_type);

                app_ha_split_ltv_data(&audio_ltv_data_db.obj_queue);
            }
        }
    }

    return true;
}

bool app_ha_send_voice_effect()
{
    T_APP_HA_LTV_DATA_ITEM *p_item = NULL;
    uint8_t active_hfp_index = app_hfp_get_active_idx();

    while (true)
    {
        p_item = os_queue_out(&voice_ltv_data_db.obj_queue);

        if (p_item)
        {
            audio_vse_apply(app_db.br_link[active_hfp_index].ha_voice_instance, voice_ltv_data_db.seq_num++,
                            p_item->buf,
                            p_item->len);
            free(p_item);

            break;
        }
        else
        {
            T_HA_EFFECT_TYPE effect_type;
            uint16_t effect;
            uint8_t prepend_len = 0;
            uint16_t params_len = 0;
            uint16_t group_ltv_data_len = 0;
            uint16_t ha_type = 0;

            if (app_ha_pop_voice_effect(&effect_type, &effect) == false)
            {
                //ltv_data queue is empty & ha_effect queue is empty
                break;
            }

            app_ha_wrap_voice_cmd(HA_STEREO_TYPE_L, effect_type, effect, &prepend_len, &params_len, &ha_type);

            if (params_len)
            {
                //initialize group_ltv_data from effect, push ltv_data
                app_ha_generate_group_ltv_data(HA_STEREO_TYPE_L, group_ltv_data, prepend_len, params_len, ha_type);

                app_ha_split_ltv_data(&voice_ltv_data_db.obj_queue);
            }
        }
    }

    return true;
}

bool app_ha_wrap_send_adsp_cmd(T_HA_EFFECT_TYPE effect_type, uint16_t effect)
{
    uint8_t *buf = NULL;
    uint8_t *ptr = NULL;
    uint16_t len = 0;
    bool ret = false;

    switch (effect_type)
    {
    case HA_EFFECT_TYPE_PROG_OBJ:
        {
            uint16_t prog_obj_size = app_ha_prog_get_object_size(effect);

            buf = calloc(prog_obj_size, 1);
            app_ha_prog_get_object(current_ha_mode_db->prog_db->selectable_prog_idx, effect, buf,
                                   &prog_obj_size);

            if (prog_obj_size)
            {
                switch (effect)
                {
                case HA_PROG_OBJ_VOLUME_LEVEL:
                    {
                        // modify from data payload, to type | payload
                        uint8_t volume_level = buf[0];
                        uint8_t balance_level = buf[1];
                        uint8_t data_buf[12] = {0};

                        app_ha_generate_gain_db(volume_level, balance_level, data_buf);
                        app_ha_wrap_ullrha_cmd(data_buf, 12);
                        ptr = data_buf;
                        len = 12;
                        ret = audio_probe_adsp_send(ptr, len);
                    }
                    break;

                case HA_PROG_OBJ_RNS:
                    {

                    }
                    break;

                default:
                    {
                        if (prog_obj_size > 4)
                        {
                            app_ha_wrap_ullrha_cmd(buf, prog_obj_size);
                            ptr = buf;
                            len = prog_obj_size;
                            ret = audio_probe_adsp_send(ptr, len);
                        }
                    }
                    break;
                }
            }

            free(buf);
            buf = NULL;
        }
        break;

    case HA_EFFECT_TYPE_GLOBAL_OBJ:
        {
            uint16_t obj_size = app_ha_get_global_obj_size(effect);

            buf = calloc(obj_size, 1);

            if (buf == NULL)
            {
                break;
            }

            app_ha_get_global_object((T_APP_HA_GLOBAL_OBJ)effect, buf, &obj_size);

            switch (effect)
            {
            case HA_GLOBAL_OBJ_ULLRHA_WDRC:
            case HA_GLOBAL_OBJ_ULLRHA_WDRC_ENABLED:
                {
                    if (obj_size > 4)
                    {
                        app_ha_wrap_ullrha_cmd(buf, obj_size);
                        ret = audio_probe_adsp_send(buf, obj_size);
                    }
                }
                break;

            default:
                {

                }
                break;
            }

            free(buf);
            buf = NULL;
        }
        break;

    case HA_EFFECT_TYPE_DSP_PARAMS_OBJ:
        {
            uint16_t obj_size = 0;
            uint8_t *params_buf = NULL;

            switch (effect)
            {
            case HA_PARAMS_RNS:
                {
                    if (app_ha_generate_ullrha_rns_cmd(&params_buf, &len))
                    {
                        app_ha_wrap_ullrha_cmd(params_buf, len);
                        ret = audio_probe_adsp_send(params_buf, len);
                    }
                }
                break;

            case HA_PARAMS_SPEECH_ENHANCE:
                {
                    if (app_ha_generate_ullrha_sppech_enhancement_cmd(&params_buf, &len))
                    {
                        app_ha_wrap_ullrha_cmd(params_buf, len);
                        ret = audio_probe_adsp_send(params_buf, len);
                    }
                }
                break;

            case HA_PARAMS_WDRC:
                {
                    if (app_ha_generate_ullrha_wdrc_cmd(&params_buf, &len))
                    {
                        app_ha_wrap_ullrha_cmd(params_buf, len);
                        ret = audio_probe_adsp_send(params_buf, len);
                    }
                }
                break;

            case HA_PARAMS_WDRC_ENABLED:
                {
                    if (app_ha_generate_ullrha_wdrc_enabled_cmd(&params_buf, &len))
                    {
                        app_ha_wrap_ullrha_cmd(params_buf, len);
                        ret = audio_probe_adsp_send(params_buf, len);
                    }
                }
                break;

            case HA_PARAMS_HA_TEST:
                {
                    if (app_ha_generate_ullrha_test_cmd(&params_buf, &len))
                    {
                        app_ha_wrap_ullrha_cmd(params_buf, len);
                        ret = audio_probe_adsp_send(params_buf, len);
                    }
                }
                break;

            case HA_PARAMS_SPK_COMPENSATION:
                {
                    if (app_ha_generate_ullrha_spk_compensation_cmd(&params_buf, &len))
                    {
                        app_ha_wrap_ullrha_cmd(params_buf, len);
                        ret = audio_probe_adsp_send(params_buf, len);
                    }
                }
                break;

            case HA_PARAMS_MIC_COMPENSATION:
                {
                    if (app_ha_generate_ullrha_mic_compensation_cmd(&params_buf, &len))
                    {
                        app_ha_wrap_ullrha_cmd(params_buf, len);
                        ret = audio_probe_adsp_send(params_buf, len);
                    }
                }
                break;

            case HA_PARAMS_MIC_SETTING:
                {
                    if (app_ha_generate_ullrha_mic_setting_cmd(&params_buf, &len))
                    {
                        app_ha_wrap_ullrha_cmd(params_buf, len);
                        ret = audio_probe_adsp_send(params_buf, len);
                    }
                }
                break;

            case HA_PARAMS_MAX_GAIN_SETTING:
                {
                    if (app_ha_generate_ullrha_max_gain_setting_cmd(&params_buf, &len))
                    {
                        app_ha_wrap_ullrha_cmd(params_buf, len);
                        ret = audio_probe_adsp_send(params_buf, len);
                    }
                }
                break;

            case HA_PARAMS_FBC_SETTING:
                {
                    if (app_ha_generate_ullrha_fbc_setting_cmd(&params_buf, &len))
                    {
                        app_ha_wrap_ullrha_cmd(params_buf, len);
                        ret = audio_probe_adsp_send(params_buf, len);
                    }
                }
                break;

            case HA_PARAMS_DC_REMOVER:
                {
                    if (app_ha_generate_ullrha_dc_remover_cmd(&params_buf, &len))
                    {
                        app_ha_wrap_ullrha_cmd(params_buf, len);
                        ret = audio_probe_adsp_send(params_buf, len);
                    }
                }
                break;

            case HA_PARAMS_BYPASS_HA_PROCESSING:
                {
                    if (app_ha_generate_ullrha_bypass_cmd(&params_buf, &len))
                    {
                        app_ha_wrap_ullrha_cmd(params_buf, len);
                        ret = audio_probe_adsp_send(params_buf, len);
                    }
                }
                break;

            case HA_PARAMS_DEHOWLING_PARA:
                {
                    if (app_ha_generate_ullrha_dehowling_cmd(&params_buf, &len))
                    {
                        app_ha_wrap_ullrha_cmd(params_buf, len);
                        ret = audio_probe_adsp_send(params_buf, len);
                    }
                }
                break;

            case HA_PARAMS_MIC_SWAP:
                {
                    if (app_ha_generate_ullrha_mic_swap_cmd(&params_buf, &len))
                    {
                        app_ha_wrap_ullrha_cmd(params_buf, len);
                        ret = audio_probe_adsp_send(params_buf, len);
                    }
                }
                break;

            default:
                {

                }
                break;
            }
        }
        break;

    case HA_EFFECT_TYPE_CTRL_OBJ:
        {
            uint8_t *params_buf = NULL;

            switch (effect)
            {
            case HA_ADSP_CMD_INIT_FINISH:
                {
                    app_ha_generate_ullrha_init_finish_cmd(&params_buf, &len);
                    ret = audio_probe_adsp_send(params_buf, len);
                }
                break;

            default:
                {

                }
                break;
            }
        }
        break;
    }

    return ret;
}

static void app_ha_rha_vse_cback(T_AUDIO_EFFECT_INSTANCE  instance,
                                 T_AUDIO_VSE_EVENT        event_type,
                                 uint16_t                 seq_num,
                                 void                     *event_buf,
                                 uint16_t                 buf_len)
{
    switch (event_type)
    {
    case AUDIO_VSE_EVENT_ENABLE:
        {

        }
        break;

    case AUDIO_VSE_EVENT_TRANS_RSP:
        {
            app_ha_send_rha_effect();
        }
        break;
    }
}

static void app_ha_audio_vse_cback(T_AUDIO_EFFECT_INSTANCE  instance,
                                   T_AUDIO_VSE_EVENT        event_type,
                                   uint16_t                 seq_num,
                                   void                     *event_buf,
                                   uint16_t                 buf_len)
{
    switch (event_type)
    {
    case AUDIO_VSE_EVENT_ENABLE:
        {
            app_ha_clear_audio_effect();

            //app_ha_generate_audio_wdrc_cmd
            app_ha_push_audio_effect(HA_EFFECT_TYPE_DSP_AUDIO_PARAMS_OBJ, HA_AUDIO_PARAMS_MIC_SETTINGS);
            app_ha_push_audio_effect(HA_EFFECT_TYPE_DSP_AUDIO_PARAMS_OBJ, HA_AUDIO_PARAMS_WDRC);
            app_ha_push_audio_effect(HA_EFFECT_TYPE_DSP_AUDIO_PARAMS_OBJ, HA_AUDIO_PARAMS_WDRC_ENABLED);

            app_ha_send_audio_effect();
        }
        break;

    case AUDIO_VSE_EVENT_TRANS_RSP:
        {
            app_ha_send_audio_effect();
        }
        break;
    }
}

static void app_ha_voice_vse_cback(T_AUDIO_EFFECT_INSTANCE  instance,
                                   T_AUDIO_VSE_EVENT        event_type,
                                   uint16_t                 seq_num,
                                   void                     *event_buf,
                                   uint16_t                 buf_len)
{
    switch (event_type)
    {
    case AUDIO_VSE_EVENT_ENABLE:
        {
            app_ha_clear_voice_effect();

            app_ha_push_voice_effect(HA_EFFECT_TYPE_DSP_VOICE_PARAMS_OBJ, HA_VOICE_PARAMS_MIC_SETTINGS);
            app_ha_push_voice_effect(HA_EFFECT_TYPE_DSP_VOICE_PARAMS_OBJ, HA_VOICE_PARAMS_WDRC);
            app_ha_push_voice_effect(HA_EFFECT_TYPE_DSP_VOICE_PARAMS_OBJ, HA_VOICE_PARAMS_WDRC_ENABLED);

            app_ha_send_voice_effect();
        }
        break;

    case AUDIO_VSE_EVENT_TRANS_RSP:
        {
            app_ha_send_voice_effect();
        }
        break;
    }
}

void app_ha_audio_instance_create(T_APP_BR_LINK *p_link)
{
    if (p_link->a2dp_track_handle != NULL)
    {
        p_link->ha_audio_instance = audio_vse_create(AUDIO_VSE_COMPANY_ID_REALTEK, 0x00,
                                                     app_ha_audio_vse_cback);

        audio_track_effect_attach(p_link->a2dp_track_handle, p_link->ha_audio_instance);
    }
}

void app_ha_voice_instance_create(T_APP_BR_LINK *p_link)
{
    if (p_link->sco_track_handle != NULL)
    {
        p_link->ha_voice_instance = audio_vse_create(AUDIO_VSE_COMPANY_ID_REALTEK, 0x00,
                                                     app_ha_voice_vse_cback);

        audio_track_effect_attach(p_link->sco_track_handle, p_link->ha_voice_instance);
    }
}

void app_ha_audio_instance_release(T_APP_BR_LINK *p_link)
{
    if (p_link->ha_audio_instance != NULL)
    {
        audio_track_effect_detach(p_link->a2dp_track_handle, p_link->ha_audio_instance);
        audio_vse_release(p_link->ha_audio_instance);
        p_link->ha_audio_instance = NULL;
    }
}

void app_ha_voice_instance_release(T_APP_BR_LINK *p_link)
{
    if (p_link->ha_voice_instance != NULL)
    {
        audio_track_effect_detach(p_link->sco_track_handle, p_link->ha_voice_instance);
        audio_vse_release(p_link->ha_voice_instance);
        p_link->ha_voice_instance = NULL;
    }
}

static void app_ha_audio_cback(T_AUDIO_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    switch (event_type)
    {
    case AUDIO_EVENT_PASSTHROUGH_ENABLED:
        {
            T_AUDIO_EVENT_PARAM_PASSTHROUGH_STARTED *param = (T_AUDIO_EVENT_PARAM_PASSTHROUGH_STARTED *)
                                                             event_buf;

            if (param->mode == AUDIO_PASSTHROUGH_MODE_NORMAL)
            {
                if (app_ha_set_ha_mode(HA_MODE_RHA) == false)
                {
                    break;
                }

                current_ha_mode_db->effect_db->apply_cback();
            }
            else if (param->mode == AUDIO_PASSTHROUGH_MODE_LOW_LATENCY &&
                     app_apt_is_ullrha_state((T_ANC_APT_STATE)app_db.current_listening_state))
            {
                //set HA_MODE_ULLRHA behavior would be triggered when receive ADSP boot_done event
                //no need to do operations here
            }
        }
        break;

    case AUDIO_EVENT_TRACK_SIGNAL_OUT_REFRESHED:
        {
            T_AUDIO_EVENT_PARAM *payload = event_buf;
            uint8_t buf[5];
            void *a2dp_track_handle = app_ha_get_audio_track();
            T_AUDIO_TRACK_STATE state = AUDIO_TRACK_STATE_RELEASED;

            app_audio_hearing_state_get(&state);

            if (a2dp_track_handle)
            {
                audio_track_signal_out_monitoring_stop(a2dp_track_handle);
            }
            else if (state == AUDIO_TRACK_STATE_STARTED)
            {
                audio_track_signal_out_monitoring_stop(app_audio_hearing_track_handle_get());
            }

            buf[0] = app_cfg_const.bud_side;

            //left_gain, right_gain are uint16_t type which respectively receive signed value
            //posted by dsp
            //To get original audio gain measured by dsp, mcu needs to do type cast to recover
            //signed dsp value and do bit operation
            buf[1] = (uint8_t)((int16_t)(payload->track_signal_out_refreshed.left_gain_table[0]) >> 7);
            buf[2] = (uint8_t)((int16_t)(payload->track_signal_out_refreshed.left_gain_table[0]) >> 15);
            buf[3] = (uint8_t)((int16_t)(payload->track_signal_out_refreshed.right_gain_table[0]) >> 7);
            buf[4] = (uint8_t)((int16_t)(payload->track_signal_out_refreshed.right_gain_table[0]) >> 15);

            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_report_event_broadcast(EVENT_HA_AUDIO_VOLUME_INFO, buf, 5);
            }
            else
            {
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                    APP_REMOTE_MSG_HA_PROGRAM_RELAY_AUDIO_VOLUME,
                                                    buf,
                                                    5);
            }
        }
        break;

    case AUDIO_EVENT_TRACK_STATE_CHANGED:
        {
            T_AUDIO_EVENT_PARAM *param = event_buf;
            uint8_t i = 0;
            uint8_t app_idx = app_a2dp_get_active_idx();
            T_AUDIO_STREAM_TYPE mode = (T_AUDIO_STREAM_TYPE)0xFF;

            if (param->track_state_changed.handle == app_audio_hearing_track_handle_get() &&
                param->track_state_changed.state == AUDIO_TRACK_STATE_STARTED &&
                ha_hearing_test_db.buf)
            {
                for (i = 0; i < 7; i++)
                {
                    app_ha_hearing_test_tone_generator();
                    app_audio_hearing_write(1, ha_hearing_test_db.buf, ha_hearing_test_db.buf_len);
                }
            }

            if (param->track_state_changed.handle)
            {
                audio_track_stream_type_get(param->track_state_changed.handle, &mode);

                if (mode == AUDIO_STREAM_TYPE_PLAYBACK)
                {
                    last_playback_handle = param->track_state_changed.handle;
                }
            }

            if (app_ha_get_is_hearing_test() == true &&
                param->track_state_changed.handle == last_playback_handle &&
                param->track_state_changed.state == AUDIO_TRACK_STATE_RELEASED &&
                ha_hearing_test_db.buf)
            {
                app_ha_hearing_test_start();
            }
        }
        break;

    case AUDIO_EVENT_TRACK_BUFFER_LOW:
        {
            T_AUDIO_EVENT_PARAM_TRACK_BUFFER_LOW *payload = event_buf;
            uint8_t i = 0;

            if (payload->handle == app_audio_hearing_track_handle_get() &&
                ha_hearing_test_db.buf)
            {
                for (i = 0; i < 3; i++)
                {
                    app_ha_hearing_test_tone_generator();
                    app_audio_hearing_write(1, ha_hearing_test_db.buf, ha_hearing_test_db.buf_len);
                }
            }
        }
        break;

    default:
        break;
    }
}

static void app_ha_effect_apply_cback(void)
{
    app_ha_clear_rha_effect();
    app_ha_prog_load(current_ha_mode_db->prog_db->selectable_prog_idx);
    app_ha_send_rha_effect();
}

#if F_APP_LISTENING_MODE_SUPPORT
void app_ha_listening_delay_start()
{
    app_start_timer(&timer_idx_ha_listening_delay, "ha_listening_delay",
                    ha_timer_id, APP_TIMER_HA_LISTENING_DELAY, 0, false,
                    app_cfg_const.time_delay_to_open_apt_when_in_ear * 1000);
}
#endif

T_HA_HAP_INFO app_ha_hearing_get_prog_info(uint8_t id)
{
    T_HA_HAP_INFO info = {0};
    uint8_t name[HA_PROG_OBJ_NAME_SIZE] = {0};
    uint16_t len;
    uint16_t len_real;
    info.index = id;
    if (id < current_ha_mode_db->prog_db->prog_num)
    {
        app_ha_prog_get_object(id, HA_PROG_OBJ_NAME, name, &len);
        //name struct: len_L - len_H - name string
        len_real = (uint16_t)name[0] + ((uint16_t)name[1] << 8);
        if (len_real <= HA_PROG_NAME_SIZE)
        {
            info.name_len = len_real;
            memcpy(info.p_name, name + 2, info.name_len);
        }
    }
    return info;
}

uint8_t app_ha_hearing_get_prog_num(void)
{
    return current_ha_mode_db->prog_db->prog_num;
}

uint8_t app_ha_hearing_get_active_prog_id(void)
{
    return current_ha_mode_db->prog_db->selectable_prog_idx;
}

static void app_ha_trigger_apply_ullrha_effect()
{
    T_HA_EFFECT_TYPE effect_type;
    uint16_t effect = 0;

    ha_adsp_mgr->is_send = true;

    while (true)
    {
        if (app_ha_pop_ullrha_effect(&effect_type, &effect))
        {
            if (app_ha_wrap_send_adsp_cmd(effect_type, effect))
            {
                break;
            }
        }
        else
        {
            ha_adsp_mgr->is_send = false;
            break;
        }
    }
}

void app_ha_adjust_volume_level(bool is_vol_up)
{
    if (app_apt_is_normal_apt_started())
    {
        uint8_t data[4] = {0};
        uint8_t cmd_buf[12] = {0};
        uint16_t size = 0;
        uint8_t volume_level = 0;
        uint8_t balance_level = 0;

        app_ha_prog_get_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_VOLUME_LEVEL,
                               data, &size);
        volume_level = data[0];
        balance_level = data[1];

        volume_level = app_ha_get_volume_level(is_vol_up, volume_level);

        if (volume_level <= 100)
        {
            app_ha_generate_gain_db(volume_level, balance_level, cmd_buf);
            app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, cmd_buf, 12);
            app_ha_send_rha_effect();

            data[0] = volume_level;
            app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_VOLUME_LEVEL,
                                   data);

            app_ha_get_all_direction_obj(current_ha_mode_db->prog_db->selectable_prog_idx,
                                         HA_PROG_OBJ_VOLUME_LEVEL);

        }
    }
    else if (app_apt_is_ullrha_state((T_ANC_APT_STATE)app_db.current_listening_state))
    {
        uint8_t data[4] = {0};
        uint8_t cmd_buf[12] = {0};
        uint16_t size = 0;
        uint8_t volume_level = 0;
        uint8_t balance_level = 0;

        app_ha_prog_get_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_VOLUME_LEVEL,
                               data, &size);
        volume_level = data[0];
        balance_level = data[1];

        volume_level = app_ha_get_volume_level(is_vol_up, volume_level);

        if (volume_level <= 100)
        {
            app_ha_generate_gain_db(volume_level, balance_level, cmd_buf);

            data[0] = volume_level;
            app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_VOLUME_LEVEL,
                                   data);

            app_ha_get_all_direction_obj(current_ha_mode_db->prog_db->selectable_prog_idx,
                                         HA_PROG_OBJ_VOLUME_LEVEL);
            app_ha_wrap_send_adsp_cmd(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_VOLUME_LEVEL);
        }
    }
}

static bool app_ha_prog_reset(uint8_t prog_id)
{
    uint16_t payload_len;
    uint16_t base = 0;
    uint32_t dsp_prog_base = 0;
    uint8_t buf[HA_BLOCK] = {0};
    uint16_t offset = 0;
    uint8_t i = 0;

    if (!app_ha_dsp_prog_is_set())
    {
        return false;
    }

    fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                       app_ha_dsp_prog_offset_get(&payload_len, current_ha_mode_db->mode),
                       buf, HA_PROG_LIST_HEADER_LEN);

    if (buf[0] == 0) //check prog num
    {
        return false;
    }

    base = app_ha_get_prog_base(current_ha_mode_db->mode);
    offset = 0;
    memset(buf, 0, HA_BLOCK);

    dsp_prog_base = app_ha_dsp_prog_offset_get(&payload_len, current_ha_mode_db->mode);
    payload_len = current_ha_mode_db->prog_db->prog_size_arr[prog_id];
    offset = current_ha_mode_db->prog_db->prog_offset_arr[prog_id];

    for (i = 0; i < payload_len / HA_BLOCK; i++)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                           dsp_prog_base + offset,
                           buf, HA_BLOCK);

        ha_ext_ftl_save_data(buf, base + offset, HA_BLOCK);
        offset += HA_BLOCK;
    }

    if (payload_len % HA_BLOCK)
    {
        fmc_flash_nor_read(flash_cur_bank_img_payload_addr_get(FLASH_IMG_DSPCONFIG) +
                           dsp_prog_base + offset,
                           buf, payload_len % HA_BLOCK);
        ha_ext_ftl_save_data(buf, base + offset, payload_len % HA_BLOCK);
    }

    app_ha_global_obj_get(HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS, buf);

    // 0 is unsync, 1 is sync
    buf[prog_id] = !app_cfg_const.rws_disallow_sync_apt_volume;

    app_ha_global_obj_set(HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS, buf, HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS_SIZE);

    if (app_ha_check_switch_listening_mode(prog_id) == false)
    {
        app_ha_prog_load(prog_id);

        if (current_ha_mode_db->mode == HA_MODE_ULLRHA)
        {
            app_ha_trigger_apply_ullrha_effect();
        }
    }

    return true;
}

bool app_ha_hearing_set_prog_id(uint8_t id)
{
    uint8_t prog_id = id;

    if (prog_id < current_ha_mode_db->prog_db->prog_num)
    {
        if (prog_id != current_ha_mode_db->prog_db->selectable_prog_idx)
        {
            if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
            {
                uint8_t event_data[2];

                app_ha_prog_set_selectable_idx(prog_id);

                if (app_ha_check_switch_listening_mode(prog_id) == false)
                {
                    app_ha_prog_load(prog_id);

                    if (current_ha_mode_db->mode == HA_MODE_ULLRHA)
                    {
                        app_ha_trigger_apply_ullrha_effect();
                    }
                }

                event_data[0] = HA_PROG_OPCODE_SET_ID;
                event_data[1] = prog_id;
                app_report_event_broadcast(EVENT_HA_PROGRAM_INFO, event_data, 2);
            }
            else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA, APP_REMOTE_MSG_HA_PROGRAM_SET_ID,
                                                   &prog_id, 1,
                                                   REMOTE_TIMER_HIGH_PRECISION, 0,
                                                   true);
            }
        }
        else
        {
            uint8_t event_data[2];

            event_data[0] = HA_PROG_OPCODE_SET_ID;
            event_data[1] = prog_id;
            app_report_event_broadcast(EVENT_HA_PROGRAM_INFO, event_data, 2);
        }
        return true;
    }

    return false;
}

void app_ha_switch_hearable_prog()
{
    app_ha_prog_set_selectable_idx((current_ha_mode_db->prog_db->selectable_prog_idx + 1) %
                                   current_ha_mode_db->prog_db->prog_num);

    if (app_ha_check_switch_listening_mode(current_ha_mode_db->prog_db->selectable_prog_idx) ==
        false)
    {
        app_ha_prog_load(current_ha_mode_db->prog_db->selectable_prog_idx);

        if (current_ha_mode_db->mode == HA_MODE_ULLRHA)
        {
            app_ha_trigger_apply_ullrha_effect();
        }
    }

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY ||
        app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
    {
        uint8_t event_data[2];

        event_data[0] = HA_PROG_OPCODE_SET_ID;
        event_data[1] = current_ha_mode_db->prog_db->selectable_prog_idx;
        app_report_event_broadcast(EVENT_HA_PROGRAM_INFO, event_data, sizeof(uint8_t) * 2);
    }
}

static void app_ha_hearing_test_tone_fade_in_sample_generator(uint32_t seq, uint32_t len,
                                                              uint32_t buf_idx)
{
    uint32_t i = 0;
    int32_t val = 0;

    for (i = seq; i < seq + len; i++)
    {
        val = ((double)i / ha_hearing_test_db.filter_samples) * ha_hearing_test_db.mapping_table[i %
                  HA_HEARING_TEST_TONE_TABLE_MAX_LEN];

        ha_hearing_test_db.buf[buf_idx++] = val & 0x000000FF;
        ha_hearing_test_db.buf[buf_idx++] = (val & 0x0000FF00) >> 8;
        ha_hearing_test_db.buf[buf_idx++] = (val & 0x00FF0000) >> 16;
    }
}

static void app_ha_hearing_test_tone_duration_sample_generator(uint32_t seq, uint32_t len,
                                                               uint32_t buf_idx)
{
    uint32_t i = 0;
    int32_t val = 0;

    for (i = seq; i < seq + len; i++)
    {
        val = ha_hearing_test_db.mapping_table[i % HA_HEARING_TEST_TONE_TABLE_MAX_LEN];

        ha_hearing_test_db.buf[buf_idx++] = val & 0x000000FF;
        ha_hearing_test_db.buf[buf_idx++] = (val & 0x0000FF00) >> 8;
        ha_hearing_test_db.buf[buf_idx++] = (val & 0x00FF0000) >> 16;
    }
}

static void app_ha_hearing_test_tone_fade_out_sample_generator(uint32_t seq, uint32_t len,
                                                               uint32_t buf_idx)
{
    uint32_t i = 0;
    int32_t val = 0;

    for (i = seq; i < seq + len; i++)
    {
        val = (1 - ((double)i / ha_hearing_test_db.filter_samples)) * ha_hearing_test_db.mapping_table[i %
                  HA_HEARING_TEST_TONE_TABLE_MAX_LEN];

        ha_hearing_test_db.buf[buf_idx++] = val & 0x000000FF;
        ha_hearing_test_db.buf[buf_idx++] = (val & 0x0000FF00) >> 8;
        ha_hearing_test_db.buf[buf_idx++] = (val & 0x00FF0000) >> 16;
    }
}

static void app_ha_hearing_test_tone_interval_sample_generator(uint32_t seq, uint32_t len,
                                                               uint32_t buf_idx)
{
    uint32_t i = 0;

    for (i = seq; i < seq + len; i++)
    {
        ha_hearing_test_db.buf[buf_idx++] = 0;
        ha_hearing_test_db.buf[buf_idx++] = 0;
        ha_hearing_test_db.buf[buf_idx++] = 0;
    }
}

static void app_ha_hearing_test_tone_spefify_window_sample_generator(uint32_t test_tone_seq,
                                                                     uint32_t test_tone_num, uint32_t buf_idx)
{
    int8_t i = 0;
    uint32_t tp_seq = 0;
    uint32_t tp_num = 0;
    uint32_t cou = 0;
    uint8_t generator_idx = 0;

    for (i = HA_HEARING_TEST_TONE_PHASE; i >= 0; i--)
    {
        if (test_tone_seq >= ha_hearing_test_db.offset_list[i])
        {
            generator_idx = i;
            break;
        }
    }

    tp_seq = test_tone_seq;
    tp_num = test_tone_num;
    cou = 0;

    while (tp_num > 0)
    {
        if (tp_seq < ha_hearing_test_db.offset_list[generator_idx])
        {
            generator_idx += 1;
            continue;
        }

        if (tp_seq + tp_num < ha_hearing_test_db.offset_list[generator_idx + 1])
        {
            cou = tp_num;
        }
        else
        {
            if (generator_idx + 1 < 1 + HA_HEARING_TEST_TONE_PHASE)
            {
                cou = ha_hearing_test_db.offset_list[generator_idx + 1] - tp_seq;
            }
        }

        if (generator_idx < HA_HEARING_TEST_TONE_PHASE)
        {
            ha_hearing_test_db.test_tone_func[generator_idx](tp_seq -
                                                             ha_hearing_test_db.offset_list[generator_idx], cou, buf_idx);
        }

        tp_seq += cou;
        tp_num -= cou;
        buf_idx += (cou * HA_HEARING_TEST_SAMPLE_DEPTH_IN_BYTES);
        generator_idx += 1;

        if (generator_idx + 1 >= 1 + HA_HEARING_TEST_TONE_PHASE)
        {
            break;
        }
    }
}

static void app_ha_hearing_test_update_mapping_table()
{
    uint16_t i = 0;

    for (i = 0; i < HA_HEARING_TEST_TONE_TABLE_MAX_LEN; i++)
    {
        ha_hearing_test_db.mapping_table[i] = ha_hearing_test_db.amplitude * round(sin(
                                                                                       i * 2 * 3.14159265358979323846 / ha_hearing_test_db.samples_per_sine_wave) *
                                                                                   HA_HEARING_TEST_SAMPLE_DEPTH_MAX_VAL);
    }
}

static void app_ha_hearing_test_tone_set(uint32_t freq, int32_t gain, uint16_t timeout_ms)
{
    uint16_t i = 0;
    uint32_t fade_in_num  = round(ha_hearing_test_db.sample_rate * (double)
                                  ha_hearing_test_db.fade_in_out_ms / 1000);
    uint32_t duration_num = round(ha_hearing_test_db.sample_rate * (double)
                                  ha_hearing_test_db.duration_ms / 1000);
    uint32_t fade_out_num = round(ha_hearing_test_db.sample_rate * (double)
                                  ha_hearing_test_db.fade_in_out_ms / 1000);
    uint32_t interval_num = round(ha_hearing_test_db.sample_rate * (double)
                                  ha_hearing_test_db.interval_ms / 1000);

    ha_hearing_test_db.offset_list[i++] = 0;
    ha_hearing_test_db.offset_list[i] = ha_hearing_test_db.offset_list[i - 1] + fade_in_num;
    i++;
    ha_hearing_test_db.offset_list[i] = ha_hearing_test_db.offset_list[i - 1] + duration_num;
    i++;
    ha_hearing_test_db.offset_list[i] = ha_hearing_test_db.offset_list[i - 1] + fade_out_num;
    i++;
    ha_hearing_test_db.offset_list[i] = ha_hearing_test_db.offset_list[i - 1] + interval_num;

    ha_hearing_test_db.test_tone_sample_num = fade_in_num + duration_num + fade_out_num + interval_num;

    ha_hearing_test_db.filter_samples = ha_hearing_test_db.fade_in_out_ms *
                                        ha_hearing_test_db.sample_rate / 1000;

    ha_hearing_test_db.freq = freq;
    ha_hearing_test_db.gain = gain;

    ha_hearing_test_db.amplitude = pow(10.0, gain / 20.0);
    ha_hearing_test_db.samples_per_sine_wave = round(ha_hearing_test_db.sample_rate * 100 / freq) /
                                               100; //step

    app_stop_timer(&timer_idx_ha_hearing_test_tone);

    if (timeout_ms)
    {
        app_start_timer(&timer_idx_ha_hearing_test_tone, "ha_hearing_test",
                        ha_timer_id, APP_TIMER_HA_HEARING_TEST_TONE, 0, false,
                        timeout_ms);
    }
}

static void app_ha_hearing_test_tone_init(uint32_t sample_rate, uint32_t freq,
                                          uint32_t fade_in_out_ms, uint32_t duration_ms,
                                          uint32_t interval_ms, int32_t gain,
                                          uint32_t buf_len)
{
    uint16_t i = 0;
    uint32_t fade_in_num  = round(sample_rate * (double)fade_in_out_ms / 1000);
    uint32_t duration_num = round(sample_rate * (double)duration_ms / 1000);
    uint32_t fade_out_num = round(sample_rate * (double)fade_in_out_ms / 1000);
    uint32_t interval_num = round(sample_rate * (double)interval_ms / 1000);

    ha_hearing_test_db.sample_rate = sample_rate;
    ha_hearing_test_db.freq = freq;
    ha_hearing_test_db.fade_in_out_ms = fade_in_out_ms;
    ha_hearing_test_db.duration_ms = duration_ms;
    ha_hearing_test_db.interval_ms = interval_ms;
    ha_hearing_test_db.gain = gain;

    ha_hearing_test_db.offset_list[i++] = 0;
    ha_hearing_test_db.offset_list[i] = ha_hearing_test_db.offset_list[i - 1] + fade_in_num;
    i++;
    ha_hearing_test_db.offset_list[i] = ha_hearing_test_db.offset_list[i - 1] + duration_num;
    i++;
    ha_hearing_test_db.offset_list[i] = ha_hearing_test_db.offset_list[i - 1] + fade_out_num;
    i++;
    ha_hearing_test_db.offset_list[i] = ha_hearing_test_db.offset_list[i - 1] + interval_num;

    ha_hearing_test_db.filter_samples = fade_in_out_ms * sample_rate / 1000;
    ha_hearing_test_db.amplitude = pow(10.0, gain / 20.0);
    ha_hearing_test_db.samples_per_sine_wave = round(ha_hearing_test_db.sample_rate * 100 / freq) /
                                               100; //step

    ha_hearing_test_db.test_tone_sample_num = fade_in_num + duration_num + fade_out_num + interval_num;

    i = 0;
    ha_hearing_test_db.test_tone_func[i++] = app_ha_hearing_test_tone_fade_in_sample_generator;
    ha_hearing_test_db.test_tone_func[i++] = app_ha_hearing_test_tone_duration_sample_generator;
    ha_hearing_test_db.test_tone_func[i++] = app_ha_hearing_test_tone_fade_out_sample_generator;
    ha_hearing_test_db.test_tone_func[i++] = app_ha_hearing_test_tone_interval_sample_generator;

    ha_hearing_test_db.buf = calloc(buf_len, sizeof(uint8_t));
    ha_hearing_test_db.buf_len = buf_len;
    ha_hearing_test_db.buf_sample_num = buf_len / HA_HEARING_TEST_SAMPLE_DEPTH_IN_BYTES;
    ha_hearing_test_db.frame_len = HA_HEARING_TEST_FRAME_LEN;

    app_ha_hearing_test_update_mapping_table();
    app_stop_timer(&timer_idx_ha_hearing_test_tone);
}

static void app_ha_hearing_test_tone_generator()
{
    uint32_t buf_left_sample_num = 0;
    uint32_t buf_idx = 0;
    static uint32_t test_tone_seq = 0;
    uint32_t cou = 0;

    buf_left_sample_num = ha_hearing_test_db.buf_sample_num;
    buf_idx = 0;

    while (true)
    {
        if (test_tone_seq + buf_left_sample_num <= ha_hearing_test_db.test_tone_sample_num)
        {
            cou = buf_left_sample_num;
        }
        else
        {
            cou = ha_hearing_test_db.test_tone_sample_num - test_tone_seq;
        }

        app_ha_hearing_test_tone_spefify_window_sample_generator(test_tone_seq, cou, buf_idx);

        test_tone_seq += cou;
        buf_idx += (cou * HA_HEARING_TEST_SAMPLE_DEPTH_IN_BYTES);
        buf_left_sample_num -= cou;

        if (test_tone_seq == ha_hearing_test_db.test_tone_sample_num)
        {
            app_ha_hearing_test_update_mapping_table();
            test_tone_seq = 0;
        }

        if (buf_idx == (ha_hearing_test_db.buf_sample_num * HA_HEARING_TEST_SAMPLE_DEPTH_IN_BYTES))
        {
            break;
        }
    }

    return;
}

static void app_ha_hearing_test_start()
{
    T_AUDIO_FORMAT_INFO format_info_test;
    uint8_t i = 0;
    uint32_t actual_mhz;

    org_actual_mhz = pm_cpu_freq_get();
    pm_cpu_freq_req(&ha_hearing_test_freq_handle, 100, &actual_mhz);

    app_ha_hearing_test_update_mapping_table();

    format_info_test.type = AUDIO_FORMAT_TYPE_PCM;
    format_info_test.attr.pcm.sample_rate = ha_hearing_test_db.sample_rate;
    format_info_test.attr.pcm.frame_length = ha_hearing_test_db.frame_len;
    format_info_test.attr.pcm.chann_num = 1;
    format_info_test.attr.pcm.bit_width = HA_HEARING_TEST_SAMPLE_DEPTH;
    format_info_test.attr.pcm.chann_location = AUDIO_CHANNEL_LOCATION_MONO;

    app_audio_hearing_start(format_info_test, HA_HEARING_TEST_TONE_DEFAULT_VOLUME);
}

static void app_ha_hearing_test_stop()
{
    uint32_t actual_mhz = 0;

    app_audio_hearing_stop();
    pm_cpu_freq_req(&ha_hearing_test_freq_handle, org_actual_mhz, &actual_mhz);
}

static void app_ha_hearing_test_init()
{
    app_ha_hearing_test_tone_init(HA_HEARING_TEST_SAMPLE_RATE, HA_HEARING_TEST_TONE_DEFAULT_FREQ,
                                  HA_HEARING_TEST_TONE_FADE_IN_OUT_MS, HA_HEARING_TEST_TONE_DURATION_MS,
                                  HA_HEARING_TEST_TONE_INTERVAL_MS, HA_HEARING_TEST_TONE_DEFAULT_GAIN, HA_HEARING_TEST_FRAME_LEN);
}

static void app_ha_hearing_test_deinit()
{
    app_stop_timer(&timer_idx_ha_hearing_test_tone);

    free(ha_hearing_test_db.buf);
    ha_hearing_test_db.buf = NULL;
}

void app_ha_check_trigger_stop_hearing_test(uint8_t status)
{
    if (app_ha_get_is_hearing_test() == true)
    {
        app_ha_hearing_test_stop();
        app_ha_set_is_hearing_test(false);
        app_ha_hearing_test_deinit();

        app_report_event_broadcast(EVENT_HA_HEARING_TEST_STATUS_INFO, &status, 1);
    }
    else if (app_ha_get_bud_is_hearing_test() == true)
    {
        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                            APP_REMOTE_MSG_HA_PROGRAM_RELAY_STOP_HEARING_TEST,
                                            NULL,
                                            0);

        app_report_event_broadcast(EVENT_HA_HEARING_TEST_STATUS_INFO, &status, 1);
    }
}

static void app_ha_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_HA_LISTENING_DELAY:
        {
            app_stop_timer(&timer_idx_ha_listening_delay);

#if F_APP_LISTENING_MODE_SUPPORT
            app_listening_special_event_trigger(LISTENING_MODE_SPECIAL_EVENT_IN_EAR);

#if F_APP_APT_SUPPORT
            if (app_apt_is_apt_on_state(app_db.current_listening_state))
            {
                current_apt_vol_level_cnt = 0;

                if (low_to_high_gain_level[current_apt_vol_level_cnt] <= app_dsp_cfg_vol.apt_volume_out_max)
                {
                    audio_passthrough_volume_out_set(low_to_high_gain_level[current_apt_vol_level_cnt]);
                    app_start_timer(&timer_idx_ha_apt_vol_level, "ha_apt_vol_level",
                                    ha_timer_id, APP_TIMER_HA_APT_VOL_LEVEL, 0, true,
                                    500);
                }
                else
                {
                    app_apt_volume_out_set(app_cfg_nv.apt_volume_out_level);
                }
            }
#endif
#endif
        }
        break;

    case APP_TIMER_HA_APT_VOL_LEVEL:
        {
            current_apt_vol_level_cnt++;

            if (low_to_high_gain_level[current_apt_vol_level_cnt] <= app_dsp_cfg_vol.apt_volume_out_max)
            {
                audio_passthrough_volume_out_set(low_to_high_gain_level[current_apt_vol_level_cnt]);

                if (current_apt_vol_level_cnt >= 4)
                {
                    app_stop_timer(&timer_idx_ha_apt_vol_level);
                }
            }
            else
            {
                app_stop_timer(&timer_idx_ha_apt_vol_level);
            }
        }
        break;

    case APP_TIMER_HA_HEARING_TEST_TONE:
        {
            app_ha_hearing_test_stop();
            app_ha_set_is_hearing_test(false);
            app_ha_hearing_test_deinit();
        }
        break;

    default:
        break;
    }
}

static void app_ha_mode_set(T_APP_HA_MODE ha_mode)
{
    if (ha_mode < HA_MODE_TOTAL)
    {
        if (app_ha_set_ha_mode(ha_mode) == false)
        {
            return;
        }

        if (ha_mode == HA_MODE_RHA)
        {
            if (app_db.last_normal_rha_state == ANC_OFF_NORMAL_APT_ON)
            {
                app_listening_state_machine(EVENT_NORMAL_APT_ON, true, true);
            }
            else if (app_listening_is_anc_apt_on_state(app_db.last_normal_rha_state))
            {
                app_listening_state_machine(ANC_APT_STATE_TO_EVENT(app_db.last_normal_rha_state), true, true);
            }
        }
        else if (ha_mode == HA_MODE_ULLRHA)
        {
            app_listening_state_machine(LLAPT_STATE_TO_EVENT(app_db.last_ullrha_state), true, true);
        }
    }
}

void app_ha_load_default_rha_ullrha_preset(uint8_t preset_idx)
{
    uint8_t *buf = NULL;
    uint16_t len = 0;

    if (preset_idx != HA_PRESET_NOT_CONFIGURED)
    {
        return;
    }

    //reload WDRC, WDRC_enabled params from dsp_config, if preset_idx is not configured
    if (current_ha_mode == HA_MODE_RHA)
    {
        app_ha_params_to_global_obj(current_ha_mode, HA_PARAMS_WDRC, HA_GLOBAL_OBJ_RHA_WDRC);
        app_ha_params_to_global_obj(current_ha_mode, HA_PARAMS_WDRC_ENABLED,
                                    HA_GLOBAL_OBJ_RHA_WDRC_ENABLED);
    }
    else if (current_ha_mode == HA_MODE_ULLRHA)
    {
        app_ha_params_to_global_obj(current_ha_mode, HA_PARAMS_WDRC, HA_GLOBAL_OBJ_ULLRHA_WDRC);
        app_ha_params_to_global_obj(current_ha_mode, HA_PARAMS_WDRC_ENABLED,
                                    HA_GLOBAL_OBJ_ULLRHA_WDRC_ENABLED);
    }

    if (app_apt_is_normal_apt_started() &&
        app_ha_get_ha_mode() == HA_MODE_RHA)
    {

        len = app_ha_get_global_obj_size(HA_GLOBAL_OBJ_RHA_WDRC);

        if (len)
        {
            buf = calloc(len, 1);
            app_ha_global_obj_get(HA_GLOBAL_OBJ_RHA_WDRC, buf);

            if (buf)
            {
                app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, buf, len);
                app_ha_send_rha_effect();
            }

            free(buf);
            buf = NULL;
        }

        //send wdrc_enabled h2d cmd
        len = app_ha_get_global_obj_size(HA_GLOBAL_OBJ_RHA_WDRC_ENABLED);

        if (len)
        {
            buf = calloc(len, 1);
            app_ha_global_obj_get(HA_GLOBAL_OBJ_RHA_WDRC_ENABLED, buf);

            if (buf)
            {
                app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, buf, len);
                app_ha_send_rha_effect();
            }

            free(buf);
            buf = NULL;
        }
    }
    else if (app_apt_is_ullrha_state((T_ANC_APT_STATE)app_db.current_listening_state) &&
             app_ha_get_ha_mode() == HA_MODE_ULLRHA)
    {
        //send wdrc h2d cmd
        len = app_ha_get_global_obj_size(HA_GLOBAL_OBJ_ULLRHA_WDRC);

        if (len)
        {
            buf = calloc(len, 1);
            app_ha_global_obj_get(HA_GLOBAL_OBJ_ULLRHA_WDRC, buf);

            if (buf)
            {
                app_ha_wrap_ullrha_cmd(buf, len);
                audio_probe_adsp_send(buf, len);
            }

            free(buf);
            buf = NULL;
        }

        //send wdrc_enabled h2d cmd
        len = app_ha_get_global_obj_size(HA_GLOBAL_OBJ_RHA_WDRC_ENABLED);

        if (len)
        {
            buf = calloc(len, 1);
            app_ha_global_obj_get(HA_GLOBAL_OBJ_ULLRHA_WDRC_ENABLED, buf);

            if (buf)
            {
                app_ha_wrap_ullrha_cmd(params_buf, len);
                audio_probe_adsp_send(buf, len);
            }

            free(buf);
            buf = NULL;
        }
    }
}

void app_ha_load_default_audio_preset(uint8_t preset_idx)
{
    uint8_t *buf = NULL;
    uint16_t len = 0;

    if (preset_idx != HA_PRESET_NOT_CONFIGURED)
    {
        return;
    }

    //reload WDRC, WDRC_enabled params from dsp_config, if preset_idx is not configured
    app_ha_audio_params_to_global_obj(HA_AUDIO_PARAMS_MIC_SETTINGS, HA_GLOBAL_OBJ_AUDIO_MIC_SETTINGS);
    app_ha_audio_params_to_global_obj(HA_AUDIO_PARAMS_WDRC, HA_GLOBAL_OBJ_AUDIO_WDRC);
    app_ha_audio_params_to_global_obj(HA_AUDIO_PARAMS_WDRC_ENABLED, HA_GLOBAL_OBJ_AUDIO_WDRC_ENABLE);

    if (!app_a2dp_is_streaming())
    {
        return;
    }

    app_ha_push_audio_effect(HA_EFFECT_TYPE_GLOBAL_OBJ, HA_GLOBAL_OBJ_AUDIO_MIC_SETTINGS);
    app_ha_push_audio_effect(HA_EFFECT_TYPE_GLOBAL_OBJ, HA_GLOBAL_OBJ_AUDIO_WDRC);
    app_ha_push_audio_effect(HA_EFFECT_TYPE_GLOBAL_OBJ, HA_GLOBAL_OBJ_AUDIO_WDRC_ENABLE);
    app_ha_send_audio_effect();
}

void app_ha_load_default_voice_preset(uint8_t preset_idx)
{
    uint8_t *buf = NULL;
    uint16_t len = 0;

    if (preset_idx != HA_PRESET_NOT_CONFIGURED)
    {
        return;
    }

    //reload WDRC, WDRC_enabled params from dsp_config, if preset_idx is not configured
    app_ha_voice_params_to_global_obj(HA_VOICE_PARAMS_MIC_SETTINGS, HA_GLOBAL_OBJ_VOICE_MIC_SETTINGS);
    app_ha_voice_params_to_global_obj(HA_VOICE_PARAMS_WDRC, HA_GLOBAL_OBJ_VOICE_WDRC);
    app_ha_voice_params_to_global_obj(HA_VOICE_PARAMS_WDRC_ENABLED, HA_GLOBAL_OBJ_VOICE_WDRC_ENABLE);

    if (app_hfp_get_call_status() == APP_CALL_IDLE)
    {
        return;
    }

    app_ha_push_audio_effect(HA_EFFECT_TYPE_GLOBAL_OBJ, HA_GLOBAL_OBJ_VOICE_MIC_SETTINGS);
    app_ha_push_audio_effect(HA_EFFECT_TYPE_GLOBAL_OBJ, HA_GLOBAL_OBJ_VOICE_WDRC);
    app_ha_push_audio_effect(HA_EFFECT_TYPE_GLOBAL_OBJ, HA_GLOBAL_OBJ_VOICE_WDRC_ENABLE);
    app_ha_send_voice_effect();
}

void app_ha_set_preset(uint8_t mode, uint8_t preset_idx)
{
    uint8_t data_buf[HA_GLOBAL_OBJ_PRESET_IDX_SIZE] = {0};
    uint16_t version = 0;
    uint16_t is_enabled = 0;

    if (preset_idx == HA_PRESET_INVALID)
    {
        return;
    }

    app_ha_global_obj_get(HA_GLOBAL_OBJ_PRESET_IDX, data_buf);
    data_buf[mode] = preset_idx;
    app_ha_global_obj_set(HA_GLOBAL_OBJ_PRESET_IDX, data_buf, HA_GLOBAL_OBJ_PRESET_IDX);

    switch (mode)
    {
    case APP_HA_PRESET_MODE_AUDIO:
        {
            app_ha_load_default_audio_preset(preset_idx);
        }
        break;

    case APP_HA_PRESET_MODE_VOICE:
        {
            app_ha_load_default_voice_preset(preset_idx);
        }
        break;

    case APP_HA_PRESET_MODE_RHA:
    case APP_HA_PRESET_MODE_ULLRHA:
        {
            app_ha_load_default_rha_ullrha_preset(preset_idx);
        }
        break;

    default:
        {

        }
        break;
    }
}

#if F_APP_ERWS_SUPPORT
static uint16_t app_ha_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    return app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_HA, 0, NULL, true, total);
}

static void app_ha_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                               T_REMOTE_RELAY_STATUS status)
{
    switch (msg_type)
    {
    case APP_REMOTE_MSG_HA_PROGRAM_SET_ID:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_ha_prog_set_selectable_idx(*(uint8_t *)buf);

                if (app_ha_check_switch_listening_mode(current_ha_mode_db->prog_db->selectable_prog_idx) ==
                    false)
                {
                    app_ha_prog_load(current_ha_mode_db->prog_db->selectable_prog_idx);

                    if (current_ha_mode_db->mode == HA_MODE_ULLRHA)
                    {
                        app_ha_trigger_apply_ullrha_effect();
                    }
                }

                if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                {
                    uint8_t event_data[2];

                    event_data[0] = HA_PROG_OPCODE_SET_ID;
                    event_data[1] = current_ha_mode_db->prog_db->selectable_prog_idx;
                    app_report_event_broadcast(EVENT_HA_PROGRAM_INFO, event_data, 2);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_SET_OBJ:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_ha_prog_set_object(*(uint8_t *)buf, *(uint8_t *)(buf + 1), buf + 2);
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_SET_ALL_OBJ:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t type = *(uint8_t *)buf;
                uint16_t type_len = app_ha_prog_get_object_size(type);
                uint8_t i = 0;

                for (i = 0; i < current_ha_mode_db->prog_db->prog_num; i++)
                {
                    app_ha_prog_set_object(i, type, buf + 1 + (type_len * i));
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_GET_SPK_RESPONSE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_ha_get_spk_response(true);
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_RELAY_SPK_RESPONSE:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_report_event_broadcast(EVENT_HA_SPK_RESPONSE_INFO, buf, len);
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_GET_AUDIO_VOLUME:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_ha_get_audio_volume();
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_RELAY_AUDIO_VOLUME:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_report_event_broadcast(EVENT_HA_AUDIO_VOLUME_INFO, buf, len);
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_RESET_OBJ:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t event_payload[2] = {0};
                static uint8_t reset_status = 0;

                if (len == 0) //sent from single / primary
                {
                    reset_status = (uint8_t)app_ha_prog_reset(current_ha_mode_db->prog_db->selectable_prog_idx);

                    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
                    {
                        app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA, APP_REMOTE_MSG_HA_PROGRAM_RESET_OBJ,
                                                           &reset_status, 1,
                                                           REMOTE_TIMER_HIGH_PRECISION, 0,
                                                           false);
                    }
                    else if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
                    {
                        event_payload[0] = HA_PROG_OPCODE_RESET_OBJ;
                        event_payload[1] = reset_status;
                        app_report_event_broadcast(EVENT_HA_PROGRAM_INFO, event_payload, 2);
                    }
                }
                else //sent from secondary
                {
                    event_payload[0] = HA_PROG_OPCODE_RESET_OBJ;
                    event_payload[1] = reset_status & buf[0];
                    app_report_event_broadcast(EVENT_HA_PROGRAM_INFO, event_payload, 2);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_GET_ALL_DIRECTION_OBJ:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t prog_id = buf[0];
                uint8_t type = buf[2];
                uint16_t obj_size = app_ha_prog_get_object_size(type);
                uint16_t offset = 5;

                if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT)
                {
                    offset += obj_size;
                }

                app_ha_prog_get_object(prog_id, type, buf + offset, &obj_size);

                if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY) &&
                    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                        APP_REMOTE_MSG_HA_PROGRAM_GET_ALL_DIRECTION_OBJ,
                                                        buf, len);
                }
                else
                {
                    app_report_event_broadcast(EVENT_HA_PROGRAM_INFO, buf + 1, len - 1);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_SET_HA_VOLUME:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint16_t size = 0;
                uint8_t remote_role = buf[2];
                uint8_t remote_volume = buf[0];
                uint8_t local_volume = 0;

                app_ha_prog_get_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_VOLUME_LEVEL,
                                       buf, &size);
                local_volume = buf[0];

                if (remote_volume < local_volume)
                {
                    buf[0] = remote_volume;
                    app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_VOLUME_LEVEL,
                                           buf);
                }

                if (remote_role == REMOTE_SESSION_ROLE_PRIMARY) // sent from pri
                {
                    buf[2] = app_cfg_nv.bud_role;
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                        APP_REMOTE_MSG_HA_PROGRAM_SET_HA_VOLUME,
                                                        buf,
                                                        HA_PROG_OBJ_VOLUME_LEVEL_SIZE);
                }
                else // sent from sec
                {
                    uint8_t data[3] = {0};

                    data[0] = 0; //is_need_to_report
                    data[1] = 1; //is_sync
                    data[2] = 1; //is_need_to_apply
                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                       APP_REMOTE_MSG_HA_PROGRAM_SET_HA_VOLUME_SYNC_STATUS,
                                                       data, 3,
                                                       REMOTE_TIMER_HIGH_PRECISION, 0,
                                                       true);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_SET_HA_VOLUME_SYNC_STATUS:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t data_buf[HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS_SIZE] = {0};
                uint8_t event_buf[4 + 2 * HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS_SIZE] = {0};
                uint8_t status = 1;
                uint8_t is_need_to_report = buf[0];
                uint8_t is_sync = buf[1];
                uint8_t is_need_to_apply = buf[2];
                uint16_t size = 0;
                uint8_t volume_level = 0;
                uint8_t balance_level = 0;

                current_ha_mode_db->prog_db->current_prog_is_sync_status = is_sync;
                app_ha_global_obj_get(HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS, data_buf);
                data_buf[current_ha_mode_db->prog_db->selectable_prog_idx] = is_sync;
                app_ha_global_obj_set(HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS, data_buf,
                                      HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS_SIZE);

                if (is_need_to_apply)
                {
                    app_ha_prog_get_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_VOLUME_LEVEL,
                                           data_buf, &size);
                    volume_level = data_buf[0];
                    balance_level = data_buf[1];

                    app_ha_generate_gain_db(volume_level, balance_level, data_buf);

                    if (app_apt_is_normal_apt_started())
                    {
                        app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, data_buf, 12);
                        app_ha_send_rha_effect();
                    }
                }

                if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
                {
                    app_report_event_broadcast(EVENT_HA_VOLUME_SYNC_STATUS, &status, 1);

                    if (is_need_to_report)
                    {
                        app_report_event_broadcast(EVENT_HA_VOLUME_SYNC_STATUS_INFO, &is_sync, 1);
                    }
                    if (is_sync)
                    {
                        event_buf[0] = HA_PROG_OPCODE_GET_ALL_DIRECTION_OBJ;
                        event_buf[1] = HA_PROG_OBJ_VOLUME_LEVEL;
                        event_buf[2] = HA_PROG_OBJ_VOLUME_LEVEL_SIZE;
                        event_buf[3] = HA_PROG_OBJ_VOLUME_LEVEL_SIZE >> 8;

                        app_ha_prog_get_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_VOLUME_LEVEL,
                                               data_buf, &size);
                        memcpy(event_buf + 4, data_buf, HA_PROG_OBJ_VOLUME_LEVEL_SIZE);
                        memcpy(event_buf + 4 + HA_PROG_OBJ_VOLUME_LEVEL_SIZE, data_buf, HA_PROG_OBJ_VOLUME_LEVEL_SIZE);

                        app_report_event_broadcast(EVENT_HA_PROGRAM_INFO, event_buf,
                                                   4 + 2 * HA_PROG_OBJ_VOLUME_LEVEL_SIZE);
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_SET_HA_NR:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t is_burn = buf[0];
                uint8_t *cmd_buf = buf + 1;

                if (is_burn)
                {
                    app_ha_prog_set_object_with_tool_data(current_ha_mode_db->prog_db->selectable_prog_idx,
                                                          HA_PROG_OBJ_NR, cmd_buf);
                }

                if (current_ha_mode_db->mode == HA_MODE_RHA)
                {
                    app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, cmd_buf, 12);
                    app_ha_send_rha_effect();
                }
                else
                {
                    app_ha_wrap_send_adsp_cmd(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_NR);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_SET_HA_SPEECH_ENHANCE:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t is_burn = buf[0];
                uint8_t *cmd_buf = buf + 1;

                if (is_burn)
                {
                    app_ha_prog_set_object_with_tool_data(current_ha_mode_db->prog_db->selectable_prog_idx,
                                                          HA_PROG_OBJ_SPEECH_ENHANCE,
                                                          cmd_buf);
                }

                if (current_ha_mode_db->mode == HA_MODE_RHA)
                {
                    app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, cmd_buf, 12);
                    app_ha_send_rha_effect();
                }
                else
                {
                    app_ha_wrap_send_adsp_cmd(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_SPEECH_ENHANCE);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_SET_HA_BF:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t is_burn = buf[0];
                uint8_t *cmd_buf = buf + 1;

                if (is_burn)
                {
                    app_ha_prog_set_object_with_tool_data(current_ha_mode_db->prog_db->selectable_prog_idx,
                                                          HA_PROG_OBJ_BF, cmd_buf);
                }

                if (current_ha_mode_db->mode == HA_MODE_RHA)
                {
                    app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, cmd_buf, 12);
                    app_ha_send_rha_effect();
                }
                else
                {
                    app_ha_wrap_send_adsp_cmd(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_BF);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_GET_HA_VOL:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if ((app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY) &&
                    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
                {
                    //buf: prog_id, buds' status, L level, R level, balance
                    uint8_t prog_id = buf[0];
                    uint16_t offset = 2;
                    uint8_t data[4] = {0};
                    uint16_t size = 0;

                    app_ha_prog_get_object(prog_id, HA_PROG_OBJ_VOLUME_LEVEL, data, &size);
                    buf[offset + app_cfg_const.bud_side] = data[0]; //set curr bud's volume

                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA, APP_REMOTE_MSG_HA_PROGRAM_GET_HA_VOL, buf,
                                                        len);
                }
                else
                {
                    app_report_event_broadcast(EVENT_HA_PROGRAM_VOL_STATUS_INFO, buf + 1, len - 1);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_SET_HA_VOL:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t is_burn = buf[0];
                uint8_t volume_level = buf[1 + app_cfg_const.bud_side];
                uint8_t balance_level = buf[3];
                uint8_t cmd_buf[12] = {0};
                uint8_t volume_level_data[4] = {0};

                app_ha_generate_gain_db(volume_level, balance_level, cmd_buf);

                if (is_burn)
                {
                    uint16_t tool_data_size = app_ha_prog_get_object_size(HA_PROG_OBJ_TOOL_DATA);
                    uint8_t *tool_data = calloc(tool_data_size, 1);
                    uint16_t offset = 2;

                    volume_level_data[0] = volume_level;
                    volume_level_data[1] = balance_level;

                    app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx,
                                           HA_PROG_OBJ_VOLUME_LEVEL,
                                           volume_level_data);
                    app_ha_prog_get_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_TOOL_DATA,
                                           tool_data,
                                           &tool_data_size);

                    tool_data[offset] = buf[1]; //L_level
                    tool_data[offset + 1] = buf[2]; //R_level

                    offset = 4 + tool_data[1] * 2;
                    tool_data[offset] = balance_level;

                    app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_TOOL_DATA,
                                           tool_data);

                    free(tool_data);
                    tool_data = NULL;
                }

                if (current_ha_mode_db->mode == HA_MODE_RHA)
                {
                    app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, cmd_buf, 12);
                    app_ha_send_rha_effect();
                }
                else
                {
                    app_ha_wrap_send_adsp_cmd(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_VOLUME_LEVEL);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_STOP_A2DP_AUDIO_TRACK:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t app_idx = app_a2dp_get_active_idx();

                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    bt_avrcp_pause(app_db.br_link[app_idx].bd_addr);
                }

                if (audio_track_release(app_db.br_link[app_idx].a2dp_track_handle) == false)
                {
                    if (app_ha_get_is_hearing_test())
                    {
                        app_ha_hearing_test_start();
                    }
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_DISABLE_AUDIO_EQ_START_HEARING_TEST:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (app_ha_get_is_hearing_test())
                {
                    app_ha_hearing_test_start();
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_RELAY_HEARING_TEST_STATUS:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                bud_is_hearing_test = buf[0];
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_RELAY_STOP_HEARING_TEST:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                app_ha_hearing_test_stop();
                app_ha_set_is_hearing_test(false);
                app_ha_hearing_test_deinit();
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_RELAY_HA_VOLUME_STATUS:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t volume_buf[HA_PROG_OBJ_VOLUME_LEVEL_SIZE] = {0};
                uint16_t size = 0;

                app_ha_prog_get_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_VOLUME_LEVEL,
                                       volume_buf,
                                       &size);

                //need to update both earbuds' volume sync status to not sync
                if (buf[0] != volume_buf[0])
                {
                    uint8_t data[3] = {0};

                    data[0] = 1; //is_need_to_report
                    data[1] = 0; //is_sync
                    data[2] = 0; //is_need_to_apply
                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                       APP_REMOTE_MSG_HA_PROGRAM_SET_HA_VOLUME_SYNC_STATUS,
                                                       data, 3,
                                                       REMOTE_TIMER_HIGH_PRECISION, 0,
                                                       true);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_SET_HA_MODE:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED)
            {
                app_ha_mode_set((T_APP_HA_MODE)buf[0]);
            }
        }
        break;

    case APP_REMOTE_MSG_HA_CTRL_OVP_TRAINING:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t data_buf[12];
                uint8_t is_start = buf[0];

                memset(data_buf, 0, 12);
                app_ha_generate_ovp_training_cmd(is_start, data_buf);
                app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, data_buf, 12);
                app_ha_send_rha_effect();
            }
        }
        break;

    case APP_REMOTE_MSG_HA_PROGRAM_RELAY_OVP_TRAINING_STATUS:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD) //sent by secondary
            {
                bool bud_train_status = buf[0] & BIT0;
                uint8_t event_data[2] = {0};

                app_ha_set_ovp_training_status(REMOTE_SESSION_ROLE_SECONDARY, bud_train_status);

                if (app_ha_get_ovp_training_is_ready_status()) //if pri, sec buds are ready, report training status
                {
                    uint8_t status = HA_OVP_TRAINING_STATUS_FAILED;

                    //if train failed, reset both buds' training params
                    if ((uint8_t)app_ha_get_ovp_training_status() == 0)
                    {
                        app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                           APP_REMOTE_MSG_HA_CTRL_OVP_TRAINING_RESET_PARAMS,
                                                           &status, 1,
                                                           REMOTE_TIMER_HIGH_PRECISION, 0,
                                                           true);
                    }
                    else
                    {
                        status = HA_OVP_TRAINING_STATUS_SUCCESS;
                    }

                    event_data[0] = status;
                    event_data[1] = buf[0] >> 1;

                    app_report_event_broadcast(EVENT_HA_OVP_TRAINING_STATUS_INFO, event_data, 2);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_CTRL_OVP_TRAINING_RESET_PARAMS:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                if (buf[0] == HA_OVP_TRAINING_STATUS_FAILED) // train failed
                {
                    app_ha_global_obj_reset(HA_GLOBAL_OBJ_OVP_TRAINING_DATA);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_SET_CLASSIFY:
        {
            if (status == REMOTE_RELAY_STATUS_SYNC_TOUT ||
                status == REMOTE_RELAY_STATUS_SYNC_EXPIRED ||
                status == REMOTE_RELAY_STATUS_SYNC_REF_CHANGED ||
                status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t is_enable = buf[0];

                app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &len, HA_DSP_FEATURE_HA_ENV_CLASSIFICATION);

                if (len)
                {
                    len = 8 + len;
                    buf = calloc(len, 1);
                    app_ha_generate_classify_cmd(is_enable, buf, len);
                    app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, buf, len);
                    app_ha_send_rha_effect();
                    free(buf);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_HA_GLOBAL_GET_ALL_DIRECTION_OBJ:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint16_t ctrl_data_len = buf[0] + (buf[1] >> 8);
                uint8_t bud_role = buf[2];
                uint8_t type = buf[3];

                //current bud is secondary, need to get current bud's data and return to primary bud
                if ((bud_role == REMOTE_SESSION_ROLE_PRIMARY) &&
                    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
                {
                    switch (type)
                    {
                    case HA_GLOBAL_OBJ_PRESET_IDX:
                        {
                            uint8_t mode = buf[4];
                            uint8_t data_buf[HA_GLOBAL_OBJ_PRESET_IDX_SIZE] = {0};
                            uint8_t relay_buf[6] = {0};
                            uint16_t relay_buf_len = 6;

                            memcpy(relay_buf, buf, 6);
                            app_ha_global_obj_get((T_APP_HA_GLOBAL_OBJ)type, data_buf);
                            relay_buf[2] = app_cfg_nv.bud_role;
                            relay_buf[5] = data_buf[mode];

                            app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                                APP_REMOTE_MSG_HA_GLOBAL_GET_ALL_DIRECTION_OBJ,
                                                                relay_buf, relay_buf_len);

                        }
                        break;

                    default:
                        {

                        }
                        break;
                    }
                }
                else //current bud is primary, and have received secondary buds' data
                {
                    switch (type)
                    {
                    case HA_GLOBAL_OBJ_PRESET_IDX:
                        {
                            uint8_t mode = buf[4];
                            uint8_t data_buf[HA_GLOBAL_OBJ_PRESET_IDX_SIZE] = {0};
                            uint8_t event_data[3] = {0};
                            uint16_t event_data_len = 3;

                            app_ha_global_obj_get((T_APP_HA_GLOBAL_OBJ)type, data_buf);

                            event_data[0] = mode;
                            event_data[1 + app_cfg_const.bud_side] = data_buf[mode];
                            //set remote bud's data which bud_side is opposite to current bud
                            event_data[1 + (app_cfg_const.bud_side ? 0 : 1)] = buf[5];

                            app_report_event_broadcast(EVENT_HA_PRESET_IDX_INFO, event_data, 3);
                        }
                        break;

                    default:
                        {

                        }
                        break;
                    }
                }

            }
        }
        break;

    case APP_REMOTE_MSG_HA_SET_PRESET_IDX:
        {
            if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
            {
                uint8_t mode = buf[0];
                uint8_t preset_idx = buf[1];

                if (preset_idx != HA_PRESET_INVALID)
                {
                    app_ha_set_preset(mode, preset_idx);
                }
            }
        }
        break;

    default:
        break;
    }
}
#endif

void app_ha_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t app_idx,
                       uint8_t *ack_pkt)
{
    uint16_t cmd_id = (uint16_t)(cmd_ptr[0] | (cmd_ptr[1] << 8));

    switch (cmd_id)
    {
    case CMD_HA_ACCESS_PROGRAM:
        {
            switch (cmd_ptr[2])
            {
            case HA_PROG_OPCODE_GET_NUM:
                {
                    uint8_t event_data[2];

                    event_data[0] = cmd_ptr[2];
                    event_data[1] = current_ha_mode_db->prog_db->prog_num;

                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    app_report_event(cmd_path, EVENT_HA_PROGRAM_INFO, app_idx,
                                     event_data, sizeof(uint8_t) * 2);
                }
                break;

            case HA_PROG_OPCODE_GET_ID:
                {
                    uint8_t event_data[2];

                    event_data[0] = cmd_ptr[2];
                    event_data[1] = current_ha_mode_db->prog_db->selectable_prog_idx;

                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    app_report_event(cmd_path, EVENT_HA_PROGRAM_INFO, app_idx,
                                     event_data, sizeof(uint8_t) * 2);
                }
                break;

            case HA_PROG_OPCODE_SET_ID:
                {
                    if (app_ha_hearing_set_prog_id(cmd_ptr[3]))
                    {
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    }
                    else
                    {
                        ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    }
                }
                break;

            case HA_PROG_OPCODE_GET_OBJ:
                {
                    uint8_t prog_id = cmd_ptr[3];
                    uint8_t type = cmd_ptr[4];

                    if (prog_id < current_ha_mode_db->prog_db->prog_num)
                    {
                        uint16_t len = 3 + app_ha_prog_get_object_size(type);
                        uint8_t *event_data = malloc(sizeof(uint8_t) * len);
                        uint16_t size = 0;

                        memset(event_data, 0, len);

                        event_data[0] = cmd_ptr[2];
                        event_data[1] = len - 3;
                        event_data[2] = (len - 3) >> 8;
                        app_ha_prog_get_object(prog_id, type, event_data + 3, &size);

                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                        app_report_event(cmd_path, EVENT_HA_PROGRAM_INFO, app_idx,
                                         event_data, sizeof(uint8_t) * len);
                        free(event_data);
                    }
                    else
                    {
                        ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    }
                }
                break;

            case HA_PROG_OPCODE_SET_OBJ:
                {
                    uint8_t prog_id = cmd_ptr[3];
                    uint8_t type = cmd_ptr[4];

                    if (prog_id < current_ha_mode_db->prog_db->prog_num)
                    {
                        uint16_t type_len = app_ha_prog_get_object_size(type);

                        if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                        {
                            app_ha_prog_set_object(prog_id, type, cmd_ptr + 5);
                        }
                        else
                        {
                            app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA, APP_REMOTE_MSG_HA_PROGRAM_SET_OBJ,
                                                               cmd_ptr + 3, type_len + 2,
                                                               REMOTE_TIMER_HIGH_PRECISION, 0,
                                                               true);
                        }

                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    }
                    else
                    {
                        ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    }
                }
                break;

            case HA_PROG_OPCODE_GET_ALL_OBJ:
                {
                    uint8_t type = cmd_ptr[3];
                    uint16_t type_len = app_ha_prog_get_object_size(type);
                    uint16_t event_len = 3 + (type_len * current_ha_mode_db->prog_db->prog_num);
                    uint8_t *event_data = calloc(event_len, sizeof(uint8_t));
                    uint16_t size = 0;
                    uint8_t i = 0;

                    event_data[0] = cmd_ptr[2];
                    event_data[1] = event_len - 3;
                    event_data[2] = (event_len - 3) >> 8;

                    for (i = 0; i < current_ha_mode_db->prog_db->prog_num; i++)
                    {
                        if (3 + (type_len * i) < event_len)
                        {
                            app_ha_prog_get_object(i, type, event_data + 3 + (type_len * i), &size);
                        }
                    }

                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    app_report_event(cmd_path, EVENT_HA_PROGRAM_INFO, app_idx, event_data, sizeof(uint8_t) * event_len);
                    free(event_data);
                }
                break;

            case HA_PROG_OPCODE_SET_ALL_OBJ:
                {
                    uint8_t type = cmd_ptr[3];
                    uint16_t type_len = app_ha_prog_get_object_size(type);
                    uint8_t i = 0;

                    if ((cmd_len - 4) == (type_len * current_ha_mode_db->prog_db->prog_num))
                    {
                        if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                        {
                            for (i = 0; i < current_ha_mode_db->prog_db->prog_num; i++)
                            {
                                app_ha_prog_set_object(i, type, cmd_ptr + 4 + (type_len * i));
                            }
                        }
                        else
                        {
                            app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA, APP_REMOTE_MSG_HA_PROGRAM_SET_ALL_OBJ,
                                                               cmd_ptr + 3, (type_len * current_ha_mode_db->prog_db->prog_num) + 1,
                                                               REMOTE_TIMER_HIGH_PRECISION, 0,
                                                               true);
                        }
                    }
                    else
                    {
                        ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                    }

                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                }
                break;

            case HA_PROG_OPCODE_RESET_OBJ:
                {
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                    if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                    {
                        app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA, APP_REMOTE_MSG_HA_PROGRAM_RESET_OBJ,
                                                           NULL, 0,
                                                           REMOTE_TIMER_HIGH_PRECISION, 0,
                                                           true);
                    }
                    else
                    {
                        uint8_t event_data[2] = {0};
                        uint16_t event_len = 2;

                        event_data[0] = cmd_ptr[2];
                        event_data[1] = (uint8_t)app_ha_prog_reset(current_ha_mode_db->prog_db->selectable_prog_idx);
                        app_report_event(cmd_path, EVENT_HA_PROGRAM_INFO, app_idx, event_data, event_len);
                    }
                }
                break;

            case HA_PROG_OPCODE_GET_ALL_DIRECTION_OBJ:
                {
                    uint8_t prog_id = cmd_ptr[3];
                    uint8_t type = cmd_ptr[4];

                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                    app_ha_get_all_direction_obj(prog_id, type);
                }
                break;
            }
        }
        break;

    case CMD_HA_SPK_RESPONSE:
        {
            if (cmd_len < 3)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                uint8_t direction = cmd_ptr[2];

                if (direction <= DEVICE_BUD_SIDE_RIGHT)
                {
                    if (direction == app_cfg_const.bud_side)
                    {
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                        app_ha_get_spk_response(false);
                    }
                    else if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                    {
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                            APP_REMOTE_MSG_HA_PROGRAM_GET_SPK_RESPONSE,
                                                            NULL,
                                                            0);
                    }
                    else
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    }
                }
                else
                {
                    ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                }
            }
        }
        break;

    case CMD_HA_AUDIO_VOLUME_GET:
        {
            if (cmd_len < 3)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                uint8_t direction = cmd_ptr[2];

                if (direction <= DEVICE_BUD_SIDE_RIGHT)
                {
                    if (direction == app_cfg_const.bud_side)
                    {
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                        app_ha_get_audio_volume();
                    }
                    else if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                    {
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                            APP_REMOTE_MSG_HA_PROGRAM_GET_AUDIO_VOLUME,
                                                            NULL,
                                                            0);
                    }
                    else
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                        app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                    }
                }
                else
                {
                    ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                }
            }
        }
        break;

    case CMD_HA_VOLUME_SYNC_STATUS:
        {
            if (cmd_len < 3)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                uint8_t buf[HA_PROG_OBJ_VOLUME_LEVEL_SIZE] = {0};
                uint8_t status = 0;
                uint16_t size = 0;

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    if (cmd_ptr[2]) // if is_sync is enabled, need to sync minimun HA volume
                    {
                        app_ha_prog_get_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_VOLUME_LEVEL,
                                               buf, &size);
                        buf[2] = app_cfg_nv.bud_role;
                        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                            APP_REMOTE_MSG_HA_PROGRAM_SET_HA_VOLUME,
                                                            buf,
                                                            HA_PROG_OBJ_VOLUME_LEVEL_SIZE);
                    }
                    else
                    {
                        uint8_t data[3] = {0};

                        data[0] = 0; //is_need_to_report
                        data[1] = cmd_ptr[2]; //is_sync
                        data[2] = 0; //is_need_to_apply
                        app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                           APP_REMOTE_MSG_HA_PROGRAM_SET_HA_VOLUME_SYNC_STATUS,
                                                           data, 3,
                                                           REMOTE_TIMER_HIGH_PRECISION, 0,
                                                           true);
                    }
                }
                else
                {
                    status = 0;
                    app_report_event(cmd_path, EVENT_HA_VOLUME_SYNC_STATUS, app_idx, &status, 1);
                }
            }
        }
        break;

    case CMD_HA_VOLUME_SYNC_STATUS_GET:
        {
            if (cmd_len < 3)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                uint8_t prog_id = cmd_ptr[2];
                uint8_t is_sync = 0;

                if (prog_id >= current_ha_mode_db->prog_db->prog_num)
                {
                    ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                }
                else
                {
                    uint8_t buf[HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS_SIZE] = {0};

                    app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                    app_ha_global_obj_get(HA_GLOBAL_OBJ_VOLUME_SYNC_STATUS, buf);
                    is_sync = buf[prog_id];

                    app_report_event(cmd_path, EVENT_HA_VOLUME_SYNC_STATUS_INFO, app_idx, &is_sync, 1);
                }
            }
        }
        break;

    case CMD_HA_PROGRAM_NR_GET:
        {
            if (cmd_len < 3)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                uint8_t prog_id = cmd_ptr[2];
                uint8_t obj_type = HA_PROG_OBJ_NR;
                uint8_t *data = calloc(4, 1);

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                app_ha_prog_get_object_status(prog_id, obj_type, data);
                app_report_event(cmd_path, EVENT_HA_PROGRAM_NR_STATUS_INFO, app_idx, data, 4);

                free(data);
            }
        }
        break;

    case CMD_HA_PROGRAM_NR_SET:
        {
            if (cmd_len < 5)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                uint8_t is_burn = cmd_ptr[2];
                uint8_t is_enabled = cmd_ptr[3];
                uint8_t level = cmd_ptr[4];
                uint8_t data_buf[13] = {0};
                uint8_t *cmd_buf = data_buf + 1;

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                data_buf[0] = is_burn;
                app_ha_generate_nr_cmd(is_enabled, level, cmd_buf);

                if ((app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SINGLE) &&
                    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
                {
                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                       APP_REMOTE_MSG_HA_PROGRAM_SET_HA_NR,
                                                       data_buf, 13,
                                                       REMOTE_TIMER_HIGH_PRECISION, 0,
                                                       true);
                }
                else
                {
                    if (is_burn)
                    {
                        app_ha_prog_set_object_with_tool_data(current_ha_mode_db->prog_db->selectable_prog_idx,
                                                              HA_PROG_OBJ_NR, cmd_buf);
                    }

                    if (current_ha_mode_db->mode == HA_MODE_RHA)
                    {
                        app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, cmd_buf, 12);
                        app_ha_send_rha_effect();
                    }
                    else
                    {
                        app_ha_wrap_send_adsp_cmd(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_NR);
                    }
                }
            }
        }
        break;

    case CMD_HA_PROGRAM_SPEECH_ENHANCE_GET:
        {
            if (cmd_len < 3)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                uint8_t prog_id = cmd_ptr[2];
                uint8_t obj_type = HA_PROG_OBJ_SPEECH_ENHANCE;
                uint8_t *data = calloc(4, 1);

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                app_ha_prog_get_object_status(prog_id, obj_type, data);
                app_report_event(cmd_path, EVENT_HA_PROGRAM_NR_STATUS_INFO, app_idx, data, 4);

                free(data);
            }
        }
        break;

    case CMD_HA_PROGRAM_SPEECH_ENHANCE_SET:
        {
            if (cmd_len < 5)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                uint8_t is_burn = cmd_ptr[2];
                uint8_t is_enabled = cmd_ptr[3];
                uint8_t level = cmd_ptr[4];
                uint8_t data_buf[13] = {0};
                uint8_t *cmd_buf = data_buf + 1;

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                data_buf[0] = is_burn;
                app_ha_generate_speech_enhance_cmd(is_enabled, level, cmd_buf);

                if ((app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SINGLE) &&
                    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
                {
                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                       APP_REMOTE_MSG_HA_PROGRAM_SET_HA_SPEECH_ENHANCE,
                                                       data_buf, 13,
                                                       REMOTE_TIMER_HIGH_PRECISION, 0,
                                                       true);
                }
                else
                {
                    if (is_burn)
                    {
                        app_ha_prog_set_object_with_tool_data(current_ha_mode_db->prog_db->selectable_prog_idx,
                                                              HA_PROG_OBJ_SPEECH_ENHANCE,
                                                              cmd_buf);
                    }

                    if (current_ha_mode_db->mode == HA_MODE_RHA)
                    {
                        app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, cmd_buf, 12);
                        app_ha_send_rha_effect();
                    }
                    else
                    {
                        app_ha_wrap_send_adsp_cmd(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_SPEECH_ENHANCE);
                    }
                }
            }
        }
        break;

    case CMD_HA_PROGRAM_BF_GET:
        {
            if (cmd_len < 3)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                uint8_t prog_id = cmd_ptr[2];
                uint8_t obj_type = HA_PROG_OBJ_BF;
                uint8_t *data = calloc(4, 1);

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
                app_ha_prog_get_object_status(prog_id, obj_type, data);
                app_report_event(cmd_path, EVENT_HA_PROGRAM_BF_STATUS_INFO, app_idx, data, 4);

                free(data);
            }
        }
        break;

    case CMD_HA_PROGRAM_BF_SET:
        {
            if (cmd_len < 4)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                uint8_t is_burn = cmd_ptr[2];
                uint8_t is_enabled = cmd_ptr[3];
                uint8_t data_buf[13] = {0};
                uint8_t *cmd_buf = data_buf + 1;

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                data_buf[0] = is_burn;
                app_ha_generate_bf_cmd(is_enabled, cmd_buf);

                if ((app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SINGLE) &&
                    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
                {
                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                       APP_REMOTE_MSG_HA_PROGRAM_SET_HA_BF,
                                                       data_buf, 13,
                                                       REMOTE_TIMER_HIGH_PRECISION, 0,
                                                       true);
                }
                else
                {
                    if (is_burn)
                    {
                        app_ha_prog_set_object_with_tool_data(current_ha_mode_db->prog_db->selectable_prog_idx,
                                                              HA_PROG_OBJ_BF, cmd_buf);
                    }

                    if (current_ha_mode_db->mode == HA_MODE_RHA)
                    {
                        app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, cmd_buf, 12);
                        app_ha_send_rha_effect();
                    }
                    else
                    {
                        app_ha_wrap_send_adsp_cmd(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_BF);
                    }
                }
            }
        }
        break;

    case CMD_HA_PROGRAM_VOL_GET:
        {
            if (cmd_len < 3)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                uint8_t prog_id = cmd_ptr[2];

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                app_ha_get_all_direction_obj_status(prog_id, HA_PROG_OBJ_VOLUME_LEVEL);
            }
        }
        break;

    case CMD_HA_PROGRAM_VOL_SET:
        {
            if (cmd_len < 6)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                if ((app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SINGLE) &&
                    (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
                {
                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                       APP_REMOTE_MSG_HA_PROGRAM_SET_HA_VOL,
                                                       cmd_ptr + 2, 4,
                                                       REMOTE_TIMER_HIGH_PRECISION, 0,
                                                       true);
                }
                else
                {
                    uint8_t is_burn = cmd_ptr[2];
                    uint8_t volume_level = cmd_ptr[3 + app_cfg_const.bud_side];
                    uint8_t balance_level = cmd_ptr[5];
                    uint8_t cmd_buf[12] = {0};
                    uint8_t volume_level_data[4] = {0};

                    app_ha_generate_gain_db(volume_level, balance_level, cmd_buf);

                    if (is_burn)
                    {
                        uint16_t tool_data_size = app_ha_prog_get_object_size(HA_PROG_OBJ_TOOL_DATA);
                        uint8_t *tool_data = calloc(tool_data_size, 1);
                        uint16_t offset = 2;

                        volume_level_data[0] = volume_level;
                        volume_level_data[1] = balance_level;

                        app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx,
                                               HA_PROG_OBJ_VOLUME_LEVEL,
                                               volume_level_data);
                        app_ha_prog_get_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_TOOL_DATA,
                                               tool_data,
                                               &tool_data_size);

                        tool_data[offset] = cmd_ptr[3]; //L_level
                        tool_data[offset + 1] = cmd_ptr[4]; //R_level

                        offset = 4 + tool_data[1] * 2;
                        tool_data[offset] = balance_level;

                        app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_TOOL_DATA,
                                               tool_data);

                        free(tool_data);
                        tool_data = NULL;
                    }

                    if (current_ha_mode_db->mode == HA_MODE_RHA)
                    {
                        app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, cmd_buf, 12);
                        app_ha_send_rha_effect();
                    }
                    else
                    {
                        app_ha_wrap_send_adsp_cmd(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_VOLUME_LEVEL);
                    }
                }
            }
        }
        break;

    case CMD_HA_MODE_SET:
        {
            T_APP_HA_MODE ha_mode = (T_APP_HA_MODE)cmd_ptr[2];

            if (ha_mode >= HA_MODE_TOTAL)
            {
                ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
            }
            else if (ha_mode == HA_MODE_RHA)
            {
                if (!(app_cfg_const.normal_apt_support && app_cfg_const.enable_ha) ||
                    !app_apt_open_condition_check(ANC_OFF_NORMAL_APT_ON))
                {
                    ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                }
            }
            else if (ha_mode == HA_MODE_ULLRHA)
            {
                if (!(app_cfg_const.llapt_support && app_cfg_const.enable_ha &&
                      (app_apt_ullrha_scenario_num_get() > 0)) ||
                    !app_apt_open_condition_check(ANC_OFF_LLAPT_ON_SCENARIO_1))
                {
                    ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                }
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if (ack_pkt[2] != CMD_SET_STATUS_PARAMETER_ERROR)
            {
                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                       APP_REMOTE_MSG_HA_PROGRAM_SET_HA_MODE,
                                                       cmd_ptr + 2, 1,
                                                       REMOTE_TIMER_HIGH_PRECISION, 0,
                                                       true);
                }
                else
                {
                    app_ha_mode_set(ha_mode);
                }
            }
        }
        break;

    case CMD_HA_MODE_GET:
        {
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

            if ((!app_apt_is_apt_on_state(app_db.current_listening_state) &&
                 !app_listening_is_anc_apt_on_state(app_db.current_listening_state)) &&
                (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED))
            {
                app_listening_report(EVENT_HA_MODE, &app_db.remote_current_listening_state, 1);
            }
            else
            {
                app_listening_report(EVENT_HA_MODE, &app_db.current_listening_state, 1);
            }
        }
        break;

    case CMD_HA_GET_DSP_CFG_GAIN:
        {
            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            app_ha_get_dsp_cfg_gain();
        }
        break;

    case CMD_HA_OVP_TRAINING_START:
    case CMD_HA_OVP_TRAINING_STOP:
        {
            uint8_t is_start = (cmd_id == CMD_HA_OVP_TRAINING_START);

            if (cmd_len < 2)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
            }
            else
            {
                ha_ovp_training_db.is_ready = false;
                ha_ovp_training_db.bud_is_ready = false;

                if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    uint8_t data_buf[12];

                    memset(data_buf, 0, 12);
                    app_ha_generate_ovp_training_cmd(is_start, data_buf);
                    app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, data_buf, 12);
                    app_ha_send_rha_effect();
                }
                else
                {

                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                       APP_REMOTE_MSG_HA_CTRL_OVP_TRAINING,
                                                       &is_start, 1,
                                                       REMOTE_TIMER_HIGH_PRECISION, 0,
                                                       true);
                }
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_HA_PRESET_IDX_GET:
        {
            if (cmd_len < 3)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                uint8_t mode = cmd_ptr[2];

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    uint8_t event_data[3] = {0, 0xFF, 0xFF};
                    uint8_t data_buf[HA_GLOBAL_OBJ_PRESET_IDX_SIZE];

                    app_ha_global_obj_get(HA_GLOBAL_OBJ_PRESET_IDX, data_buf);

                    event_data[0] = mode;
                    event_data[1 + app_cfg_const.bud_side] = data_buf[mode];

                    app_report_event(cmd_path, EVENT_HA_PRESET_IDX_INFO, app_idx, event_data, 3);
                }
                else
                {
                    uint16_t ctrl_data_len = 6;
                    uint8_t ctrl_data[6] = {0};
                    uint8_t type = HA_GLOBAL_OBJ_PRESET_IDX;

                    ctrl_data[0] = ctrl_data_len & 0xFF;
                    ctrl_data[1] = ctrl_data_len >> 8;
                    ctrl_data[2] = app_cfg_nv.bud_role;
                    ctrl_data[3] = type;
                    ctrl_data[4] = mode;
                    ctrl_data[5] = 0; //preset_idx column

                    app_ha_get_all_direction_global_obj(ctrl_data);
                }
            }
        }
        break;

    case CMD_HA_SET_CLASSIFY:
        {
            if (cmd_len < 3)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
            }
            else
            {
                uint8_t is_enable = cmd_ptr[2];
                uint16_t len = 0;
                uint8_t *buf = NULL;

                app_ha_dsp_ext_para_offset_get(HA_MODE_RHA, &len, HA_DSP_FEATURE_HA_ENV_CLASSIFICATION);

                if (len == 0)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                }
                else
                {

                    if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                    {
                        len = 8 + len;
                        buf = calloc(len, 1);
                        app_ha_generate_classify_cmd(is_enable, buf, len);
                        app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, buf, len);
                        app_ha_send_rha_effect();
                        free(buf);
                    }
                    else
                    {
                        app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                           APP_REMOTE_MSG_HA_SET_CLASSIFY,
                                                           &is_enable, 1,
                                                           REMOTE_TIMER_HIGH_PRECISION, 0,
                                                           true);
                    }

                    buf = NULL;
                }
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_HA_PRESET_IDX_SET:
        {
            if (cmd_len < 3)
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
            }
            else
            {
                uint8_t mode = cmd_ptr[2];
                uint8_t preset_idx = 0;
                uint8_t event_data[2] = {0};

                preset_idx = cmd_ptr[3 + app_cfg_const.bud_side];

                if (preset_idx != HA_PRESET_INVALID)
                {
                    app_ha_set_preset(mode, preset_idx);
                }

                if (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED)
                {
                    uint8_t relay_buf[2] = {0};

                    relay_buf[0] = mode;
                    //get oppisite bud_side column data to set remote bud
                    relay_buf[1] = cmd_ptr[3 + (app_cfg_const.bud_side ? 0 : 1)];

                    if (relay_buf[1] != HA_PRESET_INVALID)
                    {
                        app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA, APP_REMOTE_MSG_HA_SET_PRESET_IDX, relay_buf,
                                                            2);
                    }
                }

                app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);

                event_data[0] = mode;
                event_data[1] = HA_PRESET_SET_SUCCESS;
                app_report_event(cmd_path, EVENT_HA_PRESET_IDX_SET_INFO, app_idx, event_data, 2);
            }
        }
        break;

    case CMD_MP_HA_BYPASS_ALGORITHM:
        {
            if (app_apt_is_normal_apt_started())
            {
                uint8_t data_buf[12];

                memset(data_buf, 0, 12);
                data_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                data_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                data_buf[2] = 2;
                data_buf[4] = 8; //HA test
                data_buf[8] = 1; //HA test
                data_buf[10] = 0; //outputgain L
                data_buf[11] = 0; //outputgain H
                app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, data_buf, 12);
                app_ha_send_rha_effect();
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_MP_HA_ENABLE_ALGORITHM:
        {
            if (app_apt_is_normal_apt_started())
            {
                uint8_t data_buf[12];

                memset(data_buf, 0, 12);
                data_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                data_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                data_buf[2] = 2;
                data_buf[4] = 8; //HA test
                data_buf[8] = 0; //exit HA test
                data_buf[10] = 0; //outputgain L
                data_buf[11] = 0; //outputgain H
                app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, data_buf, 12);
                app_ha_send_rha_effect();
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_MP_HA_SET_GRAPHIC_EQ:
        {
            if (app_apt_is_normal_apt_started())
            {
                uint8_t *data_buf = NULL;
                uint8_t graphic_eq_len = 132;

                if (cmd_len - 2 != graphic_eq_len)
                {
                    ack_pkt[2] = CMD_SET_STATUS_PARAMETER_ERROR;
                }
                else
                {
                    uint8_t payload_len = ((graphic_eq_len + 3) / 4) * 4 + 8;
                    data_buf = malloc(payload_len);

                    if (data_buf)
                    {
                        memset(data_buf, 0, payload_len);
                        data_buf[0] = AUDIO_PROBE_HA_PARA & 0xFF;
                        data_buf[1] = (AUDIO_PROBE_HA_PARA & 0xFF00) >> 8;
                        data_buf[2] = (((graphic_eq_len + 4) + 3) / 4) & 0xFF;
                        data_buf[3] = ((((graphic_eq_len + 4) + 3) / 4) & 0xFF00) >> 8;
                        data_buf[4] = 7; //HA test
                        memcpy(&data_buf[8], &cmd_ptr[2], graphic_eq_len);
                        app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, data_buf, payload_len);
                        app_ha_send_rha_effect();
                        free(data_buf);
                    }
                    else
                    {
                        ack_pkt[2] = CMD_SET_STATUS_PROCESS_FAIL;
                    }
                }
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    case CMD_MP_HA_SET_FULL_ON_GAIN:
        {
            if (app_apt_is_normal_apt_started())
            {
                uint8_t data_buf[12];
                memset(data_buf, 0, 12);
                app_ha_generate_gain_db(100, 50, data_buf);
                app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, data_buf, 12);
                app_ha_send_rha_effect();
            }
            else
            {
                ack_pkt[2] = CMD_SET_STATUS_DISALLOW;
            }

            app_report_event(cmd_path, EVENT_ACK, app_idx, ack_pkt, 3);
        }
        break;

    default:
        break;
    }
}

void app_ha_handle_rha_effect_payload(uint8_t *buf)
{
    uint8_t effect_type = buf[0];
    uint16_t tool_data_len = buf[1] + (buf[2] << 8);
    uint16_t effect_l_len = buf[3] + (buf[4] << 8);
    uint16_t effect_r_len = buf[5] + (buf[6] << 8);
    uint16_t effect_offset = 0;
    uint16_t effect_len = 0;

    switch (effect_type)
    {
    case HA_EFFECT_WDRC:
    case HA_EFFECT_NR:
    case HA_EFFECT_GRAPHIC_EQ:
    case HA_EFFECT_FBC:
    case HA_EFFECT_SPEECH_ENHANCE:
    case HA_EFFECT_BF:
    case HA_EFFECT_WNR:
    case HA_EFFECT_INR:
    case HA_EFFECT_RNS:
    case HA_EFFECT_OUTPUT_DRC:
    case HA_EFFECT_ADAPTIVE_BF:
    case HA_EFFECT_OVP:
    case HA_EFFECT_MAX_TOTAL_POWER_OUTPUT_CTRL:
        {
            //effect tool_data set
            app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_TOOL_DATA,
                                   buf + 7);

            if (effect_l_len > 0 && (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT))
            {
                effect_offset = 7 + tool_data_len;
                effect_len = effect_l_len;
            }
            else if (effect_r_len > 0 && (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT))
            {
                effect_offset = 7 + tool_data_len + effect_l_len;
                effect_len = effect_r_len;
            }

            if (effect_len > 0)
            {
                switch (effect_type)
                {
                case HA_EFFECT_WDRC:
                case HA_EFFECT_GRAPHIC_EQ:
                case HA_EFFECT_FBC:
                case HA_EFFECT_SPEECH_ENHANCE:
                case HA_EFFECT_BF:
                case HA_EFFECT_WNR:
                case HA_EFFECT_INR:
                case HA_EFFECT_OUTPUT_DRC:
                case HA_EFFECT_ADAPTIVE_BF:
                case HA_EFFECT_OVP:
                case HA_EFFECT_MAX_TOTAL_POWER_OUTPUT_CTRL:
                    {
                        app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx,
                                               HA_EFFECT_TO_PROG_OBJ(effect_type),
                                               buf + effect_offset);

                        if ((audio_passthrough_state_get() == AUDIO_PASSTHROUGH_STATE_STARTED) &&
                            app_apt_is_apt_on_state(app_db.current_listening_state))
                        {
                            app_ha_push_rha_effect(HA_EFFECT_TYPE_PROG_OBJ, HA_EFFECT_TO_PROG_OBJ(effect_type));
                            app_ha_send_rha_effect();
                        }
                    }
                    break;

                case HA_EFFECT_NR:
                    {
                        app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx,
                                               HA_EFFECT_TO_PROG_OBJ(effect_type),
                                               buf + effect_offset);

                        if ((audio_passthrough_state_get() == AUDIO_PASSTHROUGH_STATE_STARTED) &&
                            app_apt_is_apt_on_state(app_db.current_listening_state))
                        {
                            app_ha_push_rha_effect(HA_EFFECT_TYPE_PROG_OBJ, HA_EFFECT_TO_PROG_OBJ(effect_type));
                            app_ha_push_rha_effect(HA_EFFECT_TYPE_PROG_OBJ, HA_EFFECT_TO_PROG_OBJ(HA_EFFECT_WDRC));
                            app_ha_push_rha_effect(HA_EFFECT_TYPE_PROG_OBJ, HA_EFFECT_TO_PROG_OBJ(HA_EFFECT_GRAPHIC_EQ));
                            app_ha_send_rha_effect();
                        }
                    }
                    break;

                case HA_EFFECT_RNS:
                    {
                        app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx,
                                               HA_EFFECT_TO_PROG_OBJ(effect_type),
                                               buf + effect_offset);

                        if ((audio_passthrough_state_get() == AUDIO_PASSTHROUGH_STATE_STARTED) &&
                            app_apt_is_apt_on_state(app_db.current_listening_state))
                        {
                            app_ha_push_rha_effect(HA_EFFECT_TYPE_PROG_OBJ, HA_EFFECT_TO_PROG_OBJ(effect_type));
                            app_ha_send_rha_effect();
                        }
                    }
                    break;
                }
            }
        }
        break;

    default:
        break;
    }
}

void app_ha_handle_ullrha_effect_payload(uint8_t *buf)
{
    uint8_t effect_type = buf[0];
    uint16_t tool_data_len = buf[1] + (buf[2] << 8);
    uint16_t effect_l_len = buf[3] + (buf[4] << 8);
    uint16_t effect_r_len = buf[5] + (buf[6] << 8);
    uint16_t effect_offset = 0;
    uint16_t effect_len = 0;

    switch (effect_type)
    {
    case HA_EFFECT_WDRC:
    case HA_EFFECT_NR:
    case HA_EFFECT_FBC:
    case HA_EFFECT_SPEECH_ENHANCE:
    case HA_EFFECT_BF:
    case HA_EFFECT_WNR:
    case HA_EFFECT_INR:
    case HA_EFFECT_RNS:
    case HA_EFFECT_OUTPUT_DRC:
    case HA_EFFECT_INTEGRATED_EQ:
    case HA_EFFECT_ADAPTIVE_BF:
    case HA_EFFECT_OVP:
    case HA_EFFECT_MAX_TOTAL_POWER_OUTPUT_CTRL:
        {
            //effect tool_data set
            app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_TOOL_DATA,
                                   buf + 7);

            if (effect_l_len > 0 && (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT))
            {
                effect_offset = 7 + tool_data_len;
                effect_len = effect_l_len;
            }
            else if (effect_r_len > 0 && (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT))
            {
                effect_offset = 7 + tool_data_len + effect_l_len;
                effect_len = effect_r_len;
            }

            //apply effects
            if (effect_len > 0)
            {
                switch (effect_type)
                {
                case HA_EFFECT_WDRC:
                case HA_EFFECT_INTEGRATED_EQ:
                case HA_EFFECT_FBC:
                case HA_EFFECT_SPEECH_ENHANCE:
                case HA_EFFECT_BF:
                case HA_EFFECT_WNR:
                case HA_EFFECT_INR:
                case HA_EFFECT_OUTPUT_DRC:
                case HA_EFFECT_ADAPTIVE_BF:
                case HA_EFFECT_OVP:
                case HA_EFFECT_RNS:
                case HA_EFFECT_MAX_TOTAL_POWER_OUTPUT_CTRL:
                    {
                        app_ha_effect_set((T_APP_HA_EFFECT)effect_type, true, buf + effect_offset, effect_len);
                        app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx,
                                               HA_EFFECT_TO_PROG_OBJ(effect_type),
                                               buf + effect_offset);
                        app_ha_wrap_send_adsp_cmd(HA_EFFECT_TYPE_PROG_OBJ, HA_EFFECT_TO_PROG_OBJ(effect_type));
                    }
                    break;

                case HA_EFFECT_NR:
                    {
                        app_ha_effect_set((T_APP_HA_EFFECT)effect_type, true, buf + effect_offset, effect_len);
                        app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx,
                                               HA_EFFECT_TO_PROG_OBJ(effect_type),
                                               buf + effect_offset);
                        app_ha_wrap_send_adsp_cmd(HA_EFFECT_TYPE_PROG_OBJ, HA_EFFECT_TO_PROG_OBJ(effect_type));
                        app_ha_wrap_send_adsp_cmd(HA_EFFECT_TYPE_PROG_OBJ, HA_EFFECT_TO_PROG_OBJ(HA_EFFECT_INTEGRATED_EQ));
                    }
                    break;
                }
            }
        }
        break;

    default:
        break;
    }
}

static void app_ha_handle_effect_payload(uint8_t *buf)
{
    if (current_ha_mode_db->mode == HA_MODE_RHA)
    {
        app_ha_handle_rha_effect_payload(buf);
    }
    else
    {
        app_ha_handle_ullrha_effect_payload(buf);
    }
}

static void app_ha_handle_rha_data_payload(uint8_t *buf)
{
    uint8_t prog_obj_type = buf[0];
    uint16_t tool_data_len  = buf[1] + (buf[2] << 8);
    uint16_t data_l_len   = buf[3] + (buf[4] << 8);
    uint16_t data_r_len   = buf[5] + (buf[6] << 8);
    uint16_t data_offset = 0;
    uint16_t data_len = 0;

    switch (prog_obj_type)
    {
    case HA_PROG_OBJ_VOLUME_LEVEL:
        {
            //effect tool_data set
            app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_TOOL_DATA,
                                   buf + 7);

            if (data_l_len > 0 && (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT))
            {
                data_offset = 7 + tool_data_len;
                data_len = data_l_len;
            }
            else if (data_r_len > 0 && (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT))
            {
                data_offset = 7 + tool_data_len + data_l_len;
                data_len = data_r_len;
            }

            if (data_len > 0)
            {
                uint8_t data_buf[12] = {0};
                uint8_t volume_level = buf[data_offset];
                uint8_t balance_level = buf[data_offset + 1];

                app_ha_generate_gain_db(volume_level, balance_level, data_buf);
                app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx, prog_obj_type,
                                       buf + data_offset);

                if ((audio_passthrough_state_get() == AUDIO_PASSTHROUGH_STATE_STARTED) &&
                    app_apt_is_apt_on_state(app_db.current_listening_state))
                {
                    app_ha_push_rha_effect(HA_EFFECT_TYPE_PROG_OBJ, HA_PROG_OBJ_VOLUME_LEVEL);
                    app_ha_send_rha_effect();
                }

            }
        }
        break;

    default:
        break;
    }
}

static void app_ha_handle_ullrha_data_payload(uint8_t *buf)
{
    uint8_t prog_obj_type = buf[0];
    uint16_t tool_data_len  = buf[1] + (buf[2] << 8);
    uint16_t data_l_len   = buf[3] + (buf[4] << 8);
    uint16_t data_r_len   = buf[5] + (buf[6] << 8);
    uint16_t data_offset = 0;
    uint16_t data_len = 0;

    switch (prog_obj_type)
    {
    case HA_PROG_OBJ_VOLUME_LEVEL:
        {
            //effect tool_data set
            app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_TOOL_DATA,
                                   buf + 7);

            if (data_l_len > 0 && (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT))
            {
                data_offset = 7 + tool_data_len;
                data_len = data_l_len;
            }
            else if (data_r_len > 0 && (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT))
            {
                data_offset = 7 + tool_data_len + data_l_len;
                data_len = data_r_len;
            }

            if (data_len > 0)
            {
                uint8_t data_buf[12] = {0};
                uint8_t volume_level = buf[data_offset];
                uint8_t balance_level = buf[data_offset + 1];

                app_ha_generate_gain_db(volume_level, balance_level, data_buf);

                app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx, prog_obj_type,
                                       buf + data_offset);

                if (app_apt_is_ullrha_state((T_ANC_APT_STATE)app_db.current_listening_state))
                {
                    app_ha_wrap_send_adsp_cmd(HA_EFFECT_TYPE_PROG_OBJ, prog_obj_type);
                }
            }
        }
        break;

    default:
        break;
    }
}

static void app_ha_handle_all_rha_effect_payload(uint8_t *buf)
{
    uint8_t effect_type = buf[0];
    uint16_t tool_data_len = buf[1] + (buf[2] << 8);
    uint16_t effect_l_len = buf[3] + (buf[4] << 8);
    uint16_t effect_r_len = buf[5] + (buf[6] << 8);
    uint16_t effect_offset = 0;
    uint16_t effect_len = 0;
    uint16_t offset = 0;
    uint8_t tool_data[HA_PROG_OBJ_TOOL_DATA_SIZE] = {0};
    uint16_t size = 0;
    uint8_t i = 0;

    switch (effect_type)
    {
    case HA_EFFECT_OUTPUT_DRC:
        {
            offset = app_ha_get_effect_ui_from_tool_data(HA_EFFECT_OUTPUT_DRC);

            //effect tool_data set
            app_ha_prog_set_object(current_ha_mode_db->prog_db->selectable_prog_idx, HA_PROG_OBJ_TOOL_DATA,
                                   buf + 7);

            if (offset < HA_PROG_OBJ_TOOL_DATA_SIZE)
            {
                for (i = 0; i < current_ha_mode_db->prog_db->prog_num; i++)
                {
                    app_ha_prog_get_object(i, HA_PROG_OBJ_TOOL_DATA, tool_data, &size);
                    memcpy(tool_data + offset, buf + 7 + offset, 16);
                    app_ha_prog_set_object(i, HA_PROG_OBJ_TOOL_DATA, tool_data);
                }
            }

            if (effect_l_len > 0 && (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT))
            {
                effect_offset = 7 + tool_data_len;
                effect_len = effect_l_len;
            }
            else if (effect_r_len > 0 && (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT))
            {
                effect_offset = 7 + tool_data_len + effect_l_len;
                effect_len = effect_r_len;
            }

            if (effect_len > 0)
            {
                app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, buf + effect_offset, effect_len);
                app_ha_send_rha_effect();

                for (i = 0; i < current_ha_mode_db->prog_db->prog_num; i++)
                {
                    app_ha_prog_set_object(i, HA_EFFECT_TO_PROG_OBJ(effect_type),
                                           buf + effect_offset);
                }
            }
        }
        break;

    default:
        break;
    }
}

static void app_ha_handle_all_effect_payload(uint8_t *buf)
{
    if (current_ha_mode_db->mode == HA_MODE_RHA)
    {
        app_ha_handle_all_rha_effect_payload(buf);
    }
}

static void app_ha_handle_data_payload(uint8_t *buf)
{
    if (current_ha_mode_db->mode == HA_MODE_RHA)
    {
        app_ha_handle_rha_data_payload(buf);
    }
    else
    {
        app_ha_handle_ullrha_data_payload(buf);
    }
}

void app_ha_handle_hearing_test_payload(uint8_t *buf, uint16_t len)
{
    switch (buf[0])
    {
    case HA_HEARING_TEST_OPCODE_CREATE:
        {
            app_ha_hearing_test_init();
        }
        break;

    case HA_HEARING_TEST_OPCODE_START:
        {
            uint8_t app_idx = app_a2dp_get_active_idx();

            app_ha_set_is_hearing_test(false);

            if (app_db.br_link[app_idx].a2dp_track_handle)
            {
                app_ha_set_is_hearing_test(true);

                if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    bt_avrcp_pause(app_db.br_link[app_idx].bd_addr);

                    if (audio_track_release(app_db.br_link[app_idx].a2dp_track_handle) == false)
                    {
                        if (app_ha_get_is_hearing_test())
                        {
                            app_ha_hearing_test_start();
                        }
                    }
                }
                else
                {
                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                       APP_REMOTE_MSG_HA_PROGRAM_STOP_A2DP_AUDIO_TRACK,
                                                       NULL, 0,
                                                       REMOTE_TIMER_HIGH_PRECISION, 0,
                                                       true);
                }
            }
            else
            {
                app_ha_set_is_hearing_test(true);

                if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
                {
                    app_ha_hearing_test_start();
                }
                else
                {
                    app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                       APP_REMOTE_MSG_HA_PROGRAM_DISABLE_AUDIO_EQ_START_HEARING_TEST,
                                                       NULL, 0,
                                                       REMOTE_TIMER_HIGH_PRECISION, 0,
                                                       true);
                }
            }
        }
        break;

    case HA_HEARING_TEST_OPCODE_STOP:
        {
            app_ha_hearing_test_stop();
            app_ha_set_is_hearing_test(false);
            app_stop_timer(&timer_idx_ha_hearing_test_tone);
        }
        break;

    case HA_HEARING_TEST_OPCODE_SET:
        {
            uint16_t freq = buf[1] + (buf[2] << 8);
            int16_t gain = buf[3] + (buf[4] << 8);
            uint16_t timeout_ms = 0;

            if (gain > HA_HEARING_TEST_TONE_MAX_GAIN)
            {
                break;
            }

            if (len == 7)
            {
                timeout_ms = buf[5] + (buf[6] << 8);
            }

            app_ha_hearing_test_tone_set((uint32_t)freq, (int32_t)gain, timeout_ms);
        }
        break;

    case HA_HEARING_TEST_OPCODE_DESTROY:
        {
            app_ha_hearing_test_deinit();
        }
        break;
    }
}

static void app_ha_handle_global_payload(uint8_t *data_buf)
{
    uint8_t target_obj_type = data_buf[0];
    uint16_t target_obj_size_l = data_buf[1] + (data_buf[2] << 8);
    uint16_t target_obj_size_r = data_buf[3] + (data_buf[4] << 8);
    uint16_t target_obj_size = 0;
    uint32_t base = app_ha_get_global_obj_base();
    uint8_t buf[4] = {0};
    uint8_t i = 0;

    if ((app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT && target_obj_size_l == 0) ||
        (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT && target_obj_size_r == 0))
    {
        return;
    }

    target_obj_size = app_cfg_const.bud_side ? target_obj_size_r : target_obj_size_l;

    data_buf += 5; //target to data_L

    if (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT)
    {
        data_buf += target_obj_size_l; //target to data_R
    }

    app_ha_get_global_obj_offset(target_obj_type);

    switch (target_obj_type)
    {
    case HA_GLOBAL_OBJ_RHA_WDRC:
    case HA_GLOBAL_OBJ_RHA_WDRC_ENABLED:
        {
            app_ha_global_obj_set((T_APP_HA_GLOBAL_OBJ)target_obj_type, data_buf, target_obj_size);

            if (app_apt_is_normal_apt_started() &&
                app_ha_get_ha_mode() == HA_MODE_RHA)
            {
                app_ha_push_rha_runtime_effect(HA_STEREO_TYPE_L, data_buf, target_obj_size);
                app_ha_send_rha_effect();
            }
        }
        break;

    case HA_GLOBAL_OBJ_ULLRHA_WDRC:
    case HA_GLOBAL_OBJ_ULLRHA_WDRC_ENABLED:
        {
            if (target_obj_size > 4)
            {
                uint8_t *ptr = NULL;
                uint16_t obj_size = 0;

                data_buf[2] = (target_obj_size - 4) & 0xFF;
                data_buf[3] = (target_obj_size - 4) >> 8;

                app_ha_global_obj_set((T_APP_HA_GLOBAL_OBJ)target_obj_type, data_buf, target_obj_size);
            }

            if (app_apt_is_ullrha_state((T_ANC_APT_STATE)app_db.current_listening_state) &&
                app_ha_get_ha_mode() == HA_MODE_ULLRHA)
            {
                app_ha_wrap_ullrha_cmd(data_buf, target_obj_size);
                audio_probe_adsp_send((uint8_t *)data_buf, target_obj_size);
            }
        }
        break;

    case HA_GLOBAL_OBJ_AUDIO_WDRC:
    case HA_GLOBAL_OBJ_AUDIO_WDRC_ENABLE:
    case HA_GLOBAL_OBJ_AUDIO_MIC_SETTINGS:
        {
            //convert ipc1.0 cmd to ipc2.0 cmd
            uint16_t payload_len = target_obj_size - 8;

            data_buf[6] = payload_len & 0xFF;
            data_buf[7] = payload_len >> 8;

            app_ha_global_obj_set((T_APP_HA_GLOBAL_OBJ)target_obj_type, data_buf + 4, target_obj_size - 4);

            if (app_a2dp_is_streaming())
            {
                app_ha_push_audio_effect(HA_EFFECT_TYPE_GLOBAL_OBJ, target_obj_type);
                app_ha_send_audio_effect();
            }
        }
        break;

    case HA_GLOBAL_OBJ_VOICE_WDRC:
    case HA_GLOBAL_OBJ_VOICE_WDRC_ENABLE:
    case HA_GLOBAL_OBJ_VOICE_MIC_SETTINGS:
        {
            //convert ipc1.0 cmd to ipc2.0 cmd
            uint16_t payload_len = target_obj_size - 8;

            data_buf[6] = payload_len & 0xFF;
            data_buf[7] = payload_len >> 8;

            app_ha_global_obj_set((T_APP_HA_GLOBAL_OBJ)target_obj_type, data_buf + 4, target_obj_size - 4);

            if (app_hfp_get_call_status() != APP_CALL_IDLE)
            {
                app_ha_push_voice_effect(HA_EFFECT_TYPE_GLOBAL_OBJ, target_obj_type);
                app_ha_send_voice_effect();
            }
        }
        break;

    default:
        {

        }
        break;
    }
}

void app_ha_handle_dsp_report_payload(uint8_t *payload)
{
    if (payload[0] == 0x40)
    {
        if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED ||
            app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            app_report_event_broadcast(EVENT_HA_CLASSIFY_INFO, payload + 4, 1);
        }
    }
}

void app_ha_cmd_cback(uint8_t msg_type, uint8_t *buf, uint16_t len)
{
    switch (msg_type)
    {
    case APP_HA_CMD_MSG_SET_EFFECT_CMD:
        {
            app_ha_handle_effect_payload(buf);
        }
        break;

    case APP_HA_DATA_MSG_SET_PARAMS:
        {
            app_ha_handle_data_payload(buf);
        }
        break;

    case APP_HA_HEARING_TEST_MSG_SET:
        {
            app_ha_handle_hearing_test_payload(buf, len);
        }
        break;

    case APP_HA_GLOBAL_OBJ_MSG_SET:
        {
            app_ha_handle_global_payload(buf);
        }
        break;

    case APP_HA_CMD_MSG_SET_ALL_PARAMS:
        {
            app_ha_handle_all_effect_payload(buf);
        }
        break;

    default:
        break;
    }

    return;
}

void app_ha_get_params_payload_cback(uint8_t msg_type, uint8_t *buf)
{
    if (buf == NULL)
    {
        return;
    }

    switch (msg_type)
    {
    case APP_HA_BANDWIDTH_EQ_PARAMS_GET:
        {
            app_ha_get_band_width_params(HA_MODE_ULLRHA, buf);
        }
        break;

    case APP_HA_CUSTOMER_EQ_PARAMS_GET:
        {
            app_ha_get_customer_eq(HA_MODE_ULLRHA, buf);
        }
        break;

    case APP_HA_RNS_COMPENSATION_PARAMS_GET:
        {
            app_ha_get_rns_compensation(HA_MODE_ULLRHA, buf);
        }
        break;

    default:
        break;
    }

    return;
}

void app_ha_get_params_payload_size_cback(uint8_t msg_type, uint16_t *len)
{
    switch (msg_type)
    {
    case APP_HA_BANDWIDTH_EQ_PARAMS_GET:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_ULLRHA, len, HA_DSP_FEATURE_HA_BAND_WIDTH);
        }
        break;

    case APP_HA_CUSTOMER_EQ_PARAMS_GET:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_ULLRHA, len, HA_DSP_FEATURE_HA_CUSTOMER_EQ);
        }
        break;

    case APP_HA_RNS_COMPENSATION_PARAMS_GET:
        {
            app_ha_dsp_ext_para_offset_get(HA_MODE_ULLRHA, len, HA_DSP_FEATURE_HA_RNS_COMPENSATION);
        }
        break;

    default:
        break;
    }

    return;
}

static void app_ha_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
    T_BT_EVENT_PARAM *param = event_buf;

    switch (event_type)
    {
    case BT_EVENT_HFP_CALL_STATUS:
        {
            T_APP_BR_LINK *p_link;

            p_link = app_link_find_br_link(param->hfp_call_status.bd_addr);

            if (p_link != NULL)
            {
                switch (param->hfp_call_status.curr_status)
                {
                case BT_HFP_CALL_INCOMING:
                    {
                        app_ha_check_trigger_stop_hearing_test(1);
                    }
                    break;

                default:
                    {

                    }
                    break;
                }
            }
        }
        break;

    case BT_EVENT_REMOTE_CONN_CMPL:
        {
            T_APP_BR_LINK *p_link = NULL;

            //sync secondary earbud's program id by primary earbud's program id
            if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
            {
                app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                    APP_REMOTE_MSG_HA_PROGRAM_SET_ID,
                                                    (uint8_t *) & (current_ha_mode_db->prog_db->selectable_prog_idx),
                                                    sizeof(uint8_t));
            }

            p_link = app_link_find_br_link(param->remote_conn_cmpl.bd_addr);

            if (p_link != NULL)
            {
                app_ha_sync_volume_sync();
            }
        }
        break;

    default:
        {

        }
        break;
    }
}

static void app_ha_probe_cback(T_AUDIO_PROBE_EVENT event, void *buf)
{
    switch (event)
    {
    case PROBE_OVP_TRAINING_REPORT:
        {
            uint8_t *ptr = (uint8_t *)buf;
            uint8_t event_data[2] = {0};

            if (app_db.remote_session_state == REMOTE_SESSION_STATE_DISCONNECTED)
            {
                uint8_t status = HA_OVP_TRAINING_STATUS_FAILED;

                if (ptr[0] & BIT0 == 1) //success
                {
                    status = HA_OVP_TRAINING_STATUS_SUCCESS;
                    app_ha_global_obj_set(HA_GLOBAL_OBJ_OVP_TRAINING_DATA, ptr + 4,
                                          HA_GLOBAL_OBJ_OVP_TRAINING_DATA_SIZE);
                }

                event_data[0] = status;
                event_data[1] = ptr[0] >> 1; //score

                app_report_event_broadcast(EVENT_HA_OVP_TRAINING_STATUS_INFO, event_data, 2);
            }
            else
            {
                if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
                {
                    app_ha_set_ovp_training_status(REMOTE_SESSION_ROLE_PRIMARY, ptr[0] & BIT0);

                    if (ptr[0] & BIT0 == 1) //success
                    {
                        app_ha_global_obj_set(HA_GLOBAL_OBJ_OVP_TRAINING_DATA, ptr + 4,
                                              HA_GLOBAL_OBJ_OVP_TRAINING_DATA_SIZE);
                    }

                    if (app_ha_get_ovp_training_is_ready_status()) //if pri, sec buds are ready, report training status
                    {
                        uint8_t status = HA_OVP_TRAINING_STATUS_FAILED;

                        if (app_ha_get_ovp_training_status() == 1) //train success
                        {
                            status = HA_OVP_TRAINING_STATUS_SUCCESS;
                        }
                        else //train failed, reset both buds' training params
                        {
                            app_relay_sync_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                               APP_REMOTE_MSG_HA_CTRL_OVP_TRAINING_RESET_PARAMS,
                                                               &status, 1,
                                                               REMOTE_TIMER_HIGH_PRECISION, 0,
                                                               true);
                        }

                        event_data[0] = status;
                        event_data[1] = ptr[0] >> 1; //score

                        app_report_event_broadcast(EVENT_HA_OVP_TRAINING_STATUS_INFO, event_data, 2);
                    }
                }
                else //secondary
                {
                    app_relay_async_single_with_raw_msg(APP_MODULE_TYPE_HA,
                                                        APP_REMOTE_MSG_HA_PROGRAM_RELAY_OVP_TRAINING_STATUS,
                                                        (uint8_t *)ptr,
                                                        1);

                    if (ptr[0] == 1) //success
                    {
                        app_ha_global_obj_set(HA_GLOBAL_OBJ_OVP_TRAINING_DATA, ptr + 4,
                                              HA_GLOBAL_OBJ_OVP_TRAINING_DATA_SIZE);
                    }
                }
            }
        }
        break;

    case PROBE_HA_STATUS_REPORT:
        {
            uint8_t *ptr = (uint8_t *)buf;

            app_ha_handle_dsp_report_payload(ptr);
        }
        break;

    default:
        {

        }
        break;
    }
}

void app_ha_multi_ha_mode_init()
{
    uint8_t i = 0;
    uint8_t mode_num = 0;
    T_HA_MODE_DB *prev_ptr = NULL;
    T_HA_MODE_DB *cur_ptr = NULL;

    ullrha_is_enable = app_ha_ullrha_is_enable();
    mode_num = ullrha_is_enable ? 2 : 1;
    ha_mode_db = calloc(mode_num, sizeof(T_HA_MODE_DB));
    params_buf = calloc(HA_MODE_PARAMS_BUF_SIZE, 1);

    for (i = 0; i < mode_num; i++)
    {
        cur_ptr = ha_mode_db + i;
        cur_ptr->mode = (T_APP_HA_MODE)i;

        cur_ptr->effect_db = calloc(1, sizeof(T_HA_EFFECT_DB));
        cur_ptr->effect_db->queue = calloc(1, sizeof(T_OS_QUEUE));
        os_queue_init(cur_ptr->effect_db->queue);

        cur_ptr->prog_db = calloc(1, sizeof(T_HA_PROG_DB));
        cur_ptr->prog_db->obj_queue = calloc(1, sizeof(T_OS_QUEUE));
        os_queue_init(cur_ptr->prog_db->obj_queue);

        if (prev_ptr)
        {
            prev_ptr->next = cur_ptr;
        }

        cur_ptr->next = NULL;
        prev_ptr = cur_ptr;
    }

    current_ha_mode_db = ha_mode_db;

    while (current_ha_mode_db)
    {
        app_ha_effect_set_scheme();
        app_ha_prog_init();
        current_ha_mode_db = current_ha_mode_db->next;
    }

    if (ullrha_is_enable)
    {
        ha_ull_effect_db = calloc(1, sizeof(T_HA_ULL_EFFECT_DB));
        ha_ull_effect_db->count = 0;

        ha_ull_effect_db->queue = calloc(1, sizeof(T_OS_QUEUE));
        os_queue_init(ha_ull_effect_db->queue);
        ha_ull_effect_db->queue->count = 0;

        ha_adsp_mgr = calloc(1, sizeof(T_HA_ADSP_MGR));
        ha_adsp_mgr->is_send = false;
    }
}

static void app_ha_audio_probe_adsp_cback(T_AUDIO_PROBE_ADSP_EVENT event, uint8_t *buf, uint8_t len)
{
    switch (event)
    {
    case PROBE_ADSP_BOOT_DONE:
        {
            T_HA_EFFECT_TYPE effect_type;
            uint16_t effect = 0;
            uint32_t require_mips = 80;
            uint32_t real_mips = 0;

            ha_ull_effect_db->sel_idx = 0;
            ha_ull_effect_db->count = 0;
            ha_adsp_mgr->is_send = true;

            if (app_ha_set_ha_mode(HA_MODE_ULLRHA) == false)
            {
                break;
            }

            pm_dsp3_freq_set(require_mips, &real_mips);

            app_ha_push_ullrha_effect(HA_EFFECT_TYPE_CTRL_OBJ, HA_ADSP_CMD_BOOT_ACK);

            current_ha_mode_db->effect_db->apply_cback();

            app_ha_push_ullrha_effect(HA_EFFECT_TYPE_CTRL_OBJ, HA_ADSP_CMD_INIT_FINISH);

            while (true)
            {
                if (app_ha_pop_ullrha_effect(&effect_type, &effect))
                {
                    if (app_ha_wrap_send_adsp_cmd(effect_type, effect))
                    {
                        break;
                    }
                }
                else
                {
                    ha_adsp_mgr->is_send = false;
                    break;
                }
            }
        }
        break;

    case PROBE_ADSP_ACK:
        {
            T_HA_EFFECT_TYPE effect_type;
            uint16_t effect = 0;

            if (ha_adsp_mgr->is_send == true)
            {
                while (true)
                {
                    if (app_ha_pop_ullrha_effect(&effect_type, &effect))
                    {
                        if (app_ha_wrap_send_adsp_cmd(effect_type, effect))
                        {
                            break;
                        }
                    }
                    else
                    {
                        ha_adsp_mgr->is_send = false;
                        break;
                    }
                }
            }
        }
        break;

    default:
        {

        }
        break;
    }
}

void app_ha_init()
{
    app_apt_set_first_ullrha_scenario(&app_db.last_ullrha_state);

    app_apt_set_first_normal_rha_scenario(&app_db.last_normal_rha_state);

    ha_ext_ftl_storage_init();

    os_queue_init(&ha_params_db.obj_queue);

    group_ltv_data = calloc(GROUP_LTV_DATA_MAX_SIZE, 1);

    ha_cmd_db = calloc(1, sizeof(T_HA_CMD_DB));
    ha_cmd_db->queue = calloc(1, sizeof(T_OS_QUEUE));
    os_queue_init(ha_cmd_db->queue);

    audio_cmd_db = calloc(1, sizeof(T_HA_CMD_DB));
    audio_cmd_db->queue = calloc(1, sizeof(T_OS_QUEUE));
    os_queue_init(audio_cmd_db->queue);

    voice_cmd_db = calloc(1, sizeof(T_HA_CMD_DB));
    voice_cmd_db->queue = calloc(1, sizeof(T_OS_QUEUE));
    os_queue_init(voice_cmd_db->queue);

    app_ha_params_init();

    app_ha_check_init();

    app_ha_multi_ha_mode_init();

    app_ha_global_obj_init();

    app_timer_reg_cb(app_ha_timeout_cb, &ha_timer_id);
#if F_APP_ERWS_SUPPORT
    app_relay_cback_register(app_ha_relay_cback, app_ha_parse_cback,
                             APP_MODULE_TYPE_HA, APP_REMOTE_MSG_HA_TOTAL);
#endif
    app_cmd_cback_register(app_ha_cmd_cback, APP_CMD_MODULE_TYPE_HA);
    app_cmd_retrieve_cback_register(app_ha_get_params_payload_cback,
                                    app_ha_get_params_payload_size_cback, APP_CMD_MODULE_TYPE_HA);
    bt_mgr_cback_register(app_ha_bt_cback);
    audio_mgr_cback_register(app_ha_audio_cback);
    app_ha_effect_apply_cback_register(app_ha_effect_apply_cback);
    audio_probe_dsp_cback_register(app_ha_probe_cback);
    audio_probe_adsp_cback_register(app_ha_audio_probe_adsp_cback);

    ha_instance = audio_vse_create(AUDIO_VSE_COMPANY_ID_REALTEK, 0x00, app_ha_rha_vse_cback);
    audio_passthrough_effect_attach(ha_instance);
}

#endif
