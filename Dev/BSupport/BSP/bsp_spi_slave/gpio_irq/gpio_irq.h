/*
 * gpio_irq.h
 *
 *  Created on: Jul 15, 2025
 *      Author: HTSANG
 */

#ifndef BSUPPORT_BSP_BSP_SPI_SLAVE_GPIO_IRQ_GPIO_IRQ_H_
#define BSUPPORT_BSP_BSP_SPI_SLAVE_GPIO_IRQ_GPIO_IRQ_H_

#include "board.h"

#define DReady_GPIO_Port		GPIOC
#define DReady_Pin				LL_GPIO_PIN_9

#define GetDone_GPIO_Port		GPIOC
#define GetDone_Pin				LL_GPIO_PIN_8

#define GPIO_IRQ_Init()  		DReady_Pin_Clear()
#define GPIO_IRQ_Error()  		DReady_Pin_Clear()
#define GPIO_IRQ_TransDone()  	DReady_Pin_Clear()
#define GPIO_IRQ_DReady()  		DReady_Pin_Set()

void DReady_Pin_Clear(void);
void DReady_Pin_Set(void);

#endif /* BSUPPORT_BSP_BSP_SPI_SLAVE_GPIO_IRQ_GPIO_IRQ_H_ */
