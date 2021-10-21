#pragma once

#include "../include/w32/common.hpp"
#include "../include/w32/mainloop.hpp"
#include "../include/w32/enumerators.hpp"
#include "../include/w32/messages.hpp"

#include "game.hpp"
#include "argumentSystem.hpp"

namespace gol
{
	class GameOfLifeClass : w32::MainBase
	{
	public:
		enum class Window : std::uint8_t
		{
			appClass,
			create,
			init,
			invalidHandle,
			noSimData,
			timer,

			num_values
		};

	private:
		static constexpr ::LPCWSTR c_className{ L"GameOfLifeClass" }, c_windowName{ L"Game of Life" };
		static constexpr w32::FancyEnumerator<Window, ::LPCWSTR, std::underlying_type_t<Window>(Window::num_values)> errs{{
			L"Error creating application class!",
			L"Error creating window!",
			L"Error initialising window for the first time!",
			L"Invalid window handle value!",
			L"Error retrieving data for the simulation!",
			L"Failed to create timer!"
		}};
		w32::Msg m_msg{ c_windowName };

		w32::UniqueWnd m_wnd;
		static constexpr UINT c_defDelay{ 50 };
		UINT_PTR m_timerId{};

		bool m_classCreated{ false };
		bool p_createClass() noexcept;
		bool p_createWindow() noexcept;

		static ::LRESULT CALLBACK s_windowProc(::HWND, ::UINT, ::WPARAM, ::LPARAM) noexcept;

		void timerFunc() const noexcept;
		int messageLoop() noexcept;
		void drawFrame() noexcept;

		gol::ArgSystem m_arguments;
		gol::Logic m_gameLogic;

		bool m_toBeDestroyed{ false };

	public:
		GameOfLifeClass(HINSTANCE hInst, HINSTANCE hPrevInst, LPCWSTR lpCmdArgs, int nCmdShow);
		~GameOfLifeClass() noexcept;

		int run() override;
	};
}
