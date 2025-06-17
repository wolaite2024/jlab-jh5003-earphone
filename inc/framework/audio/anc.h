/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _ANC_H_
#define _ANC_H_

#include <stdint.h>
#include <stdbool.h>
#include "audio_type.h"
#include "audio_route.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_ANC Active Noise Cancellation
 *
 * \brief   Enable and disable Active Noise Cancellation.
 * \details Active Noise Cancellation (ANC), or Active Noise Reduction (ANR), uses noise-cancelling
 *          speakers to reduce unwanted background noise. ANC will help you get away from the noise
 *          world when enjoying music or just quiet your mind. While enabling ANC will consume more
 *          battery power.
 */

/**
 * \brief Define Active Noise Cancellation (ANC) states.
 *
 * \ingroup AUDIO_ANC
 */
typedef enum t_anc_state
{
    ANC_STATE_STOPPED       = 0x00, /**< ANC that was stopped. */
    ANC_STATE_STARTING      = 0x01, /**< ANC that was starting. */
    ANC_STATE_STARTED       = 0x02, /**< ANC that was started. */
    ANC_STATE_STOPPING      = 0x03, /**< ANC that was stopping. */
} T_ANC_STATE;

/**
 * \brief   Enable Active Noise Cancellation (ANC).
 *
 * \param[in] scenario_id   ANC scenario ID to set.
 *
 * \return  The result of enabling ANC.
 * \retval  true    ANC was enabled successfully.
 * \retval  false   ANC was failed to enable.
 *
 * \ingroup AUDIO_ANC
 */
bool anc_enable(uint8_t scenario_id);

/**
 * \brief   Disable Active Noise Cancellation (ANC).
 *
 * \return  The result of disabling ANC.
 * \retval  true    ANC was disabled successfully.
 * \retval  false   ANC was failed to disable.
 *
 * \ingroup AUDIO_ANC
 */
bool anc_disable(void);

/**
 * \brief   Get the state of Active Noise Cancellation (ANC).
 *
 * \return  The state \ref T_ANC_STATE of Active Noise Cancellation (ANC).
 *
 * \ingroup AUDIO_ANC
 */
T_ANC_STATE anc_state_get(void);

/**
 * \brief   Get the binding Audio Route path of the specific ANC session.
 *
 * \param[in]  device   The device bitmask \ref AUDIO_DEVICE_BITMASK of the ANC session.
 * \param[out] path     The Audio Route path \ref T_AUDIO_ROUTE_PATH.
 *
 * \return          The status of getting the ANC route path.
 * \retval  true    ANC route path was got successfully.
 * \retval  false   ANC route path was failed to get.
 *
 * \ingroup AUDIO_ANC
 */
bool anc_route_path_take(uint32_t            device,
                         T_AUDIO_ROUTE_PATH *path);

/**
 * \brief   Free the binding Audio Route path of the specific ANC session.
 *
 * \param[in] path Audio Route path \ref T_AUDIO_ROUTE_PATH.
 *
 * \ingroup AUDIO_ANC
 */
void anc_route_path_give(T_AUDIO_ROUTE_PATH *path);

/**
 * \brief   Bind the constructed Audio Route resources for the specific ANC session.
 *
 * \param[in] path  The Audio Route path \ref T_AUDIO_ROUTE_PATH.
 *
 * \note    The Audio Route allocates the Audio Route resources containing the routing entries specified
 *          by the Audio Route path, then binds the Audio Route resources to the ANC session handle.
 *          When the ANC session handle is released, the Audio Route resources are also released.
 *
 * \return  The status of binding the Audio Route resources.
 * \retval  true    Audio Route resources were bound successfully.
 * \retval  false   Audio Route resources were failed to bind.
 *
 * \ingroup AUDIO_ANC
 */
bool anc_route_bind(T_AUDIO_ROUTE_PATH path);

/**
 * \brief   Unbind the constructed Audio Route resources for the specific ANC session.
 *
 * \return  The status of unbinding the Audio Route resources.
 * \retval  true    Audio Route resources were unbound successfully.
 * \retval  false   Audio Route resources were failed to unbind.
 *
 * \ingroup AUDIO_ANC
 */
bool anc_route_unbind(void);

/**
 * \brief   Add or update the Audio Route entry to the specific ANC session.
 *
 * \param[in] entry Audio Route entry.
 *
 * \note    Update the Audio Route entry specified by \ref T_AUDIO_ROUTE_IO_TYPE if this Audio Route
 *          entry exists, otherwise, add the Audio Route entry into the Audio Route resources.
 *
 * \return The status of adding or updating the Audio Route entry.
 *
 * \ingroup AUDIO_ANC
 */
bool anc_route_add(T_AUDIO_ROUTE_ENTRY *entry);

/**
 * \brief   Remove the Audio Route entry of the specific ANC session.
 *
 * \param[in] io_type  The logical IO type of the Audio Route entry.
 *
 * \return The status of removing the Audio Route entry.
 *
 * \ingroup AUDIO_ANC
 */
bool anc_route_remove(T_AUDIO_ROUTE_IO_TYPE io_type);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _ANC_H_ */
