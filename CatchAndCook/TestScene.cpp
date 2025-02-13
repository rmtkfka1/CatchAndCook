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
#include "WaterHeight.h"
#include "Terrain.h"

void TestScene::Init()
{
	Scene::Init();


	//{
	//	auto& a = ResourceManager::main->Load<Model>(L"kind",L"../Resources/Models/Kindred/kindred_unity.fbx",VertexType::Vertex_GeoMetry);
	//	auto& object =a->CreateGameObject(GetCast<Scene>());
	//	object->_transform->SetLocalPosition(vec3(0,500.0f,0));
	//}

	/*{
		auto& a = ResourceManager::main->Load<SceneLoader>(L"TestScene1",L"../Resources/Datas/Scenes/TestScene1.json");
		auto& object= a->Load(GetCast<Scene>());
	}*/



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
		root->_transform->SetLocalScale(vec3(3000.0f,1.0f,1.0f));
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

		shared_ptr<GameObject> root = CreateGameObject(L"Y");

		root->_transform->SetLocalPosition(vec3(0, 0, 3000.0f));
		root->_transform->SetLocalScale(vec3(1.0f,1.0f, 3000.0f));
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

		shared_ptr<GameObject> root = CreateGameObject(L"Z");

		root->_transform->SetLocalPosition(vec3(0, 3000.0f, 0.0f));
		root->_transform->SetLocalScale(vec3(1.0f, 3000.0f,1.0f));
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

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"test", L"test.hlsl", GeoMetryProp,
			ShaderArg{}, info);

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"start", L"Textures/start.jpg");
		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> root = CreateGameObject(L"root_test");

		//root->GetComponent<Transform>()->_useTerrain=true;
	
		auto meshRenderer = root->AddComponent<MeshRenderer>();
		meshRenderer->SetDebugShader(ResourceManager::main->Get<Shader>(L"DebugNormal"));

		root->_transform->SetLocalScale(vec3(1,1,1));
	/*	root->AddComponent<WaterHeight>()->SetOffset(6.0f);*/
		root->AddComponent<testComponent>();
	
		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		material->SetHandle("g_tex_0", texture->GetSRVCpuHandle());

		meshRenderer->AddMaterials({ material });
		meshRenderer->AddMesh(GeoMetryHelper::LoadRectangleBox(6.0f));
	
	}

	{
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::NONE;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"cubemap", L"cubemap.hlsl",GeoMetryProp,
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
		ShaderInfo info;
		info._zTest = true;
		info._stencilTest = false;
		info.cullingType = CullingType::WIREFRAME;
		//info.cullingType = CullingType::NONE;
		info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;

		shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"seatest",L"seatest.hlsl",GeoMetryProp,
			ShaderArg{{{"VS_Main","vs"},{"PS_Main","ps"},{"HS_Main","hs"},{"DS_Main","ds"}}},info);

		shared_ptr<Material> material = make_shared<Material>();

		shared_ptr<GameObject> gameObject = CreateGameObject(L"grid_orgin");
		auto meshRenderer = gameObject->AddComponent<MeshRenderer>();

		//meshRenderer->SetDebugShader(ResourceManager::main->Get<Shader>(L"DebugNormal_Sea"));
		gameObject->_transform->SetLocalPosition(vec3(0,0,0));

		material = make_shared<Material>();
		material->SetShader(shader);
		material->SetPass(RENDER_PASS::Forward);
		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"Sea",L"Textures/sea/sea.jpg");
		shared_ptr<Texture> texture1 = ResourceManager::main->Load<Texture>(L"Sea2",L"Textures/sea/sea2.jpg");
		material->SetHandle("g_tex_0",texture->GetSRVCpuHandle());
		material->SetHandle("g_tex_1",texture1->GetSRVCpuHandle());

		meshRenderer->AddMaterials({material});

		auto& mesh = GeoMetryHelper::LoadGripMeshControlPoints(2000.0f,2000.0f,20,20);
		mesh->SetTopolgy(D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
		meshRenderer->AddMesh(mesh);
	}


	//{
	//	ShaderInfo info;
	//	info._zTest = true;
	//	info._stencilTest = false;
	//	info.cullingType = CullingType::WIREFRAME;
	//	info._primitiveType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;

	//	shared_ptr<Shader> shader = ResourceManager::main->Load<Shader>(L"TerrainTest",L"Terrain.hlsl",StaticProp,
	//	ShaderArg{{{"VS_Main","vs"},{"PS_Main","ps"},{"HS_Main","hs"},{"DS_Main","ds"}}},info);

	//	shared_ptr<Material> material = make_shared<Material>();

	//	shared_ptr<GameObject> gameObject = CreateGameObject(L"Terrain");

	//	auto meshRenderer = gameObject->AddComponent<MeshRenderer>();

	//	auto& terrain = gameObject->AddComponent<Terrain>();
	//	terrain->SetHeightMap(L"../Resources/Textures/HeightMap/Terrain_Height.raw",L"../Resources/Textures/HeightMap/Terrain_Height.png",vec2(5000.0f,5000.0f));
	//	gameObject->_transform->SetLocalPosition(vec3(0, 600.0f,0));
	//	  
	//	material = make_shared<Material>();
	//	material->SetHandle("g_tex_0",ResourceManager::main->Load<Texture>(L"HeightMap",L"Textures/HeightMap/terrainAlbedo.png")->GetSRVCpuHandle());
	//	material->SetShader(shader);
	//	material->SetPass(RENDER_PASS::Forward);
	//
	//	meshRenderer->AddMaterials({ material });
	//}

	
#pragma region sprite
	for (int i = 0; i < 1; ++i)
	{
		shared_ptr<GameObject> gameObject = CreateGameObject(L"SpriteTest");
		auto spriteRender = gameObject->AddComponent<SpriteRenderer>();

		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"spriteTest", L"Textures/spriteTest.jpg");
		shared_ptr<BasicSprite> sprite = make_shared<BasicSprite>();

		spriteRender->SetSprite(sprite);
		sprite->AddAction(make_shared<DragAction>(KeyCode::LeftMouse));
		sprite->AddAction(make_shared<EnableDisableKeyAction>(KeyCode::I));

		SpriteRect rect;
		rect.left = 1024 / 7 * i;
		rect.top = 0;
		rect.right = 1024 / 7 * (i + 1);
		rect.bottom = 1024 / 4;
		sprite->SetPos(vec3(0 + i * WINDOW_WIDTH / 5, 0, 0.04f));
		sprite->SetSize(vec2(WINDOW_WIDTH / 5, WINDOW_HEIGHT / 3));
		sprite->SetTexture(texture);
		sprite->SetUVCoord(rect);

		{
			shared_ptr<BasicSprite> childSprite = make_shared<BasicSprite>();
			childSprite->SetPos(vec3(200, 0, 0.03f));
			childSprite->SetSize(vec2(50, 50));
			childSprite->SetTexture(ResourceManager::main->Load<Texture>(L"disable", L"Textures/disable.png"));
			childSprite->AddAction(make_shared<DisableMouseAction>(KeyCode::LeftMouse));
			sprite->AddChildern(childSprite);
		}

	};

	{

		shared_ptr<GameObject> gameObject = CreateGameObject(L"AnimationSprite");
		auto spriteRender = gameObject->AddComponent<SpriteRenderer>();
		shared_ptr<Texture> texture = ResourceManager::main->Load<Texture>(L"fire", L"Textures/fire.png");
		shared_ptr<AnimationSprite> sprite = make_shared<AnimationSprite>();

		spriteRender->SetSprite(sprite);
		sprite->SetTexture(texture);
		sprite->SetPos(vec3(0, 0, 0.3f));
		sprite->SetSize(vec2(500, 500));
		sprite->SetFrameRate(0.05f);
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
	};

	{
		shared_ptr<GameObject> gameObject = CreateGameObject(L"TextTest");
		auto spriteRender = gameObject->AddComponent<SpriteRenderer>();
		shared_ptr<TextSprite> sprite = make_shared<TextSprite>();

		spriteRender->SetSprite(sprite);
		sprite->SetPos(vec3(300.0f, 0.0f, 0.000001));
		sprite->SetSize(vec2(300, 300));
		sprite->SetText(L"CATCH & COOK");
		sprite->CreateObject(512, 256, L"Arial", FontColor::WHITE, 60);
	}
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
