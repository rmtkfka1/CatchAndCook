﻿#include "pch.h"
#include "TestScene_jin.h"

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


void TestScene_jin::Init()
{
	Scene::Init();

	shared_ptr<Material> materialO = make_shared<Material>();
	//shared_ptr<Mesh> mesh = a->_modelMeshList[0]->GetMesh();
	shared_ptr<Mesh> mesh = GeoMetryHelper::LoadRectangleBox(1.0f);

	for (int i = 0; i < 1; ++i)
	{
		{
			shared_ptr<Shader> shader = ResourceManager::main->Get<Shader>(L"Deffered");

			shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"start", L"Textures/start.jpg");

			shared_ptr<GameObject> root = CreateGameObject(L"root_test");

			auto meshRenderer = root->AddComponent<MeshRenderer>();
			auto& collider = root->AddComponent<Collider>();
			collider->SetBoundingBox(vec3(0, 0, 0), vec3(1.0f, 1.0f, 1.0f));




			root->AddComponent<testComponent>();
			root->_transform->SetLocalScale(vec3(1.0f, 1.0f, 1.0f));
			root->_transform->SetLocalPosition(vec3(0, 0, 0));
			root->_transform->SetLocalRotation(vec3(0, 0, 0));
			root->SetType(GameObjectType::Dynamic);



			materialO->SetShader(shader);
			materialO->SetPass(RENDER_PASS::Deffered);
			materialO->SetHandle("_BaseMap", texture->GetSRVCpuHandle());

			meshRenderer->AddMaterials({ materialO });
			meshRenderer->AddMesh(mesh);
		}
	};


	{
		ShaderInfo info;
		info._zTest = true;
		info._zWrite = false;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"cubemap", L"cubemap.hlsl", GeoMetryProp,
			ShaderArg{}, info);

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"cubemap", L"Textures/cubemap/output.dds", TextureType::CubeMap);
		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"cubeMap");

		gameObject->_transform->SetLocalRotation(vec3(0, 90.0f, 0) * D2R);

		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();
		meshRenderer->SetCulling(false);

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetHandle("g_tex_0", texture->GetSRVCpuHandle());

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
	}
	if (false)
	{

		{
			ShaderInfo info;
			info._zTest = true;
			info._stencilTest = false;
			info.cullingType = CullingType::BACK;
			info.cullingType = CullingType::NONE;
			info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;

			shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"seatest", L"seatest.hlsl", GeoMetryProp,
				ShaderArg{ {{"VS_Main","vs"},{"PS_Main","ps"},{"HS_Main","hs"},{"DS_Main","ds"}} }, info);

			shared_ptr<Material> material = make_shared<Material>();

			shared_ptr<GameObject> gameObject = CreateGameObject(L"grid_orgin");
			auto meshRenderer = gameObject->AddComponent<MeshRenderer>();
			gameObject->AddComponent<WaterController>();

			//meshRenderer->SetDebugShader(ResourceManager::main->Get<Shader>(L"DebugNormal_Sea"));
			gameObject->_transform->SetLocalPosition(vec3(0, 9.5f, 0));

			material = make_shared<Material>();
			material->SetShader(shader);
			material->SetPass(RENDER_PASS::Forward);
			material->SetHandle("_cubeMap", ResourceManager::main->Get<Texture>(L"cubemap")->GetSRVCpuHandle());
			material->SetUseMaterialParams(true);
			meshRenderer->AddMaterials({ material });

			auto& mesh = GeoMetryHelper::LoadGripMeshControlPoints(20000.0f, 20000.0f, 1000, 1000, false);
			mesh->SetTopolgy(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
			meshRenderer->AddMesh(mesh);
			meshRenderer->SetCulling(false);

		}

	};

	


	ResourceManager::main->LoadAlway<SceneLoader>(L"test", L"../Resources/Datas/Scenes/MainField2.json");
	auto sceneLoader = ResourceManager::main->Get<SceneLoader>(L"test");
	sceneLoader->Load(GetCast<Scene>());
}

void TestScene_jin::Update()
{
	Scene::Update();
	//Gizmo::Text(L"1234sadfsdjkaflhsaldkfasdfasdfsadfsdafasdf",10,vec3(1,1,1),vec3(0,0,-1),vec3(0,1,0));
	//Gizmo::Text(L"안녕하세요",30,vec3(3,1,1),vec3(-0.707,0,-0.707),vec3(0,1,0));
	//Gizmo::Image(ResourceManager::main->Get<Texture>(L"none"), vec3(-2,0,2),vec3(0,0,-1),vec3(0,1,0));
	//Gizmo::Image(ResourceManager::main->Get<Texture>(L"none_debug"),vec3(-1,0,2),vec3(0.707,0,-0.707),vec3(0,1,0));


	//auto camera = CameraManager::main->GetActiveCamera();
	//Vector3 worldPos = camera->GetScreenToWorldPosition(Input::main->GetMousePosition());
	//Vector3 worldDir = (worldPos - camera->GetCameraPos());
	//worldDir.Normalize();
	//float dis = 1000;
	//auto a = ColliderManager::main->RayCast({worldPos,worldDir}, dis);
	//if(a)
	//{
	//	Gizmo::Ray(a.worldPos, a.normal, 1);
	//}
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
