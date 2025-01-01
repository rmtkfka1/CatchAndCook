#include "pch.h"
#include "Game.h"
#include "Core.h"
void Game::Init(HWND hwnd)
{

	Time::main = make_unique<Time>();
	Time::main->Init(hwnd);

	Input::main = make_unique<Input>();

	core->Init(hwnd);

}

void Game::Run()
{
	Input::main->Update();
	Time::main->Update();

	core->RenderBegin();

	core->RenderEnd();

}
