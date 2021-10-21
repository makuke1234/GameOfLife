#include "argumentSystem.hpp"

#include "../include/w32/textconverter.hpp"
#include "../include/w32/fastfile.hpp"

#include <shellapi.h>

#include <stdexcept>
#include <sstream>

std::pair<int, char **> gol::cmdArgsToVec(LPCWSTR args, gol::ArgAllocT allocator)
{
	int argc;
	wchar_t ** wideArgs = ::CommandLineToArgvW(args, &argc);

	char ** argv = static_cast<char **>(allocator(argc * sizeof(char *)));
	if (argv == nullptr)
	{
		throw std::bad_alloc{};
	}

	std::string tempStr;
	for (int i = 0; i < argc; ++i)
	{
		tempStr.assign(utf::conv<std::string>(wideArgs[i]));
		argv[i] = static_cast<char *>(allocator((tempStr.size() + 1) * sizeof(char)));
		if (argv[i] == nullptr)
		{
			throw std::bad_alloc{};
		}
		std::copy(tempStr.begin(), tempStr.end() + 1, argv[i]);
	}

	// Release wide args
	::LocalFree(wideArgs);

	return std::make_pair(argc, argv);
}

gol::ArgSystem::ArgSystem(LPCWSTR args)
	: m_origPair{ gol::cmdArgsToVec(args, &ArgSystem::newFunc) }
{
	this->m_tokeniser = argparser::Tokeniser(this->m_origPair);
	// Do argument parsing here
	auto token{ this->m_tokeniser.tokenise(argparser::regex::dashTemplate("input=", "in=", "infile="), 1) };
	if (token == true)
	{
		this->fileName = utf::conv<std::wstring>(token.get());
	}
}
gol::ArgSystem::~ArgSystem()
{
	for (int i = 0; i < this->m_origPair.first; ++i)
	{
		delete[] this->m_origPair.second[i];
	}
	delete[] this->m_origPair.second;
	
	this->m_origPair.first = 0;
	this->m_origPair.second = nullptr;
}

[[nodiscard]] bool gol::ArgSystem::empty() const noexcept
{
	return this->fileName.empty();
}


[[nodiscard]] gol::Logic::CoordGridT gol::ArgSystem::parseFile() const
{
	if (this->fileName.empty())
		return {};

	w32::FastFile inputf(this->fileName);

	std::string infoStr;
	{
		auto info{ inputf.readAll() };
		infoStr = { info.begin(), info.end() };
	}

	inputf.close();

	std::istringstream infoStream{ infoStr };

	gol::Logic::CoordGridT grid;

	// Start file parsing here
	while (infoStream.good())
	{
		gol::Coord coord;
		infoStream >> coord.x >> coord.y;
		grid.emplace_back(coord);
	}


	return grid;
}
