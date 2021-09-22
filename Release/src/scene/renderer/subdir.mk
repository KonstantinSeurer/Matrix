################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/scene/renderer/MaterialBuffer.cpp \
../src/scene/renderer/ModelMap.cpp \
../src/scene/renderer/ShadelessForwardRenderer.cpp 

OBJS += \
./src/scene/renderer/MaterialBuffer.o \
./src/scene/renderer/ModelMap.o \
./src/scene/renderer/ShadelessForwardRenderer.o 

CPP_DEPS += \
./src/scene/renderer/MaterialBuffer.d \
./src/scene/renderer/ModelMap.d \
./src/scene/renderer/ShadelessForwardRenderer.d 


# Each subdirectory must supply rules for building sources it contributes
src/scene/renderer/%.o: ../src/scene/renderer/%.cpp src/scene/renderer/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++17 -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/json/include" -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/libshaderc/include" -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/stb" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


