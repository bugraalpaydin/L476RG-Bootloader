#include "goto_address.h"
#include "stm32l476xx.h"

void goto_address(uint32_t address){
    NVIC_SystemReset();
    //SCB->VTOR = 0x00000;
    //void (*app_reset_handler)(void) = (void*)(*(volatile uint32_t*)(address+4));
    //__set_MSP((*(volatile uint32_t*)(address)));
    //app_reset_handler();
   
}
