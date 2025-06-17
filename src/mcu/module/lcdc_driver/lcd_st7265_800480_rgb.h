#ifndef _LCD_ST7265_800480_RGB_H_
#define _LCD_ST7265_800480_RGB_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "stdint.h"
#include "stdbool.h"


#define ST7265_800480_LCD_WIDTH                   800
#define ST7265_800480_LCD_HEIGHT                  480

#define ST7265_DRV_PIXEL_BITS                     16
#define BACKLIGHT_PIN                             P2_1


void rtk_lcd_hal_init(void);
void rtk_lcd_hal_update_framebuffer(uint8_t *p_buf, uint32_t size);
void rtk_lcd_hal_set_window(uint16_t xStart, uint16_t yStart, uint16_t w, uint16_t h);
void rtk_lcd_hal_start_transfer(uint8_t *buf, uint32_t len);
void rtk_lcd_hal_transfer_done(void);
void rtk_lcd_hal_set_reset(bool reset);



#ifdef __cplusplus
}
#endif

#endif /* _LCD_st7265_800480_RGB_H_ */
