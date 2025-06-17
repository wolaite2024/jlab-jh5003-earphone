/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _AUDIO_VSE_
#define _AUDIO_VSE_

#include <stdbool.h>
#include <stdint.h>
#include "audio_type.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup AUDIO_VSE  Audio Vendor Specific Effect
 *
 * \brief   Control and customize the Vendor Specific Effects in the audio framework.
 * \details Application can use this Audio Effect modeling to define and implement varied vendor
 *          effects.
 */

/**
 * \defgroup AUDIO_VSE_COMPANY_ID Audio VSE Company ID
 *
 * \brief Define all the registered company IDs.
 *
 * \details Company IDs are 16-bit UUIDs that vendor companies shall apply for a unique one from
 *          the SDK host. Note that if you use any unapplied company IDs in your programs, we do
 *          not guarantee that the underlying functions will work as expected.
 *
 * \ingroup AUDIO_VSE
 * @{
 */
#define AUDIO_VSE_COMPANY_ID_REALTEK    0x0000  /**< Realtek Semiconductor Corporation */
#define AUDIO_VSE_COMPANY_ID_MALLEUSTEK 0x0001  /**< MalleusTek Co., Ltd. */
#define AUDIO_VSE_COMPANY_ID_SONY       0x0002  /**< Sony Group Corporation */
#define AUDIO_VSE_COMPANY_ID_SAVITECH   0x0003  /**< Savitech Corporation */

/**
 * @}
 */

/**
 * \brief  Audio VSE event type for \ref P_AUDIO_VSE_CBACK.
 *
 * \ingroup AUDIO_VSE
 */
typedef enum t_audio_vse_event
{
    AUDIO_VSE_EVENT_ENABLE       = 0x00, /**< Effect enable notification from VSE's attached stream session */
    AUDIO_VSE_EVENT_DISABLE      = 0x01, /**< Effect disable notification from VSE's attached stream session */
    AUDIO_VSE_EVENT_TRANS_RSP    = 0x02, /**< Effect response transaction for VSE instance */
    AUDIO_VSE_EVENT_TRANS_IND    = 0x03, /**< Effect indication transaction for VSE instance */
    AUDIO_VSE_EVENT_TRANS_NOTIFY = 0x04, /**< Effect notify transaction for VSE instance */
} T_AUDIO_VSE_EVENT;

/**
 * \brief Audio VSE event callback prototype.
 *
 * \param[in] instance  The Audio VSE instance \ref T_AUDIO_EFFECT_INSTANCE.
 * \param[in] event     The Audio VSE event \ref T_AUDIO_VSE_EVENT.
 * \param[in] seq_num   The Audio VSE transaction sequence number.
 * \param[in] ltv_buf   The Audio VSE LTV metadata buffer for the corresponding \ref T_AUDIO_VSE_EVENT.
 * \param[in] ltv_len   The Audio VSE LTV metadata buffer length.
 *
 * \note  If parameter event_type is \ref AUDIO_VSE_EVENT_ENABLE or \ref AUDIO_VSE_EVENT_DISABLE,
 *        parameter seq_num is unused and shall be ignored, parameter event_buf shall be NULL, and
 *        parameter buf_len shall be 0. If parameter event_type is \ref AUDIO_VSE_EVENT_TRANS_RSP,
 *        \ref AUDIO_VSE_EVENT_TRANS_IND, or \ref AUDIO_VSE_EVENT_TRANS_NOTIFY, parameter event_buf
 *        is constructed in LTV metadata format and parameter buf_len specifies the total length of
 *        LTV metadata.
 *
 * \ingroup AUDIO_VSE
 */
typedef void (*P_AUDIO_VSE_CBACK)(T_AUDIO_EFFECT_INSTANCE  instance,
                                  T_AUDIO_VSE_EVENT        event,
                                  uint16_t                 seq_num,
                                  void                    *ltv_buf,
                                  uint16_t                 ltv_len);

/**
 * \brief   Create an audio VSE instance.
 *
 * \param[in] company_id Registered Company ID \ref AUDIO_VSE_COMPANY_ID.
 * \param[in] effect_id  Vendor defined effect ID.
 * \param[in] cback      Audio VSE callback function.
 *
 * \note    The effect ID domain is separated per company ID, and vendors can manage their own
 *          effect IDs independently.
 *
 * \return  The instance of the audio VSE \ref T_AUDIO_EFFECT_INSTANCE. If the
 *          returned instance is NULL, the audio VSE instance was failed to create.
 *
 * \ingroup AUDIO_VSE
 */
T_AUDIO_EFFECT_INSTANCE audio_vse_create(uint16_t          company_id,
                                         uint16_t          effect_id,
                                         P_AUDIO_VSE_CBACK cback);

/**
 * \brief   Apply the specific audio VSE instance's metadata.
 *
 * \param[in] instance  Audio VSE instance \ref T_AUDIO_EFFECT_INSTANCE.
 * \param[in] seq_num   Audio VSE instance's request transaction sequence number.
 * \param[in] ltv_buf   Audio VSE instance's LTV metadata buffer.
 * \param[in] ltv_len   Audio VSE instance's LTV metadata buffer length.
 *
 * \note    The metadata buffer consists of one or more LTV metadata that each metadata shall be
 *          constructed in Length-Type-Value format.
 *
 * \return  The status of applying the audio VSE instance's metadata.
 * \retval  true    Audio VSE instance's metadata was applied successfully.
 * \retval  false   Audio VSE instance's metadata was failed to apply.
 *
 * \ingroup AUDIO_VSE
 */
bool audio_vse_apply(T_AUDIO_EFFECT_INSTANCE  instance,
                     uint16_t                 seq_num,
                     void                    *ltv_buf,
                     uint16_t                 ltv_len);

/**
 * \brief   Confirm the specific audio VSE instance's indication.
 *
 * \param[in] instance  Audio VSE instance \ref T_AUDIO_EFFECT_INSTANCE.
 * \param[in] seq_num   Audio VSE instance's indication transaction sequence number.
 * \param[in] ltv_buf  Audio VSE instance's LTV metadata buffer.
 * \param[in] ltv_len  Audio VSE instance's LTV metadata buffer length.
 *
 * \note    The metadata buffer consists of one or more LTV metadata that each metadata shall be
 *          constructed in Length-Type-Value format.
 *
 * \return  The status of confirming the audio VSE instance's indication.
 * \retval  true    Audio VSE instance's indication was confirmed successfully.
 * \retval  false   Audio VSE instance's indication was failed to confirm.
 *
 * \ingroup AUDIO_VSE
 */
bool audio_vse_cfm(T_AUDIO_EFFECT_INSTANCE  instance,
                   uint16_t                 seq_num,
                   void                    *ltv_buf,
                   uint16_t                 ltv_len);

/**
 * \brief   Release the specific audio VSE instance.
 *
 * \param[in] instance  Audio VSE instance \ref T_AUDIO_EFFECT_INSTANCE.
 *
 * \return  The status of releasing the audio VSE instance.
 * \retval  true    Audio VSE instance was released successfully.
 * \retval  false   Audio VSE instance was failed to release.
 *
 * \ingroup AUDIO_VSE
 */
bool audio_vse_release(T_AUDIO_EFFECT_INSTANCE instance);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* _AUDIO_VSE_ */
