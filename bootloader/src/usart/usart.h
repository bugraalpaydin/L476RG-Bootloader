#ifndef __USART_H
#define __USART_H

#include "main.h"

void uart_init(void);
void bootloader_uart_init(void);
void bootloader_uart_receive(void);

#define BUFFER_SIZE 10000

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
