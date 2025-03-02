#include "pch.h"
#include "LightingTest.h"
#include "Transform.h"
#include "MeshRenderer.h"
#include "random"
void LightingTest::Init()
{
	Scene::Init();

	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"color", L"color.hlsl", ColorProp,
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


	}

	random_device rd;
	mt19937 gen(rd());
	uniform_real_distribution<float> dis(0.3f, 1.0f);

	for (int i = 0; i < 30; ++i)
	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"color", L"color.hlsl", ColorProp,
			ShaderArg{}, info);

		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> root = CreateGameObject(L"Object");

		root->_transform->SetLocalPosition(vec3(20+ i*50, 0, 0.0f));
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
