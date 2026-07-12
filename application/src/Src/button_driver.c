#include "button_driver.h"
#include <stm32l476xx.h>

//PC3 
void button_init(void){
    RCC->AHB2ENR |= (1<<2);//enable gpioc clock

    //pc3 input mode
    GPIOC->MODER &= ~(1<<7);
    GPIOC->MODER &= ~(1<<6);
    
    //pc3 push-pull mode
    GPIOC->OTYPER &= ~(1<<3);

    //pc3 pul-up mode
    GPIOC->PUPDR &= ~(1<<7);
    GPIOC->PUPDR |= (1<<6);

    //exti3 line to pc3
    SYSCFG->EXTICR[0]   &= ~(1<<15);
    SYSCFG->EXTICR[0]   &= ~(1<<14);
    SYSCFG->EXTICR[0]   |= (1<<13);
    SYSCFG->EXTICR[0]   &= ~(1<<12);

    EXTI->IMR1      |= (1<<3); //line3 interrupt is not masked
    EXTI->FTSR1     |= (1<<3); //line3 falling trigger
    EXTI->EMR1      |= (1<<3); //line3 event is not masked

    NVIC_EnableIRQ(EXTI3_IRQn);
    NVIC_SetPriority(EXTI3_IRQn, 0);
}

uint32_t button_read(void){
  uint32_t data = (GPIOC->IDR & (1<<3));
  return data;
}

void EXTI3_IRQHandler(void){
    // EXTI hattı kontrol ediliyor
    if (EXTI->PR1 & (1 << 3)) {
        // Buton durumunu oku
        if (!button_read()) {
        // Kesme bayrağını temizle
            EXTI->PR1 |= (1 << 3);
            // Adrese atla (bootloader fonksiyonunu çağır)
            goto_address(0x08000000);
        }
    }
}
