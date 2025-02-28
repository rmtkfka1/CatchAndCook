#pragma once
class Game
{
public:

	void Init(HWND hwnd);
	void PrevUpdate();
	void Run();
	void Release();
	void CameraUpdate(); //temp
	void SetHandle(HWND hwnd,HINSTANCE hInst);
private:
	HWND _hwnd;
	HINSTANCE _hInstance;

	bool _fullScreen = false;
	bool _quit = false;
};

