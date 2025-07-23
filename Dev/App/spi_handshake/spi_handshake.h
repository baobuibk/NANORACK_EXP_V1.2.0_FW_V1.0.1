/*
 * spi_handshake.h
 *
 *  Created on: Jul 23, 2025
 *      Author: HTSANG
 */

#ifndef APP_SPI_HANDSHAKE_SPI_HANDSHAKE_H_
#define APP_SPI_HANDSHAKE_SPI_HANDSHAKE_H_

#include "sst.h"
#include "fsm.h"

typedef struct spi_handshake_task_t spi_handshake_task_t;
typedef struct spi_handshake_evt_t  spi_handshake_evt_t;
typedef struct spi_handshake_task_init_t  spi_handshake_task_init_t;

typedef state_t (* spi_handshake_task_handler_t )(spi_handshake_task_t * const me, spi_handshake_evt_t * const e);

struct spi_handshake_evt_t{
	SST_Evt super;
};
struct spi_handshake_task_t{
	SST_Task super;
	SST_TimeEvt spi_handshake_poll_check_finish_timer;
	spi_handshake_task_handler_t state; /* the "state variable" */
	uint32_t count;
};

struct spi_handshake_task_init_t {
	spi_handshake_task_handler_t init_state;
	spi_handshake_evt_t * current_evt;
	circular_buffer_t * event_buffer;
};


void spi_handshake_task_singleton_ctor(void);
void spi_handshake_task_start(uint8_t priority);


void spi_handshake_task_data_ready(spi_handshake_task_t *const me);


#endif /* APP_SPI_HANDSHAKE_SPI_HANDSHAKE_H_ */
