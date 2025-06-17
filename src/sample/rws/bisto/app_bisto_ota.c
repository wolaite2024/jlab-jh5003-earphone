#if BISTO_FEATURE_SUPPORT

#include "app_bisto_ota.h"
#include "app_ota.h"
#include "app_bisto_msg.h"
#include "rtl876x_wdg.h"
#include "image_header.h"
#include "trace.h"
#include "fmc_api.h"
#include "rom_ext.h"
#include "dfu_api.h"


#define SUB_IMG_HEADERS0_OFFSET     MERGED_HEADER0_OFFSET + MERGED_HEADER_SIZE

#define SUB_IMG_MAX_NUM             18
#define SUB_BIN_NUM                 2
#define IMG_CTRL_HEADER_SIZE        12
#define PAYLOAD_BLOCK_SIZE          512



typedef enum
{
    OTA_EVENT_NONE,
    OTA_EVENT_BEGIN,
    OTA_EVENT_UPDATE_DATA,
    OTA_EVENT_STATE_ENTER,
} OTA_EVENT;


typedef enum
{
    OTA_IDLE                        = 0,
    OTA_PROC_COMBINATION_HEADER     = 1,
    OTA_PROC_MERGED_HEADER_INFO0    = 2,
    OTA_PROC_SUB_IMG_HEADERS0       = 3,
    OTA_PROC_MERGED_HEADER_INFO1    = 4,
    OTA_PROC_SUB_IMG_HEADERS1       = 5,
    OTA_PROC_SUB_IMG_CTRL_HEADER    = 6,
    OTA_PROC_SUB_IMG_PAYLOAD        = 7,
} OTA_STATE;


typedef enum
{
    SUB_IMG_WRITE_SUCCESS       = 0,
    SUB_IMG_WRITE_FAIL          = 1,
    SUB_IMG_WRITE_SUCCESS_LAST  = 2,
} SUB_IMG_WRITE_RES;


typedef enum
{
    CTRL_IMG_HEADER_STATE,
    CTRL_IMG_PAYLOAD_STATE,
} SUB_IMG_PROC_STATE;


typedef struct
{
    uint32_t    state;
    uint32_t    last_state;
    void       *param;
} APP_BISTO_OTA_STATE_ENTER_PARAM;


typedef struct
{
    SUB_IMG_HEADER  header;
    IMG_ID          img_id;
    uint32_t        start_offset;
} SUB_IMG_HEADER_INFO;


typedef struct
{
    SUB_IMG_HEADER  sub_img_hdr;
    uint32_t        sub_img_offset;
} APP_BISTO_SUB_IMG_INFO;


typedef struct
{
    MERGED_HEADER   merged_hdr;
    uint32_t        sub_img_num;
} APP_BISTO_MERGED_HDR_INFO;


typedef struct
{
    APP_BISTO_MERGED_HDR_INFO   merged_hdr_info;
    APP_BISTO_SUB_IMG_INFO      sub_img_infos[SUB_IMG_MAX_NUM];
} APP_BISTO_SUB_BIN_INFO;


typedef struct
{
    APP_BISTO_SUB_IMG_INFO const   *sub_img_infos;
    uint32_t                        start_sub_img_idx;
    uint32_t                        cur_sub_img_idx;
    uint32_t                        end_sub_img_idx;
    uint32_t                        start_write_addr;
    uint32_t                        cur_write_addr;
    uint32_t                        end_write_addr;
    uint32_t                        start_offset;
    uint32_t                        cur_offset;
    uint32_t                        end_offset;
} SUB_IMG_DOWNLOADER;


static struct
{
    OTA_STATE               state;
    COMBINATION_HEADER      combination_hdr;
    APP_BISTO_SUB_BIN_INFO  sub_bin_infos[SUB_BIN_NUM];
    SUB_IMG_DOWNLOADER      sub_img_downloader;
} app_bisto_ota =
{
    .state = OTA_IDLE,
    .combination_hdr = {0},
    .sub_bin_infos = {[0] = {0}, [1] = {0}},
    .sub_img_downloader = {0}
};



bool app_bisto_ota_event_handle(OTA_EVENT event, void *param, uint32_t param_len);


static void app_bisto_ota_sub_img_downloader_init(APP_BISTO_SUB_IMG_INFO const *sub_img_infos,
                                                  uint32_t start_sub_img_idx, uint32_t end_sub_img_idx)
{
    SUB_IMG_DOWNLOADER *p_downloader = &app_bisto_ota.sub_img_downloader;

    p_downloader->sub_img_infos = sub_img_infos;
    p_downloader->start_sub_img_idx = p_downloader->cur_sub_img_idx = start_sub_img_idx;
    p_downloader->end_sub_img_idx = end_sub_img_idx;
    p_downloader->start_write_addr = 0;
    p_downloader->cur_write_addr = 0;
    p_downloader->end_write_addr = 0;
    p_downloader->start_offset = 0;
    p_downloader->cur_offset = 0;
    p_downloader->end_offset = 0;
}


static void app_bisto_ota_sub_img_calc_offset(void)
{
    SUB_IMG_DOWNLOADER *p_downloader = &app_bisto_ota.sub_img_downloader;
    APP_BISTO_SUB_IMG_INFO const *p_sub_img_info =
        &p_downloader->sub_img_infos[p_downloader->cur_sub_img_idx];

    p_downloader->start_offset = p_sub_img_info->sub_img_offset + MP_HEADER_SIZE;
    p_downloader->cur_offset = p_downloader->start_offset;
    p_downloader->end_offset = p_sub_img_info->sub_img_offset + p_sub_img_info->sub_img_hdr.size;

    APP_PRINT_TRACE3("sub_img_downloader_calc_offset: start_offset %d, cur_offset %d, end_offset %d",
                     p_downloader->start_offset, p_downloader->cur_offset, p_downloader->end_offset);
}


static void app_bisto_ota_sub_img_calc_addr(void)
{
    SUB_IMG_DOWNLOADER      *p_downloader = &app_bisto_ota.sub_img_downloader;
    SUB_IMG_HEADER const   *p_sub_img_hdr =
        &p_downloader->sub_img_infos[p_downloader->cur_sub_img_idx].sub_img_hdr;

    //ToDo: remove MP_HEADER_SIZE for start addr
    p_downloader->start_write_addr = p_sub_img_hdr->download_addr;
    p_downloader->cur_write_addr = p_downloader->start_write_addr;
    p_downloader->end_write_addr = p_sub_img_hdr->download_addr + p_sub_img_hdr->size - MP_HEADER_SIZE;

    APP_PRINT_TRACE1("app_bisto_ota_sub_img_calc_addr: img flashing size %d KB",
                     (p_downloader->end_write_addr - p_downloader->start_write_addr) / 1024);

    APP_PRINT_TRACE3("app_bisto_ota_sub_img_calc_addr: start_write_addr 0x%08x, cur_write_addr 0x%08x, end_write_addr 0x%08x",
                     p_downloader->start_write_addr, p_downloader->cur_write_addr, p_downloader->end_write_addr);
}


static void app_bisto_ota_sub_img_hdr_read_request(void)
{
    SUB_IMG_DOWNLOADER  *p_downloader           = &app_bisto_ota.sub_img_downloader;

    APP_PRINT_TRACE2("app_bisto_ota_sub_img_hdr_read_request: cur_offset %d, size %d",
                     p_downloader->cur_offset, IMG_CTRL_HEADER_SIZE);

    bisto_ota_read_request(p_downloader->cur_offset, IMG_CTRL_HEADER_SIZE);
    p_downloader->cur_offset += IMG_CTRL_HEADER_SIZE;
}


static void app_bisto_ota_sub_img_erase(void)
{
    SUB_IMG_DOWNLOADER *p_downloader = &app_bisto_ota.sub_img_downloader;

    uint32_t erase_addr = p_downloader->start_write_addr;
    uint32_t image_total_len = p_downloader->end_write_addr - p_downloader->start_write_addr;
    uint32_t sector_num = (image_total_len + FMC_SEC_SECTION_LEN - 1) / FMC_SEC_SECTION_LEN;

    APP_PRINT_TRACE3("app_bisto_ota_sub_img_erase: erase_addr 0x%08x, image_total_len %d, sector_num %d",
                     erase_addr, image_total_len, sector_num);

    for (uint32_t i = 0; i < sector_num; i++, erase_addr += FMC_SEC_SECTION_LEN)
    {
        // ToDo: back erase
        fmc_flash_nor_erase(erase_addr, FMC_FLASH_NOR_ERASE_SECTOR);
    }
}


static bool app_bisto_ota_bisto_flash_write_with_chk(uint32_t addr, uint8_t *data, uint32_t len)
{
    uint8_t *readback = NULL;
    uint32_t readback_len = len < PAYLOAD_BLOCK_SIZE ? len : PAYLOAD_BLOCK_SIZE;
    int32_t  chk_times = (len + (readback_len - 1)) / readback_len;
    bool res = true;

    APP_PRINT_TRACE2("app_bisto_ota_bisto_flash_write_with_chk: readback_len %d, chk_times %d",
                     readback_len,
                     chk_times);

    readback = bisto_malloc(readback_len);
    if (readback == NULL)
    {
        return false;
    }

    uint8_t bp_level;
    app_flash_get_bp_lv((uint32_t)&bp_level);
    app_flash_set_bp_lv(0);

//  Todo:need flash write
    fmc_flash_nor_write(addr, (uint8_t *)data, len);

    app_flash_set_bp_lv(bp_level);

    //ToDo: recover flash check
    for (uint32_t i = 0, target_addr = addr; i < chk_times;
         i++, data += PAYLOAD_BLOCK_SIZE, target_addr += PAYLOAD_BLOCK_SIZE)
    {
        fmc_flash_nor_read(target_addr, readback, readback_len);
        if (memcmp(data, readback, len) != 0)
        {
            APP_PRINT_ERROR0("app_bisto_ota_bisto_flash_write_with_chk: flash write failed");
            res = false;
        }
    }

    bisto_free(readback);

    //ToDo: need write back to res
    return res;
}


static void app_bisto_ota_sub_img_write_ctrl_hdr(T_IMG_CTRL_HEADER_FORMAT *p_img_header)
{
    bool res = false;
    SUB_IMG_DOWNLOADER *p_downloader = &app_bisto_ota.sub_img_downloader;

    APP_PRINT_TRACE1("app_bisto_ota_sub_img_write_ctrl_hdr: cur_write_addr %p",
                     p_downloader->cur_write_addr);

    uint8_t bp_level;
    app_flash_get_bp_lv((uint32_t)&bp_level);
    app_flash_set_bp_lv(0);

    p_img_header->ctrl_flag.not_ready = 0x1;
    res = app_bisto_ota_bisto_flash_write_with_chk(p_downloader->cur_write_addr,
                                                   (uint8_t *)p_img_header,
                                                   IMG_CTRL_HEADER_SIZE);
    if (res == false)
    {
        APP_PRINT_ERROR0("app_bisto_ota_sub_img_write_ctrl_hdr: flash write failed");
    }

    p_downloader->cur_write_addr += IMG_CTRL_HEADER_SIZE;
}


static void app_bisto_ota_sub_img_payload_request(void)
{
    SUB_IMG_DOWNLOADER *p_downloader = &app_bisto_ota.sub_img_downloader;

    uint32_t remain_size = p_downloader->end_write_addr - p_downloader->cur_write_addr;
    uint32_t request_size = 0;

    request_size = PAYLOAD_BLOCK_SIZE < remain_size ? PAYLOAD_BLOCK_SIZE : remain_size;

    APP_PRINT_TRACE3("app_bisto_ota_sub_img_payload_request: cur_offset %d, remain_size %d, request_size %d",
                     p_downloader->cur_offset, remain_size, request_size);

    bisto_ota_read_request(p_downloader->cur_offset, request_size);
    p_downloader->cur_offset += request_size;
}


static SUB_IMG_WRITE_RES app_bisto_ota_sub_img_write_payload_blk(uint8_t *blk, uint32_t blk_len)
{
    bool res = false;
    SUB_IMG_DOWNLOADER *p_downloader = &app_bisto_ota.sub_img_downloader;

    APP_PRINT_TRACE2("app_bisto_ota_sub_img_write_payload_blk: addr 0x%08x, blk size %d",
                     p_downloader->cur_write_addr, blk_len);

    res = app_bisto_ota_bisto_flash_write_with_chk(p_downloader->cur_write_addr, blk, blk_len);
    if (res == false)
    {
        APP_PRINT_ERROR0("app_bisto_ota_sub_img_write_payload_blk: flash write fail");
    }

    p_downloader->cur_write_addr += blk_len;

    APP_PRINT_TRACE1("app_bisto_ota_sub_img_write_payload_blk: cur_write_addr %p",
                     p_downloader->cur_write_addr);

    if (p_downloader->cur_write_addr >= p_downloader->end_write_addr)
    {
        APP_PRINT_TRACE0("app_bisto_ota_sub_img_write_payload_blk: the last blk");

        T_IMG_CTRL_HEADER_FORMAT *img_ctrl_hdr = NULL;
        img_ctrl_hdr = (T_IMG_CTRL_HEADER_FORMAT *)p_downloader->start_write_addr;

        //Todo: recover this chk
        bool check_crc = dfu_check_sha256(img_ctrl_hdr, true);
        bool check_sha256 = dfu_check_crc(img_ctrl_hdr, true);

        APP_PRINT_INFO2("app_bisto_ota_sub_img_write_payload_blk: check_crc %d, crc_sha256 %d", check_crc,
                        check_sha256);
        //ToDo: recover crc check
//        if (check_crc == false)
//        {
//            APP_PRINT_ERROR0("sub_img_write_payload_blk: crc check faild");
//            return SUB_IMG_WRITE_FAIL;
//        }

        if (check_sha256 == false)
        {
            APP_PRINT_ERROR0("app_bisto_ota_sub_img_write_payload_blk: sha256 check faild");
            return SUB_IMG_WRITE_FAIL;
        }

        APP_PRINT_TRACE0("app_bisto_ota_sub_img_write_payload_blk: img_payload write finished");

        return SUB_IMG_WRITE_SUCCESS_LAST;
    }


    return SUB_IMG_WRITE_SUCCESS;
}


BISTO_ERR app_bisto_ota_apply(void)
{
    trigger_wdt_reset(RESET_ALL);

    return BISTO_OK;
}


bool app_bisto_ota_state_enter(OTA_STATE state, OTA_STATE last_state, void *param)
{
    app_bisto_msg_send(IO_BISTO_OTA_STATE_ENTER_MSG, (void *)state);
    return true;
}


bool app_bisto_ota_state_enter_handle(OTA_STATE state)
{
    app_bisto_ota_event_handle(OTA_EVENT_STATE_ENTER, (void *)state, 0);

    return true;
}


bool app_bisto_ota_proc_merged_hdr(APP_BISTO_MERGED_HDR_INFO *p_merged_hdr_info, uint8_t *data,
                                   uint32_t len)
{
    get_merged_header_by_stream(data, &p_merged_hdr_info->merged_hdr);
    p_merged_hdr_info->sub_img_num = get_file_num_by_file_indicator(
                                         p_merged_hdr_info->merged_hdr.sub_file_indicator);
    APP_PRINT_TRACE1("app_bisto_ota_proc_merged_hdr: sub_image_num %d", p_merged_hdr_info->sub_img_num);
    return true;
}


void app_bisto_ota_proc_sub_img_hdrs(APP_BISTO_SUB_IMG_INFO *sub_img_infos,
                                     uint32_t sub_img_num, uint8_t *data, uint32_t len, uint32_t start_offset)
{
    for (uint32_t i = 0; i < sub_img_num; i++)
    {
        get_sub_img_header_by_stream(data, &sub_img_infos[i].sub_img_hdr);
        //data ptr must move on!
        data += SUB_IMG_HEADER_SIZE;

        if (i == 0)
        {
            sub_img_infos[i].sub_img_offset = start_offset;
        }
        else
        {
            sub_img_infos[i].sub_img_offset = sub_img_infos[i - 1].sub_img_offset + sub_img_infos[i -
                                              1].sub_img_hdr.size;
        }

        APP_PRINT_TRACE2("app_bisto_ota_proc_sub_img_hdrs: img[%d].sub_img_offset %d",
                         i, sub_img_infos[i].sub_img_offset);
    }
}


static void app_bisto_ota_sub_img_preparation(void)
{
    app_bisto_ota_sub_img_calc_offset();
    app_bisto_ota_sub_img_calc_addr();
    app_bisto_ota_sub_img_erase();
}


static void app_bisto_ota_sub_img_preparation_new(void)
{
    SUB_IMG_DOWNLOADER *p_downloader = &app_bisto_ota.sub_img_downloader;
    p_downloader->cur_sub_img_idx++;
    APP_PRINT_TRACE1("app_bisto_ota_sub_img_preparation_new: cur_sub_img_idx %d",
                     p_downloader->cur_sub_img_idx);
    app_bisto_ota_sub_img_preparation();
}


static bool app_bisto_ota_is_last_sub_img(void)
{
    SUB_IMG_DOWNLOADER *p_downloader = &app_bisto_ota.sub_img_downloader;

    APP_PRINT_TRACE2("app_bisto_ota_is_last_sub_img: cur_sub_img_idx %d, end_sub_img_idx %d",
                     p_downloader->cur_sub_img_idx, p_downloader->end_sub_img_idx);

    return p_downloader->cur_sub_img_idx == p_downloader->end_sub_img_idx;
}


union
{
    uint16_t value;
    struct
    {
        uint16_t xip: 1; // payload is executed on flash
        uint16_t enc: 1; // all the payload is encrypted
        uint16_t load_when_boot: 1; // load image when boot
        uint16_t enc_load: 1; // encrypt load part or not
        uint16_t enc_key_select: 3; // referenced to ENC_KEY_SELECT
        uint16_t not_ready: 1; //for copy image in ota
        uint16_t not_obsolete: 1; //for copy image in ota
        uint16_t integrity_check_en_in_boot: 1; // enable image integrity check in boot flow
        uint16_t header_size: 4; // unit:KB, set for tool
        uint16_t rsvd: 2;
    };
} temp_ctrl_flag = {.value = 0};


static void app_bisto_ota_sub_img_clear_not_ready(void)
{
    T_IMG_HEADER_FORMAT *ota_hdr_img    = NULL;
    uint32_t            sub_img_addr    = 0;
    T_IMG_CTRL_HEADER_FORMAT *img_ctrl_header = NULL;


    ota_hdr_img = (T_IMG_HEADER_FORMAT *)get_temp_ota_bank_addr_by_img_id(IMG_OTA);

    for (IMG_ID img_id = IMG_OTA; img_id < IMG_MAX; img_id++)
    {
        sub_img_addr = get_temp_ota_bank_addr_by_img_id(img_id);
        img_ctrl_header = (T_IMG_CTRL_HEADER_FORMAT *)sub_img_addr;

        if (img_id != IMG_OTA && !HAL_READ32((uint32_t)&ota_hdr_img->fsbl_size, (img_id - IMG_SBL) * 8))
        {
            APP_PRINT_ERROR0("app_bisto_ota_sub_img_clear_not_ready: fsbl size check failed");
            continue;
        }

        if (img_ctrl_header == NULL || img_id != img_ctrl_header->image_id)
        {
            APP_PRINT_ERROR0("app_bisto_ota_sub_img_clear_not_ready: img_id check failed");
            continue;
        }

        fmc_flash_nor_read((uint32_t)&img_ctrl_header->ctrl_flag.value, (uint8_t *)&temp_ctrl_flag.value,
                           sizeof(temp_ctrl_flag.value));
        temp_ctrl_flag.not_ready = 0;

        app_bisto_ota_bisto_flash_write_with_chk((uint32_t)&img_ctrl_header->ctrl_flag.value,
                                                 (uint8_t *)&temp_ctrl_flag.value, sizeof(temp_ctrl_flag.value));

        APP_PRINT_TRACE1("app_bisto_ota_sub_img_clear_not_ready: clear not ready for img 0x%x successfully",
                         img_id);
    }
}


static bool app_bisto_ota_img_ctrl_hdr_trans_action(APP_BISTO_SUB_IMG_INFO *sub_img_infos,
                                                    uint32_t sub_img_num)
{
    uint8_t active_bank = app_ota_get_active_bank();
    uint32_t start_sub_img_idx = 0;
    uint32_t end_sub_img_idx = 0;

    if (active_bank == IMAGE_LOCATION_BANK0)
    {
        //set bank1 img idx
//ToDo: recover the img_idx
        start_sub_img_idx = sub_img_num / 2;
        end_sub_img_idx = sub_img_num - 1;
    }
    else
    {
        //set bank0 img idx
        start_sub_img_idx = 0;
        end_sub_img_idx = sub_img_num / 2 - 1;
    }

    APP_PRINT_TRACE3("app_bisto_ota_img_ctrl_hdr_trans_action: active bank %d, start_sub_img_idx %d, end_sub_img_idx %d",
                     active_bank, start_sub_img_idx, end_sub_img_idx);

    app_bisto_ota_sub_img_downloader_init(sub_img_infos, start_sub_img_idx, end_sub_img_idx);
    app_bisto_ota_sub_img_preparation();
    app_bisto_ota_sub_img_hdr_read_request();

    return true;
}


void app_bisto_ota_show_img_ctrl_header(T_IMG_CTRL_HEADER_FORMAT *p_img_header)
{
    APP_PRINT_INFO6("app_bisto_ota_show_img_ctrl_header 1: "
                    "ic_type 0x%02x, secure_version 0x%02x, value 0x%04x, image_id 0x%04x, crc16 0x%04x, payload_len 0x%08x",
                    p_img_header->ic_type, p_img_header->secure_version, p_img_header->ctrl_flag.value,
                    p_img_header->image_id, p_img_header->crc16, p_img_header->payload_len);

    APP_PRINT_INFO8("app_bisto_ota_show_img_ctrl_header 2: "
                    "xip %d, enc %d, load_when_boot %d, enc_load %d, enc_key_select %d, "
                    "not_ready %d, not_obsolete %d, integrity_check_en_in_boot %d",
                    p_img_header->ctrl_flag.xip, p_img_header->ctrl_flag.enc, p_img_header->ctrl_flag.load_when_boot,
                    p_img_header->ctrl_flag.enc_load, p_img_header->ctrl_flag.enc_key_select,
                    p_img_header->ctrl_flag.not_ready,
                    p_img_header->ctrl_flag.not_obsolete, p_img_header->ctrl_flag.integrity_check_en_in_boot);
}


bool app_bisto_ota_flash_write_with_chk(uint32_t dfu_addr, uint32_t length, uint8_t *data)
{
    return true;
}


OTA_STATE app_bisto_ota_sub_img_payload_proc(uint8_t *payload_blk, uint32_t blk_len)
{
    OTA_STATE new_state = OTA_PROC_SUB_IMG_PAYLOAD;

    SUB_IMG_WRITE_RES res = SUB_IMG_WRITE_FAIL;

    res = app_bisto_ota_sub_img_write_payload_blk(payload_blk, blk_len);
    if (res == SUB_IMG_WRITE_SUCCESS_LAST)
    {
        if (app_bisto_ota_is_last_sub_img())
        {
            app_bisto_ota_sub_img_clear_not_ready();
            bisto_ota_data_finish();
            new_state = OTA_IDLE;
            goto END;
        }

        app_bisto_ota_sub_img_preparation_new();
        app_bisto_ota_sub_img_hdr_read_request();
        new_state = OTA_PROC_SUB_IMG_CTRL_HEADER;
    }

    if (res == SUB_IMG_WRITE_SUCCESS)
    {
        app_bisto_ota_sub_img_payload_request();
        new_state = OTA_PROC_SUB_IMG_PAYLOAD;
    }


END:
    APP_PRINT_TRACE1("app_bisto_ota__sub_img_payload_proc: new_state %d", new_state);

    return new_state;
}


bool app_bisto_ota_event_handle(OTA_EVENT event, void *param, uint32_t param_len)
{
    APP_PRINT_TRACE3("app_bisto_ota_event_handle: state %d, event %d, param_len %d",
                     app_bisto_ota.state, event, param_len);

    switch (app_bisto_ota.state)
    {
    case OTA_IDLE:
        if (event == OTA_EVENT_BEGIN)
        {
            bisto_ota_read_request(0, COMBINATION_HEADER_SIZE);

            app_bisto_ota.state = OTA_PROC_COMBINATION_HEADER;
        }
        break;

    case OTA_PROC_COMBINATION_HEADER:
        if (event == OTA_EVENT_UPDATE_DATA)
        {
            uint8_t *data = param;
            get_combination_header_by_stream(data, &app_bisto_ota.combination_hdr);

            uint32_t merged_hdr_offset = 0 + COMBINATION_HEADER_SIZE;
            bisto_ota_read_request(merged_hdr_offset, MERGED_HEADER_SIZE);

            app_bisto_ota.state = OTA_PROC_MERGED_HEADER_INFO0;
        }
        break;

    case OTA_PROC_MERGED_HEADER_INFO0:
        if (event == OTA_EVENT_UPDATE_DATA)
        {
            uint32_t first_bin_idx = 0;
            app_bisto_ota_proc_merged_hdr(&app_bisto_ota.sub_bin_infos[first_bin_idx].merged_hdr_info, param,
                                          param_len);

            uint32_t sub_img_hdrs0_offset = COMBINATION_HEADER_SIZE + MERGED_HEADER_SIZE;
            bisto_ota_read_request(sub_img_hdrs0_offset,
                                   app_bisto_ota.sub_bin_infos[first_bin_idx].merged_hdr_info.sub_img_num * SUB_IMG_HEADER_SIZE);

            app_bisto_ota.state = OTA_PROC_SUB_IMG_HEADERS0;
        }
        break;

    case OTA_PROC_SUB_IMG_HEADERS0:
        {
            if (event == OTA_EVENT_UPDATE_DATA)
            {
                uint32_t first_bin_idx = 0;
                uint32_t start_offset = COMBINATION_HEADER_SIZE + MERGED_HEADER_SIZE + SUB_IMG_HEADER_SIZE *
                                        app_bisto_ota.sub_bin_infos[first_bin_idx].merged_hdr_info.sub_img_num;

                app_bisto_ota_proc_sub_img_hdrs(app_bisto_ota.sub_bin_infos[first_bin_idx].sub_img_infos,
                                                app_bisto_ota.sub_bin_infos[first_bin_idx].merged_hdr_info.sub_img_num,
                                                (uint8_t *)param, param_len, start_offset);

                uint32_t bin1_start_offset = COMBINATION_HEADER_SIZE +
                                             app_bisto_ota.combination_hdr.bin_infos[first_bin_idx].length;
                bisto_ota_read_request(bin1_start_offset, MERGED_HEADER_SIZE);

                app_bisto_ota.state = OTA_PROC_MERGED_HEADER_INFO1;
            }
        }
        break;

    case OTA_PROC_MERGED_HEADER_INFO1:
        if (event == OTA_EVENT_UPDATE_DATA)
        {
            uint32_t second_bin_idx = 1;


            app_bisto_ota_proc_merged_hdr(&app_bisto_ota.sub_bin_infos[second_bin_idx].merged_hdr_info, param,
                                          param_len);

            uint32_t sub_img_hdrs1_offset = COMBINATION_HEADER_SIZE +
                                            app_bisto_ota.combination_hdr.bin_infos[0].length + MERGED_HEADER_SIZE;
            bisto_ota_read_request(sub_img_hdrs1_offset,
                                   app_bisto_ota.sub_bin_infos[second_bin_idx].merged_hdr_info.sub_img_num * SUB_IMG_HEADER_SIZE);

            app_bisto_ota.state = OTA_PROC_SUB_IMG_HEADERS1;
        }
        break;

    case OTA_PROC_SUB_IMG_HEADERS1:
        if (event == OTA_EVENT_UPDATE_DATA)
        {
            uint32_t first_bin_len = app_bisto_ota.combination_hdr.bin_infos[0].length;
            APP_BISTO_SUB_BIN_INFO *p_sub_bin_info = &app_bisto_ota.sub_bin_infos[1];

            uint32_t start_offset = COMBINATION_HEADER_SIZE + first_bin_len + MERGED_HEADER_SIZE +
                                    app_bisto_ota.sub_bin_infos[1].merged_hdr_info.sub_img_num * SUB_IMG_HEADER_SIZE;

            app_bisto_ota_proc_sub_img_hdrs(p_sub_bin_info->sub_img_infos,
                                            p_sub_bin_info->merged_hdr_info.sub_img_num,
                                            (uint8_t *)param, param_len, start_offset);

            app_bisto_ota_img_ctrl_hdr_trans_action(app_bisto_ota.sub_bin_infos[0].sub_img_infos,
                                                    p_sub_bin_info->merged_hdr_info.sub_img_num);

            app_bisto_ota.state = OTA_PROC_SUB_IMG_CTRL_HEADER;
        }
        break;

    case OTA_PROC_SUB_IMG_CTRL_HEADER:
        if (event == OTA_EVENT_UPDATE_DATA)
        {
            T_IMG_CTRL_HEADER_FORMAT *p_img_header = param;
            app_bisto_ota_show_img_ctrl_header(p_img_header);
            app_bisto_ota_sub_img_write_ctrl_hdr(p_img_header);

            app_bisto_ota_sub_img_payload_request();
            app_bisto_ota.state = OTA_PROC_SUB_IMG_PAYLOAD;
        }
        break;

    case OTA_PROC_SUB_IMG_PAYLOAD:
        if (event == OTA_EVENT_UPDATE_DATA)
        {
            uint8_t *payload_blk = (uint8_t *)param;
            uint32_t blk_len = param_len;

            app_bisto_ota.state  = app_bisto_ota_sub_img_payload_proc(payload_blk, blk_len);
        }
        break;

    default:
        break;

    }

    return true;
}


bool app_bisto_ota_data_update(APP_BISTO_OTA_UPDATE_DATA *p_update_data)
{
    APP_PRINT_TRACE0("app_bisto_ota_data_update");

    app_bisto_ota_event_handle(OTA_EVENT_UPDATE_DATA, p_update_data->data, p_update_data->len);

    bisto_free(p_update_data->data);
    bisto_free(p_update_data);

    return true;
}


bool app_bisto_ota_data_update_pseudo(uint8_t *data, uint32_t len)
{
    APP_BISTO_OTA_UPDATE_DATA *p_update_data = NULL;

    p_update_data = bisto_malloc(sizeof(*p_update_data));
    if (p_update_data == NULL)
    {
        return false;
    }

    p_update_data->data = bisto_malloc(len);
    if (p_update_data->data == NULL)
    {
        bisto_free(p_update_data);
        return false;
    }

    memcpy(p_update_data->data, data, len);
    p_update_data->len = len;

    APP_PRINT_TRACE2("app_bisto_ota_data_update_pseudo: data %p, len %d", p_update_data->data,
                     p_update_data->len);

    app_bisto_msg_send(IO_BISTO_OTA_DATA_UPDATE_MSG, p_update_data);

    return true;
}


BISTO_ERR app_bisto_ota_begin_pseudo(char *ota_version, uint32_t image_len)
{
    app_bisto_msg_send(IO_BISTO_OTA_BEGIN_MSG, NULL);

    return BISTO_OK;
}


BISTO_ERR app_bisto_ota_begin(char *ota_version, uint32_t image_len)
{
    APP_PRINT_TRACE0("app_bisto_ota_begin");
    app_bisto_ota.state = OTA_IDLE;
    app_bisto_ota_event_handle(OTA_EVENT_BEGIN, NULL, 0);

    return BISTO_OK;
}


void app_bisto_ota_init(void)
{
    BISTO_OTA_CBS cbs = {NULL};
    cbs.ota_apply = app_bisto_ota_apply;
    cbs.ota_begin = app_bisto_ota_begin_pseudo;
    cbs.ota_abort = NULL;
    cbs.ota_is_allow = NULL;
    cbs.ota_data_update = app_bisto_ota_data_update_pseudo;

    app_bisto_ota.state = OTA_IDLE;

    bisto_ota_init(&cbs);
}


#endif

