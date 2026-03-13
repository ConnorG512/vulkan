#include "vulkan/engine.hpp"
#include "VkBootstrap.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>
#include <format>
#include <print>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

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
  vkb::InstanceBuilder builder;
  
  auto inst_ret = builder.set_app_name("Example Vulkan App")
    .request_validation_layers(use_validation_layers)
    .use_default_debug_messenger()
    .require_api_version(1,3,0)
    .build();

    vkb::Instance vkb_inst = inst_ret.value();

    instance = vkb_inst.instance;
    debug_messenger = vkb_inst.debug_messenger;

    if(!SDL_Vulkan_CreateSurface(window.get(), instance, nullptr, &surface))
      throw std::runtime_error(std::format("Failed to create Vulkan surface. [{}],", SDL_GetError()));

    VkPhysicalDeviceVulkan13Features features { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES};
    features.dynamicRendering = true;
    features.synchronization2 = true;

    vkb::PhysicalDeviceSelector selector {vkb_inst};
    vkb::PhysicalDevice physicalDevice = selector
      .set_minimum_version(1,3)
      .set_required_features_13(features)
      .set_required_features_12(features12)
      .set_surface(surface)
      .select()
      .value();
  
    vkb::DeviceBuilder deviceBuilder{physicalDevice};
    vkb::Device vkbDevice = deviceBuilder.build().value();

    device = vkbDevice.device;
    chosen_gpu = physicalDevice.physical_device;
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

auto Vulkan::Engine::create_swapchain(uint32_t width, uint32_t height) -> void
{
  vkb::SwapchainBuilder swapchainBuilder{chosen_gpu, device, surface};
  swapchain_image_format = VK_FORMAT_B8G8R8_UNORM;
  vkb::Swapchain vkbSwapchain = swapchainBuilder 
    .set_desired_format(VkSurfaceFormatKHR{.format = swapchain_image_format, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR})
    .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
    .set_desired_extent(width, height)
    .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
    .build()
    .value();

  swapchain_extent = vkbSwapchain.extent;
  swapchain = vkbSwapchain.swapchain;
  swapchainImages = vkbSwapchain.get_images().value();
  swapchainImageViews = vkbSwapchain.get_image_views().value();
}

auto Vulkan::Engine::destroy_swapchain() -> void
{
  vkDestroySwapchainKHR(device, swapchain, nullptr);
  for (auto& image_view : swapchainImageViews)
    vkDestroyImageView(device, image_view, nullptr);
}

auto Vulkan::Engine::cleanup() -> void
{
  if(is_initialised)
  {
    destroy_swapchain();
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device, nullptr);

    vkb::destroy_debug_utils_messenger(instance, debug_messenger);
    vkDestroyInstance(instance, nullptr);
  }
  else 
    std::println("Vulkan is not initialised for cleanup!");
}
