#pragma once

#include <array>
#include <cstdint>
#include "base/geometry.h"
#include "base/event.h"
//#include "generic/input.h"

namespace Rc
{
    enum class ButtonState
    {
        Unknown = 0,
        Up = 1 << 0,
        Down = 1 << 1
    };

    class Input
    {
    public:
        using EventMouseButton = Event<void>;

        // Windows specific
        static void Read();

        // Set mouse relative position.
        void UpdateMousePosition(int x, int y);

        // Push button identified by the virtual key code.
        void PushButton(int btn);

        // Release button identified by the virtual key code.
        void ReleaseButton(int btn);

        void SetViewport(Rectangle<int> value);

        void OnLeftButtonDown(EventMouseButton::Handler& handler)
        {
            m_event_left_button_down.Add(handler);
        }
        
        void OnLleftButtonUp(EventMouseButton::Handler& handler)
        {
            m_event_left_button_up.Add(handler);
        }
        
        void OnRightButtonDown(EventMouseButton::Handler& handler)
        {
            m_event_right_button_down.Add(handler);
        }
        
        void OnRightButtonUp(EventMouseButton::Handler& handler)
        {
            m_event_right_button_up.Add(handler);
        }

        void OnMiddleButtonDown(EventMouseButton::Handler& handler)
        {
            m_event_middle_button_down.Add(handler);
        }
        
        void OnMiddleButtonUp(EventMouseButton::Handler& handler)
        {
            m_event_middle_button_up.Add(handler);
        }

    private:
        Point<int> mouse;

        // float mouse_sensitivity
        // abs pos

        Rectangle<int> viewport {};

        std::array<ButtonState, 512> buttons {};

        inline static EventMouseButton m_event_left_button_down;
        inline static EventMouseButton m_event_left_button_up;
        inline static EventMouseButton m_event_right_button_down;
        inline static EventMouseButton m_event_right_button_up;
        inline static EventMouseButton m_event_middle_button_down;
        inline static EventMouseButton m_event_middle_button_up;
    };

} // Rc