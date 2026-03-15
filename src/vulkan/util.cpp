#include "vulkan/util.hpp"
#include <cstdint>
#include <vulkan/vulkan_core.h>

auto Vulkan::Util::image_subresource_range(VkImageAspectFlags aspect_mask) -> VkImageSubresourceRange
{
  return VkImageSubresourceRange {
    .aspectMask = aspect_mask,
    .baseMipLevel = 0,
    .levelCount = VK_REMAINING_MIP_LEVELS,
    .baseArrayLayer = 0,
    .layerCount = VK_REMAINING_MIP_LEVELS,
  };
}

auto Vulkan::Util::transition_image(VkCommandBuffer cmd, VkImage image, VkImageLayout current_layout, VkImageLayout new_layout) -> void 
{
  VkImageAspectFlags aspect_mask {(new_layout == VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL) ? VK_IMAGE_ASPECT_DEPTH_BIT : VK_IMAGE_ASPECT_COLOR_BIT};
  
  VkImageMemoryBarrier2 image_barrier {
    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
    .pNext = nullptr,
    .srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
    .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
    .dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
    .dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,

    .oldLayout = current_layout,
    .newLayout = new_layout,
    
    .image = image,
    .subresourceRange = image_subresource_range(aspect_mask),
  };

  VkDependencyInfo dep_info {
    .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
    .pNext = nullptr,
    .imageMemoryBarrierCount = 1,
    .pImageMemoryBarriers = &image_barrier,
  };

  vkCmdPipelineBarrier2(cmd, &dep_info);
}

auto copy_image_to_image(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize) -> void
{
  constexpr auto Z_OFFSET {1};
  
  VkImageBlit2 blitReigon {
    .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
    .pNext = nullptr,
    .srcSubresource = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .mipLevel = 0,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
    .srcOffsets = {
      {},
      {static_cast<int32_t>(srcSize.width), static_cast<int32_t>(srcSize.height), Z_OFFSET}
    },
    .dstSubresource = {
      .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
      .mipLevel = 0,
      .baseArrayLayer = 0,
      .layerCount = 1,
    },
    .dstOffsets = {
      {},
      {static_cast<int32_t>(dstSize.width), static_cast<int32_t>(dstSize.height), Z_OFFSET}
    },
  };

  VkBlitImageInfo2 blitInfo = {
    .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
    .pNext = nullptr,
    .srcImage = source,
    .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
    .dstImage = destination, 
    .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
    .regionCount = 1,
    .pRegions = &blitReigon,
    .filter = VK_FILTER_LINEAR,
  };

  vkCmdBlitImage2(cmd, &blitInfo);
}
