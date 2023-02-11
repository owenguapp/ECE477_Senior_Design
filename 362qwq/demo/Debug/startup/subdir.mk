################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../startup/startup_stm32.s 

OBJS += \
./startup/startup_stm32.o 


# Each subdirectory must supply rules for building sources it contributes
startup/%.o: ../startup/%.s
	@echo 'Building file: $<'
	@echo 'Invoking: MCU GCC Assembler'
	@echo $(PWD)
	arm-none-eabi-as -mcpu=cortex-m0 -mthumb -mfloat-abi=soft -I"/Users/owenggg/Desktop/362qwq/demo/StdPeriph_Driver/inc" -I"/Users/owenggg/Desktop/362qwq/demo/inc" -I"/Users/owenggg/Desktop/362qwq/demo/CMSIS/device" -I"/Users/owenggg/Desktop/362qwq/demo/CMSIS/core" -g -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


