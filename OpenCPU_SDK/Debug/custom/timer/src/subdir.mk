################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../custom/timer/src/Timers.c 

OBJS += \
./custom/timer/src/Timers.o 

C_DEPS += \
./custom/timer/src/Timers.d 


# Each subdirectory must supply rules for building sources it contributes
custom/timer/src/%.o: ../custom/timer/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler (Sourcery Lite Bare)'
	arm-none-eabi-gcc -D__OCPU_COMPILER_GCC__ -D__MQTT_TEST__ -I"${GCC_PATH}\arm-none-eabi\include" -I"D:\Quectel\New\TEST\OpenCPU_SDK\custom\IPC\inc" -I"D:\Quectel\New\TEST\OpenCPU_SDK\custom\cJSON\inc" -I"D:\Quectel\New\TEST\OpenCPU_SDK\custom\extras\inc" -I"D:\Quectel\New\TEST\OpenCPU_SDK\custom\SecureSocket\inc" -I"D:\Quectel\New\TEST\OpenCPU_SDK\custom\timer\inc" -I"D:\Quectel\New\TEST\OpenCPU_SDK\include" -I"D:\Quectel\New\TEST\OpenCPU_SDK\custom\mqtt\inc" -I"D:\Quectel\New\TEST\OpenCPU_SDK\custom\TFTDisplay\inc" -I"D:\Quectel\New\TEST\OpenCPU_SDK\ril\inc" -I"D:\Quectel\New\TEST\OpenCPU_SDK\custom\config" -I"D:\Quectel\New\TEST\OpenCPU_SDK\custom\fota\inc" -I"D:\Quectel\New\TEST\OpenCPU_SDK\custom\sms\inc" -O2 -Wall -std=c99 -c -fmessage-length=0 -mlong-calls -Wstrict-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -march=armv5te -mthumb-interwork -mfloat-abi=soft -g3 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


