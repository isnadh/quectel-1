################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
null./custom/sms/inc/lib_sms.d 


# Each subdirectory must supply rules for building sources it contributes
custom/sms/inc/lib_sms: ../custom/sms/inc/lib_sms.h
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler (Sourcery Lite Bare)'
	arm-none-eabi-gcc -D__OCPU_COMPILER_GCC__ -D__MQTT_TEST__ -I"${GCC_PATH}\arm-none-eabi\include" -I"D:\TEST\OpenCPU_SDK\include" -I"D:\TEST\OpenCPU_SDK\ril\inc" -I"D:\TEST\OpenCPU_SDK\custom\cJSON\inc" -I"D:\TEST\OpenCPU_SDK\custom\config" -I"D:\TEST\OpenCPU_SDK\custom\extras\inc" -I"D:\TEST\OpenCPU_SDK\custom\fota\inc" -I"D:\TEST\OpenCPU_SDK\custom\IPC\inc" -I"D:\TEST\OpenCPU_SDK\custom\mqtt\inc" -I"D:\TEST\OpenCPU_SDK\custom\SecureSocket\inc" -I"D:\TEST\OpenCPU_SDK\custom\TFTDisplay\inc" -I"D:\TEST\OpenCPU_SDK\custom\timer\inc" -I"D:\TEST\OpenCPU_SDK\custom\sms\inc" -Os -Wall -std=c99 -c -fmessage-length=0 -mlong-calls -Wstrict-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"custom/sms/inc/lib_sms.d" -march=armv5te -mthumb-interwork -mfloat-abi=soft -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


