#pragma once 

#include "vulkan/descriptors.hpp"
#include "vulkan/initializer.hpp"
#include "vulkan/deletion-queue.hpp"
#include "vulkan/image.hpp"

#include <vk_mem_alloc.h>
#include <cassert>
#include <cstdint>
#include <vulkan/vulkan_core.h>
#include <vector>

namespace Window {
  class Instance;
}

namespace Vulkan 
{
  class Engine 
  {
    public:
      VkInstance instance{};
      VkDebugUtilsMessengerEXT debug_messenger{};
      VkPhysicalDevice chosen_gpu{};
      VkDevice device{};
      VkSurfaceKHR surface{};

      VkSwapchainKHR swapchain{};
      VkFormat swapchain_image_format{};
      
      DeletionQueue deletion_queue{};
      
      VmaAllocator allocator{};
      AllocatedImage drawImage{};
      VkExtent2D drawExtent{
        1280,
        720,
      };

      std::vector<VkImage> swapchainImages {};
      std::vector<VkImageView> swapchainImageViews {};
      VkExtent2D swapchain_extent{};
      
      DescriptorAllocator globalDescriptorAllocator{};
      VkDescriptorSet drawImageDescriptors{};
      VkDescriptorSetLayout drawImageDescriptorLayout{};

      std::uint32_t frame_number {0};
      Vulkan::FrameData frames[Vulkan::FRAME_OVERLAP];
      auto get_current_frame() -> FrameData&
      {
        return frames[frame_number % FRAME_OVERLAP];
      }
      VkQueue graphics_queue{};
      std::uint32_t graphics_queue_family{};

      bool is_initialised {false};

      auto init(Window::Instance& application_window) -> void;
      auto init_vulkan(Window::Instance& application_window) -> void;
      auto init_swapchain() -> void;
      auto init_commands() -> void;
      auto init_sync_structures() -> void;
      auto cleanup() -> void;
      auto draw() -> void;
      auto draw_background(VkCommandBuffer cmd) -> void;

    private:
      auto create_swapchain(uint32_t width, uint32_t height) -> void;
      auto destroy_swapchain() -> void;
      auto init_descriptors() -> void;
  };
}
