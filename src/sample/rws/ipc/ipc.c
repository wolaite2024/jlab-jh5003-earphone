#include "rtl876x.h"
#include "rtl876x_rcc.h"
#include "rtl876x_uart.h"
#include "rtl876x_pinmux.h"
#include "rtl8763_syson_reg.h"
#include "trace.h"
#include "os_msg.h"
#include "shm2_api.h"
#include "audio_probe.h"
#include "section.h"
#include "ipc.h"
#include "app_cfg.h"
#include "app_ipc.h"

#define IPC_QUEUE_LEN        0x20

#define DSP2_LOG_UART                   UART3
#define DSP2_LOG_UART_APBPeriph         APBPeriph_UART3
#define DSP2_LOG_UART_APBPeriph_CLOCK   APBPeriph_UART3_CLOCK
#define DSP2_LOG_UART_TX                UART3_TX
#define DSP2_LOG_UART_RX                UART3_RX

static void *ipc_queue_handle = NULL;

typedef struct
{
    uint16_t    evt;
    uint16_t    len;
    void        *p_data;
} T_IPC_EVT_INFO;


RAM_TEXT_SECTION
void ipc_evt_trigger(uint16_t evt, void *p_data, uint16_t len)
{
    T_IPC_EVT_INFO evt_info = {.evt = evt, .p_data = p_data, .len = len};
    int8_t ret = 0;

    if (ipc_queue_handle == NULL)
    {
        ret = -1;
        goto fail;
    }

    if (os_msg_send(ipc_queue_handle, &evt_info, 0) == false)
    {
        ret = -4;
        goto fail;
    }

    return ;

fail:

    DIPC_PRINT_ERROR2("ipc_evt_trigger: ret %d, evt 0x%02x, ", ret,  evt_info.evt);

    return ;
}

uint32_t ipc_dsp2_data_recv(uint8_t *p_type, uint8_t *buffer, uint16_t length,
                            uint32_t recv_queue_id)
{
    uint32_t real_len = 0;
    real_len = shm2_data_recv(buffer, length, p_type, &recv_queue_id);
    return real_len;
}

bool ipc_dsp2_data_send(uint8_t type, uint8_t *buffer, uint16_t length, bool flush, uint8_t seq,
                        uint32_t send_queue_id)
{
    return shm2_data_send(buffer, length, flush, type, seq, send_queue_id);
}

bool ipc_dsp2_cmd_send(uint8_t *buffer, uint16_t length, bool flush)
{
    return shm2_cmd_send(buffer, length, flush);
}

uint32_t ipc_dsp2_cmd_recv(uint8_t *buffer, uint16_t length)
{
    uint32_t real_len = 0;
    real_len = shm2_cmd_recv(buffer, length);
    return real_len;
}

uint32_t ipc_dsp2_data_length_peek(uint32_t queue_id)
{
    uint32_t real_len = 0;
    real_len = shm2_d2h_data_length_peek(queue_id);
    return real_len;
}

static void ipc_evt_handle(uint16_t evt, void *p_data, uint16_t len)
{
    switch (evt)
    {
    case IPC_EVT_DSP2_READY:
        {
        }
        break;

    case IPC_EVT_D2H_CMD:
        {
        }
        break;

    case IPC_EVT_DATA_IND:
        {
        }
        break;

    default:
        break;
    }

    app_ipc_publish(DSP2_IPC_TOPIC, evt, p_data);

}

static void mailbox_cb(uint32_t evt, uint32_t param)
{
    switch (evt)
    {
    case MAILBOX_EVT_SHM2_READY:
        {
            ipc_evt_trigger(IPC_EVT_DSP2_READY, NULL, 0);
        }
        break;

    default:
        break;
    }
}

static void d2h_evt_cb(void)
{
    ipc_evt_trigger(IPC_EVT_D2H_CMD, NULL, 0);
}

static void data_ind_cb(void)
{
    ipc_evt_trigger(IPC_EVT_DATA_IND, NULL, 0);
}

static void data_ack_cb(void)
{
    ipc_evt_trigger(IPC_EVT_DATA_ACK, NULL, 0);
}

static void enable_uart3_fucntion(uint8_t enable)
{
    uint32_t temp;
    //BIT4 enable UART3 PERI function
    temp = HAL_READ32(PERIPH_REG_BASE, 0x218);
    if (enable)
    {
        temp |= BIT4;
    }
    else
    {
        temp &= ~BIT4;
    }
    HAL_WRITE32(PERIPH_REG_BASE, 0x218, temp); //Enable PERI UART3
}

static void dsp_uart_driver_init(void)
{
    RCC_PeriphClockCmd(DSP2_LOG_UART_APBPeriph, DSP2_LOG_UART_APBPeriph_CLOCK, ENABLE);
    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);

    UART_InitTypeDef uart_init_struct;
    UART_StructInit(&uart_init_struct);
    uart_init_struct.dmaEn = UART_DMA_DISABLE;
    uart_init_struct.TxDmaEn = DISABLE;
    uart_init_struct.TxWaterlevel = 15;
    //Set UART baudrate: 2000000
    uart_init_struct.div = 2;
    uart_init_struct.ovsr = 5;
    uart_init_struct.ovsr_adj = 0;

    UART_Init(DSP2_LOG_UART, &uart_init_struct);
    enable_uart3_fucntion(1);
    DSP2_LOG_UART->MISCR |= ((16 - 8) << 3) | (1U << 1);
}

static void dsp_uart_init(void)
{
    if (app_cfg2_const.dsp_uart_enable != DSP_NO_UART)
    {
        if (app_cfg2_const.dsp_uart_enable == DSP_UART_TRX)
        {
            Pinmux_Deinit(app_cfg2_const.dsp_uart_tx_pinmux);//TX
            Pad_Config(app_cfg2_const.dsp_uart_tx_pinmux, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE,
                       PAD_OUT_ENABLE, PAD_OUT_HIGH);
            Pinmux_Config(app_cfg2_const.dsp_uart_tx_pinmux, DSP2_LOG_UART_TX);
            Pinmux_Deinit(app_cfg2_const.dsp_uart_rx_pinmux);//RX
            Pad_Config(app_cfg2_const.dsp_uart_rx_pinmux, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE,
                       PAD_OUT_DISABLE, PAD_OUT_LOW);
            Pinmux_Config(app_cfg2_const.dsp_uart_rx_pinmux, DSP2_LOG_UART_RX);
        }
        else if (app_cfg2_const.dsp_uart_enable == DSP_UART_TX)
        {
            Pinmux_Deinit(app_cfg2_const.dsp_uart_tx_pinmux);//TX
            Pad_Config(app_cfg2_const.dsp_uart_tx_pinmux, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE,
                       PAD_OUT_ENABLE, PAD_OUT_HIGH);
            Pinmux_Config(app_cfg2_const.dsp_uart_tx_pinmux, DSP2_LOG_UART_TX);
        }
        else if (app_cfg2_const.dsp_uart_enable == DSP_UART_RX)
        {
            Pinmux_Deinit(app_cfg2_const.dsp_uart_rx_pinmux);//RX
            Pad_Config(app_cfg2_const.dsp_uart_rx_pinmux, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE,
                       PAD_OUT_DISABLE, PAD_OUT_LOW);
            Pinmux_Config(app_cfg2_const.dsp_uart_rx_pinmux, DSP2_LOG_UART_RX);
        }
        dsp_uart_driver_init();
    }
}

static void jtag_init(void)
{
    if (app_cfg2_const.dsp2_jtag_enable)
    {
        Pinmux_Config(app_cfg2_const.dsp2_jtrst_pinmux, DSP2_JTRST);
        Pinmux_Config(app_cfg2_const.dsp2_jtck_pinmux, DSP2_JTCK);
        Pinmux_Config(app_cfg2_const.dsp2_jtdi_pinmux, DSP2_JTDI);
        Pinmux_Config(app_cfg2_const.dsp2_jtdo_pinmux, DSP2_JTDO);
        Pinmux_Config(app_cfg2_const.dsp2_jtms_pinmux, DSP2_JTMS);


        Pad_Config(app_cfg2_const.dsp2_jtrst_pinmux, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE,
                   PAD_OUT_DISABLE, PAD_OUT_LOW);
        Pad_Config(app_cfg2_const.dsp2_jtck_pinmux, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE,
                   PAD_OUT_DISABLE, PAD_OUT_LOW);
        Pad_Config(app_cfg2_const.dsp2_jtdi_pinmux, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE,
                   PAD_OUT_DISABLE, PAD_OUT_LOW);
        Pad_Config(app_cfg2_const.dsp2_jtdo_pinmux, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE,
                   PAD_OUT_ENABLE, PAD_OUT_LOW);
        Pad_Config(app_cfg2_const.dsp2_jtms_pinmux, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE,
                   PAD_OUT_DISABLE, PAD_OUT_LOW);
    }
}

static void ipc_sw_init(void)
{
    shm2_io_ctl(SHM2_IOCTL_CALLBACK_MAILBOX, (uint32_t)mailbox_cb);
    shm2_io_ctl(SHM2_IOCTL_CALLBACK_CMD_EVENT, (uint32_t)d2h_evt_cb);
    shm2_io_ctl(SHM2_IOCTL_CALLBACK_RX_REQ, (uint32_t)data_ind_cb);
    shm2_io_ctl(SHM2_IOCTL_CALLBACK_TX_ACK, (uint32_t)data_ack_cb);
}

static void ipc_hw_init(void)
{
    jtag_init();
    dsp_uart_init();
}

void ipc_task(void *pvParameters)
{
    os_msg_queue_create(&ipc_queue_handle, "ipc evtQ", IPC_QUEUE_LEN,
                        sizeof(T_IPC_EVT_INFO));
    T_IPC_EVT_INFO evt_info;

    ipc_sw_init();
    ipc_hw_init();

    while (true)
    {
        if (os_msg_recv(ipc_queue_handle, &evt_info, 0xFFFFFFFF) == true)
        {
            ipc_evt_handle(evt_info.evt, evt_info.p_data, evt_info.len);
            APP_PRINT_INFO1("ipc_task: evt 0x%02x", evt_info.evt);
        }
    }
}
