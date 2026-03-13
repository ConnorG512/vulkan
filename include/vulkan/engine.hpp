#pragma once 

#include "vulkan/initializer.hpp"

#include <cstdint>
#include <memory>
#include <vulkan/vulkan_core.h>
#include <SDL3/SDL_video.h>
#include <vector>

namespace Vulkan 
{
  class Engine 
  {
    public:
      VkInstance instance;
      VkDebugUtilsMessengerEXT debug_messenger;
      VkPhysicalDevice chosen_gpu;
      VkDevice device;
      VkSurfaceKHR surface;

      VkSwapchainKHR swapchain;
      VkFormat swapchain_image_format;

      std::vector<VkImage> swapchainImages {};
      std::vector<VkImageView> swapchainImageViews {};
      VkExtent2D swapchain_extent;
      
      std::uint32_t frame_number;
      Vulkan::FrameData frames[Vulkan::FRAME_OVERLAP];
      auto get_current_frame() -> FrameData&
      {
        return frames[frame_number % FRAME_OVERLAP];
      }
      VkQueue graphics_queue;
      std::uint32_t graphics_queue_family;

      std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> window {nullptr, nullptr};

      bool is_initialised = false;

      auto init() -> void;
      auto init_vulkan() -> void;
      auto init_swapchain() -> void;
      auto init_commands() -> void;
      auto init_sync_structures() -> void;
      auto cleanup() -> void;
      auto draw() -> void;

    private:
      auto create_swapchain(uint32_t width, uint32_t height) -> void;
      auto destroy_swapchain() -> void;
  };
}
