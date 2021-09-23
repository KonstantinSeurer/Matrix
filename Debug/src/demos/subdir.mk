################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/demos/DemoCamera.cpp \
../src/demos/DemoSelector.cpp \
../src/demos/DoublePendulum.cpp \
../src/demos/Sky.cpp \
../src/demos/Sponza.cpp 

OBJS += \
./src/demos/DemoCamera.o \
./src/demos/DemoSelector.o \
./src/demos/DoublePendulum.o \
./src/demos/Sky.o \
./src/demos/Sponza.o 

CPP_DEPS += \
./src/demos/DemoCamera.d \
./src/demos/DemoSelector.d \
./src/demos/DoublePendulum.d \
./src/demos/Sky.d \
./src/demos/Sponza.d 


# Each subdirectory must supply rules for building sources it contributes
src/demos/%.o: ../src/demos/%.cpp src/demos/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	gcc -std=c++17 -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/json/include" -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/stb" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


