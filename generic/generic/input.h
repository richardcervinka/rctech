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
        KeyA,  // 0x001E
        KeyB,  // 0x0030
        KeyC,  // 0x002E
        KeyD,  // 0x0020
        KeyE,  // 0x0012
        KeyF,  // 0x0021
        KeyG,  // 0x0022
        KeyH,  // 0x0023
        KeyI,  // 0x0017
        KeyJ,  // 0x0024
        KeyK,  // 0x0025
        KeyL,  // 0x0026
        KeyM,  // 0x0032
        KeyN,  // 0x0031
        KeyO,  // 0x0018
        KeyP,  // 0x0019
        KeyQ,  // 0x0010
        KeyR,  // 0x0013
        KeyS,  // 0x001F
        KeyT,  // 0x0014
        KeyU,  // 0x0016
        KeyV,  // 0x002F
        KeyW,  // 0x0011
        KeyX,  // 0x002D
        KeyY,  // 0x0015
        KeyZ,  // 0x002C
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
        KeyReturn,  // 0x001C
        KeyEscape,  // 0x0001
        KeyBackspace,  // 0x000E
        KeyTab,  // 0x000F
        KeySpacebar,  // 0x0039
        KeyDash,  // 0x000C
        KeyEquals,  // 0x000D
        KeyLeftBrace,  // 0x001A
        KeyRightBrace,  // 0x001B
        KeyBackslash,  // 0x002B
        KeySemiColon,  // 0x0027
        KeyApostrophe,  // 0x0028
        KeyGraveAccent,  // 0x0029
        KeyComma,  // 0x0033
        KeyPeriod,  // 0x0034
        KeyForwardSlash,  // 0x0035
        KeyCapsLock,  // 0x003A
        KeyF1,  // 0x003B
        KeyF2,  // 0x003C
        KeyF3,  // 0x003D
        KeyF4,  // 0x003E
        KeyF5,  // 0x003F
        KeyF6,  // 0x0040
        KeyF7,  // 0x0041
        KeyF8,  // 0x0042
        KeyF9,  // 0x0043
        KeyF10,  // 0x0044
        KeyF11,  // 0x0057
        KeyF12,  // 0x0058
        KeyInsert,  // 0xE052
        KeyHome,  // 0xE047
        KeyPageUp,  // 0xE049
        KeyDeleteForward,  // 0xE053
        KeyEnd,  // 0xE04F
        KeyPageDown,  // 0xE051
        KeyRightArrow,  // 0xE04D
        KeyLeftArrow,  // 0xE04B
        KeyDownArrow,  // 0xE050
        KeyUpArrow,  // 0xE048
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
        KeyNonUsBackslash,  // 0x0056
        KeyApplication,  // 0xE05D
        KeyF13,  // 0x0064
        KeyF14,  // 0x0065
        KeyF15,  // 0x0066
        KeyF16,  // 0x0067
        KeyF17,  // 0x0068
        KeyF18,  // 0x0069
        KeyF19,  // 0x006A
        KeyF20,  // 0x006B
        KeyF21,  // 0x006C
        KeyF22,  // 0x006D
        KeyF23,  // 0x006E
        KeyF24,  // 0x0076
        KeyLeftControl,  // 0x001D
        KeyLeftShift,  // 0x002A
        KeyLeftAlt,  // 0x0038
        KeyLeftGui,  // 0xE05B
        KeyRightControl,  // 0xE01D
        KeyRightShift,  // 0x0036
        KeyRightAlt,  // 0xE038
        KeyRightGui,  // 0xE05C
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

} // Rc::Input
