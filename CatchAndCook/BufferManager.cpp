#include "pch.h"
#include "BufferManager.h"
#include "Bone.h"
#include "BufferPool.h"
#include "Material.h"
#include "Camera.h"
#include "Scene.h"
#include "Sprite.h"
#include "testComponent.h"
#include "LightManager.h"
void BufferManager::Init()
{

	_textureBufferPool = make_shared<TextureBufferPool>();
	_textureBufferPool->Init(2000, 5, 5);

	CreateBufferPool(BufferType::GlobalParam, sizeof(GlobalParam), 1);
	CreateBufferPool(BufferType::TransformParam, sizeof(TransformParam), 20480);
	CreateBufferPool(BufferType::MateriaParam, sizeof(MaterialParams), 20000);
	CreateBufferPool(BufferType::MateriaSubParam, sizeof(TestSubMaterialParam),20000);
	CreateBufferPool(BufferType::CameraParam, sizeof(CameraParams), 10);
	CreateBufferPool(BufferType::SpriteTextureParam, sizeof(SprtieTextureParam), 255);
	CreateBufferPool(BufferType::SpriteWorldParam, sizeof(SpriteWorldParam), 255);
	CreateBufferPool(BufferType::LightParam,sizeof(LightParams),1);

	CreateBufferPool(BufferType::BoneParam, sizeof(BoneParam), 256);

	CreateInstanceBufferPool(BufferType::TransformInstanceParam,sizeof(Instance_Transform),2000,128);
	CreateInstanceBufferPool(BufferType::GizmoInstanceParam,sizeof(Instance_Gizmo),10000,1);

	{
		_table = make_shared<DescritporTable>();
		_table->Init(8*20000);
	}


}

void BufferManager::Reset()
{
	_table->Reset();

	for (auto& ele : _map)
	{
		ele.second->Reset();
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

