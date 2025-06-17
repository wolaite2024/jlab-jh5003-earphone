#if AMA_FEATURE_SUPPORT

#include "app_ama_ble.h"
#include "app_ama_timer.h"
#include "app_ama_record.h"


#include "app_timer.h"
#include "trace.h"


#include <string.h>



typedef enum
{
    WAIT_ENDPOINT_SPEECH_EVT,
    GET_PLATFORM_EVT,
    ADV_TIMEOUT_EVT,
    EVT_TOTAL
} EVT;


#define INVALID_TIMERQ_ID    (0xff)


static struct
{
    uint8_t id;
    struct
    {
        uint8_t idx;
    } get_platform;

    struct
    {
        uint8_t idx;
    } wait_endpoint_speech;

    struct
    {
        uint8_t idx;
    } adv_timeout;

} timer =
{
    .id = 0,
    .get_platform = {.idx = 0},
    .wait_endpoint_speech = {.idx = 0},
};


void app_ama_timer_start_get_platform_timer(void)
{
    app_start_timer(&timer.get_platform.idx, "get_platform",
                    timer.id, GET_PLATFORM_EVT, 0, false, 100);
}


void app_ama_timer_stop_get_platform_timer(void)
{
    app_stop_timer(&timer.get_platform.idx);
}


void app_ama_timer_timeout_cb(uint8_t evt, uint16_t param)
{
    APP_PRINT_TRACE2("app_ama_timer_timeout_cb: evt 0x%02X, param %d", evt,
                     param);
    switch (evt)
    {
    case WAIT_ENDPOINT_SPEECH_EVT:
        {
            app_ama_record_force_stop_recording();
        }
        break;

    case GET_PLATFORM_EVT:
        {

        }
        break;

    case ADV_TIMEOUT_EVT:
        {
            app_ama_ble_adv_stop();
        }
        break;

    default:
        break;
    }
}


void app_ama_timer_start_wait_speech_endpoint_timer(void)
{
    app_start_timer(&timer.wait_endpoint_speech.idx,
                    "wait_endpoint_speech",
                    timer.id, WAIT_ENDPOINT_SPEECH_EVT, 0, false, 30000);

}


void app_ama_timer_stop_wait_speech_endpoint_timer(void)
{
    app_stop_timer(&timer.wait_endpoint_speech.idx);
}


void app_ama_timer_start_le_adv_timer(uint32_t timeout)
{
    app_start_timer(&timer.adv_timeout.idx,
                    "adv_timeout", timer.id,
                    ADV_TIMEOUT_EVT, 0, false, timeout);
}


bool app_ama_timer_init(void)
{
    app_timer_reg_cb(app_ama_timer_timeout_cb, &timer.id);

    return true;
}


#endif
