#include "bootloader_sm.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>

volatile bootloader_state_t current_state = BOOT_STATE_WAIT_COMMAND;

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


