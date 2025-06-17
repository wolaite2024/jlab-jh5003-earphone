/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */
#ifndef _VADBUF_DRIVER_H_
#define _VADBUF_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum t_vadbuf_drv_event
{
    VADBUF_DRV_EVENT_DISABLED,
    VADBUF_DRV_EVENT_ENABLED,
    VADBUF_DRV_EVENT_SUSPENDED,
    VADBUF_DRV_EVENT_RESUMED,
    VADBUF_DRV_EVENT_DATA_IND,
} T_VADBUF_DRV_EVENT;

typedef enum t_vadbuf_drv_channel_mode
{
    VADBUF_DRV_CHANNEL_MODE1,
    VADBUF_DRV_CHANNEL_MODE2,
    VADBUF_DRV_CHANNEL_MODE3,
    VADBUF_DRV_CHANNEL_MODE4,
} T_VADBUF_DRV_CHANNEL_MODE;

typedef enum t_vadbuf_drv_data_len
{
    VADBUF_DRV_DATA_LEN_16BIT,          /* Just for HW type */
    VADBUF_DRV_DATA_LEN_24BIT,
} T_VADBUF_DRV_DATA_LEN;

typedef enum t_vadbuf_drv_chann_len
{
    VADBUF_DRV_CHANN_LEN_16BIT,
    VADBUF_DRV_CHANN_LEN_32BIT,
} T_VADBUF_DRV_CHANN_LEN;

typedef enum t_vadbuf_drv_adc_channel_sel
{
    VADBUF_DRV_ADC_CHANNEL0,
    VADBUF_DRV_ADC_CHANNEL1,
    VADBUF_DRV_ADC_CHANNEL2,
    VADBUF_DRV_ADC_CHANNEL3,
    VADBUF_DRV_ADC_CHANNEL4,
    VADBUF_DRV_ADC_CHANNEL5,
} T_VADBUF_DRV_ADC_CHANNEL_SEL;

typedef enum t_vadbuf_drv_channel_sel
{
    VADBUF_DRV_CHANNEL0,
    VADBUF_DRV_CHANNEL1,
    VADBUF_DRV_CHANNEL2,
    VADBUF_DRV_CHANNEL3,
    VADBUF_DRV_MAX,
} T_VADBUF_DRV_CHANNEL_SEL;

typedef enum t_vadbuf_drv_vad_type
{
    VADBUF_DRV_HW_VAD,
    VADBUF_DRV_FUSION_VAD,
} T_VADBUF_DRV_VAD_TYPE;

typedef void (*T_VADBUF_DRV_CBACK)(T_VADBUF_DRV_EVENT event, void *param);

bool vadbuf_drv_init(T_VADBUF_DRV_CBACK cback);
void vadbuf_drv_deinit(void);
void vadbuf_drv_param_set(T_VADBUF_DRV_VAD_TYPE     type,
                          T_VADBUF_DRV_CHANNEL_MODE mode,
                          T_VADBUF_DRV_DATA_LEN     data_len,
                          T_VADBUF_DRV_CHANN_LEN    chann_len,
                          uint16_t                  frame_size);
void vadbuf_drv_ch_set(T_VADBUF_DRV_CHANNEL_SEL     vad_channel,
                       T_VADBUF_DRV_ADC_CHANNEL_SEL adc_channel);
bool vadbuf_drv_enable(void);
bool vadbuf_drv_disable(void);
uint16_t vadbuf_drv_data_len_peek(void);
uint16_t vadbuf_drv_data_recv(uint8_t  *buffer,
                              uint16_t  len);

#ifdef __cplusplus
}
#endif

#endif  /* _VADBUF_DRIVER_H_ */
