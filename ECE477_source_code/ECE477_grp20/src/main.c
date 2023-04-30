#include "commands.h"
#include "stm32f0xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fifo.h"
#include "tty.h"
#include "lcd.h"
#include "ff.h"
#include "midi.h"
#include "step-array.h"
#include <stdint.h>
#include <math.h>
#include "background.h"


u16 base1 = 100;
u16 height1 = 320;
u16 base2 = 122;
u16 height2 = 298;


void init_spi1_slow() {
    RCC ->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC ->APB2ENR |= RCC_APB2ENR_SPI1EN;

    GPIOB ->MODER &= ~GPIO_MODER_MODER3;
    GPIOB ->MODER |= GPIO_MODER_MODER3_1;
    GPIOB ->MODER &= ~GPIO_MODER_MODER4;
    GPIOB ->MODER |= GPIO_MODER_MODER4_1;
   GPIOB ->MODER &= ~GPIO_MODER_MODER5;
    GPIOB ->MODER |= GPIO_MODER_MODER5_1;

    GPIOB ->AFR[0] &= ~GPIO_AFRL_AFR3;
    GPIOB ->AFR[0] &= ~GPIO_AFRL_AFR4;
    GPIOB ->AFR[0] &= ~GPIO_AFRL_AFR5;

    GPIOB ->AFR[0] &= ~0xfff000;


    SPI1 ->CR1 &= ~SPI_CR1_SPE;
    SPI1 ->CR1 |= SPI_CR1_BR;
    SPI1 ->CR2 = 0x0700;
    SPI1 ->CR1 |= SPI_CR1_MSTR;
    SPI1 ->CR2 |= SPI_CR2_FRXTH;
    SPI1 ->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
    SPI1 ->CR1 |= SPI_CR1_SPE;
}

void sdcard_io_high_speed(){
    RCC ->APB2ENR |= RCC_APB2ENR_SPI1EN;
    SPI1 ->CR1 &= ~SPI_CR1_SPE;
    SPI1 ->CR1 &= ~(7<<3);
    SPI1 ->CR1 |= 0x8;
    SPI1 ->CR1 |= SPI_CR1_SPE;
}

void init_lcd_spi(){
    RCC ->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB ->MODER &= ~GPIO_MODER_MODER8;
    GPIOB ->MODER |= GPIO_MODER_MODER8_0;
    GPIOB ->MODER &= ~GPIO_MODER_MODER11;
    GPIOB ->MODER |= GPIO_MODER_MODER11_0;
    GPIOB ->MODER &= ~GPIO_MODER_MODER14;
    GPIOB ->MODER |= GPIO_MODER_MODER14_0;
    init_spi1_slow();
    sdcard_io_high_speed();
}
/*
void collision() {
    if ((base1 >= x1 && base1 <= x2) && (height1 <= y_1+50 && height2 >= y2)) {
        LCD_Clear(BLACK);
        exit(0);
    }
    else if ((base2 >= x1 && base2 <= x2) && (height1 <= y_1+50 && height2 >= y2)){
        LCD_Clear(BLACK);
        exit(0);
    }
}
*/


u16 car1_x1 = 0;
u16 car1_x2 = 40;
u16 car1_y1 = 272;
u16 car1_y2 = 292;

u16 car2_x1 = 50;
u16 car2_x2 = 90;
u16 car2_y1 = 250;
u16 car2_y2 = 270;



void reset_init(u16 *x1, u16 *x2){
    *x1 = 0;
    *x2 = 40;

}

void init_tim7() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 4800 - 1;
    TIM7->ARR = 10000 - 1;
    TIM7->DIER |= TIM_DIER_UIE;
    TIM7->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= 1 << TIM7_IRQn;
}

void TIM7_IRQHandler(void) {
    TIM7->SR &= ~TIM_SR_UIF;
    LCD_DrawFillRectangle(car1_x1, car1_y1, car1_x2, car1_y2, BLUE);
    LCD_DrawFillRectangle(car1_x1, car1_y1, car1_x2, car1_y2, BLACK);
    car1_x1 += 10;
    car1_x2 += 10;
    LCD_DrawFillRectangle(car1_x1, car1_y1, car1_x2, car1_y2, BLUE);
    if (car1_x1 >= 240){
        reset_init(&car1_x1, &car1_x2);
    }

    LCD_DrawFillRectangle(car2_x1, car2_y1, car2_x2, car2_y2, BLUE);
    LCD_DrawFillRectangle(car2_x1, car2_y1, car2_x2, car2_y2, BLACK);
    car2_x1 += 10;
    car2_x2 += 10;
    LCD_DrawFillRectangle(car2_x1, car2_y1, car2_x2, car2_y2, BLUE);
    if (car2_x1 >= 240){
        reset_init(&car2_x1, &car2_x2);
    }
    //collision();
}


/*
u16 xc = 0;
u16 xc2 = 0;
u16 xc3 = 0;
u16 xc4 = 0;
u16 xc5 = 0;
u16 yc = 5;
u16 yc2 = 5;
u16 yc3 = 5;
u16 yc4 = 5;
u16 yc5 = 5;
u16 r = 5;
u16 fill = 1;
u16 c = BLACK;
int s1;
int s2;
int s3;
int s4;
int s5;

void collision(u16 *x, u16 *y) {
    if ((*x >= base1 && *x <= base2) && (*y >= height2 && *y <= height1)) {
        LCD_Clear(BLACK);
        exit(0);
    }
}

void reset_init(u16 *y){
    //*x = 0;
    *y = 5;
}

void init_tim7() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 4800 - 1;
    TIM7->ARR = 1000 - 1;
    TIM7->DIER |= TIM_DIER_UIE;
    TIM7->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= 1 << TIM7_IRQn;
}

void TIM7_IRQHandler(void) {
    TIM7->SR &= ~TIM_SR_UIF;
    //circle 1
    if (yc == 5){
        xc = rand() % 240;
        s1 = rand() % 5 + 3;
    }
    LCD_Circle(xc, yc, r, fill, c);
    LCD_Circle(xc, yc, r, fill, WHITE);
    yc = yc + s1;
    LCD_Circle(xc, yc, r, fill, c);
    if (yc >= 335){
        LCD_Circle(xc, yc, r, fill, WHITE);
        reset_init(&yc);
    }
    collision(&xc, &yc);

    //circle 2
    if (yc2 == 5){
        xc2 = rand() % 240;
        s2 = rand() % 5 + 3;
    }
    LCD_Circle(xc2, yc2, r, fill, c);
    LCD_Circle(xc2, yc2, r, fill, WHITE);
    yc2 = yc2 + s2;
    LCD_Circle(xc2, yc2, r, fill, c);
    if (yc2 >= 335){
        LCD_Circle(xc2, yc2, r, fill, WHITE);
        reset_init(&yc2);
    }
    collision(&xc2, &yc2);
    //circle 4
    if (yc3 == 5){
        xc3 = rand() % 240;
        s3 = rand() % 5 + 3;
    }
    LCD_Circle(xc3, yc3, r, fill, c);
    LCD_Circle(xc3, yc3, r, fill, WHITE);
    yc3 = yc3 + s3;
    LCD_Circle(xc3, yc3, r, fill, c);
    if (yc3 >= 335){
        LCD_Circle(xc3, yc3, r, fill, WHITE);
        reset_init(&yc3);
    }
    collision(&xc3, &yc3);
    //circle 4
    if (yc4 == 5){
        xc4 = rand() % 240;
        s4 = rand() % 5 + 3;
    }
    LCD_Circle(xc4, yc4, r, fill, c);
    LCD_Circle(xc4, yc4, r, fill, WHITE);
    yc4 = yc4 + s4;
    LCD_Circle(xc4, yc4, r, fill, c);
    if (yc4 >= 335){
        LCD_Circle(xc4, yc4, r, fill, WHITE);
        reset_init(&yc4);
    }
    collision(&xc4, &yc4);

}
*/

void enable_ports(){
    RCC ->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC ->MODER &= ~GPIO_MODER_MODER0;
    //GPIOC ->MODER |= GPIO_MODER_MODER4_0;
    GPIOC ->MODER &= ~GPIO_MODER_MODER1;
    //GPIOC ->MODER |= GPIO_MODER_MODER1_0;
    GPIOC ->MODER &= ~GPIO_MODER_MODER2;
    //GPIOC ->MODER |= GPIO_MODER_MODER2_0;
    GPIOC ->MODER &= ~GPIO_MODER_MODER3;
    //GPIOC ->MODER |= GPIO_MODER_MODER3_0;

    GPIOC ->PUPDR &= ~GPIO_PUPDR_PUPDR0;
    GPIOC ->PUPDR &= ~GPIO_PUPDR_PUPDR1;
    GPIOC ->PUPDR &= ~GPIO_PUPDR_PUPDR2;
    GPIOC ->PUPDR &= ~GPIO_PUPDR_PUPDR3;

    GPIOC ->PUPDR |= GPIO_PUPDR_PUPDR0_1;
    GPIOC ->PUPDR |= GPIO_PUPDR_PUPDR1_1;
    GPIOC ->PUPDR |= GPIO_PUPDR_PUPDR2_1;
    GPIOC ->PUPDR |= GPIO_PUPDR_PUPDR3_1;

}

void init_exti(){
    RCC ->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
    SYSCFG ->EXTICR[0] |=SYSCFG_EXTICR1_EXTI0_PC;
    SYSCFG ->EXTICR[0] |=SYSCFG_EXTICR1_EXTI1_PC;
    SYSCFG ->EXTICR[0] |=SYSCFG_EXTICR1_EXTI2_PC;
    SYSCFG ->EXTICR[0] |=SYSCFG_EXTICR1_EXTI3_PC;
    SYSCFG ->EXTICR[1] |=SYSCFG_EXTICR2_EXTI4_PC;
    EXTI ->RTSR |= EXTI_RTSR_TR0|EXTI_RTSR_TR1|EXTI_RTSR_TR2|EXTI_RTSR_TR3| EXTI_RTSR_TR4;
    EXTI ->IMR |= EXTI_IMR_MR0|EXTI_IMR_MR1|EXTI_IMR_MR2|EXTI_IMR_MR3 | EXTI_IMR_MR4;
    NVIC ->ISER[0] = 1<<EXTI0_1_IRQn;
    NVIC ->ISER[0] = 1<<EXTI2_3_IRQn;
    NVIC ->ISER[0] = 1<<EXTI4_15_IRQn;
}

void EXTI0_1_IRQHandler(void){
    if (GPIOC -> IDR & GPIO_IDR_0) {
        EXTI -> PR |= EXTI_PR_PR0;
        LCD_DrawRectangle(base1, height1, base2, height2, WHITE);
        base1 = base1 + 20;
        base2 = base2 + 20;
        if(base2 >= 240){
            base1 = 220;
            base2 = 240;
            LCD_DrawRectangle(base1, height1, base2, height2, GREEN);

        }
        else{
            LCD_DrawRectangle(base1, height1, base2, height2, GREEN);

        }
    }

    else if (GPIOC -> IDR & GPIO_IDR_1) {
        EXTI -> PR |= EXTI_PR_PR1;
        LCD_DrawRectangle(base1, height1, base2, height2, WHITE);
        base1 = base1 - 20;
        base2 = base2 - 20;
        if(base2 <= 20){
            base1 = 0;
            base2 = 20;
            LCD_DrawRectangle(base1, height1, base2, height2, GREEN);
        }
        else{
            LCD_DrawRectangle(base1, height1, base2, height2, GREEN);
        }
    }
}


void EXTI2_3_IRQHandler(){
    if (GPIOC -> IDR & GPIO_IDR_3) {
         EXTI->PR |= EXTI_PR_PR3;
         LCD_DrawRectangle(base1, height1, base2, height2, WHITE);
         height1 = height1 - 20;
         height2 = height2 - 20;
         if(height1 <= 20){
             height1 = 20;
             height2 = 0;
             LCD_DrawRectangle(base1, height1, base2, height2, GREEN);
         }
         else{
             LCD_DrawRectangle(base1, height1, base2, height2, GREEN);
         }
     }

     else if (GPIOC -> IDR & GPIO_IDR_2) {
         EXTI -> PR |= EXTI_PR_PR2;
         LCD_DrawRectangle(base1, height1, base2, height2, WHITE);
         height1 = height1 + 20;
         height2 = height2 + 20;
         if(height1 >= 300){
             height1 = 300;
             height2 = 320;
             LCD_DrawRectangle(base1, height1, base2, height2, GREEN);
         }
         else{
             LCD_DrawRectangle(base1, height1, base2, height2, GREEN);
         }
     }
}


struct {
    uint8_t note;
    uint8_t chan;
    uint8_t volume;
    int     step;
    int     offset;
} voice[15];

MIDI_Player *mp;

void enable_port(void) {
    RCC -> AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA -> MODER |= 3 << (2*4);
}

void init_TIM2(int n) {
    RCC -> APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 48 - 1;
    TIM2->ARR = n;
    TIM2-> CR1 |= TIM_CR1_ARPE;
    TIM2-> DIER |= TIM_DIER_UIE;
    TIM2 -> CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= 1 << TIM2_IRQn;
}

void init_TIM6() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 48 - 1;
    TIM6->ARR = (200000/RATE) - 1;
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= 1 << TIM6_DAC_IRQn;
}

void init_DAC() {
    RCC -> APB1ENR |= RCC_APB1ENR_DACEN;
    DAC -> CR &= ~DAC_CR_EN1;
    DAC -> CR |= DAC_CR_TSEL1;
    DAC -> CR |= DAC_CR_TEN1;
    DAC -> CR |= DAC_CR_EN1;
}

short int wavetable[N];

void init_wavetable(void) {
    for (int i = 0; i < N; i++)
    {
        wavetable[i] = /*32767*/ 2048 + 2047 * sin(2 * M_PI * i / N);
        //wavetable[i] = 16383 * sin(2 * M_PI * i / N) + 16383.0 * (i - (N/2)) / (1.0 * N);
    }
}

void TIM2_IRQHandler(void)
{
    TIM2->SR &= ~TIM_SR_UIF;
    midi_play();
    if (mp->nexttick >= 122881)//MAXTICKS)
     {
         TIM6->CR1 &= ~TIM_CR1_CEN;
         TIM2 -> CR1 &= ~TIM_CR1_CEN;
         mp = midi_init(midifile);
         init_TIM2(10417);
         init_TIM6();
     }
}

void TIM6_DAC_IRQHandler(void) {
    TIM6->SR &= ~TIM_SR_UIF;
    DAC -> SWTRIGR |= DAC_SWTRIGR_SWTRIG1;

    int sample = 0;
    for(int i=0; i < 15; i++) {
        sample += (wavetable[voice[i].offset>>16] * voice[i].volume);// << 4;
        voice[i].offset += voice[i].step;
        if ((voice[i].offset >> 16) >= N)
            voice[i].offset -= N << 16;
    }
    sample /= 16;
    DAC -> DHR12R1 = sample;
}

void set_tempo(int time, int value, const MIDI_Header *hdr)
{
    // This assumes that the TIM2 prescaler divides by 48.
    // It sets the timer to produce an interrupt every N
    // microseconds, where N is the new tempo (value) divided by
    // the number of divisions per beat specified in the MIDI header.
    TIM2->ARR = value/hdr -> divisions - 1;
}

void note_on(int time, int chan, int key, int velo)
{
//  if (velo == 0) {
//      note_off(time, chan, key, velo);
//  }
  for(int i=0; i < sizeof voice / sizeof voice[0]; i++)
    if (voice[i].step == 0) {
      // configure this voice to have the right step and volume
      voice[i].step = step[key];
      voice[i].note = key;
      voice[i].volume = velo / 8;
      break;
    }
}

void note_off(int time, int chan, int key, int velo)
{
  for(int i=0; i < sizeof voice / sizeof voice[0]; i++)
    if (voice[i].step != 0 && voice[i].note == key) {
      // turn off this voice
      voice[i].step = 0;
      voice[i].note = 0;
      voice[i].volume = 0;
      voice[i].offset = 0;
      break;
    }
}



int main(){

    NVIC_SetPriority(TIM2_IRQn,1);
    NVIC_SetPriority(TIM6_DAC_IRQn,2);
    NVIC_SetPriority(TIM7_IRQn,0);

    enable_ports();
    init_lcd_spi();
    LCD_Setup();
    LCD_Clear(WHITE);
    LCD_DrawPicture(0,0, (const Picture *) &gimp_image);

    u16 a = 100;
    u16 b = 320;
    u16 c = 122;
    u16 d = 298;
    LCD_DrawRectangle(a, b, c, d, GREEN);

    init_exti();
    init_tim7();
    init_DAC();
    mp = midi_init(midifile);
    init_wavetable();
    enable_port();


    // The default rate for a MIDI file is 2 beats per second
    // with 48 ticks per beat.  That's 500000/48 microseconds.
    init_TIM2(10417);
    init_TIM6();

    // Nothing else to do at this point.
    for(;;)
    {
        asm("wfi");
    }


}





