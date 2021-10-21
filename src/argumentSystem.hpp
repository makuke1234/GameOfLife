#pragma once

#include "game.hpp"

#include "../include/w32/common.hpp"

#include "../include/ap/argparser.hpp"

#include <string>
#include <string_view>
#include <utility>
#include <cstdint>

namespace gol
{
	using ArgAllocT = void * (*)(std::size_t size);

	std::pair<int, char **> cmdArgsToVec(LPCWSTR args, ArgAllocT allocator);
	
	class ArgSystem
	{
	private:
		static void * newFunc(std::size_t size)
		{
			return new std::uint8_t[size];
		}

		std::pair<int, char **> m_origPair;
		argparser::Tokeniser m_tokeniser;

		std::wstring fileName;

	public:
		ArgSystem(LPCWSTR args);
		~ArgSystem();

		[[nodiscard]] bool empty() const noexcept;

		[[nodiscard]] gol::Logic::CoordGridT parseFile();
	};
}
