#include "rtk_charger.h"
#include "charger_utils.h"
#include "rtl876x_pinmux.h"
#include "app_cfg.h"

struct
{
    RTK_CHARGER_STATE_CB app_state_cb;
    RTK_CHARGER_SOC_CB   app_soc_cb;
} rtk_charger_mgr;


static void rtk_charger_charge_state_cb(T_CHARGER_STATE rtk_charger_state)
{

    if ((app_cfg_const.thermistor_power_gpio_support) &&
        (app_cfg_const.thermistor_power_pinmux != 0xFF) &&
        (app_cfg_const.ntc_power_by_pad_direct == 0))
    {
        if (rtk_charger_state == STATE_CHARGER_END)
        {
            Pad_OutputControlValue(app_cfg_const.thermistor_power_pinmux, PAD_OUT_LOW);
        }
        else
        {
            Pad_OutputControlValue(app_cfg_const.thermistor_power_pinmux, PAD_OUT_HIGH);
        }
    }

    if (rtk_charger_mgr.app_state_cb)
    {
        rtk_charger_mgr.app_state_cb(rtk_charger_state);
    }
}


static void rtk_charger_soc_cb(uint8_t state_of_charge)
{
    if (rtk_charger_mgr.app_soc_cb)
    {
        rtk_charger_mgr.app_soc_cb(state_of_charge);
    }
}


void rtk_charger_init(RTK_CHARGER_STATE_CB app_state_cb, RTK_CHARGER_SOC_CB app_soc_cb)
{
    rtk_charger_mgr.app_state_cb = app_state_cb;
    rtk_charger_mgr.app_soc_cb = app_soc_cb;

    charger_api_reg_charger_state_callback(rtk_charger_charge_state_cb);
    charger_api_reg_state_of_charge_callback(rtk_charger_soc_cb);
}

extern T_CHARGER_STATE get_charge_status(void);
extern uint32_t voltage_battery;
extern bool adc_vbat_ntc_voltage_init;
T_CHARGER_STATE rtk_charger_get_charge_state(void)
{
    return get_charge_status();//charger_api_get_charger_state();
}
struct battery_cap
{
		uint16_t cap;
		uint16_t volt;
};

#define BOARD_BATTERY_CAP_MAPPINGS      \
	{0, 3100},  \
	{5, 3300},  \
	{10, 3400}, \
	{20, 3550}, \
	{30, 3650}, \
	{40, 3750}, \
	{50, 3800}, \
	{60, 3850}, \
	{70, 3900}, \
	{80, 3950}, \
	{90, 4000}, \
	{100, 4090},

#define BATTERY_CAP_TABLE_CNT		12
static const struct battery_cap battery_cap_tbl[BATTERY_CAP_TABLE_CNT] = {
    BOARD_BATTERY_CAP_MAPPINGS
};

uint16_t voltage2capacit(uint32_t volt)
{
	const struct battery_cap *bc, *bc_prev;
	uint32_t  i, cap = 0;

	extern bool get_battery_adc_ready(void);
    if(!get_battery_adc_ready())
		return 256;
	/* %0 */
	if (volt <= battery_cap_tbl[0].volt)
		return 0;

	/* %100 */
	if (volt >= battery_cap_tbl[BATTERY_CAP_TABLE_CNT - 1].volt)
		return 100;

	for (i = 1; i < BATTERY_CAP_TABLE_CNT; i++) {
		bc = &battery_cap_tbl[i];
		if (volt < bc->volt) {
			bc_prev = &battery_cap_tbl[i - 1];
			/* linear fitting */
			cap = bc_prev->cap + (((bc->cap - bc_prev->cap) *
				(volt - bc_prev->volt)*10 + 5) / (bc->volt - bc_prev->volt)) / 10;

			break;
		}
	}

	return cap;
}

uint8_t rtk_charger_get_soc(void)
{
//	APP_PRINT_TRACE1("rtk_charger_get_soc: %d", voltage2capacit(voltage_battery));

    return (uint8_t)voltage2capacit(voltage_battery);//charger_api_get_state_of_charge();
}


uint32_t rtk_charger_get_bat_vol(void)
{
    uint16_t bat_vol = 0;

   charger_utils_get_batt_volt(&bat_vol);
    return (uint32_t)bat_vol;//voltage_battery;
}


int32_t rtk_charger_get_bat_curr(void)
{
    int16_t bat_curr = 0;

    charger_utils_get_batt_curr(&bat_curr);
    return bat_curr;
}

