#include "vulkan/engine.hpp"
#include "VkBootstrap.h"
#include "vulkan/descriptors.hpp"
#include "vulkan/initializer.hpp"
#include "vulkan/util.hpp"
#include "window.hpp"
#include "vulkan/error-handler.hpp"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <format>
#include <print>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>
#include <filesystem>

constexpr bool use_validation_layers {true};

auto Vulkan::Engine::init(Window::Instance& application_window) -> void 
{
  init_vulkan(application_window);
  init_swapchain();
  init_commands();
  init_sync_structures();
  init_descriptors();
  init_pipelines();

  is_initialised = true;
  std::println("Initialise complete!");
}

auto Vulkan::Engine::init_pipelines() -> void
{
  init_background_pipelines();
}


auto Vulkan::Engine::init_background_pipelines() -> void 
{
  VkPipelineLayoutCreateInfo computeLayout{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
    .pNext = nullptr,
    .setLayoutCount = 1,
    .pSetLayouts = &drawImageDescriptorLayout,
  };

  if(const auto vk_res = Vulkan::Error::vk_check(vkCreatePipelineLayout(device, &computeLayout, nullptr, &gradientPipeLineLayout)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());

  VkShaderModule computeDrawShader {};

  
  const char* SHADER_PATH {std::getenv("SHADER_PATH")};
  if(SHADER_PATH != nullptr)
    std::println("Shader path found: {}", SHADER_PATH);
  else
  {
    std::println("No shader path found!");
    const auto exec_path {std::filesystem::canonical("proc/self/exe").parent_path()};
    std::println("Found exec path: {}", exec_path.string());
  }

  if(!Vulkan::Util::load_shader_module("src/shader/gradiant.", device, &computeDrawShader))
    throw std::runtime_error("Failed to open Shader File!");

  VkPipelineShaderStageCreateInfo stageInfo{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .pNext = nullptr,
    .stage = VK_SHADER_STAGE_COMPUTE_BIT,
    .module = computeDrawShader,
    .pName = "main",
  };

  VkComputePipelineCreateInfo computePipelineCreateInfo{
    .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
    .pNext = nullptr,
    .stage = stageInfo,
    .layout = gradientPipeLineLayout,
  };
  
  if(const auto vk_res = Vulkan::Error::vk_check(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &gradientPipeLine)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());

  deletion_queue.push_function([this]{
      vkDestroyPipelineLayout(device, gradientPipeLineLayout, nullptr);
      vkDestroyPipeline(device, gradientPipeLine, nullptr);
      });
}

auto Vulkan::Engine::init_vulkan(Window::Instance& application_window) -> void 
{
  vkb::InstanceBuilder builder;
  
  auto inst_ret = builder.set_app_name("Example Vulkan App")
    .request_validation_layers(use_validation_layers)
    .use_default_debug_messenger()
    .require_api_version(1,3,0)
    .build();

  if(!inst_ret.has_value())
    throw std::runtime_error(std::format("Failed to initialise Vulkan! Error: [{}]", inst_ret.error().message()));

  vkb::Instance vkb_inst = inst_ret.value();

  instance = vkb_inst.instance;
  debug_messenger = vkb_inst.debug_messenger;
  
  const auto suface_result {application_window.create_vulkan_surface(instance, &surface)};
  if(!suface_result.has_value())
    throw std::runtime_error(suface_result.error());

  VkPhysicalDeviceVulkan13Features features { 
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
    .synchronization2 = true,
    .dynamicRendering = true,
  };

  VkPhysicalDeviceVulkan12Features features12 { 
    .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
    .descriptorIndexing= true,
    .bufferDeviceAddress = true,
  };

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

  VmaAllocatorCreateInfo allocatorInfo 
  {
    .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
    .physicalDevice = chosen_gpu,
    .device = device,
    .instance = instance,
  };
  vmaCreateAllocator(&allocatorInfo, &allocator);
  deletion_queue.push_function([&](){vmaDestroyAllocator(allocator);});
}

auto Vulkan::Engine::init_swapchain() -> void 
{
  create_swapchain(1280, 720);

  VkExtent3D drawImageExtent = {
    1280,
    720,
    1
  };
  
  drawImage = {
    .imageExtent = drawImageExtent,
    .imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT,
  };
  
  VkImageUsageFlags drawImageUsages {
    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | 
    VK_IMAGE_USAGE_TRANSFER_DST_BIT |
    VK_IMAGE_USAGE_STORAGE_BIT | 
    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
  };

  VkImageCreateInfo rimg_info {Vulkan::image_create_info(drawImage.imageFormat, drawImageUsages, drawImageExtent)};

  VmaAllocationCreateInfo rimg_alloc_info {
    .usage = VMA_MEMORY_USAGE_GPU_ONLY,
    .requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
  };

  vmaCreateImage(allocator, &rimg_info, &rimg_alloc_info, &drawImage.image, &drawImage.allocation, nullptr);
  
  VkImageViewCreateInfo rview_info {Vulkan::image_view_create_info(drawImage.imageFormat, drawImage.image, VK_IMAGE_ASPECT_COLOR_BIT)};
  if(const auto vk_res = Vulkan::Error::vk_check(vkCreateImageView(device, &rview_info, nullptr, &drawImage.imageView)); !vk_res.has_value())
      std::println("{}", vk_res.error());

  deletion_queue.push_function([=, this](){
        vkDestroyImageView(device, drawImage.imageView, nullptr);
        vmaDestroyImage(allocator, drawImage.image, drawImage.allocation);
      });
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
    const auto pool_res 
      {Vulkan::Error::vk_check(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &frames[index].command_pool))};
    if(!pool_res.has_value())
      throw std::runtime_error(pool_res.error());

    if(const auto vk_res = Vulkan::Error::vk_check(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &frames[index].command_pool)); !vk_res.has_value())
      std::runtime_error(vk_res.error());
    VkCommandBufferAllocateInfo cmdAllocInfo {
      .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = frames[index].command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    if(const auto vk_res = Vulkan::Error::vk_check(vkAllocateCommandBuffers(device, &cmdAllocInfo, &frames[index].main_command_buffer)); !vk_res.has_value())
      std::runtime_error(vk_res.error());
  }
}

auto Vulkan::Engine::init_sync_structures() -> void 
{
  VkFenceCreateInfo fence_create_info {Vulkan::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT)};
  VkSemaphoreCreateInfo semaphore_create_info {Vulkan::semaphore_create_info()};

  for(auto& frame : frames)
  {
    if(const auto vk_res = Vulkan::Error::vk_check(vkCreateFence(device, &fence_create_info, nullptr, &frame.render_fence)); !vk_res.has_value())
      std::runtime_error(vk_res.error());
    if(const auto vk_res = Vulkan::Error::vk_check(vkCreateSemaphore(device, &semaphore_create_info, nullptr, &frame.swapchain_semaphore)); !vk_res.has_value())
      std::runtime_error(vk_res.error());
    if(const auto vk_res = Vulkan::Error::vk_check(vkCreateSemaphore(device, &semaphore_create_info, nullptr, &frame.render_semaphore)); !vk_res.has_value())
      std::runtime_error(vk_res.error());
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

    for(auto& frame : frames)
    {
      vkDestroyCommandPool(device, frame.command_pool, nullptr);
      vkDestroyFence(device, frame.render_fence, nullptr);
      vkDestroySemaphore(device, frame.render_semaphore, nullptr);
      vkDestroySemaphore(device, frame.swapchain_semaphore, nullptr);
    }

    deletion_queue.flush();
  }
  else 
    std::println("Vulkan is not initialised for cleanup!");
}

auto Vulkan::Engine::draw_background(VkCommandBuffer cmd) -> void
{
  vkCmdBindPipeline(cmd,VK_PIPELINE_BIND_POINT_COMPUTE, gradientPipeLine);
  vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, gradientPipeLineLayout, 0, 1, &drawImageDescriptors, 0, nullptr);
  vkCmdDispatch(cmd, std::ceil(drawExtent.width / 16.0f), std::ceil(drawExtent.height / 16.0f), 1);
}

auto Vulkan::Engine::draw() -> void
{
  constexpr auto FENCE_COUNT {1};
  constexpr auto ONE_SECOND {1000000000};

  if(const auto vk_res = Vulkan::Error::vk_check(vkWaitForFences(device, FENCE_COUNT, &get_current_frame().render_fence, true, ONE_SECOND)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());
    
  if(const auto vk_res = Vulkan::Error::vk_check(vkResetFences(device, FENCE_COUNT, &get_current_frame().render_fence)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());
  
  get_current_frame().deletion_queue.flush();

  uint32_t swapchainImageIndex {};
  if(const auto vk_res = Vulkan::Error::vk_check(vkAcquireNextImageKHR(device, swapchain, ONE_SECOND, get_current_frame().swapchain_semaphore, nullptr, &swapchainImageIndex)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());

  VkCommandBuffer cmd {get_current_frame().main_command_buffer};
  if(const auto vk_res = Vulkan::Error::vk_check(vkResetCommandBuffer(get_current_frame().main_command_buffer, 0)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());
  
  // Begin
  VkCommandBufferBeginInfo cmdBeginInfo {Vulkan::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)};
  if(const auto vk_res = Vulkan::Error::vk_check(vkBeginCommandBuffer(cmd, &cmdBeginInfo)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());

  // Buffer Commands
  Vulkan::Util::transition_image(cmd, drawImage.image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_GENERAL);
  
  draw_background(cmd);

  Vulkan::Util::transition_image(cmd, drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  Vulkan::Util::transition_image(cmd, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  Vulkan::Util::copy_image_to_image(cmd, drawImage.image, swapchainImages[swapchainImageIndex], drawExtent, swapchain_extent);

  Vulkan::Util::transition_image(cmd, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  //

  if(const auto vk_res = Vulkan::Error::vk_check(vkEndCommandBuffer(get_current_frame().main_command_buffer)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());
  // End

  frame_number++;
}

auto Vulkan::Engine::init_descriptors() -> void 
{
  std::vector<DescriptorAllocator::PoolSizeRatio> sizes = {
    {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}
  };

  globalDescriptorAllocator.init_pool(device, 10, sizes);

  {
    DescriptorLayoutBuilder builder {};
    builder.add_binding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
    drawImageDescriptorLayout  = builder.build(device, VK_SHADER_STAGE_COMPUTE_BIT);
  }

  drawImageDescriptors = globalDescriptorAllocator.allocate(device, drawImageDescriptorLayout);
  VkDescriptorImageInfo imgInfo{
    .imageView = drawImage.imageView,
    .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
  };
  
  VkWriteDescriptorSet drawImageWrite {
    .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
    .pNext = nullptr,
    .dstSet = drawImageDescriptors,
    .dstBinding = 0,
    .descriptorCount = 1,
    .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
    .pImageInfo = &imgInfo,
  };
  
  vkUpdateDescriptorSets(device, 1, &drawImageWrite, 0, nullptr);
  deletion_queue.push_function([&]{
        globalDescriptorAllocator.destroy_pool(device);
        vkDestroyDescriptorSetLayout(device, drawImageDescriptorLayout, nullptr);
      });
}
