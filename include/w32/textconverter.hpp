#pragma once

#include <codecvt>
#include <locale>
#include <string_view>
#include <string>

namespace utf
{
	template<class String>
	String conv(std::basic_string_view<char> base)
	{
		return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(base.cbegin(), base.cend());
	}
	template<class String>
	String conv(std::basic_string_view<wchar_t> base)
	{
		return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.to_bytes(base.cbegin(), base.cend());
	}


}
