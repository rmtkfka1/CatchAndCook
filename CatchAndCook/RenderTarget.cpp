#include "pch.h"
#include "RenderTarget.h"
#include "Texture.h"
#include "Core.h"

RenderTarget::RenderTarget()
{
}
RenderTarget::~RenderTarget()
{
}
void RenderTarget::Init(ComPtr<IDXGISwapChain3>& swapchain)
{
	_swapChain = swapchain;

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
	{
		_RenderTargets[i] = make_shared<Texture>();
	}

	_DSTexture = make_shared<Texture>();

	_viewport = D3D12_VIEWPORT{ 0.0f,0.0f,static_cast<float>(WINDOW_WIDTH),static_cast<float>(WINDOW_HEIGHT), 0,1.0f };
	_scissorRect = D3D12_RECT{ 0,0, static_cast<LONG>(WINDOW_WIDTH),static_cast<LONG>(WINDOW_HEIGHT) };

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
		swapchain->GetBuffer(i, IID_PPV_ARGS(&_RenderTargets[i]->GetResource()));

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
	{
		_RenderTargets[i]->CreateStaticTexture(SWAP_CHAIN_FORMAT, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::RTV, true, true);
	}

	_RenderTargetIndex = swapchain->GetCurrentBackBufferIndex();
	_DSTexture->CreateStaticTexture(DEPTH_STENCIL_FORMAT, D3D12_RESOURCE_STATE_DEPTH_WRITE, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::DSV, false, true);
}

void RenderTarget::ResizeWindowSize(ComPtr<IDXGISwapChain3> swapchain, uint32 swapChainFlags)
{
	Core::main->GetBufferManager()->GetTextureBufferPool()->FreeDSVHandle(_DSTexture->GetSharedDSVHandle());

	for (uint32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
	{
		Core::main->GetBufferManager()->GetTextureBufferPool()->FreeRTVHandle(_RenderTargets[i]->GetRTVCpuHandle());
		_RenderTargets[i]->GetResource().Reset();
	}

	ThrowIfFailed(swapchain->ResizeBuffers(SWAP_CHAIN_FRAME_COUNT, WINDOW_WIDTH, WINDOW_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, swapChainFlags));

	_RenderTargetIndex = swapchain->GetCurrentBackBufferIndex();

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; i++)
		swapchain->GetBuffer(i, IID_PPV_ARGS(&_RenderTargets[i]->GetResource()));

	for (int32 i = 0; i < SWAP_CHAIN_FRAME_COUNT; ++i)
	{
		_RenderTargets[i]->CreateStaticTexture(SWAP_CHAIN_FORMAT, D3D12_RESOURCE_STATE_COMMON, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::RTV, true, true);
	}

	_viewport = D3D12_VIEWPORT{ 0.0f,0.0f,static_cast<float>(WINDOW_WIDTH),static_cast<float>(WINDOW_HEIGHT), 0,1.0f };
	_scissorRect = D3D12_RECT{ 0,0, static_cast<LONG>(WINDOW_WIDTH),static_cast<LONG>(WINDOW_HEIGHT) };
	_DSTexture->CreateStaticTexture(DEPTH_STENCIL_FORMAT, D3D12_RESOURCE_STATE_DEPTH_WRITE, WINDOW_WIDTH, WINDOW_HEIGHT, TextureUsageFlags::DSV, false, true);
}



void RenderTarget::RenderBegin()
{
	ComPtr<ID3D12GraphicsCommandList> cmdList = Core::main->GetCmdList();

	//ClearDepth();

	const float BackColor[] = { 0.0f, 0.0f, 0.0f, 0.0f };

	_RenderTargets[_RenderTargetIndex]->ResourceBarrier(D3D12_RESOURCE_STATE_RENDER_TARGET);

	cmdList->RSSetViewports(1, &_viewport);
	cmdList->RSSetScissorRects(1, &_scissorRect);
	cmdList->ClearRenderTargetView(_RenderTargets[_RenderTargetIndex]->GetRTVCpuHandle(), BackColor, 0, nullptr);
	cmdList->OMSetRenderTargets(1, &_RenderTargets[_RenderTargetIndex]->GetRTVCpuHandle(), FALSE, &_DSTexture->GetSharedDSVHandle());
}

void RenderTarget::RenderEnd()
{
	ComPtr<ID3D12GraphicsCommandList> cmdList = Core::main->GetCmdList();

	_RenderTargets[_RenderTargetIndex]->ResourceBarrier(D3D12_RESOURCE_STATE_PRESENT);
}

void RenderTarget::ClearDepth()
{
	ComPtr<ID3D12GraphicsCommandList> cmdList = Core::main->GetCmdList();
	cmdList->ClearDepthStencilView(_DSTexture->GetSharedDSVHandle(), D3D12_CLEAR_FLAG_DEPTH , 1.0f, 0, 0, nullptr);
}

void RenderTarget::ChangeIndex()
{
	_RenderTargetIndex = _swapChain->GetCurrentBackBufferIndex();
}