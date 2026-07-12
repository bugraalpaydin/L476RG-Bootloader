#include "goto_address.h"
#include "stm32l4xx_hal.h"

void goto_address(uint32_t address){ //vectorr table offsett
    /* in startup.asm vector table code we can see that first vector is _estack and second vector is Reset_Handler 
     * because of that we use address+4 
     * */
    void (*app_reset_handler)(void) = (void*)(*(volatile uint32_t*)(address+4));
    HAL_DeInit();
  __set_MSP((*(volatile uint32_t*)(address)));
    app_reset_handler();
}   
