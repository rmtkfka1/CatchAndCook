﻿#include "pch.h"
#include "Game.h"
#include "Core.h"
#include "GameObject.h"
#include "ICBufferInjector.h"
#include "IGuid.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "SceneManager.h"
#include "Transform.h"
#include "CameraManager.h"
#include "Camera.h"
#include "ColliderManager.h"
#include "Gizmo.h"
#include "Texture.h"
#include "Mesh.h"
#include "Shader.h"
#include "TextManager.h"
#include "LightManager.h"
#include "TerrainManager.h"
#include "InstancingManager.h"
#include "Profiler.h"
#include "ComputeManager.h"
#include "ImguiManager.h"
#include "NavMeshManager.h"
#include "Collider.h"
#include "GameObject.h"
#include "Terrain.h"
#include "PathStamp.h"
void Game::Init(HWND hwnd)
{
	IGuid::StaticInit();
	Core::main = make_unique<Core>();
	Core::main->Init(hwnd);

	Time::main = make_unique<Time>();
	Time::main->Init();

	Input::main = make_unique<Input>();

	Profiler::main = make_unique<Profiler>();
	Profiler::main->Init(_hwnd, _hInstance);

	ResourceManager::main = make_unique<ResourceManager>();
	ResourceManager::main->Init();

	NavMeshManager::main = make_unique<NavMeshManager>();
	NavMeshManager::main->Init();

	TerrainManager::main = make_unique<TerrainManager>();

	SceneManager::main = make_unique<SceneManager>();
	ColliderManager::main = make_unique<ColliderManager>();
	CameraManager::main = make_unique<CameraManager>();

	InjectorManager::main = make_unique<InjectorManager>();
	InjectorManager::main->Init();

	TextManager::main = make_unique<TextManager>();
	TextManager::main->Init();

#ifdef IMGUI_ON
	ImguiManager::main = make_unique<ImguiManager>();
	ImguiManager::main->Init();
#endif // IMGUI_ON

	ComputeManager::main = make_unique<ComputeManager>();
	ComputeManager::main->Init();

	Gizmo::main = std::make_unique<Gizmo>();
	Gizmo::main->Init();

	CameraManager::main->AddCamera(CameraType::DebugCamera, make_shared<DebugCamera>());
	CameraManager::main->AddCamera(CameraType::UiCamera, make_shared<UiCamera>());
	CameraManager::main->AddCamera(CameraType::SeaCamera, make_shared<SeaCamera>());
	CameraManager::main->GetCamera(CameraType::DebugCamera)->SetCameraPos(vec3(0, 0, -50.0f));
	CameraManager::main->SetActiveCamera(CameraType::DebugCamera);

	LightManager::main = make_unique<LightManager>();
	InstancingManager::main = make_unique<InstancingManager>();

	PathStamp::main = make_unique<PathStamp>();
	PathStamp::main->Init();

	box.Center = vec3(0, 0, 0);
	box.Extents = vec3(5.0f, 5.0f, 5.0f);



	//{
	//	std::shared_ptr<Light> light = std::make_shared<Light>();
	//	light->onOff = 1;
	//	light->direction.Normalize();
	//	light->fallOffStart = 0.0f;
	//	light->fallOffEnd = 4000.0f;
	//	light->spotPower = 300.0f;
	//	light->material.ambient = vec3(0.0,0.0, 0.0);
	//	light->material.diffuse = vec3(1.0f, 1.0f, 1.0f);
	//	light->material.specular = vec3(0, 0, 0);
	//	light->material.shininess = 61.0f;
	//	light->material.lightType = static_cast<int32>(LIGHT_TYPE::SPOT_LIGHT);
	//	light->strength = vec3(1.0f, 1.0f, 1.0f);
	//	LightManager::main->PushLight(light);
	//}

	{
		std::shared_ptr<Light> light = std::make_shared<Light>();
		light->onOff = 1;
		light->direction = vec3(-1.0f, -1.0f, 1.0f);
		light->position = vec3(0, 1000.0f, 0);
		light->direction.Normalize();

		light->material.ambient = vec3(0.4f, 0.4f, 0.4f);
		light->material.diffuse = vec3(1.0f, 1.0f, 1.0f);
		light->material.specular = vec3(0, 0, 0);
		light->material.shininess = 32.0f;
		light->material.lightType = static_cast<int32>(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		light->strength = vec3(1.0f, 1.0f, 1.0f);
		LightManager::main->PushLight(light);
	}

	auto scene = SceneManager::main->AddScene(SceneType::TestScene2);
};

void Game::PrevUpdate()
{
	if (Input::main->GetKeyDown(KeyCode::Esc))
	{
		Core::main->Fence();
		DestroyWindow(Core::main->GetHandle());
		Release();
		_quit = true;
		return;
	}
	if(Input::main->GetKeyDown(KeyCode::F5))
	{
		SceneManager::main->Reload();
	}
	if (Input::main->GetKeyDown(KeyCode::F9))
	{
		HWND hWnd = Core::main->GetHandle();
		if (!_fullScreen)
		{
			RECT rc;
			GetWindowRect(hWnd, &rc);

			MONITORINFO mi;
			mi.cbSize = sizeof(mi);
			GetMonitorInfo(MonitorFromWindow(hWnd, MONITOR_DEFAULTTOPRIMARY), &mi);

			SetWindowLong(hWnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
			SetWindowPos(hWnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top,
				mi.rcMonitor.right - mi.rcMonitor.left,
				mi.rcMonitor.bottom - mi.rcMonitor.top,
				SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

			WINDOW_WIDTH = mi.rcMonitor.right - mi.rcMonitor.left;
			WINDOW_HEIGHT = mi.rcMonitor.bottom - mi.rcMonitor.top;

			Core::main->ResizeWindowSize();
		}
		else
		{
			SetWindowLong(hWnd, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
			// ���� ũ��� ������ ����
			SetWindowPos(hWnd, HWND_TOP, 0, 0, 800, 600, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

			RECT rect;
			if (GetClientRect(hWnd, &rect))
			{
				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;
				WINDOW_WIDTH = width;
				WINDOW_HEIGHT = height;
			}
	
			Core::main->ResizeWindowSize();

		}
		_fullScreen = !_fullScreen;
		return;
	}
}

void Game::Run()
{
	std::shared_ptr<Scene> currentScene = SceneManager::main->GetCurrentScene();

	Profiler::Set("CPU_Time");

	Input::main->Update();
	Time::main->Update();
		PrevUpdate();

	if (_quit)
		return;

	CameraUpdate();

	
	Profiler::Set("CoreRenderBegin");
	Core::main->RenderBegin();
	Profiler::Fin();

	{
		Profiler::Set("Logic_Total");
			currentScene->Update();
			currentScene->RenderBegin();
				Profiler::Set("Logic_Light");
					LightManager::main->SetData();
				Profiler::Fin();
		Profiler::Fin();
	}

	
	Profiler::Set("Rendering_PASS", BlockTag::CPU);
		currentScene->Rendering();
		currentScene->DebugRendering();
		currentScene->RenderEnd();
	Profiler::Fin();
	Profiler::Fin();

	Profiler::Set("GPU_Time", BlockTag::GPU);
		Core::main->RenderEnd();
	Profiler::Fin();


	currentScene->Finish();
	Profiler::main->Reset();
}

void Game::Release()
{
	ColliderManager::main.reset(nullptr);
	SceneManager::main.reset(nullptr);
	Gizmo::main.reset(nullptr);
	InjectorManager::main.reset(nullptr);
	CameraManager::main.reset(nullptr);
	ResourceManager::main.reset(nullptr);
	Time::main.reset(nullptr);
	Input::main.reset(nullptr);
	Core::main.reset(nullptr);
	IGuid::StaticRelease();
}

void Game::CameraUpdate()
{
	if (CameraManager::main->GetActiveCamera()->GetCameraType() != CameraType::DebugCamera)
		return;

	shared_ptr<Camera> camera = CameraManager::main->GetCamera(CameraType::DebugCamera);

	static float speed = 500.0f;
	const float dt =Time::main->GetDeltaTime() *speed;

	if (Input::main->GetKey(KeyCode::Minus))
	{

		speed -= 10.0f * dt;

		if(speed<=0)
		{
			speed = 0;
		}
		
	}

	if (Input::main->GetKey(KeyCode::Plus))
	{
		if (speed <= 1000)
		{
			speed += 10.0f * dt;
		}
		else
		{
			speed = 1000;
		}
	}



	if (Input::main->GetKey(KeyCode::W))
	{
		auto prevPos = camera->GetCameraPos();
		auto direction = camera->GetCameraLook();
		camera->SetCameraPos(direction * dt + prevPos);
	}

	if (Input::main->GetKey(KeyCode::S))
	{
		auto prevPos = camera->GetCameraPos();
		auto direction = camera->GetCameraLook();
		camera->SetCameraPos(-direction * dt + prevPos);
	}

	if (Input::main->GetKey(KeyCode::D))
	{
		auto prevPos = camera->GetCameraPos();
		auto direction = camera->GetCameraRight();
		camera->SetCameraPos(direction * dt + prevPos);
	}

	if (Input::main->GetKey(KeyCode::A))
	{
		auto prevPos = camera->GetCameraPos();
		auto direction = camera->GetCameraRight();
		camera->SetCameraPos(-direction * dt + prevPos);
	}
	if(Input::main->GetKey(KeyCode::Space))
	{
		auto prevPos = camera->GetCameraPos();
		camera->SetCameraPos(Vector3::Up * dt + prevPos);
	}

	if(Input::main->GetKey(KeyCode::Shift))
	{
		auto prevPos = camera->GetCameraPos();
		camera->SetCameraPos(Vector3::Down * dt + prevPos);
	}

	if (Input::main->GetMouse(KeyCode::RightMouse))
	{
		static vec2 lastMousePos;
		static vec2 sumDelta =vec2::Zero;
		vec2 currentMousePos = Input::main->GetMousePosition();

		//Ƣ������ ���
		if (Input::main->GetMouseDown(KeyCode::RightMouse))
		{
			lastMousePos = currentMousePos;
		}

		float speed = 0.3f;

		vec2 delta = (currentMousePos - lastMousePos)*speed;
		lastMousePos = currentMousePos;

		sumDelta += delta;

		camera->SetCameraRotation(sumDelta.x, sumDelta.y, 0);
	}
	
	if (Input::main->GetMouseDown(KeyCode::LeftMouse))
	{
		//Ray ray;
		//vec2 mouseXY = Input::main->GetNDCMouseDownPosition(KeyCode::LeftMouse);
		//vec3 cursorNdcNear = vec3(mouseXY.x, mouseXY.y, 0.0f);
		//vec3 cursorNdcFar = vec3(mouseXY.x, mouseXY.y, 1.0f);

		//Matrix inverseProjView = CameraManager::main->GetActiveCamera()->GetCameraParam().InvertVPMatrix;

		//vec3 cursorWorldNear =
		//	vec3::Transform(cursorNdcNear, inverseProjView);

		//vec3 cursorWorldFar =
		//	vec3::Transform(cursorNdcFar, inverseProjView);

		//vec3 dir = cursorWorldFar - cursorWorldNear;
		//dir.Normalize();

		//ray.position = cursorWorldNear;
		//ray.direction = dir;

		//float dist = 0;

		//RayHit rayhit=  ColliderManager::main->RayCast(ray, dist);

		//cout << rayhit.distance << endl;
		//cout << rayhit.normal.x << " " << rayhit.normal.y << " " << rayhit.normal.z << endl;
		//cout << rayhit.worldPos.x << " " << rayhit.worldPos.y << " " << rayhit.worldPos.z << endl;

	}

	/*if (auto& terrain = SceneManager::main->GetCurrentScene()->Find(L"Terrain"))
	{
		cout << terrain->GetComponent<Terrain>()->GetLocalHeight(camera->GetCameraPos()) << endl;;
	}*/

}

void Game::SetHandle(HWND hwnd, HINSTANCE hInst)
{
	_hwnd = hwnd;
	_hInstance = hInst;
}
