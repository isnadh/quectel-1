################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../custom/mqtt/mqtt.c 

OBJS += \
./custom/mqtt/mqtt.o 

C_DEPS += \
./custom/mqtt/mqtt.d 


# Each subdirectory must supply rules for building sources it contributes
custom/mqtt/%.o: ../custom/mqtt/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler (Sourcery Lite Bare)'
	arm-none-eabi-gcc -D__OCPU_COMPILER_GCC__ -D__CUSTOMER_CODE__ -I"${GCC_PATH}\arm-none-eabi\include" -I"D:\eclips_open_cpu_workspace\M66_OpenCPU_GS3_SDK_V2.0_Eclipse\custom\mqtt\inc" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\include" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\ril\inc" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\config" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\fota\inc" -O2 -Wall -std=c99 -c -fmessage-length=0 -mlong-calls -Wstrict-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -march=armv5te -mthumb-interwork -mfloat-abi=soft -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


