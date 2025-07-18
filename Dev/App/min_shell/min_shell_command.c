/*
 * min_command.c
 *
 *  Created on: Apr 22, 2025
 *      Author: CAO HIEU
 */

#include "min_shell_command.h"
#include "lt8722.h"
#include "temperature_control.h"
#include "experiment_task.h"
#include "board.h"
#include "adc_monitor.h"

#include <stdlib.h>
#include <string.h>
#include <string.h>
#include <stdio.h>
// =================================================================
// Header Define
// =================================================================
extern temperature_control_task_t temperature_control_task_inst ;
static temperature_control_task_t *ptemperature_control_task = &temperature_control_task_inst;

extern experiment_task_t experiment_task_inst;
static experiment_task_t *pexperiment_task = &experiment_task_inst;





// =================================================================
// Command Handlers
// =================================================================

static uint16_t UpdateCRC16_XMODEM(uint16_t crc, uint8_t byte) {
    const uint16_t polynomial = 0x1021; // CRC16 XMODEM
    crc ^= (uint16_t)byte << 8;
    for (uint8_t bit = 0; bit < 8; bit++) {
        if (crc & 0x8000) {
            crc = (crc << 1) ^ polynomial;
        } else {
            crc <<= 1;
        }
    }
    return crc;
}

static void MIN_Handler_TEST_CONNECTION_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
    min_shell_debug_print("Payload TEST_CONNECTION_CMD (%d bytes):\r\n", len);
    MIN_Send(ctx, TEST_CONNECTION_ACK, payload, len);
}

static void MIN_Handler_SET_TEMP_PROFILE_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
	uint8_t buffer[2];
	uint16_t ret = 0;
    uint8_t ntc_index = payload[0];
    uint8_t tec_mask = payload[1];
    uint8_t heater_mask = payload[2];
    uint16_t tec_mv = (payload[3] << 8) | payload[4];
    uint8_t heater_duty = payload[5];
    int16_t ref_temp = (payload[6] << 8) | payload[7];


	if (ntc_index > 7)
	{
		ret++;
		min_shell_debug_print("NTC index out of range (0-7): %d\r\n", ntc_index);
	}
	if (tec_mask > 0x0F)
	{
		ret++;
		min_shell_debug_print("TEC mask out of range (0-3): %8B\r\n", tec_mask);
	}
	if (heater_mask > 0x0F)
	{
		ret++;
		min_shell_debug_print("Heater mask out of range (0-3): %8B\r\n", heater_mask);
	}
	if ((tec_mv < 500)||(tec_mv > 3000))
	{
		ret++;
		min_shell_debug_print("TEC voltage out of range (500-3000)mV: %d\r\n", tec_mv);
	}
	if (heater_duty > 100)
	{
		ret++;
		min_shell_debug_print("Heater duty out of range (0-100): %d\r\n", heater_duty);
	}
	if (ref_temp > 1000)
	{
		ret++;
		min_shell_debug_print("Heater duty out of range (max 100.0C): %d\r\n", ref_temp);
	}

	if(!ret)
	{
		// set index ntc
		//temperature_control_profile_ntc_register(ptemperature_control_task, ntc_index);
		// set register tec
		uint8_t tec_ovr = temperature_profile_tec_ovr_get(ptemperature_control_task);
		for (uint8_t i = 0; i < 4; i++)
		{
			if (tec_mask & (1<<i))			//tec i register
			{
				if(i == tec_ovr) min_shell_debug_print("tec[%d] registered in ovr mode\r\n", i);
				else
				{
					temperature_control_profile_tec_register(ptemperature_control_task, i);
					min_shell_debug_print("tec[%d] registered\r\n", i);
				}
			}
			else	temperature_control_profile_tec_unregister(ptemperature_control_task, i);
		}
		// set register heater
		for (uint8_t i = 0; i < 4; i++)
		{
			if (heater_mask & (1<<i))
			{
				temperature_control_profile_heater_register(ptemperature_control_task, i);
				min_shell_debug_print("heater[%d] registered\r\n", i);
			}
			else temperature_control_profile_heater_unregister(ptemperature_control_task, i);
		}
		// set tec mv
		temperature_control_profile_tec_voltage_set(ptemperature_control_task, tec_mv);
		// set heater duty
		temperature_control_profile_heater_duty_set(ptemperature_control_task, heater_duty);
		// set ref temp
		temperature_control_profile_setpoint_set(ptemperature_control_task, ref_temp);

		buffer[0] = MIN_RESP_OK;
		buffer[1] = MIN_ERROR_OK;
	}

	else
	{
		buffer[0] = MIN_RESP_FAIL;
		buffer[1] = MIN_RESP_FAIL;
	}


    MIN_Send(ctx, SET_TEMP_PROFILE_ACK, buffer, 2);
}

static void MIN_Handler_START_TEMP_PROFILE_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
	uint8_t reserved = 0xFF;
    min_shell_debug_print("Start temperature control auto\r\n");
    temperature_control_auto_mode_set(ptemperature_control_task);
    MIN_Send(ctx, START_TEMP_PROFILE_ACK, &reserved, 1);
}

static void MIN_Handler_STOP_TEMP_PROFILE_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
	uint8_t reserved = 0xFF;
    min_shell_debug_print("Stop temperature control auto\r\n");
    temperature_control_man_mode_set(ptemperature_control_task);
    MIN_Send(ctx, STOP_TEMP_PROFILE_ACK, &reserved, 1);
}

static void MIN_Handler_SET_OVERRIDE_TEC_PROFILE_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
	uint8_t buffer[2];
	uint16_t ret = 0;

    uint8_t tec_idx = payload[0];
    uint16_t tec_mv = (payload[1] << 8) | payload[2];

    min_shell_debug_print("Min set tec override profile\r\n");

    if(tec_idx > 4)
    {
    	ret++;
    	min_shell_debug_print("tec index out of range (0-4)\r\n");
    }
    if ((tec_mv < 500)||(tec_mv > 3000))
	{
		ret++;
		min_shell_debug_print("tec voltage out of range (500-3000)mV\r\n", tec_mv);
	}

    uint8_t tec_profile = temperature_control_profile_tec_get(ptemperature_control_task);
	if(tec_idx < 4)
	{
		if (tec_profile & (1<<tec_idx))
		{
			min_shell_debug_print("tec[%d] registered in auto mode\r\n",tec_idx);
			ret++;
		}
	}

    if(!ret)
    {

    	if(tec_idx == 4)	min_shell_debug_print("tec ovr mode is off\r\n");
    	temperature_profile_tec_ovr_register(ptemperature_control_task, tec_idx);
    	temperature_profile_tec_ovr_voltage_set(ptemperature_control_task, tec_mv);
    	buffer[0] = MIN_RESP_OK;
    	buffer[1] = MIN_ERROR_OK;
    }
    else
    {
    	buffer[0] = MIN_RESP_FAIL;
		buffer[1] = MIN_RESP_FAIL;
    }


    MIN_Send(ctx, SET_OVERRIDE_TEC_PROFILE_ACK, buffer, 2);
}

static void MIN_Handler_START_OVERRIDE_TEC_PROFILE_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
	uint8_t reserved = 0xFF;
	min_shell_debug_print("Min start tec override\r\n");
	temperature_profile_tec_ovr_enable(ptemperature_control_task);
	MIN_Send(ctx, START_OVERRIDE_TEC_PROFILE_ACK, &reserved, 1);
}

static void MIN_Handler_STOP_OVERRIDE_TEC_PROFILE_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{
	uint8_t reserved = 0xFF;
	min_shell_debug_print("Min stop tec override\r\n");
	temperature_profile_tec_ovr_disable(ptemperature_control_task);
	MIN_Send(ctx, STOP_OVERRIDE_TEC_PROFILE_ACK, &reserved, 1);
}

static void MIN_Handler_SET_SAMPLING_PROFILE_CMD(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len)
{

	min_shell_debug_print("Min set sampling profile\r\n");
	uint8_t buffer[2];
	uint16_t ret = 0;
	uint16_t samp_rate = (payload[0] << 8) | payload[1];
	uint8_t samp_pos = payload[2];
	uint8_t percent = payload[3];
	uint16_t pre_time = (payload[4] << 8) | payload[5];
	uint16_t samp_time = (payload[6] << 8) | payload[7];
	uint16_t post_time = (payload[8] << 8) | payload[9];

	if ((samp_rate < 1000) || (samp_rate > 800000))
	{
		ret++;
		min_shell_debug_print("sampling rate out of range (1K-800K)\r\n");
	}
	samp_rate /= 1000;

	if ((samp_pos == 0) || (samp_pos > 36))
	{
		ret++;
		min_shell_debug_print("sample pos out of range (1-36)\r\n");
	}
	if (percent > 100)
	{
		ret++;
		min_shell_debug_print("percent out of range (0-100)\r\n");
	}

	if (pre_time == 0)
	{
		ret++;
		min_shell_debug_print("pre_time should be larger than 0\r\n");
	}
	pre_time *= 1000;

	if (samp_time == 0)
	{
		ret++;
		min_shell_debug_print("sample time should be larger than 0\r\n");
	}
	samp_time *= 1000;

	if (post_time == 0)
	{
		ret++;
		min_shell_debug_print("post_time should be larger than 0\r\n");
	}
	post_time *= 1000;

	uint32_t num_sample = ((pre_time + samp_time + post_time) * samp_rate )/1000000;
	if (num_sample > 2048)	//larrger than 4MB
	{
		ret++;
		min_shell_debug_print("total sample must be less than 2048K \r\n");
	}

	if(!ret)
	{
		experiment_profile_t profile;
		profile.sampling_rate = samp_rate;		// kHz
		profile.pos = samp_pos;
		profile.laser_percent = percent;
		profile.pre_time = pre_time;				// us
		profile.experiment_time = samp_time;		// us
		profile.post_time = post_time;				// us
		profile.num_sample = num_sample;			// kSample
		profile.period = 1000000 / samp_rate;	// ns
		if(!experiment_task_set_profile(pexperiment_task,&profile))
		{
			buffer[0] = MIN_RESP_OK;
			buffer[1] = MIN_ERROR_OK;
			min_shell_debug_print("Min exp set profile done\r\n");
		}
		else
		{
			buffer[0] = MIN_RESP_FAIL;
			buffer[1] = MIN_RESP_FAIL;
			min_shell_debug_print("Min exp set profile error\r\n");
		}
	}
	else
	{
		buffer[0] = MIN_RESP_FAIL;
		buffer[1] = MIN_RESP_FAIL;
	}

	MIN_Send(ctx, SET_SAMPLING_PROFILE_ACK, buffer, 2);
}





// =================================================================
// Command Table
// =================================================================



static const MIN_Command_t command_table[] = {
	{ TEST_CONNECTION_CMD,  			MIN_Handler_TEST_CONNECTION_CMD },
	{ SET_TEMP_PROFILE_CMD,				MIN_Handler_SET_TEMP_PROFILE_CMD},
	{ START_TEMP_PROFILE_CMD,  			MIN_Handler_START_TEMP_PROFILE_CMD },
	{ STOP_TEMP_PROFILE_CMD,			MIN_Handler_STOP_TEMP_PROFILE_CMD},
	{ SET_OVERRIDE_TEC_PROFILE_CMD,  	MIN_Handler_SET_OVERRIDE_TEC_PROFILE_CMD },
	{ START_OVERRIDE_TEC_PROFILE_CMD,	MIN_Handler_START_OVERRIDE_TEC_PROFILE_CMD},
	{ STOP_OVERRIDE_TEC_PROFILE_CMD,	MIN_Handler_STOP_OVERRIDE_TEC_PROFILE_CMD},
	{ SET_SAMPLING_PROFILE_CMD,			MIN_Handler_SET_SAMPLING_PROFILE_CMD},

};

static const int command_table_size = sizeof(command_table) / sizeof(command_table[0]);

// =================================================================
// Helper Functions
// =================================================================

const MIN_Command_t *MIN_GetCommandTable(void) {
    return command_table;
}

int MIN_GetCommandTableSize(void) {
    return command_table_size;
}
