#pragma once

#include <SDL3/SDL_video.h>
#include <expected>
#include <memory>
#include <string>
#include <utility>

namespace Window {
class Instance {
private:
  std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window_{
      SDL_CreateWindow("Vulkan Example", 1280, 720, SDL_WINDOW_VULKAN),
      &SDL_DestroyWindow};

public:
  auto get_current_size() const noexcept -> std::expected<std::pair<int, int>, std::string>;
  auto ref() noexcept -> SDL_Window&;
  auto const_ref() const noexcept -> const SDL_Window&;
};
} // namespace Window
