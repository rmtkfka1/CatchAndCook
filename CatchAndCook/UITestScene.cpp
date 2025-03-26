#include "pch.h"
#include "UITestScene.h"
#include "Shader.h"
#include "Texture.h"
#include "GameObject.h"
#include "Transform.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include <random>
#include "Sprite.h"
#include "SpriteAction.h"
#include "TextManager.h"

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
		material->SetTexture("g_tex_0", texture);

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
		meshRenderer->SetCulling(false);
	}

	{
		shared_ptr<GameObject> root = CreateGameObject(L"Sprite1");
		auto& renderer = root->AddComponent<MeshRenderer>();
		auto& sprite = root->AddComponent<Sprite>();
		sprite->AddAction(make_shared<DragAction>(KeyCode::LeftMouse));
		sprite->SetLocalPos(vec3(300, 0, 0));
		sprite->SetSize(vec2(100, 100));

		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
		material->SetPass(RENDER_PASS::UI);
		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"start", L"Textures/start.jpg");
		material->SetTexture("_BaseMap", texture);
		renderer->AddMaterials({ material });


		{
			shared_ptr<GameObject> child = CreateGameObject(L"Sprite2");
			auto& renderer = child->AddComponent<MeshRenderer>();
			auto& sprite = child->AddComponent<Sprite>();
			child->SetParent(root);
			sprite->SetLocalPos(vec3(-100, 0, 0));
			sprite->SetSize(vec2(50, 50));

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
			material->SetPass(RENDER_PASS::UI);
			shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"disable", L"Textures/disable.png");
			material->SetTexture("_BaseMap", texture);
			renderer->AddMaterials({ material });

			{
				shared_ptr<GameObject> child2 = CreateGameObject(L"Sprite3");
				auto& renderer = child2->AddComponent<MeshRenderer>();
				auto& sprite = child2->AddComponent<Sprite>();
				child2->SetParent(child);
				sprite->SetSize(vec2(50, 50));
				sprite->SetLocalPos(vec3(100, 0, 0));


				shared_ptr<Material> material = make_shared<Material>();
				material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
				material->SetPass(RENDER_PASS::UI);
				shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"disable", L"Textures/disable.png");
				material->SetTexture("_BaseMap", texture);
				renderer->AddMaterials({ material });

			}

		};
	}

	{
		shared_ptr<GameObject> root = CreateGameObject(L"SpriteTest");
		auto& renderer = root->AddComponent<MeshRenderer>();

		auto& sprite = root->AddComponent<TextSprite>();
		sprite->SetLocalPos(vec3(0 + 200.0f, 0.0f, 0.000001f));
		sprite->SetSize(vec2(300, 300));
		sprite->SetText(L"HelloWorld");
		sprite->CreateObject(512, 256, L"Arial", FontColor::WHITE, 60);

		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
		material->SetPass(RENDER_PASS::UI);

		renderer->AddMaterials({ material });

	}

	{
		{

			shared_ptr<GameObject> gameObject = CreateGameObject(L"AnimationSprite");
			auto meshRenderer = gameObject->AddComponent<MeshRenderer>();
			shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"fire", L"Textures/fire.png");
			auto& sprite = gameObject->AddComponent<AnimationSprite>();
			sprite->SetLocalPos(vec3(0, 0, 0.3f));
			sprite->SetSize(vec2(500, 500));
			sprite->SetFrameRate(0.001f);
			sprite->SetClipingColor(vec4(0, 0, 0, 1.0f));		https://imagecolorpicker.com/

			const float TextureSize = 512.0f;

			for (int i = 0; i < 5; ++i)
			{
				float add = i * TextureSize / 5;
				for (int j = 0; j < 5; ++j)
				{

					SpriteRect rect;
					rect.left = 0 + j * TextureSize / 5;
					rect.top = add;
					rect.right = rect.left + TextureSize / 5;
					rect.bottom = rect.top + TextureSize / 5;

					sprite->PushUVCoord(rect);
				}
			}

			shared_ptr<Material> material = make_shared<Material>();
			material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
			material->SetPass(RENDER_PASS::UI);
			material->SetTexture("_BaseMap", texture);

			meshRenderer->AddMaterials({ material });

		};

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
