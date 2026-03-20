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

auto Vulkan::Pipeline::rendering_create_info(const PipelineRenderingInfoSettings &pipelineRenderingInfoSettings) noexcept
    -> VkPipelineRenderingCreateInfo {

  assert(pipelineRenderingInfoSettings.pColorAttachmentFormats != nullptr);

  return VkPipelineRenderingCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
      .pNext = pipelineRenderingInfoSettings.pNext,
      .colorAttachmentCount = pipelineRenderingInfoSettings.colorAttachmentCount,
      .pColorAttachmentFormats = pipelineRenderingInfoSettings.pColorAttachmentFormats,
      .depthAttachmentFormat = pipelineRenderingInfoSettings.depthAttachmentFormat,
      .stencilAttachmentFormat = pipelineRenderingInfoSettings.stencilAttachmentFormat,
  };
}

auto Vulkan::Pipeline::create_shader_stage_info(
    const ShaderStageSettings &shaderStageInfo) noexcept
    -> VkPipelineShaderStageCreateInfo {
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = shaderStageInfo.pNext,
      .flags = shaderStageInfo.flags,
      .stage = shaderStageInfo.stage,
      .module = shaderStageInfo.module,
      .pName = shaderStageInfo.pName,
      .pSpecializationInfo = shaderStageInfo.pSpecializationInfo,
  };
}

auto Vulkan::Pipeline::input_assembly_create_info(
    void *pNext, VkPipelineInputAssemblyStateCreateFlags flags,
    VkPrimitiveTopology topology, VkBool32 primitiveRestartEnable) noexcept
    -> VkPipelineInputAssemblyStateCreateInfo {

  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .pNext = pNext,
      .flags = flags,
      .topology = topology,
      .primitiveRestartEnable = primitiveRestartEnable,
  };
}

auto Vulkan::Pipeline::rasterization_state_create_info(
    const RasterizationStateSettings &rasterizationStateSettings, void *pNext,
    VkPipelineLayoutCreateFlags flags) noexcept
    -> VkPipelineRasterizationStateCreateInfo {
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .pNext = pNext,
      .flags = flags,
      .depthClampEnable = rasterizationStateSettings.depthBiasEnable,
      .rasterizerDiscardEnable =
          rasterizationStateSettings.rasterizerDiscardEnable,
      .polygonMode = rasterizationStateSettings.polygonMode,
      .cullMode = rasterizationStateSettings.cullMode,
      .frontFace = rasterizationStateSettings.frontFace,
      .depthBiasEnable = rasterizationStateSettings.depthBiasEnable,
      .depthBiasConstantFactor =
          rasterizationStateSettings.depthBiasConstantFactor,
      .depthBiasClamp = rasterizationStateSettings.depthBiasClamp,
      .depthBiasSlopeFactor = rasterizationStateSettings.depthBiasSlopeFactor,
      .lineWidth = 1.0f,
  };
}

auto Vulkan::Pipeline::color_blend_state_create_info(
    const void *pNext, VkPipelineColorBlendStateCreateFlags flags,
    const BlendStateSettings &blendStateSettings) noexcept
    -> VkPipelineColorBlendStateCreateInfo {
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .pNext = pNext,
      .flags = flags,
      .logicOpEnable = blendStateSettings.logicOpEnable,
      .attachmentCount = blendStateSettings.attachmentCount,
      .pAttachments = blendStateSettings.pAttachments,
      .blendConstants =
          {
              blendStateSettings.blendConstants.at(0),
              blendStateSettings.blendConstants.at(1),
              blendStateSettings.blendConstants.at(2),
              blendStateSettings.blendConstants.at(3),
          },
  };
}

auto Vulkan::Pipeline::multisample_state_create_info(
    void *pNext, VkPipelineMultisampleStateCreateFlags flags,
    const MultisampleStateSettings &multisampleSettings) noexcept
    -> VkPipelineMultisampleStateCreateInfo {
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .pNext = pNext,
      .flags = flags,
  };
}

auto Vulkan::Pipeline::graphics_pipeline_create_info(
    const GraphicsPipelineSettings &gPSettings) noexcept
    -> VkGraphicsPipelineCreateInfo {
  return VkGraphicsPipelineCreateInfo{
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = gPSettings.pNext,
      .flags = gPSettings.flags,
      .stageCount = gPSettings.stageCount,
      .pStages = gPSettings.pStages,
      .pVertexInputState = gPSettings.pVetexInputState,
      .pInputAssemblyState = gPSettings.pInputAssemblyState,
      .pTessellationState = gPSettings.pTessellationState,
      .pViewportState = gPSettings.pViewportState,
      .pRasterizationState = gPSettings.pRasterizationState,
      .pMultisampleState = gPSettings.pMultisampleState,
      .pDepthStencilState = gPSettings.pDepthStencilState,
      .pColorBlendState = gPSettings.pColorBlendState,
      .pDynamicState = gPSettings.pDynamicState,
      .layout = gPSettings.layout,
      .renderPass = VK_NULL_HANDLE,
      .subpass = 0,
      .basePipelineHandle = VK_NULL_HANDLE,
      .basePipelineIndex = -1,
  };
}

[[nodiscard]] auto Vulkan::Pipeline::create_graphics_pipelines(
    const CreateGraphicsPipelineSettings &graphicsPipelineSettings)
    -> VkResult {

  return vkCreateGraphicsPipelines(
      graphicsPipelineSettings.device, graphicsPipelineSettings.pipelineCache,
      graphicsPipelineSettings.createInfoCount,
      graphicsPipelineSettings.pCreateInfos,
      graphicsPipelineSettings.pAllocator, graphicsPipelineSettings.pPipeline);
}
