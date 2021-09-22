################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/scene/renderer/DeferredRenderer.cpp \
../src/scene/renderer/ModelAccelerationStructure.cpp \
../src/scene/renderer/ModelModelBuffer.cpp \
../src/scene/renderer/ModelVertexArray.cpp \
../src/scene/renderer/PbrDeferredPathtracer.cpp \
../src/scene/renderer/PhysicalSkyRenderer.cpp \
../src/scene/renderer/SceneAccelerationStructure.cpp \
../src/scene/renderer/SceneCameraBuffer.cpp \
../src/scene/renderer/SceneLightBuffer.cpp \
../src/scene/renderer/SceneMaterialBuffer.cpp \
../src/scene/renderer/SceneModelBuffer.cpp \
../src/scene/renderer/SceneModelMap.cpp \
../src/scene/renderer/ShadelessForwardRenderer.cpp \
../src/scene/renderer/SkyRenderer.cpp 

OBJS += \
./src/scene/renderer/DeferredRenderer.o \
./src/scene/renderer/ModelAccelerationStructure.o \
./src/scene/renderer/ModelModelBuffer.o \
./src/scene/renderer/ModelVertexArray.o \
./src/scene/renderer/PbrDeferredPathtracer.o \
./src/scene/renderer/PhysicalSkyRenderer.o \
./src/scene/renderer/SceneAccelerationStructure.o \
./src/scene/renderer/SceneCameraBuffer.o \
./src/scene/renderer/SceneLightBuffer.o \
./src/scene/renderer/SceneMaterialBuffer.o \
./src/scene/renderer/SceneModelBuffer.o \
./src/scene/renderer/SceneModelMap.o \
./src/scene/renderer/ShadelessForwardRenderer.o \
./src/scene/renderer/SkyRenderer.o 

CPP_DEPS += \
./src/scene/renderer/DeferredRenderer.d \
./src/scene/renderer/ModelAccelerationStructure.d \
./src/scene/renderer/ModelModelBuffer.d \
./src/scene/renderer/ModelVertexArray.d \
./src/scene/renderer/PbrDeferredPathtracer.d \
./src/scene/renderer/PhysicalSkyRenderer.d \
./src/scene/renderer/SceneAccelerationStructure.d \
./src/scene/renderer/SceneCameraBuffer.d \
./src/scene/renderer/SceneLightBuffer.d \
./src/scene/renderer/SceneMaterialBuffer.d \
./src/scene/renderer/SceneModelBuffer.d \
./src/scene/renderer/SceneModelMap.d \
./src/scene/renderer/ShadelessForwardRenderer.d \
./src/scene/renderer/SkyRenderer.d 


# Each subdirectory must supply rules for building sources it contributes
src/scene/renderer/%.o: ../src/scene/renderer/%.cpp src/scene/renderer/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	gcc -std=c++17 -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/json/include" -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/libshaderc/include" -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/stb" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


