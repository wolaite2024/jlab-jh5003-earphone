#ifndef __BLE_CONN_INT_H
#define __BLE_CONN_INT_H

#include <stdint.h>
#include <stdbool.h>
#include "gap_msg.h"



/**
*@brief  used to handle @ref T_GAP_CONN_STATE
*@note
*@param[in]  T_GAP_CONN_STATE  new_state
*@param[in]  conn_id  ble conn id
*@return  none
*@retval  none
*/
void ble_handle_conn_state_evt(uint8_t conn_id, T_GAP_CONN_STATE new_state);



/**
*@brief  used to handle @ref T_GAP_CONN_PARAM_UPDATE
*@note
*@param[in]  T_GAP_CONN_PARAM_UPDATE  update_info
*@return  none
*@retval  none
*/
void ble_handle_conn_update_info(T_GAP_CONN_PARAM_UPDATE update_info);


/**
* @brief  ble conn update init is used to create ble_update_table
* @param[in]  link_num the ble link number
* @return  bool
* @retval  true  success
* @retval  false  fail
*/

bool ble_conn_update_init(uint8_t link_num);
#endif

