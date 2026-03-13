#include "vulkan/engine.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <format>
#include <stdexcept>

constexpr bool use_validation_layers {false};

auto Vulkan::Engine::init() -> void 
{
  if(!SDL_Init(SDL_INIT_VIDEO))
    throw std::runtime_error(std::format("Failed to init video. [{}],", SDL_GetError()));

  SDL_WindowFlags window_flags {SDL_WINDOW_VULKAN};

  window.reset(SDL_CreateWindow("window", 1280, 720, window_flags));
  if(window == nullptr)
    throw std::runtime_error(std::format("Failed to create window. [{}],", SDL_GetError()));

  init_vulkan();
  init_swapchain();
  init_commands();
  init_sync_structures();

  is_initialised = true;
}

auto Vulkan::Engine::init_vulkan() -> void 
{

}

auto Vulkan::Engine::init_swapchain() -> void 
{

}

auto Vulkan::Engine::init_commands() -> void 
{

}

auto Vulkan::Engine::init_sync_structures() -> void 
{

}
