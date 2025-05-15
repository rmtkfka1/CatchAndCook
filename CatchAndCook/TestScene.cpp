#include "pch.h"
#include "TestScene.h"
#include "Camera.h"
#include "CameraManager.h"
#include "ColliderManager.h"
#include "Gizmo.h"
#include "MeshRenderer.h"
#include "testComponent.h"
#include "Transform.h"
#include "Mesh.h"
#include "WaterController.h"
#include "Collider.h"
#include "ComputeManager.h"
#include "PlayerController.h"
#include "testComponent.h"
#include "LightManager.h"


void TestScene::Init()
{
	Scene::Init();

	_finalShader->SetShader(ResourceManager::main->Get<Shader>(L"finalShader_MainField"));
	_finalShader->SetPass(RENDER_PASS::Forward);

	ColliderManager::main->SetCellSize(5);

	ResourceManager::main->LoadAlway<SceneLoader>(L"test6", L"../Resources/Datas/Scenes/MainField5.json");
	auto sceneLoader = ResourceManager::main->Get<SceneLoader>(L"test6");
	sceneLoader->Load(GetCast<Scene>());

	std::shared_ptr<Light> light = std::make_shared<Light>();
	light->onOff = 1;
	light->direction = vec3(-0.122f, -0.732f, 0.299f);
	light->position = vec3(0, 1000.0f, 0);
	light->direction.Normalize();

	light->material.ambient = vec3(0.4f, 0.4f, 0.4f);
	light->material.diffuse = vec3(1.0f, 1.0f, 1.0f);
	light->material.specular = vec3(0, 0, 0);
	light->material.shininess = 32.0f;
	light->material.lightType = static_cast<int32>(LIGHT_TYPE::DIRECTIONAL_LIGHT);
	light->strength = vec3(1.0f, 1.0f, 1.0f);
	LightManager::main->PushLight(light);
	LightManager::main->_lightParmas.mainLight = *light.get();

	CameraManager::main->GetCamera(CameraType::DebugCamera)->SetCameraLook(vec3(0.316199, 0.743145, -0.589706));
	CameraManager::main->GetCamera(CameraType::DebugCamera)->SetCameraPos(vec3(245.946, 79.8085, 225.333));

}

void TestScene::Update()
{
	Scene::Update();

}

void TestScene::RenderBegin()
{
	Scene::RenderBegin();
}

void TestScene::Rendering()
{
	Scene::Rendering();
}

void TestScene::DebugRendering()
{
	Scene::DebugRendering();
}

void TestScene::RenderEnd()
{
	Scene::RenderEnd();
}

void TestScene::Finish()
{
	Scene::Finish();

	if (Input::main->GetKeyDown(KeyCode::F6))
	{
		Core::main->FenceCurrentFrame();
		SceneManager::main->ChangeScene(SceneManager::main->GetCurrentScene(), SceneManager::main->FindScene(SceneType::TestScene2), true, true);
	}
}

TestScene::~TestScene()
{
}

void TestScene::UiPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	Scene::UiPass(cmdList);
}

void TestScene::TransparentPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	Scene::TransparentPass(cmdList);
}

void TestScene::ForwardPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	Scene::ForwardPass(cmdList);
}

void TestScene::DeferredPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	Scene::DeferredPass(cmdList);
}

void TestScene::ShadowPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	Scene::ShadowPass(cmdList);
}

void TestScene::FinalRender(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	Scene::FinalRender(cmdList);
}

void TestScene::SettingPrevData(RenderObjectStrucutre& data, const RENDER_PASS::PASS& pass)
{
	Scene::SettingPrevData(data, pass);
}

void TestScene::ComputePass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	ComputeManager::main->DispatchMainField(cmdList);
}
