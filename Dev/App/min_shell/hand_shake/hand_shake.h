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

#define handshake_min_ready()	(LL_GPIO_ResetOutputPin(MinBusy_GPIO_Port, MinBusy_Pin))
#define handshake_min_busy()	(LL_GPIO_SetOutputPin(MinBusy_GPIO_Port, MinBusy_Pin))

#define handshake_spi_ready()	(LL_GPIO_SetOutputPin(DReady_GPIO_Port, DReady_Pin))
#define handshake_spi_busy()	(LL_GPIO_ResetOutputPin(DReady_GPIO_Port, DReady_Pin))

#define handshake_spi_check_finish()  (LL_GPIO_IsInputPinSet(ReadDone_GPIO_Port, ReadDone_Pin))


#endif /* APP_MIN_SHELL_HAND_SHAKE_HAND_SHAKE_H_ */
