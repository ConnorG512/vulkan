#include "vulkan/engine.hpp"

auto main() -> int 
{
  Vulkan::Engine engine{};
  engine.init();
  engine.draw();
  return 0;
}
