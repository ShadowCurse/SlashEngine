#include "Slash.hpp"
#include <iostream>

class TestLayer : public slash::Layer {
 public:
  TestLayer() {
//    const std::vector<slash::Vertex> square_vertices = {
//        {{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//        {{1.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
//        {{1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
//        {{-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};
//
//    const std::vector<uint16_t> square_indices = {0, 1, 2, 2, 3, 0};
//
//    const std::vector<slash::Vertex> triangle_vertices = {
//        {{-1.0f, -1.0f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//        {{1.0f, -1.0f, 0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
//        {{1.0f, 1.0f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}};
//
//    const std::vector<uint16_t> triangle_indices = {0, 1, 2};
//
//    const std::vector<slash::Vertex> qube_vertices = {
//        {{-0.5f, -0.5f, 1.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//        {{0.5f, -0.5f, 1.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
//        {{0.5f, 0.5f, 1.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
//        {{-0.5f, 0.5f, 1.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
//
//        {{-0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
//        {{0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
//        {{0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
//        {{-0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};
//
//    const std::vector<uint16_t> qube_indices = {0, 1, 2, 2, 3, 0,
//                                                0, 3, 7, 7, 4, 0,
//                                                0, 1, 5, 5, 4, 0,
//                                                2, 1, 5, 5, 2, 6,
//                                                3, 2, 6, 6, 7, 3,
//                                                4, 5, 6, 6, 7, 4};
//
//    rotation = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//
//    //slash::ResourceManager::AddMesh("qube", qube_vertices, qube_indices);
//    //slash::ResourceManager::LoadTexture("texture", "textures/texture.jpg");
//    //slash::ResourceManager::AddModel("qube", "qube", "texture", rotation, {5, 5, 5}, {1, 1});
//    // slash::ResourceManager::BindModel("qube");
//    //slash::SceneManager::AddToScene("qube");
//
//    auto& ecs = slash::SceneManager::GetScene().GetECS();
//    square_entity = ecs.create_entity();
//    auto qube_mesh = slash::Mesh_3D{square_vertices, square_indices};
//    ecs.add_component(square_entity, qube_mesh);
//    auto qube_texture = Texture::Load("../../src/Textures/texture.jpg");
//    ecs.add_component(square_entity, qube_texture);
//    auto transform = slash::Transform();
//    transform.rotation = rotation;
//    transform.position = {5, 5, 5};
//    transform.size = {1, 1};
//    ecs.add_component(square_entity, transform);
//    slash::SceneManager::AddToScene(square_entity);
//    //
//    auto triangle_entity = ecs.create_entity();
//    auto triange_mesh = slash::Mesh_3D{triangle_vertices, triangle_indices};
//    ecs.add_component(triangle_entity, triange_mesh);
//    ecs.add_component(triangle_entity, qube_texture); // double texture
//    ecs.add_component(triangle_entity, transform);
//    slash::SceneManager::AddToScene(triangle_entity);
//    //
//    //
//    //slash::SceneManager::AddToScene(slash::GetScene(), "qube");
//    //auto scene = slash::GetScene();
//    //auto ecs = scene.GetEcs();
//    //auto entity = ecs.AddEntity("qube");
//    //ecs.add_component(entity, slash::Mesh("qube", qube_vertices, qube_indices));
//    //..
//    //scene.PullRequirements(entity); // loads compoenets for entity or do nothing
//
//    //slash::ResourceManager::AddMesh("triangle", triangle_vertices, triangle_indices);
//    //slash::ResourceManager::AddModel("triangle", "triangle", "texture", rotation, {5, 5, 5}, {1, 1});
//    //slash::SceneManager::AddToScene("triangle");
  }

//   bool Controls(slash::KeyPressedEvent &e) {
//       if (e.GetKeyCode() == SL_KEY_A) {
//         auto& transform = slash::SceneManager::GetScene().GetECS().get_component<slash::Transform>(square_entity);
//       } else if (e.GetKeyCode() == SL_KEY_D) {
//       } else if (e.GetKeyCode() == SL_KEY_W) {
//       } else if (e.GetKeyCode() == SL_KEY_S) {
//       }
//       return true;
//   }

  void on_attach() final {};
  void on_detach() final {};
  void on_update(slash::TimeStep ts) final {
  }
//  void on_event(slash::Event &event) final {
////     slash::EventDispatcher dispatcher(event);
////     if (dispatcher.Dispatch<slash::KeyPressedEvent>(std::bind(&TestLayer::Controls, this, std::placeholders::_1))) {
////         return;
////     }
//    SL_TRACE("{0}", event.ToString());
//  }

//  slash::Entity square_entity;

//  glm::mat4 rotation;
};

class TestModule {
 public:
  static void init(slash::App &app, int a) {
    std::cout << "TestModule\n";
  }
  static void remove(slash::App &app) {}
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
  app.init_module<TestModule>(12)
      .init_module<slash::WindowModule>(slash::WindowParams("Test", 800, 400))
      .init_module<slash::RenderModule>();

//  auto& wm = app.get_resource<slash::WindowManager>();
//  wm.add_window(slash::WindowParams{"Test2", 300, 200 });

//  app.init_module<slash::LayerModule>();
//  auto& layer_stack = app.get_resource<slash::LayerStack>();
//  layer_stack.push_layer<TestLayer>();

//  app.get_event<slash::MouseMovedEvent>().subscribe(mouse_move_callback);
//  app.get_event<slash::MouseButtonPressedEvent>().subscribe(mouse_button_pressed_callback);
//  app.get_event<slash::MouseButtonReleasedEvent>().subscribe(mouse_button_release_callback);
//  app.get_event<slash::MouseScrolledEvent>().subscribe(mouse_scroll_callback);
//  app.get_event<slash::KeyPressedEvent>().subscribe(key_pressed_callback);
//  app.get_event<slash::KeyReleasedEvent>().subscribe(key_release_callback);

  app.register_component<Velocity>();
  app.register_component<Position>();
  auto e = app.create_entity();
  app.add_component(e, Position{0.0f, 1.0f});
  app.add_component(e, Velocity{1.2f});
//  app.add_system(velocity_system);


  const std::vector<slash::Vertex> qube_vertices = {
      {{-0.5f, -0.5f, 1.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
      {{0.5f, -0.5f, 1.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
      {{0.5f, 0.5f, 1.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
      {{-0.5f, 0.5f, 1.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

      {{-0.5f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
      {{0.5f, -0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
      {{0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
      {{-0.5f, 0.5f, 1.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};

  const std::vector<uint16_t> qube_indices = {0, 1, 2, 2, 3, 0,
                                              0, 3, 7, 7, 4, 0,
                                              0, 1, 5, 5, 4, 0,
                                              2, 1, 5, 5, 2, 6,
                                              3, 2, 6, 6, 7, 3,
                                              4, 5, 6, 6, 7, 4};

  auto rotation = glm::rotate(glm::mat4(1.0f), glm::radians(45.0f), glm::vec3(0.0f, 0.0f, 1.0f));

  app.add_component(e, slash::Mesh_3D{qube_vertices, qube_indices});
//  app.add_component(e, slash::Texture::Load(""));
  app.add_component(e, slash::Transform{rotation, {5, 5, 5}, {1, 1}});

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
}
