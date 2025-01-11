#include "pch.h"
#include "ResourceManager.h"

unique_ptr<ResourceManager> ResourceManager::main =nullptr;

void ResourceManager::Init()
{
	CreateDefaultShader();
	CreateDefaultMesh();
	CreateDefaultMaterial();
	CreateDefaultTexture();
}

void ResourceManager::CreateDefaultMesh()
{
}

void ResourceManager::CreateDefaultShader()
{
}

void ResourceManager::CreateDefaultMaterial()
{
}

void ResourceManager::CreateDefaultTexture()
{
	_noneTexture = Load<Texture>(L"None", L"Textures/Config/noneTexture.png");
}
