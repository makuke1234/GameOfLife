#pragma once

#include <vector>
#include <cstdint>

namespace gol
{
	struct Coord
	{
		using CoordT = std::uint32_t;
		CoordT x, y;
	};

	struct Cell
	{
		#pragma pack(1)
		union
		{
			struct
			{
				std::uint8_t m_isAlive:1;
				std::uint8_t m_neighbours:4;
				std::uint8_t m_unused:3;
			} elems;
			std::uint8_t m_value{ 0 };
		};
		#pragma pack(0)

		constexpr void setAlive() noexcept
		{
			this->elems.m_isAlive = true;
		}
		constexpr void setDead() noexcept
		{
			this->elems.m_isAlive = false;
		}
		[[nodiscard]] constexpr bool isAlive() const noexcept
		{
			return this->elems.m_isAlive;
		}
		[[nodiscard]] constexpr bool isDeadNoNeighbours() const noexcept
		{
			return this->m_value == 0;
		}

		[[nodiscard]] constexpr bool futureHasToDieIsAlive() const noexcept
		{
			//return this->elems.m_isAlive & ((this->elems.m_neighbours < 2) | (this->elems.m_neighbours > 3));
			// Optimised discrete math
			//return this->m_isAlive & !((this->m_neighbours == 2) | (this->m_neighbours == 3));
			//return this->m_isAlive & !(this->m_neighbours & 0b1110 == 0b0010);
			return this->elems.m_isAlive & ((this->elems.m_neighbours & 0b1110) != 0b0010);
		}
		[[nodiscard]] constexpr bool futureHasToLive() const noexcept
		{
			// return (this->m_isAlive & (this->m_neighbours == 2 | this->m_neighbours == 3)) | (!this->m_isAlive & (this->m_neighbours == 3));
			// Optimised discrete mathematics
			return (this->elems.m_isAlive & (this->elems.m_neighbours == 2)) | (this->elems.m_neighbours == 3);
		}
		[[nodiscard]] constexpr bool futureHasToLiveIsDead() const noexcept
		{
			//return !this->m_isAlive & ((this->m_isAlive & (this->m_neighbours == 2)) | (this->m_neighbours == 3));
			return !this->elems.m_isAlive & (this->elems.m_neighbours == 3);
		}
	};

	class Logic
	{
	public:
		using CellVec = std::vector<gol::Cell>;
		using GridT = std::vector<CellVec>;
		using CoordT = gol::Coord::CoordT;
		using CoordGridT = std::vector<gol::Coord>;
	
	private:
		GridT m_grid;

	public:
		Logic(const CoordGridT & grid);
		~Logic() noexcept = default;

		void makeNewGrid();
		[[nodiscard]] const GridT & getNewGrid();
		[[nodiscard]] constexpr const GridT & getGrid() const noexcept
		{
			return this->m_grid;
		}

		[[nodiscard]] constexpr bool empty() const noexcept
		{
			return this->m_grid.empty();
		}
	};
}
