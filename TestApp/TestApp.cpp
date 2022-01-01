#include "Slash.hpp"

struct MouseAndKeyboardLogModule : slash::Dependencies<slash::EventPoolModule> {
  [[nodiscard]] auto init(slash::EventPoolModule &ep) -> bool {
    ep.get_event<slash::MouseMovedEvent>().subscribe(mouse_move_callback);
    ep.get_event<slash::MouseButtonPressedEvent>().subscribe(mouse_button_pressed_callback);
    ep.get_event<slash::MouseButtonReleasedEvent>().subscribe(mouse_button_release_callback);
    ep.get_event<slash::MouseScrolledEvent>().subscribe(mouse_scroll_callback);
    ep.get_event<slash::KeyPressedEvent>().subscribe(key_pressed_callback);
    ep.get_event<slash::KeyReleasedEvent>().subscribe(key_release_callback);
    return true;
  }

  [[nodiscard]] auto destroy(slash::EventPoolModule &) -> bool {
    return true;
  }

  static void mouse_move_callback(slash::MouseMovedEvent &e) {
    SL_INFO("Mouse move event: x:{}, y:{}", e.x, e.y);
  }

  static void mouse_scroll_callback(slash::MouseScrolledEvent &e) {
    SL_INFO("Mouse scroll event: x_offset:{}, y_offset:{}", e.x_offset, e.y_offset);
  }

  static void mouse_button_pressed_callback(slash::MouseButtonPressedEvent &e) {
    SL_INFO("Mouse button press event: button:{}", e.button);
  }

  static void mouse_button_release_callback(slash::MouseButtonReleasedEvent &e) {
    SL_INFO("Mouse button release event: button:{}", e.button);
  }

  static void key_pressed_callback(slash::KeyPressedEvent &e) {
    SL_INFO("Key pressed event: key:{}, repeat:{}", e.key_code, e.repeat_count);
  }

  static void key_release_callback(slash::KeyReleasedEvent &e) {
    SL_INFO("Key release event: key:{}", e.key_code);
  }
};

struct SomePack {
  int a;
  double b;

  auto build(slash::PackInserter inserter) {
    inserter.insert(std::move(a));
    inserter.insert(std::move(b));
  }
};

auto main() -> int {
  slash::Log::Init();
  auto app =
      slash::AppBuilder{}
          .add_module<slash::ResourcePackModule>()
          .add_module<slash::EventPoolModule>()
          .add_module<slash::SystemModule>()
          .add_module<slash::ECSModule>()
          .add_module<slash::WindowModule>()
          .add_module<slash::RenderModule>()
          .add_module<MouseAndKeyboardLogModule>()
          .build();
  app.init();

  auto translate = glm::translate(glm::mat4(1.0f), {0.0, 0.0, 0.0});
  auto rotation = glm::rotate(glm::mat4(1.0f), glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  auto scale = glm::scale(glm::mat4(1.0f), {0.5, 0.5, 0.5});

  auto mesh = slash::Square::create();
  auto transform = slash::Transform{glm::mat4{1.0f}, rotation, scale};
  auto texture = slash::Texture::Load("TestApp/texture.jpg");

  auto e = app.add_object_pack(slash::PackObject3d{mesh, transform, texture});

  auto sp = SomePack { 10, 11.1 };
  app.add_pack(sp);

  float mul = 1.0;
  app.add_system([&]() {
    auto &wm = app.get_resource<slash::WindowManager>();
    auto &window = wm.get_main_window();
    auto q = app.get_query<slash::PackObject3d>();
    if (window.is_key_pressed(SL_KEY_A)) {
      SL_INFO("A key pressed");
      SL_INFO(mul);
      for (auto[t] : q) {
        mul += 0.1f;
        t.transform.rotation = glm::rotate(glm::mat4(1.0f), mul * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
      }
    }
    if (window.is_key_pressed(SL_KEY_D)) {
      SL_INFO("D key pressed");
      SL_INFO(mul);
      for (auto[t] : q) {
        mul -= 0.1f;
        t.transform.rotation = glm::rotate(glm::mat4(1.0f), mul * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
      }
    }
  });

  app.add_system([&]() {
    auto &vrm = app.get_resource<slash::VulkanResourceManager>();
    vrm.update_transform(e);
  });

  app.run();

  app.destroy();

  SL_INFO("TestApp shutdown");
}
