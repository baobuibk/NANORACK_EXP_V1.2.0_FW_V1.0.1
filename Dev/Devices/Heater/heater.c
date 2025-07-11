/*
 * heater.c
 *
 *  Created on: Dec 17, 2024
 *      Author: SANG HUYNH
 */


#include "heater.h"
#include "main.h"

void heater_set_duty_pwm_channel(uint8_t channel, uint16_t duty_pwm)
{
    if (duty_pwm > 999) duty_pwm = 999;
    switch (channel)
    {
        case 0:
            LL_TIM_OC_SetCompareCH1(TIM1, duty_pwm);
            break;
        case 1:
            LL_TIM_OC_SetCompareCH2(TIM1, duty_pwm);
            break;
        case 2:
            LL_TIM_OC_SetCompareCH3(TIM1, duty_pwm);
            break;
        case 3:
            LL_TIM_OC_SetCompareCH4(TIM1, duty_pwm);
            break;
        default:
            break;
    }
}
