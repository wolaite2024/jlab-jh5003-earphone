/**
 *****************************************************************************************
 *     Copyright(c) 2018, Realtek Semiconductor Corporation. All rights reserved.
 *****************************************************************************************
 * @file    fmc_platform.h
 * @brief   Flexible memory controller (SPIC & PSRAMC) platform dependent implementation header file
 * @author  Yao-Yu
 * @date    2020-10-20
 * @version v0.1
 * ***************************************************************************************
 */

#ifndef _FMC_PLATFORM_H
#define _FMC_PLATFORM_H

/****************************************************************************************
 * Flexible Memory Controller Address Map
 ****************************************************************************************/
// if there is no cache, then UNCACHEABLE_ADDR should equal to FMC_MAIN_ADDR
#define FMC_MAIN0_ADDR                                      (0x02000000)
#define FMC_MAIN0_UNCACHEABLE_ADDR                          (0x04000000)
#define FMC_MAIN1_ADDR                                      (0x06000000)
#define FMC_MAIN1_UNCACHEABLE_ADDR                          (0x06000000)
#define FMC_MAIN2_ADDR                                      (0x08000000)
#define FMC_MAIN2_UNCACHEABLE_ADDR                          (0x08000000)
#define FMC_MAIN3_ADDR                                      (0x10000000)
#define FMC_MAIN3_UNCACHEABLE_ADDR                          (0x10000000)
#define FMC_MAIN4_ADDR                                      (0xC0000000)
#define FMC_MAIN4_UNCACHEABLE_ADDR                          (0xC0000000)
#define FMC_PSRAM_ADDR                                      (0xA6000000)
#define FMC_PSRAM_UNCACHEABLE_ADDR                          (0xA6000000)
#define FMC_MAIN0_SIZE                                      ( 32 * 1024 * 1024)
#define FMC_MAIN1_SIZE                                      ( 32 * 1024 * 1024)
#define FMC_MAIN2_SIZE                                      (  8 * 1024 * 1024)
#define FMC_MAIN3_SIZE                                      (256 * 1024 * 1024)
#define FMC_MAIN4_SIZE                                      (256 * 1024 * 1024)
#define FMC_PSRAM_SIZE                                      ( 32 * 1024 * 1024)
#define FMC_MAIN0_NON_CACHE_ADDR(cache_addr)                ((cache_addr) & ~(FMC_MAIN0_ADDR) | (FMC_MAIN0_UNCACHEABLE_ADDR))
#define FMC_MAIN1_NON_CACHE_ADDR(cache_addr)                ((cache_addr) & ~(FMC_MAIN1_ADDR) | (FMC_MAIN1_UNCACHEABLE_ADDR))
#define FMC_MAIN2_NON_CACHE_ADDR(cache_addr)                ((cache_addr) & ~(FMC_MAIN2_ADDR) | (FMC_MAIN2_UNCACHEABLE_ADDR))
#define FMC_MAIN3_NON_CACHE_ADDR(cache_addr)                ((cache_addr) & ~(FMC_MAIN3_ADDR) | (FMC_MAIN3_UNCACHEABLE_ADDR))
#define FMC_MAIN4_NON_CACHE_ADDR(cache_addr)                ((cache_addr) & ~(FMC_MAIN4_ADDR) | (FMC_MAIN4_UNCACHEABLE_ADDR))
#define FMC_PSRAM_NON_CACHE_ADDR(cache_addr)                ((cache_addr) & ~(FMC_PSRAM_ADDR) | (FMC_PSRAM_UNCACHEABLE_ADDR))

/****************************************************************************************
 * Flexible Memory Controller Address Map Checking
 ****************************************************************************************/
#define FMC_IS_SPIC0_CACHEABLE_ADDR(addr)                   ((addr >= FMC_MAIN0_ADDR) && (addr < FMC_MAIN0_ADDR + FMC_MAIN0_SIZE))
#define FMC_IS_SPIC0_UNCACHEABLE_ADDR(addr)                 ((addr >= FMC_MAIN0_UNCACHEABLE_ADDR) && (addr < FMC_MAIN0_UNCACHEABLE_ADDR + FMC_MAIN0_SIZE))
#define FMC_IS_SPIC0_ADDR(addr)                             (FMC_IS_SPIC0_CACHEABLE_ADDR(addr) || FMC_IS_SPIC0_UNCACHEABLE_ADDR(addr))
#define FMC_IS_SPIC1_CACHEABLE_ADDR(addr)                   ((addr >= FMC_MAIN1_ADDR) && (addr < FMC_MAIN1_ADDR + FMC_MAIN1_SIZE))
#define FMC_IS_SPIC1_UNCACHEABLE_ADDR(addr)                 ((addr >= FMC_MAIN1_UNCACHEABLE_ADDR) && (addr < FMC_MAIN1_UNCACHEABLE_ADDR + FMC_MAIN1_SIZE))
#define FMC_IS_SPIC1_ADDR(addr)                             (FMC_IS_SPIC1_CACHEABLE_ADDR(addr) || FMC_IS_SPIC1_UNCACHEABLE_ADDR(addr))
#define FMC_IS_SPIC2_CACHEABLE_ADDR(addr)                   ((addr >= FMC_MAIN2_ADDR) && (addr < FMC_MAIN2_ADDR + FMC_MAIN2_SIZE))
#define FMC_IS_SPIC2_UNCACHEABLE_ADDR(addr)                 ((addr >= FMC_MAIN2_UNCACHEABLE_ADDR) && (addr < FMC_MAIN2_UNCACHEABLE_ADDR + FMC_MAIN2_SIZE))
#define FMC_IS_SPIC2_ADDR(addr)                             (FMC_IS_SPIC2_CACHEABLE_ADDR(addr) || FMC_IS_SPIC2_UNCACHEABLE_ADDR(addr))
#define FMC_IS_SPIC3_CACHEABLE_ADDR(addr)                   ((addr >= FMC_MAIN3_ADDR) && (addr < FMC_MAIN3_ADDR + FMC_MAIN3_SIZE))
#define FMC_IS_SPIC3_UNCACHEABLE_ADDR(addr)                 ((addr >= FMC_MAIN3_UNCACHEABLE_ADDR) && (addr < FMC_MAIN3_UNCACHEABLE_ADDR + FMC_MAIN3_SIZE))
#define FMC_IS_SPIC3_ADDR(addr)                             (FMC_IS_SPIC3_CACHEABLE_ADDR(addr) || FMC_IS_SPIC3_UNCACHEABLE_ADDR(addr))
#define FMC_IS_SPIC4_CACHEABLE_ADDR(addr)                   ((addr >= FMC_MAIN4_ADDR) && (addr < FMC_MAIN4_ADDR + FMC_MAIN4_SIZE))
#define FMC_IS_SPIC4_UNCACHEABLE_ADDR(addr)                 ((addr >= FMC_MAIN4_UNCACHEABLE_ADDR) && (addr < FMC_MAIN4_UNCACHEABLE_ADDR + FMC_MAIN4_SIZE))
#define FMC_IS_SPIC4_ADDR(addr)                             (FMC_IS_SPIC4_CACHEABLE_ADDR(addr) || FMC_IS_SPIC4_UNCACHEABLE_ADDR(addr))
#define FMC_IS_PSRAMC_CACHEABLE_ADDR(addr)                  ((addr >= FMC_PSRAM_ADDR) && (addr < FMC_PSRAM_ADDR + FMC_PSRAM_SIZE))
#define FMC_IS_PSRAMC_UNCACHEABLE_ADDR(addr)                ((addr >= FMC_PSRAM_UNCACHEABLE_ADDR) && (addr < FMC_PSRAM_UNCACHEABLE_ADDR + FMC_PSRAM_SIZE))
#define FMC_IS_PSRAMC_ADDR(addr)                            (FMC_IS_PSRAMC_CACHEABLE_ADDR(addr) || FMC_IS_PSRAMC_UNCACHEABLE_ADDR(addr))

#endif
