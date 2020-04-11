#ifndef SLASHENGINE_SRC_RESOURCEMANAGER_MODEL_MANAGER_HPP_
#define SLASHENGINE_SRC_RESOURCEMANAGER_MODEL_MANAGER_HPP_

#include "Core/core.hpp"
#include "Renderer/vertex.hpp"

namespace slash {

class Slash_API Model {
 public:
  friend class ModelManager;
  Model(const std::string &name, const std::vector<Vertex> &verticies, const std::vector<uint16_t> &indices);
  ~Model() = default;

  const std::string &Name() const;
  const std::vector<Vertex> &Vertices() const;
  const std::vector<uint16_t> &Indices() const;

 private:
  std::string name_;
  std::vector<Vertex> vertices_;
  std::vector<uint16_t> indices_;
};

class Slash_API ModelManager {
 public:
  ModelManager() = default;
  ~ModelManager() = default;

  void AddModel(Model model);
  bool LoadModel(const std::string &model_name, const std::string &model_path);
  void RemoveModel(const std::string &model_name);

  [[nodiscard]] size_t GetUid(const std::string &model_name) const;
  [[nodiscard]] const Model &GetModel(const std::string &model_name) const;

 private:
  std::map<size_t, Model> models_;
};

} // namespace slash

#endif // SLASHENGINE_SRC_RESOURCEMANAGER_MODEL_MANAGER_HPP_
