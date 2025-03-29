#include "pch.h"
#include "SeaPlayerController.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Collider.h"
#include "Transform.h"
#include "TerrainManager.h"
#include "Terrain.h"
SeaPlayerController::SeaPlayerController()
{
}

SeaPlayerController::~SeaPlayerController()
{
}
void SeaPlayerController::Init()
{
	
}

void SeaPlayerController::Start()
{
	_camera = CameraManager::main->GetCamera(CameraType::SeaCamera);
	_transform = GetOwner()->_transform;
	_collider = GetOwner()->GetComponent<Collider>();

	if (auto terrian = SceneManager::main->GetCurrentScene()->Find(L"Terrain"))
	{
		cout << "찾음" << endl;
		_terrian = terrian->GetComponent<Terrain>();
	}

	_camera->SetCameraPos(_transform->GetWorldPosition() + _transform->GetForward() * 0.3f +vec3(0, _cameraHeightOffset,0));
	_camera->SetCameraLook(_transform->GetForward());
	_camera->SetCameraUp(_transform->GetUp());

}

void SeaPlayerController::Update()
{
	if (CameraManager::main->GetCameraType() == CameraType::DebugCamera)
		return;

	float dt = Time::main->GetDeltaTime();
	CalCulateYawPitchRoll();

	Quaternion rotation = Quaternion::CreateFromYawPitchRoll(_yaw * D2R, _pitch * D2R, 0);
	_transform->SetLocalRotation(rotation);
	_camera->SetCameraRotation(_yaw, _pitch, 0);

	vec3 inputDir = vec3::Zero;

	if (Input::main->GetKey(KeyCode::W))
	{
		inputDir += vec3::Forward;
	}

	if (Input::main->GetKey(KeyCode::S))
	{
		inputDir += vec3::Backward;
	}

	if (Input::main->GetKey(KeyCode::A))
	{
		inputDir += vec3::Left;
	}

	if (Input::main->GetKey(KeyCode::D))
	{
		inputDir += vec3::Right;
	}

	if (Input::main->GetKey(KeyCode::Space))
	{
		inputDir += vec3(0, 2, 0);
	}


	if (inputDir != vec3::Zero)
	{

		inputDir.Normalize();

		vec3 moveDir = vec3::Transform(inputDir, rotation);

		_velocity += moveDir * _moveForce * dt;

		if (_velocity.Length() > _maxSpeed)
		{
			_velocity.Normalize();
			_velocity *=_maxSpeed;
		}
	}

	if (_velocity.Length() > 0.001f)
	{
		vec3 currentPos = _transform->GetWorldPosition();
		vec3 nextPos = currentPos + _velocity * dt;

		if (nextPos.y < _terrian->GetLocalHeight(nextPos))
		{
			nextPos.y = _terrian->GetLocalHeight(nextPos);
		}

		_transform->SetWorldPosition(nextPos);
		_camera->SetCameraPos(_transform->GetWorldPosition()  +vec3(0, _cameraHeightOffset,0));
	}

	_velocity *= (1 - (_resistance * dt));


}

void SeaPlayerController::Update2()
{
}

void SeaPlayerController::Enable()
{
}

void SeaPlayerController::Disable()
{
}

void SeaPlayerController::RenderBegin()
{
}

void SeaPlayerController::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void SeaPlayerController::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}

void SeaPlayerController::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
}

void SeaPlayerController::SetDestroy()
{
}

void SeaPlayerController::Destroy()
{
}

void SeaPlayerController::CalCulateYawPitchRoll()
{
	if (Input::main->IsMouseLock() == false)
	{
		static vec2 lastMousePos = Input::main->GetMousePosition();

		if (Input::main->GetKeyDown(KeyCode::BackQoute))
		{
			lastMousePos = Input::main->GetMousePosition();
		}
	
		vec2 currentMousePos = Input::main->GetMousePosition();

		vec2 delta = (currentMousePos - lastMousePos) * 0.1f;

		_yaw += delta.x;
		_pitch += delta.y;
	/*	_pitch = std::clamp(_pitch, -89.0f, 89.0f);*/
		_roll = 0;

		lastMousePos = currentMousePos;
	}

	else
	{
		vec2 currentMousePos = Input::main->GetMousePosition();
		vec2 centerPos = vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);
		vec2 delta = (currentMousePos - centerPos) * 0.1f;

		_yaw += delta.x;
		_pitch += delta.y;
	/*	_pitch = std::clamp(_pitch, -89.0f, 89.0f);*/
		_roll = 0;

		POINT center;
		center.x = static_cast<LONG>(WINDOW_WIDTH/2);
		center.y = static_cast<LONG>(WINDOW_HEIGHT/2);
		ClientToScreen(Core::main->GetHandle(), &center);
		SetCursorPos(center.x, center.y);
	
	}
};