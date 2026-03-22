#include "vulkan/engine.hpp"
#include "VkBootstrap.h"
#include "file-util.hpp"
#include "vulkan/descriptors.hpp"
#include "vulkan/initializer.hpp"
#include "vulkan/rendering.hpp"
#include "vulkan/util.hpp"
#include "window.hpp"
#include "vulkan/error-handler.hpp"
#include "vulkan/pipeline.hpp"
#include "vulkan/shader-module.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl3.h"
#include "imgui/imgui_impl_vulkan.h"

#include <array>
#include <cmath>
#include <cstdint>
#include <format>
#include <print>
#include <ranges>
#include <stdexcept>
#include <vector>
#include <vulkan/vulkan_core.h>

constexpr bool use_validation_layers {true};

auto Vulkan::Engine::init(Window::Instance& application_window) -> void 
{
  init_vulkan(application_window);
  init_swapchain();
  init_commands();
  init_sync_structures();
  init_descriptors();
  init_pipelines();
  //init_imgui(application_window);

  is_initialised = true;
  std::println("Initialise complete!");
}

auto Vulkan::Engine::init_pipelines() -> void {
  init_background_pipelines();
  init_foreground_pipelines();
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
  
  const auto shader_path {::Util::get_shader_file_path("gradiant.spv")};
  Vulkan::ShaderModule gradiant{shader_path, device};
  
  VkPipelineShaderStageCreateInfo stageInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .stage = VK_SHADER_STAGE_COMPUTE_BIT,
    .module = gradiant.get_vk_module(),
    .pName = "main",
  };

  VkComputePipelineCreateInfo computePipelineCreateInfo {
    .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
    .pNext = nullptr,
    .stage = stageInfo,
    .layout = gradientPipeLineLayout,
  };

  if(const auto vk_res = Vulkan::Error::vk_check(vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &gradientPipeLine)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());

  mainDeletionQueue.push_function([this]{
      vkDestroyPipelineLayout(device, gradientPipeLineLayout, nullptr);
      vkDestroyPipeline(device, gradientPipeLine, nullptr);
      });
}

auto Vulkan::Engine::init_foreground_pipelines() -> void
{
  // Load Shaders: 
  Vulkan::ShaderModule triangleVertex{::Util::get_shader_file_path("triangle.spv"), device};
  Vulkan::ShaderModule triangleFragment{::Util::get_shader_file_path("red-fragment.spv"), device};
  auto triangleVertexStage {Pipeline::create_shader_stage_info({
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = triangleVertex.get_vk_module(),
      .pName = "main", 
      })};
  auto triangleFragmentStage {Pipeline::create_shader_stage_info({
      .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
      .module = triangleFragment.get_vk_module(),
      .pName = "main", 
      })};
  const auto pipelineStagesArray = std::to_array({triangleVertexStage, triangleFragmentStage});
  
  VkPipelineMultisampleStateCreateInfo multisampleState {
    Pipeline::multisample_state_create_info()
  };
  VkPipelineColorBlendAttachmentState colorBlendAttachmentState {
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };
  VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
    .attachmentCount = 1,
    .pAttachments = &colorBlendAttachmentState,
  };
  VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
  };
  VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
  };
  VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
    .polygonMode = VK_POLYGON_MODE_FILL,
    .lineWidth = 1.0f,
  };

  const auto dynamicStates = std::to_array({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR});
  VkPipelineDynamicStateCreateInfo dynamicStateStateCreateInfo{
    .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
    .dynamicStateCount = static_cast<std::uint32_t>(dynamicStates.size()),
    .pDynamicStates = dynamicStates.data(),
  };
  VkPipelineLayout pipelineLayout{};
  VkPipelineRenderingCreateInfo renderingCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
    .colorAttachmentCount = 1,
    .pColorAttachmentFormats = &swapchain_image_format,
  };
  VkPipelineViewportStateCreateInfo viewportStateCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
    .viewportCount = 1,
    .scissorCount = 1,
  };
  
  VkPipelineLayoutCreateInfo layoutInfo{
    Pipeline::create_pipeline_layout_info()
  };
  
  std::println("Pipeline layout result: {}.", static_cast<std::uint32_t>(vkCreatePipelineLayout(device, &layoutInfo, nullptr, &trianglePipeLineLayout)));

  VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo {
    .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    .pNext = &renderingCreateInfo,
    .flags = 0,
    .stageCount = 2, 
    .pStages = pipelineStagesArray.data(),
    .pVertexInputState = &vertexInputStateCreateInfo,
    .pInputAssemblyState = &inputAssemblyStateCreateInfo,
    .pTessellationState = nullptr,
    .pViewportState = &viewportStateCreateInfo,
    .pRasterizationState = &rasterizationStateCreateInfo,
    .pMultisampleState = &multisampleState,
    .pDepthStencilState = nullptr,
    .pColorBlendState = &colorBlendStateCreateInfo,
    .pDynamicState = &dynamicStateStateCreateInfo,
    .layout = trianglePipeLineLayout,
  };
  
  const auto pipeline_res {vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, nullptr, &trianglePipeLine)};
  std::println("Pipeline result: {}", static_cast<std::uint32_t>(pipeline_res));
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
  mainDeletionQueue.push_function([&](){vmaDestroyAllocator(allocator);});
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

  mainDeletionQueue.push_function([=, this](){
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

  if (const auto vk_res = Vulkan::Error::vk_check(vkCreateCommandPool(device, &commandPoolInfo, nullptr, &immCommandPool)); vk_res.has_value())
      std::runtime_error(vk_res.error());

  VkCommandBufferAllocateInfo cmdAllocInfo {Vulkan::command_buffer_allocate_info(immCommandPool, 1)};
  if(const auto vk_res = Vulkan::Error::vk_check(vkAllocateCommandBuffers(device, &cmdAllocInfo, &immCommandBuffer)); !vk_res.has_value())
      std::runtime_error(vk_res.error());

  mainDeletionQueue.push_function([this]{
      vkDestroyCommandPool(device, immCommandPool, nullptr);
      });
  
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
  VkFenceCreateInfo fenceCreateInfo {Vulkan::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT)};
  VkSemaphoreCreateInfo semaphoreCreateInfo {Vulkan::semaphore_create_info()};

  for(auto& frame : frames)
  {
    if(const auto vk_res = Vulkan::Error::vk_check(vkCreateFence(device, &fenceCreateInfo, nullptr, &frame.render_fence)); !vk_res.has_value())
      std::runtime_error(vk_res.error());
    if(const auto vk_res = Vulkan::Error::vk_check(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &frame.swapchain_semaphore)); !vk_res.has_value())
      std::runtime_error(vk_res.error());
    if(const auto vk_res = Vulkan::Error::vk_check(vkCreateSemaphore(device, &semaphoreCreateInfo, nullptr, &frame.render_semaphore)); !vk_res.has_value())
      std::runtime_error(vk_res.error());
  }

  if(const auto vk_res = Vulkan::Error::vk_check(vkCreateFence(device, &fenceCreateInfo, nullptr, &immFence)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());
  mainDeletionQueue.push_function([this]{vkDestroyFence(device, immFence, nullptr);});
}

auto Vulkan::Engine::create_swapchain(uint32_t width, uint32_t height) -> void
{
  vkb::SwapchainBuilder swapchainBuilder{chosen_gpu, device, surface};
  swapchain_image_format = VK_FORMAT_B8G8R8A8_UNORM;
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

    mainDeletionQueue.flush();
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

  Vulkan::Util::transition_image(cmd, drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
  VkRenderingAttachmentInfo colorAttachment { Vulkan::Rendering::create_rendering_attachment_info({.imageView = drawImage.imageView})};
  VkRenderingInfo renderingInfo{
    .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
    .renderArea = {{0,0}, {drawExtent.width, drawExtent.height}},
    .layerCount = 1,
    .colorAttachmentCount = 1,
    .pColorAttachments = &colorAttachment,
  };
  vkCmdBeginRendering(cmd, &renderingInfo);
  vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, trianglePipeLine);
  VkViewport viewport{
    .x = 0,
    .y = 0,
    .width = static_cast<float>(drawExtent.width),
    .height = static_cast<float>(drawExtent.height),
    .minDepth = 0.0f,
    .maxDepth = 1.0f,
  };
  vkCmdSetViewport(cmd, 0, 1, &viewport);
  VkRect2D scissor{{0,0}, {drawExtent.width, drawExtent.height}};
  vkCmdSetScissor(cmd, 0,1, &scissor);
  vkCmdDraw(cmd, 3,1,0,0);
  vkCmdEndRendering(cmd);

  Vulkan::Util::transition_image(cmd, drawImage.image, VK_IMAGE_LAYOUT_GENERAL, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
  Vulkan::Util::transition_image(cmd, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

  Vulkan::Util::copy_image_to_image(cmd, drawImage.image, swapchainImages[swapchainImageIndex], drawExtent, swapchain_extent);

  Vulkan::Util::transition_image(cmd, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

  //draw_imgui(cmd, swapchainImageViews[swapchainImageIndex]);

  Vulkan::Util::transition_image(cmd, swapchainImages[swapchainImageIndex], VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  
  //
  if(const auto vk_res = Vulkan::Error::vk_check(vkEndCommandBuffer(get_current_frame().main_command_buffer)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());
  // End
  
  VkCommandBufferSubmitInfo cmdInfo {Vulkan::command_buffer_submit_info(cmd)};
  VkSemaphoreSubmitInfo waitInfo {Vulkan::semaphore_submit_info(VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT_KHR, get_current_frame().swapchain_semaphore)};
  VkSemaphoreSubmitInfo signalInfo {Vulkan::semaphore_submit_info(VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, get_current_frame().render_semaphore)};
  
  VkSubmitInfo2 submit {Vulkan::submit_info(&cmdInfo, &signalInfo, &waitInfo)};

  if(const auto vk_res = Vulkan::Error::vk_check(vkQueueSubmit2(graphics_queue, 1, &submit, get_current_frame().render_fence)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());
  
  VkPresentInfoKHR presentInfo {
    .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
    .pNext = nullptr,
    .waitSemaphoreCount = 1,
    .pWaitSemaphores = &get_current_frame().render_semaphore,
    .swapchainCount = 1,
    .pSwapchains = &swapchain,
    .pImageIndices = &swapchainImageIndex,
  };

  if(const auto vk_res = Vulkan::Error::vk_check(vkQueuePresentKHR(graphics_queue, &presentInfo)); !vk_res.has_value())
      throw std::runtime_error(vk_res.error());

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
  mainDeletionQueue.push_function([&]{
        globalDescriptorAllocator.destroy_pool(device);
        vkDestroyDescriptorSetLayout(device, drawImageDescriptorLayout, nullptr);
      });
}

auto Vulkan::Engine::immediate_submit(std::function<void(VkCommandBuffer cmd)>&& function) -> void
{
  if(const auto vk_res = Vulkan::Error::vk_check(vkResetFences(device, 1, &immFence)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());
  
  if(const auto vk_res = Vulkan::Error::vk_check(vkResetCommandBuffer(immCommandBuffer, 0)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());

  VkCommandBuffer cmd {};

  VkCommandBufferBeginInfo cmdBeginInfo {Vulkan::command_buffer_begin_info(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)};
  
  if(const auto vk_res = Vulkan::Error::vk_check(vkBeginCommandBuffer(cmd, &cmdBeginInfo)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());

  function(cmd);
  
  if(const auto vk_res = Vulkan::Error::vk_check(vkEndCommandBuffer(cmd)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());
  
  VkCommandBufferSubmitInfo cmdInfo {Vulkan::command_buffer_submit_info(cmd)};
  VkSubmitInfo2 submit {Vulkan::submit_info(&cmdInfo, nullptr, nullptr)};
  
  if(const auto vk_res = Vulkan::Error::vk_check(vkQueueSubmit2(graphics_queue, 1, &submit, immFence)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());
  
  constexpr auto TIMEOUT {9999999999};
  if(const auto vk_res = Vulkan::Error::vk_check(vkWaitForFences(device, 1, &immFence, true, TIMEOUT)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());
}

auto Vulkan::Engine::init_imgui(Window::Instance& application_window) -> void 
{
  constexpr auto pool_sizes = std::to_array<VkDescriptorPoolSize>({
      {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
      {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
      {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
      {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000},
  });
  
  VkDescriptorPoolCreateInfo pool_info {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
    .maxSets = 1000,
    .poolSizeCount = static_cast<std::uint32_t>(pool_sizes.size()),
    .pPoolSizes = pool_sizes.data(),
  };

  VkDescriptorPool imguiPool{};
  if(const auto vk_res = Vulkan::Error::vk_check(vkCreateDescriptorPool(device, &pool_info, nullptr, &imguiPool)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());
  
  // Creating the ImGui context:
  ImGui::CreateContext();
  
  ImGui_ImplSDL3_InitForVulkan(application_window.get_window_instance());

  ImGui_ImplVulkan_InitInfo init_info {
    .Instance = instance,
    .PhysicalDevice = chosen_gpu,
    .Device = device,
    .Queue = graphics_queue,
    .DescriptorPool = imguiPool,
    .MinImageCount = 3,
    .ImageCount = 3,
    .PipelineInfoMain = {
      .RenderPass = VK_NULL_HANDLE,
      .Subpass = 0,
      .MSAASamples = VK_SAMPLE_COUNT_1_BIT,
    },
    .UseDynamicRendering = true,
  };
  
  ImGui_ImplVulkan_Init(&init_info);
  mainDeletionQueue.push_function([=, this]{
      ImGui_ImplVulkan_Shutdown();
      vkDestroyDescriptorPool(device, imguiPool, nullptr);
      });
}

auto Vulkan::Engine::draw_imgui(VkCommandBuffer cmd, VkImageView targetImageView) -> void
{
  VkRenderingAttachmentInfo colorAttachment {Vulkan::attachment_info(targetImageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL)};
  VkRenderingInfo renderInfo {
    .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
    .pNext = nullptr,
    .flags = 0,
    .renderArea = {
      .extent = VkExtent2D {
        .width = 1280,
        .height = 720,
      },
    },
    .layerCount = 1,
    .viewMask = 0,
    .colorAttachmentCount = 1,
    .pColorAttachments = &colorAttachment,
    .pDepthAttachment = nullptr,
    .pStencilAttachment = nullptr,
  };

  vkCmdBeginRendering(cmd, &renderInfo);
  ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
  vkCmdEndRendering(cmd);
}
