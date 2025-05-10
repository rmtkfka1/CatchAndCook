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


void TestScene::Init()
{
	Scene::Init();

	_finalShader->SetShader(ResourceManager::main->Get<Shader>(L"finalShader_MainField"));
	_finalShader->SetPass(RENDER_PASS::Forward);





	ColliderManager::main->SetCellSize(5);
	ResourceManager::main->LoadAlway<SceneLoader>(L"test6", L"../Resources/Datas/Scenes/MainField5.json");
	auto sceneLoader = ResourceManager::main->Get<SceneLoader>(L"test6");
	sceneLoader->Load(GetCast<Scene>());

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
