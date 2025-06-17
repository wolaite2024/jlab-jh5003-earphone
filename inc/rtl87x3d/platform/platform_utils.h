/**
************************************************************************************************************
*               Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
************************************************************************************************************
* @file     platform_utils.h
* @brief    utility helper function for user application
* @author   lory_xu
* @date     2017-02
* @version  v1.0
*************************************************************************************************************
*/

#ifndef _PLATFORM_UTILS_H_
#define _PLATFORM_UTILS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*============================================================================*
 *                               Header Files
*============================================================================*/

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

/** @defgroup  87x3d_PLATFORM_UTILS Platform Utilities
    * @brief Utility helper functions
    * @{
    */
#define RTK_STATIC_ASSERT_MSG(MSG, COUNTER) static_assertion_##MSG##COUNTER
#define RTK_STATIC_ASSERT(COND,MSG) typedef char RTK_STATIC_ASSERT_MSG(MSG, __COUNTER__)[(COND)?1:-1];

#define SIZE_OF_TYPE_EQUAL_TO(type, size, msg) RTK_STATIC_ASSERT(sizeof(type) == size, msg)
#define BUILD_BUG_ON_ZERO(e) (sizeof(struct { int _; int:-!!(e); }) - sizeof(struct { int _; }))
#define SAME_TYPE(a, b) __builtin_types_compatible_p(typeof(a), typeof(b))
#define MUST_BE_ARRAY(a) BUILD_BUG_ON_ZERO(SAME_TYPE((a), &(*a)))
#define ARRAY_SIZE(a) ((sizeof(a) / sizeof(*a)) + MUST_BE_ARRAY(a))

/*============================================================================*
 *                              Variables
 *============================================================================*/

/** Little Endian array to uint16 */
#define LE_ATU16(u16, a)  {                   \
        u16 = ((uint16_t)(*(a + 0)) << 0) +             \
              ((uint16_t)(*(a + 1)) << 8);              \
    }

/** Little Endian array to uint32 */
#define LE_ATU32(u32, a) {                    \
        u32 = ((uint32_t)(*(a + 0)) <<  0) +            \
              ((uint32_t)(*(a + 1)) <<  8) +            \
              ((uint32_t)(*(a + 2)) << 16) +            \
              ((uint32_t)(*(a + 3)) << 24);             \
    }
/** Little Endian uint16 to array */
#define LE_U16TA(a, u16)  {                   \
        *((uint8_t *)(a) + 0) = (uint8_t)((u16) >> 0);  \
        *((uint8_t *)(a) + 1) = (uint8_t)((u16) >> 8);  \
    }

/** Little Endian uint32 to array */
#define LE_U32TA(a, u32) {                    \
        *((uint8_t *)(a) + 0) = (uint8_t)((u32) >>  0); \
        *((uint8_t *)(a) + 1) = (uint8_t)((u32) >>  8); \
        *((uint8_t *)(a) + 2) = (uint8_t)((u32) >> 16); \
        *((uint8_t *)(a) + 3) = (uint8_t)((u32) >> 24); \
    }


/*============================================================================*
 *                              Functions
 *============================================================================*/
/** @defgroup PLATFORM_UTILS_Exported_Functions Platform Utils Exported Functions
    * @brief
    * @{
    */
/**
 * @brief initialize random number
 */
void platform_init_random_number(void);

/**
 * @brief Generate random number given max number allowed
 * @param max   to specify max number that allowed
 * @return random number
 */
extern uint32_t (*platform_random)(uint32_t max);

/**
 * @brief Busy delay for specified millisecond
 * @param t   to specify t milliseconds to delay
 * @return none
 */
extern volatile void (*platform_delay_ms)(uint32_t t);

/**
 * @brief Busy delay for specified micro second
 * @param t   to specify t micro seconds to delay
 * @return none
 */
extern volatile void (*platform_delay_us)(uint32_t t);

/**
 *
 * \xrefitem Added_API_2_12_0_0 "Added Since 2.12.0.0" "Added API"
 *
 */
void empty_function(void);

uint8_t true_function(void);

uint8_t count_1bits(uint32_t x);

uint8_t count_0bits(uint32_t x);

uint8_t count_zeros(uint8_t *data, uint8_t byte_size);

bool data_cmp(const void *d1, const void *d2, size_t length);

bool is_overlapped(uint32_t range0[2], uint32_t range1[2]);

void cpu_snapshot_dump(void);

bool get_airplane_mode(void);
void set_airplane_mode(bool airplane_mode);



/** @} */ /* End of group PLATFORM_UTILS_Exported_Functions */

/** @} */ /* End of group 87x3d_PLATFORM_UTILS */

#ifdef __cplusplus
}
#endif

#endif

