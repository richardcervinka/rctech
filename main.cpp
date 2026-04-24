#include "generic/application.h"
#include "platform/window.h"
#include "renderer/renderer.h"
#include <print>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
	Rc::Input::ButtonEvent::Handler button_push_handler {[](int key) {
		std::println("PUSH {}", key);
	}};

	Rc::Input::ButtonEvent::Handler button_release_handler {[](int key) {
		std::println("RELEASE {}", key);
	}};

	Rc::Input::OnButtonPush(button_push_handler);
	Rc::Input::OnButtonRelease(button_release_handler);

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