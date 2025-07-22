/*
 * tec_control_task.c
 *
 *  Created on: Jun 16, 2025
 *      Author: Admin
 */
#include "bsp_tec.h"
#include "bsp_heater.h"
#include "dbc_assert.h"
#include "app_signals.h"
#include "error_codes.h"
#include "uart_dbg.h"
#include "stddef.h"
#include "temperature_control.h"
#include "adc_monitor.h"


DBC_MODULE_NAME("tec_control")

temperature_control_task_t temperature_control_task_inst ;

static temperature_control_evt_t const entry_evt = {.super = {.sig = SIG_ENTRY} };
static temperature_control_evt_t const exit_evt = {.super = {.sig = SIG_EXIT} };

#define TEMPERATURE_CONTROL_TASK_TIME_LOOP 			100 //loop to check and control TEC every 100 ms
#define TEMPERATURE_CONTROL_TASK_NUM_EVENTS 		2
#define TEMPERATURE_CONTROL_WAIT_TIMEOUT_NUM  		30 // 3 second timeout for waiting for temperature control events
#define TEMPERATURE_CONTROL_HYSTERIS          		10 // hysteris 1 celcius

#define TEMP_LIM_MIN_DEFAULT						200
#define TEMP_TARGET_DEFAULT							250
#define TEMP_LIM_MAX_DEFAULT						300

#define TEMP_PRI_NTC_DEFAULT						1
#define TEMP_SEC_NTC_DEFAULT						1

#define TEMP_TEC_VOLT_DEFAULT						1000
#define TEMP_HTR_DUTY_DEFAULT						20

#define TEMP_TEC_RUNNING_DEFAULT					0x03
#define TEMP_HTR_RUNNING_DEFAULT					0x03


temperature_control_evt_t temperature_control_current_event = {0}; // Current event being processed
temperature_control_evt_t temperature_control_task_event_buffer[TEMPERATURE_CONTROL_TASK_NUM_EVENTS] = {0}; // Array to hold shell events
circular_buffer_t temperature_control_task_event_queue = {0}; // Circular buffer to hold shell events


static state_t temperature_control_state_manual_handler(temperature_control_task_t * const me, temperature_control_evt_t const * const e);
static state_t temperature_control_state_cooling_handler(temperature_control_task_t * const me, temperature_control_evt_t const * const e);
static state_t temperature_control_state_wait_heat_handler(temperature_control_task_t * const me, temperature_control_evt_t const * const e);
static state_t temperature_control_state_heating_heater_handler(temperature_control_task_t * const me, temperature_control_evt_t const * const e);
static state_t temperature_control_state_wait_cool_handler(temperature_control_task_t * const me, temperature_control_evt_t const * const e);
static state_t temperature_control_state_ntc_error_handler(temperature_control_task_t * const me, temperature_control_evt_t const * const e);

static void temperature_control_task_init(temperature_control_task_t * const me,temperature_control_evt_t const * const e)
{
	temp_control_debug_print("temperature control init\r\n");

	//temperature_control_default
	bsp_temperature_power_on();
	me->tec_heater_power_status = 1;
	temperature_control_tec_init_all(me);
	me->temperature_control_profile.profile_min_temp		= TEMP_LIM_MIN_DEFAULT;
	me->temperature_control_profile.setpoint 				= TEMP_TARGET_DEFAULT;
	me->temperature_control_profile.profile_max_temp		= TEMP_LIM_MAX_DEFAULT;

	me->temperature_control_profile.pri_NTC_idx				= TEMP_PRI_NTC_DEFAULT;
	me->temperature_control_profile.sec_NTC_idx				= TEMP_SEC_NTC_DEFAULT;

	me->temperature_control_profile.tec_voltage				= TEMP_TEC_VOLT_DEFAULT;
	me->temperature_control_profile.heater_duty_cycle		= TEMP_HTR_DUTY_DEFAULT;

//	me->temperature_control_profile.profile_tec_set			= TEMP_TEC_RUNNING_DEFAULT;
//	me->temperature_control_profile.profile_heater_set		= TEMP_HTR_RUNNING_DEFAULT;

//	int16_t temperature = temperature_monitor_get_ntc_temperature(me->temperature_control_profile.pri_NTC_idx);
//	if (temperature > me->temperature_control_profile.setpoint)
//		me->state = temperature_control_state_cooling_handler;
//	else me->state = (temperature_control_state_wait_heat_handler);
//	SST_TimeEvt_arm(&me->temperature_control_task_timeout_timer, TEMPERATURE_CONTROL_TASK_TIME_LOOP, TEMPERATURE_CONTROL_TASK_TIME_LOOP);

//	SST_TimeEvt_disarm(&me->temperature_control_task_timeout_timer);

//	em đang dev thêm đoạn này chưa hoàn chỉnh


}
static void temperature_control_task_dispatch(temperature_control_task_t * const me, temperature_control_evt_t const * const e)
{
    temperature_control_state_handler_t prev_state = me->state; /* save for later */
    state_t status = (me->state)(me, e);

    if (status == TRAN_STATUS) { /* transition taken? */
        (prev_state)(me, &exit_evt);
        (me->state)(me, &entry_evt);
    }
}
void temperature_control_task_ctor(temperature_control_task_t * const me, temperature_control_task_init_t const * const init) {
    DBC_ASSERT(0u, me != NULL);
    SST_Task_ctor(&me->super, (SST_Handler) temperature_control_task_init, (SST_Handler)temperature_control_task_dispatch, \
                                (SST_Evt *)init->current_evt, init->temperature_control_task_event_buffer);
    SST_TimeEvt_ctor(&me->temperature_control_task_timeout_timer, EVT_TEMPERATURE_CONTROL_TIMEOUT_CONTROL_LOOP, &(me->super));
    me->state = init->init_state; // Set the initial state to process handler
    me->tec_inited = 0;
    me->temperature_control_profile.profile_tec_set = 0; // all off
    me->temperature_control_profile.profile_heater_set = 0; // all off
    me->temperature_tec_ovr_profile.profile_tec_ovr_set = 4; // take index > 3 (out range)
    me->temperature_control_profile.auto_recover = 0;
    me->temperature_control_profile.profile_max_temp = 1000;
    me->temperature_control_profile.profile_min_temp = 0;
    for (uint32_t i = 0; i< 4; i++) me->tec_table[i] = init->tec_table[i];
    SST_TimeEvt_disarm(&me->temperature_control_task_timeout_timer); // Disarm the timeout timer
}

void temperature_control_task_singleton_ctor(void)
{
	circular_buffer_init(&temperature_control_task_event_queue, (uint8_t * )&temperature_control_task_event_buffer, sizeof(temperature_control_task_event_buffer), TEMPERATURE_CONTROL_TASK_NUM_EVENTS, sizeof(temperature_control_evt_t));
	temperature_control_task_init_t init = {
			.init_state = temperature_control_state_manual_handler,
			.current_evt = &temperature_control_current_event,
			.temperature_control_task_event_buffer = &temperature_control_task_event_queue,
			.tec_table = {&tec_0, &tec_1, &tec_2, &tec_3}
	};
	temperature_control_task_ctor(&temperature_control_task_inst,&init);
}

void temperature_control_task_start(uint8_t priority)
{
	SST_Task_start(&temperature_control_task_inst.super,priority);
}

static state_t temperature_control_state_manual_handler(temperature_control_task_t * const me, temperature_control_evt_t const * const e)
{
	switch (e->super.sig)
	{
		case SIG_ENTRY:
		{
			temp_control_debug_print("Entry MANUAL\r\n");
			SST_TimeEvt_disarm(&me->temperature_control_task_timeout_timer); //disable the periodic timer
			temperature_control_tec_output_disable_all(me); 	//disable all tecs
			temperature_control_heater_disable_all(me); 		//disable heater output
			me->state_num = TEMPERATURE_MAN_CONTROL;
			return HANDLED_STATUS;
		}

		case EVT_TEMPERATURE_CONTROL_HAS_CMD:
		{
			switch (e->cmd)
			{
				case TEMPERATURE_AUTOMODE_START:
				{
					temp_control_debug_print("Src: MANUAL ->> Event: CMD TEMPERATURE_AUTOMODE_START\r\n");
					if (me->tec_heater_power_status == 0)
					{
						//switch to AUTO, but tec power is off
						temperature_control_power_control(me, 1);
						temperature_control_tec_init_all(me); //initialized all tecs
						temperature_control_tec_output_disable_all(me);
					}
					if(temperature_monitor_get_ntc_error(me->temperature_control_profile.pri_NTC_idx,
						me->temperature_control_profile.sec_NTC_idx,
						me->temperature_control_profile.profile_max_temp,
						me->temperature_control_profile.profile_min_temp))
					{
						temp_control_debug_print("Src: MANUAL ->> Dest: NTC_ERROR\r\n");
						me->state = temperature_control_state_ntc_error_handler;
						return TRAN_STATUS;
					}
					int16_t temperature = temperature_monitor_get_ntc_temperature(me->temperature_control_profile.pri_NTC_idx);
					if (temperature > me->temperature_control_profile.setpoint)
					{
						temp_control_debug_print("Src: MANUAL ->> Dest: COOLING\r\n");
						me->state = temperature_control_state_cooling_handler;
					}
					else
					{
						temp_control_debug_print("Src: MANUAL ->> Dest: WAIT_HEAT\r\n");
						me->state = temperature_control_state_wait_heat_handler;
					}

					SST_TimeEvt_arm(&me->temperature_control_task_timeout_timer, TEMPERATURE_CONTROL_TASK_TIME_LOOP, TEMPERATURE_CONTROL_TASK_TIME_LOOP);

					return TRAN_STATUS;
				}
				default:
					return IGNORED_STATUS;
			}
		}
		default:
			return IGNORED_STATUS;
	}
}


static state_t temperature_control_state_cooling_handler(temperature_control_task_t * const me, temperature_control_evt_t const * const e)
{
	switch (e->super.sig)
	{
		case SIG_ENTRY:
		{
			temp_control_debug_print("Entry COOLING\r\n");
			me->counter = 0;
			temperature_control_auto_tec_set_output(me, TEC_COOL); //set all tecs on the profile to the desired voltage
			temperature_control_auto_tec_enable_output(me); // Turn on TEC output
			return HANDLED_STATUS;
		}

		case EVT_TEMPERATURE_CONTROL_TIMEOUT_CONTROL_LOOP:
		{
			temp_control_debug_print("Src: COOLING ->> Event: time_loop\r\n");
			if(temperature_monitor_get_ntc_error(me->temperature_control_profile.pri_NTC_idx,
				me->temperature_control_profile.sec_NTC_idx,
				me->temperature_control_profile.profile_max_temp,
				me->temperature_control_profile.profile_min_temp))
			{
				temp_control_debug_print("Src: COOLING ->> Dest: NTC_ERROR\r\n");
				me->state = temperature_control_state_ntc_error_handler;
				return TRAN_STATUS;
			}
			int16_t temperature = temperature_monitor_get_ntc_temperature(me->temperature_control_profile.pri_NTC_idx); //get NTC temperature
			if (temperature > me->temperature_control_profile.setpoint)
				return HANDLED_STATUS;	// Continue cooling
			else // Stop cooling, transition to stopped state to wait for natural heating
			{
				temp_control_debug_print("Src: COOLING ->> Dest: WAIT_HEAT\r\n");
				me->state = temperature_control_state_wait_heat_handler;
				return TRAN_STATUS;
			}
		}
		case EVT_TEMPERATURE_CONTROL_HAS_CMD:
		{
			switch (e->cmd)
			{
				case TEMPERATURE_MANMODE_START:
				{
					temp_control_debug_print("Src: COOLING ->> Event: CMD TEMPERATURE_MANMODE_START\r\n");
					temp_control_debug_print("Src: COOLING ->> Dest: MANUAL\r\n");
					me->state = temperature_control_state_manual_handler; // Transition to manual mode
					return TRAN_STATUS;
				}
				default: return IGNORED_STATUS;
			}
		}
		default:
			return IGNORED_STATUS;
	}
}

static state_t temperature_control_state_wait_heat_handler(temperature_control_task_t * const me, temperature_control_evt_t const * const e)
{	
	switch (e->super.sig)
	{
		case SIG_ENTRY:
		{
			temp_control_debug_print("Entry WAIT_HEAT\r\n");
   			me->counter = 0; // Reset counter for waiting
   			temperature_control_tec_output_disable_all(me); // Disable all TEC
   			temperature_control_heater_disable_all(me); 	// Disable all heater
   			return HANDLED_STATUS;
		}
		case EVT_TEMPERATURE_CONTROL_TIMEOUT_CONTROL_LOOP:
		{
			temp_control_debug_print("Src: WAIT_HEAT ->> Event: time_loop\r\n");
			if(temperature_monitor_get_ntc_error(me->temperature_control_profile.pri_NTC_idx,
				me->temperature_control_profile.sec_NTC_idx,
				me->temperature_control_profile.profile_max_temp,
				me->temperature_control_profile.profile_min_temp))
			{
				temp_control_debug_print("Src: WAIT_HEAT ->> Dest: NTC_ERROR\r\n");
				me->state = temperature_control_state_ntc_error_handler;
				return TRAN_STATUS;
			}
			int16_t temperature = temperature_monitor_get_ntc_temperature(me->temperature_control_profile.pri_NTC_idx);
			if (temperature > me->temperature_control_profile.setpoint)
			{
				// temperature larger than setpoint, turn on TEC on COOL mode
				temp_control_debug_print("Src: WAIT_HEAT ->> Dest: COOLING\r\n");
				me->state = temperature_control_state_cooling_handler;
				return TRAN_STATUS;
			}
			else
			{
				// temperature is still lower than setpoint, wait for natural heating, calculate time to wait
				me->counter++;
				if ((me->counter >= TEMPERATURE_CONTROL_WAIT_TIMEOUT_NUM) || ((me->temperature_control_profile.setpoint - temperature) > TEMPERATURE_CONTROL_HYSTERIS))
				{
					// can not wait any longer, turn on heater
					temp_control_debug_print("Src: WAIT_HEAT ->> Dest: HEATING\r\n");
					me->state = temperature_control_state_heating_heater_handler;
					return TRAN_STATUS;
   				}
			}
		}
		case EVT_TEMPERATURE_CONTROL_HAS_CMD:
		{
			switch (e->cmd)
			{
				case TEMPERATURE_MANMODE_START:
				{
					temp_control_debug_print("Src: WAIT_HEAT ->> Event: CMD TEMPERATURE_MANMODE_START\r\n");
					temp_control_debug_print("Src: WAIT_HEAT ->> Dest: MANUAL\r\n");
					me->state = temperature_control_state_manual_handler; // Transition to manual mode
					return TRAN_STATUS;
				}
				default: return IGNORED_STATUS;
			}
		}
		default: return IGNORED_STATUS;
	}
}

static state_t temperature_control_state_heating_heater_handler(temperature_control_task_t * const me, temperature_control_evt_t const * const e)
{	
	switch (e->super.sig)
	{
		case SIG_ENTRY:
		{
			temp_control_debug_print("Entry HEATING\r\n");
   			me->counter = 0; // Reset counter for heating
   			temperature_control_enable_heater(me); //turn on heater in profile
   			return HANDLED_STATUS;
  	    }
		case EVT_TEMPERATURE_CONTROL_TIMEOUT_CONTROL_LOOP:
		{
			temp_control_debug_print("Src: HEATING ->> Event: time_loop\r\n");
			if(temperature_monitor_get_ntc_error(me->temperature_control_profile.pri_NTC_idx,
				me->temperature_control_profile.sec_NTC_idx,
				me->temperature_control_profile.profile_max_temp,
				me->temperature_control_profile.profile_min_temp))
			{
				temp_control_debug_print("Src: HEATING ->> Dest: NTC_ERROR\r\n");
				me->state = temperature_control_state_ntc_error_handler;
			   return TRAN_STATUS;
			}
			int16_t temperature = temperature_monitor_get_ntc_temperature(me->temperature_control_profile.pri_NTC_idx);
			if (temperature < me->temperature_control_profile.setpoint)
				return HANDLED_STATUS; //temperature smaller than setpoint, keep heating
			else // temperature is larger than setpoint, wait for natural cooling, calculate time to wait
			{
				temp_control_debug_print("Src: HEATING ->> Dest: WAIT_COOL\r\n");
				me->state = temperature_control_state_wait_cool_handler;
				return TRAN_STATUS;
			}
		}
		case EVT_TEMPERATURE_CONTROL_HAS_CMD:
		{
			switch (e->cmd)
			{
				case TEMPERATURE_MANMODE_START:
				{
					temp_control_debug_print("Src: HEATING ->> Event: CMD TEMPERATURE_MANMODE_START\r\n");
					temp_control_debug_print("Src: HEATING ->> Dest: MANUAL\r\n");
					me->state = temperature_control_state_manual_handler; // Transition to manual mode
					return TRAN_STATUS;
				}
				default: return IGNORED_STATUS;
			}
		}
		default: return IGNORED_STATUS;
	}
}

static state_t temperature_control_state_wait_cool_handler(temperature_control_task_t * const me, temperature_control_evt_t const * const e)
{
	switch (e->super.sig)
	{
		case SIG_ENTRY:
		{
			temp_control_debug_print("Entry WAIT_COOL\r\n");
			temperature_control_tec_output_disable_all(me); //turn off all heater
			temperature_control_heater_disable_all(me);
   			return HANDLED_STATUS;
  	    }
		case EVT_TEMPERATURE_CONTROL_TIMEOUT_CONTROL_LOOP:
		{
			temp_control_debug_print("Src: WAIT_COOL ->> Event: time_loop\r\n");
			if(temperature_monitor_get_ntc_error(me->temperature_control_profile.pri_NTC_idx,
				me->temperature_control_profile.sec_NTC_idx,
				me->temperature_control_profile.profile_max_temp,
				me->temperature_control_profile.profile_min_temp))
			{
				temp_control_debug_print("Src: WAIT_COOL ->> Dest: NTC_ERROR\r\n");
				me->state = temperature_control_state_ntc_error_handler;
				return TRAN_STATUS;
			}
			int16_t temperature = temperature_monitor_get_ntc_temperature(me->temperature_control_profile.pri_NTC_idx);
			if (temperature < me->temperature_control_profile.setpoint)
			{
				// temperature automatically below setpoint, heat it up
				temp_control_debug_print("Src: WAIT_COOL ->> Dest: HEATING\r\n");
				me->state = temperature_control_state_heating_heater_handler;
				return TRAN_STATUS;
			}
			else
			{
				// temperature is larger than setpoint, wait for natural cooling, calculate time to wait
				me->counter++;
				if (me->counter >= TEMPERATURE_CONTROL_WAIT_TIMEOUT_NUM)
				{
					// can not wait any longer, turn on TEC to cool
					temp_control_debug_print("Src: WAIT_COOL ->> Dest: COOLING\r\n");
					me->state = temperature_control_state_cooling_handler;
					return TRAN_STATUS;
   				}
				return HANDLED_STATUS;
			}
		}
		case EVT_TEMPERATURE_CONTROL_HAS_CMD:
		{
			switch (e->cmd)
			{
				case TEMPERATURE_MANMODE_START:
				{
					temp_control_debug_print("Src: WAIT_COOL ->> Event: CMD TEMPERATURE_MANMODE_START\r\n");
					temp_control_debug_print("Src: WAIT_COOL ->> Dest: MANUAL\r\n");
					me->state = temperature_control_state_manual_handler; // Transition to manual mode
					return TRAN_STATUS;
				}
				default: return IGNORED_STATUS;
			}
		}
		default: return IGNORED_STATUS;
	}
}


static state_t temperature_control_state_ntc_error_handler(temperature_control_task_t * const me, temperature_control_evt_t const * const e)
{
	switch (e->super.sig)
	{
		case SIG_ENTRY:
		{
			temp_control_debug_print("Entry NTC_ERROR\r\n");
			temperature_control_tec_output_disable_all(me); 	//turn off all tec
			temperature_control_heater_disable_all(me);			//turn off all heater
   			return HANDLED_STATUS;
  	    }

		case EVT_TEMPERATURE_CONTROL_TIMEOUT_CONTROL_LOOP:
		{
			temp_control_debug_print("Src: NTC_ERROR ->> Event: time_loop\r\n");
			if(me->temperature_control_profile.auto_recover)
			{
				if(!temperature_monitor_get_ntc_error(me->temperature_control_profile.pri_NTC_idx,
					me->temperature_control_profile.sec_NTC_idx,
					me->temperature_control_profile.profile_max_temp,
					me->temperature_control_profile.profile_min_temp))
				{
					// Không còn lỗi do NTC nữa
					int16_t temperature = temperature_monitor_get_ntc_temperature(me->temperature_control_profile.pri_NTC_idx);
					if (temperature > me->temperature_control_profile.setpoint)
					{
						temp_control_debug_print("Src: NTC_ERROR (Recovery) ->> Dest: COOLING\r\n");
						me->state = temperature_control_state_cooling_handler;
					}
					else
					{
						temp_control_debug_print("Src: NTC_ERROR (Recovery) ->> Dest: WAIT_HEAT\r\n");
						me->state = temperature_control_state_wait_heat_handler;
					}
					return TRAN_STATUS;
				}
			}
			return HANDLED_STATUS;
		}

		case EVT_TEMPERATURE_CONTROL_HAS_CMD:
		{
			switch (e->cmd)
			{
				case TEMPERATURE_MANMODE_START:
				{
					temp_control_debug_print("Src: NTC_ERROR ->> Event: CMD TEMPERATURE_MANMODE_START\r\n");
					temp_control_debug_print("Src: NTC_ERROR ->> Dest: MANUAL\r\n");
					me->state = temperature_control_state_manual_handler; // Transition to manual mode
					return TRAN_STATUS;
				}
				default: return IGNORED_STATUS;
			}
		}
		default: return IGNORED_STATUS;
	}
}


void temperature_control_auto_tec_enable_output(temperature_control_task_t * const me)
{
// Enable or disable TEC output
	uint8_t tec_set = me->temperature_control_profile.profile_tec_set;
	for (uint8_t i = 0; i<4; i++)
	{
		if (tec_set & (1 << i)) temperature_control_tec_enable_output(me, i, true);
		else temperature_control_tec_enable_output(me, i, false);
	}
}

void temperature_control_enable_heater(temperature_control_task_t * const me)
{
	 uint8_t heater_duty = me->temperature_control_profile.heater_duty_cycle;
	 uint8_t heater_set = me->temperature_control_profile.profile_heater_set;
	 for (uint8_t i =0; i<4; i++)
	 {
		 if (heater_set & (1 << i)) bsp_heater_set_duty_channel(i,heater_duty);
		 else bsp_heater_turn_off_channel(i);

	 }
}

void temperature_control_heater_disable_all(temperature_control_task_t * const me)
{
	 for (uint8_t i =0; i<4; i++) bsp_heater_turn_off_channel(i);
}


uint16_t temperature_control_profile_tec_voltage_get( temperature_control_task_t *const me)
{
	return me->temperature_control_profile.tec_voltage;
	return ERROR_OK;
}

uint32_t temperature_control_auto_mode_set(temperature_control_task_t *const me)
{
	temperature_control_evt_t auto_mode_evt = {.super = {.sig = EVT_TEMPERATURE_CONTROL_HAS_CMD},
												.cmd = TEMPERATURE_AUTOMODE_START, };
	SST_Task_post(&me->super, (SST_Evt *)&auto_mode_evt);
	return ERROR_OK;
}
uint32_t temperature_control_man_mode_set(temperature_control_task_t *const me)
{
	temperature_control_evt_t man_mode_evt = {.super = {.sig = EVT_TEMPERATURE_CONTROL_HAS_CMD},
												.cmd = TEMPERATURE_MANMODE_START,
												};
	SST_Task_post(&me->super, (SST_Evt *)&man_mode_evt);
	return ERROR_OK;
}



void temperature_control_auto_tec_set_output(temperature_control_task_t * const me,uint32_t tec_dir)
{
	 uint8_t tec_set = me->temperature_control_profile.profile_tec_set;
	 uint16_t voltage_mv = me->temperature_control_profile.tec_voltage;
	 for (uint32_t i =0; i<4; i++)
	 {
		 if (tec_set & (0x01 << i))	//tec is in the profile
		 {
			uint8_t status = (me->tec_table[i]->status);
			if (!(status & (1 << TEC_INIT_POS)))	//not init yet, first init
				lt8722_init(me->tec_table[i]);
			temperature_control_tec_manual_set_output(me, i, tec_dir, voltage_mv);
			 //lt8722_set_output_voltage_channel(me->tec_table[i], tec_dir, (int64_t)voltage_ms * 1000000);
		 }
	 }
}

void temperature_control_tec_output_disable_all(temperature_control_task_t * const me)
{
	for (uint32_t i = 0;i<4;i++) lt8722_set_swen_req(me->tec_table[i],0);
}

void temperature_control_tec_init_all(temperature_control_task_t * const me)
{
	for (uint32_t i = 0;i<4;i++)
	{
		if (!lt8722_init(me->tec_table[i]))
			me->tec_inited |= (0x01 << i);
	}
}

uint32_t temperature_control_tec_init(temperature_control_task_t * const me,uint32_t tec_idx)
{
	int8_t ret;
	if (tec_idx > 3) return ERROR_NOT_SUPPORTED;
	ret = lt8722_init(me->tec_table[tec_idx]);
	if (ret) return ERROR_FAIL;
	else return ERROR_OK;
}
uint32_t temperature_control_tec_enable_output(temperature_control_task_t * const me,uint32_t tec_idx, bool value)
{
	int8_t ret;
	if (tec_idx > 3) return ERROR_NOT_SUPPORTED;
	ret = lt8722_set_swen_req(me->tec_table[tec_idx],value);
	if (ret) return ERROR_FAIL;
	else return ERROR_OK;
}

bool temperature_control_is_in_man_state(temperature_control_task_t * const me)
{
	return (me->state == temperature_control_state_manual_handler);
}


uint32_t temperature_profile_tec_ovr_register(temperature_control_task_t *const me, uint8_t tec_idx)
{
	if (tec_idx > 3)
		me->temperature_tec_ovr_profile.profile_tec_ovr_set = 0xFF;
	else
		me->temperature_tec_ovr_profile.profile_tec_ovr_set = tec_idx;
	return ERROR_OK;
}

uint32_t temperature_profile_tec_ovr_voltage_set(temperature_control_task_t *const me, uint16_t	volt_mv)
{
	if ((volt_mv < 500) || (volt_mv > 3000)) return ERROR_NOT_SUPPORTED;
	me->temperature_tec_ovr_profile.tec_ovr_voltage = volt_mv;
	return ERROR_OK;
}

uint32_t temperature_profile_tec_ovr_enable(temperature_control_task_t *const me)
{
	uint8_t tec_idx = me->temperature_tec_ovr_profile.profile_tec_ovr_set;
	uint16_t volt_mV = me->temperature_tec_ovr_profile.tec_ovr_voltage;
	uint8_t tec_status = me->tec_table[tec_idx]->status;

	if (tec_status & (1 << TEC_INIT_POS))	//tec not init
	{
		if (lt8722_init(me->tec_table[tec_idx])) return ERROR_FAIL;
	}

	if (lt8722_set_output_voltage_channel(me->tec_table[tec_idx], TEC_COOL, (int64_t)volt_mV*1000000)) return ERROR_FAIL;
	if (lt8722_set_swen_req(me->tec_table[tec_idx], 1)) return ERROR_FAIL;
	return ERROR_OK;
}

uint32_t temperature_profile_tec_ovr_disable(temperature_control_task_t *const me)
{
	if (me->temperature_tec_ovr_profile.profile_tec_ovr_set > 3) return ERROR_NOT_SUPPORTED;
	uint8_t tec_idx = me->temperature_tec_ovr_profile.profile_tec_ovr_set;
	int8_t ret;
	ret = lt8722_set_swen_req(me->tec_table[tec_idx], 0);
	if (ret) return ERROR_FAIL;
	else return ERROR_OK;
}

uint8_t temperature_profile_tec_ovr_get(temperature_control_task_t *const me)
{
	return me->temperature_tec_ovr_profile.profile_tec_ovr_set;
}

uint16_t temperature_profile_tec_ovr_get_voltage(temperature_control_task_t *const me)
{
	return me->temperature_tec_ovr_profile.tec_ovr_voltage;
}


uint32_t temperature_control_profile_tec_register(temperature_control_task_t *const me,uint8_t tec_idx)
{
	if (tec_idx > 3) return ERROR_NOT_SUPPORTED;

	me->temperature_control_profile.profile_tec_set |= (1 << tec_idx);
	return ERROR_OK;
}

uint32_t temperature_control_profile_tec_unregister(temperature_control_task_t *const me,uint8_t tec_idx)
{
	if (tec_idx > 3) return ERROR_NOT_SUPPORTED;

	me->temperature_control_profile.profile_tec_set &= ~(1 << tec_idx);
	return ERROR_OK;
}

uint8_t temperature_control_profile_tec_get(temperature_control_task_t *const me)
{
	return me->temperature_control_profile.profile_tec_set;
}
/*
 * change the profile output voltage of TEC, only work when the system in manual mode
 */
uint32_t temperature_control_profile_tec_voltage_set(temperature_control_task_t *const me, uint16_t	volt_mv)
{
	if ((volt_mv < 500) || (volt_mv > 3000)) return ERROR_NOT_SUPPORTED;
	me->temperature_control_profile.tec_voltage = volt_mv;

	return ERROR_OK;
}
uint32_t temperature_control_profile_heater_duty_set( temperature_control_task_t *const me,uint8_t	duty)
{
	if (duty > 100) return ERROR_NOT_SUPPORTED;

		me->temperature_control_profile.heater_duty_cycle = duty;
		return ERROR_OK;
}
uint8_t temperature_control_profile_heater_duty_get( temperature_control_task_t *const me)
{
	return me->temperature_control_profile.heater_duty_cycle;
}
uint32_t temperature_control_profile_heater_register( temperature_control_task_t *const me,uint8_t heater_idx)
{
	if (heater_idx > 3) return ERROR_NOT_SUPPORTED;

		me->temperature_control_profile.profile_heater_set |= (1 << heater_idx);
		return ERROR_OK;
}
uint32_t temperature_control_profile_heater_unregister(temperature_control_task_t *const me,uint8_t heater_idx)
{
	if (heater_idx > 3) return ERROR_NOT_SUPPORTED;

		me->temperature_control_profile.profile_heater_set &= ~(1 << heater_idx);
		return ERROR_OK;

}

uint8_t temperature_control_profile_heater_profile_get( temperature_control_task_t *const me)
{
	return me->temperature_control_profile.profile_heater_set;
}

uint32_t temperature_control_profile_ntc_register( temperature_control_task_t *const me, uint8_t pri_ntc_idx, uint8_t sec_ntc_idx)
{
	if (pri_ntc_idx > 7) return ERROR_NOT_SUPPORTED;
	if (sec_ntc_idx > 7) return ERROR_NOT_SUPPORTED;
	me->temperature_control_profile.pri_NTC_idx = pri_ntc_idx;
	me->temperature_control_profile.sec_NTC_idx = sec_ntc_idx;
	return ERROR_OK;
}

void temperature_control_profile_temp_lim_set(temperature_control_task_t *const me, int16_t	max_temp, int16_t min_temp)
{
	me->temperature_control_profile.profile_max_temp = max_temp;
	me->temperature_control_profile.profile_min_temp = min_temp;
}

void temperature_control_profile_temp_lim_get(temperature_control_task_t *const me, int16_t * limt_temp_ptr)
{
	limt_temp_ptr[0] = me->temperature_control_profile.profile_max_temp;
	limt_temp_ptr[1] = me->temperature_control_profile.profile_min_temp;
}

void temperature_control_profile_setpoint_set(temperature_control_task_t *const me, int16_t	setpoint)
{
	me->temperature_control_profile.setpoint = setpoint;
}
int16_t temperature_control_profile_setpoint_get(temperature_control_task_t *const me)
{
	return me->temperature_control_profile.setpoint;
}
void temperature_control_profile_ntc_get( temperature_control_task_t *const me, uint8_t * ntc_ref_ptr)
{
	ntc_ref_ptr[0] = me->temperature_control_profile.pri_NTC_idx;
	ntc_ref_ptr[1] = me->temperature_control_profile.sec_NTC_idx;
}

uint32_t temperature_control_profile_set_auto_recover(temperature_control_task_t *const me, uint32_t status)
{
	if(status > 1) return ERROR_NOT_SUPPORTED;
	me->temperature_control_profile.auto_recover = status;
	return ERROR_OK;
}

uint32_t temperature_control_profile_get_auto_recover(temperature_control_task_t *const me)
{
	return me->temperature_control_profile.auto_recover;
}

uint32_t temperature_control_tec_manual_set_output( temperature_control_task_t *const me,uint32_t tec_idx, uint32_t tec_dir, uint16_t volt_mV)
{
	if (tec_idx > 3) return ERROR_NOT_SUPPORTED;

	uint8_t tec_status = me->tec_table[tec_idx]->status;
	if (tec_status & (1 << TEC_INIT_POS))	//tec not init
	{
		if (lt8722_init(me->tec_table[tec_idx])) return ERROR_FAIL;
	}
	if (lt8722_set_output_voltage_channel(me->tec_table[tec_idx],tec_dir, (int64_t)volt_mV*1000000)) return ERROR_FAIL;
		return ERROR_OK;
}

void temperature_control_power_control(temperature_control_task_t * const me, uint32_t status)
{
	if (status)
	{
		bsp_temperature_power_on();
		me->tec_heater_power_status = 1;
	}
	else
	{
		bsp_temperature_power_off();
		me->tec_heater_power_status = 0;
	}
}
bool temperature_control_is_powered_on(temperature_control_task_t * const me)
{
	return me->tec_heater_power_status;
}

uint32_t temperature_control_set_profile(temperature_control_task_t *const me, uint16_t target_temp, uint16_t min_temp, uint16_t max_temp, \
		uint8_t pri_ntc_id, uint8_t sec_ntc_id, uint8_t auto_recover, uint8_t tec_pos_mask, uint8_t htr_pos_mask, uint16_t tec_mV, uint8_t htr_duty)
{
	temperature_control_man_mode_set(&temperature_control_task_inst);
	me->temperature_control_profile.setpoint = target_temp;
	me->temperature_control_profile.profile_min_temp = min_temp;
	me->temperature_control_profile.profile_max_temp = max_temp;
	me->temperature_control_profile.pri_NTC_idx = pri_ntc_id;
	me->temperature_control_profile.sec_NTC_idx = sec_ntc_id;
	me->temperature_control_profile.auto_recover = auto_recover;
	me->temperature_control_profile.profile_tec_set = tec_pos_mask;
	me->temperature_control_profile.profile_tec_set = htr_pos_mask;
	me->temperature_control_profile.tec_voltage = tec_mV;
	me->temperature_control_profile.heater_duty_cycle = htr_duty;
	return ERROR_OK;
}
