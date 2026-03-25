#include "vulkan/drawing.hpp"
#include <cassert>
#include <expected>

auto Vulkan::Draw::wait_for_fences(VkDevice device, std::span<const VkFence> pFences) noexcept -> std::expected<void, std::int32_t> 
{
  assert(device != VK_NULL_HANDLE);
  assert(!pFences.empty());
  assert(!pFences.data());

  const auto waitResult {vkWaitForFences(device, pFences.size(), pFences.data(), true, 1000000000)};
  if(waitResult != VK_SUCCESS)
    return std::unexpected(static_cast<std::int32_t>(waitResult));
  
  const auto resetResult {vkResetFences(device, pFences.size(), pFences.data())};
  if(resetResult != VK_SUCCESS)
    return std::unexpected(static_cast<std::int32_t>(resetResult));
  else 
    return {};
}
