/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_LINE_H_
#define _AUDIO_LINE_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_type.h"
#include "audio_route.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_LINE Audio Line
 *
 * \brief   Start, stop and control the Audio Line loopback playback.
 * \details Audio Line is either a digital (e.g. SPDIF) or an analog (e.g. AUX) jack that can be
 *          connected to another audio output device. Audio Line is used to record, relay or amplify
 *          the incoming audio stream.
 */

/**
 * \brief Define the Audio Line session handle.
 *
 * \ingroup AUDIO_LINE
 */
typedef void *T_AUDIO_LINE_HANDLE;

/**
 * \brief Define Audio Line states.
 *
 * \ingroup AUDIO_LINE
 */
typedef enum t_audio_line_state
{
    AUDIO_LINE_STATE_RELEASED      = 0x00, /**< Audio line that was released or not created yet. */
    AUDIO_LINE_STATE_CREATED       = 0x01, /**< Audio line that was created. */
    AUDIO_LINE_STATE_STARTED       = 0x02, /**< Audio line that was started. */
    AUDIO_LINE_STATE_STOPPED       = 0x03, /**< Audio line that was stopped. */
    AUDIO_LINE_STATE_PAUSED        = 0x04, /**< Audio line that was paused. */
} T_AUDIO_LINE_STATE;

/**
 * \brief   Create an Audio Line playback session.
 *
 * \param[in] device            The device in and out bitmask \ref AUDIO_DEVICE_BITMASK of the Audio Line session.
 * \param[in] sample_rate_in    Sample rate in for the Audio Line playback session. The supported values are 48000 and 96000.
 * \param[in] sample_rate_out   Sample rate out for the Audio Line playback session. The supported values are 48000 and 96000.
 *
 * \return  The session handle of Audio Line. If returned handle is NULL, the Audio Line session
 *          was failed to create.
 *
 * \ingroup AUDIO_LINE
 */
T_AUDIO_LINE_HANDLE audio_line_create(uint32_t device,
                                      uint32_t sample_rate_in,
                                      uint32_t sample_rate_out);

/**
 * \brief   Release the Audio Line playback session.
 *
 * \param[in] handle    The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 *
 * \return          The result of releasing the Audio Line playback session.
 * \retval  true    Audio Line playback session was released successfully.
 * \retval  false   Audio Line playback session was failed to release.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_release(T_AUDIO_LINE_HANDLE handle);

/**
 * \brief   Start the Audio Line playback session.
 *
 * \param[in] handle    The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 *
 * \return          The result of starting Audio Line playback session.
 * \retval  true    Audio Line playback session was started successfully.
 * \retval  false   Audio Line playback session was failed to start.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_start(T_AUDIO_LINE_HANDLE handle);

/**
 * \brief   Stop the Audio Line playback session.
 *
 * \param[in] handle    The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 *
 * \return          The result of stopping Audio Line playback session.
 * \retval  true    Audio Line playback session was stopped successfully.
 * \retval  false   Audio Line playback session was failed to stop.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_stop(T_AUDIO_LINE_HANDLE handle);

/**
 * \brief   Get the state of the specific Audio Line playback session.
 *
 * \param[in]  handle   The Audio Line playback session handle \ref T_AUDIO_LINE_HANDLE.
 * \param[out] state    The current state \ref T_AUDIO_LINE_STATE of the Audio Line playback session.
 *
 * \return          The result of getting Audio Line playback session state.
 * \retval true     Audio Line playback session state was got successfully.
 * \retval false    Audio Line playback session state was failed to get.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_state_get(T_AUDIO_LINE_HANDLE  handle,
                          T_AUDIO_LINE_STATE  *state);

/**
 * \brief   Get the maximum volume out level of the Audio Line category.
 *
 * \return  The maximum volume out level.
 *
 * \ingroup AUDIO_LINE
 */
uint8_t audio_line_volume_out_max_get(void);

/**
 * \brief   Set the maximum volume out level of the Audio Line category.
 *
 * \param[in] level The maximum volume out level to set.
 *
 * \return          The result of setting the maximum volume out level.
 * \retval  true    The maximum volume out level was set successfully.
 * \retval  false   The maximum volume out level was failed to set.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_volume_out_max_set(uint8_t level);

/**
 * \brief   Get the minimum volume out level of the Audio Line category.
 *
 * \return  The minimum volume out level.
 *
 * \ingroup AUDIO_LINE
 */
uint8_t audio_line_volume_out_min_get(void);

/**
 * \brief   Set the minimum volume out level of the Audio Line category.
 *
 * \param[in]   volume  The minimum volume out level to set.
 *
 * \return          The result of setting the minimum volume out level.
 * \retval  true    The minimum volume out level was set successfully.
 * \retval  false   The minimum volume out level was failed to set.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_volume_out_min_set(uint8_t volume);

/**
 * \brief   Get the current volume out level of the Audio Line session.
 *
 * \param[in]  handle   The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 * \param[out] volume   The volume out level of the Audio Line session.
 *
 * \return          The result of getting the current volume out level of the Audio Line session.
 * \retval true     The current volume out level of the Audio Line session was got successfully.
 * \retval false    The current volume out level of the Audio Line session was failed to get.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_volume_out_get(T_AUDIO_LINE_HANDLE  handle,
                               uint8_t             *volume);

/**
 * \brief   Set the current volume out level of the Audio Line session.
 *
 * \param[in] handle   The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 * \param[in] volume   The volume out level of the Audio Line session.
 *
 * \return           The result of setting the current volume out level of the Audio Line session.
 * \retval  true     The current volume out level of the Audio Line session was set successfully.
 * \retval  false    The current volume out level of the Audio Line session was failed to set.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_volume_out_set(T_AUDIO_LINE_HANDLE handle,
                               uint8_t             volume);

/**
 * \brief   Get the maximum volume in level of the Audio Line category.
 *
 * \return  The maximum volume in level.
 *
 * \ingroup AUDIO_LINE
 */
uint8_t audio_line_volume_in_max_get(void);

/**
 * \brief   Set the maximum volume in level of the Audio Line category.
 *
 * \param[in]   volume  The maximum volume in level to set.
 *
 * \return          The result of setting the maximum volume in level.
 * \retval  true    The maximum volume in level was set successfully.
 * \retval  false   The maximum volume in level was failed to set.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_volume_in_max_set(uint8_t volume);

/**
 * \brief   Get the minimum volume in level of the Audio Line category.
 *
 * \return  The minimum volume in level.
 *
 * \ingroup AUDIO_LINE
 */
uint8_t audio_line_volume_in_min_get(void);

/**
 * \brief   Set the minimum volume in level of the Audio Line category.
 *
 * \param[in]   volume  The minimum volume in level to set.
 *
 * \return          The result of setting the minimum volume in level.
 * \retval  true    The minimum volume in level was set successfully.
 * \retval  false   The minimum volume in level was failed to set.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_volume_in_min_set(uint8_t volume);

/**
 * \brief   Get the current volume in level of the Audio Line session.
 *
 * \param[in]  handle   The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 * \param[out] volume   The volume in level of the Audio Line session.
 *
 * \return          The result of getting the current volume in level of the Audio Line session.
 * \retval true     The current volume in level of the Audio Line session was got successfully.
 * \retval false    The current volume in level of the Audio Line session was failed to get.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_volume_in_get(T_AUDIO_LINE_HANDLE  handle,
                              uint8_t             *volume);

/**
 * \brief   Set the current volume in level of the Audio Line session.
 *
 * \param[in] handle   The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 * \param[in] volume   The volume in level of the Audio Line session.
 *
 * \return          The result of setting the current volume in level of the Audio Line session.
 * \retval true     The current volume in level of the Audio Line session was set successfully.
 * \retval false    The current volume in level of the Audio Line session was failed to set.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_volume_in_set(T_AUDIO_LINE_HANDLE handle,
                              uint8_t             volume);

/**
 * \brief  Reorder the channel out streaming layout of the specific Audio Line session.
 *
 * \param[in] handle           The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 * \param[in] channel_num      The channel out number.
 * \param[in] channel_array    The channel out array.
 *
 * \note    The channel out number shall not be 0, and each element in the corresponding
 *          channel out array table is a 4-octet bitmask.
 *
 * \return          The result of reordering the Audio Line channel out streaming layout.
 * \retval  true    Audio Line channel out streaming layout was reordering successfully.
 * \retval  false   Audio Line channel out streaming layout was failed to reorder.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_channel_out_set(T_AUDIO_LINE_HANDLE handle,
                                uint8_t             channel_num,
                                uint32_t            channel_array[]);

/**
 * \brief  Start the signal out strength monitoring of the Audio Line session.
 *
 * \param[in] handle            The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 * \param[in] refresh_interval  The refresh interval in milliseconds.
 * \param[in] freq_num          The frequency number of the signal for monitoring.
 * \param[in] freq_table        The frequency table of the signal for monitoring.
 *
 * \note    Frequency number ranges from 0 to 10, which specifies the size of frequency table.
 *          If the frequency number is set to 0, the frequency table shall be empty and the
 *          whole band of the signal shall be monitored.
 *
 * \return          The result of starting the Audio Line signal out strength monitoring.
 * \retval  true    Audio Line signal out strength monitoring was started successfully.
 * \retval  false   Audio Line signal out strength monitoring was failed to start.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_signal_out_monitoring_start(T_AUDIO_LINE_HANDLE handle,
                                            uint16_t            refresh_interval,
                                            uint8_t             freq_num,
                                            uint32_t            freq_table[]);

/**
 * \brief  Stop the signal out strength monitoring of the Audio Line session.
 *
 * \param[in] handle    The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 *
 * \return          The result of stopping the Audio Line signal out strength monitoring.
 * \retval  true    Audio Line signal out strength monitoring was stopped successfully.
 * \retval  false   Audio Line signal out strength monitoring was failed to stop.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_signal_out_monitoring_stop(T_AUDIO_LINE_HANDLE handle);

/**
 * \brief  Start the signal in strength monitoring of the Audio Line session.
 *
 * \param[in] handle            The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 * \param[in] refresh_interval  The refresh interval in milliseconds.
 * \param[in] freq_num          The frequency number of the signal for monitoring.
 * \param[in] freq_table        The frequency table of the signal for monitoring.
 *
 * \note    Frequency number ranges from 0 to 10, which specifies the size of frequency table.
 *          If the frequency number is set to 0, the frequency table shall be empty and the
 *          whole band of the signal shall be monitored.
 *
 * \return          The result of stopping the Audio Line signal in strength monitoring.
 * \retval  true    Audio Line signal in strength monitoring was stopped successfully.
 * \retval  false   Audio Line signal in strength monitoring was failed to stop.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_signal_in_monitoring_start(T_AUDIO_LINE_HANDLE handle,
                                           uint16_t            refresh_interval,
                                           uint8_t             freq_num,
                                           uint32_t            freq_table[]);

/**
 * \brief  Stop the signal in strength monitoring of the Audio Line session.
 *
 * \param[in] handle    The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 *
 * \return          The result of stopping the Audio Line signal in strength monitoring.
 * \retval  true    Audio Line signal in strength monitoring was stopped successfully.
 * \retval  false   Audio Line signal in strength monitoring was failed to stop.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_signal_in_monitoring_stop(T_AUDIO_LINE_HANDLE handle);

/**
 * \brief   Get the volume balance scale of the Audio Line category.
 *
 * \details Volume balance scale ranges from -1.0 to +1.0. If the volume balance scale
 *          is 0.0, the left channel volume and right channel volume are identical; if
 *          the volume balance scale ranges from +0.0 to +1.0, the right channel volume
 *          remains unchanged but the left channel volume scales down to (1.0 - scale)
 *          ratio; if the volume balance scale ranges from -1.0 to -0.0, the left channel
 *          volume remains unchanged but the right channel volume scales down to (1.0 + scale)
 *          ratio.
 *
 * \return  The volume balance scale of the Audio Line category.
 *          The valid returned values are from -1.0 to +1.0.
 *
 * \ingroup AUDIO_LINE
 */
float audio_line_volume_balance_get(void);

/**
 * \brief   Set the volume balance scale of the Audio Line category.
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
 * \return          The result of setting the Audio Line category volume balance scale.
 * \retval true     The volume balance scale was set successfully.
 * \retval false    The volume balance scale was failed to set.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_volume_balance_set(float scale);

/**
 * \brief   Attach the Audio Effect instance to the Audio Line session.
 *
 * \param[in] handle    The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 * \param[in] instance  The Audio Effect instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return          The result of attaching the Audio Effect instance.
 * \retval  true    Audio Effect instance was attached successfully.
 * \retval  false   Audio Effect instance was failed to attach.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_effect_attach(T_AUDIO_LINE_HANDLE     handle,
                              T_AUDIO_EFFECT_INSTANCE instance);

/**
 * \brief   Detach the Audio Effect instance from the Audio Line session.
 *
 * \param[in] handle    The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 * \param[in] instance  The Audio Effect instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return          The result of detaching the Audio Effect instance.
 * \retval  true    Audio Effect instance was detached successfully.
 * \retval  false   Audio Effect instance was failed to detach.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_effect_detach(T_AUDIO_LINE_HANDLE     handle,
                              T_AUDIO_EFFECT_INSTANCE instance);

/**
 * \brief   Get the binding Audio Route path of the specific Audio Line session.
 *
 * \param[in]  handle      The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 * \param[in]  device      The device bitmask \ref AUDIO_DEVICE_BITMASK of the Audio Line session.
 * \param[out] path        The Audio Route path \ref T_AUDIO_ROUTE_PATH.
 *
 * \return          The status of getting the Audio Line route path.
 * \retval  true    Audio Line route path was got successfully.
 * \retval  false   Audio Line route path was failed to get.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_route_path_take(T_AUDIO_LINE_HANDLE  handle,
                                uint32_t             device,
                                T_AUDIO_ROUTE_PATH  *path);

/**
 * \brief   Free the binding Audio Route path of the specific Audio Line session.
 *
 * \param[in] path Audio Route path \ref T_AUDIO_ROUTE_PATH.
 *
 * \ingroup AUDIO_LINE
 */
void audio_line_route_path_give(T_AUDIO_ROUTE_PATH *path);

/**
 * \brief   Bind the constructed Audio Route resources for the specific Audio Line session.
 *
 * \param[in] handle  The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 * \param[in] path    The Audio Route path \ref T_AUDIO_ROUTE_PATH.
 *
 * \note    The Audio Route allocates the Audio Route resources containing the routing entries specified
 *          by the Audio Route path, then binds the Audio Route resources to the Audio Line session handle.
 *          When the Audio Line session handle is released \ref audio_line_release, the Audio Route
 *          resources are also released.
 *
 * \return  The status of binding the Audio Route resources.
 * \retval  true    Audio Route resources were bound successfully.
 * \retval  false   Audio Route resources were failed to bind.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_route_bind(T_AUDIO_LINE_HANDLE handle,
                           T_AUDIO_ROUTE_PATH  path);

/**
 * \brief   Unbind the constructed Audio Route resources from the specific Audio Line session.
 *
 * \param[in] handle The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 *
 * \return  The status of unbinding the Audio Route resources.
 * \retval  true    Audio Route resources were unbound successfully.
 * \retval  false   Audio Route resources were failed to unbind.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_route_unbind(T_AUDIO_LINE_HANDLE handle);

/**
 * \brief   Add the Audio Route entry to the specific Audio Line session.
 *
 * \param[in] handle    The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 * \param[in] entry     Audio Route entry.
 *
 * \note    Add the Audio Route entry specified by \ref T_AUDIO_ROUTE_IO_TYPE if this Audio Route
 *          entry exists, otherwise, add the Audio Route entry into the Audio Route resources.
 *
 * \return  The status of adding the Audio Route entry.
 * \retval  true    Audio Route entry was added successfully.
 * \retval  false   Audio Route entry was failed to add.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_route_add(T_AUDIO_LINE_HANDLE  handle,
                          T_AUDIO_ROUTE_ENTRY *entry);

/**
 * \brief   Remove the Audio Route entry of the specific Audio Line session.
 *
 * \param[in] handle    The Audio Line session handle \ref T_AUDIO_LINE_HANDLE.
 * \param[in] io_type   The logical IO type of the Audio Route entry.
 *
 * \return  The status of removing the Audio Route entry.
 * \retval  true    Audio Route instance was removed successfully.
 * \retval  false   Audio Route instance was failed to remove.
 *
 * \ingroup AUDIO_LINE
 */
bool audio_line_route_remove(T_AUDIO_LINE_HANDLE   handle,
                             T_AUDIO_ROUTE_IO_TYPE io_type);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_LINE_H_ */
