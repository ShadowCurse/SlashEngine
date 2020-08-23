#ifndef SLASHENGINE_CORE_ENTRY_POINT_H_
#define SLASHENGINE_CORE_ENTRY_POINT_H_

#ifdef SL_LINUX
// #include "Application.hpp"
#include <iostream>

extern slash::Application *slash::CreateApplication();

int main(int argc, char **argv) {
  slash::Log::Init();
  std::cout << "Starting application\n";
  auto app = slash::CreateApplication();
  app->run();
  delete app;
  std::cout << "Application shutdown\n";
}

#endif

#endif // SLASHENGINE_CORE_ENTRY_POINT_H_