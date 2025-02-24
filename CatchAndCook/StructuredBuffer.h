#pragma once

template<typename T>
class StructuredBuffer
{
public:
	void Init(int32 count)
	{
		_elementSize = (sizeof(T));
		_elementCount = count;

		uint64 bufferSize = _elementSize * _elementCount;

		auto & device = Core::main->GetDevice();

		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(bufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&_structuredBuffer)));

		CD3DX12_RANGE readRange(0,0);

		ThrowIfFailed(_structuredBuffer->Map(0,&readRange,&_mappedData));

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; 
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.NumElements = _elementCount;
		srvDesc.Buffer.StructureByteStride = _elementSize;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;

		Core::main->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_srvHandle);

		device->CreateShaderResourceView(_structuredBuffer.Get(),&srvDesc,_srvHandle);
	}


	void Upload()
	{
		auto& cmdList = Core::main->GetCmdList();
		memcpy(_mappedData,_cpuData.data(),_cpuData.size()* _elementSize);
	}

	void AddData(T& data)
	{
		_cpuData.push_back(data);
	}

	void Clear()
	{
		_cpuData.clear();
	}

public:
	D3D12_CPU_DESCRIPTOR_HANDLE& GetSRVHandle() { return _srvHandle; }
	int32 GetCount() {return _cpuData.size();}

private:
	ComPtr<ID3D12Resource> _structuredBuffer;
	uint32 _elementCount;
	uint64 _elementSize;
	D3D12_CPU_DESCRIPTOR_HANDLE  _srvHandle;

	void* _mappedData;

	vector<T> _cpuData;
};

