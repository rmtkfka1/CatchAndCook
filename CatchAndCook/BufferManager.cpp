#include "pch.h"
#include "BufferManager.h"

#include "Bone.h"
#include "BufferPool.h"
#include "Material.h"
#include "Camera.h"
#include "Scene.h"
#include "Sprite.h"
#include "testComponent.h"
void BufferManager::Init()
{

	_textureBufferPool = make_shared<TextureBufferPool>();
	_textureBufferPool->Init(2000, 5, 5);

	CreateBufferPool(BufferType::GlobalParam, sizeof(GlobalParam), 1);
	CreateBufferPool(BufferType::TransformParam, sizeof(Matrix), 2048);
	CreateBufferPool(BufferType::MateriaParam, sizeof(MaterialParams), 2000);
	CreateBufferPool(BufferType::MateriaSubParam, sizeof(TestSubMaterialParam),2000);
	CreateBufferPool(BufferType::CameraParam, sizeof(CameraParams), 10);
	CreateBufferPool(BufferType::SpriteTextureParam, sizeof(SprtieTextureParam), 255);
	CreateBufferPool(BufferType::SpriteWorldParam, sizeof(SpriteWorldParam), 255);
	CreateBufferPool(BufferType::TestParam, sizeof(test), 255);

	CreateBufferPool(BufferType::BoneParam, sizeof(BoneParam), 256);

	{
		_table = make_shared<DescritporTable>();
		_table->Init(8*2000);
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
