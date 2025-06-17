/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_XIAOAI_AT_H_
#define _APP_XIAOAI_AT_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef XM_XIAOAI_EXTEND_AT_CMD
typedef enum
{
    FAST_CONNECT_PROFILE,
    MIOT_PROFILE,
} T_XIAOAI_XM_AT_CMD_ADV_PROFILE_TYPE;

typedef enum
{
    DENOISE_OFF,
    DENOISE_ON,
    TRANSPARENCY,
} T_XIAOAI_XM_AT_CMD_DENOISE_TYPE;

typedef enum
{
    VOICE_ASSISTANT,
    PALY_PAUSE,
    DOUBLE_LAST_SONG,
    DOUBLE_NEXT_SONG,
    DOUBLE_VOLUME_UP,
    DOUBLE_VOLUME_DOWN,
    DENOISE_CONTROL,
    GAME_MODE,
} T_XIAOAI_XM_AT_CMD_DOUBLE_CLICK_FUNC;

/**
 * app_xiaoai_at.h
 *
 * \brief  Send extend AT command data.
 *
 * \param[in] bd_addr   the bt address of the device that receiving the AT command.
 *
 */
bool xiaoai_send_extend_at_cmd(uint8_t *bd_addr);

#endif // XM_XIAOAI_EXTEND_AT_CMD

/**
 * app_xiaoai_at.h
 *
 * \brief  Send connection AT command data.
 *
 * \param[in] bd_addr   the bt address of the device that receiving the AT command.
 *
 */
bool xiaoai_send_conn_at_cmd(uint8_t *bd_addr);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _APP_XIAOAI_AT_H_ */
