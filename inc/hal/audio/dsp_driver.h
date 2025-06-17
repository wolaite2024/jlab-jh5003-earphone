/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _DSP_DRV_H_
#define _DSP_DRV_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef enum
{
    DSP_MSG_INTR_D2H_CMD            = 0x00,
    DSP_MSG_INTR_MAILBOX            = 0x03,
    DSP_MSG_CHECK_LOAD_IMAGE_FINISH = 0x05,
    DSP_MSG_LOAD_IMAGE_FIHISH       = 0x06,
    DSP_MSG_CODEC_STATE             = 0x08,
    DSP_MSG_PREPARE_READY           = 0x0C,
} T_DSP_MSG;

typedef enum t_dsp_drv_evt
{
    DSP_DRV_EVT_NONE                = 0x0,
    DSP_DRV_EVT_D2H                 = 0x1,
    DSP_DRV_EVT_MAILBOX             = 0x4,
    DSP_DRV_EVT_DSP_LOAD_PART       = 0x5,
    DSP_DRV_EVT_DSP_LOAD_FINISH     = 0x6,
    DSP_DRV_EVT_CODEC_STATE         = 0x7,
    DSP_DRV_EVT_PREPARE_READY       = 0xB,
} T_DSP_DRV_EVT;

typedef struct
{
    uint8_t     type;
    uint8_t     subtype;
    uint16_t    data_len;
    void       *p_data;
} T_DSP_SCHED_MSG;

typedef void (*P_DSP_DRV_CBACK)(uint32_t event, void *msg);

bool dsp_drv_init(void);
void dsp_drv_deinit(void);
bool dsp_drv_register_cback(P_DSP_DRV_CBACK cback);
void dsp_drv_unregister_cback(P_DSP_DRV_CBACK cback);
void dsp_drv_boot(void);
void dsp_drv_reset(void);
void dsp_drv_power_on(void);
void dsp_drv_shut_down(void);
void dsp_send_msg(uint8_t type, uint8_t subtype, void *p_data, uint16_t len);
void dsp_drv_dsp2_exist_set(uint8_t enable);
uint8_t dsp_drv_dsp2_exist_get(void);
// TODO: integrate to other place with dsp_shut_down()
uint32_t dsp_drv_boot_ref_cnt_get(void);

bool dsp_drv_is_fpga_mode(void);

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* _DSP_DRV_H_ */
