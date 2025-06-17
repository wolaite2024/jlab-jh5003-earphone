#ifndef _APP_GFPS_TIMER_H_
#define _APP_GFPS_TIMER_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * @brief app_gfps_timer_init
 *
 */
void app_gfps_timer_init(void);

/**
 * @brief start update rpa timer
 *
 */
void app_gfps_timer_start_update_rpa(void);

/**
 * @brief stop update rpa timer
 *
 */
void app_gfps_timer_stop_update_rpa(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
