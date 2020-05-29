cmake_minimum_required(VERSION 3.15)

project(SlashEngine)

include(cmake/StandardProjectSettings.cmake)
# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_17)

# sanitizer options if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

target_precompile_headers(project_options INTERFACE slash_pch.hpp)

add_subdirectory(vendor/spdlog)
target_compile_options(spdlog PRIVATE -fPIC)
add_subdirectory(vendor/glfw3)

add_library(SlashEngine SHARED
        src/Core/application.cpp
        src/Core/log.cpp
        src/Core/layer_stack.cpp
        src/GameResources/resource_manager.cpp
        src/GameResources/model_manager.cpp
        src/GameResources/texture_manager.cpp
        src/GameResources/mesh_manager.cpp
        src/Platform/Linux/linux_input.cpp
        src/Platform/Linux/linux_window.cpp
        src/Renderer/renderer.cpp
        src/VulkanRenderer/vulkan_rendererAPI.cpp

        vendor/imgui/imgui.cpp
        vendor/imgui/imgui_demo.cpp
        vendor/imgui/imgui_draw.cpp
        vendor/imgui/imgui_widgets.cpp
        vendor/imgui/examples/imgui_impl_vulkan.cpp
        vendor/imgui/examples/imgui_impl_glfw.cpp)

target_include_directories(SlashEngine PRIVATE ${CMAKE_SOURCE_DIR}/vendor/spdlog/include)
target_include_directories(SlashEngine PRIVATE ${CMAKE_SOURCE_DIR}/vendor/glfw3/include)
target_include_directories(SlashEngine PRIVATE ${CMAKE_SOURCE_DIR}/vendor/vulkan/x86_64/include)
target_include_directories(SlashEngine PRIVATE ${CMAKE_SOURCE_DIR}/vendor/stb)
target_include_directories(SlashEngine PRIVATE ${CMAKE_SOURCE_DIR}/vendor/glm)
target_include_directories(SlashEngine PRIVATE ${CMAKE_SOURCE_DIR}/vendor/imgui/examples)
target_include_directories(SlashEngine PRIVATE ${CMAKE_SOURCE_DIR}/vendor/imgui)
target_include_directories(SlashEngine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(SlashEngine PRIVATE ${CMAKE_SOURCE_DIR})

target_link_directories(SlashEngine PRIVATE ${CMAKE_SOURCE_DIR}/vendor/vulkan/x86_64/lib)
target_link_directories(SlashEngine PRIVATE ${CMAKE_BINARY_DIR}/vendor/glfw3/src)
target_link_directories(SlashEngine PRIVATE ${CMAKE_BINARY_DIR}/vendor/spdlog)

add_definitions(-DGLFW_INCLUDE_VULKAN -DGLFW_INCLUDE_NONE -DSL_LINUX -DSL_BUILD_DLL -DSL_DEBUG -DSL_ENABLE_ASSERTS)

set_target_properties(SlashEngine PROPERTIES PUBLIC_HEADER Slash.hpp)
target_link_libraries(SlashEngine PRIVATE project_options vulkan glfw3 spdlog X11 dl)