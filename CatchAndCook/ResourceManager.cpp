﻿#include "pch.h"
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
	{
		auto finalMesh =  GeoMetryHelper::LoadRectangleMesh(1.0f);
		Add<Mesh>(L"finalMesh",finalMesh);
	}
}

void ResourceManager::CreateDefaultShader()
{
	{
		ShaderInfo info;
		info.renderTargetCount=3;

		info.RTVForamts[0]=DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1]=DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2]=DXGI_FORMAT_R8G8B8A8_UNORM;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"Deferred",L"Deferred.hlsl",StaticProp,
			ShaderArg{},info);

		shader->SetInjector({ BufferType::DefaultMaterialParam });
		shader->SetPass(RENDER_PASS::Deferred);
	}

	{
		ShaderInfo info;
		info.renderTargetCount = 3;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"Plant", L"Plant.hlsl", StaticProp,
			ShaderArg{}, info);
		shader->SetPass(RENDER_PASS::Deferred);
	}



	{
		ShaderInfo info;
		info._zTest = false;
		info._zWrite = false;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"final.hlsl", GeoMetryProp,ShaderArg{},info);
		Add<Shader>(L"finalShader",shader);
	}

	{
		ShaderInfo info;
		info._zTest = false;
		info._zWrite = false;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"final_MainField.hlsl", GeoMetryProp, ShaderArg{}, info);
		Add<Shader>(L"finalShader_MainField", shader);
	}

	{

		ShaderInfo info;
		info._zTest = false;
		info._stencilTest = false;
		info._blendEnable = true;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Forward);
		shader->Init(L"SpriteShader.hlsl", ColorProp, ShaderArg{}, info);
		Add<Shader>(L"SpriteShader", shader);
	}


	//Forward
	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({BufferType::DefaultMaterialParam});
		shader->SetPass(RENDER_PASS::Forward);
		shader->Init(L"TestForward_Total.hlsl", StaticProp, ShaderArg{}, info);
		Add<Shader>(L"DefaultForward", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({BufferType::DefaultMaterialParam});
		shader->SetMacro({{"SKINNED",nullptr}});
		shader->SetPass(RENDER_PASS::Forward);
		shader->Init(L"TestForward_Total.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"DefaultForward_Skinned", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({BufferType::DefaultMaterialParam});
		shader->SetPass(RENDER_PASS::Forward);
		shader->SetMacro({{"INSTANCED",nullptr}});
		shader->SetInstanceProp(TransformInstanceProp);
		shader->Init(L"TestForward_Total.hlsl", SkinProp,ShaderArg{},info);
		Add<Shader>(L"DefaultForward_Instanced",shader);
	}
	//Deferred
	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		info.renderTargetCount = 3;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam });
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"TestDeferred_Total.hlsl", StaticProp, ShaderArg{}, info);
		Add<Shader>(L"DefaultDeferred", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		info.renderTargetCount = 3;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam });
		shader->SetMacro({ {"SKINNED",nullptr} });
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"TestDeferred_Total.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"DefaultDeferred_Skinned", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		info.renderTargetCount = 3;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam });
		shader->SetMacro({ {"INSTANCED",nullptr} });
		shader->SetInstanceProp(TransformInstanceProp);
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"TestDeferred_Total.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"DefaultDeferred_Instanced", shader);
	}


	{
		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::UI);
		shader->Init(L"UiForward.hlsl", StaticProp, ShaderArg{});
		Add<Shader>(L"UiForward", shader);
	}


	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		info.renderTargetCount = 3;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"ForwardPreDepthNormal.hlsl", StaticProp, ShaderArg{}, info);
		Add<Shader>(L"DepthNormal", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		info.renderTargetCount = 3;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetMacro({ {"SKINNED",nullptr} });
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"ForwardPreDepthNormal.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"DepthNormal_Skinned", shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		info.renderTargetCount = 3;

		info.RTVForamts[0] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[1] = DXGI_FORMAT_R32G32B32A32_FLOAT;
		info.RTVForamts[2] = DXGI_FORMAT_R8G8B8A8_UNORM;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetMacro({ {"INSTANCED",nullptr} });
		shader->SetInstanceProp(TransformInstanceProp);
		shader->SetPass(RENDER_PASS::Deferred);
		shader->Init(L"ForwardPreDepthNormal.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"DepthNormal_Instanced", shader);
	}


	//ForwardPreDepthNormal.hlsl
	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInjector({ BufferType::DefaultMaterialParam, BufferType::PlayerMaterialParam });
		shader->SetMacro({ {"SKINNED",nullptr} });
		shader->SetPass(RENDER_PASS::Forward);
		shader->Init(L"PlayerShader.hlsl", SkinProp, ShaderArg{}, info);
		Add<Shader>(L"PlayerShader", shader);
	}



	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetInstanceProp(GizmoInstanceProp);
		shader->Init(L"Gizmo.hlsl", {},ShaderArg{{{"PS_Main","ps"},{"VS_Main","vs"},{"GS_Main","gs"}}},info);
		shader->SetPass(RENDER_PASS::Debug);
		Add<Shader>(L"Gizmo",shader);
	}

	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		info.cullingType = CullingType::BACK;

		shared_ptr<Shader> shader = make_shared<Shader>();
		shader->SetPass(RENDER_PASS::Debug);
		shader->Init(L"Gizmo_Text.hlsl", StaticProp, ShaderArg{{{"PS_Main","ps"},{"VS_Main","vs"}}}, info);
		Add<Shader>(L"GizmoTexture",shader);
	}


}

void ResourceManager::CreateDefaultMaterial()
{
	_depthNormal = std::make_shared<Material>();
	_depthNormal->SetShader(ResourceManager::main->Get<Shader>(L"DepthNormal"));
	_depthNormal->SetPass(RENDER_PASS::Deferred);

	_depthNormal_Skinned = std::make_shared<Material>();
	_depthNormal_Skinned->SetShader(ResourceManager::main->Get<Shader>(L"DepthNormal_Skinned"));
	_depthNormal_Skinned->SetPass(RENDER_PASS::Deferred);

	_depthNormal_Instanced = std::make_shared<Material>();
	_depthNormal_Instanced->SetShader(ResourceManager::main->Get<Shader>(L"DepthNormal_Instanced"));
	_depthNormal_Instanced->SetPass(RENDER_PASS::Deferred);
}

void ResourceManager::CreateDefaultTexture()
{
	_noneTexture = Load<Texture>(L"none", L"Textures/Configs/noneTexture.png");
	_noneTexture_debug = Load<Texture>(L"none_debug", L"Textures/Configs/noneTexture_debug.png");
	_bakedGITexture = Load<Texture>(L"BakedGI", L"Textures/Configs/BakedGI.png");
}

void ResourceManager::CreateDefaultAnimation()
{

}
