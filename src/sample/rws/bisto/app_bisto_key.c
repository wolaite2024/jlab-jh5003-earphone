#if BISTO_FEATURE_SUPPORT
#include "app_bisto_key.h"
#include "bisto_api.h"
#include "trace.h"
#include "app_cfg.h"


#define INVALID_BTN_ID      (0xFF)
#define INVALID_EVENT       (0xFF)


static struct
{
    bool ptt_flag;
} app_bisto_key = {.ptt_flag = false};


void app_bisto_key_btns_action(T_MMI_ACTION action)
{
    GSoundButtons btn_id = (GSoundButtons)INVALID_BTN_ID;
    GSoundButtonEvents event = (GSoundButtonEvents)INVALID_EVENT;

    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SECONDARY)
    {
        return;
    }

    APP_PRINT_TRACE2("app_bisto_key_btns_action: action %d, ptt_flag %d", action,
                     app_bisto_key.ptt_flag);

    switch ((uint32_t)action)
    {

    case MMI_BISTO_FETCH:
        btn_id = GSOUND_TARGET_BUTTON_FETCH;
        event = GSOUND_TARGET_BUTTON_EVENT_DOWN;
        bisto_btns_click_event_post(bisto_btns_get_instance(), btn_id, event);

        btn_id = GSOUND_TARGET_BUTTON_FETCH;
        event = GSOUND_TARGET_BUTTON_EVENT_UP;
        bisto_btns_click_event_post(bisto_btns_get_instance(), btn_id, event);

//        btn_id = GSOUND_TARGET_BUTTON_FETCH;
//        event = GSOUND_TARGET_BUTTON_EVENT_CLICK;
//        bisto_btns_click_event_post(bisto_btns_get_instance(), btn_id, event);
        break;

    case MMI_BISTO_PTT:
        if (app_bisto_key.ptt_flag == false)
        {
            btn_id = GSOUND_TARGET_BUTTON_GA;
            event = GSOUND_TARGET_BUTTON_EVENT_DOWN;
            bisto_btns_click_event_post(bisto_btns_get_instance(), btn_id, event);
            btn_id = GSOUND_TARGET_BUTTON_GA;
            event = GSOUND_TARGET_BUTTON_EVENT_LONG_CLICK;
            bisto_btns_click_event_post(bisto_btns_get_instance(), btn_id, event);
        }
        else
        {
            btn_id = GSOUND_TARGET_BUTTON_GA;
            event = GSOUND_TARGET_BUTTON_EVENT_UP;
            bisto_btns_click_event_post(bisto_btns_get_instance(), btn_id, event);
        }

        app_bisto_key.ptt_flag = !app_bisto_key.ptt_flag;
        break;

    default:
        break;
    }
}


int32_t app_bisto_key_init(void)
{
    bisto_btns_create();

    bisto_act_create();

    return 0;
}

#endif
