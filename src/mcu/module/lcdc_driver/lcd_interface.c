#include "app_gui.h"
#include "lcd_interface.h"
#if (LCD_INTERFACE == LCD_INTERFACE_QSPI)
#include "rtl876x.h"
#include "fmc_api.h"
#include "fmc_api_ext.h"
#elif (LCD_INTERFACE == LCD_INTERFACE_LCDC_QSPI)
#include "rtl_lcdc_dbic.h"
#endif

#if (LCD_INTERFACE == LCD_INTERFACE_QSPI)
#if (TARGET_RTL87X3E == 1)
#define SPIC_ID                             FMC_SPIC_ID_2
#define SPIC_BASE                           SPIC2
#elif (CONFIG_SOC_SERIES_RTL8773D == 1)
#define SPIC_ID                             FMC_SPIC_ID_3
#define SPIC_BASE                           SPIC3
#endif

void lcd_write(uint8_t *buf, uint32_t len)
{
    fmc_spic_enable(SPIC_ID, false);
    fmc_spic_set_user_mode(SPIC_ID);
    fmc_spic_set_multi_ch(SPIC_ID, FMC_SPIC_SING_CH, FMC_SPIC_SING_CH, FMC_SPIC_SING_CH);
    fmc_spic_set_tx_mode(SPIC_ID);

    fmc_spic_set_cmd_len(SPIC_ID, 1);
    fmc_spic_set_user_addr_len(SPIC_ID, 3);

    if (len > 4)
    {
        fmc_spic_set_txndf(SPIC_ID, len - 4);
    }
    else
    {
        fmc_spic_set_txndf(SPIC_ID, 0);
    }

    for (uint32_t i = 0; i < len; i++)
    {
        fmc_spic_set_dr(SPIC_ID, buf[i], FMC_SPIC_DATA_BYTE);
    }

    fmc_spic_enable(SPIC_ID, true);
    while (fmc_spic_in_busy(SPIC_ID));
    fmc_spic_enable(SPIC_ID, false);
}

void lcd_enter_data_output_mode(uint32_t len_byte)
{
    fmc_spic_enable(SPIC_ID, false);
    fmc_spic_set_user_mode(SPIC_ID);

    fmc_spic_set_multi_ch(SPIC_ID, FMC_SPIC_SING_CH, FMC_SPIC_SING_CH, FMC_SPIC_QUAD_CH);
    fmc_spic_set_tx_mode(SPIC_ID);
    fmc_spic_set_seq_trans(SPIC_ID, true);

    fmc_spic_set_cmd_len(SPIC_ID, 1);
    fmc_spic_set_user_addr_len(SPIC_ID, 3);
    fmc_spic_set_txndf(SPIC_ID, len_byte);

    uint32_t first_word = 0x32 | (0x002c00 << 8);
    fmc_spic_set_dr(SPIC_ID, first_word, FMC_SPIC_DATA_WORD);

    fmc_spic_set_dmac_tx_en(SPIC_ID, true);
    fmc_spic_set_dmac_tx_data_lv(SPIC_ID, 4);

    //allow spic tx ndf setting and data latency
    SPIC_BASE->CTRLR0 |= BIT11;

    fmc_spic_enable(SPIC_ID, true);
}
#elif (LCD_INTERFACE == LCD_INTERFACE_LCDC_QSPI)
#define BIT_CMD_CH(x)           (((x) & 0x00000003) << 20)
#define BIT_DATA_CH(x)          (((x) & 0x00000003) << 18)
#define BIT_ADDR_CH(x)          (((x) & 0x00000003) << 16)
#define BIT_TMOD(x)             (((x) & 0x00000003) << 8)

void lcd_write(uint8_t *buf, uint32_t len)
{
    //DBIC_Cmd(DISABLE);
    DBIC->CTRLR0 |= BIT31;
    DBIC->CTRLR0 &= ~(BIT_CMD_CH(3) | BIT_ADDR_CH(3) | BIT_DATA_CH(3));//SET CHANNEL NUM
    DBIC->CTRLR0 &= ~(BIT_TMOD(3)); //tx mode
    DBIC_TX_NDF(len - 4);
    DBIC_CmdLength(1);
    DBIC_AddrLength(3);
    DBIC_USER_LENGTH_TypeDef reg_val = {.d32 = DBIC->USER_LENGTH};

    for (uint32_t i = 0; i < len; i++)
    {
        DBIC->DR[0].byte = buf[i];
    }

    DBIC_Cmd(ENABLE);
    while (DBIC->SR & BIT0);// wait bus busy
    //DBIC_Cmd(DISABLE);
}

//total 4 byte, first byte is 0x32
void lcd_enter_data_output_mode(uint32_t len_byte)
{
//    DBIC_Cmd(DISABLE);
    LCDC_AXIMUXMode(LCDC_FW_MODE);
    DBIC_SwitchMode(DBIC_USER_MODE);
    DBIC_SwitchDirect(DBIC_TMODE_TX);
    DBIC_CmdLength(1);
    DBIC_AddrLength(3);
    DBIC_TX_NDF(len_byte);

    DBIC->CTRLR0 &= ~(BIT_CMD_CH(3) | BIT_ADDR_CH(3) | BIT_DATA_CH(3));//SET CHANNEL NUM
    DBIC->CTRLR0 |= (BIT_CMD_CH(0) | BIT_ADDR_CH(0) | BIT_DATA_CH(2));

    DBIC->FLUSH_FIFO = 0x01;

    /* must push cmd and address to handler before SPIC enable */
    LCDC_SPICCmd(0x32);
    LCDC_SPICAddr(0x002c00);

    DBIC->DMACR = 2;

    /* change this value can not influence the result. the wave is split into two periods. the first is 32 bytes. */
    DBIC->DMATDLR = 32; /* no any influence. */
//    DBIC_Sequence_Write(ENABLE);
    DBIC->ICR = 1;
    LCDC_AXIMUXMode(LCDC_HW_MODE);
//    DBIC_Cmd(ENABLE);
}
#endif
