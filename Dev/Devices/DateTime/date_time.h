/*
 * date_time.h
 *
 *  Created on: Feb 8, 2025
 *      Author: CAO HIEU
 */

#ifndef DEVICES_DATETIME_DATE_TIME_H_
#define DEVICES_DATETIME_DATE_TIME_H_
#include "stdint.h"

typedef struct {
	uint8_t day;    //!< Day: Starting at 1 for the first day
	uint8_t month;  //!< Month: Starting at 1 for January
	uint8_t year;  //!< Year in format YY (2000 - 2099)
	uint8_t hour;   //!< Hour
	uint8_t minute; //!< Minute
	uint8_t second; //!< Second
} s_DateTime;

void SoftTime_CreateTask(void);
void DateTime_GetWorkingTime(uint32_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds);
void DateTime_SetRTC(const s_DateTime *dateTime);
void DateTime_GetRTC(s_DateTime *dateTime);

#endif /* DEVICES_DATETIME_DATE_TIME_H_ */
