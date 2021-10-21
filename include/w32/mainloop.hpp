#pragma once

#include "common.hpp"

namespace w32
{
	class MainBase
	{
	protected:
		HINSTANCE hInstance, hPrevInstance;
		LPCWSTR lpCmdArgs;
		int nCmdShow;

	public:
		constexpr MainBase(HINSTANCE hI, HINSTANCE hPrev, LPCWSTR cmd, int show) noexcept
			: hInstance(hI), hPrevInstance(hPrev), lpCmdArgs(cmd), nCmdShow(show)
		{}

		virtual int run() = 0;
	};
}
