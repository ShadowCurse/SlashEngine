cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER clang)
set(CMAKE_CXX_COMPILER clang++)

project(SlashEngine CXX)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

include(cmake/StandardProjectSettings.cmake)
# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

# sanitizer options if supported by compiler
#include(cmake/Sanitizers.cmake)
#enable_sanitizers(project_options)

# Link this 'library' to use the warnings specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# allow for static analysis options
include(cmake/StaticAnalyzers.cmake)

target_precompile_headers(project_options INTERFACE slash_pch.hpp)

add_library(SlashEngine STATIC
        src/Core/modules.hpp
        src/Core/events.hpp
        src/Core/systems.hpp
        src/Core/resource.hpp

        src/GameResources/transform.hpp
        src/GameResources/mesh.hpp
        src/GameResources/texture.hpp

        src/WindowModule/window.cpp

        src/Ecs/ecs.hpp
        src/Ecs/entity.hpp
        src/Ecs/component.hpp

        #        src/Renderer/imgui_layer.cpp

        #        src/VulkanRenderer/vulkan_imgui.cpp

        src/VulkanRenderer/vulkan_core.cpp
        src/VulkanRenderer/vulkan_queue.cpp
        src/VulkanRenderer/vulkan_queue.hpp
        src/VulkanRenderer/vulkan_command_pool.cpp
        src/VulkanRenderer/vulkan_command_pool.hpp
        src/VulkanRenderer/vulkan_command_buffer.cpp
        src/VulkanRenderer/vulkan_command_buffer.hpp
        src/VulkanRenderer/vulkan_swap_chain.cpp
        src/VulkanRenderer/vulkan_swap_chain.hpp
        src/VulkanRenderer/vulkan_renderer.cpp
        src/VulkanRenderer/vulkan_renderer.hpp
        src/VulkanRenderer/vulkan_renderpass.cpp
        src/VulkanRenderer/vulkan_renderpass.hpp
        src/VulkanRenderer/vulkan_pipeline.cpp
        src/VulkanRenderer/vulkan_pipeline.hpp
        src/VulkanRenderer/vulkan_shader.cpp
        src/VulkanRenderer/vulkan_shader.hpp
        src/VulkanRenderer/vulkan_descriptor_set_layout.cpp
        src/VulkanRenderer/vulkan_descriptor_set_layout.hpp
        src/VulkanRenderer/vulkan_descriptor_pool.cpp
        src/VulkanRenderer/vulkan_descriptor_pool.hpp
        src/VulkanRenderer/vulkan_semaphore.cpp
        src/VulkanRenderer/vulkan_semaphore.hpp
        src/VulkanRenderer/vulkan_fence.cpp
        src/VulkanRenderer/vulkan_fence.hpp
        src/VulkanRenderer/vulkan_descriptor_manager.cpp
        src/VulkanRenderer/vulkan_descriptor_manager.hpp
        src/VulkanRenderer/vulkan_discriptor_set.cpp
        src/VulkanRenderer/vulkan_discriptor_set.hpp

        src/VulkanResources/vulkan_buffer.cpp
        src/VulkanResources/vulkan_buffer.hpp
        src/VulkanResources/vulkan_vertex_buffer.cpp
        src/VulkanResources/vulkan_vertex_buffer.hpp
        src/VulkanResources/vulkan_index_buffer.cpp
        src/VulkanResources/vulkan_index_buffer.hpp
        src/VulkanRenderer/vulkan_command_buffer_manager.cpp
        src/VulkanRenderer/vulkan_command_buffer_manager.hpp
        src/VulkanResources/vulkan_resource_manager.cpp
        src/VulkanResources/vulkan_resource_manager.hpp
        src/VulkanResources/vulkan_texture.cpp
        src/VulkanResources/vulkan_texture.hpp
        src/VulkanResources/vulkan_renderable_object.hpp
        src/VulkanResources/vulkan_mesh.hpp
        src/VulkanResources/vulkan_frame_buffer.cpp
        src/VulkanResources/vulkan_frame_buffer.hpp

        src/RendererModule/module.hpp

        src/Scene/camera.cpp
        src/Scene/camera.hpp

        src/WindowModule/module.hpp

        src/LayerModule/module.hpp
        src/WindowModule/window_manager.cpp
        src/WindowModule/window_manager.hpp
        src/WindowModule/events.hpp
        )

target_include_directories(SlashEngine PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_include_directories(SlashEngine PUBLIC ${CMAKE_SOURCE_DIR})

add_definitions(-DGLFW_INCLUDE_VULKAN -DGLFW_INCLUDE_NONE -DSL_LINUX -DSL_BUILD_DLL -DSL_DEBUG -DSL_ENABLE_ASSERTS -DSL_RENDERER_VALIDATION)

set_target_properties(SlashEngine PROPERTIES PUBLIC_HEADER Slash.hpp)
#target_link_libraries(SlashEngine PRIVATE project_options vulkan glfw3 spdlog glm_static X11 dl)
target_link_libraries(SlashEngine PRIVATE ${CONAN_LIBS} project_options vulkan)

#TestApp
add_executable(TestApp Test/TestApp.cpp)
target_include_directories(TestApp PRIVATE ${CMAKE_SOURCE_DIR})
target_include_directories(TestApp PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_directories(TestApp PRIVATE ${CMAKE_SOURCE_DIR}/lib/build)

set_target_properties(TestApp PROPERTIES CXX_STANDARD 20)
target_link_libraries(TestApp PRIVATE SlashEngine)
