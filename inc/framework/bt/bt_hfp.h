/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _BT_HFP_H_
#define _BT_HFP_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    BT_HFP BT HFP Profile
 *
 * \brief   Provide BT HFP profile interfaces.
 */

/**
 * \defgroup BT_HFP_HF_LOCAL_SUPPORTED_FEATURES BT HFP HF Local Supported Features
 *
 * \brief Define BT HFP HF local supported features.
 *
 * \ingroup BT_HFP
 * @{
 */

/**
 * Echo canceling (EC) and noise reduction (NR) feature. If AG also support echo canceling and noise
 * reduction, HF can request turning off the EC and NR of AG by using AT+NREC.
 */
#define BT_HFP_HF_LOCAL_EC_NR_FUNCTION               (1 << 0)
/**
 * Three-way call handling feature. If AG also support three-way calling, HF can handle three-way
 * calling by using AT+CHLD.
 */
#define BT_HFP_HF_LOCAL_THREE_WAY_CALLING            (1 << 1)
/**
 * Calling Line Identification (CLI) notification feature. If the HF has enabled the Calling Line
 * Identification, when the calling subscriber number information is available from the network, the
 * AG shall issue the +CLIP unsolicited result code just after every RING indication when the HF is
 * alerted in an incoming call.
 */
#define BT_HFP_HF_LOCAL_CLI_PRESENTATION_CAPABILITY  (1 << 2)
/**
 * Voice Recognition feature. If AG also support voice recognition, HF can use AT+BVRA command or AG
 * autonomously to activate/deactivate the voice recognition function resident in the AG.
 */
#define BT_HFP_HF_LOCAL_VOICE_RECOGNITION_ACTIVATION (1 << 3)
/**
 * Remote audio volume control and volume level synchronization feature. This function enables the
 * user to modify the speaker volume and microphone gain of the HF from the AG. It also allows the
 * HF to inform the AG of the current gain settings corresponding to the speaker volume and
 * microphone gain.
 */
#define BT_HFP_HF_LOCAL_REMOTE_VOLUME_CONTROL        (1 << 4)
/**
 * Query call list feature. If both the HF and AG support enhanced call status, HF can query the
 * list of current calls in AG by AT+CLCC.
 */
#define BT_HFP_HF_LOCAL_ENHANCED_CALL_STATUS         (1 << 5)
/**
 * Release specified call feature. the Enhanced Call Control mechanism is simply an extension of
 * the current AT+CHLD command. These extensions are defined as additional arguments to the
 * AT+CHLD command.
 */
#define BT_HFP_HF_LOCAL_ENHANCED_CALL_CONTROL        (1 << 6)
/**
 * Codec Negotiation feature. If Wide Band Speech is supported, Codec Negotiation shall also be
 * supported. If both the HF and AG do support the Codec Negotiation feature, the available codecs
 * supported on both sides can be negotiated.
 */
#define BT_HFP_HF_LOCAL_CODEC_NEGOTIATION            (1 << 7)
/**
 * HF indicator feature. If both the HF and AG support the HF Indicator feature, the HF can send
 * the AT+BIEV command with the corresponding HF indicator value whenever a change in value occurs
 * of an enabled HF indicator.
 */
#define BT_HFP_HF_LOCAL_HF_INDICATORS                (1 << 8)
/**
 * If both the HF and AG support the eSCO S4 settings, the parameter sets S4 can be used when eSCO
 * logical transports established.
 */
#define BT_HFP_HF_LOCAL_ESCO_S4_SETTINGS             (1 << 9)
/**
 * @}
 */

/**
 * \defgroup BT_HFP_HF_REMOTE_SUPPORTED_FEATURES BT HFP HF Remote Supported Features
 *
 * \brief Define BT HFP HF remote supported features.
 *
 * \ingroup BT_HFP
 * @{
 */

/**
 * Three-way call handling feature. If HF also support three-way calling, HF can handle three-way
 * calling by using AT+CHLD.
 */
#define BT_HFP_HF_REMOTE_CAPABILITY_3WAY                   (1 << 0)
/**
 * Echo canceling (EC) and noise reduction (NR) feature. If HF also support echo canceling and noise
 * reduction, HF can request turning off the EC and NR of AG by using AT+NREC.
 */
#define BT_HFP_HF_REMOTE_CAPABILITY_EC_NR                  (1 << 1)
/**
 * Voice Recognition feature. If HF also support voice recognition, HF can use AT+BVRA command or
 * AG autonomously to activate/deactivate the voice recognition function resident in the AG.
 */
#define BT_HFP_HF_REMOTE_CAPABILITY_VOICE_RECOGNITION      (1 << 2)
/**
 * Incoming call In-band ring tone feature. If in-band ring tone is used, the AG shall send the ring
 * tone to the HF via the established Audio Connection.
 */
#define BT_HFP_HF_REMOTE_CAPABILITY_INBAND_RINGING         (1 << 3)
/**
 * Attach a voice tag to a phone number. It provides a means to read numbers from the AG for the
 * purpose of creating a unique voice tag and storing the number and its linked voice tag in the
 * memory of HF.
 */
#define BT_HFP_HF_REMOTE_CAPABILITY_VOICE_TAG              (1 << 4)
/**
 * If the AG has the capability to reject incoming calls.
 */
#define BT_HFP_HF_REMOTE_CAPABILITY_REJECT_CALLS           (1 << 5)
/**
 * Query call list feature. If both the HF and AG support enhanced call status, HF can query the
 * list of current calls in AG by AT+CLCC.
 */
#define BT_HFP_HF_REMOTE_CAPABILITY_ENHANCED_CALL_STATUS   (1 << 6)
/**
 * Release specified call feature. The Enhanced Call Control mechanism is an extension of the
 * current AT+CHLD command. These extensions are defined as additional arguments to the
 * AT+CHLD command.
 */
#define BT_HFP_HF_REMOTE_CAPABILITY_ENHANCED_CALL_CONTROL  (1 << 7)
/**
 * If HF can enable/disable Extended Audio Gateway Error result codes in the AG.
 */
#define BT_HFP_HF_REMOTE_CAPABILITY_EXTENED_ERROR_RESULT   (1 << 8)
/**
 * Codec Negotiation feature. If Wide Band Speech is supported, Codec Negotiation shall also be
 * supported. If both the HF and AG do support the Codec Negotiation feature, the available codecs
 * supported on both sides can be negotiated.
 */
#define BT_HFP_HF_REMOTE_CAPABILITY_CODEC_NEGOTIATION      (1 << 9)
/**
 * HF indicator feature. If both the HF and AG support the HF Indicator feature, the HF can send the
 * AT+BIEV command with the corresponding HF indicator value whenever a change in value occurs of an
 * enabled HF indicator.
 */
#define BT_HFP_HF_REMOTE_CAPABILITY_HF_INDICATORS          (1 << 10)
/**
 * If both the HF and AG support the eSCO S4 settings, the parameter sets S4 can be used when eSCO
 * logical transports established.
 */
#define BT_HFP_HF_REMOTE_CAPABILITY_ESCO_S4_SUPPORTED   (1 << 11)
/**
 * @}
 */

/**
 * \defgroup BT_HFP_HF_SUPPORTED_CODEC_TYPE BT HFP HF Supported Codec Type
 *
 * \brief Define BT HFP HF supported codec type.
 *
 * \ingroup BT_HFP_AG
 * @{
 */
#define BT_HFP_HF_CODEC_TYPE_CVSD               (1 << 0)
#define BT_HFP_HF_CODEC_TYPE_MSBC               (1 << 1)
#define BT_HFP_HF_CODEC_TYPE_LC3                (1 << 2)
/**
 * @}ag
 */

/**
 * \defgroup BT_HFP_INDICATOR_TYPE BT HFP indicator type
 *
 * \brief Define BT HFP INDICATOR TYPE.
 *
 * \ingroup BT_HFP
 * @{
 */

#define BT_HFP_INDICATOR_TYPE_SERVICE     (1 << 0)
#define BT_HFP_INDICATOR_TYPE_CALL        (1 << 1)
#define BT_HFP_INDICATOR_TYPE_CALLSETUP   (1 << 2)
#define BT_HFP_INDICATOR_TYPE_CALLHELD    (1 << 3)
#define BT_HFP_INDICATOR_TYPE_SIGNAL      (1 << 4)
#define BT_HFP_INDICATOR_TYPE_ROAM        (1 << 5)
#define BT_HFP_INDICATOR_TYPE_BATTCHG     (1 << 6)
/**
 * @}
 */

/**
 * \brief  Define BT HFP call status.
 *
 * \ingroup BT_HFP
 */
typedef enum t_bt_hfp_call_status
{
    /**
     * no call in progress
     */
    BT_HFP_CALL_IDLE                              = 0x00,
    /**
     * an incoming call is ongoing
     */
    BT_HFP_CALL_INCOMING                          = 0x01,
    /**
     * an outgoing call is ongoing
     */
    BT_HFP_CALL_OUTGOING                          = 0x02,
    /**
     * an active call is in progress
     */
    BT_HFP_CALL_ACTIVE                            = 0x03,
    /**
     * a call is be held
     */
    BT_HFP_CALL_HELD                              = 0x04,
    /**
     * one call active and another call incoming in three-way calling
     */
    BT_HFP_CALL_ACTIVE_WITH_CALL_WAITING          = 0x05,
    /**
     * one call active and another call held in three-way calling
     */
    BT_HFP_CALL_ACTIVE_WITH_CALL_HELD             = 0x06,
} T_BT_HFP_CALL_STATUS;

/**
 * \brief   Initialize HFP profile.
 *
 * \param[in]  rfc_hfp_chann_num      RFCOMM channel number used for HFP HF.
 * \param[in]  rfc_hsp_chann_num      RFCOMM channel number used for HFP HF.
 * \param[in]  supported_features     HF supported features \ref BT_HFP_HF_LOCAL_SUPPORTED_FEATURE.
 * \param[in]  supported_codecs       HF supported codecs \ref BT_HFP_HF_SUPPORTED_CODEC_TYPE.
 *
 * \return          The status of initializing HFP profile.
 * \retval true     HFP profile was initialized successfully.
 * \retval false    HFP profile was failed to initialize.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_init(uint8_t  rfc_hfp_chann_num,
                 uint8_t  rfc_hsp_chann_num,
                 uint16_t supported_features,
                 uint8_t  supported_codecs);

/**
 * \brief   De-initialize HFP profile.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API"
 *
 * \ingroup BT_HFP
 */
void bt_hfp_deinit(void);

/**
 * \brief   Send an HFP or HSP connection request.
 *
 * \param[in]  bd_addr       Remote BT address.
 * \param[in]  server_chann  Remote dlci which can be found from the SDP info.
 * \param[in]  is_hfp        Choose to create an HFP or HSP connection.
 * \arg    true          Create a HFP connection.
 * \arg    false         Create a HSP connection.
 *
 * \return         The status of sending the HFP or HSP connection request.
 * \retval true    HFP or HSP connection request was sent successfully.
 * \retval false   HFP or HSP connection request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_connect_req(uint8_t bd_addr[6],
                        uint8_t server_chann,
                        bool    is_hfp);

/**
 * \brief   Send an HFP connection confirmation.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  accept     Accept or reject the connection indication.
 * \arg    true     Accept the connection indication.
 * \arg    false    Reject the connection indication
 *
 * \return         The status of sending the HFP connection confirmation.
 * \retval true    HFP connection confirmation was sent successfully.
 * \retval false   HFP connection confirmation was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_connect_cfm(uint8_t bd_addr[6],
                        bool    accept);

/**
 * \brief  Send an HFP disconnection request.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the HFP disconnection request.
 * \retval true    HFP disconnection request was sent successfully.
 * \retval false   HFP disconnection request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_disconnect_req(uint8_t bd_addr[6]);

/**
 * \brief  Send a vendor AT command request.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  at_cmd     The start address of buffer that stores AT command. AT command
 *                        string must end with "\r".
 *
 * \return         The status of sending the AT command request.
 * \retval true    AT command request was sent successfully.
 * \retval false   AT command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_send_vnd_at_cmd_req(uint8_t     bd_addr[6],
                                const char *at_cmd);

/**
 * \brief  Send a command to dial a specific phone number.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  number     The start address of string buffer that stores phone number.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_dial_with_number_req(uint8_t     bd_addr[6],
                                 const char *number);

/**
 * \brief  Send a command to recalling the last number dialed by the remote device.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_dial_last_number_req(uint8_t bd_addr[6]);

/**
 * \brief  Send a command to answer the incoming call.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_call_answer_req(uint8_t bd_addr[6]);

/**
 * \brief  Send a command to terminate the phone call.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_call_terminate_req(uint8_t bd_addr[6]);

/**
 * \brief  Send a command to hold the phone call.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_call_hold_req(uint8_t bd_addr[6]);

/**
 * \brief  Send a command to accept the held phone call.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_held_call_accept_req(uint8_t bd_addr[6]);

/**
 * \brief  Send a command to release the held phone call.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_held_call_release_req(uint8_t bd_addr[6]);

/**
 * \brief  Send a command to query current call list.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_current_call_list_req(uint8_t bd_addr[6]);

/**
 * \brief  Send a command to release held or waiting call when three-way calling supported.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_release_held_or_waiting_call_req(uint8_t bd_addr[6]);

/**
 * \brief  Send a command to release active call and accept held or waiting call when
 *         three-way calling supported.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_release_active_call_accept_held_or_waiting_call_req(uint8_t bd_addr[6]);

/**
 * \brief  Send a command to hold active call and accept held or waiting call when
 *         three-way calling supported.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_hold_active_call_accept_held_or_waiting_call_req(uint8_t bd_addr[6]);

/**
 * \brief  Send a command to join two calls when three-way calling supported.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_join_two_calls_req(uint8_t bd_addr[6]);

/**
 * \brief  Send a request to creat audio connection.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \note   Although the audio connection may be triggered by either AG or HF, the codec connection
 *         and the synchronous connection shall always be established by the AG (unless one of the
 *         devices is a legacy device). HF implementations that comply with the Hands-Free Profile
 *         specification Rev 0.96, 1.0 or 1.5, shall not indicate support for the codec negotiation
 *         feature and shall neither send the AT+BAC command nor the AT+BCC command to trigger an
 *         audio connection establishment by the AG. In order to guarantee backward compatibility,
 *         HFP Rev. x.y implementations shall be able to handle establishment of synchronous
 *         connections according to Hands-Free Profile specification Rev. 1.0 or 1.5.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_audio_connect_req(uint8_t bd_addr[6]);

/**
 * \brief   Send an audio connection confirmation.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API"
 *
 * \param[in] bd_addr    Remote BT address.
 * \param[in] accept     Accept or reject the audio connection indication.
 * \arg    true     Accept the audio connection indication.
 * \arg    false    Reject the audio connection indication.
 *
 * \return The status of sending audio connection confirmation.
 * \retval true     Audio connection confirmation was sent successfully.
 * \retval false    Audio connection confirmation was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_audio_connect_cfm(uint8_t bd_addr[6],
                              bool    accept);

/**
 * \brief  Send a request to disconnect audio connection.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_audio_disconnect_req(uint8_t bd_addr[6]);

/**
 * \brief  Send a command to enable voice recognition.
 *
 * \param[in]  bd_addr    Remote BT address
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_voice_recognition_enable_req(uint8_t bd_addr[6]);

/**
 * \brief  Send a command to disable voice recognition.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_voice_recognition_disable_req(uint8_t bd_addr[6]);

/**
 * \brief  Send a command to synchronize speaker gain level.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  level      Local gain level, range from 0 to 15.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_speaker_gain_level_report(uint8_t bd_addr[6],
                                      uint8_t level);

/**
 * \brief  Send a command to synchronize microphone gain level.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  level      Local gain level, range from 0 to 15.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_microphone_gain_level_report(uint8_t bd_addr[6],
                                         uint8_t level);

/**
 * \brief  Send a specific DTMF code to its network connection.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  code       DTMF codes that are '0'-'9','*','#','A','B','C','and 'D'.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_dtmf_code_transmit(uint8_t bd_addr[6],
                               char    code);

/**
 * \brief  Send a command to query the current network operator name.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_network_operator_name_query(uint8_t bd_addr[6]);

/**
 * \brief  Send a command to query the AG subscriber number information.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_subscriber_number_query(uint8_t bd_addr[6]);

/**
 * \brief  Send a command to report current battery level.
 *
 * \param[in]  bd_addr     Remote BT address.
 * \param[in]  level       The current battery level, range from 0 to 100.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_batt_level_report(uint8_t bd_addr[6],
                              uint8_t level);

/**
 * \brief  Send a command to disable the echo canceling and
 *         noise reduction functions resident in the AG.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr     Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_nrec_disable(uint8_t bd_addr[6]);

/**
 * \brief  Send a command to activate the AG individual indicators.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr            Remote BT address.
 * \param[in]  indicator_types    Activating indicator types \ref BT_HFP_INDICATOR_TYPE.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_indicator_activate(uint8_t  bd_addr[6],
                               uint32_t indicator_types);

/**
 * \brief  Send a command to deactivate the AG individual indicators.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr            Remote BT address.
 * \param[in]  indicator_types    Deactivating indicator types \ref BT_HFP_INDICATOR_TYPE.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_indicator_deactivate(uint8_t  bd_addr[6],
                                 uint32_t indicator_types);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BT_HFP_H_ */
