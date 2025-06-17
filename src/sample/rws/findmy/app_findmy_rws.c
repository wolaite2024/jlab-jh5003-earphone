#if F_APP_FINDMY_FEATURE_SUPPORT
#include <string.h>
#include "app_relay.h"
#include "app_findmy_rws.h"
#include "trace.h"
#include "app_cfg.h"
#include "ftl.h"
#include "fmc_api.h"
#include "fmna_constants_platform.h"
#include "fmna_platform_includes.h"
#include "fmna_state_machine.h"
#include "os_mem.h"

void app_findmy_relay_all(void)
{
    app_relay_async_single(APP_MODULE_TYPE_FINDMY, APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_1);
    app_relay_async_single(APP_MODULE_TYPE_FINDMY, APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_2);
    app_relay_async_single(APP_MODULE_TYPE_FINDMY, APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_3);
    app_relay_async_single(APP_MODULE_TYPE_FINDMY, APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_4);
    app_relay_async_single(APP_MODULE_TYPE_FINDMY, APP_REMOTE_MSG_FINDMY_PAIR_INFO_1);
    app_relay_async_single(APP_MODULE_TYPE_FINDMY, APP_REMOTE_MSG_FINDMY_PAIR_INFO_2);
}

void app_findmy_relay_token(void)
{
    app_relay_async_single(APP_MODULE_TYPE_FINDMY, APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_1);
    app_relay_async_single(APP_MODULE_TYPE_FINDMY, APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_2);
    app_relay_async_single(APP_MODULE_TYPE_FINDMY, APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_3);
    app_relay_async_single(APP_MODULE_TYPE_FINDMY, APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_4);
}

void app_findmy_relay_pair_info(void)
{
    app_relay_async_single(APP_MODULE_TYPE_FINDMY, APP_REMOTE_MSG_FINDMY_PAIR_INFO_1);
    app_relay_async_single(APP_MODULE_TYPE_FINDMY, APP_REMOTE_MSG_FINDMY_PAIR_INFO_2);
}

void app_findmy_handle_remote_conn_cmpl(void)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_PRIMARY)
    {
        APP_PRINT_INFO0("app_findmy_handle_remote_conn_cmpl:pri sync all info to sec");
        app_findmy_relay_all();
    }
}

void app_findmy_remote_handle_info_sync(uint8_t msg_type, uint32_t addr, uint8_t *info_data,
                                        uint16_t info_len)
{
    APP_PRINT_INFO2("app_findmy_remote_handle_info_sync: addr 0x%x,size %d", addr, info_len);
    if (info_data == NULL)
    {
        return;
    }

    switch (msg_type)
    {
    case APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_1:
        {
            if (fmc_flash_nor_erase(APPLE_AUTH_TOKEN_SAVE_ADDRESS, FMC_FLASH_NOR_ERASE_SECTOR))
            {
                if (fmc_flash_nor_write(addr, (void *)info_data, info_len))
                {
                    APP_PRINT_INFO2("app_findmy_remote_handle_info_sync: success addr 0x%x,size %d", addr, info_len);
                }
            }
        }
        break;

    case APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_2:
    case APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_3:
    case APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_4:
        if (fmc_flash_nor_write(addr, (void *)info_data, info_len))
        {
            APP_PRINT_INFO2("app_findmy_remote_handle_info_sync: success addr 0x%x,size %d", addr, info_len);
        }
        break;

    case APP_REMOTE_MSG_FINDMY_PAIR_INFO_1:
    case APP_REMOTE_MSG_FINDMY_PAIR_INFO_2:
        if (!ftl_save_to_storage((void *)info_data, addr, info_len))
        {
            APP_PRINT_INFO2("app_findmy_remote_handle_info_sync: success addr 0x%x,size %d", addr, info_len);
        }
        break;

    default:
        break;
    }
}

#if F_APP_ERWS_SUPPORT
void app_findmy_parse_cback(uint8_t msg_type, uint8_t *buf, uint16_t len,
                            T_REMOTE_RELAY_STATUS status)
{
    APP_PRINT_TRACE3("app_findmy_parse_cback: msg_type 0x%04x, status %d, len %d", msg_type, status,
                     len);

    switch (msg_type)
    {
    case APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_1:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (buf != NULL && len != 0)
            {
                uint32_t addr = FINDMY_BLE_AUTH_TOKEN_FLASH_ADDR_1;
                app_findmy_remote_handle_info_sync(msg_type, addr, buf, len);
            }
        }
        break;

    case APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_2:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (buf != NULL && len != 0)
            {
                uint32_t addr = FINDMY_BLE_AUTH_TOKEN_FLASH_ADDR_2;
                app_findmy_remote_handle_info_sync(msg_type, addr, buf, len);
            }
        }
        break;

    case APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_3:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (buf != NULL && len != 0)
            {
                uint32_t addr = FINDMY_BLE_AUTH_TOKEN_FLASH_ADDR_3;
                app_findmy_remote_handle_info_sync(msg_type, addr, buf, len);
            }
        }
        break;

    case APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_4:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (buf != NULL && len != 0)
            {
                uint32_t addr = FINDMY_BLE_AUTH_TOKEN_FLASH_ADDR_4;
                app_findmy_remote_handle_info_sync(msg_type, addr, buf, len);
            }
        }
        break;

    case APP_REMOTE_MSG_FINDMY_PAIR_INFO_1:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (buf != NULL && len != 0)
            {
                uint32_t addr = FINDMY_BLE_PAIR_INFO_FLASH_ADDR_1;
                app_findmy_remote_handle_info_sync(msg_type, addr, buf, len);
            }
        }
        break;

    case APP_REMOTE_MSG_FINDMY_PAIR_INFO_2:
        if (status == REMOTE_RELAY_STATUS_ASYNC_RCVD)
        {
            if (buf != NULL && len != 0)
            {
                uint32_t addr = FINDMY_BLE_PAIR_INFO_FLASH_ADDR_2;
                app_findmy_remote_handle_info_sync(msg_type, addr, buf, len);
            }
        }
        break;

    default:
        break;
    }
}

uint16_t app_findmy_relay_cback(uint8_t *buf, uint8_t msg_type, bool total)
{
    uint16_t payload_len = 0;
    uint8_t *msg_ptr = NULL;
    bool skip = true;
    uint16_t msg_len;
    uint8_t *p_findmy_info = os_mem_zalloc(OS_MEM_TYPE_DATA, FINDMY_NV_ERASE_MIN_SIZE);
    APP_PRINT_INFO1("app_findmy_relay_cback: msg_type %d", msg_type);

    switch (msg_type)
    {
    case APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_1:
        {
            payload_len = FINDMY_NV_ERASE_MIN_SIZE;

            if (p_findmy_info &&
                fmc_flash_nor_read(FINDMY_BLE_AUTH_TOKEN_FLASH_ADDR_1, p_findmy_info, payload_len))
            {
                msg_ptr = (uint8_t *)p_findmy_info;
            }
        }
        break;

    case APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_2:
        {
            payload_len = FINDMY_NV_ERASE_MIN_SIZE;

            if (p_findmy_info &&
                fmc_flash_nor_read(FINDMY_BLE_AUTH_TOKEN_FLASH_ADDR_2, p_findmy_info, payload_len))
            {
                msg_ptr = (uint8_t *)p_findmy_info;
            }
        }
        break;

    case APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_3:
        {
            payload_len = FINDMY_NV_ERASE_MIN_SIZE;

            if (p_findmy_info &&
                fmc_flash_nor_read(FINDMY_BLE_AUTH_TOKEN_FLASH_ADDR_3, p_findmy_info, payload_len))
            {
                msg_ptr = (uint8_t *)p_findmy_info;
            }
        }
        break;

    case APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_4:
        {
            payload_len = FINDMY_NV_ERASE_MIN_SIZE;

            if (p_findmy_info &&
                fmc_flash_nor_read(FINDMY_BLE_AUTH_TOKEN_FLASH_ADDR_4, p_findmy_info, payload_len))
            {
                msg_ptr = (uint8_t *)p_findmy_info;
            }
        }
        break;

    case APP_REMOTE_MSG_FINDMY_PAIR_INFO_1:
        {
            payload_len = FINDMY_NV_ERASE_PAIR_INFO_SIZE_1;

            if (p_findmy_info &&
                ftl_load_from_storage((uint8_t *)p_findmy_info, FTL_SAVE_PAIR_STATE_ADDR, payload_len) == 0)
            {
                msg_ptr = (uint8_t *)p_findmy_info;
            }
        }
        break;

    case APP_REMOTE_MSG_FINDMY_PAIR_INFO_2:
        {
            payload_len = FINDMY_NV_ERASE_PAIR_INFO_SIZE_2;

            if (p_findmy_info &&
                ftl_load_from_storage((uint8_t *)p_findmy_info, FTL_SAVE_BT_MAC_ADDR, payload_len) == 0)
            {
                msg_ptr = (uint8_t *)p_findmy_info;
            }
        }
        break;

    default:
        break;
    }

    msg_len = app_relay_msg_pack(buf, msg_type, APP_MODULE_TYPE_FINDMY, payload_len, msg_ptr, skip,
                                 total);
    if (p_findmy_info != NULL)
    {
        os_mem_free(p_findmy_info);
        p_findmy_info = NULL;
    }
    return msg_len;
}

bool app_findmy_relay_init(void)
{
    bool ret = app_relay_cback_register(app_findmy_relay_cback, app_findmy_parse_cback,
                                        APP_MODULE_TYPE_FINDMY, APP_REMOTE_MSG_FINDMY_MAX_MSG_NUM);

    return ret;
}
#endif
#endif
