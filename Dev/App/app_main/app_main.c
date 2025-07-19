/*
 * app_main.c
 *
 *  Created on: Jun 11, 2025
 *      Author: Admin
 */
#include "stddef.h"
#include "embedded_cli.h"
#include "shell.h"
#include "temperature_control.h"
#include "experiment_task.h"
#include "min_shell.h"
#include "error_codes.h"
#include "app_signals.h"
#include "dbc_assert.h"
#include "adc_monitor.h"
#include "photodiode_cool.h"
#include "system_log.h"

//DBC_MODULE_NAME("app_main")




void app_init(void)
{
	shell_task_ctor_singleton();
	temperature_control_task_singleton_ctor();
	monitor_task_ctor_singleton();
	experiment_task_singleton_ctor();
	tec_ovr_control_task_singleton_ctor();
	min_shell_task_ctor_singleton();
	system_log_task_ctor_singleton();
}

void app_start(void)
{
	experiment_task_start(1);
	shell_task_start(4);
	temperature_control_task_start(2);
	monitor_task_start(3);
	tec_ovr_control_task_start(5);
	system_log_task_start(6);
	min_shell_task_start(7);
	return;
}
void app_run(void)
{
	SST_Task_run();
}
