#pragma once

#include <cstdint>
#include <span>
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

class DescriptorAllocator 
{
  public: 
    struct PoolSizeRatio {
      VkDescriptorType type {};
      float ratio {};
    };
    
    auto init_pool(VkDevice device, std::uint32_t maxSets, std::span<PoolSizeRatio> poolRatios) noexcept -> void;
    auto clear_descriptors(VkDevice device) noexcept -> void;
    auto destroy_pool(VkDevice device) noexcept -> void;
    auto allocate(VkDevice device, VkDescriptorSetLayout layout) const -> VkDescriptorSet;

  private:
    VkDescriptorPool pool{};
};
} // namespace Vulkan
