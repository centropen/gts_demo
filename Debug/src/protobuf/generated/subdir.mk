################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/protobuf/generated/stockmarket.pb.cc 

OBJS += \
./src/protobuf/generated/stockmarket.pb.o 

CC_DEPS += \
./src/protobuf/generated/stockmarket.pb.d 


# Each subdirectory must supply rules for building sources it contributes
src/protobuf/generated/%.o: ../src/protobuf/generated/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


