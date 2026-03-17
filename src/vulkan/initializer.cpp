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

auto Vulkan::command_buffer_allocate_info(VkCommandPool pool, std::uint32_t count) -> VkCommandBufferAllocateInfo
{
  return VkCommandBufferAllocateInfo {
    .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
    .pNext = nullptr,
    .commandPool = pool,
    .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
    .commandBufferCount = count,
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

auto Vulkan::image_create_info(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent) -> VkImageCreateInfo
{
  return {
    .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
    .pNext = nullptr,
    .imageType = VK_IMAGE_TYPE_2D,
    .format = format,
    .extent = extent,
    .mipLevels = 1,
    .arrayLayers = 1,
    .samples = VK_SAMPLE_COUNT_1_BIT,
    .tiling = VK_IMAGE_TILING_OPTIMAL,
    .usage = usageFlags,
  };
}

auto Vulkan::image_view_create_info(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags) -> VkImageViewCreateInfo
{
  return {
    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
    .pNext = nullptr,
    .image = image,
    .viewType = VK_IMAGE_VIEW_TYPE_2D,
    .format = format,
    .subresourceRange = {
      .aspectMask = aspectFlags,
      .baseMipLevel = 0,
      .levelCount = 1,
      .baseArrayLayer = 0,
      .layerCount = 1,
    }
  };
}
