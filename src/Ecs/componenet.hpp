#ifndef SLASHENGINE_SRC_ECS_COMPONENET_HPP_
#define SLASHENGINE_SRC_ECS_COMPONENET_HPP_

#include "Core/core.hpp"
#include "entity.hpp"

namespace slash {

class BasicComponenetArray {
 public:
  virtual ~BasicComponenetArray() = default;
  virtual void EntityDestroyed(Entity entity) = 0;
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

  void InsertData(Entity entity, T componet) {
    size_t new_index = size_;
    entity_to_index_[entity] = new_index;
    index_to_entity_[new_index] = entity;
    components_[new_index] = std::move(componet);
    ++size_;
  }

  void RemoveData(Entity entity) {
    // copy element at the end into deleted element place
    auto index_of_remove_entity = entity_to_index_[entity];
    auto index_of_lat_element = size_ - 1;
    components_[index_of_remove_entity] = components_[index_of_lat_element];

    // update map
    auto entity_of_last_ = index_to_entity_[index_of_lat_element];
    entity_to_index_[entity_of_last_] = index_of_remove_entity;
    index_to_entity_[index_of_remove_entity] = entity_of_last_;

    entity_to_index_.erase(entity);
    index_to_entity_.erase(index_of_lat_element);

    --size_;
  }

  auto GetData(Entity entity) -> T & {
    return components_[entity_to_index_[entity]];
  }

  void EntityDestroyed(Entity entity) final {
    if (entity_to_index_.find(entity) != entity_to_index_.end())
      RemoveData(entity);
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
  void RegisterComponent() {
    const char *type_name = typeid(T).name();
    component_types_.insert({type_name, next_component_type_});
    component_arrays_.insert({type_name, std::make_shared<ComponenetArray<T>>()});
    ++next_component_type_;
  }

  template<typename T>
  auto GetCompponentType() -> ComponenetType {
    const char *type_name = typeid(T).name();
    return component_types_[type_name];
  }

  template<typename T>
  void AddComponent(Entity entity, T component) {
    GetComponentArray<T>()->InsertData(entity, component);
  }

  template<typename T>
  void RemoveComponent(Entity entity) {
    GetComponentArray<T>()->RemoveData(entity);
  }

  template<typename T>
  auto GetComponent(Entity entity) -> T & {
    return GetComponentArray<T>()->GetData(entity);
  }

  template<typename T>
  auto GetComponentArray() -> std::shared_ptr<ComponenetArray<T>> {
    const char *type_name = typeid(T).name();
    return std::static_pointer_cast<ComponenetArray<T>>(component_arrays_[type_name]);
  }

  void EntityDestroyed(Entity entity) {
    for (auto &pair : component_arrays_)
      pair.second->EntityDestroyed(entity);
  }
 private:
  std::unordered_map<const char *, ComponenetType> component_types_;
  std::unordered_map<const char *, std::shared_ptr<BasicComponenetArray>> component_arrays_;

  ComponenetType next_component_type_;
};

}

#endif //SLASHENGINE_SRC_ECS_COMPONENET_HPP_
