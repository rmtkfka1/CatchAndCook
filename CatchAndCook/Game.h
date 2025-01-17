#pragma once
class Game
{
public:

	void Init(HWND hwnd);
	void PrevUpdate();
	void Run();
	void Release();
	void CameraUpdate(); //temp
private:
	HWND _hwnd;

	bool _fullScreen = false;
	bool _quit = false;
};

