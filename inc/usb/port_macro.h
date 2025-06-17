/**
 * @copyright Copyright (C) 2023 Realtek Semiconductor Corporation.
 *
 * @file port_macro.h
 * @version 1.0
 * @brief
 *
 * @note:
 */
#ifndef _PORT_MACRO_H_
#define _PORT_MACRO_H_
#include "os_mem.h"

#define USB_USER_SPEC_SECTION    __attribute__((section(".text.usb.user_spec")))
#define USB_ISOC_ISR_ENTRY_SECTION    __attribute__((section(".text.usb.isoc_isr_entry")))

#ifdef CONFIG_SOC_SERIES_RTL8763E
#define USB_PKT_RAM_ALLOC(size)   os_mem_alloc(OS_MEM_TYPE_BUFFER, size)
#else
#define USB_PKT_RAM_ALLOC(size)   os_mem_alloc(OS_MEM_TYPE_DATA, size)
#endif
#define USB_PKT_RAM_FREE(ptr)   os_mem_free(ptr)
#endif
