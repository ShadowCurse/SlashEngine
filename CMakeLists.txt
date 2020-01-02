cmake_minimum_required(VERSION 3.10.2)

project(SlashEngine)

include_directories(${CMAKE_SOURCE_DIR}/vendor/spdlog/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor/glfw3/include)
include_directories(${CMAKE_SOURCE_DIR}/vendor/vulkan/x86_64/include)
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR})

link_directories(${CMAKE_SOURCE_DIR}/vendor/vulkan/x86_64/lib)
link_directories(${CMAKE_SOURCE_DIR}/vendor/glfw3/build/src)

add_library(SlashEngine SHARED 
    src/Core/Application.cpp
    src/Core/Log.cpp
    src/Core/LayerStack.cpp

    src/Platform/Linux/LinuxInput.cpp
    src/Platform/Linux/LinuxWindow.cpp

    src/Renderer/Renderer.cpp

    src/VulkanRenderer/VulkanRendererAPI.cpp

    SlashPCH.cpp
)

add_definitions(-DGLFW_INCLUDE_VULKAN -DGLFW_INCLUDE_NONE -DSL_LINUX -DSL_BUILD_DLL -DSL_DEBUG -DSL_ENABLE_ASSERTS)

set_target_properties(SlashEngine PROPERTIES CXX_STANDARD 17 PUBLIC_HEADER Slash.hpp)
TARGET_LINK_LIBRARIES(SlashEngine vulkan)
TARGET_LINK_LIBRARIES(SlashEngine glfw3)
TARGET_LINK_LIBRARIES(SlashEngine dl)
TARGET_LINK_LIBRARIES(SlashEngine X11)
TARGET_LINK_LIBRARIES(SlashEngine pthread)