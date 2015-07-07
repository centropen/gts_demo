################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/log/gtslogger.cpp 

OBJS += \
./src/log/gtslogger.o 

CPP_DEPS += \
./src/log/gtslogger.d 


# Each subdirectory must supply rules for building sources it contributes
src/log/%.o: ../src/log/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++  -DBOOST_LOG_DYN_LINK -std=c++0x -I/opt/boost_1_58_0 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


