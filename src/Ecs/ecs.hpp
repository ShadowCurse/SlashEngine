#ifndef SLASHENGINE_SRC_ECS_ECS_HPP_
#define SLASHENGINE_SRC_ECS_ECS_HPP_

#include "Core/modules.hpp"
#include "entity.hpp"
#include "component.hpp"

namespace slash {

struct PackInserter {
  PackInserter(Entity e, EntityManager& em, ComponenetManager& cm) : e_{e}, em_{em}, cm_{cm} {}
  template<typename T>
    auto insert(T &&t) {
      auto &signature = em_.get_signature(e_);
      cm_.add_component<T>(e_, std::forward<T>(t));
      signature.set(cm_.get_component_type<T>(), true);
    }
  private:
    Entity e_;
    EntityManager& em_;
    ComponenetManager& cm_;
};

template<typename T>
concept IsPack = requires(T t, PackInserter p) {
  {t.build(p)};
};
  
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
    ECSQuery &query_;
    entity_iterator iterator_;
  };

 public:
  explicit ECSQuery(std::vector<Entity> entities, std::shared_ptr<ComponenetArray<Components>> ... components)
      : Container<Components>(components) ..., entities_(std::move(entities)) {}

  auto begin() -> Iterator {
    return Iterator{*this, std::begin(entities_)};
  }
  auto end() -> Iterator {
    return Iterator{*this, std::end(entities_)};
  }
  [[nodiscard]] auto size() const -> size_t {
    return entities_.size();
  }

 private:
  template<typename C>
  auto get_component_array() const -> ComponenetArray<C> & {
    return *Container<C>::component_array;
  }

  std::vector<Entity> entities_;
};

class ECSModule : public Dependencies<> {
 public:
  ECSModule() = default;
  auto init() -> bool {
    entity_manager_ = std::make_unique<EntityManager>();
    component_manager_ = std::make_unique<ComponenetManager>();
    return true;
  }
  auto destroy() -> bool {
    return true;
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
  void unregister_component() {
    component_manager_->unregister_component<T>();
  }

  template<typename T>
  void add_component(Entity entity, T &&component) {
    component_manager_->add_component<T>(entity, std::forward<T>(component));

    auto &signature = entity_manager_->get_signature(entity);
    signature.set(component_manager_->get_component_type<T>(), true);
  }

  template<IsPack P>
  void add_pack(P &&pack) {
    auto e = create_entity();
    auto inserter = PackInserter { e, *entity_manager_.get(), *component_manager_.get() };
    pack.build(inserter);
  }

  template<typename T>
  void remove_component(Entity entity) {
    component_manager_->register_component<T>(entity);

    auto &signature = entity_manager_->get_signature(entity);
    signature.set(component_manager_->get_component_type<T>(), false);
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
    const auto &component_types = std::vector{component_manager_->get_component_type<Components>() ...};
    auto entities = entity_manager_->get_entities_with_components(component_types);
    return ECSQuery(entities, get_component_array<Components>() ...);
  }

 private:
  std::unique_ptr<EntityManager> entity_manager_;
  std::unique_ptr<ComponenetManager> component_manager_;
};

}

#endif //SLASHENGINE_SRC_ECS_ECS_HPP_
