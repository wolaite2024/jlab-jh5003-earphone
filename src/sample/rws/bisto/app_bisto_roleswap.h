#ifndef __APP_BISTO_ROLESWAP_H
#define __APP_BISTO_ROLESWAP_H


#include "app_bt_sniffing.h"


void app_bisto_roleswap_success(T_REMOTE_SESSION_ROLE role);
void app_bisto_roleswap_failed(void);
void app_bisto_roleswap_role_decide(void);
void app_bisto_roleswap_handle_remote_conn_cmpl(void);
void app_bisto_roleswap_handle_remote_disconn_cmpl(void);


void app_bisto_roleswap_init(void);

#endif




