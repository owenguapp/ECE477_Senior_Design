
//=============================================================================
// ECE 362 lab experiment 6 -- Analog Input/Output and DMA
//=============================================================================

#include "stm32f0xx.h"
#include <string.h> // for memset() declaration
#include <math.h>   // for MA_PI

// Be sure to change this to your login...
const char login[] = "yin181";


//===========================================================================
// 2.1 Configuring GPIO
//===========================================================================
void enable_ports(void) {
    //port B
    RCC ->AHBENR |= RCC_AHBENR_GPIOBEN;

    //Configures pins PB0 – PB10 to be outputs
    GPIOB ->MODER &= ~GPIO_MODER_MODER0;
    GPIOB ->MODER &= ~GPIO_MODER_MODER1;
    GPIOB ->MODER &= ~GPIO_MODER_MODER2;
    GPIOB ->MODER &= ~GPIO_MODER_MODER3;
    GPIOB ->MODER &= ~GPIO_MODER_MODER4;
    GPIOB ->MODER &= ~GPIO_MODER_MODER5;
    GPIOB ->MODER &= ~GPIO_MODER_MODER6;
    GPIOB ->MODER &= ~GPIO_MODER_MODER7;
    GPIOB ->MODER &= ~GPIO_MODER_MODER8;
    GPIOB ->MODER &= ~GPIO_MODER_MODER9;
    GPIOB ->MODER &= ~GPIO_MODER_MODER10;
    GPIOB ->MODER |= GPIO_MODER_MODER0_0;
    GPIOB ->MODER |= GPIO_MODER_MODER1_0;
    GPIOB ->MODER |= GPIO_MODER_MODER2_0;
    GPIOB ->MODER |= GPIO_MODER_MODER3_0;
    GPIOB ->MODER |= GPIO_MODER_MODER4_0;
    GPIOB ->MODER |= GPIO_MODER_MODER5_0;
    GPIOB ->MODER |= GPIO_MODER_MODER6_0;
    GPIOB ->MODER |= GPIO_MODER_MODER7_0;
    GPIOB ->MODER |= GPIO_MODER_MODER8_0;
    GPIOB ->MODER |= GPIO_MODER_MODER9_0;
    GPIOB ->MODER |= GPIO_MODER_MODER10_0;

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

//===========================================================================
// 2.2 Configuring DMA transfers
//===========================================================================
uint16_t msg[8] = { 0x0000,0x0100,0x0200,0x0300,0x0400,0x0500,0x0600,0x0700 };
extern const char font[];

void setup_dma(void) {
    RCC ->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel2 ->CCR &= ~DMA_CCR_EN;
    DMA1_Channel2 ->CPAR = (uint32_t)(&(GPIOB ->ODR));
    DMA1_Channel2 ->CMAR = (uint32_t) &msg;
    DMA1_Channel2 ->CNDTR = 8;
    DMA1_Channel2 ->CCR |= DMA_CCR_DIR;
    DMA1_Channel2 ->CCR |= DMA_CCR_MINC;
    DMA1_Channel2 ->CCR |= DMA_CCR_MSIZE_0;
    DMA1_Channel2 ->CCR |= DMA_CCR_PSIZE_0;
    DMA1_Channel2 ->CCR |= DMA_CCR_CIRC;
}

void enable_dma(void) {
    DMA1_Channel2 ->CCR |= DMA_CCR_EN;
}

void init_tim2(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    TIM2->PSC = 48 - 1;
    TIM2->ARR = 1000 - 1;
    TIM2->CR1 |= TIM_CR1_CEN;
    TIM2->DIER |= TIM_DIER_UDE;
}

void append_display(char val) {
    uint16_t x = msg[0];
    for(int i = 0; i <= 6; i++)
    {
        uint16_t temp  = msg[i + 1];
        msg[i] = temp;
    }
    x += val;
    msg[7] = x;
}

//===========================================================================
// 2.3 Debouncing a Keypad
//===========================================================================
const char keymap[] = "DCBA#9630852*741";
uint8_t hist[16];
uint8_t col;
char queue[2];
int qin;
int qout;

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
        return keymap[pop_queue()];
    }
}

//===========================================================================
// 2.4 Reading an Analog Voltage
//===========================================================================
int volume = 2048;

void setup_adc_dma(void) {
    RCC ->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel1 ->CCR &= ~DMA_CCR_EN;
    DMA1_Channel1 ->CPAR = (uint32_t)(&(ADC1->DR));
    DMA1_Channel1 ->CMAR = (uint32_t)(&volume);
    DMA1_Channel1 ->CNDTR = 1;
    DMA1_Channel1 ->CCR &= ~DMA_CCR_DIR;
    DMA1_Channel1 ->CCR &= ~DMA_CCR_MINC;
    DMA1_Channel1 ->CCR &= ~DMA_CCR_PINC;
    DMA1_Channel1 ->CCR |= DMA_CCR_MSIZE_0;
    DMA1_Channel1 ->CCR |= DMA_CCR_PSIZE_0;
    DMA1_Channel1 ->CCR |= DMA_CCR_CIRC;
}

void enable_adc_dma(void) {
    DMA1_Channel1 ->CCR |= DMA_CCR_EN;
}

void setup_adc(void) {
    RCC ->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA ->MODER |= 0xc;
    RCC ->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC ->CR2 |= RCC_CR2_HSI14ON;
    while(!(RCC ->CR2 & RCC_CR2_HSI14RDY));
    ADC1 ->CR |= ADC_CR_ADEN;
    while(!(ADC1 ->ISR & ADC_ISR_ADRDY));
    ADC1 ->CFGR1 |= ADC_CFGR1_CONT;
    ADC1 ->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG;
    ADC1 ->CHSELR = 0;
    ADC1 ->CHSELR |= 1 << 1;
    while(!(ADC1 ->ISR & ADC_ISR_ADRDY));
    ADC1 ->CR |= ADC_CR_ADSTART;
    while(!(ADC1 ->ISR & ADC_ISR_EOC));

}


//===========================================================================
// 2.5 Output Sine Wave
//===========================================================================
#define N 1000
#define RATE 20000
short int wavetable[N];
int step = 0;
int offset = 0;

void init_wavetable(void) {
    for(int i=0; i < N; i++)
        wavetable[i] = 32767 * sin(2 * M_PI * i / N);
}

void set_freq(float f) {
    step = ((f * N * 65536) / (RATE));
}

void setup_dac() {
    RCC ->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA ->MODER |= 0x300;
    RCC ->APB1ENR |= RCC_APB1ENR_DACEN;
    DAC ->CR |= DAC_CR_TEN1;
    DAC ->CR |= DAC_CR_TSEL1;
    DAC ->CR |= DAC_CR_EN1;
}

void init_tim7(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 24 - 1;
    TIM7->ARR = 100 - 1;
    TIM7->DIER |= TIM_DIER_UIE;
    TIM7->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= 1 << TIM7_IRQn;
}

void TIM7_IRQHandler(void) {
    TIM7 ->SR &= ~TIM_SR_UIF;
    DAC ->SWTRIGR |= DAC_SWTRIGR_SWTRIG1;
    offset += step;
    if ((offset >> 16) >= N)
    {
        offset = offset - (N << 16);
    }
    int sample = wavetable[offset >> 16];
    sample = sample * volume;
    sample = sample >> 16;
    sample = sample + 2048;
    DAC ->DHR12R1 = sample;

}


//===========================================================================
// Main and supporting functions
//===========================================================================
// Turn on the dot of the rightmost display element.
void dot()
{
    msg[7] |= 0x80;
}

// Read an entire floating-point number.
float getfloat()
{
    int num = 0;
    int digits = 0;
    int decimal = 0;
    int enter = 0;
    memset(msg,0,16);
    // clear_display();
    msg[7] = (7 << 8) | font['0'];
    while(!enter) {
        int key = get_keypress();
        if (digits == 8) {
            if (key != '#')
                continue;
        }
        switch(key) {
        case '0':
            if (digits == 0)
                continue;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            num = num*10 + key-'0';
            decimal <<= 1;
            digits += 1;
            if (digits == 1)
            msg[7] = (7 << 8) | font[key];
                // display[7] = font[key];
            else
                append_display(font[key]);
                //shift(key);
            break;
        case '*':
            if (decimal == 0) {
                decimal = 1;
                dot();
            }
            break;
        case '#':
            enter = 1;
            break;
        default: continue; // ABCD
        }
    }
    float f = num;
    while (decimal) {
        decimal >>= 1;
        if (decimal)
            f = f/10.0;
    }
    return f;
}

int main(void)
{
    //autotest();
    msg[0] |= font['E'];
    msg[1] |= font['C'];
    msg[2] |= font['E'];
    msg[3] |= font[' '];
    msg[4] |= font['3'];
    msg[5] |= font['6'];
    msg[6] |= font['2'];
    msg[7] |= font[' '];

    // 2.1
    enable_ports();

    // 2.2
    setup_dma();
    enable_dma();
    init_tim2();

    // 2.3
    init_tim6();

    // 2.4
    setup_adc_dma();
    enable_adc_dma();
    setup_adc();

    // 2.5
    init_wavetable();
    setup_dac();
    init_tim7();

    int key = 0;
    for(;;) {
        char key = get_keypress();
        if (key == 'A')
            set_freq(getfloat());
    }
}

const char font[] = {
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x00, // 32: space
    0x86, // 33: exclamation
    0x22, // 34: double quote
    0x76, // 35: octothorpe
    0x00, // dollar
    0x00, // percent
    0x00, // ampersand
    0x20, // 39: single quote
    0x39, // 40: open paren
    0x0f, // 41: close paren
    0x49, // 42: asterisk
    0x00, // plus
    0x10, // 44: comma
    0x40, // 45: minus
    0x80, // 46: period
    0x00, // slash
    // digits
    0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x67,
    // seven unknown
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    // Uppercase
    0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x6f, 0x76, 0x30, 0x1e, 0x00, 0x38, 0x00,
    0x37, 0x3f, 0x73, 0x7b, 0x31, 0x6d, 0x78, 0x3e, 0x00, 0x00, 0x00, 0x6e, 0x00,
    0x39, // 91: open square bracket
    0x00, // backslash
    0x0f, // 93: close square bracket
    0x00, // circumflex
    0x08, // 95: underscore
    0x20, // 96: backquote
    // Lowercase
    0x5f, 0x7c, 0x58, 0x5e, 0x79, 0x71, 0x6f, 0x74, 0x10, 0x0e, 0x00, 0x30, 0x00,
    0x54, 0x5c, 0x73, 0x7b, 0x50, 0x6d, 0x78, 0x1c, 0x00, 0x00, 0x00, 0x6e, 0x00
};
