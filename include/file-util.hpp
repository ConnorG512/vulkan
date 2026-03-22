#pragma once

#include <cstdlib>
#include <filesystem>
#include <format>
#include <fstream>
#include <string>
namespace Util {
inline constexpr auto get_file_size_from_end = [](std::ifstream &file) {
  return file.tellg();
};

inline constexpr auto get_shader_file_path =
    [](const char *shader_name) -> std::string {
  const char *shader_path{std::getenv("SHADER_PATH")};
  return (shader_path != nullptr)
             ? std::format("{}/{}", shader_path, shader_name)
             : std::string{
                   std::format("{}/shaders/{}",
                               std::filesystem::canonical("/proc/self/exe")
                                   .parent_path()
                                   .c_str(),
                               shader_name)};
};
} // namespace Util
