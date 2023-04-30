#include "commands.h"
#include "stm32f0xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>

void init(void){
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |=  1<<8;
    GPIOA->PUPDR |= 1<<9;
    GPIOA->PUPDR &= 0<<8;

//    GPIOB->MODER |= 1 << 4;
//    GPIOB->PUPDR |= 1<<5;
//    GPIOB->PUPDR &= 0<<4;

}
void enable_ports(void)
{
    GPIOA->ODR |= 1<<4;
}

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void disable_ports(void)
{
    GPIOA->ODR &= ~1<<4;
}

int main(){
    init();
    //disable_ports();
    enable_ports();

    // Nothing else to do at this point.
    //nano_wait(100000000000);
    //disable_ports();
}
