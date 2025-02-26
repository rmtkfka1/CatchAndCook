#pragma once
#include "Core.h"

class StructuredBuffer
{
public:
	
	void Init(uint32 size, uint32 elementCount);

	template<class T>
	void AddData(const T& data)
	{
		memcpy(_mappedData + _writeByteOffset, &data, sizeof(data));
		_writeOffsetIndex++;
		_writeByteOffset += sizeof(data);
	}

	template<class T>
	void AddAtData(const T& data, int index)
	{
		memcpy(_mappedData + index * sizeof(data), &data, sizeof(data));
	}

	void Clear()
	{
		_writeOffsetIndex = 0;
		_writeByteOffset = 0;
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

	void* _mappedData;
	int _writeOffsetIndex = 0;
	int _writeByteOffset = 0;
};
