#ifndef _BT_SYNC_CLK_H_
#define _BT_SYNC_CLK_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


typedef struct
{
    bool        offset_pick;
    uint8_t     burst_num;
    uint32_t    anchor_point;
    uint32_t    sdu_interval;
    uint32_t    iso_interval_us;
    uint32_t    transport_latency;
    uint32_t    guard_time;
} T_BT_SYNC_TS;

typedef enum
{
    SYNC_IN_SAFE,
    SYNC_IN_GUARD,
    SYNC_EXTEED_AP,
    SYNC_FUTURE_AP,
    SYNC_WRONG_AP,
    SYNC_SHOT_DELAY,
    SYNC_SKIP_CHECK,
} T_BT_SYNC_RISK;

typedef struct
{
    T_BT_SYNC_RISK  risk_level;
    uint32_t        ts_delay;
} T_BT_SYNC_TS_RISK;

uint32_t bt_sync_get_ts(T_BT_SYNC_TS *bt_sync_ts, T_BT_SYNC_TS_RISK *risk_info);
void bt_sync_reset_process(bool reset);
uint8_t bt_sync_invalid_check(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BT_BOND_H_ */
