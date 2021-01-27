#ifndef SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_HPP_

#include "Core/core.hpp"
#include "Core/log.hpp"
#include "stb_image.h"

namespace slash {

struct Slash_API Texture {
  static auto Load(
      const std::string &texture_path) -> Texture {
    int texWidth, texHeight, texChannel;
    stbi_uc *pixels = stbi_load(texture_path.c_str(), &texWidth, &texHeight,
                                &texChannel, STBI_rgb_alpha);
    if (!pixels) {
      SL_CORE_ERROR("Failed to load texture {0}", texture_path);
      throw std::runtime_error("Failed to load texture");
    }
    auto image_size = texWidth * texHeight * 4;
    std::vector<stbi_uc> texture_image(static_cast<size_t>(image_size));
    memcpy(texture_image.data(), pixels, static_cast<size_t>(image_size));
    Texture ret{
        std::move(texture_image),
        static_cast<uint32_t>(texWidth),
        static_cast<uint32_t>(texHeight),
        static_cast<uint32_t>(texChannel)};
    stbi_image_free(pixels);
    return ret;
  }

 public:
  std::vector<stbi_uc> pixels_;
  uint32_t width_;
  uint32_t height_;
  uint32_t channel_;
};

}

#endif // SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_HPP_
