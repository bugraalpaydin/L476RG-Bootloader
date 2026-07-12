#include "usart.h"
#include "stm32l4xx_hal.h"

volatile uart_buffer_t uart_buffer_rx;
volatile uart_buffer_t uart_buffer_tx;

UART_HandleTypeDef huart2;

volatile uint32_t expected_file_size = 0;
volatile uint8_t expected_chunk_size = 0;
volatile uint16_t received_bytes = 0;


char serial_msg[50];

void uart_init(void) {
    /* todo : initialize usart2 with HAL driver */
    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
    huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

    USART2->CR1 |= USART_CR1_RXNEIE;  // RX interrupt enabl 

    uart_buffer_rx.buffer_tail_pointer = 0;
    uart_buffer_rx.buffer_head_pointer = 0;
    uart_buffer_tx.buffer_head_pointer = 0;
    uart_buffer_tx.buffer_tail_pointer = 0;
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        Error_Handler();
    }
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
}

uint32_t uart_bytes_to_read(void) {
	if (uart_buffer_rx.buffer_head_pointer >= uart_buffer_rx.buffer_tail_pointer) {
		return uart_buffer_rx.buffer_head_pointer - uart_buffer_rx.buffer_tail_pointer;
	}
	else {
		return (BUFFER_SIZE + uart_buffer_rx.buffer_head_pointer - uart_buffer_rx.buffer_tail_pointer);
	}
}
void USART2_IRQHandler(void) {
    if((USART2->ISR & USART_ISR_RXNE) && (USART2->CR1 & USART_CR1_RXNEIE)) {
        uart_buffer_rx.buffer[uart_buffer_rx.buffer_head_pointer] = USART2->RDR;
        uart_buffer_rx.buffer_head_pointer++;
        
    }
}
