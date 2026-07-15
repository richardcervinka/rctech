#pragma once

#include <chrono>
#include <optional>
#include "platform/application.h"
#include "platform/window.h"
#include "render/renderer.h"

namespace Rc::Generic
{
    struct ApplicationInfo
    {
        std::optional<std::string> name;
    };

    class Application : private Platform::Application
    {
    public:
        using Clock = std::chrono::high_resolution_clock;

        using Platform::Application::Quit;
        using Platform::Application::GetCmdArgs;

        Application(ApplicationInfo const& info);
        ~Application();

        Application(Application const&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application const&) = delete;
        Application& operator=(Application&&) = delete;

        //void Create(ApplicationInfo const& info);

        void Initialize() override;
        
        int Run();

        void BeginFrame() override;
        void EndFrame() override;

        Clock::time_point Now() { return m_time_now; }

        std::string Name() { return m_name; }

        Window const& GetWindow() const
        {
            return *m_window;
        }

    protected: // ------------------------------ Review, needed for development

        Window& GetWindow()
        {
            return *m_window;
        }

        Render::Renderer& GetRenderer()
        {
            return *m_renderer;
        }

        Render::ResourceManager& GetResourceManager()
        {
            return *m_resource_manager;
        }

    private:
        inline static Application* m_instance {nullptr};

        std::string m_name;

        // Main window.
        std::unique_ptr<Window> m_window;

        std::unique_ptr<Render::Renderer> m_renderer;

        std::shared_ptr<Render::ResourceManager> m_resource_manager;

        // Main loop time point, application time epoch.
        Clock::time_point m_time_run;

        // Constant time between BeginFrame() and EndFrame().
        Clock::time_point m_time_now;
    };

} // Rc::Generic