cmake_minimum_required(VERSION 3.15)

project(SlashEngine)

include(cmake/StandardProjectSettings.cmake)
# Link this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_20)

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

        src/Renderer/imgui_layer.cpp

#        src/VulkanRenderer/vulkan_rendererAPI.cpp
        src/VulkanRenderer/vulkan_imgui.cpp
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

        src/RendererModule/render_module.cpp
        src/RendererModule/render_module.hpp

        src/Scene/scene_manager.cpp
        src/Scene/scene_manager.hpp

        vendor/imgui/imgui.cpp
        vendor/imgui/imgui_demo.cpp
        vendor/imgui/imgui_draw.cpp
        vendor/imgui/imgui_widgets.cpp
        vendor/imgui/examples/imgui_impl_vulkan.cpp
        vendor/imgui/examples/imgui_impl_glfw.cpp
        src/VulkanResources/vulkan_frame_buffer.cpp src/VulkanResources/vulkan_frame_buffer.hpp)

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

add_definitions(-DGLFW_INCLUDE_VULKAN -DGLFW_INCLUDE_NONE -DSL_LINUX -DSL_BUILD_DLL -DSL_DEBUG -DSL_ENABLE_ASSERTS -DSL_RENDERER_VALIDATION)

set_target_properties(SlashEngine PROPERTIES PUBLIC_HEADER Slash.hpp)
target_link_libraries(SlashEngine PRIVATE project_options vulkan glfw3 spdlog X11 dl)