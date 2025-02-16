#include "pch.h"
#include "ResourceManager.h"
#include "Global.h"



unique_ptr<ResourceManager> ResourceManager::main =nullptr;
std::wstring ResourceManager::path_model = L"../Resources/";

//ui[일주일]-> pass[1일] -> 바다구현[일주일].
//modelLoad(unity연동수정) -> TerrianSystem -> SkinnedMesh(animation) [15일]-> //navymesh 시스템 

void ResourceManager::Init()
{
	CreateDefaultShader();
	CreateDefaultMesh();
	CreateDefaultMaterial();
	CreateDefaultTexture();
	CreateDefaultModel();
}

void ResourceManager::CreateDefaultModel()
{
	ResourceManager::main->Load<Model>(L"Sphere", path_model +	L"Configs/Sphere.fbx", VertexType::Vertex_Static);
	ResourceManager::main->Load<Model>(L"Rect", path_model +		L"Configs/Rect.fbx", VertexType::Vertex_Static);
	ResourceManager::main->Load<Model>(L"Quad", path_model +		L"Configs/Quad.fbx", VertexType::Vertex_Static);
	ResourceManager::main->Load<Model>(L"Cylinder", path_model +	L"Configs/Cylinder.fbx", VertexType::Vertex_Static);
	ResourceManager::main->Load<Model>(L"Cube", path_model +		L"Configs/Cube.fbx", VertexType::Vertex_Static);
	ResourceManager::main->Load<Model>(L"Capsule", path_model +	L"Configs/Capsule.fbx", VertexType::Vertex_Static);
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
		shader->SetInjector({BufferType::DefaultMaterialParam});
		shader->Init(L"TestForward_Skinned.hlsl", StaticProp, ShaderArg{}, info);
		Add<Shader>(L"DefaultForward", shader);
	}
	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({BufferType::DefaultMaterialParam});
		shader->SetMacro({{"SKINNED",nullptr}});
		shader->Init(L"TestForward_Skinned.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"DefaultForward_Skinned", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({BufferType::DefaultMaterialParam});
		shader->SetMacro({{"INSTANCED",nullptr}});
		shader->SetInstanceProp(TransformInstanceProp);
		shader->Init(L"TestForward_Skinned.hlsl", SkinProp,ShaderArg{},info);
		Add<Shader>(L"DefaultForward_Instanced",shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"normalDraw.hlsl", GeoMetryProp, ShaderArg{ {{"PS_Main", "ps"},{"VS_Main", "vs"},
			{"GS_Main", "gs"}}}, info);
		Add<Shader>(L"DebugNormal", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->Init(L"normalDraw_sea.hlsl",GeoMetryProp, ShaderArg{ {{"PS_Main", "ps"},{"VS_Main", "vs"},
			{"GS_Main", "gs"}} }, info);
		Add<Shader>(L"DebugNormal_Sea", shader);
	}
	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInstanceProp(GizmoInstanceProp);
		shader->Init(L"Gizmo.hlsl", {},ShaderArg{{{"PS_Main","ps"},{"VS_Main","vs"},{"GS_Main","gs"}}},info);
		Add<Shader>(L"Gizmo",shader);
	}
}

void ResourceManager::CreateDefaultMaterial()
{
}

void ResourceManager::CreateDefaultTexture()
{
	_noneTexture = Load<Texture>(L"None", L"Textures/Config/noneTexture.png");
	_noneTexture_debug = Load<Texture>(L"None_Debug", L"Textures/Config/noneTexture_debug.png");
}
