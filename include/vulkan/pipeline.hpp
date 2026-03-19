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
auto rendering_create_info(VkFormat *colorFormat, VkFormat depthStencilFormat,
                           std::uint32_t colorAttachmentCount = 1) noexcept
    -> VkPipelineRenderingCreateInfo;
} // namespace Vulkan::Pipeline
