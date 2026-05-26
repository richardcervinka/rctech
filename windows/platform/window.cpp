#include "window.h"
#include <stdexcept>
#include <cassert>
#include <hidusage.h>
#include <string>
#include "base/utf.h"

namespace Rc
{
    LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        if (msg == WM_CREATE)
        {
            auto* cs = reinterpret_cast<CREATESTRUCT*>(lparam);
            auto lptr = reinterpret_cast<LONG_PTR>(cs->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, lptr);
            return 0;
        }

        if (auto* wnd = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA)))
        {
            return wnd->ProcessMessage(hwnd, msg, wparam, lparam);
        }

        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    LRESULT Window::ProcessMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
    {
        assert(this->m_hwnd == hwnd);

        switch (msg)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_SIZE:
            m_event_size.Dispatch({
                .x = 0,
                .y = 0,
                .w = LOWORD(lparam),
                .h = HIWORD(lparam)
            });
            break;

        case WM_PAINT:
            ValidateRect(hwnd, nullptr);
            return 0;

        default:
            break;
        }

        return DefWindowProc(hwnd, msg, wparam, lparam);
    }

    Window::Window(std::string_view label)
    {
        const auto ulabel = Utf16::FromUtf8(label);

        // Define window class
        const std::wstring class_name {L"MainWindowClass"};

        WNDCLASSW wc {};
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandle(NULL);
        wc.lpszClassName = class_name.c_str();

        RegisterClassW(&wc);

        // Create the window
        m_hwnd = CreateWindowEx(
            0,
            class_name.c_str(),
            reinterpret_cast<LPCWSTR>(ulabel.c_str()),
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            NULL,
            NULL,
            wc.hInstance,
            this
        );

        if (m_hwnd == NULL)
        {
            throw std::runtime_error("CreateWindowEx error");
        }
    }

    void Window::Show()
    {
        ShowWindow(m_hwnd, SW_SHOW);
        UpdateWindow(m_hwnd);
    }

    void Window::Hide()
    {
        ShowWindow(m_hwnd, SW_HIDE);
    }

    Rectangle<int> Window::GetClientArea() const
    {
        RECT rect {};

        if (GetClientRect(m_hwnd, &rect) != TRUE)
        {
            throw std::runtime_error("GetClientRect error");
        }

        return {
            .x = rect.left, 
            .y = rect.top,
            .w = rect.right - rect.left,
            .h = rect.bottom - rect.top
        };
    }

} // Rc
