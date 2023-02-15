#include "game.hpp"

#include <algorithm>

gol::Logic::Logic(const CoordGridT & grid)
{
	CoordT x{ 100 }, y{ 100 };
	// Find the max size
	std::for_each(grid.cbegin(), grid.cend(), [&x, &y](const auto & i)
	{
		x = std::max(x, i.x);
		y = std::max(y, i.y);
	});

	this->m_grid = GridT(y + 2, CellVec(x + 2));

	std::for_each(grid.cbegin(), grid.cend(), [this](const auto & i)
	{
		this->m_grid[i.y+1][i.x+1].setAlive();

		++this->m_grid[i.y][i.x  ].elems.m_neighbours;
		++this->m_grid[i.y][i.x+1].elems.m_neighbours;
		++this->m_grid[i.y][i.x+2].elems.m_neighbours;

		++this->m_grid[i.y+1][i.x  ].elems.m_neighbours;
		++this->m_grid[i.y+1][i.x+2].elems.m_neighbours;

		++this->m_grid[i.y+2][i.x  ].elems.m_neighbours;
		++this->m_grid[i.y+2][i.x+1].elems.m_neighbours;
		++this->m_grid[i.y+2][i.x+2].elems.m_neighbours;
	});
}

void gol::Logic::makeNewGrid()
{
	GridT newGrid(this->m_grid.size() + 2, CellVec(this->m_grid.front().size() + 2));
	{
		auto & firstRow{ newGrid.front() };
		const auto & origFCol{ this->m_grid.front() };
		firstRow.front().elems.m_neighbours = origFCol.front().isAlive();
		firstRow.back ().elems.m_neighbours = origFCol.back ().isAlive();
		for (std::size_t x = 1; x < (firstRow.size() - 1); ++x)
		{
			if (x > 1)
				firstRow[x].elems.m_neighbours += origFCol[x-2].isAlive();
			firstRow[x].elems.m_neighbours += origFCol[x-1].isAlive();
			if (x < origFCol.size())
				firstRow[x].elems.m_neighbours += origFCol[x  ].isAlive();
		}
	}
	for (std::size_t y = 0; y < this->m_grid.size(); ++y)
	{
		const auto & origin{ this->m_grid[y] };
		auto & destination{ newGrid[y+1] };

		std::copy(origin.cbegin(), origin.cend(), destination.begin() + 1);

		// Neighbour stuff
		if (y > 0)
		{
			destination.front().elems.m_neighbours += this->m_grid[y-1].front().isAlive();
			destination.back ().elems.m_neighbours += this->m_grid[y-1].back ().isAlive();
		}
		destination.front().elems.m_neighbours += origin.front().isAlive();
		destination.back ().elems.m_neighbours += origin.back ().isAlive();
		if (y < (this->m_grid.size() - 1))
		{
			destination.front().elems.m_neighbours += this->m_grid[y+1].front().isAlive();
			destination.back ().elems.m_neighbours += this->m_grid[y+1].back ().isAlive();
		}
	}
	{
		auto & lastRow{ newGrid.back() };
		const auto & origLCol{ this->m_grid.back() };
		lastRow.front().elems.m_neighbours = origLCol.front().isAlive();
		lastRow.back ().elems.m_neighbours = origLCol.back ().isAlive();
		for (std::size_t x = 1; x < (lastRow.size() - 1); ++x)
		{
			if (x > 1)
				lastRow[x].elems.m_neighbours += origLCol[x-2].isAlive();
			lastRow[x].elems.m_neighbours += origLCol[x-1].isAlive();
			if (x < origLCol.size())
				lastRow[x].elems.m_neighbours += origLCol[x  ].isAlive();
		}
	}


	for (CoordT y = 0; y < this->m_grid.size(); ++y)
	{
		const auto & row{ this->m_grid[y] };
		for (CoordT x = 0; x < row.size(); ++x)
		{
			if (row[x].futureHasToLiveIsDead())
			{
				newGrid[y+1][x+1].setAlive();

				++newGrid[y  ][x  ].elems.m_neighbours;
				++newGrid[y  ][x+1].elems.m_neighbours;
				++newGrid[y  ][x+2].elems.m_neighbours;
			
				++newGrid[y+1][x  ].elems.m_neighbours;
				++newGrid[y+1][x+2].elems.m_neighbours;
			
				++newGrid[y+2][x  ].elems.m_neighbours;
				++newGrid[y+2][x+1].elems.m_neighbours;
				++newGrid[y+2][x+2].elems.m_neighbours;
			}
			else if (row[x].futureHasToDieIsAlive())
			{
				newGrid[y+1][x+1].setDead();

				--newGrid[y  ][x  ].elems.m_neighbours;
				--newGrid[y  ][x+1].elems.m_neighbours;
				--newGrid[y  ][x+2].elems.m_neighbours;
			
				--newGrid[y+1][x  ].elems.m_neighbours;
				--newGrid[y+1][x+2].elems.m_neighbours;
			
				--newGrid[y+2][x  ].elems.m_neighbours;
				--newGrid[y+2][x+1].elems.m_neighbours;
				--newGrid[y+2][x+2].elems.m_neighbours;
			}
		}
	}

	std::size_t xBegin{ 1 }, xSize{ this->m_grid.front().size() };
	std::size_t yBegin{ 1 }, ySize{ this->m_grid.size() };
	for (auto i : newGrid.front())
	{
		if (i.m_value)
		{
			yBegin = 0;
			++ySize;
			break;
		}
	}
	for (auto i : newGrid.back())
	{
		if (i.m_value)
		{
			++ySize;
			break;
		}
	}
	for (std::size_t y = 0; y < newGrid.size(); ++y)
	{
		if (newGrid[y].front().m_value)
		{
			xBegin = 0;
			++xSize;
			break;
		}
	}
	for (std::size_t y = 0; y < newGrid.size(); ++y)
	{
		if (newGrid[y].back().m_value)
		{
			++xSize;
			break;
		}
	}

	/*for (auto it = newGrid.begin(); it != newGrid.end(); ++it)
	{
		std::cout << '|';
		for (auto it2 = it->begin(); it2 != it->end(); ++it2)
		{
			std::cout << (it2->isAlive() ? 'A' : ' ');
		}
		std::cout << "|\n";
	}
	std::cout << "old\n";
	for (auto it = this->m_grid.begin(); it != this->m_grid.end(); ++it)
	{
		std::cout << '|';
		for (auto it2 = it->begin(); it2 != it->end(); ++it2)
		{
			std::cout << (it2->isAlive() ? 'A' : ' ');
		}
		std::cout << "|\n";
	}

	std::cout << "neighbours\n";
	for (auto it = newGrid.begin(); it != newGrid.end(); ++it)
	{
		std::cout << '|';
		for (auto it2 = it->begin(); it2 != it->end(); ++it2)
		{
			std::printf("%2d", it2->elems.m_neighbours);
		}
		std::cout << "|\n";
	}
	std::cout << "old\n";
	for (auto it = this->m_grid.begin(); it != this->m_grid.end(); ++it)
	{
		std::cout << '|';
		for (auto it2 = it->begin(); it2 != it->end(); ++it2)
		{
			std::printf("%2d", it2->elems.m_neighbours);
		}
		std::cout << "|\n";
	}*/

	// Move the new grid to be the old one
	if (yBegin == 0 && ySize == newGrid.size() && xBegin == 0 && xSize == newGrid.front().size())
		this->m_grid = std::move(newGrid);
	else if (xBegin == 0 && xSize == newGrid.front().size())
	{
		this->m_grid.clear();
		this->m_grid.reserve(ySize);
		for (auto it = newGrid.begin() + yBegin, itEnd = newGrid.begin() + ySize; it != itEnd; ++it)
		{
			this->m_grid.emplace_back(std::move(*it));
		}
	}
	else
	{
		this->m_grid = GridT(ySize, CellVec(xSize));
		for (auto it1 = newGrid.begin() + yBegin, it2 = this->m_grid.begin(); it2 != this->m_grid.end(); ++it1, ++it2)
		{
			for (auto it3 = it1->begin() + xBegin, it4 = it2->begin(); it4 != it2->end(); ++it3, ++it4)
			{
				*it4 = *it3;
			}
		}
	}
}

[[nodiscard]] const gol::Logic::GridT & gol::Logic::getNewGrid()
{
	this->makeNewGrid();
	return this->getGrid();
}
