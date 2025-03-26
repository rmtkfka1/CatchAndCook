#include "pch.h"
#include "Scene_Sea01.h"
#include "testComponent.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Collider.h"
#include "Transform.h"
#include "WallSlideController.h"
void Scene_Sea01::Init()
{

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
			root->AddComponent<WallSlideController>();
			root->_transform->SetLocalScale(vec3(1.0f, 1.0f, 1.0f));
			root->_transform->SetLocalPosition(vec3(0, 0, 0));
			root->_transform->SetLocalRotation(vec3(0, 0, 0));
			root->SetType(GameObjectType::Dynamic);
			root->AddTag(GameObjectTag::Player);

			materialO->SetShader(shader);
			materialO->SetPass(RENDER_PASS::Deffered);
			materialO->SetTexture("_BaseMap", texture);

			meshRenderer->AddMaterials({ materialO });
			meshRenderer->AddMesh(mesh);
		}
	};

	ResourceManager::main->LoadAlway<SceneLoader>(L"test", L"../Resources/Datas/Scenes/Sea_Resources.json");
	auto sceneLoader = ResourceManager::main->Get<SceneLoader>(L"test");
	sceneLoader->Load(GetCast<Scene>());


	auto& object =Scene::Find(L"Wall01");

	if (object)
	{
		object->AddTag(GameObjectTag::Wall);
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
