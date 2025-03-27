#include "pch.h"
#include "PlayerController.h"

#include "Camera.h"
#include "CameraComponent.h"
#include "CameraManager.h"
#include "ColliderManager.h"
#include "Gizmo.h"
#include "Terrain.h"
#include "TerrainManager.h"
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
	if (CameraManager::main->GetCameraType() == CameraType::DebugCamera)
	{
		return;
	}

	Component::Update();

	auto cameraTransform = camera.lock()->GetOwner()->_transform;
	auto currentMousePosition = Input::main->GetMousePosition();
	Vector3 offset = GetOwner()->_transform->GetWorldPosition() + Vector3::Up * 1.1f;
	Vector2 between = currentMousePosition - _prevMousePosition;

	Quaternion cameraRotation = cameraTransform->GetWorldRotation();
	Vector3 cameraEuler = cameraRotation.ToEuler();

	Vector3 betweenEuler = Vector3(between.y, between.x, 0) * 0.01f * 0.5f;
	Vector3 finalEuler = betweenEuler + cameraEuler;

	finalEuler.z = 0;
	if (finalEuler.x * R2D <= -70) finalEuler.x = -70 * D2R;
	if (finalEuler.x * R2D >= 85) finalEuler.x = 85 * D2R;

	Quaternion finalRotation = Quaternion::CreateFromYawPitchRoll(finalEuler);

	Vector3 lookDirection = Vector3::Transform(Vector3::Forward, finalRotation);
	lookDirection.Normalize();

	float distance = 3.4;
	if (auto hit = ColliderManager::main->RayCast({ offset, -lookDirection }, distance))
		distance = hit.distance * 0.9f;

	cameraTransform->SetWorldPosition(offset - lookDirection * distance);
	cameraTransform->LookUp(lookDirection, Vector3::Up);

	Quaternion fieldRotation = Quaternion::CreateFromYawPitchRoll(Vector3(0, finalEuler.y, 0));

	Vector3 moveDirection = Vector3(
		(Input::main->GetKey(KeyCode::D) ? 1 : 0) - (Input::main->GetKey(KeyCode::A) ? 1 : 0), 0,
		(Input::main->GetKey(KeyCode::W) ? 1 : 0) - (Input::main->GetKey(KeyCode::S) ? 1 : 0));
	moveDirection.Normalize();
	Vector3 moveWorldDirection = Vector3::Transform(moveDirection, fieldRotation);

	if (moveWorldDirection != Vector3::Zero)
	{
		Vector3 currentPos = GetOwner()->_transform->GetWorldPosition();
		Vector3 nextPos = currentPos + moveWorldDirection * 10.0f * Time::main->GetDeltaTime();
		Vector3 upRayOffset = nextPos + Vector3::Up * 1.0f;
		if (auto hit = ColliderManager::main->RayCast({ upRayOffset, Vector3::Down }, 30))
			nextPos.y = upRayOffset.y - hit.distance;

		GetOwner()->_transform->LookUp(moveWorldDirection, Vector3::Up);
		GetOwner()->_transform->SetWorldPosition(nextPos);
	}

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
