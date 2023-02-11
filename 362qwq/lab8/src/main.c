
#include "stm32f0xx.h"

// Be sure to change this to your login...
const char login[] = "yin181";

void nano_wait(unsigned int n) {
    asm(    "        mov r0,%0\n"
            "repeat: sub r0,#83\n"
            "        bgt repeat\n" : : "r"(n) : "r0", "cc");
}


//===========================================================================
// 2.1 FROM LAB 6
// ..........................................................................
// Configuring GPIO
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
// Debouncing a Keypad
//===========================================================================
const char keymap[] = "DCBA#9630852*741";
uint8_t hist[16];
uint8_t col;
char queue[2];
int qin;
int qout;

int msg_index = 0;
uint16_t msg[8] = { 0x0000,0x0100,0x0200,0x0300,0x0400,0x0500,0x0600,0x0700 };
extern const char font[];

void drive_column(int c) {
    GPIOC->BSRR = 0xf00000 | (1 << (c + 4));
}

int read_rows() {
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

void update_history(int c, int rows) {
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

void append_display(char val) {
    uint16_t temp1 = msg[1];
    uint16_t temp2 = msg[2];
    uint16_t temp3 = msg[3];
    uint16_t temp4 = msg[4];
    uint16_t temp5 = msg[5];
    uint16_t temp6 = msg[6];
    uint16_t temp7 = msg[7];

    temp1 = temp1 & 0xff;
    temp2 = temp2 & 0xff;
    temp3 = temp3 & 0xff;
    temp4 = temp4 & 0xff;
    temp5 = temp5 & 0xff;
    temp6 = temp6 & 0xff;
    temp7 = temp7 & 0xff;

    msg[0] = 0x0000;
    msg[0] |= temp1;
    msg[1] = 0x0100;
    msg[1] |= temp2;
    msg[2] = 0x0200;
    msg[2] |= temp3;
    msg[3] = 0x0300;
    msg[3] |= temp4;
    msg[4] = 0x0400;
    msg[4] |= temp5;
    msg[5] = 0x0500;
    msg[5] |= temp6;
    msg[6] = 0x0600;
    msg[6] |= temp7;
    msg[7] = 0x0700;
    msg[7] |= val;
}


//===========================================================================
// This Lab 7
// ..........................................................................
// 2.2 Bit Bang SPI LED Array
//===========================================================================

void setup_bb(void) {
    RCC ->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB ->MODER &= ~GPIO_MODER_MODER12;
    GPIOB ->MODER |= GPIO_MODER_MODER12_0;
    GPIOB ->MODER &= ~GPIO_MODER_MODER13;
    GPIOB ->MODER |= GPIO_MODER_MODER13_0;
    GPIOB ->MODER &= ~GPIO_MODER_MODER15;
    GPIOB ->MODER |= GPIO_MODER_MODER15_0;

    GPIOB ->ODR |= GPIO_ODR_12;
    GPIOB ->ODR &= ~GPIO_ODR_13;

}

void small_delay(void) {
    //nano_wait(5000000);
}

void bb_write_bit(int val) {
    if (val == 0)
    {
        GPIOB ->ODR &= ~GPIO_ODR_15;
    }
    else
    {
        GPIOB ->ODR |= GPIO_ODR_15;
    }
    small_delay();
    GPIOB ->ODR |= GPIO_ODR_13;
    small_delay();
    GPIOB ->ODR &= ~GPIO_ODR_13;
}

void bb_write_halfword(int halfword) {
    GPIOB ->ODR &= ~GPIO_ODR_12;
    for (int i = 15; i >= 0; i--)
    {
        bb_write_bit((halfword >> i) & 1);
    }
    GPIOB ->ODR |= GPIO_ODR_12;
}

void init_tim7(void) {
    RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
    TIM7->PSC = 48 - 1;
    TIM7->ARR = 1000 - 1;
    TIM7->DIER |= TIM_DIER_UIE;
    TIM7->CR1 |= TIM_CR1_CEN;
    NVIC->ISER[0] |= 1 << TIM7_IRQn;
}

void TIM7_IRQHandler(void) {
    TIM7 ->SR &= ~TIM_SR_UIF;
    while (msg_index <= 7)
    {
        bb_write_halfword(msg[msg_index]);
        msg_index ++;
    }
    msg_index = 0;
}

//===========================================================================
// 2.3 SPI DMA LED Array
//===========================================================================
void init_spi2(void) {
    RCC ->AHBENR |= RCC_AHBENR_GPIOBEN;
    GPIOB ->MODER &= ~GPIO_MODER_MODER12;
    GPIOB ->MODER |= GPIO_MODER_MODER12_1;
    GPIOB ->MODER &= ~GPIO_MODER_MODER13;
    GPIOB ->MODER |= GPIO_MODER_MODER13_1;
    GPIOB ->MODER &= ~GPIO_MODER_MODER15;
    GPIOB ->MODER |= GPIO_MODER_MODER15_1;

    GPIOB ->AFR[1] &= ~GPIO_AFRH_AFR12;
    GPIOB ->AFR[1] &= ~GPIO_AFRH_AFR13;
    GPIOB ->AFR[1] &= ~GPIO_AFRH_AFR15;
    GPIOB ->AFR[1] |= 0x0 << (4*(12-8));
    GPIOB ->AFR[1] |= 0x0 << (4*(13-8));
    GPIOB ->AFR[1] |= 0x0 << (4*(15-8));

    RCC ->APB1ENR |= RCC_APB1ENR_SPI2EN;
    SPI2 ->CR1 &= ~SPI_CR1_SPE;
    SPI2 ->CR1 |= SPI_CR1_BR;
    SPI2 ->CR2 = SPI_CR2_DS_3 | SPI_CR2_DS_2 | SPI_CR2_DS_1 | SPI_CR2_DS_0;
    SPI2 ->CR1 |= SPI_CR1_MSTR;
    SPI2 ->CR2 |= SPI_CR2_SSOE | SPI_CR2_NSSP;
    SPI2 ->CR2 |= SPI_CR2_TXDMAEN;
    SPI2 ->CR1 |= SPI_CR1_SPE;

}

void setup_dma(void) {
    RCC ->AHBENR |= RCC_AHBENR_DMA1EN;
    DMA1_Channel5 ->CCR &= ~DMA_CCR_EN;
    DMA1_Channel5 ->CPAR = (uint32_t)(&(SPI2 -> DR));
    DMA1_Channel5 ->CMAR = (uint32_t) &msg;
    DMA1_Channel5 ->CNDTR = 8;
    DMA1_Channel5 ->CCR |= DMA_CCR_DIR;
    DMA1_Channel5 ->CCR |= DMA_CCR_MINC;
    DMA1_Channel5 ->CCR |= DMA_CCR_MSIZE_0;
    DMA1_Channel5 ->CCR |= DMA_CCR_PSIZE_0;
    DMA1_Channel5 ->CCR |= DMA_CCR_CIRC;
}

void enable_dma(void) {
    DMA1_Channel5 ->CCR |= DMA_CCR_EN;
}


//===========================================================================
// 2.4 SPI OLED Display
//===========================================================================
void setup_spi1() {
    RCC ->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA ->MODER &= ~GPIO_MODER_MODER5;
    GPIOA ->MODER |= GPIO_MODER_MODER5_1;
    GPIOA ->MODER &= ~GPIO_MODER_MODER6;
    GPIOA ->MODER |= GPIO_MODER_MODER6_1;
    GPIOA ->MODER &= ~GPIO_MODER_MODER7;
    GPIOA ->MODER |= GPIO_MODER_MODER7_1;
    GPIOA ->MODER &= ~GPIO_MODER_MODER15;
    GPIOA ->MODER |= GPIO_MODER_MODER15_1;

    GPIOA ->AFR[0] &= ~GPIO_AFRL_AFR5;
    GPIOA ->AFR[0] &= ~GPIO_AFRL_AFR6;
    GPIOA ->AFR[0] &= ~GPIO_AFRL_AFR7;
    GPIOA ->AFR[1] &= ~GPIO_AFRH_AFR15;

    GPIOA ->AFR[0] |= 0x0 << (4*5);
    GPIOA ->AFR[0] |= 0x0 << (4*6);
    GPIOA ->AFR[0] |= 0x0 << (4*7);
    GPIOA ->AFR[1] |= 0x0 << (4*(15-8));

    RCC ->APB2ENR |= RCC_APB2ENR_SPI1EN;
    SPI1 ->CR1 &= ~SPI_CR1_SPE;
    SPI1 ->CR1 |= SPI_CR1_BR;
    SPI1 ->CR2 = SPI_CR2_DS_3 | SPI_CR2_DS_0;
    SPI1 ->CR1 |= SPI_CR1_MSTR;
    SPI1 ->CR2 |= SPI_CR2_SSOE | SPI_CR2_NSSP;
    SPI1 ->CR1 |= SPI_CR1_SPE;
}

void spi_cmd(unsigned int data) {
    while ((SPI1 ->SR & SPI_SR_TXE) == 0);
    SPI1 ->DR = data;

    //while ((SPI2 ->SR & SPI_SR_TXE) == 0);
    //SPI2 ->DR = data;
}

void spi_data(unsigned int data) {
    while ((SPI1 ->SR & SPI_SR_TXE) == 0);
    SPI1 ->DR = data + 0x200;

    //while ((SPI2 ->SR & SPI_SR_TXE) == 0);
    //SPI2 ->DR = data | 0x200;
}

void spi_init_oled() {
    nano_wait(1000000);
    spi_cmd(0x38);
    spi_cmd(0x08);
    spi_cmd(0x01);

    nano_wait(2000000);
    spi_cmd(0x06);
    spi_cmd(0x02);
    spi_cmd(0x0c);
}

void spi_display1(const char *string) {
    spi_cmd(0x02);
    int i = 0;
    while (string[i] != 0)
    {
        spi_data(string[i]);
        i ++;
    }
}

void spi_display2(const char *string) {
    spi_cmd(0xc0);
    int i = 0;
    while (string[i] != 0)
    {
        spi_data(string[i]);
        i ++;
    }
}


//===========================================================================
// Main and supporting functions
//===========================================================================

int main(void)
{
    msg[0] |= font['E'];
    msg[1] |= font['C'];
    msg[2] |= font['E'];
    msg[3] |= font[' '];
    msg[4] |= font['3'];
    msg[5] |= font['6'];
    msg[6] |= font['2'];
    msg[7] |= font[' '];

    // GPIO enable
    enable_ports();

    // setup keypad
    init_tim6();

    // 2.2 LED array Bit Bang
    // setup_bb();
    // init_tim7();

    // 2.3 LED array SPI
     setup_dma();
     enable_dma();
     init_spi2();

    // 2.4 SPI OLED
     setup_spi1();
     spi_init_oled();
     spi_display1("Hello again,");
     spi_display2(login);

    int key = 0;
    for(;;) {
        char key = get_keypress();
        append_display(font[key]);
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
