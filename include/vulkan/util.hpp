#pragma once 

#include <vulkan/vulkan.h>

namespace Vulkan::Util 
{
  auto transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout) -> void;
}
