################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/math/random/XOrShiftRandomNumberGenerator.cpp 

OBJS += \
./src/math/random/XOrShiftRandomNumberGenerator.o 

CPP_DEPS += \
./src/math/random/XOrShiftRandomNumberGenerator.d 


# Each subdirectory must supply rules for building sources it contributes
src/math/random/%.o: ../src/math/random/%.cpp src/math/random/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++17 -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/json/include" -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/libshaderc/include" -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/stb" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


