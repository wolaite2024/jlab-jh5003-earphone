/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if F_APP_A2DP_CODEC_LHDC_SUPPORT
#include <stdlib.h>
#include <string.h>
#include "stdlib_corecrt.h"
#include "trace.h"
#include "os_mem.h"
#include "app_a2dp.h"
#include "app_main.h"
#include "app_cfg.h"
#include "audio_vse.h"
#include "system_status_api.h"
#include "fmc_api.h"
#include "audio_track.h"

#define DEVICE_NAME_SIZE 40
#define EUID_SIZE 14
#define CHECK_BYTE_SIZE 2
#define LICENSE_KEY_SIZE 256
#define LHDC_LICENSE_KEY_ADDR 0x02001000

#define LHDC_EFFECT_EFFECT_ID              (0)
#define LHDC_EFFECT_DATA_TYPE              (0xFF)
#define LHDC_EFFECT_VERSION                (0x00)
#define LHDC_EFFECT_KEY_UUID_MAGIC          (0x5AA5)

#define LHDC_LTV_LEN_OCTETS                (1)
#define LHDC_LTV_TYPE_OCTETS               (1)
#define LHDC_LTV_HEADER_SIZE      (LHDC_LTV_LEN_OCTETS + LHDC_LTV_TYPE_OCTETS)


#define LHDC_LTV_VER_OCTETS                (1)
#define LHDC_LTV_ENTITY_OCTETS             (1)
#define LHDC_LTV_CTRL_OCTETS               (1)
#define LHDC_LTV_CMD_OCTETS                (1)
#define LHDC_LTV_CMD_RSV_OCTETS            (1)
#define LHDC_LTV_CMD_VER_SIZE      (LHDC_LTV_VER_OCTETS + LHDC_LTV_ENTITY_OCTETS + LHDC_LTV_CTRL_OCTETS + LHDC_LTV_CMD_OCTETS + LHDC_LTV_CMD_RSV_OCTETS)

#define LHDC_LTV_DATA_SIZE         246 //max: 252

typedef union
{
    uint8_t  data[DEVICE_NAME_SIZE + EUID_SIZE + CHECK_BYTE_SIZE + LICENSE_KEY_SIZE];
    struct
    {
        uint8_t device_name[DEVICE_NAME_SIZE];
        uint8_t uuid[EUID_SIZE + CHECK_BYTE_SIZE];
        uint8_t key[LICENSE_KEY_SIZE];
    };
} T_LHDC_LICENSE_INFO;

enum
{
    LHDC_ENTITY_NULL               = 0x00,
    LHDC_ENTITY_DSP1               = 0x01,
    LHDC_ENTITY_DSP2               = 0x02,
};

enum
{
    LHDC_CTL_SINGLE                = 0x00,
    LHDC_CTL_START                 = 0x01,
    LHDC_CTL_CONTINUE              = 0x02,
    LHDC_CTL_END                   = 0x03,
};

typedef enum
{
    T_LHDC_CMD_LICENSE_KEY_START              = 0xA0,
    T_LHDC_CMD_LICENSE_KEY_END              = 0xA1,
} T_LHDC_CMD;

typedef union
{
    uint8_t  data[LHDC_LTV_HEADER_SIZE + LHDC_LTV_CMD_VER_SIZE + LHDC_LTV_DATA_SIZE];
    struct
    {
        uint8_t len;
        uint8_t type;
        union
        {
            uint8_t value[LHDC_LTV_CMD_VER_SIZE + LHDC_LTV_DATA_SIZE];
            struct
            {
                uint8_t value_version;
                uint8_t value_entity;
                uint8_t value_control;
                uint8_t value_cmd;
                uint8_t value_cmd_rsv;
                uint8_t value_dat[LHDC_LTV_DATA_SIZE];
            };
        };
    };
} T_LHDC_DATA;

static T_LHDC_DATA *lhdc_data = NULL;

static void app_lhdc_send_data(T_LHDC_CMD cmd, uint16_t len)
{
    static uint16_t seq_num = 0;
    uint8_t active_a2dp_idx = app_a2dp_get_active_idx();

    APP_PRINT_INFO2("app_lhdc_send_data: cmd 0x%02x len %d", cmd, len);

    if (lhdc_data != NULL)
    {
        if (app_db.br_link[active_a2dp_idx].lhdc_instance != NULL)
        {
            lhdc_data->len = len + LHDC_LTV_TYPE_OCTETS + LHDC_LTV_CMD_VER_SIZE;
            lhdc_data->type = LHDC_EFFECT_DATA_TYPE;
            lhdc_data->value_version = LHDC_EFFECT_VERSION;
            lhdc_data->value_entity = LHDC_ENTITY_DSP1;
            lhdc_data->value_control = LHDC_CTL_SINGLE;
            lhdc_data->value_cmd = cmd;
            lhdc_data->value_cmd_rsv = 0;

            audio_vse_apply(app_db.br_link[active_a2dp_idx].lhdc_instance, seq_num++, lhdc_data->data,
                            (lhdc_data->len + LHDC_LTV_LEN_OCTETS));
        }

        free(lhdc_data);
    }
}

static void app_lhdc_load_security_key(void)
{
    T_LHDC_LICENSE_INFO pdata;

    memset(&pdata, 0, sizeof(pdata));

    memcpy(pdata.device_name, app_cfg_const.device_name_legacy_default,
           DEVICE_NAME_SIZE);

    //total 16 bytes,flash euid 14 bytes, the last 2 bytes is check byte
    memcpy(pdata.uuid, sys_hall_get_ic_euid(), EUID_SIZE);
    pdata.uuid[EUID_SIZE] = LHDC_EFFECT_KEY_UUID_MAGIC & 0xFF;
    pdata.uuid[EUID_SIZE + 1] = LHDC_EFFECT_KEY_UUID_MAGIC >> 8;

    fmc_flash_nor_read(LHDC_LICENSE_KEY_ADDR, pdata.key, LICENSE_KEY_SIZE);

    lhdc_data = (T_LHDC_DATA *)calloc(1, sizeof(T_LHDC_DATA));
    memcpy(&lhdc_data->value_dat, &(pdata.data), LHDC_LTV_DATA_SIZE);
    app_lhdc_send_data(T_LHDC_CMD_LICENSE_KEY_START, LHDC_LTV_DATA_SIZE);

    lhdc_data = (T_LHDC_DATA *)calloc(1, sizeof(T_LHDC_DATA));
    memcpy(&lhdc_data->value_dat, &(pdata.data[LHDC_LTV_DATA_SIZE]),
           sizeof(pdata) - LHDC_LTV_DATA_SIZE);
    app_lhdc_send_data(T_LHDC_CMD_LICENSE_KEY_END, sizeof(pdata) - LHDC_LTV_DATA_SIZE);
}

void app_lhdc_audio_vse_cback(T_AUDIO_EFFECT_INSTANCE  instance,
                              T_AUDIO_VSE_EVENT        event_type,
                              uint16_t                seq_num,
                              void                    *event_buf,
                              uint16_t                 buf_len)
{
    uint8_t *event_buff = event_buf;
    uint32_t NumDataInByte, StartAddr;
    int32_t ret;

    APP_PRINT_TRACE3("app_lhdc_audio_vse_cback: event_type %d event_buff %p buf_len %d data %b",
                     event_type, event_buf, buf_len);

    switch (event_type)
    {
    case AUDIO_VSE_EVENT_ENABLE:
        app_lhdc_load_security_key();

        break;

    default:
        break;
    }
}

void app_lhdc_release(T_APP_BR_LINK *p_link)
{
    if (p_link->lhdc_instance != NULL)
    {
        APP_PRINT_TRACE1("app_lhdc_release: lhdc_instance %p", p_link->lhdc_instance);
        audio_vse_release(p_link->lhdc_instance);
        p_link->lhdc_instance = NULL;
    }
}

void app_lhdc_create(T_APP_BR_LINK *p_link)
{
    if (p_link->a2dp_track_handle != NULL)
    {
        p_link->lhdc_instance = audio_vse_create(AUDIO_VSE_COMPANY_ID_SAVITECH,
                                                 LHDC_EFFECT_EFFECT_ID,
                                                 app_lhdc_audio_vse_cback);

        APP_PRINT_TRACE1("app_lhdc_create: lhdc_instance %p", p_link->lhdc_instance);
        audio_track_effect_attach(p_link->a2dp_track_handle, p_link->lhdc_instance);
    }
}
#endif

