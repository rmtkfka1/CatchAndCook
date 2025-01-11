#include "pch.h"
#include "BufferManager.h"
#include "BufferPool.h"
#include "Material.h"
void BufferManager::Init()
{

	_textureBufferPool = make_shared<TextureBufferPool>();
	_textureBufferPool->Init(255, 5, 5);

	CreateBufferPool(BufferType::TransformParam, sizeof(Matrix), 255);
	CreateBufferPool(BufferType::MateriaParam, sizeof(MaterialParams), 255);

	{
		_table = make_shared<DescritporTable>();
		_table->Init(255);
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
