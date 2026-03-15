#include "vulkan/deletion-queue.hpp"
#include <algorithm>

auto Vulkan::DeletionQueue::push_function(std::function<void()>&& func) -> void
{
  deletors_.push_back(func);
}

auto Vulkan::DeletionQueue::flush() -> void
{
  std::ranges::for_each(deletors_, [](auto&& del_func){del_func();});
  deletors_.clear();
}
