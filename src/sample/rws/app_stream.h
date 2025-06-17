/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_STREAM_H_
#define _APP_STREAM_H_
#include "audio_track.h"

typedef enum
{
    STREAM_TYPE_A2DP,
    STREAM_TYPE_VOICE,
    STREAM_TYPE_USB_AUDIO,
    STREAM_TYPE_SD_LOCAL,
} T_STREAM_TYPE;

/**
 * @brief   Set the Universally Unique IDentifier (UUID) of the specific Audio Track.
 *
 * @param  track_handle   The Audio Track session handle \ref T_AUDIO_TRACK_HANDLE.
 * @param  type     The stream type of the audio track \ref T_STREAM_TYPE.
 * @param  param    The parameters that need to set uuid
 *
 * @retval true     Audio Track UUID was get successfully.
 * @retval false    Audio Track UUID was failed to set.
*/
bool app_stream_set_audio_track_uuid(T_AUDIO_TRACK_HANDLE track_handle,
                                     T_STREAM_TYPE type, void *param);


#endif
