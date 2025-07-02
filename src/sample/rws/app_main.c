
/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdint.h>
#include "os_mem.h"
#include "os_sched.h"
#include "os_msg.h"
#include "os_task.h"
#include "os_ext.h"
#include "system_status_api.h"
#include "rtl876x_pinmux.h"
#include "patch_header_check.h"
#include "img_ctrl_ext.h"
#include "hal_gpio_int.h"
#include "hal_gpio.h"
#include "hal_i2c.h"
#include "dlps_util.h"
#include "trace.h"
#include "audio.h"
#include "audio_probe.h"
#include "sysm.h"
#include "pm.h"
#include "gap_br.h"
#include "gap.h"
#include "test_mode.h"
#include "single_tone.h"
#include "engage.h"
#include "fmc_api.h"
#include "spp_stream.h"
#include "ble_stream.h"
#include "iap_stream.h"
#include "app_console.h"
#include "app_roleswap.h"
#include "app_roleswap_control.h"
#include "app_cfg.h"
#include "app_ipc.h"
#include "app_main.h"
#include "app_gap.h"
#include "app_led.h"
#include "app_io_msg.h"
#include "app_ble_gap.h"
#include "app_ble_client.h"
#include "app_ble_service.h"
#include "app_key_process.h"
#include "app_dlps.h"
#include "app_key_gpio.h"
#include "app_bt_policy_api.h"
#include "app_sdp.h"
#include "app_transfer.h"
#include "app_timer.h"
#include "app_audio_policy.h"
#include "app_a2dp.h"
#include "app_hfp.h"
#include "app_avrcp.h"
#include "app_iap.h"
#include "app_spp.h"
#include "app_cmd.h"
#include "app_bud_loc.h"
#include "app_ble_bond.h"
#include "app_multilink.h"
#include "app_ble_device.h"
#include "app_sensor.h"
#include "app_sensor_i2c.h"
#include "app_in_out_box.h"
#include "app_mmi.h"
#include "app_ota.h"
#include "app_tts.h"
#include "app_third_party_srv.h"
#include "app_bond.h"
#include "app_ota_tooling.h"
#include "app_bond.h"
#include "app_adp.h"
#include "app_adp_cmd.h"
#include "app_io_output.h"
#include "app_amp.h"
#include "app_line_in.h"
#include "app_ota_tooling.h"
#include "app_dongle_spp.h"
#include "app_vendor.h"

#if F_APP_ADC_SUPPORT || (F_APP_EXT_MIC_SWITCH_SUPPORT && F_APP_EXT_MIC_PLUG_IN_ADC_DETECT)
#include "app_adc.h"
#endif

#if F_APP_GAMING_CONTROLLER_SUPPORT
#include "app_dongle_controller.h"
#endif

#if (F_APP_PERIODIC_WAKEUP == 1)
#include "rtl876x_rtc.h"
#endif

#if F_APP_ERWS_SUPPORT
#include "app_relay.h"
#include "app_roleswap.h"
#include "app_rdtp.h"
#endif

#if F_APP_BT_PROFILE_PBAP_SUPPORT
#include "app_pbap.h"
#endif

#if F_APP_HFP_AG_SUPPORT
#include "app_hfp_ag.h"
#endif

#if F_APP_GPIO_ONOFF_SUPPORT
#include "app_gpio_on_off.h"
#endif

#if F_APP_TEST_SUPPORT
#include "app_test.h"
#endif

#if F_APP_VAD_SUPPORT
#include "app_vad.h"
#endif

#if F_APP_SENSOR_PX318J_SUPPORT
#include "app_sensor_px318j.h"
#endif

#if F_APP_LINEIN_SUPPORT
#include "app_line_in.h"
#endif

#if F_APP_SENSOR_JSA1225_SUPPORT
#include "app_sensor_jsa.h"
#endif

#if F_APP_SENSOR_IQS773_873_SUPPORT
#include "app_vendor_iqs773_873.h"
#endif

#if F_APP_ALC1017_SUPPORT
#include "app_alc1017.h"
#endif

#if F_APP_LISTENING_MODE_SUPPORT
#include "app_listening_mode.h"
#endif

#if F_APP_GPIO_ONOFF_SUPPORT
#include "app_io_output.h"
#endif

#if F_APP_HID_SUPPORT
#include "app_hid.h"
#endif

#if F_APP_GATT_OVER_BREDR_SUPPORT
#include "app_att.h"
#endif

#if F_APP_APT_SUPPORT
#include "app_audio_passthrough.h"
#endif

#if F_APP_NFC_SUPPORT
#include "app_nfc.h"
#endif

#if F_APP_ANC_SUPPORT
#include "app_anc.h"
#endif

#if BISTO_FEATURE_SUPPORT
#include "app_bisto_bt.h"
#endif

#if F_APP_DURIAN_SUPPORT
#include "app_durian.h"
#endif

#if F_APP_CFU_SUPPORT
#include "app_cfu.h"
#endif

#if F_APP_LOCAL_PLAYBACK_SUPPORT
#include "app_playback.h"
#include "app_playback_trans.h"
#include "audio_fs.h"
#if F_APP_LOCAL_PLAYBACK_SUPPORT && TARGET_RTL8773CO
#include "app_listen_save.h"
#endif
#endif

#if F_APP_USB_AUDIO_SUPPORT || F_APP_USB_HID_SUPPORT
#include "app_usb.h"
#endif

#if F_APP_MALLEUS_SUPPORT
#include "app_malleus.h"
#endif

#if F_APP_SENSOR_PX318J_SUPPORT
#include "app_sensor_px318j.h"
#endif

#if F_APP_SENSOR_JSA1225_SUPPORT || F_APP_SENSOR_JSA1227_SUPPORT
#include "app_sensor_jsa.h"
#endif

#if F_APP_SENSOR_IQS773_873_SUPPORT
#include "app_vendor_iqs773_873.h"
#include "app_sensor_iqs773_873.h"
#endif

#if F_APP_AIRPLANE_SUPPORT
#include "app_airplane.h"
#endif

#if F_APP_HEARABLE_SUPPORT
#include "app_hearable.h"
#endif

#if F_APP_SLIDE_SWITCH_SUPPORT
#include "app_slide_switch.h"
#endif

#if F_APP_RTK_FAST_PAIR_ADV_FEATURE_SUPPORT
#include "app_rtk_fast_pair_adv.h"
#endif

#if F_APP_CFU_FEATURE_SUPPORT
#include "app_common_cfu.h"
#endif

#if F_APP_TEAMS_FEATURE_SUPPORT
#include "app_teams_extend_led.h"
#include "app_teams_cmd.h"
#endif

#if F_APP_LEA_SUPPORT
#include "app_lea_profile.h"
#endif

#if F_APP_CAP_TOUCH_SUPPORT
#include "rtl876x_captouch.h"
#include "app_cap_touch.h"
#endif

#if F_APP_BLE_SWIFT_PAIR_SUPPORT
#include "app_ble_swift_pair.h"
#endif

#if F_APP_BRIGHTNESS_SUPPORT
#include "app_audio_passthrough_brightness.h"
#endif

#if F_APP_QDECODE_SUPPORT
#include "app_qdec.h"
#endif

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
#include "app_single_multilink_customer.h"
#elif F_APP_MUTLILINK_SOURCE_PRIORITY_UI
#include "app_multilink_customer.h"
#endif

#if F_APP_RWS_MULTI_SPK_SUPPORT
#include "msbc_sync.h"
#endif

#if F_APP_IAP_RTK_SUPPORT
#include "app_iap_rtk.h"
#endif

#if CONFIG_REALTEK_APP_BOND_MGR_SUPPORT
#include "bt_bond_api.h"
#if F_APP_LEA_SUPPORT
#include "ble_audio_bond.h"
#endif
#endif

#if F_APP_BOND_MGR_BLE_SYNC
#include "ble_bond_sync.h"
#endif

#if F_APP_SENSOR_MEMS_SUPPORT
#include "app_sensor_mems.h"
#endif

#if F_APP_HIFI4_SUPPORT
#include "app_buck.h"
#include "app_buck_tps62860.h"
#include "app_buck_apw7564.h"
#include "ipc.h"
#endif

#if F_APP_COMMON_DONGLE_SUPPORT
#include "app_dongle_common.h"
#endif

#if F_APP_GAMING_DONGLE_SUPPORT
#include "app_dongle_audio.h"
#include "app_dongle_dual_mode.h"
#include "app_dongle_data_ctrl.h"

#include "legacy_gaming.h"
#endif

#if F_APP_DATA_CAPTURE_SUPPORT
#include "app_data_capture.h"
#endif

#if F_APP_POWER_TEST
#include "power_debug.h"
#include "hal_debug.h"
#endif

#if F_APP_AUTO_POWER_TEST_LOG
#include "app_power_test.h"
#endif
#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
#include "app_dongle_source_ctrl.h"
#endif

#if F_APP_FINDMY_FEATURE_SUPPORT
#include "app_findmy_task.h"
#endif

#if F_APP_USB_SUSPEND_SUPPORT
#include "app_usb_suspend.h"
#endif

#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
#include "app_sass_policy.h"
#endif

#if F_APP_CHATGPT_SUPPORT
#include "app_chatgpt.h"
#endif

#include "app_bt_point.h"

#if F_APP_EXT_MIC_SWITCH_SUPPORT
#include "app_ext_mic_switch.h"
#endif

#define MAX_NUMBER_OF_GAP_MESSAGE       0x20    //!< indicate BT stack message queue size
#define MAX_NUMBER_OF_IO_MESSAGE        0x20    //!< indicate io queue size
#define MAX_NUMBER_OF_DSP_MSG           0x20    //!< number of dsp message reserved for DSP message handling.
#define MAX_NUMBER_OF_CODEC_MSG         0x10    //!< number of codec message reserved for CODEC message handling.
#define MAX_NUMBER_OF_ANC_MSG           0x10    //!< number of anc message reserved for ANC message handling.
#define MAX_NUMBER_OF_SYS_MSG           0x20    //!< indicate SYS timer queue size
#define MAX_NUMBER_OF_LOADER_MSG        0x10    //!< indicate Bin Loader queue size
#define MAX_NUMBER_OF_APP_TIMER_MODULE  0x30    //!< indicate app timer module size
/** indicate rx event queue size*/
#define MAX_NUMBER_OF_RX_EVENT \
    (MAX_NUMBER_OF_GAP_MESSAGE + MAX_NUMBER_OF_IO_MESSAGE + MAX_NUMBER_OF_APP_TIMER_MODULE + \
     MAX_NUMBER_OF_DSP_MSG + MAX_NUMBER_OF_CODEC_MSG + MAX_NUMBER_OF_ANC_MSG + \
     MAX_NUMBER_OF_SYS_MSG + MAX_NUMBER_OF_LOADER_MSG)

#define DEFAULT_PAGESCAN_WINDOW         0x12
#define DEFAULT_PAGESCAN_INTERVAL       0x800
#define DEFAULT_PAGE_TIMEOUT            0x2000
#define DEFAULT_SUPVISIONTIMEOUT        0x1f40
#define DEFAULT_INQUIRYSCAN_WINDOW      0x12
#define DEFAULT_INQUIRYSCAN_INTERVAL    0x1000

typedef union t_release_ver
{
    uint64_t  release_ver;
    struct __attribute__((packed))
    {
        uint8_t revision;
        uint16_t tool_ver;
        uint8_t chip_ver;
        uint16_t crb_ver;
        uint8_t rsv[2];
    };
} T_RELEASE_VER;

void *audio_evt_queue_handle;
void *audio_io_queue_handle;

T_APP_DB app_db;

/**
* @brief board_init() contains the initialization of pinmux settings and pad settings.
*
*   All the pinmux settings and pad settings shall be initiated in this function.
*   But if legacy driver is used, the initialization of pinmux setting and pad setting
*   should be peformed with the IO initializing.
*
* @return void
*/
static void board_init(void)
{
    PAD_Pull_Mode pull_mode;

#if (F_APP_PERIODIC_WAKEUP == 1)
    app_dlps_system_wakeup_clear_rtc_int();
#endif

    if (app_cfg_const.enable_data_uart)
    {
        //when select 4pogo download and uart tx shares the same pin with 3pin gpio, don't config uart tx pinmux in here
        if ((app_cfg_const.enable_4pogo_pin == 0) ||
            (app_cfg_const.gpio_box_detect_pinmux != app_cfg_const.data_uart_tx_pinmux))
        {
            Pinmux_Config(app_cfg_const.data_uart_tx_pinmux, UART0_TX);
            Pad_Config(app_cfg_const.data_uart_tx_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
        }

        Pinmux_Config(app_cfg_const.data_uart_rx_pinmux, UART0_RX);
        Pad_Config(app_cfg_const.data_uart_rx_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        if (app_cfg_const.enable_rx_wake_up)
        {
            Pinmux_Config(app_cfg_const.rx_wake_up_pinmux, DWGPIO);
            Pad_Config(app_cfg_const.rx_wake_up_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
        }

        if (app_cfg_const.enable_tx_wake_up)
        {
            Pinmux_Config(app_cfg_const.tx_wake_up_pinmux, DWGPIO);
            Pad_Config(app_cfg_const.tx_wake_up_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
        }
    }

    if (app_cfg_const.dsp_log_output_select == DSP_OUTPUT_LOG_BY_UART)
    {
        Pinmux_Config(app_cfg_const.dsp_log_pin, UART2_TX);
        Pad_Config(app_cfg_const.dsp_log_pin,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_HIGH);
    }

    //Config I2C0 pinmux. For CP control
    if ((app_cfg_const.supported_profile_mask & IAP_PROFILE_MASK) ||
        (app_cfg_const.gsensor_support) ||
#if F_APP_SENSOR_MEMS_SUPPORT
        (app_cfg_const.mems_support) ||
#endif
        (app_cfg_const.psensor_support && (app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IQS773)) ||
        (app_cfg_const.psensor_support && (app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IQS873)) ||
        (app_cfg_const.sensor_support && (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_PX)) ||
        (app_cfg_const.sensor_support && (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1225)) ||
        (app_cfg_const.sensor_support && (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1227)) ||
        (app_cfg_const.sensor_support && (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_SC7A20)) ||
        (app_cfg_const.ext_buck_support))
    {
        hal_i2c_init_pin(HAL_I2C_ID_0, app_cfg_const.i2c_0_clk_pinmux, app_cfg_const.i2c_0_dat_pinmux);

        Pinmux_Config(app_cfg_const.i2c_0_dat_pinmux, I2C0_DAT);
        Pinmux_Config(app_cfg_const.i2c_0_clk_pinmux, I2C0_CLK);

        if (app_cfg_const.chip_supply_power_to_light_sensor_digital_VDD)
        {
            Pad_Config(app_cfg_const.i2c_0_clk_pinmux,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
            Pad_Config(app_cfg_const.i2c_0_dat_pinmux,
                       PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
        }
        else
        {
#if (CONFIG_SOC_SERIES_RTL8773D || TARGET_RTL8773DFL )
            Pad_Config(app_cfg_const.i2c_0_dat_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
            Pad_Config(app_cfg_const.i2c_0_clk_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
#else
            Pad_Config(app_cfg_const.i2c_0_dat_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
            Pad_Config(app_cfg_const.i2c_0_clk_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
#endif
            Pad_PullConfigValue(app_cfg_const.i2c_0_dat_pinmux, PAD_STRONG_PULL);
            Pad_PullConfigValue(app_cfg_const.i2c_0_clk_pinmux, PAD_STRONG_PULL);
        }
    }

#if F_APP_SENSOR_MEMS_SUPPORT
    if (app_cfg_const.mems_support)
    {
        app_sensor_mems_cfg_pad();
    }
#endif

    //Config sensor pinmux
    if (app_cfg_const.sensor_support)
    {
        if (0)
        {
            /* for feature define; do nothing */
        }
#if F_APP_SENSOR_HX3001_SUPPORT
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_HX)
        {
            Pad_SetPinDrivingCurrent(app_cfg_const.sensor_detect_pinmux, LEVEL3);
        }
#endif
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1225 ||
                 app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1227)
        {
            Pinmux_Config(app_cfg_const.sensor_detect_pinmux, DWGPIO);
            Pad_Config(app_cfg_const.sensor_detect_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
        }
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_IO)
        {
            pull_mode = (app_cfg_const.iosensor_active) ? PAD_PULL_DOWN : PAD_PULL_UP;
            Pinmux_Config(app_cfg_const.sensor_detect_pinmux, DWGPIO);
            Pad_Config(app_cfg_const.sensor_detect_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, pull_mode, PAD_OUT_DISABLE, PAD_OUT_LOW);
        }
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_PX)
        {
            Pinmux_Config(app_cfg_const.sensor_detect_pinmux, DWGPIO);
            Pad_Config(app_cfg_const.sensor_detect_pinmux,
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
        }
    }

    //Config gsensor pinmux
    if (app_cfg_const.gsensor_support || app_cfg_const.psensor_support)
    {
        //INT pad config
        Pinmux_Config(app_cfg_const.gsensor_int_pinmux, DWGPIO);
        Pad_Config(app_cfg_const.gsensor_int_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
    }

    //Config output 10hz pinmux
    if (app_cfg_const.enable_external_mcu_reset)
    {
        Pad_Config(app_cfg_const.external_mcu_input_pinmux,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }

    //Config amp pinmux
    if (app_cfg_const.enable_ctrl_ext_amp)
    {
        pull_mode = (app_cfg_const.enable_ext_amp_high_active) ? PAD_PULL_DOWN : PAD_PULL_UP;
        Pinmux_Config(app_cfg_const.ext_amp_pinmux, DWGPIO);
        Pad_Config(app_cfg_const.ext_amp_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, pull_mode, PAD_OUT_DISABLE, PAD_OUT_LOW);
    }

    //Config thermistor power pinmux
    if (app_cfg_const.thermistor_power_gpio_support)
    {
        Pad_Config(app_cfg_const.thermistor_power_pinmux,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);
    }

    //Config pcba shipping mode pinmux
    Pad_Config(app_cfg_const.pcba_shipping_mode_pinmux,
               PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_ENABLE, PAD_OUT_LOW);

#if F_APP_IO_OUTPUT_SUPPORT
    //Config out indication pinmux
    app_io_output_out_indication_pinmux_init();
#endif

#if F_APP_EXT_MIC_SWITCH_SUPPORT && (F_APP_EXT_MIC_PLUG_IN_GPIO_DETECT || F_APP_EXT_MIC_SWITCH_IC_SUPPORT)
    app_ext_mic_gpio_board_init();
#endif

#if F_APP_QDECODE_SUPPORT
    if (app_cfg_const.wheel_support)
    {
        app_qdec_pad_config();
    }
#endif

#if F_APP_GPIO_MICBIAS_SUPPORT
    //For no micbias pin IC , do micbias control gpio pad init in app
    if (app_cfg_const.micbias_pinmux != MICBIAS)
    {
        Pad_Config(app_cfg_const.micbias_pinmux,
                   PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_DOWN, PAD_OUT_DISABLE, PAD_OUT_LOW);
    }
#endif

    if (app_cfg2_const.dsp_gpio_num)
    {
        for (uint8_t i = 0; i < app_cfg2_const.dsp_gpio_num; i++)
        {
            Pinmux_Config(app_cfg2_const.dsp_gpio_pinmux[i], DSP_GPIO_OUT);
            Pad_Config(app_cfg2_const.dsp_gpio_pinmux[i],
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
        }
    }

#if (CONFIG_SOC_SERIES_RTL8773D || TARGET_RTL8773DFL )
    if (app_cfg2_const.dsp2_gpio_num)
    {
        for (uint8_t i = 0; i < app_cfg2_const.dsp2_gpio_num; i++)
        {
            Pinmux_Config(app_cfg2_const.dsp2_gpio_pinmux[i], DSP2_GPIO_OUT);
            Pad_Config(app_cfg2_const.dsp2_gpio_pinmux[i],
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
        }
    }

    if (app_cfg2_const.adsp_gpio_num)
    {
        for (uint8_t i = 0; i < app_cfg2_const.adsp_gpio_num; i++)
        {
            Pinmux_Config(app_cfg2_const.adsp_gpio_pinmux[i], ANCDSP_GPIO_OUT);
            Pad_Config(app_cfg2_const.adsp_gpio_pinmux[i],
                       PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
        }
    }
#endif

    if (app_cfg2_const.dsp_jtag_enable)
    {
        Pinmux_Config(app_cfg2_const.dsp_jtck_pinmux, DSP_JTCK);
        Pad_Config(app_cfg2_const.dsp_jtck_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        Pinmux_Config(app_cfg2_const.dsp_jtdi_pinmux, DSP_JTDI);
        Pad_Config(app_cfg2_const.dsp_jtdi_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        Pinmux_Config(app_cfg2_const.dsp_jtdo_pinmux, DSP_JTDO);
        Pad_Config(app_cfg2_const.dsp_jtdo_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        Pinmux_Config(app_cfg2_const.dsp_jtms_pinmux, DSP_JTMS);
        Pad_Config(app_cfg2_const.dsp_jtms_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        Pinmux_Config(app_cfg2_const.dsp_jtrst_pinmux, DSP_JTRST);
        Pad_Config(app_cfg2_const.dsp_jtrst_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    }

#if (CONFIG_SOC_SERIES_RTL8773D || TARGET_RTL8773DFL )
    if (app_cfg2_const.dsp2_jtag_enable)
    {
        Pinmux_Config(app_cfg2_const.dsp_jtck_pinmux, DSP2_JTCK);
        Pad_Config(app_cfg2_const.dsp2_jtck_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        Pinmux_Config(app_cfg2_const.dsp2_jtdi_pinmux, DSP2_JTDI);
        Pad_Config(app_cfg2_const.dsp2_jtdi_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        Pinmux_Config(app_cfg2_const.dsp2_jtdo_pinmux, DSP2_JTDO);
        Pad_Config(app_cfg2_const.dsp2_jtdo_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        Pinmux_Config(app_cfg2_const.dsp2_jtms_pinmux, DSP2_JTMS);
        Pad_Config(app_cfg2_const.dsp2_jtms_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        Pinmux_Config(app_cfg2_const.dsp2_jtrst_pinmux, DSP2_JTRST);
        Pad_Config(app_cfg2_const.dsp2_jtrst_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    }

    if (app_cfg2_const.adsp_jtag_enable)
    {
        Pinmux_Config(app_cfg2_const.adsp_jtck_pinmux, ANCDSP_JTCK);
        Pad_Config(app_cfg2_const.adsp_jtck_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        Pinmux_Config(app_cfg2_const.adsp_jtdi_pinmux, ANCDSP_JTDI);
        Pad_Config(app_cfg2_const.adsp_jtdi_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        Pinmux_Config(app_cfg2_const.adsp_jtdo_pinmux, ANCDSP_JTDO);
        Pad_Config(app_cfg2_const.adsp_jtdo_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        Pinmux_Config(app_cfg2_const.adsp_jtms_pinmux, ANCDSP_JTMS);
        Pad_Config(app_cfg2_const.adsp_jtms_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);

        Pinmux_Config(app_cfg2_const.adsp_jtrst_pinmux, ANCDSP_JTRST);
        Pad_Config(app_cfg2_const.adsp_jtrst_pinmux,
                   PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_LOW);
    }
#endif

#if (CONFIG_SOC_SERIES_RTL8773D || TARGET_RTL8773DFL )
    Pinmux_Config(app_cfg_const.external_pa_pinmux, EN_EXPA);
    Pad_Config(app_cfg_const.external_pa_pinmux, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE,
               PAD_OUT_DISABLE, PAD_OUT_LOW);
#endif
}


#if F_APP_EXT_FLASH_SUPPORT
static bool ext_flash_init(void)
{
    return fmc_flash_nor_init(FMC_SPIC_ID_3);
}
#endif

static void driver_init(void)
{
    hal_gpio_init();
    hal_gpio_int_init();
    hal_gpio_set_debounce_time(30);
    app_adp_init();

    if (app_cfg_const.key_gpio_support)
    {
        key_gpio_initial();
        app_key_init();

        if (app_cfg_const.enable_hall_switch_function)
        {
            // Check if there are hall switch definitions
            app_key_hybird_parse_hall_switch_setting();

            // Read the hall switch status
            app_db.hall_switch_status = app_key_read_hall_switch_status();
        }
    }

    if (app_cfg_const.mfb_replace_key0)
    {
        key_gpio_mfb_init();
    }

#if F_APP_IAP_SUPPORT
    if (app_cfg_const.supported_profile_mask & IAP_PROFILE_MASK)
    {
        app_iap_cp_hw_init(I2C0);
    }
#endif

#if F_APP_CONSOLE_SUPPORT
    if (app_cfg_const.enable_data_uart || app_cfg_const.one_wire_uart_support)
    {
        app_console_init();
    }
#endif

#if F_APP_GPIO_ONOFF_SUPPORT
    //app_case_detect_gpio_driver_init();
    if (app_cfg_const.box_detect_method == GPIO_DETECT)
    {
        app_gpio_on_off_driver_init(app_cfg_const.gpio_box_detect_pinmux);
    }
#endif
     //app_cfg_const.discharger_support = 1;
    if (app_cfg_const.charger_support || app_cfg_const.discharger_support)
    {
        app_charger_init();
    }

    if (app_cfg_const.led_support)
    {
        app_led_driver_init();

#if CONFIG_REALTEK_APP_TEAMS_FEATURE_SUPPORT
        teams_extend_led_init();
        teams_extend_led_breath_init();
#endif
    }

#if F_APP_USB_AUDIO_SUPPORT
    if (app_cfg_const.usb_audio_support)
    {
        //app_usb_audio_init();
    }
#endif

#if F_APP_NFC_SUPPORT
    if (app_cfg_const.nfc_support)
    {
        app_nfc_init();
    }
#endif

#if F_APP_LINEIN_SUPPORT
    if (app_cfg_const.line_in_support)
    {
        app_line_in_driver_init();
        app_dlps_pad_wake_up_polarity_invert(app_cfg_const.line_in_pinmux);
    }
#endif

#if F_APP_AMP_SUPPORT
    if (app_cfg_const.enable_ctrl_ext_amp)
    {
        app_amp_init();
    }
#endif

#if F_APP_SENSOR_SUPPORT
    if (app_cfg_const.sensor_support)
    {
        if (0)
        {
            /* for feature define; do nothing */
        }
#if F_APP_SENSOR_HX3001_SUPPORT
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_HX)
        {
            app_sensor_ld_init();
        }
#endif
#if F_APP_SENSOR_JSA1225_SUPPORT
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1225)
        {
            if (app_cfg_const.chip_supply_power_to_light_sensor_digital_VDD)
            {
                /* Init JSA1225 after output power supply pin pull high */
            }
            else
            {
                app_sensor_i2c_init(SENSOR_ADDR_JSA, DEF_I2C_CLK_SPD, false);
                app_sensor_jsa1225_init();
                app_sensor_int_gpio_init(app_cfg_const.sensor_detect_pinmux, app_sensor_jsa_int_gpio_intr_cb);
            }
        }
#endif
#if F_APP_SENSOR_JSA1227_SUPPORT
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_JSA1227)
        {
            if (app_cfg_const.chip_supply_power_to_light_sensor_digital_VDD)
            {
                /* Init JSA1227 after output power supply pin pull high */
            }
            else
            {
                app_sensor_i2c_init(SENSOR_ADDR_JSA1227, JSA1227_I2C_CLK_SPD, false);
                app_sensor_jsa1227_init();
                app_sensor_int_gpio_init(app_cfg_const.sensor_detect_pinmux, app_sensor_jsa_int_gpio_intr_cb);
            }
        }
#endif
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_IO)
        {
            app_sensor_ld_io_init();
        }
#if F_APP_SENSOR_PX318J_SUPPORT
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_PX)
        {
            if (app_cfg_const.chip_supply_power_to_light_sensor_digital_VDD)
            {
                /* Init PX318J after output power supply pin pull high */
            }
            else
            {
                app_sensor_i2c_init(PX318J_ID, PX318J_I2C_CLK_SPD, false);
                app_sensor_px318j_init();
                app_sensor_int_gpio_init(app_cfg_const.sensor_detect_pinmux,
                                         app_sensor_px318j_int_gpio_intr_cb);
            }
        }
#endif
#if F_APP_SENSOR_SC7A20_AS_LS_SUPPORT
        else if (app_cfg_const.sensor_vendor == SENSOR_LD_VENDOR_SC7A20)
        {
            app_sensor_i2c_init(GSENSOR_I2C_SLAVE_ADDR_SILAN, DEF_I2C_CLK_SPD, false);
            app_sensor_sc7a20_as_ls_init();
        }
#endif
    }

#if F_APP_SENSOR_SL7A20_SUPPORT
    if (app_cfg_const.gsensor_support)
    {
        //P sensor and G sensor will not coexist
        //Re-use gsensor INT pinmux
        app_sensor_gsensor_init();
    }
#endif

#if F_APP_SENSOR_MEMS_SUPPORT
    if (app_cfg_const.mems_support)
    {
        app_sensor_mems_init();
    }
#endif

    if (app_cfg_const.psensor_support)
    {
        //P sensor and G sensor will not coexist
        //Re-use gsensor INT pinmux

        if (app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IO)
        {
            app_sensor_psensor_init();
        }
#if F_APP_SENSOR_IQS773_873_SUPPORT
        else if (app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IQS773)
        {
            app_sensor_i2c_init(I2C_AZQ_ADDR, DEF_I2C_CLK_SPD, false);
            if (i2c_iqs_check_device() != IQS773_MODULE)
            {
                APP_PRINT_ERROR0("No IQS773 module detected");
            }
            else
            {
                i2c_iqs773_setup();
                app_sensor_int_gpio_init(app_cfg_const.gsensor_int_pinmux, app_psensor_iqs773_873_intr_cb);
                i2c_iqs_initial();
            }
        }
        else if (app_cfg_const.psensor_vendor == PSENSOR_VENDOR_IQS873)
        {
            app_sensor_i2c_init(I2C_AZQ_ADDR, DEF_I2C_CLK_SPD, false);
            if (i2c_iqs_check_device() != IQS873_MODULE)
            {
                APP_PRINT_ERROR0("No IQS873 module detected");
            }
            else
            {
                i2c_iqs873_setup();
                app_sensor_int_gpio_init(app_cfg_const.gsensor_int_pinmux, app_psensor_iqs773_873_intr_cb);
                i2c_iqs_initial();
            }
        }
#endif
    }

    if ((app_cfg_const.sensor_support) || (app_cfg_const.gsensor_support) ||
        (app_cfg_const.psensor_support))
    {
        app_sensor_init();
    }
#endif

#if F_APP_CAP_TOUCH_SUPPORT
    if (CapTouch_IsSystemEnable())
    {
        app_cap_touch_driver_init(true);
    }
#endif

#if F_APP_EXT_MIC_SWITCH_SUPPORT
    if (app_cfg_const.boom_mic_support)
    {
        app_ext_mic_gpio_driver_init();
    }
#endif

#if F_APP_SLIDE_SWITCH_SUPPORT
    app_slide_switch_driver_init();
#endif

#if F_APP_QDECODE_SUPPORT
    if (app_cfg_const.wheel_support)
    {
        app_qdec_init_status_read();
        app_qdec_driver_init();
    }
#endif

#if F_APP_PERIODIC_WAKEUP
    app_dlps_system_wakeup_clear_rtc_int();
#endif

#if (CONFIG_SOC_SERIES_RTL8773D || TARGET_RTL8773DFL)
    audio_probe_dsp2_exist_set(app_cfg_const.ext_buck_support);
#endif

#if F_APP_HIFI4_SUPPORT
    if (app_cfg_const.ext_buck_support)
    {
        if (app_buck_tps62860_init(app_cfg_const.buck_enable_pinmux) == true)
        {
            APP_PRINT_TRACE0("driver_init: tps62860 init success");
        }
        else if (app_buck_apw7564_init() == true)
        {
            APP_PRINT_TRACE0("driver_init: apw7564 init success");
        }
        else
        {
            APP_PRINT_ERROR0("driver_init: No invalid external buck recognised, please check the hardware setting!!");
        }
    }
    else
    {
        APP_PRINT_TRACE0("driver_init: not init buck support");
    }
#endif

#if F_APP_ALC1017_SUPPORT
    app_alc1017_init();
#endif

#if F_APP_EXT_FLASH_SUPPORT
    if (app_cfg_const.external_flash_support)
    {
        ext_flash_init();
    }
#endif
}

static void app_bt_gap_init(void)
{
    uint32_t class_of_device = (uint32_t)((app_cfg_const.class_of_device[0]) |
                                          (app_cfg_const.class_of_device[1] << 8) |
                                          (app_cfg_const.class_of_device[2] << 16));

    uint16_t supervision_timeout = DEFAULT_SUPVISIONTIMEOUT;
    uint16_t link_policy = GAP_LINK_POLICY_ROLE_SWITCH | GAP_LINK_POLICY_SNIFF_MODE;
    uint8_t radio_mode = GAP_RADIO_MODE_NONE_DISCOVERABLE;
    bool limited_discoverable = false;
    bool auto_accept_acl = false;

    uint8_t  pagescan_type = GAP_PAGE_SCAN_TYPE_INTERLACED;
    uint16_t pagescan_interval = DEFAULT_PAGESCAN_INTERVAL;
    uint16_t pagescan_window = DEFAULT_PAGESCAN_WINDOW;
    uint16_t page_timeout = DEFAULT_PAGE_TIMEOUT;
    uint8_t  page_scan_repetition_mode = GAP_PAGE_SCAN_REPETITION_R1;

    uint8_t inquiryscan_type = GAP_INQUIRY_SCAN_TYPE_INTERLACED;
    uint16_t inquiryscan_window = DEFAULT_INQUIRYSCAN_WINDOW;
    uint16_t inquiryscan_interval = DEFAULT_INQUIRYSCAN_INTERVAL;
    uint8_t inquiry_mode = GAP_INQUIRY_MODE_EXTENDED_RESULT;

    uint8_t pair_mode = GAP_PAIRING_MODE_PAIRABLE;
    uint8_t io_cap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT;
    uint8_t oob_enable = false;
    uint8_t bt_mode = GAP_BT_MODE_21ENABLED;
    uint16_t auth_flags = GAP_AUTHEN_BIT_BONDING_FLAG | GAP_AUTHEN_BIT_SC_FLAG;

#if F_APP_LEA_SUPPORT
#if CONFIG_REALTEK_GFPS_LE_DEVICE_SUPPORT
    //BLE LTK CTKD BR Linkkey
#else
    auth_flags |= GAP_AUTHEN_BIT_SC_BR_FLAG;
#endif
#endif

#if CONFIG_REALTEK_APP_BOND_MGR_SUPPORT
    bt_bond_init();
#if F_APP_LEA_SUPPORT
    ble_audio_bond_init();
#endif
#endif

    if ((app_cfg_const.bud_role == REMOTE_SESSION_ROLE_SINGLE) && (app_cfg_nv.factory_reset_done == 0))
    {
        /* change the scan interval to 100ms(0xA0 * 0.625) before factory reset */
        pagescan_interval = 0xA0;
        inquiryscan_interval = 0xA0;
    }
#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
    if (extend_app_cfg_const.gfps_sass_support)
    {
        pagescan_interval = SASS_PAGESCAN_INTERVAL;
        pagescan_window = SASS_PAGESCAN_WINDOW;
    }
#endif

    gap_lib_init();

    //0: to be master
    gap_br_cfg_accept_role(1);

    gap_br_set_param(GAP_BR_PARAM_NAME, GAP_DEVICE_NAME_LEN, app_cfg_nv.device_name_legacy);

#if BISTO_FEATURE_SUPPORT
    if (extend_app_cfg_const.bisto_support)
    {
        app_bisto_bt_set_name();
    }
#endif

    gap_set_param(GAP_PARAM_BOND_PAIRING_MODE, sizeof(uint8_t), &pair_mode);
    gap_set_param(GAP_PARAM_BOND_AUTHEN_REQUIREMENTS_FLAGS, sizeof(uint16_t), &auth_flags);
    gap_set_param(GAP_PARAM_BOND_IO_CAPABILITIES, sizeof(uint8_t), &io_cap);
    gap_set_param(GAP_PARAM_BOND_OOB_ENABLED, sizeof(uint8_t), &oob_enable);

    gap_br_set_param(GAP_BR_PARAM_BT_MODE, sizeof(uint8_t), &bt_mode);
    gap_br_set_param(GAP_BR_PARAM_COD, sizeof(uint32_t), &class_of_device);
    gap_br_set_param(GAP_BR_PARAM_LINK_POLICY, sizeof(uint16_t), &link_policy);
    gap_br_set_param(GAP_BR_PARAM_SUPV_TOUT, sizeof(uint16_t), &supervision_timeout);
    gap_br_set_param(GAP_BR_PARAM_AUTO_ACCEPT_ACL, sizeof(bool), &auto_accept_acl);

    gap_br_set_param(GAP_BR_PARAM_RADIO_MODE, sizeof(uint8_t), &radio_mode);
    gap_br_set_param(GAP_BR_PARAM_LIMIT_DISCOV, sizeof(bool), &limited_discoverable);

    gap_br_set_param(GAP_BR_PARAM_PAGE_SCAN_TYPE, sizeof(uint8_t), &pagescan_type);
    gap_br_set_param(GAP_BR_PARAM_PAGE_SCAN_INTERVAL, sizeof(uint16_t), &pagescan_interval);
    gap_br_set_param(GAP_BR_PARAM_PAGE_SCAN_WINDOW, sizeof(uint16_t), &pagescan_window);
    gap_br_set_param(GAP_BR_PARAM_PAGE_TIMEOUT, sizeof(uint16_t), &page_timeout);
    gap_br_set_param(GAP_BR_PARAM_PAGE_SCAN_REPETITION_MODE, sizeof(uint8_t),
                     &page_scan_repetition_mode);

    gap_br_set_param(GAP_BR_PARAM_INQUIRY_SCAN_TYPE, sizeof(uint8_t), &inquiryscan_type);
    gap_br_set_param(GAP_BR_PARAM_INQUIRY_SCAN_INTERVAL, sizeof(uint16_t), &inquiryscan_interval);
    gap_br_set_param(GAP_BR_PARAM_INQUIRY_SCAN_WINDOW, sizeof(uint16_t), &inquiryscan_window);
    gap_br_set_param(GAP_BR_PARAM_INQUIRY_MODE, sizeof(uint8_t), &inquiry_mode);

    bt_pairing_tx_power_set(-2);

    app_ble_gap_param_init();
}

static void framework_init(void)
{
    /* System Manager */
    sys_mgr_init(audio_evt_queue_handle);

    /* RemoteController Manager */
    remote_mgr_init((T_REMOTE_SESSION_ROLE)app_cfg_nv.bud_role);
    remote_local_addr_set(app_cfg_nv.bud_local_addr);
    remote_peer_addr_set(app_cfg_nv.bud_peer_addr);

    /* Bluetooth Manager */
    bt_mgr_init();

    /* Audio Manager */
    audio_mgr_init(app_cfg_nv.audio_latency * POOL_SIZE_FACTOR, VOICE_POOL_SIZE, RECORD_POOL_SIZE,
                   NOTIFICATION_POOL_SIZE);
    audio_mgr_set_max_plc_count(app_cfg_const.maximum_packet_lost_compensation_count);

    if ((app_cfg2_const.dsp_jtag_enable) || (app_cfg2_const.dsp2_jtag_enable) ||
        (app_cfg2_const.adsp_jtag_enable))
    {
        audio_probe_disable_dsp_powerdown();
    }
}

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT || F_APP_TMAP_BMR_SUPPORT
static void app_ble_audio_init(void)
{
    app_lea_profile_init();
}
#endif

static void app_task(void *pvParameters)
{
    uint8_t event;

    app_adp_detect();
    gap_start_bt_stack(audio_evt_queue_handle, audio_io_queue_handle, MAX_NUMBER_OF_GAP_MESSAGE);

    APP_PRINT_WARN2("app_task: unused memory %d, current cpu freq %d MHz", mem_peek(),
                    pm_cpu_freq_get());

    while (true)
    {
        if (os_msg_recv(audio_evt_queue_handle, &event, 0xFFFFFFFF) == true)
        {

            if (app_cfg_const.open_dbg_log_for_system_busy == 1)
            {
                APP_PRINT_WARN2("app_task start event 0x%x waiting %d", event,
                                *((uint32_t *)audio_evt_queue_handle + 14));
            }

            if (EVENT_GROUP(event) == EVENT_GROUP_IO)
            {
                T_IO_MSG io_msg;

                if (os_msg_recv(audio_io_queue_handle, &io_msg, 0) == true)
                {
                    if (event == EVENT_IO_TO_APP)
                    {
                        app_io_msg_handler(io_msg);
                    }
                }
            }
            else if (EVENT_GROUP(event) == EVENT_GROUP_STACK)
            {
                gap_handle_msg(event);
            }
            else if (EVENT_GROUP(event) == EVENT_GROUP_FRAMEWORK)
            {
                sys_mgr_event_handle(event);
            }
            else if (EVENT_GROUP(event) == EVENT_GROUP_APP)
            {
                app_timer_handle_msg(event);
            }

            if (app_cfg_const.open_dbg_log_for_system_busy == 1)
            {
                APP_PRINT_WARN0("event end");
            }
        }
    }
}

static void app_wakeup_reason_check(void)
{
    uint8_t pm_wakeup_reason = 0;

    pm_wakeup_reason = power_down_check_wake_up_reason();

    if (pm_wakeup_reason & POWER_DOWN_WAKEUP_MFB)
    {
        app_db.wake_up_reason |= WAKE_UP_MFB;
    }

    if (pm_wakeup_reason & POWER_DOWN_WAKEUP_ADP)
    {
        app_db.wake_up_reason |= WAKE_UP_ADP;
    }

    if (pm_wakeup_reason & POWER_DOWN_WAKEUP_CTC)
    {
        app_db.wake_up_reason |= WAKE_UP_CTC;
    }

    if (pm_wakeup_reason & POWER_DOWN_WAKEUP_PAD)
    {
        uint8_t i = 0;

        app_db.wake_up_reason |= WAKE_UP_PAD;

        if (System_WakeUpInterruptValue(app_cfg_const.gpio_box_detect_pinmux))
        {
            app_db.wake_up_reason |= WAKE_UP_3PIN;
        }

        if (System_WakeUpInterruptValue(app_cfg_const.key_pinmux[0]))
        {
            app_db.wake_up_reason |= WAKE_UP_KEY0;
        }

        for (i = 1; i < MAX_KEY_NUM; i++)
        {
            if (System_WakeUpInterruptValue(app_cfg_const.key_pinmux[i]))
            {
                if (app_key_is_combinekey_power_on_off(i))
                {
                    app_db.wake_up_reason |= WAKE_UP_COMBINE_KEY_POWER_ONOFF;
                }
            }
        }
    }

    APP_PRINT_INFO2("app_wakeup_reason_check: pm_wakeup_reason: 0x%x, app_wake_up_reason: 0x%x",
                    pm_wakeup_reason, app_db.wake_up_reason);
}

int main(void)
{
    uint32_t time_entry_app;
    void *app_task_handle;
    uint16_t stack_size = 1024 * 3 - 256;
#if F_APP_HIFI4_SUPPORT
    void *ipc_task_handle = NULL;
#endif
    T_RELEASE_VER mcu_cfg;
    T_RELEASE_VER dsp_cfg;

    mcu_cfg.release_ver = get_image_release_version(IMG_MCUCONFIG);
    dsp_cfg.release_ver = get_image_release_version(IMG_DSPCONFIG);

    APP_PRINT_INFO8("mcu cfg version: %d.%d.%d.%d,dsp cfg version: %d.%d.%d.%d",
                    mcu_cfg.chip_ver, mcu_cfg.crb_ver, mcu_cfg.tool_ver, mcu_cfg.revision,
                    dsp_cfg.chip_ver, dsp_cfg.crb_ver, dsp_cfg.tool_ver, dsp_cfg.revision);

    log_module_trace_set(MODULE_HCI, LEVEL_TRACE, false);
    log_module_trace_set(MODULE_BTIF, LEVEL_TRACE, false);
    log_module_trace_set(MODULE_GAP, LEVEL_TRACE, false);

    time_entry_app = sys_timestamp_get();
    APP_PRINT_INFO1("TIME FROM PATCH TO APP: %d ms", time_entry_app);
    APP_PRINT_INFO2("APP COMPILE TIME: [%s - %s]", TRACE_STRING(__DATE__), TRACE_STRING(__TIME__));

#if F_APP_LOCAL_PLAYBACK_SUPPORT
    stack_size = 1024 * 4;
#endif

#if F_APP_TEAMS_CFU_SUPPORT
    stack_size = 1024 * 6;
#endif

#if F_APP_FINDMY_FEATURE_SUPPORT
    stack_size = 1024 * 6;
#endif

    // Check dsp image feature string
    {
        uint8_t image_feature_info[16] = {0};
        if (get_image_feature_info(IMG_DSPSYSTEM, image_feature_info, 16) != 0)
        {
            image_feature_info[15] = 0;
        }
        APP_PRINT_INFO2("IMG_DSPSYSTEM(%x) Feature String: %s", IMG_DSPSYSTEM,
                        TRACE_STRING(image_feature_info));
        memset(image_feature_info, 0, 16);
        if (get_image_feature_info(IMG_DSPAPP, image_feature_info, 16) != 0)
        {
            image_feature_info[15] = 0;
        }
        APP_PRINT_INFO2("IMG_DSPAPP(%x) Feature String: %s", IMG_DSPAPP, TRACE_STRING(image_feature_info));
    }

    {
        uint32_t cpu_freq;
        pm_cpu_freq_set(APP_BASE_CPU_FREQ, &cpu_freq);
    }

    if (sys_hall_get_reset_status())
    {
        APP_PRINT_INFO0("APP RESTART FROM WDT_SOFTWARE_RESET");
    }
    else
    {
        //APP power off reboot also in this case
        APP_PRINT_INFO0("APP START FROM HW_RESET");
    }

    os_msg_queue_create(&audio_io_queue_handle, "ioQ", MAX_NUMBER_OF_IO_MESSAGE, sizeof(T_IO_MSG));
    os_msg_queue_create(&audio_evt_queue_handle, "evtQ", MAX_NUMBER_OF_RX_EVENT, sizeof(unsigned char));
#if F_APP_FINDMY_FEATURE_SUPPORT
    os_msg_queue_create(&app_findmy_queue_handle, "findmy_msg", MAX_NUMBER_OF_IO_MESSAGE,
                        sizeof(T_FINDMY_BLE_INDICATION));
#endif

    app_init_timer(audio_evt_queue_handle, MAX_NUMBER_OF_APP_TIMER_MODULE);
    pm_cpu_freq_init();
    app_ipc_init();

    app_cfg_init();

    app_wakeup_reason_check();
    board_init();
    app_bt_gap_init();
    framework_init();

    //Callback provider for other modules MUST init fisrt
#if F_APP_ERWS_SUPPORT
    app_relay_init();
#endif
    app_ota_init();

    //driver init MUST after callback provider init
    driver_init();

    app_dlps_init();
    app_auto_power_off_init();
    app_in_out_box_init();

#if F_APP_ADP_5V_CMD_SUPPORT || F_APP_ONE_WIRE_UART_SUPPORT
    app_adp_cmd_init();
#endif

    app_mmi_init();

	extern void app_wlt_ui_init(void);
	app_wlt_ui_init();

    if (is_single_tone_test_mode()) //DUT test mode
    {
        app_audio_init();

#if F_APP_SLIDE_SWITCH_SUPPORT
        app_slide_switch_init();
#endif
        reset_single_tone_test_mode();
        mp_hci_test_init(MP_HCI_TEST_DUT_MODE);
    }
    else //Normal mode
    {
#if F_APP_GAMING_DONGLE_SUPPORT
        app_dongle_audio_init();
#endif

        app_gap_init();
        app_ble_gap_init();
        app_ble_client_init();

        //DO NOT change order: app_cfg_nv.bud_role will be affected
        app_bt_policy_init();

        app_bt_point_init();

        //DO NOT change order: app_db.remote_session_state will be affected
#if F_APP_ERWS_SUPPORT
        app_rdtp_init();
        app_bt_sniffing_init();
        app_roleswap_init();
#endif
        //DO NOT change order: roleswap ctrl init must be placed before audio init
        app_roleswap_ctrl_init();

        //Other app module MUST init after here
#if F_APP_IO_OUTPUT_SUPPORT
        app_io_output_init();
#endif

        app_ble_service_init();
#if F_APP_DURIAN_SUPPORT
        app_durian_init();
#endif

        app_hfp_init();
        app_avrcp_init();
        app_a2dp_init();
        app_sdp_init();
        app_spp_init();

#if F_APP_BT_PROFILE_PBAP_SUPPORT
        app_pbap_init();
#endif

#if F_APP_HID_SUPPORT
        app_hid_init();
#endif

#if F_APP_IAP_SUPPORT
        app_iap_init();
#endif

#if F_APP_HFP_AG_SUPPORT
        app_hfp_ag_init();
#endif

        app_audio_init();

#if F_APP_TTS_SUPPORT
        if (app_cfg_const.tts_support)
        {
            app_tts_init();
        }
#endif

        app_multi_init();

#if F_APP_SINGLE_MUTLILINK_SCENERIO_1
        app_teams_multilink_init();
#elif F_APP_MUTLILINK_SOURCE_PRIORITY_UI
        app_multilink_customer_init();
#endif

        app_bud_loc_init();
        app_loc_mgr_init();

#if F_APP_TEST_SUPPORT
        app_test_init();
#endif

#if F_APP_RTK_FAST_PAIR_ADV_FEATURE_SUPPORT
        app_rtk_fast_pair_adv_init();
#endif

#if F_APP_MALLEUS_SUPPORT
        app_malleus_init(MALLEUS_FULL_CYCLE, MALLEUS_FULL_CYCLE);
#endif

#if F_APP_ADC_SUPPORT || (F_APP_EXT_MIC_SWITCH_SUPPORT && F_APP_EXT_MIC_PLUG_IN_ADC_DETECT)
        app_adc_init();
#endif

#if F_APP_GATT_OVER_BREDR_SUPPORT
        app_att_init();
#endif

        app_device_init();
        app_ble_device_init();
        app_transfer_init();

#if F_APP_VAD_SUPPORT
        app_vad_init();
#endif

#if F_APP_CAP_TOUCH_SUPPORT
        if (CapTouch_IsSystemEnable())
        {
            app_cap_touch_init();
        }
#endif

#if F_APP_LISTENING_MODE_SUPPORT
        app_listening_mode_init();
#endif

#if F_APP_ANC_SUPPORT
        app_anc_init();
#endif

#if F_APP_APT_SUPPORT
        app_apt_init();
#endif

#if F_APP_LINEIN_SUPPORT
        app_line_in_init();
#endif
      

#if F_APP_AIRPLANE_SUPPORT
        app_airplane_init();
#endif

        app_cmd_init();
        app_bond_init();

#if F_APP_DATA_CAPTURE_SUPPORT
        app_data_capture_init();
#endif

#if F_APP_BLE_BOND_SYNC_SUPPORT
        app_ble_bond_init();
#endif

#if F_APP_BOND_MGR_BLE_SYNC
        ble_bond_sync_init();
#endif

#if F_APP_OTA_TOOLING_SUPPORT
        if (app_cfg_nv.ota_tooling_start)
        {
            app_ota_tooling_adv_init();
        }
#endif

#if AMA_FEATURE_SUPPORT || BISTO_FEATURE_SUPPORT || F_APP_IAP_RTK_SUPPORT
        spp_stream_init(0xff);
        ble_stream_init(0xff);
        iap_stream_init(0xff);
#endif

        app_third_party_srv_init();

#if F_APP_COMMON_DONGLE_SUPPORT
        app_dongle_common_init();
#endif
        app_dongle_spp_init();

#if F_APP_GAMING_DONGLE_SUPPORT
        app_dongle_data_ctrl_init();
#if F_APP_GAMING_CONTROLLER_SUPPORT || TARGET_LE_AUDIO_GAMING
        app_gaming_sync_init(false, 256);
#else
        app_gaming_sync_init(true, 128);
#endif

        if (app_cfg_const.enable_dongle_dual_mode)
        {
            app_dongle_dual_mode_init();
        }

        legacy_gaming_init(app_dongle_legacy_gaming_event_cback);
#endif

        app_vendor_init();

#if F_APP_24G_BT_AUDIO_SOURCE_CTRL_SUPPORT
        if (app_cfg_const.enable_24g_bt_audio_source_switch)
        {
            app_dongle_source_ctrl_init();
        }
#endif

#if F_APP_RWS_MULTI_SPK_SUPPORT
        msbc_sync_init();
#endif

#if F_APP_TEAMS_CUSTOMIZED_CMD_SUPPORT
        app_teams_cmd_info_init();
#endif

#if F_APP_BLE_SWIFT_PAIR_SUPPORT
        app_swift_pair_init();
#endif

#if F_APP_LOCAL_PLAYBACK_SUPPORT
        if (app_cfg_const.local_playback_support)
        {
            set_cpu_sleep_en(0);
            app_playback_init();
            app_playback_trans_file_init();
            audio_fs_init((TCHAR *)_T("0:audio"), 1024, false, NULL);
#if F_APP_LOCAL_PLAYBACK_SUPPORT && TARGET_RTL8773CO
            app_listen_save_init();
#endif
        }
#endif

#if F_APP_USB_AUDIO_SUPPORT || F_APP_USB_HID_SUPPORT
        if (app_cfg2_const.usb_config.func_en)
        {
            app_usb_init();
        }
#endif

#if F_APP_USB_SUSPEND_SUPPORT
        app_usb_suspend_init();
#endif

#if F_APP_CFU_FEATURE_SUPPORT
        app_cfu_init();
#endif

#if F_APP_IAP_RTK_SUPPORT
        app_iap_rtk_init();
#endif

#if F_APP_EXT_MIC_SWITCH_SUPPORT && F_APP_EXT_MIC_PLUG_IN_GPIO_DETECT
        app_ext_mic_gpio_detect_init();
#endif

#if F_APP_HEARABLE_SUPPORT
        if (app_cfg_const.enable_ha)
        {
            app_ha_init();
        }
#endif

#if F_APP_TMAP_CT_SUPPORT || F_APP_TMAP_UMR_SUPPORT || F_APP_TMAP_BMR_SUPPORT
        app_ble_audio_init();
#endif

#if F_APP_BRIGHTNESS_SUPPORT
        app_apt_brightness_init();
#endif

#if F_APP_SLIDE_SWITCH_SUPPORT
        app_slide_switch_init();
#endif

#if F_APP_QDECODE_SUPPORT
        if (app_cfg_const.wheel_support)
        {
            app_qdec_init();
        }
#endif

#if F_APP_AUTO_POWER_TEST_LOG
        app_power_test_init();
#endif

#if CONFIG_REALTEK_GFPS_SASS_SUPPORT
        if (extend_app_cfg_const.gfps_sass_support)
        {
            app_sass_policy_init();
        }
#endif
        if (app_cfg_const.led_support)
        {
            app_led_init();
        }

#if F_APP_CHATGPT_SUPPORT
        app_chatgpt_init();
#endif

#if F_APP_GAMING_CONTROLLER_SUPPORT
        app_dongle_controller_init();
#endif

        //increase app task priority to 2 so that user could implement some background daemon task
        os_task_create(&app_task_handle, "app_task", app_task, NULL, stack_size, 2);

#if F_APP_POWER_TEST
        hal_debug_init();
        hal_debug_task_time_proportion_init(10 * 1000); //include cache hit
        dlps_util_debug_init(10 * 1000); //ms
        dlps_util_cpu_sleep_monitor_start();
        power_test_dump_register(POWER_DEBUG_STAGE_BOOT);
#endif

#if F_APP_DEBUG_HIT_RATE_PRINT
        cache_hit_count_init(10000);
#endif
    }

#if F_APP_HIFI4_SUPPORT
    os_task_create(&ipc_task_handle, "ipc_task", ipc_task, NULL, 1024 * 2, 2);
#endif

    monitor_memory_and_timer(app_cfg_const.timer_monitor_heap_and_timer_timeout);

    APP_PRINT_INFO1("TIME FROM APP TO OS SCHED: %d ms", sys_timestamp_get() - time_entry_app);

    os_sched_start();

    return 0;
}

