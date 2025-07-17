/*
 * gpio_irq.c
 *
 *  Created on: Jul 15, 2025
 *      Author: HTSANG
 */


#include "gpio_irq.h"


void DReady_Pin_Clear(void) {
	LL_GPIO_ResetOutputPin(DReady_GPIO_Port, DReady_Pin);
}

void DReady_Pin_Set(void) {
	LL_GPIO_SetOutputPin(DReady_GPIO_Port, DReady_Pin);
}
