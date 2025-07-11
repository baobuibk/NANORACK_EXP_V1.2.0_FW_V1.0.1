/*
 * date_time.c
 *
 *  Created on: Feb 8, 2025
 *      Author: CAO HIEU
 */
#include "date_time.h"
#include "scheduler.h"

void SoftTime_Task_Update(void);

static s_DateTime s_RealTimeClock_context = {1, 1, 0, 0, 0, 0};

static struct {
    uint32_t days;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
} s_WorkingTimeClock_context = {0, 0, 0, 0};

typedef struct s_SoftTime_TaskContextTypedef
{
	SCH_TASK_HANDLE               taskHandle;
	SCH_TaskPropertyTypedef       taskProperty;
	uint32_t                      taskTick;
} s_SoftTime_TaskContextTypedef;

static s_SoftTime_TaskContextTypedef           s_SoftTime_task_context =
{
	SCH_INVALID_TASK_HANDLE,                 // Will be updated by Schedular
	{
		SCH_TASK_SYNC,                      // taskType;
		SCH_TASK_PRIO_0,                    // taskPriority;
		1000,                                // taskPeriodInMS;
		SoftTime_Task_Update,                // taskFunction;
		0							//taskTick
	},
};

void SoftTime_Task_Update(void) {
    s_RealTimeClock_context.second++;
    if (s_RealTimeClock_context.second >= 60) {
        s_RealTimeClock_context.second = 0;
        s_RealTimeClock_context.minute++;
        if (s_RealTimeClock_context.minute >= 60) {
            s_RealTimeClock_context.minute = 0;
            s_RealTimeClock_context.hour++;
            if (s_RealTimeClock_context.hour >= 24) {
                s_RealTimeClock_context.hour = 0;
                s_RealTimeClock_context.day++;

                static const uint8_t daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
                uint8_t leapYear = ((s_RealTimeClock_context.year % 4 == 0) &&
                                   ((s_RealTimeClock_context.year % 100 != 0) ||
                                		   (s_RealTimeClock_context.year % 400 == 0))) ? 1 : 0;

                uint8_t maxDays =
                		(s_RealTimeClock_context.month == 2) ? (28 + leapYear) : daysInMonth[s_RealTimeClock_context.month - 1];

                if (s_RealTimeClock_context.day > maxDays) {
                    s_RealTimeClock_context.day = 1;
                    s_RealTimeClock_context.month++;
                    if (s_RealTimeClock_context.month > 12) {
                        s_RealTimeClock_context.month = 1;
                        s_RealTimeClock_context.year++;
                        if (s_RealTimeClock_context.year > 99) {
                            s_RealTimeClock_context.year = 0;
                        }
                    }
                }
            }
        }
    }

    s_WorkingTimeClock_context.seconds++;
    if (s_WorkingTimeClock_context.seconds >= 60) {
        s_WorkingTimeClock_context.seconds = 0;
        s_WorkingTimeClock_context.minutes++;
        if (s_WorkingTimeClock_context.minutes >= 60) {
            s_WorkingTimeClock_context.minutes = 0;
            s_WorkingTimeClock_context.hours++;
            if (s_WorkingTimeClock_context.hours >= 24) {
                s_WorkingTimeClock_context.hours = 0;
                s_WorkingTimeClock_context.days++;
            }
        }
    }
}

void SoftTime_CreateTask(void) {
    s_RealTimeClock_context.year = 0;  // 2000
    s_RealTimeClock_context.month = 1; // January
    s_RealTimeClock_context.day = 1;
    s_RealTimeClock_context.hour = 0;
    s_RealTimeClock_context.minute = 0;
    s_RealTimeClock_context.second = 0;

    s_WorkingTimeClock_context.days = 0;
    s_WorkingTimeClock_context.hours = 0;
    s_WorkingTimeClock_context.minutes = 0;
    s_WorkingTimeClock_context.seconds = 0;

    SCH_TASK_CreateTask(&s_SoftTime_task_context.taskHandle, &s_SoftTime_task_context.taskProperty);
}

void DateTime_GetRTC(s_DateTime *dateTime) {
    if (dateTime == NULL) return;
    *dateTime = s_RealTimeClock_context;
}

void DateTime_SetRTC(const s_DateTime *dateTime) {
    if (dateTime == NULL) return;
    s_RealTimeClock_context = *dateTime;
}
/*@usage:
 *  s_DateTime newTime = {15, 10, 23, 14, 30, 0}; // 15/10/2023 14:30:00
 *  DateTime_SetRTC(&newTime);
 */

void DateTime_GetWorkingTime(uint32_t *days, uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
    if (days) *days = s_WorkingTimeClock_context.days;
    if (hours) *hours = s_WorkingTimeClock_context.hours;
    if (minutes) *minutes = s_WorkingTimeClock_context.minutes;
    if (seconds) *seconds = s_WorkingTimeClock_context.seconds;
}
