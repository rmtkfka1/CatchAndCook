#include "pch.h"
#include "TestScene.h"
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
void TestScene::Init()
{
	Scene::Init();

	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
	
		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"test", L"test.hlsl", StaticProp,
			ShaderArg{}, info);

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"start", L"Textures/start.jpg");
		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"test gameObject");
		gameObject->transform->SetLocalPosition(vec3(0, 0.3f, 0.8f));
		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetInjector({ InjectorManager::main->Get(BufferType::MateriaSubParam) });
		material->SetTexture("g_tex_0", texture);
	
		meshRenderer->AddMaterials({ material });
		meshRenderer->SetMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
	}


	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;
	
		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"cubemap", L"cubemap.hlsl", StaticProp,
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
		meshRenderer->SetMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
	}


	{

		for (int i = 0; i < 5; ++i)
		{
			shared_ptr<GameObject> gameObject = CreateGameObject(L"SpriteTest");
			auto spriteRender = gameObject->AddComponent<SpriteRenderer>();

			shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"spriteTest", L"Textures/spriteTest.jpg");
			shared_ptr<BasicSprite> sprite = make_shared<BasicSprite>();

			sprite->AddAction([=]() {SpriteAction::OnDragAction(KeyCode::RightMouse); });
			sprite->AddAction([=]() { SpriteAction::OnClickAction(KeyCode::LeftMouse); });

			spriteRender->SetSprite(sprite);
	
		
			//풀UV 맵핑
			if (i == 4)
			{
				sprite->SetTexture(texture);
				sprite->SetPos(vec3(0 + i * WINDOW_WIDTH / 5, 0, 0.1f));
				sprite->SetSize(vec2(WINDOW_WIDTH / 5, WINDOW_HEIGHT / 3));
				sprite->AddCollisonMap();
			}

			//스프라이트 짤라서 사용.
			else
			{
				RECT rect;
				rect.left = 1024/7 * i;
				rect.top = 0;
				rect.right = 1024/7 * (i+1);
				rect.bottom = 1024/4;
				sprite->SetPos(vec3(0 + i * WINDOW_WIDTH / 5, 0, 0.99f - 0.01f*i));
				sprite->SetSize(vec2(WINDOW_WIDTH / 5, WINDOW_HEIGHT / 3));
				sprite->SetTexture(texture);
				sprite->SetUVCoord(&rect);
				sprite->AddCollisonMap();
			}
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
