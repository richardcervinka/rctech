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
        resource_manager = renderer->GetResourceManager();
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

        resource_manager->QueryCounter();

        renderer->BeginFrame();
    }

    void Application::EndFrame()
    {
        renderer->EndFrame();

        // Submit copy commands.
        if (resource_manager->PendingTransfer())
        {
            auto lock = std::unique_lock(*resource_manager, std::defer_lock);

            if (lock.try_lock())
            {
                resource_manager->Transfer();
            }
        }
    }

} // Rc::Generic