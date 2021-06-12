#ifndef SLASHENGINE_SRC_ECS_COMPONENET_HPP_
#define SLASHENGINE_SRC_ECS_COMPONENET_HPP_

#include "Core/core.hpp"
#include "entity.hpp"

namespace slash {

class BasicComponenetArray {
 public:
  virtual ~BasicComponenetArray() = default;
  virtual void entity_destroyed(Entity entity) = 0;
};

template<typename T>
class ComponenetArray final : public BasicComponenetArray {
 public:
  class Iterator {
   public:
    explicit Iterator(ComponenetArray<T> *array, size_t position)
        : array_(array), position_(position) {}
    T &operator*() const { return array_->components_[position_]; }
    bool operator==(const Iterator &other) { return position_ == other.position_; }
    Iterator &operator++() {
      ++position_;
      return *this;
    }
   private:
    ComponenetArray<T> *array_;
    size_t position_;
  };

  friend class Iterator;

  auto begin() -> Iterator {
    return Iterator(this, 0);
  }

  auto end() -> Iterator {
    return Iterator(this, size_);
  }

  void insert_data(Entity entity, T &&component) {
    size_t new_index = size_;
    entity_to_index_[entity] = new_index;
    index_to_entity_[new_index] = entity;
    components_[new_index] = std::move(component);
    ++size_;
  }

  void remove_data(Entity entity) {
    // copy element at the end into deleted element place
    auto index_of_remove_entity = entity_to_index_[entity];
    auto index_of_lat_element = size_ - 1;
    components_[index_of_remove_entity] = std::move(components_[index_of_lat_element]);

    // update map
    auto entity_of_last_ = index_to_entity_[index_of_lat_element];
    entity_to_index_[entity_of_last_] = index_of_remove_entity;
    index_to_entity_[index_of_remove_entity] = entity_of_last_;

    entity_to_index_.erase(entity);
    index_to_entity_.erase(index_of_lat_element);

    --size_;
  }

  auto get_data(Entity entity) -> T & {
    return components_[entity_to_index_[entity]];
  }

  void entity_destroyed(Entity entity) final {
    if (entity_to_index_.find(entity) != entity_to_index_.end())
      remove_data(entity);
  }

 private:
  std::array<T, MAX_ENTITIES> components_;
  std::unordered_map<Entity, size_t> entity_to_index_;
  std::unordered_map<size_t, Entity> index_to_entity_;
  size_t size_{0};
};

class ComponenetManager {
 public:
  template<typename T>
  void register_component() {
    const char *type_name = typeid(T).name();
    component_types_.insert({type_name, next_component_type_});
    component_arrays_.insert({type_name, std::make_shared<ComponenetArray<T>>()});
    ++next_component_type_;
  }

  template<typename T>
  void unregister_component() {
    const char *type_name = typeid(T).name();
    component_types_.erase(type_name);
    component_arrays_.erase(type_name);
  }

  template<typename T>
  auto get_component_type() -> ComponenetType {
    const char *type_name = typeid(T).name();
    return component_types_[type_name];
  }

  template<typename T>
  void add_component(Entity entity, T &&component) {
    get_component_array<T>()->insert_data(entity, std::forward<T>(component));
  }

  template<typename T>
  void remove_component(Entity entity) {
    get_component_array<T>()->remove_data(entity);
  }

  template<typename T>
  auto get_component(Entity entity) -> T & {
    return get_component_array<T>()->get_data(entity);
  }

  template<typename T>
  auto get_component_array() -> std::shared_ptr<ComponenetArray<T>> {
    const char *type_name = typeid(T).name();
    return std::static_pointer_cast<ComponenetArray<T>>(component_arrays_[type_name]);
  }

  void entity_destroyed(Entity entity) {
    for (auto &pair : component_arrays_)
      pair.second->entity_destroyed(entity);
  }
 private:
  std::unordered_map<const char *, ComponenetType> component_types_;
  std::unordered_map<const char *, std::shared_ptr<BasicComponenetArray>> component_arrays_;

  ComponenetType next_component_type_;
};

}

#endif //SLASHENGINE_SRC_ECS_COMPONENET_HPP_
