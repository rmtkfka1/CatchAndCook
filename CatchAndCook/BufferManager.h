#pragma once
#include "StructuredBuffer.h"

class InstanceBufferPool;

enum class BufferType
{
	GlobalParam,
	TransformParam,
	MateriaParam,
	BoneParam,
	CameraParam,
	MateriaSubParam,
	SpriteWorldParam,
	SpriteTextureParam,
	TestParam,
	DefaultMaterialParam,
	LightParam,
	TerrainDetailsParam,
	SeaParam,
	FogParam,
	UnderWaterParam,
	PlantInfo,

	// Instance
	TransformInstanceParam,
	GizmoInstanceParam,
	GizmoTextInstanceParam,
	InstanceOffsetParam,
};

class CBufferPool;
class DescritporTable;
class TextureBufferPool;

class BufferManager
{

public:
	void Init();
	void Reset();

	void CreateBufferPool(BufferType type, uint32 size, uint32 count);
	void CreateBufferPool_Static(BufferType type, uint32 size, uint32 count);

	shared_ptr<CBufferPool>& GetBufferPool(BufferType type) { return _map[type]; }
	shared_ptr<CBufferPool>& GetBufferPool_Static(BufferType type) { return _map_notReset[type]; }
	shared_ptr<CBufferPool>& CreateAndGetBufferPool(BufferType type,uint32 size,uint32 count);
	shared_ptr<CBufferPool>& CreateAndGetBufferPool_Static(BufferType type, uint32 size, uint32 count);

	void CreateInstanceBufferPool(BufferType type, uint32 elementSize, uint32 elementCount, uint32 bufferCount);
	shared_ptr<InstanceBufferPool>& GetInstanceBufferPool(BufferType type) {return _instanceMap[type];}

	void CreateStructuredBufferPool(BufferType type, const string& name, uint32 elementSize, uint32 elementCount);
	shared_ptr<StructuredBuffer>& GetStructuredBufferPool(BufferType type) {
		return _structuredMap[type];
	}
	BufferType& GetStructuredNameToBufferType(const string& name){
		return _structuredNameMappingTable[name];
	}

	shared_ptr<TextureBufferPool>& GetTextureBufferPool() { return _textureBufferPool; }
	shared_ptr<DescritporTable>& GetTable() { return _table; }

private:
	unordered_map<BufferType, shared_ptr<CBufferPool>> _map;
	unordered_map<BufferType, shared_ptr<CBufferPool>> _map_notReset;
	unordered_map<BufferType, shared_ptr<InstanceBufferPool>> _instanceMap;
	unordered_map<BufferType, shared_ptr<StructuredBuffer>> _structuredMap;

	unordered_map<string, BufferType> _structuredNameMappingTable;

	shared_ptr<TextureBufferPool> _textureBufferPool;
	shared_ptr<DescritporTable> _table;
};

