#pragma once

#include <SDL3/SDL_video.h>
#include <expected>
#include <memory>
#include <string>
#include <utility>
#include <vulkan/vulkan_core.h>

namespace Window {
class Instance {
private:
  std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window_{
      SDL_CreateWindow("App Window", 1280, 720, SDL_WINDOW_VULKAN),
      &SDL_DestroyWindow};

public:
  Instance(std::string_view window_title, std::pair<int, int> wh);
  Instance() = default;

  [[nodiscard]] auto get_current_size() const noexcept
      -> std::expected<std::pair<int, int>, std::string>;
  [[nodiscard]] auto create_vulkan_surface(VkInstance vk_instance,
                                           VkSurfaceKHR *vk_surface)
      -> std::expected<void, std::string>;
};
} // namespace Window
