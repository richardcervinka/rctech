#pragma once

#include <memory>
#include <string>
#include <cassert>
#include <atomic>
#include <windows.h>

namespace Rc::Platform
{
	class Application
	{
	public:
		Application();

		virtual ~Application();

		static void AttachDebugConsole();

		// Stop event loop as soon as possible.
		void Quit();

		// Abort()

	protected:
		virtual void Initialize();

		// Start main message loop.
		int StartMessageLoop();

		virtual void BeginFrame();

		virtual void EndFrame();

	private:
		void ProcessMessage(MSG const& msg);

		std::atomic<bool> running {false};
	};
	
} // Rc::Platform