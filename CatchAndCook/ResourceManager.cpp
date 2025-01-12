#include "pch.h"
#include "ResourceManager.h"

unique_ptr<ResourceManager> ResourceManager::main =nullptr;

//ui[老林老]-> pass[1老] -> 官促备泅[老林老].
//modelLoad(unity楷悼荐沥) -> TerrianSystem -> SkinnedMesh(animation) [15老]-> //navymesh 矫胶袍 

void ResourceManager::Init()
{
	CreateDefaultShader();
	CreateDefaultMesh();
	CreateDefaultMaterial();
	CreateDefaultTexture();
}

void ResourceManager::CreateDefaultMesh()
{
	_meshMap.reserve(16);
}

void ResourceManager::CreateDefaultShader()
{
}

void ResourceManager::CreateDefaultMaterial()
{
}

void ResourceManager::CreateDefaultTexture()
{
	_meshMap.reserve(2048);
	_noneTexture = Load<Texture>(L"None", L"Textures/Config/noneTexture.png");
}
