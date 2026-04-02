#pragma once

#include <chrono>
#include "platform/application.h"
#include "platform/window.h"
#include "renderer/renderer.h"

namespace Rc::Generic
{
    class Application : private Platform::Application
    {
    public:
        using Clock = std::chrono::high_resolution_clock;

        Application();

        ~Application();

        void Initialize() override;

		// static void AttachDebugConsole();

		int Run();

        void BeginFrame() override;
        void EndFrame() override;

        Clock::time_point Now() { return m_time_now; }

        std::string Name() { return m_name; }

    private:
        inline static Application* m_instance {nullptr};

        std::string m_name;

        // Main window.
		std::unique_ptr<Window> m_window;

        std::unique_ptr<Renderer> m_renderer;

        // Main loop time point, application time epoch.
		Clock::time_point m_time_run;

		// Constant time between BeginFrame() and EndFrame().
		Clock::time_point m_time_now;
    };

} // Rc::Generic