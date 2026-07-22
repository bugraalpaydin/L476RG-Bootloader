/*  This header file is wrote for bootloader serial port handling
*       Author: bugra alp aydin
*       Date:   12/10/2025
***************************************************/

#ifndef INC_UARTDRIVER_H_
#define INC_UARTDRIVER_H_

#include "main.h"
#include <stdint.h>


/* for debug purposes */
extern UART_HandleTypeDef huart2;

/* shell command defines */
typedef enum {
    CMD_START_FLASH = 0x01, 
    CMD_WRITE_DATA,
    CMD_END_FLASH,    
    CMD_JUMP_APP,     
    CMD_ERASE_FLASH,  
    CMD_ACK         = 0x06,
    CMD_NACK        = 0x15
} bootloader_commands_t; 

typedef enum {
    CMD_START_SIZE  = 4, /* 1 byte start command, 3 bytes file size */
    CMD_WRITE_SIZE  = 2, /* 1 byte write command, 1 byte data */
    CMD_JUMP_SIZE   = 0,  /* 1 byte jump to application command */
    CMD_ERASE_SIZE  = 0, /* 1 byte erase flash command */
}cmd_size_t;

typedef enum {
    BOOT_STATE_WAIT_COMMAND   = 0,
    BOOT_STATE_WAIT_FILE_SIZE,
    BOOT_STATE_WAIT_CHUNK_SIZE,
    BOOT_STATE_WAIT_CHUNK_DATA,
    BOOT_STATE_WRITE_FLASH
} bootloader_state_t;

typedef enum {
    SYSTEM_IDLE       = 0,
    SYSTEM_FRESH_DATA,
    SYSTEM_SEND_DATA,
} system_events_t;

typedef struct {
    uint8_t *rx_buf;
    uint8_t *tx_buf;
    bootloader_state_t current_state;
    system_events_t system_event;
}bootloader_ctx_t;

typedef void *(state_callback_t)(void);

typedef struct{
    state_callback_t callback_function;
}bootloader_sm_t;

void bootloader_sm_handler(void);

#endif
