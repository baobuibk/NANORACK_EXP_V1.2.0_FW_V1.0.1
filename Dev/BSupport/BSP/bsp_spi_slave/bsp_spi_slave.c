/*
 * bsp_spi_slave.c
 *
 *  Created on: Jul 15, 2025
 *      Author: HTSANG
 */

#include "bsp_spi_slave.h"
#include "stm32f7xx_ll_spi.h"
#include "stm32f7xx_ll_dma.h"
#include "bsp_spi_ram.h"


static SPI_SlaveDevice_t spi_device_instance = {
    .transfer_state = SPI_TRANSFER_WAIT,
	.data_context = {
		.p_tx_buffer = 0x00,
	    .crc = 0x0000,
	    .is_valid = false
	},
    .is_initialized = false
};

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

SPI_SlaveDevice_t* SPI_SlaveDevice_GetHandle(void)
{
    return &spi_device_instance;
}

Std_ReturnType SPI_SlaveDevice_Init(uint16_t * p_tx_buffer)
{
    if (spi_device_instance.is_initialized) {
        return E_OK;
    }

//    LL_DMA_SetMemorySize(DMA2, LL_DMA_STREAM_3, LL_DMA_MDATAALIGN_BYTE);
//    LL_DMA_SetPeriphAddress(DMA2, LL_DMA_STREAM_3, LL_SPI_DMA_GetRegAddr(DMA2));

    LL_DMA_EnableIT_TC(DMA2, LL_DMA_STREAM_3);
    LL_DMA_EnableIT_TE(DMA2, LL_DMA_STREAM_3);

    spi_device_instance.data_context.is_valid = false;
    spi_device_instance.is_initialized = true;
    spi_device_instance.transfer_state = SPI_TRANSFER_PREPARE;

    spi_device_instance.data_context.p_tx_buffer = p_tx_buffer;

    bsp_spi_debug_print("%02X  %02X  %02X  %02X  %02X  %02X  %02X  %02X\r\n",
    		*(p_tx_buffer + 0),
			*(p_tx_buffer + 1),
			*(p_tx_buffer + 2),
			*(p_tx_buffer + 3),
			*(p_tx_buffer + 4),
			*(p_tx_buffer + 5),
			*(p_tx_buffer + 6),
			*(p_tx_buffer + 7));

	return E_OK;
}

Std_ReturnType SPI_SlaveDevice_CollectData(void)
{
    if (!spi_device_instance.is_initialized) {
        return E_ERROR;
    }

	uint16_t crc = 0x0000;
	for (uint16_t i = 0; i < SAMPLE_BUFFER_SIZE; i++) {
		crc = UpdateCRC16_XMODEM(crc, *(spi_device_instance.data_context.p_tx_buffer + i));
	}

	spi_device_instance.data_context.crc = crc;
	spi_device_instance.data_context.is_valid = true;

	if (SPI_SlaveDevice_ReinitDMA() != E_OK) {
		return E_ERROR;
	}

    return E_OK;
}

Std_ReturnType SPI_SlaveDevice_GetDataInfo(DataProcessContext_t *context)
{
    if (!spi_device_instance.is_initialized || !spi_device_instance.data_context.is_valid) {
        return E_ERROR;
    }

    *context = spi_device_instance.data_context;
    return E_OK;
}

Std_ReturnType SPI_SlaveDevice_ReinitDMA()
{
    if (!spi_device_instance.is_initialized) {
        return E_ERROR;
    }

    LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_3);
    while (LL_DMA_IsEnabledStream(DMA2, LL_DMA_STREAM_3));

    LL_SPI_DisableDMAReq_TX(SPI1);
    LL_SPI_Disable(SPI1);

    LL_DMA_ClearFlag_TC3(DMA2);
    LL_DMA_ClearFlag_TE3(DMA2);

    LL_DMA_ConfigAddresses(DMA2, LL_DMA_STREAM_3, (uint32_t)spi_device_instance.data_context.p_tx_buffer, LL_SPI_DMA_GetRegAddr(SPI1), LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_3, SAMPLE_BUFFER_SIZE);
//    LL_DMA_SetDataLength(DMA2, LL_DMA_STREAM_3, 100);

    LL_SPI_EnableDMAReq_TX(SPI1);
    LL_SPI_Enable(SPI1);

    LL_DMA_EnableStream(DMA2, LL_DMA_STREAM_3);

    spi_device_instance.transfer_state = SPI_TRANSFER_WAIT;
    return E_OK;
}

Std_ReturnType SPI_SlaveDevice_Disable(void)
{
    if (!spi_device_instance.is_initialized) {
        return E_ERROR;
    }

    LL_DMA_DisableStream(DMA2, LL_DMA_STREAM_3);
    LL_SPI_DisableDMAReq_TX(SPI1);

    LL_DMA_ClearFlag_TC3(DMA2);
    LL_DMA_ClearFlag_TE3(DMA2);

    spi_device_instance.transfer_state = SPI_TRANSFER_WAIT;
    spi_device_instance.data_context.is_valid = false;
    return E_OK;
}

SPI_TransferState_t SPI_SlaveDevice_GetTransferState(void)
{
    return spi_device_instance.transfer_state;
}

void SPI_SlaveDevice_SetTransferState(SPI_TransferState_t state)
{
    spi_device_instance.transfer_state = state;
}

uint16_t SPI_SlaveDevide_GetDataCRC(void) {
	return spi_device_instance.data_context.crc;
}

void DMA2_Stream3_IRQHandler(void)
{
	if (LL_DMA_IsActiveFlag_TC3(DMA2)) {
		LL_DMA_ClearFlag_TC3(DMA2);
		SPI_SlaveDevice_SetTransferState(SPI_TRANSFER_COMPLETE);
		SPI_SlaveDevice_Disable();
	}
	if (LL_DMA_IsActiveFlag_TE3(DMA2)) {
		LL_DMA_ClearFlag_TE3(DMA2);
		SPI_SlaveDevice_SetTransferState(SPI_TRANSFER_ERROR);
		SPI_SlaveDevice_Disable();
	}
}
