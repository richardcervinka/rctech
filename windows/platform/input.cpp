#include "input.h"
#include <cassert>
#include <array>
#include <cstdint>
#include <Windows.h>
#include <hidusage.h>
#include <utility>
#include "error.h"

namespace Rc::Input
{
    static ButtonEvent g_event_button_pushed;
    static ButtonEvent g_event_button_released;
    static MouseMoveEvent g_event_mouse_move;
    static std::array<ButtonState, 256> g_buttons {};

    static KeyCode TranslateMakeCode(USHORT code)
    {
        static constexpr std::array<KeyCode, 255> table
        {
            KeyCode::Unknown,  // 0x00
            KeyCode::Escape,  // 0x01
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
            KeyCode::Dash,  // 0x0C
            KeyCode::Equals,  // 0x0D
            KeyCode::Backspace,  // 0x0E
            KeyCode::Tab,  // 0x0F
            KeyCode::Q,  // 0x10
            KeyCode::W,  // 0x11
            KeyCode::E,  // 0x12
            KeyCode::R,  // 0x13
            KeyCode::T,  // 0x14
            KeyCode::Y,  // 0x15
            KeyCode::U,  // 0x16
            KeyCode::I,  // 0x17
            KeyCode::O,  // 0x18
            KeyCode::P,  // 0x19
            KeyCode::LeftBrace,  // 0x1A
            KeyCode::RightBrace,  // 0x1B
            KeyCode::Return,  // 0x1C
            KeyCode::LeftControl,  // 0x1D
            KeyCode::A,  // 0x1E
            KeyCode::S,  // 0x1F
            KeyCode::D,  // 0x20
            KeyCode::F,  // 0x21
            KeyCode::G,  // 0x22
            KeyCode::H,  // 0x23
            KeyCode::J,  // 0x24
            KeyCode::K,  // 0x25
            KeyCode::L,  // 0x26
            KeyCode::SemiColon,  // 0x27
            KeyCode::Apostrophe,  // 0x28
            KeyCode::GraveAccent,  // 0x29
            KeyCode::LeftShift,  // 0x2A
            KeyCode::Backslash,  // 0x2B
            KeyCode::Z,  // 0x2C
            KeyCode::X,  // 0x2D
            KeyCode::C,  // 0x2E
            KeyCode::V,  // 0x2F
            KeyCode::B,  // 0x30
            KeyCode::N,  // 0x31
            KeyCode::M,  // 0x32
            KeyCode::Comma,  // 0x33
            KeyCode::Period,  // 0x34
            KeyCode::ForwardSlash,  // 0x35
            KeyCode::RightShift,  // 0x36
            KeyCode::KeypadStar,  // 0x37
            KeyCode::LeftAlt,  // 0x38
            KeyCode::Spacebar,  // 0x39
            KeyCode::CapsLock,  // 0x3A
            KeyCode::F1,  // 0x3B
            KeyCode::F2,  // 0x3C
            KeyCode::F3,  // 0x3D
            KeyCode::F4,  // 0x3E
            KeyCode::F5,  // 0x3F
            KeyCode::F6,  // 0x40
            KeyCode::F7,  // 0x41
            KeyCode::F8,  // 0x42
            KeyCode::F9,  // 0x43
            KeyCode::F10,  // 0x44
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
            KeyCode::NonUsBackslash,  // 0x56
            KeyCode::F11,  // 0x57
            KeyCode::F12,  // 0x58
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
            KeyCode::F13,  // 0x64
            KeyCode::F14,  // 0x65
            KeyCode::F15,  // 0x66
            KeyCode::F16,  // 0x67
            KeyCode::F17,  // 0x68
            KeyCode::F18,  // 0x69
            KeyCode::F19,  // 0x6A
            KeyCode::F20,  // 0x6B
            KeyCode::F21,  // 0x6C
            KeyCode::F22,  // 0x6D
            KeyCode::F23,  // 0x6E
            KeyCode::Unknown,  // 0x6F
            KeyCode::Unknown,  // 0x70
            KeyCode::Unknown,  // 0x71
            KeyCode::Unknown,  // 0x72 (LANG1)
            KeyCode::Unknown,  // 0x73 (INT1)
            KeyCode::Unknown,  // 0x74
            KeyCode::Unknown,  // 0x75
            KeyCode::F24,  // 0x76
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

    static KeyCode TranslateMakeCodeE0(USHORT code)
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
            KeyCode::RightControl,  // 0x1D
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
            KeyCode::Home,  // 0x47
            KeyCode::UpArrow,  // 0x48
            KeyCode::PageUp,  // 0x49
            KeyCode::Unknown,  // 0x4A
            KeyCode::LeftArrow,  // 0x4B
            KeyCode::Unknown,  // 0x4C
            KeyCode::RightArrow,  // 0x4D
            KeyCode::Unknown,  // 0x4E
            KeyCode::End,  // 0x4F
            KeyCode::DownArrow,  // 0x50
            KeyCode::PageDown,  // 0x51
            KeyCode::Insert,  // 0x52
            KeyCode::DeleteForward,  // 0x53
            KeyCode::Unknown,  // 0x54
            KeyCode::Unknown,  // 0x55
            KeyCode::Unknown,  // 0x56
            KeyCode::Unknown,  // 0x57
            KeyCode::Unknown,  // 0x58
            KeyCode::Unknown,  // 0x59
            KeyCode::Unknown,  // 0x5A
            KeyCode::Unknown,  // 0x5B
            KeyCode::Unknown,  // 0x5C
            KeyCode::Application,  // 0x5D
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

    // Push button identified by the virtual key code.
    static void PushKey(KeyCode btn)
    {
        if (g_buttons[std::to_underlying(btn)] != ButtonState::Pushed)
        {
            g_buttons[std::to_underlying(btn)] = ButtonState::Pushed;

            g_event_button_pushed.Dispatch(btn);
        }
    }

    // Release button identified by the virtual key code.
    static void ReleaseKey(KeyCode btn)
    {
        if (g_buttons[std::to_underlying(btn)] != ButtonState::Released)
        {
            g_buttons[std::to_underlying(btn)] = ButtonState::Released;

            g_event_button_released.Dispatch(btn);
        }
    }

    void RegisterKeyboard()
    {
        std::array<RAWINPUTDEVICE, 1> rid = {
            RAWINPUTDEVICE
            {
                .usUsagePage = 0x01,
                .usUsage = HID_USAGE_GENERIC_KEYBOARD,
                .dwFlags = RIDEV_DEVNOTIFY, //RIDEV_DEVNOTIFY, RIDEV_INPUTSINK
                .hwndTarget = NULL
            }
            // TODO: RIDEV_INPUTSINK
            // If set, this enables the caller to receive the input even when the caller is not in the foreground. Note that hwndTarget must be specified.
        };

        if (RegisterRawInputDevices(rid.data(), rid.size(), sizeof(RAWINPUTDEVICE)) == FALSE)
        {
            throw SystemException(GetLastError());
        }
    }

    void RegisterMouse()
    {
        std::array<RAWINPUTDEVICE, 2> rid = {
            RAWINPUTDEVICE
            {
                .usUsagePage = 0x01,
                .usUsage = HID_USAGE_GENERIC_MOUSE,
                .dwFlags = RIDEV_DEVNOTIFY,
                .hwndTarget = NULL
            }
        };

        if (RegisterRawInputDevices(rid.data(), rid.size(), sizeof(RAWINPUTDEVICE)) == FALSE)
        {
            throw SystemException(GetLastError());
        }
    }

    void Read()
    {
        alignas(RAWINPUT) std::array<std::byte, sizeof(RAWINPUT) * 32> buffer {};

        UINT size = buffer.size();

        // Get raw input data
        auto const count = GetRawInputBuffer(reinterpret_cast<PRAWINPUT>(buffer.data()), &size, sizeof(RAWINPUTHEADER));

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

        // TODO:
        //     // Procházíme JEN klávesy, o kterých si engine myslí, že jsou STISKNUTÉ
        //     for (uint32_t scanCode : m_activePressedKeys) {
        //         int vKey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK);
                
        //         // GetAsyncKeyState zjišťuje stav přímo u ovladače/OS bypassnutím zpráv
        //         // Pokud nejvyšší bit NIJE 1 (0x8000), klávesa je Fyzicky uvolněná!
        //         if (vKey != 0 && !(GetAsyncKeyState(vKey) & 0x8000)) {
        //             // HW/OS ztratil UP zprávu -> engine stav ručně opraví
        //             SetKeyReleasedInternal(scanCode); 
        //         }
        //     }

        if ((mx != 0) || (my != 0))
        {
            g_event_mouse_move.Dispatch({mx, my});
        }
    }

    void OnButtonPushed(ButtonEvent::Handler& handler)
    {
        g_event_button_pushed.Add(handler);
    }

    void OnButtonReleased(ButtonEvent::Handler& handler)
    {
        g_event_button_released.Add(handler);
    }

    void OnMouseMove(MouseMoveEvent::Handler& handler)
    {
        g_event_mouse_move.Add(handler);
    }

    bool Pushed(KeyCode key)
    {
        return g_buttons[std::to_underlying(key)]  == ButtonState::Pushed;
    }

    bool Released(KeyCode key)
    {
        return g_buttons[std::to_underlying(key)]  == ButtonState::Released;
    }

} // Rc::Input