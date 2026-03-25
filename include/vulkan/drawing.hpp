#pragma once 

#include <cstdint>
#include <expected>
#include <span>
#include <vulkan/vulkan_core.h>

namespace Vulkan::Draw 
{
  auto wait_for_fences(VkDevice device, std::span<const VkFence> pFences) noexcept -> std::expected<void, std::int32_t>;
}
