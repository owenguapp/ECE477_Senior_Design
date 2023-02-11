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
#include <stdint.h>
#include <math.h>
#include "frog.h"
#include <string.h>
void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}

int temperature;
short vtemp;
char show_temp[100];
char show_v[100];
void enable_sensor(void)
{
    // turn on PB0, ADC8
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB->MODER &= ~GPIO_MODER_MODER0; // PB0 for output
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR0_1; // pull down PB0

    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->CR2 |= RCC_CR2_HSI14ON;
    while(!(RCC->CR2 & RCC_CR2_HSI14RDY));
    ADC1->CR |= ADC_CR_ADEN;
    while(!(ADC1->ISR & ADC_ISR_ADRDY));
    while((ADC1->CR & ADC_CR_ADSTART));
}

void init_tim2(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 4800 - 1;
    TIM2->ARR = 10000 - 1;
    TIM2->DIER |= TIM_DIER_UIE;
    TIM2->CR1 |= TIM_CR1_CEN;
    NVIC_EnableIRQ(TIM2_IRQn);
}

void TIM2_IRQHandler(void)
{
    TIM2->SR &= ~TIM_SR_UIF;
    GPIOB->MODER |= GPIO_MODER_MODER0; //Set Analog, 11
    ADC1->CHSELR = 0;
    ADC1->CHSELR |= 1 << 8;
    while(!(ADC1->ISR & ADC_ISR_ADRDY));
    ADC1->CR |= ADC_CR_ADSTART;
    while(!(ADC1->ISR & ADC_ISR_EOC));

    // gan huo
    vtemp = ADC1->DR  * 3300 / 4096;
    temperature = (13.582 - sqrt((-13.582) * (-13.582) + 4 * 0.00433 * (2230.8 - vtemp))) / (2 * (-0.00433)) + 30;
    sprintf(show_temp,"Current temperature: %d oC",temperature);
    LCD_DrawFillRectangle(175,200,235,220,BLACK);
    LCD_DrawString(15, 200, WHITE, 2016, show_temp, 16, 1);

    sprintf(show_v,"Current vtemp: %d mV",vtemp);
    LCD_DrawFillRectangle(125,220,175,240,BLACK);
    LCD_DrawString(15, 220, WHITE, 2016, show_v, 16, 1);

    // logic
    if (temperature > 19){
        LCD_DrawFillRectangle(15,240,240,260,BLACK);
        LCD_DrawString(15, 240, WHITE, 2016, "The meal is ready.", 16, 1);
    }
    else if (temperature <= 19){
        LCD_DrawFillRectangle(15,240,240,260,BLACK);
        LCD_DrawString(15, 240, WHITE, 2016, "bie chi bie chi", 16, 1);
    }

    GPIOB->MODER &= ~GPIO_MODER_MODER0; // refresh PB0
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


void enable_ports(){

    //port C
    RCC ->AHBENR |= RCC_AHBENR_GPIOCEN;

    //Configures pins PC4 – PC7 to be outputs
    GPIOC ->MODER &= ~GPIO_MODER_MODER4;
    GPIOC ->MODER &= ~GPIO_MODER_MODER5;
    GPIOC ->MODER &= ~GPIO_MODER_MODER6;
    GPIOC ->MODER &= ~GPIO_MODER_MODER7;
    GPIOC ->MODER |= GPIO_MODER_MODER4_0;
    GPIOC ->MODER |= GPIO_MODER_MODER5_0;
    GPIOC ->MODER |= GPIO_MODER_MODER6_0;
    GPIOC ->MODER |= GPIO_MODER_MODER7_0;

    //Configures pins PC0 – PC3 to be inputs
    GPIOC ->MODER &= ~GPIO_MODER_MODER0;
    GPIOC ->MODER &= ~GPIO_MODER_MODER1;
    GPIOC ->MODER &= ~GPIO_MODER_MODER2;
    GPIOC ->MODER &= ~GPIO_MODER_MODER3;

    //Configures pins PC0 – PC3 to be internally pulled low
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

    }

    else if (GPIOC -> IDR & GPIO_IDR_1) {
        EXTI -> PR |= EXTI_PR_PR1;
    }
}


void EXTI2_3_IRQHandler(){
    if (GPIOC -> IDR & GPIO_IDR_3) {
         EXTI->PR |= EXTI_PR_PR3;
    }

     else if (GPIOC -> IDR & GPIO_IDR_2) {
         EXTI -> PR |= EXTI_PR_PR2;
     }
}

//===========================================================================
// 2.3 Debouncing a Keypad
//===========================================================================
const char keymap[] = " CBA#9630852:741";
uint8_t hist[16];
uint8_t col;
char queue[2];
int qin;
int qout;
char key[15] = "               ";
int i = 0;


void drive_column(int c)
{
    GPIOC->BSRR = 0xf00000 | (1 << (c + 4));
}

int read_rows()
{
    return GPIOC->IDR & 0xf;
}

void push_queue(int n) {
    n = (n & 0xff) | 0x80;
    queue[qin] = n;
    qin ^= 1;
}

uint8_t pop_queue() {
    uint8_t tmp = queue[qout] & 0x7f;
    queue[qout] = 0;
    qout ^= 1;
    return tmp;
}

void update_history(int c, int rows)
{
    for(int i = 0; i < 4; i++) {
        hist[4*c+i] = (hist[4*c+i]<<1) + ((rows>>i)&1);
        if (hist[4*c+i] == 1)
          push_queue(4*c+i);
    }
}

void init_tim6() {
    RCC->APB1ENR |= RCC_APB1ENR_TIM6EN;
    TIM6->PSC = 48 - 1;
    TIM6->ARR = 1000 - 1;
    TIM6->DIER |= TIM_DIER_UIE;
    TIM6->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= 1 << TIM6_DAC_IRQn;
}

void TIM6_DAC_IRQHandler(void) {
    TIM6->SR &= ~TIM_SR_UIF;
    int rows = read_rows();
    update_history(col, rows);
    col = (col + 1) & 3;
    drive_column(col);
}

char get_keypress() {
    for(;;) {
        asm volatile ("wfi" : :);   // wait for an interrupt
        if (queue[qout] == 0)
            continue;
        key[i] = keymap[pop_queue()];
        i++;
        return key[i-1];
//        return keymap[pop_queue()];
    }
}

//void init_tim7() {
//    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
//    TIM7->PSC = 4800 - 1;
//    TIM7->ARR = 150000 - 1;
//    TIM7->DIER |= TIM_DIER_UIE;
//    TIM7->CR1 |= TIM_CR1_CEN;
//    NVIC->ISER[0] |= 1 << TIM7_IRQn;
//}
//
//
//void TIM7_IRQHandler(void) {
//    TIM7->SR &= ~TIM_SR_UIF;
//
//}
//int find_col(char* arr){
//    for (int i = 0; i < sizeof(arr); i++){
//        if (arr[i] == 'A'){
//            return i;
//        }
//        else {return 0;}
//    }
//}

char str[100];
int time;

void init_tim15() {
    RCC->APB2ENR |= RCC_APB2ENR_TIM15EN;
    TIM15->PSC = 4800 - 1;
    TIM15->ARR = 10000 - 1;
    TIM15->DIER |= TIM_DIER_UIE;
    TIM15->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= 1 << TIM15_IRQn;
}

void TIM15_IRQHandler(void) {
    TIM15->SR &= ~TIM_SR_UIF;
    sprintf(str,"Time: %d",time);
    LCD_DrawFillRectangle(78,0,120,18,BLACK);
    LCD_DrawString(30,0,WHITE,2016,str,16,1);
    time --;
    if (time == -1){
        LCD_DrawString(30, 180, WHITE, 2016, "Your meal is ready", 16, 1);
    }
}

int main(){


    enable_ports();
    init_lcd_spi();
    LCD_Setup();


    LCD_Clear(WHITE);
    LCD_DrawFillRectangle(0, 0, 320, 320, 0);
    LCD_DrawString(30, 30, WHITE, 2016, "Electric Lunch Box,", 18, 1);
    LCD_DrawString(30, 50, WHITE, 2016, "What can I do for you?", 18, 1);
    LCD_DrawString(30, 70, WHITE, 2016, "Time Setting:", 18, 1);
//    LCD_DrawString(30, 110, WHITE, 2016, "Heating Duration: user defined", 12, 1);
//    LCD_DrawString(30, 130, WHITE, 2016, "Heating Mode: automatic", 12, 1);
//    LCD_DrawString(30, 150, WHITE, 2016, "Current Temperature: 50°C", 12, 1);
//    LCD_DrawString(30, 170, WHITE, 2016, "Time Elapsed: 15 min", 12, 1);

    //LCD_DrawFillRectangle(a, d, c, b, GREEN);
    LCD_DrawPicture(0,0, (const Picture *) &frog);
    LCD_DrawPicture(0,300, (const Picture *) &frog);
    LCD_DrawPicture(220,0, (const Picture *) &frog);
    LCD_DrawPicture(220,300, (const Picture *) &frog);
    enable_sensor();
    init_tim2();
    init_tim6();
    //init_tim7();
//    int i = 0;
    //char key[15];
    char d;
    while(true) {
//        key[i] = get_keypress();
        d = get_keypress();
        LCD_DrawFillRectangle(30,100,150,120,BLACK);
        LCD_DrawString(30, 100, WHITE, 2016, key, 16, 1);
        //LCD_DrawString(30, 120, WHITE, 2016, key[i], 16, 1);
        if (d == ' '){
            LCD_DrawFillRectangle(30,100,150,120,BLACK);
            memset(key, ' ', 15);
            i = 0;
        }
        if (d == 'C'){
            break;
        }

    }
    int min = atoi(key);
    sprintf(str,"min: %d",min);
    LCD_DrawString(30, 120, WHITE, 2016, str, 16, 1);
    char* pos = strchr(key, ':');
    int index = (int)(pos - key);
    char x[2];
    int b = 0;
    for (int z = index + 1; z < sizeof(key); z++){
        if (key[z] != 'C'){
            x[b] = key[z];
            b ++;
        }

    }
    int sec = atoi(x);
    sprintf(str,"sec: %d",sec);
    LCD_DrawString(30, 140, WHITE, 2016, str, 16, 1);
    int total = sec + min * 60;
    sprintf(str,"Total in sec: %d",total);
    LCD_DrawString(30, 160, WHITE, 2016, str, 16, 1);
    time = total;
    init_tim15();
//    itoa(a, str, 10);
//    int inc = 0;
//    for(;;) {
//        key = get_keypress();
//        //LCD_DrawFillRectangle(30,100,150,120,BLACK);
//        LCD_DrawString(30+inc, 100, WHITE, 2016, key, 16, 1);
//        inc += 16;
//        //LCD_DrawString(30, 120, WHITE, 2016, key[i], 16, 1);
//        if (get_keypress() == ' '){
//            LCD_DrawFillRectangle(30,100,150,120,BLACK);
//            inc = 0;
//        }
//    }

    // Nothing else to do at this point.
    for(;;)
    {
        asm("wfi");
    }
}
