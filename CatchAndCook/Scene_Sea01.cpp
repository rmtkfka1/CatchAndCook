#include "pch.h"
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
#include "LightManager.h"
#include "PlantComponent.h"
#include "SkinnedMeshRenderer.h"
#include "Profiler.h"
#include "ComputeManager.h"
#include "ShadowManager.h"
#include "TerrainManager.h"
#include "CameraManager.h"
#include "InstancingManager.h"
#include "Gizmo.h"
void Scene_Sea01::Init()
{
	Scene::Init();
	//_finalShader->SetShader(ResourceManager::main->Get<Shader>(L"finalShader_MainField"));
	//_finalShader->SetPass(RENDER_PASS::Forward);

	std::shared_ptr<Light> light = std::make_shared<Light>();
	light->onOff = 1;
	light->direction = vec3(-1.0f, -1.0f, 1.0f);
	light->position = vec3(0, 1000.0f, 0);
	light->direction.Normalize();

	light->material.ambient = vec3(0.4f, 0.4f, 0.4f);
	light->material.diffuse = vec3(1.0f, 1.0f, 1.0f);
	light->material.specular = vec3(0, 0, 0);
	light->material.shininess = 32.0f;
	light->material.lightType = static_cast<int32>(LIGHT_TYPE::DIRECTIONAL_LIGHT);
	light->strength = vec3(1.0f, 1.0f, 1.0f);
	LightManager::main->PushLight(light);

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

	//vector<shared_ptr<GameObject>> gameObjects;

	//player->GetChildsAll(gameObjects);

	//for (auto& ele : gameObjects)
	//{
	//	Scene::RemoveGameObject(ele);
	//}

	//for (auto& ele : gameObjects)
	//{
	//	Scene::AddFrontGameObject(ele);
	//}


	if (player)
	{
		//player->_transform->SetPivotOffset(vec3(0, 1.0f, 0));
		player->AddComponent<SeaPlayerController>();

		vector<shared_ptr<GameObject>> childs;
		player->GetChildsAll(childs);

		for (auto& child : childs)
		{
			auto skinnedMeshRenderer = child->GetComponent<SkinnedMeshRenderer>();
			if (skinnedMeshRenderer)
			{
				skinnedMeshRenderer->SetCulling(false);
			}
		}

	}


	for (auto& gameobject : _gameObjects)
	{
		auto& meshRenderer = gameobject->GetComponent<MeshRenderer>();

		if (meshRenderer)
		{
			auto& materials = meshRenderer->GetMaterials();

			for (auto& material : materials)
			{
				cout << material->GetShader()->_name << endl;
				if (material->GetShader()->_name == "DeferredSeaPlantClip.hlsl" || "DeferredSeaPlant.hlsl")
				{
					if(gameobject->GetComponent<PlantComponent>()==nullptr)
						gameobject->AddComponent<PlantComponent>();
				}
			}
		}

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


	{
		auto plant = Find(L"Fish_1");

		if (plant)
		{
			cout << "Fish_1" << endl;
			auto finder = plant->AddComponent<PathFinder>();
			finder->SetPass(L"Fish_1");
		}
	}

	{
		auto plant = Find(L"Fish_1 (1)");

		if (plant)
		{
			cout << "Fish_2" << endl;
			auto finder = plant->AddComponent<PathFinder>();
			finder->SetPass(L"Fish_1");
		}
	}

	{
		auto plant = Find(L"Fish_1 (2)");

		if (plant)
		{
			cout << "Fish_3" << endl;
			auto finder = plant->AddComponent<PathFinder>();
			finder->SetPass(L"Fish_1");
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
	GlobalSetting();

//#ifdef _DEBUG
	//Gizmo::Width(0.5);
	//Gizmo::Frustum(CameraManager::main->GetCamera(CameraType::ComponentCamera)->_boundingFrsutum);
	//Gizmo::WidthRollBack();
	Light l;
	l.direction = Vector3(1, -1, 1);
	l.direction.Normalize();

	//auto a2 = ShadowManager::main->GetFrustums(CameraManager::main->GetCamera(CameraType::ComponentCamera).get(), &l, { 8,30,75,200 });
	//for (auto b2 : a2)
	//{
	//    Gizmo::Width(0.1);
	//    Gizmo::Frustum(b2, Vector4(1, 0, 0, 1));
	//    Gizmo::WidthRollBack();
	//}

	auto light = LightManager::main->GetMainLight();
	//auto a = ShadowManager::main->CalculateBounds(CameraManager::main->GetCamera(CameraType::SeaCamera).get(), light.get(), { 6, 20, 65, 200 });
	auto a = ShadowManager::main->CalculateBounds(CameraManager::main->GetCamera(CameraType::SeaCamera).get(), light.get(), { 3000 });
	for (auto& b : a)
	{
		Gizmo::Width(5.0f);
		Gizmo::Box(b, Vector4(0, 1, 0, 1));
		Gizmo::WidthRollBack();
	}
//#endif

	auto& cmdList = Core::main->GetCmdList();
	Core::main->GetRenderTarget()->ClearDepth();

	Profiler::Set("PASS : Shadow", BlockTag::CPU);
	ShadowPass(cmdList);
	Profiler::Fin();

	Profiler::Set("PASS : Deferred", BlockTag::CPU);
	DeferredPass(cmdList);
	Profiler::Fin();

	Profiler::Set("PASS : FinalPass", BlockTag::CPU);
	FinalRender(cmdList);
	Profiler::Fin();

	Profiler::Set("PASS : Forward", BlockTag::CPU);
	ForwardPass(cmdList);
	Profiler::Fin();

	//Profiler::Set("PASS : Transparent", BlockTag::CPU);
	//TransparentPass(cmdList); // Position,
	//Profiler::Fin();

	Profiler::Set("PASS : Compute", BlockTag::CPU);
	ComputePass(cmdList);
	Profiler::Fin();

	Profiler::Set("PASS : UI", BlockTag::CPU);
	UiPass(cmdList);
	Profiler::Fin();
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

void Scene_Sea01::ShadowPass(Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& cmdList)
{
	{
		auto light = LightManager::main->GetMainLight();
		if (light == nullptr)
		{
			cout << "없음" << endl;
			return;
		}

		auto boundings = ShadowManager::main->CalculateBounds(CameraManager::main->GetActiveCamera().get(), light.get(), { 500, 1000 ,2000, 3000 });

		auto lastShadowPos = ShadowManager::main->_lightTransform[ShadowManager::main->_lightTransform.size() - 1];
		TerrainManager::main->CullingInstancing(lastShadowPos.first, lastShadowPos.second);

		auto& targets = _passObjects[RENDER_PASS::ToIndex(RENDER_PASS::Shadow)];

		ShadowManager::main->SetData(nullptr);
		ShadowManager::main->RenderBegin();

		int i = 0;

		for (auto& bounding : boundings)
		{
			ShadowCascadeIndexParams shadowCasterParams;

			auto* cbuffer2 = Core::main->GetBufferManager()->GetBufferPool(BufferType::ShadowCascadeIndexParams)->Alloc(1);
			shadowCasterParams.cascadeIndex = i;
			memcpy(cbuffer2->ptr, &shadowCasterParams, sizeof(ShadowCascadeIndexParams));
			Core::main->GetCmdList()->SetGraphicsRootConstantBufferView(7, cbuffer2->GPUAdress);
			Core::main->GetShadowBuffer()->RenderBegin(i);

			for (auto& [shader, vec] : targets)
			{
				cmdList->SetPipelineState(shader->_pipelineState.Get());

				for (auto& renderStructure : vec)
				{
				/*	if (renderStructure.renderer->IsCulling() == true)
						if (bounding.Intersects(renderStructure.renderer->GetBound()) == false)
							continue;*/

					SettingPrevData(renderStructure, RENDER_PASS::PASS::Shadow);

					if (renderStructure.renderer->isInstancing() == false)
					{
						InstancingManager::main->RenderNoInstancing(renderStructure);
					}
					else
					{
						InstancingManager::main->AddObject(renderStructure);
					}
				}

				InstancingManager::main->Render();
			}

			Core::main->GetShadowBuffer()->RenderEnd();
			i++;
		}

		ShadowManager::main->RenderEnd();
	}
}
