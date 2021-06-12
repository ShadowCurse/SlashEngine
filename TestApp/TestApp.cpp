#include "Slash.hpp"
#include <iostream>

class TestLayer : public slash::Layer {
 public:
  TestLayer() {}

  void on_attach() final {};

  void on_detach() final {};

  void on_update(slash::TimeStep) final {
  }
//  void on_event(slash::Event &event) final {
////     slash::EventDispatcher dispatcher(event);
////     if (dispatcher.Dispatch<slash::KeyPressedEvent>(std::bind(&TestLayer::Controls, this, std::placeholders::_1))) {
////         return;
////     }
//    SL_TRACE("{0}", event.ToString());
//  }
};

class TestModule {
 public:
  static void init(slash::App &, int) {
    std::cout << "TestModule\n";
  }

  static void remove(slash::App &) {}
};

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

struct Velocity {
  float velocity{};
};

void velocity_system(Velocity &velocity) {
  if (velocity.velocity > 2.0f)
    velocity.velocity -= 0.1f;
  else
    velocity.velocity += 0.1f;
}

struct Position {
  float x{};
  float y{};
};

auto main() -> int {
  slash::App app;

  app.add_resource<slash::Camera>();

  app.init_module<TestModule>(12)
      .init_module<slash::WindowModule>(slash::WindowParams("TestApp", 800, 400))
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

  app.register_component<Velocity>();
  app.register_component<Position>();
  auto e = app.create_entity();
  app.add_component(e, Position{0.0f, 1.0f});
  app.add_component(e, Velocity{1.2f});

  auto rotation = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  // TODO pack this into one bundle
  app.add_component(e, slash::Transform{{0.0, 0.0, 0.0}, rotation, {1.0, 1.0, 1.0}});
  app.add_component(e, slash::Texture::Load("TestApp/texture.jpg"));
  app.add_mesh(e, slash::Square::create());

  app.add_system([&](slash::App &app) {
    auto &wm = app.get_resource<slash::WindowManager>();
    auto &windows = wm.get_windows();
    if (!windows.empty() && windows[0]->is_key_pressed(SL_KEY_A)) {
      SL_INFO("key pressed");
      auto &pos = app.get_component<Position>(e);
      SL_INFO("curr pos: x:{} y:{}", pos.x, pos.y);
      auto query = app.get_component_query<Velocity, Position>();
      for (auto[vel, pos]: query) {
        pos.x += vel.velocity;
        SL_INFO("new pos: x:{} y:{}", pos.x, pos.y);
      }
    }
  });

  app.run();

  SL_INFO("TestApp shutdown");
}
