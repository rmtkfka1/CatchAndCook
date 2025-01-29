#include "pch.h"
#include "TestScene_jin.h"



void TestScene_jin::Init()
{
	Scene::Init();

	//ResourceManager::main->Load<Model>(L"kind", L"../Resources/Models/Kindred/kindred_unity.fbx", VertexType::Vertex_Skinned);
	//auto model = ResourceManager::main->Get<Model>(L"kind");
	//auto obj = model->CreateGameObject(GetCast<Scene>());
	//std::cout << _gameObjects.size() << "\n";
	ResourceManager::main->Load<SceneLoader>(L"test", L"../Resources/Datas/Scenes/TestScene.json");
	ResourceManager::main->Get<SceneLoader>(L"test")->Load(GetCast<Scene>());
	std::cout << _gameObjects.size() << "\n";
}

void TestScene_jin::Update()
{
	Scene::Update();
}

void TestScene_jin::RenderBegin()
{
	Scene::RenderBegin();
}

void TestScene_jin::Rendering()
{
	Scene::Rendering();
}

void TestScene_jin::DebugRendering()
{
	Scene::DebugRendering();
}

void TestScene_jin::RenderEnd()
{
	Scene::RenderEnd();
}

void TestScene_jin::Finish()
{
	Scene::Finish();
}

TestScene_jin::~TestScene_jin()
{
}
