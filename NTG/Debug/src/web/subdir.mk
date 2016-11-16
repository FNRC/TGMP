################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/web/web.c 

OBJS += \
./src/web/web.o 

C_DEPS += \
./src/web/web.d 


# Each subdirectory must supply rules for building sources it contributes
src/web/%.o: ../src/web/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include/mysql -I/opt/libzdb/include/zdb -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


