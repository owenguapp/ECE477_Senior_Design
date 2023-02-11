#include "commands.h"
#include "stm32f0xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "fifo.h"
#include "tty.h"
#include "lcd.h"
#include "ff.h"
#include "midi.h"
#include "step-array.h"
#include <stdint.h>
#include <math.h>
#include "background.h"
#include "frog.h"
#include "gator.h"
#include "gator_left.h"
#include "gameover.h"
#include "winwin.h"

const char* string2 = "You win!";
const char* string3 = "Game Over";

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

u16 base1 = 100;
u16 height1 = 320;
u16 base2 = 120;
u16 height2 = 300;


void init_tim15() {
    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
    TIM15->PSC = 4800 - 1;
    TIM15->ARR = 10000 - 1;
    TIM15->DIER |= TIM_DIER_UIE;
    TIM15->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= 1 << TIM15_IRQn;
}

int time = 60;
char str[100];
const char* string4 = "Time is up!";

void TIM15_IRQHandler(void) {
    if(time < 0){
        LCD_Clear(WHITE);
        LCD_DrawString(90, 130, BLUE, 2016, string4, 16, 1);
        nano_wait(100000000000000000);
        LCD_Clear(WHITE);
        //LCD_DrawString(90, 130, BLUE, 2016, string3, 16, 1);
        LCD_DrawPicture(0,0, (const Picture *) &gameover);
        nano_wait(100000000000000000);
        LCD_Clear(BLACK);
        exit(0);
    }
    TIM15->SR &= ~TIM_SR_UIF;
    sprintf(str,"Time: %d",time);
    LCD_DrawFillRectangle(48,0,65,18,CYAN);
    LCD_DrawString(0,0,RED,2016,str,16,1);
    time --;


}
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
    TIM7->ARR = 150000 - 1;
    TIM7->DIER |= TIM_DIER_UIE;
    TIM7->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= 1 << TIM7_IRQn;
}


u16 rec1 = 0;
u16 rec2 = 80;
u16 tall1 = 120;
u16 tall2 = 160;

u16 rec3 = 160;
u16 rec4 = 240;
u16 tall3 = 80;
u16 tall4 = 120;

u16 rec5 = 0;
u16 rec6 = 80;
u16 tall5 = 40;
u16 tall6 = 80;

u16 o1_b1 = 0;  //o1_b1 = obstacle1_base1
u16 o1_b2 = 80;
u16 o1_h1 = 240; //o1_h1 = obstacle1_height1
u16 o1_h2 = 280;

u16 o2_b1 = 160;
u16 o2_b2 = 240;
u16 o2_h1 = 200;
u16 o2_h2 = 240;

u16 o3_b1 = 0;
u16 o3_b2 = 80;
u16 o3_h1 = 160;
u16 o3_h2 = 200;

void collision(u16 col_b1, u16 col_h1, u16 col_b2, u16 col_h2) {
    if ((col_b2 > base1) && (col_b1 < base2) && (col_h2 >= height1) && (col_h1 <= height2)) {
        LCD_Clear(WHITE);
        //LCD_DrawString(90, 130, 248, 2016, string3, 16, 1);
        LCD_DrawPicture(0,0, (const Picture *) &gameover);
        exit(0);
    }
}

int x = 0;
void check_drown() {

    if (height1 > 40) {
     if (!((rec2 > base1) && (rec1 <= base2) && (tall2 >= height1) && (tall1 <= height2))
      && !((rec4 >= base1) && (rec3 < base2) && (tall4 >= height1) && (tall3 <= height2))
      && !((rec6 > base1) && (rec5 <= base2) && (tall6 >= height1) && (tall5 <= height2))) {
         if (height1 <= 160) {
             x = 0;
             LCD_Clear(WHITE);
             //LCD_DrawString(90, 130, 248, 2016, string3, 16, 1);
             LCD_DrawPicture(0,0, (const Picture *) &gameover);
             exit(0);
         }
    }
     else {
         x = 1;

     }
    }
}

void hop(u16 hop_b1, u16 hop_h1, u16 hop_b2, u16 hop_h2) {
    if ((hop_b2 >= base1) && (hop_b1 <= base2) && (hop_h2 >= height1) && (hop_h1 <= height2)) {
        if((hop_b1 == base1) && (hop_h2 >= height1) && (hop_h2 <= height2)){
            LCD_DrawFillRectangle(base1, height2, base2, height1, GRAY);
        }
       base1 += 20;
       base2 += 20;
    }

    if (base1 >= 240){
        LCD_Clear(WHITE);
        //LCD_DrawString(90, 130, 248, 2016, string3, 16, 1);
        LCD_DrawPicture(0,0, (const Picture *) &gameover);
        exit(0);
    }

}

void hop_left(u16 hop_b1, u16 hop_h1, u16 hop_b2, u16 hop_h2) {
    if ((hop_b2 >= base1) && (hop_b1 <= base2) && (hop_h2 >= height1) && (hop_h1 <= height2)){
    //if ((base1 >= hop_b1) && (base2 <=hop_b2) && (height1 >=hop_h1) && (height2 <= hop_h2)){
        if((hop_b1 == base1) && (hop_h2 >= height1) && (hop_h2 <= height2)){
            LCD_DrawFillRectangle(base1, height2, base2, height1, BLUE);
        }
       base1 -= 20;
       base2 -= 20;
    }
    if (base2 <= 0){
        LCD_Clear(WHITE);
        //LCD_DrawString(90, 130, 248, 2016, string3, 16, 1);
        LCD_DrawPicture(0,0, (const Picture *) &gameover);
        exit(0);
    }
}

void TIM7_IRQHandler(void) {
    TIM7->SR &= ~TIM_SR_UIF;
    LCD_DrawFillRectangle(rec1, tall1, rec2, tall2, GRAY);
    LCD_DrawFillRectangle(0, tall1, rec1, tall2, BLUE);
    //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
    //LCD_DrawPicture(base1,height2, (const Picture *) &frog);

    if (rec1 >= 240) {
        LCD_DrawFillRectangle(0, tall1, 240, tall2, BLUE);
        rec1 = 0;
        rec2 = 80;
        //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
        //LCD_DrawPicture(base1,height2, (const Picture *) &frog);

    }
    hop(rec1, tall1, rec2, tall2);
    check_drown();
    rec1 += 20;
    rec2 += 20;

    LCD_DrawFillRectangle(rec3, tall3, rec4, tall4, GRAY);
    LCD_DrawFillRectangle(rec4, tall3, 240, tall4, BLUE);
    //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
    //LCD_DrawPicture(base1,height2, (const Picture *) &frog);
    if (rec4 <= 0) {
        LCD_DrawFillRectangle(0, tall3, 240, tall4, BLUE);
        rec3 = 160;
        rec4 = 240;
        //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
        //LCD_DrawPicture(base1,height2, (const Picture *) &frog);
    }

    hop_left(rec3, tall3, rec4, tall4);
    check_drown();
    rec3  -= 20;
    rec4  -= 20;


    LCD_DrawFillRectangle(rec5, tall5, rec6, tall6, GRAY);
    LCD_DrawFillRectangle(0, tall5, rec5, tall6, BLUE);
    //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
    //LCD_DrawPicture(base1,height2, (const Picture *) &frog);
    if (rec5 >= 240) {
        LCD_DrawFillRectangle(0, tall5, 240, tall6, BLUE);
        rec5 = 0;
        rec6 = 80;
        //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
        //LCD_DrawPicture(base1,height2, (const Picture *) &frog);
    }
    hop(rec5, tall5, rec6, tall6);
    check_drown();
    rec5 += 20;
    rec6 += 20;

    //LCD_DrawFillRectangle(o1_b1, o1_h1, o1_b2, o1_h2, RED);
    LCD_DrawPicture(o1_b1,o1_h1, (const Picture *) &gator);
    LCD_DrawFillRectangle(0, o1_h1, o1_b1, o1_h2, BLACK);
    //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
    LCD_DrawPicture(base1,height2, (const Picture *) &frog);

    if (o1_b2 >= 240) {
        LCD_DrawFillRectangle(0, o1_h1, 240, o1_h2, BLACK);
        o1_b1 = 0;
        o1_b2 = 80;
        //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
        //LCD_DrawPicture(base1,height2, (const Picture *) &frog);
    }
    collision(o1_b1, o1_h1, o1_b2, o1_h2);
    o1_b1 += 10;
    o1_b2 += 10;

    //LCD_DrawFillRectangle(o2_b1, o2_h1, o2_b2, o2_h2, RED);
    LCD_DrawPicture(o2_b1,o2_h1, (const Picture *) &gator_left);
    LCD_DrawFillRectangle(o2_b2, o2_h1, 240, o2_h2, BLACK);
    //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
    LCD_DrawPicture(base1,height2, (const Picture *) &frog);
    if (o2_b1 <= 0) {
        LCD_DrawFillRectangle(0, o2_h1, 240, o2_h2, BLACK);
        o2_b1 = 160;
        o2_b2 = 240;
        //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
        //LCD_DrawPicture(base1,height2, (const Picture *) &frog);
    }
    collision(o2_b1, o2_h1, o2_b2, o2_h2);
    o2_b1 -= 10;
    o2_b2 -= 10;

    //LCD_DrawFillRectangle(o3_b1, o3_h1, o3_b2, o3_h2, RED);
    LCD_DrawPicture(o3_b1,o3_h1, (const Picture *) &gator);
    LCD_DrawFillRectangle(0, o3_h1, o3_b1, o3_h2, BLACK);
    //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
    LCD_DrawPicture(base1,height2, (const Picture *) &frog);
    if (o3_b2 >= 240) {
        LCD_DrawFillRectangle(0, o3_h1, 240, o3_h2, BLACK);
        o3_b1 = 0;
        o3_b2 = 80;
        //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
        //LCD_DrawPicture(base1,height2, (const Picture *) &frog);
    }
    collision(o3_b1, o3_h1, o3_b2, o3_h2);
    o3_b1 += 10;
    o3_b2 += 10;

}

void enable_ports(){
    RCC ->AHBENR |= RCC_AHBENR_GPIOCEN;
    GPIOC ->MODER &= ~GPIO_MODER_MODER0;
    GPIOC ->MODER &= ~GPIO_MODER_MODER1;
    GPIOC ->MODER &= ~GPIO_MODER_MODER2;
    GPIOC ->MODER &= ~GPIO_MODER_MODER3;

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
}

void EXTI0_1_IRQHandler(void){
    if (GPIOC -> IDR & GPIO_IDR_0) {
        EXTI -> PR |= EXTI_PR_PR0;
        base1 = base1 + 20;
        base2 = base2 + 20;
        collision(o1_b1, o1_h1, o1_b2, o1_h2);
        collision(o2_b1, o2_h1, o2_b2, o2_h2);
        collision(o3_b1, o3_h1, o3_b2, o3_h2);
        check_drown();
        if((base2 > 240) && (height2 >= 160)){
            base1 = 220;
            base2 = 240;
            //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
            LCD_DrawPicture(base1,height2, (const Picture *) &frog);
        }
        else if ((base2 > 240) && (height2 < 160)) {
            LCD_Clear(WHITE);
            //LCD_DrawString(90, 130, 248, 2016, string3, 16, 1);
            LCD_DrawPicture(0,0, (const Picture *) &gameover);
            exit(0);
        }
        else{
            //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
            LCD_DrawPicture(base1,height2, (const Picture *) &frog);
            if (height2 < 160) {
                LCD_DrawFillRectangle(base1 - 20, height2, base1, height1, BLUE);
                if (x == 1) {
                    LCD_DrawFillRectangle(base1 - 20, height2, base1, height1, GRAY);
                }
            }
            else if (height2 >= 160) {
                LCD_DrawFillRectangle(base1 - 20, height2, base1, height1, BLACK);
            }
        }
    }

    else if (GPIOC -> IDR & GPIO_IDR_1) {
        EXTI -> PR |= EXTI_PR_PR1;
        base1 = base1 - 20;
        base2 = base2 - 20;
        collision(o1_b1, o1_h1, o1_b2, o1_h2);
        collision(o2_b1, o2_h1, o2_b2, o2_h2);
        collision(o3_b1, o3_h1, o3_b2, o3_h2);
        check_drown();
        if((base2 < 20) && (height2 >= 160)) {
            base1 = 0;
            base2 = 20;
            //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
            LCD_DrawPicture(base1,height2, (const Picture *) &frog);
        }
        else if ((base2 < 20) && (height2 <= 160)) {
            LCD_Clear(WHITE);
            //LCD_DrawString(90, 130, 248, 2016, string3, 16, 1);
            LCD_DrawPicture(0,0, (const Picture *) &gameover);
            exit(0);
        }
        else{
            //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
            LCD_DrawPicture(base1,height2, (const Picture *) &frog);
            if (height2 < 160) {
                LCD_DrawFillRectangle(base2, height2, base2+20, height1, BLUE);
                if (x == 1) {
                    LCD_DrawFillRectangle(base2, height2, base2+20, height1, GRAY);
                }
            }
            else if (height2 >= 160) {
                LCD_DrawFillRectangle(base2, height2, base2+20, height1, BLACK);
            }
        }
    }
}


void EXTI2_3_IRQHandler(){
    if (GPIOC -> IDR & GPIO_IDR_3) {
         EXTI->PR |= EXTI_PR_PR3;
         height1 = height1 - 20;
         height2 = height2 - 20;
         collision(o1_b1, o1_h1, o1_b2, o1_h2);
         collision(o2_b1, o2_h1, o2_b2, o2_h2);
         collision(o3_b1, o3_h1, o3_b2, o3_h2);
         check_drown();
         if(height1 <= 40){
             height1 = 20;
             height2 = 0;
             LCD_Clear(WHITE);
             LCD_DrawString(90, 130, 248, 2016, string2, 16, 1);
             //LCD_DrawPicture(0,0, (const Picture *) &winwin);
             exit(0);
         }
         else{
             //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
             LCD_DrawPicture(base1,height2, (const Picture *) &frog);
             if ((height1 < 320) && (height1 >= 160)) {
             LCD_DrawFillRectangle(base1, height1, base2, height1 + 20, BLACK);
             }
             else if ((height1 < 320) && (height1 < 160)){
                 LCD_DrawFillRectangle(base1, height1, base2, height1 + 20, BLUE);
                 if (x == 1) {
                     LCD_DrawFillRectangle(base1, height1, base2, height1 + 20, GRAY);
                 }
             }
         }
     }

     else if (GPIOC -> IDR & GPIO_IDR_2) {
         EXTI -> PR |= EXTI_PR_PR2;
         height1 = height1 + 20;
         height2 = height2 + 20;
         collision(o1_b1, o1_h1, o1_b2, o1_h2);
         collision(o2_b1, o2_h1, o2_b2, o2_h2);
         collision(o3_b1, o3_h1, o3_b2, o3_h2);
         check_drown();
         if(height2 >= 300){
             height1 = 320;
             height2 = 300;
             //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
             LCD_DrawPicture(base1,height2, (const Picture *) &frog);
             LCD_DrawFillRectangle(base1, 280, base2, 300, BLACK);
         }
         else {
             //LCD_DrawFillRectangle(base1, height2, base2, height1, GREEN);
             LCD_DrawPicture(base1,height2, (const Picture *) &frog);
             if (height2 > 160) {
             LCD_DrawFillRectangle(base1, height2 - 20, base2, height2, BLACK);
             }
             else {
                 LCD_DrawFillRectangle(base1, height2 - 20, base2, height2, BLUE);
                 if (x == 1) {
                     LCD_DrawFillRectangle(base1, height2 - 20, base2, height2, GRAY);
                 }
             }
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

const char* string = "Press any button to start";

int main(){

    NVIC_SetPriority(TIM2_IRQn,1);
    NVIC_SetPriority(TIM6_DAC_IRQn,2);
    NVIC_SetPriority(TIM7_IRQn,0);

    enable_ports();
    init_lcd_spi();
    LCD_Setup();

    LCD_Clear(WHITE);
    LCD_DrawFillRectangle(0, 160, 240, 320, 0);
    LCD_DrawFillRectangle(0, 0, 240, 160, BLUE);
    LCD_DrawFillRectangle(0, 0, 240, 40, CYAN);

    //LCD_DrawFillRectangle(a, d, c, b, GREEN);
    LCD_DrawPicture(base1,height2, (const Picture *) &frog);

    init_tim15();
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
