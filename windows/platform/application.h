#pragma once

#include <memory>
#include <string>
#include <cassert>
#include <atomic>
#include <windows.h>
#include "input.h"

namespace Rc::Platform
{
	class Application
	{
	public:
		Application();

		virtual ~Application();

		virtual void Initialize();

		static void AttachDebugConsole();

		// Stop event loop as soon as possible.
		void Quit();

		// Abort()

	protected:
		// Start main message loop.
		int StartMessageLoop();

		virtual void BeginFrame();

		virtual void EndFrame();

	private:
		void ProcessMessage(MSG const& msg);

		std::atomic<bool> running {false};
	};
	
} // Rc::Platform