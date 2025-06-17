#ifndef _CCP_CLIENT_INT_H_
#define _CCP_CLIENT_INT_H_

#ifdef  __cplusplus
extern "C" {
#endif      /* __cplusplus */

#include "ccp_client.h"

bool ccp_write_sig_rpt_interval(uint16_t conn_handle,  uint8_t srv_instance_id, uint8_t interval,
                                bool is_cmd,
                                bool gtbs);

#ifdef  __cplusplus
}
#endif      /*  __cplusplus */

#endif
