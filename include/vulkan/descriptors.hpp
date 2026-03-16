#pragma once

#include <cstdint>
#include <vector>
#include <vulkan/vulkan_core.h>
namespace Vulkan {
class DescriptorLayoutBuilder {
public:
  auto add_binding(std::uint32_t binding, VkDescriptorType type) -> void;
  auto clear() -> void;
  auto build(VkDevice device, VkShaderStageFlags shaderStages,
             void *pNext = nullptr, VkDescriptorSetLayoutCreateFlags flags = 0)
      -> VkDescriptorSetLayout;

private:
  std::vector<VkDescriptorSetLayoutBinding> bindings_{};
};
} // namespace Vulkan
