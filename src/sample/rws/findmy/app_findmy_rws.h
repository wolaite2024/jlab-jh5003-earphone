#ifndef __APP_FINDMY_RWS_H_
#define __APP_FINDMY_RWS_H_

#include <stdbool.h>
#include <stdint.h>
#include "fmna_constants_platform.h"
#include "fmna_crypto.h"
#include "fmna_connection.h"
#include "fmna_state_machine.h"

//nv
/* The minimum size of flash erasure. May be a flash sector size. */
#define FINDMY_NV_ERASE_MIN_SIZE                    (0x100)

#define FINDMY_NV_ERASE_PAIR_INFO_SIZE_1            (FTL_SAVE_BT_MAC_ADDR - FTL_SAVE_PAIR_STATE_ADDR)
#define FINDMY_NV_ERASE_PAIR_INFO_SIZE_2            (FTL_SAVE_ICLOUD_ID_ADDR + FTL_SAVE_ICLOUD_ID_SIZE - FTL_SAVE_BT_MAC_ADDR)

#define FINDMY_BLE_AUTH_UUID_FLASH_ADDR             (SOFTWARE_AUTH_UUID_ADDR)
#define FINDMY_BLE_AUTH_TOKEN_FLASH_ADDR_1          (SOFTWARE_AUTH_TOKEN_ADDR)
#define FINDMY_BLE_AUTH_TOKEN_FLASH_ADDR_2          (SOFTWARE_AUTH_TOKEN_ADDR+FINDMY_NV_ERASE_MIN_SIZE)
#define FINDMY_BLE_AUTH_TOKEN_FLASH_ADDR_3          (SOFTWARE_AUTH_TOKEN_ADDR+FINDMY_NV_ERASE_MIN_SIZE*2)
#define FINDMY_BLE_AUTH_TOKEN_FLASH_ADDR_4          (SOFTWARE_AUTH_TOKEN_ADDR+FINDMY_NV_ERASE_MIN_SIZE*3)

#define FINDMY_BLE_PAIR_INFO_FLASH_ADDR_1           (FTL_SAVE_PAIR_STATE_ADDR)
#define FINDMY_BLE_PAIR_INFO_FLASH_ADDR_2           (FTL_SAVE_PAIR_STATE_ADDR+FINDMY_NV_ERASE_MIN_SIZE)

/* the flash write granularity, unit: byte*/
#define FINDMY_NV_WRITE_GRAN                        (4)

/* area size. */
#define FINDMY_NV_AREA_SIZE                         (4*FINDMY_NV_ERASE_MIN_SIZE)

typedef enum
{
    APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_1                    = 0x00,
    APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_2                    = 0x01,
    APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_3                    = 0x02,
    APP_REMOTE_MSG_FINDMY_AUTH_TOKEN_4                    = 0x03,
    APP_REMOTE_MSG_FINDMY_PAIR_INFO_1                     = 0x04,
    APP_REMOTE_MSG_FINDMY_PAIR_INFO_2                     = 0x05,
    APP_REMOTE_MSG_FINDMY_MAX_MSG_NUM                     = 0x06,
} T_APP_FINDMY_REMOTE_MSG;

void app_findmy_relay_all(void);
void app_findmy_relay_token(void);
void app_findmy_relay_pair_info(void);
void app_findmy_handle_remote_conn_cmpl(void);
bool app_findmy_relay_init(void);

#endif
