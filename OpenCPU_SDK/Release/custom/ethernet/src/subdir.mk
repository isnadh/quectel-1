################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../custom/ethernet/src/Eth_spi.c \
../custom/ethernet/src/SysCalls.c \
../custom/ethernet/src/arpv4.c \
../custom/ethernet/src/bacnet_udp.c \
../custom/ethernet/src/dhcp_client.c \
../custom/ethernet/src/enc28J60.c \
../custom/ethernet/src/ethernet_init.c \
../custom/ethernet/src/icmp.c \
../custom/ethernet/src/ipv4.c \
../custom/ethernet/src/mac_address.c \
../custom/ethernet/src/network.c \
../custom/ethernet/src/read_multiple.c \
../custom/ethernet/src/syslog.c \
../custom/ethernet/src/time.c \
../custom/ethernet/src/udpv4.c 

OBJS += \
./custom/ethernet/src/Eth_spi.o \
./custom/ethernet/src/SysCalls.o \
./custom/ethernet/src/arpv4.o \
./custom/ethernet/src/bacnet_udp.o \
./custom/ethernet/src/dhcp_client.o \
./custom/ethernet/src/enc28J60.o \
./custom/ethernet/src/ethernet_init.o \
./custom/ethernet/src/icmp.o \
./custom/ethernet/src/ipv4.o \
./custom/ethernet/src/mac_address.o \
./custom/ethernet/src/network.o \
./custom/ethernet/src/read_multiple.o \
./custom/ethernet/src/syslog.o \
./custom/ethernet/src/time.o \
./custom/ethernet/src/udpv4.o 

C_DEPS += \
./custom/ethernet/src/Eth_spi.d \
./custom/ethernet/src/SysCalls.d \
./custom/ethernet/src/arpv4.d \
./custom/ethernet/src/bacnet_udp.d \
./custom/ethernet/src/dhcp_client.d \
./custom/ethernet/src/enc28J60.d \
./custom/ethernet/src/ethernet_init.d \
./custom/ethernet/src/icmp.d \
./custom/ethernet/src/ipv4.d \
./custom/ethernet/src/mac_address.d \
./custom/ethernet/src/network.d \
./custom/ethernet/src/read_multiple.d \
./custom/ethernet/src/syslog.d \
./custom/ethernet/src/time.d \
./custom/ethernet/src/udpv4.d 


# Each subdirectory must supply rules for building sources it contributes
custom/ethernet/src/%.o: ../custom/ethernet/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler (Sourcery Lite Bare)'
	arm-none-eabi-gcc -D__OCPU_COMPILER_GCC__ -D__MQTT_TEST__ -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\mqtt\inc" -I"${GCC_PATH}\arm-none-eabi\include" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\modbus\inc" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\ethernet\inc" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\bacnet\include" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\include" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\ril\inc" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\config" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\fota\inc" -Os -Wall -std=c99 -c -fmessage-length=0 -mlong-calls -Wstrict-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -march=armv5te -mthumb-interwork -mfloat-abi=soft -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


