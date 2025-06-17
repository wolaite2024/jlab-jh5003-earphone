/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      spdif_driver.h
* @brief     This file provides all the SPDIF firmware functions.
* @details
* @author    mh_chang
* @date      2023-01-30
* @version   v1.0
* *********************************************************************************************************
*/

#ifndef _SPDIF_DRIVER_H_
#define _SPDIF_DRIVER_H_

#include <stdbool.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum t_spdif_format_type
{
    SPDIF_FORMAT_LINEAR_PCM,
    SPDIF_FORMAT_NON_AUDIO,
} T_SPDIF_FORMAT_TYPE;

typedef enum t_spdif_data_len
{
    SPDIF_DATA_LEN_8_BIT,       // invalid, sync with SPORT
    SPDIF_DATA_LEN_16_BIT,
    SPDIF_DATA_LEN_20_BIT,      // invalid, sync with SPORT
    SPDIF_DATA_LEN_24_BIT,
    SPDIF_DATA_LEN_32_BIT,      // invalid, sync with SPORT
    SPDIF_DATA_LEN_MAX,
} T_SPDIF_DATA_LEN;

typedef enum t_spdif_config_dir
{
    SPDIF_CONFIG_TX = 1,
    SPDIF_CONFIG_RX,
    SPDIF_CONFIG_TRX,    // for bitmap usage
} T_SPDIF_CONFIG_DIR;

typedef enum t_spdif_role
{
    SPDIF_ROLE_MASTER,
    SPDIF_ROLE_SLAVE,
} T_SPDIF_ROLE;

bool spdif_drv_init(void);

void spdif_drv_deinit(void);

bool spdif_drv_tx_config(uint32_t sample_rate, T_SPDIF_FORMAT_TYPE format_type,
                         T_SPDIF_DATA_LEN data_length);

bool spdif_drv_rx_config(T_SPDIF_DATA_LEN data_length);

bool spdif_drv_general_config(T_SPDIF_ROLE role);

bool spdif_drv_enable(void);

bool spdif_drv_disable(void);

#ifdef __cplusplus
}
#endif

#endif /* _SPDIF_DRIVER_H_ */


