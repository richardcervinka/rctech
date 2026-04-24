#include "input.h"
#include <cassert>
#include <Windows.h>
#include "error.h"

namespace Rc
{
    void Input::PushButton(int btn)
    {
        assert(btn >= 0);
        assert(btn < buttons.size());

        if (buttons[btn] != ButtonState::Down)
        {
            buttons[btn] = ButtonState::Down;

            m_event_button_push.Dispatch(btn);
        }
    }

    void Input::ReleaseButton(int btn)
    {
        assert(btn >= 0);
        assert(btn < buttons.size());

        if (buttons[btn] != ButtonState::Up)
        {
            buttons[btn] = ButtonState::Up;

            m_event_button_release.Dispatch(btn);
        }
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
                    ReleaseButton(raw->data.keyboard.VKey);
                }
                else if (raw->data.keyboard.Flags == RI_KEY_MAKE)
                {
                    PushButton(raw->data.keyboard.VKey);
                }
            }

            pb += raw->header.dwSize;
        }

        if ((mx != 0) || (my != 0))
        {
            m_event_mouse_move.Dispatch({mx, my});
        }
    }

} // Rc