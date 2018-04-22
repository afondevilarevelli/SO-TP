################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../so-commons-library-master/src/commons/collections/dictionary.c \
../so-commons-library-master/src/commons/collections/list.c \
../so-commons-library-master/src/commons/collections/queue.c 

OBJS += \
./so-commons-library-master/src/commons/collections/dictionary.o \
./so-commons-library-master/src/commons/collections/list.o \
./so-commons-library-master/src/commons/collections/queue.o 

C_DEPS += \
./so-commons-library-master/src/commons/collections/dictionary.d \
./so-commons-library-master/src/commons/collections/list.d \
./so-commons-library-master/src/commons/collections/queue.d 


# Each subdirectory must supply rules for building sources it contributes
so-commons-library-master/src/commons/collections/%.o: ../so-commons-library-master/src/commons/collections/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


