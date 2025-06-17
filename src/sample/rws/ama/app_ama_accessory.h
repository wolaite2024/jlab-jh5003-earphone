#ifndef __APP_AMA_ACCESSORY_H
#define __APP_AMA_ACCESSORY_H


#include <stdbool.h>
#include "btm.h"


void app_ama_accessory_handle_power_off(void);
void app_ama_accessory_handle_power_on(bool is_pairing);
void app_ama_accessory_handle_enter_pair_mode(void);
void app_ama_accessory_role_update(T_REMOTE_SESSION_ROLE rtk_role);

void app_ama_accessory_init(void);
#endif






