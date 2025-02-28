

#pragma once

enum class BlockTag
{
	CPU,
	GPU,
};

struct BlockInfo
{
	BlockTag tag = BlockTag::CPU;
	double _startTime = 0;
	double _endTime = 0;

	int depth = 0;

	double GetIntervalTime() const {
		return _endTime - _startTime;
	};

	double smoothIntervalTime = 0;
	double GetIntervalSmoothTime() const {
		return smoothIntervalTime;
	};
	void UpdateSmoothTime(){
		double delta = Time::main->GetDeltaTime() * 20;
		smoothIntervalTime = std::lerp(smoothIntervalTime,GetIntervalTime(),std::min(delta, 1.0));
	}
};

class PerformanceProfiler
{
	unordered_map<string, BlockInfo> blockInfoTable;

	BlockInfo* lastInfo;
	vector<string> blockOrder;
	stack<string> blockStack;
public:
	static std::unique_ptr<PerformanceProfiler> main;

	HWND hwnd;

	PerformanceProfiler();
	void Init(HWND hParent, HINSTANCE hinstance);
	void Set(const string& key,BlockTag tag = BlockTag::CPU);
	void Fin();

	void Reset();



	Vector2 _winSize = Vector2(400,450);
    static LRESULT CALLBACK WndProc(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);
	static HWND WINAPI WinMain(HWND hParent, HINSTANCE hInst);
};

