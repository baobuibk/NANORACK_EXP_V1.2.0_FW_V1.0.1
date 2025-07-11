/*
 * app_configs.h
 *
 *  Created on: Jun 11, 2025
 *      Author: Admin
 */

#ifndef APP_CONFIGS_H_
#define APP_CONFIGS_H_

#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

#ifndef UNUSED_FUNC
#define UNUSED_FUNC __attribute__((unused))
#endif

/* En/Dis Debug/AutoComplete CMD */
#define DEBUG_ENABLE
#define CLI_AUTOCOMPLETE_ENABLE


#define SHELL_UART_INITATION		    "EXP $ "

// Kích thước của uint16_t (thường là 2 byte)
#define SHELL_UART_UINT_SIZE 2

// Số lượng binding nội bộ của SHELL_UART
#define SHELL_UART_INTERNAL_BINDING_COUNT 3

// Số lượng binding tối đa do người dùng cấu hình
#define SHELL_UART_MAX_BINDING_COUNT 5

// Kích thước bộ đệm nhận (receive buffer) tính bằng ký tự
#define SHELL_UART_RX_BUFFER_SIZE 64

// Kích thước bộ đệm lệnh (command buffer) tính bằng ký tự
#define SHELL_UART_CMD_BUFFER_SIZE 32

// Kích thước bộ đệm lịch sử (history buffer) tính bằng ký tự
#define SHELL_UART_HISTORY_BUFFER_SIZE 128

// Macro chuyển đổi từ byte sang đơn vị uint16_t, làm tròn lên
#define BYTES_TO_SHELL_UART_UINTS(bytes) (((bytes) + SHELL_UART_UINT_SIZE - 1) / SHELL_UART_UINT_SIZE)

// Macro tính kích thước buffer cần thiết cho SHELL_UART (tính bằng byte)
#define SHELL_UART_BUFFER_SIZE 1024


#endif /* CONFIGS_APP_CONFIGS_H_ */
