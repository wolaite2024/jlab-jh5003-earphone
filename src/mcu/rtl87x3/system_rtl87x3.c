/**
*****************************************************************************************
*     Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file
   * @brief
   * @details
   * @author
   * @date
   * @version
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2016 Realtek Semiconductor Corporation</center></h2>
   * *************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include <locale.h>
#include <stdlib.h>
#if !(defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
#include "rtl876x.h"
#include "core_cmFunc.h"
#endif
#include "section.h"
#include "rom_uuid.h"
#include "trace.h"
#include "version.h"
#include "patch_header_check.h"
#include "mem_config.h"

#if ((SHARE_MEMORY_GLOBAL_SIZE !=0) || (SHARE_MEMORY_TEXT_SIZE !=0))
#include <string.h>
#endif

#include "os_mem.h"
#include "platform_utils.h"
#include "crc16btx.h"
#include "platform_ext.h"
#include "system_status_api.h"
#include "upperstack_compile_stamp.h"
#include "storage.h"
#include "flash_map.h"
#include "ftl.h"
#include "fmc_api.h"


#include "hal_def.h"
#if defined(CONFIG_SOC_SERIES_RTL8773D)
#include "dma_channel.h"
#endif

#if (SUPPORT_NAND_FLASH == 1)
#include "nftl.h"
#define FTL_SIZE (0x50000)
#define FS_SIZE (0x6000000)
#endif

/** @defgroup  SYSTEM_INIT System Init
    * @brief Start up code for user application.
    * @{
    */
/*============================================================================*
 *                              Macros
 *============================================================================*/
/** @defgroup SYSTEM_INIT_Exported_Macros System Init Exported Macros
    * @brief
    * @{
    */
/**
* @brief: Macro to enable image encryption, refer to encryption_demo project for detail.
*/
#define ENCRYPT_RAM_CODE 0

#define APP_FAKE_PAYLOAD_LEN (0x78000-DEFAULT_HEADER_SIZE)

#define VP_OFFSET                 (4096 + 3072)
#define VP_SIZE                   (137 * 1024)
#define TONE_OFFSET               4096
#define TONE_SIZE                 3072

/** End of SYSTEM_INIT_Exported_Macros
    * @}
    */

/*============================================================================*
 *                              Variables
 *============================================================================*/
/** @defgroup SYSTEM_INIT_Exported_Variables System Init Exported Variables
    * @{
    */
#if defined ( __CC_ARM   ) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
#if (ENCRYPT_RAM_CODE == 1)
extern char Image$$RAM_TEXT$$RO$$Base[];
extern char Load$$RAM_TEXT$$RO$$Base[];
extern char Load$$RAM_TEXT$$RO$$Length[];
#else
extern char Image$$FLASH_TEXT$$RO$$Base[];
extern char Load$$FLASH_TEXT$$RO$$Base[];
extern char Load$$FLASH_TEXT$$RO$$Length[];
extern char Image$$RAM_GLOBAL$$ZI$$Limit[];
#endif

extern char Load$$LR$$LOAD_FLASH$$Base[];
#else
extern unsigned int *__flash_start_exe_addr__;
extern unsigned int *__image_base__;
extern unsigned int *__ram_global_data_length__;

#endif

#if !(defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
#pragma push
#pragma diag_suppress 1296 /* disable warning 1296(extened constant initialiser used)*/
#endif

#ifdef CONFIG_SOC_SERIES_RTL8773G
void app_pre_init(void)
{
    uint32_t itcm_heap_addr = HEAP_ITCM_ADDR;
#if defined ( __CC_ARM   ) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
    uint32_t app_ram_data_size = (uint32_t)Image$$RAM_GLOBAL$$ZI$$Limit - APP_GLOBAL_ADDR;
    uint32_t app_heap_data_on_addr = (uint32_t)Image$$RAM_GLOBAL$$ZI$$Limit;
#else
    uint32_t app_ram_data_size = (uint32_t) &__ram_global_data_length__,
             uint32_t app_heap_data_on_addr = (uint32_t) &__ram_global_data_length__ + APP_GLOBAL_ADDR,
#endif
    uint32_t app_heap_data_off_size = HEAP_DATA_OFF_SIZE;
    hal_heap_size_update(itcm_heap_addr, app_ram_data_size, app_heap_data_on_addr,
                         app_heap_data_off_size);
    return;
}
#endif

/**
* @brief: application header.
* @note: items in ENCRYPT_RAM_CODE macro is for encrytion solution only
*/
#ifndef CONFIG_SOC_SERIES_RTL8773G
FLASH_HEADER const T_IMG_HEADER_FORMAT img_header =
{
    .auth =
    {
        .image_mac = {[0 ... 15] = 0xFF},
    },
    .ctrl_header =
    {
#if (CONFIG_SOC_SERIES_RTL8773D == 1 || TARGET_RTL8773DFL == 1)
        .ic_type = 0xA,
#elif defined TARGET_RTL8753GFE
        .ic_type = 0xD,
#elif (CONFIG_SOC_SERIES_RTL8773E == 1)
        .ic_type = 0x11,
#else
        .ic_type = 0xB,

#endif
        .secure_version = 0,
#if (ENCRYPT_RAM_CODE == 1)
        .ctrl_flag.xip = 0,
        .ctrl_flag.enc = 1,
        .ctrl_flag.load_when_boot = 1,
        .ctrl_flag.enc_load = 0,
        .ctrl_flag.enc_key_select = ENC_KEY_OCEK,
#else
        .ctrl_flag.xip = 1,
        .ctrl_flag.enc = 0,
        .ctrl_flag.load_when_boot = 0,
        .ctrl_flag.enc_load = 0,
        .ctrl_flag.enc_key_select = NULL,
#endif
        .ctrl_flag.not_obsolete = 1,
        .image_id = IMG_MCUAPP,
        .payload_len = APP_FAKE_PAYLOAD_LEN,    //Will modify by build tool later
    },
    .uuid = DEFINE_symboltable_uuid,

#if defined ( __CC_ARM   ) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
#if (ENCRYPT_RAM_CODE == 1)
    /* to be modified based on different user scenario */
    .load_src = (uint32_t)Load$$RAM_TEXT$$RO$$Base,
    .load_dst = (uint32_t)Image$$RAM_TEXT$$RO$$Base,
    .load_len = (uint32_t)Load$$RAM_TEXT$$RO$$Length,
    .exe_base = (uint32_t)Image$$RAM_TEXT$$RO$$Base,
#else
    /* XIP test */
    .load_src = (uint32_t)Load$$FLASH_TEXT$$RO$$Base,
    .load_dst = (uint32_t)Image$$FLASH_TEXT$$RO$$Base,
    .load_len = 0,  //0 indicates all XIP
    .exe_base = (uint32_t)Image$$FLASH_TEXT$$RO$$Base,
#endif
    .image_base = (uint32_t) &Load$$LR$$LOAD_FLASH$$Base,
#else
    .load_src = 0,
    .load_dst = 0,
    .load_len = 0,//0 indicates all XIP
    .exe_base = (uint32_t) &__flash_start_exe_addr__,
    .image_base = (uint32_t) &__image_base__,
#endif
    .git_ver.sub_version =
    {
        ._version_major = VERSION_MAJOR,
        ._version_minor = VERSION_MINOR,
        ._version_revision = VERSION_REVISION,
        ._version_reserve = 0x7ff & VERSION_BUILDNUM
    },
    .git_ver._version_commitid = VERSION_GCID,
#if (CONFIG_SOC_SERIES_RTL8773D == 1 || TARGET_RTL8773DFL == 1)
    .ex_info.app_ram_info.app_common_on_addr = HEAP_COMMON_ON_ADDR,
    .ex_info.app_ram_info.app_common_on_size = HEAP_COMMON_ON_SIZE,
    .ex_info.app_ram_info.app_common_off_size = HEAP_COMMON_OFF_SIZE,
    .ex_info.app_ram_info.app_common2_on_addr = SHARE_MEMORY_HEAP_ADDR,
    .ex_info.app_ram_info.app_common2_on_size = SHARE_MEMORY_HEAP_SIZE,
    .ex_info.app_ram_info.app_dsp_share_block = ((SHARE_MEMORY_HEAP_SIZE + SHARE_MEMORY_GLOBAL_SIZE + SHARE_MEMORY_TEXT_SIZE) / (32 * 1024)),
#else
    .ex_info.app_ram_info =
    {
        .itcm_heap_addr = HEAP_ITCM1_ADDR,
        .app_ram_data_size = APP_GLOBAL_SIZE,
        .app_heap_data_on_size = HEAP_DATA_ON_SIZE,
#if (CONFIG_SOC_SERIES_RTL8773E == 1)
        .app_heap_data_off_size = HEAP_DATA_OFF_SIZE,
#endif
    }
#endif
};
#else
FLASH_HEADER const T_IMG_HEADER_FORMAT img_header =
{
    .auth =
    {
        .img_string = {'I', 'M', 'G', 'H', 'D', 'R'},
        .image_mac = {[0 ... 15] = 0xFF},
    },
    .ctrl_header =
    {
        .ic_type = IC_TYPE,
        .secure_version = 0,
        .ctrl_flag.load_when_boot = 0,
        .ctrl_flag.not_obsolete = 1,
        .image_id = IMG_MCUAPP,
        .payload_len = APP_FAKE_PAYLOAD_LEN,    //Will modify by build tool later
    },
    .uuid = DEFINE_symboltable_uuid,

#if defined ( __CC_ARM   ) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
    /* XIP test */
    .ram_load_src = (uint32_t)Load$$FLASH_TEXT$$RO$$Base,
    .ram_load_dst = (uint32_t)Image$$FLASH_TEXT$$RO$$Base,
    .ram_load_len = 0,  //0 indicates all XIP
    .exe_entry = (uint32_t)Image$$FLASH_TEXT$$RO$$Base,
    .image_base = (uint32_t) &Load$$LR$$LOAD_FLASH$$Base,
#else
    .ram_load_src = 0,
    .ram_load_dst = 0,
    .ram_load_len = 0,//0 indicates all XIP
    .exe_entry = (uint32_t) &__flash_start_exe_addr__,
    .image_base = (uint32_t) &__image_base__,
#endif
    .encrypt_header =
    {
        .ctrl_flag.enc = 0,
    },
    .git_ver.sub_version =
    {
        ._version_major = VERSION_MAJOR,
        ._version_minor = VERSION_MINOR,
        ._version_revision = VERSION_REVISION,
        ._version_reserve = 0x7ff & VERSION_BUILDNUM
    },
    .git_ver._version_commitid = VERSION_GCID,
    .ex_info.app_ext_info =
    {
        .app_pre_init = app_pre_init,
    }
};
#endif

#if !(defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
#pragma pop
#endif
/**
 *@brief header extension for each user application
*/
//FLASH_HEADER_EXT const T_IMAGE_HEADER_EXT header_ext_patch = {};

/** End of SYSTEM_INIT_Exported_Variables
    * @}
    */

/*============================================================================*
 *                              Private Functions
 *============================================================================*/
/** @defgroup SYSTEM_INIT_Exported_Functions System Init Exported Functions
    * @{
    */

#if (SHARE_MEMORY_GLOBAL_SIZE !=0)
#include <string.h>
void common2_data_init(void)
{
#if defined ( __CC_ARM   ) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
    /*copy ro*/
    extern unsigned int Load$$SHARE_RAM_DATA$$RO$$Base;
    extern unsigned int Image$$SHARE_RAM_DATA$$RO$$Base;
    extern unsigned int Image$$SHARE_RAM_DATA$$RO$$Length;

    uint32_t data_len = (uint32_t)&Image$$SHARE_RAM_DATA$$RO$$Length;
    uint32_t data_start_addr = (uint32_t)&Image$$SHARE_RAM_DATA$$RO$$Base;
    uint32_t data_load_addr = (uint32_t)&Load$$SHARE_RAM_DATA$$RO$$Base;
    memcpy((uint32_t *)data_start_addr, (uint32_t *)data_load_addr, data_len);

    /*copy rw*/
    extern unsigned int Load$$SHARE_RAM_DATA$$RW$$Base;
    extern unsigned int Image$$SHARE_RAM_DATA$$RW$$Base;
    extern unsigned int Image$$SHARE_RAM_DATA$$RW$$Length;

    data_len = (uint32_t)&Image$$SHARE_RAM_DATA$$RW$$Length;
    data_start_addr = (uint32_t)&Image$$SHARE_RAM_DATA$$RW$$Base;
    data_load_addr = (uint32_t)&Load$$SHARE_RAM_DATA$$RW$$Base;
    memcpy((uint32_t *)data_start_addr, (uint32_t *)data_load_addr, data_len);

    /*clear bss*/
    extern unsigned int Image$$SHARE_RAM_DATA$$ZI$$Base;
    extern unsigned int Image$$SHARE_RAM_DATA$$ZI$$Length;
    uint32_t bss_start_addr = (uint32_t)&Image$$SHARE_RAM_DATA$$ZI$$Base;
    uint32_t bss_len = (uint32_t)&Image$$SHARE_RAM_DATA$$ZI$$Length;
    memset((uint32_t *)bss_start_addr, 0, bss_len);
#endif
}
#endif

#if (SHARE_MEMORY_TEXT_SIZE !=0)
void common2_text_init(void)
{
#if defined ( __CC_ARM   ) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
    /*copy ro*/
    extern unsigned int Load$$SHARE_RAM_TEXT$$RO$$Base;
    extern unsigned int Image$$SHARE_RAM_TEXT$$RO$$Base;
    extern unsigned int Image$$SHARE_RAM_TEXT$$RO$$Length;

    uint32_t data_len = (uint32_t)&Image$$SHARE_RAM_TEXT$$RO$$Length;
    uint32_t data_start_addr = (uint32_t)&Image$$SHARE_RAM_TEXT$$RO$$Base;
    uint32_t data_load_addr = (uint32_t)&Load$$SHARE_RAM_TEXT$$RO$$Base;
    memcpy((uint32_t *)data_start_addr, (uint32_t *)data_load_addr, data_len);

    /*copy rw*/
    extern unsigned int Load$$SHARE_RAM_TEXT$$RW$$Base;
    extern unsigned int Image$$SHARE_RAM_TEXT$$RW$$Base;
    extern unsigned int Image$$SHARE_RAM_TEXT$$RW$$Length;
    data_len = (uint32_t)&Image$$SHARE_RAM_TEXT$$RW$$Length;
    data_start_addr = (uint32_t)&Image$$SHARE_RAM_TEXT$$RW$$Base;
    data_load_addr = (uint32_t)&Load$$SHARE_RAM_TEXT$$RW$$Base;
    memcpy((uint32_t *)data_start_addr, (uint32_t *)data_load_addr, data_len);

    /*clear bss*/
    extern unsigned int Image$$SHARE_RAM_TEXT$$ZI$$Base;
    extern unsigned int Image$$SHARE_RAM_TEXT$$ZI$$Length;
    uint32_t bss_start_addr = (uint32_t)&Image$$SHARE_RAM_TEXT$$ZI$$Base;
    uint32_t bss_len = (uint32_t)&Image$$SHARE_RAM_TEXT$$ZI$$Length;
    memset((uint32_t *)bss_start_addr, 0, bss_len);
#endif
}
#endif

#if (SUPPORT_DFU_TASK == 1)
void ota_task_init(void)
{
    if (dfu_check_ota_mode_flag())
    {
        dfu_main();
        dfu_set_ota_mode_flag(false);
        vTaskStartScheduler();
    }
}
#endif // (SUPPORT_DFU_TASK == 1)

#if (FTL_POOL_ENABLE == 1)
int32_t ftl_init(const T_STORAGE_PARTITION_INFO *info)
{
    uint32_t ret = ESUCCESS;
#if (SUPPORT_NAND_FLASH == 0)
#ifdef TARGET_RTL8753GFE
    uint16_t rom_ftl_size = 0x1000;
#else
    uint16_t rom_ftl_size = 0x2000;
#endif

    ret = ftl_pool_init(info->address, info->size, rom_ftl_size);
    if (ret == ESUCCESS)
    {
        ftl_cache_init(128);
    }
#else
    ret = ftl_pool_init(0, info->size, 0);
#endif
    return ret;
}
#endif

static const T_STORAGE_PARTITION_INFO default_partitions[] =
{
    {
        .name = FTL_PARTITION_NAME,
#if (SUPPORT_NAND_FLASH == 1)
        .address = 0,
#else
        .address = FTL_ADDR,
#endif
        .size = FTL_SIZE,
        .perm = STORAGE_PERMISSION_READ,
        .image_id = 0,
        .media_type = STORAGE_MEDIA_TYPE_NOR,
        .content_type = STORAGE_CONTENT_TYPE_TEXT,
        .init = ftl_init,
        .read = NULL,
        .write = NULL,
        .erase = NULL,
        .async_read = NULL,
        .async_write = NULL,
        .async_erase = NULL,
    },
#ifdef UPPERSTACK_ADDR
    {
        .name = UPPERSTACK_PARTITION_NAME,
        .address = UPPERSTACK_ADDR,
        .size = UPPERSTACK_SIZE,
        .perm = STORAGE_PERMISSION_READ,
        .image_id = 0,
        .media_type = STORAGE_MEDIA_TYPE_NOR,
        .content_type = STORAGE_CONTENT_TYPE_TEXT,
        .init = NULL,
        .read = NULL,
        .write = NULL,
        .erase = NULL,
        .async_read = NULL,
        .async_write = NULL,
        .async_erase = NULL,
    },
#endif
#ifdef VP_DATA_ADDR
    {
        .name = VP_PARTITION_NAME,
        .address = VP_DATA_ADDR,
        .size = VP_DATA_SIZE,
        .perm = STORAGE_PERMISSION_READ,
        .image_id = FLASH_IMG_VP,
        .media_type = STORAGE_MEDIA_TYPE_NOR,
        .content_type = STORAGE_CONTENT_TYPE_RO_DATA,
        .init = NULL,
        .read = NULL,
        .write = NULL,
        .erase = NULL,
        .async_read = NULL,
        .async_write = NULL,
        .async_erase = NULL,
    },
#endif
#if (CONFIG_SOC_SERIES_RTL8773D == 1 || TARGET_RTL8773DFL == 1)
    {
        .name = VP_PARTITION_NAME,
        .address = VP_OFFSET,
        .size = VP_SIZE,
        .perm = STORAGE_PERMISSION_READ,
        .image_id = FLASH_IMG_VP,
        .media_type = STORAGE_MEDIA_TYPE_NOR,
        .content_type = STORAGE_CONTENT_TYPE_RO_DATA,
        .init = NULL,
        .read = NULL,
        .write = NULL,
        .erase = NULL,
        .async_read = NULL,
        .async_write = NULL,
        .async_erase = NULL,
    },
#endif
#ifndef CONFIG_SOC_SERIES_RTL8773G
    {
        .name = TONE_PARTITION_NAME,
        .address = TONE_OFFSET,
        .size = TONE_SIZE,
        .perm = STORAGE_PERMISSION_READ,
        .image_id = FLASH_IMG_TONE,
        .media_type = STORAGE_MEDIA_TYPE_NOR,
        .content_type = STORAGE_CONTENT_TYPE_RO_DATA,
        .init = NULL,
        .read = NULL,
        .write = NULL,
        .erase = NULL,
        .async_read = NULL,
        .async_write = NULL,
        .async_erase = NULL,
    },
#endif
#if (SUPPORT_NAND_FLASH == 1)
    {
        .name = FILESYSTEM_NAME,
        .size = FS_SIZE,
        .perm = STORAGE_PERMISSION_READ,
        .media_type = STORAGE_MEDIA_TYPE_NOR,
        .content_type = STORAGE_CONTENT_TYPE_TEXT,
        .init = NULL,
        .read = NULL,
        .write = NULL,
        .erase = NULL,
        .async_read = NULL,
        .async_write = NULL,
        .async_erase = NULL,
    },
#endif
};

/** @} */ /* End of group SYSTEM_INIT_Exported_Functions */
/** @} */ /* End of group SYSTEM_INIT */
/*============================================================================*
 *                              Public Functions
 *============================================================================*/
/**
 * @brief  Setup the microcontroller system.
 *         Initialize the System.
 * @param  none
 * @return none
 */
#if defined(CXX_support)
void *operator new (std::size_t size)
{
    void *ptr = malloc(size);

    return ptr;
}

void *operator new[](std::size_t size)
{
    void *ptr = malloc(size);
    return ptr;
}

void operator delete (void *ptr)
{
    free(ptr);
    return;
}

void operator delete[](void *ptr)
{
    free(ptr);
    return;
}
#endif

void system_init(void)
{
#if (SUPPORT_DFU_TASK == 1)
    ota_task_init();
#endif // (SUPPORT_DFU_TASK == 1)
    __disable_irq();
    setlocale(LC_ALL, "C");

#if defined (__CC_ARM) || (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
    extern uint32_t Load$$RAM_VECTOR_TABLE$$RO$$Base;
    extern uint32_t Load$$RAM_VECTOR_TABLE$$RO$$Length;
    extern void Default_Handler(void);
    vector_table_update((IRQ_Fun *)((uint32_t)&Load$$RAM_VECTOR_TABLE$$RO$$Base),
                        (uint32_t)&Load$$RAM_VECTOR_TABLE$$RO$$Length, Default_Handler);
#else
    extern uint32_t *__ram_vector_load_ad__;
    extern uint32_t __ram_vector_table_length__;
    extern void Default_Handler(void);
    vector_table_update((IRQ_Fun *)((uint32_t)&__ram_vector_load_ad__), __ram_vector_table_length__,
                        Default_Handler);
#endif
#if (CONFIG_SOC_SERIES_RTL8773D == 1 || TARGET_RTL8773DFL == 1)
    extern void RamLayoutUpdate(uint32_t common0_size, uint32_t common1_size);
    RamLayoutUpdate(COMMON_SRAM0_CFG_SIZE, COMMON_SRAM1_CFG_SIZE);

    extern void flash_task_init(void);
    flash_task_init();
#endif

#if (SUPPORT_NAND_FLASH == 1)
#include "flash_nand_adv_driver.h"
    extern void (*flash_nand_set_bp_lv)(uint32_t idx, uint8_t bp_lv);
    flash_nand_set_bp_lv(0, 0);
    nftl_init(NFTL_ADDR, NFTL_SIZE);
#endif

    storage_partition_init(default_partitions,
                           sizeof(default_partitions) / sizeof(default_partitions[0]));

#ifndef NOT_SUPPORT_UPPERSTACK
    uint8_t upperstack_compile_stamp[16] = DEFINE_upperstack_compile_stamp;
    sys_hall_upperstack_ini(upperstack_compile_stamp);
#endif

    uint32_t random_seed = platform_random(0xFFFFFFFF);
    srand(random_seed);

#if defined(IC_TYPE_RTL87x3E)
    extern void Pad_GetMic2AuxCoPadCfg(void);
    Pad_GetMic2AuxCoPadCfg();
#endif
    //comment: to enable debug monitor if desired
    //debug_monitor_enable();
    //debug_monitor_point_set(WATCH_POINT_INDEX0, 0x200000, WATCH_POINT_BYTE, WATCH_POINT_FUNCTION_DADDR_W);

#if (SHARE_MEMORY_GLOBAL_SIZE !=0)
    common2_data_init();
#endif

#if (SHARE_MEMORY_TEXT_SIZE !=0)
    common2_text_init();
#endif

#if defined(CONFIG_SOC_SERIES_RTL8773D)
    dma_dsp_channel_set(DMA_CHANNEL_DSP_RESERVED);
#endif

#if defined(CXX_support)
#if defined (__GNUC__)
    typedef void (*init_fn_t)(void);
    extern uint32_t _init_array;
    extern uint32_t _einit_array;
    init_fn_t *fp;
    // cppcheck-suppress comparePointers
    for (fp = (init_fn_t *)&_init_array; fp < (init_fn_t *)&_einit_array; fp++)
    {
        (*fp)();
    }
#elif defined (__ARMCC_VERSION)
    typedef void PROC();
    extern const unsigned long SHT$$INIT_ARRAY$$Base[];
    extern const unsigned long SHT$$INIT_ARRAY$$Limit[];

    const unsigned long *base = SHT$$INIT_ARRAY$$Base;
    const unsigned long *lim  = SHT$$INIT_ARRAY$$Limit;

    for (; base != lim; base++)
    {
        PROC *proc = (PROC *)((const char *)base + *base);
        (*proc)();
    }
#endif
#endif

    return;
}
/** @} */ /* End of group SYSTEM_INIT_Exported_Functions */
