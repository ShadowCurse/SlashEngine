#ifndef SLASHENGINE_SRC_RESOURCEMANAGER_RESOURCE_MANAGER_HPP_
#define SLASHENGINE_SRC_RESOURCEMANAGER_RESOURCE_MANAGER_HPP_

#include "Core/core.hpp"
#include "model_manager.hpp"
#include "texture_manager.hpp"

namespace slash {

class Slash_API ResourceManager {
 public:
  static void Init();
  static void Destroy();

  static void AddModel(Model model);
  static bool LoadModel(const std::string &model_name, const std::string &model_path);
  static void RemoveModel(const std::string &model_name);
  static void BindModel(const std::string &model_name);
  static void UnBindModel(const std::string &model_name);

  static bool LoadTexture(const std::string &texture_name, const std::string &texture_path);
  static void DeleteTexture(const std::string &texture_name);
  static void BindTexture(const std::string &texture_name);
  static void UnBindTexture(const std::string &texture_name);

  static void BindObject(const std::string &model_name, const std::string &texture_name);

 private:
  ResourceManager() = default;
  ~ResourceManager() = default;

  ModelManager *model_manager_;
  TextureManager *texture_manager_;

  inline static ResourceManager *instance_ = nullptr;
};

} // namespace slash

#endif // SLASHENGINE_SRC_RESOURCEMANAGER_RESOURCE_MANAGER_HPP_
