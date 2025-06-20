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
static uint8_t app_adc_timer_id = 0;

static uint8_t timer_idx_ui_switch = 0;
static bool adc_vbat_ntc_voltage_init = false;
static uint8_t timer_idx_adc_read_vbat_ntc_voltage = 0;
static uint8_t adc_channel_vbat_ntc_voltage = 0;
static uint32_t temperature_battery = 0;
 bool wlt_time_ui_init = false;



#define PIN_2_4G_SWITCH    P5_7
#define PIN_2_4G_BT_SWITCH P1_4
#define BT_SWITCH      P1_0  

#define CHG_DONE_INFO      P4_7
#define CHG_OFF            P4_3
#define CHG_ISET           P4_5  
#define MIC_MUTE           P4_4

typedef enum
{
    APP_TIMER_ADC_READ_VBAT_NTC_VOLTAGE = 0x00,
} T_APP_ADC_TIMER;

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
uint8_t HiLowChargeFlag = 0 ;

T_CHARGER_STATE get_charge_status(void)
{
  T_CHARGER_STATE charge_status = STATE_CHARGER_END;
  static uint8_t charge_finsh_debunce = 0; 
 if(adp_get_current_state(ADP_DETECT_5V) == ADP_STATE_IN)
  {
  if(hal_gpio_get_input_level(CHG_DONE_INFO) == 0)
   	{
   	   charge_finsh_debunce = 0;
       charge_status = STATE_CHARGER_FAST_CHARGE;
    }
    else
	{
	     if(charge_finsh_debunce ++ >= 2)
		 {
		    charge_finsh_debunce = 0;
		    charge_status = STATE_CHARGER_FINISH;
	     }
    }

	if(HiLowChargeFlag == 1)
 	{
      charge_status = STATE_CHARGER_ERROR;
    }
  }
 
 IO_PRINT_INFO1("get_charge_status: charge_status %d",
                   charge_status);
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

static void board_adc_init(void)
{
    Pad_Config(ADC_3, PAD_SW_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_LOW); 
    Pinmux_Config(ADC_3, IDLE_MODE);

}

static void app_adc_vbat_ntc_voltage_read(void)
{
     IO_PRINT_INFO2("app_adc_vbat_ntc_voltage_init: adc_vbat_ntc_voltage_init %d wlt_time_ui_init %d",
                   adc_vbat_ntc_voltage_init,wlt_time_ui_init);

    if (adc_vbat_ntc_voltage_init)
    {
        adc_mgr_enable_req(adc_channel_vbat_ntc_voltage);
    }
}

 void app_adc_deinit(void)
 {
	 IO_PRINT_INFO2("app_adc_adp_voltage_deinit: adc_vbat_ntc_voltage_init %d, adc_channel_vbat_ntc_voltage %d",
					adc_vbat_ntc_voltage_init, adc_channel_vbat_ntc_voltage);
 
	 app_stop_timer(&timer_idx_adc_read_vbat_ntc_voltage);
	 if (adc_vbat_ntc_voltage_init)
	 {
		 adc_mgr_free_chann(adc_channel_vbat_ntc_voltage);
	 }
 
 
 }

 void app_temp_handle(void)
{
  static uint8_t HiLowTempTime = 0 ,ChargeHiLowTempTime = 0;
  T_ADP_STATE ADP_DET_status = 0;

	ADP_DET_status = adp_get_current_state(ADP_DETECT_5V);


    if (ADP_DET_status == ADP_STATE_OUT)
    {

			if ((temperature_battery <= 237) ||( temperature_battery >= 870))		
		        {
		            HiLowTempTime ++;		         		        
		        }
		        else
		        {
		             HiLowTempTime = 0;
		        }
				
			    if(HiLowTempTime >= 3)	
				{
				     APP_PRINT_INFO1(" Power off HiLowTempTime ===: %d",  HiLowTempTime);
					 app_adc_deinit();
					 os_delay(100);
				     app_mmi_handle_action(MMI_DEV_POWER_OFF);   
					 
				}
	     
    }
	else if (ADP_DET_status == ADP_STATE_IN)
	{

	            if ((temperature_battery <= 237) ||( temperature_battery >= 870))		
		        {
		            ChargeHiLowTempTime ++;		         		        
		        }
				 else if ((temperature_battery <= 337) ||( temperature_battery >= 748))		
		        {
		          if(HiLowChargeFlag != 1)
		           {     
		              Pad_Config(CHG_OFF, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);	
				      HiLowChargeFlag = 1;
					   APP_PRINT_INFO1("  off charge ===: %d",  HiLowChargeFlag);
		          	}
		        }
		       	else  if ((temperature_battery >= 353) ||( temperature_battery <= 730))	
		        {
		          if(HiLowChargeFlag == 1)
		           {
		              HiLowChargeFlag = 2;
					  APP_PRINT_INFO1("  open charge ===: %d",  HiLowChargeFlag);
		             Pad_Config(CHG_OFF, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_NONE, PAD_OUT_DISABLE, PAD_OUT_HIGH);
		          	} 
		        }
		        else
		        {
		             ChargeHiLowTempTime = 0;
		        } 
				
				if((app_db.device_state == APP_DEVICE_STATE_ON) &&(ChargeHiLowTempTime >= 3) )
				{
				      app_adc_deinit();
                      app_mmi_handle_action(MMI_DEV_POWER_OFF); 
					  HiLowChargeFlag = 1;
					  Pad_Config(CHG_OFF, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);	
					
					  APP_PRINT_INFO2(" Power off ChargeHiLowTempTime ===: %d HiLowChargeFlag = %d",  ChargeHiLowTempTime,HiLowChargeFlag);
				}
				else if(ChargeHiLowTempTime >= 3)
				{

                   Pad_Config(CHG_OFF, PAD_PINMUX_MODE, PAD_IS_PWRON, PAD_PULL_UP, PAD_OUT_ENABLE, PAD_OUT_HIGH);	
					  APP_PRINT_INFO2("  off charge ChargeHiLowTempTime ===: %d HiLowChargeFlag = %d",  ChargeHiLowTempTime,HiLowChargeFlag);
				}

	}
}

static void app_adc_vbat_ntc_voltage_read_callback(void *pvPara, uint32_t int_status)
{
    uint16_t adc_data;
    uint16_t sched_bit_map = 0x0001;
	bool ret = true;

    adc_mgr_read_data_req(adc_channel_vbat_ntc_voltage, &adc_data, sched_bit_map);

   temperature_battery = ADC_GetHighBypassRes(adc_data, EXT_SINGLE_ENDED(3));
    IO_PRINT_INFO1("app_adc_vbat_ntc_voltage_read_callback: temperature_battery %d",
                   temperature_battery);
    
	 T_IO_MSG adp_msg;
 
      adp_msg.type = IO_MSG_TYPE_NTC;
 
	  ret = app_io_msg_send(&adp_msg);
				   
}



static void app_adc_vbat_ntc_voltage_init(void)
{
    IO_PRINT_INFO2("app_adc_vbat_ntc_voltage_init: adc_vbat_ntc_voltage_init %d wlt_time_ui_init %d",
                   adc_vbat_ntc_voltage_init,wlt_time_ui_init);

    if (adc_vbat_ntc_voltage_init)
    {
        /* already init */
        return;
    }

    board_adc_init();

    /* High bypass resistance mode config, Please notice that the input voltage of
       adc channel using high bypass mode should not be over 0.9V */
    ADC_HighBypassCmd(3, ENABLE);

    ADC_InitTypeDef ADC_InitStruct;

    ADC_StructInit(&ADC_InitStruct);
    ADC_InitStruct.adcClock = ADC_CLK_39K;
    ADC_InitStruct.bitmap = 0x0001;
    ADC_InitStruct.schIndex[0] = EXT_SINGLE_ENDED(3);
    if (!adc_mgr_register_req(&ADC_InitStruct,
                              (adc_callback_function_t)app_adc_vbat_ntc_voltage_read_callback,
                              &adc_channel_vbat_ntc_voltage))
    {
        IO_PRINT_ERROR0("app_adc_vbat_ntc_voltage_init: adc_mgr_register_req failed");
        return;
    }

    adc_vbat_ntc_voltage_init = true;
}

static void app_adc_timeout_cb(uint8_t timer_evt, uint16_t param)
{
    switch (timer_evt)
    {
    case APP_TIMER_ADC_READ_VBAT_NTC_VOLTAGE:
        {
            app_adc_vbat_ntc_voltage_read();
        }
	    app_led_check_charging_mode(0);
        break;

    default:
		
        break;
    }
}

static void adc_charger_init(void)
{
    app_timer_reg_cb(app_adc_timeout_cb, &app_adc_timer_id);

    app_adc_vbat_ntc_voltage_init();

    app_start_timer(&timer_idx_adc_read_vbat_ntc_voltage, "adc_read_vbat_ntc_voltage",
                    app_adc_timer_id, APP_TIMER_ADC_READ_VBAT_NTC_VOLTAGE, 0, true,
                    1000);
}

void app_wlt_ui_init(void)
{
    Initialize_charge();
	Initialize_switch_ui();
    app_timer_reg_cb(app_ui_timeout_cb, &app_ui_timer_id);
	adc_charger_init();
	
}

void wlt_ui_timer_start(void)
{
   if(wlt_time_ui_init)
   	{
    	return ;
   	}
    
	app_start_timer(&timer_idx_ui_switch, "ui_switch",
						   app_ui_timer_id, 1, 0, true,
						   200);
    wlt_time_ui_init = true;
}

void wlt_ui_timer_stop(void)
{
	app_stop_timer(timer_idx_ui_switch);
	app_adc_deinit();
}


