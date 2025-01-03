#pragma once

class Time
{
public:

	static unique_ptr<Time> main;


	void Init(HWND hwnd);
	void Update();

	uint32 GetFps() { return _fps; }
	auto GetTime() { return _gameStartClock; }
	float GetDeltaTime() { return _deltaTime; }

private:
	uint64	_frequency = 0;
	uint64	_prevCount = 0;
	float	_deltaTime = 0.f;

	std::chrono::time_point<std::chrono::steady_clock> _gameStartClock{};
	float deltaTimeLimitValue = 0.33333;


private:

	HWND	_hwnd;
	uint32	_frameCount = 0;
	float	_frameTime = 0.f;
	uint32	_fps = 0;


};

