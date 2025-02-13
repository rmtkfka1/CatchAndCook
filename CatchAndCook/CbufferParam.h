#pragma once

#include "ICBufferInjector.h"
#include "Material.h"

#define p1(n) float padding##n;
#define p2(n) float padding##n[2];
#define p3(n) float padding##n[3];

struct alignas(16) TransformParam
{
	Matrix localToWorld;
	Matrix worldToLocal;
	Vector3 worldPos; p1(0);
};

struct BoneParam
{
	Matrix boneMatrixs[256];
	Matrix boneInvertMatrixs[256];
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
	Vector3 fieldSize; p1(0);
	int detailsCount; p3(1);
	int blendCount; p3(2);
	Vector4 tileST[12];
	Vector4 textureActive[12];
	//Vector4 color;
};

CBUFFER_INJECTOR("TerrainDetailsParam",TerrainDetailsParam,256,BufferType::TerrainDetailsParam, std::shared_ptr<Material>,
	data.fieldSize = Vector3(source->GetPropertyVector("fieldSize"));
	data.detailsCount = source->GetPropertyInt("detailsCount");
	data.blendCount = source->GetPropertyInt("blendCount");
	for(int i=0;i<12;i++) {
		data.tileST[i] = Vector4(source->GetPropertyVector(std::format("_tileST{0}", i)));
		data.textureActive[i] = Vector4(source->GetPropertyVector(std::format("_active{0}",i)));
	}
	/*data.tileOffset = Vector2(source->GetPropertyVector("tileOffset"));
	data.color = Vector4(source->GetPropertyVector("_Color"));*/
)