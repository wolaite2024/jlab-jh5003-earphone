#if DMA_FEATURE_SUPPORT
#include "dma_service_api.h"
#include "app_cfg.h"
#include "stddef.h"
#include "stdio.h"
#include "app_dma_device.h"
#include "app_dma_record.h"
#include "string.h"


#define N_ELEMENTS(X)       (sizeof(X)/sizeof(X[0]))


struct
{
    char bd_addr_str[sizeof("xx:xx:xx:xx:xx:xx") + 1];
} app_dma_mgr = {.bd_addr_str = ""};


static void app_dma_gen_bd_addr_str(uint8_t bd_addr[6], char *str)
{
    sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x", bd_addr[0], bd_addr[1], bd_addr[2],
            bd_addr[3], bd_addr[4], bd_addr[5]);
}


void app_dma_set_device_info(DeviceInformation *p_device_info)
{
    memcpy(p_device_info, &(DeviceInformation)DEVICE_INFORMATION__INIT, sizeof(DeviceInformation));

    app_dma_gen_bd_addr_str(app_cfg_nv.bud_local_addr, app_dma_mgr.bd_addr_str);
    p_device_info->classic_bluetooth_mac = app_dma_mgr.bd_addr_str;

    p_device_info->device_type = "HEADPHONE";

    p_device_info->disable_heart_beat = true;

    p_device_info->enable_advanced_security = false;

    p_device_info->firmware_version = "CO SDK";

    p_device_info->initiator_type = INITIATOR_TYPE__TAP;

    p_device_info->manufacturer = "RealTek";

    p_device_info->model = "RTL8773CO";

    p_device_info->name = "EVB";

    p_device_info->no_a2dp = false;

    p_device_info->no_at_command = true;

    AudioFormat audio_formats[] = {AUDIO_FORMAT__OPUS_16KHZ_32KBPS_CBR_0_20MS, AUDIO_FORMAT__OPUS_16KHZ_16KBPS_CBR_0_20MS, AUDIO_FORMAT__MSBC};
    p_device_info->n_supported_audio_formats = N_ELEMENTS(audio_formats);
    p_device_info->supported_audio_formats = audio_formats;

    Transport transports[] = {TRANSPORT__BLUETOOTH_LOW_ENERGY, TRANSPORT__BLUETOOTH_RFCOMM, TRANSPORT__BLUETOOTH_IAP};
    p_device_info->n_supported_transports = N_ELEMENTS(transports);
    p_device_info->supported_transports = transports;

    p_device_info->ota_version = "";

    p_device_info->product_id = "1111";

    p_device_info->serial_number = "1234";

    p_device_info->software_version = "5678";

    p_device_info->support_fm = false;

    p_device_info->support_media = true;
}


void app_dma_set_device_cfg(DeviceConfiguration *p_device_cfg)
{
    memcpy(p_device_cfg, &(DeviceConfiguration)DEVICE_CONFIGURATION__INIT, sizeof(DeviceConfiguration));

    p_device_cfg->needs_assistant_override = false;
    p_device_cfg->needs_setup = true;
}


void app_dma_set_pair_info(PairInformation *p_pair_info)
{
    memcpy(p_pair_info, &(PairInformation)PAIR_INFORMATION__INIT, sizeof(PairInformation));

    //ToDo: add pair info
}


void app_dma_set_speech_initiator(SpeechInitiator *p_speech_initiator)
{
    memcpy(p_speech_initiator, &(SpeechInitiator)SPEECH_INITIATOR__INIT, sizeof(SpeechInitiator));

    p_speech_initiator->play_prompt_tone = true;
    p_speech_initiator->type = SPEECH_INITIATOR__TYPE__TAP;
    p_speech_initiator->wake_word = &(SpeechInitiator__WakeWord)SPEECH_INITIATOR__WAKE_WORD__INIT;
}


static void app_dma_set_speech_settings(SpeechSettings *p_speech_settings)
{
    memcpy(p_speech_settings, &(SpeechSettings)SPEECH_SETTINGS__INIT, sizeof(SpeechSettings));

    p_speech_settings->audio_format = app_dma_record_get_format();
    p_speech_settings->audio_profile = AUDIO_PROFILE__NEAR_FIELD;
    p_speech_settings->audio_source = AUDIO_SOURCE__STREAM;
}


static void app_dma_service_init(void)
{
    DMA_SERVICE_INIT_PARAMS service_params = {0};
    app_dma_set_device_info(&service_params.device_info);
    app_dma_set_device_cfg(&service_params.device_cfg);
    app_dma_set_pair_info(&service_params.pair_info);
    app_dma_set_speech_initiator(&service_params.speech_initiator);
    app_dma_set_speech_settings(&service_params.speech_settings);

    app_dma_service_init(&service_params);
}





void app_dma_init(void)
{
    app_dma_record_init();

    app_dma_device_init();

    app_dma_service_init();
}

#endif

