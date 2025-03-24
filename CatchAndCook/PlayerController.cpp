#include "pch.h"
#include "PlayerController.h"

#include "Camera.h"
#include "CameraComponent.h"
#include "CameraManager.h"
#include "ColliderManager.h"
#include "Gizmo.h"
#include "Transform.h"

PlayerController::~PlayerController()
{
}

bool PlayerController::IsExecuteAble()
{
	return Component::IsExecuteAble();
}

void PlayerController::Init()
{
	Component::Init();

}

void PlayerController::Start()
{
	Component::Start();
	camera = FindObjectByType<CameraComponent>();

}

void PlayerController::Update()
{
	Component::Update();

	auto cameraTransform = camera.lock()->GetOwner()->_transform;
	auto currentMousePosition = Input::main->GetMousePosition();
	Vector3 offset = GetOwner()->_transform->GetWorldPosition() + Vector3::Up;
	Vector2 between = currentMousePosition - _prevMousePosition;

	Quaternion cameraRotation = cameraTransform->GetWorldRotation();
	Vector3 cameraEuler = cameraRotation.ToEuler();
	Vector3 lookDirection = Vector3::Transform(Vector3::Forward, cameraRotation);

	Vector3 betweenEuler = Vector3(between.y, between.x, 0) * 0.01f;
	Vector3 finalEuler = betweenEuler + cameraEuler;

	finalEuler.z = 0;
	if (finalEuler.x * R2D <= -89.99)
	{
		finalEuler.x = -89.99;
		std::cout << betweenEuler.x * R2D + cameraEuler.x * R2D << "\n";
		std::cout << finalEuler.x << "\n";
	}
	//if (betweenEuler.x * R2D + cameraEuler.x * R2D >= 89.99)
		//betweenEuler.x += 89.99 - cameraEuler.x;

	Quaternion finalRotation = Quaternion::CreateFromYawPitchRoll(finalEuler);

	lookDirection = Vector3::Transform(Vector3::Forward, finalRotation);
	lookDirection.Normalize();

	float distance = 5;
	auto hit = ColliderManager::main->RayCast({ offset, -lookDirection }, distance);
	if (hit)
		distance = hit.distance - 0.1f;
	

	cameraTransform->SetWorldPosition(offset - lookDirection * distance);
	cameraTransform->LookUp(lookDirection, Vector3::Up);
	

	//between.x = 
	_prevMousePosition = currentMousePosition;
	//GetOwner()->_transform->

	if (Input::main->GetMouse(KeyCode::LeftMouse))
	{
		auto mousePos = Input::main->GetMousePosition();
		auto camera = CameraManager::main->GetActiveCamera();
		auto cameraPos = camera->GetCameraPos();
		vec3 cameraDir = camera->GetScreenToWorldPosition(mousePos) - camera->GetCameraPos();
		cameraDir.Normalize();
		
		auto hit = ColliderManager::main->RayCast({ cameraPos, cameraDir }, 500);
		if (hit)
		{
			//Gizmo::Box({ hit.worldPos, Vector3::One });
			//std::cout << hit.worldPos.x <<", "<< hit.worldPos.y <<","<< hit.worldPos.z << "\n";
			Gizmo::Width(0.5f);
			Gizmo::Ray(hit.worldPos, hit.normal, 2);
			Gizmo::WidthRollBack();
		}
	}
	//GetOwner()->_transform->SetWorldPosition(GetOwner()->_transform->GetWorldPosition() + Vector3::Forward * 10.0f * Time::main->GetDeltaTime());
}

void PlayerController::Update2()
{
	Component::Update2();
}

void PlayerController::Enable()
{
	Component::Enable();
}

void PlayerController::Disable()
{
	Component::Disable();
}

void PlayerController::RenderBegin()
{
	Component::RenderBegin();
}

void PlayerController::CollisionBegin(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionBegin(collider, other);
}

void PlayerController::CollisionEnd(const std::shared_ptr<Collider>& collider, const std::shared_ptr<Collider>& other)
{
	Component::CollisionEnd(collider, other);
}

void PlayerController::ChangeParent(const std::shared_ptr<GameObject>& prev, const std::shared_ptr<GameObject>& current)
{
	Component::ChangeParent(prev, current);
}

void PlayerController::SetDestroy()
{
	Component::SetDestroy();
}

void PlayerController::Destroy()
{
	Component::Destroy();
}
