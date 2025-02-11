#pragma once

#include "ICBufferInjector.h"
#include "Material.h"

#define p1(n) float padding##n;
#define p2(n) float padding##n[2];
#define p3(n) float padding##n[3];

struct alignas(16) TransformParam
{
	Matrix localToWorld;
	Vector3 worldPos; p1(0);
};


struct alignas(16) TestSubMaterialParam
{
	vec2 uv;
	p2(0);
};

CBUFFER_INJECTOR("popo", TestSubMaterialParam, 256, BufferType::MateriaSubParam, std::shared_ptr<Material>,
	data.uv = Vector2(source->GetPropertyVector("uv"));
)


struct alignas(16) DefaultMaterialParam
{
	Vector4 color;
	Vector4 _baseMapST;
};

CBUFFER_INJECTOR("DefaultMaterialParam", DefaultMaterialParam, 256, BufferType::DefaultMaterialParam, std::shared_ptr<Material>,
	// data <- source
	data.color = Vector4(source->GetPropertyVector("_Color"));
	data._baseMapST = Vector4(source->GetPropertyVector("_BaseMap_ST"));
)

struct alignas(16) TerrainDetailsParam
{
	int detailsCount; p3(0);
	Vector4 tileST[4];
	//Vector4 color;
};

CBUFFER_INJECTOR("TerrainDetailsParam", TerrainDetailsParam, 256, BufferType::TerrainDetailsParam, std::shared_ptr<Material>,
	/*data.tileST = Vector2(source->GetPropertyVector("tileST"));
	data.tileOffset = Vector2(source->GetPropertyVector("tileOffset"));
	data.color = Vector4(source->GetPropertyVector("_Color"));*/
)