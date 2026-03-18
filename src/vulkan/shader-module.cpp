#include "file-util.hpp"
#include "vulkan/shader-module.hpp"

#include <cassert>
#include <cstdint>
#include <fstream>
#include <print>
#include <vector>
#include <vulkan/vulkan_core.h>

Vulkan::ShaderModule::ShaderModule(const std::string &path, VkDevice device)
    : device_{device}
    , shader_module_{load_shader(path)} {}

Vulkan::ShaderModule::~ShaderModule() 
{
  vkDestroyShaderModule(device_, shader_module_, nullptr);
}


auto Vulkan::ShaderModule::load_shader(const std::string &path)
    -> VkShaderModule {
  assert(!path.empty());

  std::ifstream file(path, std::ios::ate | std::ios::binary);
  if (!file.is_open()) {
    std::println("failed to open file!");
    return {};
  }

  const std::size_t file_size = Util::get_file_size_from_end(file);

  std::vector<std::uint32_t> buffer(file_size / sizeof(std::uint32_t));

  file.seekg(0);
  file.read(reinterpret_cast<char *>(buffer.data()), file_size);

  VkShaderModuleCreateInfo createInfo{
      .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
      .pNext = nullptr,
      .codeSize = buffer.size() * sizeof(std::uint32_t),
      .pCode = buffer.data(),
  };

  VkShaderModule shaderModule{};
  vkCreateShaderModule(device_, &createInfo, nullptr, &shaderModule);

  return std::move(shader_module_);
}
