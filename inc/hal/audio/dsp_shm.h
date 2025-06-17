/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _DSP_SHM_H_
#define _DSP_SHM_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void (*T_DSP_SHM_IRQ_CBACK)(void);
typedef void (*T_DSP_SHM_MAILBOX_CBACK)(uint32_t group, uint32_t value);

bool dsp_shm_init(T_DSP_SHM_IRQ_CBACK     event,
                  T_DSP_SHM_MAILBOX_CBACK mailbox);
void dsp_shm_deinit(void);
void dsp_shm_boot(void);
void dsp_shm_enable(void);
void dsp_shm_disable(uint8_t dsp_log_output_select, uint8_t anc_measure_mode);
void dsp_shm_on(void);

bool dsp_shm_cmd_send(uint8_t *buffer, uint16_t length, bool flush);
uint16_t dsp_shm_evt_recv(uint8_t *buffer, uint16_t length);
bool dsp_shm_data_send(void *addr, uint8_t *buffer, uint16_t length, bool flush);
uint16_t dsp_shm_data_recv(void *addr, uint8_t *buffer, uint16_t length);
uint32_t dsp_shm_data_flush(void *addr);
uint16_t dsp_shm_data_len_peek(void *addr);
uint16_t dsp_shm_evt_len_peek(void);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* _DSP_SHM_H_ */
