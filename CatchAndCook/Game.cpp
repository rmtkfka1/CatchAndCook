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
#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"

void Game::Init(HWND hwnd)
{
	IGuid::StaticInit();
	Time::main = make_unique<Time>();
	Time::main->Init(hwnd);
	Input::main = make_unique<Input>();
	Core::main = make_unique<Core>();
	Core::main->Init(hwnd);

	ResourceManager::main = make_unique<ResourceManager>();
	ResourceManager::main->Init();
	SceneManager::main = make_unique<SceneManager>();
	CameraManager::main = make_unique<CameraManager>();

	InjectorManager::main = make_unique<InjectorManager>();
	InjectorManager::main->Init();
	InjectorManager::main->Register<TestSubMaterialParamInjector>(BufferType::MateriaSubParam);

	std::shared_ptr<Scene> scene = std::make_shared<Scene>();
	scene->Init("test");

	SceneManager::main->AddScene(scene);
	SceneManager::main->ChangeScene(scene);

	_mesh = make_shared<Mesh>();

	vector<Vertex_Static> data;

	data.resize(4);
	
	data[0].position = vec3(-0.5f, 0.5f, 0.3f);
	data[1].position = vec3(0.5f, 0.5f, 0.3f);
	data[2].position = vec3(0.5f, -0.5f, 0.3f);
	data[3].position = vec3(-0.5f, -0.5f, 0.3f);

	data[0].uvs[0] = vec2(0, 0);
	data[1].uvs[0] = vec2(1.0f, 0);
	data[2].uvs[0] = vec2(1.0f, 1.0f);
	data[3].uvs[0] = vec2(0, 1.0f);

	data[0].uvs[1] = vec2(0, 0);
	data[1].uvs[1] = vec2(1.0f, 0);
	data[2].uvs[1] = vec2(1.0f, 1.0f);
	data[3].uvs[1] = vec2(0, 1.0f);

	vector<uint32> indices = { 0,1,2 ,0,2,3 };

	_mesh->Init(data, indices);

	ShaderInfo info;
	info._zTest = false;
	info._stencilTest = false;

	_shader = ResourceManager::main->Load<Shader>(L"test.hlsl", L"test.hlsl", StaticProp,
		ShaderArg{}, info);

	_texture = ResourceManager::main->Load<Texture>(L"start",L"Textures/start.jpg");

	_gameObjects = SceneManager::main->GetCurrentScene()->CreateGameObject(L"test gameObject");
	_gameObjects->transform->SetLocalPosition(vec3(0, 0.3f, 0));
	_meshRenderer = _gameObjects->AddComponent<MeshRenderer>();

	_material = make_shared<Material>();
	_material->SetShader(_shader);
	_material->SetPass(RENDER_PASS::Forward);
	_material->SetInjector({ InjectorManager::main->Get(BufferType::MateriaSubParam) });
	_material->SetTexture("g_tex_0",_texture);
	_material->SetPropertyVector("uv", vec4(0.3,-0.3,0,0));

	_meshRenderer->AddMaterials({_material });
	_meshRenderer->SetMesh(_mesh);

	CameraManager::main->AddCamera(CameraType::ThirdPersonCamera, static_pointer_cast<Camera>(make_shared<ThirdPersonCamera>()));
	CameraManager::main->GetCamera(CameraType::ThirdPersonCamera)->SetCameraPos(vec3(0.5f, 0, -5.0f));
}

void Game::Run()
{

	Input::main->Update();
	Time::main->Update();

	std::shared_ptr<Scene> currentScene = SceneManager::main->GetCurrentScene();
	currentScene->Update();
	
	_material->SetPropertyVector("uv",_material->GetPropertyVector("uv") + vec4(0.01,0,0,0));
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
	CameraManager::main.reset(nullptr);
	InjectorManager::main.reset(nullptr);
	ResourceManager::main.reset(nullptr);
	Time::main.reset(nullptr);
	Input::main.reset(nullptr);
	IGuid::StaticRelease();
	Core::main.reset(nullptr);
}
