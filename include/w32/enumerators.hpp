#pragma once

#include <type_traits>
#include <array>
#include <cstdint>

namespace w32
{
	template<class EnumClass, class Type, std::size_t size>
	class FancyEnumerator
	{
	public:
		using EnumClassT = typename std::underlying_type<EnumClass>::type;
		using ScalarT = Type;
	private:
		std::array<Type, size> m_data;

	public:
		constexpr FancyEnumerator(const std::array<Type, size> & arr) noexcept
			: m_data(arr)
		{}


		[[nodiscard]] constexpr const ScalarT & operator()(EnumClass item) const noexcept
		{
			return this->m_data[EnumClassT(item)];
		}
		[[nodiscard]] constexpr const ScalarT & operator()(EnumClassT item) const noexcept
		{
			return this->m_data[item];
		}
	};
}
