/*
 * Copyright (c) 2018, Realsil Semiconductor Corporation. All rights reserved.
 */

#ifndef _Pll_H_
#define _Pll_H_

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


/**
 * @brief Enable PLL4 in 160M.

*/
/**
 * pll.h
 *
 * \brief   Enable PLL4 in 160M..
 * \param[in]  none.
 * \return     none.
 * @note the pll4 is fixed to 160M due to some limitation.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     pll4_enable_cko1();
 * }
 * \endcode
 *
 * \ingroup  PLL
 */
void pll4_enable_cko1(void);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _Pll_H_ */
