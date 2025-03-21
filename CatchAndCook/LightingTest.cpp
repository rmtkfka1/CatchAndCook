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
#include "SpriteRenderer.h"
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

	ResourceManager::main->LoadAlway<SceneLoader>(L"test", L"../Resources/Datas/Scenes/Sea_Resources.json");
	auto sceneLoader = ResourceManager::main->Get<SceneLoader>(L"test");
	sceneLoader->Load(GetCast<Scene>());

	auto& a = ResourceManager::main->Load<Model>(L"kind", L"../Resources/Models/PaperPlane.obj", VertexType::Vertex_Skinned);
	cout << a->_modelMeshList.size() << endl;
	shared_ptr<Material> materialO = make_shared<Material>();
	shared_ptr<Mesh> mesh = a->_modelMeshList[0]->GetMesh();
	//shared_ptr<Mesh> mesh = GeoMetryHelper::LoadRectangleBox(1.0f);

	for (int i = 0; i < 50; ++i)
	{
		{
			shared_ptr<Shader> shader = ResourceManager::main->Get<Shader>(L"Deffered");

			shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"start", L"Textures/start.jpg");

			shared_ptr<GameObject> root = CreateGameObject(L"root_test");
			root->AddComponent<BoidsMove>();
			

			auto meshRenderer = root->AddComponent<MeshRenderer>();
			auto& collider = root->AddComponent<Collider>();
			collider->SetBoundingBox(vec3(0, 0, 0), vec3(1.0f, 1.0f, 1.0f));

			root->_transform->SetLocalScale(vec3(5.0f, 5.0f, 5.0f));
			root->_transform->SetLocalPosition(vec3(30 * i, 0, 0));

			if (i == 1)
				root->_transform->SetLocalRotation(vec3(0, 45.0f, 0) * R2D);

			materialO->SetShader(shader);
			materialO->SetPass(RENDER_PASS::Deffered);
			materialO->SetHandle("_BaseMap", texture->GetSRVCpuHandle());

			meshRenderer->AddMaterials({ materialO });
			meshRenderer->AddMesh(mesh);
		}
	}


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
