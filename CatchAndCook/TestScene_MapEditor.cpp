﻿#include "pch.h"
#include "TestScene_MapEditor.h"
#include "MeshRenderer.h"
#include "testComponent.h"
#include "Transform.h"


void TestScene_MapEditor::Init()
{
	Scene::Init();

	{
		ShaderInfo info;
		info._zTest = true;
		info._zWrite = false;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"cubemap",L"cubemap.hlsl",GeoMetryProp,
			ShaderArg{},info);

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"cubemap",L"Textures/cubemap/output.dds",TextureType::CubeMap);
		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"cubeMap");

		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();


		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetTexture("g_tex_0", texture);

		meshRenderer->SetCulling(false);
		meshRenderer->AddMaterials({material});
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
	}


	//ResourceManager::main->Load<Model>(L"kind", L"../Resources/Models/Kindred/kindred_unity.fbx", VertexType::Vertex_Skinned);
	//ResourceManager::main->Load<Model>(L"kind",L"../Resources/Models/testB.fbx",VertexType::Vertex_Skinned);
	//auto model = ResourceManager::main->Get<Model>(L"kind");
	//auto obj = model->CreateGameObject(GetCast<Scene>());
	//obj->_transform->SetWorldScale(vec3(100,100,100));

	ResourceManager::main->LoadAlway<SceneLoader>(L"test",L"../Resources/Datas/Scenes/TestScene9.json");
	auto a = ResourceManager::main->Get<SceneLoader>(L"test");
	a->Load(GetCast<Scene>());

	//int i=0;
	//Find(L"testB")->ForHierarchyAll([&](const std::shared_ptr<GameObject>& obj)
	//{
	//	std::cout << std::to_string(obj->GetName()) <<"\n";
	//});
	//std::cout << i <<"\n";
	std::cout << _gameObjects.size() << "\n";

	//a[0]->_transform->SetLocalRotation(Vector3(0,180,0) * D2R);
}

void TestScene_MapEditor::Update()
{
	Scene::Update();
}

void TestScene_MapEditor::RenderBegin()
{
	Scene::RenderBegin();
}

void TestScene_MapEditor::Rendering()
{
	Scene::Rendering();
}

void TestScene_MapEditor::DebugRendering()
{
	Scene::DebugRendering();
}

void TestScene_MapEditor::RenderEnd()
{
	Scene::RenderEnd();
}

void TestScene_MapEditor::Finish()
{
	Scene::Finish();
}

TestScene_MapEditor::~TestScene_MapEditor()
{}
