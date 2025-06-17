#ifndef _OTP_CLIENT_INT_H_
#define _OTP_CLIENT_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ots_client.h"

#define LE_AUDIO_MSG_OTS_CLIENT_DIS_DONE      (LE_AUDIO_MSG_GROUP_OTP_CLIENT |0x01)
#define LE_AUDIO_MSG_OTS_CLIENT_READ_RES      (LE_AUDIO_MSG_GROUP_OTP_CLIENT |0x02)
#define LE_AUDIO_MSG_OTS_CLIENT_OLCP_INFO     (LE_AUDIO_MSG_GROUP_OTP_CLIENT |0x03)

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
