/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <string.h>
#include "trace.h"
#include "app_stream.h"

bool app_stream_set_audio_track_uuid(T_AUDIO_TRACK_HANDLE track_handle, T_STREAM_TYPE type,
                                     void *param)
{
    uint8_t  uuid[8] = {0};
    switch (type)
    {
    case STREAM_TYPE_A2DP:
    case STREAM_TYPE_VOICE:
        {
            uuid[0] = 'B';
            uuid[1] = 'T';
            memcpy(&uuid[2], (uint8_t *)param, 6);
        }
        break;
    case STREAM_TYPE_USB_AUDIO:
        {
            uuid[0] = 'U';
            uuid[1] = 'S';
            uuid[2] = 'B';
        }
        break;
    case STREAM_TYPE_SD_LOCAL:
        {
            uuid[0] = 'S';
            uuid[1] = 'D';
        }
        break;
    default:
        break;
    }

    return audio_track_uuid_set(track_handle, uuid);
}
