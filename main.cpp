#include "generic/application.h"
#include "generic/char_input.h"
#include "generic/input.h"
#include <print>
#include <iostream>

int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PWSTR cmdline, int cmdshow)
{
    Rc::Input::ButtonEvent::Handler button_push_handler {[](auto key) {
        std::cout << "PUSH " << int(key) << std::endl;
        //std::println("PUSH {}", int(key));
    }};

    Rc::Input::ButtonEvent::Handler button_release_handler {[](auto key) {
        std::cout << "RELEASE " << int(key) << std::endl;
        //std::println("RELEASE {}", int(key));
    }};

    Rc::CharInput::CharEvent::Handler char_handler {[](char32_t ch) {
        std::println("CHAR {}", unsigned(ch));
    }};

    Rc::Input::OnButtonPushed(button_push_handler);
    Rc::Input::OnButtonReleased(button_release_handler);
    Rc::CharInput::OnChar(char_handler);

    Rc::Generic::Application app;

    app.Create({
        .name = "RcTech"
    });

    app.Run();

    return 0;
}