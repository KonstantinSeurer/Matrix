################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/graphics/vulkan_glfw/GLFWVulkanWindow.cpp \
../src/graphics/vulkan_glfw/VulkanBuffer.cpp \
../src/graphics/vulkan_glfw/VulkanCommandBuffer.cpp \
../src/graphics/vulkan_glfw/VulkanContext.cpp \
../src/graphics/vulkan_glfw/VulkanDescriptorSet.cpp \
../src/graphics/vulkan_glfw/VulkanDescriptorSetLayout.cpp \
../src/graphics/vulkan_glfw/VulkanDevice.cpp \
../src/graphics/vulkan_glfw/VulkanFence.cpp \
../src/graphics/vulkan_glfw/VulkanImage.cpp \
../src/graphics/vulkan_glfw/VulkanMemoryAllocator.cpp \
../src/graphics/vulkan_glfw/VulkanPipeline.cpp \
../src/graphics/vulkan_glfw/VulkanRenderPass.cpp \
../src/graphics/vulkan_glfw/VulkanSemaphoreChain.cpp \
../src/graphics/vulkan_glfw/VulkanSwapchain.cpp \
../src/graphics/vulkan_glfw/VulkanUtil.cpp \
../src/graphics/vulkan_glfw/VulkanframeBuffer.cpp 

OBJS += \
./src/graphics/vulkan_glfw/GLFWVulkanWindow.o \
./src/graphics/vulkan_glfw/VulkanBuffer.o \
./src/graphics/vulkan_glfw/VulkanCommandBuffer.o \
./src/graphics/vulkan_glfw/VulkanContext.o \
./src/graphics/vulkan_glfw/VulkanDescriptorSet.o \
./src/graphics/vulkan_glfw/VulkanDescriptorSetLayout.o \
./src/graphics/vulkan_glfw/VulkanDevice.o \
./src/graphics/vulkan_glfw/VulkanFence.o \
./src/graphics/vulkan_glfw/VulkanImage.o \
./src/graphics/vulkan_glfw/VulkanMemoryAllocator.o \
./src/graphics/vulkan_glfw/VulkanPipeline.o \
./src/graphics/vulkan_glfw/VulkanRenderPass.o \
./src/graphics/vulkan_glfw/VulkanSemaphoreChain.o \
./src/graphics/vulkan_glfw/VulkanSwapchain.o \
./src/graphics/vulkan_glfw/VulkanUtil.o \
./src/graphics/vulkan_glfw/VulkanframeBuffer.o 

CPP_DEPS += \
./src/graphics/vulkan_glfw/GLFWVulkanWindow.d \
./src/graphics/vulkan_glfw/VulkanBuffer.d \
./src/graphics/vulkan_glfw/VulkanCommandBuffer.d \
./src/graphics/vulkan_glfw/VulkanContext.d \
./src/graphics/vulkan_glfw/VulkanDescriptorSet.d \
./src/graphics/vulkan_glfw/VulkanDescriptorSetLayout.d \
./src/graphics/vulkan_glfw/VulkanDevice.d \
./src/graphics/vulkan_glfw/VulkanFence.d \
./src/graphics/vulkan_glfw/VulkanImage.d \
./src/graphics/vulkan_glfw/VulkanMemoryAllocator.d \
./src/graphics/vulkan_glfw/VulkanPipeline.d \
./src/graphics/vulkan_glfw/VulkanRenderPass.d \
./src/graphics/vulkan_glfw/VulkanSemaphoreChain.d \
./src/graphics/vulkan_glfw/VulkanSwapchain.d \
./src/graphics/vulkan_glfw/VulkanUtil.d \
./src/graphics/vulkan_glfw/VulkanframeBuffer.d 


# Each subdirectory must supply rules for building sources it contributes
src/graphics/vulkan_glfw/%.o: ../src/graphics/vulkan_glfw/%.cpp src/graphics/vulkan_glfw/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -std=c++17 -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/json/include" -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/shaderc/libshaderc/include" -I"/home/konstantin/Desktop/matrix/Matrix/dependencies/stb" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


