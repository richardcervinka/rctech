#pragma once

#include <array>
#include <cstdint>
#include "base/geometry.h"
#include "base/event.h"

namespace Rc
{
    class Input
    {
    public:
        using ButtonEvent = Event<int>;
        using MouseButtonEvent = Event<void>;
        using MouseMoveEvent = Event<Point<int>>;

        enum class ButtonState
        {
            Unknown = 0,
            Up = 1 << 0,
            Down = 1 << 1
        };

        // Windows specific
        static void Read();

        static void OnButtonPush(ButtonEvent::Handler& handler)
        {
            m_event_button_push.Add(handler);
        }

        static void OnButtonRelease(ButtonEvent::Handler& handler)
        {
            m_event_button_release.Add(handler);
        }

        // Set handler of relative mouse position.
        static void OnMouseMove(MouseMoveEvent::Handler& handler)
        {
            m_event_mouse_move.Add(handler);
        }

        static void OnLeftButtonDown(MouseButtonEvent::Handler& handler)
        {
            m_event_left_button_down.Add(handler);
        }
        
        static void OnLleftButtonUp(MouseButtonEvent::Handler& handler)
        {
            m_event_left_button_up.Add(handler);
        }
        
        static void OnRightButtonDown(MouseButtonEvent::Handler& handler)
        {
            m_event_right_button_down.Add(handler);
        }
        
        static void OnRightButtonUp(MouseButtonEvent::Handler& handler)
        {
            m_event_right_button_up.Add(handler);
        }

        static void OnMiddleButtonDown(MouseButtonEvent::Handler& handler)
        {
            m_event_middle_button_down.Add(handler);
        }
        
        static void OnMiddleButtonUp(MouseButtonEvent::Handler& handler)
        {
            m_event_middle_button_up.Add(handler);
        }

    private:
        // Push button identified by the virtual key code.
        static void PushButton(int btn);

        // Release button identified by the virtual key code.
        static void ReleaseButton(int btn);

        inline static std::array<ButtonState, 512> buttons {};

        inline static ButtonEvent m_event_button_push;
        inline static ButtonEvent m_event_button_release;
        inline static MouseMoveEvent m_event_mouse_move;
        inline static MouseButtonEvent m_event_left_button_down;
        inline static MouseButtonEvent m_event_left_button_up;
        inline static MouseButtonEvent m_event_right_button_down;
        inline static MouseButtonEvent m_event_right_button_up;
        inline static MouseButtonEvent m_event_middle_button_down;
        inline static MouseButtonEvent m_event_middle_button_up;
    };

} // Rc