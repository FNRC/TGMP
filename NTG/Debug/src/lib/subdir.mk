################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/lib/error.c \
../src/lib/string.c \
../src/lib/tcp_connect.c \
../src/lib/wrapsock.c \
../src/lib/wrapunix.c 

OBJS += \
./src/lib/error.o \
./src/lib/string.o \
./src/lib/tcp_connect.o \
./src/lib/wrapsock.o \
./src/lib/wrapunix.o 

C_DEPS += \
./src/lib/error.d \
./src/lib/string.d \
./src/lib/tcp_connect.d \
./src/lib/wrapsock.d \
./src/lib/wrapunix.d 


# Each subdirectory must supply rules for building sources it contributes
src/lib/%.o: ../src/lib/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include/mysql -I/opt/libzdb/include/zdb -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


