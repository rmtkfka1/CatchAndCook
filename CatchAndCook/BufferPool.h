#pragma once




/*************************
*                        *
*  ConstantBuffer Pool   *
*                        *
**************************/


/*************************
*                        *
*  TextureBuffer  Pool   *
*                        *
**************************/

struct heapStructure
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
	heapStructure _srvHeap;
	heapStructure _rtvHeap;
	heapStructure _dsvHeap;

};





/*********************************
*                                *
*    GraphicsDescriptorTable     *
*                                *
**********************************/



