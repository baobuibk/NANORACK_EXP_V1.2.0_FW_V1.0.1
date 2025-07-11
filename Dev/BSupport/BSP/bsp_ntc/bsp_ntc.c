/*
 * bsp_ntc.c
 *
 *  Created on: Jun 22, 2025
 *      Author: Admin
 */


#include "bsp_ntc.h"
#include "ntc.h"
#include "stm32f7xx_ll_dma.h"
#include "app_signals.h"

#include "adc_monitor.h"
extern monitor_task_t monitor_task_inst ;


static uint16_t ntc_ADC_value[8] = {0,0,0,0,0,0,0,0};
static uint16_t ntc_ADC_value_average[8] = {0,0,0,0,0,0,0,0};
static uint32_t sample_count = 0;

static monitor_evt_t const ntc_adc_evt = {.super = {.sig = EVT_MONITOR_NTC_ADC_COMPLETED} };

void bsp_ntc_adc_init(void)
{
	ntc_adc_dma_init(ntc_ADC_value, 8);
}

void bsp_ntc_trigger_adc(void)
{
	ntc_adc_trigger();
}

int16_t bsp_ntc_get_temperature(uint32_t ntc_channel)
{
	return ntc_convert(ntc_ADC_value_average[ntc_channel]);
}


void DMA2_Stream0_IRQHandler(void)
{
	if (LL_DMA_IsActiveFlag_TC0(DMA2))
	{
		LL_DMA_ClearFlag_TC0(DMA2);
	}

	if (sample_count == 0)
	{
		for (uint32_t i = 0; i < 8; i++ ) ntc_ADC_value_average[i] = ntc_ADC_value[i];
		sample_count++;
	}
	else if(sample_count < 10)
	{
		for (uint32_t i = 0; i< 8; i++ ) ntc_ADC_value_average[i] = (ntc_ADC_value[i] + ntc_ADC_value_average[i])/2;
		sample_count++;
	}
	else
	{
		sample_count = 0;
		SST_Task_post(&monitor_task_inst.super, (SST_Evt *)&ntc_adc_evt); //post to temperature monitor task
	}
}

