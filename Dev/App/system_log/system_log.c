/*
 * system_log.c
 *
 *  Created on: Jul 14, 2025
 *      Author: Admin
 */
#include "lwl.h"
#include "adc_monitor.h"
#include "app_signals.h"
#include "system_log.h"
#include "error_codes.h"
#include "dbc_assert.h"

DBC_MODULE_NAME("system_log")

system_log_task_t system_log_task_inst;
#define SYSTEM_LOG_NUM_EVENT 1
#define DEFAULT_POLL_TIME 1000


system_log_evt_t system_log_current_event = {0};
system_log_evt_t system_log_event_buffer[SYSTEM_LOG_NUM_EVENT];
circular_buffer_t system_log_event_queue = {0};

static void system_log_task_init(system_log_task_t * const me, system_log_evt_t * const e);
//static void system_log_task_dispatch(system_log_task_t * const me, system_log_evt_t * const e);
static state_t system_log_normal_state_handler(system_log_task_t * const me, system_log_evt_t * const e);

void system_log_house_keeping(void);

void system_log_task_ctor(system_log_task_t * const me, system_log_task_init_t * const init)
{
	SST_Task_ctor(&me->super, (SST_Handler)system_log_task_init, (SST_Handler)system_log_normal_state_handler, (SST_Evt*)init->current_evt, init->event_buffer);
	SST_TimeEvt_ctor(&me->system_log_timer, EVT_SYSTEM_LOG_POLL, &me->super);
	me->state = init->init_state;
	me->interval = DEFAULT_POLL_TIME;
	SST_TimeEvt_disarm(&me->system_log_timer);
}

void system_log_task_ctor_singleton()
{
	system_log_task_init_t init = {
			.current_evt = &system_log_current_event,
			.event_buffer = &system_log_event_queue,
			.init_state = system_log_normal_state_handler
	};
	circular_buffer_init(&system_log_event_queue, (uint8_t *)&system_log_event_buffer, sizeof(system_log_event_buffer), SYSTEM_LOG_NUM_EVENT, sizeof(system_log_evt_t));
	lwl_stdio_init();
	system_log_task_ctor(&system_log_task_inst,&init);

}

static void system_log_task_init(system_log_task_t * const me, system_log_evt_t * const e)
{
	lwl_start();
	SST_TimeEvt_arm(&me->system_log_timer, me->interval, me->interval);
}

void system_log_task_start(uint8_t priority)
{
	SST_Task_start(&system_log_task_inst.super, priority);
}
static state_t system_log_normal_state_handler(system_log_task_t * const me, system_log_evt_t * const e)
{
	// assume the only event is SYSTEM_LOG_POLL
	switch (e->super.sig)
	{
	case EVT_SYSTEM_LOG_POLL:
		system_log_house_keeping();
	}
	return HANDLED_STATUS;
}


void system_log_house_keeping()
{
	int16_t ntc_temp[8] = {0};
//	temperature_monitor_get_all_ntc_temperature(&ntc_temp[0]);
	LWL(TIMESTAMP, LWL_4(SST_getTick()));
	LWL(TEMPERATURE_NTC,LWL_2(ntc_temp[0]), LWL_2(ntc_temp[1]),LWL_2(ntc_temp[2]),LWL_2(ntc_temp[3]),\
						LWL_2(ntc_temp[4]),LWL_2(ntc_temp[5]),LWL_2(ntc_temp[6]),LWL_2(ntc_temp[7]));
	for (uint32_t i=0;i<8;i++)
		LWL(TEMPERATURE_SINGLE_NTC,LWL_1(i),LWL_2(temperature_monitor_get_ntc_temperature(i)));


}

void system_log_set_interval(uint32_t interval)
{
	system_log_task_inst.interval = interval;
}
void system_log_enable()
{
	SST_TimeEvt_arm(&system_log_task_inst.system_log_timer, system_log_task_inst.interval, system_log_task_inst.interval);

}
void system_log_disable()
{
	SST_TimeEvt_disarm(&system_log_task_inst.system_log_timer);

}
