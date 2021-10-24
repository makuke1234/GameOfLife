#include "mainClass.hpp"

gol::GameOfLifeClass::GameOfLifeClass(HINSTANCE hInst, HINSTANCE hPrevInst, LPCWSTR lpCmdArgs, int nCmdShow)
	: MainBase(hInst, hPrevInst, lpCmdArgs, nCmdShow), m_arguments(lpCmdArgs), m_gameLogic(m_arguments.parseFile())
{
	if (m_arguments.empty())
	{
		this->m_msg.box(this->errs(Window::noSimData), w32::Msg::IError | w32::Msg::BOK);
		m_toBeDestroyed = true;
	}
}
gol::GameOfLifeClass::~GameOfLifeClass() noexcept
{
	if (this->m_timerId != 0)
	{
		::KillTimer(this->m_wnd, this->m_timerId);
	}
}

bool gol::GameOfLifeClass::p_createClass() noexcept
{
	if (this->m_classCreated)
		return true;

	::WNDCLASSEXW w{};
	w.cbSize = sizeof w;
	w.hbrBackground = static_cast<::HBRUSH>(::GetStockObject(COLOR_WINDOW));
	w.hCursor = ::LoadCursorW(nullptr, IDC_ARROW);
	w.hIconSm = w.hIcon = ::LoadIconW(this->hInstance, IDI_APPLICATION);
	w.hInstance = this->hInstance;
	w.lpszClassName = this->c_className;
	w.lpfnWndProc = &this->s_windowProc;

	if (!::RegisterClassExW(&w))
	{
		this->m_msg.box(this->errs(Window::appClass), w32::Msg::IError | w32::Msg::BOK);
		return false;
	}
	
	this->m_classCreated = true;
	return true;
}

bool gol::GameOfLifeClass::p_createWindow() noexcept
{
	this->m_wnd.reset(::CreateWindowExW(
		0,
		this->c_className,
		this->c_windowName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		nullptr,
		nullptr,
		this->hInstance,
		this
	));
	if (this->m_wnd == nullptr)
	{
		this->m_msg.box(this->errs(Window::create), w32::Msg::IError | w32::Msg::BOK);
		return false;
	}
	this->m_msg = w32::Msg(this->m_wnd.get<::HWND &>(), this->c_windowName);
	this->m_timerId = ::SetTimer(this->m_wnd, 0, this->c_defDelay, nullptr);
	if (this->m_timerId == 0)
	{
		this->m_msg.box(this->errs(Window::timer), w32::Msg::IError | w32::Msg::BOK);
		return false;
	}

	::ShowWindow(this->m_wnd.get(), this->nCmdShow);
	::UpdateWindow(this->m_wnd.get());
	return true;
}

::LRESULT CALLBACK gol::GameOfLifeClass::s_windowProc(::HWND hwnd, ::UINT uMsg, ::WPARAM wp, ::LPARAM lp) noexcept
{
	static GameOfLifeClass * This{ nullptr };

	if (This == nullptr && uMsg != WM_CREATE)
		return ::DefWindowProcW(hwnd, uMsg, wp, lp);

	switch (uMsg)
	{
	case WM_TIMER:
		This->timerFunc();
		break;
	case WM_PAINT:
		This->drawFrame();
		break;
	case WM_CLOSE:
		This->m_wnd.~UniquePtrHolder();
		break;
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	case WM_CREATE:
		if (auto csw{ reinterpret_cast<::CREATESTRUCTW *>(lp)}; csw != nullptr)
		{
			This = static_cast<GameOfLifeClass *>(csw->lpCreateParams);
		}
		if (This == nullptr)
		{
			w32::Msg::nullbox().box(
				GameOfLifeClass::errs(Window::init),
				GameOfLifeClass::c_windowName,
				w32::Msg::IError | w32::Msg::BOK
			);
			return -1;
		}
		break;
	default:
		return ::DefWindowProcW(hwnd, uMsg, wp, lp);
	}

	return 0;
}

void gol::GameOfLifeClass::drawFrame() noexcept
{
	::RECT r{};
	::GetClientRect(this->m_wnd, &r);
	auto width{ r.right - r.left }, height{ r.bottom - r.top };

	::PAINTSTRUCT ps{};
	auto hdc{ ::BeginPaint(this->m_wnd, &ps) };

	const auto & grid{ this->m_gameLogic.getGrid() };
	auto bufx{ int(grid.front().size()) }, bufy{ int(grid.size()) }, rbufx{ bufx }, rbufy{ bufy };
	if (width * bufy / height > bufx)
	{
		rbufx = width * bufy / height;
	}
	else if (height * bufy / width > bufy)
	{
		rbufy = height * bufx / width;
	}
	/* *** Double buffering *** */

	auto memdc{ ::CreateCompatibleDC(nullptr) };
	auto memBitmap{ ::CreateCompatibleBitmap(hdc, rbufx, rbufy) };
	::SelectObject(memdc, memBitmap);

	/* ************************ */


	/* ******* Painting ******* */

	// Create screen buffer


	static std::vector<COLORREF> scrbuf;

	constexpr auto color = [](bool color) noexcept
	{
		return w32::BGR(int(color) * 239, int(color) * 228, int(color) * 176);
	};

	scrbuf.resize(rbufx * rbufy);
	for (int y = 0; y < bufy; ++y)
	{
		const auto & row{ grid[y] };
		for (int x = 0; x < bufx; ++x)
		{
			scrbuf[y * rbufx + x] = color(!row[x].isAlive());
		}
		for (int x = bufx; x < rbufx; ++x)
		{
			scrbuf[y * rbufx + x] = color(true);
		}
	}
	for (int y = bufy; y < rbufy; ++y)
	{
		for (int x = 0; x < rbufx; ++x)
		{
			scrbuf[y * rbufx + x] = color(true);
		}
	}

	// Create bitmap
	auto bmp{ ::CreateBitmap(rbufx, rbufy, 1, 8 * sizeof(COLORREF), scrbuf.data()) };
	::SelectObject(memdc, bmp);


	/* ************************ */
	
	/* *** Double buffering *** */

	::StretchBlt(hdc, 0, 0, width, height, memdc, 0, 0, rbufx, rbufy, SRCCOPY);
	//::BitBlt(hdc, 0, 0, width, height, memdc, 0, 0, SRCCOPY);
	::DeleteObject(memBitmap);
	::DeleteObject(memdc);

	/* ************************ */

	::DeleteObject(bmp);

	::EndPaint(this->m_wnd, &ps);

	this->m_gameLogic.makeNewGrid();
}

void gol::GameOfLifeClass::timerFunc() const noexcept
{
	::InvalidateRect(this->m_wnd, nullptr, TRUE);
}

int gol::GameOfLifeClass::messageLoop() noexcept
{
	::MSG msg{};
	::BOOL ret;
	while ((ret = ::GetMessageW(&msg, nullptr, 0, 0)) != 0)
	{
		if (ret == -1)
		{
			this->m_msg.box(this->errs(Window::invalidHandle), w32::Msg::IError | w32::Msg::BOK);
			return -1;
		}
		::TranslateMessage(&msg);
		::DispatchMessageW(&msg);
	}
	return int(msg.wParam);
}

int gol::GameOfLifeClass::run()
{
	if (this->m_toBeDestroyed)
		return -1;

	if (!this->p_createClass())
		return 1;
	
	if (!this->p_createWindow())
		return 2;


	return this->messageLoop();
}
