################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/dbi/ntgs_mysql.c 

OBJS += \
./src/dbi/ntgs_mysql.o 

C_DEPS += \
./src/dbi/ntgs_mysql.d 


# Each subdirectory must supply rules for building sources it contributes
src/dbi/%.o: ../src/dbi/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/include/mysql -I/opt/libzdb/include/zdb -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


