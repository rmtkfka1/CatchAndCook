#pragma once

#include "ICBufferInjector.h"
#include "Material.h"

struct TestSubMaterialParam
{
	vec2 uv;
};

CBUFFER_INJECTOR("popo", TestSubMaterialParam, 256, BufferType::MateriaSubParam, std::shared_ptr<Material>,
	data.uv = Vector2(source->GetPropertyVector("uv"));
)


struct DefaultMaterialParam
{
	Vector4 color;
	Vector4 _baseMapST;
};

CBUFFER_INJECTOR("DefaultMaterialParam", DefaultMaterialParam, 256, BufferType::DefaultMaterialParam, std::shared_ptr<Material>,
	// data <- source
	data.color = Vector4(source->GetPropertyVector("_Color"));
	data._baseMapST = Vector4(source->GetPropertyVector("_BaseMap_ST"));
)

struct TerrainDetailsParam
{
	int detailsCount;
	Vector4 tileST[4];
	//Vector4 color;
};

CBUFFER_INJECTOR("TerrainDetailsParam", TerrainDetailsParam, 256, BufferType::TerrainDetailsParam, std::shared_ptr<Material>,
	/*data.tileST = Vector2(source->GetPropertyVector("tileST"));
	data.tileOffset = Vector2(source->GetPropertyVector("tileOffset"));
	data.color = Vector4(source->GetPropertyVector("_Color"));*/
)