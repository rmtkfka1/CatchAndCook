#include "pch.h"
#include "Game.h"
#include "Core.h"
void Game::Init(HWND hwnd)
{
	Time::main = make_unique<Time>();
	Time::main->Init(hwnd);
	Input::main = make_unique<Input>();
	Core::main = make_unique<Core>();
	Core::main->Init(hwnd);
}

void Game::Run()
{
	Input::main->Update();
	Time::main->Update();
	Core::main->RenderBegin();
	Core::main->Render();
	Core::main->RenderEnd();

}

void Game::Release()
{
	Input::main.reset(nullptr);
	Time::main.reset(nullptr);
	Core::main.reset(nullptr);
}
