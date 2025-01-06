#pragma once


class RenderTarget;
class RootSignature;
class Shader;
class Mesh;
class Texture;
class GameObject;
class BufferManager;

class Core
{
public:

	static unique_ptr<Core> main;

	Core();
	~Core();

	void Init(HWND hwnd);

	void RenderBegin();
	void Render();
	void RenderEnd();

	void Fence();
	void FlushResCMDQueue();

public:
	ComPtr<ID3D12Device5>& GetDevice() { return _device; }
	ComPtr<ID3D12GraphicsCommandList>& GetCmdList() { return _cmdList; }
	ComPtr<ID3D12GraphicsCommandList>& GetResCmdList() { return _resCmdList; }
	shared_ptr<RenderTarget>& GetRenderTarget() { return _renderTarget; }
	shared_ptr<RootSignature>& GetRootSignature() { return _rootSignature; };
	shared_ptr< BufferManager>& GetBufferManager() { return _bufferManager; }

	

private:
	void InitDirectX12();

private:
	void CreateDevice(bool EnableDebugLayer, bool EnableGBV);
	void CreateSwapChain();
	void CreateCmdQueue();
	void SetDebugLayerInfo();

private:
	shared_ptr<RenderTarget> _renderTarget;
	shared_ptr<RootSignature> _rootSignature;
	shared_ptr<BufferManager> _bufferManager;

private:
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

	//temp
	shared_ptr<Shader> _shader;
	vector<shared_ptr<GameObject>> _gameObjects;
	shared_ptr<Mesh> _mesh;
	shared_ptr<Texture> _texture;

};

