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
	uint32 count = 1;
	bool isAlloc = false;
};

class CBufferPool
{

public:
	CBufferPool();
	~CBufferPool();
	void Init(uint32 size, uint32 count);
	void Reset();
	CBufferContainer* Alloc(uint32 count = 1);
	//void Free(CBufferContainer* cbuffer);

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
*        DescritporTable		 *
*                                *
**********************************/

struct tableContainer
{
	D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle;
};


class DescritporTable
{


public:
	void Init(uint32 count);
	tableContainer Alloc(uint32 count);
	void CopyHandle(D3D12_CPU_DESCRIPTOR_HANDLE& destHandle, D3D12_CPU_DESCRIPTOR_HANDLE& sourceHandle ,uint32 index);
	void Reset();

	ComPtr<ID3D12DescriptorHeap>& GetDescriptorHeap() { return _heap; }
private:
	ComPtr<ID3D12DescriptorHeap> _heap;
	uint32 _currentIndex=0; //현재몇개할당됬는지
	uint32 _count=0;  // handle 갯수
	uint32 _size = 0; 

	D3D12_CPU_DESCRIPTOR_HANDLE _cpuHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE _gpuHandle;

};


/*************************
*                        *
*  InstanceBuffer Pool   *
*                        *
**************************/

struct InstanceBufferContainer
{
	//D3D12_GPU_VIRTUAL_ADDRESS   GPUAdress; //View 에다가 바로꼽아줄때 사용
	D3D12_VERTEX_BUFFER_VIEW	_bufferView = {};
	void* ptr;
	uint32 elementCount = 1;
	bool isAlloc = false;

	int writeOffset = 0;
	int writeIndex = 0;
	template<class T>
	void AddData(const T& data)
	{
		assert(writeOffset < elementCount * sizeof(T));
		memcpy(static_cast<char*>(ptr) + writeOffset, &data, sizeof(T));
		writeOffset += sizeof(T);
		++writeIndex;
	}
	template<class T>
	void SetData(const T& data, int index)
	{
		assert(index < elementCount);
		memcpy(static_cast<char*>(ptr) + index * sizeof(T), &data, sizeof(T));
	}
	void SetIndex(int index, int size)
	{
		writeIndex = index;
		writeOffset = index * size;
	}
	void Free(){
		isAlloc = false;
		Clear();
	}
	void Clear(){
		writeOffset = 0;
		writeIndex = 0;
	}
};



class InstanceBufferPool
{

public:
	InstanceBufferPool();
	~InstanceBufferPool();
	void Init(uint32 size,uint32 elementCount, uint32 bufferCount);
	void Reset();
	InstanceBufferContainer* Alloc();

private:
	vector<InstanceBufferContainer> _containers;

	ComPtr<ID3D12Resource> _resource;
	void* offsetPtr = nullptr;

	uint32 _currentIndex = 0;

	uint32 _elementSize = 0;
	uint32 _elementCount = 0;
	uint32 _bufferSize = 0;
	uint32 _bufferCount = 0;

};

