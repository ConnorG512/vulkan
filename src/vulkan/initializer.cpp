#pragma once

#include "vulkan/initializer.hpp"
#include <vulkan/vulkan_core.h>

auto Vulkan::fence_create_info(VkFenceCreateFlags flags) -> VkFenceCreateInfo
{
  VkFenceCreateInfo info {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .pNext = nullptr,
    .flags = flags,
  };

  return info;
}

auto Vulkan::semaphore_create_info(VkSemaphoreCreateFlags flags) -> VkSemaphoreCreateInfo
{
  VkSemaphoreCreateInfo info {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = nullptr,
    .flags = flags,
  };
  return info;
}

auto command_buffer_begin_info(VkCommandBufferUsageFlags flags) -> VkCommandBufferBeginInfo
{
  VkCommandBufferBeginInfo info = 
  {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .pNext = nullptr,
    .flags = flags,
    .pInheritanceInfo = nullptr,
  };
  return info;
}
