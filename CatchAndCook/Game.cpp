#include "pch.h"
#include "Game.h"
#include "Core.h"
#include "GameObject.h"
#include "ICBufferInjector.h"
#include "IGuid.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "SceneManager.h"
#include "Transform.h"
#include "CameraManager.h"
#include "Camera.h"
void Game::Init(HWND hwnd)
{
	IGuid::StaticInit();
	Time::main = make_unique<Time>();
	Time::main->Init(hwnd);
	Input::main = make_unique<Input>();
	Core::main = make_unique<Core>();
	Core::main->Init(hwnd);

	SceneManager::main = make_unique<SceneManager>();
	CameraManager::main = make_unique<CameraManager>();


	InjectorManager::main = make_unique<InjectorManager>();
	InjectorManager::main->Init();
	InjectorManager::main->Register<TestSubMaterialParamInjector>(BufferType::MateriaSubParam);

	std::shared_ptr<Scene> scene = std::make_shared<Scene>();
	scene->Init("test");
	SceneManager::main->AddScene(scene);
	SceneManager::main->ChangeScene(scene);

	Core::main->_gameObjects = SceneManager::main->GetCurrentScene()->CreateGameObject(L"test gameObject");
	Core::main->_gameObjects->transform->SetLocalPosition(vec3(0, 0.3f, 0));
	Core::main->_meshRenderer = Core::main->_gameObjects->AddComponent<MeshRenderer>();

	Core::main->_material = make_shared<Material>();
	Core::main->_material->SetShader(Core::main->_shader);
	Core::main->_material->SetPass(RENDER_PASS::Forward);
	Core::main->_material->SetInjector({ InjectorManager::main->Get(BufferType::MateriaSubParam) });
	Core::main->_material->SetTexture("g_tex_0", Core::main->_texture);
	Core::main->_material->SetPropertyVector("uv", vec4(0.3,-0.3,0,0));

	Core::main->_meshRenderer->AddMaterials({ Core::main->_material });
	Core::main->_meshRenderer->SetMesh(Core::main->_mesh);

	CameraManager::main->AddCamera(CameraType::ThirdPersonCamera, static_pointer_cast<Camera>(make_shared<ThirdPersonCamera>()));
	CameraManager::main->GetCamera(CameraType::ThirdPersonCamera)->SetCameraPos(vec3(0.5f, 0, -20.0f));
}

void Game::Run()
{
	Input::main->Update();
	Time::main->Update();

	std::shared_ptr<Scene> currentScene = SceneManager::main->GetCurrentScene();
	currentScene->Update();
	
	Core::main->_material->SetPropertyVector("uv", Core::main->_material->GetPropertyVector("uv") + vec4(0.01,0,0,0));
	Core::main->RenderBegin();
	currentScene->RenderBegin();
	currentScene->Rendering();
	currentScene->RenderEnd();
	Core::main->RenderEnd();
	currentScene->Finish();


}

void Game::Release()
{
	SceneManager::main.reset(nullptr);
	InjectorManager::main.reset(nullptr);
	Core::main.reset(nullptr);
	Time::main.reset(nullptr);
	Input::main.reset(nullptr);
	IGuid::StaticRelease();

}
