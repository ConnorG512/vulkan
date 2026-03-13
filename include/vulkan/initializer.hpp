#pragma once

#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace Vulkan 
{
  struct FrameData 
  {
    VkCommandPool command_pool;
    VkCommandBuffer main_command_buffer;
    VkSemaphore swapchain_semaphore, render_semaphore;
    VkFence render_fence;
  };
  constexpr std::uint32_t FRAME_OVERLAP = 2;

  auto fence_create_info(VkFenceCreateFlags flags = 0) -> VkFenceCreateInfo;
  auto semaphore_create_info(VkSemaphoreCreateFlags flags = 0) -> VkSemaphoreCreateInfo;
  auto command_buffer_begin_info(VkCommandBufferUsageFlags flags = 0) -> VkCommandBufferBeginInfo;
}
