/*
 * hand_shake.c
 *
 *  Created on: Jul 21, 2025
 *      Author: HTSANG
 */

#include "hand_shake.h"


void MinBusy_Pin_Clear(void) {
	LL_GPIO_ResetOutputPin(MinBusy_GPIO_Port, MinBusy_Pin);
}

void MinBusy_Pin_Set(void) {
	LL_GPIO_SetOutputPin(MinBusy_GPIO_Port, MinBusy_Pin);
}

void DReady_Pin_Clear(void) {
	LL_GPIO_ResetOutputPin(DReady_GPIO_Port, DReady_Pin);
}

void DReady_Pin_Set(void) {
	LL_GPIO_SetOutputPin(DReady_GPIO_Port, DReady_Pin);
}

