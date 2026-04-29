#include "application.h"
#include <cassert>
#include <stdexcept>
#include <hidusage.h>
#include <iostream>
#include <cstddef>
#include <chrono>
#include <array>
#include "input.h"
#include "error.h"
#include "generic/char_input.h"
#include "input.h"

namespace Rc::Platform
{
    static void RegisterRawInput()
    {
        std::array<RAWINPUTDEVICE, 2> rid = {
            // Keyboard
            RAWINPUTDEVICE
            {
                .usUsagePage = 0x01,
                .usUsage = HID_USAGE_GENERIC_KEYBOARD,
                .dwFlags = RIDEV_DEVNOTIFY, //RIDEV_DEVNOTIFY,
                .hwndTarget = NULL
            },
            // Mouse
            RAWINPUTDEVICE
            {
                .usUsagePage = 0x01,
                .usUsage = HID_USAGE_GENERIC_MOUSE,
                .dwFlags = RIDEV_DEVNOTIFY,
                .hwndTarget = NULL
            }
        };

        if (!RegisterRawInputDevices(rid.data(), rid.size(), sizeof(RAWINPUTDEVICE)))
        {
            throw SystemException(GetLastError());
        }
    }

    Application::Application()
    {
    }

    Application::~Application()
    {
    }

    void Application::Initialize()
    {
        RegisterRawInput();
    }

    void Application::AttachDebugConsole()
    {
        if (!AttachConsole(ATTACH_PARENT_PROCESS))
        {
            AllocConsole();
            SetConsoleTitle("Debug Console");
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

} // Rc::Platform