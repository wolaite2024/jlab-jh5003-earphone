#ifndef __BLE_ADV_DATA_INT_H
#define __BLE_ADV_DATA_INT_H


#include <stdbool.h>
#include "gap.h"
#include "gap_msg.h"
#include "gap_le_types.h"



/**
 *  @brief interface of handle connect msg
 *  @param[in]  conn_id    connection id whick refer to link
 *  @param[in]  new_state  new link connection state
 *  @param[in]  disc_cause  disconnect cause if new_state is disconnected
 */
void ble_adv_data_handle_conn_state(uint8_t conn_id, T_GAP_CONN_STATE new_state,
                                    uint16_t disc_cause);



/**
 *  @brief interface of init adv data manager
 *  @param[in]  adv_handle
 *  @param[in]  update_scan_data  support on not update scan rsp data while update adv data
 *  @return operation result
 *  @retval true  success
 *  @retval false failed
 */
bool ble_adv_data_init(bool update_scan_data, T_GAP_LOCAL_ADDR_TYPE own_address_type,
                       T_GAP_REMOTE_ADDR_TYPE peer_address_type,
                       uint32_t adv_interval, uint32_t scan_rsp_len, uint8_t *scan_rsp_data);

#endif
