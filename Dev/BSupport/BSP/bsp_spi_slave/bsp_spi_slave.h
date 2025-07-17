/*
 * bsp_spi_slave.h
 *
 *  Created on: Jul 15, 2025
 *      Author: HTSANG
 */

#ifndef BSUPPORT_BSP_BSP_SPI_SLAVE_BSP_SPI_SLAVE_H_
#define BSUPPORT_BSP_BSP_SPI_SLAVE_BSP_SPI_SLAVE_H_

#include "board.h"
#include "basetypedef.h"
#include "stdbool.h"
#include "stdint.h"


#define SAMPLE_BUFFER_SIZE		(16* 1024)		//16KB

typedef enum {
	SPI_TRANSFER_PREPARE,
	SPI_TRANSFER_WAIT,
	SPI_TRANSFER_COMPLETE,
	SPI_TRANSFER_ERROR
} SPI_TransferState_t;

typedef struct {
	uint8_t sample_buffer[SAMPLE_BUFFER_SIZE];
	uint16_t crc; // CRC16-XMODEM of the data
	_Bool is_valid; // Flag indicating if context is valid
} DataProcessContext_t;

typedef struct {
	SPI_TransferState_t transfer_state;
	DataProcessContext_t data_context;
	_Bool is_initialized;
} SPI_SlaveDevice_t;

SPI_SlaveDevice_t* SPI_SlaveDevice_GetHandle(void);
Std_ReturnType SPI_SlaveDevice_Init(void);
Std_ReturnType SPI_SlaveDevice_CollectData(void);
Std_ReturnType SPI_SlaveDevice_GetDataInfo(DataProcessContext_t *context);
Std_ReturnType SPI_SlaveDevice_ReinitDMA(void);
Std_ReturnType SPI_SlaveDevice_Disable(void);
SPI_TransferState_t SPI_SlaveDevice_GetTransferState(void);
void SPI_SlaveDevice_SetTransferState(SPI_TransferState_t state);
uint16_t SPI_SlaveDevide_GetDataCRC(void);

#endif /* BSUPPORT_BSP_BSP_SPI_SLAVE_BSP_SPI_SLAVE_H_ */
