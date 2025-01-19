#pragma once
class TextManager
{
public:
	static unique_ptr<TextManager> main;

public:
	void Init();

private:
	void InitD2D();
	void InitDWrite();



private:
	ComPtr<ID2D1Device2> _device = nullptr;
	ComPtr<ID2D1DeviceContext2> _context = nullptr;

	ComPtr<ID2D1Bitmap1> _bitMapGpu = nullptr;     //렌더타겟 (GPU 메모리)
	ComPtr<ID2D1Bitmap1> _bitMapReadBack = nullptr; //렌더타겟 (ReadBack :시스템 메모리)

	//IDWriteFontCollection1* m_pFontCollection = nullptr;  //폰트관리.
	ComPtr<ID2D1SolidColorBrush> _brush = nullptr;	  //폰트색상

	ComPtr<IDWriteFactory5> _factory = nullptr;
	//DWRITE_LINE_METRICS*	m_pLineMetrics = nullptr;
	//DWORD					m_dwMaxLineMetricsNum = 0;
};

