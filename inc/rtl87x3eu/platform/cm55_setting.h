#ifndef CM55_SETTING_H
#define CM55_SETTING_H

/* ----------------Configuration of the Processor and Core Peripherals---------------- */
#define __CM55_REV                     0x0001U   //!< Core revision r0p1
#define __SAUREGION_PRESENT            0U        /* SAU regions present */
#define __MPU_PRESENT                  1U        /* MPU present */
#define __VTOR_PRESENT                 1U        /* VTOR present */
#define __NVIC_PRIO_BITS               3U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig         0U        /* Set to 1 if different SysTick Config is used */
#define __FPU_PRESENT                  1U        /* FPU present */
#define __FPU_DP                       0U        /* single precision FPU */
#define __DSP_PRESENT                  1U        /* DSP extension present */
#define __ICACHE_PRESENT               1U        /* I-cache present */
#define __DCACHE_PRESENT               1U        /* D-cache present */

#include "vector_table.h"
#include "core_cm55.h"                       /* Processor and core peripherals */
#include "cachel1_armv7.h"

#endif // #define CM55_SETTING_H