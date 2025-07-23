/*
 * bsp_system.h
 *
 *  Created on: Jul 21, 2025
 *      Author: DELL
 */

#ifndef BSUPPORT_BSP_BSP_SYSTEM_BSP_SYSTEM_H_
#define BSUPPORT_BSP_BSP_SYSTEM_BSP_SYSTEM_H_

#include <math.h>
#include <stm32f7xx.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stm32f7xx_hal.h>
#include <stm32f7xx_ll_bus.h>

#define METADATA_MEM_BASE         	0x08018000U  // Sector 3 (metadata, 16 KB)
#define METADATA_SECTOR				3

#define FOTA_SUCCESS               			0
#define FOTA_FAILED                 		1

typedef struct _s_firmware_info_
{
    bool fota_rqt;
    uint32_t address;      // Địa chỉ firmware
    uint32_t length;       // Độ dài firmware
    uint32_t crc;          // CRC cho firmware
    uint8_t version_major;
    uint8_t version_minor;
    uint8_t version_patch;
} s_firmware_info;


uint8_t System_On_Bootloader_Reset(void);

#endif /* BSUPPORT_BSP_BSP_SYSTEM_BSP_SYSTEM_H_ */
