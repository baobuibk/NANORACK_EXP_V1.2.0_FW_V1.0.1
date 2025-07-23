#ifndef _LWL_H_
#define _LWL_H_

/*
 * @brief Interface declaration of lwl module.
 *
 * See implementation file for information about this module.
 *
 * MIT License
 * 
 * Copyright (c) 2021 Eugene R Schroeder
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <stdbool.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Common macros
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Type definitions
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Public (global) externs
////////////////////////////////////////////////////////////////////////////////

// Following variable is global to allow efficient access by macros,
// but it is considered private.
// Macros for logging
#define LWL_1(a) (uint32_t)(a)
#define LWL_2(a) (uint32_t)(a), (uint32_t)(a) >> 8
#define LWL_3(a) (uint32_t)(a), (uint32_t)(a) >> 8, (uint32_t)(a) >> 16
#define LWL_4(a) (uint32_t)(a), (uint32_t)(a) >> 8, (uint32_t)(a) >> 16, (uint32_t)(a) >> 24


////////////////////////////////////////////////////////////////////////////////
// Public (global) function declarations
////////////////////////////////////////////////////////////////////////////////

// Core module interface functions.
// Define log message IDs
enum {
    INVALID = 0, // ID 0 is reserved (invalid)
    TIMESTAMP = 1,
    TEMPERATURE_NTC,
	TEMPERATURE_SINGLE_NTC,
    TEMPERATURE_ERROR,
    TEMPERATURE_AUTOMMODE_ON,
    TEMPERATURE_AUTOMMODE_TEC_ON,
    TEMPERATURE_AUTOMMODE_TEC_OFF,
    TEMPERATURE_TEC_ON,
    TEMPERATURE_TEC_OFF,
    TEMPERATURE_TEC_STATUS,
    TEMPERATURE_HEATER_ON,
    TEMPERATURE_HEATER_OFF,
    TEMPERATURE_HEATER_STATUS,
    TEMPERATURE_INTERNAL_LASER_ON,
    TEMPERATURE_INTERNAL_LASER_OFF,
    TEMPERATURE_EXTERNAL_LASER_ON,
    TEMPERATURE_EXTERNAL_LASER_OFF,
    PHOTODIODE_START_SAMPLING,
    PHOTODIODE_FINISH_SAMPLING,
    SYSTEM_RESET,
    SYSTEM_INITIALIZED,
    SYSTEM_STARTED
};

void lwl_start(void);

// Other APIs.
void lwl_rec(uint8_t id, int32_t num_arg_bytes, ...);
void lwl_enable(bool on);
void lwl_dump(void);
uint8_t* lwl_get_buffer(uint32_t* len);
void LWL(uint8_t id, ...) ;
// The special __COUNTER__ macro (not official C but supported by many
// compilers) is used to generate LWL IDs.

void lwl_buffer_full_notify(void);

void lwl_clear_notification(void);


#endif // _LWL_H_
