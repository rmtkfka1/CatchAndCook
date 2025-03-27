#include "pch.h"
#include "SeaPlayerComponent.h"
#include "CameraManager.h"
#include "Camera.h"
#include "Transform.h"
SeaPlayerComponent::SeaPlayerComponent()
{
}

SeaPlayerComponent::~SeaPlayerComponent()
{
}

void SeaPlayerComponent::Init()
{
	_camera = CameraManager::main->GetCamera(CameraType::ThirdPersonCamera);
}

void SeaPlayerComponent::Start()
{
	_prevMousePosition = Input::main->GetMousePosition();
}

void SeaPlayerComponent::Update()
{
	if( CameraManager::main->GetCameraType()== CameraType::DebugCamera)
	{
		return;
	}


	Component::Update();

	vec2 currentMousePos = Input::main->GetMousePosition();
	vec2 delta = (currentMousePos - _prevMousePosition) * _speed;
	_prevMousePosition = currentMousePos;

	_camera->SetCameraRotation(delta.x, delta.y, 0);
	_camera->SetCameraPos(vec3(GetOwner()->_transform->GetWorldPosition().x , GetOwner()->_transform->GetWorldPosition().y,
		GetOwner()->_transform->GetWorldPosition().z-10.0f));


}

void SeaPlayerComponent::Update2()
{

}

void SeaPlayerComponent::Enable()
{
}

void SeaPlayerComponent::Disable()
{
}

void SeaPlayerComponent::RenderBegin()
{
}

void SeaPlayerComponent::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{

}

void SeaPlayerComponent::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
}