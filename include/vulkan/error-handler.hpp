#pragma once

#include <concepts>
#include <expected>
#include <format>
#include <string>
#include <string_view>
#include <vulkan/vulkan_core.h>

namespace Vulkan::Error {
[[nodiscard]] auto get_vk_error(VkResult vk_result) noexcept
    -> std::string_view;

template <typename F, typename... Args>
  requires std::invocable<F, Args...>
[[nodiscard]] auto vk_check(F vk_func, Args &&...args)
    -> std::expected<void, std::string> {

  const VkResult result{vk_func(std::forward<Args>(args)...)};
  if (result == VK_SUCCESS)
    return {};
  else {
    return std::unexpected(
        std::format("VK_ERROR: [{}].", get_vk_error(result)));
  }
}
} // namespace Vulkan::Error
