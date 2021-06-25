#ifndef SLASHENGINE_CAMERA_HPP
#define SLASHENGINE_CAMERA_HPP

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace slash {

class Camera {
 public:
  Camera() {
    view_ =
        glm::lookAt(glm::vec3(0.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f));
  }

  ~Camera() = default;

  void SetRatio(float ratio) {
    proj_ = glm::perspective(glm::radians(45.0f),
                             ratio,
                             0.1f, 10.0f);
    proj_[1][1] *= -1;
  }

  [[nodiscard]] constexpr glm::mat4 GetView() const { return view_; }

  [[nodiscard]] constexpr glm::mat4 GetProj() const { return proj_; }

 private:
  glm::mat4 view_;
  glm::mat4 proj_;
};

} // namespace slash



#endif //SLASHENGINE_CAMERA_HPP
