################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../so-commons-library-master/tests/integration-tests/logger/main.c 

OBJS += \
./so-commons-library-master/tests/integration-tests/logger/main.o 

C_DEPS += \
./so-commons-library-master/tests/integration-tests/logger/main.d 


# Each subdirectory must supply rules for building sources it contributes
so-commons-library-master/tests/integration-tests/logger/%.o: ../so-commons-library-master/tests/integration-tests/logger/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


