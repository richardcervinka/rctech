#include "generic/application.h"
#include "platform/window.h"
#include "renderer/renderer.h"
#include <iostream>

int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
{
    Rc::Platform::Application::AttachDebugConsole(); // ------------------------- move to debug
    
	Rc::Generic::Application app;
	
    app.Initialize();

	app.Run();

	return 0;
}