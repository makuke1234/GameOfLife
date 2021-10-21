#include "mainClass.hpp"

int WINAPI wWinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPWSTR lpCmdArgs, int nCmdShow)
{
	gol::GameOfLifeClass game(hInst, hPrevInstance, lpCmdArgs, nCmdShow);

	return game.run();
}
