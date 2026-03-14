#pragma once

#include <expected>
#include <string>
#include <string_view>
#include <vulkan/vulkan_core.h>

namespace Vulkan::Error {
[[nodiscard]] auto get_vk_error(VkResult vk_result) noexcept
    -> std::string_view;

[[nodiscard]] auto vk_check(VkResult vk_result)
    -> std::expected<void, std::string>;

} // namespace Vulkan::Error
