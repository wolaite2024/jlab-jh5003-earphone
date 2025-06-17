#if BISTO_FEATURE_SUPPORT

#include "app_cfg.h"

#include "app_bisto.h"
#include "app_bisto_bt.h"
#include "app_bisto_info.h"
#include "app_bisto_util.h"
#include "app_bisto_key.h"
#include "app_bisto_ble.h"
#include "app_bisto_battery.h"
#include "app_bisto_reset.h"
#include "app_bisto_audio.h"
#include "app_bisto_roleswap.h"
#include "app_bisto_ota.h"
#include "trace.h"

#include <string.h>




void app_bisto_handle_enter_pair_mode(void)
{
    if (app_cfg_nv.bud_role != REMOTE_SESSION_ROLE_SECONDARY)
    {
        app_bisto_ble_adv_start();
    }
}


void app_bisto_handle_power_on(bool is_pairing)
{
    if (app_cfg_nv.bud_role == REMOTE_SESSION_ROLE_SINGLE)
    {
        app_bisto_ble_adv_start();
    }
}




void app_bisto_init(void)
{
    /* utilities module should be init first */
    app_bisto_util_init();

    app_bisto_info_init();

    app_bisto_bt_init();

    app_bisto_ble_init();

    app_bisto_key_init();

    app_bisto_bat_init();

    app_bisto_reset_init();

    app_bisto_audio_init();

    app_bisto_roleswap_init();

    app_bisto_ota_init();

    /* start up gsound lib, must be called last */
    bisto_startup_gsound();

    APP_PRINT_INFO0("bisto: app_bisto_init");
}



#endif

