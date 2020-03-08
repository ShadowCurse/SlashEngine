cmake_minimum_required(VERSION 3.15.0)

project(SlashEngine)

include_directories(${CMAKE_SOURCE_DIR}/vendor/spdlog/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor/glfw3/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor/vulkan/x86_64/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor/stb)
include_directories(${CMAKE_SOURCE_DIR}/vendor/glm)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR})

link_directories(${CMAKE_SOURCE_DIR}/vendor/vulkan/x86_64/lib)
link_directories(${CMAKE_SOURCE_DIR}/vendor/glfw3/build/src)

add_library(SlashEngine SHARED
        src/Core/application.cpp
        src/Core/log.cpp
        src/Core/layer_stack.cpp

        src/Platform/Linux/linux_input.cpp
        src/Platform/Linux/linux_window.cpp

        src/Renderer/renderer.cpp

    src/VulkanRenderer/VulkanRendererAPI.cpp

    slash_pch.cpp
)

add_definitions(-DGLFW_INCLUDE_VULKAN -DGLFW_INCLUDE_NONE -DSL_LINUX -DSL_BUILD_DLL -DSL_DEBUG -DSL_ENABLE_ASSERTS)

set_target_properties(SlashEngine PROPERTIES CXX_STANDARD 17  PUBLIC_HEADER Slash.hpp)
target_compile_options(SlashEngine PRIVATE -Wall -Wextra -pedantic -Werror)
TARGET_LINK_LIBRARIES(SlashEngine vulkan)
TARGET_LINK_LIBRARIES(SlashEngine glfw3)
TARGET_LINK_LIBRARIES(SlashEngine dl)
TARGET_LINK_LIBRARIES(SlashEngine X11)
TARGET_LINK_LIBRARIES(SlashEngine pthread)