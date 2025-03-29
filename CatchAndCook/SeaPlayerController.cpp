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

}

void SeaPlayerController::Start()
{
	_camera = CameraManager::main->GetCamera(CameraType::SeaCamera);
	_transform = GetOwner()->_transform;
	_collider = GetOwner()->GetComponent<Collider>();


}

void SeaPlayerController::Update()
{
	//if (CameraManager::main->GetCameraType() == CameraType::DebugCamera)
	//	return;

	 CalCulateYawPitchRoll();

	 Quaternion yaw = Quaternion::CreateFromYawPitchRoll(_yaw * D2R, _pitch*D2R, 0);
	_transform->SetLocalRotation(yaw);

	vec3 movedir = vec3::Zero;

	if (Input::main->GetKey(KeyCode::UpArrow))
	{
		movedir += _transform->GetForward();
	}

	if (Input::main->GetKey(KeyCode::DownArrow))
	{
		movedir -= _transform->GetForward();
	}

	if (Input::main->GetKey(KeyCode::RightArrow))
	{
		movedir += _transform->GetRight();
	}

	if (Input::main->GetKey(KeyCode::LeftArrow))
	{
		movedir -= _transform->GetRight();
	}

	_transform->SetLocalPosition(_transform->GetLocalPosition() + movedir * 0.1f);
	_camera->SetCameraRotation(_yaw, _pitch, 0);
	_camera->SetCameraPos(_transform->GetWorldPosition() +_transform->GetForward()*0.3f);

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
	static vec2 lastMousePos = Input::main->GetMousePosition();
	vec2 currentMousePos = Input::main->GetMousePosition();

	vec2 delta = (currentMousePos - lastMousePos) * 0.1f; 

	_yaw += delta.x;
	_pitch += delta.y;
	_roll = 0;

	lastMousePos = currentMousePos;
}
