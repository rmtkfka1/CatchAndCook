#pragma once

template <typename T>
class StructuredBuffer
{
public:
	void Init(vector<T>& cpuData)
	{
		uint32 elementSize = (sizeof(T));
		uint32 elementCount = static_cast<uint32>(cpuData.size());

		_bufferSize = sizeof(T) * elementCount;

		_count = static_cast<uint32>(cpuData.size());

		auto & device = Core::main->GetDevice();

		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(_bufferSize, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS),
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&_structuredBuffer)));

		ThrowIfFailed(device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(_bufferSize),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&_uploadBuffer)));
		 
		auto cmdList = Core::main->GetResCmdList();

		CD3DX12_RANGE readRange(0, 0);

		ThrowIfFailed(_uploadBuffer->Map(0, &readRange, &_mappedData));

		memcpy(_mappedData, cpuData.data(), _bufferSize);

		cmdList->CopyResource(_structuredBuffer.Get(), _uploadBuffer.Get());

		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			_structuredBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE));

		Core::main->FlushResCMDQueue();

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING; // 기본 매핑 추가
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.NumElements = elementCount;
		srvDesc.Buffer.StructureByteStride = elementSize;
		srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;

		Core::main->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_srvHandle);

		device->CreateShaderResourceView(_structuredBuffer.Get(), &srvDesc, _srvHandle);

		D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.NumElements = elementCount;
		uavDesc.Buffer.StructureByteStride = elementSize;
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;

		Core::main->GetBufferManager()->GetTextureBufferPool()->AllocSRVDescriptorHandle(&_uavHandle);

		device->CreateUnorderedAccessView(_structuredBuffer.Get(), nullptr, &uavDesc, _uavHandle);
	}

	void Upload(vector<T>& cpuData)
	{
		auto& cmdList = Core::main->GetCmdList();

		memcpy(_mappedData, cpuData.data(), _bufferSize);

		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			_structuredBuffer.Get(),
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE,
			D3D12_RESOURCE_STATE_COPY_DEST));

		cmdList->CopyResource(_structuredBuffer.Get(), _uploadBuffer.Get());

		cmdList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(
			_structuredBuffer.Get(),
			D3D12_RESOURCE_STATE_COPY_DEST,
			D3D12_RESOURCE_STATE_ALL_SHADER_RESOURCE));
	}

public:
	D3D12_CPU_DESCRIPTOR_HANDLE& GetSRVHandle() { return _srvHandle; }
	D3D12_CPU_DESCRIPTOR_HANDLE& GetUAVHandle() { return _uavHandle; }

private:
	ComPtr<ID3D12Resource> _structuredBuffer;
	ComPtr<ID3D12Resource> _uploadBuffer;
	uint32 _count;
	uint64 _bufferSize;
	D3D12_CPU_DESCRIPTOR_HANDLE  _srvHandle;
	D3D12_CPU_DESCRIPTOR_HANDLE  _uavHandle;

	void* _mappedData;
};

