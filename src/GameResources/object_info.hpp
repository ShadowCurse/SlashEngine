#ifndef SLASHENGINE_SRC_GAMERESOURCES_OBJECT_INFO_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_OBJECT_INFO_HPP_

#include "Core/core.hpp"

namespace slash {

struct Slash_API ObjectInfo {
  size_t uid;
  size_t vertex_uid;
  size_t index_uid;
  size_t texture_uid;
};

} // namespace slash

#endif //SLASHENGINE_SRC_GAMERESOURCES_OBJECT_INFO_HPP_
