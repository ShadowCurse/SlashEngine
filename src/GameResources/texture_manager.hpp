#ifndef SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_MANAGER_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_MANAGER_HPP_

#include "Core/core.hpp"
#include "texture.hpp"

class Slash_API TextureManager {
 public:
  TextureManager() = default;
  ~TextureManager() = default;

  bool LoadTexture(const std::string &texture_name, const std::string &texture_path);
  void DeleteTexture(const std::string &texture_name);
  [[nodiscard]] size_t GetUid(const std::string &texture_name) const;
  [[nodiscard]] const Texture &GetTexture(size_t uid) const;

 private:
  std::map<size_t, Texture> textures_;
};

#endif // SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_MANAGER_HPP_
