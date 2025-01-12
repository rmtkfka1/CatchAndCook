#pragma once

class Time
{
public:
	static unique_ptr<Time> main;


	void Init();
	void Update();

	uint64 GetFps() { return _fps; }
	auto GetStartTime() { return _gameStartClock; }
	double GetDeltaTime() { return _deltaTime; }
	double GetTotalTime() { return _totalTime; }

private:
	LARGE_INTEGER _frequency = LARGE_INTEGER();
	LARGE_INTEGER _prevCount = LARGE_INTEGER();
	double	_deltaTime = 0.33333;
	double	_totalTime = 0.0;

	std::chrono::time_point<std::chrono::steady_clock> _gameStartClock{};
	double deltaTimeLimitValue = 0.33333;


private:
	uint64	_fps = 0;
};

