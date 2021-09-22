/*
 * VulkanGraphicsContext.cpp
 *
 *  Created on: Jun 3, 2021
 *      Author: konstantin
 */

#include "VulkanUtil.h"

#include <cstring>
#include <GLFW/glfw3.h>

#include "VulkanContext.h"
#include "VulkanDevice.h"
#include "GLFWVulkanWindow.h"

#include "../../Logging.h"

namespace matrix {

namespace graphics {

static void checkLayerSupport(Array<const char*> &layers) {
	u32 count;
	assertVkResult(vkEnumerateInstanceLayerProperties(&count, null), SOURCE_LOCATION());

	Array<VkLayerProperties> properties(count);
	assertVkResult(vkEnumerateInstanceLayerProperties(&count, properties.data()), SOURCE_LOCATION());

	out("Supported layers:");
	for (auto layer : properties) {
		out("\t" + String(layer.layerName));
	}

	for (s32 i = layers.size() - 1; i >= 0; i--) {
		const char *layer = layers[i];

		bool found = false;
		for (u32 j = 0; j < count; j++) {
			if (strcmp(layer, properties[j].layerName) == 0) {
				found = true;
				break;
			}
		}

		if (!found) {
			err(String("Instance layer ") + layer + "is not supported!");
			layers.erase(layers.begin() + i);
			continue;
		}
	}
}

VulkanContext::VulkanContext() {
	Array<const char*> layers = { "VK_LAYER_KHRONOS_validation", "VK_LAYER_MESA_overlay" };
	checkLayerSupport(layers);

	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = null;
	appInfo.pEngineName = "Matrix";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.pApplicationName = "";
	appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_2;

	VkInstanceCreateInfo info;
	info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	info.pNext = null;
	info.flags = 0;
	info.pApplicationInfo = &appInfo;

	info.ppEnabledExtensionNames = glfwGetRequiredInstanceExtensions(&info.enabledExtensionCount);

	info.enabledLayerCount = layers.size();
	info.ppEnabledLayerNames = layers.data();

	assertVkResult(vkCreateInstance(&info, null, &instance), SOURCE_LOCATION());

	u32 count;
	assertVkResult(vkEnumeratePhysicalDevices(instance, &count, null), SOURCE_LOCATION());

	Array<VkPhysicalDevice> devices(count);
	assertVkResult(vkEnumeratePhysicalDevices(instance, &count, devices.data()), SOURCE_LOCATION());

	for (VkPhysicalDevice device : devices) {
		availableDevices.push_back(allocate<VulkanDevice>(device));
	}
}

VulkanContext::~VulkanContext() {

}

const Array<Ref<Device>>& VulkanContext::getAvailableDevices() {
	return availableDevices;
}

Ref<WindowSurface> VulkanContext::createSurface(Ref<Window> window) const {
	return allocate<VulkanWindowSurface>(this, ((GLFWWindow*) window.get())->getWindow());
}

Ref<Context> createContext() {
	return allocate<VulkanContext>();
}

}

}
