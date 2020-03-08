#ifndef SLASHENGINE_CORE_ENTRY_POINT_H_
#define SLASHENGINE_CORE_ENTRY_POINT_H_

#ifdef SL_LINUX
// #include "Application.hpp"

extern slash::Application *slash::CreateApplication();

int main(int argc, char **argv) {
  slash::Log::Init();
  auto app = slash::CreateApplication();
  app->run();
  delete app;
}

#endif

#endif // SLASHENGINE_CORE_ENTRY_POINT_H_