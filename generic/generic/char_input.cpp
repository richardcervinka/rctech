#include "char_input.h"

namespace Rc::CharInput
{
    CharEvent g_event_char;

    void OnChar(CharEvent::Handler& handler)
    {
        g_event_char.Add(handler);
    }

    void Dispatch(char32_t ch)
    {
        g_event_char.Dispatch(ch);
    }

} // Rc::Input