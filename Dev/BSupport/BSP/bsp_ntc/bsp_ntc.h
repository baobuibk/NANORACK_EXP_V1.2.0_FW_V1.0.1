/*
 * bsp_ntc.h
 *
 *  Created on: Jun 22, 2025
 *      Author: Admin
 */

#ifndef BSUPPORT_BSP_NTC_BSP_NTC_H_
#define BSUPPORT_BSP_NTC_BSP_NTC_H_

#include "board.h"
void bsp_ntc_trigger_adc(void);
void bsp_ntc_adc_init(void);
int16_t bsp_ntc_get_temperature(uint32_t ntc_channel);
#endif /* BSUPPORT_BSP_NTC_BSP_NTC_H_ */
