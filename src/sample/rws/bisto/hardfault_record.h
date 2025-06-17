/////////////////////////////////////////////////
//
// HardFault_Record, HardFault_Record save to flash
// Realtek CN3-BT, Raven Su
//
/////////////////////////////////////////////////

#ifndef HardFault_Record_h
#define HardFault_Record_h

#include <stdint.h>

typedef struct
{
    uint32_t stacked_r0;
    uint32_t stacked_r1;
    uint32_t stacked_r2;
    uint32_t stacked_r3;
    uint32_t stacked_r4;
    uint32_t stacked_r5;
    uint32_t stacked_r6;
    uint32_t stacked_r7;
    uint32_t stacked_r8;
    uint32_t stacked_r9;
    uint32_t stacked_r10;
    uint32_t stacked_r11;
    uint32_t stacked_r12;
    uint32_t stacked_lr;
    uint32_t stacked_pc;
    uint32_t stacked_psr;
    uint32_t old_sp;
    uint32_t msp;
    uint32_t cfsr;
    uint32_t bus_fault_address;
    uint32_t memmanage_fault_address;

    uint32_t PRIMASK;
    uint32_t BASEPRI;
    uint32_t EXC_RETURN;

    uint32_t HFSR;
    uint32_t DFSR;
    uint32_t AFSR;
} T_EXCEPTION_RECORD;


typedef struct _hardfault_dump_page_beg
{
    uint8_t flag;
    uint8_t version;
    uint16_t magic_pattern;
    uint32_t dump_count;
    uint8_t uptime_hour;
    uint8_t uptime_minute;
    uint8_t uptime_second;
    uint8_t upload_flag;
    uint8_t log_length[3];
    uint8_t crc8;
    uint32_t fw_version;
    uint32_t reserved[7];
} HFDUMP_PAGE_BEG;

/******** bit definitions of hardfault configuration ********/
#define BIT_ENABLE_SAVE_HARDFAULT 0x00000001    /* BIT0 */
#define BIT_ENABLE_SAVE_COREDUMP 0x00000002    /*BIT1*/
#define BIT_ENABLE_DUMP_HARDFAULT 0x00000004    /* BIT2 */
#define BIT_CLEAR_HISTROY_AFTER_DUMP   0x00000008   /* BIT3 */
#define BIT_CLEAR_HISTROY_BEFORE_SAVING 0x00000010  /* BIT4 */

/**
 * @brief try to save to flash when hardfaul happens.
 * @param
 * @return
*/
extern void (*hardfault_try_save_to_flash)(T_EXCEPTION_RECORD *record);

/**
 * @brief try to save coredump to flash when hardfaul happens.
 * @param
 * @return
*/
extern void (*coredump_try_save_to_flash)(T_EXCEPTION_RECORD *record);

/**
 * @brief try to dump stored hardfaul happens from flash.
 * @param
 * @return
*/
extern void (*cpu_register_dump_history)(void);
/**
 * @brief dump registers when hardfault.
 * @param
 * @return
*/
void cpu_register_dump(T_EXCEPTION_RECORD *pHardFault_Record);

#endif

