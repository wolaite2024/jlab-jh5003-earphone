/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _VAD_H_
#define _VAD_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_VAD Voice Activity Detection
 *
 * \brief   Enable and disable Voice Activity Detection.
 * \details Voice Activity detection (VAD) is a technique used in speech processing to detect the
 *          presence or absence of human speech. The main uses of VAD are in speech recognition and
 *          speech coding, e.g. hot words wakeup and voice recording. It can avoid unnecessary
 *          coding and transmission of silence packets to save on computation and network bandwidth.
 */

/**
 * \brief Define the supported VAD type.
 *
 * \ingroup AUDIO_VAD
 */
typedef enum t_vad_type
{
    VAD_TYPE_SW     = 0x00, /**< Software VAD engine type */
    VAD_TYPE_HW     = 0x01, /**< Hardware VAD engine type */
    VAD_TYPE_FUSION = 0x02, /**< Fusion VAD engine type */
} T_VAD_TYPE;

/**
 * \brief Define the VAD aggressiveness level.
 *
 * \ingroup AUDIO_VAD
 */
typedef enum t_vad_aggressiveness_level
{
    VAD_AGGRESSIVENESS_LEVEL_LOW     = 0x00,
    VAD_AGGRESSIVENESS_LEVEL_MEDIUM  = 0x01,
    VAD_AGGRESSIVENESS_LEVEL_HIGH    = 0x02,
} T_VAD_AGGRESSIVENESS_LEVEL;

/**
 * \brief Define the VAD filter level.
 *
 * \ingroup AUDIO_VAD
 */
typedef enum t_vad_filter_level
{
    VAD_FILTER_LEVEL_DEFAULT         = 0x00,
    VAD_FILTER_LEVEL_SILENT          = 0x01,
    VAD_FILTER_LEVEL_SUPPRESSED      = 0x02,
} T_VAD_FILTER_LEVEL;

/**
 * \brief   Enable the Voice Activity Detection.
 *
 * \param[in] type                   The VAD type \ref T_VAD_TYPE.
 * \param[in] aggressiveness_level   The aggressiveness level \ref T_VAD_AGGRESSIVENESS_LEVEL.
 * \param[in] format_info            The encoding format information \ref T_AUDIO_FORMAT_INFO.
 *
 * \note Voice Activity Detection is a singleton design that the application shall destroy the
 *       instance before creating a new instance.
 *
 * \return          The status of enabling Voice Activity Detection.
 * \retval  true    The Voice Activity Detection was enabled successfully.
 * \retval  false   The Voice Activity Detection was failed to enable.
 *
 * \ingroup AUDIO_VAD
 */
bool vad_enable(T_VAD_TYPE                 type,
                T_VAD_AGGRESSIVENESS_LEVEL aggressiveness_level,
                T_AUDIO_FORMAT_INFO        format_info);

/**
 * \brief   Filter Voice Activity Detection.
 *
 * \param[in] level   The filter level \ref T_VAD_FILTER_LEVEL.
 *
 * \return          The status of filtering Voice Activity Detection.
 * \retval  true    The Voice Activity Detection was filtered successfully.
 * \retval  false   The Voice Activity Detection was failed to filter.
 *
 * \ingroup AUDIO_VAD
 */
bool vad_filter(T_VAD_FILTER_LEVEL level);

/**
 * \brief   Disable Voice Activity Detection.
 *
 * \return          The status of disabling Voice Activity Detection.
 * \retval  true    The Voice Activity Detection was disabled successfully.
 * \retval  false   The Voice Activity Detection was failed to disable.
 *
 * \ingroup AUDIO_VAD
 */
bool vad_disable(void);

/**
 * \brief   Attach the Audio Effect instance to the Voice Activity Detection stream.
 *
 * \param[in] instance  The Audio Effect instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of attaching the Audio Effect instance.
 * \retval  true    The Audio Effect instance was attached successfully.
 * \retval  false   The Audio Effect instance was failed to attach.
 *
 * \ingroup AUDIO_KWS
 */
bool vad_effect_attach(T_AUDIO_EFFECT_INSTANCE instance);

/**
 * \brief   Detach the Audio Effect instance from the Voice Activity Detection stream.
 *
 * \param[in] instance  The Audio Effect instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of detaching the Audio Effect instance.
 * \retval  true    The Audio Effect instance was detached successfully.
 * \retval  false   The Audio Effect instance was failed to detach.
 *
 * \ingroup AUDIO_KWS
 */
bool vad_effect_detach(T_AUDIO_EFFECT_INSTANCE instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _VAD_H_ */
