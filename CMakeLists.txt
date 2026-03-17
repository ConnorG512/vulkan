cmake_minimum_required(VERSION 3.28)
project(Vulkan CXX)

set(EXECUTABLE_NAME vulkan-app)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)
find_package(vk-bootstrap REQUIRED)
find_package(imgui REQUIRED)
find_package(VulkanMemoryAllocator REQUIRED)

add_executable(${EXECUTABLE_NAME} 
  src/main.cpp
  src/vma.cpp
  src/window.cpp
  
  src/vulkan/deletion-queue.cpp
  src/vulkan/descriptors.cpp
  src/vulkan/engine.cpp
  src/vulkan/error-handler.cpp
  src/vulkan/initializer.cpp
  src/vulkan/util.cpp

  third-party/imgui/imgui.cpp
  third-party/imgui/imgui_demo.cpp
  third-party/imgui/imgui_draw.cpp
  third-party/imgui/imgui_tables.cpp
  third-party/imgui/imgui_widgets.cpp
  third-party/imgui/imgui_impl_sdl3.cpp
  third-party/imgui/imgui_impl_vulkan.cpp
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
  # Libraries:
  Vulkan::Vulkan
  GPUOpen::VulkanMemoryAllocator
  SDL3::SDL3
  vk-bootstrap::vk-bootstrap
  imgui::imgui
)

target_include_directories(${EXECUTABLE_NAME} PRIVATE 
  "${CMAKE_SOURCE_DIR}/include"
  "${CMAKE_SOURCE_DIR}/third-party"
)

install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)
