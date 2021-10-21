#pragma once

#include "common.hpp"

#include <string>

namespace w32
{
	class Msg
	{
	public:
		enum class Result : std::uint8_t
		{
			OK = IDOK,
			Cancel = IDCANCEL,
			Abort = IDABORT,
			Retry = IDRETRY,
			Ignore = IDIGNORE,
			Yes = IDYES,
			No = IDNO
		};
		enum Param : ::LONG
		{
			BOK = MB_OK,
			BOKCancel = MB_OKCANCEL,
			BAbortRetryIgnore = MB_ABORTRETRYIGNORE,
			BYesNoCancel = MB_YESNOCANCEL,
			BYesNo = MB_YESNO,
		
			IError = MB_ICONERROR,
			IQuestion = MB_ICONQUESTION,
			IWarn = MB_ICONWARNING,
			IInfo = MB_ICONINFORMATION,
		};
		using ParamT = std::underlying_type_t<Param>;

	private:
		::HWND & m_window;
		::LPCWSTR m_caption;
		
		static inline ::HWND nullHandle{ nullptr };

	public:
		[[nodiscard]] static const Msg & nullbox() noexcept
		{
			static const Msg c_msg;
			return c_msg;
		};

		constexpr Msg(::HWND & window = nullHandle, ::LPCWSTR caption = L"") noexcept
			: m_window(window), m_caption(caption)
		{}
		constexpr Msg(::LPCWSTR caption) noexcept
			: m_window(nullHandle), m_caption(caption)
		{}
		Msg(const Msg &) = delete;
		Msg(Msg && other) noexcept
			: m_window(other.m_window), m_caption(other.m_caption)
		{
			other.m_window = this->nullHandle;
			other.m_caption = L"";
		}
		Msg & operator=(const Msg &) = delete;
		Msg & operator=(Msg && other) noexcept
		{
			this->m_window = other.m_window;
			this->m_caption = other.m_caption;

			other.m_window = this->nullHandle;
			other.m_caption = L"";

			return *this;
		}
		~Msg() noexcept = default;

		Msg::Result box(const std::wstring & msg, const std::wstring & caption, Msg::ParamT params = Msg::BOK) const noexcept
		{
			return static_cast<Msg::Result>(::MessageBoxW(this->m_window, msg.c_str(), caption.c_str(), params));
		}
		Msg::Result box(const std::wstring & msg, Msg::ParamT params = Msg::BOK) const noexcept
		{
			return static_cast<Msg::Result>(::MessageBoxW(this->m_window, msg.c_str(), this->m_caption, params));
		}
	};

}
