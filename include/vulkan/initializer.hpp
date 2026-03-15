#pragma once

#include "vulkan/deletion-queue.hpp"

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
    DeletionQueue deletion_queue{};
  };
  constexpr std::uint32_t FRAME_OVERLAP = 2;

  auto fence_create_info(VkFenceCreateFlags flags = 0) -> VkFenceCreateInfo;
  auto semaphore_create_info(VkSemaphoreCreateFlags flags = 0) -> VkSemaphoreCreateInfo;
  auto semaphore_submit_info(VkPipelineStageFlags2 stage_mask, VkSemaphore semaphore) -> VkSemaphoreSubmitInfo;
  auto command_buffer_begin_info(VkCommandBufferUsageFlags flags = 0) -> VkCommandBufferBeginInfo;
  auto command_buffer_submit_info(VkCommandBuffer cmd) -> VkCommandBufferSubmitInfo;
  auto submit_info(VkCommandBufferSubmitInfo *cmd, VkSemaphoreSubmitInfo *signal_semaphore_info, VkSemaphoreSubmitInfo *wait_semaphore_info) -> VkSubmitInfo2;
}
