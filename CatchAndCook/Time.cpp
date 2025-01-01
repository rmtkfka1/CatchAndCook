#include "pch.h"
#include "Time.h"


unique_ptr<Time> Time::main=nullptr;

void Time::Init(HWND hwnd)
{
	_hwnd = hwnd;
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount)); // CPU Å¬·°

	_gameStartClock = std::chrono::steady_clock::now();

}

void Time::Update()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	_deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);

	_deltaTime = std::min(_deltaTime, deltaTimeLimitValue);

	_prevCount = currentCount;

	_frameCount++;
	_frameTime += _deltaTime;

	if (_frameTime > 1.0f)
	{
		_fps = static_cast<uint32>(_frameCount / _frameTime);
		_frameTime = 0.f;
		_frameCount = 0;
	}

}


