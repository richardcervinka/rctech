#include "application.h"

namespace Rc::Generic
{
    Application::Application(ApplicationInfo const& info)
    {
        assert(m_instance == nullptr);

        m_instance = this;

        m_name = info.name.value_or("RcTech");
        
        m_window = std::make_unique<Window>(info.name.value_or("RcTech"));
        m_window->Show();

        m_renderer = std::make_unique<Render::Renderer>();
        m_renderer->Initialize(*m_window);
        m_resource_manager = m_renderer->GetResourceManager();
    }

    Application::~Application()
    {
        assert(m_instance != nullptr);

        m_instance = nullptr;
    }

    void Application::Initialize()
    {
        Platform::Application::Initialize();
    }

    int Application::Run()
    {
        m_time_run = Clock::now();

        return Platform::Application::StartMessageLoop();
    }

    void Application::BeginFrame()
    {
        m_time_now = Clock::now();

        m_renderer->BeginFrame();
    }

    void Application::EndFrame()
    {
        m_renderer->EndFrame();
    }

} // Rc::Generic