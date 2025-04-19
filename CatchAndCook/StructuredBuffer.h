#pragma once
#include "Core.h"

class StructuredBuffer
{
public:
	
	void Init(uint32 size, uint32 elementCount);

	template<class T>
	void AddData(const T& data)
	{
		memcpy(static_cast<uint8*>(_mappedData) + _writeOffsetIndex * sizeof(T), &data, sizeof(T));
		_writeOffsetIndex++;
	}

	void Clear()
	{
		_writeOffsetIndex = 0;
	}

	int GetOffset() const
	{
		return _writeOffsetIndex;
	}

public:
	D3D12_CPU_DESCRIPTOR_HANDLE& GetSRVHandle() { return _srvHandle; }
	int32 GetCount() const {return _writeOffsetIndex;}

private:
	ComPtr<ID3D12Resource> _structuredBuffer;
	D3D12_CPU_DESCRIPTOR_HANDLE  _srvHandle;
	uint32 _elementCount;
	uint64 _elementSize;

	void* _mappedData = nullptr;
	int _writeOffsetIndex = 0;
};
