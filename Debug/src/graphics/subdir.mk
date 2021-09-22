################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/graphics/Buffer.cpp \
../src/graphics/Device.cpp \
../src/graphics/Image.cpp \
../src/graphics/ImageLoader.cpp \
../src/graphics/Noise.cpp \
../src/graphics/Pipeline.cpp \
../src/graphics/RenderWindow.cpp 

OBJS += \
./src/graphics/Buffer.o \
./src/graphics/Device.o \
./src/graphics/Image.o \
./src/graphics/ImageLoader.o \
./src/graphics/Noise.o \
./src/graphics/Pipeline.o \
./src/graphics/RenderWindow.o 

CPP_DEPS += \
./src/graphics/Buffer.d \
./src/graphics/Device.d \
./src/graphics/Image.d \
./src/graphics/ImageLoader.d \
./src/graphics/Noise.d \
./src/graphics/Pipeline.d \
./src/graphics/RenderWindow.d 


# Each subdirectory must supply rules for building sources it contributes
src/graphics/%.o: ../src/graphics/%.cpp src/graphics/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	gcc -std=c++17 -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/json/include" -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/libshaderc/include" -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/stb" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


