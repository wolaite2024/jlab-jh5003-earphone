/**
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _APP_VAD_H_
#define _APP_VAD_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if F_APP_VAD_SUPPORT
/**
 * app_vad.h
 *
 * \brief Define Vad state.
 *
 * \ingroup APP_VAD
 */
typedef enum t_vad_sm_state
{
    VAD_STOPPED,
    VAD_STARTING,
    VAD_STARTED,
    VAD_STOPPING,
} T_VAD_SM_STATE;

/**
 * app_vad.h
 *
 * \brief   Init vad parameters.
 *
 * \param[in] Void.
 * \ingroup APP_VAD
 */
void app_vad_init(void);

/**
 * app_vad.h
 *
 * \brief   Enable vad function.
 *
 * \param[in] vad_type               The type \ref T_VAD_TYPE.
 * \param[in] bit_width              The vad bit wdth \ref T_VAD_DATA_LEN.
 * \param[in] frame_length           The vad frame length\ref VAD_BLOCK_SIZE/(8*(bit_widit)Byte).
 *
 * \note Voice activity detection is a singleton design that the application shall destroy the
 *       instance before creating a new instance.
 *
 * \return          The status of enabling vad function.
 * \retval  true    Vad function was enabled successfully.
 * \retval  false   Vad function was failed to enable.
 *
 * \ingroup APP_VAD
 */
bool app_vad_enable(uint8_t  vad_type,
                    uint8_t  bit_width,
                    uint16_t frame_length);

/**
 * app_vad.h
 *
 * \brief   Disable app vad function.
 *
 * \return          The status of disabling vad function.
 * \retval  true    Vad function was disabled successfully.
 * \retval  false   Vad function was failed to disable.
 *
 * \ingroup APP_VAD
 */
bool app_vad_disable(void);
#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
