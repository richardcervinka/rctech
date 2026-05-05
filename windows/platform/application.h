#pragma once

#include <memory>
#include <string>
#include <cassert>
#include <atomic>
#include <vector>
#include <windows.h>

namespace Rc::Platform
{
	class Application
	{
	public:
		Application();

		virtual ~Application();

		// Abort()

		static void AttachDebugConsole(); // --------------- to protected

	protected:
		virtual void Initialize();

		// Start main message loop.
		int StartMessageLoop();

		virtual void BeginFrame();

		virtual void EndFrame();

		// Stop event loop as soon as possible.
		void Quit();

		std::vector<std::u32string> GetCmdArgs() const;

	private:
		void ProcessMessage(MSG const& msg);

		std::atomic<bool> running {false};
	};
	
} // Rc::Platform