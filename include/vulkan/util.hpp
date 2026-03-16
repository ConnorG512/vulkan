#pragma once 

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <string_view>

namespace Vulkan::Util 
{
  auto image_subresource_range(VkImageAspectFlags aspect_mask) -> VkImageSubresourceRange;
  auto transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout) -> void;
  auto copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize) -> void;
  
  // String view must be null terminated.
  auto load_shader_module(std::string_view file_path, VkDevice device, VkShaderModule *outShaderModule) -> bool;
}
