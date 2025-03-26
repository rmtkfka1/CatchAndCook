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

	/*{
		shared_ptr<GameObject> root = CreateGameObject(L"SpriteTest");
		auto& renderer =root->AddComponent<MeshRenderer>();
		auto& sprite = root->AddComponent<Sprite>();
		sprite->SetPos(vec3(0, 0, 0));
		sprite->SetSize(vec2(100, 100));
		sprite->AddAction(make_shared<DragAction>(KeyCode::RightMouse));

		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
		material->SetPass(RENDER_PASS::UI);
		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"start", L"Textures/start.jpg");
		material->SetTexture("_BaseMap", texture);
		renderer->AddMaterials({ material });
	}*/

	{
		shared_ptr<GameObject> root = CreateGameObject(L"SpriteTest");
		auto& renderer = root->AddComponent<MeshRenderer>();

		auto& sprite = root->AddComponent<TextSprite>();
		sprite->SetPos(vec3(0 + 200.0f, 0.0f, 0.000001f));
		sprite->SetSize(vec2(300, 300));
		sprite->SetText(L"HelloWorld");
		sprite->CreateObject(512, 256, L"Arial", FontColor::WHITE, 60);

		shared_ptr<Material> material = make_shared<Material>();
		material->SetShader(ResourceManager::main->Get<Shader>(L"SpriteShader"));
		material->SetPass(RENDER_PASS::UI);

		renderer->AddMaterials({ material });

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
