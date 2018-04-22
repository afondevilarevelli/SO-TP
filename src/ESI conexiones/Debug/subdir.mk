################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../a\ planificador\ y\ coordinador.c 

OBJS += \
./a\ planificador\ y\ coordinador.o 

C_DEPS += \
./a\ planificador\ y\ coordinador.d 


# Each subdirectory must supply rules for building sources it contributes
a\ planificador\ y\ coordinador.o: ../a\ planificador\ y\ coordinador.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"a planificador y coordinador.d" -MT"a\ planificador\ y\ coordinador.d" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


