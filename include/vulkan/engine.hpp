#pragma once 

#include <vulkan/vulkan_core.h>
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

      auto init_vulkan() -> void;
      auto init_swapchain() -> void;
      auto init_commands() -> void;
      auto init_sync_structures() -> void;
  };
}
