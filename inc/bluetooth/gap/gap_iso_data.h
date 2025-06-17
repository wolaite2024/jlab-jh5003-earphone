/**
*********************************************************************************************************
*               Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      gap_iso_data.h
* @brief     Header file for GAP ISO Data
* @details
* @author
* @date      2021-07-12
* @version   v0.8
* *********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef _GAP_ISO_DATA_H_
#define _GAP_ISO_DATA_H_

#ifdef  __cplusplus
extern "C"
{
#endif

/*============================================================================*
 *                        Header Files
 *============================================================================*/

/** @addtogroup BT_Host Bluetooth Host
  * @{
  */

/** @addtogroup GAP_LE GAP LE Module
  * @{
  */

/** @addtogroup GAP_LE_ISO_DATA GAP LE ISO Data Module
  * @brief GAP LE ISO Data Module
  * @{
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/** @defgroup GAP_LE_ISO_DATA_Exported_Functions GAP LE ISO Data Exported Functions
  * @brief
  * @{
  */

/**
 * @brief       Release the ISO Data buffer that GAP supplied.
 *
 * @note This function is used for @ref ISOCH_ROLE_INITIATOR, @ref ISOCH_ROLE_ACCEPTOR and @ref BIG_MGR_ROLE_SYNC_RECEIVER.
 *
 * @param[in]   p_buf   Point the buffer that needs to release.
 *
 * @return Send request operation.
 * @retval GAP_CAUSE_SUCCESS  Operation success.
 * @retval Others             Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_gap_direct_callback(uint8_t cb_type, void *p_cb_data)
    {
        T_BT_DIRECT_CB_DATA *p_data = (T_BT_DIRECT_CB_DATA *)p_cb_data;

        ......
        switch (cb_type)
        {
        case BT_DIRECT_MSG_ISO_DATA_IND:
            {
                // Start of the SDU
                uint8_t *p_iso_data = p_data->p_bt_direct_iso->p_buf + p_data->p_bt_direct_iso->offset;
                ......
                gap_iso_data_cfm(p_data->p_bt_direct_iso->p_buf);
            }
            break;
            ......
        }

    }
 * \endcode
 */
bool gap_iso_data_cfm(void *p_buf);

/**
 * @brief       Send isochronous data to Controller.
 *
 * @note This function is used for @ref ISOCH_ROLE_INITIATOR, @ref ISOCH_ROLE_ACCEPTOR and @ref BIG_MGR_ROLE_ISOC_BROADCASTER.
 *
 * @param[in]   p_data        Point to data to be sent.
 * @param[in]   handle        Connection handle of the CIS or BIS.
 * @param[in]   iso_sdu_len   Length of the SDU to be sent.
 * @param[in]   ts_flag       Indicates whether it contains time_stamp.
 *                            True: contain time_stamp.
 *                            False: does not contain time_stamp.
 * @param[in]   time_stamp    A time in microseconds. The time_stamp is used when @ref ts_flag is True.
 * @param[in]   pkt_seq_num   The sequence number of the SDU.
 *
 * @return Send request operation.
 * @retval GAP_CAUSE_SUCCESS  Send request operation success.
 * @retval Others             Send request operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void test(void)
    {
        T_GAP_CAUSE cause = gap_iso_send_data(p_data, handle, iso_sdu_len, ts_flag, time_stamp, pkt_seq_num);
    }
 * \endcode
 */
T_GAP_CAUSE gap_iso_send_data(uint8_t *p_data, uint16_t handle, uint16_t iso_sdu_len, bool ts_flag,
                              uint32_t time_stamp, uint16_t pkt_seq_num);

/** End of GAP_LE_ISO_DATA_Exported_Functions
  * @}
  */

/** End of GAP_LE_ISO_DATA
  * @}
  */

/** End of GAP_LE
  * @}
  */

/** End of BT_Host
  * @}
  */

#ifdef  __cplusplus
}
#endif

#endif /* _GAP_ISO_DATA_H_ */
