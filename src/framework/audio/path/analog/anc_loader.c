/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#if (CONFIG_REALTEK_AM_ANC_SUPPORT == 1)
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "trace.h"
#include "bin_loader.h"
#include "anc_driver.h"
#include "anc_loader.h"

/* TODO Remove Start */
extern T_DSP_IMAGE_EXT *p_anc_image_record;
extern uint32_t *p_image_buffer;

bool dynamic_idx_flag = false;
/* TODO Remove End */

typedef struct t_anc_loader_db
{
    T_BIN_LOADER_SESSION_HANDLE loader_session;
    P_ANC_LOADER_CBACK cback;
    uint8_t reload_count;
    uint8_t pre_scenario_id;
    uint8_t scenario_id;
    uint8_t image_type;
} T_ANC_LOADER_DB;

T_ANC_LOADER_DB anc_loader_db = {0};

#define ADSP_FW_RELOAD_COUNT         5

bool anc_loader_next_bin(void)
{
#if (AM_ADSP_SUPPORT==1)
    T_SHM_SCENARIO scenario = (T_SHM_SCENARIO)anc_loader_db.scenario_id;

    LOADER_PRINT_ERROR1("anc_loader_next_bin: ADSP load fail 0x%02X",
                        scenario);
    //Reload algorithm FW
    if ((scenario <= SHM_SCENARIO_ADSP_ADAPTIVE_ANC) && (scenario > SHM_SCENARIO_ADSP_NONE) &&
        (anc_loader_db.reload_count <= ADSP_FW_RELOAD_COUNT))
    {
        anc_loader_db.reload_count++;
        anc_drv_enable_adaptive_anc(0, 0);
        anc_drv_turn_on_adsp(0);
        anc_loader_bin_load(scenario);
    }
#endif
    // dummy callback, do nothing
    return true;
}

bool anc_loader_finish(void)
{
    LOADER_PRINT_TRACE5("anc_loader_finish, type: 0x%X, image_type: 0x%X, p_anc_image_record: 0x%X, grp_info: 0x%X, payload: %p",
                        p_anc_image_record->type, anc_loader_db.image_type,
                        p_anc_image_record, p_anc_image_record->group_info, p_image_buffer);

#if (AM_ADSP_SUPPORT==1)
    if ((p_anc_image_record->type == (SHM_SCENARIO_ADSP_ADAPTIVE_ANC - SHM_SCENARIO_ADSP_APT)) &&
        (anc_loader_db.image_type == SHM_SCENARIO_ADSP_ADAPTIVE_ANC))
    {
        anc_drv_set_adsp_load_finished(1);

        if (anc_drv_get_adsp_para_source() == ANC_DRV_ADSP_PARA_SRC_FROM_TOOL)
        {
            anc_drv_turn_on_adsp(1);
        }
        anc_loader_db.cback(ANC_LOADER_EVENT_SCENARIO_COMPLETE);
    }
#endif
#if (AM_ADSP_SUPPORT==1)
    else if ((p_anc_image_record->type != (SHM_SCENARIO_ADSP_ADAPTIVE_ANC - SHM_SCENARIO_ADSP_APT)) &&
             (anc_loader_db.image_type == p_anc_image_record->type))
#else
    if (anc_loader_db.image_type == p_anc_image_record->type)
#endif
    {
#if (AM_ADSP_SUPPORT==1)
        if ((p_anc_image_record->type >= SHM_SCENARIO_ANC_0_CFG) &&
            (p_anc_image_record->type <= SHM_SCENARIO_ANC_APT_FILTER_INFO))
#else
        if (((p_anc_image_record->type >= SHM_SCENARIO_ANC_0_CFG) &&
             (p_anc_image_record->type <= SHM_SCENARIO_ANC_APT_COEF)) ||
            (p_anc_image_record->type == SHM_SCENARIO_ANC_APT_FILTER_INFO))
#endif
        {
            // parsing ANC image
            if (p_image_buffer)
            {
                anc_drv_image_parser(p_anc_image_record, p_image_buffer);
                free(p_image_buffer);
                p_image_buffer = NULL;

                if (p_anc_image_record->type == SHM_SCENARIO_ANC_0_CFG)
                {
#if (AM_ADSP_SUPPORT==1)
                    anc_drv_adaptive_anc_boot_up();
#endif
                    anc_loader_db.cback(ANC_LOADER_EVENT_CONFIGURATION_COMPLETE);
                }
                else if (p_anc_image_record->type == SHM_SCENARIO_ANC_APT_COEF)
                {
                    dynamic_idx_flag = false;
#if (AM_ADSP_SUPPORT==1)
                    uint32_t target_gain = anc_drv_get_active_adsp_scenario();
                    if (target_gain & ANC_DRV_GRP_INFO_APT_NR_EN)
                    {
                        anc_loader_db.scenario_id = SHM_SCENARIO_ADSP_APT;
                    }
                    else if (target_gain & ANC_DRV_GRP_INFO_APT_EQ_EN)
                    {
                        anc_loader_db.scenario_id = SHM_SCENARIO_ADSP_APT;
                    }
                    else if (target_gain & (ANC_DRV_GRP_INFO_ADAPTIVE_ANC |
                                            ANC_DRV_GRP_INFO_ULLRHA))
                    {
                        anc_loader_db.scenario_id = SHM_SCENARIO_ADSP_ADAPTIVE_ANC;
                    }

                    if (target_gain & (ANC_DRV_GRP_INFO_APT_NR_EN |
                                       ANC_DRV_GRP_INFO_APT_EQ_EN |
                                       ANC_DRV_GRP_INFO_ADAPTIVE_ANC |
                                       ANC_DRV_GRP_INFO_ULLRHA))
                    {
                        anc_drv_set_adsp_para_source(ANC_DRV_ADSP_PARA_SRC_FROM_BIN);
                    }

                    if (target_gain & (ANC_DRV_GRP_INFO_ADAPTIVE_ANC |
                                       ANC_DRV_GRP_INFO_ULLRHA))
                    {
                        LOADER_PRINT_TRACE1("adsp_loader_algorithm_code: algorithm = 0x%x", anc_loader_db.scenario_id);
                        anc_drv_set_adsp_ram_ctrl(true);
                        anc_drv_set_adsp_load_finished(0);
                        anc_loader_bin_load(anc_loader_db.scenario_id);
                        anc_loader_db.pre_scenario_id = anc_loader_db.scenario_id;
                        anc_loader_db.cback(ANC_LOADER_EVENT_ADSP_IMAGE);
                    }
                    else
                    {
                        anc_loader_db.cback(ANC_LOADER_EVENT_SCENARIO_COMPLETE);
                    }
#else
                    anc_loader_db.cback(ANC_LOADER_EVENT_SCENARIO_COMPLETE);
#endif
                }
                else if (p_anc_image_record->type == SHM_SCENARIO_ANC_APT_FILTER_INFO)
                {
                    dynamic_idx_flag = true;
                    anc_loader_bin_load(SHM_SCENARIO_ANC_APT_COEF);
                }
#if (AM_ADSP_SUPPORT==1)
                else if (p_anc_image_record->type == SHM_SCENARIO_ANC_ADSP_PARA)
                {
                    anc_loader_db.cback(ANC_LOADER_EVENT_ADSP_PARA_COMPLETE);
                }
                else if (p_anc_image_record->type == SHM_SCENARIO_ANC_ADSP_LOOSE_FIT)
                {
                    anc_loader_db.cback(ANC_LOADER_EVENT_ADSP_LOOSE_FIT_COMPLETE);
                }
#endif
            }
        }
    }
    else
    {
        if (anc_loader_db.image_type == SHM_SCENARIO_ANC_APT_FILTER_INFO)
        {
            dynamic_idx_flag = true;
            anc_loader_bin_load(SHM_SCENARIO_ANC_APT_COEF);
        }
    }

    return true;
}

void anc_loader_bin_cback(T_BIN_LOADER_SESSION_HANDLE  session,
                          T_BIN_LOADER_EVENT           event,
                          uint32_t                     id,
                          void                        *context)
{
    switch (event)
    {
    case BIN_LOADER_EVENT_EXECUTED:
        break;

    case BIN_LOADER_EVENT_CONTINUED:
        anc_send_msg(ANC_MSG_LOAD_PART, 0, NULL, 0);
        break;

    case BIN_LOADER_EVENT_COMPLETED:
        anc_send_msg(ANC_MSG_LOAD_FINISH, 0, NULL, 0);
        break;

    case BIN_LOADER_EVENT_FAILED:
        anc_send_msg(ANC_MSG_LOAD_FINISH, 0, NULL, 0);
        break;
    }
}

bool anc_loader_init(P_ANC_LOADER_CBACK cback)
{
    int32_t ret = 0;

    anc_loader_db.loader_session = bin_loader_session_create(anc_loader_bin_cback);
    if (anc_loader_db.loader_session != NULL)
    {
        anc_loader_db.cback = cback;
    }
    else
    {
        ret = 3;
        goto fail_create_session;
    }

    anc_loader_db.reload_count       = 0;
    anc_loader_db.pre_scenario_id    = SHM_SCENARIO_ADSP_NONE;
    anc_loader_db.scenario_id        = SHM_SCENARIO_ADSP_NONE;
    dynamic_idx_flag = false;

    return true;

fail_create_session:
    LOADER_PRINT_ERROR1("anc_loader_init: failed %d", -ret);
    return false;
}

void anc_loader_deinit(void)
{
    if (anc_loader_db.loader_session != NULL)
    {
        bin_loader_session_destroy(anc_loader_db.loader_session);
        anc_loader_db.loader_session = NULL;
        anc_loader_db.cback = NULL;
    }
}

bool anc_loader_bin_load(uint8_t image_type)
{
    if (anc_loader_db.loader_session != NULL)
    {
        if (image_type == SHM_SCENARIO_ANC_APT_COEF)
        {
            if (dynamic_idx_flag == false)
            {
                image_type = SHM_SCENARIO_ANC_APT_FILTER_INFO;
            }
        }
        anc_loader_db.image_type = image_type;
        bin_loader_token_issue(anc_loader_db.loader_session, image_type, NULL);

        return true;
    }
    else
    {
        LOADER_PRINT_ERROR0("anc_loader_bin_load: failed");

        return false;
    }
}

void anc_loader_set_img_load_param(uint8_t sub_type,
                                   uint8_t scenario_id)
{
    anc_drv_set_img_load_param(sub_type, scenario_id);
}
#endif
