#include "application.h"

namespace Rc::Generic
{
    Application::Application()
    {
        assert(m_instance == nullptr);

        m_instance = this;
    }

    Application::~Application()
    {
        assert(m_instance != nullptr);

        m_instance = nullptr;
    }

    void Application::Initialize()
    {
        Platform::Application::Initialize();

        m_window = std::make_unique<Window>();
        m_window->Show();

        m_renderer = std::make_unique<Renderer>();
        m_renderer->Initialize(*m_window);
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