################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/planner/dispatcher.c \
../src/planner/ltp.c 

OBJS += \
./src/planner/dispatcher.o \
./src/planner/ltp.o 

C_DEPS += \
./src/planner/dispatcher.d \
./src/planner/ltp.d 


# Each subdirectory must supply rules for building sources it contributes
src/planner/%.o: ../src/planner/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


