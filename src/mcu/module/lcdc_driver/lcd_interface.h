#ifndef _LCD_INTERFACE_H_
#define _LCD_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

void lcd_write(uint8_t *buf, uint32_t len);
void lcd_enter_data_output_mode(uint32_t len_byte);

#ifdef __cplusplus
}
#endif

#endif /* _LCD_INTERFACE_H_ */
