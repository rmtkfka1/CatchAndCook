#pragma once
class Game
{

public:

	void Init(HWND hwnd);
	void Run();
	void Release();

private:
	HWND _hwnd;

};

