#ifndef SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_MANAGER_HPP_
#define SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_MANAGER_HPP_

#include "Core/core.hpp"
#include "texture.hpp"

class Slash_API TextureManager {
public:
  bool LoadTexture(const std::string &texture_name,
                   const std::string &texture_path);
  void RemoveTexture(const std::string &texture_name);
  [[nodiscard]] std::shared_ptr<Texture>
  GetTexture(const std::string &texture_name);

private:
  [[nodiscard]] size_t GenUid(const std::string &texture_name) const;
  std::map<size_t, std::shared_ptr<Texture>> textures_;
};

#endif // SLASHENGINE_SRC_GAMERESOURCES_TEXTURE_MANAGER_HPP_
