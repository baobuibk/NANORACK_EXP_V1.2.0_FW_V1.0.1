/*
 * status_led.h
 *
 *  Created on: Nov 20, 2024
 *      Author: SANG HUYNH
 */

#ifndef LED_STATUS_LED_H_
#define LED_STATUS_LED_H_

typedef	enum Status_Led_StateTypedef {EXP_POWERUP=0, EXP_NORMAL, EXP_ERROR,} Status_Led_StateTypedef;

void  LED_Status_Init(void);
void  LED_Status_CreateTask(void);

#endif /* LED_STATUS_LED_H_ */
