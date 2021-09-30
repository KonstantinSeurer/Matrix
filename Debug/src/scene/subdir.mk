################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/scene/Camera.cpp \
../src/scene/GLTF.cpp \
../src/scene/Material.cpp \
../src/scene/Model.cpp \
../src/scene/Scene.cpp \
../src/scene/Transform.cpp \
../src/scene/Volume.cpp 

OBJS += \
./src/scene/Camera.o \
./src/scene/GLTF.o \
./src/scene/Material.o \
./src/scene/Model.o \
./src/scene/Scene.o \
./src/scene/Transform.o \
./src/scene/Volume.o 

CPP_DEPS += \
./src/scene/Camera.d \
./src/scene/GLTF.d \
./src/scene/Material.d \
./src/scene/Model.d \
./src/scene/Scene.d \
./src/scene/Transform.d \
./src/scene/Volume.d 


# Each subdirectory must supply rules for building sources it contributes
src/scene/%.o: ../src/scene/%.cpp src/scene/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	gcc -std=c++17 -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/json/include" -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/stb" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


