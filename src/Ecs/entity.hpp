#ifndef SLASHENGINE_SRC_ECS_ENTITY_HPP_
#define SLASHENGINE_SRC_ECS_ENTITY_HPP_

#include "Core/core.hpp"

namespace slash {

using Entity = uint32_t;

constexpr Entity MAX_ENTITIES = 5000;

using ComponenetType = uint8_t;

constexpr ComponenetType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;

class EntityManager {
 public:
  EntityManager() {
    for (Entity e(0); e < MAX_ENTITIES; ++e)
      availabe_entities_.push(e);
  }
  auto CreateEntity() -> Entity {
    auto e = availabe_entities_.front();
    availabe_entities_.pop();
    ++living_entity_count_;
    return e;
  }
  void DestroyEntity(Entity entity) {
    signatures_[entity].reset();
    availabe_entities_.push(entity);
    --living_entity_count_;
  }
  void SetSignature(Entity entity, Signature signature) {
    signatures_[entity] = signature;
  }
  auto GetSignature(Entity entity) -> Signature {
    return signatures_[entity];
  }

 private:
  std::queue<Entity> availabe_entities_;
  std::array<Signature, MAX_ENTITIES> signatures_;
  uint32_t living_entity_count_;
};

}
#endif //SLASHENGINE_SRC_ECS_ENTITY_HPP_
