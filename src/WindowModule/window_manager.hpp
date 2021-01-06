#ifndef SLASHENGINE_SRC_WINDOWMODULE_WINDOW_MANAGER_HPP_
#define SLASHENGINE_SRC_WINDOWMODULE_WINDOW_MANAGER_HPP_

#include "Core/core.hpp"
#include "window.hpp"

namespace slash {

struct Slash_API WindowParams {
  explicit WindowParams(std::string title = "SlashEngine", uint width = 1920,
                        uint height = 1080)
      : title(std::move(title)), width(width), height(height) {}

  std::string title;
  uint width;
  uint height;
};

class WindowManager {
 public:
  WindowManager();
  ~WindowManager();

  void add_window(WindowParams params);
  [[nodiscard]] auto get_windows() const -> const std::vector<std::unique_ptr<Window>> &;
  void run() const;

 private:
  static void error_callback(int error, const char *description);
  void window_close_callback(size_t window_id);

  std::vector<std::unique_ptr<Window>> windows_;
};

}

#endif //SLASHENGINE_SRC_WINDOWMODULE_WINDOW_MANAGER_HPP_
