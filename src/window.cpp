#include "window.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>
#include <cassert>
#include <format>
#include <vulkan/vulkan_core.h>

Window::Instance::Instance(std::string_view window_title,
                           std::pair<int, int> wh)
    : window_{SDL_CreateWindow(window_title.data(), wh.first, wh.second,
                               SDL_WINDOW_VULKAN),
              &SDL_DestroyWindow} {}

[[nodiscard]] auto Window::Instance::get_current_size() const noexcept
    -> std::expected<std::pair<int, int>, std::string> {

  std::int32_t x{};
  std::int32_t y{};

  if (!SDL_GetWindowSize(window_.get(), &x, &y))
    return std::unexpected(
        std::format("Failed SDL_GetWindowSize, Error [{}]", SDL_GetError()));

  return std::expected<std::pair<int, int>, std::string>{{x, y}};
}

[[nodiscard]] auto
Window::Instance::create_vulkan_surface(VkInstance vk_instance,
                                        VkSurfaceKHR *vk_surface)
    -> std::expected<void, std::string> {
  assert(vk_surface != nullptr);
  if (!SDL_Vulkan_CreateSurface(window_.get(), vk_instance, nullptr,
                                vk_surface))
    return std::unexpected(
        std::format("Failed SDL_Vulkan_CreateSurface [{}]", SDL_GetError()));
  else
    return {};
}
