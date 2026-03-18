#include <vulkan/vulkan_core.h>
#pragma one 

namespace Vulkan::Pipeline 
{
  auto create_compute_pipeline(VkShaderModule computeShader, VkPipelineLayout pipelineLayout, const char *pName = "main") noexcept -> VkComputePipelineCreateInfo;
}
