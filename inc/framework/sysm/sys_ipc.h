/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _SYS_IPC_H_
#define _SYS_IPC_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef void *T_SYS_IPC_HANDLE;

typedef bool (*P_SYS_IPC_CBACK)(uint32_t id, void *msg);

bool sys_ipc_init(void);

void sys_ipc_deinit(void);

bool sys_ipc_publish(const char *topic,
                     uint32_t    id,
                     void       *msg);

T_SYS_IPC_HANDLE sys_ipc_subscribe(const char      *topic,
                                   P_SYS_IPC_CBACK  cback);

bool sys_ipc_unsubscribe(T_SYS_IPC_HANDLE handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _SYS_IPC_H_ */
