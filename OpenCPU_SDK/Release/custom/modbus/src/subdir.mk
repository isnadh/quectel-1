################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../custom/modbus/src/modbus_init.c 

OBJS += \
./custom/modbus/src/modbus_init.o 

C_DEPS += \
./custom/modbus/src/modbus_init.d 


# Each subdirectory must supply rules for building sources it contributes
custom/modbus/src/%.o: ../custom/modbus/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler (Sourcery Lite Bare)'
	arm-none-eabi-gcc -D__OCPU_COMPILER_GCC__ -D__MQTT_TEST__ -I"D:\OpenCPU\OpenCPU_SDK\custom\SecureSocket\inc" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\extras\inc" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\extras\inc" -I"D:\OpenCPU\OpenCPU_SDK\custom\extras\inc" -I"D:\OpenCPU\OpenCPU_SDK\custom\timer\inc" -I"D:\OpenCPU\OpenCPU_SDK\custom\cJSON\inc" -I"D:\OpenCPU\OpenCPU_SDK\custom\modbus\inc" -I"D:\OpenCPU\OpenCPU_SDK\custom\SD_Card\inc" -I"D:\OpenCPU\OpenCPU_SDK\custom\mqtt\inc" -I"${GCC_PATH}\arm-none-eabi\include" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\include" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\ril\inc" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\config" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\SecureSocket\inc" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\fota\inc" -Os -Wall -std=c99 -c -fmessage-length=0 -mlong-calls -Wstrict-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -march=armv5te -mthumb-interwork -mfloat-abi=soft -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


