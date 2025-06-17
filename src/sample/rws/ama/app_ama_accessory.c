#if AMA_FEATURE_SUPPORT

#include "app_main.h"
#include "app_cfg.h"
#include "app_bt_policy_api.h"
#include "ama_api.h"
#include "app_ama_accessory.h"
#include "app_ama_transport.h"
#include "device.pb.h"
#include "app_ama_record.h"

#include "app_ama_ble.h"


static struct
{
    AudioFormat audio_format;
} app_ama_accessory = {.audio_format = AudioFormat_OPUS_16KHZ_32KBPS_CBR_0_20MS};


void app_ama_accessory_role_update(T_REMOTE_SESSION_ROLE rtk_role)
{
    AMA_ROLE ama_role = AMA_ROLE_SINGLE;

    APP_PRINT_TRACE1("app_ama_accessory_role_update: mcu role %d", rtk_role);

    switch (rtk_role)
    {
    case REMOTE_SESSION_ROLE_SINGLE:
        ama_role = AMA_ROLE_SINGLE;
        break;

    case REMOTE_SESSION_ROLE_PRIMARY:
        ama_role = AMA_ROLE_PRIMARY;
        break;

    case REMOTE_SESSION_ROLE_SECONDARY:
        ama_role = AMA_ROLE_SECONDARY;
        break;
    }

    ama_service_update_role(ama_role);
}


void app_ama_accessory_handle_power_off(void)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_ama_ble_adv_stop();
    }
}


void app_ama_accessory_handle_power_on(bool is_pairing)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        app_ama_ble_adv_start();
    }
}

void app_ama_accessory_handle_enter_pair_mode(void)
{
    APP_PRINT_INFO0("app_ama_accessory_handle_enter_pair_mode");

    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_ama_ble_adv_start();
    }
}


static void app_ama_accessory_info(void)
{
    char *serial_number = "1234567890";
    char name[sizeof(app_cfg_nv.device_name_legacy) + 1] = {'\0'}; //local array, should check the length!!!
    char device_type[sizeof(extend_app_cfg_const.ama_id) + 1] = {'\0'};//local array, should check the length!!!
    pb_size_t supported_transports_count = 0;
    Transport supported_transports[DEVICE_SUPPORTED_TRANSPORTS_COUNT_MAX] = {(Transport)0};

    memcpy(name, app_cfg_nv.device_name_legacy, sizeof(app_cfg_nv.device_name_legacy));
    memcpy(device_type, extend_app_cfg_const.ama_id, sizeof(extend_app_cfg_const.ama_id));

    supported_transports_count = 0;

    if (app_ama_transport_supported(AMA_SPP_STREAM))
    {
        supported_transports[supported_transports_count] = Transport_BLUETOOTH_RFCOMM;
        supported_transports_count++;
    }

    if (app_ama_transport_supported(AMA_BLE_STREAM))
    {
        supported_transports[supported_transports_count] = Transport_BLUETOOTH_LOW_ENERGY;
        supported_transports_count++;
    }

    if (app_ama_transport_supported(AMA_IAP_STREAM))
    {
        supported_transports[supported_transports_count] = Transport_BLUETOOTH_IAP;
        supported_transports_count++;
    }

    ama_set_device_info(serial_number, name, device_type,
                        supported_transports_count,
                        supported_transports);
}


static void app_ama_accessory_cfg(void)
{
    bool needs_assistant_override = false;
    bool needs_setup = false;
    ama_set_device_config(needs_assistant_override, needs_setup);
}


static void app_ama_accessory_speech_setting(void)
{
    SpeechInitiator speech_initiator;
    speech_initiator.type = SpeechInitiator_Type_TAP;
    speech_initiator.wake_word.start_index_in_samples = 0;
    speech_initiator.wake_word.end_index_in_samples = 0;
    speech_initiator.wake_word.near_miss = 0;
    speech_initiator.wake_word.metadata.size = 0;
    //speech_initiator.wake_word.metadata.bytes


    ama_set_speech_setting(&speech_initiator, AudioProfile_FAR_FIELD,
                           app_ama_accessory.audio_format,
                           AudioSource_STREAM);

    app_ama_record_init(app_ama_accessory.audio_format);
}


void app_ama_accessory_init(void)
{
    app_ama_accessory_info();

    app_ama_accessory_cfg();

    app_ama_accessory_speech_setting();
}

#endif
