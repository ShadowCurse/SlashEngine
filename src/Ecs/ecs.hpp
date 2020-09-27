#ifndef SLASHENGINE_SRC_ECS_ECS_HPP_
#define SLASHENGINE_SRC_ECS_ECS_HPP_

#include "Core/core.hpp"
#include "entity.hpp"
#include "componenet.hpp"

namespace slash {

class ECS {
 public:
  ECS() {
    entity_manager_ = std::make_unique<EntityManager>();
    component_manager_ = std::make_unique<ComponenetManager>();
  }

  auto CreateEntity() -> Entity {
    return entity_manager_->CreateEntity();
  }

  void DestroyEntity(Entity entity) {
    entity_manager_->DestroyEntity(entity);
    component_manager_->EntityDestroyed(entity);
  }

  template<typename T>
  void RegisterComponent() {
    component_manager_->RegisterComponent<T>();
  }

  template<typename T>
  void AddComponent(Entity entity, T component) {
    component_manager_->AddComponent<T>(entity, component);

    auto signature = entity_manager_->GetSignature(entity);
    signature.set(component_manager_->GetCompponentType<T>(), true);
    entity_manager_->SetSignature(entity, signature);
  }

  template<typename T>
  void RemoveComponent(Entity entity) {
    component_manager_->RegisterComponent<T>(entity);

    auto signature = entity_manager_->GetSignature(entity);
    signature.set(component_manager_->GetCompponentType<T>(), false);
    entity_manager_->SetSignature(entity, signature);
  }

  template<typename T>
  auto GetComponent(Entity entity) -> T & {
    return component_manager_->GetComponent<T>(entity);
  }

  template<typename T>
  auto GetComponentArray() -> std::shared_ptr<ComponenetArray < T>>
  {
    return component_manager_->GetComponentArray<T>();
  }

 private:
  std::unique_ptr<EntityManager> entity_manager_;
  std::unique_ptr<ComponenetManager> component_manager_;
};

}

#endif //SLASHENGINE_SRC_ECS_ECS_HPP_
