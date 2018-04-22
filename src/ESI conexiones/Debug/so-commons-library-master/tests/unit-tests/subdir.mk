################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../so-commons-library-master/tests/unit-tests/test_bitarray.c \
../so-commons-library-master/tests/unit-tests/test_config.c \
../so-commons-library-master/tests/unit-tests/test_dictionary.c \
../so-commons-library-master/tests/unit-tests/test_list.c \
../so-commons-library-master/tests/unit-tests/test_queue.c \
../so-commons-library-master/tests/unit-tests/test_string.c 

OBJS += \
./so-commons-library-master/tests/unit-tests/test_bitarray.o \
./so-commons-library-master/tests/unit-tests/test_config.o \
./so-commons-library-master/tests/unit-tests/test_dictionary.o \
./so-commons-library-master/tests/unit-tests/test_list.o \
./so-commons-library-master/tests/unit-tests/test_queue.o \
./so-commons-library-master/tests/unit-tests/test_string.o 

C_DEPS += \
./so-commons-library-master/tests/unit-tests/test_bitarray.d \
./so-commons-library-master/tests/unit-tests/test_config.d \
./so-commons-library-master/tests/unit-tests/test_dictionary.d \
./so-commons-library-master/tests/unit-tests/test_list.d \
./so-commons-library-master/tests/unit-tests/test_queue.d \
./so-commons-library-master/tests/unit-tests/test_string.d 


# Each subdirectory must supply rules for building sources it contributes
so-commons-library-master/tests/unit-tests/%.o: ../so-commons-library-master/tests/unit-tests/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


