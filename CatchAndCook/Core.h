#pragma once

class TextureBufferPool;
class RenderTarget;
class RootSignature;

class Core
{
public:
	Core();
	~Core();

	void Init(HWND hwnd);

	void RenderBegin();
	void RenderEnd();

	void Fence();
	void FlushResCMDQueue();

public:
	ComPtr<ID3D12Device5>& GetDevice() { return _device; }
	ComPtr<ID3D12GraphicsCommandList>& GetCmdList() { return _cmdList; }
	ComPtr<ID3D12GraphicsCommandList>& GetresCmdList() { return _resCmdList; }
	shared_ptr<TextureBufferPool>& GetTextureBufferPool() { return _textureBufferPool; }
	shared_ptr<RenderTarget>& GetRenderTarget() { return _renderTarget; }

protected:


private:
	void InitDirectX12();

private:
	void CreateDevice(bool EnableDebugLayer, bool EnableGBV);
	void CreateSwapChain();
	void CreateCmdQueue();
	void SetDebugLayerInfo();

protected:
	shared_ptr<TextureBufferPool> _textureBufferPool;
	shared_ptr<RenderTarget> _renderTarget;
	shared_ptr<RootSignature> _rootSignature;
protected:
	ComPtr<ID3D12Device5> _device = nullptr;
	ComPtr<IDXGIFactory4> _factory = nullptr;
	D3D_FEATURE_LEVEL _FeatureLevel = D3D_FEATURE_LEVEL_11_0;
	DXGI_ADAPTER_DESC1 _adapterDesc = {};

	ComPtr<ID3D12CommandQueue> _cmdQueue = nullptr;

	ComPtr<ID3D12CommandAllocator> _cmdMemory = {};
	ComPtr<ID3D12GraphicsCommandList> _cmdList = {};
	ComPtr<IDXGISwapChain3> _swapChain = nullptr;

	ComPtr<ID3D12CommandAllocator> _resCmdMemory{};
	ComPtr<ID3D12GraphicsCommandList> _resCmdList{};

	ComPtr<ID3D12Fence> _fence = nullptr;
	HANDLE _fenceEvent = nullptr;
	uint64 _fenceValue = 0;

	HWND _hwnd{};

};
