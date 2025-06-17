/**
*********************************************************************************************************
*               Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
**********************************************************************************************************
* @file     hal_i2c.c
* @brief    This file provides all the I2C hal functions.
* @details
* @author
* @date
* @version  v0.1
*********************************************************************************************************
*/

#include <string.h>
#include <stdlib.h>
#include "rtl876x_i2c.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_rcc.h"
#include "rtl876x_nvic.h"
#include "rtl876x_gdma.h"
#include "io_dlps.h"

//#include "vector_table.h"
#include "os_sync.h"
#include "os_mem.h"

#include "hal_i2c.h"
#include "hal_dma_internal.h"
#include "trace.h"
#include "hal_gpio.h"
#include "platform_utils.h"

#if defined(IC_TYPE_RTL87x3EU)
#include "dma_channel.h"
#endif

#define I2C_WAIT_FOREVER                    (0xFFFFFFFFUL)
#define IS_INVALID_I2C_ID(id)               ((id) >= HAL_I2C_ID_MAX)
#define IS_INVALID_DMA_CH_NUM(ch_num)       ((ch_num) >= GDMA_TOTAL_CH_COUNT)
#define I2C_STATUS_TO_HAL_RET(status)       ((T_I2C_STATUS)(status))

extern void I2C_DLPSEnter(void *peri_reg, void *p_store_buf);
extern void I2C_DLPSExit(void *peri_reg, void *p_store_buf);

#if (CHIP_I2C_NUM == 1)
static I2C_TypeDef *const g_i2c_regbase[HAL_I2C_ID_MAX] = {I2C0};
//static const IRQn_Type g_i2c_port_to_irq_num[HAL_I2C_ID_MAX] = {I2C0_IRQn};
//static VECTORn_Type g_i2c_port_to_vector[HAL_I2C_ID_MAX] = {I2C0_VECTORn};
static const uint32_t apb_peri_i2c[HAL_I2C_ID_MAX] =
{APBPeriph_I2C0};
static const uint32_t apb_peri_clk_i2c[HAL_I2C_ID_MAX] = {APBPeriph_I2C0_CLOCK};
static const uint8_t g_i2c_port_to_sda_pin[HAL_I2C_ID_MAX] = {I2C0_DAT};
static const uint8_t g_i2c_port_to_scl_pin[HAL_I2C_ID_MAX] = {I2C0_CLK};
static const uint8_t g_i2c_gdma_handshake_tx[HAL_I2C_ID_MAX] = {GDMA_Handshake_I2C0_TX};
//static const uint8_t g_i2c_gdma_handshake_rx[HAL_I2C_ID_MAX] = {GDMA_Handshake_I2C0_RX};
#elif (CHIP_I2C_NUM == 3)
static I2C_TypeDef *const g_i2c_regbase[HAL_I2C_ID_MAX] = {I2C0, I2C1, I2C2};
//static const IRQn_Type g_i2c_port_to_irq_num[HAL_I2C_ID_MAX] = {I2C0_IRQn, I2C1_IRQn, I2C2_IRQn};
//static VECTORn_Type g_i2c_port_to_vector[HAL_I2C_ID_MAX] = {I2C0_VECTORn, I2C1_VECTORn, I2C2_VECTORn};
static const uint32_t apb_peri_i2c[HAL_I2C_ID_MAX] =
{APBPeriph_I2C0, APBPeriph_I2C1, APBPeriph_I2C2};
static const uint32_t apb_peri_clk_i2c[HAL_I2C_ID_MAX] = {APBPeriph_I2C0_CLOCK, APBPeriph_I2C1_CLOCK, APBPeriph_I2C2_CLOCK};
static const uint8_t g_i2c_port_to_sda_pin[HAL_I2C_ID_MAX] = {I2C0_DAT, I2C1_DAT, I2C2_DAT};
static const uint8_t g_i2c_port_to_scl_pin[HAL_I2C_ID_MAX] = {I2C0_CLK, I2C1_CLK, I2C2_CLK};
static const uint8_t g_i2c_gdma_handshake_tx[HAL_I2C_ID_MAX] = {GDMA_Handshake_I2C0_TX, GDMA_Handshake_I2C1_TX, GDMA_Handshake_I2C2_TX};
//static const uint8_t g_i2c_gdma_handshake_rx[HAL_I2C_ID_MAX] = {GDMA_Handshake_I2C0_RX, GDMA_Handshake_I2C1_RX, GDMA_Handshake_I2C2_RX};
#endif

typedef enum
{
    I2C_STATE_CLOSED,
    I2C_STATE_OPENED
} T_HAL_I2C_STATE;

typedef struct _t_hal_i2c
{
    union
    {
        struct
        {
            uint32_t scl_pin           : 8;
            uint32_t sda_pin           : 8;
            uint32_t status            : 4;
            uint32_t lock_type         : 2;
            uint32_t rx_dma_en         : 1;
            uint32_t rx_dma_ch_num     : 4;
            uint32_t tx_dma_en         : 1;
            uint32_t tx_dma_ch_num     : 4;
        };
        uint32_t i2c_config_u32;
    };

    void *sem;
    uint16_t master_write_stop_cmd;
    P_HAL_I2C_CALLBACK i2c_callback;
    P_HAL_I2C_CALLBACK i2c_tx_dma_callback;
    P_HAL_I2C_CALLBACK i2c_rx_dma_callback;
    void *dma_lli;
    uint32_t lock_flag;
} T_HAL_I2C;

//static uint32_t master_read_cmd = BIT8;
//static uint32_t master_read_stop_cmd = (BIT8 | BIT9);
//static uint32_t master_write_stop_cmd = BIT9;
static T_HAL_I2C g_hal_i2c[HAL_I2C_ID_MAX];

static uint32_t i2c_require(T_I2C_ID i2c_id)
{
    uint32_t ret = 0;
    switch (g_hal_i2c[i2c_id].lock_type)
    {
    case I2C_LOCK_DISABLE:
        break;
    case I2C_LOCK_TYPE_MUTEX:
        if (g_hal_i2c[i2c_id].sem)
        {
            ret = (os_mutex_take(g_hal_i2c[i2c_id].sem, I2C_WAIT_FOREVER) ? true : false);
        }
        break;
    case I2C_LOCK_TYPE_SEMAPHORE:
        {
            ret = (os_sem_take(g_hal_i2c[i2c_id].sem, I2C_WAIT_FOREVER) ? true : false);
        }
        break;
    default:
        break;
    }
    return ret;
}

static void i2c_release(T_I2C_ID i2c_id, uint32_t flag)
{
    if (!flag)
    {
        return;
    }

    switch (g_hal_i2c[i2c_id].lock_type)
    {
    case I2C_LOCK_DISABLE:
        break;
    case I2C_LOCK_TYPE_MUTEX:
        os_mutex_give(g_hal_i2c[i2c_id].sem);
        break;
    case I2C_LOCK_TYPE_SEMAPHORE:
        os_sem_give(g_hal_i2c[i2c_id].sem);
        break;
    default:
        break;
    }
}

static void hal_i2c_create_sem(T_I2C_ID i2c_id)
{
    switch (g_hal_i2c[i2c_id].lock_type)
    {
    case I2C_LOCK_DISABLE:
        break;
    case I2C_LOCK_TYPE_MUTEX:
        if (g_hal_i2c[i2c_id].sem == 0)
        {
            os_mutex_create(&g_hal_i2c[i2c_id].sem);
        }
        break;
    case I2C_LOCK_TYPE_SEMAPHORE:
        if (g_hal_i2c[i2c_id].sem == 0)
        {
            os_sem_create(&g_hal_i2c[i2c_id].sem, "i2c_sem", 1, 1);
        }
        break;
    default:
        break;
    }
}

static void hal_i2c_gen_stop_signal(T_I2C_ID i2c_id)
{
    uint8_t scl_pin = g_hal_i2c[i2c_id].scl_pin;
    uint8_t sda_pin = g_hal_i2c[i2c_id].sda_pin;

    /*gpio init*/
    hal_gpio_init_pin(scl_pin, GPIO_TYPE_CORE, GPIO_DIR_INPUT, GPIO_PULL_NONE);
    hal_gpio_init_pin(sda_pin, GPIO_TYPE_CORE, GPIO_DIR_INPUT, GPIO_PULL_NONE);

    IO_PRINT_ERROR2("i2c get current status scl_pin level =%d, sda_pin level =%d ",
                    hal_gpio_get_input_level(scl_pin),
                    hal_gpio_get_input_level(sda_pin));

    hal_gpio_init_pin(scl_pin, GPIO_TYPE_CORE, GPIO_DIR_OUTPUT, GPIO_PULL_NONE);
    hal_gpio_init_pin(sda_pin, GPIO_TYPE_CORE, GPIO_DIR_OUTPUT, GPIO_PULL_NONE);

    /*generate i2c stop signal*/
    hal_gpio_set_level(scl_pin, GPIO_LEVEL_HIGH);
    hal_gpio_set_level(sda_pin, GPIO_LEVEL_LOW);
    platform_delay_us(20);
    hal_gpio_set_level(sda_pin, GPIO_LEVEL_HIGH);

    Pinmux_Config(scl_pin, g_i2c_port_to_scl_pin[i2c_id]);
    Pinmux_Config(sda_pin, g_i2c_port_to_sda_pin[i2c_id]);
    Pad_Config(sda_pin,
               PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(scl_pin,
               PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
}

static void hal_i2c_bus_clear_check(T_I2C_ID i2c_id, I2C_Status status, const char *func)
{
    IO_PRINT_ERROR4("hal i2c fail, func %s, id %x, slave_addr 0x%x, status %x", TRACE_STRING(func),
                    g_i2c_regbase[i2c_id]->IC_TAR, i2c_id, status);
    if (status == I2C_TIMEOUT)
    {
        hal_i2c_gen_stop_signal(i2c_id);
    }
}

static void hal_i2c_set_slave_address(T_I2C_ID i2c_id, uint16_t slave_address)
{
    I2C_TypeDef *I2Cx = g_i2c_regbase[i2c_id];

    if (slave_address != I2Cx->IC_TAR)
    {
        I2C_Cmd(I2Cx, DISABLE);
        I2C_SetSlaveAddress(I2Cx, slave_address);
        I2C_Cmd(I2Cx, ENABLE);
    }
}

static void hal_i2c_general_call(T_I2C_ID i2c_id, bool enable)
{
    I2C_TypeDef *I2Cx = g_i2c_regbase[i2c_id];

    if (enable)
    {
        I2Cx->IC_TAR |= BIT11;
        I2Cx->IC_TAR &= ~BIT10;
    }
    else
    {
        I2Cx->IC_TAR &= ~BIT11;
    }
}

static void i2c_tx_gdma_handler_imp(T_HAL_DMA_EVENT isr_event, uint32_t param)
{
    T_I2C_ID i2c_id = (T_I2C_ID)param;
    i2c_release(i2c_id, g_hal_i2c[i2c_id].lock_flag);
    T_HAL_I2C_EVENT i2c_event = {.event = 0};
    if (g_hal_i2c[i2c_id].i2c_tx_dma_callback)
    {
        i2c_event.I2C_EVENT_DMA_TX_DONE = 1;
        g_hal_i2c[i2c_id].i2c_tx_dma_callback(i2c_id, i2c_event);
    }
}

T_I2C_STATUS hal_i2c_init_master_mode(T_I2C_ID i2c_id, const T_I2C_MASTER_CONFIG *i2c_config)
{
    if (IS_INVALID_I2C_ID(i2c_id))
    {
        return I2C_STATUS_ERROR_INVALID_PARAMETER;
    }

    //TODO, add resource check later. For app sensor repeatedly init i2c
    if (g_hal_i2c[i2c_id].status == I2C_STATE_OPENED)
    {
        if (g_hal_i2c[i2c_id].lock_type == 0)
        {
            g_hal_i2c[i2c_id].lock_type = i2c_config->lock_type;
        }
    }
    else
    {
        /* first initialzation */
        memset(g_hal_i2c, 0, sizeof(T_HAL_I2C));
        g_hal_i2c[i2c_id].lock_type = i2c_config->lock_type;
    }

    g_hal_i2c[i2c_id].status = I2C_STATE_OPENED;

    I2C_InitTypeDef i2c_param;

    RCC_PeriphClockCmd(apb_peri_i2c[i2c_id], apb_peri_clk_i2c[i2c_id], ENABLE);

    I2C_StructInit(&i2c_param);

    i2c_param.I2C_ClockSpeed = i2c_config->i2c_speed;
    i2c_param.I2C_DeviveMode = I2C_DeviveMode_Master;
    i2c_param.I2C_AddressMode = (I2CAddressMode_TypeDef)i2c_config->addr_mode;
    i2c_param.I2C_SlaveAddress = i2c_config->target_slave_addr;
    i2c_param.I2C_Ack = I2C_Ack_Enable;
    i2c_param.I2C_RxDmaEn           = ENABLE;
    i2c_param.I2C_RxWaterlevel      = 1;
    i2c_param.I2C_TxDmaEn           = ENABLE;
    i2c_param.I2C_TxWaterlevel      = 6;
    I2C_Init(g_i2c_regbase[i2c_id], &i2c_param);
    I2C_Cmd(g_i2c_regbase[i2c_id], ENABLE);

    hal_i2c_create_sem(i2c_id);

    return I2C_STATUS_OK;
}

T_I2C_STATUS hal_i2c_init_slave_mode(T_I2C_ID i2c_id, const T_I2C_SLAVE_CONFIG *i2c_config)
{
    if (IS_INVALID_I2C_ID(i2c_id))
    {
        return I2C_STATUS_ERROR_INVALID_PARAMETER;
    }

#if 0  //TODO, add resource check later
    if (g_hal_i2c[i2c_id].status == I2C_STATE_OPENED)
    {
        return I2C_STATUS_ERROR_BUSY;
    }
#endif

    /* first initialzation */
    memset(g_hal_i2c, 0, sizeof(T_HAL_I2C));
    g_hal_i2c[i2c_id].status = I2C_STATE_OPENED;

    I2C_InitTypeDef i2c_param;

    RCC_PeriphClockCmd(apb_peri_i2c[i2c_id], apb_peri_clk_i2c[i2c_id], ENABLE);

    I2C_StructInit(&i2c_param);

    i2c_param.I2C_DeviveMode = I2C_DeviveMode_Slave;
    i2c_param.I2C_AddressMode = (I2CAddressMode_TypeDef)i2c_config->addr_mode;
    i2c_param.I2C_SlaveAddress = i2c_config->slave_addr;
    i2c_param.I2C_Ack = I2C_Ack_Enable;

    I2C_Init(g_i2c_regbase[i2c_id], &i2c_param);
    I2C_Cmd(g_i2c_regbase[i2c_id], ENABLE);

    return I2C_STATUS_OK;
}

T_I2C_STATUS hal_i2c_init_pin(T_I2C_ID i2c_id, uint8_t scl_pin, uint8_t sda_pin)
{
    if (IS_INVALID_I2C_ID(i2c_id))
    {
        return I2C_STATUS_ERROR_INVALID_PARAMETER;
    }

    Pinmux_Config(scl_pin, g_i2c_port_to_scl_pin[i2c_id]);
    Pinmux_Config(sda_pin, g_i2c_port_to_sda_pin[i2c_id]);
    Pad_Config(sda_pin,
               PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);
    Pad_Config(scl_pin,
               PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW);

    g_hal_i2c[i2c_id].scl_pin = scl_pin;
    g_hal_i2c[i2c_id].sda_pin = sda_pin;

    return I2C_STATUS_OK;
}

T_I2C_STATUS hal_i2c_enable_tx_dma_mode(T_I2C_ID i2c_id, uint8_t dma_channel_num,
                                        P_HAL_I2C_CALLBACK cb)
{
    if (IS_INVALID_I2C_ID(i2c_id) || (IS_INVALID_DMA_CH_NUM(dma_channel_num)))
    {
        return I2C_STATUS_ERROR_INVALID_PARAMETER;
    }

    if (g_hal_i2c[i2c_id].dma_lli == 0)
    {
        g_hal_i2c[i2c_id].dma_lli = calloc(2, sizeof(GDMA_LLIDef));
    }

    g_hal_i2c[i2c_id].tx_dma_en = 1;
    g_hal_i2c[i2c_id].tx_dma_ch_num = dma_channel_num;
    g_hal_i2c[i2c_id].i2c_tx_dma_callback = cb;

    GDMA_LLIDef *i2c_gdma_lli = (GDMA_LLIDef *)g_hal_i2c[i2c_id].dma_lli;

    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    /*--------------DMA init-----------------------------*/
    GDMA_InitTypeDef GDMA_InitStruct;
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum          = dma_channel_num;
    GDMA_InitStruct.GDMA_DIR                 = GDMA_DIR_MemoryToPeripheral;
    GDMA_InitStruct.GDMA_BufferSize          = 0;
    GDMA_InitStruct.GDMA_SourceInc           = DMA_SourceInc_Inc;
    GDMA_InitStruct.GDMA_DestinationInc      = DMA_DestinationInc_Fix;
    GDMA_InitStruct.GDMA_SourceDataSize      = GDMA_DataSize_HalfWord;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_HalfWord;
    GDMA_InitStruct.GDMA_SourceMsize         = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_DestinationMsize    = GDMA_Msize_4;
    GDMA_InitStruct.GDMA_SourceAddr          = (uint32_t)0;
    GDMA_InitStruct.GDMA_DestinationAddr     = (uint32_t)(&(g_i2c_regbase[i2c_id]->IC_DATA_CMD));
    GDMA_InitStruct.GDMA_DestHandshake       = g_i2c_gdma_handshake_tx[i2c_id];
    GDMA_InitStruct.GDMA_Multi_Block_Mode    = LLI_TRANSFER;
    GDMA_InitStruct.GDMA_Multi_Block_En      = 0;
    GDMA_InitStruct.GDMA_Multi_Block_Struct  = (uint32_t)i2c_gdma_lli;
    GDMA_Init(DMA_CH_BASE(dma_channel_num), &GDMA_InitStruct);

    i2c_gdma_lli[0].SAR = 0;
    i2c_gdma_lli[0].DAR = (uint32_t)GDMA_InitStruct.GDMA_DestinationAddr;
    i2c_gdma_lli[0].LLP = (uint32_t)&i2c_gdma_lli[1];
    i2c_gdma_lli[0].CTL_LOW = BIT(0)
                              | (GDMA_DataSize_HalfWord << 1)
                              | (GDMA_DataSize_HalfWord << 4)
                              | (GDMA_InitStruct.GDMA_DestinationInc << 7)
                              | (GDMA_InitStruct.GDMA_SourceInc << 9)
                              | (GDMA_InitStruct.GDMA_DestinationMsize << 11)
                              | (GDMA_InitStruct.GDMA_SourceMsize << 14)
                              | (GDMA_InitStruct.GDMA_DIR << 20)
                              | (GDMA_InitStruct.GDMA_Multi_Block_Mode);
    i2c_gdma_lli[0].CTL_HIGH = 0;

    i2c_gdma_lli[1].SAR = (uint32_t)g_hal_i2c[i2c_id].master_write_stop_cmd;
    i2c_gdma_lli[1].DAR = (uint32_t)GDMA_InitStruct.GDMA_DestinationAddr;
    i2c_gdma_lli[1].LLP = 0;//(uint32_t)&i2c_gdma_lli[2];
    i2c_gdma_lli[1].CTL_LOW = BIT(0)
                              | (GDMA_DataSize_HalfWord << 1)
                              | (GDMA_DataSize_HalfWord << 4)
                              | (GDMA_InitStruct.GDMA_DestinationInc << 7)
                              | (GDMA_InitStruct.GDMA_SourceInc << 9)
                              | (GDMA_InitStruct.GDMA_DestinationMsize << 11)
                              | (GDMA_InitStruct.GDMA_SourceMsize << 14)
                              | (GDMA_InitStruct.GDMA_DIR << 20);
    i2c_gdma_lli[1].CTL_HIGH = 1;

    hal_dma_update_isr_cb((T_DMA_CH_NUM)dma_channel_num, i2c_tx_gdma_handler_imp, (uint32_t)(i2c_id));

    /*-----------------DMA IRQ init-------------------*/
    NVIC_InitTypeDef nvic_init_struct;
    nvic_init_struct.NVIC_IRQChannel         = DMA_CH_IRQ(dma_channel_num);
    nvic_init_struct.NVIC_IRQChannelCmd      = (FunctionalState)ENABLE;
    nvic_init_struct.NVIC_IRQChannelPriority = 5;
    NVIC_Init(&nvic_init_struct);

    GDMA_INTConfig(dma_channel_num, GDMA_INT_Transfer, ENABLE);

    return I2C_STATUS_OK;
}

T_I2C_STATUS hal_i2c_enable_rx_dma_mode(T_I2C_ID i2c_id, uint8_t dma_channel_num,
                                        P_HAL_I2C_CALLBACK cb)
{
    if (IS_INVALID_I2C_ID(i2c_id) || (IS_INVALID_DMA_CH_NUM(dma_channel_num)))
    {
        return I2C_STATUS_ERROR_INVALID_PARAMETER;
    }

    RCC_PeriphClockCmd(APBPeriph_GDMA, APBPeriph_GDMA_CLOCK, ENABLE);
    GDMA_InitTypeDef GDMA_InitStruct;
    /*--------------GDMA init-----------------------------*/
    GDMA_StructInit(&GDMA_InitStruct);
    GDMA_InitStruct.GDMA_ChannelNum      = dma_channel_num;
    GDMA_InitStruct.GDMA_DIR             = GDMA_DIR_PeripheralToMemory;//GDMA_DIR_PeripheralToMemory
    GDMA_InitStruct.GDMA_BufferSize      = 0;
    GDMA_InitStruct.GDMA_SourceInc       = DMA_SourceInc_Fix;
    GDMA_InitStruct.GDMA_DestinationInc  = DMA_DestinationInc_Inc;
    GDMA_InitStruct.GDMA_SourceDataSize  = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_DestinationDataSize = GDMA_DataSize_Byte;
    GDMA_InitStruct.GDMA_SourceMsize      = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_DestinationMsize = GDMA_Msize_1;
    GDMA_InitStruct.GDMA_SourceAddr      = (uint32_t) & (g_i2c_regbase[i2c_id]->IC_DATA_CMD);
    GDMA_InitStruct.GDMA_DestinationAddr = (uint32_t)0;
    GDMA_InitStruct.GDMA_SourceHandshake = GDMA_Handshake_I2C1_RX;
    GDMA_Init(DMA_CH_BASE(dma_channel_num), &GDMA_InitStruct);

    hal_dma_update_isr_cb((T_DMA_CH_NUM)dma_channel_num, i2c_tx_gdma_handler_imp, (uint32_t)(i2c_id));

    /*-----------------GDMA IRQ init-------------------*/
    NVIC_InitTypeDef nvic_init_struct;
    nvic_init_struct.NVIC_IRQChannel         = DMA_CH_IRQ(dma_channel_num);
    nvic_init_struct.NVIC_IRQChannelCmd      = (FunctionalState)ENABLE;
    nvic_init_struct.NVIC_IRQChannelPriority = 3;
    NVIC_Init(&nvic_init_struct);

    GDMA_INTConfig(dma_channel_num, GDMA_INT_Transfer, ENABLE);

    return I2C_STATUS_OK;
}

T_I2C_STATUS hal_i2c_master_set_speed(T_I2C_ID i2c_id, uint32_t clock_speed)
{
    if (IS_INVALID_I2C_ID(i2c_id))
    {
        return I2C_STATUS_ERROR_INVALID_PARAMETER;
    }
#if 0
    if (g_hal_i2c[i2c_id].status == I2C_STATE_CLOSED)
    {
        return I2C_STATUS_ERROR_NOT_INIT;
    }
#endif
    uint32_t flag = i2c_require(i2c_id);

    I2C_Cmd(g_i2c_regbase[i2c_id], DISABLE);
    I2C_SetClockSpeed(g_i2c_regbase[i2c_id], clock_speed);
    I2C_Cmd(g_i2c_regbase[i2c_id], ENABLE);

    i2c_release(i2c_id, flag);

    return I2C_STATUS_OK;
}

T_I2C_STATUS hal_i2c_master_write(T_I2C_ID i2c_id, uint16_t slave_addr, uint8_t *write_buf,
                                  uint32_t write_len)
{
    if (IS_INVALID_I2C_ID(i2c_id))
    {
        return I2C_STATUS_ERROR_INVALID_PARAMETER;
    }
#if 0
    if (g_hal_i2c[i2c_id].status == I2C_STATE_CLOSED)
    {
        return I2C_STATUS_ERROR_NOT_INIT;
    }
#endif
    uint32_t flag = i2c_require(i2c_id);
    I2C_Status status;

    hal_i2c_set_slave_address(i2c_id, slave_addr);

    status = I2C_MasterWrite(g_i2c_regbase[i2c_id], write_buf, write_len);

    if (status != I2C_Success)
    {
        hal_i2c_bus_clear_check(i2c_id, status, __func__);
    }

    i2c_release(i2c_id, flag);

    return I2C_STATUS_TO_HAL_RET(status);
}

T_I2C_STATUS hal_i2c_master_write_device(T_I2C_ID i2c_id, uint16_t slave_addr, uint8_t *write_buf0,
                                         uint32_t write_len0, uint8_t *write_buf, uint32_t write_len)
{
    if (IS_INVALID_I2C_ID(i2c_id))
    {
        return I2C_STATUS_ERROR_INVALID_PARAMETER;
    }

    uint32_t flag = i2c_require(i2c_id);
    I2C_Status status;

    hal_i2c_set_slave_address(i2c_id, slave_addr);

    status = I2C_MasterWriteDevice(g_i2c_regbase[i2c_id], write_buf0, write_len0, write_buf, write_len);

    if (status != I2C_Success)
    {
        hal_i2c_bus_clear_check(i2c_id, status, __func__);
    }

    i2c_release(i2c_id, flag);

    return I2C_STATUS_TO_HAL_RET(status);
}

T_I2C_STATUS hal_i2c_master_write_reg(T_I2C_ID i2c_id, uint16_t slave_addr, uint8_t reg_addr,
                                      uint8_t reg_data)
{
    return hal_i2c_master_write_device(i2c_id, slave_addr, &reg_addr, 1, &reg_data, 1);
}

T_I2C_STATUS hal_i2c_master_write_8b_reg_16b(T_I2C_ID i2c_id, uint16_t slave_addr, uint8_t reg_addr,
                                             uint16_t reg_data)
{
    return hal_i2c_master_write_device(i2c_id, slave_addr, &reg_addr, 1, (uint8_t *)&reg_data, 2);
}

T_I2C_STATUS hal_i2c_master_write_regs(T_I2C_ID i2c_id, uint16_t slave_addr, uint8_t reg_addr,
                                       uint8_t *write_buf, uint32_t write_len)
{
    return hal_i2c_master_write_device(i2c_id, slave_addr, &reg_addr, 1, write_buf, write_len);
}

T_I2C_STATUS hal_i2c_master_write_16b_regs(T_I2C_ID i2c_id, uint16_t slave_addr, uint16_t reg_addr,
                                           uint8_t *write_buf, uint32_t write_len)
{
    return hal_i2c_master_write_device(i2c_id, slave_addr, (uint8_t *)&reg_addr, 2, write_buf,
                                       write_len);
}

T_I2C_STATUS hal_i2c_master_read(T_I2C_ID i2c_id, uint16_t slave_addr, uint8_t *read_buf,
                                 uint32_t read_len)
{
    if (IS_INVALID_I2C_ID(i2c_id))
    {
        return I2C_STATUS_ERROR_INVALID_PARAMETER;
    }

#if 0
    if (g_hal_i2c[i2c_id].status == I2C_STATE_CLOSED)
    {
        return I2C_STATUS_ERROR_NOT_INIT;
    }
#endif

    uint32_t flag = i2c_require(i2c_id);
    I2C_Status status;

    hal_i2c_set_slave_address(i2c_id, slave_addr);

    status = I2C_MasterRead(g_i2c_regbase[i2c_id], read_buf, read_len);

    if (status != I2C_Success)
    {
        hal_i2c_bus_clear_check(i2c_id, status, __func__);
    }

    i2c_release(i2c_id, flag);

    return I2C_STATUS_TO_HAL_RET(status);
}

T_I2C_STATUS hal_i2c_master_write_to_read(T_I2C_ID i2c_id, uint16_t slave_addr,
                                          uint8_t *write_buf, uint32_t write_len,
                                          uint8_t *read_buf, uint32_t read_len)
{
    if (IS_INVALID_I2C_ID(i2c_id))
    {
        return I2C_STATUS_ERROR_INVALID_PARAMETER;
    }

#if 0
    if (g_hal_i2c[i2c_id].status == I2C_STATE_CLOSED)
    {
        return I2C_STATUS_ERROR_NOT_INIT;
    }
#endif

    uint32_t flag = i2c_require(i2c_id);
    I2C_Status status;

    hal_i2c_set_slave_address(i2c_id, slave_addr);

    status = I2C_RepeatRead(g_i2c_regbase[i2c_id], (uint8_t *)write_buf, write_len, read_buf, read_len);
    if (status != I2C_Success)
    {
        hal_i2c_bus_clear_check(i2c_id, status, __func__);
    }

    i2c_release(i2c_id, flag);

    return I2C_STATUS_TO_HAL_RET(status);
}

T_I2C_STATUS hal_i2c_master_general_call(T_I2C_ID i2c_id, uint8_t data)
{
    if (IS_INVALID_I2C_ID(i2c_id))
    {
        return I2C_STATUS_ERROR_INVALID_PARAMETER;
    }

#if 0
    if (g_hal_i2c[i2c_id].status == I2C_STATE_CLOSED)
    {
        return I2C_STATUS_ERROR_NOT_INIT;
    }
#endif

    uint32_t flag = i2c_require(i2c_id);
    I2C_Status status;

    hal_i2c_general_call(i2c_id, true);

    status = I2C_MasterWrite(g_i2c_regbase[i2c_id], (uint8_t *)&data, 1);

    if (status != I2C_Success)
    {
        hal_i2c_bus_clear_check(i2c_id, status, __func__);
    }

    hal_i2c_general_call(i2c_id, false);

    i2c_release(i2c_id, flag);

    return I2C_STATUS_TO_HAL_RET(status);
}

T_I2C_STATUS hal_i2c_master_write_by_dma(T_I2C_ID i2c_id, uint16_t slave_addr,
                                         uint16_t *write_buf, uint32_t write_len)
{
    if (IS_INVALID_I2C_ID(i2c_id))
    {
        return I2C_STATUS_ERROR_INVALID_PARAMETER;
    }

#if 0
    if (g_hal_i2c[i2c_id].status == I2C_STATE_CLOSED)
    {
        return I2C_STATUS_ERROR_NOT_INIT;
    }
#endif

    if ((g_hal_i2c[i2c_id].tx_dma_en == 0) || (g_hal_i2c[i2c_id].dma_lli == NULL))
    {
        return I2C_STATUS_ERROR_DMA_NOT_INIT;
    }

    g_hal_i2c[i2c_id].lock_flag = i2c_require(i2c_id);

    GDMA_LLIDef *i2c_gdma_lli = (GDMA_LLIDef *)g_hal_i2c[i2c_id].dma_lli;
    GDMA_ChannelTypeDef *GDMAx = DMA_CH_BASE(g_hal_i2c[i2c_id].tx_dma_ch_num);
    g_hal_i2c[i2c_id].master_write_stop_cmd = BIT9 | write_buf[write_len - 1];
    i2c_gdma_lli[0].LLP = (uint32_t)&i2c_gdma_lli[1];
    i2c_gdma_lli[0].CTL_HIGH = write_len - 1;
    i2c_gdma_lli[0].SAR = (uint32_t)write_buf;
    i2c_gdma_lli[0].CTL_LOW |= LLP_SELECTED_BIT;
    i2c_gdma_lli[1].SAR = (uint32_t)&g_hal_i2c[i2c_id].master_write_stop_cmd;

    GDMA_SetLLPMode(GDMAx, 0);
    GDMA_SetLLPAddress(GDMAx, (uint32_t)&i2c_gdma_lli[0]);
    GDMA_SetLLPMode(GDMAx, LLI_TRANSFER);

    /*-----------------start to receive data-----------*/
    GDMA_Cmd(g_hal_i2c[i2c_id].tx_dma_ch_num, ENABLE);

    return I2C_STATUS_OK;
}

