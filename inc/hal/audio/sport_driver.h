/**
*********************************************************************************************************
*               Copyright(c) 2019, Realtek Semiconductor Corporation. All rights reserved.
*********************************************************************************************************
* @file      sport_driver.h
* @brief    This file provides all the SPORT firmware functions.
* @details
* @author
* @date      2019-08-26
* @version   v1.0
* *********************************************************************************************************
*/


#ifndef _SPORT_DRIVER_H_
#define _SPORT_DRIVER_H_

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum t_sport_id
{
    SPORT_ID0,
    SPORT_ID1,
    SPORT_ID2,
    SPORT_ID3
} T_SPORT_ID;

typedef enum t_sport_bus_sel
{
    SPORT_BUS_SEL_SYS = 0,
    SPORT_BUS_SEL_DSP = 1,
    SPORT_BUS_SEL_MAX = 2,
} T_SPORT_BUS_SEL;

typedef enum t_sport_clk_src
{
    SPORT_CLK_XTAL,
    SPORT_CLK_PLL,
    SPORT_CLK_SEL_MAX,
} T_SPORT_CLK_SRC;

typedef enum t_sport_codec_sel
{
    SPORT_CODEC_SEL_INT,
    SPORT_CODEC_SEL_EXT,
    SPORT_CODEC_SEL_MAX,
} T_SPORT_CODEC_SEL;

typedef enum t_sport_format_type
{
    SPORT_FORMAT_I2S,
    SPORT_FORMAT_LEFT_JUSTIFY,
    SPORT_FORMAT_PCM_MODE_A,
    SPORT_FORMAT_PCM_MODE_B,
    SPORT_FORMAT_SEL_MAX,
} T_SPORT_FORMAT_TYPE;

typedef enum t_sport_role
{
    SPORT_ROLE_MASTER,
    SPORT_ROLE_SLAVE,
    SPORT_ROLE_SEL_MAX,
} T_SPORT_ROLE;

typedef enum t_sport_loopback
{
    SPORT_LOOPBACK_DISABLE,
    SPORT_LOOPBACK_TX_TO_RX,
    SPORT_LOOPBACK_RX_TO_TX,
    SPORT_LOOPBACK_SEL_MAX,
} T_SPORT_LOOPBACK;

typedef enum t_sport_channel_mode
{
    SPORT_MODE_MONO,
    SPORT_MODE_STEREO,
    SPORT_MODE_SEL_MAX,
} T_SPORT_CHANNEL_MODE;

typedef enum t_sport_tdm_mode
{
    SPORT_TDM_DISABLE,
    SPORT_TDM_MODE_4,
    SPORT_TDM_MODE_6,
    SPORT_TDM_MODE_8,
} T_SPORT_TDM_MODE;

typedef enum t_sport_direct_out_ch
{
    SPORT_DIRECT_OUT_NONE   = 0x00,
    SPORT_DIRECT_OUT_EN_0   = 0x01, // BIT0
    SPORT_DIRECT_OUT_EN_1   = 0x02, // BIT1
    SPORT_DIRECT_OUT_EN_2   = 0x04, // BIT2
    SPORT_DIRECT_OUT_EN_3   = 0x08, // BIT3
    SPORT_DIRECT_OUT_EN_4   = 0x10, // BIT4
    SPORT_DIRECT_OUT_EN_5   = 0x20, // BIT5
    SPORT_DIRECT_OUT_EN_6   = 0x40, // BIT6
    SPORT_DIRECT_OUT_EN_7   = 0x80, // BIT7
} T_SPORT_DIRECT_OUT_EN;

typedef enum t_sport_direct_mode
{
    SPORT_DIRECT_MODE_DISABLE       = 0x00,
    SPORT_DIRECT_MODE_SRC_SPORT0    = 0x04,
    SPORT_DIRECT_MODE_SRC_SPORT1    = 0x05,
    SPORT_DIRECT_MODE_SRC_SPORT2    = 0x06,
    SPORT_DIRECT_MODE_SRC_SPORT3    = 0x07,
    SPORT_DIRECT_MODE_SEL_MAX,
} T_SPORT_DIRECT_MODE;

typedef enum t_sport_channel_len
{
    SPORT_CHANNEL_LEN_8_BIT,
    SPORT_CHANNEL_LEN_16_BIT,
    SPORT_CHANNEL_LEN_20_BIT,
    SPORT_CHANNEL_LEN_24_BIT,
    SPORT_CHANNEL_LEN_32_BIT,
    SPORT_CHANNEL_LEN_MAX,
} T_SPORT_CHANNEL_LEN;

typedef enum t_sport_data_len
{
    SPORT_DATA_LEN_8_BIT,
    SPORT_DATA_LEN_16_BIT,
    SPORT_DATA_LEN_20_BIT,
    SPORT_DATA_LEN_24_BIT,
    SPORT_DATA_LEN_32_BIT,
    SPORT_DATA_LEN_MAX,
} T_SPORT_DATA_LEN;

typedef enum t_sport_tx_channel_sel
{
    SPORT_TX_CHANNEL_0,                             // 0
    SPORT_TX_CHANNEL_1,                             // 1
    SPORT_TX_CHANNEL_2,                             // 2
    SPORT_TX_CHANNEL_3,                             // 3
    SPORT_TX_CHANNEL_4,                             // 4
    SPORT_TX_CHANNEL_5,                             // 5
    SPORT_TX_CHANNEL_6,                             // 6
    SPORT_TX_CHANNEL_7,                             // 7
    SPORT_TX_CHANNEL_SEL_MAX,                       // 8
} T_SPORT_TX_CHANNEL_SEL;

typedef enum t_sport_tx_sel
{
    SPORT_TX_FIFO_0_REG_0_L,                        // 0
    SPORT_TX_FIFO_0_REG_0_R,                        // 1
    SPORT_TX_FIFO_0_REG_1_L,                        // 2
    SPORT_TX_FIFO_0_REG_1_R,                        // 3
    SPORT_TX_FIFO_1_REG_0_L,                        // 4
    SPORT_TX_FIFO_1_REG_0_R,                        // 5
    SPORT_TX_FIFO_1_REG_1_L,                        // 6
    SPORT_TX_FIFO_1_REG_1_R,                        // 7
    SPORT_TX_DIRECT_REG,                            // 8

    SPORT_TX_DIRECT_IN_A0 = SPORT_TX_DIRECT_REG,    // 8
    SPORT_TX_DIRECT_IN_A1,                          // 9
    SPORT_TX_DIRECT_IN_A2,                          // 10
    SPORT_TX_DIRECT_IN_A3,                          // 11
    SPORT_TX_DIRECT_IN_A4,                          // 12
    SPORT_TX_DIRECT_IN_A5,                          // 13
    SPORT_TX_DIRECT_IN_A6,                          // 14
    SPORT_TX_DIRECT_IN_A7,                          // 15
    SPORT_TX_DIRECT_IN_B0,                          // 16
    SPORT_TX_DIRECT_IN_B1,                          // 17
    SPORT_TX_DIRECT_IN_B2,                          // 18
    SPORT_TX_DIRECT_IN_B3,                          // 19
    SPORT_TX_DIRECT_IN_B4,                          // 20
    SPORT_TX_DIRECT_IN_B5,                          // 21
    SPORT_TX_DIRECT_IN_B6,                          // 22
    SPORT_TX_DIRECT_IN_B7,                          // 23
    SPORT_TX_DIRECT_IN_C0,                          // 24
    SPORT_TX_DIRECT_IN_C1,                          // 25
    SPORT_TX_DIRECT_IN_C2,                          // 26
    SPORT_TX_DIRECT_IN_C3,                          // 27
    SPORT_TX_DIRECT_IN_C4,                          // 28
    SPORT_TX_DIRECT_IN_C5,                          // 29
    SPORT_TX_DIRECT_IN_C6,                          // 30
    SPORT_TX_DIRECT_IN_C7,                          // 31
    SPORT_TX_DIRECT_IN_SPORT0_TX_FIFO_0_REG_0_L,    // 32
    SPORT_TX_DIRECT_IN_SPORT0_TX_FIFO_0_REG_0_R,    // 33
    SPORT_TX_DIRECT_IN_SPORT0_TX_FIFO_0_REG_1_L,    // 34
    SPORT_TX_DIRECT_IN_SPORT0_TX_FIFO_0_REG_1_R,    // 35
    SPORT_TX_DIRECT_IN_TDM_RX_CH,                   // 36
    SPORT_TX_SEL_MAX,
} T_SPORT_TX_SEL;

typedef enum t_sport_rx_fifo_sel
{
    SPORT_RX_FIFO_0_REG_0_L,                        // 0
    SPORT_RX_FIFO_0_REG_0_R,                        // 1
    SPORT_RX_FIFO_0_REG_1_L,                        // 2
    SPORT_RX_FIFO_0_REG_1_R,                        // 3
    SPORT_RX_FIFO_1_REG_0_L,                        // 4
    SPORT_RX_FIFO_1_REG_0_R,                        // 5
    SPORT_RX_FIFO_1_REG_1_L,                        // 6
    SPORT_RX_FIFO_1_REG_1_R,                        // 7
    SPORT_RX_FIFO_SEL_MAX,                          // 8
} T_SPORT_RX_FIFO_SEL;

typedef enum t_sport_rx_sel
{
    SPORT_RX_CHANNEL_0,                             // 0
    SPORT_RX_CHANNEL_1,                             // 1
    SPORT_RX_CHANNEL_2,                             // 2
    SPORT_RX_CHANNEL_3,                             // 3
    SPORT_RX_CHANNEL_4,                             // 4
    SPORT_RX_CHANNEL_5,                             // 5
    SPORT_RX_CHANNEL_6,                             // 6
    SPORT_RX_CHANNEL_7,                             // 7
    SPORT_RX_DIRECT_IN_A0,                          // 8
    SPORT_RX_DIRECT_IN_A1,                          // 9
    SPORT_RX_DIRECT_IN_A2,                          // 10
    SPORT_RX_DIRECT_IN_A3,                          // 11
    SPORT_RX_DIRECT_IN_A4,                          // 12
    SPORT_RX_DIRECT_IN_A5,                          // 13
    SPORT_RX_DIRECT_IN_A6,                          // 14
    SPORT_RX_DIRECT_IN_A7,                          // 15
    SPORT_RX_DIRECT_IN_B0,                          // 16
    SPORT_RX_DIRECT_IN_B1,                          // 17
    SPORT_RX_DIRECT_IN_B2,                          // 18
    SPORT_RX_DIRECT_IN_B3,                          // 19
    SPORT_RX_DIRECT_IN_B4,                          // 20
    SPORT_RX_DIRECT_IN_B5,                          // 21
    SPORT_RX_DIRECT_IN_B6,                          // 22
    SPORT_RX_DIRECT_IN_B7,                          // 23
    SPORT_RX_DIRECT_IN_C0,                          // 24
    SPORT_RX_DIRECT_IN_C1,                          // 25
    SPORT_RX_DIRECT_IN_C2,                          // 26
    SPORT_RX_DIRECT_IN_C3,                          // 27
    SPORT_RX_DIRECT_IN_C4,                          // 28
    SPORT_RX_DIRECT_IN_C5,                          // 29
    SPORT_RX_DIRECT_IN_C6,                          // 30
    SPORT_RX_DIRECT_IN_C7,                          // 31
    SPORT_RX_SEL_MAX,
} T_SPORT_RX_SEL;

typedef enum t_sport_config_dir
{
    SPORT_CONFIG_TX = 1,
    SPORT_CONFIG_RX,
    SPORT_CONFIG_TRX,    // for bitmap usage
} T_SPORT_CONFIG_DIR;


/*============================================================================*
 *                         Functions
 *============================================================================*/

void sport_drv_reset(T_SPORT_ID id);

/* trx start & stop control */
void sport_drv_trx_start_ctrl(T_SPORT_ID id, uint8_t dir, bool is_enable);
void sport_drv_trx_lrck_ctrl(T_SPORT_ID id, uint8_t dir, bool is_enable);
void sport_drv_mcu_rx_int_ctrl(T_SPORT_ID id, uint8_t dir, bool is_enable);
void sport_drv_dsp_ctrl_mode(T_SPORT_ID id, bool is_enable);
uint32_t sport_drv_get_fifo_data(T_SPORT_ID id);

/* sport config API */
bool sport_drv_tx_config(T_SPORT_ID             id,
                         T_SPORT_TDM_MODE       tdm_mode,
                         T_SPORT_FORMAT_TYPE    format_type,
                         T_SPORT_CHANNEL_MODE   channel_mode,
                         T_SPORT_CHANNEL_LEN    channel_length,
                         T_SPORT_DATA_LEN       data_length,
                         uint32_t               sample_rate);

bool sport_drv_rx_config(T_SPORT_ID             id,
                         T_SPORT_TDM_MODE       tdm_mode,
                         T_SPORT_FORMAT_TYPE    format_type,
                         T_SPORT_CHANNEL_MODE   channel_mode,
                         T_SPORT_CHANNEL_LEN    channel_length,
                         T_SPORT_DATA_LEN       data_length,
                         uint32_t               sample_rate);

bool sport_drv_general_config(T_SPORT_ID             id,
                              T_SPORT_ROLE           role,
                              T_SPORT_CLK_SRC        clk_src,
                              T_SPORT_CODEC_SEL      codec_sel);

bool sport_drv_data_sel_config(T_SPORT_ID               id,
                               T_SPORT_DIRECT_MODE      direct_mode,
                               T_SPORT_DIRECT_OUT_EN    direct_out_en,
                               T_SPORT_TX_SEL          *tx_channel_map,
                               T_SPORT_RX_SEL          *rx_fifo_map);

bool sport_drv_mclk_config(bool mclk_enable, float mclk_freq);

bool sport_drv_bus_config(T_SPORT_ID id, T_SPORT_BUS_SEL bus_sel);

bool sport_drv_enable(T_SPORT_ID id);

bool sport_drv_disable(T_SPORT_ID id);

/* other function */
void sport_drv_direct_set_sample_rate(T_SPORT_ID id, uint8_t fixed_bclk,
                                      uint32_t tx_bclk_setting,
                                      uint32_t tx_bclk_div, uint32_t rx_bclk_setting, uint32_t rx_bclk_div);
bool sport_drv_init(void);

void sport_drv_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* _SPORT_DRIVER_H_ */

/******************* (C) COPYRIGHT 2019 Realtek Semiconductor Corporation *****END OF FILE****/


