#ifndef __BLE_EXT_ADV_INT_H
#define __BLE_EXT_ADV_INT_H


#include <stdint.h>
#include "gap_callback_le.h"
#include "gap_msg.h"
#include "gap_ext_adv.h"


/**
*@brief used to handle extend adv start setting
*@note
*@param[in] cb_data  @ref T_LE_CB_DATA
*@return    none
* <b>Example usage</b>
 * \code{.c}
    static T_APP_RESULT app_ble_gap_cb(uint8_t cb_type, void *p_cb_data)
    {
    ......
        case GAP_MSG_LE_EXT_ADV_START_SETTING:
            ble_ext_adv_mgr_handle_gap_cb(cb_type, &cb_data);
            break;
    ......
    }
 * \endcode
*/
void ble_ext_adv_mgr_handle_gap_cb(uint8_t cb_type, T_LE_CB_DATA *p_data);


/**
*@brief used to handle conn state for each adv set
*@note
*@param[in] conn_id
*@param[in] new_state  @ref T_GAP_CONN_STATE
*@param[in] disc_cause
*@return    none
* <b>Example usage</b>
 * \code{.c}
    static void le_handle_new_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state,
                                         uint16_t disc_cause)
    {
        T_APP_LE_LINK *p_link;

        APP_PRINT_TRACE3("le_handle_new_conn_state_evt: conn_id %d, new_state %d, cause 0x%04x",
                         conn_id, new_state, disc_cause);
        ble_ext_adv_mgr_handle_conn_state(conn_id, new_state, disc_cause);
    }

    void app_ble_gap_handle_gap_msg(T_IO_MSG *p_bee_io_msg)
    {
        APP_PRINT_TRACE1("app_ble_gap_handle_gap_msg: subtype %d", p_bee_io_msg->subtype);
        T_LE_GAP_MSG stack_msg;
        T_APP_LE_LINK *p_link;

        memcpy(&stack_msg, &p_bee_io_msg->u.param, sizeof(p_bee_io_msg->u.param));

        switch (p_bee_io_msg->subtype)
        {
        case GAP_MSG_LE_DEV_STATE_CHANGE:
            {
                le_handle_dev_state_change_evt(stack_msg.msg_data.gap_dev_state_change.new_state,
                                               stack_msg.msg_data.gap_dev_state_change.cause);
            }
            break;

        case GAP_MSG_LE_CONN_STATE_CHANGE:
            {
                le_handle_new_conn_state_evt(stack_msg.msg_data.gap_conn_state_change.conn_id,
                                             (T_GAP_CONN_STATE)stack_msg.msg_data.gap_conn_state_change.new_state,
                                             stack_msg.msg_data.gap_conn_state_change.disc_cause);
            }
            break;
        }
    }
 * \endcode
*/
void ble_ext_adv_mgr_handle_conn_state(uint8_t conn_id, T_GAP_CONN_STATE new_state,
                                       uint16_t disc_cause);



/**
*@brief used to handle extend adv state
*@note
*@param[in] adv_handle ble adv handle, each adv set has a adv handle
*@param[in] new_state  @ref T_GAP_EXT_ADV_STATE
*@param[in] cause
*@return    none
* <b>Example usage</b>
 * \code{.c}
    void app_ble_gap_handle_gap_msg(T_IO_MSG *p_bee_io_msg)
    {
    ......
       case GAP_MSG_LE_EXT_ADV_STATE_CHANGE:
        {
            ble_ext_adv_mgr_handle_adv_state(stack_msg.msg_data.gap_ext_adv_state_change.adv_handle,
                                         (T_GAP_EXT_ADV_STATE)stack_msg.msg_data.gap_ext_adv_state_change.new_state,
                                         stack_msg.msg_data.gap_ext_adv_state_change.cause);
        }
        break;
    ......
    }
 * \endcode
*/
void ble_ext_adv_mgr_handle_adv_state(uint8_t adv_handle, T_GAP_EXT_ADV_STATE new_state,
                                      uint16_t cause);


/**
*@brief    Allocate storage space for ble_ext_adv_set_table and create a queue to store extend advertising event
*@note     This api shall be used first if you want to use the module of ble_ext_adv_mgr
*@param[in] adv_handle_num the maximum of ble adv handle, each adv set has a adv handle
*@return   T_GAP_CAUSE
*@retval   GAP_CAUSE_SUCCESS  success
*@retval   GAP_CAUSE_INVALID_PARAM  the adv_handle_num is 0
*@retval   GAP_CAUSE_NO_RESOURCE  no space in RAM_TYPE_DATA_ON to allocate
*
 * <b>Example usage</b>
 * \code{.c}
    void app_ble_gap_init(void)
    {
        ble_ext_adv_mgr_init(MAX_ADV_SET_NUMBER);
    }
 * \endcode
*/
T_GAP_CAUSE ble_ext_adv_mgr_init(uint8_t adv_handle_num);

#endif

