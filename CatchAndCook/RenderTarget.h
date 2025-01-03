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

	vector<shared_ptr<Texture>> GetRenderTarget() { return vector<std::shared_ptr<Texture>>{_RenderTargets.begin(), _RenderTargets.end()}; };
	shared_ptr<Texture> GetDSTexture() { return _DSTexture; };
private:

	ComPtr<IDXGISwapChain3> _swapChain;
	array<shared_ptr<Texture>, SWAP_CHAIN_FRAME_COUNT> _RenderTargets;
	shared_ptr<Texture> _DSTexture;
	D3D12_VIEWPORT	_viewport = {};
	D3D12_RECT		_scissorRect = {};
	uint32	_RenderTargetIndex = 0;
};