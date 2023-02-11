//============================================================================
// ECE 362 Lab Experiment 5
// Basic Timers
// x78gVXhUVkpzv8ns
//AbfX**agvYMg4W#p
//============================================================================
.cpu cortex-m0
.thumb
.syntax unified

// RCC configuration registers
.equ  RCC,      0x40021000
.equ  AHBENR,   0x14
.equ  GPIOCEN,  0x00080000
.equ  GPIOBEN,  0x00040000
.equ  GPIOAEN,  0x00020000
.equ  APB1ENR,  0x1c
.equ  TIM6EN,   1<<4
.equ  TIM7EN,   1<<5

// NVIC configuration registers
.equ NVIC, 0xe000e000
.equ ISER, 0x100
.equ ICER, 0x180
.equ ISPR, 0x200
.equ ICPR, 0x280
.equ IPR,  0x400
.equ TIM6_DAC_IRQn, 17
.equ TIM7_IRQn,     18

// Timer configuration registers
.equ TIM6, 0x40001000
.equ TIM7, 0x40001400
.equ TIM_CR1,  0x00
.equ TIM_CR2,  0x04
.equ TIM_DIER, 0x0c
.equ TIM_SR,   0x10
.equ TIM_EGR,  0x14
.equ TIM_CNT,  0x24
.equ TIM_PSC,  0x28
.equ TIM_ARR,  0x2c

// Timer configuration register bits
.equ TIM_CR1_CEN,  1<<0
.equ TIM_DIER_UDE, 1<<8
.equ TIM_DIER_UIE, 1<<0
.equ TIM_SR_UIF,   1<<0

// GPIO configuration registers
.equ  GPIOC,    0x48000800
.equ  GPIOB,    0x48000400
.equ  GPIOA,    0x48000000
.equ  MODER,    0x00
.equ  PUPDR,    0x0c
.equ  IDR,      0x10
.equ  ODR,      0x14
.equ  BSRR,     0x18
.equ  BRR,      0x28

//============================================================================
// void enable_ports(void) {
//   // Set up the ports and pins exactly as directed.
// }
.global enable_ports
enable_ports:
	//init c
	ldr  r0, =RCC
	ldr  r1, [r0, #AHBENR]
	ldr  r2, =GPIOCEN
	orrs r1, r2
	str  r1, [r0, #AHBENR]

	//Configures pins PC4 – PC7 to be outputs
	//Configures pin PC9 to be an output
	ldr  r0, =GPIOC
	ldr  r1, [r0, #MODER]
	ldr  r2, =0xcff00
	bics r1, r2
	str  r1, [r0, #MODER]
	ldr  r2, =0x45500
	orrs r1, r2
	str  r1, [r0, #MODER]

	//Configures pins PC0 – PC3 to be inputs
	ldr  r0, =GPIOC
	ldr  r1, [r0, #MODER]
	ldr  r2, =0xff
	bics r1, r2
	str  r1, [r0, #MODER]

	//Configures pins PC0 – PC3 to be internally pulled low
	ldr  r0, =GPIOC
	ldr  r1, [r0, #PUPDR]
	ldr  r2, =0xff
	bics r1, r2
	str  r1, [r0, #PUPDR]
	ldr  r2, =0xaa
	orrs r1, r2
	str  r1, [r0, #PUPDR]

	//init b
	ldr  r0, =RCC
	ldr  r1, [r0, #AHBENR]
	ldr  r2, =GPIOBEN
	orrs r1, r2
	str  r1, [r0, #AHBENR]

	//Configures pins PB0 – PB10 to be outputs
	ldr  r0, =GPIOB
	ldr  r1, [r0, #MODER]
	ldr  r2, =0x3fffff
	bics r1, r2
	str  r1, [r0, #MODER]
	ldr  r2, =0x155555
	orrs r1, r2
	str  r1, [r0, #MODER]
//============================================================================
// TIM6_ISR() {
//   TIM6->SR &= ~TIM_SR_UIF
//   if (GPIOC->ODR & (1<<9))
//     GPIOC->BRR = 1<<9;
//   else
//     GPIOC->BSRR = 1<<9;
// }
.type TIM6_DAC_IRQHandler, %function
.global TIM6_DAC_IRQHandler
TIM6_DAC_IRQHandler:
	push {lr}
	ldr r0,=TIM6
	ldr r1,[r0,#TIM_SR]
	ldr r2,=TIM_SR_UIF
	bics r1,r2
	str r1,[r0,#TIM_SR]

	ldr r0,=GPIOC
	ldr r1,[r0,#ODR]
	movs r2,#1
	movs r3,#9
	lsls r2,r3
	ands r1,r2
	//lsls r1,r3
	cmp r1,#0
	beq else1
//if1
	ldr r0,=GPIOC
	movs r2,#1
	movs r3,#9
	lsls r2,r3
	str r2,[r0,#BRR]
	b endif1
else1:
	ldr r0,=GPIOC
	movs r2,#1
	movs r3,#9
	lsls r2,r3
	str r2,[r0,#BSRR]
endif1:
	pop {pc}


//============================================================================
// Implement the setup_tim6 subroutine below.  Follow the instructions in the
// lab text.
.global setup_tim6
setup_tim6:
	push {lr}
	//Enables the RCC clock to Timer 6
	ldr r0,=RCC
	ldr r1,[r0,#APB1ENR]
	ldr r2,=TIM6EN
	orrs r1,r2
	str r1,[r0,#APB1ENR]

	//Configure TIM6_PSC to prescale the system clock by 48000.
	ldr r0,=TIM6
	ldr r1,=47999
	str r1,[r0,#TIM_PSC]

	//Configure the Timer 6 auto-reload register (TIM6_ARR) to have a counting period of 500
	ldr r1,=499
	str r1,[r0,#TIM_ARR]

	//Configure the Timer 6 DMA/Interrupt Enable Register (TIM6_DIER) to enable the UIE flag
	ldr r0,=TIM6
	ldr r1,[r0,#TIM_DIER]
	ldr r2,=TIM_DIER_UIE
	orrs r1,r2
	str r1,[r0,#TIM_DIER]

	//Enable Timer 6 to start counting by setting the CEN bit in the Timer 6 Control Register 1
	ldr r0,=TIM6
	ldr r1,[r0,#TIM_CR1]
	ldr r2,=TIM_CR1_CEN
	orrs r1,r2
	str r1,[r0,#TIM_CR1]

	//Enable the interrupt for Timer 6 in the NVIC ISER

	ldr  r0,=NVIC
    ldr  r1,=ISER
    ldr  r2,=1<<TIM6_DAC_IRQn
    str  r2,[r0,r1]

    pop {pc}


//============================================================================
// void show_char(int col, char ch) {
//   GPIOB->ODR = ((col & 7) << 8) | font[ch];
// }
.global show_char
show_char:
	push {r4-r7, lr}
	//r0 = col, r1 = ch
	movs r4,r0 //r4 = col
	movs r5,r1 //r5 = ch
	movs r6,#7
	ands r4,r6 //r4 = col & 7
	movs r6,#8
	lsls r4,r6
	ldr r6,=font
	ldrb r7,[r6,r5]
	orrs r4,r7
	ldr r5,=GPIOB
	str r4,[r5,#ODR]

	pop {r4-r7, pc}
//============================================================================
// nano_wait(int x)
// Wait the number of nanoseconds specified by x.
.global nano_wait
nano_wait:
	subs r0,#83
	bgt nano_wait
	bx lr

//============================================================================
// This subroutine is provided for you to fill the LED matrix with "AbCdEFgH".
// It is a very useful subroutine for debugging.  Study it carefully.
.global fill_alpha
fill_alpha:
	push {r4,r5,lr}
	movs r4,#0
fillloop:
	movs r5,#'A' // load the character 'A' (integer value 65)
	adds r5,r4
	movs r0,r4
	movs r1,r5
	bl   show_char
	adds r4,#1
	movs r0,#7
	ands r4,r0
	ldr  r0,=1000000
	bl   nano_wait
	b    fillloop
	pop {r4,r5,pc} // not actually reached

//============================================================================
// void drive_column(int c) {
//   c = c & 3;
//   GPIOC->BSRR = 0xf00000 | (1 << (c + 4));
// }
.global drive_column
drive_column:
	push {lr}
	//r0 = col
	movs r1,#3
	ands r1,r0
	adds r1,#4
	movs r2,#1
	lsls r2,r1
	ldr r3,=0xf00000
	orrs r2,r3

	ldr r3,=GPIOC
	str r2,[r3,#BSRR]
	pop {pc}
//============================================================================
// int read_rows(void) {
//   return GPIOC->IDR & 0xf;
// }
.global read_rows
read_rows:
	push {r4-r7, lr}
	ldr r4,=GPIOC
	ldr r5,[r4,#IDR]
	ldr r6,=0xf
	ands r5,r6
	movs r0,r5
	pop {r4-r7, pc}

//============================================================================
// void update_history(int c, int r) {
//   c = c & 3;
//   for(int n=0; n<4; n++) {
//     history[4*c + n] <<= 1;
//     history[4*c + n] |= (r>>n) & 1;
//   }
// }
.global update_history
update_history:
	push {r4-r7, lr}
	//r0 = c, r1 = r
	movs r7,#0 //r7 = n

	movs r2,#3
	ands r2,r0 //r2 = c
yesyes:
	cmp r7,#4
	blt loop1
	b endw
loop1:
	ldr r3,=hist
	movs r4,#4
	muls r4,r2
	adds r4,r7 //r4 = 4*c + n
	ldrb r5,[r3,r4]
	movs r6,#1
	lsls r5,r6 //r5 = history[4*c + n]
	movs r6,r1 //r6 = r
	lsrs r6,r7 //(r>>n)
	movs r3,#1
	ands r6,r3 //r6 = (r>>n) & 1
	adds r5,r6
	ldr r3,=hist
	strb r5,[r3,r4]
	adds r7,#1
	b yesyes
endw:
	pop {r4-r7, pc}
//============================================================================
// TIM7_ISR() {
//    TIM7->SR &= ~TIM_SR_UIF
//    update_history(col);
//    show_char(col, disp[col])
//    col = col + 1;
//    drive_column(col);
// }
.type TIM7_IRQHandler, %function
.global TIM7_IRQHandler
TIM7_IRQHandler:
	push {r4-r7, lr}
	ldr r0,=TIM7
	ldr r1,[r0,#TIM_SR]
	ldr r2,=TIM_SR_UIF
	bics r1,r2
	str r1,[r0,#TIM_SR]

	bl read_rows
	movs r1,r0 //r1 = rows
	ldr r2,=col
	ldrb r0, [r2]

	bl update_history
	ldr r3,=disp
	ldr r2,=col
	ldrb r0,[r2]
	ldrb r1,[r3,r0] //r1 = disp[col]
	bl show_char

	ldr r2,=col
	ldrb r0,[r2] //r0 = col
	adds r0,#1
	movs r5,#7
	ands r0,r5
	strb r0,[r2] //r0 = col
	bl drive_column

	pop {r4-r7, pc}
//============================================================================
// Implement the setup_tim7 subroutine below.  Follow the instructions
// in the lab text.
.global setup_tim7
setup_tim7:
	push {lr}
	//Enables the RCC clock to Timer 7
	ldr r0,=RCC
	ldr r1,[r0,#APB1ENR]
	ldr r2,=TIM7EN
	orrs r1,r2
	str r1,[r0,#APB1ENR]

	//Configure TIM7_PSC to prescale the system clock by 48000.
	ldr r0,=TIM7
	ldr r1,=4799
	str r1,[r0,#TIM_PSC]

	//Configure the Timer 7 auto-reload register (TIM7_ARR) to have a counting period of 500
	ldr r1,=9
	str r1,[r0,#TIM_ARR]

	//Configure the Timer 7 DMA/Interrupt Enable Register (TIM7_DIER) to enable the UIE flag
	ldr r0,=TIM7
	ldr r1,[r0,#TIM_DIER]
	ldr r2,=TIM_DIER_UIE
	orrs r1,r2
	str r1,[r0,#TIM_DIER]

	//Enable the interrupt for Timer 7 in the NVIC ISER
	ldr  r0,=NVIC
    ldr  r1,=ISER
    ldr  r2,=1<<TIM7_IRQn
    str  r2,[r0,r1]

	//Enable Timer 7 to start counting by setting the CEN bit in the Timer 7 Control Register 1
	ldr r0,=TIM7
	ldr r1,[r0,#TIM_CR1]
	ldr r2,=TIM_CR1_CEN
	orrs r1,r2
	str r1,[r0,#TIM_CR1]

    pop {pc}

//============================================================================
// int wait_for(char val) {
//   for(;;) {
//     wfi
//     for(int n=0; n<16; n++)
//       if (hist[n] == val)
//         return n;
//   }
// }
.global wait_for
wait_for:
	wfi
	movs r1,#0
loop2:
	ldr r2,=hist
	ldrb r3, [r2,r1]

	cmp r3,r0
	beq then
	adds r1,#1
	cmp r1,#16
	blt loop2
	b wait_for
then:
	movs r0,r1
	bx lr


//============================================================================
// void shift_display() {
//   for(int i=0; i<7; i++)
//     disp[i] = disp[i+1];
// }
.global shift_display
shift_display:
	push {r4-r7, lr}
	movs r0,#0
yes:
	cmp r0,#7
	blt loop3
	b end
loop3:
	ldr r1,=disp
	movs r3,r0
	adds r3,#1
	ldrb r4,[r1,r3]
	strb r4,[r1,r0]
	adds r0,#1
	b yes
end:
	pop {r4-r7, pc}


//============================================================================
// This subroutine is provided for you to call the functions you wrote.
// It waits for a key to be pressed.  When it finds one, it immediately
// shifts the display left, looks up the character for the key, and writes
// the new character in the rightmost element of disp.
// Then it waits for a key to be released.
.global display_key
display_key:
	push {r4,lr}
	movs r0,#1
	bl   wait_for

	movs r4,r0
	bl   shift_display
	ldr  r0,=disp
	ldr  r2,=keymap
	ldrb r1,[r2,r4]
	strb r1,[r0,#7]

	movs r0,#0xfe
	bl   wait_for

	pop {r4,pc}

.global login
login: .string "yin181" // Replace with your login.
.balign 2

.global main
main:
	//bl autotest
	bl enable_ports
	bl setup_tim6
	//bl fill_alpha
	bl setup_tim7
display_loop:
	bl display_key
	b  display_loop
	// Does not return.


//============================================================================
// Map the key numbers in the history array to characters.
// We just use a string for this.
.global keymap
keymap:
.string "DCBA#9630852*741"

//============================================================================
// This table is a *font*.  It provides a mapping between ASCII character
// numbers and the LED segments to illuminate for those characters.
// For instance, the character '2' has an ASCII value 50.  Element 50
// of the font array should be the 8-bit pattern to illuminate segments
// A, B, D, E, and G.  Spread out, those patterns would correspond to:
//   .GFEDCBA
//   01011011 = 0x5b
// Accessing the element 50 of the font table will retrieve the value 0x5b.
//
.global font
font:
.space 32
.byte  0x00 // 32: space
.byte  0x86 // 33: exclamation
.byte  0x22 // 34: double quote
.byte  0x76 // 35: octothorpe
.byte  0x00 // dollar
.byte  0x00 // percent
.byte  0x00 // ampersand
.byte  0x20 // 39: single quote
.byte  0x39 // 40: open paren
.byte  0x0f // 41: close paren
.byte  0x49 // 42: asterisk
.byte  0x00 // plus
.byte  0x10 // 44: comma
.byte  0x40 // 45: minus
.byte  0x80 // 46: period
.byte  0x00 // slash
.byte  0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07
.byte  0x7f, 0x67
.space 7
// Uppercase alphabet
.byte  0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x6f, 0x76, 0x30, 0x1e, 0x00, 0x38, 0x00
.byte  0x37, 0x3f, 0x73, 0x7b, 0x31, 0x6d, 0x78, 0x3e, 0x00, 0x00, 0x00, 0x6e, 0x00
.byte  0x39 // 91: open square bracket
.byte  0x00 // backslash
.byte  0x0f // 93: close square bracket
.byte  0x00 // circumflex
.byte  0x08 // 95: underscore
.byte  0x20 // 96: backquote
// Lowercase alphabet
.byte  0x5f, 0x7c, 0x58, 0x5e, 0x79, 0x71, 0x6f, 0x74, 0x10, 0x0e, 0x00, 0x30, 0x00
.byte  0x54, 0x5c, 0x73, 0x7b, 0x50, 0x6d, 0x78, 0x1c, 0x00, 0x00, 0x00, 0x6e, 0x00
.balign 2

//============================================================================
// Data structures for this experiment.
// Guard bytes are placed between variables so that autotest can (potentially)
// detect corruption caused by bad updates.
//
.data
.global col
.global hist
.global disp
guard1: .byte 0
disp: .string "Hello..."
guard2: .byte 0
col: .byte 0
guard3: .byte 0
hist: .byte 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
guard4: .byte 0
