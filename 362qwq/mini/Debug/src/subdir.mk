################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/background.c \
../src/commands.c \
../src/crocodile.c \
../src/crocodile_left.c \
../src/ff.c \
../src/fifo.c \
../src/frog.c \
../src/gameover.c \
../src/gator.c \
../src/invader.c \
../src/lcd.c \
../src/main3.c \
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
./src/ff.o \
./src/fifo.o \
./src/frog.o \
./src/gameover.o \
./src/gator.o \
./src/invader.o \
./src/lcd.o \
./src/main3.o \
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
./src/ff.d \
./src/fifo.d \
./src/frog.d \
./src/gameover.d \
./src/gator.d \
./src/invader.d \
./src/lcd.d \
./src/main3.d \
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
	arm-none-eabi-gcc -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -DSTM32 -DSTM32F0 -DSTM32F091RCTx -DDEBUG -DSTM32F091 -DUSE_STDPERIPH_DRIVER -I"C:/Users/Huihao Yin/workspace/mini/StdPeriph_Driver/inc" -I"C:/Users/Huihao Yin/workspace/mini/inc" -I"C:/Users/Huihao Yin/workspace/mini/CMSIS/device" -I"C:/Users/Huihao Yin/workspace/mini/CMSIS/core" -O0 -g3 -Wall -fmessage-length=0 -ffunction-sections -c -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


