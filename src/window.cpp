#include "window.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_video.h>
#include <format>

auto Window::Instance::getCurrentSize() const noexcept
    -> std::expected<std::pair<int, int>, std::string> {

  std::int32_t x{};
  std::int32_t y{};

  if (!SDL_GetWindowSize(window_.get(), &x, &y))
    return std::unexpected(
        std::format("Failed SDL_GetWindowSize, Error [{}]", SDL_GetError()));

  return std::expected<std::pair<int, int>, std::string>{{x, y}};
}
