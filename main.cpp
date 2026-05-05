#include "generic/application.h"
#include "platform/window.h"
#include "renderer/renderer.h"
#include "generic/char_input.h"
#include "generic/input.h"
#include <print>

// TODO
// int argc;
// LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

int APIENTRY wWinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PWSTR cmdline, int cmdshow)
{
	Rc::Input::ButtonEvent::Handler button_push_handler {[](auto key) {
		std::println("PUSH {}", int(key));
	}};

	Rc::Input::ButtonEvent::Handler button_release_handler {[](auto key) {
		std::println("RELEASE {}", int(key));
	}};

	Rc::CharInput::CharEvent::Handler char_handler {[](char32_t ch) {
		std::println("CHAR {}", unsigned(ch));
	}};

	Rc::Input::OnButtonPushed(button_push_handler);
	Rc::Input::OnButtonReleased(button_release_handler);
	Rc::CharInput::OnChar(char_handler);

    Rc::Platform::Application::AttachDebugConsole(); // ------------------------- move to debug
    
	Rc::Generic::Application app;
	
	Rc::Generic::ApplicationInfo app_info
	{
		.name = "RcTech"
	};
	
    app.Create(app_info);

	app.Run();

	return 0;
}