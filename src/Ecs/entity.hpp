#ifndef SLASHENGINE_SRC_ECS_ENTITY_HPP_
#define SLASHENGINE_SRC_ECS_ENTITY_HPP_

#include "slash_pch.hpp"

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
  auto create_entity() -> Entity {
    auto e = availabe_entities_.front();
    availabe_entities_.pop();
    ++living_entity_count_;
    return e;
  }
  void destroy_entity(Entity entity) {
    signatures_[entity].reset();
    availabe_entities_.push(entity);
    --living_entity_count_;
  }
//  void SetSignature(Entity entity, Signature signature) {
//    signatures_[entity] = signature;
//  }
  auto get_signature(Entity entity) -> Signature & {
    return signatures_[entity];
  }

  auto get_entities_with_components(const std::vector<ComponenetType> &component_types) -> std::vector<Entity> {
    std::vector<Entity> ret;
    for (const auto &signature: signatures_) {
      bool has_components{true};
      for (const auto type: component_types)
        if (!signature.test(type)) {
          has_components = false;
          break;
        }
      if (has_components)
        ret.emplace_back(&signature - &signatures_[0]);
    }
    return ret;
  }

 private:
  std::queue<Entity> availabe_entities_;
  std::array<Signature, MAX_ENTITIES> signatures_;
  uint32_t living_entity_count_{};
};

}
#endif //SLASHENGINE_SRC_ECS_ENTITY_HPP_
