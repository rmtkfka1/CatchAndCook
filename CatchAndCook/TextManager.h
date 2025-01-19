#pragma once


class TextHandle
{
public:
	TextHandle() {};
	~TextHandle() { delete[] sysMemory; sysMemory = nullptr; }
	
	BYTE* sysMemory;
	ComPtr<ID2D1Bitmap1> bitMapGpu;
	ComPtr<ID2D1Bitmap1> bitMapRead;
	ComPtr<ID2D1SolidColorBrush> brush;
	ComPtr<IDWriteTextFormat> font;

	uint32 width;
	uint32 height;

	float fontSize;
};

enum class FontColor
{
	WHITE,
	BLACK,
};


class TextManager
{
public:
	static unique_ptr<TextManager> main;

public:
	void Init();
	shared_ptr<TextHandle> AllocTextStrcture(int width ,int height , FontColor color , float size );

private:
	void InitD2D();


private:
	ComPtr<ID2D1Device2> _device = nullptr;
	ComPtr<ID2D1DeviceContext2> _context = nullptr;

	ComPtr<ID2D1Bitmap1> _bitMapGpu = nullptr;     //렌더타겟 (GPU 메모리)
	ComPtr<ID2D1Bitmap1> _bitMapReadBack = nullptr; //렌더타겟 (ReadBack :시스템 메모리)

	ComPtr<IDWriteFactory5> _factory = nullptr;

	unordered_map<FontColor, ComPtr<ID2D1SolidColorBrush>> _brushMap;

	//추가적인 공부필요
	 
	//IDWriteFontCollection1* m_pFontCollection = nullptr;  //폰트관리.
	//DWRITE_LINE_METRICS*	m_pLineMetrics = nullptr;
	//DWORD					m_dwMaxLineMetricsNum = 0;
};

