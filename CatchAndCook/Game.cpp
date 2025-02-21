#include "pch.h"
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

void Game::Init(HWND hwnd)
{
	IGuid::StaticInit();
	Time::main = make_unique<Time>();
	Time::main->Init();
	Input::main = make_unique<Input>();
	Core::main = make_unique<Core>();
	Core::main->Init(hwnd);

	ResourceManager::main = make_unique<ResourceManager>();
	ResourceManager::main->Init();

	TerrainManager::main = make_unique<TerrainManager>();

	SceneManager::main = make_unique<SceneManager>();
	ColliderManager::main = make_unique<ColliderManager>();
	CameraManager::main = make_unique<CameraManager>();

	InjectorManager::main = make_unique<InjectorManager>();
	InjectorManager::main->Init();

	//------------------

	Gizmo::main = std::make_unique<Gizmo>();
	Gizmo::main->Init();


	CameraManager::main->AddCamera(CameraType::ThirdPersonCamera, static_pointer_cast<Camera>(make_shared<ThirdPersonCamera>()));
	CameraManager::main->GetCamera(CameraType::ThirdPersonCamera)->SetCameraPos(vec3(0, 0, -5.0f));
	CameraManager::main->SetActiveCamera(CameraType::ThirdPersonCamera);

	TextManager::main = make_unique<TextManager>();
	TextManager::main->Init();

	LightManager::main = make_unique<LightManager>();
	{
		Light light;

		light.direction = vec3(0,-1.0f,-1.0f);
		light.material.ambient = vec3(1.0f,1.0f,1.0f);
		light.material.diffuse = vec3(0.5f,0.5f,0.5f);
		light.material.specular = vec3(1.0f,1.0f,1.0f);
		light.material.shininess = 64.0f;
		light.material.lightType = static_cast<int32>(LIGHT_TYPE::DIRECTIONAL_LIGHT);
		light.strength = vec3(1.0f,1.0f,1.0f);
		LightManager::main->PushLight(light);
	}


	auto scene = SceneManager::main->AddScene(SceneType::TestScene2);
}

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

	Input::main->Update();
	Time::main->Update();
	PrevUpdate();
	if (_quit)
		return;

	CameraUpdate();

	std::shared_ptr<Scene> currentScene = SceneManager::main->GetCurrentScene();
	Core::main->RenderBegin();
	LightManager::main->SetData();

	auto camera = CameraManager::main->GetActiveCamera();
	camera->Calculate();
	Vector3 worldPos = camera->GetScreenToWorldPosition(Input::main->GetMousePosition());
	Vector3 worldDir = (worldPos - camera->GetCameraPos());
	worldDir.Normalize();
	float dis = 1000;
	auto a = ColliderManager::main->RayCast({worldPos,worldDir}, dis);
	if(a)
	{
		Gizmo::Ray(a.worldPos, a.normal, 1);
	}

	currentScene->Update();
	currentScene->RenderBegin();
	currentScene->Rendering();
	currentScene->DebugRendering();
	currentScene->RenderEnd();
	Core::main->RenderEnd();
	currentScene->Finish();

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
	shared_ptr<Camera> camera = CameraManager::main->GetActiveCamera();

	const float speed = 60.0f;
	const float dt =Time::main->GetDeltaTime() *speed;

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

	if (Input::main->GetMouse(KeyCode::LeftMouse))
	{
		static vec2 lastMousePos;
		vec2 currentMousePos = Input::main->GetMousePosition();

		//Ƣ������ ���
		if (Input::main->GetMouseDown(KeyCode::LeftMouse))
		{
			lastMousePos = currentMousePos;
		}

		float speed = 0.3f;

		vec2 delta = (currentMousePos - lastMousePos)*speed;
		lastMousePos = currentMousePos;

		camera->SetCameraRotation(delta.x, delta.y, 0);
	}

	


}
