/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _TTS_H_
#define _TTS_H_

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_TTS Text-to-Speech
 *
 * \brief   Create, control and destroy the Text-to-Speech.
 * \details Text-to-Speech (TTS) synthesizes speech from normal language text for playback or
 *          to create voice files.
 */

/**
 * \brief The TTS session handle definition.
 *
 * \ingroup AUDIO_TTS
 */
typedef void *T_TTS_HANDLE;

/**
 * \brief   Get the maximum volume level of the TTS.
 *
 * \return  The maximum volume level of the TTS.
 *
 * \ingroup AUDIO_TTS
 */
uint8_t tts_volume_max_get(void);

/**
 * \brief   Set the maximum volume level of the TTS.
 *
 * \param[in] volume    The maximum volume level of the TTS.
 *
 * \return          The status of setting the TTS maximum volume level.
 * \retval  true    The TTS maximum volume level was set successfully.
 * \retval  false   The TTS maximum volume level was failed to set.
 *
 * \ingroup AUDIO_TTS
 */
bool tts_volume_max_set(uint8_t volume);

/**
 * \brief   Get the minimum volume level of the TTS.
 *
 * \return  The minimum volume level of the TTS.
 *
 * \ingroup AUDIO_TTS
 */
uint8_t tts_volume_min_get(void);

/**
 * \brief   Set the minimum volume level of the TTS.
 *
 * \param[in] volume    The minimum volume level of the TTS.
 *
 * \return          The status of setting the TTS minimum volume level.
 * \retval  true    The TTS minimum volume level was set successfully.
 * \retval  false   The TTS minimum volume level was failed to set.
 *
 * \ingroup AUDIO_TTS
 */
bool tts_volume_min_set(uint8_t volume);

/**
 * \brief   Get the current volume level of the TTS.
 *
 * \return  The current volume level of the TTS.
 *
 * \ingroup AUDIO_TTS
 */
uint8_t tts_volume_get(void);

/**
 * \brief   Set the current volume level of the TTS.
 *
 * \details If the TTS volume was muted previously, setting the volume at any level excluding
 *          the minimum level \ref tts_volume_min_get() will unmute and activate the volume level.
 *          When the TTS volume was already muted, setting the volume at the minimum level will
 *          have no effect.
 *
 * \param[in] volume    The volume level to set. See \ref tts_volume_max_get() and \ref
 *                      tts_volume_min_get() for the valid volume level range.
 *
 * \return          The status of setting the TTS current volume level.
 * \retval  true    The TTS current volume level was set successfully.
 * \retval  false   The TTS current volume level was failed to set.
 *
 * \ingroup AUDIO_TTS
 */
bool tts_volume_set(uint8_t volume);

/**
 * \brief   Mute the current volume level of the TTS.
 *
 * \return          The status of muting the TTS volume level.
 * \retval true     The TTS volume level was muted successfully.
 * \retval false    The TTS volume level was failed to mute.
 *
 * \ingroup AUDIO_TTS
 */
bool tts_volume_mute(void);

/**
 * \brief   Unmute the current volume level of the TTS.
 *
 * \return          The status of unmuting the TTS volume level.
 * \retval true     The TTS volume level was unmuted successfully.
 * \retval false    The TTS volume level was failed to unmute.
 *
 * \ingroup AUDIO_TTS
 */
bool tts_volume_unmute(void);

/**
 * \brief   Get the volume balance scale of the TTS.
 *
 * \details Volume balance scale ranges from -1.0 to +1.0. If the volume balance scale
 *          is 0.0, the left channel volume and right channel volume are identical; if
 *          the volume balance scale ranges from +0.0 to +1.0, the right channel volume
 *          remains unchanged but the left channel volume scales down to (1.0 - scale)
 *          ratio; if the volume balance scale ranges from -1.0 to -0.0, the left channel
 *          volume remains unchanged but the right channel volume scales down to (1.0 + scale)
 *          ratio.
 *
 * \return  The volume balance scale of the TTS.
 *          The valid returned values are from -1.0 to +1.0.
 *
 * \ingroup AUDIO_TTS
 */
float tts_volume_balance_get(void);

/**
 * \brief   Set the volume balance scale of the TTS.
 *
 * \details Volume balance scale ranges from -1.0 to +1.0. If the volume balance scale
 *          is 0.0, the left channel volume and right channel volume are identical; if
 *          the volume balance scale ranges from +0.0 to +1.0, the right channel volume
 *          remains unchanged but the left channel volume scales down to (1.0 - scale)
 *          ratio; if the volume balance scale ranges from -1.0 to -0.0, the left channel
 *          volume remains unchanged but the right channel volume scales down to (1.0 + scale)
 *          ratio.
 *
 * \param[in] scale The volume balance scale ranges from -1.0 to +1.0.
 *
 * \return          The status of setting the TTS volume balance scale.
 * \retval true     The volume balance scale was set successfully.
 * \retval false    The volume balance scale was failed to set.
 *
 * \ingroup AUDIO_TTS
 */
bool tts_volume_balance_set(float scale);

/**
 * \brief   Create a TTS session instance.
 *
 * \param[in] len       The length of TTS session.
 * \param[in] frame_cnt The frame count of TTS session.
 * \param[in] cfg       The configure of TTS session.
 *
 * \return     The instance handle of TTS session. If returned handle is NULL,
 *             the TTS session instance was failed to create.
 *
 * \ingroup AUDIO_TTS
 */
T_TTS_HANDLE tts_create(uint32_t len, uint16_t frame_cnt, uint32_t cfg);

/**
 * \brief   Start the TTS session.
 *
 * \param[in] handle    The specific TTS session handle.
 * \param[in] relay     Relay the TTS to other remote identical devices.
 *
 * \return          The status of starting TTS session.
 * \retval true     The TTS session was started successfully.
 * \retval false    The TTS session was failed to start.
 *
 * \ingroup AUDIO_TTS
 */
bool tts_start(T_TTS_HANDLE handle, bool relay);

/**
 * \brief   Play a TTS session frame.
 *
 * \param[in] handle    The specific TTS session handle.
 * \param[in] buf       The Pointer to the TTS session frame.
 * \param[in] len       The length of the TTS session frame.
 *
 * \return          The status of playing a TTS session frame.
 * \retval true     The TTS session frame was played successfully.
 * \retval false    The TTS session frame was failed to play.
 *
 * \ingroup AUDIO_TTS
 */
bool tts_play(T_TTS_HANDLE handle, void *buf, uint16_t len);

/**
 * \brief   Stop the TTS session.
 *
 * \param[in] handle    The specific TTS session handle.
 *
 * \return          The status of stopping the TTS session.
 * \retval true     The TTS session was stopped successfully.
 * \retval false    The TTS session was failed to stop.
 *
 * \ingroup AUDIO_TTS
 */
bool tts_stop(T_TTS_HANDLE handle);

/**
 * \brief   Destroy the TTS session instance.
 *
 * \param[in] handle    The specific TTS session handle.
 *
 * \return          The status of destroying the TTS session.
 * \retval true     The TTS session was destroyed successfully.
 * \retval false    The TTS session was failed to destroy.
 *
 * \ingroup AUDIO_TTS
 */
bool tts_destroy(T_TTS_HANDLE handle);

/**
 * \brief   Return the count remaining TTSs.
 *
 * \return  The count of remaining TTSs.
 *
 * \ingroup AUDIO_TTS
 */
uint8_t tts_remaining_count_get(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _TTS_H_ */
