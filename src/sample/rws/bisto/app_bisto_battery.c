#if BISTO_FEATURE_SUPPORT

#include "app_bisto_battery.h"
#include "bisto_api.h"


void app_bisto_bat_init(void)
{
    bisto_bat_create();
}


void app_bisto_bat_state_cb(T_APP_CHARGER_STATE app_charger_state)
{
    bool is_charging = false;

    if (app_charger_state == APP_CHARGER_STATE_CHARGING)
    {
        is_charging = true;
    }
    else
    {
        is_charging = false;
    }

    bisto_bat_state_update_cb(bisto_bat_get_instance(), is_charging);
}


void app_bisto_bat_state_of_charge_cb(uint8_t state_of_charge)
{
    bisto_bat_soc_update_cb(bisto_bat_get_instance(), state_of_charge);
}



#endif
