.syntax unified
.cpu cortex-m0
.fpu softvfp
.thumb

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

.equ STK, 0xe000e010
.equ CSR, 0x0
.equ RVR, 0x4
.equ CVR, 0x8


.type SysTick_Handler, %function
.global SysTick_Handler
SysTick_Handler:
	push {lr}
	ldr  r0, =GPIOC // Load, into RO, base address of GPIOC
	ldr  r1, [r0, #ODR] // Load, into R1, value of GPIOC offset by MODER
	movs r3, #1
	lsls r2, r3, #6
	eors r2, r1 // Combine R2 into R1
	str  r2, [r0, #ODR]

	ldr r3, =STK
	ldr r0, =1499999 // 12M ticks = 1/4 second
	str r0, [r3, #RVR] // Set the reset value
	movs r0, #3
	str r0, [r3, #CSR]
	pop {pc}



.global main
main:

	ldr  r0, =RCC // Load, into RO, base address of RCC
	ldr  r1, [r0, #AHBENR] // Load, into R1, value of RCC offset by AHBENR
	ldr  r2, =GPIOCEN// Load, into R2, value to enable Port C
	orrs r1, r2 // Combine R2 into R1
	str  r1, [r0, #AHBENR] // Store new bits into RCC_AHBENR

	ldr  r0, =GPIOC // Load, into RO, base address of GPIOC
	ldr  r1, [r0, #MODER] // Load, into R1, value of GPIOC offset by MODER
	ldr  r2, =0x3000
	bics r1, r2 // Combine R2 into R1
	str  r1, [r0, #MODER]

	ldr  r0, =GPIOC // Load, into RO, base address of GPIOC
	ldr  r1, [r0, #PUPDR] // Load, into R1, value of GPIOC offset by MODER
	ldr  r2, =0x2000
	orrs r1, r2 // Combine R2 into R1
	str  r1, [r0, #IDR]

	ldr  r0, =GPIOC // Load, into RO, base address of GPIOC
	ldr  r1, [r0, #MODER] // Load, into R1, value of GPIOC offset by MODER
	ldr  r2, =0x1000
	orrs r1, r2 // Combine R2 into R1
	str  r1, [r0, #MODER]

	b SysTick_Handler






