/*
 * cli_command.c
 *
 *  Created on: Apr 1, 2025
 *      Author: HTSANG
 */

#include "cli_command.h"
#include "ntc.h"
#include "lt8722.h"
#include "temperature_control.h"
#include "experiment_task.h"
#include "shell.h"
#include <stdlib.h>
#include <string.h>
#include "board.h"
#include "adc_monitor.h"

extern temperature_control_task_t temperature_control_task_inst ;
static temperature_control_task_t *ptemperature_control_task = &temperature_control_task_inst;

extern experiment_task_t experiment_task_inst;
static experiment_task_t *pexperiment_task = &experiment_task_inst;

static bool is_measured = false;

/*************************************************
 *                Private variable                 *
 *************************************************/


/*************************************************
 *                Command Define                 *
 *************************************************/


/*************************************************
 *                Command Define                 *
 *************************************************/

static void CMD_Clear_CLI(EmbeddedCli *cli, char *args, void *context);
static void CMD_Reset(EmbeddedCli *cli, char *args, void *context);

static void CMD_NTC_Get_Temp(EmbeddedCli *cli, char *args, void *context);
static void CMD_PWR_5V_Set(EmbeddedCli *cli, char *args, void *context);
static void CMD_PWR_5V_Get(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_Init(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_Set_Profile_Volt(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_Get_Profile_Volt(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_Profile_Register(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_Profile_Get(EmbeddedCli *cli, char *args, void *context);

static void CMD_TEC_OVR_Set_Output(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_OVR_Start(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_OVR_Stop(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_OVR_Get(EmbeddedCli *cli, char *args, void *context);

static void CMD_TEC_Man_Set_Volt(EmbeddedCli *cli, char *args, void *context);
static void CMD_TEC_Man_Set_Output(EmbeddedCli *cli, char *args, void *context);
static void CMD_HTR_Set_Profile_Duty(EmbeddedCli *cli, char *args, void *context);
static void CMD_HTR_Get_Profile_Duty(EmbeddedCli *cli, char *args, void *context);
static void CMD_Heater_Profile_Register(EmbeddedCli *cli, char *args, void *context);
static void CMD_Heater_Profile_Get(EmbeddedCli *cli, char *args, void *context);



static void CMD_Ref_Set_Temp(EmbeddedCli *cli, char *args, void *context);
static void CMD_Ref_Get_Temp(EmbeddedCli *cli, char *args, void *context);
static void CMD_Ref_Set_NTC(EmbeddedCli *cli, char *args, void *context);
static void CMD_Ref_Get_NTC(EmbeddedCli *cli, char *args, void *context);
static void CMD_Start_Auto_Mode(EmbeddedCli *cli, char *args, void *context);
static void CMD_Stop_Auto_Mode(EmbeddedCli *cli, char *args, void *context);


static void CMD_Set_Laser_Int_Current(EmbeddedCli *cli, char *args, void *context);
static void CMD_Set_Laser_Ext_Current(EmbeddedCli *cli, char *args, void *context);
static void CMD_Laser_Get_Current(EmbeddedCli *cli, char *args, void *context);
static void CMD_Int_Laser_Switch_On(EmbeddedCli *cli, char *args, void *context);
static void CMD_Ext_Laser_Switch_On(EmbeddedCli *cli, char *args, void *context);
static void CMD_Int_Laser_Switch_Off(EmbeddedCli *cli, char *args, void *context);
static void CMD_Ext_Laser_Switch_Off(EmbeddedCli *cli, char *args, void *context);

static void cmd_exp_set_profile(EmbeddedCli *cli, char *args, void *context);
static void cmd_exp_get_profile(EmbeddedCli *cli, char *args, void *context);
static void cmd_exp_start_measuring(EmbeddedCli *cli, char *args, void *context);
static void cmd_exp_ram_read(EmbeddedCli *cli, char *args, void *context);
static void cmd_exp_data_transfer(EmbeddedCli *cli, char *args, void *context);
//static void CMD_TEC_Set_Auto(EmbeddedCli *cli, char *args, void *context);
//static void CMD_TEC_Get_Auto(EmbeddedCli *cli, char *args, void *context);
//static void CMD_HTR_Set_Auto(EmbeddedCli *cli, char *args, void *context);
//static void CMD_HTR_Get_Auto(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Temp_Set_Auto(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Temp_Get_Auto(EmbeddedCli *cli, char *args, void *context);
//
//static void CMD_Sens_List(EmbeddedCli *cli, char *args, void *context);
//static void CMD_LSMSens_Get(EmbeddedCli *cli, char *args, void *context);
//static void CMD_H3LSens_Get(EmbeddedCli *cli, char *args, void *context);
//static void CMD_BMESens_Get(EmbeddedCli *cli, char *args, void *context);
//static void CMD_H250Sens_Get(EmbeddedCli *cli, char *args, void *context);
//static void CMD_K33Sens_Get(EmbeddedCli *cli, char *args, void *context);
//
//// Laser Photo
//static void CMD_Set_Laser(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Get_Current(EmbeddedCli *cli, char *args, void *context);
//static void CMD_PD_Get(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Sample_Set_PD(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Sample_Set_Rate(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Sample_Trig(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Sample_Status_Get(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Sample_Get(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Sample_Get_Char(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Sample_Get_Buf(EmbeddedCli *cli, char *args, void *context);
//static void CMD_Sample_Get_Buf_Char(EmbeddedCli *cli, char *args, void *context);

/*************************************************
 *                 Command  Array                *
 *************************************************/
// Guide: Command bindings are declared in the following order:
// { category, name, help, tokenizeArgs, context, binding }
// - category: Command group; set to NULL if grouping is not needed.
// - name: Command name (required)
// - help: Help string describing the command (required)
// - tokenizeArgs: Set to true to automatically split arguments when the command is called.
// - context: Pointer to a command-specific context; can be NULL.
// - binding: Callback function that handles the command.

static const CliCommandBinding cliStaticBindings_internal[] = {
    // Common
    { "Ultis", "help",         "Print list of all available CLI commands [Firmware: 1]", false,  NULL, CMD_Help },
    { "Ultis", "cls",          "Clear the console output screen",                        false,  NULL, CMD_Clear_CLI },
    { "Ultis", "reset",        "Perform MCU software reset",                             false,  NULL, CMD_Reset },
    // NTC
    { "NTC",   "ntc_get_temp", "Read temperature value from NTC sensor [ch: 0-7, a=all]", true,   NULL, CMD_NTC_Get_Temp },

    // Power
    { "PWR",   "pwr_5v_set",   "Turn ON/OFF 5V power supply [0:OFF / 1:ON]",             true,   NULL, CMD_PWR_5V_Set },
    { "PWR",   "pwr_5v_get",   "Read current state of 5V power supply",                  false,  NULL, CMD_PWR_5V_Get },

    // TEC
    { "TEC",   "tec_init",     "Initialize TEC driver for specified channel [0-3, a=all]", true,  NULL, CMD_TEC_Init },
    { "TEC",   "tec_profile_volt_set", "Set profile voltage for TEC channel: tec_volt [500~2500mV]", true,  NULL, CMD_TEC_Set_Profile_Volt },
    { "TEC",   "tec_profile_volt_get", "Get current profile voltage of TEC [0-3, a=all]",         true,  NULL, CMD_TEC_Get_Profile_Volt },
    { "TEC",   "tec_profile_set", "register Tec for profile: 0/1 0/1 0/1 0/1 ]", true,  NULL, CMD_TEC_Profile_Register},
    { "TEC",   "tec_profile_get", "get the tec profile ]", true,  NULL, CMD_TEC_Profile_Get},
	{ "TEC",   "tec_man_volt_set",  "Set TEC output : tec_idx C/H voltage",        true,  NULL, CMD_TEC_Man_Set_Volt },
	{ "TEC",   "tec_man_output_set",  "Set TEC output : tec_idx EN/DIS",        true,  NULL, CMD_TEC_Man_Set_Output },
	// TEC override
	{ "TEC",   "tec_ovr_set",  "Set TEC_override output : [tec_idx] [voltage]",        true,  NULL, CMD_TEC_OVR_Set_Output },
	{ "TEC",   "tec_ovr_start",  "Set TEC_override enable",        true,  NULL, CMD_TEC_OVR_Start },
	{ "TEC",   "tec_ovr_stop",  "Set TEC_override disable",        true,  NULL, CMD_TEC_OVR_Stop },
	{ "TEC",   "tec_ovr_get",  "Get TEC_override status",        true,  NULL, CMD_TEC_OVR_Get },

	// Heater
    { "Heater", "htr_profile_duty_set","Set duty cycle for heater [0-3]: [0~100%]",              true,  NULL, CMD_HTR_Set_Profile_Duty },
    { "Heater", "htr_profile_duty_get","Read current duty cycle setting of heater [0-3, a=all]", true,  NULL, CMD_HTR_Get_Profile_Duty },
    { "Heater", "htr_profile_set", "register heater for profile: 0/1 0/1 0/1 0/1 ]", true,  NULL, CMD_Heater_Profile_Register},
	{ "Heater", "htr_profile_get", "get the heater profile ]", true,  NULL, CMD_Heater_Profile_Get},
    // Reference temperature
    { "Ref",   "ref_temp_set", "Set reference temperature for control logic (°C)",       true,  NULL, CMD_Ref_Set_Temp },
    { "Ref",   "ref_temp_get", "Read current reference temperature setting",              true,  NULL, CMD_Ref_Get_Temp },
    { "Ref",   "ref_ntc_set",  "Select NTC channel used for temperature feedback [0-7]",  true,  NULL, CMD_Ref_Set_NTC },
    { "Ref",   "ref_ntc_get",  "Get currently selected NTC channel for control",          true,  NULL, CMD_Ref_Get_NTC },

    // Automatic control
    { "Auto",  "auto_control_start", "start auto control mode",      true,  NULL, CMD_Start_Auto_Mode },
    { "Auto",  "auto_control_stop", "start auto control mode",      true,  NULL, CMD_Stop_Auto_Mode },


// Laser Photo
	{ "Laser", "laser_int_set_current",    "format: laser_int_set_current [percent]",  true, NULL, CMD_Set_Laser_Int_Current },
	{ "Laser", "laser_ext_set_current",    "format: laser_ext_set_current  [percent]",  true, NULL, CMD_Set_Laser_Ext_Current },
	{ "Laser", "laser_get_current",    "format: laser_get_current [0/1]",  true, NULL, CMD_Laser_Get_Current },
	{ "Laser", "laser_int_switch_on",    "format: laser_int_switch_on [pos]",  true, NULL, CMD_Int_Laser_Switch_On },
	{ "Laser", "laser_ext_switch_on",    "format: laser_ext_switch_on [pos]",  true, NULL, CMD_Ext_Laser_Switch_On },
	{ "Laser", "laser_int_switch_off",    "format: laser_int_switch_off",  true, NULL, CMD_Int_Laser_Switch_Off },
	{ "Laser", "laser_ext_switch_off",    "format: laser_ext_switch_off",  true, NULL, CMD_Ext_Laser_Switch_Off },


	{ "Experiment", "exp_set_profile",    "format: exp_set_profile sampling_rate pos laser_percent pre_time experiment_time post_time",  true, NULL, cmd_exp_set_profile },
	{ "Experiment", "exp_get_profile",    "format: exp_get_profile",  true, NULL, cmd_exp_get_profile },
	{ "Experiment", "exp_start_measuring",    "format: exp_start_measuring",  true, NULL, cmd_exp_start_measuring },
	{ "Experiment", "exp_ram_read",    "format: exp_ram_read [address] [num_sample] [mode]",  true, NULL, cmd_exp_ram_read },
	{ "Experiment", "exp_data_transfer",    "format: exp_data_transfer",  true, NULL, cmd_exp_data_transfer },


	//	{ NULL, "get_current",  "format: get_current [int/ext]",                                   true, NULL, CMD_Get_Current },
//	    { NULL, "pd_get",       "format: pd_get [pd_index]",                                       true, NULL, CMD_PD_Get },
//	    { NULL, "sp_set_pd",    "format: sp_set_pd [photo_index]",                                 true, NULL, CMD_Sample_Set_PD },
//	    { NULL, "sp_set_rate",  "format: sp_set_rate [sampling_rate] [num_samples]",               true, NULL, CMD_Sample_Set_Rate },
//	    { NULL, "sp_trig",      "format: sp_trig",                                                 true, NULL, CMD_Sample_Trig },
//	    { NULL, "sp_status",    "format: sp_status",                                               true, NULL, CMD_Sample_Status_Get },

	//    { "Auto",  "auto_tec_set", "Enable/disable TECs for auto control [tecX_en 0/1]",      true,  NULL, CMD_TEC_Set_Auto },
//    { "Auto",  "auto_tec_get", "List enabled TEC channels for auto control [0-3, a]",     true,  NULL, CMD_TEC_Get_Auto },
//    { "Auto",  "auto_htr_set", "Enable/disable heaters for auto control [htrX_en 0/1]",   true,  NULL, CMD_HTR_Set_Auto },
//    { "Auto",  "auto_htr_get", "List enabled heater channels for auto control [0-3, a]",  true,  NULL, CMD_HTR_Get_Auto },
//    { "Auto",  "auto_temp_set","Enable/disable full temperature control routine [0/1]",   true,  NULL, CMD_Temp_Set_Auto },
//    { "Auto",  "auto_temp_get","Read current state of temperature control routine",       true,  NULL, CMD_Temp_Get_Auto },


//    { "TEC",   "tec_dir_get",  "Read direction setting of TEC [0-3, a=all]",              true,  NULL, CMD_TEC_Get_Dir },
//
//    // Sensors
//    { "Sensor","sens_list",    "List all available connected sensors",                    true,  NULL, CMD_Sens_List },
//    { "Sensor","lsm_sens_get", "Read LSM6DSOX data [0=all, 1=acc, 2=gyro]",               true,  NULL, CMD_LSMSens_Get },
//    { "Sensor","h3l_sens_get", "Read high-g acceleration data from H3LIS331 sensor",      true,  NULL, CMD_H3LSens_Get },
//    { "Sensor","bme_sens_get", "Read BME280 sensor [0=all, 1=temp, 2=RH, 3=pressure]",     true,  NULL, CMD_BMESens_Get },
//    { "Sensor","h250_sens_get","Read CO₂ concentration from H-250(G)-3V sensor",          false, NULL, CMD_H250Sens_Get },
//    { "Sensor","k33_sens_get", "Read CO₂ concentration from K33 ICB-F 10% sensor",        false, NULL, CMD_K33Sens_Get },
//
//	// Laser Photo
//    { NULL, "set_laser",    "format: set_laser [int/ext] [laser_index] [dac_val]",             true, NULL, CMD_Set_Laser },
//    { NULL, "get_current",  "format: get_current [int/ext]",                                   true, NULL, CMD_Get_Current },
//    { NULL, "pd_get",       "format: pd_get [pd_index]",                                       true, NULL, CMD_PD_Get },
//    { NULL, "sp_set_pd",    "format: sp_set_pd [photo_index]",                                 true, NULL, CMD_Sample_Set_PD },
//    { NULL, "sp_set_rate",  "format: sp_set_rate [sampling_rate] [num_samples]",               true, NULL, CMD_Sample_Set_Rate },
//    { NULL, "sp_trig",      "format: sp_trig",                                                 true, NULL, CMD_Sample_Trig },
//    { NULL, "sp_status",    "format: sp_status",                                               true, NULL, CMD_Sample_Status_Get },
//    { NULL, "sp_get",       "format: sp_get [num_samples]",                                    true, NULL, CMD_Sample_Get },
//    { NULL, "sp_get_c",     "format: sp_get_c [num_samples]",                                  true, NULL, CMD_Sample_Get_Char },
//    { NULL, "sp_get_buf",   "format: sp_get_buf",                                              true, NULL, CMD_Sample_Get_Buf },
//    { NULL, "sp_get_buf_c", "format: sp_get_buf_c",                                            true, NULL, CMD_Sample_Get_Buf_Char },
};

/*************************************************
 *             Command List Function             *
 *************************************************/


static void CMD_Clear_CLI(EmbeddedCli *cli, char *args, void *context) {
    char buffer[10];
    snprintf(buffer, sizeof(buffer), "\33[2J");
    embeddedCliPrint(cli, buffer);
}

static void CMD_Reset(EmbeddedCli *cli, char *args, void *context) {
	NVIC_SystemReset();
    embeddedCliPrint(cli, "");
}

static void CMD_NTC_Get_Temp(EmbeddedCli *cli, char *args, void *context) {
    // TODO: Implement NTC temperature get logic
	uint32_t channel = 0;
	int16_t temp = 0;
	uint8_t tokenCount = embeddedCliGetTokenCount(args);
		if (tokenCount != 1)
		{
			cli_printf(cli, "command require one argument\r\n");
			return;
		}
	const char *arg1 = embeddedCliGetToken(args, 1);
	if (*arg1 == 'a')
	{

		for (uint8_t channel = 0; channel < 8; channel++)
		{
			temp = temperature_monitor_get_ntc_temperature(channel);
			if (temp == (int16_t)0x8000)	cli_printf(cli, "ntc[%d] = ERROR\r\n",channel);
			else cli_printf(cli, "ntc[%d] = %d C\r\n",channel,temp);
		}
	}
	else if ((*arg1 >= '0') &&(*arg1 <=  '7'))
	{
		channel = atoi(arg1);
		temp = temperature_monitor_get_ntc_temperature(channel);
		if (temp == (int16_t)0x8000)	cli_printf(cli, "ntc[%d] = ERROR\r\n",channel);
		else cli_printf(cli, "ntc[%d] = %d C\r\n",channel,temp);
	}
	else cli_printf(cli, "Wrong arguments\r\n");

}

static void CMD_PWR_5V_Set(EmbeddedCli *cli, char *args, void *context) {
	uint32_t tokenCount = embeddedCliGetTokenCount(args);
	if (tokenCount != 1)
	{
		cli_printf(cli, "command require one argument\r\n");
		return;
	}
	if (!temperature_control_is_in_man_state(ptemperature_control_task))
	{
		cli_printf(cli, "System is in auto mode, change to manual mode before controlling power\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);
	uint32_t status = atoi(arg1);

	temperature_control_power_control(ptemperature_control_task , status);


	cli_printf(cli,"OK\r\n");
}

static void CMD_PWR_5V_Get(EmbeddedCli *cli, char *args, void *context) {
	if (temperature_control_is_powered_on(ptemperature_control_task)) {
		cli_printf(cli, "OK\r\n");
	}
	else {
		cli_printf(cli, "ERROR\r\n");
	}
}

static void CMD_TEC_Init(EmbeddedCli *cli, char *args, void *context) {
    // TODO: Implement TEC initialization logic
	uint32_t channel;
	uint32_t tokenCount = embeddedCliGetTokenCount(args);
	if (tokenCount != 1)
	{
		cli_printf(cli, "command require one argument\r\n");
		return;
	}
	if (!temperature_control_is_in_man_state(ptemperature_control_task))
	{
		cli_printf(cli, "can not init TEC manually in AUTO mode, please change to MANUAL mode\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);

	uint32_t result = 0;

	if (*arg1 == 'a' ) {
		/* Init TEC 0 -> 3 */
		for (channel = 0; channel < 4; channel++) {
			result = temperature_control_tec_init(ptemperature_control_task, channel);
			// if init is success
			if (!result) {
				temperature_control_tec_enable_output(ptemperature_control_task,channel,0); //disable output
				cli_printf(cli, "tec[%d] inited\r\n",channel);
			}
			else
				cli_printf(cli, "tec[%d] init failed\r\n",channel);
		}
	}
	else if ((*arg1 >= '0') && (*arg1 <= '3')) {
		channel = atoi(arg1);
		result = temperature_control_tec_init(ptemperature_control_task, channel);
		if (!result) {
			temperature_control_tec_enable_output(ptemperature_control_task,channel,0); //disable output
			cli_printf(cli, "tec[%d] inited\r\n",channel);
		}
		else
			cli_printf(cli, "tec[%d] init failed\r\n",channel);

	}

}

static void CMD_TEC_Set_Profile_Volt(EmbeddedCli *cli, char *args, void *context) {
    // TODO: Implement TEC voltage set logic
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 1)
	{
		cli_printf(cli, "require 1 voltage value for 4 tecs\r\n");
		return;
	}

	const char *arg1 = embeddedCliGetToken(args, 1);
	uint16_t volt;
	volt = atoi(arg1);


		if ((volt < 500) || (volt > 3000))
		{
			cli_printf(cli, "tec[%d] voltage is out of range (500mV-3000mV)\r\n");
			return;
		}

	temperature_control_profile_tec_voltage_set(ptemperature_control_task,volt);
	cli_printf(cli, "OK\r\n");
}

static void CMD_TEC_Get_Profile_Volt(EmbeddedCli *cli, char *args, void *context) {
    // TODO: Implement TEC voltage get logic


	uint16_t tec_volt;
	tec_volt = temperature_control_profile_tec_voltage_get(ptemperature_control_task);
	cli_printf(cli, "profile setpoint of TEC = %d mV\r\n",tec_volt);
	return;

}
static void CMD_TEC_Profile_Register(EmbeddedCli *cli, char *args, void *context)
{
	uint8_t tec_ena[4];
	uint32_t i;
	uint32_t tokenCount = embeddedCliGetTokenCount(args);
	if (tokenCount != 4)
	{
		cli_printf(cli, "require 4 value for 4 tecs (0/1)\r\n");
		return;
	}

	uint8_t tec_ovr = temperature_profile_tec_ovr_get(ptemperature_control_task);
	for (i=0;i<4;i++)
	{
		tec_ena[i] = atoi( embeddedCliGetToken(args, i+1));
		if (tec_ena[i] >  1) {
			cli_printf(cli, "argument %d need to be in range 0/1 \r\n",i);
			return;
		}
		if (tec_ena[i] == 0) temperature_control_profile_tec_unregister(ptemperature_control_task,i);
		else
		{
			if(i == tec_ovr) cli_printf(cli, "tec[%d] registered in ovr mode\r\n",i);
			else
			{
				temperature_control_profile_tec_register(ptemperature_control_task, i);
				cli_printf(cli, "tec[%d] registered\r\n",i);
			}
		}
	}
	cli_printf(cli, "OK\r\n");
}

static void CMD_TEC_Profile_Get(EmbeddedCli *cli, char *args, void *context)
{
	uint8_t tec_profile = temperature_control_profile_tec_get(ptemperature_control_task);
	for (uint32_t i=0;i<4;i++)
	{
		if (tec_profile & (1<<i)) cli_printf(cli, "tec[%d] registered\r\n",i);
		else cli_printf(cli, "tec[%d] unregistered\r\n",i);
	}
}

//manuallly set the Tec voltage fỏmat: xxx tec_id tec_dir tec_volt
static void CMD_TEC_Man_Set_Volt(EmbeddedCli *cli, char *args, void *context) {
    // TODO: Implement TEC direction set logic
	if (!temperature_control_is_in_man_state(ptemperature_control_task))
	{
		cli_printf(cli, "Not in manual mode, can not set\r\n");
		return;
	}
	uint32_t tokenCount = embeddedCliGetTokenCount(args);
	if (tokenCount != 3)
	{
		cli_printf(cli, "format: command tec_id tec_dir volt_mV\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);
	uint32_t tec_id = atoi(arg1);
	if (tec_id  > 3)
	{
		cli_printf(cli, "arg1: tec id out of range (0-3)\r\n");
				return;
	}
	const char *arg2 = embeddedCliGetToken(args, 2);
	uint32_t tec_dir = atoi(arg2);

	if (tec_dir > 1)
	{
		cli_printf(cli, "arg2: dir is COOL[0] or HEAT[1]\r\n");
		return;
	}
	//tec_dir = TEC_HEAT;  tec_dir = TEC_COOL;

	const char *arg3 = embeddedCliGetToken(args, 3);
	uint16_t tec_volt_mV = atoi(arg3);
	if (tec_volt_mV > 3000)
	{
		cli_printf(cli, "arg3: voltage is out of range (0-3000mV)\r\n");
		return;
	}

	uint32_t ret;
	ret = temperature_control_tec_manual_set_output( ptemperature_control_task,tec_id, tec_dir, tec_volt_mV);
	if (ret)
	{
		cli_printf(cli, "ERROR\r\n");
		return;
	}
	cli_printf(cli, "OK\r\n");
}

//manuallly enable/disable tec output: cmd tec_id 0/1
static void CMD_TEC_Man_Set_Output(EmbeddedCli *cli, char *args, void *context)
{
	if (!temperature_control_is_in_man_state(ptemperature_control_task))
	{
		cli_printf(cli, "Not in manual mode, can not set\r\n");
		return;
	}
	uint32_t tokenCount = embeddedCliGetTokenCount(args);
	if (tokenCount != 2)
	{
		cli_printf(cli, "format: command tec_id 0/1\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);
	uint32_t tec_id = atoi(arg1);
	if (tec_id  > 3)
	{
		cli_printf(cli, "arg1: tec id out of range (0-3)\r\n");
				return;
	}
	const char *arg2 = embeddedCliGetToken(args, 2);
	uint32_t tec_out_ena = atoi(arg2);
	uint32_t ret = temperature_control_tec_enable_output(ptemperature_control_task,tec_id,tec_out_ena);
	if (ret)
	{
		cli_printf(cli, "ERROR\r\n");
		return;
	}
	cli_printf(cli, "OK\r\n");
}


static void CMD_TEC_OVR_Set_Output(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);
	if (tokenCount != 2)
	{
		cli_printf(cli, "format: tec_ovr_set [id] [volt_mV]\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);
	uint8_t tec_profile = temperature_control_profile_tec_get(ptemperature_control_task);
	uint8_t tec_id;
	if(*arg1 == 'o')
	{
		tec_id = 4;
		cli_printf(cli, "tec ovr mode is off\r\n");
	}
	else if ((*arg1 >= '0') && (*arg1 <= '3'))
	{
		tec_id = atoi(arg1);
		if (tec_profile & (1<<tec_id))
		{
			cli_printf(cli, "tec[%d] registered in auto mode\r\n",tec_id);
			return;
		}
	}


	const char *arg2 = embeddedCliGetToken(args, 2);
	uint32_t mvolt = atoi(arg2);

	if ((mvolt < 500) || (mvolt > 3000))
	{
		cli_printf(cli, "tec[%d] voltage is out of range (500mV-3000mV)\r\n");
		return;
	}


	temperature_profile_tec_ovr_register(ptemperature_control_task, tec_id);
	temperature_profile_tec_ovr_voltage_set(ptemperature_control_task, mvolt);
	cli_printf(cli, "OK\r\n");
}


static void CMD_TEC_OVR_Start(EmbeddedCli *cli, char *args, void *context)
{
	temperature_profile_tec_ovr_enable(ptemperature_control_task);
	cli_printf(cli, "OK\r\n");
}
static void CMD_TEC_OVR_Stop(EmbeddedCli *cli, char *args, void *context)
{
	temperature_profile_tec_ovr_disable(ptemperature_control_task);
	cli_printf(cli, "OK\r\n");
}

static void CMD_TEC_OVR_Get(EmbeddedCli *cli, char *args, void *context)
{
	uint8_t tec_idx = temperature_profile_tec_ovr_get(ptemperature_control_task);
	uint16_t volt_mv = temperature_profile_tec_ovr_get_voltage(ptemperature_control_task);
	if(tec_idx == 4)
	{
		cli_printf(cli, "tec ovr mode is off\r\n");
		return;
	}
	cli_printf(cli, "tec[%d] registered in ovr mode with %dmV\r\n",tec_idx, volt_mv);
}



static void CMD_HTR_Set_Profile_Duty(EmbeddedCli *cli, char *args, void *context) {
    // TODO: Implement Heater duty cycle set logic
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 1)
	{
		cli_printf(cli, "require 1 duty value for 4 heater\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);
	uint8_t duty = atoi(arg1);
	if (duty > 100)
	{
		cli_printf(cli, "require duty in range of 0-100\r\n");
		return;
	}

	temperature_control_profile_heater_duty_set(ptemperature_control_task,duty);
	cli_printf(cli, "OK\r\n");
	//TODO: post profile change signal
}

static void CMD_HTR_Get_Profile_Duty(EmbeddedCli *cli, char *args, void *context) {
    // TODO: Implement Heater duty cycle get logic
	uint8_t profile_duty = temperature_control_profile_heater_duty_get(ptemperature_control_task);
	cli_printf(cli, "heater profile duty = %d\r\n",profile_duty);
}
static void CMD_Heater_Profile_Register(EmbeddedCli *cli, char *args, void *context)
{
	uint8_t heater_ena[4];
	uint32_t i;
	uint32_t tokenCount = embeddedCliGetTokenCount(args);
	if (tokenCount != 4)
	{
		cli_printf(cli, "require 4 value for 4 heater (0/1)\r\n");
		return;
	}
	for (i=0;i<4;i++)
	{
		heater_ena[i] = atoi( embeddedCliGetToken(args, i+1));
		if (heater_ena[i] >  1) {
			cli_printf(cli, "argument %d need to be in range 0/1 \r\n",i);
			return;
		}
		if (heater_ena[i] == 0) temperature_control_profile_heater_unregister(ptemperature_control_task,i);
		else temperature_control_profile_heater_register(ptemperature_control_task, i);
	}
	cli_printf(cli, "OK\r\n");
}

static void CMD_Heater_Profile_Get(EmbeddedCli *cli, char *args, void *context)
{
	uint8_t heater_profile = temperature_control_profile_heater_profile_get(ptemperature_control_task);
	for (uint32_t i=0;i<4;i++)
	{
		if (heater_profile & (1<<i)) cli_printf(cli, "heater[%d] registered\r\n");
		else cli_printf(cli, "heater[%d] unregistered\r\n");
	}
}


static void CMD_Ref_Set_Temp(EmbeddedCli *cli, char *args, void *context) {
    // TODO: Implement reference temperature set logic
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 1)
	{
		cli_printf(cli, "require 1 temperature (251 mean 25.1)\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);
	int16_t setpoint = atoi(arg1);
	temperature_control_profile_setpoint_set(ptemperature_control_task,setpoint);
	cli_printf(cli, "OK\r\n");

}

static void CMD_Ref_Get_Temp(EmbeddedCli *cli, char *args, void *context) {
    // TODO: Implement reference temperature get logic

	int16_t setpoint = temperature_control_profile_setpoint_get(ptemperature_control_task);
	cli_printf(cli, "Reference Temperature: %.2f *C\r\n", (float)setpoint/10);
}

static void CMD_Ref_Set_NTC(EmbeddedCli *cli, char *args, void *context) {
    // TODO: Implement reference NTC set logic
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 1)
	{
		cli_printf(cli, "require 1 NTC index\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);
	uint32_t NTC_Ref = atoi(arg1);
	if (NTC_Ref > 7) {
		cli_printf(cli, "NTC index out of range (0-7)\r\n");
		return;
	}
	temperature_control_profile_ntc_register(ptemperature_control_task,NTC_Ref);
	cli_printf(cli, "OK\r\n");
}

static void CMD_Ref_Get_NTC(EmbeddedCli *cli, char *args, void *context) {
    // TODO: Implement reference NTC get logic
	uint8_t NTC_Ref = 0;
	NTC_Ref = temperature_control_profile_ntc_get( ptemperature_control_task);
	cli_printf(cli, "NTC Ref is %d\r\n", NTC_Ref);
}
static void CMD_Start_Auto_Mode(EmbeddedCli *cli, char *args, void *context)
{
	temperature_control_auto_mode_set(ptemperature_control_task);
	cli_printf(cli, "OK\r\n");
}
static void CMD_Stop_Auto_Mode(EmbeddedCli *cli, char *args, void *context)
{
	temperature_control_man_mode_set(ptemperature_control_task);
	cli_printf(cli, "OK\r\n");
}

/*
 * format: laser_int_set_current  percent
 */
static void CMD_Set_Laser_Int_Current(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 1)
	{
		cli_printf(cli, "format: laser_set_current percent \r\n");
		return;
	}

	const char *arg1 = embeddedCliGetToken(args, 1);
	int16_t percent = atoi(arg1);
	if (percent > 100)

		{
			cli_printf(cli, "argument 1 out of range,(0-100)\r\n");
			return;
		}
	experiment_task_laser_set_current(pexperiment_task, 0, percent);
	cli_printf(cli, "OK\r\n");
}

static void CMD_Set_Laser_Ext_Current(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 1)
	{
		cli_printf(cli, "format: laser_set_current 0/1 percent (0 for internal, 1 for external\r\n");
		return;
	}

	const char *arg1 = embeddedCliGetToken(args, 1);
	int16_t percent = atoi(arg1);
	if (percent > 100)

		{
			cli_printf(cli, "argument 1 out of range,(0-100)\r\n");
			return;
		}
	experiment_task_laser_set_current(pexperiment_task, 1, percent);
	cli_printf(cli, "OK\r\n");
}


static void CMD_Laser_Get_Current(EmbeddedCli *cli, char *args, void *context)
{
	uint16_t current = 0;
	uint8_t tokenCount = embeddedCliGetTokenCount(args);
	if (tokenCount != 1)
	{
		cli_printf(cli, "command require one argument\r\n");
		return;
	}
	const char *arg1 = embeddedCliGetToken(args, 1);
	if (*arg1 == 'a')
	{

		for (uint8_t channel = 0; channel < 2; channel++)
		{
			current = laser_monitor_get_laser_current(channel);
			cli_printf(cli, "laser_current[%s]: %d mA\r\n", (channel < 1) ? "int":"ext", current);
		}
	}
	else if ((*arg1 >= '0') &&(*arg1 <=  '1'))
	{
		uint8_t channel = atoi(arg1);
		current = laser_monitor_get_laser_current(channel);
		cli_printf(cli, "laser_current[%s]: %d mA\r\n", (channel < 1) ? "int":"ext", current);
	}
	else cli_printf(cli, "Wrong arguments\r\n");

}


static void CMD_Int_Laser_Switch_On(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 1)
	{
		cli_printf(cli, "format: laser_int_switch_on [pos]\r\n");
		return;
	}

	const char *arg1 = embeddedCliGetToken(args, 1);
	int32_t laser_idx = atoi(arg1);
	if ((laser_idx > INTERNAL_CHAIN_CHANNEL_NUM) || (laser_idx < 1))
	{
		cli_printf(cli, "argument 1 out of range,(1-36)\r\n");
		return;
	}
	experiment_task_int_laser_switchon(pexperiment_task,  laser_idx);
	cli_printf(cli, "OK\r\n");
}



static void CMD_Ext_Laser_Switch_On(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 1)
	{
		cli_printf(cli, "format: laser_ext_switch_on [pos]\r\n");
		return;
	}

	const char *arg1 = embeddedCliGetToken(args, 1);
	int32_t laser_idx = atoi(arg1);
	if ((laser_idx > EXTERNAL_CHAIN_CHANNEL_NUM)||(laser_idx < 1))
	{
		cli_printf(cli, "argument 1 out of range,(1-8)\r\n");
		return;
	}
	experiment_task_ext_laser_switchon(pexperiment_task,  laser_idx);
	cli_printf(cli, "OK\r\n");
}

static void CMD_Int_Laser_Switch_Off(EmbeddedCli *cli, char *args, void *context)
{
	experiment_task_int_laser_switchoff(pexperiment_task);
	cli_printf(cli, "OK\r\n");
}
static void CMD_Ext_Laser_Switch_Off(EmbeddedCli *cli, char *args, void *context){
	experiment_task_ext_laser_switchoff(pexperiment_task);
	cli_printf(cli, "OK\r\n");
}

/*
 * format: exp_set_profile sampling_rate pos laser_percent pre_time experiment_time post_time
 * sampling rate in KSample
 * time in us unit
 */

static void cmd_exp_set_profile(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 6)
	{
		cli_printf(cli, "format: exp_set_profile [sampling_rate pos] [laser_percent] [pre_time] [experiment_time] [post_time]\r\n");
		return;
	}
	uint32_t sampling_rate = atoi(embeddedCliGetToken(args, 1));
	if ((sampling_rate < 1000) || (sampling_rate > 800000))
	{
		cli_printf(cli, "sampling rate out of range (1K-800K)\r\n");
		return;
	}
	sampling_rate /= 1000;

	uint32_t pos = atoi(embeddedCliGetToken(args, 2));
	if ((pos == 0) || (pos > 36))
	{
		cli_printf(cli, "pos out of range (1-36)\r\n");
		return;
	}

	uint32_t percent = atoi(embeddedCliGetToken(args, 3));
	if (percent > 100)
	{
		cli_printf(cli, "percent out of range (0-100)\r\n");
		return;
	}

	uint32_t pre_time = atoi(embeddedCliGetToken(args, 4));
	if (pre_time == 0)
	{
		cli_printf(cli, "pre_time should be larger than 0\r\n");
		return;
	}
	pre_time *= 1000;

	uint32_t sample_time = atoi(embeddedCliGetToken(args, 5));
	if (sample_time == 0)
	{
		cli_printf(cli, "sample time should be larger than 0\r\n");
		return;
	}
	sample_time *= 1000;

	uint32_t post_time = atoi(embeddedCliGetToken(args, 6));
	if (post_time == 0)
	{
		cli_printf(cli, "post_time should be larger than 0\r\n");
		return;
	}
	post_time *= 1000;

	uint32_t num_sample = ((pre_time + sample_time + post_time) * sampling_rate )/1000000;
	if (num_sample > 2048)	//larrger than 4MB
	{
		cli_printf(cli, "total sample must be less than 2048K \r\n");
		return;
	}

	experiment_profile_t profile;
	profile.sampling_rate = sampling_rate;		// kHz
	profile.pos = pos;
	profile.laser_percent = percent;
	profile.pre_time = pre_time;				// us
	profile.experiment_time = sample_time;		// us
	profile.post_time = post_time;				// us
	profile.num_sample = num_sample;			// kSample
	profile.period = 1000000 / sampling_rate;	// ns
	if(!experiment_task_set_profile(pexperiment_task,&profile)) cli_printf(cli, "OK\r\n");
	else cli_printf(cli, "ERROR\r\n");
}

static void cmd_exp_get_profile(EmbeddedCli *cli, char *args, void *context)
{
	experiment_profile_t profile;
	experiment_task_get_profile(pexperiment_task, &profile);
	cli_printf(cli, "sampling_rate: %d Hz\r\n"
					"position     : %d\r\n"
					"percent      : %d %\r\n"
					"pre_time     : %d ms\r\n"
					"sample_time  : %d ms\r\n"
					"post_time    : %d ms\r\n"
					"num_sample   : %d kSample\r\n",
					profile.sampling_rate*1000,
					profile.pos,
					profile.laser_percent,
					profile.pre_time/1000,
					profile.experiment_time/1000,
					profile.post_time/1000,
					profile.num_sample);

}
static void cmd_exp_start_measuring(EmbeddedCli *cli, char *args, void *context)
{
	if (experiment_start_measuring(pexperiment_task))
		cli_printf(cli, "Wrong profile, please check \r\n");
	else
	{
		is_measured = true;
	}
}

static void cmd_exp_ram_read(EmbeddedCli *cli, char *args, void *context)
{
	uint32_t tokenCount = embeddedCliGetTokenCount(args);

	if (tokenCount != 3)
	{
		cli_printf(cli, "format: exp_ram_read [address] [num_sample] [mode]\r\n");
		return;
	}

	if(	(pexperiment_task->sub_state != NO_SUBSTATE)&&
		(pexperiment_task->sub_state != S_AQUI_ERROR)&&
		(pexperiment_task->sub_state != S_AQUI_TIMEOUT))
	{
		cli_printf(cli, "EXP is in sampling process!\r\n");
		return;
	}

	// 32Mb = 4096kB = 2048kW = 2097152W -> addr < (2097152 - 1)W
	uint32_t start_address = atoi(embeddedCliGetToken(args, 1));		// calculated by halfword
	if (start_address > 2097151)
	{
		cli_printf(cli, "address out of range (0-2097151 nsamples)\r\n");
		return;
	}

	// 32Mb = 4096kB = 2048kW = 2097152W -> num_sample <= (2097152 - addr) sample
	uint32_t num_data = atoi(embeddedCliGetToken(args, 2));		// calculated by halfword
	if ((num_data > 2097152 - start_address)||(num_data < 512))
	{
		cli_printf(cli, "num samples out of range (512-%d samples)\r\n", 2097152 - start_address);
		return;
	}

	uint8_t mode = atoi(embeddedCliGetToken(args, 3));		// calculated by halfword
	if (mode > 1)
	{
		cli_printf(cli, "mode format is 0: print by ascii, 1: print by binary\r\n");
		return;
	}

	experiment_task_get_ram_data(pexperiment_task, start_address, num_data, mode);

}

static void cmd_exp_data_transfer(EmbeddedCli *cli, char *args, void *context)
{
	if(is_measured) experiment_task_data_transfer(pexperiment_task);
	else cli_printf(cli, "please use 'start measure' first!\r\n");
	return;

}
//static void CMD_TEC_Set_Auto(EmbeddedCli *cli, char *args, void *context) {
//    // TODO: Implement TEC auto mode set logic
//	const char *arg1 = embeddedCliGetToken(args, 1);
//	const char *arg2 = embeddedCliGetToken(args, 2);
//	const char *arg3 = embeddedCliGetToken(args, 3);
//	const char *arg4 = embeddedCliGetToken(args, 4);
//	uint8_t tec_0_en = atoi(arg1);
//	uint8_t tec_1_en = atoi(arg2);
//	uint8_t tec_2_en = atoi(arg3);
//	uint8_t tec_3_en = atoi(arg4);
//	temperature_set_tec_auto(tec_0_en, tec_1_en, tec_2_en, tec_3_en);
//	char buffer[60];
//	if (tec_0_en) {
//		snprintf(buffer, sizeof(buffer), "TEC 0 is ena");
//		embeddedCliPrint(cli, buffer);
//	}
//	if (tec_1_en) {
//		snprintf(buffer, sizeof(buffer), "TEC 1 is ena");
//		embeddedCliPrint(cli, buffer);
//	}
//	if (tec_2_en) {
//		snprintf(buffer, sizeof(buffer), "TEC 2 is ena");
//		embeddedCliPrint(cli, buffer);
//	}
//	if (tec_3_en) {
//		snprintf(buffer, sizeof(buffer), "TEC 3 is ena");
//		embeddedCliPrint(cli, buffer);
//	}
//	embeddedCliPrint(cli, "");
//}
//
//static void CMD_TEC_Get_Auto(EmbeddedCli *cli, char *args, void *context) {
//    // TODO: Implement TEC auto mode get logic
//	const char *arg1 = embeddedCliGetToken(args, 1);
//	uint8_t tec_en[4];
//	temperature_get_tec_auto(&tec_en[0], &tec_en[1], &tec_en[2], &tec_en[3]);
//	char buffer[60];
//	if (*arg1 == 'a' || *arg1 == '\0') {
//		for (uint8_t channel = 0; channel < 4; channel++) {
//			if (tec_en[channel])
//				snprintf(buffer, sizeof(buffer), "TEC %d is ena", channel);
//			else
//				snprintf(buffer, sizeof(buffer), "TEC %d is dis", channel);
//			embeddedCliPrint(cli, buffer);
//		}
//	}
//	else if (*arg1 == '0' || *arg1 == '1' || *arg1 == '2' || *arg1 == '3') {
//		int channel = atoi(arg1);
//		if (tec_en[channel])
//			snprintf(buffer, sizeof(buffer), "TEC %d is ena", channel);
//		else
//			snprintf(buffer, sizeof(buffer), "TEC %d is dis", channel);
//		embeddedCliPrint(cli, buffer);
//	}
//	embeddedCliPrint(cli, "");
//}
//
//static void CMD_HTR_Set_Auto(EmbeddedCli *cli, char *args, void *context) {
//    // TODO: Implement Heater auto mode set logic
//	const char *arg1 = embeddedCliGetToken(args, 1);
//	const char *arg2 = embeddedCliGetToken(args, 2);
//	const char *arg3 = embeddedCliGetToken(args, 3);
//	const char *arg4 = embeddedCliGetToken(args, 4);
//	uint8_t htr_en[4] = {atoi(arg1), atoi(arg2), atoi(arg3), atoi(arg4)};
//	temperature_set_heater_auto(htr_en[0], htr_en[1], htr_en[2], htr_en[3]);
//	char buffer[60];
//	for (uint8_t channel = 0; channel < 4; channel++) {
//		if (htr_en[channel]) {
//			snprintf(buffer, sizeof(buffer), "Heater %d is ena", channel);
//			embeddedCliPrint(cli, buffer);
//		}
//	}
//	embeddedCliPrint(cli, "");
//}
//
//static void CMD_HTR_Get_Auto(EmbeddedCli *cli, char *args, void *context) {
//    // TODO: Implement Heater auto mode get logic
//	const char *arg1 = embeddedCliGetToken(args, 1);
//	uint8_t htr_en[4];
//	temperature_get_heater_auto(&htr_en[0], &htr_en[1], &htr_en[2], &htr_en[3]);
//	char buffer[60];
//	if (*arg1 == 'a' || *arg1 == '\0') {
//		for (uint8_t channel = 0; channel < 4; channel++) {
//			if (htr_en[channel])
//				snprintf(buffer, sizeof(buffer), "Heater %d is ena", channel);
//			else
//				snprintf(buffer, sizeof(buffer), "Heater %d is dis", channel);
//			embeddedCliPrint(cli, buffer);
//		}
//	}
//	else if (*arg1 == '0' || *arg1 == '1' || *arg1 == '2' || *arg1 == '3') {
//		int channel = atoi(arg1);
//		if (htr_en[channel])
//			snprintf(buffer, sizeof(buffer), "Heater %d is ena", channel);
//		else
//			snprintf(buffer, sizeof(buffer), "Heater %d is dis", channel);
//		embeddedCliPrint(cli, buffer);
//	}
//	embeddedCliPrint(cli, "");
//}
//
//static void CMD_Temp_Set_Auto(EmbeddedCli *cli, char *args, void *context) {
//    // TODO: Implement auto temperature set logic
//	const char *arg1 = embeddedCliGetToken(args, 1);
//	uint8_t Temp_auto = atoi(arg1)? 1: 0;
//	temperature_set_auto_ctrl(Temp_auto);
//	if (Temp_auto)
//		embeddedCliPrint(cli, "Temp is auto ctrl");
//	else
//		embeddedCliPrint(cli, "Temp isn't auto ctrl");
//	embeddedCliPrint(cli, "");
//}
//
//static void CMD_Temp_Get_Auto(EmbeddedCli *cli, char *args, void *context) {
//    // TODO: Implement auto temperature get logic
//	uint8_t Temp_auto = 0;
//	temperature_get_auto_ctrl(&Temp_auto);
//	if (Temp_auto)
//		embeddedCliPrint(cli, "Temp is auto ctrl");
//	else
//		embeddedCliPrint(cli, "Temp isn't auto ctrl");
//	embeddedCliPrint(cli, "");
//}
//
//
//static void CMD_Sens_List(EmbeddedCli *cli, char *args, void *context) {
//	// TODO:
//	char buffer[100];
//	NTC_get_temperature(NTC_Temperature);
//	int16_t temp;
//	for (uint8_t channel = 0; channel < 8; channel++) {
//		temp = NTC_Temperature[channel];
//		if (temp != 0x7FFF) {
//			Sensor_list.ntc = 1;
//			break;
//		}
//	}
//	Sensor_I2C_Init();
//	strcpy(buffer, "sensor:");
//	if (Sensor_list.ntc) {
//		strcat(buffer, "ntc,");
//	}
//	if (Sensor_list.lsm) {
//		strcat(buffer, "lsm,");
//	}
//	if (Sensor_list.bmp) {
//		strcat(buffer, "bmp,");
//	}
//	if (Sensor_list.bme) {
//		strcat(buffer, "bme,");
//	}
//	if (Sensor_list.h3l) {
//		strcat(buffer, "h3l,");
//	}
//	if (Sensor_list.h250) {
//		strcat(buffer, "h250,");
//	}
//	if (Sensor_list.k33) {
//		strcat(buffer, "k33,");
//	}
//	if (Sensor_list.sfc) {
//		strcat(buffer, "sfc,");
//	}
//
//	size_t len = strlen(buffer);
//	if (len >= 1 && buffer[len - 1] == ',') {
//	    buffer[len - 1] = '\0';
//	}
//
//	embeddedCliPrint(cli, buffer);
//	embeddedCliPrint(cli, "");
//}
//
//static void CMD_LSMSens_Get(EmbeddedCli *cli, char *args, void *context) {
//    // TODO: Implement LSM sensor get logic
//	LSM6DSOX_Read_Data(&LSM6DSOX_Data);
//    char buffer[80];
//
//    const char *arg1 = embeddedCliGetToken(args, 1);
//	if (*arg1 == 'a' || *arg1 == '\0')
//		snprintf(buffer, sizeof(buffer), "accel:%d %d %d(g),gyro:%d %d %d(dps)",
//								LSM6DSOX_Data.Accel.x, LSM6DSOX_Data.Accel.y, LSM6DSOX_Data.Accel.z,
//								LSM6DSOX_Data.Gyro.x, LSM6DSOX_Data.Gyro.y, LSM6DSOX_Data.Gyro.z);
//	else if (*arg1 == '0')
//		snprintf(buffer, sizeof(buffer), "accel:%d %d %d(g)", LSM6DSOX_Data.Accel.x, LSM6DSOX_Data.Accel.y, LSM6DSOX_Data.Accel.z);
//	else if (*arg1 == '1')
//		snprintf(buffer, sizeof(buffer), "gyro:%d %d %d(dps)", LSM6DSOX_Data.Gyro.x, LSM6DSOX_Data.Gyro.y, LSM6DSOX_Data.Gyro.z);
//	embeddedCliPrint(cli, buffer);
//	embeddedCliPrint(cli, "");
//}
//
//static void CMD_H3LSens_Get(EmbeddedCli *cli, char *args, void *context) {
//    // TODO: Implement H3L sensor get logic
//	H3LIS331DL_Get_Accel(&H3LIS331DL_Data);
//	char buffer[50];
//	snprintf(buffer, sizeof(buffer), "accel:%d %d %d(g)", (int16_t)H3LIS331DL_Data.x, (int16_t)H3LIS331DL_Data.y, (int16_t)H3LIS331DL_Data.z);
//	embeddedCliPrint(cli, buffer);
//	embeddedCliPrint(cli, "");
//}
//
//static void CMD_BMESens_Get(EmbeddedCli *cli, char *args, void *context) {
//    // TODO: Implement BME sensor get logic
//	BME280_Read_Data(&BME280_Data);
//	char buffer[80];
//
//	const char *arg1 = embeddedCliGetToken(args, 1);
//	if (*arg1 == 'a' || *arg1 == '\0')
//		snprintf(buffer, sizeof(buffer), "temp:%.2f(C),humid:%.2f(%%),press:%.2f(hPa)", BME280_Data.temperature, BME280_Data.humidity, BME280_Data.pressure);
//	else if (*arg1 == '0')
//		snprintf(buffer, sizeof(buffer), "temp:%.2f(C)", BME280_Data.temperature);
//	else if (*arg1 == '1')
//		snprintf(buffer, sizeof(buffer), "humid:%.2f(%%)", BME280_Data.humidity);
//	else if (*arg1 == '2')
//		snprintf(buffer, sizeof(buffer), "press:%.2f(hPa)", BME280_Data.pressure);
//	embeddedCliPrint(cli, buffer);
//	embeddedCliPrint(cli, "");
//}
//
//static void CMD_H250Sens_Get(EmbeddedCli *cli, char *args, void *context) {
//	// TODO: Implement H250 sensor get logic
//	H250_I2C_Read_Data(&H250_I2C_Data);
//	char buffer[30];
//	snprintf(buffer, sizeof(buffer), "co2:%.2f(%%)", (float)H250_I2C_Data/100.0f);
//	embeddedCliPrint(cli, buffer);
//	embeddedCliPrint(cli, "");
//}
//
//static void CMD_K33Sens_Get(EmbeddedCli *cli, char *args, void *context) {
//    // TODO: Implement K33 sensor get logic
//	K33_Read_Data(&K33_Data);
//	char buffer[80];
//
//	const char *arg1 = embeddedCliGetToken(args, 1);
//	if (*arg1 == 'a' || *arg1 == '\0')
//		snprintf(buffer, sizeof(buffer), "co2:%.2f(%%),temp:%.2f(C),humid:%.2f(%%)", (float)(K33_Data.CO2/1000.0f), (float)(K33_Data.Temp/100.0f), (float)(K33_Data.RH/100.0f));
//	else if (*arg1 == '0')
//		snprintf(buffer, sizeof(buffer), "co2:%.2f(%%)", (float)(K33_Data.CO2/1000.0f));
//	else if (*arg1 == '1')
//		snprintf(buffer, sizeof(buffer), "temp:%.2f(C)", (float)(K33_Data.Temp/100.0f));
//	else if (*arg1 == '2')
//		snprintf(buffer, sizeof(buffer), "humid:%.2f(%%)", (float)(K33_Data.RH/100.0f));
//	embeddedCliPrint(cli, buffer);
//	embeddedCliPrint(cli, "");
//}
//
//static void CMD_Set_Laser(EmbeddedCli *cli, char *args, void *context) {
//    int argc = embeddedCliGetTokenCount(args);
//    if (argc < 3) {
//        embeddedCliPrint(cli, "Too few args. Format: set_laser [int/ext] [laser_index] [dac_val]\n");
//        return;
//    }
//
//    const char *mode = embeddedCliGetToken(args, 1);
//    uint8_t laser_ind = (uint8_t)atoi(embeddedCliGetToken(args, 2));
//
//    if (strcmp(mode, "int") == 0) {
//        if (laser_ind == 0) {
//            if (argc != 3) {
//                embeddedCliPrint(cli, "Usage: set_laser int 0\n");
//                return;
//            }
//            MCP4902_Shutdown(&DAC_device, MCP4902_CHA);
//            ADG1414_Chain_SwitchAllOff(&laser_int);
//        } else if (laser_ind <= 36) {
////            if (argc != 4) {
////                embeddedCliPrint(cli, "Usage: set_laser int [1-36] [dac_val_percent]\n");
////                return;
////            }
//
//            float percent = (float)atof(embeddedCliGetToken(args, 3));
//            if (percent < 0 || percent > 100) {
//                embeddedCliPrint(cli, "Percent must be 0-100\n");
//                return;
//            }
//
//            uint8_t dac_val = Map(percent, 0, 100, 0, 255);
//            MCP4902_Set_DAC(&DAC_device, MCP4902_CHA, dac_val);
//            ADG1414_Chain_SwitchOn(&laser_int, laser_ind);
//        } else {
//            embeddedCliPrint(cli, "Laser index out of range for int (1-36)\n");
//            return;
//        }
//    } else if (strcmp(mode, "ext") == 0) {
//        if (laser_ind == 0) {
//            if (argc != 3) {
//                embeddedCliPrint(cli, "Usage: set_laser ext 0\n");
//                return;
//            }
//            MCP4902_Shutdown(&DAC_device, MCP4902_CHB);
//            ADG1414_Chain_SwitchAllOff(&laser_ext);
//        } else if (laser_ind <= 8) {
////            if (argc != 4) {
////                embeddedCliPrint(cli, "Usage: set_laser ext [1-8] [dac_val_percent]\n");
////                return;
////            }
//
//            float percent = (float)atof(embeddedCliGetToken(args, 3));
//            if (percent < 0 || percent > 100) {
//                embeddedCliPrint(cli, "Percent must be 0-100\n");
//                return;
//            }
//
//            uint8_t dac_val = Map(percent, 0, 100, 0, 255);
//            MCP4902_Set_DAC(&DAC_device, MCP4902_CHB, dac_val);
//            ADG1414_Chain_SwitchOn(&laser_ext, laser_ind);
//        } else {
//            embeddedCliPrint(cli, "Laser index out of range for ext (1-8)\n");
//            return;
//        }
//    } else {
//        embeddedCliPrint(cli, "Invalid mode. Use 'int' or 'ext'\n");
//        return;
//    }
//
//    embeddedCliPrint(cli, "Laser set successfully\n");
//}
//
//static void CMD_Get_Current(EmbeddedCli *cli, char *args, void *context) {
//    return;
//}
//
//static void CMD_PD_Get(EmbeddedCli *cli, char *args, void *context) {
////    int argc = embeddedCliGetTokenCount(args);
////    if (argc != 2) {
////        embeddedCliPrint(cli, "Usage: pd_get [pd_index]\n");
////        return;
////    }
//
//    uint8_t pd_ind = (uint8_t)atoi(embeddedCliGetToken(args, 1));
//    if (pd_ind < 1 || pd_ind > 36) {
//        embeddedCliPrint(cli, "Invalid pd_index. Must be in 1–36\n");
//        return;
//    }
//
////    SPI_SetDataLength(SPI2, LL_SPI_DATAWIDTH_8BIT);
////    SPI_SetPrescaler(SPI2, LL_SPI_BAUDRATEPRESCALER_DIV16);
//    ADG1414_Chain_SwitchOn(&photo_sw, pd_ind);
//    LL_mDelay(10);
//
//    ADS8327_Read_Data_Polling(&photo_adc, 1000);
//
//    char msg[64];
//    snprintf(msg, sizeof(msg), "\r\nPD_index[%d]: %d\n", pd_ind, (uint16_t)photo_adc.ADC_val);
//    embeddedCliPrint(cli, msg);
//}
//
//static void CMD_Sample_Set_PD(EmbeddedCli *cli, char *args, void *context) {
////    int argc = embeddedCliGetTokenCount(args);
////    if (argc < 2) {
////        embeddedCliPrint(cli, "Too few arguments. Format: sample_set_pd [pd_index]\n");
////        return;
////    }
////    if (argc > 2) {
////        embeddedCliPrint(cli, "Too many arguments. Format: sample_set_pd [pd_index]\n");
////        return;
////    }
//
//    const char *arg1 = embeddedCliGetToken(args, 1);
//    uint8_t pd_ind = (uint8_t)atoi(arg1);
//
//    if (pd_ind < 1 || pd_ind > 36) {
//        embeddedCliPrint(cli, "Invalid PD index. Must be between 1 and 36.\n");
//        return;
//    }
//
////    SPI_SetDataLength(SPI2, LL_SPI_DATAWIDTH_8BIT);
////    SPI_SetPrescaler(SPI2, LL_SPI_BAUDRATEPRESCALER_DIV16);
//    ADG1414_Chain_SwitchOn(&photo_sw, pd_ind);
//    photo_index = pd_ind;
//
//    char buf[64];
//    snprintf(buf, sizeof(buf), "PD index %d selected successfully.\n", pd_ind);
//    embeddedCliPrint(cli, buf);
//}
//
//static void CMD_Sample_Set_Rate(EmbeddedCli *cli, char *args, void *context) {
//    int argc = embeddedCliGetTokenCount(args);
//    if (argc < 3) {
//        embeddedCliPrint(cli, "Too few arguments. Format: sp_set_rate [sampling_rate] [num_samples]\n");
//        return;
//    }
//
//    uint32_t sp_rate = (uint32_t)atoi(embeddedCliGetToken(args, 1));
//    uint32_t num_sample = (uint32_t)atoi(embeddedCliGetToken(args, 2));
//
//    if (sp_rate < 1 || sp_rate > 330000 || num_sample < 1 || num_sample > 50000) {
//        embeddedCliPrint(cli, "Invalid sampling rate or number of samples.\n");
//        return;
//    }
//
//    uint32_t AutoReload = ROUND(1000000.0f / sp_rate) - 1;
//    LL_TIM_DisableIT_UPDATE(TIM1);
//    LL_TIM_DisableCounter(TIM1);
//    LL_TIM_SetAutoReload(TIM1, AutoReload);
//
//    adc_rec_total = num_sample;
//    samp_rate = sp_rate;
//
//    embeddedCliPrint(cli, "Sampling rate and sample count set successfully.\n");
//}
//
//static void CMD_Sample_Trig(EmbeddedCli *cli, char *args, void *context) {
//    if (!adc_rec_total || !samp_rate) {
//        embeddedCliPrint(cli, "Sampling rate or sample count is not set.\n");
//        return;
//    }
//
//    memset(adc_rec_buf, 0x00, adc_rec_total * 2);
//    adc_ptr = adc_rec_buf;
//    adc_rec_ind = 0;
//
//    SPI_SetDataLength(SPI2, LL_SPI_DATAWIDTH_16BIT);
//    SPI_SetPrescaler(SPI2, LL_SPI_BAUDRATEPRESCALER_DIV2);
//
//    LL_TIM_SetCounter(TIM1, 0);
//    LL_TIM_EnableIT_UPDATE(TIM1);
//    LL_TIM_EnableCounter(TIM1);
//
//    embeddedCliPrint(cli, "Sampling started.\n");
//}
//
//static void CMD_Sample_Status_Get(EmbeddedCli *cli, char *args, void *context) {
//    char buf[128];
//    snprintf(buf, sizeof(buf), "Photo: %d   Sampling_Rate: %ld SPS   Num_Samples: %ld S\n",
//             photo_index, samp_rate, adc_rec_total);
//    embeddedCliPrint(cli, buf);
//
//    if (adc_rec_ind == adc_rec_total) {
//        embeddedCliPrint(cli, "-> ADC Data ready to get!\n");
//    } else {
//        embeddedCliPrint(cli, "-> ADC Data is not ready!\n");
//    }
//}
//
//static void CMD_Sample_Get(EmbeddedCli *cli, char *args, void *context) {
//    if (!adc_rec_ind) {
//        embeddedCliPrint(cli, "Please send cmd 'sp_trig' first!\n");
//        return;
//    }
//
//    uint32_t num_sample = (uint32_t)atoi(embeddedCliGetToken(args, 1));
//    if (num_sample < 1 || num_sample > 50000) {
//        embeddedCliPrint(cli, "Invalid number of samples.\n");
//        return;
//    }
//
//    uint16_t crc_val = 0xffff;
//    uint8_t bytes_temp[3];
//    uint32_t header = (0x000FFFFF & num_sample) | 0xFFF00000;
//    bytes_temp[0] = (uint8_t)(header >> 16);
//    bytes_temp[1] = (uint8_t)(header >> 8);
//    bytes_temp[2] = (uint8_t)header;
//    UART_Driver_SendString(USART6, (const char *)bytes_temp);
//
//    for (uint32_t i = 0; i < num_sample; i++) {
//        crc16_CCITT_update(&crc_val, adc_rec_buf[i]);
//        bytes_temp[0] = adc_rec_buf[i] >> 8;
//        bytes_temp[1] = adc_rec_buf[i] & 0xFF;
//        UART_Driver_SendString(USART6, (const char *)bytes_temp);
//    }
//
//    bytes_temp[0] = crc_val >> 8;
//    bytes_temp[1] = crc_val & 0xFF;
//    UART_Driver_SendString(USART6, (const char *)bytes_temp);
//}
//
//static void CMD_Sample_Get_Char(EmbeddedCli *cli, char *args, void *context) {
//    if (!adc_rec_ind) {
//        embeddedCliPrint(cli, "Please send cmd 'sp_trig' first!\n");
//        return;
//    }
//
//    uint32_t num_sample = (uint32_t)atoi(embeddedCliGetToken(args, 1));
//    if (num_sample < 1 || num_sample > 50000) {
//        embeddedCliPrint(cli, "Invalid number of samples.\n");
//        return;
//    }
//
//    uint16_t crc_val = 0xffff;
//    char ascii_buf[5];
//    embeddedCliPrint(cli, "\n");
//
//    for (uint32_t i = 0; i < num_sample; i++) {
//        crc16_CCITT_update(&crc_val, adc_rec_buf[i]);
//        htoa(adc_rec_buf[i], ascii_buf);
//        UART_Driver_SendString(USART6, ascii_buf);
//    }
//
//    htoa(crc_val, ascii_buf);
//    UART_Driver_SendString(USART6, ascii_buf);
//}
//
//static void CMD_Sample_Get_Buf(EmbeddedCli *cli, char *args, void *context) {
//    if (!adc_rec_ind) {
//        embeddedCliPrint(cli, "Please send cmd 'sp_trig' first!\n");
//        return;
//    }
//
//    uint16_t crc_val = 0xffff;
//    uint8_t bytes_temp[3];
//    uint32_t header = (0x000FFFFF & adc_rec_total) | 0xFFF00000;
//
//    bytes_temp[0] = (uint8_t)(header >> 16);
//    bytes_temp[1] = (uint8_t)(header >> 8);
//    bytes_temp[2] = (uint8_t)header;
//    UART_Driver_SendString(USART6, (const char *)bytes_temp);
//
//    for (uint32_t i = 0; i < adc_rec_total; i++) {
//        crc16_CCITT_update(&crc_val, adc_rec_buf[i]);
//        bytes_temp[0] = adc_rec_buf[i] >> 8;
//        bytes_temp[1] = adc_rec_buf[i] & 0xFF;
//        UART_Driver_SendString(USART6, (const char *)bytes_temp);
//    }
//
//    bytes_temp[0] = crc_val >> 8;
//    bytes_temp[1] = crc_val & 0xFF;
//    UART_Driver_SendString(USART6, (const char *)bytes_temp);
//}
//
//static void CMD_Sample_Get_Buf_Char(EmbeddedCli *cli, char *args, void *context) {
//    if (!adc_rec_ind) {
//        embeddedCliPrint(cli, "Please send cmd 'sp_trig' first!\n");
//        return;
//    }
//
//    uint16_t crc_val = 0xffff;
//    char ascii_buf[5];
//    embeddedCliPrint(cli, "\n");
//
//    for (uint32_t i = 0; i < adc_rec_total; i++) {
//        crc16_CCITT_update(&crc_val, adc_rec_buf[i]);
//        htoa(adc_rec_buf[i], ascii_buf);
//        UART_Driver_SendString(USART6, ascii_buf);
//    }
//
//    htoa(crc_val, ascii_buf);
//    UART_Driver_SendString(USART6, ascii_buf);
//}
/*************************************************
 *                  End CMD List                 *
 *************************************************/

/*************************************************
 *                Getter - Helper                *
 *************************************************/
const CliCommandBinding *getCliStaticBindings(void) {
    return cliStaticBindings_internal;
}

uint16_t getCliStaticBindingCount(void) {
    return sizeof(cliStaticBindings_internal) / sizeof(cliStaticBindings_internal[0]);
}
