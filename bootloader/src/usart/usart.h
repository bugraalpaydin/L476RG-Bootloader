#ifndef __USART_H
#define __USART_H

#include "main.h"
#include <stdint.h>
#include "bootloader_sm.h"

void uart_init(void);
uint32_t uart_read(uint8_t *buf, uint32_t len);
uint32_t uart_bytes_to_read(void);
void uart_write_buf(uint8_t *buf, uint32_t len);

#define BUFFER_SIZE 10000

extern volatile bootloader_state_t current_state;
extern volatile system_events_t system_event;
extern UART_HandleTypeDef huart2;


typedef struct {
    uint8_t buffer[BUFFER_SIZE];
    uint32_t buffer_head_pointer;
    uint32_t buffer_tail_pointer;
    uint32_t received_bytes;
}uart_buffer_t;

extern volatile uart_buffer_t uart_buffer_tx;
extern volatile uart_buffer_t uart_buffer_rx;

#endif
