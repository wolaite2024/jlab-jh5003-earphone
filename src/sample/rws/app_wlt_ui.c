/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file
   * @brief
   * @details
   * @author
   * @date
   * @version
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */

/*============================================================================*
 *                              Header Files
 *============================================================================*/

#include "trace.h"

#include "rtl876x_nvic.h"
#include "rtl876x_rcc.h"
#include "rtl876x_pinmux.h"
#include "rtl876x_adc.h"
#include "adc_manager.h"
#include "system_status_api.h"
#include "hal_adp.h"
#include "app_main.h"
#include "app_cfg.h"
#include "app_report.h"
#include "app_cfg.h"
#include "app_timer.h"
#include "app_io_msg.h"

#include "app_mmi.h"
#include "charger_utils.h"
#include "pmu_api.h"
#include "app_dlps.h"
#include "hal_gpio_int.h"
#include "hal_gpio.h"
#include "app_dongle_source_ctrl.h"
#include "charger_api.h"

static uint8_t app_ui_timer_id = 0;
static uint8_t timer_idx_ui_switch = 0;

#define PIN_2_4G_SWITCH    P5_7
#define PIN_2_4G_BT_SWITCH P1_4
#define BT_SWITCH      P1_0  

#define CHG_DONE_INFO      P4_7
#define CHG_OFF            P4_3
#define CHG_ISET           P4_5  
#define MIC_MUTE           P4_4

void MIC_MUTE_MMI(void)
{
  static uint8_t pre_action = 0;
   if ((hal_gpio_get_input_level(MIC_MUTE) == 0) &&  (pre_action != 1))
	{
	       pre_action = 1; 
	      app_mmi_handle_action(MMI_DEV_MIC_MUTE);

	 }
	else if ((hal_gpio_get_input_level(MIC_MUTE) == 1) &&  (pre_action != 2))
	{ 
	       pre_action = 2;
           app_mmi_handle_action(MMI_DEV_MIC_MUTE_UNMUTE);
    }
}

void switch_3_mmi(void)
{
     static uint8_t pre_action = 0;
   
	// APP_PRINT_INFO2("switch_3_mmi PIN_2_4G_BT_SWITCH: %d pre_action: %d",hal_gpio_get_input_level(PIN_2_4G_BT_SWITCH),pre_action);
	if ((hal_gpio_get_input_level(PIN_2_4G_SWITCH) == 0) &&  (pre_action != 1))
	{
	       pre_action = 1; 
		   app_cfg_nv.allowed_source = ALLOWED_SOURCE_24G;
		   APP_PRINT_INFO1("switch_3_mmi pre_action: %d", pre_action);
	      app_mmi_handle_action(MMI_24G_BT_AUDIO_SOURCE_SWITCH);

	 }
	else if ((hal_gpio_get_input_level(PIN_2_4G_BT_SWITCH) == 0) &&  (pre_action != 2))
	{ 
	       pre_action = 2;
		   app_cfg_nv.allowed_source = ALLOWED_SOURCE_BT_24G;
		   APP_PRINT_INFO1("switch_3_mmi pre_action: %d",pre_action);
           app_mmi_handle_action(MMI_24G_BT_AUDIO_SOURCE_SWITCH);
    }
	else if ((hal_gpio_get_input_level(BT_SWITCH) == 0) &&  (pre_action != 3))
	{
	       pre_action = 3;
		   app_cfg_nv.allowed_source = ALLOWED_SOURCE_BT;
		   APP_PRINT_INFO1("switch_3_mmi   pre_action: %d",pre_action);
           app_mmi_handle_action(MMI_24G_BT_AUDIO_SOURCE_SWITCH);

	}
	
}
static void app_ui_timeout_cb(uint8_t timer_evt, uint16_t param)
{
  
    switch_3_mmi();
	MIC_MUTE_MMI();
	app_start_timer(&timer_idx_ui_switch, "ui_switch",
					   app_ui_timer_id, 1, 0, false,
					   200);

}

void Initialize_charge(void)
{

    /* CHG Pad Config */
	    Pinmux_Config(CHG_DONE_INFO, DWGPIO);
    Pad_Config(CHG_DONE_INFO, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
		Pinmux_Config(CHG_ISET, DWGPIO);
    Pad_Config(CHG_ISET, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);
		Pinmux_Config(CHG_OFF, DWGPIO);
	Pad_Config(CHG_OFF, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_HIGH);
		Pinmux_Config(MIC_MUTE, DWGPIO);
	Pad_Config(MIC_MUTE, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);

}
T_CHARGER_STATE get_charge_status(void)
{
  T_CHARGER_STATE charge_status = STATE_CHARGER_END;
  
 if(adp_get_current_state(ADP_DETECT_5V) == ADP_STATE_IN)
  {
  if(hal_gpio_get_input_level(CHG_DONE_INFO) == 0)
   	{
       charge_status = STATE_CHARGER_FAST_CHARGE;
    }
    else
		charge_status = STATE_CHARGER_FINISH;
  }
   return  charge_status;
}

void Initialize_switch_ui(void)
{

    /* CHG Pad Config */
    Pinmux_Config(PIN_2_4G_SWITCH, DWGPIO);
    Pad_Config(PIN_2_4G_SWITCH, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);
	Pinmux_Config(PIN_2_4G_BT_SWITCH, DWGPIO);
    Pad_Config(PIN_2_4G_BT_SWITCH, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);	
	Pinmux_Config(BT_SWITCH, DWGPIO);
	Pad_Config(BT_SWITCH, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_DISABLE, PAD_OUT_HIGH);

}

void app_wlt_ui_init(void)
{
    Initialize_charge();
	Initialize_switch_ui();
    app_timer_reg_cb(app_ui_timeout_cb, &app_ui_timer_id);
	app_start_timer(&timer_idx_ui_switch, "ui_switch",
					   app_ui_timer_id, 1, 0, false,
					   2100);


}


