#pragma once

#include <fstream>
namespace Util {
inline constexpr auto get_file_size_from_end = [](std::ifstream &file) -> auto {
  return file.tellg();
};
} // namespace Util
