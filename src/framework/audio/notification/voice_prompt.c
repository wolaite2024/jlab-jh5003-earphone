/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdbool.h>
#include <stdint.h>

#include "trace.h"
#include "notification.h"
#include "voice_prompt.h"

static T_VOICE_PROMPT_LANGUAGE_ID vp_language = VOICE_PROMPT_LANGUAGE_ENGLISH;

T_VOICE_PROMPT_MODE voice_prompt_mode_get(void)
{
    return (T_VOICE_PROMPT_MODE)notification_mode_get(NOTIFICATION_TYPE_VP);
}

bool voice_prompt_mode_set(T_VOICE_PROMPT_MODE mode)
{
    return notification_mode_set(NOTIFICATION_TYPE_VP, (uint8_t)mode);
}

bool voice_prompt_language_set(T_VOICE_PROMPT_LANGUAGE_ID language)
{
    if (language < VOICE_PROMPT_LANGUAGE_INVALID)
    {
        vp_language = language;
        return true;
    }

    return false;
}

T_VOICE_PROMPT_LANGUAGE_ID voice_prompt_language_get(void)
{
    return vp_language;
}

uint8_t voice_prompt_volume_max_get(void)
{
    return notification_volume_max_get(NOTIFICATION_TYPE_VP);
}

bool voice_prompt_volume_max_set(uint8_t volume)
{
    return notification_volume_max_set(NOTIFICATION_TYPE_VP, volume);
}

uint8_t voice_prompt_volume_min_get(void)
{
    return notification_volume_min_get(NOTIFICATION_TYPE_VP);
}

bool voice_prompt_volume_min_set(uint8_t volume)
{
    return notification_volume_min_set(NOTIFICATION_TYPE_VP, volume);
}

uint8_t voice_prompt_volume_get(void)
{
    return notification_volume_get(NOTIFICATION_TYPE_VP);
}

bool voice_prompt_volume_set(uint8_t volume)
{
    T_VOICE_PROMPT_MODE mode;

    mode = (T_VOICE_PROMPT_MODE)notification_mode_get(NOTIFICATION_TYPE_VP);
    if (mode == VOICE_PROMPT_MODE_AUDIBLE)
    {
        return notification_volume_set(NOTIFICATION_TYPE_VP, volume);
    }

    return false;
}

bool voice_prompt_volume_mute(void)
{
    T_VOICE_PROMPT_MODE mode;

    mode = (T_VOICE_PROMPT_MODE)notification_mode_get(NOTIFICATION_TYPE_VP);
    if (mode == VOICE_PROMPT_MODE_AUDIBLE ||
        mode == VOICE_PROMPT_MODE_FIXED)
    {
        return notification_volume_mute(NOTIFICATION_TYPE_VP);
    }

    return false;
}

bool voice_prompt_volume_unmute(void)
{
    T_VOICE_PROMPT_MODE mode;

    mode = (T_VOICE_PROMPT_MODE)notification_mode_get(NOTIFICATION_TYPE_VP);
    if (mode == VOICE_PROMPT_MODE_AUDIBLE ||
        mode == VOICE_PROMPT_MODE_FIXED)
    {
        return notification_volume_unmute(NOTIFICATION_TYPE_VP);
    }

    return false;
}

bool voice_prompt_play(uint8_t                    index,
                       T_VOICE_PROMPT_LANGUAGE_ID language,
                       bool                       relay)
{
    T_VOICE_PROMPT_MODE mode;

    mode = (T_VOICE_PROMPT_MODE)notification_mode_get(NOTIFICATION_TYPE_VP);

    AUDIO_PRINT_TRACE4("voice_prompt_play: mode %u, index 0x%02x, language %u, relay %u",
                       mode, index, language, relay);

    if (mode == VOICE_PROMPT_MODE_SILENT)
    {
        return false;
    }

    if (index == NOTIFICATION_INVALID_INDEX)
    {
        return false;
    }

    if (language >= VOICE_PROMPT_LANGUAGE_INVALID)
    {
        return false;
    }

    return notification_push(NOTIFICATION_TYPE_VP, index, language, NULL, relay);
}

float voice_prompt_volume_balance_get(void)
{
    return notification_volume_balance_get(NOTIFICATION_TYPE_VP);
}

bool voice_prompt_volume_balance_set(float scale)
{
    return notification_volume_balance_set(NOTIFICATION_TYPE_VP, scale);
}

bool voice_prompt_stop(void)
{
    return notification_stop(NOTIFICATION_TYPE_VP, NULL);
}

bool voice_prompt_cancel(uint8_t index,
                         bool    relay)
{
    return notification_cancel(NOTIFICATION_TYPE_VP, index, relay);
}

bool voice_prompt_flush(bool relay)
{
    return notification_flush(NOTIFICATION_TYPE_VP, relay);
}

uint8_t voice_prompt_remaining_count_get(void)
{
    return notification_remaining_count_get(NOTIFICATION_TYPE_VP);
}
