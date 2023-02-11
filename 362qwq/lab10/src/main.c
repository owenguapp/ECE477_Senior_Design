/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f0xx.h"
			
void init_usart5()
{
    //port C
    RCC ->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC ->MODER &= ~GPIO_MODER_MODER12;
    GPIOC ->MODER |= GPIO_MODER_MODER12_1;
    GPIOC ->AFR[1] &= ~GPIO_AFRH_AFR12;
    GPIOC ->AFR[1] |= 0x2 << (4*(12-8));

    //port D
    RCC ->AHBENR |= RCC_AHBENR_GPIODEN;
    GPIOD ->MODER &= ~GPIO_MODER_MODER2;
    GPIOD ->MODER |= GPIO_MODER_MODER2_1;
    GPIOD ->AFR[0] &= ~GPIO_AFRL_AFR2;
    GPIOD ->AFR[0] |= 0x2 << 4 * 2;

    RCC ->APB1ENR |= RCC_APB1ENR_USART5EN;
    USART5 ->CR1 &= ~USART_CR1_UE;
    USART5 ->CR1 &= ~USART_CR1_M;
    USART5 ->CR2 &= ~USART_CR2_STOP;
    USART5 ->CR1 &= ~USART_CR1_PCE;
    USART5 ->CR1 &= ~USART_CR1_OVER8;
    USART5 ->BRR |= 0x1a1;
    USART5 ->CR1 |= USART_CR1_TE;
    USART5 ->CR1 |= USART_CR1_RE;
    USART5 ->CR1 |= USART_CR1_UE;
    while (!(USART5 ->ISR & USART_ISR_TEACK)) { }
    while (!(USART5 ->ISR & USART_ISR_REACK)) { }
}

//#define STEP21
#if defined(STEP21)
int main(void)
{
    init_usart5();
    for(;;) {
        while (!(USART5->ISR & USART_ISR_RXNE)) { }
        char c = USART5->RDR;
        while(!(USART5->ISR & USART_ISR_TXE)) { }
        USART5->TDR = c;
    }
}
#endif

//#define STEP22
#if defined(STEP22)
#include <stdio.h>
int __io_putchar(int c) {
    if(c == '\n')
    {
        while(!(USART5->ISR & USART_ISR_TXE)) { }
        USART5->TDR = '\r';
    }
    while(!(USART5->ISR & USART_ISR_TXE)) { }
    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
     while (!(USART5->ISR & USART_ISR_RXNE)) { }
     char c = USART5->RDR;
     if(c == '\r')
     {
         c = '\n';
     }
     putchar(c);
     return c;
}

int main() {
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

//#define STEP23
#if defined(STEP23)
#include <stdio.h>
#include "fifo.h"
#include "tty.h"
int __io_putchar(int c) {
    if(c == '\n')
    {
        while(!(USART5->ISR & USART_ISR_TXE)) { }
        USART5->TDR = '\r';
    }
    while(!(USART5->ISR & USART_ISR_TXE)) { }
    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
    char c = line_buffer_getchar();
    return c;
}

int main() {
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

//#define STEP24
#if defined(STEP24)
#include <stdio.h>
#include "fifo.h"
#include "tty.h"

#define FIFOSIZE 16
char serfifo[FIFOSIZE];
int seroffset = 0;
int __io_putchar(int c) {
    if(c == '\n')
    {
        while(!(USART5->ISR & USART_ISR_TXE)) { }
        USART5->TDR = '\r';
    }
    while(!(USART5->ISR & USART_ISR_TXE)) { }
    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
    char c = interrupt_getchar();
    return c;
}

void enable_tty_interrupt(){
    while ((USART5->ISR & USART_ISR_RXNE)) {}
    USART5 ->CR1 |= USART_CR1_RXNEIE;
    NVIC->ISER[0] |= 1 << USART3_8_IRQn;
    USART5 ->CR3 |= USART_CR3_DMAR;

    RCC->AHBENR |= RCC_AHBENR_DMA2EN;
    DMA2->RMPCR |= DMA_RMPCR2_CH2_USART5_RX;
    DMA2_Channel2 ->CCR &= ~DMA_CCR_EN;  // First make sure DMA is turned off
    DMA2_Channel2 ->CPAR = (uint32_t)(&(USART5 ->RDR));
    DMA2_Channel2 ->CMAR = (uint32_t) &serfifo;
    DMA2_Channel2 ->CNDTR = FIFOSIZE;
    DMA2_Channel2 ->CCR &= ~DMA_CCR_DIR;
    DMA2_Channel2 ->CCR |= DMA_CCR_MINC;
    DMA2_Channel2 ->CCR &= ~DMA_CCR_MSIZE;
    DMA2_Channel2 ->CCR &= ~DMA_CCR_PSIZE;
    DMA2_Channel2 ->CCR |= DMA_CCR_CIRC;
    DMA2_Channel2 ->CCR |= DMA_CCR_PL;
    DMA2_Channel2 ->CCR |= DMA_CCR_EN;

}

int interrupt_getchar(void){
    while(fifo_newline(&input_fifo) == 0) {
        asm volatile ("wfi"); // wait for an interrupt
    }
    char ch = fifo_remove(&input_fifo);
    return ch;
}

void USART3_4_5_6_7_8_IRQHandler(void) {
    while(DMA2_Channel2->CNDTR != sizeof serfifo - seroffset) {
        if (!fifo_full(&input_fifo))
            insert_echo_char(serfifo[seroffset]);
        seroffset = (seroffset + 1) % sizeof serfifo;
    }
}

int main() {
    init_usart5();
    enable_tty_interrupt();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif

#define STEP22a
#if defined(STEP22a)
#include <stdio.h>
int __io_putchar(int c) {
    while(!(USART5->ISR & USART_ISR_TXE)) { }
    USART5->TDR = c;
    return c;
}

int __io_getchar(void) {
     while (!(USART5->ISR & USART_ISR_RXNE)) { }
     char c = USART5->RDR;
     return c;
}

int main() {
    init_usart5();
    setbuf(stdin,0);
    setbuf(stdout,0);
    setbuf(stderr,0);
    printf("Enter your name: ");
    char name[80];
    fgets(name, 80, stdin);
    printf("Your name is %s", name);
    printf("Type any characters.\n");
    for(;;) {
        char c = getchar();
        putchar(c);
    }
}
#endif
