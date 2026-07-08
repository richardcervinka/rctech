#pragma once

#include "base/event.h"
#include "base/geometry.h"

namespace Rc::Input
{
    enum class ButtonState
    {
        Unknown = 0,
        Pushed = 1 << 0,
        Released = 1 << 1
    };
    
    enum class KeyCode
    {
        Unknown,
        A,  // 0x001E
        B,  // 0x0030
        C,  // 0x002E
        D,  // 0x0020
        E,  // 0x0012
        F,  // 0x0021
        G,  // 0x0022
        H,  // 0x0023
        I,  // 0x0017
        J,  // 0x0024
        K,  // 0x0025
        L,  // 0x0026
        M,  // 0x0032
        N,  // 0x0031
        O,  // 0x0018
        P,  // 0x0019
        Q,  // 0x0010
        R,  // 0x0013
        S,  // 0x001F
        T,  // 0x0014
        U,  // 0x0016
        V,  // 0x002F
        W,  // 0x0011
        X,  // 0x002D
        Y,  // 0x0015
        Z,  // 0x002C
        Key1,  // 0x0002
        Key2,  // 0x0003
        Key3,  // 0x0004
        Key4,  // 0x0005
        Key5,  // 0x0006
        Key6,  // 0x0007
        Key7,  // 0x0008
        Key8,  // 0x0009
        Key9,  // 0x000A
        Key0,  // 0x000B
        Return,  // 0x001C
        Escape,  // 0x0001
        Backspace,  // 0x000E
        Tab,  // 0x000F
        Spacebar,  // 0x0039
        Dash,  // 0x000C
        Equals,  // 0x000D
        LeftBrace,  // 0x001A
        RightBrace,  // 0x001B
        Backslash,  // 0x002B
        SemiColon,  // 0x0027
        Apostrophe,  // 0x0028
        GraveAccent,  // 0x0029
        Comma,  // 0x0033
        Period,  // 0x0034
        ForwardSlash,  // 0x0035
        CapsLock,  // 0x003A
        F1,  // 0x003B
        F2,  // 0x003C
        F3,  // 0x003D
        F4,  // 0x003E
        F5,  // 0x003F
        F6,  // 0x0040
        F7,  // 0x0041
        F8,  // 0x0042
        F9,  // 0x0043
        F10,  // 0x0044
        F11,  // 0x0057
        F12,  // 0x0058
        Insert,  // 0xE052
        Home,  // 0xE047
        PageUp,  // 0xE049
        DeleteForward,  // 0xE053
        End,  // 0xE04F
        PageDown,  // 0xE051
        RightArrow,  // 0xE04D
        LeftArrow,  // 0xE04B
        DownArrow,  // 0xE050
        UpArrow,  // 0xE048
        KeypadForwardSlash,  // 0xE035
        KeypadStar,  // 0x0037
        KeypadDash,  // 0x004A
        KeypadPlus,  // 0x004E
        KeypadENTER,  // 0xE01C
        Keypad1,  // 0x004F
        Keypad2,  // 0x0050
        Keypad3,  // 0x0051
        Keypad4,  // 0x004B
        Keypad5,  // 0x004C
        Keypad6,  // 0x004D
        Keypad7,  // 0x0047
        Keypad8,  // 0x0048
        Keypad9,  // 0x0049
        Keypad0,  // 0x0052
        KeypadPeriod,  // 0x0053
        KeypadEquals,  // 0x0059
        NonUsBackslash,  // 0x0056
        Application,  // 0xE05D
        F13,  // 0x0064
        F14,  // 0x0065
        F15,  // 0x0066
        F16,  // 0x0067
        F17,  // 0x0068
        F18,  // 0x0069
        F19,  // 0x006A
        F20,  // 0x006B
        F21,  // 0x006C
        F22,  // 0x006D
        F23,  // 0x006E
        F24,  // 0x0076
        LeftControl,  // 0x001D
        LeftShift,  // 0x002A
        LeftAlt,  // 0x0038
        LeftGui,  // 0xE05B
        RightControl,  // 0xE01D
        RightShift,  // 0x0036
        RightAlt,  // 0xE038
        RightGui,  // 0xE05C
        MouseLeft,
        MouseRight,
        MouseMiddle,
        Mouse4,
        Mouse5
    };

    using ButtonEvent = Event<KeyCode>;
    using MouseMoveEvent = Event<Point<int>>;

    void OnButtonPushed(ButtonEvent::Handler& handler);

    void OnButtonReleased(ButtonEvent::Handler& handler);

    // Set handler of relative mouse position.
    void OnMouseMove(MouseMoveEvent::Handler& handler);

    bool Pushed(KeyCode key);
    bool Released(KeyCode key);

} // Rc::Input
