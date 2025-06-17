/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _BIN_LOADER_DRIVER_H_
#define _BIN_LOADER_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef  __cplusplus
extern "C" {
#endif /* __cplusplus */

/* TODO Remove Start */
#define FPGA_DSP_ROM_TEST

#ifdef _IS_ASIC_
#undef FPGA_DSP_ROM_TEST
#endif

#define MAGIC_PATTERN               0x43215A5A
#define MAGIC_PATTERN_END           0x1234A5A5
#define SIGNATURE_FAKE_DSP_ROM      0x1234

typedef struct
{
    uint32_t   magic_pattern;
    uint8_t    type;
    uint8_t    image_size_minus_ofs;  // real size = image_size - image_size_ofs
    uint8_t    rsv8[2];
    uint16_t   checksum;        // if 0, the crc16 is ignored
    uint16_t   encrypt;         // if not 0, the image is encrypted

    union
    {
        uint32_t image_dst_addr;    // DSP image usage
        uint32_t group_info;        // ANC image usage
        struct
        {
            /* bit[0]: ANC, bit[1]: APT, bit[2]: ANC + APT, bit[3]: reserved */
            uint32_t sub_type:          4;  /* bit[3:0] filtergrouptype */
            uint32_t anc_apt_scenario:  8;  /* bit[11:4] scenario id */
            uint32_t scenario_mode:     8;  /* bit[19:12] enumerate for phone APP */
            uint32_t l_result_for_fit:  1;  /* bit[20] L channel response for ear tips fit */
            uint32_t r_result_for_fit:  1;  /* bit[21] R channel response for ear tips fit */
            uint32_t apt_brightness:    1;  /* bit[22] apt_brightness */
            uint32_t apt_ullrha:        1;  /* bit[23] apt_ullrha */
            uint32_t rsvd_group_info:   8;  /* bit[31:24] reserved */
        };
    };

    uint32_t   image_size;      // include dummy for 16 bytes for AES block
} T_DSP_IMAGE; // 20 bytes

typedef enum
{
    // up 200 will put into dsp_sys_image.bin
    SHM_SCENARIO_SYS_ROM = 255, // only fpga
    SHM_SCENARIO_SYS_RAM = 254,
    SHM_SCENARIO_SYS_PATCH = 253,
    SHM_SCENARIO_DSP2_SYS_1 = 252,
    SHM_SCENARIO_DSP2_SYS_2 = 251,
    SHM_SCENARIO_DSP2_SYS_3 = 250,
    SHM_SCENARIO_SYS_RAM_1 = 249,
    SHM_SCENARIO_SYS_RAM_2 = 248,

    SHM_SCENARIO_ADSP_ADAPTIVE_ANC = 240,
    SHM_SCENARIO_ADSP_APT = 239,
    SHM_SCENARIO_ADSP_NONE = 238,

    // 128 ~ 135 will put into anc image
    SHM_SCENARIO_ANC_0_CFG  = 128,
    SHM_SCENARIO_ANC_1_COEF = 129,
    SHM_SCENARIO_ANC_2_COEF = 130,
    SHM_SCENARIO_ANC_3_COEF = 131,
    SHM_SCENARIO_ANC_4_COEF = 132,
    SHM_SCENARIO_ANC_5_COEF = 133,
    SHM_SCENARIO_ANC_EAR_FIT = 134,
    SHM_SCENARIO_ANC_APT_COEF = 135,
    SHM_SCENARIO_ANC_ADSP_PARA = 136,
    SHM_SCENARIO_ANC_ADSP_LOOSE_FIT = 137,
    SHM_SCENARIO_ANC_APT_FILTER_INFO = 139,
    // up 200 will put into dsp_sys_image.bin
    // below 200 will put into dsp_scenario_image.bin
    SHM_SCENARIO_A2DP = 0,
    SHM_SCENARIO_SCO = 1,
    SHM_SCENARIO_VOICE_RECOGNITION = 2,
    SHM_SCENARIO_PROPRIETARY_VOICE = 3,
    SHM_SCENARIO_OPUS_VOICE = 4,
    SHM_SCENARIO_UHQ_VOICE = 5,
    SHM_SCENARIO_PROPRIETARY_AUDIO1 = 6,
    SHM_SCENARIO_PROPRIETARY_AUDIO2 = 7,
    SHM_SCENARIO_MP3 = 8,
    SHM_SCENARIO_LDAC = 9,
    SHM_SCENARIO_UHQ = 10,
    SHM_SCENARIO_LHDC = 11,
    SHM_SCENARIO_OPUS_AUDIO = 12,
    SHM_SCENARIO_PROPRIETARY_AUDIO3 = 13,
    SHM_SCENARIO_PROPRIETARY_AUDIO4 = 14,
    SHM_SCENARIO_ALL = 15,

    SHM_SCANRRIO_TOTAL
} T_SHM_SCENARIO;

/* TODO Remove End */

typedef enum t_bin_loader_drv_event
{
    BIN_LOADER_DRV_EVENT_CONTINUED  = 0x00,
    BIN_LOADER_DRV_EVENT_COMPLETED  = 0x01,
    BIN_LOADER_DRV_EVENT_FAILED     = 0x02,
} T_BIN_LOADER_DRV_EVENT;

typedef void (*BIN_LOADER_DRV_CBACK)(T_BIN_LOADER_DRV_EVENT event);

bool bin_loader_drv_init(BIN_LOADER_DRV_CBACK cback);
void bin_loader_drv_deinit(void);
bool bin_loader_drv_image_verify(void);
uint32_t bin_loader_drv_skip_img_header(uint32_t image_ptr);
void bin_loader_drv_cpu_decrypt_image(uint32_t data_addr, uint32_t *enc_aes_key);
bool bin_loader_drv_image_load(uint32_t id);
void bin_loader_drv_anc_img_info_set(uint32_t sub_type, uint32_t scenario_id);
bool bin_loader_drv_anc_check_img_valid(uint8_t type, uint32_t sub_type, uint32_t scenario_id);

extern uint32_t *p_image_buffer;

#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /* _BIN_LOADER_DRIVER_H_ */
