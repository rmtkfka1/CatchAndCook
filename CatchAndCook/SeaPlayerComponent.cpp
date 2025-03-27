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