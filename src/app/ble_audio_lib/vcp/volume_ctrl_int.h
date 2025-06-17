#ifndef _VOL_CTRL_INT_H_
#define _VOL_CTRL_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ble_audio.h"

#if LE_AUDIO_VCS_CLIENT_SUPPORT
bool vcs_client_init(void);
#endif
#if LE_AUDIO_VOCS_CLIENT_SUPPORT
bool vocs_client_init(uint8_t cfg);
#endif
#if LE_AUDIO_MICS_CLIENT_SUPPORT
bool mics_client_init(void);
#endif
#if LE_AUDIO_AICS_CLIENT_SUPPORT
bool aics_client_init(uint8_t cfg);
#endif

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
