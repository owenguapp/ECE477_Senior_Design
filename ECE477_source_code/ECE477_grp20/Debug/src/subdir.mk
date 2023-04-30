################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/background.c \
../src/commands.c \
../src/crocodile.c \
../src/crocodile_left.c \
../src/fifo.c \
../src/frog.c \
../src/gameover.c \
../src/keypad.c \
../src/lcd.c \
../src/midi.c \
../src/syscalls.c \
../src/system_stm32f0xx.c \
../src/tty.c \
../src/winwin.c 

OBJS += \
./src/background.o \
./src/commands.o \
./src/crocodile.o \
./src/crocodile_left.o \
./src/fifo.o \
./src/frog.o \
./src/gameover.o \
./src/keypad.o \
./src/lcd.o \
./src/midi.o \
./src/syscalls.o \
./src/system_stm32f0xx.o \
./src/tty.o \
./src/winwin.o 

C_DEPS += \
./src/background.d \
./src/commands.d \
./src/crocodile.d \
./src/crocodile_left.d \
./src/fifo.d \
./src/frog.d \
./src/gameover.d \
./src/keypad.d \
./src/lcd.d \
./src/midi.d \
./src/syscalls.d \
./src/system_stm32f0xx.d \
./src/tty.d \
./src/winwin.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Compiler'
	@echo $(PWD)
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F091RCTx -DDEBUG -DSTM32F091 -DUSE_STDPERIPH_DRIVER -I"C:/Users/kansi/workspace/demo/StdPeriph_Driver/inc" -I"C:/Users/kansi/workspace/demo/inc" -I"C:/Users/kansi/workspace/demo/CMSIS/device" -I"C:/Users/kansi/workspace/demo/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

