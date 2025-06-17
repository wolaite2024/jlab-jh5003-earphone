#ifndef LOW_STACK_H_
#define LOW_STACK_H_

#include <stdint.h>
#include <stdbool.h>

#if defined (__cplusplus)
extern "C" {
#endif

void low_stack_init(void);
bool low_stack_task_is_running(void);
void low_stack_wake(void);

extern uint8_t (*rws_read_bt_piconet)(uint16_t, uint8_t *, uint8_t *);

uint8_t read_bt_rssi_callback(uint16_t conn_handle, int8_t *last_rssi, int8_t *avg_rssi);

/** @defgroup  HAL_87x3d_LOW_STACK    Lower Stack API
    * @brief lower stack API
    * @{
    */
/** @defgroup HAL_87x3d_LOW_STACK_Exported_Functions LOW STACK Exported Functions
    * @{
    */

/**
 * @brief Encrypt a 16-byte data
 * @param key  Encryption key
 * @param data Input data to encrypt
 * @param out  Output data encrypted
 * @return None
 */
extern void (*ll_driver_encrypt_16bytes)(void *key, void *data, void *out);
#define LL_Driver_Encrypt_16Bytes() ll_driver_encrypt_16bytes();

/**
 * @brief Generate a 32bit random value
 * @return A 32bit random value
 */
extern uint32_t (*ll_driver_random)(void);
#define LL_Driver_Random() ll_driver_random();

/**
 * @brief Read bluetooth piconet clock for application
 * @param conn_handle   identify which connection to be used
 * @param bb_clk_timer  piconet clk[27:0] (unit : half slot)
 * @param bb_clk_us     count up timer (range 0~624 us) \n
 * \p if bb_clk_timer[0] = 1, 312 <= bb_clk_us <= 624; \n
 * \p if bb_clk_timer[0] = 0, 0 <= bb_clk_us <= 312;
 * @return A 8bit value
 */
extern uint8_t (*rws_read_bt_piconet_clk)(uint16_t conn_handle, uint32_t *bb_clk_timer,
                                          uint16_t *bb_clk_us);
#define RWS_Read_BT_Piconet_CLK() rws_read_bt_piconet_clk();

/**
 *
 * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
 *
 */
extern bool (*bt_clk_index_read)(uint16_t conn_handle, uint8_t *clk_index, uint8_t *role);

/**
 * @brief Return expected A2DP sequence number of the recovery link
 * @param conn_handle  Connection handle of the recovery link which must be
 * primary role
 * @return 0x0000~0xFFFF: valid expected A2DP sequence number of the recovery
 * link; negative number is error code
 */
extern int32_t (*recovery_link_get_expected_a2dp_seq)(uint16_t conn_handle);
#define Recovery_Link_Get_Expected_A2dp_Seq() recovery_link_get_expected_a2dp_seq();

extern void (*lc_get_high_dpi_native_clock)(uint32_t *, uint16_t *);

/** @} */ /* End of group HAL_87x3d_LOWERSTACK_Exported_Functions lowerstack Exported Functions */
/** @} */ /* End of group HAL_87x3d_LOWERSTACK */

#if defined (__cplusplus)
}
#endif

#endif /* ! LOW_STACK_H_ */
