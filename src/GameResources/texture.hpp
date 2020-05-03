#ifndef SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_HPP_

#include "Core/core.hpp"
#include "stb_image.h"

class Slash_API Texture {
public:
  friend class TextureManager;
  Texture(std::string name, size_t uid, std::vector<stbi_uc> pixels, int width,
          int height, int channel)
      : name_(std::move(name)), uid_(uid), pixels_(std::move(pixels)),
        width_(width), height_(height), channel_(channel) {}

public:
  std::string name_;
  size_t uid_;
  std::vector<stbi_uc> pixels_;
  int width_;
  int height_;
  int channel_;
};

#endif // SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_HPP_
