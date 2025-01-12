#include "pch.h"
#include "Time.h"


unique_ptr<Time> Time::main=nullptr;

void Time::Init()
{
	::QueryPerformanceFrequency(&_frequency);
	::QueryPerformanceCounter(&_prevCount); // CPU Å¬·°
	_gameStartClock = std::chrono::steady_clock::now();

}

void Time::Update()
{
	LARGE_INTEGER currentCount;
	::QueryPerformanceCounter(&currentCount);

	_deltaTime = (currentCount.QuadPart - _prevCount.QuadPart) / static_cast<double>(_frequency.QuadPart);
	_deltaTime = std::min(_deltaTime, deltaTimeLimitValue);
	_totalTime += _deltaTime;
	_prevCount = currentCount;
	_fps = std::round(1.0 / _deltaTime);
}


