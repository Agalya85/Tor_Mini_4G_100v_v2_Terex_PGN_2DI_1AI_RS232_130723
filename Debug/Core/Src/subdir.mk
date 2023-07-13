################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/UserEeprom.c \
../Core/Src/adc.c \
../Core/Src/can.c \
../Core/Src/deviceinfo.c \
../Core/Src/dma.c \
../Core/Src/error_handling.c \
../Core/Src/gpio.c \
../Core/Src/gsmEC200U.c \
../Core/Src/i2c.c \
../Core/Src/iwdg.c \
../Core/Src/main.c \
../Core/Src/payload.c \
../Core/Src/queue.c \
../Core/Src/rtc.c \
../Core/Src/serial_comm.c \
../Core/Src/serial_flash_w25_fsm.c \
../Core/Src/spi.c \
../Core/Src/stm32l4xx_hal_msp.c \
../Core/Src/stm32l4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_reset.c \
../Core/Src/system_stm32l4xx.c \
../Core/Src/tim.c \
../Core/Src/usart.c \
../Core/Src/user_Immobilization.c \
../Core/Src/user_MqttSubSperator.c \
../Core/Src/user_RS232.c \
../Core/Src/user_adc.c \
../Core/Src/user_can.c \
../Core/Src/user_digital_input.c \
../Core/Src/user_flash.c \
../Core/Src/user_rtc.c \
../Core/Src/user_timer.c 

OBJS += \
./Core/Src/UserEeprom.o \
./Core/Src/adc.o \
./Core/Src/can.o \
./Core/Src/deviceinfo.o \
./Core/Src/dma.o \
./Core/Src/error_handling.o \
./Core/Src/gpio.o \
./Core/Src/gsmEC200U.o \
./Core/Src/i2c.o \
./Core/Src/iwdg.o \
./Core/Src/main.o \
./Core/Src/payload.o \
./Core/Src/queue.o \
./Core/Src/rtc.o \
./Core/Src/serial_comm.o \
./Core/Src/serial_flash_w25_fsm.o \
./Core/Src/spi.o \
./Core/Src/stm32l4xx_hal_msp.o \
./Core/Src/stm32l4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_reset.o \
./Core/Src/system_stm32l4xx.o \
./Core/Src/tim.o \
./Core/Src/usart.o \
./Core/Src/user_Immobilization.o \
./Core/Src/user_MqttSubSperator.o \
./Core/Src/user_RS232.o \
./Core/Src/user_adc.o \
./Core/Src/user_can.o \
./Core/Src/user_digital_input.o \
./Core/Src/user_flash.o \
./Core/Src/user_rtc.o \
./Core/Src/user_timer.o 

C_DEPS += \
./Core/Src/UserEeprom.d \
./Core/Src/adc.d \
./Core/Src/can.d \
./Core/Src/deviceinfo.d \
./Core/Src/dma.d \
./Core/Src/error_handling.d \
./Core/Src/gpio.d \
./Core/Src/gsmEC200U.d \
./Core/Src/i2c.d \
./Core/Src/iwdg.d \
./Core/Src/main.d \
./Core/Src/payload.d \
./Core/Src/queue.d \
./Core/Src/rtc.d \
./Core/Src/serial_comm.d \
./Core/Src/serial_flash_w25_fsm.d \
./Core/Src/spi.d \
./Core/Src/stm32l4xx_hal_msp.d \
./Core/Src/stm32l4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_reset.d \
./Core/Src/system_stm32l4xx.d \
./Core/Src/tim.d \
./Core/Src/usart.d \
./Core/Src/user_Immobilization.d \
./Core/Src/user_MqttSubSperator.d \
./Core/Src/user_RS232.d \
./Core/Src/user_adc.d \
./Core/Src/user_can.d \
./Core/Src/user_digital_input.d \
./Core/Src/user_flash.d \
./Core/Src/user_rtc.d \
./Core/Src/user_timer.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32L433xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/UserEeprom.d ./Core/Src/UserEeprom.o ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/can.d ./Core/Src/can.o ./Core/Src/deviceinfo.d ./Core/Src/deviceinfo.o ./Core/Src/dma.d ./Core/Src/dma.o ./Core/Src/error_handling.d ./Core/Src/error_handling.o ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gsmEC200U.d ./Core/Src/gsmEC200U.o ./Core/Src/i2c.d ./Core/Src/i2c.o ./Core/Src/iwdg.d ./Core/Src/iwdg.o ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/payload.d ./Core/Src/payload.o ./Core/Src/queue.d ./Core/Src/queue.o ./Core/Src/rtc.d ./Core/Src/rtc.o ./Core/Src/serial_comm.d ./Core/Src/serial_comm.o ./Core/Src/serial_flash_w25_fsm.d ./Core/Src/serial_flash_w25_fsm.o ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/stm32l4xx_hal_msp.d ./Core/Src/stm32l4xx_hal_msp.o ./Core/Src/stm32l4xx_it.d ./Core/Src/stm32l4xx_it.o ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/system_reset.d ./Core/Src/system_reset.o ./Core/Src/system_stm32l4xx.d ./Core/Src/system_stm32l4xx.o ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/user_Immobilization.d ./Core/Src/user_Immobilization.o ./Core/Src/user_MqttSubSperator.d ./Core/Src/user_MqttSubSperator.o ./Core/Src/user_RS232.d ./Core/Src/user_RS232.o ./Core/Src/user_adc.d ./Core/Src/user_adc.o ./Core/Src/user_can.d ./Core/Src/user_can.o ./Core/Src/user_digital_input.d ./Core/Src/user_digital_input.o ./Core/Src/user_flash.d ./Core/Src/user_flash.o ./Core/Src/user_rtc.d ./Core/Src/user_rtc.o ./Core/Src/user_timer.d ./Core/Src/user_timer.o

.PHONY: clean-Core-2f-Src

