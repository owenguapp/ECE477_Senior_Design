#include "commands.h"
#include "stm32f0xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>


void enable_ports(void)
{
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER |= 1 << 16;
    GPIOB->ODR |= 1<<8;
}

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

void disable_ports(void)
{
    GPIOB->ODR &= ~1<<8;
}

int main(){
    enable_ports();

    // Nothing else to do at this point.
    // nano_wait(100000000000);
    // disable_ports();
}
