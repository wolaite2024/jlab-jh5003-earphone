#ifndef _GATT_SERVICE_CLIENT_H_
#define _GATT_SERVICE_CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "bt_gattc_storage.h"

extern uint8_t client_supported_feature;

bool gatt_service_add(void);
bool gatt_service_load_data(T_GATTC_STORAGE_CB *p_gattc_cb);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif  /* _IAS_CLIENT_H_ */
