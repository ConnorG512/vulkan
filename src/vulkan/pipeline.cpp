#include "vulkan/pipeline.hpp"
#include <cassert>
#include <vulkan/vulkan_core.h>

auto Vulkan::Pipeline::create_compute_pipeline(VkShaderModule computeShader,
                                               VkPipelineLayout pipelineLayout,
                                               const char *pName) noexcept
    -> VkComputePipelineCreateInfo {
  return VkComputePipelineCreateInfo{
      .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
      .pNext = nullptr,
      .stage =
          {
              .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
              .pNext = nullptr,
              .stage = VK_SHADER_STAGE_COMPUTE_BIT,
              .module = computeShader,
              .pName = pName,
          },
      .layout = pipelineLayout,
  };
}

auto Vulkan::Pipeline::create_pipeline_layout_info(
    const VkDescriptorSetLayout &setLayout,
    std::uint32_t setLayoutCount) noexcept -> VkPipelineLayoutCreateInfo {
  return VkPipelineLayoutCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .setLayoutCount = setLayoutCount,
      .pSetLayouts = &setLayout,
  };
}

auto Vulkan::Pipeline::rendering_create_info(
    VkFormat *colorFormat, VkFormat depthStencilFormat,
    std::uint32_t colorAttachmentCount) noexcept
    -> VkPipelineRenderingCreateInfo {

  assert(colorFormat != nullptr);

  return VkPipelineRenderingCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
      .pNext = nullptr,
      .colorAttachmentCount = colorAttachmentCount,
      .pColorAttachmentFormats = nullptr,
      .depthAttachmentFormat = depthStencilFormat,
      .stencilAttachmentFormat = depthStencilFormat,
  };
}
