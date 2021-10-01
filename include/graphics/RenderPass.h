/*
 * Renderpass.h
 *
 *  Created on: Jun 12, 2021
 *      Author: konstantin
 */

#ifndef GRAPHICS_RENDERPASS_H_
#define GRAPHICS_RENDERPASS_H_

#include "Image.h"
#include "Pipeline.h"

namespace matrix {

namespace graphics {

enum class LoadOperation {
	CLEAR, LOAD, DONT_CARE
};

enum class StoreOperation {
	STORE, DONT_CARE
};

enum class AttachmentType {
	COLOR, DEPTH, STENCIL, DEPTH_STENCIL
};

struct Attachment {
	Ref<const ImageFormat> format;
	u8 sampleCount;
	LoadOperation loadOperation;
	StoreOperation storeOperation;
	ImageLayout beginLayout;
	ImageLayout endLayout;
};

struct AttachmentReference {
	u32 index;
	ImageLayout layout;
	AttachmentType type;

	AttachmentReference(u32 index, ImageLayout layout, AttachmentType type) :
			index(index), layout(layout), type(type) {
	}
};

struct Subpass {
	PipelineType pipelineType;
	Array<AttachmentReference> colorAttachments;
	Optional<AttachmentReference> depthStencilAttachment;
};

class RenderPass {
protected:
	Array<Attachment> attachments;
	Array<Subpass> subpasses;
public:
	RenderPass() {
	}

	virtual ~RenderPass() {
	}

	const Array<Attachment>& getAttachments() const {
		return attachments;
	}

	const Array<Subpass>& getSubpasses() const {
		return subpasses;
	}
};

}

}

#endif /* GRAPHICS_RENDERPASS_H_ */
