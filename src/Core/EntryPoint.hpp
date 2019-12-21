#ifndef SL_ENTRYPOINT
#define SL_ENTRYPOINT

#ifdef SL_LINUX
// #include "Application.hpp"

extern Slash::Application* Slash::CreateApplication();

int main(int argc, char** argv)
{
    Slash::Log::Init();
    auto app = Slash::CreateApplication();
    app->run();
    delete app;
}

#endif

#endif