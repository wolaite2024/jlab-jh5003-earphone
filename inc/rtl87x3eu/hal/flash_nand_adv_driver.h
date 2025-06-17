/**
 *****************************************************************************************
 *     Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
 *****************************************************************************************
 * @file    flash_nor_adv_driver.h
 * @brief   Nor flash advanced driver API header file
 * @author  yuhungweng
 * @date    2021-05-21
 * @version v0.1
 * ***************************************************************************************
 */

#ifndef _FLASH_NAND_ADV_DRIVER_H
#define _FLASH_NAND_ADV_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

bool flash_nand_set_clock_160M_QSPI_mode(void);
bool fmc_flash_nand_page_write(uint32_t addr_page_align, uint8_t *page_size_data,
                               uint16_t data_length);
#endif
