#include "pch.h"
#include "Game.h"

void Game::Init(HWND hwnd)
{

	Time::main = make_unique<Time>();
	Time::main->Init(hwnd);

	Input::main = make_unique<Input>();


}

void Game::Run()
{

	Input::main->Update();
	Time::main->Update();
}
