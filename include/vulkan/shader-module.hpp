#include <string>
#include <vulkan/vulkan_core.h>

namespace Vulkan {
class ShaderModule {
public:
  ShaderModule(const std::string &path, VkDevice device);
  ~ShaderModule();

private:
  VkDevice device_{};
  VkShaderModule shader_module_{};

  auto load_shader(const std::string &path) -> VkShaderModule;
};
} // namespace Vulkan
