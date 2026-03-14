#include "window.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_video.h>
#include <cassert>
#include <format>

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

[[nodiscard]] auto Window::Instance::ref() noexcept -> SDL_Window & {
  assert(window_ != nullptr);
  return *window_;
}

[[nodiscard]] auto Window::Instance::const_ref() const noexcept
    -> const SDL_Window & {
  assert(window_ != nullptr);
  return *window_;
}
