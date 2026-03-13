#include "vulkan/engine.hpp"
#include "VkBootstrap.h"
#include "vulkan/initializer.hpp"
#include "vulkan/util.hpp"

#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_vulkan.h>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <print>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

#define VK_CHECK(x)                                                                     \
	do                                                                              \
	{                                                                               \
		VkResult err = x;                                                       \
		if (err)                                                                \
		{                                                                       \
                  std::println(stderr, "VK_CHECK Error: {}", static_cast<int>(err));    \
			abort();                                                        \
		}                                                                       \
	} while (0)

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

    VkPhysicalDeviceVulkan12Features features12 { .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES};
    features12.bufferDeviceAddress = true;
    features12.descriptorIndexing = true;

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

    graphics_queue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    graphics_queue_family = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
}

auto Vulkan::Engine::init_swapchain() -> void 
{

}

auto Vulkan::Engine::init_commands() -> void 
{
  VkCommandPoolCreateInfo commandPoolInfo {
    .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
    .pNext = nullptr,
    .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
    .queueFamilyIndex = graphics_queue_family
  };
  
  for(int index = 0; index < FRAME_OVERLAP; index++)
  {
    VK_CHECK(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &frames[index].command_pool));
    VkCommandBufferAllocateInfo cmdAllocInfo {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = frames[index].command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };
    VK_CHECK(vkAllocateCommandBuffers(device, &cmdAllocInfo, &frames[index].main_command_buffer));
  }
}

auto Vulkan::Engine::init_sync_structures() -> void 
{
  VkFenceCreateInfo fence_create_info {Vulkan::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT)};
  VkSemaphoreCreateInfo semaphore_create_info {Vulkan::semaphore_create_info()};

  for(auto& frame : frames)
  {
    VK_CHECK(vkCreateFence(device, &fence_create_info, nullptr, &frame.render_fence));
    VK_CHECK(vkCreateSemaphore(device, &semaphore_create_info, nullptr, &frame.swapchain_semaphore));
    VK_CHECK(vkCreateSemaphore(device, &semaphore_create_info, nullptr, &frame.render_semaphore));
  }
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
    vkDeviceWaitIdle(device);
    for(int index = 0; index < FRAME_OVERLAP; index++)
      vkDestroyCommandPool(device, frames[index].command_pool, nullptr);

    destroy_swapchain();
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyDevice(device, nullptr);

    vkb::destroy_debug_utils_messenger(instance, debug_messenger);
    vkDestroyInstance(instance, nullptr);
  }
  else 
    std::println("Vulkan is not initialised for cleanup!");
}

auto Vulkan::Engine::draw() -> void
{
  constexpr auto FENCE_COUNT {1};
  constexpr auto ONE_SECOND {1000000000};

  VK_CHECK(vkWaitForFences(device, FENCE_COUNT, &get_current_frame().render_fence, true, ONE_SECOND));
  VK_CHECK(vkResetFences(device, FENCE_COUNT, &get_current_frame().render_fence));

  uint32_t swapchainImageIndex {};
  VK_CHECK(vkAcquireNextImageKHR(device, swapchain, ONE_SECOND, get_current_frame().swapchain_semaphore, nullptr, &swapchainImageIndex));

  VkCommandBuffer cmd {get_current_frame().main_command_buffer};
  VK_CHECK(vkResetCommandBuffer(cmd, 0));
  VkCommandBufferBeginInfo cmdBeginInfo {Vulkan::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)};
  VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

  Vulkan::Util::transition_image(cmd, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);

  VkClearColorValue clearValue;
  auto flash {std::abs(std::sin(frame_number / 120.f))};
  clearValue = {{0.0f, 0.0f, flash, 1.0f}};

  VkImageSubresourceRange clearRange {Vulkan::Util::image_subresource_range(VK_IMAGE_ASPECT_COLOR_BIT)};
  vkCmdClearColorImage(cmd, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &clearRange);
  Vulkan::Util::transition_image(cmd, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  VK_CHECK(vkEndCommandBuffer(cmd));
}
