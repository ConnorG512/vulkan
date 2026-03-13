#pragma once 

#include <memory>
#include <vulkan/vulkan_core.h>
#include <SDL3/SDL_video.h>

namespace Vulkan 
{
  class Engine 
  {
    public:
      VkInstance instance;
      VkDebugUtilsMessengerEXT debug_messenger;
      VkPhysicalDevice chosen_gpu;
      VkDevice device;
      VkSurfaceKHR suface;

      std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window {nullptr, nullptr};

      bool is_initialised = false;

      auto init() -> void;
      auto init_vulkan() -> void;
      auto init_swapchain() -> void;
      auto init_commands() -> void;
      auto init_sync_structures() -> void;
  };
}
