#ifndef SLASHENGINE_SRC_GAMERESOURCES_RESOURCE_MANAGER_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_RESOURCE_MANAGER_HPP_

#include "Core/core.hpp"
#include <VulkanResources/vulkan_renderable_object.hpp>

namespace slash {

class Slash_API ResourceManager {
public:
  static void Init();
  static void Destroy();
};

} // namespace slash

#endif // SLASHENGINE_SRC_RESOURCEMANAGER_RESOURCE_MANAGER_HPP_
