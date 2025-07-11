/*
 * tec_control.h
 *
 *  Created on: Jun 16, 2025
 *      Author: Admin
 */

#ifndef APP_TEMPERATURE_CONTROL_TEMPERATURE_CONTROL_TASK_H_
#define APP_TEMPERATURE_CONTROL_TEMPERATURE_CONTROL_TASK_H_

#define TEMPERATURE_CONTROl_COMMAND_PAYLOAD_LENGTH	15
#include "sst.h"
#include "fsm.h"

typedef struct temperature_control_task_t temperature_control_task_t;
typedef struct temperature_control_evt_t temperature_control_evt_t;
typedef struct temperature_control_task_init_t temperature_control_task_init_t;
typedef struct single_tec_data_t single_tec_data_t;
typedef struct temperature_control_profile_t temperature_control_profile_t;
typedef struct temperature_tec_ovr_profile_t temperature_tec_ovr_profile_t;
#define TEC_COOL 0
#define TEC_HEAT 1

enum {TEMPERATURE_POWER_OFF,TEMPERATURE_POWER_ON};
enum {STOPPED, MANUAL, COOLING_TEC, HEATING_HEATER, HEATING_HEATER_TEC, OFF_WAIT_HEAT};
enum {TEC_POWER, TEMPERATURE_MAN_CONTROL, HEATER_MAN_CONTROL, CONFIGURE_PROFILE,TEMPERATURE_AUTOMODE_START,TEMPERATURE_MANMODE_START};
typedef state_t (*temperature_control_state_handler_t)(temperature_control_task_t * const me, temperature_control_evt_t  const * const e);
struct single_tec_data_t {
	uint8_t tec_status_reg;
	uint8_t tec_volt;

};
struct temperature_control_evt_t{
	SST_Evt super;
	uint8_t cmd;
	uint8_t payload[TEMPERATURE_CONTROl_COMMAND_PAYLOAD_LENGTH];

};


struct temperature_control_profile_t{
	uint8_t	NTC_idx;	//NTC for reference temperature
	uint8_t profile_tec_set;	//bit=1 is enabled, lower 4 bit for tec
	uint8_t profile_heater_set;	//bit=1 is enabled, lower 4 bit for heater
	uint8_t heater_duty_cycle; //duty cycle for each heater, 0-100%
	int16_t setpoint;
    uint16_t tec_voltage; // output voltage for each tec
    };

struct temperature_tec_ovr_profile_t{
	uint8_t profile_tec_ovr_set;	//index of tec ovr set
    uint16_t tec_ovr_voltage; // output voltage for each tec
    };

struct temperature_control_task_t {
    SST_Task super;
    temperature_control_state_handler_t state; /* the "state variable" */
    uint32_t state_num;
    uint32_t	counter;
    SST_TimeEvt temperature_control_task_timeout_timer;
    uint8_t		tec_heater_power_status;
    struct lt8722_dev * tec_table[4]; //pointer to 4 TECs
    uint8_t		tec_inited; //low 4 bits for TEC init status, high 4 bit for output status

    temperature_control_profile_t temperature_control_profile;
    temperature_tec_ovr_profile_t temperature_tec_ovr_profile;
} ;

struct temperature_control_task_init_t {
	temperature_control_state_handler_t init_state;
	temperature_control_evt_t * current_evt;
	circular_buffer_t * temperature_control_task_event_buffer;
	struct lt8722_dev * tec_table[4];
};

void temperature_control_task_singleton_ctor(void);
void temperature_control_task_start(uint8_t priority);



void temperature_control_power_control(temperature_control_task_t * const me, uint32_t status);
bool temperature_control_is_powered_on(temperature_control_task_t * const me);

bool temperature_control_is_tec_inited(uint32_t channel);
void temperature_control_setter_tec_disable(uint32_t channel);
uint32_t temperature_control_setter_tec_init(uint32_t channel);

uint32_t temperature_control_man_mode_set(temperature_control_task_t *const me);

void temperature_control_auto_tec_set_output(temperature_control_task_t * const me,uint32_t tec_dir);
void temperature_control_tec_output_disable_all(temperature_control_task_t * const me);
void temperature_control_tec_init_all(temperature_control_task_t * const me);
uint32_t temperature_control_tec_init(temperature_control_task_t * const me,uint32_t tec_idx);
uint32_t temperature_control_tec_enable_output(temperature_control_task_t * const me,uint32_t tec_idx, bool value);
void temperature_control_heater_disable_all(temperature_control_task_t * const me);
void temperature_control_enable_heater(temperature_control_task_t * const me);
void temperature_control_tec_disable_all(temperature_control_task_t * const me);
void temperature_control_auto_tec_enable_output(temperature_control_task_t * const me);
bool temperature_control_is_in_man_state(temperature_control_task_t * const me);

uint32_t temperature_control_profile_tec_register(temperature_control_task_t *const me,uint8_t tec_idx);
uint32_t temperature_control_profile_tec_unregister(temperature_control_task_t *const me,uint8_t tec_idx);
uint8_t temperature_control_profile_tec_get(temperature_control_task_t *const me);
uint32_t temperature_control_profile_tec_voltage_set(temperature_control_task_t *const me, uint16_t	volt_mv);
uint16_t temperature_control_profile_tec_voltage_get( temperature_control_task_t *const me);
void temperature_control_profile_setpoint_set(temperature_control_task_t *const me, int16_t	setpoint);
int16_t temperature_control_profile_setpoint_get(temperature_control_task_t *const me);

uint32_t temperature_profile_tec_ovr_register(temperature_control_task_t *const me, uint8_t tec_idx);
uint32_t temperature_profile_tec_ovr_voltage_set(temperature_control_task_t *const me, uint16_t	volt_mv);
uint32_t temperature_profile_tec_ovr_enable(temperature_control_task_t *const me);
uint32_t temperature_profile_tec_ovr_disable(temperature_control_task_t *const me);
uint8_t temperature_profile_tec_ovr_get(temperature_control_task_t *const me);
uint16_t temperature_profile_tec_ovr_get_voltage(temperature_control_task_t *const me);

uint32_t temperature_control_tec_manual_set_output( temperature_control_task_t *const me,uint32_t tec_idx, uint32_t tec_dir, uint16_t volt_mV);
uint32_t temperature_control_auto_mode_set(temperature_control_task_t *const me);
uint16_t temperature_control_profile_tec_voltage_get( temperature_control_task_t *const me);




uint32_t temperature_control_profile_heater_duty_set( temperature_control_task_t *const me,uint8_t	duty);
uint8_t temperature_control_profile_heater_duty_get( temperature_control_task_t *const me);
uint32_t temperature_control_profile_heater_register( temperature_control_task_t *const me,uint8_t heater_idx);
uint32_t temperature_control_profile_heater_unregister(temperature_control_task_t *const me,uint8_t heater_idx);
uint8_t temperature_control_profile_heater_profile_get( temperature_control_task_t *const me);

uint32_t temperature_control_profile_ntc_register( temperature_control_task_t *const me,uint8_t ntc_idx);
uint8_t temperature_control_profile_ntc_get( temperature_control_task_t *const me);


#endif /* APP_TEMPERATURE_CONTROL_TEMPERATURE_CONTROL_TASK_H_ */
