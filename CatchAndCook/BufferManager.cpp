#include "pch.h"
#include "BufferManager.h"
#include "BufferPool.h"
#include "Material.h"
#include "Camera.h"
#include "Scene.h"
#include "Sprite.h"
void BufferManager::Init()
{

	_textureBufferPool = make_shared<TextureBufferPool>();
	_textureBufferPool->Init(3000, 5, 5);

	CreateBufferPool(BufferType::GlobalParam, sizeof(GlobalParam), 1);
	CreateBufferPool(BufferType::TransformParam, sizeof(Matrix), 50000);
	CreateBufferPool(BufferType::MateriaParam, sizeof(MaterialParams), 3000);
	CreateBufferPool(BufferType::MateriaSubParam, sizeof(TestSubMaterialParam), 3000);
	CreateBufferPool(BufferType::CameraParam, sizeof(CameraParams), 10);
	CreateBufferPool(BufferType::SpriteTextureParam, sizeof(SprtieTextureParam), 3000);
	CreateBufferPool(BufferType::SpriteWorldParam, sizeof(SpriteWorldParam), 3000);

	CreateBufferPool_Static(BufferType::BoneParam, sizeof(SpriteWorldParam), 3000);

	{
		_table = make_shared<DescritporTable>();
		_table->Init(20000);
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
