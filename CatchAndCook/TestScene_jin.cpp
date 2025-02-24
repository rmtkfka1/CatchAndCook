#include "pch.h"
#include "TestScene_jin.h"

#include "Gizmo.h"
#include "MeshRenderer.h"
#include "testComponent.h"
#include "Transform.h"


void TestScene_jin::Init()
{
	Scene::Init();

	{
		ShaderInfo info;
		info._zTest = true;
		info._zWrite = false;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"cubemap",L"cubemap.hlsl",GeoMetryProp,
			ShaderArg{},info);

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"cubemap",L"Textures/cubemap/output.dds",TextureType::CubeMap);
		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"cubeMap");

		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();
		meshRenderer->SetCulling(false);

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetHandle("g_tex_0",texture->GetSRVCpuHandle());

		meshRenderer->AddMaterials({material});
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
	}


	ResourceManager::main->LoadAlway<SceneLoader>(L"test", L"../Resources/Datas/Scenes/TestScene9.json");
	auto sceneLoader = ResourceManager::main->Get<SceneLoader>(L"test");
	sceneLoader->Load(GetCast<Scene>());
}

void TestScene_jin::Update()
{
	Scene::Update();
	Gizmo::Text(L"1234sadfsdjkaflhsaldkfasdfasdfsadfsdafasdf",10,vec3(1,1,1),vec3(0,0,-1),vec3(0,1,0));
	Gizmo::Text(L"안녕하세요",30,vec3(3,1,1),vec3(-0.707,0,-0.707),vec3(0,1,0));
	Gizmo::Image(ResourceManager::main->Get<Texture>(L"none"), vec3(-2,0,2),vec3(0,0,-1),vec3(0,1,0));
	Gizmo::Image(ResourceManager::main->Get<Texture>(L"none_debug"),vec3(-1,0,2),vec3(0.707,0,-0.707),vec3(0,1,0));
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
