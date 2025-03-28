#include "pch.h"
#include "LightingTest.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "random"
#include "Mesh.h"
#include "ImguiManager.h"
#include "WaterController.h"
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Sprite.h"
#include "TextManager.h"
#include "SpriteAction.h"
#include "testComponent.h"
#include "StructuredBuffer.h"
#include "Mesh.h"
#include "WaterHeight.h"
#include "Terrain.h"
#include "BoidsMove.h"
#include <random>
#include "Model.h"
#include "ModelMesh.h"
#include "Collider.h"

void LightingTest::Init()
{
	Scene::Init();


	shared_ptr<Material> materialO = make_shared<Material>();
	//shared_ptr<Mesh> mesh = a->_modelMeshList[0]->GetMesh();
	shared_ptr<Mesh> mesh = GeoMetryHelper::LoadRectangleBox(1.0f);

	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"cubemap", L"cubemap.hlsl", GeoMetryProp,
			ShaderArg{}, info);

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"cubemap", L"Textures/cubemap/output.dds", TextureType::CubeMap);
		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"cubeMap");

		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();


		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetTexture("g_tex_0", texture);

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
		meshRenderer->SetCulling(false);
	}

	for (int i = 0; i < 1; ++i)
	{
		
			shared_ptr<Shader> shader = ResourceManager::main->Get<Shader>(L"Deffered");

			shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"start", L"Textures/start.jpg");

			shared_ptr<GameObject> root = CreateGameObject(L"root_test");

			auto meshRenderer = root->AddComponent<MeshRenderer>();
			auto& collider = root->AddComponent<Collider>();


			collider->SetBoundingBox(vec3(0, 0, 0), vec3(1.0f, 1.0f, 1.0f));

			root->SetType(GameObjectType::Dynamic);
			root->AddTag(GameObjectTag::Player);

			materialO->SetShader(shader);
			materialO->SetPass(RENDER_PASS::Deffered);
			materialO->SetTexture("_BaseMap", texture);

			meshRenderer->AddMaterials({ materialO });
			meshRenderer->AddMesh(mesh);
		
	};




}

void LightingTest::Update()
{
	Scene::Update();
}

void LightingTest::RenderBegin()
{
	Scene::RenderBegin();
}

void LightingTest::Rendering()
{
	Scene::Rendering();
}

void LightingTest::RenderEnd()
{
	Scene::RenderEnd();
}

void LightingTest::Finish()
{
	Scene::Finish();
}
