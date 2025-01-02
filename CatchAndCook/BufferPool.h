#pragma once


/*************************
*                        *
*  ConstantBuffer Pool   *
*                        *
**************************/

struct CBufferContainer
{
	D3D12_CPU_DESCRIPTOR_HANDLE	CPUHandle; //Descriptor Table 에다가 핸들 복사할떄 사용
	D3D12_GPU_VIRTUAL_ADDRESS   GPUAdress; //View 에다가 바로꼽아줄때 사용
	void* ptr;
};

class CBufferPool
{

public:
	CBufferPool();
	~CBufferPool();
	void Init(uint32 size, uint32 count);
	CBufferContainer* Alloc(uint32 count=1);

private:

	vector<CBufferContainer> _container;

	ComPtr<ID3D12DescriptorHeap> _heap{};
	ComPtr<ID3D12Resource> _resource;

	uint32 _currentIndex = 0;
	uint32 _elementSize = 0;
	uint32 _count=0;

};


/*************************
*                        *
*  TextureBuffer  Pool   *
*                        *
**************************/

struct HeapStructure
{
	vector<uint32> indexGenator;
	ComPtr<ID3D12DescriptorHeap> heap;

	int32 heapSize = 0;
	int32 handleIncrementSize = 0;
	int32 currentIndex = 0;

};


class TextureBufferPool
{

public:
	void Init(int32 SrvUavCount , int32 RTVCount , int32 DSVCount);

	void FreeSRVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle);
	void FreeRTVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle);
	void FreeDSVHandle(D3D12_CPU_DESCRIPTOR_HANDLE handle);

	void AllocSRVDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* hanlde);
	void AllocRTVDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* hanlde);
	void AllocDSVDescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE* hanlde);

private:

	int32 AllocSRV();
	int32 AllocRTV();
	int32 AllocDSV();

private:
	HeapStructure _srvHeap;
	HeapStructure _rtvHeap;
	HeapStructure _dsvHeap;

};





/*********************************
*                                *
*    GraphicsDescriptorTable     *
*                                *
**********************************/



