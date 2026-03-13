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
}
