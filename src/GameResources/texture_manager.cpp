#include "texture_manager.hpp"
#include "Core/log.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture(std::vector<stbi_uc> pixels, int width, int height,
                 int channel)
    : pixels_(std::move(pixels)), width_(width), height_(height),
      channel_(channel) {}

bool TextureManager::LoadTexture(const std::string &texture_name,
                                 const std::string &texture_path) {
  int texWidth, texHeight, texChannel;
  stbi_uc *pixels = stbi_load(texture_path.c_str(), &texWidth, &texHeight, &texChannel,
                              STBI_rgb_alpha);
  if (!pixels) {
    SL_CORE_ERROR("Failed to load texture {0}", texture_path);
    return false;
  }
  auto uid = std::hash<std::string>{}(texture_name);
  int image_size = texWidth * texHeight * 4;
  std::vector<stbi_uc> texture(image_size);
  memcpy(texture.data(), pixels, static_cast<size_t>(image_size));
  textures_.insert(
      {uid, Texture{std::move(texture), texWidth, texHeight, texChannel}});
  stbi_image_free(pixels);
  return true;
}

void TextureManager::DeleteTexture(const std::string &texture_name) {
  auto uid = std::hash<std::string>{}(texture_name);
  if (textures_.find(uid) == textures_.end()) {
    SL_CORE_ERROR("Trying to delete texture {0} that does not exist", texture_name);
    return;
  }
  textures_.erase(uid);
}

size_t TextureManager::GetUid(const std::string &texture_name) const {
  return std::hash<std::string>{}(texture_name);;
}

const Texture &TextureManager::GetTexture(size_t uid) const {
  return textures_.at(uid);
}
