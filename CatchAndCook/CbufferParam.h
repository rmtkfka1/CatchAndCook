#pragma once

#include "ICBufferInjector.h"
#include "Material.h"

struct TestSubMaterialParam
{
	vec2 uv;
};

CBUFFER_INJECTOR("popo", TestSubMaterialParam, BufferType::MateriaSubParam, std::shared_ptr<Material>,
	param.uv = Vector2(source->GetPropertyVector("uv"));
)


struct DefaultMaterialParam
{
	Vector4 color;
	Vector4 _baseMapST;
};

CBUFFER_INJECTOR("DefaultMaterialParam", DefaultMaterialParam, BufferType::MateriaSubParam, std::shared_ptr<Material>,
	param.color = Vector4(source->GetPropertyVector("_Color"));
	param._baseMapST = Vector4(source->GetPropertyVector("_BaseMap_ST"));
)


struct TerrainDetailsParam
{
	Vector2 tileSize;
	Vector2 tileOffset;
	Vector4 color;
};
CBUFFER_INJECTOR("TerrainDetailsParam", TerrainDetailsParam, BufferType::MateriaSubParam, std::shared_ptr<Material>,
	param.tileSize = Vector2(source->GetPropertyVector("tileSize"));
	param.tileOffset = Vector2(source->GetPropertyVector("tileOffset"));
	param.color = Vector4(source->GetPropertyVector("_Color"));
)