#include "Slash.hpp"
#include <iostream>

 class TestLayer : public slash::layer::Layer {
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
//    square_entity = ecs.CreateEntity();
//    auto qube_mesh = slash::Mesh_3D{square_vertices, square_indices};
//    ecs.AddComponent(square_entity, qube_mesh);
//    auto qube_texture = Texture::Load("../../src/Textures/texture.jpg");
//    ecs.AddComponent(square_entity, qube_texture);
//    auto transform = slash::Transform();
//    transform.rotation = rotation;
//    transform.position = {5, 5, 5};
//    transform.size = {1, 1};
//    ecs.AddComponent(square_entity, transform);
//    slash::SceneManager::AddToScene(square_entity);
//    //
//    auto triangle_entity = ecs.CreateEntity();
//    auto triange_mesh = slash::Mesh_3D{triangle_vertices, triangle_indices};
//    ecs.AddComponent(triangle_entity, triange_mesh);
//    ecs.AddComponent(triangle_entity, qube_texture); // double texture
//    ecs.AddComponent(triangle_entity, transform);
//    slash::SceneManager::AddToScene(triangle_entity);
//    //
//    //
//    //slash::SceneManager::AddToScene(slash::GetScene(), "qube");
//    //auto scene = slash::GetScene();
//    //auto ecs = scene.GetEcs();
//    //auto entity = ecs.AddEntity("qube");
//    //ecs.AddComponent(entity, slash::Mesh("qube", qube_vertices, qube_indices));
//    //..
//    //scene.PullRequirements(entity); // loads compoenets for entity or do nothing
//
//    //slash::ResourceManager::AddMesh("triangle", triangle_vertices, triangle_indices);
//    //slash::ResourceManager::AddModel("triangle", "triangle", "texture", rotation, {5, 5, 5}, {1, 1});
//    //slash::SceneManager::AddToScene("triangle");
  }

//   bool Controls(slash::KeyPressedEvent &e) {
//       if (e.GetKeyCode() == SL_KEY_A) {
//         auto& transform = slash::SceneManager::GetScene().GetECS().GetComponent<slash::Transform>(square_entity);
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
  void on_event(slash::Event &event) final {
//     slash::EventDispatcher dispatcher(event);
//     if (dispatcher.Dispatch<slash::KeyPressedEvent>(std::bind(&TestLayer::Controls, this, std::placeholders::_1))) {
//         return;
//     }
    SL_TRACE("{0}", event.ToString());
  }

//  slash::Entity square_entity;

//  glm::mat4 rotation;
};

class TestModule {
 public:
  TestModule(slash::App& app, int a) {
    std::cout << "TestModule\n";
  }
};

auto main() -> int {

    slash::App app;
    app.init_module_2<TestModule>(12)
       .init_module_2<slash::WindowModule>(slash::WindowParams("Test", 800,400));

    auto& wm = app.get_resource<slash::WindowManager>();
    wm.add_window(slash::WindowParams{"Test2", 300, 200 });

    app.run();
}
