/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _DSP_LOADER_H_
#define _DSP_LOADER_H_

#include <stdbool.h>
#include <stdint.h>
#include "audio_type.h"

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum t_dsp_loader_event
{
    DSP_LOADER_EVENT_FINISH = 0x00,
} T_DSP_LOADER_EVENT;

typedef void (*P_DSP_LOADER_CBACK)(T_DSP_LOADER_EVENT event,
                                   uint32_t           param);

bool dsp_loader_init(P_DSP_LOADER_CBACK cback);

void dsp_loader_deinit(void);

void dsp_loader_run(void);

bool dsp_loader_bin_load(T_AUDIO_CATEGORY category);

bool dsp_loader_bin_match(T_AUDIO_CATEGORY category);

void dsp_loader_set_test_bin(bool enable);

void dsp_loader_finish(void);

void dsp_loader_next_bin(void);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* _DSP_LOADER_H_ */
