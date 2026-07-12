/*  This header file is wrote for bootloader serial port handling
*       Author: bugra alp aydin
*       Date:   12/10/2025
***************************************************/

#ifndef INC_UARTDRIVER_H_
#define INC_UARTDRIVER_H_

#include "main.h"

#define BUFFER_SIZE 10000


typedef struct {
    uint8_t buffer[BUFFER_SIZE];
    uint32_t buffer_head_pointer;
    uint32_t buffer_tail_pointer;
    uint32_t received_bytes;
}uart_buffer_t;

/* shell command defines */
typedef enum {
    CMD_START_FLASH = 1, 
    CMD_WRITE_DATA,
    CMD_END_FLASH,    
    CMD_JUMP_APP,     
    CMD_ERASE_FLASH,  
    CMD_ACK = 0x06, 
    CMD_NACK = 0x15
} bootloader_protocol_t; 

typedef enum {
    BOOT_STATE_WAIT_COMMAND,
    BOOT_STATE_WAIT_FILE_SIZE,
    BOOT_STATE_WAIT_CHUNK_SIZE,
    BOOT_STATE_WAIT_CHUNK_DATA,
} bootloader_state_t;


extern volatile uart_buffer_t uart_buffer_tx;
extern volatile uart_buffer_t uart_buffer_rx;

extern UART_HandleTypeDef huart2;

void bootloader_uart_init(void);
void bootloader_uart_receive(void);
void bootloader_sm_handler(void);

#endif
