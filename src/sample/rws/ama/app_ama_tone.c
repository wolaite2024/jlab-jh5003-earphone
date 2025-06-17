#if AMA_FEATURE_SUPPORT

#include "app_ama_tone.h"
#include "app_cfg.h"
#include "ama_service_api.h"


static struct
{
    bool internal_tone_enabled;
} app_ama_tone = {.internal_tone_enabled = true};


void app_ama_tone_play(T_APP_AUDIO_TONE_TYPE tone_type)
{
    if (!app_ama_tone.internal_tone_enabled)
    {
        app_audio_tone_type_play(tone_type, true, true);
    }
    else
    {
        APP_PRINT_TRACE1("app_ama_tone_play: only play AMA internal earcon, type %d", tone_type);
    }
}



void app_ama_tone_init(void)
{
    app_ama_tone.internal_tone_enabled = !extend_app_cfg_const.ama_mcu_ringtone_enable;

    if (app_ama_tone.internal_tone_enabled == true)
    {
        ama_earcon_enable();
    }
    else
    {
        ama_earcon_disable();
    }

    return;
}



#endif



