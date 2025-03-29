#include "pch.h"
#include "SeaPlayerController.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Collider.h"
#include "Transform.h"

SeaPlayerController::SeaPlayerController()
{
}

SeaPlayerController::~SeaPlayerController()
{
}
void SeaPlayerController::Init()
{
	_centerMousePos = vec2(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
	POINT center;
	center.x = static_cast<LONG>(_centerMousePos.x);
	center.y = static_cast<LONG>(_centerMousePos.y);
	ClientToScreen(Core::main->GetHandle(), &center);
	SetCursorPos(center.x, center.y);

}

void SeaPlayerController::Start()
{
	_camera = CameraManager::main->GetCamera(CameraType::SeaCamera);
	_transform = GetOwner()->_transform;
	_collider = GetOwner()->GetComponent<Collider>();


}

void SeaPlayerController::Update()
{
	float dt = Time::main->GetDeltaTime();
	CalCulateYawPitchRoll();
	Quaternion rotation = Quaternion::CreateFromYawPitchRoll(_yaw * D2R, _pitch * D2R, 0);
	_transform->SetLocalRotation(rotation);

	vec3 inputDir = vec3(
		(Input::main->GetKey(KeyCode::RightArrow) ? 1.0f : 0.0f) - (Input::main->GetKey(KeyCode::LeftArrow) ? 1.0f : 0.0f),
		0,
		(Input::main->GetKey(KeyCode::UpArrow) ? 1.0f : 0.0f) - (Input::main->GetKey(KeyCode::DownArrow) ? 1.0f : 0.0f)
	);


	if (inputDir.LengthSquared() > 0.001f)
		inputDir.Normalize();

	vec3 moveDir = vec3::Transform(inputDir, rotation);

	_velocity += moveDir * _moveForce * dt ;

	if (_velocity.Length() > _maxSpeed)
	{
		_velocity.Normalize();
		_velocity *= _maxSpeed;
	}

	vec3 currentPos = _transform->GetLocalPosition();
	vec3 nextPos = currentPos + _velocity * dt;
	_transform->SetLocalPosition(nextPos);

	_velocity *= (1 - (_resistance * dt));

	_camera->SetCameraRotation(_yaw, _pitch, 0);
	_camera->SetCameraPos(_transform->GetWorldPosition() + _transform->GetForward() * 0.3f);

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
		vec2 currentMousePos = Input::main->GetMousePosition();

		vec2 delta = (currentMousePos - lastMousePos) * 0.1f;

		_yaw += delta.x;
		_pitch += delta.y;
		_pitch = std::clamp(_pitch, -89.0f, 89.0f);
		_roll = 0;

		lastMousePos = currentMousePos;
	}

	else
	{
	

		vec2 currentMousePos = Input::main->GetMousePosition();

		vec2 delta = (currentMousePos - _centerMousePos) * 0.1f;

		_yaw += delta.x;
		_pitch += delta.y;
		_pitch = std::clamp(_pitch, -89.0f, 89.0f);
		_roll = 0;

		SetCursorPos(Input::main->GetCenterMousePos().x, Input::main->GetCenterMousePos().y);
	
	}
};