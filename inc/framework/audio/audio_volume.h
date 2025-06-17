/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_VOLUME_H_
#define _AUDIO_VOLUME_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_VOLUME Audio Volume
 *
 * \brief   Control Audio Stream volume level.
 * \details Applications can get the Audio Stream volume level range of each stream type, and
 *          adjust the specific Audio Stream volume level.
 */

/**
 * \brief   Get the maximum volume out level of the specific Audio Stream type.
 *
 * \param[in] type  The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The maximum volume out level of the specific Audio Stream type.
 *
 * \ingroup AUDIO_VOLUME
 */
uint8_t audio_volume_out_max_get(T_AUDIO_STREAM_TYPE type);

/**
 * \brief   Set the maximum volume out level of the specific Audio Stream type.
 *
 * \param[in] type      The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 * \param[in] volume    The maximum volume out level.
 *
 * \return          The status of setting the Audio Stream maximum volume out level.
 * \retval  true    The Audio Stream maximum volume out level was set successfully.
 * \retval  false   The Audio Stream maximum volume out level was failed to set.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_out_max_set(T_AUDIO_STREAM_TYPE type, uint8_t volume);

/**
 * \brief   Get the minimum volume out level of the specific Audio Stream type.
 *
 * \param[in] type  The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The minimum volume out level of the specific Audio Stream type.
 *
 * \ingroup AUDIO_VOLUME
 */
uint8_t audio_volume_out_min_get(T_AUDIO_STREAM_TYPE type);

/**
 * \brief   Set the minimum volume out level of the specific Audio Stream type.
 *
 * \param[in] type      The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 * \param[in] volume    The minimum volume out level.
 *
 * \return          The status of setting the Audio Stream minimum volume out level.
 * \retval  true    The Audio Stream minimum volume out level was set successfully.
 * \retval  false   The Audio Stream minimum volume out level was failed to set.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_out_min_set(T_AUDIO_STREAM_TYPE type, uint8_t volume);

/**
 * \brief   Get the current volume out level of the specific Audio Stream type.
 *
 * \param[in] type  The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The current volume out level of the specific Audio Stream type.
 *
 * \ingroup AUDIO_VOLUME
 */
uint8_t audio_volume_out_get(T_AUDIO_STREAM_TYPE type);

/**
 * \brief   Set the current volume out level of the specific Audio Stream type.
 *
 * \param[in] type      The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 * \param[in] volume    The volume out level of the Audio Stream type.
 *
 * \return          The status of setting the current Audio Stream volume out level.
 * \retval true     The current volume out level was set successfully.
 * \retval false    The current volume out level was failed to set.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_out_set(T_AUDIO_STREAM_TYPE type, uint8_t volume);

/**
 * \brief  Mute the current volume out level of the specific Audio Stream type.
 *
 * \param[in] type      The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return          The status of muting the current volume out level.
 * \retval  true    The current Audio Volume out level was muted successfully.
 * \retval  false   The current Audio Volume out level was failed to mute.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_out_mute(T_AUDIO_STREAM_TYPE type);

/**
 * \brief  Unmute the current volume out level of the specific Audio Stream type.
 *
 * \param[in] type      The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return          The status of unmuting the current volume out level.
 * \retval  true    The current Audio Volume out level was unmuted successfully.
 * \retval  false   The current Audio Volume out level was failed to unmute.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_out_unmute(T_AUDIO_STREAM_TYPE type);

/**
 * \brief   Get the current volume out level mute status of the specific Audio Stream type.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API"
 *
 * \return          The volume out level mute status of the Audio Stream type.
 * \retval true     The Audio Stream current volume out level was muted.
 * \retval false    The Audio Stream current volume out level was unmuted.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_out_is_muted(T_AUDIO_STREAM_TYPE type);

/**
 * \brief   Get the maximum volume in level of the specific Audio Stream type.
 *
 * \param[in] type  The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The maximum volume in level of the specific Audio Stream type.
 *
 * \ingroup AUDIO_VOLUME
 */
uint8_t audio_volume_in_max_get(T_AUDIO_STREAM_TYPE type);

/**
 * \brief   Set the maximum volume in level of the specific Audio Stream type.
 *
 * \param[in] type      The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 * \param[in] volume    The maximum volume in level.
 *
 * \return          The status of setting the Audio Stream maximum volume in level.
 * \retval  true    The Audio Stream maximum volume in level was set successfully.
 * \retval  false   The Audio Stream maximum volume in level was failed to set.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_in_max_set(T_AUDIO_STREAM_TYPE type, uint8_t volume);

/**
 * \brief   Get the minimum volume in level of the specific Audio Stream type.
 *
 * \param[in] type  The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The minimum volume in level of the specific Audio Stream type.
 *
 * \ingroup AUDIO_VOLUME
 */
uint8_t audio_volume_in_min_get(T_AUDIO_STREAM_TYPE type);

/**
 * \brief   Set the minimum volume in level of the specific Audio Stream type.
 *
 * \param[in] type      The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 * \param[in] volume    The minimum volume in level.
 *
 * \return          The status of setting the Audio Stream minimum volume in level.
 * \retval  true    The Audio Stream minimum volume in level was set successfully.
 * \retval  false   The Audio Stream minimum volume in level was failed to set.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_in_min_set(T_AUDIO_STREAM_TYPE type, uint8_t volume);

/**
 * \brief   Get the current volume in level of the specific Audio Stream type.
 *
 * \param[in] type  The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The current volume in level of the specific Audio Stream type.
 *
 * \ingroup AUDIO_VOLUME
 */
uint8_t audio_volume_in_get(T_AUDIO_STREAM_TYPE type);

/**
 * \brief   Set the current volume in level of the specific Audio Stream type.
 *
 * \param[in] type      The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 * \param[in] volume    The volume in level of the Audio Stream type.
 *
 * \return          The status of setting the current Audio Stream volume in level.
 * \retval true     The current volume in level was set successfully.
 * \retval false    The current volume in level was failed to set.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_in_set(T_AUDIO_STREAM_TYPE type, uint8_t volume);

/**
 * \brief  Mute the current volume in level of the specific Audio Stream type.
 *
 * \param[in] type     The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return          The status of muting the current volume in level.
 * \retval  true    The current Audio Volume in level was muted successfully.
 * \retval  false   The current Audio Volume in level was failed to mute.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_in_mute(T_AUDIO_STREAM_TYPE type);

/**
 * \brief  Unmute the current volume in level of the specific Audio Stream type.
 *
 * \param[in] type      The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return          The status of unmuting the current volume in level.
 * \retval  true    The current Audio Volume in level was unmuted successfully.
 * \retval  false   The current Audio Volume in level was failed to unmute.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_in_unmute(T_AUDIO_STREAM_TYPE type);

/**
 * \brief   Get the current volume in level mute status of the specific Audio Stream type.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API"
 *
 * \return          The volume in level mute status of the Audio Stream type.
 * \retval true     The Audio Stream current volume in level was muted.
 * \retval false    The Audio Stream current volume in level was unmuted.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_in_is_muted(T_AUDIO_STREAM_TYPE type);

/**
 * \brief   Get the volume balance scale of the specific audio stream type.
 *
 * \details Volume balance scale ranges from -1.0 to +1.0. If the volume balance scale
 *          is 0.0, the left channel volume and right channel volume are identical; if
 *          the volume balance scale ranges from +0.0 to +1.0, the right channel volume
 *          remains unchanged but the left channel volume scales down to (1.0 - scale)
 *          ratio; if the volume balance scale ranges from -1.0 to -0.0, the left channel
 *          volume remains unchanged but the right channel volume scales down to (1.0 + scale)
 *          ratio.
 *
 * \param[in] type  The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 *
 * \return  The volume balance scale of the specific Audio Stream type.
 *          The valid returned values are from -1.0 to +1.0.
 *
 * \ingroup AUDIO_VOLUME
 */
float audio_volume_balance_get(T_AUDIO_STREAM_TYPE type);

/**
 * \brief   Set the volume balance scale of the specific Audio Stream type.
 *
 * \details Volume balance scale ranges from -1.0 to +1.0. If the volume balance scale
 *          is 0.0, the left channel volume and right channel volume are identical; if
 *          the volume balance scale ranges from +0.0 to +1.0, the right channel volume
 *          remains unchanged but the left channel volume scales down to (1.0 - scale)
 *          ratio; if the volume balance scale ranges from -1.0 to -0.0, the left channel
 *          volume remains unchanged but the right channel volume scales down to (1.0 + scale)
 *          ratio.
 *
 * \param[in] type  The Audio Stream type \ref T_AUDIO_STREAM_TYPE.
 * \param[in] scale The volume balance scale ranges from -1.0 to +1.0.
 *
 * \return          The status of setting the Audio Stream volume balance scale.
 * \retval true     The volume balance scale was set successfully.
 * \retval false    The volume balance scale was failed to set.
 *
 * \ingroup AUDIO_VOLUME
 */
bool audio_volume_balance_set(T_AUDIO_STREAM_TYPE type, float scale);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_VOLUME_H_ */
