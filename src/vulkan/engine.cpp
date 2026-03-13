#include "vulkan/engine.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>
#include <format>
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
    vkb::VkPhysicalDevice physicalDevice = selector
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
