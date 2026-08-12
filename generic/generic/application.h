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

        Clock::time_point Now() { return time_now; }

        std::string Name() { return name; }

        Window const& GetWindow() const
        {
            return *window;
        }

    protected: // ------------------------------ Review, needed for development

        Window& GetWindow()
        {
            return *window;
        }

        Render::Renderer& GetRenderer()
        {
            return *renderer;
        }

    private:
        inline static Application* instance {nullptr};

        std::string name;

        // Main window.
        std::unique_ptr<Window> window;

        std::unique_ptr<Render::Renderer> renderer;

        // Main loop time point, application time epoch.
        Clock::time_point time_run;

        // Constant time between BeginFrame() and EndFrame().
        Clock::time_point time_now;
    };

} // Rc::Generic