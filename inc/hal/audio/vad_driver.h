/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _VAD_DRIVER_H_
#define _VAD_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum t_vad_drv_event
{
    VAD_DRV_EVENT_DISABLED,
    VAD_DRV_EVENT_ENABLED,
} T_VAD_DRV_EVENT;

typedef enum t_vad_drv_adc_channel_sel
{
    VAD_DRV_ADC_CHANNEL0,
    VAD_DRV_ADC_CHANNEL1,
    VAD_DRV_ADC_CHANNEL2,
    VAD_DRV_ADC_CHANNEL3,
    VAD_DRV_ADC_CHANNEL4,
    VAD_DRV_ADC_CHANNEL5,
} T_VAD_DRV_ADC_CHANNEL_SEL;

typedef void (*T_VAD_DRV_CBACK)(T_VAD_DRV_EVENT event, void *param);

bool vad_drv_init(T_VAD_DRV_CBACK cback);
void vad_drv_deinit(void);
void vad_drv_param_config(T_VAD_DRV_ADC_CHANNEL_SEL ch_sel,
                          uint32_t                  sample_rate);
bool vad_drv_enable(void);
bool vad_drv_disable(void);

#ifdef __cplusplus
}
#endif

#endif /* _VAD_DRIVER_H_ */
