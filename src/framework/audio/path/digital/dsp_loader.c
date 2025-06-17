/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "trace.h"
#include "bin_loader.h"
#include "dsp_mgr.h"
#include "dsp_driver.h"
#include "dsp_loader.h"

/* TODO Remove Start */
#include "bin_loader_driver.h"

#define DSP_FW_RELOAD_COUNT         5

typedef enum t_dsp_loader_state
{
    DSP_LOADER_STATE_IDLE   = 0x1,
    DSP_LOADER_STATE_SYS    = 0x2,
    DSP_LOADER_STATE_PATCH  = 0x3,
    DSP_LOADER_STATE_ALGO   = 0x4,
    DSP2_LOADER_STATE_SYS   = 0x5,
} T_DSP_LOADER_STATE;

bool fix_scenario = false;

void dsp_loader_set_test_bin(bool enable)
{
    fix_scenario = enable;
}

/* TODO Remove End */

typedef struct t_dsp_loader_db
{
    T_BIN_LOADER_SESSION_HANDLE loader_session;
    P_DSP_LOADER_CBACK          cback;
    T_SHM_SCENARIO              loader_scenario;
    T_DSP_LOADER_STATE          state;
    uint8_t                     reload_count;
} T_DSP_LOADER_DB;

static T_DSP_LOADER_DB *dsp_loader_db = NULL;

void dsp_loader_scenario_set(T_SHM_SCENARIO scenario)
{
    dsp_loader_db->loader_scenario = scenario;
}

T_SHM_SCENARIO dsp_loader_scenario_get(void)
{
    return dsp_loader_db->loader_scenario;
}

T_SHM_SCENARIO dsp_loader_matched_scenario_get(T_AUDIO_CATEGORY category)
{
    T_SHM_SCENARIO scenario = SHM_SCENARIO_A2DP;
    T_SHM_SCENARIO loader_scenario = dsp_loader_scenario_get();

#if (CONFIG_SOC_SERIES_RTL8773D == 0)
    if (category == AUDIO_CATEGORY_VOICE)
    {
        scenario = SHM_SCENARIO_SCO;
    }
    else if ((category == AUDIO_CATEGORY_TONE) ||
             (category == AUDIO_CATEGORY_VP) ||
             (category == AUDIO_CATEGORY_LINE))
    {
        if ((loader_scenario == SHM_SCENARIO_SCO) ||
            (loader_scenario == SHM_SCENARIO_A2DP))
        {
            scenario = loader_scenario;
        }
    }
#endif

    if (fix_scenario == true)
    {
        scenario = SHM_SCENARIO_PROPRIETARY_VOICE; /* for mp test */
    }

    return scenario;
}

bool dsp_loader_bin_match(T_AUDIO_CATEGORY category)
{
    T_SHM_SCENARIO loader_scenario = dsp_loader_scenario_get();

    if (dsp_loader_matched_scenario_get(category) != loader_scenario)
    {
        return false;
    }

    return true;
}

void dsp_loader_next_bin(void)
{
    T_SHM_SCENARIO scenario = dsp_loader_scenario_get();

    if ((scenario < SHM_SCENARIO_SYS_PATCH) &&
        (dsp_loader_db->reload_count <= DSP_FW_RELOAD_COUNT))
    {
        dsp_loader_db->reload_count++;
        DIPC_PRINT_TRACE1("bin_loader_drv_check_image_correct: CRC check fail 0x%X", scenario);
        // retry is done in bin_loader_driver, if still fail at last,
        // something worng in the system. It is useless to do anything here
        //dsp_drv_reset();
        //bin_loader_token_issue(load_db->dsp_load_session, scenario, NULL);
    }
}

void dsp_loader_finish(void)
{
    uint32_t algorithm;

    T_SHM_SCENARIO scenario = dsp_loader_scenario_get();

    if (scenario == SHM_SCENARIO_SYS_ROM)
    {
        algorithm = SHM_SCENARIO_SYS_RAM;
        bin_loader_token_issue(dsp_loader_db->loader_session, algorithm, NULL);
        dsp_loader_scenario_set(SHM_SCENARIO_SYS_RAM);
        dsp_loader_db->state = DSP_LOADER_STATE_SYS;
    }
#if (CONFIG_SOC_SERIES_RTL8773D == 1)/*https://jira.realtek.com/browse/BBPRO3RD-718*/
    else if (scenario == SHM_SCENARIO_SYS_RAM)
    {
        algorithm = SHM_SCENARIO_SYS_RAM_1;
        bin_loader_token_issue(dsp_loader_db->loader_session, algorithm, NULL);
        dsp_loader_scenario_set(SHM_SCENARIO_SYS_RAM_1);
        dsp_loader_db->state = DSP_LOADER_STATE_SYS;
    }
    else if (scenario == SHM_SCENARIO_SYS_RAM_1)
    {
        algorithm = SHM_SCENARIO_SYS_RAM_2;
        bin_loader_token_issue(dsp_loader_db->loader_session, algorithm, NULL);
        dsp_loader_scenario_set(SHM_SCENARIO_SYS_RAM_2);
        dsp_loader_db->state = DSP_LOADER_STATE_SYS;
    }
    else if (scenario == SHM_SCENARIO_SYS_RAM_2)
    {
        algorithm = SHM_SCENARIO_DSP2_SYS_1;
        if (dsp_drv_dsp2_exist_get())
        {
            bin_loader_token_issue(dsp_loader_db->loader_session, algorithm, NULL);
            dsp_loader_scenario_set(SHM_SCENARIO_DSP2_SYS_1);
            dsp_loader_db->state = DSP2_LOADER_STATE_SYS;
        }
        else
        {
            DIPC_PRINT_INFO1("disable dps2, skip loading 0x%x", algorithm);
            algorithm = SHM_SCENARIO_SYS_PATCH;
            bin_loader_token_issue(dsp_loader_db->loader_session, algorithm, NULL);
            dsp_loader_scenario_set(SHM_SCENARIO_SYS_PATCH);
            dsp_loader_db->state = DSP_LOADER_STATE_PATCH;
        }

    }
    else if (scenario == SHM_SCENARIO_DSP2_SYS_1)
    {
        algorithm = SHM_SCENARIO_DSP2_SYS_2;
        if (dsp_drv_dsp2_exist_get())
        {
            bin_loader_token_issue(dsp_loader_db->loader_session, algorithm, NULL);
            dsp_loader_scenario_set(SHM_SCENARIO_DSP2_SYS_2);
            dsp_loader_db->state = DSP2_LOADER_STATE_SYS;
        }
        else
        {
            DIPC_PRINT_INFO1("disable dps2, skip loading 0x%x", algorithm);
            algorithm = SHM_SCENARIO_SYS_PATCH;
            bin_loader_token_issue(dsp_loader_db->loader_session, algorithm, NULL);
            dsp_loader_scenario_set(SHM_SCENARIO_SYS_PATCH);
            dsp_loader_db->state = DSP_LOADER_STATE_PATCH;
        }

    }
    else if (scenario == SHM_SCENARIO_DSP2_SYS_2)
    {
        algorithm = SHM_SCENARIO_DSP2_SYS_3;
        if (dsp_drv_dsp2_exist_get())
        {
            bin_loader_token_issue(dsp_loader_db->loader_session, algorithm, NULL);
            dsp_loader_scenario_set(SHM_SCENARIO_DSP2_SYS_3);
            dsp_loader_db->state = DSP2_LOADER_STATE_SYS;
        }
        else
        {
            DIPC_PRINT_INFO1("disable dps2, skip loading 0x%x", algorithm);
            algorithm = SHM_SCENARIO_SYS_PATCH;
            bin_loader_token_issue(dsp_loader_db->loader_session, algorithm, NULL);
            dsp_loader_scenario_set(SHM_SCENARIO_SYS_PATCH);
            dsp_loader_db->state = DSP_LOADER_STATE_PATCH;
        }
    }
    else if (scenario == SHM_SCENARIO_DSP2_SYS_3)
    {
        DIPC_PRINT_INFO0("dsp2_load_image_end");
        algorithm = SHM_SCENARIO_SYS_PATCH;
        bin_loader_token_issue(dsp_loader_db->loader_session, algorithm, NULL);
        dsp_loader_scenario_set(SHM_SCENARIO_SYS_PATCH);
        dsp_loader_db->state = DSP_LOADER_STATE_PATCH;
    }
#else
    else if (scenario == SHM_SCENARIO_SYS_RAM)
    {
        algorithm = SHM_SCENARIO_SYS_PATCH;
        bin_loader_token_issue(dsp_loader_db->loader_session, algorithm, NULL);
        dsp_loader_scenario_set(SHM_SCENARIO_SYS_PATCH);
        dsp_loader_db->state = DSP_LOADER_STATE_PATCH;
    }
#endif
    else
    {
        dsp_loader_db->state = DSP_LOADER_STATE_ALGO;

        if (dsp_loader_db->cback != NULL)
        {
            dsp_loader_db->cback(DSP_LOADER_EVENT_FINISH, scenario);
        }
    }
}

void dsp_loader_bin_cback(T_BIN_LOADER_SESSION_HANDLE  session,
                          T_BIN_LOADER_EVENT           event,
                          uint32_t                     id,
                          void                        *context)
{
    switch (event)
    {
    case BIN_LOADER_EVENT_EXECUTED:
        break;

    case BIN_LOADER_EVENT_CONTINUED:
        dsp_send_msg(DSP_MSG_CHECK_LOAD_IMAGE_FINISH, 0, NULL, 0);
        break;

    case BIN_LOADER_EVENT_COMPLETED:
        dsp_send_msg(DSP_MSG_LOAD_IMAGE_FIHISH, 0, NULL, 0);
        break;

    case BIN_LOADER_EVENT_FAILED:
        dsp_send_msg(DSP_MSG_LOAD_IMAGE_FIHISH, 0, NULL, 0);
        break;
    }
}

bool dsp_loader_init(P_DSP_LOADER_CBACK cback)
{
    int32_t ret = 0;

    if (dsp_loader_db != NULL)
    {
        ret = 1;
        goto fail_check_db;
    }

    dsp_loader_db = calloc(1, sizeof(T_DSP_LOADER_DB));
    if (dsp_loader_db == NULL)
    {
        ret = 2;
        goto fail_alloc_db;
    }

    dsp_loader_db->loader_session = bin_loader_session_create(dsp_loader_bin_cback);
    if (dsp_loader_db->loader_session == NULL)
    {
        ret = 3;
        goto fail_create_session;
    }

    dsp_loader_db->state = DSP_LOADER_STATE_IDLE;
    dsp_loader_db->cback = cback;

    // remove dsp_drv_is_fpga_mode when bin loader policy is ready
    if (dsp_drv_is_fpga_mode())
    {
        dsp_loader_db->loader_scenario = SHM_SCENARIO_SYS_ROM;
    }
    else
    {
        dsp_loader_db->loader_scenario = SHM_SCENARIO_SYS_RAM;
    }

    return true;

fail_create_session:
    free(dsp_loader_db);
    dsp_loader_db = NULL;
fail_alloc_db:
fail_check_db:
    DIPC_PRINT_ERROR1("dsp_loader_init: failed %d", -ret);
    return false;
}

void dsp_loader_deinit(void)
{
    if (dsp_loader_db != NULL)
    {
        bin_loader_session_destroy(dsp_loader_db->loader_session);
        free(dsp_loader_db);
        dsp_loader_db = NULL;
    }
}

void dsp_loader_run(void)
{
    if (dsp_loader_db->state == DSP_LOADER_STATE_IDLE)
    {
        dsp_loader_db->state = DSP_LOADER_STATE_SYS;
        bin_loader_token_issue(dsp_loader_db->loader_session,
                               dsp_loader_db->loader_scenario,
                               NULL);
    }
    else
    {
        dsp_loader_finish();
    }
}

bool dsp_loader_bin_load(T_AUDIO_CATEGORY category)
{
    T_SHM_SCENARIO scenario = dsp_loader_matched_scenario_get(category);

    DIPC_PRINT_TRACE3("dsp_loader_bin_load: category %d, new scenario %d, old scenario %d",
                      category, scenario, dsp_loader_db->loader_scenario);

    if (scenario != dsp_loader_db->loader_scenario)
    {
        dsp_loader_db->loader_scenario = scenario;
        bin_loader_token_issue(dsp_loader_db->loader_session, scenario, NULL);
        return true;
    }
    else
    {
        dsp_loader_finish();
        return false;
    }
}
