#include "pch.h"
#include "ResourceManager.h"
#include "Global.h"



unique_ptr<ResourceManager> ResourceManager::main =nullptr;
std::wstring ResourceManager::path_model = L"../Resources/";

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
	ResourceManager::main->Load<Model>(L"Sphere", path_model + L"Models/Sphere.fbx", VertexType::Vertex_Static);
	ResourceManager::main->Load<Model>(L"Rect", path_model + L"Models/Rect.fbx", VertexType::Vertex_Static);
	ResourceManager::main->Load<Model>(L"Quad", path_model + L"Models/Quad.fbx", VertexType::Vertex_Static);
	ResourceManager::main->Load<Model>(L"Cylinder", path_model + L"Models/Cylinder.fbx", VertexType::Vertex_Static);
	ResourceManager::main->Load<Model>(L"Cube", path_model + L"Models/Cube.fbx", VertexType::Vertex_Static);
	ResourceManager::main->Load<Model>(L"Capsule", path_model + L"Models/Capsule.fbx", VertexType::Vertex_Static);
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
		shader->Init(L"SpriteShader.hlsl", ColorProp, ShaderArg{}, info);
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

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"normalDraw.hlsl", StaticProp, ShaderArg{ {{"PS_Main", "ps"},{"VS_Main", "vs"},
			{"GS_Main", "gs"}}}, info);
		Add<Shader>(L"DebugNormal", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"normalDraw_sea.hlsl", StaticProp, ShaderArg{ {{"PS_Main", "ps"},{"VS_Main", "vs"},
			{"GS_Main", "gs"}} }, info);
		Add<Shader>(L"DebugNormal_Sea", shader);
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
