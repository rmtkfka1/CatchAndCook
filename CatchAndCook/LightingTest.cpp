#include "pch.h"
#include "LightingTest.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "random"
#include "Mesh.h"
#include "ImguiManager.h"
#include "WaterController.h"
void LightingTest::Init()
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
			gameObject->_transform->SetLocalPosition(vec3(0, 0.0f, 0));

			material = make_shared<Material>();
			material->SetShader(shader);
			material->SetPass(RENDER_PASS::Forward);
			shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"Sea", L"Textures/sea/sea.jpg");
			//shared_ptr<Texture> texture1 = ResourceManager::main->Load<Texture>(L"Sea2", L"Textures/sea/0001.png");
			material->SetHandle("_baseMap", texture->GetSRVCpuHandle());
			material->SetHandle("_cubeMap", ResourceManager::main->Get<Texture>(L"cubemap")->GetSRVCpuHandle());

			meshRenderer->AddMaterials({ material });

			auto& mesh = GeoMetryHelper::LoadGripMeshControlPoints(1024.0f, 1024.0f, 500, 500, false);
			mesh->SetTopolgy(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
			meshRenderer->AddMesh(mesh);
			meshRenderer->SetCulling(false);
		}

	};

	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"simple", L"simple.hlsl", ColorProp,
			ShaderArg{}, info);

		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> root = CreateGameObject(L"Light");

		root->_transform->SetLocalPosition(vec3(0, 0, 0.0f));
		root->_transform->SetLocalScale(vec3(1.0f,1.0f,1.0f));
		auto meshRenderer = root->AddComponent<MeshRenderer>();

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);

		meshRenderer->AddMaterials({ material });

		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBoxWithColor(1.0f, vec4(1, 1, 1, 1)));
		ImguiManager::main->_light = root.get();

	}

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> dis(0.3f, 1.0f);

	for (int i = 0; i < 10; ++i)
	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"color", L"color.hlsl", ColorProp,
			ShaderArg{}, info);

		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> root = CreateGameObject(L"Object");

		root->_transform->SetLocalPosition(vec3(dis(gen)*150.0f, dis(gen)*150.0f, dis(gen)*150.0f));
		root->_transform->SetLocalScale(vec3(10.0f, 10.0f, 10.0f));
		auto meshRenderer = root->AddComponent<MeshRenderer>();

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);

		meshRenderer->AddMaterials({ material });

		vec3 randomColor = { dis(gen),dis(gen),dis(gen) };

		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBoxWithColor(1.0f, vec4(randomColor.x, randomColor.y, randomColor.z, 1)));
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
