#include "application.h"

namespace Rc::Generic
{
    Application::Application(ApplicationInfo const& info)
    {
        assert(instance == nullptr);

        instance = this;

        name = info.name.value_or("RcTech");
        
        window = std::make_unique<Window>(info.name.value_or("RcTech"));
        window->Show();

        renderer = std::make_unique<Render::Renderer>();
        renderer->Initialize(*window);
    }

    Application::~Application()
    {
        assert(instance != nullptr);

        instance = nullptr;
    }

    void Application::Initialize()
    {
        Platform::Application::Initialize();
    }

    int Application::Run()
    {
        time_run = Clock::now();

        return Platform::Application::StartMessageLoop();
    }

    void Application::BeginFrame()
    {
        time_now = Clock::now();

        renderer->BeginFrame();
    }

    void Application::EndFrame()
    {
        renderer->EndFrame();
    }

} // Rc::Generic