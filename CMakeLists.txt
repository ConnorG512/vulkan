cmake_minimum_required(VERSION 3.28)
project(Vulkan CXX)

set(EXECUTABLE_NAME vulkan-app)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)
find_package(vk-bootstrap REQUIRED)

add_executable(${EXECUTABLE_NAME} 
  src/main.cpp
  src/vulkan/engine.cpp
  src/vulkan/initializer.cpp
  src/vulkan/util.cpp
  src/window.cpp
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
  # Libraries:
  Vulkan::Vulkan
  SDL3::SDL3
  vk-bootstrap::vk-bootstrap
)

target_include_directories(${EXECUTABLE_NAME} PRIVATE 
  "${CMAKE_SOURCE_DIR}/include"
)

install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)
