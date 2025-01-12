#pragma once

enum class BufferType
{
	GlobalParam,
	TransformParam,
	MateriaParam,
	CameraParam,
	MateriaSubParam,
	SpriteParam

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

	shared_ptr<CBufferPool> GetBufferPool(BufferType type) { return _map[type]; }
	shared_ptr<TextureBufferPool> GetTextureBufferPool() { return _textureBufferPool; }
	shared_ptr<DescritporTable> GetTable() { return _table; }

private:
	unordered_map<BufferType, shared_ptr<CBufferPool>> _map;
	shared_ptr<TextureBufferPool> _textureBufferPool;
	shared_ptr<DescritporTable> _table;
};

