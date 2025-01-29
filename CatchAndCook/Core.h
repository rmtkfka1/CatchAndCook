#pragma once
#include "RenderTarget.h"


class Component;
class RenderTarget;
class RootSignature;
class Shader;
class Mesh;
class Texture;
class GameObject;
class BufferManager;
class MeshRenderer;
class Material;

class Core
{
public:

	static unique_ptr<Core> main;

	Core();
	~Core();

	void Init(HWND hwnd);

	void RenderBegin();
	void RenderEnd();

	void Fence();
	void FlushResCMDQueue();
	void ResizeWindowSize();
	HWND GetHandle() { return _hwnd; }

public:
	ComPtr<ID3D12Device5>& GetDevice() { return _device; }
	ComPtr<ID3D12GraphicsCommandList>& GetCmdList() { return _cmdList; }
	ComPtr<ID3D12GraphicsCommandList>& GetResCmdList() { return _resCmdList; }
	ComPtr<ID3D12CommandQueue>& GetCmdQueue() { return _cmdQueue; }
	shared_ptr<RenderTarget>& GetRenderTarget() { return _renderTarget; }
	shared_ptr<RootSignature>& GetRootSignature() { return _rootSignature; };
	shared_ptr<BufferManager>& GetBufferManager() { return _bufferManager; }

	void SetPipelineState(Shader* shader);
	void SetPipelineSetting(Material* material);

private:
	void AdjustWinodwSize();
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
	uint32 _swapChainFlags;

	ComPtr<ID3D12CommandAllocator> _resCmdMemory{};
	ComPtr<ID3D12GraphicsCommandList> _resCmdList{};

	ComPtr<ID3D12Fence> _fence = nullptr;
	HANDLE _fenceEvent = nullptr;
	uint64 _fenceValue = 0;

	HWND _hwnd{0};

	Shader* currentShader = nullptr;
	int currentStencil = -1;
public:
	
public:
	//temp
	//array<std::vector<std::shared_ptr<Packet>>, RENDER_PASS::Count> passPackets;
};

