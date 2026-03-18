#include "vulkan/pipeline.hpp"

auto Vulkan::Pipeline::create_compute_pipeline(VkShaderModule computeShader, VkPipelineLayout pipelineLayout, const char *pName) noexcept -> VkComputePipelineCreateInfo
{
  VkPipelineShaderStageCreateInfo stageInfo {
    .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
    .pNext = nullptr,
    .stage = VK_SHADER_STAGE_COMPUTE_BIT,
    .module = computeShader,
    .pName = pName,
  };

  return VkComputePipelineCreateInfo {
    .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
    .pNext = nullptr,
    .stage = stageInfo,
    .layout = pipelineLayout,
  };
}
