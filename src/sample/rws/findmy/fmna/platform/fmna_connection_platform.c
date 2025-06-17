/*
 *      Copyright (C) 2020 Apple Inc. All Rights Reserved.
 *
 *      Find My Network ADK is licensed under Apple Inc's MFi Sample Code License Agreement,
 *      which is contained in the License.txt file distributed with the Find My Network ADK,
 *      and only to those who accept that license.
 */

#if F_APP_FINDMY_FEATURE_SUPPORT
#include "fmna_connection_platform.h"
#include "fmna_platform_includes.h"
#include "fmna_constants.h"
#include "fmna_connection.h"
#include "fmna_gatt_platform.h"
#include "fmna_state_machine.h"
#include "fmna_peer_manager.h"
#include <gap_conn_le.h>
#include "fmc_api.h"
#include "app_findmy_rws.h"

bool disc_cause_timeout = false;
static void fmna_connection_token_storage_init(void);

fmna_ret_code_t fmna_connection_platform_disconnect(uint16_t conn_handle)
{
    return le_disconnect((uint8_t)conn_handle);
}

void fmna_connection_platform_gap_params_init(void)
{
    fmna_connection_token_storage_init();
}


void fmna_connection_platform_conn_params_init(void)
{
//    fmna_ret_code_t ret_code;

}

/// Handle encryption request for Find My Network accessories.
///
/// @details    This function bypasses Peer Manager handling. It is heavily based on
///             sec_info_request_process function from security_dispatcher.c.
//static void fmna_sec_info_request_process()
//{
//    uint8_t ltk[16];

//    // Copy over our own LTK
//    memcpy(ltk, fmna_connection_get_active_ltk(), GAP_SEC_KEY_LEN);
////TODO: perhaps need modify long term key while request sec info
//
//}


void fmna_ble_peripheral_evt(T_IO_MSG *p_gap_msg)
{
    T_LE_GAP_MSG gap_msg;
    memcpy(&gap_msg, &p_gap_msg->u.param, sizeof(p_gap_msg->u.param));
    APP_PRINT_TRACE1("fmna_ble_peripheral_evt: subtype %d", p_gap_msg->subtype);
    switch (p_gap_msg->subtype)
    {
    case GAP_MSG_LE_CONN_STATE_CHANGE:
        if (gap_msg.msg_data.gap_conn_state_change.new_state == GAP_CONN_STATE_CONNECTED)
        {
            uint16_t conn_interval;
            pm_conn_exclude_add(gap_msg.msg_data.gap_conn_state_change.conn_id);
            le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval,
                              gap_msg.msg_data.gap_conn_state_change.conn_id);
            fmna_connection_connected_handler(gap_msg.msg_data.gap_conn_state_change.conn_id,
                                              conn_interval);
            fmna_on_connect(gap_msg.msg_data.gap_conn_state_change.conn_id);
        }
        else if (gap_msg.msg_data.gap_conn_state_change.new_state == GAP_CONN_STATE_DISCONNECTED)
        {
            if (gap_msg.msg_data.gap_conn_state_change.disc_cause != (HCI_ERR | HCI_ERR_CONN_TIMEOUT))
            {
                disc_cause_timeout = false;
            }
            else
            {
                disc_cause_timeout = true;
            }
            pm_conn_exclude_delete(gap_msg.msg_data.gap_conn_state_change.conn_id);
            fmna_connection_disconnected_handler(gap_msg.msg_data.gap_conn_state_change.conn_id,
                                                 gap_msg.msg_data.gap_conn_state_change.disc_cause);
            fmna_on_disconnect(gap_msg.msg_data.gap_conn_state_change.conn_id);
        }
        break;

    case GAP_MSG_LE_CONN_PARAM_UPDATE:
        if (gap_msg.msg_data.gap_conn_param_update.status == GAP_CONN_PARAM_UPDATE_STATUS_SUCCESS)
        {
            uint16_t conn_interval;
            le_get_conn_param(GAP_PARAM_CONN_INTERVAL, &conn_interval,
                              gap_msg.msg_data.gap_conn_state_change.conn_id);
            fmna_connection_conn_param_update_handler(gap_msg.msg_data.gap_conn_state_change.conn_id,
                                                      conn_interval);
        }
        break;

//ZZZFindMy: security update
    case GAP_MSG_LE_AUTHEN_STATE_CHANGE:
        if (gap_msg.msg_data.gap_authen_state.new_state == GAP_AUTHEN_STATE_COMPLETE)
        {
//            if (fmna_connection_is_fmna_paired())
//            {
//                // mark as encrypted in the connection record
//                fmna_connection_update_connection_info(gap_msg.msg_data.gap_authen_state.conn_id,
//                                                       FMNA_MULTI_STATUS_ENCRYPTED,
//                                                       true);

//                // BT pairing completed successfully/ link was encrypted. Send BONDED event to state machine.
//                fmna_evt_handler(FMNA_SM_EVENT_BONDED, NULL);
//            }
            fmna_pm_conn_sec_handle(gap_msg.msg_data.gap_authen_state.conn_id);
        }
        break;

    /* TODO: confirm security request, need apply another LTK?
        case BLE_GAP_EVT_SEC_INFO_REQUEST:
            if (fmna_connection_is_fmna_paired())
            {
                NRF_LOG_INFO("FMNA BLE_GAP_EVT_SEC_INFO_REQUEST");

                fmna_sec_info_request_process(p_gap_evt);
            }
            break;
    */

    /* TODO: reject pairing if fmna_connection_is_fmna_paired() returns TRUE
        case BLE_GAP_EVT_SEC_PARAMS_REQUEST:
            {
                NRF_LOG_INFO("FMNA BLE_GAP_EVT_SEC_PARAMS_REQUEST");

                if (fmna_connection_is_fmna_paired())
                {
                    // Reject the incoming security request if we are already FMNA paired.
                    NRF_LOG_ERROR("FMNA Already paired. Reject request.");

                    ret_code_t ret_code = sd_ble_gap_sec_params_reply(p_gap_evt->conn_handle,
                                                                      BLE_GAP_SEC_STATUS_PAIRING_NOT_SUPP, NULL, NULL);
                    if (NRF_SUCCESS != ret_code)
                    {
                        NRF_LOG_ERROR("sd_ble_gap_sec_params_reply err 0x%x", ret_code);
                    }
                }

            } break;
    */
    default:
        // no implementation needed
        break;
    }
}

void fmna_connection_platform_log_token_help(void *auth_token, uint16_t token_size, void *auth_uuid,
                                             uint16_t uuid_size)
{
    APP_PRINT_INFO3("fmna_connection_platform_log_token_help: MFi token UUID %b, uuid_size %d, token_size %d",
                    TRACE_BINARY(uuid_size, auth_uuid), uuid_size, token_size);
}

#include "os_sched.h"
/* TODO: token log, fill in later  */
#define MFI_TOKEN_MAX_LOG_CHUNK 64
void fmna_connection_platform_log_token(void *auth_token, uint16_t token_size, uint8_t isCrash)
{
    uint16_t token_remaining = token_size;
    uint8_t *p_temp = auth_token;
    uint16_t to_print;

    // cppcheck-suppress syntaxError
    FMNA_LOG_INFO("fmna_connection_platform_log_token: MFi Token");
    while (token_remaining)
    {
        if (token_remaining > MFI_TOKEN_MAX_LOG_CHUNK)
        {
            to_print = MFI_TOKEN_MAX_LOG_CHUNK;
        }
        else
        {
            to_print = token_remaining;
        }
        FMNA_LOG_HEXDUMP_INFO(p_temp, to_print);
        token_remaining -= to_print;
        p_temp += to_print;
        os_delay(10);
    }
}


char num_to_char(uint8_t nibble)
{
    if (nibble < 10)
    {
        return (('0' + nibble));
    }

    return (('a' + nibble - 10));
}
//#include "system_rtl876x.h"
//TODO: confirm spec, any SN can be accepted?
void fmna_connection_platform_get_serial_number(uint8_t *pSN, uint8_t length)
{
    uint8_t temp[8];
    uint16_t remaining = length;
    int i = 0;
    uint8_t bt_addr[6];
    gap_get_param(GAP_PARAM_BD_ADDR, bt_addr);
//    uint8_t chip_id = get_ic_type();
    uint8_t chip_id = *(uint8_t *)0x002028f3;
    // xor device id and bd addr to identify the device
    *((uint32_t *)temp) =  chip_id;
    *((uint32_t *)temp) ^= *((uint32_t *)bt_addr);

    *((uint32_t *)(temp + 4)) =  chip_id;
    *((uint32_t *)(temp + 4)) ^= *((uint32_t *)(bt_addr + 4));

    // Convert to a character string
    for (; i < 8 && remaining; ++i)
    {
        pSN[2 * i] = num_to_char((temp[i] & 0x0f));
        remaining--;
        if (remaining)
        {
            pSN[2 * i + 1] = num_to_char(((temp[i] >> 4) & 0x0f));
            remaining--;
        }
    }

    // Pad remaining with 'f'
    if (remaining)
    {
        pSN[i] = 'f';
        remaining--;
        i++;
    }
}

// TODO remove / replace this with POR storage
// Using fstorage as temp storage for Token
// Please note this implementation is not 100% safe
// There is a window after erase and before write completes
// that there is no Token stored. A reset in this window could
// cause the device to become unusable.
static void *m_p_update_token_data;
static uint16_t m_update_token_size;

bool m_token_is_erasing = false;
bool m_new_token_stored = false;

static void fmna_connection_token_storage_init(void)
{
    //store token, FTL preferred
}

void fmna_connection_update_mfi_token_storage(void *p_data, uint16_t data_size)
{
    m_p_update_token_data = p_data;
    m_update_token_size = data_size;
    uint8_t m_temp_auth_buffer[SOFTWARE_AUTH_TOKEN_BLEN + SOFTWARE_AUTH_UUID_BLEN];

    if (data_size != (SOFTWARE_AUTH_TOKEN_BLEN + SOFTWARE_AUTH_UUID_BLEN))
    {
        FMNA_LOG_ERROR("fmna_connection_update_mfi_token_storage: token length not accepted");
        m_new_token_stored = false;
        return;
    }
    else
    {
        bool m_flash_op_result = false;
        //Backup token
        m_flash_op_result = fmc_flash_nor_read(APPLE_AUTH_TOKEN_SAVE_ADDRESS,
                                               (uint8_t *)m_temp_auth_buffer, SOFTWARE_AUTH_TOKEN_BLEN + SOFTWARE_AUTH_UUID_BLEN);
        if (!m_flash_op_result)
        {
            m_new_token_stored = false;
            return;
        }
        m_flash_op_result = fmc_flash_nor_erase(APPLE_AUTH_TOKEN_BACKUP_ADDRESS,
                                                FMC_FLASH_NOR_ERASE_SECTOR);
        if (!m_flash_op_result)
        {
            m_new_token_stored = false;
            return;
        }
        m_flash_op_result = fmc_flash_nor_write(APPLE_AUTH_TOKEN_BACKUP_ADDRESS,
                                                (uint8_t *)m_temp_auth_buffer, SOFTWARE_AUTH_TOKEN_BLEN + SOFTWARE_AUTH_UUID_BLEN);
        if (!m_flash_op_result)
        {
            m_new_token_stored = false;
            return;
        }
        if (memcmp((const void *)APPLE_AUTH_TOKEN_BACKUP_ADDRESS, (const void *)m_temp_auth_buffer,
                   SOFTWARE_AUTH_TOKEN_BLEN + SOFTWARE_AUTH_UUID_BLEN))
        {
            FMNA_LOG_ERROR("fmna_connection_update_mfi_token_storage: token backup region not matched");
            m_new_token_stored = false;
            return;
        }
        FMNA_LOG_INFO("fmna_connection_update_mfi_token_storage: token update");
        //Update token
        m_flash_op_result = fmc_flash_nor_erase(APPLE_AUTH_TOKEN_SAVE_ADDRESS, FMC_FLASH_NOR_ERASE_SECTOR);
        if (!m_flash_op_result)
        {
            m_new_token_stored = false;
            return;
        }
        m_flash_op_result = fmc_flash_nor_write(APPLE_AUTH_TOKEN_SAVE_ADDRESS,
                                                (uint8_t *)m_p_update_token_data, m_update_token_size);
        if (!m_flash_op_result)
        {
            m_new_token_stored = false;
            return;
        }

        if (memcmp((const void *)APPLE_AUTH_TOKEN_SAVE_ADDRESS, m_p_update_token_data,
                   SOFTWARE_AUTH_TOKEN_BLEN + SOFTWARE_AUTH_UUID_BLEN))
        {
            FMNA_LOG_ERROR("fmna_connection_update_mfi_token_storage: token backup region not matched");
            m_new_token_stored = false;
            return;
        }
        m_new_token_stored = true;
        fmna_state_machine_dispatch_event(FMNA_SM_EVENT_FMNA_PAIRING_MFITOKEN);

        if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
        {
            app_findmy_relay_all();
        }
    }

//find a ftl addr for token
//    ftl_save(m_p_update_token_data, FTL_SAVE_TOKEN_ADDR, m_update_token_size);
}

bool fmna_connection_mfi_token_stored(void)
{
    return m_new_token_stored;
}

#endif
