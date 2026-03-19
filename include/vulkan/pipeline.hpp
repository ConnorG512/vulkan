#pragma once

#include <cstdint>
#include <vulkan/vulkan_core.h>

namespace Vulkan::Pipeline {
auto create_compute_pipeline(VkShaderModule computeShader,
                             VkPipelineLayout pipelineLayout,
                             const char *pName = "main") noexcept
    -> VkComputePipelineCreateInfo;

auto create_pipeline_layout_info(const VkDescriptorSetLayout &setLayout,
                                 std::uint32_t setLayoutCount = 1) noexcept
    -> VkPipelineLayoutCreateInfo;
auto rendering_create_info(VkFormat *colorFormat, VkFormat depthStencilFormat,
                           std::uint32_t colorAttachmentCount = 1) noexcept
    -> VkPipelineRenderingCreateInfo;

auto input_assembly_create_info(
    void *pNext = nullptr, VkPipelineInputAssemblyStateCreateFlags flags = 0,
    VkPrimitiveTopology topology =
        VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
    VkBool32 primitiveRestartEnable = VK_FALSE)
    -> VkPipelineInputAssemblyStateCreateInfo;

auto rasterization_state_create_info() -> VkPipelineRasterizationStateCreateInfo;
auto colour_blend_state_create_info() -> VkPipelineColorBlendStateCreateInfo;
auto multisample_state_create_info() -> VkPipelineMultisampleStateCreateInfo;

enum class ShaderType 
{
  vertex,
  fragment,
  compute,
};
auto create_shader_stage_info(VkShaderStageFlagBits shaderStage) -> VkPipelineShaderStageCreateInfo;

struct InfoPointers {
  const VkPipelineShaderStageCreateInfo *pStages = nullptr;
  const VkPipelineVertexInputStateCreateInfo *pVetexInputState = nullptr;
  const VkPipelineInputAssemblyStateCreateInfo *pInputAssemblyState = nullptr;
  const VkPipelineTessellationStateCreateInfo *pTessellationState = nullptr;
  const VkPipelineViewportStateCreateInfo *pViewportState = nullptr;
  const VkPipelineRasterizationStateCreateInfo *pRasterizationState = nullptr;
  const VkPipelineMultisampleStateCreateInfo *pMultisampleState = nullptr;
  const VkPipelineDepthStencilStateCreateInfo *pDepthStencilState = nullptr;
  const VkPipelineColorBlendStateCreateInfo *pColorBlendState = nullptr;
  const VkPipelineDynamicStateCreateInfo *pDynamicState = nullptr;
};
auto graphics_pipeline_create_info(const InfoPointers &infoPointers,
                                   VkPipelineLayout pipelineLayout,
                                   std::uint32_t flags = 0,
                                   void *pNext = nullptr,
                                   std::uint32_t stageCount = 1) noexcept
    -> VkGraphicsPipelineCreateInfo;
} // namespace Vulkan::Pipeline
