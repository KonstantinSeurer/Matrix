################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/math/Base.cpp \
../src/math/Mat2.cpp \
../src/math/Mat3.cpp \
../src/math/Mat4.cpp \
../src/math/Quat.cpp \
../src/math/Vec2.cpp \
../src/math/Vec3.cpp \
../src/math/Vec4.cpp 

OBJS += \
./src/math/Base.o \
./src/math/Mat2.o \
./src/math/Mat3.o \
./src/math/Mat4.o \
./src/math/Quat.o \
./src/math/Vec2.o \
./src/math/Vec3.o \
./src/math/Vec4.o 

CPP_DEPS += \
./src/math/Base.d \
./src/math/Mat2.d \
./src/math/Mat3.d \
./src/math/Mat4.d \
./src/math/Quat.d \
./src/math/Vec2.d \
./src/math/Vec3.d \
./src/math/Vec4.d 


# Each subdirectory must supply rules for building sources it contributes
src/math/%.o: ../src/math/%.cpp src/math/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++17 -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/json/include" -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/libshaderc/include" -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/stb" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


