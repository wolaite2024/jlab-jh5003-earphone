#ifndef _APP_BISTO_BATTERY_H_
#define _APP_BISTO_BATTERY_H_


#include "app_charger.h"


void app_bisto_bat_init(void);

void app_bisto_bat_state_cb(T_APP_CHARGER_STATE app_charger_state);

void app_bisto_bat_state_of_charge_cb(uint8_t state_of_charge);

#endif






