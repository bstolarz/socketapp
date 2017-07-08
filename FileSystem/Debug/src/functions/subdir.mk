################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/functions/auxiliares.c \
../src/functions/bitmap.c \
../src/functions/config.c \
../src/functions/handler.c \
../src/functions/operaciones.c 

OBJS += \
./src/functions/auxiliares.o \
./src/functions/bitmap.o \
./src/functions/config.o \
./src/functions/handler.o \
./src/functions/operaciones.o 

C_DEPS += \
./src/functions/auxiliares.d \
./src/functions/bitmap.d \
./src/functions/config.d \
./src/functions/handler.d \
./src/functions/operaciones.d 


# Each subdirectory must supply rules for building sources it contributes
src/functions/%.o: ../src/functions/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


