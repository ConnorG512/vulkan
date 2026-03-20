#pragma once

#include <array>
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
        VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    VkBool32 primitiveRestartEnable = VK_FALSE) noexcept
    -> VkPipelineInputAssemblyStateCreateInfo;

struct RasterizationStateSettings {
  VkBool32 depthClampEnable = VK_FALSE;
  VkBool32 rasterizerDiscardEnable = VK_FALSE;
  VkBool32 depthBiasEnable = VK_FALSE;
  VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL;
  VkCullModeFlags cullMode = VK_CULL_MODE_NONE;
  VkFrontFace frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
  float depthBiasConstantFactor = 1.0f;
  float depthBiasClamp = 1.0f;
  float depthBiasSlopeFactor = 1.0f;
};
auto rasterization_state_create_info(
    const RasterizationStateSettings &rasterizationStateSettings = {},
    void *pNext = nullptr, VkPipelineLayoutCreateFlags flags = 0) noexcept
    -> VkPipelineRasterizationStateCreateInfo;

struct BlendStateSettings {
  VkBool32 logicOpEnable = VK_TRUE;
  VkLogicOp logicOp = VK_LOGIC_OP_NO_OP;
  std::uint32_t attachmentCount = 1;
  const VkPipelineColorBlendAttachmentState *pAttachments = nullptr;
  std::array<float, 4> blendConstants = {1.0f, 1.0f, 1.0f, 1.0f};
};
auto color_blend_state_create_info(
    const void *pNext = nullptr, VkPipelineColorBlendStateCreateFlags flags = 0,
    const BlendStateSettings &blendStateSettings = {}) noexcept
    -> VkPipelineColorBlendStateCreateInfo;

struct MultisampleStateSettings {
  VkSampleCountFlagBits rasterizationSamples = VK_SAMPLE_COUNT_32_BIT;
  VkBool32 sampleShadingEnable = VK_FALSE;
  float minSampleShading = 1.0f;
  const VkSampleMask *pSampleMask = nullptr;
  VkBool32 alphaToCoverageEnable = VK_FALSE;
  VkBool32 alphaToOneEnable = VK_FALSE;
};
auto multisample_state_create_info(
    void *pNext = nullptr, VkPipelineMultisampleStateCreateFlags flags = 0,
    const MultisampleStateSettings &multisampleSettings = {}) noexcept
    -> VkPipelineMultisampleStateCreateInfo;

struct ShaderStageSettings {
  const void *pNext{nullptr};
  VkPipelineShaderStageCreateFlags flags{0};
  VkShaderStageFlagBits stage{};
  VkShaderModule module{};
  const char *pName{"main"};
  const VkSpecializationInfo *pSpecializationInfo{nullptr};
};
auto create_shader_stage_info(
    const ShaderStageSettings &shaderStageInfo = {}) noexcept
    -> VkPipelineShaderStageCreateInfo;

struct GraphicsPipelineSettings {
  const void *pNext {nullptr};
  VkPipelineCreateFlags flags {0};
  std::uint32_t stageCount {2};
  const VkPipelineShaderStageCreateInfo *pStages {nullptr};
  const VkPipelineVertexInputStateCreateInfo *pVetexInputState {nullptr};
  const VkPipelineInputAssemblyStateCreateInfo *pInputAssemblyState {nullptr};
  const VkPipelineTessellationStateCreateInfo *pTessellationState {nullptr};
  const VkPipelineViewportStateCreateInfo *pViewportState {nullptr};
  const VkPipelineRasterizationStateCreateInfo *pRasterizationState {nullptr};
  const VkPipelineMultisampleStateCreateInfo *pMultisampleState {nullptr};
  const VkPipelineDepthStencilStateCreateInfo *pDepthStencilState {nullptr};
  const VkPipelineColorBlendStateCreateInfo *pColorBlendState {nullptr};
  const VkPipelineDynamicStateCreateInfo *pDynamicState {nullptr};
  VkPipelineLayout layout {};
  VkRenderPass renderPass {};
  uint32_t subpass {0};
  VkPipeline basePipelineHandle {};
  int32_t basePipelineIndex {0};
};
auto graphics_pipeline_create_info(const GraphicsPipelineSettings& gPSettings = {}) noexcept
    -> VkGraphicsPipelineCreateInfo;

struct CreateGraphicsPipelineSettings {
  VkDevice device{};
  VkPipelineCache pipelineCache{};
  uint32_t createInfoCount{1};
  const VkGraphicsPipelineCreateInfo *pCreateInfos{nullptr};
  const VkAllocationCallbacks *pAllocator{nullptr};
  VkPipeline *pPipeline{nullptr};
};
[[nodiscard]] auto create_graphics_pipelines(
    const CreateGraphicsPipelineSettings &graphicsPipelineSettings = {})
    -> VkResult;
} // namespace Vulkan::Pipeline
