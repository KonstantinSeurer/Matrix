/*
 * GLFWWindow.h
 *
 *  Created on: Jun 2, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_GLFWVULKANWINDOW_H_
#define GRAPHICS_VULKAN_GLFW_GLFWVULKANWINDOW_H_

#include "../Window.h"
#include "VulkanContext.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

namespace matrix {

namespace graphics {

class VulkanWindowSurface: public WindowSurface {
private:
	VkSurfaceKHR surface;
	const VulkanContext* context;
	GLFWwindow *window;
public:
	VulkanWindowSurface(const VulkanContext* context, GLFWwindow *window);

	virtual ~VulkanWindowSurface();

	VkSurfaceKHR getSurface() const {
		return surface;
	}

	GLFWwindow* getWindow() const {
		return window;
	}
};

class GLFWWindow: public Window {
private:
	GLFWwindow *window;
	String title;
public:
	input::KeyboardState keyboardState;
public:
	GLFWWindow(u32 width, u32 height, const String &title, WindowType type);
	virtual ~GLFWWindow();

	virtual u32 getWidth() const;
	virtual u32 getHeight() const;

	virtual void setWidth(u32 width);
	virtual void setHeight(u32 height);

	virtual const String& getTitile() const;
	virtual void setTitle(const String &title);

	virtual bool isCloseRequested() const;
	virtual void update();

	virtual input::MouseState getMouseState() const;
	virtual input::KeyboardState getKeyboardState() const;

	GLFWwindow *getWindow() const {
		return window;
	}
};

}

}

#endif /* GRAPHICS_VULKAN_GLFW_GLFWVULKANWINDOW_H_ */
