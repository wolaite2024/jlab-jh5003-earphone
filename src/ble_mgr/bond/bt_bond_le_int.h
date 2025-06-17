/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      ftl_key_mgr_le_int.h
* @brief     key storage function.
* @details
* @author    jane
* @date      2016-02-18
* @version   v0.1
* *********************************************************************************************************
*/

#ifndef     BT_BOND_LE_INT_H
#define     BT_BOND_LE_INT_H

#ifdef __cplusplus
extern "C"
{
#endif

#if CONFIG_REALTEK_APP_BOND_MGR_SUPPORT
#include <bt_bond_ftl_int.h>
#include <bt_bond_le.h>

#define LE_BOND_EXIST_BIT            0x01
#define LE_LOCAL_IRK_DIST_BIT        0x02

void bt_le_key_init(void);
uint8_t bt_le_get_key(T_LE_BOND_ENTRY *p_entry, T_GAP_KEY_TYPE key_type, uint8_t *key);
bool bt_le_save_key(T_LE_BOND_ENTRY *p_entry, T_GAP_KEY_TYPE key_type, uint8_t key_length,
                    uint8_t *key);
T_LE_BOND_ENTRY *bt_le_allocate_key_entry(uint8_t *bd_addr, T_GAP_REMOTE_ADDR_TYPE bd_type,
                                          uint8_t *local_bd, uint8_t local_bd_type);

T_LE_BOND_ENTRY *bt_le_find_entry_by_aes(uint8_t *unresolved_addr, uint8_t addr_type,
                                         uint8_t *local_bd, uint8_t local_bd_type);

void bt_le_key_send_update_info(T_LE_BOND_ENTRY *p_entry);

bool bt_le_set_bond_state(T_LE_BOND_ENTRY *p_entry, uint8_t flags);
#endif

#ifdef __cplusplus
}
#endif

#endif /* FLASH_KEY_MGR_LE_INT_H */
