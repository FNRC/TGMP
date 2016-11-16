################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/utils/getlocalip.c \
../src/utils/gtring.c \
../src/utils/http.c \
../src/utils/scanroutine.c \
../src/utils/text.c \
../src/utils/threadpool.c \
../src/utils/url.c 

OBJS += \
./src/utils/getlocalip.o \
./src/utils/gtring.o \
./src/utils/http.o \
./src/utils/scanroutine.o \
./src/utils/text.o \
./src/utils/threadpool.o \
./src/utils/url.o 

C_DEPS += \
./src/utils/getlocalip.d \
./src/utils/gtring.d \
./src/utils/http.d \
./src/utils/scanroutine.d \
./src/utils/text.d \
./src/utils/threadpool.d \
./src/utils/url.d 


# Each subdirectory must supply rules for building sources it contributes
src/utils/%.o: ../src/utils/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include/mysql -I/opt/libzdb/include/zdb -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


