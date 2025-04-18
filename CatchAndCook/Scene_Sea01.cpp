﻿#include "pch.h"
#include "Scene_Sea01.h"
#include "testComponent.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Collider.h"
#include "Transform.h"
#include "PathStamp.h"
#include "PlayerController.h"
#include "CameraComponent.h"
#include "SeaPlayerController.h"
#include "FishMonster.h"
void Scene_Sea01::Init()
{
	Scene::Init();
	//_finalShader->SetShader(ResourceManager::main->Get<Shader>(L"finalShader_MainField"));
	//_finalShader->SetPass(RENDER_PASS::Forward);

#pragma region DebugXYZ
	{

		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"color", L"color.hlsl", ColorProp,
			ShaderArg{}, info);

		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> root = CreateGameObject(L"X");

		root->_transform->SetLocalPosition(vec3(3000.0f, 0, 0.0f));
		root->_transform->SetLocalScale(vec3(3000.0f, 1.0f, 1.0f));
		auto meshRenderer = root->AddComponent<MeshRenderer>();

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);

		meshRenderer->AddMaterials({ material });

		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBoxWithColor(1.0f, vec4(1, 0, 0, 0)));


	}

	{


		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"color", L"color.hlsl", ColorProp,
			ShaderArg{}, info);

		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> root = CreateGameObject(L"Y");

		root->_transform->SetLocalPosition(vec3(0, 0, 3000.0f));
		root->_transform->SetLocalScale(vec3(1.0f, 1.0f, 3000.0f));
		auto meshRenderer = root->AddComponent<MeshRenderer>();

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBoxWithColor(1.0f, vec4(0, 1, 0, 0)));
	}

	{


		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"color", L"color.hlsl", ColorProp,
			ShaderArg{}, info);

		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> root = CreateGameObject(L"Z");

		root->_transform->SetLocalPosition(vec3(0, 3000.0f, 0.0f));
		root->_transform->SetLocalScale(vec3(1.0f, 3000.0f, 1.0f));
		auto meshRenderer = root->AddComponent<MeshRenderer>();

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBoxWithColor(1.0f, vec4(0, 0, 1, 0)));
	}
#pragma endregion

	ResourceManager::main->LoadAlway<SceneLoader>(L"test", L"../Resources/Datas/Scenes/sea.json");
	auto sceneLoader = ResourceManager::main->Get<SceneLoader>(L"test");
	sceneLoader->Load(GetCast<Scene>());

	auto player = Find(L"seaPlayer");

	if (player)
	{
		//player->_transform->SetPivotOffset(vec3(0, 1.0f, 0));
		player->AddComponent<SeaPlayerController>();
	
	}

	{
		auto plant = Find(L"2");

		if (plant)
		{
			auto finder = plant->AddComponent<FishMonster>();
			finder->ReadPathFile(L"TutleMove");
		}
	}

	{
		auto plant = Find(L"shark");

		if (plant)
		{
			auto finder = plant->AddComponent<FishMonster>();
			finder->ReadPathFile(L"SharkMove");
		}
	}

	
}

void Scene_Sea01::Update()
{
	Scene::Update();

	PathStamp::main->Run();
}

void Scene_Sea01::RenderBegin()
{
	Scene::RenderBegin();
}

void Scene_Sea01::Rendering()
{
	Scene::Rendering();
}

void Scene_Sea01::DebugRendering()
{
	Scene::DebugRendering();
}

void Scene_Sea01::RenderEnd()
{
	Scene::RenderEnd();
}

void Scene_Sea01::Finish()
{
	Scene::Finish();
}
