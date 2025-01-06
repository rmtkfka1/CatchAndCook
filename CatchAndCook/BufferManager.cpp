#include "pch.h"
#include "BufferManager.h"
#include "BufferPool.h"
#include "Material.h"
void BufferManager::Init()
{

	_textureBufferPool = make_shared<TextureBufferPool>();
	_textureBufferPool->Init(255, 5, 5);

	{
		shared_ptr<CBufferPool> transformBuffer = make_shared<CBufferPool>();
		transformBuffer->Init(sizeof(Matrix), 255);
		_map[BufferType::TransformParam] = transformBuffer;
	}

	{
		shared_ptr<CBufferPool> materialParams = make_shared<CBufferPool>();
		materialParams->Init(sizeof(MaterialParams), 255);
		_map[BufferType::MateriaParam] = materialParams;
	}

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
