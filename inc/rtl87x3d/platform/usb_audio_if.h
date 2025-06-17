#ifndef __USB_AUDIO_IF_H
#define __USB_AUDIO_IF_H

#include <stdint.h>
#include <stdbool.h>

#define USB_AUDIO_IF_HID_REPORT                0x01
#define USB_AUDIO_IF_CAPTURE_CALLBACK_REG      0x02
#define USB_AUDIO_IF_CAPTURE_DATA_UPSTREAM     0x03

#define USB_AUDIO_IF_REG_AUDIO1_DESC           0x10
#define USB_AUDIO_IF_REG_AUDIO2_DESC           0x30
#define USB_AUDIO_IF_REG_AUDIO2_CTRL           0x31

typedef struct _usb_if_param
{
    uint32_t    type;
    void        *param;
} T_USB_IF_PARAM;

bool usb_audio_interface(void *audio_if);

typedef void (*APP_USB_AUDIO_PLUG_CALLBACK)(uint8_t audio_path, uint8_t bit_res, uint8_t sf,
                                            uint8_t chann_mode);
typedef void (*APP_USB_AUDIO_UNPLUG_CALLBACK)(uint8_t audio_path);
typedef bool (*APP_USB_AUDIO_CHECK_CALLBACK)(uint8_t audio_path);
typedef bool (*AU_PUT_USB_AUDIO_DATA_CALLBACK)(uint8_t *p_data, uint16_t len);
typedef void (*APP_SET_USB_AUDIO_GAIN_CALLBACK)(uint8_t lr_chann, uint8_t gain);
typedef bool (*AU_PULL_USB_AUDIO_DATA_CALLBACK)(uint8_t *p_data, uint16_t len);
typedef bool (*USB_CAP_CALLBACK)(uint32_t para);
extern USB_CAP_CALLBACK usb_cap_cb;

void usb_if_set_app_usb_audio_plug_callback(APP_USB_AUDIO_PLUG_CALLBACK callback_func);
void usb_if_set_app_usb_audio_unplug_callback(APP_USB_AUDIO_UNPLUG_CALLBACK callback_func);
void usb_if_set_au_put_audio_data_callback(AU_PUT_USB_AUDIO_DATA_CALLBACK callback_func);
void usb_if_set_app_set_usb_audio_gain_callback(APP_SET_USB_AUDIO_GAIN_CALLBACK callback_func);
void usb_if_set_au_pull_audio_data_callback(AU_PULL_USB_AUDIO_DATA_CALLBACK callback_func);

#endif
