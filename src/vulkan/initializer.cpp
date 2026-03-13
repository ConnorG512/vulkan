#pragma once

#include "vulkan/initializer.hpp"
#include <cassert>
#include <vulkan/vulkan_core.h>

auto Vulkan::fence_create_info(VkFenceCreateFlags flags) -> VkFenceCreateInfo
{
  return VkFenceCreateInfo {
    .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
    .pNext = nullptr,
    .flags = flags,
  };
}

auto Vulkan::semaphore_create_info(VkSemaphoreCreateFlags flags) -> VkSemaphoreCreateInfo
{
  return VkSemaphoreCreateInfo {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
    .pNext = nullptr,
    .flags = flags,
  };
}

auto Vulkan::semaphore_submit_info(VkPipelineStageFlags2 stage_mask, VkSemaphore semaphore) -> VkSemaphoreSubmitInfo
{
  return VkSemaphoreSubmitInfo {
    .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
    .pNext = nullptr,
    .semaphore = semaphore,
    .value = 1,
    .stageMask = stage_mask,
    .deviceIndex = 0,
  };
}

auto Vulkan::command_buffer_begin_info(VkCommandBufferUsageFlags flags) -> VkCommandBufferBeginInfo
{
  return VkCommandBufferBeginInfo  
  {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
    .pNext = nullptr,
    .flags = flags,
    .pInheritanceInfo = nullptr,
  };
}

auto Vulkan::command_buffer_submit_info(VkCommandBuffer cmd) -> VkCommandBufferSubmitInfo
{
  return VkCommandBufferSubmitInfo {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
    .pNext = nullptr,
    .commandBuffer = cmd,
    .deviceMask = 0,
  };
}

auto Vulkan::submit_info(VkCommandBufferSubmitInfo *cmd, VkSemaphoreSubmitInfo *signal_semaphore_info, VkSemaphoreSubmitInfo *wait_semaphore_info) -> VkSubmitInfo2
{
  assert(cmd != nullptr);

  return VkSubmitInfo2 {
    .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
    .pNext = nullptr,
    .waitSemaphoreInfoCount = static_cast<uint32_t>(wait_semaphore_info == nullptr ? 0 : 1),
    .pWaitSemaphoreInfos = wait_semaphore_info,
    .commandBufferInfoCount = 1,
    .pCommandBufferInfos = cmd,
    .signalSemaphoreInfoCount = static_cast<uint32_t>(signal_semaphore_info == nullptr ? 0 : 1),
    .pSignalSemaphoreInfos = signal_semaphore_info,
  };
}
