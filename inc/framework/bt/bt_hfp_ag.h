/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */


#ifndef _BT_HFP_AG_H_
#define _BT_HFP_AG_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \defgroup    BT_HFP_AG BT HFP AG Profile
 *
 * \brief   Provide BT HFP AG profile interfaces.
 */

/**
 * \defgroup BT_HFP_AG_LOCAL_SUPPORTED_FEATURES BT HFP AG Local Supported Features
 *
 * \brief Define BT HFP AG local supported features.
 *
 * \ingroup BT_HFP
 * @{
 */

/**
 * Three-way call handling feature. If HF also support three-way calling, HF can handle three-way
 * calling by using AT+CHLD.
 */
#define BT_HFP_AG_LOCAL_CAPABILITY_3WAY                   (1 << 0)
/**
 * Echo canceling (EC) and noise reduction (NR) feature. If HF also support echo canceling and
 * noise reduction, HF can request turning off the EC and NR by using AT+NREC.
 */
#define BT_HFP_AG_LOCAL_CAPABILITY_EC_NR                  (1 << 1)
/**
 * Voice recognition feature. If HF also support voice recognition, HF can use AT+BVRA command or
 * AG autonomously to activate/deactivate the voice recognition function resident in the AG.
 */
#define BT_HFP_AG_LOCAL_CAPABILITY_VOICE_RECOGNITION      (1 << 2)
/**
 * Incoming call in-band ring tone feature. If in-band ring tone is used, the AG shall send the
 * ring tone to the HF via the established audio connection.
 */
#define BT_HFP_AG_LOCAL_CAPABILITY_INBAND_RINGING         (1 << 3)
/**
 * Attach a voice tag to a phone number. It provides a means to read numbers from the AG for the
 * purpose of creating a unique voice tag and storing the number and its linked voice tag in the
 * memory of HF.
 */
#define BT_HFP_AG_LOCAL_CAPABILITY_VOICE_TAG              (1 << 4)
/**
 * If the AG has the capability to reject incoming calls.
 */
#define BT_HFP_AG_LOCAL_CAPABILITY_REJECT_CALLS           (1 << 5)
/**
 * Query call list feature. If both the HF and AG support enhanced call status, HF can query the
 * list of current calls in AG by AT+CLCC.
 */
#define BT_HFP_AG_LOCAL_CAPABILITY_ENHANCED_CALL_STATUS   (1 << 6)
/**
 * Release specified call feature. the enhanced call control mechanism is simply an extension of
 * the current AT+CHLD command. These extensions are defined as additional arguments to the
 * AT+CHLD command.
 */
#define BT_HFP_AG_LOCAL_CAPABILITY_ENHANCED_CALL_CONTROL  (1 << 7)
/**
 * If HF can enable/disable extended audio gateway error result codes in the AG.
 */
#define BT_HFP_AG_LOCAL_CAPABILITY_EXTENED_ERROR_RESULT   (1 << 8)
/**
 * Codec Negotiation feature. If Wide Band Speech is supported, Codec Negotiation shall also be
 * supported. If both the HF and AG do support the Codec Negotiation feature, the available codecs
 * supported on both sides can be negotiated.
 */
#define BT_HFP_AG_LOCAL_CAPABILITY_CODEC_NEGOTIATION      (1 << 9)
/**
 * HF indicator feature. If both the HF and AG support the HF Indicator feature, the HF can send the
 * AT+BIEV command with the corresponding HF indicator value whenever a change in value occurs of an
 * enabled HF indicator.
 */
#define BT_HFP_AG_LOCAL_CAPABILITY_HF_INDICATORS          (1 << 10)
/**
 * If both the HF and AG support the eSCO S4 settings, the parameter sets S4 can be used when eSCO
 * logical transports established.
 */
#define BT_HFP_AG_LOCAL_CAPABILITY_ESCO_S4_SUPPORTED   (1 << 11)
/**
 * @}
 */

/**
 * \defgroup BT_HFP_AG_REMOTE_SUPPORTED_FEATURES BT HFP AG Remote Supported Features
 *
 * \brief Define BT HFP AG Remote Supported Features.
 *
 * \ingroup BT_HFP_AG
 * @{
 */

/**
 * Echo canceling (EC) and noise reduction (NR) feature. If AG also support echo canceling and noise
 * reduction, HF can request turning off the EC and NR by using AT+NREC.
 */
#define BT_HFP_AG_REMOTE_EC_NR_FUNCTION               (1 << 0)
/**
 * Three-way call handling feature. If AG also support three-way calling, HF can handle three-way
 * calling by using AT+CHLD.
 */
#define BT_HFP_AG_REMOTE_THREE_WAY_CALLING            (1 << 1)
/**
 * Calling Line Identification (CLI) notification feature. If the HF has enabled the Calling Line
 * Identification, when the calling subscriber number information is available from the network,
 * the AG shall issue the +CLIP unsolicited result code just after every RING indication when the
 * HF is alerted in an incoming call.
 */
#define BT_HFP_AG_REMOTE_CLI_PRESENTATION_CAPABILITY  (1 << 2)
/**
 * Voice recognition feature. If AG also support voice recognition, HF can use AT+BVRA command or
 * AG autonomously to activate/deactivate the voice recognition function resident in the AG.
 */
#define BT_HFP_AG_REMOTE_VOICE_RECOGNITION_ACTIVATION (1 << 3)
/**
 * Remote audio volume control and volume level synchronization feature. This function enables the
 * user to modify the speaker volume and microphone gain of the HF from the AG. It also allows the
 * HF to inform the AG of the current gain settings corresponding to the speaker volume and
 * microphone gain.
 */
#define BT_HFP_AG_REMOTE_REMOTE_VOLUME_CONTROL        (1 << 4)
/**
 * Query call list feature. If both the HF and AG support enhanced call status, HF can query the
 * list of current calls in AG by AT+CLCC.
 */
#define BT_HFP_AG_REMOTE_ENHANCED_CALL_STATUS         (1 << 5)
/**
 * Release specified call feature. the enhanced call control mechanism is simply an extension of
 * the current AT+CHLD command. These extensions are defined as additional arguments to the
 * AT+CHLD command.
 */
#define BT_HFP_AG_REMOTE_ENHANCED_CALL_CONTROL        (1 << 6)
/**
 * Codec Negotiation feature. If Wide Band Speech is supported, Codec Negotiation shall also be
 * supported. If both the HF and AG do support the Codec Negotiation feature, the available codecs
 * supported on both sides can be negotiated.
 */
#define BT_HFP_AG_REMOTE_CODEC_NEGOTIATION            (1 << 7)
/**
 * HF indicator feature. If both the HF and AG support the HF indicator feature, the HF can send the
 * AT+BIEV command with the corresponding HF indicator value whenever a change in value occurs of an
 * enabled HF indicator.
 */
#define BT_HFP_AG_REMOTE_HF_INDICATORS                (1 << 8)
/**
 * If both the HF and AG support the eSCO S4 settings, the parameter sets S4 can be used when eSCO
 * logical transports established.
 */
#define BT_HFP_AG_REMOTE_ESCO_S4_SETTINGS             (1 << 9)
/**
 * @}
 */

/**
 * \defgroup BT_HFP_AG_EXTENDED_ERROR_RESULT_CODE BT HFP AG Extended Audio Gateway Error Result Code
 *
 * \brief Define BT HFP AG extended audio gateway error result code.
 *
 * \ingroup BT_HFP_AG
 * @{
 */
/**/
#define BT_HFP_AG_ERR_AG_FAILURE           0  /* AG failure. */
#define BT_HFP_AG_ERR_NO_CONN_PHONE        1  /* No connection to phone. */
#define BT_HFP_AG_ERR_OP_NOT_ALLOWED       3  /* Operation not allowed. */
#define BT_HFP_AG_ERR_OP_NOT_SUPPORTED     4  /* Operation not supported. */
#define BT_HFP_AG_ERR_PHSIM_PIN_REQ        5  /* PH-SIM PIN required. */
#define BT_HFP_AG_ERR_SIM_NOT_INSERTED     10 /* SIM not inserted. */
#define BT_HFP_AG_ERR_SIM_PIN_REQ          11 /* SIM PIN required. */
#define BT_HFP_AG_ERR_SIM_PUK_REQ          12 /* SIM PUK required. */
#define BT_HFP_AG_ERR_SIM_FAILURE          13 /* SIM failure. */
#define BT_HFP_AG_ERR_SIM_BUSY             14 /* SIM busy. */
#define BT_HFP_AG_ERR_INCORRECT_PWD        16 /* Incorrect password. */
#define BT_HFP_AG_ERR_SIM_PIN2_REQ         17 /* SIM PIN2 required. */
#define BT_HFP_AG_ERR_SIM_PUK2_REQ         18 /* SIM PUK2 required. */
#define BT_HFP_AG_ERR_MEMORY_FULL          20 /* Memory full. */
#define BT_HFP_AG_ERR_INVALID_INDEX        21 /* Invalid index. */
#define BT_HFP_AG_ERR_MEMORY_FAILURE       23 /* Memory failure. */
#define BT_HFP_AG_ERR_TEXT_TOO_LONG        24 /* Text string too long. */
#define BT_HFP_AG_ERR_INVALID_CHAR_IN_TSTR 25 /* Invalid characters in text string. */
#define BT_HFP_AG_ERR_DSTR_TOO_LONG        26 /* Dial string too long. */
#define BT_HFP_AG_ERR_INVALID_CHAR_IN_DSTR 27 /* Invalid characters in dial string. */
#define BT_HFP_AG_ERR_NO_NETWORK_SERVICE   30 /* No network service. */
#define BT_HFP_AG_ERR_NETWORK_TIMEOUT      31 /* Network timeout. */
#define BT_HFP_AG_ERR_NETWORK_NOT_ALLOWED  32 /* Network not allowed - emergency service only. */
/**
 * @}
 */

/**
 * \defgroup BT_HFP_AG_SUPPORTED_CODEC_TYPE BT HFP AG Supported Codec Type
 *
 * \brief Define BT HFP AG supported codec type.
 *
 * \ingroup BT_HFP_AG
 * @{
 */
#define BT_HFP_AG_CODEC_TYPE_CVSD               (1 << 0)
#define BT_HFP_AG_CODEC_TYPE_MSBC               (1 << 1)
#define BT_HFP_AG_CODEC_TYPE_LC3                (1 << 2)
/**
 * @}
 */

/**
 * \brief  Define BT HFP AG call status.
 *
 * \ingroup BT_HFP_AG
 */
typedef enum t_bt_hfp_ag_call_status
{
    /**
     * No call in progress.
     */
    BT_HFP_AG_CALL_IDLE                     = 0x00,
    /**
     * An incoming call is ongoing.
     */
    BT_HFP_AG_CALL_INCOMING                 = 0x01,
    /**
     * An outgoing call is ongoing.
     */
    BT_HFP_AG_CALL_OUTGOING                 = 0x02,
    /**
     * An active call is in progress.
     */
    BT_HFP_AG_CALL_ACTIVE                   = 0x03,
    /**
     * A call is held.
     */
    BT_HFP_AG_CALL_HELD                     = 0x04,
    /**
     * One call active and another call incoming in three-way calling.
     */
    BT_HFP_AG_CALL_ACTIVE_WITH_CALL_WAITING = 0x05,
    /**
     * One call active and another call held in three-way calling.
     */
    BT_HFP_AG_CALL_ACTIVE_WITH_CALL_HOLD    = 0x06,
} T_BT_HFP_AG_CALL_STATUS;

/**
 * \brief  Define BT HFP AG service indicator.
 *
 * \ingroup BT_HFP_AG
 */
typedef enum t_bt_hfp_ag_service_indicator
{
    /**
     * Service status of AG is available.
     */
    BT_HFP_AG_SERVICE_STATUS_AVAILABLE     = 0x00,
    /**
     * Service status of AG is unavailable.
     */
    BT_HFP_AG_SERVICE_STATUS_UNAVAILABLE   = 0x01,
} T_BT_HFP_AG_SERVICE_INDICATOR;

/**
 * \brief  Define BT HFP AG call indicator.
 *
 * \ingroup BT_HFP_AG
 */
typedef enum t_bt_hfp_ag_call_indicator
{
    /**
     * No call in progress.
     */
    BT_HFP_AG_NO_CALL_IN_PROGRESS  = 0x00,
    /**
     * An active call in progress.
     */
    BT_HFP_AG_CALL_IN_PROGRESS     = 0x01,
} T_BT_HFP_AG_CALL_INDICATOR;

/**
 * \brief  Define BT HFP AG call setup indicator.
 *
 * \ingroup BT_HFP_AG
 */
typedef enum t_bt_hfp_ag_call_setup_indicator
{
    /**
     * No call in progress.
     */
    BT_HFP_AG_CALL_SETUP_STATUS_IDLE              = 0x00,
    /**
     * An incoming call is ongoing.
     */
    BT_HFP_AG_CALL_SETUP_STATUS_INCOMING_CALL     = 0x01,
    /**
     * An outgoing call is ongoing.
     */
    BT_HFP_AG_CALL_SETUP_STATUS_OUTGOING_CALL     = 0x02,
    /**
     * Remote party being alerted in an outgoing call.
     */
    BT_HFP_AG_CALL_SETUP_STATUS_ALERTING          = 0x03,
} T_BT_HFP_AG_CALL_SETUP_INDICATOR;

/**
 * \brief  Define BT HFP AG call held indicator.
 *
 * \ingroup BT_HFP_AG
 */
typedef enum t_bt_hfp_ag_call_held_indicator
{
    /**
     * No call be held.
     */
    BT_HFP_AG_CALL_HELD_STATUS_IDLE                  = 0x00,
    /**
     * One call active and another call held in three-way calling.
     */
    BT_HFP_AG_CALL_HELD_STATUS_HOLD_AND_ACTIVE_CALL  = 0x01,
    /**
     * Only one call be held.
     */
    BT_HFP_AG_CALL_HELD_STATUS_HOLD_NO_ACTIVE_CALL   = 0x02,
} T_BT_HFP_AG_CALL_HELD_INDICATOR;

/**
 * \brief  Define BT HFP AG roaming indicator.
 *
 * \ingroup BT_HFP_AG
 */
typedef enum t_bt_hfp_ag_roaming_indicator
{
    /**
     * Roaming status of AG is inactive.
     */
    BT_HFP_AG_ROAMING_STATUS_INACTIVE = 0x00,
    /**
     * Roaming status of AG is active.
     */
    BT_HFP_AG_ROAMING_STATUS_ACTIVE   = 0x01,
} T_BT_HFP_AG_ROAMING_INDICATOR;

/**
 * \brief  Define BT HFP AG current call status.
 *
 * \ingroup BT_HFP_AG
 */
typedef enum t_bt_hfp_ag_current_call_status
{
    /**
     * Call is in active.
     */
    BT_HFP_AG_CURRENT_CALL_STATUS_ACTIVE                    = 0x00,
    /**
     * Call is be held.
     */
    BT_HFP_AG_CURRENT_CALL_STATUS_HELD                      = 0x01,
    /**
     * An outgoing call is ongoing, outgoing calls only.
     */
    BT_HFP_AG_CURRENT_CALL_STATUS_DIALING                   = 0x02,
    /**
     * Remote party being alerted in an outgoing call, outgoing calls only.
     */
    BT_HFP_AG_CURRENT_CALL_STATUS_ALERTING                  = 0x03,
    /**
     * An incoming call is ongoing, incoming calls only.
     */
    BT_HFP_AG_CURRENT_CALL_STATUS_INCOMING                  = 0x04,
    /**
     * An incoming call is waiting, incoming calls only.
     */
    BT_HFP_AG_CURRENT_CALL_STATUS_WAITING                   = 0x05,
    /**
     * Call is be held by respond and hold.
     */
    BT_HFP_AG_CURRENT_CALL_STATUS_CALL_HELD_BY_RSP_AND_HOLD = 0x06,
} T_BT_HFP_AG_CURRENT_CALL_STATUS;

/**
 * \brief  Define BT HFP AG current call mode.
 *
 * \ingroup BT_HFP_AG
 */
typedef enum t_bt_hfp_ag_current_call_mode
{
    /**
     * Current call mode is voice.
     */
    BT_HFP_AG_CURRENT_CALL_MODE_VOICE = 0x00,
    /**
     * Current call mode is data.
     */
    BT_HFP_AG_CURRENT_CALL_MODE_DATA  = 0x01,
    /**
     * Current call mode is facsimile.
     */
    BT_HFP_AG_CURRENT_CALL_MODE_FAX   = 0x02,
} T_BT_HFP_AG_CURRENT_CALL_MODE;

/**
 * \brief   Initialize HFP AG profile.
 *
 * \param[in]  link_num                HFP HF/HSP AG maximum connected link number.
 * \param[in]  rfc_hfp_chann_num       RFCOMM channel number used for HFP AG.
 * \param[in]  rfc_hsp_chann_num       RFCOMM channel number used for HSP AG.
 * \param[in]  supported_features      AG supported features \ref BT_HFP_AG_LOCAL_SUPPORTED_FEATURE.
 * \param[in]  supported_codecs        AG supported codecs \ref BT_HFP_AG_SUPPORTED_CODEC_TYPE.
 *
 * \return          The status of initializing HFP AG profile.
 * \retval true     HFP AG profile was initialized successfully.
 * \retval false    HFP AG profile was failed to initialize.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_init(uint8_t  rfc_hfp_chann_num,
                    uint8_t  rfc_hsp_chann_num,
                    uint16_t supported_features,
                    uint8_t  supported_codecs);

/**
 * \brief   De-initialize HFP AG profile.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API"
 *
 * \ingroup BT_HFP_AG
 */
void bt_hfp_ag_deinit(void);

/**
 * \brief   Send an HFP AG connection request.
 *
 * \param[in]  bd_addr       Remote BT address.
 * \param[in]  server_chann  Server channel which can be found from the SDP info.
 * \param[in]  is_hfp        Choose to create an HFP or HSP connection.
 * \arg    true          Create an HFP connection.
 * \arg    false         Create an HSP connection.
 *
 * \return         The status of sending the HFP or HSP connection request.
 * \retval true    HFP or HSP connection request was sent successfully.
 * \retval false   HFP or HSP connection request was failed to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_connect_req(uint8_t bd_addr[6],
                           uint8_t server_chann,
                           bool    is_hfp);

/**
 * \brief   Send an HFP AG connection confirmation.
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
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_connect_cfm(uint8_t bd_addr[6],
                           bool    accept);

/**
 * \brief  Send an HFP AG disconnection request.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the HFP disconnection request.
 * \retval true    HFP disconnection request was sent successfully.
 * \retval false   HFP disconnection request was failed to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_disconnect_req(uint8_t bd_addr[6]);

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
 *         devices is a legacy device). AG implementations that comply with the Hands-Free Profile
 *         specification Rev. 0.96, 1.0 or 1.5, shall not indicate support for the codec negotiation
 *         feature and shall neither send the +BCS unsolicited response to the HF. In order to
 *         guarantee backward compatibility, HFP Rev. x.y implementations shall be able to handle
 *         establishment of synchronous connections according to Hands-Free profile specification
 *         Rev. 1.0 or 1.5.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_audio_connect_req(uint8_t bd_addr[6]);

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
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_audio_connect_cfm(uint8_t bd_addr[6],
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
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_audio_disconnect_req(uint8_t bd_addr[6]);

/**
 * \brief  Send an vendor AT command request.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  at_cmd     The start address of buffer that stores AT command. AT command
 *                        string must end with "\r\n".
 *
 * \return         The status of sending the AT command request.
 * \retval true    AT command request was sent successfully.
 * \retval false   AT command request was failed to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_send_vnd_at_cmd_req(uint8_t     bd_addr[6],
                                   const char *at_cmd);

/**
 * \brief  Send an OK result code.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the OK result code.
 * \retval true    OK result code was sent successfully.
 * \retval false   OK result code was failed to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_ok_send(uint8_t bd_addr[6]);

/**
 * \brief  Send an Error result code.
 *
 * \param[in]  bd_addr     Remote BT address.
 * \param[in]  error_code  HFP AG extended audio gateway error result code \ref BT_HFP_AG_EXTENDED_ERROR_RESULT_CODE.
 *
 * \return         The status of sending the error result code.
 * \retval true    Error result code was sent successfully.
 * \retval false   Error result code was failed to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_error_send(uint8_t bd_addr[6],
                          uint8_t error_code);

/**
 * \brief  Send HFP AG indicators during HFP service level connection establishment.
 *
 * \param[in]  bd_addr              Remote BT address.
 * \param[in]  service_indicator    HFP AG service availability indicator.
 * \param[in]  call_indicator       HFP AG standard call status indicator.
 * \param[in]  call_setup_indicator HFP AG call set up status indicator.
 * \param[in]  call_held_indicator  HFP AG call hold status indicator.
 * \param[in]  signal_indicator     HFP AG signal strength indicator.
 * \param[in]  roaming_indicator    HFP AG roaming status indicator.
 * \param[in]  batt_chg_indicator   HFP AG battery charge indicator.
 *
 * \return         The status of sending the command request.
 * \retval true    Request has been sent successfully.
 * \retval false   Request was fail to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_indicators_send(uint8_t                          bd_addr[6],
                               T_BT_HFP_AG_SERVICE_INDICATOR    service_indicator,
                               T_BT_HFP_AG_CALL_INDICATOR       call_indicator,
                               T_BT_HFP_AG_CALL_SETUP_INDICATOR call_setup_indicator,
                               T_BT_HFP_AG_CALL_HELD_INDICATOR  call_held_indicator,
                               uint8_t                          signal_indicator,
                               T_BT_HFP_AG_ROAMING_INDICATOR    roaming_indicator,
                               uint8_t                          batt_chg_indicator);

/**
 * \brief  Notify the incoming call.
 *
 * \param[in]  bd_addr         Remote BT address.
 * \param[in]  call_num        HFP AG incoming call number.
 * \param[in]  call_num_len    HFP AG incoming call number length with the maximum of 20 including '\0'.
 * \param[in]  call_num_type   HFP AG incoming call number type.
 *
 * \return         The status of sending the command request.
 * \retval true    Command has been sent successfully.
 * \retval false   Command was fail to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_call_incoming(uint8_t     bd_addr[6],
                             const char *call_num,
                             uint8_t     call_num_len,
                             uint8_t     call_num_type);

/**
 * \brief  Answer the incoming or outgoing alerting call.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command has been sent successfully.
 * \retval false   Command was fail to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_call_answer(uint8_t bd_addr[6]);

/**
 * \brief  Hold the active call.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command has been sent successfully.
 * \retval false   Command was fail to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_call_hold(uint8_t bd_addr[6]);

/**
 * \brief  Accept the held call. The call will be active.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command has been sent successfully.
 * \retval false   Command was fail to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_held_call_accept(uint8_t bd_addr[6]);

/**
 * \brief  Release the held call.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API"
 *
 * \return         The status of sending the command request.
 * \retval true    Command has been sent successfully.
 * \retval false   Command was fail to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_held_call_release(uint8_t bd_addr[6]);

/**
 * \brief  Reject the incoming call, terminate the outgoing call, or ongoing call.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command has been sent successfully.
 * \retval false   Command was fail to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_call_terminate(uint8_t bd_addr[6]);

/**
 * \brief  Dial the outgoing call.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command has been sent successfully.
 * \retval false   Command was fail to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_call_dial(uint8_t bd_addr[6]);

/**
 * \brief  Notify the outgoing call alerted.
 *
 * \param[in]  bd_addr    Remote BT address.
 *
 * \return         The status of sending the command request.
 * \retval true    Command has been sent successfully.
 * \retval false   Command was fail to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_call_alert(uint8_t bd_addr[6]);

/**
 * \brief  send the call waiting notification
 *
 * \param[in]  bd_addr         Remote BT address.
 * \param[in]  call_num        HFP AG call waiting number.
 * \param[in]  call_num_len    HFP AG call waiting number length with the maximum of 20 including '\0'.
 * \param[in]  call_num_type   HFP AG call waiting number type.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_call_waiting_send(uint8_t     bd_addr[6],
                                 const char *call_num,
                                 uint8_t     call_num_len,
                                 uint8_t     call_num_type);

/**
 * \brief  Set HF microphone volume level.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  level      HF microphone gain level to set that ranges from 0 to 15.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_microphone_gain_set(uint8_t bd_addr[6],
                                   uint8_t level);

/**
 * \brief  Set HF speaker volume level.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  level      HF speaker gain level to set that ranges from 0 to 15.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_speaker_gain_set(uint8_t bd_addr[6],
                                uint8_t level);

/**
 * \brief  Send service availability indicator.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  status     HFP AG service availability indicator.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_service_indicator_send(uint8_t                       bd_addr[6],
                                      T_BT_HFP_AG_SERVICE_INDICATOR status);

/**
 * \brief  Send call indicator.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  status     HFP AG call indicator.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_call_indicator_send(uint8_t                    bd_addr[6],
                                   T_BT_HFP_AG_CALL_INDICATOR status);

/**
 * \brief  Send call setup indicator.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  status     HFP AG call setup indicator.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_call_setup_indicator_send(uint8_t                          bd_addr[6],
                                         T_BT_HFP_AG_CALL_SETUP_INDICATOR status);

/**
 * \brief  Send call hold indicator.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  status     HFP AG call hold indicator.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_call_held_indicator_send(uint8_t                         bd_addr[6],
                                        T_BT_HFP_AG_CALL_HELD_INDICATOR status);

/**
 * \brief  Send signal strength indicator.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  value      HFP AG signal strength, ranges from 0 to 5.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_signal_strength_send(uint8_t bd_addr[6],
                                    uint8_t value);

/**
 * \brief  Send roaming indicator.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  status     HFP AG roaming indicator.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_roaming_indicator_send(uint8_t                       bd_addr[6],
                                      T_BT_HFP_AG_ROAMING_INDICATOR status);

/**
 * \brief  Send battery charge indicator.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  value      HFP AG battery level, ranges from 0 to 5.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_battery_charge_send(uint8_t bd_addr[6],
                                   uint8_t value);

/**
 * \brief  Set AG ring interval.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  interval   AG ring interval in milliseconds with default value as 5000ms.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_ring_interval_set(uint8_t  bd_addr[6],
                                 uint16_t interval);

/**
 * \brief  Change HFP or HSP AG in-band ring tone setting dynamically after service
 *         level connection establishment.
 *
 * \param[in]  bd_addr    Remote BT address.
 * \param[in]  enable     Enable HFP or HSP AG in-band ring tone or not dynamically.
 *
 * \return         The status of changing HFP or HSP AG in-band ring tone setting dynamically.
 * \retval true    Success to change HFP or HSP AG in-band ring tone setting dynamically.
 * \retval false   Failed to change HFP or HSP AG in-band ring tone setting dynamically.
 *
 * \ingroup BT_HFP_AG
 */
bool bt_hfp_ag_inband_ringing_set(uint8_t bd_addr[6],
                                  bool    enable);

/**
 * \brief  Send HFP AG current calls list.
 *
 * \param[in]  bd_addr         Remote BT address.
 * \param[in]  call_idx        HFP AG current call index.
 * \param[in]  call_dir        HFP AG current call direction.
 * \arg    0   outgoing.
 * \arg    1   incoming.
 * \param[in]  call_status     HFP AG current call status.
 * \param[in]  call_mode       HFP AG current call mode.
 * \param[in]  mpty            HFP AG current multi-party call or not.
 * \arg    0   this call is not a member of a multi-party (conference) call.
 * \arg    1   this call IS a member of a multi-party (conference) call.
 * \param[in]  call_num        HFP AG current call number.
 * \param[in]  call_num_type   HFP AG current call number type.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP_AG
*/
bool bt_hfp_ag_current_calls_list_send(uint8_t                          bd_addr[6],
                                       uint8_t                          call_idx,
                                       uint8_t                          call_dir,
                                       T_BT_HFP_AG_CURRENT_CALL_STATUS  call_status,
                                       T_BT_HFP_AG_CURRENT_CALL_MODE    call_mode,
                                       uint8_t                          mpty,
                                       const char                      *call_num,
                                       uint8_t                          call_num_type);

/**
* \brief  Indicate the currently selected operator.
*
* \param[in]  bd_addr         Remote BT address.
* \param[in]  operator        HFP AG the currently selected operator.
* \param[in]  operator_len    HFP AG the currently selected operator length with the maximum of 17 including '\0'.
*
* \return         The status of sending the command request.
* \retval true    Command has been sent successfully.
* \retval false   Command was fail to send.
*
* \ingroup BT_HFP_AG
*/
bool bt_hfp_ag_network_operator_name_send(uint8_t     bd_addr[6],
                                          const char *operator,
                                          uint8_t     operator_len);

/**
* \brief  Send subscriber number information.
*
* \param[in]  bd_addr         Remote BT address.
* \param[in]  call_num        HFP AG subscriber number.
* \param[in]  call_num_len    HFP AG subscriber number length with the maximum of 20 including '\0'.
* \param[in]  call_num_type   HFP AG subscriber number type.
* \param[in]  service         HFP AG service the subscriber number relates to, Shall be either 4 (voice) or 5 (fax).
*
* \return         The status of sending the command request.
* \retval true    Command has been sent successfully.
* \retval false   Command was fail to send.
*
* \ingroup BT_HFP_AG
*/
bool bt_hfp_ag_subscriber_number_send(uint8_t     bd_addr[6],
                                      const char *call_num,
                                      uint8_t     call_num_len,
                                      uint8_t     call_num_type,
                                      uint8_t     service);

/**
 * \brief  Send a command to inform HF of activating/deactivating
 *         the voice recognition function of AG.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API"
 *
 * \param[in]  bd_addr     Remote BT address.
 * \param[in]  enable      Activate/deactivate the voice recognition function.
 *
 * \return         The status of sending the command request.
 * \retval true    Command request was sent successfully.
 * \retval false   Command request was failed to send.
 *
 * \ingroup BT_HFP
 */
bool bt_hfp_ag_voice_recognition_set(uint8_t bd_addr[6],
                                     bool    enable);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BT_HFP_AG_H_ */
