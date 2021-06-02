#ifndef SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_HPP_

#include "Core/core.hpp"
#include "Core/log.hpp"
#include "stb_image.h"

namespace slash {

struct Slash_API Texture {
  static auto Load(const std::string &texture_path) -> Texture;

 public:
  std::vector<stbi_uc> pixels_;
  uint32_t width_;
  uint32_t height_;
  uint32_t channel_;
};

}

#endif // SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_HPP_
