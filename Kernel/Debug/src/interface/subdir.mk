################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/interface/filesystem.c \
../src/interface/memory.c 

OBJS += \
./src/interface/filesystem.o \
./src/interface/memory.o 

C_DEPS += \
./src/interface/filesystem.d \
./src/interface/memory.d 


# Each subdirectory must supply rules for building sources it contributes
src/interface/%.o: ../src/interface/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


