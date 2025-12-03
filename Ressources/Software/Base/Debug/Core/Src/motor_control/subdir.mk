################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/motor_control/asserv.c \
../Core/Src/motor_control/motor.c 

OBJS += \
./Core/Src/motor_control/asserv.o \
./Core/Src/motor_control/motor.o 

C_DEPS += \
./Core/Src/motor_control/asserv.d \
./Core/Src/motor_control/motor.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/motor_control/%.o Core/Src/motor_control/%.su Core/Src/motor_control/%.cyclo: ../Core/Src/motor_control/%.c Core/Src/motor_control/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32G474xx -c -I../Core/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc -I../Drivers/STM32G4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32G4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-motor_control

clean-Core-2f-Src-2f-motor_control:
	-$(RM) ./Core/Src/motor_control/asserv.cyclo ./Core/Src/motor_control/asserv.d ./Core/Src/motor_control/asserv.o ./Core/Src/motor_control/asserv.su ./Core/Src/motor_control/motor.cyclo ./Core/Src/motor_control/motor.d ./Core/Src/motor_control/motor.o ./Core/Src/motor_control/motor.su

.PHONY: clean-Core-2f-Src-2f-motor_control

