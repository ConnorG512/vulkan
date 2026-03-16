#pragma once 

#include <deque>
#include <functional>
namespace Vulkan 
{
  class DeletionQueue 
  {
    private:
      std::deque<std::function<void()>> deletors_ {};

    public:
      auto push_function(std::function<void()>&& func) -> void;
      auto flush() -> void;
  };
}
