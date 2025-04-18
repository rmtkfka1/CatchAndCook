﻿#pragma once

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


struct alignas(16) DefaultMaterialParam
{
	Vector4 color;
	Vector4 _baseMapST;
};

CBUFFER_INJECTOR("DefaultMaterialParam", DefaultMaterialParam, 1000, BufferType::DefaultMaterialParam, std::shared_ptr<Material>,
	// data <- source
	data.color = Vector4(source->GetPropertyVector("_Color"));
	data._baseMapST = Vector4(source->GetPropertyVector("_BaseMap_ST"));
)


struct alignas(16) EnvMaterialParam
{
	Vector4 color;
	Vector4 _baseMapST;
	Vector4 emissionColor = Vector4(0, 0, 0, 0);
	float emission = 0;
	Vector3 padding;
};

CBUFFER_INJECTOR("EnvMaterialParam", EnvMaterialParam, 1024, BufferType::EnvMaterialParam, std::shared_ptr<Material>,
	// data <- source
	data.color = Vector4(source->GetPropertyVector("_Color"));
	data._baseMapST = Vector4(source->GetPropertyVector("_BaseMap_ST"));
	data.emissionColor = Vector4(source->GetPropertyVector("_EmissionColor"));
	data.emission = source->GetPropertyFloat("_Emission");
)



struct alignas(16) PlayerMaterialParam
{
	Vector4 temp;
};

CBUFFER_INJECTOR("PlayerMaterialParam", PlayerMaterialParam, 256, BufferType::PlayerMaterialParam, std::shared_ptr<Material>,
	data.temp = Vector4(1, 0, 0, 1);
	// data <- source
	//data.color = Vector4(source->GetPropertyVector("_Color"));
	//data._baseMapST = Vector4(source->GetPropertyVector("_BaseMap_ST"));
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

CBUFFER_INJECTOR("TerrainDetailsParam", TerrainDetailsParam, 256, BufferType::TerrainDetailsParam, std::shared_ptr<Material>,
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

////////////////////////////////////////////////////////////////////////////

//struct alignas(16) SeaPlantParam
//{
//
//	vec4 color = vec4(1, 1, 1, 1);
//	float amplitude = 0.2f;
//	float frequency = 0.5f;
//	float boundsCenterY;
//	float boundsSizeY;
//	
//};
//
//CBUFFER_INJECTOR("SeaPlantParam", SeaPlantParam, 256, BufferType::SeaPlantParam, std::shared_ptr<Material>,
//	data.color = Vector4(source->GetPropertyVector("_Color"));
//)