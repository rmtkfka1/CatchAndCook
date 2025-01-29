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
#include "testComponent.h"
#include "StructuredBuffer.h"
#include "Mesh.h"
#include "ScreenParticle.h"

void TestScene::Init()
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
		root->_transform->SetLocalScale(vec3(3000.0f, 0.1f, 0.1f));
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

		shared_ptr<GameObject> root = CreateGameObject(L"X");

		root->_transform->SetLocalPosition(vec3(0, 0, 3000.0f));
		root->_transform->SetLocalScale(vec3(0.1f, 0.1f, 3000.0f));
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

		shared_ptr<GameObject> root = CreateGameObject(L"X");

		root->_transform->SetLocalPosition(vec3(0, 3000.0f, 0.0f));
		root->_transform->SetLocalScale(vec3(0.1f, 3000.0f, 0.1f));
		auto meshRenderer = root->AddComponent<MeshRenderer>();

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBoxWithColor(1.0f, vec4(0, 0, 1, 0)));
	}
#pragma endregion

	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"test", L"test.hlsl", StaticProp,
			ShaderArg{}, info);

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"start", L"Textures/start.jpg");
		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> root = CreateGameObject(L"root_test");
	
		auto meshRenderer = root->AddComponent<MeshRenderer>();
		meshRenderer->SetDebugShader(ResourceManager::main->Get<Shader>(L"DebugNormal"));

		root->AddComponent<testComponent>();

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetInjector({ InjectorManager::main->Get(BufferType::MateriaSubParam) });
		material->SetHandle("g_tex_0", texture->GetSRVCpuHandle());

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
	
	}




	//{

	//	ShaderInfo info;
	//	info._zTest = true;
	//	info._stencilTest = false;
	//	info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;

	//	shared_ptr<Shader> shader = make_shared<Shader>();
	//	shader->Init(L"screenParticle.hlsl", StaticProp, ShaderArg{ {{"PS_Main", "ps"},{"VS_Main", "vs"},
	//		{"GS_Main", "gs"}} }, info);

	//	shared_ptr<Material> material = make_shared<Material>();

	//	shared_ptr<GameObject> object = CreateGameObject(L"testing_fire");

	//	object->_transform->SetLocalPosition(vec3(0, 0.0f, 50.0f));
	//	auto meshRenderer = object->AddComponent<MeshRenderer>();

	//	object->AddComponent<ScreenParticle>();
	//
	//	material = make_shared<Material>();
	//	material->SetShader(shader);
	//	material->SetPass(RENDER_PASS::Forward);
	//	meshRenderer->AddMaterials({ material });

	//	auto& mesh = GeoMetryHelper::LoadRectangleBox(10.0f);
	//	mesh->SetTopolgy(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	//	mesh->SetVertexCount(2048);
	//	meshRenderer->AddMesh(mesh);
	//};




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
		material->SetHandle("g_tex_0", texture->GetSRVCpuHandle());

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBox(1.0f));
	}

	{
		//18프레임 => LOD 기능 240 방어 가능할듯.
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::WIREFRAME;
		info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"seatest", L"seatest.hlsl", StaticProp,
			ShaderArg{ {{"VS_Main", "vs"}, {"PS_Main", "ps"}, {"HS_Main", "hs"},{"DS_Main", "ds"}  }}, info);
	
		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"grid_orgin");
		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();

		//meshRenderer->SetDebugShader(ResourceManager::main->Get<Shader>(L"DebugNormal_Sea"));
		gameObject->_transform->SetLocalPosition(vec3(0, 10.0f, 0));

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"start", L"Textures/start.jpg");
		material->SetHandle("g_tex_0", texture->GetSRVCpuHandle());

		meshRenderer->AddMaterials({ material });

		auto& mesh = GeoMetryHelper::LoadGripMeshControlPoints(2000.0f, 2000.0f, 100, 100);
		mesh->SetTopolgy((D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST));
		meshRenderer->AddMesh(mesh);
	}


	//{
	//	/*meshRenderer->AddMesh(GeoMetryHelper::LoadGripMesh(2000.0f, 2000.0f, 4000, 4000));*/ 	//11프레임
	//	ShaderInfo info;
	//	info._zTest = true;
	//	info._stencilTest = false;
	//	info.cullingType = CullingType::WIREFRAME;

	//	shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"testgrid", L"sea.hlsl", StaticProp,
	//		ShaderArg{}, info);

	//	shared_ptr<Material> material = make_shared<Material>();

	//	shared_ptr<GameObject> gameObject = CreateGameObject(L"grid");
	//	auto meshRenderer = gameObject->AddComponent<MeshRenderer>();

	//	gameObject->_transform->SetLocalPosition(vec3(100.0f, 0, 0));

	//	material = make_shared<Material>();
	//	material->SetShader(shader);
	//	material->SetPass(RENDER_PASS::Forward);
	//	material->SetHandle("g_tex_0", ResourceManager::main->GetNoneTexture()->GetSRVCpuHandle());

	//	meshRenderer->AddMaterials({ material });
	//	meshRenderer->AddMesh(GeoMetryHelper::LoadGripMesh(2000.0f, 2000.0f, 4000, 4000));
	//}

#pragma region sprite
	//for (int i = 0; i < 1; ++i)
	//{
	//	shared_ptr<GameObject> gameObject = CreateGameObject(L"SpriteTest");
	//	auto spriteRender = gameObject->AddComponent<SpriteRenderer>();

	//	shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"spriteTest", L"Textures/spriteTest.jpg");
	//	shared_ptr<BasicSprite> sprite = make_shared<BasicSprite>();

	//	spriteRender->SetSprite(sprite);
	//	sprite->AddAction(make_shared<DragAction>(KeyCode::LeftMouse));
	//	sprite->AddAction(make_shared<EnableDisableKeyAction>(KeyCode::I));

	//	SpriteRect rect;
	//	rect.left = 1024 / 7 * i;
	//	rect.top = 0;
	//	rect.right = 1024 / 7 * (i + 1);
	//	rect.bottom = 1024 / 4;
	//	sprite->SetPos(vec3(0 + i * WINDOW_WIDTH / 5, 0, 0.04f));
	//	sprite->SetSize(vec2(WINDOW_WIDTH / 5, WINDOW_HEIGHT / 3));
	//	sprite->SetTexture(texture);
	//	sprite->SetUVCoord(rect);

	//	{
	//		shared_ptr<BasicSprite> childSprite = make_shared<BasicSprite>();
	//		childSprite->SetPos(vec3(200, 0, 0.03f));
	//		childSprite->SetSize(vec2(50, 50));
	//		childSprite->SetTexture(ResourceManager::main->Load<Texture>(L"disable", L"Textures/disable.png"));
	//		childSprite->AddAction(make_shared<DisableMouseAction>(KeyCode::LeftMouse));
	//		sprite->AddChildern(childSprite);
	//	}

	//};

	//{

	//	shared_ptr<GameObject> gameObject = CreateGameObject(L"AnimationSprite");
	//	auto spriteRender = gameObject->AddComponent<SpriteRenderer>();
	//	shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"fire", L"Textures/fire.png");
	//	shared_ptr<AnimationSprite> sprite = make_shared<AnimationSprite>();

	//	spriteRender->SetSprite(sprite);
	//	sprite->SetTexture(texture);
	//	sprite->SetPos(vec3(0, 0, 0.3f));
	//	sprite->SetSize(vec2(500, 500));
	//	sprite->SetFrameRate(0.05f);
	//	sprite->SetClipingColor(vec4(0, 0, 0, 1.0f));		https://imagecolorpicker.com/

	//	const float TextureSize = 512.0f;

	//	for (int i = 0; i < 5; ++i)
	//	{
	//		float add = i * TextureSize / 5;
	//		for (int j = 0; j < 5; ++j)
	//		{

	//			SpriteRect rect;
	//			rect.left = 0 + j * TextureSize / 5;
	//			rect.top = add;
	//			rect.right = rect.left + TextureSize / 5;
	//			rect.bottom = rect.top + TextureSize / 5;

	//			sprite->PushUVCoord(rect);
	//		}
	//	}
	//};

	//{
	//	shared_ptr<GameObject> gameObject = CreateGameObject(L"TextTest");
	//	auto spriteRender = gameObject->AddComponent<SpriteRenderer>();
	//	shared_ptr<TextSprite> sprite = make_shared<TextSprite>();

	//	spriteRender->SetSprite(sprite);
	//	sprite->SetPos(vec3(300.0f, 0.0f, 0.000001));
	//	sprite->SetSize(vec2(300, 300));
	//	sprite->SetText(L"캐치앤쿡");
	//	sprite->CreateObject(512, 256, L"Arial", FontColor::WHITE, 123);
	//}
#pragma endregion

};

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
