#include "pch.h"
#include "TestScene.h"
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "SpriteRenderer.h"
void TestScene::Init()
{
	Scene::Init();

	{
		ShaderInfo info;
		info._zTest = false;
		info._stencilTest = false;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"test.hlsl", L"test.hlsl", StaticProp,
			ShaderArg{}, info);

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"start", L"Textures/start.jpg");
		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"test gameObject");
		gameObject->transform->SetLocalPosition(vec3(0, 0.3f, 0));
		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetInjector({ InjectorManager::main->Get(BufferType::MateriaSubParam) });
		material->SetTexture("g_tex_0", texture);
		material->SetPropertyVector("uv", vec4(0.3, -0.3, 0, 0));

		meshRenderer->AddMaterials({ material });
		meshRenderer->SetMesh(GeoMetryHelper::LoadRectangleMesh(1.0f));
	}

	{

		for (int i = 0; i < 5; ++i)
		{
			shared_ptr<GameObject> gameObject = CreateGameObject(L"SpriteTest");
			auto spriteRender = gameObject->AddComponent<SpriteRenderer>();

			shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"start", L"Textures/start.jpg");

			RECT rect { 0,0,440*5,440 * 5 };
			spriteRender->SetTexture(texture, &rect);
			spriteRender->SetPos(vec3(0+ i * WINDOW_WIDTH/5, 0, 0.5f));
			spriteRender->SetSize(vec2(WINDOW_WIDTH/5, WINDOW_HEIGHT / 3));
			spriteRender->_temp = i;




		}
	}

}

void TestScene::Update()
{
	Scene::Update();
}

void TestScene::RenderBegin()
{
	Scene::RenderBegin();
}

void TestScene::Rendering()
{
	Scene::Rendering();
}

void TestScene::RenderEnd()
{
	Scene::RenderEnd();
}

void TestScene::Finish()
{
	Scene::Finish();
}
