#ifndef SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_HPP_

#include "Core/core.hpp"
#include "stb_image.h"

class Slash_API Texture {
 public:
  friend class TextureManager;
  Texture(std::vector<stbi_uc> pixels, int width, int height, int channel);
  ~Texture() = default;

 public:
  std::vector<stbi_uc> pixels_;
  int width_;
  int height_;
  int channel_;
};

#endif //SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_HPP_
