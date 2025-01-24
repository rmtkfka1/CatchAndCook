#include "pch.h"
#include "ResourceManager.h"
#include "Global.h"
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

void ResourceManager::CreateDefaultModel()
{

}

void ResourceManager::CreateDefaultMesh()
{

}

void ResourceManager::CreateDefaultShader()
{
	{

		ShaderInfo info;
		info._zTest = false;
		info._stencilTest = false;
		info._blendEnable = true;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"SpriteShader.hlsl", SpriteProp, ShaderArg{}, info);
		Add<Shader>(L"SpriteShader", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"TestForward.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"DefaultForward", shader);
	}

}

void ResourceManager::CreateDefaultMaterial()
{
}

void ResourceManager::CreateDefaultTexture()
{
	_noneTexture = Load<Texture>(L"None", L"Textures/Config/noneTexture.png");
	_noneTexture = Load<Texture>(L"None_Debug", L"Textures/Config/noneTexture_debug.png");
}
