################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/input/Keyboard.cpp \
../src/input/Mouse.cpp 

OBJS += \
./src/input/Keyboard.o \
./src/input/Mouse.o 

CPP_DEPS += \
./src/input/Keyboard.d \
./src/input/Mouse.d 


# Each subdirectory must supply rules for building sources it contributes
src/input/%.o: ../src/input/%.cpp src/input/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	gcc -std=c++17 -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/json/include" -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/libshaderc/include" -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/stb" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


