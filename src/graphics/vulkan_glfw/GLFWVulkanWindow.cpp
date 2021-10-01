/*
 * Window.cpp
 *
 *  Created on: Jun 2, 2021
 *      Author: konstantin
 */

#include "GLFWVulkanWindow.h"
#include "VulkanUtil.h"
#include "VulkanSwapchain.h"
#include <Logging.h>

namespace matrix
{

	namespace graphics
	{

		VulkanWindowSurface::VulkanWindowSurface(const VulkanContext *context, GLFWwindow *window) : context(context), window(window)
		{
			assertVkResult(glfwCreateWindowSurface(context->getInstance(), window, null, &surface), SOURCE_LOCATION());
		}

		VulkanWindowSurface::~VulkanWindowSurface()
		{
			vkDestroySurfaceKHR(context->getInstance(), surface, null);
		}

		static u32 windowCount = 0;

		static UnorderedMap<int, input::Key> glfwKeys = {{GLFW_KEY_ESCAPE, input::Key::ESCAPE}, {GLFW_KEY_F1, input::Key::F1}, {GLFW_KEY_F2, input::Key::F2}, {GLFW_KEY_F3, input::Key::F3}, {GLFW_KEY_F4, input::Key::F4}, {GLFW_KEY_F5, input::Key::F5}, {GLFW_KEY_F6, input::Key::F6}, {GLFW_KEY_F7, input::Key::F7}, {GLFW_KEY_F8, input::Key::F8}, {GLFW_KEY_F9, input::Key::F9}, {GLFW_KEY_F10, input::Key::F10}, {GLFW_KEY_F11, input::Key::F11}, {GLFW_KEY_F12, input::Key::F12}, {GLFW_KEY_PRINT_SCREEN, input::Key::PRINT}, {GLFW_KEY_TAB, input::Key::TAB}, {GLFW_KEY_Q, input::Key::Q}, {GLFW_KEY_W, input::Key::W}, {GLFW_KEY_E, input::Key::E}, {GLFW_KEY_R, input::Key::R}, {GLFW_KEY_T, input::Key::T}, {GLFW_KEY_Z, input::Key::Z}, {GLFW_KEY_U, input::Key::U}, {GLFW_KEY_I, input::Key::I}, {GLFW_KEY_O, input::Key::O}, {GLFW_KEY_P, input::Key::P}, {GLFW_KEY_CAPS_LOCK, input::Key::CAPS}, {GLFW_KEY_A, input::Key::A}, {GLFW_KEY_S, input::Key::S}, {GLFW_KEY_D, input::Key::D}, {GLFW_KEY_F, input::Key::F}, {GLFW_KEY_G, input::Key::G}, {GLFW_KEY_H, input::Key::H}, {GLFW_KEY_J, input::Key::J}, {GLFW_KEY_K, input::Key::K}, {GLFW_KEY_L, input::Key::L}, {GLFW_KEY_LEFT_SHIFT, input::Key::LEFT_SHIFT}, {GLFW_KEY_Y, input::Key::Y}, {GLFW_KEY_X, input::Key::X}, {GLFW_KEY_C, input::Key::C}, {GLFW_KEY_V, input::Key::V}, {GLFW_KEY_B, input::Key::B}, {GLFW_KEY_N, input::Key::N}, {GLFW_KEY_M, input::Key::M}, {GLFW_KEY_COMMA, input::Key::COLON}, {GLFW_KEY_PERIOD, input::Key::DOT}, {GLFW_KEY_MINUS, input::Key::MINUS}, {GLFW_KEY_RIGHT_SHIFT, input::Key::RIGHT_SHIFT}, {GLFW_KEY_LEFT_CONTROL, input::Key::LEFT_CONTROL}, {GLFW_KEY_LEFT_SUPER, input::Key::HOME}, {GLFW_KEY_LEFT_ALT, input::Key::LEFT_ALT}, {GLFW_KEY_SPACE, input::Key::SPACE}, {GLFW_KEY_RIGHT_ALT, input::Key::RIGHT_ALT}, {GLFW_KEY_RIGHT_CONTROL, input::Key::RIGHT_CONTROL}, {GLFW_KEY_LEFT, input::Key::LEFT}, {GLFW_KEY_RIGHT, input::Key::RIGHT}, {GLFW_KEY_UP, input::Key::UP}, {GLFW_KEY_DOWN, input::Key::DOWN}, {GLFW_KEY_BACKSPACE, input::Key::BACKSPACE}, {GLFW_KEY_ENTER, input::Key::ENTER}, {GLFW_KEY_DELETE, input::Key::DELETE}, {GLFW_KEY_1, input::Key::D1}, {GLFW_KEY_2, input::Key::D2}, {GLFW_KEY_3, input::Key::D3}, {GLFW_KEY_4, input::Key::D4}, {GLFW_KEY_5, input::Key::D5}, {GLFW_KEY_6, input::Key::D6}, {GLFW_KEY_7, input::Key::D7}, {GLFW_KEY_8, input::Key::D8}, {GLFW_KEY_9, input::Key::D9}, {GLFW_KEY_0, input::Key::D0}, {GLFW_KEY_KP_1, input::Key::N1}, {GLFW_KEY_KP_2, input::Key::N2}, {GLFW_KEY_KP_3, input::Key::N3}, {GLFW_KEY_KP_4, input::Key::N4}, {GLFW_KEY_KP_5, input::Key::N5}, {GLFW_KEY_KP_6, input::Key::N6}, {GLFW_KEY_KP_7, input::Key::N7}, {GLFW_KEY_KP_8, input::Key::N8}, {GLFW_KEY_KP_9, input::Key::N9}, {GLFW_KEY_KP_0, input::Key::N0}};

		static UnorderedMap<GLFWwindow *, GLFWWindow *> windows;

		void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
		{
			if (action == GLFW_REPEAT)
			{
				return;
			}

			if (glfwKeys.find(key) == glfwKeys.end())
			{
				err("Unsupported key(code=" + toString(key) + ")!");
				return;
			}

			windows[window]->keyboardState.setKeyPressed(glfwKeys.at(key), action == GLFW_PRESS);
		}

		GLFWWindow::GLFWWindow(u32 width, u32 height, const String &title, WindowType type)
		{
			if (windowCount == 0)
			{
				if (glfwInit() != GLFW_TRUE)
				{
					err("Could not initialize GLFW!");
					return;
				}
			}

			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			switch (type)
			{
			case WindowType::WINDOWED:
				glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
				break;
			case WindowType::RESIZABLE_WINDOWED:
				glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
				break;
			case WindowType::FULLSCREEN:
				glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
				break;
			}
			window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

			windows[window] = this;

			glfwSetKeyCallback(window, keyCallback);

			windowCount++;
		}

		GLFWWindow::~GLFWWindow()
		{
			windows.erase(window);
			glfwDestroyWindow(window);
			windowCount--;
			if (windowCount == 0)
			{
				glfwTerminate();
			}
		}

		u32 GLFWWindow::getWidth() const
		{
			s32 width;
			glfwGetWindowSize(window, &width, null);
			return (u32)width;
		}

		u32 GLFWWindow::getHeight() const
		{
			s32 height;
			glfwGetWindowSize(window, null, &height);
			return (u32)height;
		}

		void GLFWWindow::setWidth(u32 width)
		{
			glfwSetWindowSize(window, width, getHeight());
		}

		void GLFWWindow::setHeight(u32 height)
		{
			glfwSetWindowSize(window, getWidth(), height);
		}

		const String &GLFWWindow::getTitile() const
		{
			return this->title;
		}

		void GLFWWindow::setTitle(const String &title)
		{
			glfwSetWindowTitle(window, title.c_str());
			this->title = title;
		}

		bool GLFWWindow::isCloseRequested() const
		{
			return glfwWindowShouldClose(window) == GLFW_TRUE;
		}

		void GLFWWindow::update()
		{
			glfwPollEvents();
		}

		using namespace input;

		MouseState GLFWWindow::getMouseState() const
		{
			MouseState result;

			f64 xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			result.position = math::Vec2((f32)xpos, (f32)ypos);

			result.setButtonPressed(MouseButton::LEFT, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS);
			result.setButtonPressed(MouseButton::RIGHT, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS);
			result.setButtonPressed(MouseButton::CENTER, glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS);

			return result;
		}

		input::KeyboardState GLFWWindow::getKeyboardState() const
		{
			return keyboardState;
		}

		Ref<Window> createWindow(u32 width, u32 height, const String &title, WindowType type)
		{
			return allocate<GLFWWindow>(width, height, title, type);
		}

	}

}
