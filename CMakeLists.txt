cmake_minimum_required(VERSION 3.28)
project(Vulkan CXX)

set(EXECUTABLE_NAME vulkan)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${EXECUTABLE_NAME} 
  src/main.cpp
  src/vulkan/engine.cpp
)

find_package(Vulkan REQUIRED)
find_package(SDL3 REQUIRED)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
  # Libraries:
  Vulkan::Vulkan
  SDL3::SDL3
)

target_include_directories(${EXECUTABLE_NAME} PRIVATE 
  "${CMAKE_SOURCE_DIR}/include"
)

install(TARGETS ${EXECUTABLE_NAME} RUNTIME DESTINATION bin)
