/*
 * RenderWindow.cpp
 *
 *  Created on: Jun 13, 2021
 *      Author: konstantin
 */

#include "RenderWindow.h"
#include "../Logging.h"

namespace matrix
{

namespace graphics
{

RenderWindow::RenderWindow(RenderWindowSettings settings) :
callbacks(settings.callbacks)
{
	window = createWindow(settings.width, settings.height, settings.title, settings.type);

	context = createContext();
	auto devices = context->getAvailableDevices();
	device = getBestDevice(devices);

	out("device: " + device->getName());

	deviceInstance = device->createInstance();

	surface = context->createSurface(window);

	swapchain = deviceInstance->createSwapchain(surface);

	for (auto image : swapchain->getImages())
	{
		swapchainImageViews.push_back(deviceInstance->createImageView2D(image, 0, 1));
	}

	for (u32 i = 0; i < getBufferCount(); i++)
	{
		FrameData data;

		data.commandBuffer = deviceInstance->createCommandBuffer();
		data.fence = deviceInstance->createFence();

		data.presentCommandBuffer = deviceInstance->createCommandBuffer();
		data.presentFence = deviceInstance->createFence();

		buffers.push_back(data);
	}
}

RenderWindow::~RenderWindow()
{
}

void RenderWindow::run()
{
	Time lastTime;
	u32 frameCount = 0;
	Time deltaLastTime;
	Time startTime;
	input::MouseState prevMouseState;

	while (!window->isCloseRequested())
	{
		if (frameIndex > 0)
		{
			u32 imageIndex = swapchain->pollImage();

			u64 prevFrameIndex = frameIndex - 1;
			u32 prevBufferIndex = prevFrameIndex % getBufferCount();
			FrameData &prevFrame = buffers[prevBufferIndex];

			deviceInstance->submit(prevFrame.commandBuffer, prevFrame.fence);
			prevFrame.pending = true;

			if (prevFrame.presentPending)
			{
				deviceInstance->wait( { prevFrame.presentFence });
				prevFrame.presentPending = false;
			}

			prevFrame.presentCommandBuffer->record([&]()
			{
				auto image = swapchain->getImages()[imageIndex];

				prevFrame.presentCommandBuffer->barrier(
				{	ImageLayoutTransition(image, ImageLayout::UNDEFINED, ImageLayout::TRANSFER_DESTINATION)});
				prevFrame.presentCommandBuffer->blitImage2D(callbacks.getRenderResult(prevBufferIndex), ImageLayout::TRANSFER_SOURCE,
				swapchainImageViews[imageIndex], ImageLayout::TRANSFER_DESTINATION, SamplingMode::NEAREST);
				prevFrame.presentCommandBuffer->barrier(
				{	ImageLayoutTransition(image, ImageLayout::TRANSFER_DESTINATION, ImageLayout::PRESENT)});
			});

			deviceInstance->submit(prevFrame.presentCommandBuffer, prevFrame.presentFence);
			prevFrame.presentPending = true;
		}

		Time now;
		if ((now - deltaLastTime).milliSeconds() > 1.0f)
		{
			input::MouseState mouse = window->getMouseState();
			if (frameIndex == 0)
			{
				prevMouseState = mouse;
			}

			UpdateData data;
			data.time = now - startTime;
			data.delta = now - deltaLastTime;
			data.mouse.position = mouse.position;
			data.mouse.pressedButtons = mouse.pressedButtons;
			data.mouse.movement = mouse.position - prevMouseState.position;
			data.keyboard = window->getKeyboardState();

			callbacks.update(data);

			deltaLastTime = now;
			prevMouseState = mouse;
		}

		u32 bufferIndex = frameIndex % getBufferCount();
		FrameData &currentFrame = buffers[bufferIndex];

		if (currentFrame.pending)
		{
			deviceInstance->wait( { currentFrame.fence });
			currentFrame.pending = false;
		}

		callbacks.updateDescriptors(bufferIndex);

		currentFrame.commandBuffer->record([&]()
		{
			callbacks.render(currentFrame.commandBuffer, bufferIndex);
		});

		if (frameIndex > 0)
		{
			swapchain->present();
			window->update();
		}

		frameIndex++;

		frameCount++;

		if ((Time() - lastTime).seconds() > 1.0f)
		{
			out(toString(frameCount) + "fps");

			lastTime = Time();
			frameCount = 0;
		}
	}

	deviceInstance->wait();
}

}

}
