/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_CMD_H_
#define _APP_CMD_H_

#include <stdint.h>
#include <stdbool.h>

#include "app_flags.h"
#include "app_report.h"
#include "app_relay.h"
#include "app_link_util.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/** @defgroup APP_CMD App Cmd
  * @brief App Cmd
  * @{
  */

/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup APP_CMD_Exported_Macros App Cmd Macros
   * @{
   */

#define CMD_SET_VER_MAJOR                   0x01
#define CMD_SET_VER_MINOR                   0x08
#define EQ_SPEC_VER_MAJOR                   0x03
#define EQ_SPEC_VER_MINOR                   0x01
#define EQ_SPEC_VER_MAJOR_OLD               0x01    /* for support old dsp config */
#define EQ_SPEC_VER_MINOR_OLD               0x00    /* for support old dsp config */
#define CMD_SYNC_BYTE                       0xAA

//align dsp_driver.h/codec_driver.h define
#define APP_MIC_SEL_DMIC_1                  0x00
#define APP_MIC_SEL_DMIC_2                  0x01
#define APP_MIC_SEL_AMIC_1                  0x02
#define APP_MIC_SEL_AMIC_2                  0x03
#define APP_MIC_SEL_AMIC_3                  0x04
#define APP_MIC_SEL_DISABLE                 0x07

#define CHECK_IS_LCH        (app_cfg_const.bud_side == DEVICE_BUD_SIDE_LEFT)
#define CHECK_IS_RCH        (app_cfg_const.bud_side == DEVICE_BUD_SIDE_RIGHT)

#define INVALID_VALUE       0xFF
#define L_CH_PRIMARY        0x01
#define R_CH_PRIMARY        0x02

#define L_CH_BATT_LEVEL     CHECK_IS_LCH ? app_db.local_batt_level : (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ? app_db.remote_batt_level : 0
#define R_CH_BATT_LEVEL     CHECK_IS_RCH ? app_db.local_batt_level : (app_db.remote_session_state == REMOTE_SESSION_STATE_CONNECTED) ? app_db.remote_batt_level : 0

// for CMD_DEDICATE_BUD_COUPLING
#define CMD_TYPE_QUERY                          0x00
#define CMD_TYPE_UPDATE                         0x01

#if F_APP_DEVICE_CMD_SUPPORT
//for CMD_INQUIRY
#define START_INQUIRY                           0x00
#define STOP_INQUIRY                            0x01
#define NORMAL_INQUIRY                          0x00
#define PERIODIC_INQUIRY                        0x01
#define MAX_INQUIRY_TIME                        0x30

//for CMD_SERVICES_SEARCH
#define START_SERVICES_SEARCH                   0x00
#define STOP_SERVICES_SEARCH                    0x01

//for CMD_PAIR_REPLY and CMD_SSP_CONFIRMATION
#define ACCEPT_PAIRING_REQ                      0x00
#define REJECT_PAIRING_REQ                      0x01

//for CMD_SET_PAIRING_CONTROL
#define ENABLE_AUTO_ACCEPT_ACL_ACF_REQ          0x00
#define ENABLE_AUTO_REJECT_ACL_ACF_REQ          0x01
#define FORWARD_ACL_ACF_REQ_TO_HOST             0x02

//for CMD_GET_REMOTE_DEV_ATTR_INFO
#define GET_AVRCP_ATTR_INFO                     0x00
#define GET_PBAP_ATTR_INFO                      0x01
#endif

#if F_APP_AVRCP_CMD_SUPPORT
#define ALL_ELEMENT_ATTR                        0x00
#define MAX_NUM_OF_ELEMENT_ATTR                 0x07
#endif

//for CMD_GET_LINK_KEY
#define GET_ALL_LINK_KEY                        0x00
#define GET_SPECIAL_ADDR_LINK_KEY               0x01
#define GET_PRORITY_LINK_KEY                    0x02

//for CMD_SET_AND_READ_DLPS
#define SET_DLPS_DISABLE                        0x00
#define SET_DLPS_ENABLE                         0x01
#define GET_DLPS_STATUS                         0x02

// for command handler
#define PARAM_START_POINT                       2
#define COMMAND_ID_LENGTH                       2

#define APP_CMD_RELAY_HEADER_LEN                4

#define MP_CMD_HCI_OPCODE                       0xFCEB

/** End of APP_CMD_Exported_Macros
    * @}
    */
/*============================================================================*
 *                              Types
 *============================================================================*/
/** @defgroup APP_CMD_Exported_Types App Cmd Types
  * @{
  */
/**  @brief  embedded uart, spp or le vendor command type.
  *    <b> Only <b> valid when BT SOC connects to external MCU via data uart, spp or le.
  *    refer to SDK audio sample code for definition
  */
typedef enum
{
    CMD_ACK                             = 0x0000,
    CMD_BT_READ_PAIRED_RECORD           = 0x0001,
    CMD_BT_CREATE_CONNECTION            = 0x0002,
    CMD_BT_DISCONNECT                   = 0x0003,
    CMD_MMI                             = 0x0004,
    CMD_LEGACY_DATA_TRANSFER            = 0x0005,
    CMD_ASSIGN_BUFFER_SIZE              = 0x0006,
    CMD_BT_READ_LINK_INFO               = 0x0007,
    CMD_TONE_GEN                        = 0x0008,
    CMD_BT_GET_REMOTE_NAME              = 0x0009,
    CMD_BT_IAP_LAUNCH_APP               = 0x000A,
    CMD_TTS                             = 0x000B,
    CMD_INFO_REQ                        = 0x000C,

    CMD_DAC_GAIN_CTRL                   = 0x000F,
    CMD_ADC_GAIN_CTRL                   = 0x0010,
    CMD_BT_SEND_AT_CMD                  = 0x0011,
    CMD_SET_CFG                         = 0x0012,
    CMD_INDICATION                      = 0x0013,
    CMD_LINE_IN_CTRL                    = 0x0014,
    CMD_LANGUAGE_GET                    = 0x0015,
    CMD_LANGUAGE_SET                    = 0x0016,
    CMD_GET_CFG_SETTING                 = 0x0017,
    CMD_GET_STATUS                      = 0x0018,  // used to get part of bud info
    CMD_SUPPORT_MULTILINK               = 0x0019,

    CMD_BT_HFP_DIAL_WITH_NUMBER         = 0x001B,
    CMD_GET_BD_ADDR                     = 0x001C,
    CMD_STRING_MODE                     = 0x001E,
    CMD_SET_VP_VOLUME                   = 0x001F,

    CMD_SET_AND_READ_DLPS               = 0x0020,
    CMD_GET_BUD_INFO                    = 0x0021,  // used to get complete bud info

#if F_APP_DEVICE_CMD_SUPPORT
    CMD_INQUIRY                         = 0x0022,
    CMD_SERVICES_SEARCH                 = 0x0023,
    CMD_SET_PAIRING_CONTROL             = 0x0024,
    CMD_SET_PIN_CODE                    = 0x0025,
    CMD_GET_PIN_CODE                    = 0x0026,
    CMD_PAIR_REPLY                      = 0x0027,
    CMD_SSP_CONFIRMATION                = 0x0028,
    CMD_GET_CONNECTED_DEV_ID            = 0x0029,
    CMD_GET_REMOTE_DEV_ATTR_INFO        = 0x002A,
#endif

    CMD_DEDICATE_BUD_COUPLING           = 0x002B,
    CMD_APK_STATE_UPDATE_NOTIFY         = 0x002C,

    CMD_LE_START_ADVERTISING            = 0x0100,
    CMD_LE_STOP_ADVERTISING             = 0x0101,
    CMD_LE_DATA_TRANSFER                = 0x0102,
    CMD_LE_START_SCAN                   = 0x0103,
    CMD_LE_STOP_SCAN                    = 0x0104,
    CMD_LE_GET_ADDR                     = 0x0105,

    CMD_ANCS_REGISTER                   = 0x0110,
    CMD_ANCS_GET_NOTIFICATION_ATTR      = 0x0111,
    CMD_ANCS_GET_APP_ATTR               = 0x0112,
    CMD_ANCS_PERFORM_NOTIFICATION_ACTION = 0x0113,

    CMD_AMS_REGISTER                    = 0x0114,
    CMD_AMS_WRITE_REMOTE_CMD            = 0x0115,
    CMD_AMS_WRITE_ENTITY_UPD_CMD        = 0x0116,

    CMD_AUDIO_EQ_QUERY                  = 0x0200,
    CMD_AUDIO_EQ_PARAM_SET              = 0x0203,
    CMD_AUDIO_EQ_PARAM_GET              = 0x0204,
    CMD_AUDIO_EQ_INDEX_SET              = 0x0205,
    CMD_AUDIO_EQ_INDEX_GET              = 0x0206,
    CMD_AUDIO_DSP_CTRL_SEND             = 0x0207,
    CMD_AUDIO_CODEC_CTRL_SEND           = 0x0208,
    CMD_SEND_RAW_PAYLOAD                = 0x0209,
#if 0
    //supported only in cmd set version v0.0.0.1
    CMD_DSP_GET_AUDIO_EQ_SETTING_IDX    = 0x0207,
    CMD_DSP_SET_AUDIO_EQ_SETTING_IDX    = 0x0208,
    CMD_DSP_SET_APT_GAIN                = 0x0209,
#endif
    CMD_SET_VOLUME                      = 0x020A,
#if F_APP_APT_SUPPORT
    CMD_APT_EQ_INDEX_SET                = 0x020B,
    CMD_APT_EQ_INDEX_GET                = 0x020C,
#endif
    CMD_DSP_DEBUG_SIGNAL_IN             = 0x020D,

#if F_APP_APT_SUPPORT
    CMD_SET_APT_VOLUME_OUT_LEVEL        = 0x020E,
    CMD_GET_APT_VOLUME_OUT_LEVEL        = 0x020F,
#endif

    // for equalizer page
    CMD_AUDIO_EQ_QUERY_PARAM            = 0x0210,

    CMD_SET_NOTIFICATION_VOL_LEVEL      = 0x0211,
    CMD_GET_NOTIFICATION_VOL_LEVEL      = 0x0212,
    CMD_DSP_TOOL_OPERATION              = 0x0213,  // for DSP tool

    CMD_RESET_EQ_DATA                   = 0x0214,
    CMD_SET_SIDETONE                    = 0x0215,

    CMD_SET_SPATIAL_AUDIO               = 0x0216,
    CMD_AUDIO_DSP_SCENARIO_CHECK        = 0x0217,

    CMD_GET_RAW_PAYLOAD                 = 0x0218,

#if F_APP_DATA_CAPTURE_SUPPORT
    CMD_DATA_CAPTURE_START_STOP         = 0x0220,
    CMD_DATA_CAPTURE_ENTER_EXIT         = 0x0222,
#endif

    //for good test
    CMD_LED_TEST                        = 0x0300,
    CMD_CLEAR_MP_DATA                   = 0x0301,
    CMD_BT_GET_LOCAL_ADDR               = 0x0302,
    CMD_GET_LEGACY_RSSI                 = 0x0303,
    CMD_GET_RF_POWER                    = 0x0304,
    CMD_GET_CRYSTAL_TRIM                = 0x0305,
    CMD_GET_LINK_KEY                    = 0x0306,
    CMD_GET_COUNTRY_CODE                = 0x0307,
    CMD_GET_FW_VERSION                  = 0x0308,
    CMD_BT_BOND_INFO_CLEAR              = 0x0309,
    CMD_GET_ADC_VALUE_1                 = 0x030A,
    CMD_GET_ADC_VALUE_2                 = 0x030B,
    CMD_GET_UNSIZE_RAM                  = 0x030C,
    CMD_GET_FLASH_DATA                  = 0x030D,
    CMD_MIC_SWITCH                      = 0x030E,
    CMD_GET_PACKAGE_ID                  = 0x030F,
    CMD_SWITCH_TO_HCI_DOWNLOAD_MODE     = 0x0310,
    CMD_GET_PAD_VOLTAGE                 = 0x0311,
    CMD_PX318J_CALIBRATION              = 0x0312,
    CMD_READ_CODEC_REG                  = 0x0313,
    CMD_WRITE_CODEC_REG                 = 0x0314,
    CMD_GET_NUM_OF_CONNECTION           = 0x0315,
    CMD_REG_ACCESS                      = 0X0316,
    CMD_DSP_TEST_MODE                   = 0X0317,
    CMD_IQS773_CALIBRATION              = 0x0318,
    CMD_GET_IMAGE_INFO                  = 0x0319,
    CMD_GET_MAX_TRANSMIT_SIZE           = 0x031A,
    CMD_DUMP_FLASH_DATA                 = 0x031B,
    CMD_LOG_SWITCH                      = 0x0321,
    CMD_LOG_MASK_SET                    = 0x0322,
    CMD_LOG_MASK_GET                    = 0x0323,
    CMD_ROLESWAP_ENABLE                 = 0x0324,

    CMD_RF_XTAK_K                       = 0x032A,
    CMD_RF_XTAL_K_GET_RESULT            = 0x032B,

    //0x0400 ~ 0x04FF reserved for profile
#if F_APP_HFP_CMD_SUPPORT
    CMD_SEND_DTMF                       = 0x0400,
    CMD_GET_OPERATOR                    = 0x0401,
    CMD_GET_SUBSCRIBER_NUM              = 0x0402,
#endif

#if F_APP_AVRCP_CMD_SUPPORT
    CMD_AVRCP_LIST_SETTING_ATTR         = 0x0410,
    CMD_AVRCP_LIST_SETTING_VALUE        = 0x0411,
    CMD_AVRCP_GET_CURRENT_VALUE         = 0x0412,
    CMD_AVRCP_SET_VALUE                 = 0x0413,
    CMD_AVRCP_ABORT_DATA_TRANSFER       = 0x0414,
    CMD_AVRCP_SET_ABSOLUTE_VOLUME       = 0x0415,
    CMD_AVRCP_GET_PLAY_STATUS           = 0x0416,
    CMD_AVRCP_GET_ELEMENT_ATTR          = 0x0417,
#endif

#if F_APP_PBAP_CMD_SUPPORT
    CMD_PBAP_DOWNLOAD                   = 0x0420,
    CMD_PBAP_DOWNLOAD_CONTROL           = 0x0421,
    CMD_PBAP_DOWNLOAD_GET_SIZE          = 0x0422,
#endif

    CMD_OTA_DEV_INFO                    = 0x0600,
    CMD_OTA_IMG_VER                     = 0x0601,
    CMD_OTA_START                       = 0x0602,
    CMD_OTA_PACKET                      = 0x0603,
    CMD_OTA_VALID                       = 0x0604,
    CMD_OTA_RESET                       = 0x0605,
    CMD_OTA_ACTIVE_RESET                = 0x0606,
    CMD_OTA_BUFFER_CHECK_ENABLE         = 0x0607,
    CMD_OTA_BUFFER_CHECK                = 0x0608,
    CMD_OTA_IMG_INFO                    = 0x0609,
    CMD_OTA_SECTION_SIZE                = 0x060A,
    CMD_OTA_DEV_EXTRA_INFO              = 0x060B,
    CMD_OTA_PROTOCOL_TYPE               = 0x060C,
    CMD_OTA_GET_RELEASE_VER             = 0x060D,
    CMD_OTA_INACTIVE_BANK_VER           = 0x060E,
    CMD_OTA_COPY_IMG                    = 0x060F,
    CMD_OTA_CHECK_SHA256                = 0x0610,
    CMD_OTA_ROLESWAP                    = 0x0611,
    CMD_OTA_TEST_EN                     = 0x0612,

#if (F_APP_LOCAL_PLAYBACK_SUPPORT == 1)
    /* only support BBPro2 */
    CMD_PLAYBACK_QUERY_INFO                 = 0x0680,
    CMD_PLAYBACK_GET_LIST_DATA              = 0x0681,
    CMD_PLAYBACK_TRANS_START                = 0x0682,
    CMD_PLAYBACK_TRANS_CONTINUE             = 0x0683,
    CMD_PLAYBACK_REPORT_BUFFER_CHECK        = 0x0684,
    CMD_PLAYBACK_VALID_SONG                 = 0x0685,
    CMD_PLAYBACK_TRIGGER_ROLE_SWAP          = 0x0686,
    CMD_PLAYBACK_TRANS_CANCEL               = 0x0687,
    CMD_PLAYBACK_EXIT_TRANS                 = 0x0688,
    CMD_PLAYBACK_PERMANENT_DELETE_SONG      = 0x0689,
    CMD_PLAYBACK_PLAYLIST_ADD_SONG          = 0x068A,
    CMD_PLAYBACK_PLAYLIST_DELETE_SONG       = 0x068B,
    CMD_PLAYBACK_PERMANENT_DELETE_ALL_SONG  = 0x068C,
#endif

    CMD_GET_SUPPORTED_MMI_LIST          = 0x0700,
    CMD_GET_SUPPORTED_CLICK_TYPE        = 0x0701,
    CMD_GET_SUPPORTED_CALL_STATUS       = 0x0702,
    CMD_GET_KEY_MMI_MAP                 = 0x0703,
    CMD_SET_KEY_MMI_MAP                 = 0x0704,

    CMD_RESET_KEY_MMI_MAP               = 0x0707,

    CMD_GET_RWS_KEY_MMI_MAP             = 0x0708,
    CMD_SET_RWS_KEY_MMI_MAP             = 0x0709,
    CMD_RESET_RWS_KEY_MMI_MAP           = 0x070A,

    CMD_VENDOR_SEPC                     = 0x0800, //It has been reserved for vendor customer A, please dont't use this value.

    CMD_DFU_START                       = 0x0900,

    //for customize
    CMD_RSV1                            = 0x0A00,
    CMD_RSV2                            = 0x0A01,
    CMD_RSV3                            = 0x0A02,
    CMD_SET_MERIDIAN_SOUND_EFFECT_MODE  = 0x0A03,
    CMD_LG_CUSTOMIZED_FEATURE           = 0x0A04,
    CMD_MALLEUS_CUSTOMIZED_FEATURE      = 0x0A05,
    CMD_JSA_CALIBRATION                 = 0x0A06,
    CMD_MIC_MP_VERIFY_BY_HFP            = 0x0A07,
    CMD_GET_DSP_CONFIG_GAIN             = 0x0A08,
    CMD_CUSTOMIZED_TOZO_FEATURE         = 0x0A09,
    CMD_RSV4                            = 0x0A0A,
    CMD_IO_PIN_PULL_HIGH                = 0x0A0B,
    CMD_ENTER_BAT_OFF_MODE              = 0x0A0C,
    CMD_SASS_FEATURE                    = 0x0A0D,

    //for HCI command
    CMD_ANC_VENDOR_COMMAND              = 0x0B00,
    CMD_WDG_RESET                       = 0x0B01,
    CMD_DUAL_MIC_MP_VERIFY              = 0x0B02,

    CMD_SOUND_PRESS_CALIBRATION         = 0x0B10,
    CMD_CAP_TOUCH_CTL                   = 0x0B11,
    CMD_GET_IMU_SENSOR_DATA             = 0x0B12,

    CMD_SAVE_DATA_TO_FLASH              = 0x0B30,

    //for ANC command
    CMD_ANC_TEST_MODE                   = 0x0C00,
    CMD_ANC_WRITE_GAIN                  = 0x0C01,
    CMD_ANC_READ_GAIN                   = 0x0C02,
    CMD_ANC_BURN_GAIN                   = 0x0C03,
    CMD_ANC_COMPARE                     = 0x0C04,
    CMD_ANC_GEN_TONE                    = 0x0C05,
    CMD_ANC_CONFIG_DATA_LOG             = 0x0C06,
    CMD_ANC_READ_DATA_LOG               = 0x0C07,
    CMD_ANC_READ_MIC_CONFIG             = 0x0C08,
    CMD_ANC_READ_SPEAKER_CHANNEL        = 0x0C09,
    CMD_ANC_READ_REGISTER               = 0x0C0A,
    CMD_ANC_WRITE_REGISTER              = 0x0C0B,
    CMD_LLAPT_WRITE_GAIN                = 0x0C0C,
    CMD_ANC_LLAPT_READ_GAIN             = 0x0C0D,
    CMD_LLAPT_BURN_GAIN                 = 0x0C0E,
    CMD_ANC_LLAPT_FEATURE_CONTROL       = 0x0C0F,

    CMD_ANC_GEN_TONE_CHANNEL_SET        = 0x0C10, // deprecated, reserved for BBLite
    CMD_ANC_CONFIG_MEASURE_MIC          = 0x0C11,
    CMD_ANC_LLAPT_WRITE_GAIN            = 0x0C12,
    CMD_ANC_LLAPT_BURN_GAIN             = 0x0C13,

    CMD_ANC_QUERY                       = 0x0C20,
    CMD_ANC_ENABLE_DISABLE              = 0x0C21, // discard for phone
    CMD_LLAPT_QUERY                     = 0x0C22,
    CMD_LLAPT_ENABLE_DISABLE            = 0x0C23, // discard
    CMD_SPECIFY_ANC_QUERY               = 0x0C24,

    CMD_RAMP_GET_INFO                   = 0x0C26,
    CMD_RAMP_BURN_PARA_START            = 0x0C27,
    CMD_RAMP_BURN_PARA_CONTINUE         = 0x0C28,
    CMD_RAMP_BURN_GRP_INFO              = 0x0C29,
    CMD_RAMP_MULTI_DEVICE_APPLY         = 0x0C2A,

    CMD_LISTENING_MODE_CYCLE_SET        = 0x0C2B,
    CMD_LISTENING_MODE_CYCLE_GET        = 0x0C2C,

    CMD_VENDOR_SPP_COMMAND              = 0x0C2D,

    CMD_APT_VOLUME_INFO                 = 0x0C2E,
    CMD_APT_VOLUME_SET                  = 0x0C2F,
    CMD_APT_VOLUME_STATUS               = 0x0C30,
    CMD_LLAPT_BRIGHTNESS_INFO           = 0x0C31,
    CMD_LLAPT_BRIGHTNESS_SET            = 0x0C32,
    CMD_LLAPT_BRIGHTNESS_STATUS         = 0x0C33,
    CMD_LLAPT_SCENARIO_CHOOSE_INFO      = 0x0C36,
    CMD_LLAPT_SCENARIO_CHOOSE_TRY       = 0x0C37,
    CMD_LLAPT_SCENARIO_CHOOSE_RESULT    = 0x0C38,
    CMD_APT_GET_POWER_ON_DELAY_TIME     = 0x0C39,
    CMD_APT_SET_POWER_ON_DELAY_TIME     = 0x0C3A,
    CMD_LISTENING_STATE_SET             = 0x0C3B,
    CMD_LISTENING_STATE_STATUS          = 0x0C3C,
    CMD_APT_TYPE_QUERY                  = 0x0C3D,
    CMD_ASSIGN_APT_TYPE                 = 0x0C3E,

    // ADSP PARA
    CMD_ANC_GET_ADSP_INFO               = 0x0C40,
    CMD_ANC_SEND_ADSP_PARA_START        = 0x0C41,
    CMD_ANC_SEND_ADSP_PARA_CONTINUE     = 0x0C42,
    CMD_ANC_ENABLE_DISABLE_ADAPTIVE_ANC = 0x0C43,

    CMD_ANC_SCENARIO_CHOOSE_INFO        = 0x0C44,
    CMD_ANC_SCENARIO_CHOOSE_TRY         = 0x0C45,
    CMD_ANC_SCENARIO_CHOOSE_RESULT      = 0x0C46,

    CMD_APT_VOLUME_MUTE_SET             = 0x0C47,
    CMD_APT_VOLUME_MUTE_STATUS          = 0x0C48,

    CMD_ANC_GET_SCENARIO_IMAGE          = 0x0C49,
    CMD_ANC_GET_FILTER_INFO             = 0x0C4A,

    // OTA Tooling section
    CMD_OTA_TOOLING_PARKING             = 0x0D00,
    CMD_MEMORY_DUMP                     = 0x0D22,

#if F_APP_SAIYAN_MODE
    CMD_SAIYAN_GAIN_CTL                 = 0x0D29,
#endif

    CMD_GET_LOW_LATENCY_MODE_STATUS     = 0x0E01,
    CMD_GET_EAR_DETECTION_STATUS        = 0x0E02,
    CMD_GET_GAMING_MODE_EQ_INDEX        = 0x0E03,
    CMD_SET_LOW_LATENCY_LEVEL           = 0x0E04,

    CMD_MP_TEST                         = 0x0F00,
    CMD_FORCE_ENGAGE                    = 0x0F01,
    CMD_AGING_TEST_CONTROL              = 0x0F02,
    CMD_ADP_CMD_CONTROL                 = 0x0F03,

#if F_APP_DURIAN_SUPPORT
    CMD_AVP_LD_EN                       = 0x1100,
    CMD_AVP_ANC_CYCLE_SET               = 0x1101,
    CMD_AVP_CLICK_SET                   = 0x1102,
    CMD_AVP_CONTROL_SET                 = 0x1103,
    CMD_AVP_ANC_SETTINGS                = 0x1104,
#endif

#if F_APP_SAIYAN_EQ_FITTING
    CMD_HW_EQ_TEST_MODE                     = 0x1200,
    CMD_HW_EQ_START_SET                     = 0x1201,
    CMD_HW_EQ_CONTINUE_SET                  = 0x1202,
    CMD_HW_EQ_GET_CALIBRATE_FLAG            = 0x1203,
    CMD_HW_EQ_CLEAR_CALIBRATE_FLAG          = 0x1204,
    CMD_HW_EQ_SET_TEST_MODE_TMP_EQ          = 0x1205,
    CMD_HW_EQ_SET_TEST_MODE_TMP_EQ_ADVANCED = 0x1206,
#endif

#if F_APP_HEARABLE_SUPPORT
    CMD_HA_ACCESS_PROGRAM               = 0x2000,
    CMD_HA_SPK_RESPONSE                 = 0x2001,
    CMD_HA_GET_DSP_CFG_GAIN             = 0x2002,
    CMD_HA_AUDIO_VOLUME_GET             = 0x2003,
    CMD_HA_VOLUME_SYNC_STATUS           = 0x2004,
    CMD_HA_VOLUME_SYNC_STATUS_GET       = 0x2005,
    CMD_HA_PROGRAM_NR_GET               = 0x2006,
    CMD_HA_PROGRAM_NR_SET               = 0x2007,
    CMD_HA_PROGRAM_SPEECH_ENHANCE_GET   = 0x2008,
    CMD_HA_PROGRAM_SPEECH_ENHANCE_SET   = 0x2009,
    CMD_HA_PROGRAM_VOL_GET              = 0x200A,
    CMD_HA_PROGRAM_VOL_SET              = 0x200B,
    CMD_HA_PROGRAM_BF_GET               = 0x200C,
    CMD_HA_PROGRAM_BF_SET               = 0x200D,
    CMD_HA_OVP_TRAINING_START           = 0x200E,
    CMD_HA_OVP_TRAINING_STOP            = 0x200F,
    CMD_HA_MODE_SET                     = 0x2010,
    CMD_HA_MODE_GET                     = 0x2011,
    CMD_HA_SET_CLASSIFY                 = 0x2014,
    CMD_HA_PRESET_IDX_GET               = 0x2015,
    CMD_HA_PRESET_IDX_SET               = 0x2016,

    //for HA MP test
    CMD_MP_HA_BYPASS_ALGORITHM          = 0x2020,
    CMD_MP_HA_SET_GRAPHIC_EQ            = 0x2021,
    CMD_MP_HA_SET_FULL_ON_GAIN          = 0x2022,
    CMD_MP_HA_ENABLE_ALGORITHM          = 0x2023,
#endif

#if F_APP_SS_SUPPORT
    CMD_SS_REQ                          = 0x2200,
#endif

#if F_APP_LEA_REALCAST_SUPPORT
    CMD_LEA_SYNC_BIS                    = 0x3000,
    CMD_LEA_SYNC_CANCEL                 = 0x3001,
    CMD_LEA_SCAN_START                  = 0x3002,
    CMD_LEA_SCAN_STOP                   = 0x3003,
    CMD_LEA_PA_START                    = 0x3004,
    CMD_LEA_PA_STOP                     = 0x3005,
    CMD_LEA_CTRL_VOL                    = 0x3006,
    CMD_LEA_GET_DEVICE_STATE            = 0x3007,
#endif

#if F_APP_CHARGER_CASE_SUPPORT
    CMD_CHARGER_CASE_CONNECTED          = 0x8101,
    CMD_CHARGER_CASE_ENTER_OTA_MODE     = 0x8102,
    CMD_CHARGER_CASE_FIND_CHARGER_CASE  = 0x8103,
    CMD_CHARGER_CASE_LINK_INFO_SET      = 0x8104,
    CMD_CHARGER_CASE_PEER_ADDR_SET      = 0x8105,
    CMD_CHARGER_CASE_GET_BT_ADDR        = 0x8106,
    CMD_CHARGER_CASE_INFO               = 0x8107,
#endif

#if F_APP_FIND_EAR_BUD_SUPPORT
    CMD_FIND_EAR_BUD                    = 0x8110,
#endif
} T_CMD_ID;

/** @brief  packet type for legacy transfer*/
typedef enum t_pkt_type
{
    PKT_TYPE_SINGLE = 0x00,
    PKT_TYPE_START = 0x01,
    PKT_TYPE_CONT = 0x02,
    PKT_TYPE_END = 0x03
} T_PKT_TYPE;

typedef enum
{
    BUD_TYPE_SINGLE      = 0x00,
    BUD_TYPE_RWS         = 0x01,
} T_BUD_TYPE;

typedef enum
{
    APP_REMOTE_MSG_CMD_GET_FW_VERSION           = 0x00,
    APP_REMOTE_MSG_CMD_REPORT_FW_VERSION        = 0x01,
    APP_REMOTE_MSG_CMD_GET_OTA_FW_VERSION       = 0x02,
    APP_REMOTE_MSG_CMD_REPORT_OTA_FW_VERSION    = 0x03,
    APP_REMOTE_MSG_DSP_DEBUG_SIGNAL_IN_SYNC     = 0x04,    // only support BBPro2

    APP_REMOTE_MSG_SYNC_EQ_CTRL_BY_SRC          = 0x05,
    APP_REMOTE_MSG_SYNC_RAW_PAYLOAD             = 0x06,
    APP_REMOTE_MSG_CMD_GET_UI_OTA_VERSION       = 0x07,
    APP_REMOTE_MSG_CMD_REPORT_UI_OTA_VERSION    = 0x08,
    APP_REMOTE_MSG_CMD_SYNC_MAX_PAYLOAD_LEN     = 0x09,
    APP_REMOTE_MSG_CMD_SYNC_FIND_EAR_BUD        = 0x0A,
    APP_REMOTE_MSG_CMD_GET_RAW_PAYLOAD          = 0x0B,
    APP_REMOTE_MSG_CMD_RELAY_RAW_PAYLOAD        = 0x0C,
    APP_REMOTE_MSG_CMD_TOTAL                    = 0x0D,
} T_CMD_REMOTE_MSG;

//for CMD_SET_CFG
typedef enum
{
    CFG_SET_LE_NAME                  = 0x00,
    CFG_SET_LEGACY_NAME              = 0x01,
    CFG_SET_AUDIO_LATENCY            = 0x02,
    CFG_SET_SUPPORT_CODEC            = 0x03,
    CFG_SET_SERIAL_ID                = 0x04,
    CFG_SET_SERIAL_SINGLE_ID         = 0x05,
    CFG_SET_HFP_REPORT_BATT          = 0x06,
    CFG_SET_DISABLE_REPORT_AVP_ID    = 0x07,
} T_CMD_SET_CFG_TYPE;

//for CMD_GET_CFG_SETTING
typedef enum
{
    CFG_GET_LE_NAME                  = 0x00,
    CFG_GET_LEGACY_NAME              = 0x01,
    CFG_GET_IC_NAME                  = 0x02,
    CFG_GET_COMPANY_ID_AND_MODEL_ID  = 0x03,
    CFG_GET_AVP_ID                   = 0x04,
    CFG_GET_AVP_LEFT_SINGLE_ID       = 0x05,
    CFG_GET_AVP_RIGHT_SINGLE_ID      = 0x06,
    CFG_GET_MAX                      = 0x07,
} T_CMD_GET_CFG_TYPE;

/**  @brief CMD Set Info Request type. */
typedef enum
{
    CMD_SET_INFO_TYPE_VERSION = 0x00,
    CMD_INFO_GET_CAPABILITY   = 0x01,
} T_CMD_SET_INFO_TYPE;

typedef enum
{
    GET_STATUS_RWS_STATE                  = 0x00,
    GET_STATUS_RWS_CHANNEL                = 0x01,
    GET_STATUS_BATTERY_STATUS             = 0x02,
    GET_STATUS_APT_STATUS                 = 0x03,
    GET_STATUS_APP_STATE                  = 0x04,
    GET_STATUS_BUD_ROLE                   = 0x05,
    GET_STATUS_APT_NR_STATUS              = 0x06,
    GET_STATUS_APT_VOL                    = 0x07,
    GET_STATUS_LOCK_BUTTON                = 0x08,
    GET_STATUS_FIND_ME                    = 0x09,
    GET_STATUS_ANC_STATUS                 = 0x0A,
    GET_STATUS_LLAPT_STATUS               = 0x0B,
    GET_STATUS_RWS_DEFAULT_CHANNEL        = 0x0C,
    GET_STATUS_RWS_BUD_SIDE               = 0x0D,
    GET_STATUS_RWS_SYNC_APT_VOL           = 0x0E,
    GET_STATUS_FACTORY_RESET_STATUS       = 0x0F,
    GET_STATUS_AUTO_REJECT_CONN_REQ_STATUS = 0x10,
    GET_STATUS_RADIO_MODE                 = 0x11,
    GET_STATUS_SCO_STATUS                 = 0x12,
    GET_STATUS_MIC_MUTE_STATUS            = 0x13,
    CFG_GET_SPATIAL_AUDIO_MODE            = 0x14,
    GET_STATUS_AVRCP_PLAY_STATUS          = 0x15,

    /* for LG (BBLite D-cut) */
    GET_STATUS_BUD_ROLE_FOR_LG            = 0xA0,
    GET_STATUS_VOLUME                     = 0xA1,
    GET_STATUS_MERIDIAN_SOUND_EFFECT_MODE = 0xA2,
    GET_STATUS_LIGHT_SENSOR               = 0xA3,

#if F_APP_DURIAN_SUPPORT
    GET_STATUS_AVP_RWS_VER                = 0xA4,
    GET_STATUS_AVP_MULTILINK_ON_OFF       = 0xA5,
    GET_STATUS_AVP_PROFILE_CONNECT        = 0xA6,
    GET_STATUS_AVP_CONTROL_SET            = 0xA7,
    GET_STATUS_AVP_CLICK_SET              = 0xA8,
    GET_STATUS_AVP_ANC_APT_CYCLE          = 0xA9,
    GET_STATUS_AVP_BUD_LOCAL              = 0xAA,
    GET_STATUS_AVP_ANC_SETTINGS           = 0xAB,
#endif
} T_CMD_GET_STATUS_TYPE;

#if F_APP_CHARGER_CASE_SUPPORT
typedef enum
{
    CHARGER_CASE_GET_MUTE_STATUS            = 0x01,
    CHARGER_CASE_GET_BUD_LOCATION_STATUS    = 0x02,
    CHARGER_CASE_GET_BUD_BATTERY_STATUS     = 0x03,
    CHARGER_CASE_GET_BUD_INFO               = 0x04,
    CHARGER_CASE_GET_CONNECT_STATUS         = 0x05,
    CHARGER_CASE_GET_IN_CASE_STATUS         = 0x06,
} T_CHARGER_CASE_GET_STATUS_TYPE;
#endif
typedef struct
{
    // Byte 0
    uint8_t snk_support_get_set_le_name : 1;
    uint8_t snk_support_get_set_br_name : 1;
    uint8_t snk_support_get_set_vp_language : 1;
    uint8_t snk_support_get_battery_info : 1;
    uint8_t snk_support_ota : 1;
    uint8_t snk_support_change_channel : 1;
    uint8_t snk_support_rsv1 : 2;

    // Byte 1
    uint8_t snk_support_tts : 1;
    uint8_t snk_support_get_set_rws_state : 1;
    uint8_t snk_support_get_set_apt_state : 1; //bit 10
    uint8_t snk_support_get_set_eq_state : 1;
    uint8_t snk_support_get_set_vad_state : 1;
    uint8_t snk_support_get_set_anc_state : 1;
    uint8_t snk_support_get_set_llapt_state : 1; //bit 14
    uint8_t snk_support_get_set_listening_mode_cycle : 1;

    // Byte 2
    uint8_t snk_support_llapt_brightness : 1;
    uint8_t snk_support_anc_eq : 1;
    uint8_t snk_support_apt_eq : 1;
    uint8_t snk_support_notification_vol_adjustment : 1;
    uint8_t snk_support_apt_eq_adjust_separate : 1; //bit20
    uint8_t snk_support_multilink_support : 1;
    uint8_t snk_support_avp : 1; //bit22
    uint8_t snk_support_nr_on_off : 1;

    // Byte 3
    uint8_t snk_support_llapt_scenario_choose : 1;
    uint8_t snk_support_ear_detection : 1;
    uint8_t snk_support_power_on_delay_apply_apt_on : 1;
    uint8_t snk_support_phone_set_anc_eq : 1; //bit 27
    uint8_t snk_support_new_report_bud_status_flow : 1;
    uint8_t snk_support_new_report_listening_status : 1;
    uint8_t snk_support_apt_volume_force_adjust_sync : 1; //bit 30
    uint8_t snk_support_reset_key_remap : 1;

    // Byte 4
    uint8_t snk_support_ansc : 1;
    uint8_t snk_support_vibrator : 1;
    uint8_t snk_support_change_mfb_func : 1;
    uint8_t snk_support_gaming_mode : 1;
    uint8_t snk_support_gaming_mode_eq : 1;
    uint8_t snk_support_key_remap : 1;
    uint8_t snk_support_HA: 1;
    uint8_t snk_support_local_playback : 1;  //bit39

    // Byte 5
    uint8_t snk_support_rsv5_1 : 2;
    uint8_t snk_support_anc_scenario_choose : 1;
    uint8_t snk_support_rws_key_remap : 1;
    uint8_t snk_support_user_eq : 1;
    uint8_t snk_support_reset_key_map_by_bud : 1;
    uint8_t snk_support_get_set_serial_id : 1;
    uint8_t snk_support_rsv5_3 : 1;

    //byte 6
    uint8_t snk_support_data_capture : 1;
    uint8_t snk_support_anc_apt_coexist : 1; //bit 49
    uint8_t snk_support_spatial_audio : 1;
    uint8_t snk_support_ui_ota_version : 1;
    uint8_t snk_support_anc_apt_scenario_separate : 1; //bit 52
    uint8_t snk_support_3bin_scenario: 1; //bit 53
    uint8_t snk_support_rsv6_2 : 2;

    // Byte 7
    uint8_t snk_support_rsv7_1 : 1;
    uint8_t snk_support_voice_eq : 1; //bit 57
    uint8_t snk_support_anc_llapt_apply_burn : 1; //bit 58
    uint8_t snk_support_spk_eq_independent_cfg : 1;//bit 59
    uint8_t snk_support_spk_eq_compensation_cfg : 1;//bit 60
    uint8_t snk_support_log_status_control : 1;//bit 61
    uint8_t snk_not_support_normal_apt_volume_adjust : 1;//bit 62
    uint8_t snk_not_support_llapt_volume_adjust : 1;//bit 63

    //Byte 8
    uint8_t snk_support_rsv8_1 : 1;
    uint8_t snk_support_listening_mode_custom_cycle : 1;//bit65
    uint8_t snk_support_ullrha : 1;//bit66
    uint8_t snk_support_rsv8_2 : 1;//bit67
    uint8_t snk_support_charger_case : 1;//bit68
    uint8_t snk_support_rsv8_3 : 3;
} T_SNK_CAPABILITY;

/**  @brief  cmd set status to phone
  */
typedef enum
{
    CMD_SET_STATUS_COMPLETE = 0x00,
    CMD_SET_STATUS_DISALLOW = 0x01,
    CMD_SET_STATUS_UNKNOW_CMD = 0x02,
    CMD_SET_STATUS_PARAMETER_ERROR = 0x03,
    CMD_SET_STATUS_BUSY = 0x04,
    CMD_SET_STATUS_PROCESS_FAIL = 0x05,
    CMD_SET_STATUS_ONE_WIRE_EXTEND = 0x06,
    CMD_SET_STATUS_TTS_REQ = 0x07,
    CMD_SET_STATUS_MUSIC_REQ = 0x08,
    CMD_SET_STATUS_VERSION_INCOMPATIBLE = 0x09,
    CMD_SET_STATUS_SCENARIO_ERROR = 0x0A,

    CMD_SET_STATUS_GATT = 0x11,
    CMD_SET_STATUS_GATT_ERROR = 0x12,
} T_CMD_SET_STATUS;

typedef struct
{
    uint32_t flash_data_start_addr_tmp;
    uint32_t flash_data_start_addr;
    uint32_t flash_data_size;
    uint8_t flash_data_type;
} T_FLASH_DATA;

typedef union
{
    uint8_t d8[16];
    struct
    {
        uint32_t ver_major: 4;      //!< major version
        uint32_t ver_minor: 8;      //!< minor version
        uint32_t ver_revision: 15;  //!< revision version
        uint32_t ver_reserved: 5;   //!< reserved
        uint32_t ver_commitid;      //!< git commit id
        uint8_t customer_name[8];   //!< customer name
    };
} T_PATCH_IMG_VER_FORMAT;

typedef union
{
    uint32_t version;
    struct
    {
        uint32_t ver_major: 8;      //!< major version
        uint32_t ver_minor: 8;      //!< minor version
        uint32_t ver_revision: 8;   //!< revision version
        uint32_t ver_reserved: 8;   //!< reserved
    };
} T_APP_UI_IMG_VER_FORMAT;

typedef union
{
    uint8_t version[4];
    struct
    {
        uint8_t cmd_set_ver_major;
        uint8_t cmd_set_ver_minor;
        uint8_t eq_spec_ver_major;
        uint8_t eq_spec_ver_minor;
    };
} T_SRC_SUPPORT_VER_FORMAT;

typedef enum
{
    LE_RSSI,
    LEGACY_RSSI
} T_RSSI_TYPE;

typedef enum
{
    REG_ACCESS_READ,
    REG_ACCESS_WRITE,
} T_CMD_REG_ACCESS_ACTION;

typedef enum
{
    REG_ACCESS_TYPE_AON,
    REG_ACCESS_TYPE_AON2B,
    REG_ACCESS_TYPE_DIRECT,
} T_CMD_REG_ACCESS_TYPE;

typedef enum
{
    DSP_TOOL_OPCODE_BRIGHTNESS = 0x0000,
    DSP_TOOL_OPCODE_HW_EQ      = 0x0001,
    DSP_TOOL_OPCODE_GAIN       = 0x0002,
    DSP_TOOL_OPCODE_SW_EQ      = 0x0003,
} T_CMD_DSP_TOOL_OPCODE;

typedef void (*P_APP_CMD_PARSE_CBACK)(uint8_t msg_type, uint8_t *buf, uint16_t len);
typedef void (*P_APP_CMD_RETRIEVE_CBACK)(uint8_t msg_type, uint8_t *buf);
typedef void (*P_APP_CMD_RETRIEVE_SIZE_CBACK)(uint8_t msg_type, uint16_t *len);

typedef enum
{
    APP_CMD_MODULE_TYPE_NONE              = 0x00,
    APP_CMD_MODULE_TYPE_AUDIO_POLICY      = 0x01,
    APP_CMD_MODULE_TYPE_HA                = 0x02,
} T_APP_CMD_MODULE_TYPE;

typedef struct t_app_cmd_parse_cback_item
{
    struct t_app_cmd_parse_cback_item   *p_next;
    P_APP_CMD_PARSE_CBACK               parse_cback;
    uint16_t                            module_total_len;
    T_APP_CMD_MODULE_TYPE               module_type;
    uint8_t                             msg_type_max;
} T_APP_CMD_PARSE_CBACK_ITEM;

typedef struct t_app_cmd_retrieve_cback_item
{
    struct t_app_cmd_retrieve_cback_item    *p_next;
    P_APP_CMD_RETRIEVE_CBACK                retrieve_cback;
    P_APP_CMD_RETRIEVE_SIZE_CBACK           retrieve_size_cback;
    uint16_t                                module_total_len;
    T_APP_CMD_MODULE_TYPE                   module_type;
    uint8_t                                 msg_type_max;
} T_APP_CMD_RETRIEVE_CBACK_ITEM;

/** End of APP_CMD_Exported_Types
    * @}
    */
/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup APP_CMD_Exported_Functions App Cmd Functions
    * @{
    */
/**
    * @brief  App process uart or embedded spp vendor command.
    * @param  cmd_ptr command type
    * @param  cmd_len command length
    * @param  cmd_path command path is used to distinguish from uart, le, spp or iap channel.
    * @param  rx_seqn recieved command sequence
    * @param  app_idx received rx command device index
    * @return void
    */
void app_cmd_handler(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path, uint8_t rx_seqn,
                     uint8_t app_idx);

#if (F_APP_OTA_TOOLING_SUPPORT == 1)
/**
    * @brief  Execute OTA tooling parking process
    * @param  void
    * @return void
    */
void app_cmd_ota_tooling_parking(void);

/**
    * @brief  Stop OTA tooling parking related timer
    * @param  void
    * @return void
    */
void app_cmd_stop_ota_parking_power_off(void);
#endif

#if F_APP_DEVICE_CMD_SUPPORT
/**
    * @brief  Get auto reject connection request flag.
    * @param  void
    * @return bool, true if auto reject is enabled
    */
bool app_cmd_get_auto_reject_conn_req_flag(void);

/**
    * @brief  Get auto accept connection request flag.
    * @param  void
    * @return bool, true if auto accept is enabled
    */
bool app_cmd_get_auto_accept_conn_req_flag(void);

/**
    * @brief  Get report ATTR info flag.
    * @param  void
    * @return bool, true if report ATTR info is enabled
    */
bool app_cmd_get_report_attr_info_flag(void);

/**
    * @brief  Set report attr info flag.
    * @param  bool
    * @return void
    */
void app_cmd_set_report_attr_info_flag(bool flag);
#endif

/* @brief  app cmd init
*
* @param  void
* @return none
*/
void app_cmd_init(void);

/**
    * @brief  Send command relay msg to buds
    * @param  cmd_id command id
    * @param  cmd_ptr command type
    * @param  cmd_len command length
    * @param  module_type module type of relay msg
    * @param  relay_cmd_id command id of relay msg
    * @param  sync true if relay msg needs to sync
    * @return bool, true if command is relayed successfully
    */
bool app_cmd_relay_command_set(uint16_t cmd_id, uint8_t *cmd_ptr, uint16_t cmd_len,
                               T_APP_MODULE_TYPE module_type, uint8_t relay_cmd_id, bool sync);
/**
    * @brief  Send event relay msg to buds
    * @param  event_id event id
    * @param  event_ptr event type
    * @param  event_len event length
    * @param  module_type module type of relay msg
    * @param  relay_event_id event id of relay msg
    * @param  sync true if relay msg needs to sync
    * @return bool, true if event is relayed successfully
    */
bool app_cmd_relay_event(uint16_t event_id, uint8_t *event_ptr, uint16_t event_len,
                         T_APP_MODULE_TYPE module_type, uint8_t relay_event_id);
/**
    * @brief  Get tool connect status
    * @param  void
    * @return bool, true if tool is connected
    */
bool app_cmd_get_tool_connect_status(void);

/**
    * @brief  Check if the cmd set version of source device is capatible with the version of earphone.
    * @param  value new EQ control status
    * @param  need_relay if relay to another bud is needed
    * @return void
    */
void app_cmd_update_eq_ctrl(uint8_t value, bool need_relay);

/**
    * @brief  Get the cmd set and EQ spec version of source device.
    * @param  cmd_path command path is used to distinguish from uart, le, spp or iap channel.
    * @param  app_idx received rx command device index
    * @return T_SRC_SUPPORT_VER_FORMAT, support version of source
    */
T_SRC_SUPPORT_VER_FORMAT *app_cmd_get_src_version(uint8_t cmd_path, uint8_t app_idx);

/**
    * @brief  Check if the cmd set version of source device is capatible with the version of earphone.
    * @param  cmd_path command path is used to distinguish from uart, le, spp or iap channel.
    * @param  app_idx received rx command device index
    * @return bool, true if the result is valid
    */
bool app_cmd_check_src_cmd_version(uint8_t cmd_path, uint8_t app_idx);

/**
    * @brief  Check if the EQ spec version of source device is capatible with the version of earphone.
    * @param  cmd_path command path is used to distinguish from uart, le, spp or iap channel.
    * @param  app_idx received rx command device index
    * @return bool, true if the result is valid
    */
bool app_cmd_check_src_eq_spec_version(uint8_t cmd_path, uint8_t app_idx);
void app_cmd_handle_mp_cmd_hci_evt(void *p_gap_vnd_cmd_cb_data);
bool app_cmd_cback_register(P_APP_CMD_PARSE_CBACK parse_cb, T_APP_CMD_MODULE_TYPE module_type);
/**
    * @brief  Get the maximum payload length of the transmission.
    * @param  void
    * @return maximum payload length
    */
uint16_t app_cmd_get_max_payload_len(void);

/**
    * @brief  Get system capability.
    * @param  void
    * @return T_SNK_CAPABILITY, the payload that contains bitmask bits
    */
T_SNK_CAPABILITY app_cmd_get_system_capability(void);

#if F_APP_CHARGER_CASE_SUPPORT
/**
    * @brief  disable charger case report when ble disconnect.
    * @param  link_id disconnected BLE device's link id.
    * @return void
    */
void app_cmd_charger_case_handle_ble_disconn(uint8_t link_id);

/**
    * @brief  record avrcp volume for charger case report.
    * @param  vol avrcp volume.
    * @return void
    */
void app_cmd_charger_case_record_level(uint8_t vol, uint8_t *addr);

/**
    * @brief  get charger case vol level.
    * @param  void.
    * @return level
    */
uint8_t app_cmd_charger_case_get_level(void);
#endif

/**
    * @brief  check slave latency update.
    * @param  conn_id   le device's conn id
    * @param  skip_rtk  skip rtk link check
    * @return void
    */
void app_cmd_check_slave_latency_update(uint8_t conn_id, bool skip_rtk);

bool app_cmd_retrieve_cback_register(P_APP_CMD_RETRIEVE_CBACK retrieve_cb,
                                     P_APP_CMD_RETRIEVE_SIZE_CBACK retrieve_size_cb,
                                     T_APP_CMD_MODULE_TYPE module_type);
/** @} */ /* End of group APP_CMD_Exported_Functions */
/** End of APP_CMD
* @}
*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_CMD_H_ */
