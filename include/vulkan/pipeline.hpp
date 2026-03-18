#include <cstdint>
#include <vulkan/vulkan_core.h>
#pragma one

namespace Vulkan::Pipeline {
auto create_compute_pipeline(VkShaderModule computeShader,
                             VkPipelineLayout pipelineLayout,
                             const char *pName = "main") noexcept
    -> VkComputePipelineCreateInfo;

auto create_pipeline_layout_info(const VkDescriptorSetLayout &setLayout,
                                 std::uint32_t setLayoutCount = 1) noexcept
    -> VkPipelineLayoutCreateInfo;
} // namespace Vulkan::Pipeline
