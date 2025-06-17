/**
*****************************************************************************************
*     Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file    module_lcd_lcdc_qspi.h
* @brief   This file provides LCDC QSPI functions
* @author
* @date
* @version v1.0
* *************************************************************************************
*/

#ifndef _MODULE_LCD_LCDC_QSPI_H_
#define _MODULE_LCD_LCDC_QSPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

#define INPUT_PIXEL_BYTES                   2
#if INPUT_PIXEL_BYTES == 3
#error "LCDC DMA doesn't allow 3 bytes input"
#endif

void lcd_pin_config(uint8_t lcd_rst);
void lcd_set_reset(bool reset);
void lcd_set_dma_ch_num(uint8_t num);

#ifdef __cplusplus
}
#endif

#endif /* _RTL876X_MODULE_LCD_QSPI_H_ */
