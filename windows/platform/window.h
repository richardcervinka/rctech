#pragma once

#include <windows.h>
#include "base/geometry.h"
#include "base/event.h"
#include <functional>

namespace Rc
{
	//
	// Main application window
	//
	class Window
	{
	public:
		using EventSize = Event<Rectangle<int>>;

		explicit Window(std::u32string_view label);

		void Show();

		void Hide();

		Rectangle<int> GetClientArea() const;

		HWND Hwnd() const { return m_hwnd; }

		void OnEventSize(EventSize::Handler& handler) { m_event_size.Add(handler); }

	private:
		// Window procedure static function.
		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		// Window procedure member function.
		LRESULT ProcessMessage(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		HWND m_hwnd {NULL};

		EventSize m_event_size;
	};

} // Rc