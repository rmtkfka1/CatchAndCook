#pragma once
class Texture;

namespace RENDER_PASS
{
	enum PASS
	{
		Shadow = 1 << 0,
		Deffered = 1 << 1,
		Forward = 1 << 2,
		Transparent = 1 << 3,
		PostProcessing = 1 << 4,
		UI = 1 << 5,
		Debug = 1 << 6,
	};

	const int Count = 7;

	inline PASS operator|(PASS a, PASS b) {
		return static_cast<PASS>(static_cast<int>(a) | static_cast<int>(b));
	}


	inline bool HasFlag(PASS value, PASS flag) {
		return (static_cast<int>(value) & static_cast<int>(flag)) != 0;
	}

	inline int ToIndex(PASS pass) {
		switch (pass) {
		case Shadow: return 0;
		case Deffered: return 1;
		case Forward: return 2;
		case Transparent: return 3;
		case PostProcessing: return 4;
		case UI: return 5;
		case Debug: return 6;
		default: assert(false && "Invalid PASS value");
		}
	}
}

class RenderTarget
{

public:
	RenderTarget();
	~RenderTarget();

public:
	void Init(ComPtr<IDXGISwapChain3>& swapchain);
	void ResizeWindowSize(ComPtr<IDXGISwapChain3> swapchain, uint32 swapChainFlags);
	void ChangeIndex();
	void RenderBegin();
	void RenderEnd();
	void ClearDepth();

	array<shared_ptr<Texture>, SWAP_CHAIN_FRAME_COUNT> GetRenderTargetAll() { return _RenderTargets; }
	shared_ptr<Texture> GetRenderTarget() { return _RenderTargets[_RenderTargetIndex]; }
	shared_ptr<Texture> GetDSTexture() { return _DSTexture; };
private:

	ComPtr<IDXGISwapChain3> _swapChain;
	array<shared_ptr<Texture>, SWAP_CHAIN_FRAME_COUNT> _RenderTargets;
	shared_ptr<Texture> _DSTexture;
	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};
	uint32	_RenderTargetIndex = 0;
};