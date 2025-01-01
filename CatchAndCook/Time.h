#pragma once

class Time
{
public:

	static unique_ptr<Time> main;


	void Init(HWND hwnd);
	void Update();

	uint32 GetFps() { return _fps; }
	float GetDeltaTime() { return _deltaTime; }

private:
	uint64	_frequency = 0;
	uint64	_prevCount = 0;
	float	_deltaTime = 0.f;

private:

	HWND	_hwnd;
	uint32	_frameCount = 0;
	float	_frameTime = 0.f;
	uint32	_fps = 0;

public:
	uint32 _objectCount = 0;
};

