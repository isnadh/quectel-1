################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../custom/mqtt/src/MQTTConnectClient.c \
../custom/mqtt/src/MQTTConnectServer.c \
../custom/mqtt/src/MQTTDeserializePublish.c \
../custom/mqtt/src/MQTTFormat.c \
../custom/mqtt/src/MQTTPacket.c \
../custom/mqtt/src/MQTTSerializePublish.c \
../custom/mqtt/src/MQTTSubscribeClient.c \
../custom/mqtt/src/MQTTSubscribeServer.c \
../custom/mqtt/src/MQTTUnsubscribeClient.c \
../custom/mqtt/src/MQTTUnsubscribeServer.c \
../custom/mqtt/src/mqtt.c 

OBJS += \
./custom/mqtt/src/MQTTConnectClient.o \
./custom/mqtt/src/MQTTConnectServer.o \
./custom/mqtt/src/MQTTDeserializePublish.o \
./custom/mqtt/src/MQTTFormat.o \
./custom/mqtt/src/MQTTPacket.o \
./custom/mqtt/src/MQTTSerializePublish.o \
./custom/mqtt/src/MQTTSubscribeClient.o \
./custom/mqtt/src/MQTTSubscribeServer.o \
./custom/mqtt/src/MQTTUnsubscribeClient.o \
./custom/mqtt/src/MQTTUnsubscribeServer.o \
./custom/mqtt/src/mqtt.o 

C_DEPS += \
./custom/mqtt/src/MQTTConnectClient.d \
./custom/mqtt/src/MQTTConnectServer.d \
./custom/mqtt/src/MQTTDeserializePublish.d \
./custom/mqtt/src/MQTTFormat.d \
./custom/mqtt/src/MQTTPacket.d \
./custom/mqtt/src/MQTTSerializePublish.d \
./custom/mqtt/src/MQTTSubscribeClient.d \
./custom/mqtt/src/MQTTSubscribeServer.d \
./custom/mqtt/src/MQTTUnsubscribeClient.d \
./custom/mqtt/src/MQTTUnsubscribeServer.d \
./custom/mqtt/src/mqtt.d 


# Each subdirectory must supply rules for building sources it contributes
custom/mqtt/src/%.o: ../custom/mqtt/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler (Sourcery Lite Bare)'
	arm-none-eabi-gcc -D__OCPU_COMPILER_GCC__ -D__MQTT_TEST__ -I"${GCC_PATH}\arm-none-eabi\include" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\include" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\ril\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\cJSON\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\config" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\extras\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\fota\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\IPC\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\mqtt\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\SecureSocket\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\TFTDisplay\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\timer\inc" -I"D:\Quectel_M66\TEST\OpenCPU_SDK\custom\sms\inc" -Os -Wall -std=c99 -c -fmessage-length=0 -mlong-calls -Wstrict-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -march=armv5te -mthumb-interwork -mfloat-abi=soft -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


