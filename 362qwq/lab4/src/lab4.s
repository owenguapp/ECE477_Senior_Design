.syntax unified
.cpu cortex-m0
.fpu softvfp
.thumb

//==================================================================
// ECE 362 Lab Experiment 4
// Interrupts
//==================================================================

// RCC config registers
.equ  RCC,      0x40021000
.equ  AHBENR,   0x14
.equ  GPIOCEN,  0x00080000
.equ  GPIOBEN,  0x00040000
.equ  GPIOAEN,  0x00020000
.equ  APB2ENR,  0x18
.equ  SYSCFGCOMPEN, 1

// GPIO config registers
.equ  GPIOC,    0x48000800
.equ  GPIOB,    0x48000400
.equ  GPIOA,    0x48000000
.equ  MODER,    0x00
.equ  PUPDR,    0x0c
.equ  IDR,      0x10
.equ  ODR,      0x14
.equ  BSRR,     0x18
.equ  BRR,      0x28

// SYSCFG config registers
.equ SYSCFG, 0x40010000
.equ EXTICR1, 0x08
.equ EXTICR2, 0x0c
.equ EXTICR3, 0x10
.equ EXTICR4, 0x14

// External interrupt config registers
.equ EXTI,  0x40010400
.equ IMR,   0
.equ EMR,   0x4
.equ RTSR,  0x8
.equ FTSR,  0xc
.equ SWIER, 0x10
.equ PR,    0x14

// Variables to register things for EXTI on pin 0
.equ EXTI_RTSR_TR0, 1<<0
.equ EXTI_IMR_MR0,  1<<0
.equ EXTI_PR_PR0,   1<<0
// Variables to register things for EXTI on pin 1
.equ EXTI_RTSR_TR1, 1<<1
.equ EXTI_IMR_MR1,  1<<1
.equ EXTI_PR_PR1,   1<<1
// Variables to register things for EXTI on pin 2
.equ EXTI_RTSR_TR2, 1<<2
.equ EXTI_IMR_MR2,  1<<2
.equ EXTI_PR_PR2,   1<<2
// Variables to register things for EXTI on pin 3
.equ EXTI_RTSR_TR3, 1<<3
.equ EXTI_IMR_MR3,  1<<3
.equ EXTI_PR_PR3,   1<<3
// Variables to register things for EXTI on pin 4
.equ EXTI_RTSR_TR4, 1<<4
.equ EXTI_IMR_MR4,  1<<4
.equ EXTI_PR_PR4,   1<<4

// SysTick counter variables....
.equ STK, 0xe000e010
.equ CSR, 0x0
.equ RVR, 0x4
.equ CVR, 0x8

// NVIC config registers
.equ NVIC, 0xe000e000
.equ ISER, 0x100
.equ ICER, 0x180
.equ ISPR, 0x200
.equ ICPR, 0x280
.equ IPR,  0x400
.equ EXTI0_1_IRQn,5  // External interrupt number for pins 0 and 1 is IRQ 5.
.equ EXTI2_3_IRQn,6  // External interrupt number for pins 2 and 3 is IRQ 6.
.equ EXTI4_15_IRQn,7 // External interrupt number for pins 4 - 15 is IRQ 7.

// GPIO config registers
.equ  GPIOC,    0x48000800
.equ  GPIOB,    0x48000400
.equ  GPIOA,    0x48000000
.equ  MODER,    0x00
.equ  PUPDR,    0x0c
.equ  IDR,      0x10
.equ  ODR,      0x14
.equ  BSRR,     0x18
.equ  BRR,      0x28

//==========================================================
// nano_wait
// Loop for approximately the specified number of nanoseconds.
// Write the entire subroutine below.
.global nano_wait
nano_wait:
	subs r0,#83
	bgt  nano_wait
	bx lr

//==========================================================
// initc
// Enable the RCC clock for GPIO C and configure pins as
// described in section 2.2.1.
// Do not modify any other pin configuration.
// Parameters: none
// Write the entire subroutine below.
.global initc
initc:
	ldr  r0, =RCC // Load, into RO, base address of RCC
	ldr  r1, [r0, #AHBENR] // Load, into R1, value of RCC offset by AHBENR
	ldr  r2, =GPIOCEN// Load, into R2, value to enable Port C
	orrs r1, r2 // Combine R2 into R1
	str  r1, [r0, #AHBENR] // Store new bits into RCC_AHBENR

	ldr  r0, =GPIOC // Load, into RO, base address of GPIOC
	ldr  r1, [r0, #MODER] // Load, into R1, value of GPIOC offset by MODER
	ldr  r2, =0x3ff
	bics r1, r2 // Combine R2 into R1
	str  r1, [r0, #MODER]

	ldr  r0, =GPIOC // Load, into RO, base address of GPIOC
	ldr  r1, [r0, #PUPDR] // Load, into R1, value of GPIOC offset by MODER
	ldr  r2, =0x2aa
	orrs r1, r2 // Combine R2 into R1
	str  r1, [r0, #IDR]

	ldr  r0, =GPIOC // Load, into RO, base address of GPIOC
	ldr  r1, [r0, #MODER] // Load, into R1, value of GPIOC offset by MODER
	ldr  r2, =0x55500
	orrs r1, r2 // Combine R2 into R1
	str  r1, [r0, #MODER]


//==========================================================
// initb
// Enable the RCC clock for GPIO B and configure pins as
// described in section 2.2.2
// Do not modify any other pin configuration.
// Parameters: none
// Write the entire subroutine below.
.global initb
initb:
	ldr  r0, =RCC // Load, into RO, base address of RCC
	ldr  r1, [r0, #AHBENR] // Load, into R1, value of RCC offset by AHBENR
	ldr  r2, =GPIOBEN// Load, into R2, value to enable Port C
	orrs r1, r2 // Combine R2 into R1
	str  r1, [r0, #AHBENR] // Store new bits into RCC_AHBENR

	ldr  r0, =GPIOB // Load, into RO, base address of GPIOC
	ldr  r1, [r0, #MODER] // Load, into R1, value of GPIOC offset by MODER
	ldr  r2, =0x3f3
	bics r1, r2 // Combine R2 into R1
	str  r1, [r0, #MODER]

	ldr  r0, =GPIOB // Load, into RO, base address of GPIOC
	ldr  r1, [r0, #PUPDR] // Load, into R1, value of GPIOC offset by MODER
	ldr  r2, =0xa0
	orrs r1, r2 // Combine R2 into R1
	str  r1, [r0, #IDR]

	ldr  r0, =GPIOC // Load, into RO, base address of GPIOC
	ldr  r1, [r0, #MODER] // Load, into R1, value of GPIOC offset by MODER
	ldr  r2, =0x550000
	orrs r1, r2 // Combine R2 into R1
	str  r1, [r0, #MODER]

//==========================================================
// togglexn
// Change the ODR value from 0 to 1 or 1 to 0 for a specified
// pin of Port C.
// Parameters: r0 holds the base address of the GPIO port
//                to use
//             r1 holds the pin number to toggle
// Write the entire subroutine below.
.global togglexn
togglexn:
	push {lr}
	ldr r2, [r0, #ODR]
	movs r3, #1
	lsls r3, r1
	eors r3, r2
	str r3, [r0, #ODR]
	pop {pc}

//==========================================================
//EXTI 0 2 4
.type EXTI0_1_IRQHandler, %function
.global EXTI0_1_IRQHandler
EXTI0_1_IRQHandler:
	push {lr}
	ldr r0, =EXTI
	//ldr r1, [r0, #PR]
	ldr r2, =EXTI_PR_PR0
	//orrs r2, r1
	str r2, [r0, #PR]

	ldr  r0,=GPIOB
	movs r1,#8
	b togglexn
	pop {pc}


.type EXTI2_3_IRQHandler, %function
.global EXTI2_3_IRQHandler
EXTI2_3_IRQHandler:
	push {lr}
	ldr r0, =EXTI
	//ldr r1, [r0, #PR]
	ldr r2, =EXTI_PR_PR2
	//orrs r2, r1
	str r2, [r0, #PR]

	ldr  r0,=GPIOB
	movs r1,#9
	b togglexn
	pop {pc}

.type EXTI4_15_IRQHandler, %function
.global EXTI4_15_IRQHandler
EXTI4_15_IRQHandler:
	push {lr}
	ldr r0, =EXTI
	//ldr r1, [r0, #PR]
	ldr r2, =EXTI_PR_PR4
	//orrs r2, r1
	str r2, [r0, #PR]

	ldr  r0,=GPIOB
	movs r1,#10
	b togglexn
	pop {pc}

//==========================================================
// Write the EXTI interrupt handler for pins 2 and 3 below.
// Copy the name from startup/startup_stm32.s, create a label
// of that name below, declare it to be global, and declare
// it to be a function.
// It acknowledge the pending bit for pin 3, and it should
// call toggle_portc_pin(8).

//==========================================================
// Write the EXTI interrupt handler for pins 4-15 below.
// It should acknowledge the pending bit for pin4, and it
// should call toggle_portc_pin(9).

//init_exti
.global init_exti
init_exti:
	push {lr}
	// Student code goes below
	//2.4.1
	ldr  r0, =RCC // Load, into RO, base address of RCC
	ldr  r1, [r0, #APB2ENR] // Load, into R1, value of RCC offset by AHBENR
	ldr  r2, =SYSCFGCOMPEN// Load, into R2, value to enable Port C
	orrs r1, r2 // Combine R2 into R1
	str  r1, [r0, #APB2ENR] // Store new bits into RCC_AHBENR

	//2.4.2
	ldr r0, =SYSCFG
	ldr r1, [r0, #EXTICR1]
	ldr r2, =0x1111
	str r2, [r0, #EXTICR1]

	ldr r0, =SYSCFG
	ldr r1, [r0, #EXTICR2]
	ldr r2, =0x1
	str r2, [r0, #EXTICR2]

	//2.4.3
	ldr r0, =EXTI
	ldr r1, [r0, #RTSR]
	ldr r2, =EXTI_RTSR_TR0
	orrs r2, r1
	str r2, [r0, #RTSR]

	ldr r0, =EXTI
	ldr r1, [r0, #RTSR]
	ldr r2, =EXTI_RTSR_TR2
	orrs r2, r1
	str r2, [r0, #RTSR]

	ldr r0, =EXTI
	ldr r1, [r0, #RTSR]
	ldr r2, =EXTI_RTSR_TR4
	orrs r2, r1
	str r2, [r0, #RTSR]

	//2.4.4
	ldr r0, =EXTI
	ldr r1, [r0, #IMR]
	ldr r2, =EXTI_IMR_MR0
	orrs r2, r1
	str r2, [r0, #IMR]

	ldr r0, =EXTI
	ldr r1, [r0, #IMR]
	ldr r2, =EXTI_IMR_MR2
	orrs r2, r1
	str r2, [r0, #IMR]

	ldr r0, =EXTI
	ldr r1, [r0, #IMR]
	ldr r2, =EXTI_IMR_MR4
	orrs r2, r1
	str r2, [r0, #IMR]

	//2.4.5
	ldr  r2,=1<<5
    ldr  r0,=NVIC
    ldr  r1,=ISER
    str  r2,[r0,r1]

    ldr  r2,=1<<6
    ldr  r0,=NVIC
    ldr  r1,=ISER
    str  r2,[r0,r1]

    ldr  r2,=1<<7
    ldr  r0,=NVIC
    ldr  r1,=ISER
    str  r2,[r0,r1]

	// Student code goes above
	pop {pc}
//==========================================================
// enable_exti
// Enable the SYSCFG subsystem, and select Port B for
// pins 2, 3, and 4.
// Parameters: none
.global enable_exti
enable_exti:
	push {lr}
	// Student code goes below

	// Student code goes above
	pop  {pc}

//==========================================================
// set_col
// Set the specified column level to logic "high.
// Set the other three three columns to logic "low".


//==========================================================
// The current_col variable.
.data
.global current_col
current_col:
        .word 1
.text

//==========================================================
// SysTick_Handler
// The ISR for the SysTick interrupt.
//
.global SysTick_Handler
.type SysTick_Handler, %function
SysTick_Handler:
	push {lr}
	// Student code goes below

	// Student code goes above
	pop  {pc}

//==========================================================
// enable_systick
// Enable the SysTick interrupt to occur every 0.5 seconds.
// Parameters: none
.global enable_systick
enable_systick:
	push {lr}
	// Student code goes below

	// Student code goes above
	pop  {pc}


//==========================================================
// adjust_priorities
// Set the priority for EXTI pins 2-3 interrupt to 192.
// Set the priority for EXTI pins 4-15 interrupt to 128.
// Do not adjust the priority for any other interrupts.
.global adjust_priorities
adjust_priorities:
	push {lr}
	// Student code goes below

	// Student code goes above
	pop  {pc}

//==========================================================
// The main subroutine will call everything else.
// It will never return.
.global main
main:
	//bl autotest // Uncomment when most things are working
	bl initb
	bl initc
	bl init_exti
	bl init_systick
	bl adjust_priorities

endless_loop:
	ldr  r0,=GPIOC
	movs r1,#9
	bl   togglexn
	ldr  r0,=500000000
	bl   nano_wait
	b    endless_loop

