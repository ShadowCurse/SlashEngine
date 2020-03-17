#ifndef SLASHENGINE_SRC_RESOURCEMANAGER_RESOURCE_MANAGER_HPP_
#define SLASHENGINE_SRC_RESOURCEMANAGER_RESOURCE_MANAGER_HPP_

#include "Core/core.hpp"
#include "model_manager.hpp"

namespace slash {

class Slash_API ResourceManager {
public:
  ResourceManager() = default;
  ~ResourceManager() = default;

  // init's all managers
  static void Init();
  static void Destroy();
  static ModelManager *GetModelManager();

private:
  ModelManager *model_manager_;

  inline static ResourceManager *instance_ = nullptr;
};

} // namespace slash

#endif // SLASHENGINE_SRC_RESOURCEMANAGER_RESOURCE_MANAGER_HPP_
