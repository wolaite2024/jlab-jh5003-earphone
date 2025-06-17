#ifndef _APP_AMA_TIMER_H_
#define _APP_AMA_TIMER_H_


#include <stdint.h>
#include <stdbool.h>

bool app_ama_timer_init(void);


void app_ama_timer_start_get_platform_timer(void);
void app_ama_timer_stop_get_platform_timer(void);
void app_ama_timer_start_wait_speech_endpoint_timer(void);
void app_ama_timer_stop_wait_speech_endpoint_timer(void);
void app_ama_timer_start_le_adv_timer(uint32_t timeout);

#endif

