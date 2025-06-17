/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _KWS_H_
#define _KWS_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_KWS Keyword Spotting
 *
 * \brief   Enable and disable keyword spotting.
 * \details Keyword Spotting (KWS) is a technique used in speech processing to detect
 *          the presence or absence of human speech. The main uses of KWS are in speech recognition
 *          and speech coding, e.g. hot words wakeup and voice recording. It can avoid unnecessary
 *          coding and transmission of silence packets to save on computation and network bandwidth.
 */

/**
 * \brief   Define the supported KWS effect content type.
 *
 * \ingroup AUDIO_KWS
 */
typedef enum t_kws_effect_content_type
{
    KWS_EFFECT_CONTENT_TYPE_VOICE  = 0x00,
    KWS_EFFECT_CONTENT_TYPE_RECORD = 0x01,
    KWS_EFFECT_CONTENT_TYPE_VAD    = 0x02,
} T_KWS_EFFECT_CONTENT_TYPE;

/**
 * \brief   Define the KWS event.
 *
 * \ingroup AUDIO_KWS
 */
typedef enum t_kws_event
{
    KWS_EVENT_RELEASED       = 0x0,
    KWS_EVENT_CREATED        = 0x1,
    KWS_EVENT_STARTED        = 0x2,
    KWS_EVENT_STOPPED        = 0x3,
    KWS_EVENT_DATA_IND       = 0x4,
    KWS_EVENT_DATA_FILLED    = 0x5,
} T_KWS_EVENT;

/**
 * \brief Define the KWS session handle.
 *
 * \ingroup AUDIO_KWS
 */
typedef void *T_KWS_HANDLE;

/**
 * \brief Define the KWS session cback.
 *
 * \ingroup AUDIO_KWS
 */
typedef bool (*T_KWS_CBACK)(T_KWS_HANDLE handle,
                            T_KWS_EVENT  event,
                            uint32_t     param);

/**
 * \brief   Create a KWS effect instance.
 *
 * \param[in] type KWS content type \ref T_KWS_EFFECT_CONTENT_TYPE.
 *
 * \return  The instance of the KWS engine \ref T_AUDIO_EFFECT_INSTANCE. If the
 *          returned instance is NULL, the KWS instance was failed to create.
 *
 * \ingroup AUDIO_KWS
 */
T_AUDIO_EFFECT_INSTANCE kws_effect_create(T_KWS_EFFECT_CONTENT_TYPE type);

/**
 * \brief   Enable the specific KWS effect instance.
 *
 * \param[in] instance  KWS effect instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return          The status of enabling the KWS effect instance.
 * \retval  true    KWS effect instance was enabled successfully.
 * \retval  false   KWS effect instance was failed to enable.
 *
 * \ingroup AUDIO_KWS
 */
bool kws_effect_enable(T_AUDIO_EFFECT_INSTANCE instance);

/**
 * \brief   Disable the specific KWS effect instance.
 *
 * \param[in] instance  KWS effect instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return          The status of disabling the KWS effect instance.
 * \retval  true    KWS effect instance was disabled successfully.
 * \retval  false   KWS effect instance was failed to disable.
 *
 * \ingroup AUDIO_KWS
 */
bool kws_effect_disable(T_AUDIO_EFFECT_INSTANCE instance);

/**
 * \brief   Release the specific KWS effect instance.
 *
 * \param[in] instancE  KWS effect instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of releasing the KWS effect instance.
 * \retval  true    KWS effect instance was released successfully.
 * \retval  false   KWS effect instance was failed to release.
 *
 * \ingroup AUDIO_KWS
 */
bool kws_effect_release(T_AUDIO_EFFECT_INSTANCE instance);


/**
 * \brief Create a KWS session node.
 *
 * \param[in] format_info The decoding format information \ref T_AUDIO_FORMAT_INFO.
 * \param[in] cback       KWS session cback \ref T_KWS_CBACK.
 *
 * \return  The instance handle of KWS node. If returned handle is NULL, the KWS
 *          node instance was failed to create.
 *
 * \ingroup AUDIO_KWS
 */
T_KWS_HANDLE kws_create(T_AUDIO_FORMAT_INFO format_info,
                        T_KWS_CBACK         cback);

/**
 * \brief   Start keyword spotting.
 *
 * \param[in] handle KWS session handle \ref T_KWS_HANDLE.
 *
 * \note Keyword spotting is a singleton design that the application shall destroy the
 *       instance before creating a new instance.
 *
 * \return          The status of enabling keyword spotting.
 * \retval  true    Keyword spotting was started successfully.
 * \retval  false   Keyword spotting was failed to start.
 *
 * \ingroup AUDIO_KWS
 */
bool kws_start(T_KWS_HANDLE handle);

/**
 * \brief   Stop keyword spotting.
 *
 * \param[in] handle KWS session handle \ref T_KWS_HANDLE.
 *
 * \return          The status of stopping keyword spotting.
 * \retval  true    Keyword spotting was stopped successfully.
 * \retval  false   Keyword spotting was failed to stop.
 *
 * \ingroup AUDIO_KWS
 */
bool kws_stop(T_KWS_HANDLE handle);

/**
 * \brief   Release keyword spotting.
 *
 * \param[in] handle KWS session handle \ref T_KWS_HANDLE.
 *
 * \return          The status of disabling keyword spotting.
 * \retval  true    Keyword spotting was released successfully.
 * \retval  false   Keyword spotting was failed to release.
 *
 * \ingroup AUDIO_KWS
 */
bool kws_release(T_KWS_HANDLE handle);

/**
 * \brief   Fill the encoded data for keyword spotting.
 *
 * \param[in] handle      KWS session handle \ref T_KWS_HANDLE.
 * \param[in] timestamp   The timestamp of the buffer passed into KWS session.
 * \param[in] seq_num     The sequence number of the buffer passed into KWS session.
 * \param[in] status      The frame status of the buffer passed into KWS session.
 * \param[in] frame_num   The frame number of the buffer passed into KWS session.
 * \param[in] buf         The buffer that holds the filled data.
 * \param[in] len         The buffer length.
 *
 * \return          The status of filling for keyword spotting.
 * \retval  true    The data was filled successfully.
 * \retval  false   The data was failed to fill.
 *
 * \ingroup AUDIO_KWS
 */
bool kws_fill(T_KWS_HANDLE           handle,
              uint32_t               timestamp,
              uint16_t               seq_num,
              T_AUDIO_STREAM_STATUS  status,
              uint8_t                frame_num,
              void                  *buf,
              uint16_t               len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _KWS_H_ */
