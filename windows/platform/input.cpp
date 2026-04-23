#include "input.h"
#include <cassert>
#include <Windows.h>
#include "error.h"
#include <iostream> // ---------------------

namespace Rc
{
    void Input::UpdateMousePosition(int x, int y)
    {
        mouse.x = x;
        mouse.y = y;
    }

    void Input::PushButton(int btn)
    {
        assert(btn >= 0);
        assert(btn < buttons.size());

        buttons[btn] = ButtonState::Down;

        std::cout << btn << " push" << std::endl;
    }

    void Input::ReleaseButton(int btn)
    {
        assert(btn >= 0);
        assert(btn < buttons.size());

        buttons[btn] = ButtonState::Up;

        std::cout << btn << " release" << std::endl;
    }

    void Input::SetViewport(Rectangle<int> value)
    {
        viewport = value;

        //------------------------ align current position
    }

    void Input::Read()
    {
        alignas(RAWINPUT) std::array<std::byte, sizeof(RAWINPUT) * 32> buffer;

        UINT size = buffer.size();

        // Get raw input data
        auto const count = GetRawInputBuffer((PRAWINPUT)buffer.data(), &size, sizeof(RAWINPUTHEADER));

        // No raw inputs.
        if (count == 0)
        {
            return;
        }

        if (count == static_cast<UINT>(-1))
        {
            throw SystemException(GetLastError());
        }

        auto const* pb = buffer.data();

        // Mouse relative position.
        int mx = 0;
        int my = 0;


        for (UINT i = 0; i < count; i++)
        {
            auto const* raw = reinterpret_cast<RAWINPUT const*>(pb);

            if (raw->header.dwType == RIM_TYPEMOUSE)
            {
                if (raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
                {
                    mx += raw->data.mouse.lLastX;
                    my += raw->data.mouse.lLastY;

                    // input.UpdateMousePosition(mx, my);
                }
                // MOUSE_MOVE_ABSOLUTE ... ?

                auto const btn_flags = raw->data.mouse.usButtonFlags;

                if ((btn_flags & RI_MOUSE_LEFT_BUTTON_DOWN) != 0)
                {
                    m_event_left_button_down.Dispatch();
                }
                if ((btn_flags & RI_MOUSE_LEFT_BUTTON_UP) != 0)
                {
                    m_event_left_button_up.Dispatch();
                }
                if ((btn_flags & RI_MOUSE_RIGHT_BUTTON_DOWN) != 0)
                {
                    m_event_right_button_down.Dispatch();
                }
                if ((btn_flags & RI_MOUSE_RIGHT_BUTTON_UP) != 0)
                {
                    m_event_right_button_up.Dispatch();
                }
                if ((btn_flags & RI_MOUSE_MIDDLE_BUTTON_DOWN) != 0)
                {
                    m_event_middle_button_down.Dispatch();
                }
                if ((btn_flags & RI_MOUSE_MIDDLE_BUTTON_UP) != 0)
                {
                    m_event_middle_button_up.Dispatch();
                }
                if ((btn_flags & RI_MOUSE_BUTTON_4_DOWN) != 0)
                {
                    // input.PushButton(VK_XBUTTON1);
                }
                if ((btn_flags & RI_MOUSE_BUTTON_4_UP) != 0)
                {
                    // input.ReleaseButton(VK_XBUTTON1);
                }
                if ((btn_flags & RI_MOUSE_BUTTON_5_DOWN) != 0)
                {
                    // input.PushButton(VK_XBUTTON2);
                }
                if ((btn_flags & RI_MOUSE_BUTTON_5_UP) != 0)
                {
                    // input.ReleaseButton(VK_XBUTTON2);
                }
            }
            else if (raw->header.dwType == RIM_TYPEKEYBOARD)
            {
                if ((raw->data.keyboard.Flags & RI_KEY_BREAK) != 0)
                {
                    // input.ReleaseButton(raw->data.keyboard.VKey);
                }
                else
                {
                    // input.PushButton(raw->data.keyboard.VKey);
                }
            }

            pb += raw->header.dwSize;
        }
    }

} // Rc