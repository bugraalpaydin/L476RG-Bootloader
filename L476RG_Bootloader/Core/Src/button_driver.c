#include "button_driver.h"
#include <stm32l476xx.h>


//for interrup generation PC13 -> EXTI13
//SYSCFG->EXTICR4  bits 7:4  0010

void button_init(void){
  //user button is PC13
  RCC->AHB2ENR |= (1<<2);

  GPIOC->MODER &= ~(1<<27);
  GPIOC->MODER &= ~(1<<26);

  GPIOC->PUPDR &= ~(1<<27);
  GPIOC->PUPDR &= ~(1<<26);

  EXTI->IMR1 |= (1<<17);
  EXTI->EMR1 |= (1<<17);
  EXTI->FTSR1 |= (1<<13);

  //NVIC_EnableIRQ(EXTI15_10_IRQn);
  NVIC->ISER[1] |= (1<<8);
  //1111 1111
  NVIC->IP[10] |= (3<<4);
  NVIC_SetPriority(EXTI15_10_IRQn, 1);
}
uint32_t button_read(void){
  uint32_t data = (GPIOC->IDR & (1<<13));
  return data;
}


void EXTI15_10_IRQHandler(void){
    // EXTI hattı kontrol ediliyor
    if (EXTI->PR1 & (1 << 13)) {
        // Kesme bayrağını temizle
        EXTI->PR1 |= (1 << 13);

        // Buton durumunu oku
        if (!button_read()) {
            // Adrese atla (bootloader fonksiyonunu çağır)
            goto_address(0x08040000);
        }
    }
}
