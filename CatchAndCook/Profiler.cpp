#include "pch.h"
#include "Profiler.h"

Profiler::Profiler()
{
}
std::unique_ptr<Profiler> Profiler::main = nullptr;

void Profiler::Init(HWND hParent, HINSTANCE hinstance)
{
	hwnd = WinMain(hParent, hinstance);
}


void Profiler::Set(const string& key, BlockTag tag)
{
	auto [it, tf] = blockInfoTable.try_emplace({key,{}});
	auto& info = it->second;
	info.tag = tag;
	info._startTime = Time::main->GetClockNow();
	info.depth = blockStack.size();

	blockOrder.push_back(key);
	blockStack.push(key);
}

void Profiler::Fin()
{
	auto& info = blockInfoTable[blockStack.top()];
	info._endTime = Time::main->GetClockNow();

	blockStack.pop();
}

void Profiler::Reset()
{
	InvalidateRect(hwnd,nullptr, TRUE);
	UpdateWindow(hwnd);
	//SendMessageA(hwnd, WM_PAINT, 0, 0);

	for(auto& data : blockInfoTable)
		data.second.UpdateSmoothTime();
	blockOrder.clear();
}

LRESULT Profiler::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(msg == WM_PAINT) {
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd,&ps);


		vec2 padding = vec2(10,10);

		TextOutA(hdc,padding.x, padding.y,"CPU/CPU Performance Profiler",strlen("CPU/CPU Performance Profiler"));

		vec2 progressOffset = vec2(padding.x,max(10, 30));
		vec2 progressSize = vec2(((main->_winSize.x - padding.x) - progressOffset.x), 15);


		float total = 0;
		for(auto& data : main->blockOrder)
		{
			auto& info = main->blockInfoTable[data];
			total += info.GetIntervalSmoothTime();
		}
		RECT rect = {
			0,(int)progressOffset.y,
			(int)main->_winSize.x,(int)(progressOffset.y +  progressSize.y)
		};
		FillRect(hdc,&rect,(HBRUSH)GetStockObject(WHITE_BRUSH));

		int currentX = progressOffset.x;
		for(auto& data : main->blockOrder)
		{
			auto& info = main->blockInfoTable[data];
			int barWidth =  static_cast<int>(info.GetIntervalSmoothTime() / total * progressSize.x);
			rect = {currentX,(int)progressOffset.y,currentX + barWidth,(int)progressOffset.y + (int)progressSize.y};


			if(info.tag == BlockTag::CPU)
				SetDCBrushColor(hdc, RGB( 150, 190, 230 - 5 * info.depth));
			if(info.tag == BlockTag::GPU)
				SetDCBrushColor(hdc,RGB(180, 240, 150));

			FillRect(hdc,&rect,(HBRUSH)GetStockObject(DC_BRUSH));
			currentX += barWidth;
		}

		int index = 0;
		for(auto& data : main->blockOrder)
		{
			auto& info = main->blockInfoTable[data];
			string text = std::format("{0} : {1:.3}ms         \0",data, std::round(info.GetIntervalSmoothTime() * 100000) / 100);
			TextOutA(hdc,padding.x + 10 * info.depth,50 + (index * 20),text .c_str(),text.size());
			++index;
		}

		EndPaint(hWnd,&ps);
	} else if(msg == WM_DESTROY) {
		PostQuitMessage(0);
	} else {
		return DefWindowProcA(hWnd,msg,wParam,lParam);
	}
	return 0;
}

HWND Profiler::WinMain(HWND hParent,HINSTANCE hInst)
{
	WNDCLASSA wc = {0};
	wc.lpfnWndProc = Profiler::WndProc; // static 함수여야 함
	wc.hInstance = hInst;
	wc.lpszClassName = "MySubWndClass";
	RegisterClassA(&wc);

	int desiredClientWidth  = static_cast<int>(main->_winSize.x);
	int desiredClientHeight = static_cast<int>(main->_winSize.y);

	// 클라이언트 영역 크기를 담을 RECT 구조체 초기화
	RECT rc = {0,0,desiredClientWidth,desiredClientHeight};

	// 지정한 창 스타일에 맞게 전체 창 크기를 조정합니다.
	// WS_OVERLAPPEDWINDOW 스타일을 사용하고, 메뉴가 없으므로 FALSE를 전달합니다.
	AdjustWindowRect(&rc,WS_OVERLAPPEDWINDOW,FALSE);

	// 조정된 전체 창 크기는 다음과 같습니다.
	int windowWidth  = rc.right - rc.left;
	int windowHeight = rc.bottom - rc.top;

	HWND hwnd = CreateWindowA(
		"MySubWndClass",          // 등록한 클래스 이름
		"Sub Window",             // 창 제목
		WS_OVERLAPPEDWINDOW | WS_VISIBLE, // 창 스타일
		10,10,                   // 창의 좌측 상단 좌표
		windowWidth,windowHeight,// 전체 창 크기 (비클라이언트 영역 + 테두리, 타이틀 바 등)
		hParent,                  // 부모 창 핸들 (없다면 nullptr)
		nullptr,
		hInst,
		nullptr
	);

	if(!hwnd) {
		MessageBoxA(nullptr,"CreateWindowA failed!","Error",MB_OK);
		return nullptr;
	}

	ShowWindow(hwnd,SW_SHOW);
	UpdateWindow(hwnd);
	SetFocus(hParent);
	return hwnd;
}
