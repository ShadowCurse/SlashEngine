#include "Slash.hpp"
#include <iostream>

void mouse_move_callback(slash::MouseMovedEvent &e) {
  SL_INFO("Mouse move event: x:{}, y:{}", e.x, e.y);
}

void mouse_scroll_callback(slash::MouseScrolledEvent &e) {
  SL_INFO("Mouse scroll event: x_offset:{}, y_offset:{}", e.x_offset, e.y_offset);
}

void mouse_button_pressed_callback(slash::MouseButtonPressedEvent &e) {
  SL_INFO("Mouse button press event: button:{}", e.button);
}

void mouse_button_release_callback(slash::MouseButtonReleasedEvent &e) {
  SL_INFO("Mouse button release event: button:{}", e.button);
}

void key_pressed_callback(slash::KeyPressedEvent &e) {
  SL_INFO("Key pressed event: key:{}, repeat:{}", e.key_code, e.repeat_count);
}

void key_release_callback(slash::KeyReleasedEvent &e) {
  SL_INFO("Key release event: key:{}", e.key_code);
}

auto main() -> int {
  slash::App app;

  app.add_resource<slash::Camera>();

  app.init_module<slash::WindowModule>(slash::WindowParams("TestApp", 800, 400))
     .init_module<slash::RenderModule>();

//  auto& wm = app.get_resource<slash::WindowManager>();
//  wm.add_window(slash::WindowParams{"Test2", 300, 200 });

//  app.init_module<slash::LayerModule>();
//  auto& layer_stack = app.get_resource<slash::LayerStack>();
//  layer_stack.push_layer<TestLayer>();

  app.get_event<slash::MouseMovedEvent>().subscribe(mouse_move_callback);
  app.get_event<slash::MouseButtonPressedEvent>().subscribe(mouse_button_pressed_callback);
  app.get_event<slash::MouseButtonReleasedEvent>().subscribe(mouse_button_release_callback);
  app.get_event<slash::MouseScrolledEvent>().subscribe(mouse_scroll_callback);
  app.get_event<slash::KeyPressedEvent>().subscribe(key_pressed_callback);
  app.get_event<slash::KeyReleasedEvent>().subscribe(key_release_callback);

//  auto e = app.create_entity();

  auto translate = glm::translate(glm::mat4(1.0f), {0.0, 0.0, 0.0});
  auto rotation = glm::rotate(glm::mat4(1.0f), glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
  auto scale = glm::scale(glm::mat4(1.0f), {0.5, 0.5, 0.5});

  auto mesh = slash::Square::create();
  auto transform = slash::Transform{glm::mat4{1.0f}, rotation, scale};
  auto texture = slash::Texture::Load("TestApp/texture.jpg");

  auto e = app.add_pack(slash::PackObject3d{mesh, transform, texture});

  float mul = 1.0;

  app.add_system([&](slash::App &app) {
    auto &wm = app.get_resource<slash::WindowManager>();
    auto &windows = wm.get_windows();
    auto q = app.get_component_query<slash::PackObject3d>();
    if (!windows.empty() && windows[0]->is_key_pressed(SL_KEY_A)) {
      SL_INFO("A key pressed");
      SL_INFO(mul);
      for (auto[t] : q) {
        mul += 0.1f;
        t.transform.rotation = glm::rotate(glm::mat4(1.0f), mul * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
      }
    }
    if (!windows.empty() && windows[0]->is_key_pressed(SL_KEY_D)) {
      SL_INFO("D key pressed");
      SL_INFO(mul);
      for (auto[t] : q) {
        mul -= 0.1f;
        t.transform.rotation = glm::rotate(glm::mat4(1.0f), mul * glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
      }
    }
  });

  app.add_system([&](slash::App &app) {
    auto &vrm = app.get_resource<slash::VulkanResourceManager>();
    vrm.update_transform(e);
  });

  app.run();



  SL_INFO("TestApp shutdown");
}
