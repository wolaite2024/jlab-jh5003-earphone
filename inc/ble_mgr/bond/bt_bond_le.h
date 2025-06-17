/**
*********************************************************************************************************
*               Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      bt_bond_le.h
* @brief     Key storage function.
* @details
* @author    Jane
* @date      2016-02-18
* @version   v1.0
* *********************************************************************************************************
*/

/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef     BT_BOND_LE_H
#define     BT_BOND_LE_H

#ifdef __cplusplus
extern "C"
{
#endif

/** @defgroup BT_BOND_LE Bluetooth Bond LE
  * @brief Bluetooth Bond LE Module
  * @{
  */

/*============================================================================*
 *                        Header Files
 *============================================================================*/
#include <gap.h>
#include <gap_le_types.h>
#include "bt_bond_common.h"

/*============================================================================*
 *                         Macros
 *============================================================================*/
/** @defgroup BT_BOND_LE_Exported_Macros Bluetooth Bond LE Exported Macros
  * @{
  */

/** @defgroup BT_BOND_LE_STORAGE_BITS LE Key Storage Bits
* @{
 */
#define LE_KEY_STORE_REMOTE_BD_BIT   0x01
#define LE_KEY_STORE_LOCAL_LTK_BIT   0x02
#define LE_KEY_STORE_REMOTE_LTK_BIT  0x04
#define LE_KEY_STORE_REMOTE_IRK_BIT  0x08
#define LE_KEY_STORE_CCCD_DATA_BIT   0x40
#define LE_KEY_STORE_LOCAL_IRK_BIT   0x80
#define LE_KEY_STORE_REMOTE_CLIENT_SUPPORTED_FEATURES_BIT  0x0100
#define LE_KEY_STORE_REMOTE_SRV_CHANGE_AWARE_STATE_BIT     0x0200
/**
  * @}
  */

/** @defgroup BT_BOND_LE_MODIFY_BITS LE Bond Modify Bits
* @{
 */
#define LE_BOND_MODIFY_NEW         0x01
#define LE_BOND_MODIFY_REPAIR      0x02
#define LE_BOND_MODIFY_CCCD        0x10
#define LE_BOND_MODIFY_REMOTE_SYNC 0x20
#define LE_BOND_MODIFY_FLAG        0x40

#define LE_BOND_LOCAL_ADDRESS_ANY  0xff
/**
  * @}
  */

/** End of BT_BOND_LE_Exported_Macros
  * @}
  */

/*============================================================================*
 *                         Types
 *============================================================================*/
/** @defgroup BT_BOND_LE_Exported_Types Bluetooth Bond LE Exported Types
  * @{
  */

/** @brief LE CCCD info */
typedef struct
{
    uint16_t        handle;
    uint16_t        ccc_bits;
} T_BT_CCCD_ELEM;

typedef struct
{
    uint8_t data_length;
    uint8_t padding[3];
    uint8_t data[1];
} T_BT_CCCD_DATA;

/** @brief LE key entry */
typedef struct
{
    bool is_used;
    uint8_t idx;
    uint16_t flags;
    uint16_t modify_flags;
    uint8_t remote_bd_type;
    uint8_t remote_identity_addr_type;
    uint8_t remote_bd[6];
    uint8_t remote_identity_addr[6];
    T_BT_BOND_INFO bond_info;
} T_LE_BOND_ENTRY;

typedef struct
{
    uint16_t modify_flags;
    T_LE_BOND_ENTRY *p_entry;
} T_BT_LE_BOND_ADD;

typedef struct
{
    uint16_t modify_flags;
    T_LE_BOND_ENTRY *p_entry;
} T_BT_LE_BOND_REMOVE;

typedef struct
{
    uint16_t modify_flags;
    T_LE_BOND_ENTRY *p_entry;
} T_BT_LE_BOND_UPDATE;

typedef struct
{
    uint16_t modify_flags;
} T_BT_LE_BOND_CLEAR;

typedef struct
{
    uint16_t modify_flags;
    T_LE_BOND_ENTRY *p_entry;
} T_BT_LE_SET_HIGH_PRIORITY;

/** @brief  Structure for BT_BOND_MSG_LE_BOND_GET.*/
typedef struct
{
    uint8_t         bd_addr[6];
    uint8_t         bd_type;
    uint8_t         local_bd_addr[6];
    uint8_t         local_bd_type;
    T_GAP_KEY_TYPE  key_type;           /**< Key type. */
    T_GAP_CFM_CAUSE cfm_cause;          /**< Provided by APP. */
    uint8_t         key_len;            /**< Provided by APP. */
    uint8_t         key_data[28];       /**< Provided by APP. */
} T_BT_LE_BOND_GET;

/** @brief  Structure for BT_BOND_MSG_LE_BOND_FULL.*/
typedef struct
{
    bool            new_bond;         /**<
                                         * \arg    true  : New pairing device which needs to save bond information.
                                         * \arg    false : The number of bonds is full when syncing bond information. */
    uint8_t         bd_addr[6];       /**< Used when the parameter new_bond is true. */
    uint8_t         bd_type;          /**< Used when the parameter new_bond is true. */
    uint8_t         local_bd_addr[6]; /**< Used when the parameter new_bond is true. */
    uint8_t         local_bd_type;    /**< Used when the parameter new_bond is true. */
} T_BT_LE_BOND_FULL;

typedef union
{
    T_BT_LE_BOND_ADD          *p_le_bond_add;
    T_BT_LE_BOND_REMOVE       *p_le_bond_remove;
    T_BT_LE_BOND_UPDATE       *p_le_bond_update;
    T_BT_LE_SET_HIGH_PRIORITY *p_le_bond_high_priority;
    T_BT_LE_BOND_CLEAR        *p_le_bond_clear;
    T_BT_LE_BOND_GET          *p_le_bond_get;
    T_BT_LE_BOND_FULL         *p_le_bond_full;
} T_BT_LE_BOND_CB_DATA;
/** End of BT_BOND_LE_Exported_Types
  * @}
  */

/*============================================================================*
 *                         Functions
 *============================================================================*/
/**
 * @defgroup BT_BOND_LE_Exported_Functions Bluetooth Bond LE Exported Functions
 *
 * @{
 */

/**
 * @brief Find key entry by Bluetooth device address and address type.
 * @param[in] bd_addr Remote Bluetooth device address.
 * @param[in] bd_type Remote Bluetooth device address type.
 * @param[in] local_bd_addr Local Bluetooth device address.
 * @param[in] local_bd_type Local Bluetooth device address type.
 * @return Pointer to the found key entry: @ref T_LE_BOND_ENTRY.
 * @retval null    No entry found.
 * @retval Others  Pointer to the found key entry.
 */
T_LE_BOND_ENTRY *bt_le_find_key_entry(uint8_t *bd_addr, T_GAP_REMOTE_ADDR_TYPE bd_type,
                                      uint8_t *local_bd_addr, uint8_t local_bd_type);

/**
 * @brief Find key entry number by remote identity address and remote identity address type.
 * @param identity_bd_addr Remote identity address.
 * @param identity_bd_type Remote identity address type.
 * @return Key entry number.
 */
uint8_t bt_le_find_key_entry_num(uint8_t *identity_bd_addr, uint8_t identity_bd_type);

/**
 * @brief Find key entry by index.
 * @param[in] idx Key entry index.
 * @return Pointer to the found key entry: @ref T_LE_BOND_ENTRY.
 * @retval null     No entry found.
 * @retval Others   Pointer to the found key entry.
 */
T_LE_BOND_ENTRY *bt_le_find_key_entry_by_idx(uint8_t idx);

/**
 * @brief Find key entry by priority.
 * @param[in] priority          Indicate device priority. Range: [1, bt_le_get_bond_dev_num()]
 *                          priority=1 indicates the highest priority device.
 *                          priority=bt_le_get_bond_dev_num() indicates the lowest priority device.
 * @return Pointer to the key entry: @ref T_LE_BOND_ENTRY.
 * @retval null   No entry found.
 * @retval Others Pointer to the found key entry.
 */
T_LE_BOND_ENTRY *bt_le_find_key_entry_by_priority(uint8_t priority);

/**
 * @brief Get bonded device numbers
 * @return Bonded device nuamber.
 */
uint8_t bt_le_get_bond_dev_num(void);

/**
 * @brief Get the low priority bond device key entry.
 * @return Pointer to the key entry: @ref T_LE_BOND_ENTRY.
 */
T_LE_BOND_ENTRY *bt_le_get_low_priority_bond(void);

/**
 * @brief Set the high priority bond device key entry.
 * @param[in] p_entry Pointer to the key entry of the bonded device: @ref T_LE_BOND_ENTRY.
 * @return   Operation result.
 * @retval   true   Success.
 * @retval   false  Failed.
 */
bool bt_le_set_high_priority_bond(T_LE_BOND_ENTRY *p_entry);

/**
 * @brief Resolve random address of the specified bonded device.
 * @param[in]       unresolved_addr       Unresolved remote Bluetooth device address.
 * @param[in,out]   resolved_addr         Resolved remote Bluetooth device address.
 * @param[in,out]   resolved_addr_type    Resolved remote Bluetooth device address type.
 * @return Operation result.
 * @retval true     Operation success.
 * @retval false    Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_handle_authen_state_evt(uint8_t conn_id, uint8_t new_state, uint16_t status)
    {
        switch (new_state)
        {
        ......
        case GAP_AUTHEN_STATE_COMPLETE:
           {
               ......
               if (status == 0)
               {
                    uint8_t addr[6];
                    T_GAP_REMOTE_ADDR_TYPE bd_type;
                    uint8_t resolved_addr[6];
                    T_GAP_IDENT_ADDR_TYPE resolved_bd_type;
                    le_get_conn_addr(conn_id, addr, &bd_type);
                    if (bd_type == GAP_REMOTE_ADDR_LE_RANDOM)
                    {
                        if (bt_le_resolve_random_address(addr, resolved_addr, &resolved_bd_type))
                        {
                            APP_PRINT_INFO2("GAP_AUTHEN_STATE_COMPLETE: resolved_addr %s, resolved_addr_type %d",
                                            TRACE_BDADDR(resolved_addr), resolved_bd_type);
                        }
                        else
                        {
                            APP_PRINT_INFO0("GAP_AUTHEN_STATE_COMPLETE: resolved addr failed");
                        }
                    }
               }
               ......
           }
           break;
           ...
        }
    }
 * \endcode
 */
bool bt_le_resolve_random_address(uint8_t *unresolved_addr, uint8_t *resolved_addr,
                                  T_GAP_IDENT_ADDR_TYPE *resolved_addr_type);

/**
 * @brief Get maximum pairing information number that can be stored.
 *
 * This function can be called after @ref le_gap_init is invoked.
 *
 * @return Maximum number of LE paired device information that can be stored.
 */
uint8_t bt_le_get_max_le_paired_device_num(void);

/**
 * @brief Get device LTK information.
 * @param[in] p_entry        Pointer to the key entry of bonded device.
 * @param[in] remote         Read the remote LTK or the local LTK.
 * @param[out] p_key_len     Pointer to LTK key length to read.
 * @param[out] p_ltk         Pointer to LTK to read.
 * @return Operation result.
 * @retval true     Operation success.
 * @retval false    Operation failure.
 */
bool bt_le_get_dev_ltk(T_LE_BOND_ENTRY *p_entry, bool remote, uint8_t *p_key_len,
                       uint8_t *p_ltk);
/**
 * @brief Update device LTK information.
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Experimental Added API"
 * @param[in] p_entry        Pointer to the key entry of bonded device.
 * @param[in] remote         Set the remote LTK or the local LTK.
 * @param[in] key_len        LTK key length to set.
 * @param[in] p_ltk          Pointer to LTK to set.
 * @return Operation result.
 * @retval true     Operation success.
 * @retval false    Operation failure.
 *
 * <b>Example usage</b>
 * \code{.c}
   void test(uint8_t new_ltk[16])
   {
       bt_le_set_dev_ltk(p_entry, false, 16, new_ltk);
   }
 * \endcode
 */
bool bt_le_set_dev_ltk(T_LE_BOND_ENTRY *p_entry, bool remote, uint8_t key_len,
                       uint8_t *p_ltk);
/**
 * @brief Get device IRK information.
 * @param[in] p_entry        Pointer to the key entry of bonded device.
 * @param[in] remote         Read the remote IRK or the local IRK.
 * @param[out] p_irk         Pointer to IRK to read.
 * @return Operation result.
 * @retval true     Operation success.
 * @retval false    Operation failure.
 */
bool bt_le_get_dev_irk(T_LE_BOND_ENTRY *p_entry, bool remote, uint8_t *p_irk);

/**
 * @brief Erase all keys of bonded devices.
 * @return  void.
 *
 * <b>Example usage</b>
 * \code{.c}
    void app_ble_device_handle_factory_reset(void)
    {
        ......
        #if CONFIG_REALTEK_APP_BOND_MGR_SUPPORT
            bt_le_clear_all_keys();
        #else
            le_bond_clear_all_keys();
        #endif
        ......
    }

    void ble_bond_sync_cb(uint8_t cb_type, void *p_cb_data)
    {
        ......
        switch (cb_type)
        {
            ......
            case BT_BOND_MSG_LE_BOND_CLEAR:
                {
                    if ((cb_data.p_le_bond_clear->modify_flags & LE_BOND_MODIFY_REMOTE_SYNC) == 0)
                    {
                        ble_bond_sync_send_info(APP_BT_BOND_CLEAR, NULL, 0);
                    }
                }
                break;
            ......
        }
    }
 * \endcode
 */
void bt_le_clear_all_keys(void);

/**
 * @brief Delete bond information by entry.
 * @param[in] p_entry Pointer to the key entry of bonded device.
 */
void bt_le_delete_bond(T_LE_BOND_ENTRY *p_entry);

/**
 * \brief Set bond flag of a particular LE bond.
 * \xrefitem Added_API_2_11_1_0 "Added Since 2.11.1.0" "Added API"
 * @param[in] p_entry Pointer to the key entry of bonded device.
 * \param[in] flag    Bond flag value to be set.
 * \return Result of setting bond flag.
 * \retval true  Set bond flag successfully.
 * \retval false Failed to set bond flag.
 *
 * <b>Example usage</b>
 * \code{.c}
   int test(void)
   {
       uint8_t idx = 0;
       uint16_t bond_flag = 0x01;
       T_LE_BOND_ENTRY *p_entry = bt_le_find_key_entry_by_idx(idx);
       if (p_entry)
       {
           bt_le_set_bond_flag(p_entry, bond_flag);
       }
   }
   \endcode
 */
bool bt_le_set_bond_flag(T_LE_BOND_ENTRY *p_entry, uint16_t flag);

/**
 * \brief Get bond flag stored in FTL for a particular LE bond.
 * \xrefitem Added_API_2_11_1_0 "Added Since 2.11.1.0" "Added API"
 * @param[in]  p_entry Pointer to the key entry of bonded device.
 * \param[out] p_flag  Bond flag of the LE bond.
 * \return Result of getting bond flag.
 * \retval true  Get bond flag successfully.
 * \retval false Failed to get bond flag.
 *
 * <b>Example usage</b>
 * \code{.c}
   int test(void)
   {
       uint8_t idx = 0;
       uint16_t bond_flag = 0;
       T_LE_BOND_ENTRY *p_entry = bt_le_find_key_entry_by_idx(idx);
       if (p_entry)
       {
           bt_le_get_bond_flag(p_entry, &bond_flag);
       }
   }
   \endcode
 */
bool bt_le_get_bond_flag(T_LE_BOND_ENTRY *p_entry, uint16_t *p_flag);

/**
 * \brief  Configure local IRK information to resolve the local privacy address.
 * \xrefitem Added_API_2_13_1_0 "Added Since 2.13.1.0" "Added API"
 * \param[in] local_identity_address_type Local identity address type.
 * \param[in] local_identity_address      Local identity address.
 * \param[in] local_irk                   Pointer to local IRK (16 octets).
 * \return Result of set local IRK.
 * \retval true  Set local IRK successfully.
 * \retval false Failed to set local IRK.
  */
bool bt_le_set_local_irk(uint8_t local_identity_address_type,
                         uint8_t *local_identity_address, uint8_t *local_irk);
/** End of BT_BOND_LE_Exported_Functions
  * @}
  */
/** End of BT_BOND_LE
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* BT_BOND_LE_H */
