################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../custom/bacnet/src/apdu.c \
../custom/bacnet/src/bacaddr.c \
../custom/bacnet/src/bacapp.c \
../custom/bacnet/src/bacdcode.c \
../custom/bacnet/src/bacdevobjpropref.c \
../custom/bacnet/src/bachand.c \
../custom/bacnet/src/bacint.c \
../custom/bacnet/src/bacpropstates.c \
../custom/bacnet/src/bacreal.c \
../custom/bacnet/src/bacstr.c \
../custom/bacnet/src/bactext.c \
../custom/bacnet/src/bigend.c \
../custom/bacnet/src/bip.c \
../custom/bacnet/src/iam.c \
../custom/bacnet/src/indtext.c \
../custom/bacnet/src/memcopy.c \
../custom/bacnet/src/npdu.c \
../custom/bacnet/src/proplist.c \
../custom/bacnet/src/rp.c \
../custom/bacnet/src/rpm.c \
../custom/bacnet/src/tsm.c \
../custom/bacnet/src/whois.c \
../custom/bacnet/src/wp.c \
../custom/bacnet/src/wpm.c 

OBJS += \
./custom/bacnet/src/apdu.o \
./custom/bacnet/src/bacaddr.o \
./custom/bacnet/src/bacapp.o \
./custom/bacnet/src/bacdcode.o \
./custom/bacnet/src/bacdevobjpropref.o \
./custom/bacnet/src/bachand.o \
./custom/bacnet/src/bacint.o \
./custom/bacnet/src/bacpropstates.o \
./custom/bacnet/src/bacreal.o \
./custom/bacnet/src/bacstr.o \
./custom/bacnet/src/bactext.o \
./custom/bacnet/src/bigend.o \
./custom/bacnet/src/bip.o \
./custom/bacnet/src/iam.o \
./custom/bacnet/src/indtext.o \
./custom/bacnet/src/memcopy.o \
./custom/bacnet/src/npdu.o \
./custom/bacnet/src/proplist.o \
./custom/bacnet/src/rp.o \
./custom/bacnet/src/rpm.o \
./custom/bacnet/src/tsm.o \
./custom/bacnet/src/whois.o \
./custom/bacnet/src/wp.o \
./custom/bacnet/src/wpm.o 

C_DEPS += \
./custom/bacnet/src/apdu.d \
./custom/bacnet/src/bacaddr.d \
./custom/bacnet/src/bacapp.d \
./custom/bacnet/src/bacdcode.d \
./custom/bacnet/src/bacdevobjpropref.d \
./custom/bacnet/src/bachand.d \
./custom/bacnet/src/bacint.d \
./custom/bacnet/src/bacpropstates.d \
./custom/bacnet/src/bacreal.d \
./custom/bacnet/src/bacstr.d \
./custom/bacnet/src/bactext.d \
./custom/bacnet/src/bigend.d \
./custom/bacnet/src/bip.d \
./custom/bacnet/src/iam.d \
./custom/bacnet/src/indtext.d \
./custom/bacnet/src/memcopy.d \
./custom/bacnet/src/npdu.d \
./custom/bacnet/src/proplist.d \
./custom/bacnet/src/rp.d \
./custom/bacnet/src/rpm.d \
./custom/bacnet/src/tsm.d \
./custom/bacnet/src/whois.d \
./custom/bacnet/src/wp.d \
./custom/bacnet/src/wpm.d 


# Each subdirectory must supply rules for building sources it contributes
custom/bacnet/src/%.o: ../custom/bacnet/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Windows GCC C Compiler (Sourcery Lite Bare)'
	arm-none-eabi-gcc -D__OCPU_COMPILER_GCC__ -D__MQTT_TEST__ -I"${GCC_PATH}\arm-none-eabi\include" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\include" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\modbus\inc" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\ethernet\inc" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\mqtt\inc" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\bacnet\include" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\ril\inc" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\config" -I"D:\OpenCPU\eclipse_workspace\OpenCPU_SDK\custom\fota\inc" -O2 -Wall -std=c99 -c -fmessage-length=0 -mlong-calls -Wstrict-prototypes -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -march=armv5te -mthumb-interwork -mfloat-abi=soft -g3 -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


