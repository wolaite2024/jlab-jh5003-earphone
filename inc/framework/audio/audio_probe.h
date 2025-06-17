/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_PROBE_H_
#define _AUDIO_PROBE_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    AUDIO_PROBE Audio Probe
 *
 * \brief   Test, analyze and make profiling on the Audio Subsystem.
 *
 * \details Audio Probe provides the testing or profiling interfaces, which applications can analyze
 *          the Audio Subsystem.
 *
 * \note    Audio Probe interfaces are used only for testing and profiling. Application shall not
 *          use Audio Probe interfaces in any formal product scenarios.
 */

/**
 * \brief   Define Audio Probe event from DSP.
 *
 * \note    Only provide Audio Probe event for application using.
 *
 * \ingroup AUDIO_PROBE
 */
typedef enum
{
    PROBE_SCENARIO_STATE             = 0x00,
    PROBE_HA_STATUS_REPORT           = 0x01,
    PROBE_SEG_SEND_REQ_DATA          = 0x02,
    PROBE_SEG_SEND_ERROR             = 0x03,
    PROBE_SYNC_REF_REQUEST           = 0x04,
    PROBE_OVP_TRAINING_REPORT        = 0X05,
} T_AUDIO_PROBE_EVENT;

typedef enum
{
    // DSP capture: the field will be deleted
    AUDIO_PROBE_DSP_EVT_MAILBOX_DSP_DATA                = 0x11,
} T_AUDIO_PROBE_DSP_EVENT;

typedef enum
{
    PROBE_ADSP_BOOT_DONE             = 0x00,
    PROBE_ADSP_ACK                   = 0x01,
    PROBE_ADSP_LOG                   = 0x02,
} T_AUDIO_PROBE_ADSP_EVENT;

/**
 * \brief   Define Audio Probe command id for DSP.
 *
 * \note    Should be referenced to h2d_cmd.
 *
 * \ingroup AUDIO_PROBE
 */
typedef enum
{
    AUDIO_PROBE_TEST_MODE       = 0x02,
    AUDIO_PROBE_UPLINK_SYNCREF  = 0x5E,
    // HA: below 2 fields will be deleted
    AUDIO_PROBE_SEG_SEND        = 0x63,
    AUDIO_PROBE_HA_PARA         = 0x70,
} T_AUDIO_PROBE_DSP_CMD;

/**
 * \brief Define Audio Probe callback prototype.
 *
 * \note    Do <b>not</b> touch this interface in any product scenarios.
 *
 * \param[in]   event   The event for DSP \ref T_AUDIO_PROBE_EVENT.
 * \param[in]   buf     The buffer that holds the DSP probe data.
 *
 * \ingroup AUDIO_PROBE
 */
typedef void (*P_AUDIO_PROBE_DSP_CABCK)(T_AUDIO_PROBE_EVENT event, void *buf);


typedef void (*P_AUDIO_PROBE_ADSP_CABCK)(T_AUDIO_PROBE_ADSP_EVENT event, uint8_t *buf, uint8_t len);

/**
 * \brief Define Audio Probe callback prototype of DSP status.
 *
 * \note    Do <b>not</b> touch this interface in any product scenarios.
 *
 * \param[in]   event   The event for DSP \ref T_AUDIO_PROBE_DSP_EVENT_MSG.
 * \param[in]   msg     The message that holds the DSP status.
 *
 * \ingroup AUDIO_PROBE
 */
typedef void (*P_AUDIO_PROBE_DSP_EVENT_CABCK)(uint32_t event, void *msg);

/**
 * \brief   Register Audio Probe DSP callback.
 *
 * \note    Do <b>not</b> touch this interface in any product scenarios.
 *
 * \param[in] cback The Audio Probe DSP callback \ref P_AUDIO_PROBE_DSP_CABCK.
 *
 * \return          The status of registering Audio Probe DSP callback.
 * \retval true     Audio Probe DSP callback was registered successfully.
 * \retval false    Audio Probe DSP callback was failed to register.
 *
 * \ingroup AUDIO_PROBE
 */
bool audio_probe_dsp_cback_register(P_AUDIO_PROBE_DSP_CABCK cback);

/**
 * \brief   Unregister Audio Probe DSP callback.
 *
 * \note    Do <b>not</b> touch this interface in any product scenarios.
 *
 * \param[in] cback The Audio Probe DSP callback \ref P_AUDIO_PROBE_DSP_CABCK.
 *
 * \return          The status of unregistering Audio Probe DSP callback.
 * \retval true     Audio Probe DSP callback was unregistered successfully.
 * \retval false    Audio Probe DSP callback was failed to unregister.
 *
 * \ingroup AUDIO_PROBE
 */
bool audio_probe_dsp_cback_unregister(P_AUDIO_PROBE_DSP_CABCK cback);

/**
 * \brief   Register Audio Probe ADSP callback.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \param[in] cback The Audio Probe ADSP callback \ref P_AUDIO_PROBE_ADSP_CABCK.
 *
 * \return          The status of registering Audio Probe ADSP callback.
 * \retval true     Audio Probe ADSP callback was registered successfully.
 * \retval false    Audio Probe ADSP callback was failed to register.
 *
 * \ingroup AUDIO_PROBE
 */
bool audio_probe_adsp_cback_register(P_AUDIO_PROBE_ADSP_CABCK cback);

/**
 * \brief   Unregister Audio Probe ADSP callback.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \param[in] cback The Audio Probe ADSP callback \ref P_AUDIO_PROBE_ADSP_CABCK.
 *
 * \return          The status of unregistering Audio Probe ADSP callback.
 * \retval true     Audio Probe ADSP callback was unregistered successfully.
 * \retval false    Audio Probe ADSP callback was failed to unregister.
 *
 * \ingroup AUDIO_PROBE
 */
bool audio_probe_adsp_cback_unregister(P_AUDIO_PROBE_ADSP_CABCK cback);

/**
 * \brief   Send Audio Probe data to DSP.
 *
 * \note    Do <b>not</b> touch this interface in any product scenarios.
 *
 * \param[in] buf   The probe data buffer address.
 * \param[in] len   The probe data buffer length.
 *
 * \return          The status of sending Audio Probe DSP data.
 * \retval true     Audio Probe DSP data was sent successfully.
 * \retval false    Audio Probe DSP data was failed to send.
 *
 * \ingroup AUDIO_PROBE
 */
bool audio_probe_dsp_send(uint8_t *buf, uint16_t len);

/**
 * \brief   Send probe data to ADSP.
 *
 * \note    Do <b>NOT</b> touch this interface in any product scenarios.
 *
 * \param[in] buf   The probing data buffer address.
 * \param[in] len   The probing data buffer length.
 *
 * \return          The status of sending ADSP probe data.
 * \retval true     ADSP probe data was sent successfully.
 * \retval false    ADSP probe data was failed to send.
 *
 * \ingroup AUDIO_PROBE
 */
bool audio_probe_adsp_send(uint8_t *buf, uint16_t len);

/**
 * \brief   Register Audio Probe DSP status callback.
 *
 * \note    Do <b>not</b> touch this interface in any product scenarios.
 *
 * \param[in] cback The DSP probe callback \ref P_AUDIO_PROBE_DSP_EVENT_CABCK.
 *
 * \return          The status of registering Audio Probe DSP status callback.
 * \retval true     Audio Probe DSP status callback was registered successfully.
 * \retval false    Audio Probe DSP status callback was failed to register.
 *
 * \ingroup AUDIO_PROBE
 */
bool audio_probe_dsp_evt_cback_register(P_AUDIO_PROBE_DSP_EVENT_CABCK cback);

/**
 * \brief   Unregister Audio Probe DSP status callback.
 *
 * \note    Do <b>not</b> touch this interface in any product scenarios.
 *
 * \param[in] cback The DSP probe callback \ref P_AUDIO_PROBE_DSP_EVENT_CABCK.
 *
 * \return          The status of unregistering Audio Probe DSP status callback.
 * \retval true     Audio Probe DSP status callback was unregistered successfully.
 * \retval false    Audio Probe DSP status callback was failed to unregister.
 *
 * \ingroup AUDIO_PROBE
 */
bool audio_probe_dsp_evt_cback_unregister(P_AUDIO_PROBE_DSP_EVENT_CABCK cback);

/**
 * \brief   Malloc RAM from playback buffer.
 *
 * \param[in] buf_size The buffer size need to malloc from playback buffer.
 *
 * \ingroup AUDIO_PROBE
 */
void *audio_probe_media_buffer_malloc(uint16_t buf_size);

/**
 * \brief   Free RAM to playback buffer.
 *
 * \return          The status of freeing playback buffer.
 * \retval true     The playback buffer was freed successfully.
 * \retval false    The playback buffer was failed to free.
 *
 * \ingroup AUDIO_PROBE
 */
bool audio_probe_media_buffer_free(void *p_buf);

/**
 * \brief    Power down DSP.
 *
 * \ingroup AUDIO_PROBE
 */
void audio_probe_disable_dsp_powerdown(void);

/**
 * \brief   Control loading DSP test bin.
 *
 * \ingroup AUDIO_PROBE
 */
void audio_probe_dsp_test_bin_set(bool enable);

/**
 * \brief   set DSP2 enable or not.
 *
 * \param[in] set DSP2 should be enable or not.
 *
 * \ingroup AUDIO_PROBE
 */
void audio_probe_dsp2_exist_set(bool enable);

/**
 * \brief   Start the engine communication with DSP2.
 *
 * \return              The engine instance ID.
 * \retval  zero        The engine is failed to start.
 * \retval  non-zero    The engine is started successfully.
 *
 * \ingroup AUDIO_PROBE
 */
uint32_t engine_start(void);

/**
 * \brief   Stop the engine communication with DSP2.
 *
 * \retval  zero        The engine is stopped successfully.
 * \retval  non-zero    The engine is failed to stop, the retval is error code.
 *
 * \ingroup AUDIO_PROBE
 */
int32_t engine_stop(uint32_t instance);
#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_PROBE_H_ */
