#include "pch.h"
#include "Scene_Sea01.h"
#include "testComponent.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Collider.h"
#include "Transform.h"
#include "WallSlideController.h"
#include "PlayerController.h"
#include "CameraComponent.h"
#include "SeaPlayerController.h"
void Scene_Sea01::Init()
{

	ResourceManager::main->LoadAlway<SceneLoader>(L"test", L"../Resources/Datas/Scenes/sea.json");
	auto sceneLoader = ResourceManager::main->Get<SceneLoader>(L"test");
	sceneLoader->Load(GetCast<Scene>());

	auto player = Find(L"player");

	if (player)
	{
		//player->_transform->SetPivotOffset(vec3(0, 1.0f, 0));
		player->AddComponent<SeaPlayerController>();
	}

	
}

void Scene_Sea01::Update()
{
	Scene::Update();
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
