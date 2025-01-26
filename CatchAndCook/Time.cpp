#include "pch.h"
#include "Time.h"


unique_ptr<Time> Time::main=nullptr;

void Time::Init()
{
	::QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&_frequency));
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&_prevCount)); // CPU Å¬·°
}

void Time::Update()
{
	uint64 currentCount;
	::QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&currentCount));

	_deltaTime = (currentCount - _prevCount) / static_cast<float>(_frequency);
	_prevCount = currentCount;

	_frameCount++;
	_frameTime += _deltaTime;

	if (_frameTime > 1.0f)
	{
		WCHAR wchTxt[64];
		swprintf_s(wchTxt, 64, L"FPS: %u", _fps);
		SetWindowText(Core::main->GetHandle(), wchTxt);

		_fps = static_cast<uint32>(_frameCount / _frameTime);
		_frameTime = 0.f;
		_frameCount = 0;
	}

    _time += _deltaTime;


}


