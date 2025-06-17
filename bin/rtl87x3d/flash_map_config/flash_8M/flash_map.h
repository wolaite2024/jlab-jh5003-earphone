/** Copyright(c) 2018, Realtek Semiconductor Corporation.All rights reserved.*/

#ifndef _FLASH_MAP_H_
#define _FLASH_MAP_H_

#define IMG_HDR_SIZE                    0x00001000  //Changable, default 4K, modify in eFuse if needed

#define CFG_FILE_PAYLOAD_LEN            0x00001000  //Fixed

/* ========== High Level Flash Layout Configuration ========== */
#define EQ_FITTING_ADDR                 0x02000000
#define EQ_FITTING_SIZE                 0x00000400  //1K Bytes
#define RSV_ADDR                        0x02000400
#define RSV_SIZE                        0x00001C00  //7K Bytes
#define OEM_CFG_ADDR                    0x02002000
#define OEM_CFG_SIZE                    0x00002000  //8K Bytes
#define BOOT_PATCH_ADDR                 0x02004000
#define BOOT_PATCH_SIZE                 0x00002000  //8K Bytes
#define OTA_BANK0_ADDR                  0x02006000
#define OTA_BANK0_SIZE                  0x003F5000  //4052K Bytes
#define OTA_BANK1_ADDR                  0x023FB000
#define OTA_BANK1_SIZE                  0x003F5000  //4052K Bytes
#define FTL_ADDR                        0x027F0000
#define FTL_SIZE                        0x0000F000  //60K Bytes
#define BKP_DATA1_ADDR                  0x00000000
#define BKP_DATA1_SIZE                  0x00000000  //0K Bytes
#define BKP_DATA2_ADDR                  0x00000000
#define BKP_DATA2_SIZE                  0x00000000  //0K Bytes
#define OTA_TMP_ADDR                    0x00000000
#define OTA_TMP_SIZE                    0x00000000  //0K Bytes
#define HARDFAULT_REC_ADDR              0x027FF000
#define HARDFAULT_REC_SIZE              0x00001000  //4K Bytes

/* ========== OTA Bank0 Flash Layout Configuration ========== */
#define BANK0_OTA_HDR_ADDR              0x02006000
#define BANK0_OTA_HDR_SIZE              0x00001000  //4K Bytes
#define BANK0_FSBL_ADDR                 0x02007000
#define BANK0_FSBL_SIZE                 0x00003000  //12K Bytes
#define BANK0_ROM_PATCH_ADDR            0x0200A000
#define BANK0_ROM_PATCH_SIZE            0x0001c000  //112K Bytes
#define BANK0_STACK_PATCH_ADDR          0x02026000
#define BANK0_STACK_PATCH_SIZE          0x0003c000  //240K Bytes
#define BANK0_APP_ADDR                  0x02062000
#define BANK0_APP_SIZE                  0x001DE000  //1912K Bytes
#define BANK0_DSP_SYS_ADDR              0x02240000
#define BANK0_DSP_SYS_SIZE              0x000A0000  //640K Bytes
#define BANK0_DSP_APP_ADDR              0x022E0000
#define BANK0_DSP_APP_SIZE              0x000c0000  //768K Bytes
#define BANK0_DSP_CFG_ADDR              0x023A0000
#define BANK0_DSP_CFG_SIZE              0x0000A000  //40K Bytes
#define BANK0_APP_CFG_ADDR              0x023AA000
#define BANK0_APP_CFG_SIZE              0x0001A000  //104K Bytes
#define BANK0_EXT_IMG0_ADDR             0x023C4000
#define BANK0_EXT_IMG0_SIZE             0x0000A000  //40K Bytes
#define BANK0_EXT_IMG1_ADDR             0x023CE000
#define BANK0_EXT_IMG1_SIZE             0x0000A000  //40K Bytes
#define BANK0_EXT_IMG2_ADDR             0x023D8000
#define BANK0_EXT_IMG2_SIZE             0x0000A000  //40K Bytes
#define BANK0_EXT_IMG3_ADDR             0x023E2000
#define BANK0_EXT_IMG3_SIZE             0x00019000  //100K Bytes

/* ========== OTA Bank1 Flash Layout Configuration ========== */
#define BANK1_OTA_HDR_ADDR              0x023FB000
#define BANK1_OTA_HDR_SIZE              0x00001000  //4K Bytes
#define BANK1_FSBL_ADDR                 0x023FC000
#define BANK1_FSBL_SIZE                 0x00003000  //12K Bytes
#define BANK1_ROM_PATCH_ADDR            0x023FF000
#define BANK1_ROM_PATCH_SIZE            0x0001c000  //112K Bytes
#define BANK1_STACK_PATCH_ADDR          0x0241B000
#define BANK1_STACK_PATCH_SIZE          0x0003c000  //240K Bytes
#define BANK1_APP_ADDR                  0x02457000
#define BANK1_APP_SIZE                  0x001DE000  //1912 Bytes
#define BANK1_DSP_SYS_ADDR              0x02635000
#define BANK1_DSP_SYS_SIZE              0x000A0000  //640K Bytes
#define BANK1_DSP_APP_ADDR              0x026D5000
#define BANK1_DSP_APP_SIZE              0x000C0000  //768K Bytes
#define BANK1_DSP_CFG_ADDR              0x02795000
#define BANK1_DSP_CFG_SIZE              0x0000A000  //40K Bytes
#define BANK1_APP_CFG_ADDR              0x0279F000
#define BANK1_APP_CFG_SIZE              0x0001A000  //104K Bytes
#define BANK1_EXT_IMG0_ADDR             0x027B9000
#define BANK1_EXT_IMG0_SIZE             0x0000A000  //40K Bytes
#define BANK1_EXT_IMG1_ADDR             0x027C3000
#define BANK1_EXT_IMG1_SIZE             0x0000A000  //40K Bytes
#define BANK1_EXT_IMG2_ADDR             0x027CD000
#define BANK1_EXT_IMG2_SIZE             0x0000A000  //40K Bytes
#define BANK1_EXT_IMG3_ADDR             0x027D7000
#define BANK1_EXT_IMG3_SIZE             0x00019000  //100K Bytes


#endif /* _FLASH_MAP_H_ */
