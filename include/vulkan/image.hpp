#pragma once

#include <vk_mem_alloc.h>
#include <vulkan/vulkan_core.h>

namespace Vulkan {
struct AllocatedImage {
  VkImage image{};
  VkImageView imageView{};
  VmaAllocation allocation{};
  VkExtent3D imageExtent{};
  VkFormat format{};
};
} // namespace Vulkan
