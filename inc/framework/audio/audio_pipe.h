/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_PIPE_H_
#define _AUDIO_PIPE_H_

#include <stdbool.h>
#include <stdint.h>

#include "audio_type.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_PIPE Audio Pipe
 *
 * \brief   Create, control and destroy Audio Pipe sessions.
 * \details
 */

/**
 * \brief Define the Audio Pipe session handle.
 *
 * \ingroup AUDIO_PIPE
 */
typedef void *T_AUDIO_PIPE_HANDLE;

/**
 * \brief Define the Audio Pipe callback event.
 *
 * \ingroup AUDIO_PIPE
 */
typedef enum t_audio_pipe_event
{
    AUDIO_PIPE_EVENT_RELEASED       = 0x0,
    AUDIO_PIPE_EVENT_CREATED        = 0x1,
    AUDIO_PIPE_EVENT_STARTED        = 0x2,
    AUDIO_PIPE_EVENT_STOPPED        = 0x3,
    AUDIO_PIPE_EVENT_DATA_IND       = 0x4,
    AUDIO_PIPE_EVENT_DATA_FILLED    = 0x5,
    AUDIO_PIPE_EVENT_MIXED          = 0x6,
    AUDIO_PIPE_EVENT_DEMIXED        = 0x7,
} T_AUDIO_PIPE_EVENT;

/**
 * \brief Define the Audio Pipe callback prototype.
 *
 * \param[in]   handle  Audio Pipe handle \ref T_AUDIO_PIPE_HANDLE.
 * \param[in]   event   Audio Pipe callback event.
 * \param[in]   param   Audio Pipe callback event parameter.
 *
 * \return  The result of the callback function.
 *
 * \ingroup AUDIO_PIPE
 */
typedef bool (*T_AUDIO_PIPE_CBACK)(T_AUDIO_PIPE_HANDLE handle,
                                   T_AUDIO_PIPE_EVENT  event,
                                   uint32_t            param);

/**
 * \brief Create an Audio Pipe session.
 *
 * \param[in]   mode        Audio Pipe stream mode \ref T_AUDIO_STREAM_MODE.
 * \param[in]   src_info    Audio Pipe source endpoint format info \ref T_AUDIO_FORMAT_INFO.
 * \param[in]   snk_info    Audio Pipe sink endpoint format info \ref T_AUDIO_FORMAT_INFO.
 * \param[in]   gain        The gain value (dB) of the Audio Pipe.
 * \param[in]   cback       Audio Pipe callback \ref T_AUDIO_PIPE_CBACK.
 *
 * \return  The session handle of Audio Pipe. If returned handle is NULL, the Audio Pipe session
 *          was failed to create.
 *
 * \ingroup AUDIO_PIPE
 */
T_AUDIO_PIPE_HANDLE audio_pipe_create(T_AUDIO_STREAM_MODE mode,
                                      T_AUDIO_FORMAT_INFO src_info,
                                      T_AUDIO_FORMAT_INFO snk_info,
                                      int16_t             gain,
                                      T_AUDIO_PIPE_CBACK  cback);

/**
 * \brief Release the Audio Pipe session.
 *
 * \param[in]   handle  Audio Pipe session handle \ref T_AUDIO_PIPE_HANDLE.
 *
 * \return          The result of releasing the Audio Pipe session.
 * \retval true     Audio Pipe session was released successfully.
 * \retval false    Audio Pipe session was failed to release.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_release(T_AUDIO_PIPE_HANDLE handle);

/**
 * \brief Start the Audio Pipe session.
 *
 * \param[in]   handle  Audio Pipe session handle \ref T_AUDIO_PIPE_HANDLE.
 *
 * \return          The result of starting the Audio Pipe session.
 * \retval true     Audio Pipe session was started successfully.
 * \retval false    Audio Pipe session was failed to start.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_start(T_AUDIO_PIPE_HANDLE handle);

/**
 * \brief Stop the Audio Pipe session.
 *
 * \param[in]   handle  Audio Pipe session handle \ref T_AUDIO_PIPE_HANDLE.
 *
 * \return          The result of stopping the Audio Pipe session.
 * \retval true     Audio Pipe session was stopped successfully.
 * \retval false    Audio Pipe session was failed to stop.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_stop(T_AUDIO_PIPE_HANDLE handle);

/**
 * \brief   Get the source endpoint and sink endpoint format information of the specific Audio Pipe session.
 *
 * \param[in]  handle       The Audio Pipe session handle \ref T_AUDIO_PIPE_HANDLE.
 * \param[out] src_info     The source endpoint format information \ref T_AUDIO_FORMAT_TYPE of the Audio Pipe session.
 * \param[out] src_info     The sink endpoint format information \ref T_AUDIO_FORMAT_TYPE of the Audio Pipe session.
 *
 * \return          The status of getting the Audio Pipe source endpoint and sink endpoint format information.
 * \retval true     Audio Pipe source endpoint and sink endpoint format information was got successfully.
 * \retval false    Audio Pipe source endpoint and sink endpoint format information was failed to get.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_format_info_get(T_AUDIO_PIPE_HANDLE  handle,
                                T_AUDIO_FORMAT_INFO *src_info,
                                T_AUDIO_FORMAT_INFO *snk_info);

/**
 * \brief Get the current gain value of the specific Audio Pipe session.
 *
 * \param[in]   handle      Audio Pipe session handle \ref T_AUDIO_PIPE_HANDLE.
 * \param[out]  gain_left   The left channel gain value of the Audio Pipe session.
 * \param[out]  gain_right  The right channel gain value of the Audio Pipe session.
 *
 * \return          The result of getting the gain value.
 * \retval true     Gain value was got successfully.
 * \retval false    Gain value was failed to get.
 *
 * \note            Gain value is in step unit (dB * 2^7).
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_gain_get(T_AUDIO_PIPE_HANDLE  handle,
                         int16_t             *gain_left,
                         int16_t             *gain_right);

/**
 * \brief Set the current gain value of the specific Audio Pipe session.
 *
 * \param[in]   handle      Audio Pipe session handle \ref T_AUDIO_PIPE_HANDLE.
 * \param[in]   gain_left   The left channel gain value of the Audio Pipe session.
 * \param[in]   gain_right  The right channel gain value of the Audio Pipe session.
 *
 * \return          The result of setting the gain value.
 * \retval true     Gain value was set successfully.
 * \retval false    Gain value was failed to set.
 *
 * \note            Gain value is in step unit (dB * 2^7).
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_gain_set(T_AUDIO_PIPE_HANDLE handle,
                         int16_t             gain_left,
                         int16_t             gain_right);

/**
 * \brief  Reorder the channel out streaming layout of the specific Audio Pipe session.
 *
 * \param[in] handle           The Audio Pipe session handle \ref T_AUDIO_PIPE_HANDLE.
 * \param[in] channel_num      The channel out number.
 * \param[in] channel_array    The channel out array.
 *
 * \note    The channel out number shall not be 0, and each element in the corresponding
 *          channel out array table is a 4-octet bitmask.
 *
 * \return          The result of reordering the Audio Pipe channel out streaming layout.
 * \retval  true    Audio Pipe channel out streaming layout was reordering successfully.
 * \retval  false   Audio Pipe channel out streaming layout was failed to reorder.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_channel_out_set(T_AUDIO_PIPE_HANDLE handle,
                                uint8_t             channel_num,
                                uint32_t            channel_array[]);

/**
 * \brief Fill data into the Audio Pipe source endpoint.
 *
 * \param[in]   handle       Audio Pipe handle \ref T_AUDIO_PIPE_HANDLE.
 * \param[in]   timestamp    The timestamp of the buffer passed into Audio Pipe.
 * \param[in]   seq_num      The sequence number of the buffer passed into Audio Pipe.
 * \param[in]   status       The frame status of the buffer passed into Audio Pipe.
 * \param[in]   frame_num    The frame number of the buffer passed into Audio Pipe.
 * \param[in]   buf          The buffer that holds the filled data.
 * \param[in]   len          The buffer length.
 *
 * \return          The result of filling the audio data.
 * \retval true     Audio data was filled successfully.
 * \retval false    Audio data was failed to fill.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_fill(T_AUDIO_PIPE_HANDLE    handle,
                     uint32_t               timestamp,
                     uint16_t               seq_num,
                     T_AUDIO_STREAM_STATUS  status,
                     uint8_t                frame_num,
                     void                  *buf,
                     uint16_t               len);

/**
 * \brief Drain data from the Audio Pipe sink endpoint.
 *
 * \param[in]   handle      Audio Pipe handle \ref T_AUDIO_PIPE_HANDLE.
 * \param[out]  timestamp   The timestamp of the buffer passed out from Audio Subsystem.
 * \param[out]  seq_num     The sequence number of the buffer passed out from Audio Subsystem.
 * \param[out]  status      The frame status of the buffer passed out from Audio Subsystem.
 * \param[out]  frame_num   The frame number of the buffer passed out from Audio Subsystem.
 * \param[out]  buf         The buffer that holds the drained data.
 * \param[out]  len         The actual size in bytes drained from the Audio Pipe.
 *
 * \return          The result of draining the audio data.
 * \retval true     Audio data was drained successfully.
 * \retval false    Audio data was failed to drain.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_drain(T_AUDIO_PIPE_HANDLE    handle,
                      uint32_t              *timestamp,
                      uint16_t              *seq_num,
                      T_AUDIO_STREAM_STATUS *status,
                      uint8_t               *frame_num,
                      void                  *buf,
                      uint16_t              *len);

/**
 * \brief Add the auxiliary codec pipe session to the Pre-Mixer point of the prime codec pipe session.
 *
 * \param[in]   prime_handle      The prime Audio Pipe handle \ref T_AUDIO_PIPE_HANDLE.
 * \param[out]  auxiliary_handle  The auxiliary Audio Pipe handle \ref T_AUDIO_PIPE_HANDLE.
 *
 * \return          The result of the mixing operation.
 * \retval true     The two pipes were mixed successfully.
 * \retval false    The two pipes were failed to mix.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_pre_mix(T_AUDIO_PIPE_HANDLE prime_handle,
                        T_AUDIO_PIPE_HANDLE auxiliary_handle);

/**
 * \brief Add the auxiliary codec pipe session to the Post-Mixer point of the prime codec pipe session.
 *
 * \param[in]   prime_handle      The prime Audio Pipe handle \ref T_AUDIO_PIPE_HANDLE.
 * \param[out]  auxiliary_handle  The auxiliary Audio Pipe handle \ref T_AUDIO_PIPE_HANDLE.
 *
 * \return          The result of the mixing operation.
 * \retval true     The two pipes were mixed successfully.
 * \retval false    The two pipes were failed to mix.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_post_mix(T_AUDIO_PIPE_HANDLE prime_handle,
                         T_AUDIO_PIPE_HANDLE auxiliary_handle);

/**
 * \brief Remove the auxiliary codec pipe session from the Pre-Mixer point or the Post-Mixer point
 *        of the prime codec pipe session.
 *
 * \param[in]   prime_handle      The prime Audio Pipe handle \ref T_AUDIO_PIPE_HANDLE.
 * \param[out]  auxiliary_handle  The auxiliary Audio Pipe handle \ref T_AUDIO_PIPE_HANDLE.
 *
 * \return          The result of the demixing operation.
 * \retval true     The two pipes were demixed successfully.
 * \retval false    The two pipes were failed to demix.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_demix(T_AUDIO_PIPE_HANDLE prime_handle,
                      T_AUDIO_PIPE_HANDLE auxiliary_handle);

/**
 * \brief Toggle the Asynchronous Sample Rate Converter (ASRC) function of the specific Audio Pipe session.
 *
 * \param[in]   handle     Audio Pipe session handle \ref T_AUDIO_PIPE_HANDLE.
 * \param[in]   toggle     Enable or disable ASRC function.
 * \arg \c      0          Disable (default action).
 * \arg \c      1          Enable.
 *
 * \return          The result of toggle the ASRC function.
 * \retval true     The ASRC function toggle was set successfully.
 * \retval false    The ASRC function toggle was failed to set.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_asrc_toggle(T_AUDIO_PIPE_HANDLE handle,
                            uint8_t             toggle);

/**
 * \brief Set the current Asynchronous Sample Rate Converter (ASRC) ratio of the specific Audio Pipe session.
 *
 * \param[in]   handle     Audio Pipe session handle \ref T_AUDIO_PIPE_HANDLE.
 * \param[in]   ratio      ASRC ratio in ppm of the Audio Pipe session.
 *
 * \return          The result of setting the ASRC ratio.
 * \retval true     ASRC ratio was set successfully.
 * \retval false    ASRC ratio was failed to set.
 *
 * \ingroup AUDIO_PIPE
 */
bool audio_pipe_asrc_set(T_AUDIO_PIPE_HANDLE handle,
                         int32_t             ratio);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_PIPE_H_ */
