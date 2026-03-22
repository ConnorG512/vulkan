#include <vulkan/rendering.hpp>

auto Vulkan::Rendering::create_rendering_attachment_info(
    const RenderingAttachmentSettings &renderingAttachmentSettings)
    -> VkRenderingAttachmentInfo {
  const VkRenderingAttachmentInfo result{
      .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
      .pNext = renderingAttachmentSettings.pNext,
      .imageView = renderingAttachmentSettings.imageView,
      .imageLayout = renderingAttachmentSettings.imageLayout,
      .resolveMode = renderingAttachmentSettings.resolveMode,
      .resolveImageView = renderingAttachmentSettings.resolveImageView,
      .resolveImageLayout = renderingAttachmentSettings.resolveImageLayout,
      .loadOp = renderingAttachmentSettings.loadOp,
      .storeOp = renderingAttachmentSettings.storeOp,
      .clearValue = renderingAttachmentSettings.clearValue,
  };

  return result;
}
