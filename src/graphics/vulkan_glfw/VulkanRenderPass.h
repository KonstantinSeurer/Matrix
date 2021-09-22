/*
 * VulkanRenderpass.h
 *
 *  Created on: Jun 12, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_VULKAN_GLFW_VULKANRENDERPASS_H_
#define GRAPHICS_VULKAN_GLFW_VULKANRENDERPASS_H_

#include <vulkan/vulkan.h>
#include "../RenderPass.h"

namespace matrix {

namespace graphics {

class VulkanRenderPass: public RenderPass {
private:
	VkDevice device;
	VkRenderPass renderPass;
public:
	VulkanRenderPass(VkDevice device, const Array<Attachment> &attachments, const Array<Subpass> &subpasses);
	virtual ~VulkanRenderPass();

	VkRenderPass getRenderPass() const {
		return renderPass;
	}
};

}

}

#endif /* GRAPHICS_VULKAN_GLFW_VULKANRENDERPASS_H_ */
