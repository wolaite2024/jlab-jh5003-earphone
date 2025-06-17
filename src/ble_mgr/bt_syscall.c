#include "bt_syscall.h"

#ifdef CONFIG_SOC_SERIES_RTL8773E
extern uint32_t (*lowerstack_SystemCall)(uint32_t, uint32_t, uint32_t, uint32_t);
#elif defined(CONFIG_SOC_SERIES_RTL87x3EU)
extern uint32_t (*lowerstack_SystemCall)(uint32_t, uint32_t, uint32_t, uint32_t);
#else
extern uint8_t (*rcp_lc_get_exp_transfer_time)(void *buf, ...);
#define lowerstack_SystemCall(opcode, param0, param1, param2) rcp_lc_get_exp_transfer_time((void *)opcode, param0, param1, param2)
#endif

#define EXT_OP_LOWERSTACK_GET_LE_ISO_SYNC_REF_AP_INFO     0x02

bool bt_get_le_iso_sync_ref_ap_info(T_BT_LE_ISO_SYNC_REF_AP_INFO *p_info)
{
    uint8_t ret;
    ret = lowerstack_SystemCall(EXT_OP_LOWERSTACK_GET_LE_ISO_SYNC_REF_AP_INFO, (uint32_t)p_info, 0, 0);
    if (ret == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

