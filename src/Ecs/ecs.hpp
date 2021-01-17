#ifndef SLASHENGINE_SRC_ECS_ECS_HPP_
#define SLASHENGINE_SRC_ECS_ECS_HPP_

#include <utility>

#include "Core/core.hpp"
#include "entity.hpp"
#include "componenet.hpp"

namespace slash {

template<typename T>
struct Container {
  explicit Container(auto array) : component_array{array} {}
  std::shared_ptr<ComponenetArray<T>> component_array;
};

template<typename ... Components>
class ECSQuery : public Container<Components> ... {
 public:
  using entity_iterator = std::vector<Entity>::iterator;

  class Iterator {
   public:
    Iterator(ECSQuery &query, entity_iterator iterator) : query_{query}, iterator_{iterator} {}

    friend auto operator==(const Iterator &first, const Iterator &second)
    -> bool {
      return first.iterator_ == second.iterator_;
    }
    friend auto operator!=(const Iterator &first, const Iterator &second)
    -> bool {
      return !(first == second);
    }
    auto operator++() -> Iterator & {
      if (iterator_ != std::end(query_.entities_))
        ++iterator_;
      return *this;
    }
    auto operator*() const -> std::tuple<Components &...> {
      return {query_.get_component_array<Components>().get_data(*iterator_) ...};
    }

   private:
    entity_iterator iterator_;
    ECSQuery &query_;
  };

 public:
  explicit ECSQuery(std::vector<Entity> entities, std::shared_ptr<ComponenetArray<Components>> ... components)
      : entities_(std::move(entities)), Container<Components>(components) ... {}

  auto begin() -> Iterator {
    return Iterator{*this, std::begin(entities_)};
  }
  auto end() -> Iterator {
    return Iterator{*this, std::end(entities_)};
  }

 private:
  template<typename C>
  auto get_component_array() const -> ComponenetArray<C> & {
    return *Container<C>::component_array;
  }

  std::vector<Entity> entities_;
};

class ECS {
 public:
  ECS() {
    entity_manager_ = std::make_unique<EntityManager>();
    component_manager_ = std::make_unique<ComponenetManager>();
  }

  auto create_entity() -> Entity {
    return entity_manager_->create_entity();
  }

  void destroy_entity(Entity entity) {
    entity_manager_->destroy_entity(entity);
    component_manager_->entity_destroyed(entity);
  }

  template<typename T>
  void register_component() {
    component_manager_->register_component<T>();
  }

  template<typename T>
  void add_component(Entity entity, T component) {
    component_manager_->add_component<T>(entity, component);

    auto &signature = entity_manager_->get_signature(entity);
    signature.set(component_manager_->get_compponent_type<T>(), true);
  }

  template<typename T>
  void remove_component(Entity entity) {
    component_manager_->register_component<T>(entity);

    auto &signature = entity_manager_->get_signature(entity);
    signature.set(component_manager_->get_compponent_type<T>(), false);
  }

  template<typename T>
  auto get_component(Entity entity) -> T & {
    return component_manager_->get_component<T>(entity);
  }

  template<typename T>
  auto get_component_array() -> std::shared_ptr<ComponenetArray<T>> {
    return component_manager_->get_component_array<T>();
  }

  template<typename ... Components>
  auto get_query() {
    const auto &component_types = std::vector{component_manager_->get_compponent_type<Components>() ...};
    auto entities = entity_manager_->get_entities_with_components(component_types);
    return ECSQuery(entities, get_component_array<Components>() ...);
  }

 private:
  std::unique_ptr<EntityManager> entity_manager_;
  std::unique_ptr<ComponenetManager> component_manager_;
};

}

#endif //SLASHENGINE_SRC_ECS_ECS_HPP_
