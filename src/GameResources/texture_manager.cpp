#include "texture_manager.hpp"
#include "Core/log.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool TextureManager::LoadTexture(const std::string &texture_name,
                                 const std::string &texture_path) {
  auto uid = GenUid(texture_name);
  if (textures_.find(uid) != textures_.end()) {
    SL_CORE_ERROR(
        "Trying to add texture with name {}, but the name is already used",
        texture_name);
    return false;
  }
  int texWidth, texHeight, texChannel;
  stbi_uc *pixels = stbi_load(texture_path.c_str(), &texWidth, &texHeight,
                              &texChannel, STBI_rgb_alpha);
  if (!pixels) {
    SL_CORE_ERROR("Failed to load texture {0}", texture_path);
    return false;
  }
  int image_size = texWidth * texHeight * 4;
  std::vector<stbi_uc> texture_image(image_size);
  memcpy(texture_image.data(), pixels, static_cast<size_t>(image_size));
  textures_.insert({uid, std::make_shared<Texture>(
                             texture_name, uid, std::move(texture_image),
                             texWidth, texHeight, texChannel)});
  stbi_image_free(pixels);
  return true;
}

void TextureManager::RemoveTexture(const std::string &texture_name) {
  auto uid = std::hash<std::string>{}(texture_name);
  if (textures_.find(uid) == textures_.end()) {
    SL_CORE_ERROR("Trying to delete texture {0} that does not exist",
                  texture_name);
    return;
  }
  textures_.erase(uid);
}

std::shared_ptr<Texture>
TextureManager::GetTexture(const std::string &texture_name) {
  auto uid = GenUid(texture_name);
  if (textures_.find(uid) == textures_.end()) {
    SL_CORE_ERROR("No texture with name {0} exist", texture_name);
    return {};
  }
  return textures_[uid];
}

size_t TextureManager::GenUid(const std::string &texture_name) const {
  return std::hash<std::string>{}("Texture" + texture_name);
}
