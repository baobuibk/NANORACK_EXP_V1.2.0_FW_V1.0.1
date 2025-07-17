/*
 * min_shell_command.h
 *
 *  Created on: Jun 13, 2025
 *      Author: Admin
 */

#ifndef APP_MIN_SHELL_MIN_SHELL_COMMAND_H_
#define APP_MIN_SHELL_MIN_SHELL_COMMAND_H_


#include "min_shell.h"

#include <stdint.h>


// =================================================================
// Command IDs (Maximum ID: 63)
// =================================================================

#define TEST_CONNECTION_CMD                         0x01
#define TEST_CONNECTION_ACK                         0x02

#define SET_TEMP_PROFILE_CMD                        0x03
#define SET_TEMP_PROFILE_ACK                        0x04

#define START_TEMP_PROFILE_CMD                      0x05
#define START_TEMP_PROFILE_ACK                      0x06

#define STOP_TEMP_PROFILE_CMD                       0x07
#define STOP_TEMP_PROFILE_ACK                       0x08

#define SET_OVERRIDE_TEC_PROFILE_CMD                0x09
#define SET_OVERRIDE_TEC_PROFILE_ACK                0x0A

#define START_OVERRIDE_TEC_PROFILE_CMD              0x0B
#define START_OVERRIDE_TEC_PROFILE_ACK              0x0C

#define STOP_OVERRIDE_TEC_PROFILE_CMD               0x0D
#define STOP_OVERRIDE_TEC_PROFILE_ACK               0x0E

#define SET_SAMPLING_PROFILE_CMD                    0x11
#define SET_SAMPLING_PROFILE_ACK                    0x12

#define SET_LASER_INTENSITY_CMD                     0x13
#define SET_LASER_INTENSITY_ACK                     0x14

#define SET_POSITION_CMD                            0x15
#define SET_POSITION_ACK                            0x16

#define START_SAMPLING_CYCLE_CMD                    0x17
#define START_SAMPLING_CYCLE_ACK                    0x18

#define GET_INFO_SAMPLE_CMD                         0x19
#define GET_INFO_SAMPLE_ACK                         0x1A

#define GET_CHUNK_CMD                               0x1B
#define GET_CHUNK_ACK                               0x1C

#define GET_CHUNK_CRC_CMD							1
#define GET_CHUNK_CRC_ACK							1

#define SET_EXT_LASER_PROFILE_CMD                   0x1D
#define SET_EXT_LASER_PROFILE_ACK                   0x1E

#define TURN_ON_EXT_LASER_CMD                       0x21
#define TURN_ON_EXT_LASER_ACK                       0x22

#define TURN_OFF_EXT_LASER_CMD                      0x23
#define TURN_OFF_EXT_LASER_ACK                      0x24

//-------------------------------------------------------
#define CUSTOM_COMMAND_CMD							0x36
#define CUSTOM_COMMAND_ACK							0x37
//----
#define PING_CMD                                    0x38  ///< Ping request
#define PONG_CMD                                    0x39  ///< Pong response
//----
#define	MIN_RESP_NAK		                        0x3A
#define	MIN_RESP_ACK		                        0x3B
//----
#define	MIN_RESP_WRONG		                        0x3C
#define	MIN_RESP_DONE		                        0x3D
//----
#define MIN_RESP_FAIL		                        0x3E
#define	MIN_RESP_OK			                        0x3F
//----
#define MIN_ERROR_OK								0xFF


//-------------------------------------------------------
#define MIN_RESP_LEN_2								2

/**
 * @brief Command handler function type.
 * @param ctx Pointer to the MIN context.
 * @param payload Pointer to the received payload data.
 * @param len Length of the payload.
 */
typedef void (*MIN_CommandHandler)(MIN_Context_t *ctx, const uint8_t *payload, uint8_t len);

/**
 * @brief Structure to map command IDs to their handlers.
 */
typedef struct {
    uint8_t id;                    ///< Command ID
    MIN_CommandHandler handler;    ///< Handler function for the command
} MIN_Command_t;

/**
 * @brief Gets the command table.
 * @return Pointer to the command table.
 */
const MIN_Command_t *MIN_GetCommandTable(void);

/**
 * @brief Gets the size of the command table.
 * @return Number of entries in the command table.
 */
int MIN_GetCommandTableSize(void);






#endif /* APP_MIN_SHELL_MIN_SHELL_COMMAND_H_ */
