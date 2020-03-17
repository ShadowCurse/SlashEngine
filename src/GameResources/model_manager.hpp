#ifndef SLASHENGINE_SRC_RESOURCEMANAGER_MODEL_MANAGER_HPP_
#define SLASHENGINE_SRC_RESOURCEMANAGER_MODEL_MANAGER_HPP_

#include "Core/core.hpp"
#include "Renderer/vertex.hpp"

namespace slash {

struct Slash_API Model {
  std::string name;
  std::vector<Vertex> vertices;
  std::vector<uint16_t> indices;
  bool binded = false;
};

class Slash_API ModelManager {
public:
  ModelManager() = default;
  ~ModelManager() = default;

  void AddModel(Model model);
  void RemoveModel(const std::string &model_name);
  void BindModel(const std::string &model_name);
  void UnBindModel(const std::string &model_name);

private:
  std::map<size_t, Model> models_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_RESOURCEMANAGER_MODEL_MANAGER_HPP_
