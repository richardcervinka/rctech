#pragma once

#include "base/event.h"

namespace Rc::CharInput
{
    using CharEvent = Event<char32_t>;

    void OnChar(CharEvent::Handler& handler);

    void Dispatch(char32_t ch);

} // Rc::Input