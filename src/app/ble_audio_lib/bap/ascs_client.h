#ifndef _ASCS_CLIENT_H_
#define _ASCS_CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ascs_def.h"
#include "ascs_def.h"
#include "codec_def.h"
#include "ble_audio_mgr.h"

#if LE_AUDIO_ASCS_CLIENT_SUPPORT

/**
 * \defgroup    LEA_GAF_ASCS_Client Audio Stream Control Client
 *
 * \brief   The client for Audio Stream Control Service.
 */

/**
 * ascs_client.h
 *
 * \brief  Define Codec Data Max Length.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
#define CODEC_DATA_MAX_LEN                 32

/**
 * ascs_client.h
 *
 * \defgroup LE_AUDIO_ASCS_MSG MCP LE Audio ASCS Message
 *
 * \brief  Define ASCS Client Message Type.
 *
 * \ingroup LEA_GAF_ASCS_Client
 * \{
 */
#define LE_AUDIO_MSG_ASCS_CLIENT_DIS_DONE          (LE_AUDIO_MSG_GROUP_ASCS_CLIENT | LE_AUDIO_MSG_INTERNAL | 0x00)
#define LE_AUDIO_MSG_ASCS_CLIENT_CP_NOTIFY         (LE_AUDIO_MSG_GROUP_ASCS_CLIENT | LE_AUDIO_MSG_INTERNAL |0x02)
#define LE_AUDIO_MSG_ASCS_CLIENT_READ_ASE_DATA     (LE_AUDIO_MSG_GROUP_ASCS_CLIENT | LE_AUDIO_MSG_INTERNAL |0x03)
#define LE_AUDIO_MSG_ASCS_CLIENT_ASE_DATA_NOTIFY   (LE_AUDIO_MSG_GROUP_ASCS_CLIENT | LE_AUDIO_MSG_INTERNAL |0x04)
#define LE_AUDIO_MSG_ASCS_CLIENT_READ_ASE_ALL      (LE_AUDIO_MSG_GROUP_ASCS_CLIENT | LE_AUDIO_MSG_INTERNAL |0x05)
#define LE_AUDIO_MSG_ASCS_CLIENT_SETUP_DATA_PATH   (LE_AUDIO_MSG_GROUP_ASCS_CLIENT | LE_AUDIO_MSG_INTERNAL |0x06)
#define LE_AUDIO_MSG_ASCS_CLIENT_REMOVE_DATA_PATH  (LE_AUDIO_MSG_GROUP_ASCS_CLIENT | LE_AUDIO_MSG_INTERNAL |0x07)
/**
 * End of LE_AUDIO_ASCS_MSG
 * \}
 */

/**
 * ascs_client.h
 *
 * \defgroup    ASCS_CLT_ROLE ASCS Client Role
 *
 * \brief  Define ASCS Client Role.
 *
 * \ingroup LEA_GAF_ASCS_Client
 * \{
 */
#define UNICAST_CLT_SRC_ROLE     0x10
#define UNICAST_CLT_SNK_ROLE     0x20
#define UNICAST_CLT_ROLE_MASK    0x30
/**
 * End of ASCS_CLT_ROLE
 * \}
 */

/**
 * ascs_client.h
 *
 * \brief  ASCS client discover service result.
 *         The message data for LE_AUDIO_MSG_ASCS_CLIENT_DIS_DONE.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
typedef struct
{
    uint16_t conn_handle;
    bool    is_found;
    bool    load_from_ftl;
    uint8_t snk_ase_num;
    uint8_t src_ase_num;
} T_ASCS_CLIENT_DIS_DONE;

/**
 * ascs_client.h
 *
 * \brief  ASCS client control point notify data.
 *         The message data for LE_AUDIO_MSG_ASCS_CLIENT_CP_NOTIFY.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t opcode;
    uint8_t number_of_ase;
    T_ASE_CP_NOTIFY_ARRAY_PARAM *p_param;
} T_ASCS_CLIENT_CP_NOTIFY;

/**
 * ascs_client.h
 *
 * \brief  ASCS client ASE data.
 *         The message for LE_AUDIO_MSG_ASCS_CLIENT_READ_ASE_DATA
 *         and LE_AUDIO_MSG_ASCS_CLIENT_ASE_DATA_NOTIFY.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t instance_id;
    bool notify;
    uint16_t cause;
    uint8_t direction;
    T_ASE_CHAR_DATA ase_data;
} T_ASCS_CLIENT_ASE_DATA;

/**
 * ascs_client.h
 *
 * \brief  ASCS client read all the ASE result.
 *         The message data for LE_AUDIO_MSG_ASCS_CLIENT_READ_ASE_ALL.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t snk_ase_num;
    uint8_t src_ase_num;
    uint16_t cause;
} T_ASCS_CLIENT_READ_ASE_ALL;

/**
 * ascs_client.h
 *
 * \brief  ASCS client setup data path parameter.
 *         The message data for LE_AUDIO_MSG_ASCS_CLIENT_SETUP_DATA_PATH.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t ase_id;
    uint8_t path_direction;        /**< DATA_PATH_INPUT_FLAG or DATA_PATH_OUTPUT_FLAG */
    uint16_t cis_conn_handle;
    T_CODEC_CFG codec_parsed_data;
    uint8_t  codec_cfg_len;
    uint8_t *p_codec_cfg;
} T_ASCS_CLIENT_SETUP_DATA_PATH;

/**
 * ascs_client.h
 *
 * \brief  ASCS client remove data path parameter.
 *         The message data for LE_AUDIO_MSG_ASCS_CLIENT_REMOVE_DATA_PATH.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
typedef struct
{
    uint16_t conn_handle;
    uint8_t ase_id;
    uint8_t path_direction;        /**< DATA_PATH_INPUT_FLAG or DATA_PATH_OUTPUT_FLAG */
    uint16_t cis_conn_handle;
    uint16_t cause;
} T_ASCS_CLIENT_REMOVE_DATA_PATH;

/**
 * ascs_client.h
 *
 * \brief  ASCS client control point codec configure parameter.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
typedef struct
{
    uint8_t ase_id;
    uint8_t target_latency;
    uint8_t target_phy;
    uint8_t codec_id[5];
    uint8_t codec_spec_cfg_len;
    uint8_t codec_spec_cfg[CODEC_DATA_MAX_LEN];
} T_ASE_CP_CODEC_CFG_PARAM;

/**
 * ascs_client.h
 *
 * \brief  ASCS client control point metadata parameter.
 *         Used for enable and update metadata operation.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
typedef struct
{
    uint8_t ase_id;
    uint8_t metadata_len;
    uint8_t *p_data;
} T_ASE_CP_METADATA_PARAM;

/**
 * ascs_client.h
 *
 * \brief  Enable the ASCS service cccd.
 *
 * \param[in]  conn_handle      Connection handle.
 *
 * \return         The result of enable ASCS service cccd.
 * \retval true    Enable ASCS service cccd success.
 * \retval false   Enable ASCS service cccd failed.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
void ascs_client_enable_cccd(uint16_t conn_handle);

/**
 * ascs_client.h
 *
 * \brief  ASCS client read ASE.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  instance_id      ASCS service instance id.
 * \param[in]  direction        Direction.
 *
 * \return         The result of read ASE.
 * \retval true    Read ASE success.
 * \retval false   Read ASE failed.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
bool ascs_client_read_ase(uint16_t conn_handle, uint8_t instance_id, uint8_t direction);

/**
 * ascs_client.h
 *
 * \brief  ASCS client read all the ASEs.
 *
 * \param[in]  conn_handle      Connection handle.
 *
 * \return         The result of read all the ASEs.
 * \retval true    Read all the ASEs success.
 * \retval false   Read all the ASEs failed.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
bool ascs_client_read_ase_all(uint16_t conn_handle);

/**
 * ascs_client.h
 *
 * \brief  ASCS client get ASE data by ASE id.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  ase_id           ASE id.
 * \param[in, out]  p_ase_data  Pointer to ASE characteristic data: @ref T_ASE_CHAR_DATA.
 *
 * \return         The result of get ASE data by ASE id.
 * \retval true    Get ASE data by ASE id success.
 * \retval false   Get ASE data by ASE id failed.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
bool ascs_client_get_ase_data_by_id(uint16_t conn_handle, uint8_t ase_id,
                                    T_ASE_CHAR_DATA *p_ase_data);

/**
 * ascs_client.h
 *
 * \brief  ASCS client get codec data.
 *
 * \param[in]  conn_handle        Connection handle.
 * \param[in]  ase_id             ASE id.
 * \param[in, out]  p_codec_data  Pointer to codec data: @ref T_ASE_DATA_CODEC_CONFIGURED.
 *
 * \return         The result of get codec data.
 * \retval true    Get codec data success.
 * \retval false   Get codec data failed.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
bool ascs_client_get_codec_data(uint16_t conn_handle, uint8_t ase_id,
                                T_ASE_DATA_CODEC_CONFIGURED *p_codec_data);

/**
 * ascs_client.h
 *
 * \brief  ASCS client get metadata.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  ase_id           ASE id.
 * \param[in, out]  p_metadata  Pointer to metadata: @ref T_ASE_DATA_WITH_METADATA.
 *
 * \return         The result of get metadata.
 * \retval true    Get metadata success.
 * \retval false   Get metadata failed.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
bool ascs_client_get_metadata(uint16_t conn_handle, uint8_t ase_id,
                              T_ASE_DATA_WITH_METADATA *p_metadata);

/**
 * ascs_client.h
 *
 * \brief  ASCS control point configure codec parameter.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  num_ases         ASE number.
 * \param[in]  p_codec          Pointer to codec parameter: @ref T_ASE_CP_CODEC_CFG_PARAM.
 *
 * \return         The result of ASCS control point configure codec parameter.
 * \retval true    ASCS control point configure codec parameter success.
 * \retval false   ASCS control point configure codec parameter failed.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
bool ase_cp_config_codec(uint16_t conn_handle, uint8_t num_ases,
                         T_ASE_CP_CODEC_CFG_PARAM *p_codec);

/**
 * ascs_client.h
 *
 * \brief  ASCS control point configure QoS parameter.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  num_ases         ASE number.
 * \param[in]  p_qos            Pointer to QoS parameter: @ref T_ASE_CP_QOS_CFG_ARRAY_PARAM.
 *
 * \return         The result of ASCS control point configure QoS parameter.
 * \retval true    ASCS control point configure QoS parameter success.
 * \retval false   ASCS control point configure QoS parameter failed.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
bool ase_cp_config_qos(uint16_t conn_handle, uint8_t num_ases,
                       T_ASE_CP_QOS_CFG_ARRAY_PARAM *p_qos);

/**
 * ascs_client.h
 *
 * \brief  ASCS control point action enable operation.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  num_ases         ASE number.
 * \param[in]  p_metadata       Pointer to metadata parameter: @ref T_ASE_CP_METADATA_PARAM.
 *
 * \return         The result of ASCS control point action enable operation.
 * \retval true    ASCS control point action enable operation success.
 * \retval false   ASCS control point action enable operation failed.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
bool ase_cp_enable(uint16_t conn_handle, uint8_t num_ases,
                   T_ASE_CP_METADATA_PARAM *p_metadata);

/**
 * ascs_client.h
 *
 * \brief  ASCS control point action update metadata operation.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  num_ases         ASE number.
 * \param[in]  p_metadata       Pointer to metadata parameter: @ref T_ASE_CP_METADATA_PARAM.
 *
 * \return         The result of ASCS control point action update metadata operation.
 * \retval true    ASCS control point action update metadata operation success.
 * \retval false   ASCS control point action update metadata operation failed.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
bool ase_cp_update_metadata(uint16_t conn_handle, uint8_t num_ases,
                            T_ASE_CP_METADATA_PARAM *p_metadata);

/**
 * ascs_client.h
 *
 * \brief  ASCS control point action disable operation.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  num_ases         ASE number.
 * \param[in]  p_ase_id         Pointer to ASE id.
 *
 * \return         The result of ASCS control point action disable operation.
 * \retval true    ASCS control point action disable operation success.
 * \retval false   ASCS control point action disable operation failed.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
bool ase_cp_disable(uint16_t conn_handle, uint8_t num_ases, uint8_t *p_ase_id);

/**
 * ascs_client.h
 *
 * \brief  ASCS control point action release operation.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  num_ases         ASE number.
 * \param[in]  p_ase_id         Pointer to ASE id.
 *
 * \return         The result of ASCS control point action release operation.
 * \retval true    ASCS control point action release operation success.
 * \retval false   ASCS control point action release operation failed.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
bool ase_cp_release(uint16_t conn_handle, uint8_t num_ases, uint8_t *p_ase_id);

/**
 * ascs_client.h
 *
 * \brief  ASCS client configure application control handshake.
 *         This API is only used when the server is audio source.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  ase_id           ASE id.
 * \param[in]  app_handshake    Application whether to control handshake.
 * \arg    true    Application control handshake.
 * \arg    false   Application not control handshake.
 *
 * \return         The result of configure application control handshake.
 * \retval true    ASCS client configure application control handshake success.
 * \retval false   ASCS client configure application control handshake failed.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
bool ascs_client_app_ctl_handshake(uint16_t conn_handle, uint8_t ase_id, bool app_handshake);

/**
 * ascs_client.h
 *
 * \brief  ASCS control point action receiver start ready operation.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  ase_id           ASE id.
 *
 * \return         The result of ASCS control point action receiver start ready operation.
 * \retval true    ASCS control point action receiver start ready operation success.
 * \retval false   ASCS control point action receiver start ready operation failed.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
bool ase_cp_rec_start_ready(uint16_t conn_handle, uint8_t ase_id);

/**
 * ascs_client.h
 *
 * \brief  ASCS control point action receiver stop ready operation.
 *
 * \param[in]  conn_handle      Connection handle.
 * \param[in]  ase_id           ASE id.
 *
 * \return         The result of ASCS control point action receiver stop ready operation.
 * \retval true    ASCS control point action receiver stop ready operation success.
 * \retval false   ASCS control point action receiver stop ready operation failed.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
bool ase_cp_rec_stop_ready(uint16_t conn_handle, uint8_t ase_id);

/**
 * ascs_client.h
 *
 * \brief  Initialize the AICS client.
 *
 * \param[in]  ascs_cfg      ASCS client role: @ref ASCS_CLT_ROLE.
 *
 * \return         The result of initialize the AICS client.
 * \retval true    Initialize the AICS client success.
 * \retval false   Initialize the AICS client failed.
 *
 * \ingroup LEA_GAF_ASCS_Client
 */
bool ascs_client_init(uint8_t ascs_cfg);

#endif
#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
