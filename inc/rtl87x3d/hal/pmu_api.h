/**
*****************************************************************************************
*     Copyright(c) 2022, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     pmu_api.h
* @brief    This file provides pmu API wrapper for sdk customer.
* @details
* @author
* @date     2022-12-22
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
#include "io_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    PMU_CODEC   = 0x01,
    PMU_USB     = 0x02,
} PMU_MODULE;

typedef enum
{
    LDO_TYPE_USB                         = 1,
    LDO_TYPE_AUX1                        = 2,
    LDO_TYPE_AUX2                        = 3,
    LDO_TYPE_AUX3                        = 4,
    LDO_TYPE_MAX_NUM                     = 8,
} T_LDO_TYPE;

typedef enum
{
    LDO_MODE_HQ                          = 0, /* support for LDO_AUX3, LDO_AUX1, LDO_AUX2, LDO_USB */
    LDO_MODE_NM                          = 1, /* support for LDO_AUX3, LDO_AUX1, LDO_AUX2, LDO_USB */
    LDO_MODE_LQ                          = 2, /* support for LDO_AUX3 */
    LDO_MODE_OFF                         = 3,
    LDO_MODE_MAX_NUM                     = 3,
    LDO_MODE_UNDEFINE                    = 4,
} T_LDO_MODE;

typedef enum
{
    POWER_ALWAYS_ACTIVE                   = 0, /* Active in all power mode */
    POWER_AON_DOMAIN                      = 1, /* Active in Power down/DLPS/Active mode. */
    POWER_PON_DOMAIN                      = 2, /* Active in DLPS/Active mode */
    POWER_CORE_DOMAIN                     = 4, /* Active in Active mode only */
    POWER_ALWAYS_INACTIVE                 = 6, /* Always inactive */
    POWER_DOMAIN_UNKNOWN                  = 7, /* Power domain unknown */
} T_POWER_DOMAIN;

/**
 * pmu_api.h
 *
 * \brief   Control LDO_PA on/off for thermistor power.
 *
 * \param[in]  is_enable     enable or disable LDO PA output.
 *
 * \return                   The status enable LDO PA.
 * \retval true              LDO PA is enabled successfully.
 * \retval false             Fail to enable ldo PA due to patch not support.
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     pmu_ldo_pa_control(true);
 *     // thermistor can now be accessed
 * }
 * \endcode
 *
 * \ingroup  PMU
 */
bool pmu_ldo_pa_control(bool is_enable);

/**
 * pmu_api.h
 *
 * \brief   Set vcore3 to PON domain.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Added API"
 *
 * \param[in]  pmu_module     pmu module. @ref PMU_MODULE.
 *                              PMU_CODEC: Codec set vcore3 to PON domain
 *                              PMU_USB: USB set vcore3 to PON domain
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     pmu_vcore3_pon_domain_enable(PMU_CODEC);
 * }
 * \endcode
 *
 * \ingroup  PMU
 */
void pmu_vcore3_pon_domain_enable(PMU_MODULE pmu_module);

/**
 * pmu_api.h
 *
 * \brief   Set vcore3 to CORE domain.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Added API"
 *
 * \param[in]  pmu_module     pmu module. @ref PMU_MODULE.
 *                              PMU_CODEC: Codec set vcore3 to CORE domain
 *                              PMU_USB: USB set vcore3 to CORE domain
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     pmu_vcore3_pon_domain_disable(PMU_CODEC);
 * }
 * \endcode
 *
 * \ingroup  PMU
 */
void pmu_vcore3_pon_domain_disable(PMU_MODULE pmu_module);

/**
 * pmu_api.h
 *
 * \brief   Set ldo hq to PON domain.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Added API"
 *
 * \param[in]  pmu_module     pmu module. @ref PMU_MODULE.
 *                              PMU_CODEC: Codec set ldo hq to PON domain
 *                              PMU_USB: USB set ldo hq to PON domain
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     pmu_ldo_hq_pon_domain_enable(PMU_CODEC);
 * }
 * \endcode
 *
 * \ingroup  PMU
 */
void pmu_ldo_hq_pon_domain_enable(PMU_MODULE pmu_module);

/**
 * pmu_api.h
 *
 * \brief   Set ldo hq to CORE domain.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Added API"
 *
 * \param[in]  pmu_module     pmu module. @ref PMU_MODULE.
 *                              PMU_CODEC: Codec set ldo hq to CORE domain
 *                              PMU_USB: USB set ldo hq to CORE domain
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     pmu_ldo_hq_pon_domain_disable(PMU_CODEC);
 * }
 * \endcode
 *
 * \ingroup  PMU
 */
void pmu_ldo_hq_pon_domain_disable(PMU_MODULE pmu_module);

/**
 * pmu_api.h
 *
 * \brief   Set audio hq to PON domain.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Added API"
 *
 * \param[in]  pmu_module     pmu module. @ref PMU_MODULE.
 *                              PMU_CODEC: Codec set audio hq to PON domain
 *                              PMU_USB: USB set audio hq to PON domain
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     pmu_audio_hq_mode_enable(PMU_CODEC);
 * }
 * \endcode
 *
 * \ingroup  PMU
 */
void pmu_audio_hq_mode_enable(PMU_MODULE pmu_module);

/**
 * pmu_api.h
 *
 * \brief   Set audio hq to CORE domain.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Added API"
 *
 * \param[in]  pmu_module     pmu module. @ref PMU_MODULE.
 *                              PMU_CODEC: Codec set audio hq to CORE domain
 *                              PMU_USB: USB set audio hq to CORE domain
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     pmu_audio_hq_mode_disable(PMU_CODEC);
 * }
 * \endcode
 *
 * \ingroup  PMU
 */
void pmu_audio_hq_mode_disable(PMU_MODULE pmu_module);

/**
 * pmu_api.h
 *
 * \brief   Enable xtal when enter dlps.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Added API"
 *
 * \param[in]  pmu_module     pmu module. @ref PMU_MODULE.
 *                              PMU_CODEC: Enable xtal when entering dlps for Codec on
 *                              PMU_USB: Enable xtal when entering dlps for USB on
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     pmu_xtal_use_enable(PMU_CODEC);
 * }
 * \endcode
 *
 * \ingroup  PMU
 */
void pmu_xtal_use_enable(PMU_MODULE pmu_module);

/**
 * pmu_api.h
 *
 * \brief   Disable xtal when enter dlps.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Added API"
 *
 * \param[in]  pmu_module     pmu module. @ref PMU_MODULE.
 *                              PMU_CODEC: Disable xtal when entering dlps for Codec off
 *                              PMU_USB: Disable xtal when entering dlps for USB off
 *
 * <b>Example usage</b>
 * \code{.c}
 * int test(void)
 * {
 *     pmu_xtal_use_disable(PMU_CODEC);
 * }
 * \endcode
 *
 * \ingroup  PMU
 */
void pmu_xtal_use_disable(PMU_MODULE pmu_module);

/**
 * pmu_api.h
 *
 * \brief   Set the LDO power domain mode.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Added API"
 *
 * \param[in] ldo_type         LDO type module. @ref LDO_TYPE.
 *      \arg      LDO_TYPE_USB     LDO_USB, can be set to LDO_MODE_HQ and LDO_MODE_NM
 *      \arg      LDO_TYPE_AUX1    LDO_AUX1, can be set to LDO_MODE_HQ and LDO_MODE_NM
 *      \arg      LDO_TYPE_AUX2    LDO_AUX2, can be set to LDO_MODE_HQ and LDO_MODE_NM
 *      \arg      LDO_TYPE_AUX3    LDO_AUX3, can be set to LDO_MODE_LQ and LDO_MODE_NM
 * \param[in] ldo_mode         LDO mode setting.
 *      \arg      LDO_MODE_HQ      LDO High quiescent mode
 *      \arg      LDO_MODE_NM      LDO Normal mode
 *      \arg      LDO_MODE_LQ      LDO Low quiescent mode
 * \param[in] power_domain     Power domain of LDO.
 *      \arg      ALWAYS_ACTIVE:   Active in all power mode
 *      \arg      AON_DOMAIN:      Active in Power down/DLPS/Active mode.
 *      \arg      PON_DOMAIN:      Active in DLPS/Active mode.
 *      \arg      CORE_DOMAIN:     Active in Active mode only.
 * \return  power_domain     the result of setting LDO power domain
 *      \retval      true:   The LDO power domain is set successfully
 *      \retval      false:  The LDO power domain is failed to set due to invalid parameter.
 * <b>Example usage</b>
 * \code{.c}
 * int main(void)
 * {
 *      pmu_api_set_ldo_power_domain(LDO_TYPE_AUX3, LDO_MODE_NM, POWER_PON_DOMAIN);
 * }
 * \endcode
 *
 * \ingroup  PMU
 */
bool pmu_api_set_ldo_power_domain(T_LDO_TYPE ldo_type, T_LDO_MODE ldo_mode,
                                  T_POWER_DOMAIN power_domain);


/**
 * pmu_api.h
 *
 * \brief   Get the LDO power domain mode.
 *
 * \xrefitem Experimental_Added_API_2_14_0_0 "Experimental Added Since 2.14.0.0" "Added API"
 *
 * \param[in]  ldo_type         LDO type module. @ref LDO_TYPE.
 *      \arg      LDO_TYPE_USB     LDO_USB, can be set to LDO_MODE_HQ and LDO_MODE_NM
 *      \arg      LDO_TYPE_AUX1    LDO_AUX1, can be set to LDO_MODE_HQ and LDO_MODE_NM
 *      \arg      LDO_TYPE_AUX2    LDO_AUX2, can be set to LDO_MODE_HQ and LDO_MODE_NM
 *      \arg      LDO_TYPE_AUX3    LDO_AUX3, can be set to LDO_MODE_LQ and LDO_MODE_NM
 * \param[in]  ldo_mode         LDO mode setting.
 *      \arg      LDO_MODE_HQ      LDO High quiescent mode
 *      \arg      LDO_MODE_NM      LDO Normal mode
 *      \arg      LDO_MODE_LQ      LDO Low quiescent mode
 * \return  power_domain     the current setting of the power domain
 *      \retval   POWER_ALWAYS_ACTIVE:   Active in all power mode
 *      \retval   POWER_AON_DOMAIN:      Active in Power down/DLPS/Active mode.
 *      \retval   POWER_PON_DOMAIN:      Active in DLPS/Active mode.
 *      \retval   POWER_CORE_DOMAIN:     Active in Active mode only.
 *      \retval   POWER_DOMAIN_UNKNOWN:  failed to get power domain due to invalid parameter.
 * <b>Example usage</b>
 * \code{.c}
 * int main(void)
 * {
 *      T_POWER_DOMAIN power_domain = pmu_api_get_ldo_power_domain(LDO_TYPE_AUX3, LDO_MODE_NM);
 * }
 * \endcode
 *
 * \ingroup  PMU
 */
T_POWER_DOMAIN pmu_api_get_ldo_power_domain(T_LDO_TYPE ldo_type, T_LDO_MODE ldo_mode);

#ifdef __cplusplus
}
#endif

#endif
