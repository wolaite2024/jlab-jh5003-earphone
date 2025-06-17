#include "app_gui.h"
#if (TARGET_LCD_DEVICE == LCD_DEVICE_NT35110)
#include "board.h"
#include "section.h"
#include "rtl876x_if8080.h"
#include "trace.h"
#include "module_lcd_8080.h"
#include "lcd_nt35510_480_8080.h"
#include "platform_utils.h"

void nt35510_write_cmd(uint16_t command)
{
    IF8080_SetCS();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    IF8080_ResetCS();
    uint8_t cmd_low, cmd_high;
    cmd_low = command;
    cmd_high = command >> 8;
    IF8080_SendCommand(cmd_high);
    IF8080_SendCommand(cmd_low);
}

void nt35510_write_data(uint8_t data)
{
    IF8080_SendData(&data, 1);
}

void nt35510_write_data16(uint16_t data)
{
    uint8_t data_buf[2];
    data_buf[0] = data >> 8;
    data_buf[1] = data;
    IF8080_SendData(data_buf, 2);
}

void lcd_nt35510_power_on(void)
{
//    nt35510_write_cmd(0x1100);
////    platform_delay_ms(160);
//    nt35510_write_cmd(0x2900);
    lcd_set_backlight(100);
}

void lcd_nt35510_power_off(void)
{
    nt35510_write_cmd(0x2800);
    nt35510_write_cmd(0x1000);
    lcd_set_backlight(0);
}


void lcd_nt35510_set_window(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd)
{
    IF8080_SwitchMode(IF8080_MODE_MANUAL);

    //OTM8009,NT35510  OK20190601
    nt35510_write_cmd(0x2a00);
    nt35510_write_data16(xStart >> 8);
    nt35510_write_cmd(0x2a01);
    nt35510_write_data16(xStart & 0xff);
    nt35510_write_cmd(0x2a02);
    nt35510_write_data16(xEnd >> 8);
    nt35510_write_cmd(0x2a03);
    nt35510_write_data16(xEnd & 0xff);

    nt35510_write_cmd(0x2b00);
    nt35510_write_data16(yStart >> 8);
    nt35510_write_cmd(0x2b01);
    nt35510_write_data16(yStart & 0xff);
    nt35510_write_cmd(0x2b02);
    nt35510_write_data16(yEnd >> 8);
    nt35510_write_cmd(0x2b03);
    nt35510_write_data16(yEnd & 0xff);

    IF8080_SetCS();
    /* Enable Auto mode */
    IF8080_SwitchMode(IF8080_MODE_AUTO);

    IF8080_ClearTxCounter();

    /* Configure command */
    uint8_t cmd[2] = {0x2C, 0x00};
    IF8080_SetCmdSequence(cmd, 2);

    /* Enable GDMA TX */
    IF8080_GDMACmd(ENABLE);

    /* Configure pixel number */
    uint32_t len = (xEnd - xStart + 1) * (yEnd - yStart + 1) * rtk_gui_config.pixel_bytes;
    IF8080_SetTxDataLen(len);

    /* Start output */
    IF8080_AutoModeCmd(IF8080_Auto_Mode_Direction_WRITE, ENABLE);
}

void lcd_nt35510_init(void)
{
    ///NT35510-HSD43
    //PAGE1
    nt35510_write_cmd(0xF000);    nt35510_write_data16(0x0055);
    nt35510_write_cmd(0xF001);    nt35510_write_data16(0x00AA);
    nt35510_write_cmd(0xF002);    nt35510_write_data16(0x0052);
    nt35510_write_cmd(0xF003);    nt35510_write_data16(0x0008);
    nt35510_write_cmd(0xF004);    nt35510_write_data16(0x0001);

    //Set AVDD 5.2V
    nt35510_write_cmd(0xB000);    nt35510_write_data16(0x000D);
    nt35510_write_cmd(0xB001);    nt35510_write_data16(0x000D);
    nt35510_write_cmd(0xB002);    nt35510_write_data16(0x000D);

    //Set AVEE 5.2V
    nt35510_write_cmd(0xB100);    nt35510_write_data16(0x000D);
    nt35510_write_cmd(0xB101);    nt35510_write_data16(0x000D);
    nt35510_write_cmd(0xB102);    nt35510_write_data16(0x000D);

    //Set VCL -2.5V
    nt35510_write_cmd(0xB200);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xB201);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xB202);    nt35510_write_data16(0x0000);

    //Set AVDD Ratio
    nt35510_write_cmd(0xB600);    nt35510_write_data16(0x0044);
    nt35510_write_cmd(0xB601);    nt35510_write_data16(0x0044);
    nt35510_write_cmd(0xB602);    nt35510_write_data16(0x0044);

    //Set AVEE Ratio
    nt35510_write_cmd(0xB700);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xB701);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xB702);    nt35510_write_data16(0x0034);

    //Set VCL -2.5V
    nt35510_write_cmd(0xB800);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xB801);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xB802);    nt35510_write_data16(0x0034);

    //Control VGH booster voltage rang
    nt35510_write_cmd(0xBF00);    nt35510_write_data16(0x0001); //VGH:7~18V

    //VGH=15V(1V/step)  Free pump
    nt35510_write_cmd(0xB300);    nt35510_write_data16(0x000f);     //08
    nt35510_write_cmd(0xB301);    nt35510_write_data16(0x000f);     //08
    nt35510_write_cmd(0xB302);    nt35510_write_data16(0x000f);     //08

    //VGH Ratio
    nt35510_write_cmd(0xB900);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xB901);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xB902);    nt35510_write_data16(0x0034);

    //VGL_REG=-10(1V/step)
    nt35510_write_cmd(0xB500);    nt35510_write_data16(0x0008);
    nt35510_write_cmd(0xB501);    nt35510_write_data16(0x0008);
    nt35510_write_cmd(0xB502);    nt35510_write_data16(0x0008);

    nt35510_write_cmd(0xC200);    nt35510_write_data16(0x0003);

    //VGLX Ratio
    nt35510_write_cmd(0xBA00);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xBA01);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xBA02);    nt35510_write_data16(0x0034);

    //VGMP/VGSP=4.5V/0V
    nt35510_write_cmd(0xBC00);    nt35510_write_data16(0x0000);     //00
    nt35510_write_cmd(0xBC01);    nt35510_write_data16(0x0078);     //C8 =5.5V/90=4.8V
    nt35510_write_cmd(0xBC02);    nt35510_write_data16(0x0000);     //01

    //VGMN/VGSN=-4.5V/0V
    nt35510_write_cmd(0xBD00);    nt35510_write_data16(0x0000); //00
    nt35510_write_cmd(0xBD01);    nt35510_write_data16(0x0078); //90
    nt35510_write_cmd(0xBD02);    nt35510_write_data16(0x0000);

    //Vcom=-1.4V(12.5mV/step)
    nt35510_write_cmd(0xBE00);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xBE01);    nt35510_write_data16(0x0064); //HSD:64;Novatek:50=-1.0V, 80  5f

    //Gamma (R+)
    nt35510_write_cmd(0xD100);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD101);    nt35510_write_data16(0x0033);
    nt35510_write_cmd(0xD102);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD103);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xD104);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD105);    nt35510_write_data16(0x003A);
    nt35510_write_cmd(0xD106);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD107);    nt35510_write_data16(0x004A);
    nt35510_write_cmd(0xD108);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD109);    nt35510_write_data16(0x005C);
    nt35510_write_cmd(0xD10A);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD10B);    nt35510_write_data16(0x0081);
    nt35510_write_cmd(0xD10C);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD10D);    nt35510_write_data16(0x00A6);
    nt35510_write_cmd(0xD10E);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD10F);    nt35510_write_data16(0x00E5);
    nt35510_write_cmd(0xD110);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD111);    nt35510_write_data16(0x0013);
    nt35510_write_cmd(0xD112);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD113);    nt35510_write_data16(0x0054);
    nt35510_write_cmd(0xD114);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD115);    nt35510_write_data16(0x0082);
    nt35510_write_cmd(0xD116);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD117);    nt35510_write_data16(0x00CA);
    nt35510_write_cmd(0xD118);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD119);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD11A);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD11B);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD11C);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD11D);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xD11E);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD11F);    nt35510_write_data16(0x0067);
    nt35510_write_cmd(0xD120);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD121);    nt35510_write_data16(0x0084);
    nt35510_write_cmd(0xD122);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD123);    nt35510_write_data16(0x00A4);
    nt35510_write_cmd(0xD124);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD125);    nt35510_write_data16(0x00B7);
    nt35510_write_cmd(0xD126);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD127);    nt35510_write_data16(0x00CF);
    nt35510_write_cmd(0xD128);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD129);    nt35510_write_data16(0x00DE);
    nt35510_write_cmd(0xD12A);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD12B);    nt35510_write_data16(0x00F2);
    nt35510_write_cmd(0xD12C);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD12D);    nt35510_write_data16(0x00FE);
    nt35510_write_cmd(0xD12E);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD12F);    nt35510_write_data16(0x0010);
    nt35510_write_cmd(0xD130);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD131);    nt35510_write_data16(0x0033);
    nt35510_write_cmd(0xD132);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD133);    nt35510_write_data16(0x006D);

    //Gamma (G+)
    nt35510_write_cmd(0xD200);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD201);    nt35510_write_data16(0x0033);
    nt35510_write_cmd(0xD202);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD203);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xD204);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD205);    nt35510_write_data16(0x003A);
    nt35510_write_cmd(0xD206);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD207);    nt35510_write_data16(0x004A);
    nt35510_write_cmd(0xD208);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD209);    nt35510_write_data16(0x005C);
    nt35510_write_cmd(0xD20A);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD20B);    nt35510_write_data16(0x0081);
    nt35510_write_cmd(0xD20C);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD20D);    nt35510_write_data16(0x00A6);
    nt35510_write_cmd(0xD20E);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD20F);    nt35510_write_data16(0x00E5);
    nt35510_write_cmd(0xD210);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD211);    nt35510_write_data16(0x0013);
    nt35510_write_cmd(0xD212);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD213);    nt35510_write_data16(0x0054);
    nt35510_write_cmd(0xD214);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD215);    nt35510_write_data16(0x0082);
    nt35510_write_cmd(0xD216);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD217);    nt35510_write_data16(0x00CA);
    nt35510_write_cmd(0xD218);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD219);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD21A);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD21B);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD21C);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD21D);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xD21E);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD21F);    nt35510_write_data16(0x0067);
    nt35510_write_cmd(0xD220);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD221);    nt35510_write_data16(0x0084);
    nt35510_write_cmd(0xD222);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD223);    nt35510_write_data16(0x00A4);
    nt35510_write_cmd(0xD224);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD225);    nt35510_write_data16(0x00B7);
    nt35510_write_cmd(0xD226);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD227);    nt35510_write_data16(0x00CF);
    nt35510_write_cmd(0xD228);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD229);    nt35510_write_data16(0x00DE);
    nt35510_write_cmd(0xD22A);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD22B);    nt35510_write_data16(0x00F2);
    nt35510_write_cmd(0xD22C);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD22D);    nt35510_write_data16(0x00FE);
    nt35510_write_cmd(0xD22E);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD22F);    nt35510_write_data16(0x0010);
    nt35510_write_cmd(0xD230);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD231);    nt35510_write_data16(0x0033);
    nt35510_write_cmd(0xD232);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD233);    nt35510_write_data16(0x006D);

    //Gamma (B+)
    nt35510_write_cmd(0xD300);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD301);    nt35510_write_data16(0x0033);
    nt35510_write_cmd(0xD302);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD303);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xD304);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD305);    nt35510_write_data16(0x003A);
    nt35510_write_cmd(0xD306);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD307);    nt35510_write_data16(0x004A);
    nt35510_write_cmd(0xD308);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD309);    nt35510_write_data16(0x005C);
    nt35510_write_cmd(0xD30A);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD30B);    nt35510_write_data16(0x0081);
    nt35510_write_cmd(0xD30C);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD30D);    nt35510_write_data16(0x00A6);
    nt35510_write_cmd(0xD30E);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD30F);    nt35510_write_data16(0x00E5);
    nt35510_write_cmd(0xD310);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD311);    nt35510_write_data16(0x0013);
    nt35510_write_cmd(0xD312);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD313);    nt35510_write_data16(0x0054);
    nt35510_write_cmd(0xD314);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD315);    nt35510_write_data16(0x0082);
    nt35510_write_cmd(0xD316);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD317);    nt35510_write_data16(0x00CA);
    nt35510_write_cmd(0xD318);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD319);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD31A);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD31B);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD31C);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD31D);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xD31E);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD31F);    nt35510_write_data16(0x0067);
    nt35510_write_cmd(0xD320);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD321);    nt35510_write_data16(0x0084);
    nt35510_write_cmd(0xD322);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD323);    nt35510_write_data16(0x00A4);
    nt35510_write_cmd(0xD324);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD325);    nt35510_write_data16(0x00B7);
    nt35510_write_cmd(0xD326);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD327);    nt35510_write_data16(0x00CF);
    nt35510_write_cmd(0xD328);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD329);    nt35510_write_data16(0x00DE);
    nt35510_write_cmd(0xD32A);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD32B);    nt35510_write_data16(0x00F2);
    nt35510_write_cmd(0xD32C);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD32D);    nt35510_write_data16(0x00FE);
    nt35510_write_cmd(0xD32E);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD32F);    nt35510_write_data16(0x0010);
    nt35510_write_cmd(0xD330);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD331);    nt35510_write_data16(0x0033);
    nt35510_write_cmd(0xD332);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD333);    nt35510_write_data16(0x006D);

    //Gamma (R-)
    nt35510_write_cmd(0xD400);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD401);    nt35510_write_data16(0x0033);
    nt35510_write_cmd(0xD402);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD403);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xD404);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD405);    nt35510_write_data16(0x003A);
    nt35510_write_cmd(0xD406);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD407);    nt35510_write_data16(0x004A);
    nt35510_write_cmd(0xD408);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD409);    nt35510_write_data16(0x005C);
    nt35510_write_cmd(0xD40A);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD40B);    nt35510_write_data16(0x0081);
    nt35510_write_cmd(0xD40C);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD40D);    nt35510_write_data16(0x00A6);
    nt35510_write_cmd(0xD40E);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD40F);    nt35510_write_data16(0x00E5);
    nt35510_write_cmd(0xD410);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD411);    nt35510_write_data16(0x0013);
    nt35510_write_cmd(0xD412);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD413);    nt35510_write_data16(0x0054);
    nt35510_write_cmd(0xD414);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD415);    nt35510_write_data16(0x0082);
    nt35510_write_cmd(0xD416);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD417);    nt35510_write_data16(0x00CA);
    nt35510_write_cmd(0xD418);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD419);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD41A);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD41B);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD41C);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD41D);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xD41E);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD41F);    nt35510_write_data16(0x0067);
    nt35510_write_cmd(0xD420);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD421);    nt35510_write_data16(0x0084);
    nt35510_write_cmd(0xD422);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD423);    nt35510_write_data16(0x00A4);
    nt35510_write_cmd(0xD424);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD425);    nt35510_write_data16(0x00B7);
    nt35510_write_cmd(0xD426);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD427);    nt35510_write_data16(0x00CF);
    nt35510_write_cmd(0xD428);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD429);    nt35510_write_data16(0x00DE);
    nt35510_write_cmd(0xD42A);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD42B);    nt35510_write_data16(0x00F2);
    nt35510_write_cmd(0xD42C);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD42D);    nt35510_write_data16(0x00FE);
    nt35510_write_cmd(0xD42E);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD42F);    nt35510_write_data16(0x0010);
    nt35510_write_cmd(0xD430);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD431);    nt35510_write_data16(0x0033);
    nt35510_write_cmd(0xD432);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD433);    nt35510_write_data16(0x006D);

    //Gamma (G-)
    nt35510_write_cmd(0xD500);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD501);    nt35510_write_data16(0x0033);
    nt35510_write_cmd(0xD502);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD503);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xD504);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD505);    nt35510_write_data16(0x003A);
    nt35510_write_cmd(0xD506);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD507);    nt35510_write_data16(0x004A);
    nt35510_write_cmd(0xD508);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD509);    nt35510_write_data16(0x005C);
    nt35510_write_cmd(0xD50A);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD50B);    nt35510_write_data16(0x0081);
    nt35510_write_cmd(0xD50C);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD50D);    nt35510_write_data16(0x00A6);
    nt35510_write_cmd(0xD50E);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD50F);    nt35510_write_data16(0x00E5);
    nt35510_write_cmd(0xD510);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD511);    nt35510_write_data16(0x0013);
    nt35510_write_cmd(0xD512);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD513);    nt35510_write_data16(0x0054);
    nt35510_write_cmd(0xD514);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD515);    nt35510_write_data16(0x0082);
    nt35510_write_cmd(0xD516);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD517);    nt35510_write_data16(0x00CA);
    nt35510_write_cmd(0xD518);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD519);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD51A);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD51B);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD51C);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD51D);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xD51E);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD51F);    nt35510_write_data16(0x0067);
    nt35510_write_cmd(0xD520);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD521);    nt35510_write_data16(0x0084);
    nt35510_write_cmd(0xD522);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD523);    nt35510_write_data16(0x00A4);
    nt35510_write_cmd(0xD524);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD525);    nt35510_write_data16(0x00B7);
    nt35510_write_cmd(0xD526);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD527);    nt35510_write_data16(0x00CF);
    nt35510_write_cmd(0xD528);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD529);    nt35510_write_data16(0x00DE);
    nt35510_write_cmd(0xD52A);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD52B);    nt35510_write_data16(0x00F2);
    nt35510_write_cmd(0xD52C);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD52D);    nt35510_write_data16(0x00FE);
    nt35510_write_cmd(0xD52E);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD52F);    nt35510_write_data16(0x0010);
    nt35510_write_cmd(0xD530);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD531);    nt35510_write_data16(0x0033);
    nt35510_write_cmd(0xD532);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD533);    nt35510_write_data16(0x006D);

    //Gamma (B-)
    nt35510_write_cmd(0xD600);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD601);    nt35510_write_data16(0x0033);
    nt35510_write_cmd(0xD602);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD603);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xD604);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD605);    nt35510_write_data16(0x003A);
    nt35510_write_cmd(0xD606);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD607);    nt35510_write_data16(0x004A);
    nt35510_write_cmd(0xD608);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD609);    nt35510_write_data16(0x005C);
    nt35510_write_cmd(0xD60A);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD60B);    nt35510_write_data16(0x0081);
    nt35510_write_cmd(0xD60C);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD60D);    nt35510_write_data16(0x00A6);
    nt35510_write_cmd(0xD60E);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD60F);    nt35510_write_data16(0x00E5);
    nt35510_write_cmd(0xD610);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD611);    nt35510_write_data16(0x0013);
    nt35510_write_cmd(0xD612);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD613);    nt35510_write_data16(0x0054);
    nt35510_write_cmd(0xD614);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD615);    nt35510_write_data16(0x0082);
    nt35510_write_cmd(0xD616);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD617);    nt35510_write_data16(0x00CA);
    nt35510_write_cmd(0xD618);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD619);    nt35510_write_data16(0x0000);
    nt35510_write_cmd(0xD61A);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD61B);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xD61C);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD61D);    nt35510_write_data16(0x0034);
    nt35510_write_cmd(0xD61E);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD61F);    nt35510_write_data16(0x0067);
    nt35510_write_cmd(0xD620);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD621);    nt35510_write_data16(0x0084);
    nt35510_write_cmd(0xD622);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD623);    nt35510_write_data16(0x00A4);
    nt35510_write_cmd(0xD624);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD625);    nt35510_write_data16(0x00B7);
    nt35510_write_cmd(0xD626);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD627);    nt35510_write_data16(0x00CF);
    nt35510_write_cmd(0xD628);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD629);    nt35510_write_data16(0x00DE);
    nt35510_write_cmd(0xD62A);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD62B);    nt35510_write_data16(0x00F2);
    nt35510_write_cmd(0xD62C);    nt35510_write_data16(0x0002);
    nt35510_write_cmd(0xD62D);    nt35510_write_data16(0x00FE);
    nt35510_write_cmd(0xD62E);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD62F);    nt35510_write_data16(0x0010);
    nt35510_write_cmd(0xD630);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD631);    nt35510_write_data16(0x0033);
    nt35510_write_cmd(0xD632);    nt35510_write_data16(0x0003);
    nt35510_write_cmd(0xD633);    nt35510_write_data16(0x006D);

    //PAGE0
    nt35510_write_cmd(0xF000);    nt35510_write_data16(0x0055);
    nt35510_write_cmd(0xF001);    nt35510_write_data16(0x00AA);
    nt35510_write_cmd(0xF002);    nt35510_write_data16(0x0052);
    nt35510_write_cmd(0xF003);    nt35510_write_data16(0x0008);
    nt35510_write_cmd(0xF004);    nt35510_write_data16(0x0000);

    //480x800
    nt35510_write_cmd(0xB500);    nt35510_write_data16(0x0050);

    //nt35510_write_cmd(0x2C00);    nt35510_write_data16(0x0006); //8BIT 6-6-6?

    //Dispay control
    nt35510_write_cmd(0xB100);    nt35510_write_data16(0x00CC);
    nt35510_write_cmd(0xB101);    nt35510_write_data16(0x0000); // S1->S1440:00;S1440->S1:02

    //Source hold time (Nova non-used)
    nt35510_write_cmd(0xB600);    nt35510_write_data16(0x0005);

    //Gate EQ control    (Nova non-used)
    nt35510_write_cmd(0xB700);    nt35510_write_data16(0x0077);  //HSD:70;Nova:77
    nt35510_write_cmd(0xB701);    nt35510_write_data16(0x0077); //HSD:70;Nova:77

    //Source EQ control (Nova non-used)
    nt35510_write_cmd(0xB800);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xB801);    nt35510_write_data16(0x0003); //HSD:05;Nova:07
    nt35510_write_cmd(0xB802);    nt35510_write_data16(0x0003); //HSD:05;Nova:07
    nt35510_write_cmd(0xB803);    nt35510_write_data16(0x0003); //HSD:05;Nova:07

    //Inversion mode: column
    nt35510_write_cmd(0xBC00);    nt35510_write_data16(0x0002); //00: column
    nt35510_write_cmd(0xBC01);    nt35510_write_data16(0x0000); //01:1dot
    nt35510_write_cmd(0xBC02);    nt35510_write_data16(0x0000);

    //Frame rate    (Nova non-used)
    nt35510_write_cmd(0xBD00);    nt35510_write_data16(0x0001);
    nt35510_write_cmd(0xBD01);    nt35510_write_data16(0x0084);
    nt35510_write_cmd(0xBD02);    nt35510_write_data16(0x001c); //HSD:06;Nova:1C
    nt35510_write_cmd(0xBD03);    nt35510_write_data16(0x001c); //HSD:04;Nova:1C
    nt35510_write_cmd(0xBD04);    nt35510_write_data16(0x0000);

    //LGD timing control(4H/4-delay_ms)
    nt35510_write_cmd(0xC900);    nt35510_write_data16(0x00D0); //3H:0x50;4H:0xD0    //D
    nt35510_write_cmd(0xC901);    nt35510_write_data16(0x0002);  //HSD:05;Nova:02
    nt35510_write_cmd(0xC902);    nt35510_write_data16(0x0050); //HSD:05;Nova:50
    nt35510_write_cmd(0xC903);    nt35510_write_data16(0x0050); //HSD:05;Nova:50    ;STV delay_ms time
    nt35510_write_cmd(0xC904);    nt35510_write_data16(0x0050); //HSD:05;Nova:50    ;CLK delay_ms time

    nt35510_write_cmd(0x3600);    nt35510_write_data16(0x00C0);
    nt35510_write_cmd(0x3500);    nt35510_write_data16(0x0000);

    nt35510_write_cmd(0xFF00);    nt35510_write_data16(0x00AA);
    nt35510_write_cmd(0xFF01);    nt35510_write_data16(0x0055);
    nt35510_write_cmd(0xFF02);    nt35510_write_data16(0x0025);
    nt35510_write_cmd(0xFF03);    nt35510_write_data16(0x0001);

    nt35510_write_cmd(0xFC00);    nt35510_write_data16(0x0016);
    nt35510_write_cmd(0xFC01);    nt35510_write_data16(0x00A2);
    nt35510_write_cmd(0xFC02);    nt35510_write_data16(0x0026);
    nt35510_write_cmd(0x3A00);    nt35510_write_data16(0x0006);

    nt35510_write_cmd(0x3A00);    nt35510_write_data16(0x0055);
    //Sleep out
    nt35510_write_cmd(0x1100);     //?
    platform_delay_ms(160);

    //Display on
    nt35510_write_cmd(0x2900);
}
#endif
