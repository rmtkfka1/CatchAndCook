#pragma once
class Texture;

class RenderTarget
{

public:
	RenderTarget();
	~RenderTarget();

public:
	void Init(ComPtr<IDXGISwapChain3>& swapchain);
	void Resize(DWORD BackBufferWidth, DWORD BackBufferHeight, ComPtr<IDXGISwapChain3> swapchain, UINT	_swapChainFlags);
	void ChangeIndex();
	void RenderBegin();
	void RenderEnd();
	void ClearDepth();
private:

	ComPtr<IDXGISwapChain3> _swapChain;
	shared_ptr<Texture> _RenderTargets[SWAP_CHAIN_FRAME_COUNT];  
	shared_ptr<Texture> _DSTexture;
	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};
	uint32	_RenderTargetIndex = 0;
};