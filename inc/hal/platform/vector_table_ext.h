/**
*********************************************************************************************************
*               Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     vector_table_ext.h
* @brief    Vector table extend implementation header file
* @details
* @author   yuhungweng
* @date     2021-9-02
* @version  v1.0
*********************************************************************************************************
*/

#include "stdbool.h"
#include "vector_table.h"

/** @defgroup  HAL_VectorTable    Vector Table EXT
    * @brief VectorTable wrapper.
    * @{
    */

/** @defgroup HAL_VectorTable_Exported_Functions Vector Table EXT Exported Functions
    * @brief
    * @{
    */

/**
    * @brief    Update vector table in RAM.
    * @param    v_num  Vector number.
    * @param    isr_handler ISR execution function.
    * @retval   true Success.
    * @retval   false Fail.
    * @note     Update ISR execution function.
    */
bool RamVectorTableUpdate(VECTORn_Type v_num, IRQ_Fun isr_handler);

/** @} */ /* End of group HAL_VectorTable_Exported_Functions */

/** @} */ /* End of group HAL_VectorTable */


