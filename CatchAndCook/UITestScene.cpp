#include "pch.h"
#include "UITestScene.h"
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include <random>

void UITestScene::Init()
{
	Scene::Init();

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
		material->SetHandle("g_tex_0", texture->GetSRVCpuHandle());

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
		meshRenderer->SetCulling(false);
	}

	random_device urd;
	mt19937 gen(urd());
	uniform_real_distribution<float> ddis(-1000.0f, 1000.0f);
	uniform_real_distribution<float> dis(1, 30.0f);
	uniform_real_distribution<float> rotate(-360.0f, 360.0f);
	//auto& a = ResourceManager::main->Load<Model>(L"kind", L"../Resources/Models/PaperPlane.obj", VertexType::Vertex_Skinned);

	shared_ptr<Material> materialO = make_shared<Material>();
	materialO->SetPass(RENDER_PASS::UI);
	//shared_ptr<Mesh> mesh = a->_modelMeshList[0]->GetMesh();
	shared_ptr<Mesh> mesh = GeoMetryHelper::LoadRectangleBox(1.0f);

	for (int i = 0; i < 10; ++i)
	{
		{
			shared_ptr<Shader> shader = ResourceManager::main->Get<Shader>(L"UiForward");

			shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"start", L"Textures/start.jpg");

			shared_ptr<GameObject> root = CreateGameObject(L"root_test");

			auto meshRenderer = root->AddComponent<MeshRenderer>();
			root->_transform->SetLocalPosition(vec3(WINDOW_WIDTH/2-30, 0+60*i,100.0f));
			root->_transform->SetLocalScale(vec3(60.0f, 60.0f, 60.0f));
			root->SetType(GameObjectType::Static);
			root->AddTag(GameObjectTag::Wall);


			materialO->SetShader(shader);
			materialO->SetHandle("_BaseMap", texture->GetSRVCpuHandle());

			meshRenderer->AddMaterials({ materialO });
			meshRenderer->AddMesh(mesh);
		}
	}
}

void UITestScene::Update()
{
	Scene::Update();
}

void UITestScene::RenderBegin()
{
	Scene::RenderBegin();
}

void UITestScene::Rendering()
{
	Scene::Rendering();
}

void UITestScene::RenderEnd()
{
	Scene::RenderEnd();
}

void UITestScene::Finish()
{
	Scene::Finish();
}
