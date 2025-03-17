#include "pch.h"
#include "BufferManager.h"
#include "Bone.h"
#include "BufferPool.h"
#include "Material.h"
#include "Camera.h"
#include "InstancingManager.h"
#include "Scene.h"
#include "Sprite.h"
#include "testComponent.h"
#include "LightManager.h"
#include "WaterController.h"
#include "ComputeManager.h"
void BufferManager::Init()
{

	_textureBufferPool = make_shared<TextureBufferPool>();
	_textureBufferPool->Init(2000,10,5);

	{
		CreateBufferPool(BufferType::GlobalParam,sizeof(GlobalParam),1);
		CreateBufferPool(BufferType::TransformParam,sizeof(TransformParam),1000);
		CreateBufferPool(BufferType::MateriaParam,sizeof(MaterialParams),1000);
		CreateBufferPool(BufferType::MateriaSubParam,sizeof(TestSubMaterialParam),1000);
		CreateBufferPool(BufferType::CameraParam,sizeof(CameraParams),10);
		CreateBufferPool(BufferType::SpriteTextureParam,sizeof(SprtieTextureParam),255);
		CreateBufferPool(BufferType::SpriteWorldParam,sizeof(SpriteWorldParam),255);
		CreateBufferPool(BufferType::LightParam,sizeof(LightParams),1);

		CreateBufferPool(BufferType::BoneParam,sizeof(BoneParam),256);
		CreateBufferPool(BufferType::InstanceOffsetParam,sizeof(InstanceOffsetParam),10000);
		CreateBufferPool(BufferType::SeaParam, sizeof(TerrainDetailsParam), 1);
		CreateBufferPool(BufferType::FogParam, sizeof(FogParam), 1);
		CreateBufferPool(BufferType::UnderWaterParam, sizeof(UnderWaterParam), 1);

	}

	{
		CreateStructuredBufferPool(BufferType::TransformParam,"TransformDatas",sizeof(Instance_Transform),20000);
	}

	{
		CreateInstanceBufferPool(BufferType::TransformInstanceParam,sizeof(Instance_Transform),10000,128);
		CreateInstanceBufferPool(BufferType::GizmoInstanceParam,sizeof(Instance_Gizmo),500000,1);
	}
	
	{
		_table = make_shared<DescritporTable>();
		_table->Init(60000);
	}


}

void BufferManager::Reset()
{
	_table->Reset();

	for (auto& ele : _map) {
		ele.second->Reset();
	}

	for(auto& ele : _structuredMap) {
		ele.second->Clear();
	}
}

void BufferManager::CreateBufferPool(BufferType type, uint32 size, uint32 count)
{
	shared_ptr<CBufferPool> transformBuffer = make_shared<CBufferPool>();
	transformBuffer->Init(size, count);
	_map[type] = transformBuffer;
}

void BufferManager::CreateBufferPool_Static(BufferType type, uint32 size, uint32 count)
{
	shared_ptr<CBufferPool> transformBuffer = make_shared<CBufferPool>();
	transformBuffer->Init(size, count);
	_map_notReset[type] = transformBuffer;
}

void BufferManager::CreateInstanceBufferPool(BufferType type,uint32 elementSize,uint32 elementCount,uint32 bufferCount)
{
	shared_ptr<InstanceBufferPool> transformBuffer = make_shared<InstanceBufferPool>();
	transformBuffer->Init(elementSize,elementCount,bufferCount);
	_instanceMap[type] = transformBuffer;
}

void BufferManager::CreateStructuredBufferPool(BufferType type,const string& name,uint32 elementSize, uint32 elementCount)
{
	shared_ptr<StructuredBuffer> strBuffer = make_shared<StructuredBuffer>();
	strBuffer->Init(elementSize, elementCount);
	_structuredMap[type] = strBuffer;
	_structuredNameMappingTable[name] = BufferType::TransformParam;
}


shared_ptr<CBufferPool>& BufferManager::CreateAndGetBufferPool(BufferType type, uint32 size, uint32 count)
{
	if(_map.contains(type))
		return _map[type];

	shared_ptr<CBufferPool> transformBuffer = make_shared<CBufferPool>();
	transformBuffer->Init(size,count);
	_map[type] = transformBuffer;
	return _map[type];
}

shared_ptr<CBufferPool>& BufferManager::CreateAndGetBufferPool_Static(BufferType type, uint32 size, uint32 count)
{
	if(_map_notReset.contains(type))
		return _map_notReset[type];
	shared_ptr<CBufferPool> transformBuffer = make_shared<CBufferPool>();
	transformBuffer->Init(size,count);
	_map_notReset[type] = transformBuffer;
	return _map_notReset[type];
}

