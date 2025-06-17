/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     pmu_api.c
* @brief    This file provides pmu api wrapper for sdk customer.
* @details
* @author   mj_mengjie_han
* @date     2021-04-07
* @version  v1.0
* *************************************************************************************/
/*============================================================================*
 *               Define to prevent recursive inclusion
 *============================================================================*/
#ifndef __PMU_API_H_
#define __PMU_API_H_


/*============================================================================*
 *                               Header Files
*============================================================================*/
#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    PMU_CODEC   = 0x01,
    PMU_USB     = 0x02,
} PMU_MODULE;

void pmu_set_clk_32k_power_in_powerdown(bool para);

/**
 * pmu_api.h
 *
 * \brief   Set vcore2 to PON domain.
 *
 *
 * \param[in]  pmu_module     pmu module. @ref PMU_MODULE.
 *                              PMU_CODEC: Codec set vcore2 to PON domain
 *                              PMU_USB: USB set vcore2 to PON domain
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     pmu_vcore2_pon_domain_enable(PMU_CODEC);
 * }
 * \endcode
 *
 * \ingroup  PMU
 */
void pmu_vcore2_pon_domain_enable(PMU_MODULE pmu_module);

/**
 * pmu_api.h
 *
 * \brief   Set vcore2 to CORE domain.
 *
 *
 * \param[in]  pmu_module     pmu module. @ref PMU_MODULE.
 *                              PMU_CODEC: Codec set vcore2 to CORE domain
 *                              PMU_USB: USB set vcore2 to CORE domain
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     pmu_vcore2_pon_domain_disable(PMU_CODEC);
 * }
 * \endcode
 *
 * \ingroup  PMU
 */
void pmu_vcore2_pon_domain_disable(PMU_MODULE pmu_module);

#ifdef __cplusplus
}
#endif

#endif
