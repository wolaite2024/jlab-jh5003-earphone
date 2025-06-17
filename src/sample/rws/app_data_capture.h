#ifndef _APP_DATA_CAPTURE_H_
#define _APP_DATA_CAPTURE_H_

#if F_APP_DATA_CAPTURE_SUPPORT
#define DATA_CAPTURE_DATA_START_MASK                 0x0001
#define DATA_CAPTURE_DATA_SWAP_TO_MASTER             0x0002
#define DATA_CAPTURE_DATA_START_SCO_MODE             0x0004
#define DATA_CAPTURE_DATA_CHANGE_TO_SCO_MODE         0x0008
#define DATA_CAPTURE_RAW_DATA_EXECUTING              0x0010
#define DATA_CAPTURE_DATA_LOG_EXECUTING              0x0020
#define DATA_CAPTURE_DATA_SAIYAN_EXECUTING           0x0040
#define DATA_CAPTURE_DATA_DSP2_MODE                  0x0080
#define DATA_CAPTURE_DATA_USER_MIC_EXECUTING         0x0100
#define DATA_CAPTURE_DATA_ENTER_TEST_MODE            0x0200
#define DATA_CAPTURE_DATA_ACOUSTICS_MP_EXECUTING     0x0400
#define DATA_CAPTURE_DATA_ENTER_START                0x0800

typedef enum
{
    ROLESWAP_EVENT_DATA_CAPTURE_RSV                  = 0x00,
    ROLESWAP_EVENT_DATA_CAPTURE_REPORT_BUD_SIDE      = 0x01,
    ROLESWAP_EVENT_DATA_CAPTURE_DATA_STATE           = 0x02,

    ROLESWAP_EVENT_DATA_CAPTURE_MAX                  = 0x03,
} T_DATA_CAPTURE_EVENT;

#if F_APP_SUPPORT_CAPTURE_DUAL_BUD | F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
typedef enum
{
    INVALID_MIC         = 0x00,
    FF_MIC              = 0x01,
    FB_MIC              = 0x02,
    VOICE_MIC           = 0x04,
    SECONDARY_VOICE_MIC = 0x08,
    PRIMARY_APT_MIC     = 0x10,
    SECONDARY_APT_MIC   = 0x20,
} T_MIC_INDEX;
#endif

typedef struct
{
    uint8_t     bandwith;
    uint8_t     tpoll;
    uint16_t    flush_tout;
    uint8_t     type_num;
    uint8_t     capture_enable;
} T_CAPTURE_HEADER;

typedef struct
{
    uint8_t le_op_code;
    uint8_t saiyan_enable;
    uint16_t rsv;
} T_SAIYAN_MODE;

extern uint16_t dsp_capture_data_state;
extern T_SAIYAN_MODE data_capture_saiyan;

uint16_t app_data_capture_get_state(void);

void app_data_capture_init(void);

void app_data_capture_cmd_handle(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                                 uint8_t app_idx,
                                 uint8_t *ack_pkt);
void app_data_capture_start_process(T_CAPTURE_HEADER *param, uint8_t cmd_path, uint8_t app_idx);
void app_data_capture_stop_process(uint8_t cmd_path, uint8_t app_idx);
bool app_data_capture_executing_check(void);
void app_data_capture_send_gain(void);
void app_data_capture_mode_ctl(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                               uint8_t app_idx, uint8_t *ack_pkt);
bool app_data_capture_need_roleswap(void);

#if F_APP_SAIYAN_MODE
void app_data_capture_saiyan_mode_ctl(uint8_t start, uint8_t op_code);
void app_data_capture_gain_ctl(uint8_t *cmd_ptr, uint16_t cmd_len, uint8_t cmd_path,
                               uint8_t app_idx, uint8_t *ack_pkt);
#endif

#if F_APP_SUPPORT_CAPTURE_ACOUSTICS_MP
T_MIC_INDEX app_data_capture_mic_index_get(void);
#endif

#endif

#endif
