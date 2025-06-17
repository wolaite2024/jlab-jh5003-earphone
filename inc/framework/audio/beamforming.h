/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _BEAMFORMING_H_
#define _BEAMFORMING_H_

#include <stdbool.h>

#include "audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup AUDIO_BEAMFORMING Beamforming
 *
 * \brief   Enable and disable Beamforming effect.
 * \details Beamforming technique in the acoustic field is applied to enhance signals from the
 *          desired directions or angle, and suppress noises from other directions.
 */

/**
 * \brief   Define the supported Beamforming content type.
 *
 * \ingroup AUDIO_BEAMFORMING
 */
typedef enum t_beamforming_content_type
{
    BEAMFORMING_CONTENT_TYPE_VOICE       = 0x00,
    BEAMFORMING_CONTENT_TYPE_RECORD      = 0x01,
    BEAMFORMING_CONTENT_TYPE_PASSTHROUGH = 0x02,
} T_BEAMFORMING_CONTENT_TYPE;

/**
 * \brief   Define Beamforming direction type.
 *
 * \ingroup AUDIO_BEAMFORMING
 */
typedef enum t_beamforming_direction
{
    BEAMFORMING_DIRECTION_FRONT       = 0x00,
    BEAMFORMING_DIRECTION_LEFT        = 0x01,
    BEAMFORMING_DIRECTION_RIGHT       = 0x02,
    BEAMFORMING_DIRECTION_BACK        = 0x03,
    BEAMFORMING_DIRECTION_ADAPTIVE    = 0x04,
} T_BEAMFORMING_DIRECTION;

/**
 * \brief   Create a Beamforming instance.
 *
 * \param[in] type       Beamforming content type \ref T_BEAMFORMING_CONTENT_TYPE.
 * \param[in] direction  Beamforming direction type \ref T_BEAMFORMING_DIRECTION.
 *
 * \return  The instance of the Beamforming engine \ref T_AUDIO_EFFECT_INSTANCE. If the
 *          returned instance is NULL, the Beamforming instance was failed to create.
 *
 * \ingroup AUDIO_BEAMFORMING
 */
T_AUDIO_EFFECT_INSTANCE beamforming_create(T_BEAMFORMING_CONTENT_TYPE type,
                                           T_BEAMFORMING_DIRECTION    direction);

/**
 * \brief   Enable the specific Beamforming instance.
 *
 * \param[in] instance  Beamforming instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return          The status of enabling the Beamforming instance.
 * \retval  true    Beamforming instance was enabled successfully.
 * \retval  false   Beamforming instance was failed to enable.
 *
 * \ingroup AUDIO_BEAMFORMING
 */
bool beamforming_enable(T_AUDIO_EFFECT_INSTANCE instance);

/**
 * \brief   Disable the specific Beamforming instance.
 *
 * \param[in] instance  Beamforming instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return          The status of disabling the Beamforming instance.
 * \retval  true    Beamforming instance was disabled successfully.
 * \retval  false   Beamforming instance was failed to disable.
 *
 * \ingroup AUDIO_BEAMFORMING
 */
bool beamforming_disable(T_AUDIO_EFFECT_INSTANCE instance);

/**
 * \brief   Release the specific Beamforming instance.
 *
 * \param[in] instance  Beamforming instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of releasing the Beamforming instance.
 * \retval  true    Beamforming instance was released successfully.
 * \retval  false   Beamforming instance was failed to release.
 *
 * \ingroup AUDIO_BEAMFORMING
 */
bool beamforming_release(T_AUDIO_EFFECT_INSTANCE instance);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BEAMFORMING_H_ */
