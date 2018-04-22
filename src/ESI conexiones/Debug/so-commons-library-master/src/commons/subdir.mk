################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../so-commons-library-master/src/commons/bitarray.c \
../so-commons-library-master/src/commons/config.c \
../so-commons-library-master/src/commons/error.c \
../so-commons-library-master/src/commons/log.c \
../so-commons-library-master/src/commons/process.c \
../so-commons-library-master/src/commons/string.c \
../so-commons-library-master/src/commons/temporal.c \
../so-commons-library-master/src/commons/txt.c 

OBJS += \
./so-commons-library-master/src/commons/bitarray.o \
./so-commons-library-master/src/commons/config.o \
./so-commons-library-master/src/commons/error.o \
./so-commons-library-master/src/commons/log.o \
./so-commons-library-master/src/commons/process.o \
./so-commons-library-master/src/commons/string.o \
./so-commons-library-master/src/commons/temporal.o \
./so-commons-library-master/src/commons/txt.o 

C_DEPS += \
./so-commons-library-master/src/commons/bitarray.d \
./so-commons-library-master/src/commons/config.d \
./so-commons-library-master/src/commons/error.d \
./so-commons-library-master/src/commons/log.d \
./so-commons-library-master/src/commons/process.d \
./so-commons-library-master/src/commons/string.d \
./so-commons-library-master/src/commons/temporal.d \
./so-commons-library-master/src/commons/txt.d 


# Each subdirectory must supply rules for building sources it contributes
so-commons-library-master/src/commons/%.o: ../so-commons-library-master/src/commons/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


