/*
 * RenderWindow.h
 *
 *  Created on: Jun 13, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_RENDERWINDOW_H_
#define GRAPHICS_RENDERWINDOW_H_

#include "../Base.h"
#include "../Time.h"
#include "Window.h"
#include "Context.h"
#include "../Events.h"

namespace matrix
{

namespace graphics
{

struct RenderWindowCallbacks
{
	Function<void(u32)> updateDescriptors = null;
	Function<void(const UpdateData&)> update = null;
	Function<void(Ref<CommandBuffer>, u32)> render = null;
	Function<Ref<ImageView2D>(u32)> getRenderResult = null;
};

struct RenderWindowSettings
{
	u32 width = 1600;
	u32 height = 900;
	String title = "Matrix application";
	WindowType type = WindowType::WINDOWED;

	RenderWindowCallbacks callbacks;
};

class RenderWindow
{
private:
	struct FrameData
	{
		Ref<CommandBuffer> commandBuffer;
		Ref<Fence> fence;
		bool pending = false;

		Ref<CommandBuffer> presentCommandBuffer;
		Ref<Fence> presentFence;
		bool presentPending = false;
	};

	Ref<Window> window;
	Ref<Context> context;
	Ref<Device> device;
	Ref<DeviceInstance> deviceInstance;
	Ref<WindowSurface> surface;
	Ref<Swapchain> swapchain;
	Array<Ref<ImageView2D>> swapchainImageViews;
	Array<FrameData> buffers;
	u64 frameIndex = 0;
public:
	RenderWindowCallbacks callbacks;

	RenderWindow(RenderWindowSettings settings);
	~RenderWindow();

	void run();

	u32 getBufferCount() const
	{
		return swapchain->getImages().size();
	}

	Ref<Window> getWindow() const
	{
		return window;
	}

	Ref<Context> getContext() const
	{
		return context;
	}

	Ref<Device> getDevice() const
	{
		return device;
	}

	Ref<DeviceInstance> getDeviceInstance() const
	{
		return deviceInstance;
	}

	Ref<WindowSurface> getSurface() const
	{
		return surface;
	}

	Ref<Swapchain> getSwapchain() const
	{
		return swapchain;
	}

	const Array<Ref<ImageView2D>>& getSwapchainImageViews() const
	{
		return swapchainImageViews;
	}

	u32 getWidth() const
	{
		return window->getWidth();
	}

	u32 getHeight() const
	{
		return window->getHeight();
	}

	bool isCloseRequested() const
	{
		return window->isCloseRequested();
	}
};

}

}

#endif /* GRAPHICS_RENDERWINDOW_H_ */
