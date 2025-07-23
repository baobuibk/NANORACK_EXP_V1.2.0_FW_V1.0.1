/*
 * spi_handshake.c
 *
 *  Created on: Jul 23, 2025
 *      Author: HTSANG
 */


#include "spi_handshake.h"
#include "app_signals.h"
#include "hand_shake/hand_shake.h"

//DBC_MODULE_NAME("spi_handshake")

#define SPI_HANDSHAKE_NUM_EVENT 		5
#define ONE_SECOND_POLL_TIME 			1000
#define TIME_OUT_SPI_TRANS				100		//Unit: seconds

spi_handshake_task_t spi_handshake_task_inst;
circular_buffer_t spi_handshake_event_queue = {0};
static spi_handshake_evt_t spi_handshake_current_event = {0};
static spi_handshake_evt_t spi_handshake_event_buffer[SPI_HANDSHAKE_NUM_EVENT];

static void spi_handshake_task_init(spi_handshake_task_t * const me, spi_handshake_evt_t * const e);
static void spi_handshake_task_dispatch(spi_handshake_task_t * const me, spi_handshake_evt_t * const e);
static state_t spi_handshake_handler(spi_handshake_task_t * const me, spi_handshake_evt_t * const e);

static void spi_handshake_task_finish(spi_handshake_task_t *const me);

void spi_handshake_task_ctor(spi_handshake_task_t * const me, spi_handshake_task_init_t * const init)
{
	SST_Task_ctor(&me->super, (SST_Handler)spi_handshake_task_init, (SST_Handler)spi_handshake_task_dispatch, (SST_Evt*)init->current_evt, init->event_buffer);
	SST_TimeEvt_ctor(&me->spi_handshake_poll_check_finish_timer, EVT_SPI_HANDSHAKE_POLL_CHECK_FINISH, &me->super);
	me->state = init->init_state;
	me->count = 0;
	SST_TimeEvt_disarm(&me->spi_handshake_poll_check_finish_timer);
}

void spi_handshake_task_singleton_ctor()
{
	spi_handshake_task_init_t init = {
			.current_evt = &spi_handshake_current_event,
			.event_buffer = &spi_handshake_event_queue,
			.init_state = spi_handshake_handler
	};
	circular_buffer_init(&spi_handshake_event_queue, (uint8_t *)&spi_handshake_event_buffer, sizeof(spi_handshake_event_buffer), SPI_HANDSHAKE_NUM_EVENT, sizeof(spi_handshake_evt_t));
	spi_handshake_task_ctor(&spi_handshake_task_inst,&init);
}

static void spi_handshake_task_init(spi_handshake_task_t * const me, spi_handshake_evt_t * const e)
{
	handshake_spi_busy();
}

static void spi_handshake_task_dispatch(spi_handshake_task_t * const me, spi_handshake_evt_t * const e)
{
    (me->state)(me, e);
}

void spi_handshake_task_start(uint8_t priority)
{
	SST_Task_start(&spi_handshake_task_inst.super, priority);
}

static state_t spi_handshake_handler(spi_handshake_task_t * const me, spi_handshake_evt_t * const e)
{
	switch (e->super.sig)
	{
		case EVT_SPI_HANDSHAKE_POLL_CHECK_FINISH:
			me->count ++;
			if (handshake_spi_check_finish() || me->count > TIME_OUT_SPI_TRANS)
			{
				me->count = 0;
				spi_handshake_task_finish(me);
			}
			return HANDLED_STATUS;
		case EVT_SPI_HANDSHAKE_FINISH:
		case EVT_SPI_HANDSHAKE_TIMEOUT:
			SST_TimeEvt_disarm(&me->spi_handshake_poll_check_finish_timer);
			handshake_spi_busy();
			return HANDLED_STATUS;
		default:
			return IGNORED_STATUS;
	}
}

void spi_handshake_task_data_ready(spi_handshake_task_t *const me)
{
	handshake_spi_ready();
	SST_TimeEvt_arm(&me->spi_handshake_poll_check_finish_timer, ONE_SECOND_POLL_TIME, ONE_SECOND_POLL_TIME);
}

void spi_handshake_task_finish(spi_handshake_task_t *const me)
{
	spi_handshake_evt_t spi_finish_evt = {.super = {.sig = EVT_SPI_HANDSHAKE_FINISH}, };
	SST_Task_post(&me->super, (SST_Evt *)&spi_finish_evt);
}
