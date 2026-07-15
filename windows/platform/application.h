#pragma once

#include <string>
#include <cassert>
#include <atomic>
#include <vector>
#include <windows.h>

namespace Rc::Platform
{
    class Application
    {
    public:
        Application() = default;

        virtual ~Application() = default;

        Application(Application const&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application const&) = delete;
        Application& operator=(Application&&) = delete;

        virtual void Initialize();

        // Stop event loop as soon as possible.
        void Quit();

        // Abort()

    protected:
        // Start main message loop.
        int StartMessageLoop();

        virtual void BeginFrame();

        virtual void EndFrame();

        std::vector<std::string> GetCmdArgs() const;

    private:
        void ProcessMessage(MSG const& msg);

        std::atomic<bool> running {false};
    };
    
} // Rc::Platform