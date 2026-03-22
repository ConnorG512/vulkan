#pragma once 

#include <vulkan/vulkan_core.h>
namespace Vulkan::Rendering 
{
struct RenderingAttachmentSettings {
  const void* pNext = nullptr;
  VkImageView imageView = {};
  VkImageLayout imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
  VkResolveModeFlagBits resolveMode = {};
  VkImageView resolveImageView = {};
  VkImageLayout resolveImageLayout = {};
  VkAttachmentLoadOp loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
  VkAttachmentStoreOp storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  VkClearValue clearValue = {};
};
auto create_rendering_attachment_info(const RenderingAttachmentSettings& renderingAttachmentSettings = {}) -> VkRenderingAttachmentInfo;

struct RenderingSettings {};
auto create_rendering_info() -> VkRenderingInfo;
}
