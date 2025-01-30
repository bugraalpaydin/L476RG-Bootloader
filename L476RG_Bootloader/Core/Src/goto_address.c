#include "goto_address.h"

void goto_address(uint32_t address){
    void (*app_reset_handler)(void) = (void*)(*(volatile uint32_t*)(address+4));
  __set_MSP((*(volatile uint32_t*)(address)));
    app_reset_handler();
   
}