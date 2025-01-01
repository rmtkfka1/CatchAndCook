#include "pch.h"
#include "BufferPool.h"
#include "Core.h"


/*************************
*                        *
*  TextureBuffer Pool    *
*                        *
**************************/


void TextureBufferPool::Init(int32 SrvUavCount, int32 RTVCount ,int32 DSVCount)
{
	{
		_srvHeap.heapSize = SrvUavCount;
		_srvHeap.handleIncrementSize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		_srvHeap.indexGenator.resize(_srvHeap.heapSize, -1);

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = SrvUavCount;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  // Ensure shader visibility if needed

		ThrowIfFailed(core->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_srvHeap.heap)));
	}

	{
		_rtvHeap.heapSize = RTVCount;
		_rtvHeap.handleIncrementSize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		_rtvHeap.indexGenator.resize(_rtvHeap.heapSize, -1);

		D3D12_DESCRIPTOR_HEAP_DESC RTVHeapDesc = {};
		RTVHeapDesc.NumDescriptors = RTVCount;
		RTVHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		RTVHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  // Ensure shader visibility if needed

		ThrowIfFailed(core->GetDevice()->CreateDescriptorHeap(&RTVHeapDesc, IID_PPV_ARGS(&_rtvHeap.heap)));
	}

	{
		_dsvHeap.heapSize = DSVCount;
		_dsvHeap.handleIncrementSize = core->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		_dsvHeap.indexGenator.resize(_dsvHeap.heapSize, -1);

		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
		srvHeapDesc.NumDescriptors = DSVCount;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;  // Ensure shader visibility if needed

		ThrowIfFailed(core->GetDevice()->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&_dsvHeap.heap)));

	}

}

void TextureBufferPool::FreeSRVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE start = _srvHeap.heap->GetCPUDescriptorHandleForHeapStart();
	DWORD index = (DWORD)(handle.ptr - start.ptr) / _srvHeap.handleIncrementSize;

	assert(index >= 0);

	_srvHeap.indexGenator[index] = -1;
	_srvHeap.currentIndex--;
}

void TextureBufferPool::FreeRTVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE start = _rtvHeap.heap->GetCPUDescriptorHandleForHeapStart();
	DWORD index = (DWORD)(handle.ptr - start.ptr) / _rtvHeap.handleIncrementSize;

	assert(index >= 0);

	_rtvHeap.indexGenator[index] = -1;
	_rtvHeap.currentIndex--;

}

void TextureBufferPool::FreeDSVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle)
{
	D3D12_CPU_DESCRIPTOR_HANDLE start = _dsvHeap.heap->GetCPUDescriptorHandleForHeapStart();
	DWORD index = (DWORD)(handle.ptr - start.ptr) / _dsvHeap.handleIncrementSize;

	assert(index >= 0);

	_dsvHeap.indexGenator[index] = -1;
	_dsvHeap.currentIndex--;
}

void TextureBufferPool::AllocSRVDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* handle)
{

	assert(_srvHeap.currentIndex < _srvHeap.heapSize);

	int32 index = AllocSRV();

	if (index == -1) {
		throw std::runtime_error("No free descriptor handles available");
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHandle(_srvHeap.heap->GetCPUDescriptorHandleForHeapStart(), index, _srvHeap.handleIncrementSize);
	*handle = DescriptorHandle;
	_srvHeap.currentIndex++;

}

void TextureBufferPool::AllocRTVDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* handle)
{

	assert(_rtvHeap.currentIndex < _rtvHeap.heapSize);

	int32 index = AllocRTV();

	if (index == -1) {
		throw std::runtime_error("No free descriptor handles available");
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHandle(_rtvHeap.heap->GetCPUDescriptorHandleForHeapStart(), index, _rtvHeap.handleIncrementSize);
	*handle = DescriptorHandle;
	_rtvHeap.currentIndex++;
}

void TextureBufferPool::AllocDSVDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* handle)
{

	assert(_dsvHeap.currentIndex < _dsvHeap.heapSize);

	int32 index = AllocDSV();

	if (index == -1) {
		throw std::runtime_error("No free descriptor handles available");
	}

	CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHandle(_dsvHeap.heap->GetCPUDescriptorHandleForHeapStart(), index, _dsvHeap.handleIncrementSize);
	*handle = DescriptorHandle;
	_dsvHeap.currentIndex++;

}



int32 TextureBufferPool::AllocSRV()
{
	for (int i = 0; i < _srvHeap.indexGenator.size(); ++i)
	{
		if (_srvHeap.indexGenator[i] == -1)
		{
			_srvHeap.indexGenator[i] = 1;
			return i;
		}
	}

	return -1;  // No available slots
}

int32 TextureBufferPool::AllocRTV()
{
	for (int i = 0; i < _rtvHeap.indexGenator.size(); ++i)
	{
		if (_rtvHeap.indexGenator[i] == -1)
		{
			_rtvHeap.indexGenator[i] = 1;
			return i;
		}
	}

	return -1;  // No available slots
}

int32 TextureBufferPool::AllocDSV()
{
	for (int i = 0; i < _dsvHeap.indexGenator.size(); ++i)
	{
		if (_dsvHeap.indexGenator[i] == -1)
		{
			_dsvHeap.indexGenator[i] = 1;
			return i;
		}
	}

	return -1;  // No available slots
}


///////////////////////////////////////////////////////////


/*************************
*                        *
*    DescriptorTable     *
*                        *
**************************/

