#include "input.h"
#include <cassert>
#include <Windows.h>
#include "error.h"

namespace Rc
{
    KeyCode TranslateMakeCode(USHORT code)
    {
        static constexpr std::array<KeyCode, 255> table
        {
            KeyCode::Unknown,  // 0x00
            KeyCode::KeyEscape,  // 0x01
            KeyCode::Key1,  // 0x02
            KeyCode::Key2,  // 0x03
            KeyCode::Key3,  // 0x04
            KeyCode::Key4,  // 0x05
            KeyCode::Key5,  // 0x06
            KeyCode::Key6,  // 0x07
            KeyCode::Key7,  // 0x08
            KeyCode::Key8,  // 0x09
            KeyCode::Key9,  // 0x0A
            KeyCode::Key0,  // 0x0B
            KeyCode::KeyDash,  // 0x0C
            KeyCode::KeyEquals,  // 0x0D
            KeyCode::KeyBackspace,  // 0x0E
            KeyCode::KeyTab,  // 0x0F
            KeyCode::KeyQ,  // 0x10
            KeyCode::KeyW,  // 0x11
            KeyCode::KeyE,  // 0x12
            KeyCode::KeyR,  // 0x13
            KeyCode::KeyT,  // 0x14
            KeyCode::KeyY,  // 0x15
            KeyCode::KeyU,  // 0x16
            KeyCode::KeyI,  // 0x17
            KeyCode::KeyO,  // 0x18
            KeyCode::KeyP,  // 0x19
            KeyCode::KeyLeftBrace,  // 0x1A
            KeyCode::KeyRightBrace,  // 0x1B
            KeyCode::KeyReturn,  // 0x1C
            KeyCode::KeyLeftControl,  // 0x1D
            KeyCode::KeyA,  // 0x1E
            KeyCode::KeyS,  // 0x1F
            KeyCode::KeyD,  // 0x20
            KeyCode::KeyF,  // 0x21
            KeyCode::KeyG,  // 0x22
            KeyCode::KeyH,  // 0x23
            KeyCode::KeyJ,  // 0x24
            KeyCode::KeyK,  // 0x25
            KeyCode::KeyL,  // 0x26
            KeyCode::KeySemiColon,  // 0x27
            KeyCode::KeyApostrophe,  // 0x28
            KeyCode::KeyGraveAccent,  // 0x29
            KeyCode::KeyLeftShift,  // 0x2A
            KeyCode::KeyBackslash,  // 0x2B
            KeyCode::KeyZ,  // 0x2C
            KeyCode::KeyX,  // 0x2D
            KeyCode::KeyC,  // 0x2E
            KeyCode::KeyV,  // 0x2F
            KeyCode::KeyB,  // 0x30
            KeyCode::KeyN,  // 0x31
            KeyCode::KeyM,  // 0x32
            KeyCode::KeyComma,  // 0x33
            KeyCode::KeyPeriod,  // 0x34
            KeyCode::KeyForwardSlash,  // 0x35
            KeyCode::KeyRightShift,  // 0x36
            KeyCode::KeypadStar,  // 0x37
            KeyCode::KeyLeftAlt,  // 0x38
            KeyCode::KeySpacebar,  // 0x39
            KeyCode::KeyCapsLock,  // 0x3A
            KeyCode::KeyF1,  // 0x3B
            KeyCode::KeyF2,  // 0x3C
            KeyCode::KeyF3,  // 0x3D
            KeyCode::KeyF4,  // 0x3E
            KeyCode::KeyF5,  // 0x3F
            KeyCode::KeyF6,  // 0x40
            KeyCode::KeyF7,  // 0x41
            KeyCode::KeyF8,  // 0x42
            KeyCode::KeyF9,  // 0x43
            KeyCode::KeyF10,  // 0x44
            KeyCode::Unknown,  // 0x45 (NumLock E0)
            KeyCode::Unknown,  // 0x46 (ScrollLock)
            KeyCode::Keypad7,  // 0x47
            KeyCode::Keypad8,  // 0x48
            KeyCode::Keypad9,  // 0x49
            KeyCode::KeypadDash,  // 0x4A
            KeyCode::Keypad4,  // 0x4B
            KeyCode::Keypad5,  // 0x4C
            KeyCode::Keypad6,  // 0x4D
            KeyCode::KeypadPlus,  // 0x4E
            KeyCode::Keypad1,  // 0x4F
            KeyCode::Keypad2,  // 0x50
            KeyCode::Keypad3,  // 0x51
            KeyCode::Keypad0,  // 0x52
            KeyCode::KeypadPeriod,  // 0x53
            KeyCode::Unknown,  // 0x54 (XT PrintScreen)
            KeyCode::Unknown,  // 0x55
            KeyCode::KeyNonUsBackslash,  // 0x56
            KeyCode::KeyF11,  // 0x57
            KeyCode::KeyF12,  // 0x58
            KeyCode::KeypadEquals,  // 0x59
            KeyCode::Unknown,  // 0x5A
            KeyCode::Unknown,  // 0x5B
            KeyCode::Unknown,  // 0x5C
            KeyCode::Unknown,  // 0x5D
            KeyCode::Unknown,  // 0x5E
            KeyCode::Unknown,  // 0x5F
            KeyCode::Unknown,  // 0x60
            KeyCode::Unknown,  // 0x61
            KeyCode::Unknown,  // 0x62
            KeyCode::Unknown,  // 0x63
            KeyCode::KeyF13,  // 0x64
            KeyCode::KeyF14,  // 0x65
            KeyCode::KeyF15,  // 0x66
            KeyCode::KeyF16,  // 0x67
            KeyCode::KeyF17,  // 0x68
            KeyCode::KeyF18,  // 0x69
            KeyCode::KeyF19,  // 0x6A
            KeyCode::KeyF20,  // 0x6B
            KeyCode::KeyF21,  // 0x6C
            KeyCode::KeyF22,  // 0x6D
            KeyCode::KeyF23,  // 0x6E
            KeyCode::Unknown,  // 0x6F
            KeyCode::Unknown,  // 0x70
            KeyCode::Unknown,  // 0x71
            KeyCode::Unknown,  // 0x72 (LANG1)
            KeyCode::Unknown,  // 0x73 (INT1)
            KeyCode::Unknown,  // 0x74
            KeyCode::Unknown,  // 0x75
            KeyCode::KeyF24,  // 0x76
            KeyCode::Unknown,  // 0x77
            KeyCode::Unknown,  // 0x78
            KeyCode::Unknown,  // 0x79
            KeyCode::Unknown,  // 0x7A
            KeyCode::Unknown,  // 0x7B
            KeyCode::Unknown,  // 0x7C
            KeyCode::Unknown,  // 0x7D
            KeyCode::Unknown,  // 0x7E (KeypadComma)
            KeyCode::Unknown  // 0x7F
        };

        if (code > 0x7F)
        {
            return KeyCode::Unknown;
        }

        return table[static_cast<uint8_t>(code & 0x7F)];
    }

    KeyCode TranslateMakeCodeE0(USHORT code)
    {
        static constexpr std::array<KeyCode, 255> table
        {
            KeyCode::Unknown,  // 0x00
            KeyCode::Unknown,  // 0x01
            KeyCode::Unknown,  // 0x02
            KeyCode::Unknown,  // 0x03
            KeyCode::Unknown,  // 0x04
            KeyCode::Unknown,  // 0x05
            KeyCode::Unknown,  // 0x06
            KeyCode::Unknown,  // 0x07
            KeyCode::Unknown,  // 0x08
            KeyCode::Unknown,  // 0x09
            KeyCode::Unknown,  // 0x0A
            KeyCode::Unknown,  // 0x0B
            KeyCode::Unknown,  // 0x0C
            KeyCode::Unknown,  // 0x0D
            KeyCode::Unknown,  // 0x0E
            KeyCode::Unknown,  // 0x0F
            KeyCode::Unknown,  // 0x10
            KeyCode::Unknown,  // 0x11
            KeyCode::Unknown,  // 0x12
            KeyCode::Unknown,  // 0x13
            KeyCode::Unknown,  // 0x14
            KeyCode::Unknown,  // 0x15
            KeyCode::Unknown,  // 0x16
            KeyCode::Unknown,  // 0x17
            KeyCode::Unknown,  // 0x18
            KeyCode::Unknown,  // 0x19
            KeyCode::Unknown,  // 0x1A
            KeyCode::Unknown,  // 0x1B
            KeyCode::Unknown,  // 0x1C
            KeyCode::KeyRightControl,  // 0x1D
            KeyCode::Unknown,  // 0x1E
            KeyCode::Unknown,  // 0x1F
            KeyCode::Unknown,  // 0x20
            KeyCode::Unknown,  // 0x21
            KeyCode::Unknown,  // 0x22
            KeyCode::Unknown,  // 0x23
            KeyCode::Unknown,  // 0x24
            KeyCode::Unknown,  // 0x25
            KeyCode::Unknown,  // 0x26
            KeyCode::Unknown,  // 0x27
            KeyCode::Unknown,  // 0x28
            KeyCode::Unknown,  // 0x29
            KeyCode::Unknown,  // 0x2A
            KeyCode::Unknown,  // 0x2B
            KeyCode::Unknown,  // 0x2C
            KeyCode::Unknown,  // 0x2D
            KeyCode::Unknown,  // 0x2E
            KeyCode::Unknown,  // 0x2F
            KeyCode::Unknown,  // 0x30
            KeyCode::Unknown,  // 0x31
            KeyCode::Unknown,  // 0x32
            KeyCode::Unknown,  // 0x33
            KeyCode::Unknown,  // 0x34
            KeyCode::Unknown,  // 0x35
            KeyCode::Unknown,  // 0x36
            KeyCode::Unknown,  // 0x37
            KeyCode::Unknown,  // 0x38
            KeyCode::Unknown,  // 0x39
            KeyCode::Unknown,  // 0x3A
            KeyCode::Unknown,  // 0x3B
            KeyCode::Unknown,  // 0x3C
            KeyCode::Unknown,  // 0x3D
            KeyCode::Unknown,  // 0x3E
            KeyCode::Unknown,  // 0x3F
            KeyCode::Unknown,  // 0x40
            KeyCode::Unknown,  // 0x41
            KeyCode::Unknown,  // 0x42
            KeyCode::Unknown,  // 0x43
            KeyCode::Unknown,  // 0x44
            KeyCode::Unknown,  // 0x45
            KeyCode::Unknown,  // 0x46
            KeyCode::KeyHome,  // 0x47
            KeyCode::KeyUpArrow,  // 0x48
            KeyCode::KeyPageUp,  // 0x49
            KeyCode::Unknown,  // 0x4A
            KeyCode::KeyLeftArrow,  // 0x4B
            KeyCode::Unknown,  // 0x4C
            KeyCode::KeyRightArrow,  // 0x4D
            KeyCode::Unknown,  // 0x4E
            KeyCode::KeyEnd,  // 0x4F
            KeyCode::KeyDownArrow,  // 0x50
            KeyCode::KeyPageDown,  // 0x51
            KeyCode::KeyInsert,  // 0x52
            KeyCode::KeyDeleteForward,  // 0x53
            KeyCode::Unknown,  // 0x54
            KeyCode::Unknown,  // 0x55
            KeyCode::Unknown,  // 0x56
            KeyCode::Unknown,  // 0x57
            KeyCode::Unknown,  // 0x58
            KeyCode::Unknown,  // 0x59
            KeyCode::Unknown,  // 0x5A
            KeyCode::Unknown,  // 0x5B
            KeyCode::Unknown,  // 0x5C
            KeyCode::KeyApplication,  // 0x5D
            KeyCode::Unknown,  // 0x5E
            KeyCode::Unknown,  // 0x5F
            KeyCode::Unknown,  // 0x60
            KeyCode::Unknown,  // 0x61
            KeyCode::Unknown,  // 0x62
            KeyCode::Unknown,  // 0x63
            KeyCode::Unknown,  // 0x64
            KeyCode::Unknown,  // 0x65
            KeyCode::Unknown,  // 0x66
            KeyCode::Unknown,  // 0x67
            KeyCode::Unknown,  // 0x68
            KeyCode::Unknown,  // 0x69
            KeyCode::Unknown,  // 0x6A
            KeyCode::Unknown,  // 0x6B
            KeyCode::Unknown,  // 0x6C
            KeyCode::Unknown,  // 0x6D
            KeyCode::Unknown,  // 0x6E
            KeyCode::Unknown,  // 0x6F
            KeyCode::Unknown,  // 0x70
            KeyCode::Unknown,  // 0x71
            KeyCode::Unknown,  // 0x72
            KeyCode::Unknown,  // 0x73
            KeyCode::Unknown,  // 0x74
            KeyCode::Unknown,  // 0x75
            KeyCode::Unknown,  // 0x76
            KeyCode::Unknown,  // 0x77
            KeyCode::Unknown,  // 0x78
            KeyCode::Unknown,  // 0x79
            KeyCode::Unknown,  // 0x7A
            KeyCode::Unknown,  // 0x7B
            KeyCode::Unknown,  // 0x7C
            KeyCode::Unknown,  // 0x7D
            KeyCode::Unknown,  // 0x7E
            KeyCode::Unknown,  // 0x7F

            // 0x80–0xFF unused for E0-prefixed scan codes
        };

        if (code > 0x7F)
        {
            return KeyCode::Unknown;
        }

        return table[static_cast<uint8_t>(code & 0x7F)];
    }


    void Input::PushKey(KeyCode btn)
    {
        if (m_buttons[(int)btn] != ButtonState::Released)
        {
            m_buttons[(int)btn] = ButtonState::Released;

            m_event_button_pushed.Dispatch(btn);
        }
    }

    void Input::ReleaseKey(KeyCode btn)
    {
        if (m_buttons[(int)btn] != ButtonState::Pushed)
        {
            m_buttons[(int)btn] = ButtonState::Pushed;

            m_event_button_released.Dispatch(btn);
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

                auto const btn_flags = raw->data.mouse.usButtonFlags;

                if ((btn_flags & RI_MOUSE_LEFT_BUTTON_DOWN) != 0)
                {
                    PushKey(KeyCode::MouseLeft);
                }
                if ((btn_flags & RI_MOUSE_LEFT_BUTTON_UP) != 0)
                {
                    ReleaseKey(KeyCode::MouseLeft);
                }
                if ((btn_flags & RI_MOUSE_RIGHT_BUTTON_DOWN) != 0)
                {
                    PushKey(KeyCode::MouseRight);
                }
                if ((btn_flags & RI_MOUSE_RIGHT_BUTTON_UP) != 0)
                {
                    ReleaseKey(KeyCode::MouseRight);
                }
                if ((btn_flags & RI_MOUSE_MIDDLE_BUTTON_DOWN) != 0)
                {
                    PushKey(KeyCode::MouseMiddle);
                }
                if ((btn_flags & RI_MOUSE_MIDDLE_BUTTON_UP) != 0)
                {
                    ReleaseKey(KeyCode::MouseMiddle);
                }
                if ((btn_flags & RI_MOUSE_BUTTON_4_DOWN) != 0)
                {
                    PushKey(KeyCode::Mouse4);
                }
                if ((btn_flags & RI_MOUSE_BUTTON_4_UP) != 0)
                {
                    ReleaseKey(KeyCode::Mouse4);
                }
                if ((btn_flags & RI_MOUSE_BUTTON_5_DOWN) != 0)
                {
                    PushKey(KeyCode::Mouse5);
                }
                if ((btn_flags & RI_MOUSE_BUTTON_5_UP) != 0)
                {
                    ReleaseKey(KeyCode::Mouse5);
                }
            }
            else if (raw->header.dwType == RIM_TYPEKEYBOARD)
            { 
                auto code = KeyCode::Unknown;

                if ((raw->data.keyboard.Flags & RI_KEY_E0) != 0)
                {
                    code = TranslateMakeCodeE0(raw->data.keyboard.MakeCode);
                }
                else if ((raw->data.keyboard.Flags & RI_KEY_E1) == 0)
                {
                    code = TranslateMakeCode(raw->data.keyboard.MakeCode);
                }

                if (code != KeyCode::Unknown)
                {
                    if ((raw->data.keyboard.Flags & RI_KEY_BREAK) != 0)
                    {
                        ReleaseKey(code);
                    }
                    else
                    {
                        PushKey(code);
                    }
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