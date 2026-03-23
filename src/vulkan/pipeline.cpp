#include "vulkan/pipeline.hpp"

#include <cassert>
#include <expected>
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
) noexcept -> VkPipelineLayoutCreateInfo {
  return VkPipelineLayoutCreateInfo{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
      .pNext = nullptr,
      .setLayoutCount = {},
      .pSetLayouts = nullptr,
      .pushConstantRangeCount = 0,
      .pPushConstantRanges = nullptr,
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
  
    const VkPipelineShaderStageCreateInfo result {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = shaderStageInfo.pNext,
      .flags = shaderStageInfo.flags,
      .stage = shaderStageInfo.stage,
      .module = shaderStageInfo.module,
      .pName = shaderStageInfo.pName,
      .pSpecializationInfo = shaderStageInfo.pSpecializationInfo,
    };
    
    /*
      https://docs.vulkan.org/refpages/latest/refpages/source/VkShaderStageFlagBits.html
      Smallest valid value for shader is 1, should never be 0 and will be invalid if default initialised with 0.
    */
    assert(result.stage != 0);
    return result;
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
    const MultisampleStateSettings &multisampleSettings) noexcept
    -> VkPipelineMultisampleStateCreateInfo {
  return {
      .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
      .pNext = multisampleSettings.pNext,
      .flags = multisampleSettings.flags,
      .rasterizationSamples = multisampleSettings.rasterizationSamples,
      .sampleShadingEnable = multisampleSettings.sampleShadingEnable,
      .minSampleShading = multisampleSettings.minSampleShading,
      .pSampleMask = multisampleSettings.pSampleMask,
      .alphaToCoverageEnable = multisampleSettings.alphaToCoverageEnable,
      .alphaToOneEnable = multisampleSettings.alphaToOneEnable,
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
      .pVertexInputState = gPSettings.pVertexInputState,
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

auto Vulkan::Pipeline::init_graphics_pipeline(
    std::span<VkPipelineShaderStageCreateInfo> shaderStages, VkDevice device,
    VkPipeline &pipeline, VkPipelineLayout pipelineLayout, VkFormat &colorAttachment)
    -> std::expected<void, std::int32_t> {
  
  assert(!shaderStages.empty());

  VkPipelineInputAssemblyStateCreateInfo inputAssembly{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
      .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
  };
  
  VkPipelineVertexInputStateCreateInfo vertexInputState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
  };
  
  VkPipelineMultisampleStateCreateInfo multisampleState{
      multisample_state_create_info()};

  VkPipelineViewportStateCreateInfo viewportState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
      .viewportCount = 1,
      .scissorCount = 1,
  };
  VkPipelineRasterizationStateCreateInfo rasterizationState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
      .polygonMode = VK_POLYGON_MODE_FILL,
      .lineWidth = 1.0f,
  };
  VkPipelineColorBlendAttachmentState colorBlendAttachmentState{
      .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
  };
  VkPipelineColorBlendStateCreateInfo colorBlendState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
      .attachmentCount = 1,
      .pAttachments = &colorBlendAttachmentState,
  };
  
  const auto dynamicStates =
      std::to_array({VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR});
  
  VkPipelineDynamicStateCreateInfo dynamicState{
      .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
      .dynamicStateCount = static_cast<std::uint32_t>(dynamicStates.size()),
      .pDynamicStates = dynamicStates.data(),
  };

  VkPipelineRenderingCreateInfo renderingCreateInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
    .colorAttachmentCount = 1,
    .pColorAttachmentFormats = &colorAttachment,
  };
  
  const VkGraphicsPipelineCreateInfo pipelineInfo{
      .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
      .pNext = &renderingCreateInfo,
      .flags = 0,
      .stageCount = static_cast<std::uint32_t>(shaderStages.size()),
      .pStages = shaderStages.data(),
      .pVertexInputState = &vertexInputState,
      .pInputAssemblyState = &inputAssembly,
      .pTessellationState = nullptr,
      .pViewportState = &viewportState,
      .pRasterizationState = &rasterizationState,
      .pMultisampleState = &multisampleState,
      .pDepthStencilState = nullptr,
      .pColorBlendState = &colorBlendState,
      .pDynamicState = &dynamicState,
      .layout = pipelineLayout,
  };

  const auto pipeline_res{vkCreateGraphicsPipelines(
      device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline)};
  if (pipeline_res != VK_SUCCESS)
    return std::unexpected<std::uint32_t>(static_cast<std::int32_t>(pipeline_res));
  else
    return {};
}
