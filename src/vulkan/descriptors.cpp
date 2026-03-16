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
