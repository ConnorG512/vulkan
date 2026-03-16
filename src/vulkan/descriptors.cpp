#include "vulkan/descriptors.hpp"
#include "vulkan/error-handler.hpp"
#include <cstdint>
#include <stdexcept>
#include <vulkan/vulkan_core.h>

auto Vulkan::DescriptorLayoutBuilder::add_binding(std::uint32_t binding,
                                                  VkDescriptorType type)
    -> void {
  const VkDescriptorSetLayoutBinding newBinding{
      .binding = binding,
      .descriptorType = type,
      .descriptorCount = 1,
  };

  bindings_.push_back(std::move(newBinding));
}

auto Vulkan::DescriptorLayoutBuilder::clear() -> void { bindings_.clear(); }
auto Vulkan::DescriptorLayoutBuilder::build(
    VkDevice device, VkShaderStageFlags shaderStages, void *pNext,
    VkDescriptorSetLayoutCreateFlags flags) -> VkDescriptorSetLayout {

  for (auto &binding : bindings_)
    binding.stageFlags |= shaderStages;

  VkDescriptorSetLayoutCreateInfo info{
      .pNext = {},
      .flags = flags,
      .bindingCount = static_cast<uint32_t>(bindings_.size()),
      .pBindings = bindings_.data(),
  };

  VkDescriptorSetLayout set{};
  if (const auto vk_res = Vulkan::Error::vk_check(
          vkCreateDescriptorSetLayout(device, &info, nullptr, &set));
      !vk_res.has_value())
    throw std::runtime_error(vk_res.error());
  else
    return set;
}

auto Vulkan::DescriptorAllocator::init_pool(VkDevice device, std::uint32_t maxSets, std::span<PoolSizeRatio> poolRatios) noexcept -> void
{
  std::vector<VkDescriptorPoolSize> poolSizes{};
  
  for(const auto ratio : poolRatios)
  {
    poolSizes.push_back(VkDescriptorPoolSize{
        .type = ratio.type,
        .descriptorCount = static_cast<uint32_t>(ratio.ratio * maxSets),
        });
  }

  VkDescriptorPoolCreateInfo poolInfo{
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
    .flags = 0,
    .maxSets = maxSets,
    .poolSizeCount = static_cast<uint32_t>(poolSizes.size()),
    .pPoolSizes = poolSizes.data(),
  };

  vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool);
}

auto Vulkan::DescriptorAllocator::clear_descriptors(VkDevice device) noexcept -> void
{
  vkResetDescriptorPool(device, pool, 0); 
}

auto Vulkan::DescriptorAllocator::destroy_pool(VkDevice device) noexcept -> void
{
  vkDestroyDescriptorPool(device, pool, nullptr);
}

auto Vulkan::DescriptorAllocator::allocate(VkDevice device, VkDescriptorSetLayout layout) const -> VkDescriptorSet
{
  VkDescriptorSetAllocateInfo allocInfo {
    .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
    .pNext = nullptr,
    .descriptorPool = pool,
    .descriptorSetCount = 1,
    .pSetLayouts = &layout,
  };

  VkDescriptorSet ds{};
  if(const auto vk_res = Vulkan::Error::vk_check(vkAllocateDescriptorSets(device, &allocInfo, &ds)); !vk_res.has_value())
    throw std::runtime_error(vk_res.error());
  else
    return ds;
}

