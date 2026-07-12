#include "bootloader_port_driver.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>


//#define DEBUG_MON 1

volatile uart_buffer_t uart_buffer_rx;
volatile uart_buffer_t uart_buffer_tx;

UART_HandleTypeDef huart2;

volatile bootloader_state_t current_state = BOOT_STATE_WAIT_COMMAND;
volatile uint32_t expected_file_size = 0;
volatile uint8_t expected_chunk_size = 0;
volatile uint16_t received_bytes = 0;

char serial_msg[50];

void bootloader_uart_init(void) {
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


void bootloader_sm_handler(void) {
        switch(current_state) {
            case BOOT_STATE_WAIT_COMMAND:
                if(uart_buffer_rx.buffer[0] == CMD_START_FLASH) {
                    #ifdef DEBUG_MON
                        sprintf(serial_msg, "wait file size state\n");
                        HAL_UART_Transmit(&huart2, (uint8_t*)serial_msg, strlen(serial_msg), HAL_MAX_DELAY);
                        while(!(USART2->ISR & USART_ISR_TXE));
                    #endif

                    USART2->TDR = CMD_ACK;

                    // State değiştir
                    current_state = BOOT_STATE_WAIT_FILE_SIZE;
                    uart_buffer_rx.buffer_tail_pointer++;
                }
                else if(uart_buffer_rx.buffer[uart_buffer_rx.buffer_tail_pointer] == CMD_WRITE_DATA) {
                    // ACK gönder
                    sprintf(serial_msg, "wait chunk size state");
                    HAL_UART_Transmit(&huart2, (uint8_t*)serial_msg, strlen(serial_msg), HAL_MAX_DELAY);
                    while(!(USART2->ISR & USART_ISR_TXE));
                    USART2->TDR = CMD_ACK;

                    // State değiştir
                    current_state = BOOT_STATE_WAIT_CHUNK_SIZE;
                    uart_buffer_rx.buffer_tail_pointer++;
                }
            break;

            case BOOT_STATE_WAIT_FILE_SIZE:
                if((uart_buffer_rx.buffer_head_pointer - uart_buffer_rx.buffer_tail_pointer) > 3) {
                    /*
                    * uint8_t new_coming_data = uart_buffer_rx.buffer_head_pointer - uart_buffer_rx.buffer_tail_pointer;
                    * shift buffer to calculate expected file size
                    * */
                    /* yanlis */
                    expected_file_size = *(uint32_t*)uart_buffer_rx.buffer;
                    sprintf(serial_msg, "File size: %lu\r\n", (unsigned long)expected_file_size);
                    HAL_UART_Transmit(&huart2, (uint8_t*)serial_msg, strlen(serial_msg), HAL_MAX_DELAY);
                    sprintf(serial_msg, "state wait command\n");
                    HAL_UART_Transmit(&huart2, (uint8_t*)serial_msg, strlen(serial_msg), HAL_MAX_DELAY);
                    current_state = BOOT_STATE_WAIT_COMMAND;
                    uart_buffer_rx.buffer_head_pointer = uart_buffer_rx.buffer_tail_pointer;
                }
            break;

            case BOOT_STATE_WAIT_CHUNK_SIZE:

                if(uart_buffer_rx.buffer_head_pointer - uart_buffer_rx.buffer_tail_pointer > 0) {
                    expected_chunk_size = uart_buffer_rx.buffer[uart_buffer_rx.buffer_head_pointer];
                    sprintf(serial_msg, "Chunk size: %lu\r\n", (unsigned long)expected_chunk_size);
                    HAL_UART_Transmit(&huart2, (uint8_t*)serial_msg, strlen(serial_msg), HAL_MAX_DELAY);
                    current_state = BOOT_STATE_WAIT_CHUNK_DATA;
                    uart_buffer_rx.buffer_head_pointer = uart_buffer_rx.buffer_tail_pointer;
                }
            break;

        case BOOT_STATE_WAIT_CHUNK_DATA:
            HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
            if(uart_buffer_rx.buffer_head_pointer - uart_buffer_rx.buffer_tail_pointer >= expected_chunk_size) {
                /* program is received send ACK */
                sprintf(serial_msg, "bootloader state wait chunk data");
                HAL_UART_Transmit(&huart2, (uint8_t*)serial_msg, strlen(serial_msg), HAL_MAX_DELAY);
                while(!(USART2->ISR & USART_ISR_TXE));
                USART2->TDR = CMD_ACK;


                // TODO: Flash'a yaz

                current_state = BOOT_STATE_WAIT_COMMAND;
            }
        break;
        default:
            sprintf(serial_msg, "False bootloader command try again");
            HAL_UART_Transmit(&huart2, (uint8_t*)serial_msg, strlen(serial_msg), HAL_MAX_DELAY);
            current_state = BOOT_STATE_WAIT_COMMAND;
    }
}

uint32_t UART_Bytes_to_Read(void) {
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
