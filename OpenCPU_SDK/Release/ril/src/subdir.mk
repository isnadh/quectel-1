################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ril/src/ril_alarm.c \
../ril/src/ril_atResponse.c \
../ril/src/ril_audio.c \
../ril/src/ril_bluetooth.c \
../ril/src/ril_custom.c \
../ril/src/ril_dtmf.c \
../ril/src/ril_ftp.c \
../ril/src/ril_http.c \
../ril/src/ril_init.c \
../ril/src/ril_location.c \
../ril/src/ril_network.c \
../ril/src/ril_sim.c \
../ril/src/ril_system.c \
../ril/src/ril_telephony.c \
../ril/src/ril_urc.c \
../ril/src/ril_util.c 

OBJS += \
./ril/src/ril_alarm.o \
./ril/src/ril_atResponse.o \
./ril/src/ril_audio.o \
./ril/src/ril_bluetooth.o \
./ril/src/ril_custom.o \
./ril/src/ril_dtmf.o \
./ril/src/ril_ftp.o \
./ril/src/ril_http.o \
./ril/src/ril_init.o \
./ril/src/ril_location.o \
./ril/src/ril_network.o \
./ril/src/ril_sim.o \
./ril/src/ril_system.o \
./ril/src/ril_telephony.o \
./ril/src/ril_urc.o \
./ril/src/ril_util.o 

C_DEPS += \
./ril/src/ril_alarm.d \
./ril/src/ril_atResponse.d \
./ril/src/ril_audio.d \
./ril/src/ril_bluetooth.d \
./ril/src/ril_custom.d \
./ril/src/ril_dtmf.d \
./ril/src/ril_ftp.d \
./ril/src/ril_http.d \
./ril/src/ril_init.d \
./ril/src/ril_location.d \
./ril/src/ril_network.d \
./ril/src/ril_sim.d \
./ril/src/ril_system.d \
./ril/src/ril_telephony.d \
./ril/src/ril_urc.d \
./ril/src/ril_util.d 


# Each subdirectory must supply rules for building sources it contributes
ril/src/%.o: ../ril/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler (Sourcery Lite Bare)'
	arm-none-eabi-gcc -D__OCPU_COMPILER_GCC__ -D__MQTT_TEST__ -I"${GCC_PATH}\arm-none-eabi\include" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\include" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\ril\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\cJSON\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\config" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\extras\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\fota\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\IPC\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\mqtt\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\SecureSocket\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\TFTDisplay\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\timer\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\sms\inc" -Os -Wall -std=c99 -c -fmessage-length=0 -mlong-calls -Wstrict-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -march=armv5te -mthumb-interwork -mfloat-abi=soft -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


