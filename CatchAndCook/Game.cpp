#include "pch.h"
#include "Game.h"
#include "Core.h"
#include "GameObject.h"
#include "IGuid.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "SceneManager.h"

void Game::Init(HWND hwnd)
{
	Time::main = make_unique<Time>();
	Time::main->Init(hwnd);
	Input::main = make_unique<Input>();
	Core::main = make_unique<Core>();
	Core::main->Init(hwnd);
	SceneManager::main = make_unique<SceneManager>();
	IGuid::StaticInit();

	std::shared_ptr<Scene> scene = std::make_shared<Scene>();
	scene->Init("test");
	SceneManager::main->AddScene(scene);
	SceneManager::main->ChangeScene(scene);

	Core::main->_gameObjects = SceneManager::main->GetCurrentScene()->CreateGameObject(L"test gameObject");

	Core::main->_meshRenderer = Core::main->_gameObjects->AddComponent<MeshRenderer>();

	Core::main->_material = make_shared<Material>();
	Core::main->_material->SetShader(Core::main->_shader);
	Core::main->_material->SetPass(RENDER_PASS::Forward);
	Core::main->_material->SetTexture("g_tex_0", Core::main->_texture);

	Core::main->_meshRenderer->AddMaterials({ Core::main->_material });
	Core::main->_meshRenderer->SetMesh(Core::main->_mesh);
}

void Game::Run()
{
	Input::main->Update();
	Time::main->Update();

	std::shared_ptr<Scene> currentScene = SceneManager::main->GetCurrentScene();
	currentScene->Update();
	Core::main->RenderBegin();
	currentScene->RenderBegin();
	currentScene->Rendering();
	currentScene->RenderEnd();
	Core::main->RenderEnd();
	currentScene->Finish();


}

void Game::Release()
{
	Input::main.reset(nullptr);
	Time::main.reset(nullptr);
	Core::main.reset(nullptr);
	IGuid::StaticRelease();

}
