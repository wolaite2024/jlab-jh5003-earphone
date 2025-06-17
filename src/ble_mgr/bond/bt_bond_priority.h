/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      ftl_key_priorty_mgr.h
* @brief     key storage function.
* @details
* @author    jane
* @date      2016-02-18
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef     BT_BOND_PRIORITY_H
#define     BT_BOND_PRIORITY_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <bt_bond_ftl_int.h>

bool bt_bond_priority_queue_init(T_DEV_TYPE type, uint8_t max_bt_bond_num);
bool bt_bond_priority_queue_delete(T_DEV_TYPE type, uint8_t idx);
bool bt_bond_priority_queue_add(T_DEV_TYPE type, uint8_t idx);
void bt_bond_priority_queue_clear(T_DEV_TYPE type);
bool bt_bond_set_high_priority(T_DEV_TYPE type, uint8_t idx);
uint8_t bt_bond_get_low_priority(T_DEV_TYPE type);
uint8_t bt_bond_get_index_by_priority(T_DEV_TYPE type, uint8_t priority);
uint8_t bt_bond_priority_queue_find(T_DEV_TYPE type, uint8_t idx);

#ifdef __cplusplus
}
#endif

#endif /* FLASH_KEY_PRIORITY_MGR_H */
