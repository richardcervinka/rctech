#include "application.h"
#include <cassert>
#include <iostream>
#include <cstddef>
#include "input.h"
#include "generic/char_input.h"
#include "input.h"
#include "base/utf.h"

namespace Rc::Platform
{
    Application::Application()
    {
    }

    Application::~Application()
    {
    }

    void Application::Initialize()
    {
        Input::Register();
    }

    void Application::AttachDebugConsole()
    {
        if (!AttachConsole(ATTACH_PARENT_PROCESS))
        {
            AllocConsole();
            SetConsoleTitle(L"Debug Console");
        }

        freopen("CONOUT$", "w", stdout);
        freopen("CONOUT$", "w", stderr);
        freopen("CONIN$", "r", stdin);

        std::ios::sync_with_stdio(true);

        std::cout.clear();
        std::cerr.clear();
        std::cin.clear();
    }

    int Application::StartMessageLoop()
    {
        assert(running == false);

        running = true;

        MSG msg {};

        while (running)
        {
            Input::Read();

            // Fetch all windows messages.
            while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
            {
                if (msg.message == WM_QUIT)
                {
                    return static_cast<int>(msg.wParam);
                }

                ProcessMessage(msg);
            }

            try
            {
                // Update application state.
                BeginFrame();

                // Render a new frame.
                EndFrame();
            }
            catch (std::exception const& e)
            {
                return -1;
            }
        }

        return static_cast<int>(msg.wParam);
    }

    void Application::ProcessMessage(MSG const& msg)
    {
        switch (msg.message)
        {
            case WM_CHAR:
                // Ignore surrogate characters
                if (!IS_HIGH_SURROGATE(msg.wParam) && !IS_LOW_SURROGATE(msg.wParam))
                {
                    Rc::CharInput::Dispatch(static_cast<char32_t>(msg.wParam));
                }
                return;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    void Application::Quit()
    {
        PostQuitMessage(0);
    }

    void Application::BeginFrame() {}

    void Application::EndFrame() {}

    std::vector<std::string> Application::GetCmdArgs() const
    {
	    int argc = 0;
	    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

        std::vector<std::string> result;
        result.reserve(argc);
        
        for (int i = 0; i < argc; i++)
        {
            char16_t const* arg = reinterpret_cast<char16_t const*>(argv[i]);
            result.emplace_back(Utf8::FromUtf16({arg, std::wcslen(argv[i])}));
        }
        
        return result;
    }

} // Rc::Platform