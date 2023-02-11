#include "stm32f0xx.h"
#include <math.h>
#include <stdint.h>
#define SAMPLES 30
uint16_t array[SAMPLES];

void setfreq(float fre)
{
    // All of the code for this exercise will be written here.
    // Create a waveform
    for(int x=0; x < SAMPLES; x += 1)
        array[x] = 2048 + 1952 * sin(2 * M_PI * x / SAMPLES);


    // Set up the timer with TRGO
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->ARR = 48000000/fre - 1;
    TIM6->PSC = 0;
    TIM6->CR2 |= TIM_CR2_MMS_1;
    TIM6->CR1 |= TIM_CR1_CEN;
    TIM6->DIER |= TIM_DIER_UDE;

    // Set up DMA
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel3->CCR &= ~DMA_CCR_EN;
    DMA1_Channel3->CCR |= DMA_CCR_MSIZE_0;
    DMA1_Channel3->CCR |= DMA_CCR_PSIZE_0;
    DMA1_Channel3->CCR |= DMA_CCR_MINC;
    DMA1_Channel3->CCR |= DMA_CCR_CIRC;
    DMA1_Channel3->CCR |= DMA_CCR_DIR;
    DMA1_Channel3->CMAR = (uint32_t)&array;
    DMA1_Channel3->CPAR = (uint32_t)&DAC->DHR12R1;
    DMA1_Channel3->CNDTR = SAMPLES;
    DMA1_Channel3->CCR |= DMA_CCR_EN;

    // Set up DAC
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    DAC->CR &= ~DAC_CR_EN1;
    DAC->CR &= ~DAC_CR_TSEL1;
    DAC->CR |= DAC_CR_TEN1;
    DAC->CR |= DAC_CR_EN1;

    // Enable GPIO
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODER4;
}

int main(void)
{
    // Uncomment any one of the following calls ...
    //setfreq(1000);
    //setfreq(1234.5);
    //setfreq(10000);
    setfreq(100000);
}
