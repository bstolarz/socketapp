################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/threads/cpu/handler.c \
../src/threads/cpu/select.c 

OBJS += \
./src/threads/cpu/handler.o \
./src/threads/cpu/select.o 

C_DEPS += \
./src/threads/cpu/handler.d \
./src/threads/cpu/select.d 


# Each subdirectory must supply rules for building sources it contributes
src/threads/cpu/%.o: ../src/threads/cpu/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


