################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../custom/bacnet/src/handler/h_iam.c \
../custom/bacnet/src/handler/h_npdu.c \
../custom/bacnet/src/handler/h_rp_a.c \
../custom/bacnet/src/handler/h_rpm_a.c \
../custom/bacnet/src/handler/s_rp.c \
../custom/bacnet/src/handler/s_rpm.c \
../custom/bacnet/src/handler/s_whois.c \
../custom/bacnet/src/handler/s_wp.c \
../custom/bacnet/src/handler/txbuf.c 

OBJS += \
./custom/bacnet/src/handler/h_iam.o \
./custom/bacnet/src/handler/h_npdu.o \
./custom/bacnet/src/handler/h_rp_a.o \
./custom/bacnet/src/handler/h_rpm_a.o \
./custom/bacnet/src/handler/s_rp.o \
./custom/bacnet/src/handler/s_rpm.o \
./custom/bacnet/src/handler/s_whois.o \
./custom/bacnet/src/handler/s_wp.o \
./custom/bacnet/src/handler/txbuf.o 

C_DEPS += \
./custom/bacnet/src/handler/h_iam.d \
./custom/bacnet/src/handler/h_npdu.d \
./custom/bacnet/src/handler/h_rp_a.d \
./custom/bacnet/src/handler/h_rpm_a.d \
./custom/bacnet/src/handler/s_rp.d \
./custom/bacnet/src/handler/s_rpm.d \
./custom/bacnet/src/handler/s_whois.d \
./custom/bacnet/src/handler/s_wp.d \
./custom/bacnet/src/handler/txbuf.d 


# Each subdirectory must supply rules for building sources it contributes
custom/bacnet/src/handler/%.o: ../custom/bacnet/src/handler/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler (Sourcery Lite Bare)'
	arm-none-eabi-gcc -D__OCPU_COMPILER_GCC__ -D__MQTT_TEST__ -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\mqtt\inc" -I"${GCC_PATH}\arm-none-eabi\include" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\modbus\inc" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\ethernet\inc" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\bacnet\include" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\include" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\ril\inc" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\config" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\fota\inc" -Os -Wall -std=c99 -c -fmessage-length=0 -mlong-calls -Wstrict-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -march=armv5te -mthumb-interwork -mfloat-abi=soft -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


