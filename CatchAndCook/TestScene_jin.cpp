#include "pch.h"
#include "TestScene_jin.h"
#include "MeshRenderer.h"



void TestScene_jin::Init()
{
	Scene::Init();

	{
		ShaderInfo info;
		info._zTest = true;
		info._zWrite = false;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"cubemap",L"cubemap.hlsl",StaticProp,
			ShaderArg{},info);

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"cubemap",L"Textures/cubemap/output.dds",TextureType::CubeMap);
		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"cubeMap");

		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();


		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetHandle("g_tex_0",texture->GetSRVCpuHandle());

		meshRenderer->AddMaterials({material});
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
	}


	//ResourceManager::main->Load<Model>(L"kind", L"../Resources/Models/Kindred/kindred_unity.fbx", VertexType::Vertex_Skinned);
	//auto model = ResourceManager::main->Get<Model>(L"kind");
	//auto obj = model->CreateGameObject(GetCast<Scene>());
	//std::cout << _gameObjects.size() << "\n";

	ResourceManager::main->Load<SceneLoader>(L"test", L"../Resources/Datas/Scenes/TestScene2.json");
	ResourceManager::main->Get<SceneLoader>(L"test")->Load(GetCast<Scene>());
	std::cout << _gameObjects.size() << "\n";
}

void TestScene_jin::Update()
{
	Scene::Update();
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
