################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/functions/config.c \
../src/functions/console.c \
../src/functions/cpu.c \
../src/functions/file_descriptor.c \
../src/functions/heap.c \
../src/functions/log.c \
../src/functions/program.c \
../src/functions/serialization.c 

OBJS += \
./src/functions/config.o \
./src/functions/console.o \
./src/functions/cpu.o \
./src/functions/file_descriptor.o \
./src/functions/heap.o \
./src/functions/log.o \
./src/functions/program.o \
./src/functions/serialization.o 

C_DEPS += \
./src/functions/config.d \
./src/functions/console.d \
./src/functions/cpu.d \
./src/functions/file_descriptor.d \
./src/functions/heap.d \
./src/functions/log.d \
./src/functions/program.d \
./src/functions/serialization.d 


# Each subdirectory must supply rules for building sources it contributes
src/functions/%.o: ../src/functions/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


