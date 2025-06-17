#ifndef _OS_POWER_MANAGER_H_
#define _OS_POWER_MANAGER_H_

#include "platform_utils.h"
#include "os_queue.h"
#include "power_manager_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UPDATE_TICK_COUNT()\
    if (portNVIC_INT_CTRL_REG & SCB_ICSR_PENDSTSET_Msk)\
    {\
        xPortSysTickHandler();\
        portNVIC_INT_CTRL_REG = SCB_ICSR_PENDSTCLR_Msk;\
    }

#define US_TO_TICK_COUNT(us, residual)\
    cal_quotient_remainder(1000000 * (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ), configSYSTICK_CLOCK_HZ, us, &residual)

#define US_TO_SYSTICK(us, residual)\
    cal_quotient_remainder(1000000, configSYSTICK_CLOCK_HZ, us, &residual)

#define TICK_COUNT_TO_US(tick_count)\
    cal_quotient_remainder(configSYSTICK_CLOCK_HZ, 1000000 * (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ), tick_count, NULL)

#define SYSTICK_TO_US(systick)\
    cal_quotient_remainder(configSYSTICK_CLOCK_HZ, 1000000, systick, NULL)

#define SYSTICK_RELOAD_VALUE    (configSYSTICK_CLOCK_HZ / configTICK_RATE_HZ - 1)

typedef struct _PlatformPMBufferBackup
{
    bool has_dynamic_buffer;
    uint8_t *buffer_stored_addr;
    T_OS_QUEUE buffer_queue;
} PlatformPMBufferBackup;

typedef enum
{
    PLATFORM_PM_EXCLUDED_TIMER,
    PLATFORM_PM_EXCLUDED_TASK,
    PLATFORM_PM_EXCLUDED_TYPE_MAX,
} PlatformExcludedHandleType;

typedef struct _PlatformPMExcludedHandleQueueElem
{
    struct PlatformPMExcludedHandleQueueElem *pNext;
    void **handle;
} PlatformPMExcludedHandleQueueElem;

typedef union
{
    uint8_t value[1];
    struct
    {
        uint8_t os_pm_statistic:        1;
        uint8_t rsvd:                   7;
    };
} OSPMFeatureConfig;

typedef struct
{
    uint32_t last_sleep_clk;
    uint32_t last_sleep_systick;
    uint32_t tickcount_restore_remain_us;
} OSPMSystem;

typedef struct _PlatformPMBufferQueueElem
{
    struct _PlatformPMBufferQueueElem *pNext;
    uint8_t *pBufferStart;
    uint8_t *pBackup;
    uint32_t length : 31;
    uint32_t shouldFree : 1;
} PlatformPMBufferQueueElem;

bool os_register_pm_excluded_handle(void **handle, PlatformExcludedHandleType type);
bool os_unregister_pm_excluded_handle(void **handle, PlatformExcludedHandleType type);

void os_pm_stop_all_non_excluded_timer(void);

bool os_register_pm_buffer(uint8_t *pBufferStart, uint32_t length, bool shouldFree);
void os_unregister_pm_buffer(uint8_t *pBufferStart);
uint16_t os_pm_get_buffer_store_count(void);

PMCheckResult os_pm_ram_check(void);

void os_pm_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _OS_POWER_MANAGER_H_ */
