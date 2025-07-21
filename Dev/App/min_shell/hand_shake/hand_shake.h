/*
 * hand_shake.h
 *
 *  Created on: Jul 21, 2025
 *      Author: HTSANG
 */

#ifndef APP_MIN_SHELL_HAND_SHAKE_HAND_SHAKE_H_
#define APP_MIN_SHELL_HAND_SHAKE_HAND_SHAKE_H_

#include "board.h"


#define MinBusy_GPIO_Port		GPIOB
#define MinBusy_Pin				LL_GPIO_PIN_11

#define DReady_GPIO_Port		GPIOC
#define DReady_Pin				LL_GPIO_PIN_8

#define ReadDone_GPIO_Port		GPIOC
#define ReadDone_Pin			LL_GPIO_PIN_9

#define min_handshake_ready()	MinBusy_Pin_Clear()
#define min_handshake_busy()	MinBusy_Pin_Set()

#define spi_handshake_ready()	DReady_Pin_Set()
#define spi_handshake_busy()	DReady_Pin_Clear()


void MinBusy_Pin_Clear(void);
void MinBusy_Pin_Set(void);

void DReady_Pin_Clear(void);
void DReady_Pin_Set(void);


#endif /* APP_MIN_SHELL_HAND_SHAKE_HAND_SHAKE_H_ */
